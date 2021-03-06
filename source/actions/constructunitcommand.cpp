/*
     This file is part of Advanced Strategic Command; http://www.asc-hq.de
     Copyright (C) 1994-2010  Martin Bickel  and  Marc Schellenberger

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
#include "spawnunit.h"
#include "changeunitmovement.h"
#include "consumeammo.h"
#include "changeunitproperty.h"
#include "viewregistration.h"


bool ConstructUnitCommand :: externalConstructionAvail ( const ContainerBase* eht )
{
   const Vehicle* vehicle = dynamic_cast<const Vehicle*>(eht);
   if ( !eht )
      return false;
   
   MapField* fld = vehicle->getMap()->getField ( vehicle->getPosition() );
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
      if ( eht->vehiclesLoaded() < eht->baseType->maxLoadableUnits )
         return eht->baseType->hasFunction( ContainerBaseType::InternalVehicleProduction );
   
   return false;
}

bool ConstructUnitCommand :: avail ( const ContainerBase* eht )
{
   return internalConstructionAvail(eht) || externalConstructionAvail(eht);
}


ConstructUnitCommand :: ConstructUnitCommand ( ContainerBase* container )
   : ContainerCommand ( container ), mode( undefined ), vehicleTypeID(-1), newUnitID(-1)
{

}



ConstructUnitCommand::Lack ConstructUnitCommand::unitProductionPrerequisites( const VehicleType* type ) const
{
   int l = 0;
   
   if ( mode == internal ) {
      Resources cost = getContainer()->getProductionCost( type );
      for ( int r = 0; r < resourceTypeNum; r++ )
         if ( getContainer()->getAvailableResource( cost.resource(r), r ) < cost.resource(r) )
            l |= 1 << r;
      
      if ( !type->techDependency.available( getMap()->getPlayer(getContainer()).research ) && getMap()->getgameparameter( cgp_produceOnlyResearchedStuffInternally ) ) 
         l |= Lack::Research;
   
      if ( !getContainer()->vehicleUnloadable( type ) && !getContainer()->baseType->hasFunction( ContainerBaseType::ProduceNonLeavableUnits ))
         l |= Lack::Unloadability;
   } else 
      if ( mode == external ) {
      
         if ( !type->techDependency.available( getMap()->getPlayer(getContainer()).research ) && getMap()->getgameparameter( cgp_produceOnlyResearchedStuffExternally ) ) 
            l |= Lack::Research;
         
         const Vehicle* veh = dynamic_cast<const Vehicle*>(getContainer());
         if ( veh->getMovement() < veh->maxMovement() * veh->typ->unitConstructionMoveCostPercentage / 100 )
            l |= Lack::Movement;
         
         Resources cost = veh->getExternalVehicleConstructionCost( type );
         for ( int r = 0; r < resourceTypeNum; r++ )
            if ( getContainer()->getAvailableResource( cost.resource(r), r ) < cost.resource(r) )
               l |= 1 << r;
         
      }
      
   return Lack(l);
}

ConstructUnitCommand::Producables ConstructUnitCommand :: getProduceableVehicles( )
{
   Producables entries;
   if ( mode == internal ) {
      const ContainerBase::Production& prod = getContainer()->getProduction();
      for ( ContainerBase::Production::const_iterator i = prod.begin(); i != prod.end(); ++i ) 
         entries.push_back ( ProductionEntry ( *i, getContainer()->getProductionCost( *i ), unitProductionPrerequisites( *i  ) ));
   } else 
      if ( mode == external ) {
         Vehicle* veh = dynamic_cast<Vehicle*>(getContainer());
         if ( veh ) {
            ContainerConstControls cc ( getContainer() );
            for ( int i = 0; i < veh->typ->vehiclesBuildable.size(); i++ )
               for ( int j = veh->typ->vehiclesBuildable[i].from; j <= veh->typ->vehiclesBuildable[i].to; j++ )
                  if ( veh->getMap()->getgameparameter(cgp_forbid_unitunit_construction) == 0 || veh->getMap()->unitProduction.check(j) ) {
                     VehicleType* v = veh->getMap()->getvehicletype_byid ( j );
                     if ( v ) 
                        entries.push_back ( ProductionEntry ( v, veh->getExternalVehicleConstructionCost( v ), unitProductionPrerequisites( v ) ));
                  }
         }
      }
   return entries;
}

void ConstructUnitCommand :: fieldChecker( const MapCoordinate& pos )
{
   MapField* fld = getMap()->getField(pos);
   if ( !fld )
      return;
   
   if ( fld->vehicle || fld->building )
      return;
   
   VehicleType* vt = getContainer()->getMap()->getvehicletype_byid( vehicleTypeID );
   
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
      circularFieldIterator( veh->getMap(), veh->getPosition(), veh->typ->unitConstructionMaxDistance, veh->typ->unitConstructionMinDistance, FieldIterationFunctor( this, &ConstructUnitCommand::fieldChecker ) );
      for ( map<MapCoordinate,vector<int> >::const_iterator i = unitsConstructable.begin(); i != unitsConstructable.end(); ++i )
         fields.push_back ( i->first );
   }
   
   return fields;
}

bool ConstructUnitCommand :: isFieldUsable( const MapCoordinate& pos )
{
   vector<MapCoordinate> fields = getFields();
   return find( fields.begin(), fields.end(), pos ) != fields.end() ;
}


void ConstructUnitCommand :: setTargetPosition( const MapCoordinate& pos )
{
   this->target = pos;
   MapField* fld = getMap()->getField(target);
   
   if( !fld )
      throw ActionResult(21002);
   
   if ( mode != undefined && vehicleTypeID > 0  )
      setState( SetUp );
   
}

 

ActionResult ConstructUnitCommand::go ( const Context& context )
{
   if ( getState() != SetUp )
      return ActionResult(22000);

   
   Producables prods = getProduceableVehicles();
   
   const VehicleType* vehicleType = NULL;
   Resources cost;
   for ( Producables::const_iterator i = prods.begin(); i != prods.end(); ++i ) {
      if ( i->type->id == vehicleTypeID ) {
         if ( i->prerequisites.ok() ) {
            vehicleType = i->type;
            cost = i->cost;
         } else
            return ActionResult( 21702 );
      }
   }
   if ( !vehicleType )
      return ActionResult( 21701 );
   
   
   if ( mode == external && !isFieldUsable( target ))
      return ActionResult( 21703 );
  
   
   int height;
   for ( int j = 0; j < 8; j++ ) {
      int a = int( getContainer()->getHeight() ) << j;
      int b = int( getContainer()->getHeight() ) >> j;
      if ( vehicleType->height & a ) {
         height = a;
         break;
      }
      if ( vehicleType->height & b ) {
         height = b;
         break;
      }
   }
   MapCoordinate3D position ( target, height );

   if ( mode == external ) {
      
      SpawnUnit* spawnUnit = new SpawnUnit(getMap(), position, vehicleTypeID, getContainer()->getOwner() );
      ActionResult res = spawnUnit->execute( context );
      if ( !res.successful() )
         return res;
      
      spawnUnit->getUnit()->attacked = true;
      
      newUnitID = spawnUnit->getUnit()->networkid;
      
      
      if ( !res.successful() )
         return res;
      
      Vehicle* veh = dynamic_cast<Vehicle*>(getContainer() );
      if ( veh ) {      
         res = (new ChangeUnitMovement( veh, veh->maxMovement() * veh->typ->unitConstructionMoveCostPercentage/100, true ))->execute( context );
         if ( !res.successful() )
            return res;
      }
   } else {
      SpawnUnit* spawnUnit  = new SpawnUnit(getMap(), getContainer(), vehicleTypeID, newUnitID );
      ActionResult res = spawnUnit->execute( context );
      if ( !res.successful() )
         return res;
      
      spawnUnit->getUnit()->attacked = true;
      
      newUnitID = spawnUnit->getUnit()->networkid;
      
      if ( getMap()->getgameparameter(cgp_bi3_training) >= 1 ) {
         int cnt = 0;

         for ( Player::BuildingList::iterator bi = getMap()->player[getMap()->actplayer].buildingList.begin(); bi != getMap()->player[getMap()->actplayer].buildingList.end(); bi++ )
            if ( (*bi)->typ->hasFunction( ContainerBaseType::TrainingCenter  ) )
               cnt++;

         Vehicle* vehicle = spawnUnit->getUnit();
         vehicle->experience += cnt * getMap()->getgameparameter(cgp_bi3_training);
         if ( vehicle->experience > maxunitexperience )
            vehicle->experience = maxunitexperience;
      }
   }
   
   ActionResult res = (new ConsumeResource(getContainer(), cost ))->execute( context );
   
   if ( !res.successful() )
      return res;
   
   Vehicle* veh = dynamic_cast<Vehicle*>(getContainer());
   if ( veh )
      res = (new ChangeUnitProperty( veh, ChangeUnitProperty::AttackedFlag, 1 ))->execute(context);
   
   if ( context.display )
      context.display->repaintDisplay();
   
   if ( res.successful() )
      setState( Finished );
   else
      setState( Failed );
   
   return res;
}

Vehicle* ConstructUnitCommand :: getProducedUnit()
{
   if ( newUnitID > 0 )
      return getMap()->getUnit( newUnitID );
   else
      return NULL;
}

void ConstructUnitCommand :: setMode( Mode mode ) 
{ 
   this->mode = mode; 
   if ( mode == internal )
      target = getContainer()->getPosition();
};


static const int ConstructUnitCommandVersion = 1;

void ConstructUnitCommand :: readData ( tnstream& stream )
{
   ContainerCommand::readData( stream );
   int version = stream.readInt();
   if ( version > ConstructUnitCommandVersion )
      throw tinvalidversion ( "ConstructUnitCommand", ConstructUnitCommandVersion, version );
   target.read( stream );
   vehicleTypeID = stream.readInt();
   mode = (Mode) stream.readInt();
   newUnitID = stream.readInt();
}

void ConstructUnitCommand :: writeData ( tnstream& stream ) const
{
   ContainerCommand::writeData( stream );
   stream.writeInt( ConstructUnitCommandVersion );
   target.write( stream );
   stream.writeInt( vehicleTypeID );
   stream.writeInt( mode );
   stream.writeInt( newUnitID );
}

void ConstructUnitCommand :: setVehicleType( const VehicleType* type )
{
   vehicleTypeID = type->id;
   
   VehicleType* vt = getMap()->getvehicletype_byid( vehicleTypeID );
   if( !vt )
      throw ActionResult(21701);
   
   if ( mode != undefined && target.valid())
      setState( SetUp );
   
}


ASCString ConstructUnitCommand :: getCommandString() const
{
   ASCString c;
   c.format("constructUnit ( map, %d, asc.MapCoordinate( %d, %d), %d )", getContainerID(), target.x, target.y, vehicleTypeID );
   return c;
}

GameActionID ConstructUnitCommand::getID() const
{
   return ActionRegistry::ConstructUnitCommand;
}

ASCString ConstructUnitCommand::getDescription() const
{
   ASCString s = "Construct ";
   
   const VehicleType* vt = getMap()->getvehicletype_byid( vehicleTypeID );
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
   const bool r1 = registerAction<ConstructUnitCommand> ( ActionRegistry::ConstructUnitCommand );
}

