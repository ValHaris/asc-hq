
/*
    This file is part of Advanced Strategic Command; http://www.asc-hq.de
    Copyright (C) 1994-1999  Martin Bickel  and  Marc Schellenberger

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


#ifndef unitsetH
#define unitsetH

#include <vector>
#include "ascstring.h"
#include "typen.h"

class SingleUnitSet {
      public:

         enum Type { unit, building };

         class TranslationTable {
                  public:
                     std::vector<IntRange> translation;
                     std::string name;
                     void parseString ( const char* s );
               };


         int active;
         int ID;
         ASCString name;
         ASCString maintainer;
         ASCString information;

         std::vector<IntRange> unitIds;
         std::vector<IntRange> buildingIds;
         std::vector<TranslationTable*> transtab;
         bool filterBuildings;

         SingleUnitSet ( void ) : active ( 1 ), ID(0), filterBuildings ( true ) {};
         bool isMember ( int id, Type type );
         void read ( pnstream stream );
         std::vector<IntRange> parseIDs ( const char* s );

     };

typedef deallocating_vector<SingleUnitSet*> UnitSets;
extern UnitSets unitSets;

extern void loadUnitSets ( void );


class Vehicletype;
class BuildingType;
extern int getUnitSetID( const Vehicletype* veh );
extern int getUnitSetID( const BuildingType* bld );

extern bool vehicleComp( const Vehicletype* v1, const Vehicletype* v2 );


#endif
