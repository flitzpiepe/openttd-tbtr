#include "tbtr_template_vehicle.h"

TemplatePool _template_pool("TemplatePool");
INSTANTIATE_POOL_METHODS(Template)

TemplateReplacementPool _template_replacement_pool("TemplateReplacementPool");
INSTANTIATE_POOL_METHODS(TemplateReplacement)

TemplateVehicle::TemplateVehicle()
{
	this->Init(INVALID_ENGINE);
}

TemplateVehicle::TemplateVehicle(EngineID eid)
{
	this->Init(eid);
}

// TODO
TemplateVehicle::~TemplateVehicle()
{
	//TemplateVehicle* tv = this->next;
	//this->next = nullptr;
	//delete tv;
}

void TemplateVehicle::Init(EngineID eid)
{
	this->next = NULL;
	this->prev = NULL;
	this->first = this;

	this->engine_type = eid;
	this->owner_byte = _current_company;

	this->cur_image = SPR_IMG_QUERY;

	this->reuse_depot_vehicles = true;
	this->keep_remaining_vehicles = true;
	this->refit_as_template = true;
}

TemplateReplacement* FindTemplateReplacementForGroup(GroupID gid)
{
	TemplateReplacement *tr;
	FOR_ALL_TEMPLATE_REPLACEMENTS(tr) {
		if ( tr->Group() == gid )
			return tr;
	}
	return 0;
}
