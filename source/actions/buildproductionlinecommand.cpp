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


#include "buildproductionlinecommand.h"

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

bool BuildProductionLineCommand :: avail ( const ContainerBase* factory )
{
   if ( !factory  )
      return false;
   
   return factory->baseType->hasFunction( ContainerBaseType::InternalVehicleProduction ) 
      && !factory->baseType->hasFunction( ContainerBaseType::NoProductionCustomization ) ;
}


vector<const VehicleType*> BuildProductionLineCommand :: productionLinesBuyable()
{

   vector<const VehicleType*>  list;
   
   ContainerBase* container = getContainer();
   
   if ( !avail( container ))
      return list;
   
   Resources r = container->getResource( Resources(maxint, maxint, maxint), 1 );
   
   for ( int i = 0; i < vehicleTypeRepository.getNum(); ++i ) {
      VehicleType* veh = getMap()->getvehicletype_bypos ( i );
      if ( veh ) {
         bool found = find( container->getProduction().begin(), container->getProduction().end(), veh ) != container->getProduction().end();
         if ( container->baseType->vehicleFit ( veh ) && !found )
            if ( container->vehicleUnloadable(veh) || container->baseType->hasFunction( ContainerBaseType::ProduceNonLeavableUnits ))
               if ( veh->techDependency.available ( container->getMap()->getCurrentPlayer().research ))
                  if ( container->baseType->vehicleCategoriesProduceable & (1 << veh->movemalustyp))
                     list.push_back( veh );
      }
   }
   return list;
}

Resources BuildProductionLineCommand :: resourcesNeeded( const ContainerBaseType* factory, const VehicleType* veh )
{
   return factory->productionEfficiency * veh->productionCost * productionLineConstructionCostFactor ;
}



BuildProductionLineCommand :: BuildProductionLineCommand ( ContainerBase* container )
   : ContainerCommand ( container ), vehicleTypeId(-1)
{

}



void BuildProductionLineCommand::setProduction( const VehicleType* vehicleType )
{
   if ( vehicleType ) {
      vehicleTypeId = vehicleType->id;
      setState( SetUp );
   }
}


ActionResult BuildProductionLineCommand::go ( const Context& context )
{
   if ( getState() != SetUp )
      return ActionResult(22000);

   if ( !avail( getContainer() ))
      return ActionResult(22800);
   
   const VehicleType* vt = NULL;
   vector<const VehicleType*> types = productionLinesBuyable();
   for ( vector<const VehicleType*>::iterator i = types.begin(); i != types.end(); ++i )
      if ( (*i)->id == vehicleTypeId )
         vt = *i;
   
   if ( !vt )
      return ActionResult(22900);
   
   
   Resources needed = resourcesNeeded( getContainer()->baseType, vt );
   Resources avail = getContainer()->getResource( needed, true );
   if ( avail < needed  )
      return ActionResult(22901);
   
   auto_ptr<ConsumeResource> cr ( new ConsumeResource( getContainer(), needed ));
   ActionResult res = cr->execute(context);
   
   if ( !res.successful() ) {
      setState( Failed );
      return res;
   }
   
   getContainer()->addProductionLine( vt );
   
   cr.release();
   setState( Finished );

   return res;
}

ActionResult BuildProductionLineCommand::undoAction( const Context& context )
{
   VehicleType* vt = vehicleTypeRepository.getObject_byID( vehicleTypeId );
   if ( !vt )
      return ActionResult(22902);
         
   getContainer()->deleteProductionLine( vt );
   
   return ContainerCommand::undoAction( context );
}


static const int BuildProductionLineCommandVersion = 1;

void BuildProductionLineCommand :: readData ( tnstream& stream )
{
   ContainerCommand::readData( stream );
   int version = stream.readInt();
   if ( version > BuildProductionLineCommandVersion )
      throw tinvalidversion ( "BuildProductionLineCommand", BuildProductionLineCommandVersion, version );
   vehicleTypeId = stream.readInt();
}

void BuildProductionLineCommand :: writeData ( tnstream& stream ) const
{
   ContainerCommand::writeData( stream );
   stream.writeInt( BuildProductionLineCommandVersion );
   stream.writeInt( vehicleTypeId );
}


ASCString BuildProductionLineCommand :: getCommandString() const
{
   ASCString c;
   c.format("buildProductionLine ( map, %d, %d )", getContainerID(), vehicleTypeId );
   return c;

}

GameActionID BuildProductionLineCommand::getID() const
{
   return ActionRegistry::BuildProductionLineCommand;
}

ASCString BuildProductionLineCommand::getDescription() const
{
   ASCString s = "Add production line for type ";
   
   VehicleType* vt = vehicleTypeRepository.getObject_byID( vehicleTypeId );
   if ( !vt )
      s += ASCString::toString(vehicleTypeId);
   else
      s += vt->getName();
   
   if ( getContainer(true) ) {
      s += " to " + getContainer()->getName();
   }
   
   return s;
}

namespace
{
   const bool r1 = registerAction<BuildProductionLineCommand> ( ActionRegistry::BuildProductionLineCommand );
}

