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


#include "unitaction.h"

#include "../vehicle.h"
#include "../gamemap.h"

UnitAction::UnitAction( Vehicle* unit )
   : GameAction( unit->getMap() )
{
   vehicleID = unit->networkid;
}     


UnitAction::UnitAction( GameMap* gamemap )
   : GameAction( gamemap )
{
   vehicleID = -1;
}     
     
UnitAction::UnitAction( GameMap* gamemap, int vehicleID )
   : GameAction( gamemap )
{
   this->vehicleID = vehicleID;
}
      
      
Vehicle* UnitAction::getUnit( bool dontThrow )
{
   Vehicle* veh = getMap()->getUnit( vehicleID );
   if ( !veh && !dontThrow )
      throw ActionResult(21001, "ID is " + ASCString::toString(vehicleID) );
   else
      return veh;
}
      
const Vehicle* UnitAction::getUnit( bool dontThrow ) const 
{
   const Vehicle* veh = getMap()->getUnit( vehicleID );
   if ( !veh && !dontThrow )
      throw ActionResult(21001, "ID is " + ASCString::toString(vehicleID) );
   else
      return veh;
}

void UnitAction::readData ( tnstream& stream ) 
{
   int version = stream.readInt();
   if ( version != 1 )
      throw tinvalidversion ( "UnitAction", 1, version );
   
   vehicleID = stream.readInt();
};
      
      
void UnitAction::writeData ( tnstream& stream )  const
{
   stream.writeInt( 1 );
   stream.writeInt( vehicleID );
};


