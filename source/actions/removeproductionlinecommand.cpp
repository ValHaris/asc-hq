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


#include "removeproductionlinecommand.h"

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
#include "servicecommand.h"
#include "convertcontainer.h"

bool RemoveProductionLineCommand :: avail ( const ContainerBase* factory )
{
   if ( !factory  )
      return false;
   
   return factory->baseType->hasFunction( ContainerBaseType::InternalVehicleProduction ) 
      && !factory->baseType->hasFunction( ContainerBaseType::NoProductionCustomization ) ;
}


Resources RemoveProductionLineCommand :: resourcesNeeded( const Vehicletype* veh )
{
   return veh->productionCost * productionLineRemovalCostFactor;
}



RemoveProductionLineCommand :: RemoveProductionLineCommand ( ContainerBase* container )
   : ContainerCommand ( container ), vehicleTypeId(-1)
{

}



void RemoveProductionLineCommand::setRemoval( const Vehicletype* vehicleType )
{
   if ( vehicleType ) {
      vehicleTypeId = vehicleType->id;
      setState( SetUp );
   }
}


ActionResult RemoveProductionLineCommand::go ( const Context& context )
{
   if ( getState() != SetUp )
      return ActionResult(21002);

   if ( !avail( getContainer() ))
      return ActionResult(22800);
   
   const Vehicletype* vt = NULL;
   const ContainerBase::Production production = getContainer()->getProduction();
   for ( ContainerBase::Production::const_iterator i = production.begin(); i != production.end(); ++i )
      if ( (*i)->id == vehicleTypeId )
         vt = *i;
   
   if ( !vt )
      return ActionResult(22900);
   
   Resources needed = resourcesNeeded( vt );
   Resources avail = getContainer()->getResource( needed, true );
   if ( avail < needed  )
      return ActionResult(22901);
   
   auto_ptr<ConsumeResource> cr ( new ConsumeResource( getContainer(), needed ));
   ActionResult res = cr->execute(context);
   
   if ( !res.successful() ) {
      setState( Failed );
      return res;
   }
   
   getContainer()->deleteProductionLine( vt );
   
   cr.release();
   setState( Completed );

   return res;
}

ActionResult RemoveProductionLineCommand::undoAction( const Context& context )
{
   Vehicletype* vt = vehicleTypeRepository.getObject_byID( vehicleTypeId );
   if ( !vt )
      return ActionResult(22902);
         
   getContainer()->addProductionLine( vt );
   
   return ContainerCommand::undoAction( context );
}


static const int RemoveProductionLineCommandVersion = 1;

void RemoveProductionLineCommand :: readData ( tnstream& stream )
{
   ContainerCommand::readData( stream );
   int version = stream.readInt();
   if ( version > RemoveProductionLineCommandVersion )
      throw tinvalidversion ( "RemoveProductionLineCommand", RemoveProductionLineCommandVersion, version );
   vehicleTypeId = stream.readInt();
}

void RemoveProductionLineCommand :: writeData ( tnstream& stream ) const
{
   ContainerCommand::writeData( stream );
   stream.writeInt( RemoveProductionLineCommandVersion );
   stream.writeInt( vehicleTypeId );
}


ASCString RemoveProductionLineCommand :: getCommandString() const
{
   ASCString c;
   c.format("RemoveProductionLine ( %d, %d )", getContainerID(), vehicleTypeId );
   return c;

}

GameActionID RemoveProductionLineCommand::getID() const
{
   return ActionRegistry::RemoveProductionLineCommand;
}

ASCString RemoveProductionLineCommand::getDescription() const
{
   ASCString s = "Remove production line ";
   
   s += "of type " + ASCString::toString(vehicleTypeId);
   
   if ( getContainer(true) ) {
      s += " from " + getContainer()->getName();
   }
   
   return s;
}

namespace
{
   const bool r1 = registerAction<RemoveProductionLineCommand> ( ActionRegistry::RemoveProductionLineCommand );
}

