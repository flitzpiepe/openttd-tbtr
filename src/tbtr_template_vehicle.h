/*
 * This file is part of OpenTTD.
 * OpenTTD is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, version 2.
 * OpenTTD is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with OpenTTD. If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file tbtr_template_vehicle.h
 * The class definitions for template trains, template replacements and virtual trains.
 */

#ifndef TBTR_TEMPLATE_VEHICLE_H
#define TBTR_TEMPLATE_VEHICLE_H

#include "stdafx.h"
#include "core/pool_func.hpp"
#include "core/pool_type.hpp"

#include "company_func.h"
#include "newgrf_spritegroup.h"
#include "train.h"
#include "vehicle_base.h"

typedef uint32 TemplateID;

class TemplateVehicle;

/** A pool allowing to store up to ~64k templates */
typedef Pool<TemplateVehicle, TemplateID, 512, 0x10000> TemplatePool;
extern TemplatePool _template_pool;

#define NO_TEMPLATE 0;

/** Main Template Vehicle class */
struct TemplateVehicle : TemplatePool::PoolItem<&_template_pool>, BaseVehicle {
public:
	TemplateVehicle(EngineID);
	TemplateVehicle();
	~TemplateVehicle();
	TemplateID index;                   ///< Vehicle index

private:
	TemplateVehicle* next;              ///< pointer to the next template vehicle in the chain
	TemplateVehicle* prev;              ///< NOSAVE: pointer to the previous template vehicle in the chain
	TemplateVehicle* first;             ///< NOSAVE: pointer to the first template vehicle in the chain

	/** essential template info */
	// TODO is owner used anywhere in the old code?
	//      how is it retrieved when passed to the ctor?
	//      it would be good if it could be added to the almost-default ctor
	Owner owner;                        ///< template owner
	OwnerByte owner_byte;               ///< template owner byte

	/** Vehicle type + cargo info */
	EngineID engine_type;               ///< The type of engine used for this vehicle.
	CargoID cargo_type;                 ///< type of cargo this vehicle is carrying
	uint16 cargo_cap;                   ///< total capacity
	byte cargo_subtype;                 ///< cargo subtype

	/** Vehicle drawing information */
	byte spritenum;                     ///< used for drawing in a GUI
	SpriteID cur_image;                 ///< used for drawing in a GUI
	uint32 image_width;                 ///< used for drawing in a GUI
	const SpriteGroup *sgroup;          ///< used for drawing in a GUI

	/** Template usage configuration */
	bool reuse_depot_vehicles;          ///< whether to allow using exising vehicles from a depot
	bool keep_remaining_vehicles;       ///< whether to keep or sell not needed train parts
	bool refit_as_template;             ///< whether to refit the cargo configuration

	void Init(EngineID);

    bool CloneFromTrain(Train*);
};

TemplateID FindTemplateIndexForGroup(GroupID);

#endif /* !TBTR_TEMPLATE_VEHICLE_H */
