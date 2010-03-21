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


#include "setresourceprocessingratecommand.h"

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

bool SetResourceProcessingRateCommand :: avail ( const ContainerBase* container )
{
   if ( !container  )
      return false;
   
   return container->baseType->hasFunction( ContainerBaseType::MatterConverter ) 
         || container->baseType->hasFunction( ContainerBaseType::MiningStation ) ;
}




SetResourceProcessingRateCommand :: SetResourceProcessingRateCommand ( ContainerBase* container, int rate )
   : ContainerCommand ( container ), newRate(rate)
{
   setState( SetUp );
}


Resources SetResourceProcessingRateCommand::getNewPlus()
{
   Resources res;  
   ContainerBase* c = getContainer();
   for ( int r = 0; r < Resources::count; r++ )
      res.resource(r) = c->maxplus.resource(r) * newRate/100;
   
   return res;
}

      
ActionResult SetResourceProcessingRateCommand::go ( const Context& context )
{
   if ( getState() != SetUp )
      return ActionResult(22000);

   ContainerBase* c = getContainer();
   if ( !avail( c ))
      return ActionResult(22850);
   
   if ( newRate < 0 || newRate > 100 )
      return ActionResult(22851);
   
   oldRate = c->plus;
   c->plus = getNewPlus();
   
   setState( Finished );

   return ActionResult(0);
}

ActionResult SetResourceProcessingRateCommand::undoAction( const Context& context )
{
   getContainer()->plus = oldRate;
   return ContainerCommand::undoAction( context );
}


static const int SetResourceProcessingRateCommandVersion = 1;

void SetResourceProcessingRateCommand :: readData ( tnstream& stream )
{
   ContainerCommand::readData( stream );
   int version = stream.readInt();
   if ( version > SetResourceProcessingRateCommandVersion )
      throw tinvalidversion ( "SetResourceProcessingRateCommand", SetResourceProcessingRateCommandVersion, version );
   newRate = stream.readInt();
   oldRate.read( stream );
}

void SetResourceProcessingRateCommand :: writeData ( tnstream& stream ) const
{
   ContainerCommand::writeData( stream );
   stream.writeInt( SetResourceProcessingRateCommandVersion );
   stream.writeInt( newRate );
   oldRate.write( stream );
}


ASCString SetResourceProcessingRateCommand :: getCommandString() const
{
   ASCString c;
   c.format("setResourceProcessingRate ( map, %d, %d )", getContainerID(), newRate );
   return c;

}

GameActionID SetResourceProcessingRateCommand::getID() const
{
   return ActionRegistry::SetResourceProcessingRateCommand;
}

ASCString SetResourceProcessingRateCommand::getDescription() const
{
   ASCString s = "Set Resource processing rate of ";
   
   if ( getContainer(true) ) {
      s += getContainer()->getName();
   } else {
      s += " container with ID " + ASCString::toString( getContainerID() );
   }
   
   s += " to " + ASCString::toString( newRate ) + "%";
   return s;
}

namespace
{
   const bool r1 = registerAction<SetResourceProcessingRateCommand> ( ActionRegistry::SetResourceProcessingRateCommand );
}

