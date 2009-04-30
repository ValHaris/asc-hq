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


#include "diplomacycommand.h"

#include "action-registry.h"
#include "../gamemap.h"
#include "../itemrepository.h"
#include "../cannedmessages.h"
#include "../viewcalculation.h"
#include "../spfst.h"

#include "changediplomaticstate.h"

DiplomacyCommand::DiplomacyCommand( GameMap* map )
   : Command( map ), actingPlayer(-1), sneak ( false), newState(WAR), towardsPlayer(-1)
{
}


DiplomacyCommand::DiplomacyCommand( Player& player )
   : Command( player.getParentMap() ), actingPlayer( player.getPosition() ), sneak ( false), newState(WAR), towardsPlayer(-1)
{
   
}




void DiplomacyCommand::readData ( tnstream& stream )
{
   Command::readData( stream );
   stream.readInt();
   actingPlayer = stream.readInt();
   readClassContainer( generatedMessages, stream );
   sneak = stream.readInt();
   newState = (DiplomaticStates) stream.readInt();
   towardsPlayer = stream.readInt();
}


void DiplomacyCommand::writeData ( tnstream& stream ) const
{
   Command::writeData( stream );
   stream.writeInt( 1 );
   stream.writeInt( actingPlayer );
   writeClassContainer( generatedMessages, stream );
   stream.writeInt( sneak );
   stream.writeInt( newState );
   stream.writeInt( towardsPlayer );
}


void DiplomacyCommand::sneakAttack( Player& enemy )
{
   sneak = true;
   towardsPlayer = enemy.getPosition();
   newState = WAR;
   setState( SetUp ); 
}

void DiplomacyCommand::newstate( DiplomaticStates state, Player& towards )
{
   sneak = false;
   towardsPlayer = towards.getPosition();
   newState = state;
   setState( SetUp );
}


void DiplomacyCommand::sendMessage( int to, const ASCString& body )
{
   Message* m = new Message ( body, getMap(), to );
   generatedMessages.push_back( m->id );
}

void DiplomacyCommand::immediateTwoWayChange( DiplomaticStates newState, const Context& context )
{
   Player& acting = getMap()->getPlayer( actingPlayer );
   Player& towards = getMap()->getPlayer( towardsPlayer );
   
   ActionResult res = (new ChangeDiplomaticState( acting, towards, 0, newState ))->execute(context);
   if ( !res.successful() )
      throw res;
             
   res = (new ChangeDiplomaticState( towards, acting, 0, newState ))->execute(context);
   if ( !res.successful() )
      throw res;
}


ActionResult DiplomacyCommand::go ( const Context& context )
{
   if ( getState() != SetUp )
      return ActionResult(21002);
   
   if ( actingPlayer  < 0 || actingPlayer >= getMap()->getPlayerCount() )
      return ActionResult( 23100 );
   
   if ( towardsPlayer  < 0 || towardsPlayer >= getMap()->getPlayerCount() )
      return ActionResult( 23100 );
   
   Player& acting = getMap()->getPlayer( actingPlayer );
   Player& towards = getMap()->getPlayer( towardsPlayer );
   
   
   bool oldShareView = acting.diplomacy.sharesView( towards );
   
   if ( sneak ) {
      
      immediateTwoWayChange( newState, context );
      
      int to = 0;
      for ( int j = 0; j < 8; j++ )
         if ( j != actingPlayer )
            if ( getMap()->getPlayer(j).exist() )
               to |= 1 << j;

      ASCString txt;
      txt.format ( getmessage( 10001 ), acting.getName().c_str(), getMap()->player[towardsPlayer].getName().c_str() );
      sendMessage( to, txt );
   } else {
      
      
      DiplomaticStateVector& targ = towards.diplomacy;

      DiplomaticStateVector::QueuedStateChanges::iterator i = targ.queuedStateChanges.find( acting.getPosition() );
         
      DiplomaticStates currentState = acting.diplomacy.getState( towards ) ;

      bool initialProposal = i == targ.queuedStateChanges.end();
      
      if ( initialProposal || (newState < currentState && i->second > currentState)) {
         // we only send a message if this is an initial proposal OR
         // if the other player proposed a more peaceful state, but we are setting a more hostile state
         ASCString txt;
         int msgid;
         if ( newState > acting.diplomacy.getState( towardsPlayer )) 
            msgid = 10003;  //  propose peace
         else
            msgid = 10002;  //  declare war
            
         txt.format( getmessage( msgid ), acting.getName().c_str(), diplomaticStateNames[newState]  ); 
         
         sendMessage(1 << towardsPlayer, txt );
      
         ActionResult res = (new ChangeDiplomaticState( acting, towards, 1, newState ))->execute(context);
         if ( !res.successful() )
            return res;
         
         
         
      }  else {
         // we are answering a proposal by the other player
      
         if ( newState > acting.diplomacy.getState( towardsPlayer )) {
            // our proposal is about going to a more peaceful state 

            if ( newState > i->second ) {
               // we are proposing even more peace, but we'll only set the state he proposed and make a proposal ourself
               
               immediateTwoWayChange( i->second, context );
               
               ActionResult res = (new ChangeDiplomaticState( acting, towards, 1, newState ))->execute(context);
               if ( !res.successful() )
                  return res;
               
            } else {
               // he proposes less or equal peace, but we'll set the lower state 
              
               immediateTwoWayChange( newState, context );
               
            }
            
            // we'll delete the other's proposal, because we have now reacted to it
            ActionResult res = (new ChangeDiplomaticState( towards, acting, -1, newState ))->execute(context);
            if ( !res.successful() )
               return res;
            
         } else {
            if ( newState < i->second ) {
               // we go to an even more hostile state 
               immediateTwoWayChange( i->second, context );
               
               
               ActionResult res = (new ChangeDiplomaticState( towards, acting, -1, newState ))->execute(context);
               if ( !res.successful() )
                  return res;
            } else {
              // we are going to a state that is more hostile than the current one, but less hostile then the other players declaration
               immediateTwoWayChange( newState, context);
            }
         }
      }
   }
   
   if ( acting.diplomacy.sharesView( towards ) != oldShareView ) {
      computeview( getMap(), 0, false, &context );
      mapChanged( getMap() );
      repaintMap();
   }
   
   setState( Completed );
   
   return ActionResult(0);
}


ASCString DiplomacyCommand :: getCommandString() const 
{
   ASCString c;
   c.format("SetDiplomacy( %d, %d, %d, %d  )", actingPlayer, towardsPlayer, sneak, newState );
   return c;
}

GameActionID DiplomacyCommand::getID() const 
{
   return ActionRegistry::DiplomacyCommand;   
}

ASCString DiplomacyCommand::getDescription() const
{
   ASCString s; // = "Cancel research for player " + ASCString::toString(player);
   if ( sneak ) 
      s = getMap()->getPlayer( actingPlayer ).getName() + " sneak attacks " + getMap()->getPlayer( towardsPlayer ).getName() ;
   else {
      s = getMap()->getPlayer( actingPlayer ).getName() + " set diplomatic state towards " + getMap()->getPlayer( towardsPlayer ).getName() 
            + " to " + diplomaticStateNames[newState];
   }
   return s;
}


namespace {
   const bool r1 = registerAction<DiplomacyCommand> ( ActionRegistry::DiplomacyCommand );
}

