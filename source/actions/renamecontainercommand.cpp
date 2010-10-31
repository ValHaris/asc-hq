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


#include "renamecontainercommand.h"

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

bool RenameContainerCommand :: avail ( const ContainerBase* container )
{
   if ( !container  )
      return false;
   
   return true;
}


RenameContainerCommand :: RenameContainerCommand ( ContainerBase* container )
   : ContainerCommand ( container )
{

}



void RenameContainerCommand::setName( const ASCString& publicName, const ASCString& privateName )
{
   this->publicName = publicName;
   this->privateName = privateName;
   setState( SetUp );
}


ActionResult RenameContainerCommand::go ( const Context& context )
{
   if ( getState() != SetUp )
      return ActionResult(22000);

   if ( !avail( getContainer() ))
      return ActionResult(22800);
   
   ContainerBase* container = getContainer();
   
   originalPublicName = container->name;
   originalPrivateName = container->privateName;
   container->name = publicName;
   container->privateName = privateName;
   
   setState( Finished );

   return ActionResult(0);
}

ActionResult RenameContainerCommand::undoAction( const Context& context )
{
   ContainerBase* container = getContainer();
   if ( !container )
      return ActionResult(23600);
      
   container->name = originalPublicName;
   container->privateName = originalPrivateName;
   
   return ContainerCommand::undoAction( context );
}


static const int RenameContainerCommandVersion = 1;

void RenameContainerCommand :: readData ( tnstream& stream )
{
   ContainerCommand::readData( stream );
   int version = stream.readInt();
   if ( version > RenameContainerCommandVersion )
      throw tinvalidversion ( "RenameContainerCommand", RenameContainerCommandVersion, version );
   originalPublicName = stream.readString();
   originalPrivateName = stream.readString();
   publicName = stream.readString();
   privateName = stream.readString();
}

void RenameContainerCommand :: writeData ( tnstream& stream ) const
{
   ContainerCommand::writeData( stream );
   stream.writeInt( RenameContainerCommandVersion );
   stream.writeString( originalPublicName );
   stream.writeString( originalPrivateName );
   stream.writeString( publicName );
   stream.writeString( privateName );
}


ASCString RenameContainerCommand :: getCommandString() const
{
   ASCString c;
   c.format("renameContainer ( map, %d, '%s', '%s' )", getContainerID(), publicName.c_str(), privateName.c_str() );
   return c;

}

GameActionID RenameContainerCommand::getID() const
{
   return ActionRegistry::RenameContainerCommand;
}

ASCString RenameContainerCommand::getDescription() const
{
   ASCString s = "Rename ";
   
   if ( getContainer(true) ) {
      s += getContainer()->baseType->getName();
   } else
      s += originalPublicName;
   
   s += " to " + publicName;
   
   return s;
}

namespace
{
   const bool r1 = registerAction<RenameContainerCommand> ( ActionRegistry::RenameContainerCommand );
}

