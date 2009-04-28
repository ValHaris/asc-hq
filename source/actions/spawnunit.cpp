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


#include "spawnunit.h"
#include "viewregistration.h"
#include "unitfieldregistration.h"
#include "action-registry.h"

#include "../vehicle.h"
#include "../gamemap.h"
#include "../viewcalculation.h"
     
SpawnUnit::SpawnUnit( GameMap* gamemap, const MapCoordinate3D& position, int vehicleTypeID, int owner )
   : GameAction( gamemap ), pos(position), networkid(-1), carrierID(0), carrier( false ), mapNetworkIdCounterBefore(0), mapNetworkIdCounterAfter(0)
{
   this->vehicleTypeID = vehicleTypeID;
   this->owner = owner;
   
}
            
SpawnUnit::SpawnUnit( GameMap* gamemap, const ContainerBase* carrier, int vehicleTypeID )
   : GameAction( gamemap ), pos( carrier->getPosition() ), networkid(-1), carrierID( carrier->getIdentification() ), carrier(true), mapNetworkIdCounterBefore(0), mapNetworkIdCounterAfter(0)
{
   this->vehicleTypeID = vehicleTypeID;
   this->owner = carrier->getOwner();
}
            
            
ContainerBase* SpawnUnit::getCarrier( bool dontThrow )
{
   ContainerBase* veh = getMap()->getContainer( carrierID );
   if ( !veh && !dontThrow )
      throw ActionResult(21001, "ID is " + ASCString::toString(carrierID) );
   else
      return veh;
}
      
      
ASCString SpawnUnit::getDescription() const
{
   const Vehicletype* veh = getMap()->getvehicletype_byid( vehicleTypeID );
   ASCString res ;
   if ( veh )
      res = "Spawn Unit of type " + veh->getName();
   else
      res = "Spawn Object " ;
   res += " at " + pos.toString();
   return  res;
}
      
      
static const int spawnUnitStreamVersion = 2;
      
void SpawnUnit::readData ( tnstream& stream ) 
{
   int version = stream.readInt();
   if ( version < 1 || version > spawnUnitStreamVersion )
      throw tinvalidversion ( "SpawnUnit", spawnUnitStreamVersion, version );
   
   vehicleTypeID = stream.readInt();
   pos.read( stream );
   owner = stream.readInt();
   networkid = stream.readInt();
   carrierID = stream.readInt();
   carrier = stream.readInt();
   
   if ( version >= 2 ) {
      mapNetworkIdCounterAfter = stream.readInt();
      mapNetworkIdCounterBefore = stream.readInt();
   }
};
      
      
void SpawnUnit::writeData ( tnstream& stream ) const
{
   stream.writeInt( spawnUnitStreamVersion );
   stream.writeInt( vehicleTypeID );
   pos.write( stream );
   stream.writeInt( owner );
   stream.writeInt( networkid );
   stream.writeInt( carrierID );
   stream.writeInt( carrier );
   stream.writeInt( mapNetworkIdCounterAfter );
   stream.writeInt( mapNetworkIdCounterBefore );
};


GameActionID SpawnUnit::getID() const
{
   return ActionRegistry::SpawnUnit;
}

ActionResult SpawnUnit::runAction( const Context& context )
{
   tfield* fld = getMap()->getField(pos);
   if ( !fld )
      return ActionResult( 21002, pos );
   
   const Vehicletype* vehicleType = getMap()->getvehicletype_byid( vehicleTypeID );
   if ( !vehicleType )
      return ActionResult( 21801, "Vehicle id is " + ASCString::toString(vehicleTypeID));
   
   mapNetworkIdCounterBefore = getMap()->idManager.unitnetworkid;

   Vehicle* v = new Vehicle( vehicleType, getMap(), owner );
   networkid= v->networkid;
   
   mapNetworkIdCounterAfter = getMap()->idManager.unitnetworkid;
   
   v->setMovement ( 0 );
   
   
   if ( !carrier ) {
      v->xpos = pos.x;
      v->ypos = pos.y;
      v->height = pos.getBitmappedHeight();
      // RecalculateAreaView rav( getMap(), pos, vehicleType->view/10, &context );
      ActionResult res = (new UnitFieldRegistration(v, pos, UnitFieldRegistration::RegisterOnField ))->execute( context );
      if ( res.successful() )
         res = (new ViewRegistration( v, ViewRegistration::AddView ))->execute( context );
      
      evaluateviewcalculation(getMap(), pos, vehicleType->view, 0, false, &context);
      return res;
   } else {
      ContainerBase* hostingCarrier = getMap()->getContainer( carrierID );
      if ( !hostingCarrier )
         return ActionResult( 21303 );
      
      ActionResult res = (new UnitFieldRegistration(v, pos, UnitFieldRegistration::RegisterInCarrier, hostingCarrier ))->execute( context );
      return res;
   }
   
   return ActionResult(0);
}

Vehicle* SpawnUnit::getUnit()
{
   return getMap()->getUnit( networkid ); 
}


ActionResult SpawnUnit::undoAction( const Context& context )
{
   tfield* fld = getMap()->getField(pos);
   if ( !fld )
      return ActionResult( 21002, pos );
   
   if ( getMap()->idManager.unitnetworkid != mapNetworkIdCounterAfter )
      return ActionResult( 21805 );
   
   getMap()->idManager.unitnetworkid = mapNetworkIdCounterBefore;
   
   const Vehicletype* vehicleType = getMap()->getvehicletype_byid( vehicleTypeID );
   if ( !vehicleType )
      return ActionResult( 21801, "Vehicle id is " + ASCString::toString(vehicleTypeID));
   
   Vehicle* veh = getUnit();
   if ( !veh )
      return ActionResult( 21802 );
      
   if ( veh->typ->id != vehicleTypeID  )
      return ActionResult( 21803 );
   
   delete veh;
   return ActionResult(0);
}

ActionResult SpawnUnit::verify()
{
   if ( getMap()->idManager.unitnetworkid != mapNetworkIdCounterAfter )
      return ActionResult( 21805 );
   
   return ActionResult(0);
}


namespace {
   const bool r1 = registerAction<SpawnUnit> ( ActionRegistry::SpawnUnit );
}

