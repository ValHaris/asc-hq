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
#include "action-registry.h"

#include "../vehicle.h"
#include "../gamemap.h"
#include "../viewcalculation.h"
     
DestructContainer::DestructContainer( ContainerBase* container, bool suppressWreckage )
   : ContainerAction( container ), fieldRegistration( NONE ), unitBuffer(NULL), hostingCarrier(0), cargoSlot(-1)
{
   building = container->isBuilding();
   this->suppressWreckage = suppressWreckage;
}
      
      
ASCString DestructContainer::getDescription() const
{
   return "Destruct container"; // with ID " + ASCString::toString(unitID);
}
      

static const int destructContainerStreamVersion = 4;
      
void DestructContainer::readData ( tnstream& stream ) 
{
   int version = stream.readInt();
   if ( version < 1 || version > destructContainerStreamVersion )
      throw tinvalidversion ( "DestructUnit", 1, version );
   
   ContainerAction::readData( stream );
   building = stream.readInt();
   
   if ( stream.readInt() ) {
      unitBuffer = new tmemorystreambuf();
      unitBuffer->readfromstream( &stream );  
   } else
      unitBuffer = NULL;

   if ( version >= 2 )
      fieldRegistration = (FieldRegistration) stream.readInt();
   
   if ( version >= 3 ) {
      hostingCarrier = stream.readInt();
      cargoSlot = stream.readInt();
   }
   
   if ( version >= 4 )
      suppressWreckage = stream.readInt();
   else
      suppressWreckage = false;
};
      
      
void DestructContainer::writeData ( tnstream& stream ) const
{
   stream.writeInt( destructContainerStreamVersion );
   
   ContainerAction::writeData( stream );
   stream.writeInt( building );
   if ( unitBuffer ) {
      stream.writeInt( 1 );
      unitBuffer->writetostream( &stream );
   } else
      stream.writeInt( 0 );

   stream.writeInt( (int)fieldRegistration );
   stream.writeInt( hostingCarrier );
   stream.writeInt( cargoSlot );
   stream.writeInt( suppressWreckage );
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
   if ( veh ) {
      MapField* fld = getMap()->getField(veh->getPosition());

      if ( fld->vehicle == veh )
         fieldRegistration = FIRST;
      else if ( fld->secondvehicle == veh )
         fieldRegistration = SECOND;
      else if ( veh->getCarrier() ) {
         fieldRegistration = CARRIER;
         hostingCarrier = veh->getCarrier()->getIdentification();
         const ContainerBase::Cargo& cargo = veh->getCarrier()->getCargo();
         ContainerBase::Cargo::const_iterator pos = find( cargo.begin(), cargo.end(), veh );
         if ( pos == cargo.end() )
            throw ActionResult( 22200, veh->getCarrier() );
         cargoSlot = pos - cargo.begin();
      }
      
      if ( !veh->typ->wreckageObject.empty() && getMap()->state != GameMap::Destruction && !suppressWreckage ) {
         if ( fieldRegistration == FIRST || fieldRegistration == SECOND ) {
            for ( vector<int>::const_iterator i = veh->typ->wreckageObject.begin(); i != veh->typ->wreckageObject.end(); ++i ) {
               ObjectType* obj = getMap()->getobjecttype_byid( *i );
               if ( obj ) {
                  GameAction* o = new SpawnObject( getMap(), veh->getPosition(), *i, veh->direction );
                  o->execute( context ); 
               }
            }
         }
      }
   }
      
   Building* bld = dynamic_cast<Building*>(container);
   bld->unchainbuildingfromfield();
   
   if ( bld && !suppressWreckage ) {
      for (int i = 0; i < BuildingType::xdimension; i++)
         for (int j = 0; j < BuildingType::ydimension; j++)
            if ( bld->typ->fieldExists ( BuildingType::LocalCoordinate(i,j) ) ) {
               MapField* fld = bld->getField( BuildingType::LocalCoordinate(i,j) );
               if ( fld ) {
                  typedef BuildingType::DestructionObjects::const_iterator J;
                  pair<J,J> b = bld->typ->destructionObjects.equal_range(BuildingType::LocalCoordinate(i,j));
                  for ( J o = b.first; o != b.second; ++o)
                     (new SpawnObject( getMap(), bld->getFieldCoordinates(BuildingType::LocalCoordinate(i,j)), o->second  ))->execute( context );
               }
            }
   }
   
   if( fieldRegistration != CARRIER )
      container->removeview();
   
   delete container;
   
   evaluateviewcalculation( getMap(), container->getPosition(), container->baseType->view, 0, false, &context );
   
   
   return ActionResult(0);
}


ActionResult DestructContainer::undoAction( const Context& context )
{
   tmemorystream memstream( unitBuffer, tnstream::reading );
   if ( building ) {
      Building* bld = Building::newFromStream( getMap(), memstream );
      bld->chainbuildingtofield( bld->getEntry() );
      if ( bld->getOwner() < bld->getMap()->getPlayerCount() )
         bld->addview();
   } else {
      Vehicle* veh = Vehicle::newFromStream( getMap(), memstream );

      if ( fieldRegistration == FIRST )
         getMap()->getField( veh->getPosition() )->vehicle = veh;
      else if ( fieldRegistration == CARRIER ) {
         ContainerBase* carrier = getMap()->getContainer( hostingCarrier );  
         carrier->addToCargo( veh, cargoSlot );
      }

      /*
      SECOND is not a permanent registration, so we don't redo it
      if ( fieldRegistration == SECOND )
         getMap()->getField( veh->getPosition() )->vehicle = veh;
      */
        
      if ( fieldRegistration != CARRIER )
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
