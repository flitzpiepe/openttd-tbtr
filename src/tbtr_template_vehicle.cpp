#include "tbtr_template_vehicle.h"

TemplatePool _template_pool("TemplatePool");
INSTANTIATE_POOL_METHODS(Template)

TemplateVehicle::TemplateVehicle()
{
	Init(INVALID_ENGINE);
}

TemplateVehicle::TemplateVehicle(EngineID eid)
{
	Init(eid);
}

TemplateVehicle::~TemplateVehicle()
{
	//TemplateVehicle* tv = this->next;
	//this->next = nullptr;
	//delete tv;
}

void TemplateVehicle::Init(EngineID eid)
{
	next = NULL;
	prev = NULL;
	first = this;

	engine_type = eid;
	owner_byte = _current_company;

	cur_image = SPR_IMG_QUERY;

	reuse_depot_vehicles = true;
	keep_remaining_vehicles = true;
	refit_as_template = true;
}
