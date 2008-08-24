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


#include "changeunitmovement.h"

#include "../vehicle.h"
#include "../gamemap.h"
     
ChangeUnitMovement::ChangeUnitMovement( GameMap* gamemap, int vehicleID, int movement, bool delta )
   : UnitAction( gamemap, vehicleID ), originalMovement(-1), resultingMovement(-1)
{
   this->movement = movement;
   this->delta = delta;
}
      
      
ASCString ChangeUnitMovement::getDescription() const
{
   ASCString res = "Change movement ";
   if ( getUnit(false) ) 
      res += " of unit " + getUnit(false)->getName();
   
   if ( delta )
      res += " by ";
   else 
      res += " to ";
   res += ASCString::toString(movement) + " points ";
   return  res;
}
      
      
void ChangeUnitMovement::readData ( tnstream& stream ) 
{
   UnitAction::readData( stream );
   int version = stream.readInt();
   if ( version != 1 )
      throw tinvalidversion ( "ChangeUnitMovement", 1, version );
   
   movement = stream.readInt();
   delta = stream.readInt();
   originalMovement = stream.readInt();
   resultingMovement = stream.readInt();
   
};
      
      
void ChangeUnitMovement::writeData ( tnstream& stream ) const
{
   UnitAction::writeData( stream );
   stream.writeInt( 1 );
   stream.writeInt( movement );
   stream.writeInt( delta );
   stream.writeInt( originalMovement );
   stream.writeInt( resultingMovement );
};


GameActionID ChangeUnitMovement::getID() const
{
   return ActionRegistry::ChangeUnitMovement;
}

ActionResult ChangeUnitMovement::runAction( const Context& context )
{
   Vehicle* veh = getUnit();
   
   originalMovement = veh->getMovement( false, false );
   
   if ( delta )
      veh->decreaseMovement( movement );
   else
      veh->setMovement( movement );
   
   resultingMovement = veh->getMovement( false, false );
   return ActionResult(0);
}


ActionResult ChangeUnitMovement::undoAction( const Context& context )
{
   getUnit()->setMovement ( originalMovement );
   return ActionResult(0);
}

ActionResult ChangeUnitMovement::preCheck()
{
   if ( getUnit()->getMovement( false, false ) != originalMovement )
      throw ActionResult( 21103, getUnit() );
   
   return ActionResult(0);
}

ActionResult ChangeUnitMovement::postCheck()
{
   if ( getUnit()->getMovement( false, false ) != resultingMovement )
      throw ActionResult( 21103, getUnit() );
   
   return ActionResult(0);
}


namespace {
   const bool r1 = registerAction<ChangeUnitMovement> ( ActionRegistry::ChangeUnitMovement );
}
