#include "tbtr_template_vehicle.h"

TemplatePool _template_pool("TemplatePool");
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
	//TemplateVehicle* tv = this->next;
	//this->next = nullptr;
	//delete tv;
}

/**
 * Initialize this template vehicle with default values.
 */
void TemplateVehicle::Init(EngineID eid)
{
	this->next = NULL;
	this->prev = NULL;
	this->first = this;

	this->engine_type = eid;
	this->owner = _current_company;

	this->cur_image = SPR_IMG_QUERY;

	this->reuse_depot_vehicles = true;
	this->keep_remaining_vehicles = true;
	this->refit_as_template = true;
}

/**
 * Make this template vehicle match a train. This assumes that this template is not yet part
 * of any chain.
 *
 * @param train:  the (first vehicle of the) train which acts as preimage for the template
 */
bool TemplateVehicle::CloneFromTrain(const Train* train)
{
	Train* clicked = Train::Get(train->index);
	if ( !clicked )
		return false;

	int len = CountVehiclesInChain(clicked);
	if ( !TemplateVehicle::CanAllocateItem(len) )
		return false;

	this->engine_type = clicked->engine_type;
	this->subtype = clicked->subtype;
	this->railtype = clicked->railtype;
	this->cargo_type = clicked->cargo_type;
	this->cargo_subtype = clicked->cargo_subtype;
	this->cargo_cap = clicked->cargo_cap;
	const GroundVehicleCache *gcache = clicked->GetGroundVehicleCache();
	this->max_speed = clicked->GetDisplayMaxSpeed();
	this->power = gcache->cached_power;
	this->weight = gcache->cached_weight;
	this->max_te = gcache->cached_max_te / 1000;
	this->spritenum = clicked->spritenum;
	VehicleSpriteSeq seq;
	clicked->GetImage(DIR_W, EIT_PURCHASE, &seq);
	this->cur_image = seq.seq[0].sprite;
	Point *p = new Point();
	this->image_width = clicked->GetDisplayImageWidth(p);

	if ( train->GetNextVehicle() )
	{
		TemplateVehicle* tv = new TemplateVehicle();
		tv->CloneFromTrain(train->Next());
		next = tv;
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
