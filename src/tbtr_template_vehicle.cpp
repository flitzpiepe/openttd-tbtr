
#include "stdafx.h"

#include "tbtr_template_vehicle.h"
#include "engine_gui.h"

TemplatePool _template_pool("Template");
INSTANTIATE_POOL_METHODS(Template)

/*
 * Default CTOR
 */
TemplateVehicle::TemplateVehicle()
{
	this->Init(INVALID_ENGINE);
}

/*
 * CTOR: initialize this template vehicle with a given engine id
 *
 * @param eid: the engine id for this template
 */
TemplateVehicle::TemplateVehicle(EngineID eid)
{
	this->Init(eid);
}

/*
 * Default desctructor
 */
TemplateVehicle::~TemplateVehicle()
{
	TemplateVehicle* tv = this->next;
	this->next = NULL;
	delete tv;
}

/**
 * Calculate the total cost of buying all vehicles in this template.
 *
 * @return: the money value of the calculated cost
 */
Money TemplateVehicle::CalculateCost() const
{
	Money val = 0;
	const TemplateVehicle* tv = this;
	for (; tv; tv=tv->GetNextUnit())
		val += (Engine::Get(tv->engine_type))->GetCost();
	return val;
}

/**
 * Make this template vehicle match a train. This assumes that this template is not yet part
 * of any chain.
 *
 * @param train:  the (first vehicle of the) train which acts as preimage for the template
 */
bool TemplateVehicle::CloneFromTrain(const Train* train, TemplateVehicle* chainHead)
{
	if ( !train )
		return false;

	int len = CountVehiclesInChain(train);
	if ( !TemplateVehicle::CanAllocateItem(len) )
		return false;

	this->first = chainHead ? chainHead : this;
	this->first->last = this;
	this->engine_type = train->engine_type;
	this->subtype = train->subtype;
	this->railtype = train->railtype;
	this->cargo_type = train->cargo_type;
	this->cargo_subtype = train->cargo_subtype;
	this->cargo_cap = train->cargo_cap;
	const GroundVehicleCache* gcache = train->GetGroundVehicleCache();
	this->max_speed = train->GetDisplayMaxSpeed();
	this->power = gcache->cached_power;
	this->weight = gcache->cached_weight;
	this->max_te = gcache->cached_max_te / 1000;

	if ( train->GetNextUnit() )
	{
		TemplateVehicle* tv = new TemplateVehicle();
		if ( chainHead == NULL )
			chainHead = this;
		tv->CloneFromTrain(train->GetNextUnit(), chainHead);
		this->next = tv;
	}

	return true;
}

/**
 * Check if this template vehicle contains any locos or wagons of the given rail type.
 *
 * @param railtype: the rail type to check for
 * @return:         true, if there is a unit of the given rail type in this template
 */
bool TemplateVehicle::ContainsRailType(RailType railtype) const
{
	const TemplateVehicle* tv = this;
	/* For non-electrified rail engines, the whole chain must not contain any electrified engines or wagons */
	if ( railtype == RAILTYPE_BEGIN || railtype == RAILTYPE_RAIL ) {
		while ( tv ) {
		if ( tv->railtype != railtype )
			return false;
		tv = tv->GetNextUnit();
		}
		return true;
	}
	/* For electrified rail engines, non-electrified engines or wagons are also allowed */
	while ( tv ) {
		if ( tv->railtype == railtype )
			return true;
		tv = tv->GetNextUnit();
	}
	return false;
}

/**
 * Count all groups that are using this template.
 *
 * @return: the count
 */
int TemplateVehicle::CountGroups() const
{
	int count = 0;
	Group* g;
	FOR_ALL_GROUPS(g)
	{
		if (g->owner == this->owner && g->template_id == this->index)
			++count;
	}
	return count;
}

/**
 * Draw a template
 *
 * @param left:  left border of the bounding box
 * @param right: right border of the bounding box
 * @param y:     y-coordinate of the bounding box
 */
void TemplateVehicle::Draw(uint left, uint right, int y, int x_offset=0) const
{
	/* don't draw outside of the bounding box'es area */
	if ( this->sprite_width + left >= right )
		return;

	/* draw this + rest of the chain */
	if ( x_offset <= 0 ) {
		DrawVehicleEngine(left, right, left, y+10, this->engine_type, GetEnginePalette(this->engine_type, this->owner), EIT_PURCHASE);
		left += this->sprite_width;
	}
	TemplateVehicle* next = this->GetNextUnit();
	if ( next )
		next->Draw(left, right, y, x_offset-this->sprite_width);
}

/**
 * Calculate the sum of all sprite widths of this template and the rest of the chain
 */
uint TemplateVehicle::GetChainDisplayLength() const
{
	uint sum = 0;
	for ( const TemplateVehicle* tmp=this; tmp; tmp=tmp->next )
		sum += tmp->sprite_width;
	return sum;
}

/*
 * Return the next 'real' unit following this template, i.e. disregarding articulated parts.
 *
 * @return:    the next template vehicle following *this in the consist.
 */
TemplateVehicle* TemplateVehicle::GetNextUnit() const
{
	TemplateVehicle* tv = this->next;
	while ( tv && HasBit(tv->subtype, GVSF_ARTICULATED_PART) ) tv = tv->next;
	if ( tv && HasBit(tv->subtype, GVSF_MULTIHEADED) && !HasBit(tv->subtype, GVSF_ENGINE) )
		tv = tv->next;
	return tv;
}

/**
 * Initialize this template vehicle with default values.
 *
 * @param eid: the engine id for this template
 */
void TemplateVehicle::Init(EngineID eid)
{
	this->next = NULL;
	this->prev = NULL;
	this->first = this;
	this->last = this;

	this->engine_type = eid;
	this->owner = _current_company;
	this->real_length = 0;
	this->image_width = 0;

	/* cache the sprite dimensions for this template's engine */
	GetTrainSpriteSize(this->engine_type, this->sprite_width, this->sprite_height, this->sprite_xoff, this->sprite_yoff, EIT_PURCHASE);

	this->reuse_depot_vehicles = true;
	this->keep_remaining_vehicles = true;
	this->refit_as_template = true;
}

/**
 * Return whether a given train will be treated by template replacement.
 *
 * @t:      the train to check
 * @return: true, if it will be considered for template replacement
 */
bool TemplateVehicle::TrainNeedsReplacement(Train* t)
{
	TemplateVehicle* tv = this;
	while ( tv && t )
	{
		if ( t->engine_type != tv->engine_type )
			return true;
		if ( t->subtype != tv->subtype )
			return true;
		if ( t->cargo_type != tv->cargo_type )
			return true;
		if ( t->cargo_subtype != tv->cargo_subtype )
			return true;
		tv = tv->GetNextUnit();
		t = t->GetNextUnit();
	}
	/* check if one chain ended before the other */
	return (!tv && t) || (tv && !t);
}

/**
 * Update the last pointer on each member of this chain of TemplateVehicle's
 *
 * @param last:   the new last vehicle
 */
void TemplateVehicle::UpdateLastVehicle(TemplateVehicle* last)
{
	TemplateVehicle* tmp = this->first;
	while ( tmp ) {
		tmp->last = last;
		tmp = tmp->next;
	}
}
