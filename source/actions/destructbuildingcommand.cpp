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


#include "destructbuildingcommand.h"

#include "../vehicle.h"
#include "../mapfield.h"
#include "../gamemap.h"
#include "../viewcalculation.h"
#include "../spfst.h"
#include "../mapdisplayinterface.h"
#include "action-registry.h"
#include "../itemrepository.h"
#include "../containercontrols.h"
#include "changeunitproperty.h"
#include "spawnbuilding.h"
#include "changeunitmovement.h"
#include "consumeresource.h"
#include "destructcontainer.h"



bool DestructBuildingCommand :: avail ( const Vehicle* eht )
{
   if ( !eht )
      return false;

   if ( eht->attacked == false && !eht->hasMoved() )
      if ( eht->getOwner() == eht->getMap()->actplayer )
            if ( eht->typ->hasFunction( ContainerBaseType::ConstructBuildings  ) || !eht->typ->buildingsBuildable.empty() )
               if ( eht->getTank().fuel >= destruct_building_fuel_usage * eht->typ->fuelConsumption )
                  return true;

   return false;
}


DestructBuildingCommand :: DestructBuildingCommand ( Vehicle* container )
      : UnitCommand ( container )
{

}

Resources DestructBuildingCommand::getDestructionCost( const Building* bld) const 
{
   Resources r;
   r.material = - bld->typ->productionCost.material * (100 - bld->damage) / destruct_building_material_get / 100;
   r.fuel = destruct_building_fuel_usage * getUnit()->typ->fuelConsumption;
   return r;
}


vector<MapCoordinate> DestructBuildingCommand::getFields()
{
   vector<MapCoordinate> fields;
   Vehicle* veh = getUnit();
   for ( int d = 0; d < 6; ++d ) {
      MapCoordinate pos = getNeighbouringFieldCoordinate( veh->getPosition(), d );
      tfield* fld = getMap()->getField(pos);
      if ( fld->building && getheightdelta( log2(veh->height), log2(fld->building->typ->height)) == 0 && !fld->building->typ->buildingNotRemovable ) 
         fields.push_back( pos );
   }

   return fields;
}

bool DestructBuildingCommand :: isFieldUsable( const MapCoordinate& pos )
{
   vector<MapCoordinate> fields = getFields();
   return find( fields.begin(), fields.end(), pos ) != fields.end() ;
}


void DestructBuildingCommand :: setTargetPosition( const MapCoordinate& pos )
{
   this->target = pos;
   tfield* fld = getMap()->getField(target);

   if ( !fld )
      throw ActionResult(21002);

   setState( SetUp );

}



ActionResult DestructBuildingCommand::go ( const Context& context )
{
   if ( getState() != SetUp )
      return ActionResult(21002);
   
   
   Building* building = getMap()->getField( target )->building;
   if ( !building )
      return ActionResult(22502);
   
   const BuildingType* buildingType = building->typ;
   
   Resources cost = getDestructionCost( building );
   
   auto_ptr<DestructContainer> dc ( new DestructContainer( building ));
   ActionResult res = dc->execute( context );
   if ( res.successful() )
      dc.release();
   else
      return res;
   
   auto_ptr<ConsumeResource> cr ( new ConsumeResource( getUnit(), cost ));
   res = cr->execute( context );
   if ( res.successful() )
      cr.release();
   else
      return res;
   
   auto_ptr<ChangeUnitMovement> cum ( new ChangeUnitMovement( getUnit(), 0 ));
   res = cum->execute( context );
   if ( res.successful() )
      cum.release();
   else
      return res;

   auto_ptr<ChangeUnitProperty> cup ( new ChangeUnitProperty( getUnit(), ChangeUnitProperty::AttackedFlag, 1 ));
   res = cup->execute( context );
   if ( res.successful() )
      cup.release();
   else
      return res;
   
   evaluateviewcalculation( getMap(), target, buildingType->view, 0, false, &context );

   
   if ( context.display )
      context.display->repaintDisplay();

   return ActionResult(0);
}



static const int DestructBuildingCommandVersion = 1;

void DestructBuildingCommand :: readData ( tnstream& stream )
{
   UnitCommand::readData( stream );
   int version = stream.readInt();
   if ( version > DestructBuildingCommandVersion )
      throw tinvalidversion ( "DestructBuildingCommand", DestructBuildingCommandVersion, version );
   target.read( stream );
}

void DestructBuildingCommand :: writeData ( tnstream& stream ) const
{
   UnitCommand::writeData( stream );
   stream.writeInt( DestructBuildingCommandVersion );
   target.write( stream );
}



ASCString DestructBuildingCommand :: getCommandString() const
{
   ASCString c;
   c.format("DestructBuilding ( %d, %d, %d )", getUnitID(), target.x, target.y );
   return c;
}

GameActionID DestructBuildingCommand::getID() const
{
   return ActionRegistry::DestructBuildingCommand;
}

ASCString DestructBuildingCommand::getDescription() const
{
   ASCString s = "Destruct building at "+ target.toString() ;


   if ( getUnit() ) {
      s += " with " + getUnit()->getName();
   }
   return s;
}

namespace
{
const bool r1 = registerAction<DestructBuildingCommand> ( ActionRegistry::DestructBuildingCommand );
}

