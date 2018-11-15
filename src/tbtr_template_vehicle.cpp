
#include "stdafx.h"

#include "autoreplace_func.h"
#include "command_func.h"

#include "tbtr_debug.h"
#include "tbtr_template_vehicle.h"

// TODO rm later
CommandCost CmdMoveRailVehicle(TileIndex tile, DoCommandFlag flags, uint32 p1, uint32 p2, const char *text);

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
	this->first = NULL;

	this->engine_type = eid;
	this->owner = _current_company;
	this->real_length = 0;

	this->cur_image = SPR_IMG_QUERY;

	this->reuse_depot_vehicles = true;
	this->keep_remaining_vehicles = true;
	this->refit_as_template = true;
}

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

// TODO old functions
void RefitTrainFromTemplate(Train* t, TemplateVehicle* tv)
{}
void BreakUpRemainders(Train* t)
{}
void TransferCargoForTrain(Train* t1, Train* t2)
{}

/**
 * Neutralize a train's status (group, orders, etc).
 * @param train:	the train to be neutralized
 */
CommandCost NeutralizeStatus(Train* train)
{
	CommandCost cc = CommandCost();

	/* remove from current group */
	cc.AddCost(DoCommand(train->tile, DEFAULT_GROUP, train->index, DC_EXEC, CMD_ADD_VEHICLE_GROUP));

	/* reset orders for this vehicle */
	train->current_order = INVALID_ORDER;
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
CommandCost NeutralizeRemainderChain(Train *train) {
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
Train* DepotContainsEngine(TileIndex tile, EngineID engine, Train* t=0)
{}
bool ChainContainsVehicle(Train *t, Train* m)
{}
Train* ChainContainsEngine(Train *t, EngineID eid)
{}
bool TrainMatchesTemplate(const Train *t, TemplateVehicle *tv)
{}
bool TrainMatchesTemplateRefit(const Train *t, TemplateVehicle *tv)
{}
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
 * Find the first,best matching vehicle of a train for a given template vehicle.
 *
 * In any case the train must match the template's engine type. Among all of those we select the one
 * that also matches the refit and among those the one with the maximum amount of cargo.
 *
 * @param tv:		the template we are looking for
 * @param train:	the train we are looking in
 * @return:			the train we found, may be null
 */
// TODO bad scenario
// 	all vehicles have 0 cargo
// 	one is selected because of correct engine type
// 	later veh also fits AND has matching refit, too
// 		--> but will not be selected because its cargo isn't greater than that of the original one
// TODO should we prefer vehicles with 0 cargo? who knows whether it will fit the vehicle's current orders ...
Train* FindMatchingTrainInChain(TemplateVehicle* tv, Train* train)
{
	//			- must match engine_id
	//			- try to find one with matching refit
	//			- of those, choose the one with the max. #cargo
	Train* found = NULL;
	bool check_refit = tv->first->refit_as_template;
	for ( Train* tmp=train; tmp!=NULL; tmp=tmp->GetNextUnit() )
	{
		if ( tmp->engine_type == tv->engine_type )
		{
			if ( check_refit == false )
				return found;
			// first vehicle we found, take it!
			if ( found == NULL )
				found = tmp;
			// otherwise also compare refit and carried cargo
			// we want one with matching refit, and among those the one with
			// the maximum current cargo
			else
				if ( tmp->cargo_type==tv->cargo_type && tmp->cargo_subtype == tv->cargo_subtype )
					if ( tmp->cargo.StoredCount() > found->cargo.StoredCount() )
						found = tmp;
		}
	}
	return found;
}

/**
 * TODO adapt this text
 * Check, if any train in a given Depot contains a given EngineID
 * @param tile:     the tile of the depot
 * @param eid:      the EngineID to look up
 * @param not_in    this Train will be ignored during the check
 */
// TODO bad scenario
// 	all vehicles have 0 cargo
// 	one is selected because of correct engine type
// 	later veh also fits AND has matching refit, too
// 		--> but will not be selected because its cargo isn't greater than that of the original one
// TODO should we prefer vehicles with 0 cargo? who knows whether it will fit the vehicle's current orders ...
Train* FindMatchingTrainInDepot(TemplateVehicle* tv, TileIndex tile, Train* not_in)
{
	Train* found = NULL;
	Train* train;
	bool check_refit = tv->first->refit_as_template;
	FOR_ALL_TRAINS(train) {
		// conditions: v is stopped in the given depot, has the right engine and if 'not_in' is given v must not be contained within 'not_in'
		// if 'not_in' is NULL, no check is needed
		if ( train->tile == tile
				// If the veh belongs to a chain, wagons will not return true on IsStoppedInDepot(), only primary vehicles will
				// in case of t not a primary veh, we demand it to be a free wagon to consider it for replacement
				&& ((train->IsPrimaryVehicle() && train->IsStoppedInDepot()) || train->IsFreeWagon())
				&& train->engine_type == tv->engine_type
				&& (not_in==0 || ChainContainsVehicle(not_in, train)==0))
			// already found a matching vehicle, keep checking for matching refit + cargo amount
			if ( found != NULL )
			{
				if ( check_refit == true )
				{
					if ( train->cargo_type==tv->cargo_type && train->cargo_subtype==tv->cargo_subtype )
						// find something with a minimal amount of cargo, so that we can transfer more from the
						// original chain into it later
						if ( train->cargo.StoredCount() < found->cargo.StoredCount() )
							found = train;
				}
			}
			else
				found = train;
	}
	return found;
}

// TODO check all loops and ifs in this file for {} convention ... -.-

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
	// TODO check if null
	Train* incoming = Train::GetIfValid(id_inc);
	bool stayInDepot = p2;

	// TODO review what is needed
	Train	*new_chain=0,
			*remainder_chain=0,
			*tmp_chain=0;
	TileIndex tile = incoming->tile;
	TemplateVehicle *template_vehicle = GetTemplateForTrain(incoming);
	TemplateVehicle *tv = template_vehicle;
	EngineID eid = template_vehicle->engine_type;
	bool sellRemainders = !template_vehicle->keep_remaining_vehicles;
	bool refit_train = template_vehicle->refit_as_template;

	// TODO review what is needed
	CommandCost buy(EXPENSES_NEW_VEHICLES);
	CommandCost move_cost(EXPENSES_NEW_VEHICLES);
	CommandCost tmp_result(EXPENSES_NEW_VEHICLES);

	/* first some tests on necessity and sanity */
	if ( template_vehicle == NULL )
		return buy;

	/*
	 * An array of already used vehicle IDs
	 *
	 * This is used during a simulate-only run of this function because we need to keep track of the
	 * train parts that have already been used to create the new chain. */
	VehicleID* simuTrainParts[128];
	uint simuIndex = 0;

	bool simulate = true;

	// remember for CopyHeadSpecificThings()
	Train* old_head = incoming;

	// TODO comment
	for ( TemplateVehicle* cur_tmpl=template_vehicle ; cur_tmpl!=NULL ; cur_tmpl=cur_tmpl->GetNextUnit() )
	{
		bool vehicle_from_incoming = false;		///< used later during the refit decision
		// TODO rename (is always set to the new vehicle later)
		Train* found = FindMatchingTrainInChain(cur_tmpl, incoming);
		/* maybe try to find a matching vehicle in the depot */
		if ( found == NULL && tv->reuse_depot_vehicles )
			found = FindMatchingTrainInDepot(cur_tmpl, tile, incoming);
		/* found a matching vehicle somewhere: use it ... */
		if ( found != NULL )
		{
			vehicle_from_incoming = true;
			/* find the first vehicle in incoming, which is != found */
			incoming = (found == incoming) ? incoming->GetNextUnit() : incoming;

			if ( new_chain == NULL )
			{
				/* move the vehicle from the old chain to the new */
				CommandCost ccMove = DoCommand(tile, found->index, INVALID_VEHICLE, flags, CMD_MOVE_RAIL_VEHICLE);
				new_chain = found;
			 }
			 else
			 {
				CommandCost ccMove = DoCommand(tile, found->index, new_chain->index, flags, CMD_MOVE_RAIL_VEHICLE);
			 }
		}
		/* ... otherwise buy a new one */
		else
		{
			CommandCost cc = DoCommand(tile, cur_tmpl->engine_type, 0, flags, CMD_BUILD_VEHICLE);
			buy.AddCost(cc);
			found = Train::Get(_new_vehicle_id);
			if ( new_chain == NULL )
			{
				new_chain = found;
				CommandCost ccMove = DoCommand(tile, new_chain->index, INVALID_VEHICLE, flags, CMD_MOVE_RAIL_VEHICLE);
			}
			else
			{
				CommandCost ccMove = DoCommand(tile, found->index, new_chain->index, flags, CMD_MOVE_RAIL_VEHICLE);
			}
		}

		/* maybe refit as template */
		if ( refit_train )
		{
			CargoID cargo_type = cur_tmpl->cargo_type;
			byte cargo_subtype = cur_tmpl->cargo_subtype;
			buy.AddCost(DoCommand(0, found->index, cargo_type | (cargo_subtype<<8) | (1<<16), flags, GetCmdRefitVeh(found)));
		}
		/* or maybe refit as current vehicle, in case of newly bought vehicle */
		else if ( vehicle_from_incoming )
		{
			// TODO
		}
	}

	if ( flags == DC_EXEC )
	{
		CommandCost ccCopy = CopyHeadSpecificThings(old_head, new_chain, flags);

		if ( incoming )
			TransferCargo(incoming, new_chain);

		if ( !sellRemainders )
		{
			if ( incoming && incoming != new_chain )
				incoming->unitnumber = GetFreeUnitNumber(incoming->type);
			if ( incoming )
				NeutralizeRemainderChain(incoming);
		}

		if ( !stayInDepot )
			new_chain->vehstatus &= ~VS_STOPPED;
	}

	if ( sellRemainders )
		buy.AddCost(DoCommand(incoming->tile, incoming->index|(1<<20), 0, flags, CMD_SELL_VEHICLE));

	return buy;




	// TODO review and adapt and remove later
	// TODO after removing this, check which helper functions are still needed

	bool need_replacement = !TrainMatchesTemplate(incoming, tv);
	bool need_refit = !TrainMatchesTemplateRefit(incoming, tv);
	bool use_refit = tv->refit_as_template;
	CargoID store_refit_ct = CT_INVALID;
	short store_refit_csubt = 0;
	// if a train shall keep its old refit, store the refit setting of its first vehicle
	if ( !use_refit ) {
		for ( Train *getc=incoming; getc; getc=getc->GetNextUnit() )
			if ( getc->cargo_type != CT_INVALID ) {
				store_refit_ct = getc->cargo_type;
				break;
			}
	}

	if ( !need_replacement ) {
		if ( !need_refit || !use_refit ) {
			/* before returning, release incoming train first if 2nd param says so */
			if ( !stayInDepot ) incoming->vehstatus &= ~VS_STOPPED;
			return buy;
		}
	}

	/* define replacement behaviour */
	bool reuseDepot = tv->IsSetReuseDepotVehicles();
	bool keepRemainders = tv->IsSetKeepRemainingVehicles();

	if ( need_replacement ) {
		/// step 1: generate primary for newchain and generate remainder_chain
			// 1. primary of incoming might already fit the template
				// leave incoming's primary as is and move the rest to a free chain = remainder_chain
			// 2. needed primary might be one of incoming's member vehicles
			// 3. primary might be available as orphan vehicle in the depot
			// 4. we need to buy a new engine for the primary
			// all options other than 1. need to make sure to copy incoming's primary's status
		if ( eid == incoming->engine_type ) {													// 1
			new_chain = incoming;
			remainder_chain = incoming->GetNextUnit();
			if ( remainder_chain )
				move_cost.AddCost(CmdMoveRailVehicle(tile, flags, remainder_chain->index|(1<<20), INVALID_VEHICLE, 0));
		}
		else if ( (tmp_chain = ChainContainsEngine(incoming, eid)) && tmp_chain!=NULL )	{		// 2
			// new_chain is the needed engine, move it to an empty spot in the depot
			new_chain = tmp_chain;
			move_cost.AddCost(DoCommand(tile, new_chain->index, INVALID_VEHICLE, flags,CMD_MOVE_RAIL_VEHICLE));
			remainder_chain = incoming;
		}
		else if ( reuseDepot && (tmp_chain = DepotContainsEngine(tile, eid, incoming)) && tmp_chain!=NULL ) {	// 3
			new_chain = tmp_chain;
			move_cost.AddCost(DoCommand(tile, new_chain->index, INVALID_VEHICLE, flags, CMD_MOVE_RAIL_VEHICLE));
			remainder_chain = incoming;
		}
		else {																				// 4
			tmp_result = DoCommand(tile, eid, 0, flags, CMD_BUILD_VEHICLE);
			/* break up in case buying the vehicle didn't succeed */
			if ( !tmp_result.Succeeded() )
				return tmp_result;
			buy.AddCost(tmp_result);
			new_chain = Train::Get(_new_vehicle_id);
			/* make sure the newly built engine is not attached to any free wagons inside the depot */
			move_cost.AddCost ( DoCommand(tile, new_chain->index, INVALID_VEHICLE, flags, CMD_MOVE_RAIL_VEHICLE) );
			/* prepare the remainder chain */
			remainder_chain = incoming;
		}
		// If we bought a new engine or reused one from the depot, copy some parameters from the incoming primary engine
		if ( incoming != new_chain && flags == DC_EXEC) {
			CopyHeadSpecificThings(incoming, new_chain, flags);
			NeutralizeStatus(incoming);
			// additionally, if we don't want to use the template refit, refit as incoming
			// the template refit will be set further down, if we use it at all
			if ( !use_refit ) {
				uint32 cb = GetCmdRefitVeh(new_chain);
				DoCommandP(new_chain->tile, new_chain->index, store_refit_ct | store_refit_csubt << 8 | 1 << 16 , cb);
			}

		}

		/// step 2: fill up newchain according to the template
			// foreach member of template (after primary):
				// 1. needed engine might be within remainder_chain already
				// 2. needed engine might be orphaned within the depot (copy status)
				// 3. we need to buy (again)						   (copy status)
		TemplateVehicle *cur_tmpl = tv->GetNextUnit();
		Train *last_veh = new_chain;
		while (cur_tmpl) {
			// 1. engine contained in remainder chain
			if ( (tmp_chain = ChainContainsEngine(remainder_chain, cur_tmpl->engine_type)) && tmp_chain!=NULL )	{
				// advance remainder_chain (if necessary) to not lose track of it
				if ( tmp_chain == remainder_chain )
					remainder_chain = remainder_chain->GetNextUnit();
				move_cost.AddCost(CmdMoveRailVehicle(tile, flags, tmp_chain->index, last_veh->index, 0));
			}
			// 2. engine contained somewhere else in the depot
			else if ( reuseDepot && (tmp_chain = DepotContainsEngine(tile, cur_tmpl->engine_type, new_chain)) && tmp_chain!=NULL ) {
				move_cost.AddCost(CmdMoveRailVehicle(tile, flags, tmp_chain->index, last_veh->index, 0));
			}
			// 3. must buy new engine
			else {
				tmp_result = DoCommand(tile, cur_tmpl->engine_type, 0, flags, CMD_BUILD_VEHICLE);
				if ( !tmp_result.Succeeded() )
					return tmp_result;
				buy.AddCost(tmp_result);
				tmp_chain = Train::Get(_new_vehicle_id);
				move_cost.AddCost(CmdMoveRailVehicle(tile, flags, tmp_chain->index, last_veh->index, 0));
			}
			if ( need_refit && flags == DC_EXEC ) {
				if ( use_refit ) {
					uint32 cb = GetCmdRefitVeh(tmp_chain);
					DoCommandP(tmp_chain->tile, tmp_chain->index, cur_tmpl->cargo_type | cur_tmpl->cargo_subtype << 8 | 1 << 16 , cb);
				} else {
					uint32 cb = GetCmdRefitVeh(tmp_chain);
					DoCommandP(tmp_chain->tile, tmp_chain->index, store_refit_ct | store_refit_csubt << 8 | 1 << 16 , cb);
				}
			}
			cur_tmpl = cur_tmpl->GetNextUnit();
			last_veh = tmp_chain;
		}
	}
	/* no replacement done */
	else {
		new_chain = incoming;
	}
	/// step 3: reorder and neutralize the remaining vehicles from incoming
		// wagons remaining from remainder_chain should be filled up in as few freewagonchains as possible
		// each locos might be left as singular in the depot
		// neutralize each remaining engine's status

	// refit, only if the template option is set so
	if ( use_refit && (need_refit || need_replacement) ) {
		RefitTrainFromTemplate(new_chain, tv);
	}

	if ( new_chain && remainder_chain )
		for ( Train *ct=remainder_chain; ct; ct=ct->GetNextUnit() )
			TransferCargoForTrain(ct, new_chain);

	// point incoming to the newly created train so that starting/stopping from the calling function can be done
	incoming = new_chain;
	if ( !stayInDepot && flags == DC_EXEC )
		new_chain->vehstatus &= ~VS_STOPPED;

	if ( remainder_chain && keepRemainders && flags == DC_EXEC )
		BreakUpRemainders(remainder_chain);
	else if ( remainder_chain ) {
		buy.AddCost(DoCommand(tile, remainder_chain->index | (1<<20), 0, flags, CMD_SELL_VEHICLE));
	}
	return buy;
}
