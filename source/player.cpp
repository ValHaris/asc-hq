
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
#include "cannedmessages.h"


PlayerID::PlayerID( const Player& p ) : num( p.getPosition() ) {};


SigC::Signal4<void,GameMap*,int,int,DiplomaticStates> DiplomaticStateVector::anyStateChanged;
SigC::Signal3<void,GameMap*,int,int> DiplomaticStateVector::shareViewChanged;


const char* diplomaticStateNames[diplomaticStateNum+1] = 
{
   "War",
   "Truce",
   "Peace",
   "Association",
   "Alliance",
   NULL
};



DiplomaticStateVector::DiplomaticStateVector( Player& _player ) : player( _player )
{
}


void DiplomaticStateVector::turnBegins()
{
   for ( QueuedStateChanges::iterator i = queuedStateChanges.begin(); i != queuedStateChanges.end(); ++i ) {
      if ( getState(i->first) > i->second ) {
         // we are changing to a more aggressive state, which doesn't need the confirmation of the counterpart
         setState( i->first, i->second );
         player.getParentMap()->player[ i->first ].diplomacy.setState( player.getPosition(), i->second);
      } else {
         // we are browsing through the queued changes of the target player to look if he accepted our proposal
         
         DiplomaticStateVector& target = player.getParentMap()->player[ i->first ].diplomacy;
         QueuedStateChanges::iterator t = target.queuedStateChanges.find( player.getPosition() );
         if ( t != target.queuedStateChanges.end() ) {
            // this should have been handled when the other player set his proposal 
         } else {
            // he did not answer
         }
      }
   }
   queuedStateChanges.clear();
}



DiplomaticStates DiplomaticStateVector::getState( int towardsPlayer ) const
{
   if ( towardsPlayer < 0 )
      return WAR;

   if ( player.getPosition() == towardsPlayer )
      return ALLIANCE;
      
   if ( towardsPlayer < states.size() )
      return states[towardsPlayer];
   else
      return WAR;
}

void DiplomaticStateVector::setState( int towardsPlayer, DiplomaticStates s, bool fireSignal )
{
   assert( towardsPlayer >= 0 );

   if ( towardsPlayer >= states.size() ) 
      states.resize(towardsPlayer+1);
      
   states[towardsPlayer] = s;
   
   if ( fireSignal )
      anyStateChanged( player.getParentMap(), player.getPosition(), towardsPlayer,s);
}

void DiplomaticStateVector::sneakAttack( int towardsPlayer )
{
   assert( towardsPlayer >= 0 );

   setState( towardsPlayer, WAR );         
   player.getParentMap()->player[ towardsPlayer ].diplomacy.setState( player.getPosition(), WAR );
   
   int to = 0;
   for ( int j = 0; j < 8; j++ )
      if ( j != player.getPosition() )
         to |= 1 << j;

   ASCString txt;
   txt.format ( getmessage( 10001 ), player.getName().c_str(), player.getParentMap()->player[towardsPlayer].getName().c_str() );
   new Message ( txt, player.getParentMap(), to );
}


void DiplomaticStateVector::changeToState( int towardsPlayer, DiplomaticStates s, bool mail )
{
   assert( towardsPlayer >= 0 );

   int msgid;
   if ( s > getState( towardsPlayer ))
      msgid = 10003;  //  propose peace
   else
      msgid = 10002;  // declare war


   bool oldShareView = sharesView(towardsPlayer);

   setState( towardsPlayer, s );
   
   DiplomaticStateVector& targ = player.getParentMap()->player[ towardsPlayer ].diplomacy;
   targ.setState( player.getPosition(), s );

   if( sharesView(towardsPlayer) != oldShareView )
      shareViewChanged( player.getParentMap(), player.getPosition(), towardsPlayer );

   if ( mail ) {
      ASCString txt;
      txt.format( getmessage( msgid ), player.getName().c_str(), diplomaticStateNames[s]  );
      new Message ( txt, player.getParentMap(), 1 << towardsPlayer );
   }

   for ( int p = 0; p < player.getParentMap()->getPlayerCount(); ++p )
      if ( p != player.getPosition() ) {
         if ( getState( p ) == ALLIANCE ) {
            // we have an allied player which must now react
            DiplomaticStateVector& ally = player.getParentMap()->getPlayer( p ).diplomacy;
            if ( ally.getState( towardsPlayer ) != s )
               ally.changeToState( towardsPlayer, s );
      
         }
         
         if ( targ.getState( p ) == ALLIANCE ) {
            // the opponent has an ally which must now react
            if ( getState( p ) != s )
               changeToState( p, s );
      
         }
      }
}


void DiplomaticStateVector::propose( int towardsPlayer, DiplomaticStates s )
{
   assert( towardsPlayer >= 0 );

   DiplomaticStateVector& targ = player.getParentMap()->player[ towardsPlayer ].diplomacy;

   QueuedStateChanges::iterator i = targ.queuedStateChanges.find( player.getPosition() );
         
   DiplomaticStates currentState = getState( towardsPlayer ) ;

   if ( i == targ.queuedStateChanges.end() || (s < currentState && i->second > currentState)) {
      // we only send a message if this is an initial proposal OR
      // if the other player proposed a more peaceful state, but we are setting a more hostile state
      ASCString txt;
      int msgid;
      if ( s > getState( towardsPlayer )) 
         msgid = 10003;  //  propose peace
      else
         msgid = 10002;  //  declare war
            
      txt.format( getmessage( msgid ), player.getName().c_str(), diplomaticStateNames[s]  ); 
      new Message ( txt, player.getParentMap(), 1 << towardsPlayer );
      
      queuedStateChanges[towardsPlayer] = s;
   }  else {
      // we are answering a proposal by the other player
      
      if ( s > getState( towardsPlayer )) {
         // our proposal is about going to a more peaceful state 

         if ( s > i->second ) {
            // we are proposing even more peace, but we'll only set the state he proposed and keep our proposal
            changeToState( towardsPlayer, i->second );
         } else {
            // he proposes less or equal peace, but we'll set the state he proposed and delete his proposal, because it's fulfilled
            changeToState( towardsPlayer, s, true );  // s < i->second would only send mail if the proposal differs
            targ.queuedStateChanges.erase( i );
         }
      } else {
         if ( s < i->second ) {
            // we go to an even more hostile state 
            changeToState( towardsPlayer, i->second );
            targ.queuedStateChanges.erase( i );
         } else {
            // we are going to a state that is more hostile than the current one, but less hostile then the other players declaration
            changeToState( towardsPlayer, s, false );
         }

      }
   }
   
}

bool DiplomaticStateVector::getProposal( int fromPlayer, DiplomaticStates* state )
{
   assert( fromPlayer >= 0 );

   DiplomaticStateVector& targ = player.getParentMap()->player[ fromPlayer ].diplomacy;

   QueuedStateChanges::iterator i = targ.queuedStateChanges.find( player.getPosition() );
         
   if ( i != targ.queuedStateChanges.end() ) {
      if ( state )
         *state = i->second;
      return true;
   } else
      return false;
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


Player :: Player() 
        : diplomacy( *this )
{
   ai = NULL;
   parentMap = NULL;
   queuedEvents = 0;
   ASCversion = 0;
   color = 0;
}

void Player :: setParentMap( GameMap* map, int pos )
{
   parentMap = map;
   player = pos;
   parentMap->sigPlayerTurnBegins.connect( SigC::slot( *this, &Player::turnBegins ));   
   parentMap->sigPlayerUserInteractionBegins.connect( SigC::slot( *this, &Player::userInteractionBegins ));   
   parentMap->sigPlayerTurnEnds.connect( SigC::slot( *this, &Player::turnEnds ));   
}


void Player :: turnBegins( Player& p )
{
   if ( &p == this ) {
      diplomacy.turnBegins();
   }   
}

void Player :: userInteractionBegins( Player& p )
{
   if ( &p == this ) {
   }
}

void Player :: turnEnds( Player& p )
{
   if ( &p == this ) {
      sendQueuedMessages();
   }
}



DI_Color Player :: getColor()
{
   if ( color == DI_Color(0)) {
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
   } else
      return color;
}

ASCString Player :: getName( ) const
{
   if ( name.length() ) 
      return name;

   if ( stat == off )
      return "off";

   return ASCString("Player ") + ASCString::toString(getPosition() );
}

int Player::getHumanPlayerNum( const GameMap* gamemap )
{
   int h = 0;
   for ( int p = 0; p < gamemap->getPlayerCount(); ++p )
      if ( gamemap->getPlayer(p).exist() && gamemap->getPlayer(p).isHuman() )
         ++h;
   return h;
}



void Player :: sendQueuedMessages()
{
   MessagePntrContainer::iterator mi = getParentMap()->unsentmessage.begin();
   while ( mi != getParentMap()->unsentmessage.end() ) {
      sentmessage.push_back ( *mi );
      for ( int i = 0; i < 8; i++ ) {
         if ( (*mi)->to & ( 1 << i ))
            getParentMap()->player[ i ].unreadmessage.push_back ( *mi );
         if ( (*mi)->cc & ( 1 << i ))
            getParentMap()->player[ i ].unreadmessage.push_back ( *mi );
      }
   
      mi = getParentMap()->unsentmessage.erase ( mi );
   }
}


bool Player::exist() const
{
  return !(buildingList.empty() && vehicleList.empty());
}

template <typename T> void swapData( T& t1, T& t2 )
{
   T temp = t1;
   t1 = t2;
   t2 = temp;
}

void Player::swap ( Player& secondPlayer )
{
   if ( &secondPlayer == this)
      return;

   typedef VehicleList VL;
   typedef VehicleList::iterator VLI;

   VL vl;
   for ( VLI i = vehicleList.begin(); i != vehicleList.end(); ) {
      (*i)->color = secondPlayer.player*8;
      vl.push_back ( *i );
      i = vehicleList.erase( i );
   }

   for ( VLI i = secondPlayer.vehicleList.begin(); i != secondPlayer.vehicleList.end(); ) {
      (*i)->color = player*8;
      vehicleList.push_back ( *i );
      i = secondPlayer.vehicleList.erase( i );
   }

   for ( VLI i = vl.begin(); i != vl.end(); ) {
      secondPlayer.vehicleList.push_back ( *i );
      i = vl.erase( i );
   }


   typedef Player::BuildingList BL;
   typedef Player::BuildingList::iterator BLI;

   BL bl;
   for ( BLI i = buildingList.begin(); i != buildingList.end(); ++i)
      bl.push_back ( *i );

   BL bl2 = secondPlayer.buildingList;
   for ( BLI i = bl2.begin(); i != bl2.end(); ++i)
      (*i)->convert( player );

   for ( BLI i = bl.begin(); i != bl.end(); ++i)
      (*i)->convert( secondPlayer.player );

   for (int i =0; i < parentMap->xsize * parentMap->ysize ;i++ ) {
      tfield* fld = &parentMap->field[i];

      VisibilityStates temp = fld->getVisibility( player );
      fld->setVisibility( fld->getVisibility( secondPlayer.player), player);
      fld->setVisibility( temp, secondPlayer.player);


      for ( tfield::MineContainer::iterator i = fld->mines.begin(); i != fld->mines.end(); i++ )
         if ( i->player == player )
            i->player = secondPlayer.player;
         else
            if ( i->player == secondPlayer.player  )
               i->player = player;
      if ( fld->resourceview ) {
         bool b = fld->resourceview->visible & (1 << player);
         int m = fld->resourceview->materialvisible[player];
         int f = fld->resourceview->fuelvisible[player];

         if ( fld->resourceview->visible & (1 << secondPlayer.player ) )
            fld->resourceview->setview( player, fld->resourceview->materialvisible[secondPlayer.player], fld->resourceview->fuelvisible[secondPlayer.player] );
         else
            fld->resourceview->resetview( player );

         if ( b ) 
            fld->resourceview->setview( secondPlayer.player, m, f );
         else
            fld->resourceview->resetview( secondPlayer.player );
      }
   } /* endfor */

   swapData( research, secondPlayer.research );
   swapData( existanceAtBeginOfTurn, secondPlayer.existanceAtBeginOfTurn );

   delete ai;
   ai = NULL;

   delete secondPlayer.ai;
   secondPlayer.ai = NULL;

   swapData( stat, secondPlayer.stat );
   swapData( passwordcrc, secondPlayer.passwordcrc );
   swapData( dissections, secondPlayer.dissections );
   swapData( unreadmessage, secondPlayer.unreadmessage );
   swapData( oldmessage, secondPlayer.oldmessage );
   swapData( sentmessage, secondPlayer.sentmessage );
   swapData( queuedEvents, secondPlayer.queuedEvents );
   swapData( ASCversion, secondPlayer.ASCversion );
   swapData( playTime, secondPlayer.playTime );
   swapData( cursorPos, secondPlayer.cursorPos );
   swapData( email, secondPlayer.email );
   swapData( name, secondPlayer.name );
   swapData( color, secondPlayer.color );

   diplomacy.swap( secondPlayer.player );


   int a = player;
   int b = secondPlayer.player;
   for ( int i= 0; i < parentMap->getPlayerCount(); ++i )
      if ( i != a ) {
         swapData( parentMap->tribute.paid[i][a], parentMap->tribute.paid[i][b] );
         swapData( parentMap->tribute.paid[a][i], parentMap->tribute.paid[b][i] );
         swapData( parentMap->tribute.avail[i][a], parentMap->tribute.avail[i][b] );
         swapData( parentMap->tribute.avail[a][i], parentMap->tribute.avail[b][i] );
         swapData( parentMap->tribute.payStatusLastTurn[i][a], parentMap->tribute.payStatusLastTurn[i][b] );
         swapData( parentMap->tribute.payStatusLastTurn[a][i], parentMap->tribute.payStatusLastTurn[b][i] );
      }

}

void DiplomaticStateVector::swap( int secondPlayer )
{
   DiplomaticStateVector& secondDSV = player.getParentMap()->getPlayer(secondPlayer).diplomacy;

   swapData( states, secondDSV.states );
   swapData( queuedStateChanges, secondDSV.queuedStateChanges );

   for ( int i= 0; i < player.getParentMap()->getPlayerCount(); ++i )
      swapData( player.getParentMap()->getPlayer(i).diplomacy.states[secondPlayer],  player.getParentMap()->getPlayer(i).diplomacy.states[player.getPosition()] );

}


void Player::merge ( Player& secondPlayer )
{

}

