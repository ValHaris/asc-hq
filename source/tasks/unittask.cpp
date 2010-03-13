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


#include "unittask.h"

#include "../gamemap.h"

Vehicle* UnitTask::getUnit()
{
   return getMap()->getUnit( unitNetworkID );
}

UnitTask::UnitTask( GameMap* gamemap )
   : Task( gamemap, -1 ), unitNetworkID( -1 )
{
   
}


UnitTask::UnitTask( Vehicle* unit )
   : Task( unit->getMap()->getPlayer( unit ) )
{
   unitNetworkID = unit->networkid;
}

UnitTask::UnitTask( GameMap* gamemap, int unitID )
   : Task( gamemap, gamemap->getUnit(unitID)->getOwner() ), unitNetworkID ( unitID )
{
};


void UnitTask::readData ( tnstream& stream )
{
   Task::readData( stream );
   unitNetworkID = stream.readInt();
}

void UnitTask::writeData ( tnstream& stream )
{
   Task::writeData( stream );
   stream.writeInt( unitNetworkID );
}

