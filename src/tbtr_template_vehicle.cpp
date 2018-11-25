
#include "stdafx.h"

#include "autoreplace_func.h"
#include "command_func.h"

#include "tbtr_debug.h"
#include "tbtr_template_vehicle.h"

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
	//TemplateVehicle* tv = this->next;
	//this->next = nullptr;
	//delete tv;
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
	for (; tv; tv=tv->Next())
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
	Train* clicked = Train::Get(train->index);
	if ( !clicked )
		return false;

	int len = CountVehiclesInChain(clicked);
	if ( !TemplateVehicle::CanAllocateItem(len) )
		return false;

	this->first = chainHead ? chainHead : this;
	this->engine_type = clicked->engine_type;
	this->subtype = clicked->subtype;
	this->railtype = clicked->railtype;
	this->cargo_type = clicked->cargo_type;
	this->cargo_subtype = clicked->cargo_subtype;
	this->cargo_cap = clicked->cargo_cap;
	const GroundVehicleCache* gcache = clicked->GetGroundVehicleCache();
	this->max_speed = clicked->GetDisplayMaxSpeed();
	this->power = gcache->cached_power;
	this->weight = gcache->cached_weight;
	this->max_te = gcache->cached_max_te / 1000;
	this->spritenum = clicked->spritenum;
	VehicleSpriteSeq seq;
	clicked->GetImage(DIR_W, EIT_PURCHASE, &seq);
	this->cur_image = seq.seq[0].sprite;
	Point* p = new Point();
	this->image_width = clicked->GetDisplayImageWidth(p);

	if ( train->GetNextVehicle() )
	{
		TemplateVehicle* tv = new TemplateVehicle();
		if ( chainHead == NULL )
			chainHead = this;
		tv->CloneFromTrain(train->Next(), chainHead);
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
void TemplateVehicle::Draw(int left, int right, int y) const
{
	int offset = left;
	PaletteID pal = GetEnginePalette(this->engine_type, this->owner);
	DrawSprite(this->cur_image, pal, offset, y+12);

	if (this->next)
		this->next->Draw(offset+this->image_width, right, y);
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
	this->first = NULL;

	this->engine_type = eid;
	this->owner = _current_company;
	this->real_length = 0;

	this->cur_image = SPR_IMG_QUERY;

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
 * Check if a TemplateVehicle and a Train share the same refit settings.
 *
 * @param tv: the TemplateVehicle
 * @param t:  the Train
 * @return:   true if all refit related settings are the same, false otherwise
 */
bool CheckRefit(const TemplateVehicle* tv, const Train* t)
{
	return tv->cargo_type==t->cargo_type && tv->cargo_subtype==t->cargo_subtype;
}

/**
 * Return the template vehicle that is assigned to a train's group.
 *
 * @param t:	the train
 */
TemplateVehicle* GetTemplateForTrain(Train* t)
{
	GroupID gid = t->group_id;
	if ( gid == DEFAULT_GROUP )
		return NULL;

	TemplateID tid = Group::Get(gid)->template_id;
	if ( tid == INVALID_TEMPLATE )
		return NULL;

	return TemplateVehicle::Get(tid);
}

/**
 * Neutralize a train's status (group, orders, etc).
 * @param train:	the train to be neutralized
 */
CommandCost NeutralizeStatus(Train* train)
{
	CommandCost cc = CommandCost();

	/* remove from current group */
	cc.AddCost(DoCommand(train->tile, DEFAULT_GROUP, train->index, DC_EXEC, CMD_ADD_VEHICLE_GROUP));

	/* reset orders and statistics and such for the train */
	train->current_order = INVALID_ORDER;
	train->current_order_time = 0;
	train->lateness_counter = 0;
	train->timetable_start = 0;
	train->service_interval = 0;
	train->cur_real_order_index = INVALID_VEH_ORDER_ID ;
	train->cur_implicit_order_index = INVALID_VEH_ORDER_ID;
	train->vehicle_flags = 0;
	train->profit_this_year = 0;
	train->profit_last_year = 0;

	/* unshare and delete */
	cc.AddCost(DoCommand(train->tile, train->index, -1, DC_EXEC, CMD_DELETE_ORDER));

	/* make sure the vehicle is stopped */
	train->vehstatus |= VS_STOPPED;

	return cc;
}

/**
 * Break up a train into neutral chains inside the depot
 *
 * Engines are moved onto a new line each while the wagons will from a FreeWagonChain.
 * Orders and group assignment will be removed from the primary vehicles and it is ensured that they are
 * stopped in the depot.
 *
 * @param train:	the chain to be processed
 */
CommandCost NeutralizeRemainderChain(Train* train) {
	CommandCost cc = CommandCost();
	Train* nextVeh = train->GetNextUnit();
	while ( train != NULL )
	{
		if ( HasBit(train->subtype, GVSF_ENGINE) )
		{
			cc.AddCost(DoCommand(train->tile, train->index, INVALID_VEHICLE, DC_EXEC, CMD_MOVE_RAIL_VEHICLE));
			NeutralizeStatus(train);
		}
		train = nextVeh;
		if (nextVeh != NULL)
			nextVeh = nextVeh->GetNextUnit();
	}
	return cc;
}

/**
 * Ensure that a train is not part of another given chain.
 *
 * @param t:     the train
 * @param chain: the chain that must not include t
 * @return:      true if t is NOT a part of chain
 */
bool NotInChain(const Train* t, const Train* chain)
{
	while ( chain )
	{
		if ( t == chain )
			return false;
		chain = chain->GetNextUnit();
	}
	return true;
}

/**
 * Transfer as much cargo from a given train onto another train.
 *
 * The cargo shall be moved as far as it fits onto the new train.
 * No priority is given to any type of cargo, i.e. the first cargo that is found in the src chain will be transfered first.
 *
 * @param src:  the train from which the cargo will be moved away
 * @param dest: the train from which the cargo will be moved to
 */
void TransferCargo(Train* src, Train* dest)
{
	assert(dest->IsPrimaryVehicle());

	while ( src )
	{
		CargoID _cargo_type = src->cargo_type;
		byte _cargo_subtype = src->cargo_subtype;

		// how much cargo has to be moved (if possible)
		uint remainingAmount = src->cargo.TotalCount();
		// each vehicle in the new chain shall be given as much of the old cargo as possible, until none is left
		for (Train* tmp=dest; tmp!=NULL && remainingAmount>0; tmp=tmp->GetNextUnit())
		{
			if (tmp->cargo_type == _cargo_type && tmp->cargo_subtype == _cargo_subtype)
			{
				// calculate the free space for new cargo on the current vehicle
				uint curCap = tmp->cargo_cap - tmp->cargo.TotalCount();
				uint moveAmount = std::min(remainingAmount, curCap);
				// move (parts of) the old vehicle's cargo onto the current vehicle of the new chain
				if (moveAmount > 0)
				{
					src->cargo.Shift(moveAmount, &tmp->cargo);
					remainingAmount -= moveAmount;
				}
			}
		}
		src = src->GetNextUnit();
	}

	/* Update train weight etc */
	dest->ConsistChanged(ConsistChangeFlags::CCF_LOADUNLOAD);
}

/**
 * Find the first, best matching vehicle of a train for a given template vehicle.
 *
 * In any case the train must match the template's engine type. Among all of those we select the one
 * that also matches the refit and among those the one with the maximum amount of cargo.
 *
 * @param tv:     the template we are looking for
 * @param train:  the train we are looking in
 * @return:       pointer to the train we found, may be null
 */
Train* FindMatchingTrainInChain(TemplateVehicle* tv, Train* train)
{
	Train* found = NULL;
	bool check_refit = tv->first->refit_as_template;
	for ( Train* tmp=train; tmp!=NULL; tmp=tmp->GetNextUnit() ) {
		/* minimal matching condition: by engine_type */
		if ( tmp->engine_type == tv->engine_type ) {
			/* first matching engine_type we take! */
			if ( found == NULL )
				found = tmp;
			/* in case we're also interested in the refit setting */
			if ( check_refit && CheckRefit(tv,tmp)==true ) {
				/* the previously selected train had the wrong refit so this one is definitely better */
				if ( CheckRefit(tv,found) == false ) {
					found = tmp;
				}
				/* or both refits match but the current one has more cargo */
				else if ( tmp->cargo.StoredCount() > found->cargo.StoredCount() ) {
					found = tmp;
				}
			}
		}
	}
	return found;
}

/**
 * Find the first, best matching vehicle inside a depot for a given template vehicle.
 *
 * In any case the train must match the template's engine type. Among all of those we select the one
 * that also matches the refit and among those the one with the maximum amount of cargo.
 * Only trains which are not in a specific group will be considered.
 *
 * @param tv:     the template vehicle which's configuration we are looking for
 * @param tile:   the tile of the depot
 * @param ignore: vehicle must not be in this chain, default is NULL
 * @return:       pointer to the train we found, may be null
 */
Train* FindMatchingTrainInDepot(TemplateVehicle* tv, TileIndex tile, Train* ignore=NULL)
{
	Train* found = NULL;
	Train* train;
	bool check_refit = tv->first->refit_as_template;
	FOR_ALL_TRAINS(train) {
		if ( train->tile == tile
				// If the veh belongs to a chain, wagons will not return true on IsStoppedInDepot(), only primary vehicles will
				// in case of t not a primary veh, we demand it to be a free wagon to consider it for replacement
				&& ((train->IsPrimaryVehicle() && train->IsStoppedInDepot()) || train->IsFreeWagon())
				&& train->engine_type == tv->engine_type
				&& train->group_id == DEFAULT_GROUP
				&& (ignore==NULL || NotInChain(train, ignore)) )
		{
			// already found a matching vehicle, keep checking for matching refit + cargo amount
			if ( found != NULL && check_refit == true)
			{
				if ( train->cargo_type==tv->cargo_type && train->cargo_subtype==tv->cargo_subtype )
					// find something with a minimal amount of cargo, so that we can transfer more from the
					// original chain into it later
					if ( train->cargo.StoredCount() < found->cargo.StoredCount() )
						found = train;
			}
			else
				found = train;
		}
	}
	return found;
}

/**
 * Perform the actual template replacement, or just simulate it. Return the overall cost for the whole replacement in any case.
 *
 * @param tile:     the tile of the incoming train
 * @param flags:    command flags
 * @param p1:       first parameter list
 * @param p2:       second parameter list
 * @param msg:      command message
 */
CommandCost CmdTemplateReplacement(TileIndex ti, DoCommandFlag flags, uint32 p1, uint32 p2, char const* msg)
{
	VehicleID id_inc = GB(p1, 0, 20);
	Train* incoming = Train::GetIfValid(id_inc);
	if ( incoming == NULL )
		return CommandCost();
	Train *new_chain=0;
	TileIndex tile = incoming->tile;
	TemplateVehicle* template_vehicle = GetTemplateForTrain(incoming);
	CommandCost cc(EXPENSES_NEW_VEHICLES);

	bool stayInDepot = p2;
	bool sellRemainders = !template_vehicle->keep_remaining_vehicles;
	bool refit_train = template_vehicle->refit_as_template;

	/* first some tests on necessity and sanity */
	if ( template_vehicle == NULL )
		return cc;

	// remember for CopyHeadSpecificThings()
	Train* old_head = incoming;

	/*
	 * Procedure
	 *
	 * For each vehicle in the template, create a matching train part.
	 * The new vehicle may be obtained by:
	 * 		- finding a matching one in the incoming train
	 * 		- finding a matching one in the depot (if the template says so)
	 * 		- buying a new one
	 * The new vehicle is refitted as the template (if the template says so).
	 */
	for ( TemplateVehicle* cur_tmpl=template_vehicle ; cur_tmpl!=NULL ; cur_tmpl=cur_tmpl->GetNextUnit() )
	{
		/* try to find a matching vehicle in the incoming train */
		Train* new_vehicle = FindMatchingTrainInChain(cur_tmpl, incoming);

		/* nothing found -> try to find a matching vehicle in the depot */
		if ( new_vehicle == NULL && template_vehicle->reuse_depot_vehicles )
			new_vehicle = FindMatchingTrainInDepot(cur_tmpl, tile, new_chain);

		/* found a matching vehicle somewhere: use it ... */
		if ( new_vehicle != NULL )
		{
			/* find the first vehicle in incoming, which is != new_vehicle */
			incoming = (new_vehicle == incoming) ? incoming->GetNextUnit() : incoming;

			if ( new_chain == NULL )
			{
				/* move the vehicle from the old chain to the new */
				CommandCost ccMove = DoCommand(tile, new_vehicle->index, INVALID_VEHICLE, flags, CMD_MOVE_RAIL_VEHICLE);
				if ( flags == DC_EXEC )
					cc.AddCost(ccMove);
				new_chain = new_vehicle;
			 }
			 else
			 {
				CommandCost ccMove = DoCommand(tile, new_vehicle->index, new_chain->Last()->index, flags, CMD_MOVE_RAIL_VEHICLE);
				if ( flags == DC_EXEC )
					cc.AddCost(ccMove);
			 }
		}
		/* ... otherwise buy a new one */
		else
		{
			CommandCost ccBuild = DoCommand(tile, cur_tmpl->engine_type, 0, flags, CMD_BUILD_VEHICLE);
			cc.AddCost(ccBuild);
			new_vehicle = Train::Get(_new_vehicle_id);

			/* form the new chain */
			if ( new_chain == NULL )
			{
				new_chain = new_vehicle;
				CommandCost ccMove = DoCommand(tile, new_chain->Last()->index, INVALID_VEHICLE, flags, CMD_MOVE_RAIL_VEHICLE);
				if ( flags == DC_EXEC )
					cc.AddCost(ccMove);
			}
			/* or just append to it, if it already exists */
			else
			{
				CommandCost ccMove = DoCommand(tile, new_vehicle->index, new_chain->Last()->index, flags, CMD_MOVE_RAIL_VEHICLE);
				if ( flags == DC_EXEC )
					cc.AddCost(ccMove);
			}
		}

		/* maybe refit as template */
		if ( refit_train )
		{
			CargoID cargo_type = cur_tmpl->cargo_type;
			byte cargo_subtype = cur_tmpl->cargo_subtype;
			CommandCost ccRefit = DoCommand(0, new_vehicle->index, cargo_type | (cargo_subtype<<8) | (1<<16), flags, GetCmdRefitVeh(new_vehicle));
			if ( flags==DC_EXEC )
				cc.AddCost(ccRefit);
		}
	}

	/* some postprocessing steps */
	if ( flags == DC_EXEC )
	{
		/* train orders, group, etc. */
		CommandCost ccCopy = CopyHeadSpecificThings(old_head, new_chain, flags);
		cc.AddCost(ccCopy);

		/* cargo */
		if ( incoming )
			TransferCargo(incoming, new_chain);

		/* make the remainders sit peacefully in the depot */
		if ( !sellRemainders )
		{
			if ( incoming && incoming != new_chain )
				incoming->unitnumber = GetFreeUnitNumber(incoming->type);
			if ( incoming )
				NeutralizeRemainderChain(incoming);
		}

		/* launch new chain */
		if ( !stayInDepot )
			new_chain->vehstatus &= ~VS_STOPPED;
	}

	/* sell remainders */
	if ( sellRemainders && incoming!=NULL )
		cc.AddCost(DoCommand(incoming->tile, incoming->index|(1<<20), 0, flags, CMD_SELL_VEHICLE));

	return cc;
}
