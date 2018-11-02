#include "tbtr_template_vehicle.h"
// TODO maybe move these to a different file, together with the command-specific functions defined at the end
// of this file
#include "autoreplace_func.h"
#include "train.h"
#include "command_type.h"
#include "command_func.h"
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
		tv->first = chainHead;
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
	return TemplateVehicle::Get(Group::Get(t->group_id)->template_id);
}

// TODO stubs to be implemented later

void RefitTrainFromTemplate(Train* t, TemplateVehicle* tv)
{}
void TransferCargoForTrain(Train* o, Train* n)
{}
void BreakUpRemainders(Train* t)
{}
Train* DepotContainsEngine(TileIndex tile, EngineID engine, Train* t=0)
{}
void NeutralizeStatus(Train* t)
{}
bool ChainContainsVehicle(Train *t, Train* m)
{}
Train* ChainContainsEngine(Train *t, EngineID eid)
{}
CommandCost CopyHeadSpecificThings(Vehicle *old_head, Vehicle *new_head, DoCommandFlag flags)
{}
bool TrainMatchesTemplate(const Train *t, TemplateVehicle *tv)
{}
bool TrainMatchesTemplateRefit(const Train *t, TemplateVehicle *tv)
{}

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

	Train	*new_chain=0,
			*remainder_chain=0,
			*tmp_chain=0;
	TileIndex tile = incoming->tile;
	TemplateVehicle *tv = GetTemplateForTrain(incoming);
	EngineID eid = tv->engine_type;

	CommandCost buy(EXPENSES_NEW_VEHICLES);
	CommandCost move_cost(EXPENSES_NEW_VEHICLES);
	CommandCost tmp_result(EXPENSES_NEW_VEHICLES);

	/* first some tests on necessity and sanity */
	if ( tv == NULL )
		return buy;

	/*
	 * An array of already used vehicle IDs
	 *
	 * This is used during a simulate-only run of this function because we need to keep track of the
	 * train parts that have already been used to create the new chain. */
	VehicleID* simuTrainParts[128];
	uint simuIndex = 0;

	bool simulate = true;


	return buy;

	// TODO review and adapt and remove later

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
