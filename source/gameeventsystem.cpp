/*! \file gameeventsystem.cpp
    \brief The event handling of ASC
*/

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

#include <stdio.h>

#include <cstring>

#include "global.h"
#include "buildingtype.h"
#include "vehicletype.h"
#include "gameeventsystem.h"
#include "typen.h"
#include "gamemap.h"
#include "spfst.h"
#include "util/messaginghub.h"




void         checktimedevents ( GameMap* gamemap, MapDisplayInterface* md )
{
   if ( gamemap->eventTimes.empty() )
      return;
                              
   GameTime t = gamemap->eventTimes.back();
   if ( gamemap->time.abstime >= t.abstime ) {
      checkevents( gamemap, md );
      gamemap->eventTimes.pop_back();
   }
   
}



void eventReady( GameMap* gamemap )
{
   for ( int i = 0; i < 8; i++ )
      gamemap->player[i].queuedEvents++;
}



bool checkevents( GameMap* gamemap, MapDisplayInterface* md )
{
   // this is not for synchronizing between threads, but in the same threat inside the call stack 
   static bool isRunning = false;
   if ( isRunning )
      return false;

   VariableLocker l( isRunning );
   
   gamemap->player[gamemap->actplayer].queuedEvents++;
   while ( gamemap->player[gamemap->actplayer].queuedEvents ) {

      gamemap->player[gamemap->actplayer].queuedEvents = 0;

      for ( GameMap::Events::iterator ev = gamemap->events.begin(); ev != gamemap->events.end(); ++ev )
         (*ev)->check( md );

   }
   return true;
}





Event::Event ( GameMap& map_ ) : gamemap ( map_ ), playerBitmap(0xff), reArmNum(0), action(NULL)
{
   id = ++gamemap.eventID;
   triggerTime.abstime = -1;
   status = Untriggered;
   triggerConnection = AND;
}


EventTrigger::State EventTrigger::state( int player )
{
  if ( triggerFinal ) {
     return stateCache;
  } else {
     State s = getState( player );
     if ( invert ) {
        switch ( s ) {
            case unfulfilled:       return fulfilled;
            case fulfilled:         return unfulfilled;
            case finally_failed:    triggerFinal = true;
                                    stateCache = finally_fulfilled;
                                    return finally_fulfilled;
            case finally_fulfilled: triggerFinal = true;
                                    stateCache = finally_failed;
                                    return finally_failed;
        }
        return unfulfilled;
     } else {
        if ( s == finally_fulfilled || s == finally_failed ) {
           stateCache = s;
           triggerFinal = true;
        }
        return s;
     }
  }
}

bool EventTrigger::isFulfilled()
{
   if ( event ) {
      for ( int i = 0; i < 8; ++i )
         if ( event->playerBitmap & (1<<i)) {
            State s = state( i );
            if ( s == fulfilled || s == finally_fulfilled)
                return true;
         }
   }
   return false;
}


const int currentEventVersion = 4;

void Event::write ( tnstream& stream )
{
   stream.writeInt( currentEventVersion );
   stream.writeInt( trigger.size() );
   for ( int i = 0; i < trigger.size(); i++ ) {
      stream.writeInt( trigger[i]->getTriggerID() );
      stream.writeInt( trigger[i]->invert );
      trigger[i]->writeData( stream );
   }
   stream.writeInt ( id );
   stream.writeInt( playerBitmap );
   stream.writeString ( description );
   stream.writeInt( triggerTime.abstime );
   stream.writeInt( delayedexecution.turn );
   stream.writeInt( delayedexecution.move );
   stream.writeInt( reArmNum );
   stream.writeInt ( triggerConnection );
   stream.writeInt ( status );
   if ( action ) {
      stream.writeInt( 1 );
      stream.writeInt ( action->getActionID() );
      action->writeData( stream );
   } else
      stream.writeInt( 0 );
}

void Event::clear()
{
   if ( action ) {
      delete action;
      action = NULL;
   }
   for ( Trigger::iterator i = trigger.begin(); i != trigger.end(); ++i )
      delete *i;
   trigger.clear();
}

void Event::read ( tnstream& stream )
{
   clear();

   int version = stream.readInt();
   if ( version > currentEventVersion )
      throw tinvalidversion ( "Event", currentEventVersion, version );

   int triggerNum = stream.readInt();
   for ( int i = 0; i < triggerNum; i++ ) {
      EventTrigger* et = spawnTrigger( EventTriggerID( stream.readInt()) );
      if ( version > 2 )
         et->invert = stream.readInt();
      et->readData ( stream );
      trigger.push_back ( et );
   }

   id = stream.readInt();
   playerBitmap = stream.readInt();
   description = stream.readString();
   triggerTime.abstime = stream.readInt();
   delayedexecution.turn = stream.readInt();
   delayedexecution.move = stream.readInt();

   if ( version >= 2 )
      reArmNum = stream.readInt();
   else
      reArmNum = 0;

   if ( version >= 4 ) {
      int tc = stream.readInt();
      if ( tc == 0 )
         triggerConnection = AND;
      else
         triggerConnection = OR;

      status = Status ( stream.readInt() );
   } else {
      triggerConnection = AND;
      status = Untriggered;
   }

   if( stream.readInt() ) {
      spawnAction ( stream.readInt( ));
      action->readData( stream );
   } else
      stream.writeInt( 0 );
}



void Event::execute( MapDisplayInterface* md )
{
   if ( playerBitmap & (1 << gamemap.actplayer )) {
      if ( status == Triggered ) {

         if ( delayedexecution.move  || delayedexecution.turn ) {

            debugMessage( "Event " + description + " timer started", 1);

            triggerTime.set ( gamemap.time.turn() + delayedexecution.turn, gamemap.time.move() + delayedexecution.move - 1 );
            if ( triggerTime.move() < 0 )
               triggerTime.set ( triggerTime.turn(), 0 );

            if ( gamemap.time.abstime < triggerTime.abstime ) {
               gamemap.eventTimes.push_back ( triggerTime );
               sort ( gamemap.eventTimes.begin(), gamemap.eventTimes.end(), GameTime::comp );
            }

         } else
            triggerTime = gamemap.time;

         status = Timed;
      }
      if ( status == Timed && gamemap.time.abstime >= triggerTime.abstime ) {
         
         gamemap.actions.breakUndo();
         
         if ( action )
            action->execute( md );
         
         if ( reArmNum > 0 )
            status = Untriggered;
         else
            status = Executed;

         debugMessage( "Event " + description + " executed", 1);

         executed();

         if ( reArmNum > 0 ) {
            arm();
            --reArmNum;
            check(md);
         }
      }
   }
}

void Event::debugMessage( const ASCString& msg, int threshhold ) {
   if ( gamemap.getgameparameter(cgp_debugEvents) >= threshhold  )
      infoMessage(msg);

   displayLogMessage ( 6, " EVENT: MESSAGE: " + msg + " \n" );
}

void Event::check( MapDisplayInterface* md )
{
    if ( playerBitmap & (1 << gamemap.actplayer ))
      if ( status == Untriggered ) {
         if ( triggerConnection == AND ) {
            status = Triggered;
            for ( Trigger::iterator i = trigger.begin(); i != trigger.end(); i++ ) {
               EventTrigger::State s = (*i)->state( gamemap.actplayer );
               if ( s == EventTrigger::unfulfilled || s == EventTrigger::finally_failed ) {
                  status = Untriggered;
                  debugMessage( "Event " + description + " / Trigger " + (*i)->getDetailledName() + ": false" );
               } else
                  debugMessage( "Event " + description + " / Trigger " + (*i)->getDetailledName() + ": true" );
            }
         } else {
            for ( Trigger::iterator i = trigger.begin(); i != trigger.end(); i++ ) {
               EventTrigger::State s = (*i)->state( gamemap.actplayer );
               if ( s == EventTrigger::fulfilled || s == EventTrigger::finally_fulfilled ) {
                  status = Triggered;
                  debugMessage( "Event " + description + " / Trigger " + (*i)->getDetailledName() + ": true" );
               } else
                  debugMessage( "Event " + description + " / Trigger " + (*i)->getDetailledName() + ": false" );

            }
         }

         if ( status == Triggered )
            debugMessage( "Event " + description + " triggered", 1 );

      }



   if ( status == Triggered || status == Timed )
      execute( md );
}


void Event::spawnAction( EventActionID eai )
{
   action = eventActionFactory::Instance().createObject( eai );
   action->setMap ( &gamemap );
}

EventTrigger* Event::spawnTrigger( EventTriggerID eti )
{
   EventTrigger* et = eventTriggerFactory::Instance().createObject( eti );
   et->setMap ( &gamemap );
   et->setEvent ( this );
   return et;
}


void Event::arm()
{
   for ( Trigger::iterator i = trigger.begin(); i != trigger.end(); ++i )
      (*i)->arm();

   if ( status == Timed ) {
      gamemap.eventTimes.push_back ( triggerTime );
      sort( gamemap.eventTimes.begin(), gamemap.eventTimes.end(), GameTime::comp );
   }
}


Event::~Event()
{
  clear();
}
