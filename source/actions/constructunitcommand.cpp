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


#include "constructunitcommand.h"

#include "../vehicle.h"
#include "../mapfield.h"
#include "../gamemap.h"
#include "../viewcalculation.h"
#include "../spfst.h"
#include "../mapdisplayinterface.h"
#include "action-registry.h"
#include "../itemrepository.h"
#include "../containercontrols.h"
#include "consumeresource.h"


bool ConstructUnitCommand :: externalConstructionAvail ( const ContainerBase* eht )
{
   const Vehicle* vehicle = dynamic_cast<const Vehicle*>(eht);
   if ( !eht )
      return false;
   
   tfield* fld = vehicle->getMap()->getField ( vehicle->getPosition() );
   if ( fld && fld->vehicle == vehicle )
      if (vehicle->getOwner() == vehicle->getMap()->actplayer )
         if ( vehicle->typ->vehiclesBuildable.size() )
            if ( !vehicle->attacked )
               return true;
   return false;
}

bool ConstructUnitCommand :: internalConstructionAvail( const ContainerBase* eht )
{
   if ( eht->getOwner() == eht->getMap()->actplayer )
      return eht->baseType->hasFunction( ContainerBaseType::InternalVehicleProduction );
}

bool ConstructUnitCommand :: avail ( const ContainerBase* eht )
{
   return internalConstructionAvail(eht) || externalConstructionAvail(eht);
}


ConstructUnitCommand :: ConstructUnitCommand ( ContainerBase* container )
   : ContainerCommand ( container ), state( undefined ), vehicleTypeID(-1)
{

}


map<const Vehicletype*,ConstructUnitCommand::Lack> ConstructUnitCommand :: getProduceableVehicles(bool internally )
{
   map<const Vehicletype*, Lack> avail;
   if ( internally ) {
      ContainerConstControls cc ( getContainer() );
      
      const ContainerBase::Production& prod = getContainer()->getProduction();
      for ( ContainerBase::Production::const_iterator i = prod.begin(); i != prod.end(); ++i ) {
         int req =  cc.unitProductionPrerequisites( *i );
         if ( !(req & ~(1+2+4)))  // we are filtering out the bits for lack of resource
            avail[*i] = req;
      }
   } else {
      Vehicle* veh = dynamic_cast<Vehicle*>(getContainer());
      if ( veh ) {
         ContainerConstControls cc ( getContainer() );
         for ( int i = 0; i < veh->typ->vehiclesBuildable.size(); i++ )
            for ( int j = veh->typ->vehiclesBuildable[i].from; j <= veh->typ->vehiclesBuildable[i].to; j++ )
               if ( veh->getMap()->getgameparameter(cgp_forbid_unitunit_construction) == 0 || veh->getMap()->unitProduction.check(j) ) {
                  Vehicletype* v = veh->getMap()->getvehicletype_byid ( j );
                  if ( v ) {
                     int req =  cc.unitProductionPrerequisites( v );
                     if ( !(req & ~(1+2+4)))  // we are filtering out the bits for lack of resource
                        avail[v] = req;
                  }
               }
      }
   }
   return avail;
}

void ConstructUnitCommand :: fieldChecker( const MapCoordinate& pos )
{
   tfield* fld = getMap()->getField(pos);
   if ( !fld )
      return;
   
   if ( fld->vehicle || fld->building )
      return;
   
   Vehicletype* vt = getContainer()->getMap()->getvehicletype_byid( vehicleTypeID );
   
   if ( fieldvisiblenow( getMap()->getField(pos)) && vt ) {
      Vehicle* veh = dynamic_cast<Vehicle*>(getContainer() );
      if ( veh && veh->vehicleconstructable( vt, pos.x, pos.y ))
         unitsConstructable[pos].push_back(vehicleTypeID);
   }
}


vector<MapCoordinate> ConstructUnitCommand::getFields()
{
   vector<MapCoordinate> fields;
   Vehicle* veh = dynamic_cast<Vehicle*>(getContainer() );
   if ( vehicleTypeID > 0 && veh ) {
      // circularFieldIterator( veh->getMap(), veh->getPosition(), veh->typ->unitConstructionMaxDistance, veh->typ->unitConstructionMinDistance, FieldIterationFunctor( *this, &ConstructUnitCommand::fieldChecker ) );
      for ( map<MapCoordinate,vector<int> >::const_iterator i = unitsConstructable.begin(); i != unitsConstructable.end(); ++i )
         fields.push_back ( i->first );
   }
   
   return fields;
}


ActionResult ConstructUnitCommand::searchFields()
{
   if ( !getContainer() )
      return ActionResult(201);

   unitsConstructable.clear();

   // circularFieldIterator( getMap(), getContainer()->getPosition(), 1, 1, FieldIterationFunctor( this, &ConstructUnitCommand::fieldChecker ));

   if ( unitsConstructable.size() ) {
      setState(Evaluated);
      return ActionResult(0);
   } else
      return ActionResult(21700);
}


void ConstructUnitCommand :: setTarget( const MapCoordinate& target, int vehicleTypeID )
{
   this->target = target;
   this->vehicleTypeID = vehicleTypeID;

   tfield* fld = getMap()->getField(target);
   
   if( !fld )
      throw ActionResult(21002);
   
   Vehicletype* vt = getMap()->getvehicletype_byid( vehicleTypeID );
   if( !vt )
      throw ActionResult(21701);
   
   if ( state != undefined )
      setState( SetUp );
}

ActionResult ConstructUnitCommand::go ( const Context& context )
{
   MapCoordinate targetPosition;

   if ( getState() != SetUp )
      return ActionResult(21002);

   searchFields();

   // Vehicletype* vt = getMap()->getvehicletype_byid( vehicleTypeID );
   
   // RecalculateAreaView rav ( actmap, target, maxViewRange / maxmalq + 1, &context );
      
   ActionResult res(0);
   return res;

}

static const int ConstructUnitCommandVersion = 1;

void ConstructUnitCommand :: readData ( tnstream& stream )
{
   ContainerCommand::readData( stream );
   int version = stream.readInt();
   if ( version > ConstructUnitCommandVersion )
      throw tinvalidversion ( "ConstructUnitCommand", ConstructUnitCommandVersion, version );
   target.read( stream );
   vehicleTypeID = stream.readInt();
   state = (Mode) stream.readInt();
}

void ConstructUnitCommand :: writeData ( tnstream& stream ) const
{
   ContainerCommand::writeData( stream );
   stream.writeInt( ConstructUnitCommandVersion );
   target.write( stream );
   stream.writeInt( vehicleTypeID );
   stream.writeInt( state );
}

ASCString ConstructUnitCommand :: getCommandString() const
{
   ASCString c;
   c.format("ConstructVehicle ( %d, %d, %d, %d )", getContainer()->getIdentification(), target.x, target.y, vehicleTypeID );
   return c;

}

GameActionID ConstructUnitCommand::getID() const
{
   return ActionRegistry::ConstructUnitCommand;
}

ASCString ConstructUnitCommand::getDescription() const
{
   ASCString s = "Construct ";
   
   const Vehicletype* vt = getMap()->getvehicletype_byid( vehicleTypeID );
   if ( vt )
      s += vt->name;
   else
      s += "unit";
   
   if ( getContainer() ) {
      s += " by " + getContainer()->getName();
   }
   s += " at " + target.toString();
   return s;
}

namespace
{
//   const bool r1 = registerAction<ConstructUnitCommand> ( ActionRegistry::ConstructUnitCommand );
}

