/*! \file gameeventsystem.cpp
    \brief The event handling of ASC
*/

/*
    This file is part of Advanced Strategic Command; http://www.asc-hq.de
    Copyright (C) 1994-2003  Martin Bickel  and  Marc Schellenberger

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





void         checktimedevents ( MapDisplayInterface* md )
{
   if ( actmap->eventTimes.empty() )
      return;
                              
   GameTime t = actmap->eventTimes.back();
   if ( actmap->time.abstime >= t.abstime ) {
      checkevents( md );
      actmap->eventTimes.pop_back();
   }
}



void eventReady()
{
   for ( int i = 0; i < 8; i++ )
      actmap->player[i].queuedEvents++;
}



void         checkevents( MapDisplayInterface* md )
{
   actmap->player[actmap->actplayer].queuedEvents++;
   while ( actmap->player[actmap->actplayer].queuedEvents ) {

      actmap->player[actmap->actplayer].queuedEvents = 0;

      for ( tmap::Events::iterator ev = actmap->events.begin(); ev != actmap->events.end(); ++ev )
         (*ev)->check( md );

   }
}




const char* EventTriggerName[] = { "Nothing (always true)",
                                 "Nothing (always false)",
                                 "Turn Passed",
                                 "Unit Lost",
                                 "Unit Conquered",
                                 "UnitDestroyed",
                                 "All Buildings Lost",
                                 "All Units Lost",
                                 "Building Conquered",
                                 "Building Lost",
                                 "Building Destroyed",
                                 "Building Detected",
                                 "Event Triggered",
                                 "All Enemy Units Destroyed",
                                 "All Enemy Buildings Destroyed",
                                 "Any unit reaches area",
                                 "Specific unit reaches area",
                                 "Resource Tribute" };

const char* EventActionName[] = { "Nothing",
                                 "Wind Change",
                                 "Change Game Parameter",
                                 "Display Message (from text file)",
                                 "Weather Change",
                                 "Map Change",
                                 "Add Object",
                                 "MapChange Completed",
                                 "Change Building Damage",
                                 "Next Map",
                                 "Lose Map",
                                 "Display Ellipse",
                                 "Remove Ellipse",
                                 "Change Building Owner",
                                 "Remove all objects",
                                 "Display Message (immediate text)",
                                 "Add Production Capability",
                                 "Change Diplomatic Status",
                                 "Add Resources to Building",
                                 "Reinforcements" };


Event::Event ( tmap& map_ ) : gamemap ( map_ ), action(NULL), playerBitmap(0xff), reArmNum(0)
{
   id = ++gamemap.eventID;
   triggerTime.abstime = -1;
   status = Untriggered;
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
      triggerConnection = TriggerConnection ( stream.readInt() );
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

            triggerTime.set ( gamemap.time.turn() + delayedexecution.turn, gamemap.time.move() + delayedexecution.move );
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
         if ( action )
            action->execute( md );

         executed();
         if ( reArmNum > 0 ) {
            status = Untriggered;
            arm();
            --reArmNum;
         } else
            status = Executed;
      }
   }
}

void Event::check( MapDisplayInterface* md )
{
   if ( playerBitmap & (1 << gamemap.actplayer ))
      if ( status == Untriggered ) {
         if ( triggerConnection == AND ) {
            status = Triggered;
            for ( Trigger::iterator i = trigger.begin(); i != trigger.end(); i++ ) {
               EventTrigger::State s = (*i)->state( gamemap.actplayer );
               if ( s == EventTrigger::unfulfilled || s == EventTrigger::finally_failed )
                  status = Untriggered;
            }
         } else {
            for ( Trigger::iterator i = trigger.begin(); i != trigger.end(); i++ ) {
               EventTrigger::State s = (*i)->state( gamemap.actplayer );
               if ( s == EventTrigger::fulfilled || s == EventTrigger::finally_fulfilled )
                  status = Triggered;
            }
         }
      }

   if ( status == Triggered || status == Timed )
      execute( md );
}


void Event::spawnAction( EventActionID eai )
{
   action = actionFactory::Instance().createObject( eai );
   action->setMap ( &gamemap );
}

EventTrigger* Event::spawnTrigger( EventTriggerID eti )
{
   EventTrigger* et = triggerFactory::Instance().createObject( eti );
   et->setMap ( &gamemap );
   et->setEvent ( this );
   return et;
}


void Event::arm()
{
   for ( Trigger::iterator i = trigger.begin(); i != trigger.end(); ++i )
      (*i)->arm();
}


Event::~Event()
{
  clear();
}
