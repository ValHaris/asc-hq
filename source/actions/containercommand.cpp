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


#include "containercommand.h"

#include "../vehicle.h"
#include "../gamemap.h"
     
ContainerCommand::ContainerCommand( ContainerBase* container )
   : Command( container->getMap() )
{
   this->containerID = container->getIdentification();
}

ContainerCommand::ContainerCommand( GameMap* map )
   : Command( map )
{
   this->containerID = -1;
}
 
      
      
ContainerBase* ContainerCommand::getContainer( bool dontThrow )
{
   ContainerBase* veh = getMap()->getContainer( containerID );
   if ( !veh && !dontThrow )
      throw ActionResult(21001, "ID is " + ASCString::toString(containerID) );
   else
      return veh;
}
      
const ContainerBase* ContainerCommand::getContainer( bool dontThrow ) const 
{
   const ContainerBase* veh = getMap()->getContainer( containerID );
   if ( !veh && !dontThrow )
      throw ActionResult(21001, "ID is " + ASCString::toString(containerID) );
   else
      return veh;
}

void ContainerCommand::readData ( tnstream& stream ) 
{
   Command::readData( stream );
   int version = stream.readInt();
   if ( version != 1 )
      throw tinvalidversion ( "containerID", 1, version );
   
   containerID = stream.readInt();
};
      
      
void ContainerCommand::writeData ( tnstream& stream ) const
{
   Command::writeData( stream );
   stream.writeInt( 1 );
   stream.writeInt( containerID );
};

ActionResult ContainerCommand::checkExecutionPrecondition() const
{
   if ( getMap()->actplayer != getContainer()->getOwner() )
      return ActionResult(101);
   else
      return ActionResult(0);
}

