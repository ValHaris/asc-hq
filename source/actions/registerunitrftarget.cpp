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


#include "registerunitrftarget.h"

#include "../vehicle.h"
     
     
RegisterUnitRFTarget::RegisterUnitRFTarget( GameMap* gamemap, int vehicleID, int weapon, int targetUnitID )
   : UnitAction( gamemap, vehicleID )
{
   this->weapon= weapon;
   this->targetUnitID = targetUnitID;
}
      
      
ASCString RegisterUnitRFTarget::getDescription() const
{
   ASCString res = "Register Unit RF Target ";
   if ( getUnit(false) ) 
      res += " of unit " + getUnit(false)->getName();
   
   return  res;
}
      
      
void RegisterUnitRFTarget::readData ( tnstream& stream ) 
{
   UnitAction::readData( stream );
   int version = stream.readInt();
   if ( version != 1 )
      throw tinvalidversion ( "ChangeUnitMovement", 1, version );
   
   targetUnitID = stream.readInt();
   weapon = stream.readInt();
};
      
      
void RegisterUnitRFTarget::writeData ( tnstream& stream )
{
   UnitAction::readData( stream );
   stream.writeInt( 1 );
   stream.writeInt( targetUnitID );
   stream.writeInt( weapon );
};


GameActionID RegisterUnitRFTarget::getID()
{
   return ActionRegistry::RegisterUnitRFTarget;
}

ActionResult RegisterUnitRFTarget::runAction( const Context& context )
{
   Vehicle* veh = getUnit();
   veh->reactionfire.weaponShots[weapon]--;
   veh->reactionfire.nonattackableUnits.push_back ( targetUnitID );
   
   return ActionResult(0);
}


ActionResult RegisterUnitRFTarget::undoAction( const Context& context )
{
   Vehicle* veh = getUnit();
   veh->reactionfire.weaponShots[weapon]++;
   
   remove( veh->reactionfire.nonattackableUnits.begin(), veh->reactionfire.nonattackableUnits.end(), targetUnitID );
   return ActionResult(0);
}



