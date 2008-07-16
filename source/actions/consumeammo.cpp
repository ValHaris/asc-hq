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


#include "consumeammo.h"

#include "../vehicle.h"
#include "../gamemap.h"
     
ConsumeAmmo::ConsumeAmmo( GameMap* gamemap, int vehicleID, int ammoType, int slot, int count )
   : GameAction( gamemap )
{
   this->vehicleID = vehicleID;
   this->ammoType = ammoType;
   this->slot = slot;
   this->count = count;
   
   resultingAmmount = -1;
}
      
      
ASCString ConsumeAmmo::getDescription() const
{
   ASCString res = "Consume " + ASCString::toString(count) + " pieces of ammo from slot " + ASCString::toString(slot);
   const Vehicle* veh = getMap()->getUnit( vehicleID );
   if ( veh ) 
      res += " of unit " + veh->getName();
   return  res;
}
      
      
void ConsumeAmmo::readData ( tnstream& stream ) 
{
   int version = stream.readInt();
   if ( version != 1 )
      throw tinvalidversion ( "ConsumeAmmo", 1, version );
   
   vehicleID = stream.readInt();
   ammoType = stream.readInt();
   slot = stream.readInt();
   count = stream.readInt();
   
   resultingAmmount = stream.readInt();
   
};
      
      
void ConsumeAmmo::writeData ( tnstream& stream )
{
   stream.writeInt( 1 );
   stream.writeInt( vehicleID );
   stream.writeInt( ammoType );
   stream.writeInt( slot );
   stream.writeInt( count );
   stream.writeInt( resultingAmmount );
};


GameActionID ConsumeAmmo::getID() const
{
   return ActionRegistry::ConsumeAmmo;
}

ActionResult ConsumeAmmo::runAction( const Context& context )
{
   Vehicle* veh = getMap()->getUnit( vehicleID );
   if ( veh == NULL )
      return ActionResult( 21001, veh);
   
   if ( veh->ammo[slot] < count )
      return ActionResult( 21100, veh);
   
   veh->ammo[slot] -= count;
   resultingAmmount = veh->ammo[slot];
   return ActionResult(0);
}


ActionResult ConsumeAmmo::undoAction( const Context& context )
{
   Vehicle* veh = getMap()->getUnit( vehicleID );
   if ( veh == NULL )
      return ActionResult( 21001, veh);
   
   if ( veh->ammo[slot] + count > veh->typ->weapons.weapon[slot].count )
      return ActionResult( 21101, veh);
   
   veh->ammo[slot] += count;
   return ActionResult(0);
}

ActionResult ConsumeAmmo::postCheck()
{
   Vehicle* veh = getMap()->getUnit( vehicleID );
   if ( veh == NULL )
      return ActionResult( 21001, veh);
   
   if ( veh->ammo[slot] != resultingAmmount )
      return ActionResult( 21102, veh );  
   
   return ActionResult(0);
}


namespace {
   const bool r1 = registerAction<ConsumeAmmo> ( ActionRegistry::ConsumeAmmo );
}
