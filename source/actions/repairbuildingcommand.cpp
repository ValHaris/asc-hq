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


#include "repairbuildingcommand.h"

#include "../vehicle.h"
#include "../mapfield.h"
#include "../gamemap.h"
#include "../viewcalculation.h"
#include "../spfst.h"
#include "../mapdisplayinterface.h"
#include "action-registry.h"
#include "changeunitproperty.h"
#include "changecontainerproperty.h"
#include "consumeresource.h"


      
bool RepairBuildingCommand :: avail ( const Building* building )
{
   if ( !building )
      return false;
   
   if ( !building->damage )
      return false;
   else
      return building->repairableDamage() > 0;
   
}


RepairBuildingCommand :: RepairBuildingCommand ( Building* building )
   : ContainerCommand ( building )
{
   if ( avail( building ))
      setState( SetUp );
}

RepairBuildingCommand::RepairData RepairBuildingCommand::getCost()
{
   RepairData data;
   data.newDamage = getContainer()->getMaxRepair ( getContainer(), 0, data.cost  );
   data.damageDelta = getContainer()->damage - data.newDamage;
   return data;   
}




ActionResult RepairBuildingCommand::go ( const Context& context )
{
   if ( getState() != SetUp )
      return ActionResult(22000);

   ContainerBase* servicer = getContainer();
   Building* building = dynamic_cast<Building*>(servicer);
   if ( !building )
      return ActionResult( 22700 );
   
   if ( !avail( building ))
      return ActionResult( 22700 );
   
   
   RepairData data = getCost();
   
   auto_ptr<ChangeContainerProperty> propChange ( new ChangeContainerProperty( building, ChangeContainerProperty::Damage, data.newDamage ));
   ActionResult res = propChange->execute( context );
   if ( res.successful() )
      propChange.release();
   else
      return res;
   
   auto_ptr<ChangeContainerProperty> propChange2 ( new ChangeContainerProperty( building, ChangeContainerProperty::RepairedThisTurn, data.damageDelta, false ));
   res = propChange2->execute( context );
   if ( res.successful() )
      propChange2.release();
   else
      return res;
   
   
   auto_ptr<ConsumeResource> resource ( new ConsumeResource( getContainer(), data.cost ));
   res = resource->execute( context );
   if ( res.successful() )
      resource.release();
   
   return res;
}



static const int RepairBuildingCommandVersion = 1;

void RepairBuildingCommand :: readData ( tnstream& stream )
{
   ContainerCommand::readData( stream );
   int version = stream.readInt();
   if ( version > RepairBuildingCommandVersion )
      throw tinvalidversion ( "RepairBuildingCommand", RepairBuildingCommandVersion, version );
}

void RepairBuildingCommand :: writeData ( tnstream& stream ) const
{
   ContainerCommand::writeData( stream );
   stream.writeInt( RepairBuildingCommandVersion );
}


ASCString RepairBuildingCommand :: getCommandString() const
{
   ASCString c;
   c.format("RepairBuilding ( %d )", getContainerID() );
   return c;

}

GameActionID RepairBuildingCommand::getID() const
{
   return ActionRegistry::RepairBuildingCommand;
}

ASCString RepairBuildingCommand::getDescription() const
{
   ASCString s = "Repair building " + ASCString::toString( getContainerID() );
   
   return s;
}

namespace
{
   const bool r1 = registerAction<RepairBuildingCommand> ( ActionRegistry::RepairBuildingCommand );
}

