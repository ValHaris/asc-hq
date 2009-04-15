/*
     This file is part of Advanced Strategic Command; http://www.asc-hq.de
     Copyright (C) 1994-2008  Martin Bickel  and  Marc Schellenberger
 
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


#include "spawnbuilding.h"
#include "viewregistration.h"
#include "unitfieldregistration.h"
#include "removeobject.h"

#include "../vehicle.h"
#include "../gamemap.h"
#include "../viewcalculation.h"
     
SpawnBuilding::SpawnBuilding( GameMap* gamemap, const MapCoordinate& position, int buildingTypeID, int owner )
   : GameAction( gamemap ), pos(position)
{
   this->buildingTypeID = buildingTypeID;
   this->owner = owner;
   
}
            
      
ASCString SpawnBuilding::getDescription() const
{
   const BuildingType* bld = getMap()->getbuildingtype_byid( buildingTypeID );
   ASCString res ;
   if ( bld )
      res = "Spawn Building of type " + bld->getName();
   else
      res = "Spawn Building " ;
   res += " at " + pos.toString();
   return  res;
}
      
      
static const int SpawnBuildingStreamVersion = 1;
      
void SpawnBuilding::readData ( tnstream& stream ) 
{
   int version = stream.readInt();
   if ( version < 1 || version > SpawnBuildingStreamVersion )
      throw tinvalidversion ( "SpawnBuilding", SpawnBuildingStreamVersion, version );
   
   buildingTypeID = stream.readInt();
   pos.read( stream );
   owner = stream.readInt();
};
      
      
void SpawnBuilding::writeData ( tnstream& stream ) const
{
   stream.writeInt( SpawnBuildingStreamVersion );
   stream.writeInt( buildingTypeID );
   pos.write( stream );
   stream.writeInt( owner );
};


GameActionID SpawnBuilding::getID() const
{
   return ActionRegistry::SpawnBuilding;
}

ActionResult SpawnBuilding::runAction( const Context& context )
{
   tfield* fld = getMap()->getField(pos);
   if ( !fld )
      return ActionResult( 21002, pos );
   
   const BuildingType* buildingType = getMap()->getbuildingtype_byid( buildingTypeID );
   if ( !buildingType )
      return ActionResult( 22500, "Building id is " + ASCString::toString(buildingTypeID));
   
   for ( int a = 0; a < 4; a++)
      for ( int b = 0; b < 6; b++ ) 
         if ( buildingType->fieldExists ( BuildingType::LocalCoordinate( a, b ) ) ) {
            tfield* field = getMap()->getField( buildingType->getFieldCoordinate( pos, BuildingType::LocalCoordinate(a,b) ));
            if (field == NULL)
               return ActionResult(22501);
         }

   for ( int a = 0; a < 4; a++)
      for ( int b = 0; b < 6; b++ ) 
         if ( buildingType->fieldExists ( BuildingType::LocalCoordinate( a, b ) ) ) {
            MapCoordinate fpos = buildingType->getFieldCoordinate( pos, BuildingType::LocalCoordinate(a,b) );
            tfield* field = getMap()->getField( fpos );

            vector<int> objectsToDelete;
            
            tfield::ObjectContainer::iterator i = field->objects.begin();
            while ( i != field->objects.end()) {
               if ( !i->typ->canExistBeneathBuildings )
                  objectsToDelete.push_back( i->typ->id );
               i++;
            };
            
            for ( vector<int>::iterator j = objectsToDelete.begin(); j != objectsToDelete.end(); ++j )
               (new RemoveObject( getMap(), fpos, *j ))->execute( context );
      }

         
   Building* gbde = NULL;
   if ( !getMap()->getField(pos)->building ) {
      gbde = new Building ( getMap(), pos, buildingType, owner );

      gbde->plus = gbde->typ->defaultProduction;
      gbde->maxplus = gbde->typ->defaultProduction;
      gbde->bi_resourceplus = gbde->typ->defaultProduction;

      gbde->actstorage.fuel = 0;
      gbde->actstorage.material = 0;
      gbde->actstorage.energy = 0;
      gbde->netcontrol = 0;
      gbde->connection = 0;
      gbde->visible = true;
      gbde->setCompletion ( 0 );
   }
   else {
      gbde = getMap()->getField(pos)->building;
      if (gbde->getCompletion() < gbde->typ->construction_steps-1)
         gbde->setCompletion( gbde->getCompletion()+1 );

   }

   
   ActionResult res = (new ViewRegistration( gbde , ViewRegistration::AddView ))->execute( context );
   
   evaluateviewcalculation( getMap(), gbde->getPosition(), gbde->typ->view, 0, false, &context );
   
   
   return res;
}

Building* SpawnBuilding::getBuilding()
{
   return getMap()->getField(pos)->building; 
}


ActionResult SpawnBuilding::undoAction( const Context& context )
{
   tfield* fld = getMap()->getField(pos);
   if ( !fld )
      return ActionResult( 21002, pos );
   
   const BuildingType* buildingType = getMap()->getbuildingtype_byid( buildingTypeID );
   if ( !buildingType )
      return ActionResult( 22500, "Building id is " + ASCString::toString(buildingTypeID));
   
   if ( !fld->building || fld->building->typ != buildingType )
      return ActionResult( 22502 );
   
   if ( fld->building->getCompletion() > 0 )
      fld->building->setCompletion( fld->building->getCompletion() -1 );
   else
      delete fld->building;
   
   return ActionResult(0);
}

ActionResult SpawnBuilding::verify()
{
   return ActionResult(0);
}


namespace {
   const bool r1 = registerAction<SpawnBuilding> ( ActionRegistry::SpawnBuilding );
}

