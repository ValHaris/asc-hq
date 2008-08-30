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


#include "destructcontainer.h"
#include "spawnobject.h"

#include "../vehicle.h"
#include "../gamemap.h"
     
DestructContainer::DestructContainer( ContainerBase* container )
   : ContainerAction( container ), unitBuffer(NULL)
{
   building = container->isBuilding();
}
      
      
ASCString DestructContainer::getDescription() const
{
   return "Destruct container"; // with ID " + ASCString::toString(unitID);
}
      
      
void DestructContainer::readData ( tnstream& stream ) 
{
   int version = stream.readInt();
   if ( version != 1 )
      throw tinvalidversion ( "DestructUnit", 1, version );
   
   ContainerAction::readData( stream );
   building = stream.readInt();
   
   if ( stream.readInt() ) {
      unitBuffer = new tmemorystreambuf();
      unitBuffer->readfromstream( &stream );  
   } else
      unitBuffer = NULL;
};
      
      
void DestructContainer::writeData ( tnstream& stream ) const
{
   stream.writeInt( 1 );
   
   ContainerAction::writeData( stream );
   stream.writeInt( building );
   if ( unitBuffer ) {
      stream.writeInt( 1 );
      unitBuffer->writetostream( &stream );
   } else
      stream.writeInt( 0 );
};


GameActionID DestructContainer::getID() const
{
   return ActionRegistry::DestructContainer;
}

ActionResult DestructContainer::runAction( const Context& context )
{
   ContainerBase* container = getContainer();

   unitBuffer = new tmemorystreambuf();
   tmemorystream memstream( unitBuffer, tnstream::writing );
   container->write( memstream );
   
   Vehicle* veh = dynamic_cast<Vehicle*>(container);
   if ( veh )
      if ( !veh->typ->wreckageObject.empty() && getMap()->state != GameMap::Destruction ) {
         tfield* fld = getMap()->getField(veh->getPosition());
         if ( fld->vehicle == veh ) {
            for ( vector<int>::const_iterator i = veh->typ->wreckageObject.begin(); i != veh->typ->wreckageObject.end(); ++i ) {
               ObjectType* obj = getMap()->getobjecttype_byid( *i );
               if ( obj ) {
                  GameAction* o = new SpawnObject( getMap(), veh->getPosition(), *i, veh->direction );
                  o->execute( context ); 
               }
            }
         }
      }
      
   Building* bld = dynamic_cast<Building*>(container);
   if ( bld ) {
      for (int i = 0; i < BuildingType::xdimension; i++)
         for (int j = 0; j < BuildingType::ydimension; j++)
            if ( bld->typ->fieldExists ( BuildingType::LocalCoordinate(i,j) ) ) {
               tfield* fld = bld->getField( BuildingType::LocalCoordinate(i,j) );
               if ( fld && fld->building == bld ) {
                  typedef BuildingType::DestructionObjects::const_iterator J;
                  pair<J,J> b = bld->typ->destructionObjects.equal_range(BuildingType::LocalCoordinate(i,j));
                  for ( J o = b.first; o != b.second; ++o)
                     (new SpawnObject( getMap(), bld->getFieldCoordinates(BuildingType::LocalCoordinate(i,j)), o->second  ))->execute( context );
               }
            }
   }
   delete container;
   return ActionResult(0);
}


ActionResult DestructContainer::undoAction( const Context& context )
{
   tmemorystream memstream( unitBuffer, tnstream::reading );
   if ( building ) {
      Building* bld = Building::newFromStream( getMap(), memstream );
      bld->chainbuildingtofield( bld->getEntry() );
      bld->addview();
   } else {
      Vehicle* veh = Vehicle::newFromStream( getMap(), memstream );
      getMap()->getField( veh->getPosition() )->vehicle = veh;
      veh->addview();
   }
   
   return ActionResult(0);
}

DestructContainer::~DestructContainer()
{
   delete unitBuffer;  
}


namespace {
   const bool r1 = registerAction<DestructContainer> ( ActionRegistry::DestructContainer );
}