/***************************************************************************
                          textfiletags.cpp  -  description
                             -------------------
    begin                : Fri Jul 27 2001
    copyright            : (C) 2001 by Martin Bickel
    email                : bickel@asc-hq.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "textfiletags.h"
#include "typen.h"
#include "vehicletype.h"
#include "buildingtype.h"

const char* weatherTags[cwettertypennum-1]
    = { "dry",
        "light_rain",
        "heavy_rain",
        "few_snow",
        "much_snow" };

const char*  bodenarten[cbodenartennum]
    = { "Shallow_water",
        "Lowland",
        "Swamp",
        "Forest",
        "High_mountains",
        "Road",
        "Railroad",
        "Building_entry_dont_use_it" ,
        "Harbour",
        "Runway"  ,
        "Pipeline",
        "Buried_pipeline",
        "Water",
        "Deep_water",
        "Hard_sand",
        "Soft_sand",
        "Track_possible",
        "Small_rocks",
        "Mud",
        "Snow",
        "Deep_snow",
        "Mountains",
        "Very_shallow_water",
        "Large_rocks",
        "Lava",
        "Ditch",
        "Hillside",
        "Turret_foundation",
        "Morass",
        "Installation",
        "Pack_ice",
        "River",
        "Frozen_water" };

 const char* heightTags[choehenstufennum]
    = { "deep_submerged",
        "submerged",
        "floating",
        "ground_level",
        "low_level_flight",
        "medium_level_flight",
        "high_level_flight",
        "orbit" };

 const char* vehicleAbilities[cvehiclefunctionsnum]
     = {           "sonar",
                   "paratrooper",
                   "mine_layer",
                   "trooper",
                   "repair_vehicle",
                   "conquer_buildings",
                   "move_after_attack",
                   "satelliteview",
                   "construct_ALL_buildings",
                   "view_mines",
                   "construct_vehicles",
                   "construct_specific_buildings",
                   "fuel_transfer",
                   "icebreaker",
                   "no_midair_refuel",
                   "material_transfer",
                   "__UNUSED1__",
                   "leaves_tracks",
                   "manual_resource_drilling",
                   "__UNUSED2__",
                   "self_repairing",
                   "resource_converter",
                   "automatic_resource_searching",
                   "Kamikaze_only",
                   "immune_to_mines",
                   "energy_transfer" };

const char*  weaponTags[cwaffentypennum]
   = {  "cruise_missile",
        "mine",
        "bomb",
        "air_missile",
        "ground_missile",
        "torpedo",
        "machinegun",
        "cannon",
        "service",
        "ammo_transfer",
        "laser",
        "shootable" };

const char*  unitCategoryTags[cmovemalitypenum] =
   { "default",
     "light_tracked_vehicle",
     "medium_tracked_vehicle",
     "heavy_tracked_vehicle",
     "light_wheeled_vehicle",
     "medium_wheeled_vehicle",
     "heavy_wheeled_vehicle",
     "trooper",
     "rail_vehicle",
     "medium_aircraft",
     "medium_ship",
     "building|turret|object",
     "light_aircraft",
     "heavy_aircraft",
     "light_ship",
     "heavy_ship",
     "helicopter",
     "hoovercraft"
   };

const char*  buildingFunctionTags[cbuildingfunctionnum]  =
              { "HeadQuarters",
                "Training_Facility",
                "__unused__",
                "Vehicle_Production",
                "Ammo_Production",
                "__unused__",
                "__unused__",
                "__unused__",
                "Repair_Facility",
                "Recycling",
                "Research",
                "Sonar",
                "Wind_Power_Plant",
                "Solar_Power_Plant",
                "Matter_Converter",
                "Mining_Station",
                "External_Loading",
                "Produce_Units_that_cannot_leave" };


 const char* weatherAbbrev[cwettertypennum]
    = { "_dry",
        "_lr",
        "_hr",
        "_fs",
        "_ms",
        "" };


