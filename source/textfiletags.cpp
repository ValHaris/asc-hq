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

const char* weatherTags[cwettertypennum-1]
    = { "dry",
        "light_rain",
        "heavy_rain",
        "few_snow",
        "much_snow" };

const char*  bodenarten[cbodenartennum]
    = { "Shallow_water",
        "Lowland",
        "Swamp_thick",
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
        "Swamp_thin",
        "Installation",
        "Pack_ice",
        "River",
        "Frozen_water" };

 const char* weatherAbbrev[cwettertypennum]
    = { "_dry",
        "_lr",
        "_hr",
        "_fs",
        "_ms",
        "" };


