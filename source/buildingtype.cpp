/***************************************************************************
                          buildingtype.cpp  -  description
                             -------------------
    begin                : Fri Sep 29 2000
    copyright            : (C) 2000 by Martin Bickel
    email                : bickel@asc-hq.org
 ***************************************************************************/

/*! \file buildingtype.cpp
    \brief The implementation of the buildingtype class
*/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <algorithm>

#include "vehicletype.h"
#include "buildingtype.h"

#include "errors.h"

#ifdef sgmain
 #include "spfst.h"
#endif



const char*  cbuildingfunctions[cbuildingfunctionnum]  =
              { "HQ",
                "training",
                "unused (was: refinery)",
                "vehicle production",
                "ammunition production",
                "unused (was: energy prod)",
                "unused (was: material prod)",
                "unused (was: fuel prod)",
                "repair facility",
                "recycling",
                "research",
                "sonar",
                "wind power plant",
                "solar power plant",
                "matter converter (was: power plant)",
                "mining station",
                "external loading",
                "construct units that cannot move out" };



void*   BuildingType :: getpicture ( const LocalCoordinate& localCoordinate )
{
   #ifdef HEXAGON
   return w_picture[0][0][localCoordinate.x][localCoordinate.y];
   #else
   return w_picture[0][localCoordinate.x][localCoordinate.y];
   #endif
}


#define compensatebuildingcoordinateorgx (a) (dx & (~a))
#define compensatebuildingcoordinatex ( + (dx & ~b) )



MapCoordinate  BuildingType :: getFieldCoordinate ( const MapCoordinate& entryPosition, const LocalCoordinate& localCoordinates )
{
   int orgx = entryPosition.x - entry.x - (entry.y & ~entryPosition.y & 1 );
   int orgy = entryPosition.y - entry.y;

   int dx = orgy & 1;

   int yy = orgy + localCoordinates.y;
   int xx = orgx + localCoordinates.x + (dx & ~yy);
   MapCoordinate mc ( xx, yy );
   return mc;
}

int    BuildingType :: vehicleloadable ( pvehicletype fzt ) const
{
   if ( special & cgproduceAllUnitsB )
      return 1;

   if (    (loadcapacity >= fzt->maxsize()  &&  ((unitheightreq & fzt->height) || !unitheightreq) && !(unitheight_forbidden & fzt->height)  && (loadcapability & fzt->height))
        || ( fzt->functions & cf_trooper ) )
        return 1;

   return 0;
}




