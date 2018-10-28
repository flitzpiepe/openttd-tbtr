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
#include "group.h"
#include "newgrf_spritegroup.h"
#include "train.h"
#include "vehicle_base.h"
#include "vehicle_func.h"

typedef int16 TemplateID;
#define INVALID_TEMPLATE -1

class TemplateVehicle;

#define FOR_ALL_TEMPLATES_FROM(var, start) FOR_ALL_ITEMS_FROM(TemplateVehicle, template_index, var, start)
#define FOR_ALL_TEMPLATES(var) FOR_ALL_TEMPLATES_FROM(var, 0)

/** A pool allowing to store up to ~64k templates */
typedef Pool<TemplateVehicle, TemplateID, 512, 0x10000> TemplatePool;
extern TemplatePool _template_pool;

/** Main Template Vehicle class
 *
 * A template vehicle is basically like a train with a minimal set of attributes. I.e. it is a chain
 * of template vehicles, like a train.
 *
 * All templates are stored in their own pool so that they don't interfere with a company's allowed number of
 * trains. */
struct TemplateVehicle : TemplatePool::PoolItem<&_template_pool>, BaseVehicle {
public:
	TemplateVehicle(EngineID);
	TemplateVehicle();
	~TemplateVehicle();
	TemplateID index;                   ///< Vehicle index

	TemplateVehicle* next;              ///< pointer to the next template vehicle in the chain
	TemplateVehicle* prev;              ///< NOSAVE: pointer to the previous template vehicle in the chain
	TemplateVehicle* first;             ///< NOSAVE: pointer to the first template vehicle in the chain

	/** essential template info */
	Owner owner;                        ///< template owner

	/** Vehicle type + cargo info */
	EngineID engine_type;               ///< The type of engine used for this vehicle.
    byte subtype;                       ///< The vehicle subtype
	RailTypeByte railtype;              ///< The railtype of this vehicle
	uint16 max_speed;
	uint16 power;
	uint16 weight;
	uint16 max_te;

	CargoID cargo_type;                 ///< type of cargo this vehicle is carrying
	uint16 cargo_cap;                   ///< total capacity
	byte cargo_subtype;                 ///< cargo subtype

	/** Vehicle drawing information */
	uint16 real_length;                 ///< template length in tile units, for drawing in the gui
	byte spritenum;                     ///< used for drawing in a GUI
	SpriteID cur_image;                 ///< used for drawing in a GUI
	uint32 image_width;                 ///< used for drawing in a GUI
	const SpriteGroup *sgroup;          ///< used for drawing in a GUI

	/** Template usage configuration */
	bool reuse_depot_vehicles;          ///< whether to allow using exising vehicles from a depot
	bool keep_remaining_vehicles;       ///< whether to keep or sell not needed train parts
	bool refit_as_template;             ///< whether to refit the cargo configuration

	void Init(EngineID);

public:
	inline TemplateVehicle* Next() const {return this->next;}

	inline uint16 GetRealLength() const {return real_length;}
    inline bool HasOwner(Owner owner) const {return this->owner == owner;}
    inline bool IsPrimaryVehicle() const {return HasBit(this->subtype, GVSF_FRONT);}
    inline bool IsFreeWagonChain() const {return HasBit(this->subtype, GVSF_FREE_WAGON);}
	inline bool IsSetReuseDepotVehicles() const {return reuse_depot_vehicles;}
	inline bool IsSetKeepRemainingVehicles() const {return keep_remaining_vehicles;}
	inline bool IsSetRefitAsTemplate() const {return refit_as_template;}

	Money CalculateCost() const;
    bool CloneFromTrain(const Train*, TemplateVehicle*);
    bool ContainsRailType(RailType) const;
	/* Count the number of groups which use this template vehicle */
	int CountGroups() const;
	TemplateVehicle* GetNextUnit() const;

	void Draw(int, int, int) const;
};

TemplateID FindTemplateIndexForGroup(GroupID);

#endif /* !TBTR_TEMPLATE_VEHICLE_H */
