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


#include <cmath>

#include "changeunitmovement.h"
#include "changeunitproperty.h"

#include "../vehicle.h"
#include "../gamemap.h"
     
      
ChangeUnitMovement::ChangeUnitMovement( Vehicle* veh, int movement, bool delta, bool recursive )
   : UnitAction( veh->getMap(), veh->networkid ), originalMovement(-1), resultingMovement(-1)
{
   this->movement = movement;
   this->delta = delta;
   this->recursive = recursive;
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
      
static const int changeUnitMovementStreamVersion = 2;      
      
void ChangeUnitMovement::readData ( tnstream& stream ) 
{
   UnitAction::readData( stream );
   int version = stream.readInt();
   if ( version < 1 || version > changeUnitMovementStreamVersion )
      throw tinvalidversion ( "ChangeUnitMovement", changeUnitMovementStreamVersion, version );
   
   movement = stream.readInt();
   delta = stream.readInt();
   originalMovement = stream.readInt();
   resultingMovement = stream.readInt();
   if ( version >= 2 )
      recursive = stream.readInt();
   else
      recursive = true;
};
      
      
void ChangeUnitMovement::writeData ( tnstream& stream ) const
{
   UnitAction::writeData( stream );
   stream.writeInt( changeUnitMovementStreamVersion );
   stream.writeInt( movement );
   stream.writeInt( delta );
   stream.writeInt( originalMovement );
   stream.writeInt( resultingMovement );
   stream.writeInt( recursive );
};


GameActionID ChangeUnitMovement::getID() const
{
   return ActionRegistry::ChangeUnitMovement;
}

void ChangeUnitMovement::decreaseMovement( Vehicle* veh, float fraction, const Context& context )
{
   if ( recursive )
      if ( veh->typ->movement[ log2 ( veh->height ) ] ) {
         float cargoFraction = fraction;
         if ( veh->cargoNestingDepth() == 0 && veh->typ->cargoMovementDivisor != 0)
            cargoFraction /= veh->typ->cargoMovementDivisor;
            
         for ( Vehicle::Cargo::const_iterator i = veh->getCargo().begin(); i != veh->getCargo().end(); ++i )
            if ( *i ) 
               decreaseMovement( *i, cargoFraction, context );
      }
      
   float f = float(veh->maxMovement()) * fraction;
   float f2 = ceil(f);
   if ( fabs ( f-f2) > 0.999 )
      f2 = round(f);
   
   int i = int( f2 );
   int newMovement = veh->getMovement(false,false) - i;
   
   (new ChangeUnitProperty(veh, ChangeUnitProperty::Movement, newMovement ))->execute( context );
}


ActionResult ChangeUnitMovement::runAction( const Context& context )
{
   Vehicle* veh = getUnit();
   
   originalMovement = veh->getMovement( false, false );
   
   float fraction;
   if ( delta ) {
      fraction = float(movement) / float( veh->maxMovement());
   } else {
      fraction = float(originalMovement - movement) / float( veh->maxMovement());
   }
   
   decreaseMovement( veh, fraction, context );
   
   resultingMovement = veh->getMovement( false, false );
   return ActionResult(0);
}


ActionResult ChangeUnitMovement::undoAction( const Context& context )
{
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
