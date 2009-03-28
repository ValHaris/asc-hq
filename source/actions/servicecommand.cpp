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


#include "servicecommand.h"

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
#include "servicing.h"
#include "changeunitmovement.h"
#include "consumeammo.h"


bool ServiceCommand :: availExternally ( ContainerBase* eht )
{
   ServiceTargetSearcher sts( eht, ServiceTargetSearcher::checkAmmo + ServiceTargetSearcher::checkResources );
   return sts.externallyAvailable();
}

bool ServiceCommand :: avail ( ContainerBase* source, ContainerBase* target )
{
   if ( target->getCarrier() == source ) {
      ServiceCommand sc( source );
      const ServiceTargetSearcher::Targets& dest  = sc.getDestinations();
      
      return find( dest.begin(), dest.end(), target )  != dest.end();
      
   } else {
      ServiceTargetSearcher sts( source, ServiceTargetSearcher::checkAmmo + ServiceTargetSearcher::checkResources );
      if ( !sts.externallyAvailable())
         return false;
      
      return find( sts.getTargets().begin(), sts.getTargets().end(), target ) != sts.getTargets().end();
   }
}


ServiceCommand :: ServiceCommand ( ContainerBase* container )
   : ContainerCommand ( container ), targetSearcher(NULL), transferHandler(NULL),destinationSpecified(false), destinationContainerID(0)
{

}

const ServiceTargetSearcher::Targets& ServiceCommand::getDestinations()
{
   delete targetSearcher;
   targetSearcher = new ServiceTargetSearcher( getContainer(), ServiceTargetSearcher::checkAmmo + ServiceTargetSearcher::checkResources );
   targetSearcher->startSearch();
   return targetSearcher->getTargets();  
}
 
void ServiceCommand::setDestination( ContainerBase* destination )
{
   destinationSpecified = true;
   destinationContainerID = destination->getIdentification();
}
 
ContainerBase* ServiceCommand::getDestination()
{
   if ( !destinationSpecified )  
      return NULL;
   
   return getMap()->getContainer( destinationContainerID );
}

 
TransferHandler& ServiceCommand::getTransferHandler()
{
   delete transferHandler;
   transferHandler = NULL;
   if ( !getDestination() ) 
      throw ActionResult( 22000 );
      
   transferHandler = new TransferHandler( getContainer(), getDestination() );
   return *transferHandler;
}
 
void ServiceCommand::saveTransfers()
{
   values.clear();
   if ( transferHandler ) {
      TransferHandler::Transfers& transfers = transferHandler->getTransfers();
      for ( TransferHandler::Transfers::iterator i = transfers.begin(); i != transfers.end(); ++i )
         values[ (*i)->getID()] = (*i)->getAmount( (*i)->getDstContainer() );
      
      setState( SetUp );
   }
}
 

ActionResult ServiceCommand::go ( const Context& context )
{
   if ( getState() != SetUp )
      return ActionResult(21002);

   
   TransferHandler& handler = getTransferHandler();
   
   for ( Values::iterator i = values.begin(); i != values.end(); ++i ) {
      TransferHandler::Transfers& transfers = handler.getTransfers();
      for ( TransferHandler::Transfers::iterator t = transfers.begin(); t != transfers.end(); ++t )
         if ( (*t)->getID() == i->first ) {
            if ( !(*t)->setDestAmount( i->second ) )
               return ActionResult( 22001, (*t)->getName() );
            (*t)->commit( context );
         }
   }
   
   setState( Completed );
      
/*   
   ActionResult res = (new ConsumeResource(getContainer(), cost ))->execute( context );
   
   if ( context.display )
      context.display->repaintDisplay();
   
   if ( res.successful() )
      setState( Completed );
   else
      setState( Failed );
   
   return res;*/
   ActionResult res(0);
   return res;
}


static const int ServiceCommandVersion = 1;

void ServiceCommand :: readData ( tnstream& stream )
{
   ContainerCommand::readData( stream );
   int version = stream.readInt();
   if ( version > ServiceCommandVersion )
      throw tinvalidversion ( "ServiceCommand", ServiceCommandVersion, version );
   
   destinationSpecified = stream.readInt();
   destinationContainerID = stream.readInt();
   
   int size = stream.readInt();
   for ( int i = 0; i < size; ++i )  {
      int key = stream.readInt();
      int value = stream.readInt();
      values[key] = value;
   }
}

void ServiceCommand :: writeData ( tnstream& stream ) const
{
   ContainerCommand::writeData( stream );
   stream.writeInt( ServiceCommandVersion );
   
   stream.writeInt( destinationSpecified );
   stream.writeInt( destinationContainerID );
   
   stream.writeInt( values.size() );
   for ( Values::const_iterator i = values.begin(); i != values.end(); ++i ) {
      stream.writeInt( i->first );
      stream.writeInt( i->second );
   }
      
}


ASCString ServiceCommand :: getCommandString() const
{
   ASCString c;
   c.format("ServiceCommand " );
   return c;
}

GameActionID ServiceCommand::getID() const
{
   return ActionRegistry::ServiceCommand;
}

ASCString ServiceCommand::getDescription() const
{
   ASCString s = "Service with ";
   
   if ( getContainer() ) {
      s += " by " + getContainer()->getName();
   }
   return s;
}


ServiceCommand::~ServiceCommand()
{
   delete targetSearcher;  
   delete transferHandler;
}

namespace
{
   const bool r1 = registerAction<ServiceCommand> ( ActionRegistry::ServiceCommand );
}

