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


#include "changediplomaticstate.h"

#include "action-registry.h"
#include "../gamemap.h"
#include "../itemrepository.h"


ChangeDiplomaticState::ChangeDiplomaticState( Player& fromplayer, Player& towardsPlayer, int queue, DiplomaticStates newState )
   : GameAction( fromplayer.getParentMap() )
{
   this->fromPlayer = fromplayer.getPosition();
   this->towardsPlayer = towardsPlayer.getPosition();
   this->queued = queue;
   this->newState = newState;
   this->oldState = -1; 
}

void ChangeDiplomaticState::readData ( tnstream& stream )
{
   stream.readInt();
   fromPlayer = stream.readInt();
   towardsPlayer = stream.readInt();
   queued = stream.readInt();
   newState = (DiplomaticStates) stream.readInt();
   oldState = stream.readInt();
}


void ChangeDiplomaticState::writeData ( tnstream& stream ) const
{
   stream.writeInt( 1 );
   stream.writeInt( fromPlayer );
   stream.writeInt( towardsPlayer );
   stream.writeInt( queued );
   stream.writeInt( newState );
   stream.writeInt( oldState );
}

ActionResult ChangeDiplomaticState::runAction ( const Context& context )
{
   DiplomaticStateVector& d = getMap()->getPlayer( fromPlayer ).diplomacy;
   
   if ( queued > 0)  {
      DiplomaticStateVector::QueuedStateChanges::iterator i =  d.queuedStateChanges.find( towardsPlayer );
      if ( i != d.queuedStateChanges.end() )
         oldState = i->second;
      else
         oldState = -1;
      
      d.queuedStateChanges[towardsPlayer] = newState;
   } else 
   if ( queued == 0 ) {
      oldState = d.getState( towardsPlayer );
      d.setState( towardsPlayer, newState );
   } else 
      if ( queued == -1 ) {
         DiplomaticStateVector::QueuedStateChanges::iterator i =  d.queuedStateChanges.find( towardsPlayer );
         if ( i != d.queuedStateChanges.end() ) {
            oldState = i->second;
            d.queuedStateChanges.erase( i );
         } else
            oldState = -1;
      }
      
   return ActionResult(0);
}


ActionResult ChangeDiplomaticState::undoAction( const Context& context )
{
   DiplomaticStateVector& d = getMap()->getPlayer( fromPlayer ).diplomacy;
   
   if ( queued == 1 ) {
      DiplomaticStateVector::QueuedStateChanges::iterator i =  d.queuedStateChanges.find( towardsPlayer );
      if ( oldState == -1 )
         d.queuedStateChanges.erase( i );
      else
         i->second = (DiplomaticStates) oldState;
      
   } else 
   if ( queued == 0 ) {
      d.setState( towardsPlayer, (DiplomaticStates) oldState );
   } else
      if ( queued == -1 ) {
         if ( oldState < 0 || oldState >= diplomaticStateNum )
            return ActionResult(23101);
         d.queuedStateChanges[towardsPlayer] = (DiplomaticStates) oldState;  
      }
   
   return ActionResult(0);
}


ASCString ChangeDiplomaticState :: getCommandString() const 
{
   ASCString c;
   c.format("changediplomaticstate ( %d %d %d %d )", fromPlayer, towardsPlayer, queued, (int)newState );
   return c;
}

GameActionID ChangeDiplomaticState::getID() const 
{
   return ActionRegistry::ChangeDiplomaticState;   
}

ASCString ChangeDiplomaticState::getDescription() const
{
   ASCString s = "Change diplomatic state between " + ASCString::toString(fromPlayer) + " and " + ASCString::toString(towardsPlayer) 
         + " to " + diplomaticStateNames[newState];
   return s;
}


namespace {
   const bool r1 = registerAction<ChangeDiplomaticState> ( ActionRegistry::ChangeDiplomaticState );
}

