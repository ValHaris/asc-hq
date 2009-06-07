/*
     This file is part of Advanced Strategic Command; http://www.asc-hq.de
     Copyright (C) 1994-2008  Martin Bickel  

     This program is free software; you can redistribute it and/or modify
     it under the terms of the GNU General Public License as published by
     the Free Software Foundation; either version 2 of the License, or
     (at your option) any later version.

     This program is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU General Public License for more details.

     You should have received a copy of the GNU General Public License
     along with this program; see the file COPYING. If not, write to the
     Free Software Foundation, Inc., 59 Temple Place, Suite 330,
     Boston, MA  02111-1307  USA
*/


#ifndef commonH
#define commonH

#include "../typen.h"

class GameMap;
class ObjectType;
class BuildingType;
class Vehicletype;
class TerrainType;

extern GameMap* getActiveMap();

extern const ObjectType* getObjectType( int id );
extern const BuildingType* getBuildingType( int id );
extern const Vehicletype* getUnitType( int id );
extern const TerrainType* getTerrainType( int id );

class TerrainBitNames {
   public:
      enum { Shallow_water,  
         Lowland,      
         Swamp,
         Forest,
         High_mountains,
         Road,
         Railroad,
         Building_entry_dont_use_it ,
         Harbour,
         Runway  ,
         Pipeline,
         Buried_pipeline,
         Water,
         Deep_water,
         Hard_sand,
         Soft_sand,
         Track_possible,
         Small_rocks,
         Mud,
         Snow,
         Deep_snow,
         Mountains,
         Very_shallow_water,
         Large_rocks,
         Lava,
         Ditch,
         Hillside,
         Turret_foundation,
         Morass,
         Installation,
         Pack_ice,
         River,
         Frozen_water,
         bridge,
         lava_barrier,
         spaceport,
         beacon,
         fire  };
};

      
#endif
