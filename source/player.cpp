/*! \file gamemap.cpp
    \brief Implementation of THE central asc class: tmap 
*/

/***************************************************************************
                          gamemap.cpp  -  description
                             -------------------
    begin                : Tue Oct 24 2000
    copyright            : (C) 2000 by Martin Bickel
    email                : bickel@asc-hq.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <algorithm>
#include <ctime>
#include <cmath>

#include "global.h"
#include "misc.h"
#include "typen.h"
#include "vehicletype.h"
#include "buildingtype.h"
#include "player.h"
#include "gamemap.h"

SigC::Signal3<void,int,int,DiplomaticStates> DiplomaticStateVector::anyStateChanged;


const char* diplomaticStateNames[diplomaticStateNum+1] = 
{
   "War",
   "Truce",
   "Peace",
   "Peace with shared radar",
   "Alliance",
   NULL
};



DiplomaticStateVector::DiplomaticStateVector( Player& _player ) : player( _player )
{
}

void DiplomaticStateVector::deployHooks()
{
   player.turnBegins.connect( SigC::slot( *this, &DiplomaticStateVector::turnBegins ));
}

void DiplomaticStateVector::turnBegins( )
{
   for ( QueuedStateChanges::iterator i = queuedStateChanges.begin(); i != queuedStateChanges.end(); ++i ) {
      if ( states[i->first] > i->second ) {
         // we are changing to a more aggressive state, which doesn't need the confirmation of the counterpart
         setState( i->first, i->second );
         player.getParentMap()->player[ i->first ].diplomacy.setState( player.getPosition(), i->second);
      } else {
         // we are browsing through the queued changes of the target player to look if a accepted our proposal
         
         DiplomaticStateVector& target = player.getParentMap()->player[ i->first ].diplomacy;
         QueuedStateChanges::iterator t = target.queuedStateChanges.find( player.getPosition() );
         if ( t != target.queuedStateChanges.end() ) {
            if ( t->second > i->second ) {
               // he proposes even more peace, we'll only set the state we proposed, but keep his proposal
               setState( i->first, i->second );
               target.setState( player.getPosition(), i->second );
            } else {
               // he proposes less peace, but we'll set the state he proposed and delete his proposal, because it's fulfilled
               setState( i->first, t->second );
               target.setState( player.getPosition(), t->second );
               target.queuedStateChanges.erase( t );
            }
         }
      }
   }
   queuedStateChanges.clear();
}


DiplomaticStates DiplomaticStateVector::getState( int towardsPlayer ) const
{
   if ( towardsPlayer < states.size() )
      return states[towardsPlayer];
   else
      return WAR;
}

void DiplomaticStateVector::setState( int towardsPlayer, DiplomaticStates s )
{
   if ( towardsPlayer >= states.size() ) 
      states.resize(towardsPlayer+1);
      
   states[towardsPlayer] = s;
   stateChanged(towardsPlayer,s);
}

void DiplomaticStateVector::sneakAttack( int towardsPlayer )
{
   setState( towardsPlayer, WAR );         
   player.getParentMap()->player[ towardsPlayer ].diplomacy.setState( player.getPosition(), WAR );
}


void DiplomaticStateVector::propose( int towardsPlayer, DiplomaticStates s )
{
   queuedStateChanges[towardsPlayer] = s;  
}

      
void DiplomaticStateVector::read ( tnstream& stream )
{
   stream.readInt();
   int size = stream.readInt();
   states.resize( size );
   for ( int i = 0; i < size; ++i )
      states[i] = DiplomaticStates( stream.readInt() );
      
   queuedStateChanges.clear();
      
   size = stream.readInt();
   for ( int i = 0; i< size; ++i ) {
      int p = stream.readInt();
      DiplomaticStates s = DiplomaticStates( stream.readInt() );
      queuedStateChanges[p] = s;
   }   
}

void DiplomaticStateVector::write ( tnstream& stream ) const
{
   stream.writeInt( 1 );
   stream.writeInt( states.size() );
   for ( int i = 0; i< states.size(); ++i )
      stream.writeInt( int( states[i]));

   stream.writeInt( queuedStateChanges.size() );
   for ( QueuedStateChanges::const_iterator i = queuedStateChanges.begin(); i != queuedStateChanges.end(); ++i ) {
      stream.writeInt( i->first );
      stream.writeInt( int(i->second) );
   }   
}



const char* Player :: playerStatusNames[6] = { "Human Player", 
                                                       "AI player",
                                                       "off",
                                                       "Supervisor",
                                                       "suspended",
                                                       NULL };


Player :: Player() : diplomacy( *this )
{
   diplomacy.deployHooks();
   network = NULL;   
   ai = NULL;
   parentMap = NULL;

   queuedEvents = 0;
   if ( player < 8 ) {
      humanname = "human ";
      humanname += strrr( player );
      computername = "computer ";
      computername += strrr( player );
   } else
      humanname = computername = "neutral";

   ASCversion = 0;
}


DI_Color Player :: getColor()
{
   switch ( player ) {
      case 0: return DI_Color( 0xe0, 0, 0 );
      case 1: return DI_Color( 0, 0x71, 0xdb );
      case 2: return DI_Color( 0xbc, 0xb3, 0 );
      case 3: return DI_Color( 0, 0xaa, 0 );
      case 4: return DI_Color( 0xbc, 0, 0 );
      case 5: return DI_Color( 0xb2, 0, 0xb2 );
      case 6: return DI_Color( 0,0, 0xaa );
      case 7: return DI_Color( 0xbc, 0x67, 0 );
      case 8: return DI_Color( 0xaa, 0xaa, 0xaa );
   };
   return DI_Color(0, 0, 0);
}

const ASCString& Player :: getName( )
{
   static ASCString off = "off";
   switch ( stat ) {
     case 0: return humanname;
     case 1: return computername;
     default: return off;
   }
}


const ASCString& tmap :: getPlayerName ( int playernum )
{
   if ( playernum >= 8 )
      playernum /= 8;

   return player[playernum].getName();
}




bool Player::exist()
{
  return !(buildingList.empty() && vehicleList.empty());
}

