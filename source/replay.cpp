/***************************************************************************
                          replay.cpp  -  description
                             -------------------
    begin                : Sun Jan 21 2001
    copyright            : (C) 2001 by Martin Bickel
    email                : bickel@asc-hq.org
 ***************************************************************************/

/*! \file replay.cpp
    \brief Everything for recording and playing replays

    Replays being the moves of other players or the AI, which can be viewed 
    later (especially useful in email games)
*/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <stdarg.h>
#include "replay.h"
#include "typen.h"
#include "buildingtype.h"
#include "vehicletype.h"
#include "spfst.h"
#include "controls.h"
#include "dialog.h"
#include "stack.h"
#include "gameoptions.h"
#include "sg.h"
#include "viewcalculation.h"
#include "dashboard.h"
#include "itemrepository.h"
#include "building_controls.h"


trunreplay runreplay;

int startreplaylate = 0;



void checkforreplay ( void )
{
   if ( !actmap->replayinfo )
      return;

   int rpnum  = 0;
   int s = actmap->actplayer + 1;
   if ( s >= 8 )
       s = 0;
   while ( s != actmap->actplayer ) {
       if ( actmap->replayinfo->map[s] && actmap->replayinfo->guidata[s] )
          rpnum++;

       if ( s < 7 )
          s++;
       else
          s = 0;
   }


   if ( actmap->replayinfo  &&  rpnum  &&  actmap->player[ actmap->actplayer ].stat == Player::human )
      if (choice_dlg("run replay of last turn ?","~y~es","~n~o") == 1) {
         int s = actmap->actplayer + 1;
         if ( s >= 8 )
            s = 0;
         while ( s != actmap->actplayer ) {
            if ( s >= 8 )
               s = 0;
             if ( actmap->replayinfo->map[s] && actmap->replayinfo->guidata[s] ) {

                npush ( lockdisplaymap );
                lockdisplaymap = 0;

                int t;

                do {
                   t = runreplay.run ( s );
                } while ( t ); /* enddo */

                npop ( lockdisplaymap );

             }
             if ( s < 7 )
                 s++;
              else
                 s = 0;
          }


      }
}


void initReplayLogging()
{
   if ( startreplaylate ) {
      actmap->replayinfo = new tmap::ReplayInfo;
      startreplaylate = 0;
   }

   if ( actmap->replayinfo && actmap->player[ actmap->actplayer ].stat != Player::off ) {
      if ( actmap->replayinfo->actmemstream )
         displaymessage2( "actmemstream already open at begin of turn ",2 );

      if ( actmap->replayinfo->guidata[actmap->actplayer] ) {
         delete actmap->replayinfo->guidata[actmap->actplayer];
         actmap->replayinfo->guidata[actmap->actplayer] = NULL;
      }

      savereplay ( actmap->actplayer );

      actmap->replayinfo->guidata[actmap->actplayer] = new tmemorystreambuf;
      actmap->replayinfo->actmemstream = new tmemorystream ( actmap->replayinfo->guidata[actmap->actplayer], tnstream::writing );
   }
}

void closeReplayLogging()
{
   if ( actmap->replayinfo )
      if ( actmap->replayinfo->actmemstream ) {
         delete actmap->replayinfo->actmemstream;
         actmap->replayinfo->actmemstream = NULL;
      }
}


int ReplayMapDisplay :: checkMapPosition ( int x, int y )
{
   if ( x >= actmap->xsize )
      x = actmap->xsize - 1;
   if ( y >= actmap->ysize )
      y = actmap->ysize - 1;

   int a = actmap->xpos;
   int b = actmap->ypos;
   int xss = idisplaymap.getscreenxsize();
   int yss = idisplaymap.getscreenysize();

   if ((x < a) || (x >= a + xss )) {
      if (x >= xss / 2)
         actmap->xpos = (x - xss / 2);
      else
         actmap->xpos = 0;

   }

   if (y < b   ||   y >= b + yss  ) {
      if (y >= yss / 2)
         actmap->ypos = (y - yss / 2);
      else
         actmap->ypos = 0;
      if ( actmap->ypos & 1 )
         actmap->ypos--;

   }

   if (actmap->xpos + xss > actmap->xsize)
      actmap->xpos = actmap->xsize - xss ;
   if (actmap->ypos + yss  > actmap->ysize)
      actmap->ypos = actmap->ysize - yss ;

   if ((actmap->xpos != a) || (actmap->ypos != b))
      return 1;
   else
      return 0;
}


int ReplayMapDisplay :: displayMovingUnit ( const MapCoordinate3D& start, const MapCoordinate3D& dest, pvehicle vehicle, int fieldnum, int totalmove, SoundLoopManager* slc )
{
   if ( actmap->playerView < 0 )
      return 0;

   if ( fieldvisiblenow ( getfield ( start.x, start.y ), actmap->playerView) || fieldvisiblenow ( getfield ( dest.x, dest.y ), actmap->playerView)) {
      if ( checkMapPosition  ( start.x, start.y ))
         displayMap();

      int fc = mapDisplay->displayMovingUnit ( start, dest, vehicle, fieldnum, totalmove, slc );
      if ( fc == 1 ) {
         mapDisplay->resetMovement();
         mapDisplay->displayMap();
      }

      return fc;
   } else
      return 0;
}

void ReplayMapDisplay :: displayPosition ( int x, int y )
{
   if ( fieldvisiblenow ( getfield ( x, y ), actmap->playerView )) {
      checkMapPosition  ( x, y );
      mapDisplay->displayPosition ( x, y );
   }
}

void ReplayMapDisplay :: removeActionCursor ( void )
{
   cursor.hide();
}

void ReplayMapDisplay :: displayActionCursor ( int x1, int y1, int x2, int y2, int secondWait )
{
   if ( x1 >= 0 && y1 >= 0 ) {
      int i = fieldvisiblenow ( getfield ( x1, y1 ), actmap->playerView );
      if( i ) {
         cursor.gotoxy ( x1, y1, i );
         if ( x2 >= 0 && y2 >= 0 )
            wait( 30 );
         else
            wait();
      }
   }

   if ( x2 >= 0 && y2 >= 0 ) {
      int i = fieldvisiblenow ( getfield ( x2, y2 ), actmap->playerView );
      if( i ) {
         cursor.gotoxy ( x2, y2, i );
         if ( secondWait )
            wait();
      }
   }
}

void ReplayMapDisplay :: wait ( int minTime )
{
   int t = ticker;
   while ( ticker < t + max ( cursorDelay, minTime ) )
      releasetimeslice();
}




LockReplayRecording::LockReplayRecording( tmap::ReplayInfo& _ri )
                    : ri ( _ri )
{
   ri.stopRecordingActions++;
}

LockReplayRecording::~LockReplayRecording()
{
   ri.stopRecordingActions--;
}


void logtoreplayinfo ( trpl_actions _action, ... )
{
   char action = _action;
   if ( actmap->replayinfo && actmap->replayinfo->actmemstream && !actmap->replayinfo->stopRecordingActions) {
      pnstream stream = actmap->replayinfo->actmemstream;

      va_list paramlist;
      va_start ( paramlist, _action );

      if ( action == rpl_attack || action == rpl_reactionfire ) {
         int x1 =  va_arg ( paramlist, int );
         int y1 =  va_arg ( paramlist, int );
         int x2 =  va_arg ( paramlist, int );
         int y2 =  va_arg ( paramlist, int );
         int ad1 =  va_arg ( paramlist, int );
         int ad2 =  va_arg ( paramlist, int );
         int dd1 =  va_arg ( paramlist, int );
         int dd2 =  va_arg ( paramlist, int );
         int wpnum =  va_arg ( paramlist, int );
         int size = 9;
         stream->writeChar ( action );
         stream->writeInt ( size );
         stream->writeInt ( x1 );
         stream->writeInt ( y1 );
         stream->writeInt ( x2 );
         stream->writeInt ( y2 );
         stream->writeInt ( ad1 );
         stream->writeInt ( ad2 );
         stream->writeInt ( dd1 );
         stream->writeInt ( dd2 );
         stream->writeInt ( wpnum );
      }

      if ( action == rpl_move ) {
         int x1 =  va_arg ( paramlist, int );
         int y1 =  va_arg ( paramlist, int );
         int x2 =  va_arg ( paramlist, int );
         int y2 =  va_arg ( paramlist, int );
         int nwid = va_arg ( paramlist, int );
         stream->writeChar ( action );
         int size = 5;
         stream->writeInt ( size );
         stream->writeInt ( x1 );
         stream->writeInt ( y1 );
         stream->writeInt ( x2 );
         stream->writeInt ( y2 );
         stream->writeInt ( nwid );
      }
      if ( action == rpl_move2 || action == rpl_move3 || action == rpl_move4 ) {
         int x1 =  va_arg ( paramlist, int );
         int y1 =  va_arg ( paramlist, int );
         int x2 =  va_arg ( paramlist, int );
         int y2 =  va_arg ( paramlist, int );
         int nwid = va_arg ( paramlist, int );
         int height = va_arg ( paramlist, int );

         stream->writeChar ( action );
         if ( action == rpl_move2 ) {
            int size = 6;
            stream->writeInt ( size );
         } else {
            int size = 7;
            stream->writeInt ( size );
         }
         stream->writeInt ( x1 );
         stream->writeInt ( y1 );
         stream->writeInt ( x2 );
         stream->writeInt ( y2 );
         stream->writeInt ( nwid );
         stream->writeInt ( height );
         if ( action == rpl_move3 || action == rpl_move4 ) {
            int nointerrupt = va_arg ( paramlist, int );
            stream->writeInt ( nointerrupt );
         }
      }

      if ( action == rpl_changeheight || action == rpl_changeheight2 ) {
         int x1 =  va_arg ( paramlist, int );
         int y1 =  va_arg ( paramlist, int );
         int x2 =  va_arg ( paramlist, int );
         int y2 =  va_arg ( paramlist, int );
         int nwid = va_arg ( paramlist, int );
         int oldheight = va_arg ( paramlist, int );
         int newheight = va_arg ( paramlist, int );
         stream->writeChar ( action );
         if ( action == rpl_changeheight ) {
            int size = 7;
            stream->writeInt ( size );
         } else {
            int size = 8;
            stream->writeInt ( size );
         }
         stream->writeInt ( x1 );
         stream->writeInt ( y1 );
         stream->writeInt ( x2 );
         stream->writeInt ( y2 );
         stream->writeInt ( nwid );
         stream->writeInt ( oldheight );
         stream->writeInt ( newheight );
         if ( action == rpl_changeheight2 ) {
            int noInterrupt = va_arg ( paramlist, int );
            stream->writeInt ( noInterrupt );
         }
      }
      if ( action == rpl_convert ) {
         int x =  va_arg ( paramlist, int );
         int y =  va_arg ( paramlist, int );
         int col =  va_arg ( paramlist, int );
         stream->writeChar ( action );
         int size = 3;
         stream->writeInt ( size );
         stream->writeInt ( x );
         stream->writeInt ( y );
         stream->writeInt ( col );
      }
      if ( action == rpl_buildobj || action == rpl_remobj ) {
         int x =  va_arg ( paramlist, int );
         int y =  va_arg ( paramlist, int );
         int id =  va_arg ( paramlist, int );
         stream->writeChar ( action );
         int size = 3;
         stream->writeInt ( size );
         stream->writeInt ( x );
         stream->writeInt ( y );
         stream->writeInt ( id );
      }
      if ( action == rpl_buildobj2 || action == rpl_remobj2 ) {
         int x =  va_arg ( paramlist, int );
         int y =  va_arg ( paramlist, int );
         int id =  va_arg ( paramlist, int );
         int unit = va_arg ( paramlist, int );
         stream->writeChar ( action );
         int size = 4;
         stream->writeInt ( size );
         stream->writeInt ( x );
         stream->writeInt ( y );
         stream->writeInt ( id );
         stream->writeInt ( unit );
      }
      if ( action == rpl_buildtnk || action == rpl_buildtnk3 || action == rpl_buildtnk4 ) {
         int x =  va_arg ( paramlist, int );
         int y =  va_arg ( paramlist, int );
         int id =  va_arg ( paramlist, int );
         int col =  va_arg ( paramlist, int );
         stream->writeChar ( action );
         int size;
         if ( action == rpl_buildtnk )
            size = 4;
         else
            if ( action == rpl_buildtnk4 )
               size = 7;
            else
               size = 6;

         stream->writeInt ( size );
         stream->writeInt ( x );
         stream->writeInt ( y );
         stream->writeInt ( id );
         stream->writeInt ( col );
         if ( action == rpl_buildtnk3 || action == rpl_buildtnk4 ) {
            stream->writeInt ( va_arg ( paramlist, int ) ); // constructor x
            stream->writeInt ( va_arg ( paramlist, int ) ); // constructor x
            if ( action == rpl_buildtnk4  )
               stream->writeInt ( va_arg ( paramlist, int ) ); // unit height
         }
      }
      if ( action == rpl_buildtnk2 ) {
         stream->writeChar ( action );
         int size = 5;
         stream->writeInt ( size );
         stream->writeInt ( va_arg ( paramlist, int ) );
         stream->writeInt ( va_arg ( paramlist, int ) );
         stream->writeInt ( va_arg ( paramlist, int ) );
         stream->writeInt ( va_arg ( paramlist, int ) );
         stream->writeInt ( va_arg ( paramlist, int ) );
      }
      if ( action == rpl_putbuilding ) {
         int x =  va_arg ( paramlist, int );
         int y =  va_arg ( paramlist, int );
         int id = va_arg ( paramlist, int );
         int col =  va_arg ( paramlist, int );
         stream->writeChar ( action );
         int size = 4;
         stream->writeInt ( size );
         stream->writeInt ( x );
         stream->writeInt ( y );
         stream->writeInt ( id );
         stream->writeInt ( col );
      }
      if ( action == rpl_putmine ) {
         int x =  va_arg ( paramlist, int );
         int y =  va_arg ( paramlist, int );
         int col =  va_arg ( paramlist, int );
         int typ = va_arg ( paramlist, int );
         int strength = va_arg ( paramlist, int );
         stream->writeChar ( action );
         int size = 5;
         stream->writeInt ( size );
         stream->writeInt ( x );
         stream->writeInt ( y );
         stream->writeInt ( col );
         stream->writeInt ( typ );
         stream->writeInt ( strength );
      }
      if ( action == rpl_removemine ) {
         int x =  va_arg ( paramlist, int );
         int y =  va_arg ( paramlist, int );
         stream->writeChar ( action );
         int size = 2;
         stream->writeInt ( size );
         stream->writeInt ( x );
         stream->writeInt ( y );
      }
      if ( action == rpl_removebuilding ) {
         int x =  va_arg ( paramlist, int );
         int y =  va_arg ( paramlist, int );
         stream->writeChar ( action );
         int size = 2;
         stream->writeInt ( size );
         stream->writeInt ( x );
         stream->writeInt ( y );
      }

      if ( action == rpl_produceunit ) {
         int id = va_arg ( paramlist, int );
         int col = va_arg ( paramlist, int );
         int x =  va_arg ( paramlist, int );
         int y =  va_arg ( paramlist, int );
         int cl = va_arg ( paramlist, int );
         int nwid = va_arg ( paramlist, int );
         stream->writeChar ( action );
         int size = 6;
         stream->writeInt ( size );
         stream->writeInt ( id );
         stream->writeInt ( col );
         stream->writeInt ( x );
         stream->writeInt ( y );
         stream->writeInt ( cl );
         stream->writeInt ( nwid );
      }
      if ( action == rpl_removeunit ) {
         int x =  va_arg ( paramlist, int );
         int y =  va_arg ( paramlist, int );
         int nwid = va_arg ( paramlist, int );
         stream->writeChar ( action );
         int size = 3;
         stream->writeInt ( size );
         stream->writeInt ( x );
         stream->writeInt ( y );
         stream->writeInt ( nwid );
      }
      if ( action == rpl_trainunit ) {
         int x =  va_arg ( paramlist, int );
         int y =  va_arg ( paramlist, int );
         int exp =  va_arg ( paramlist, int );
         int nwid = va_arg ( paramlist, int );
         stream->writeChar ( action );
         int size = 4;
         stream->writeInt ( size );
         stream->writeInt ( x );
         stream->writeInt ( y );
         stream->writeInt ( exp );
         stream->writeInt ( nwid );
      }
      if ( action == rpl_shareviewchange ) {
         stream->writeChar ( action );
         if ( actmap->shareview ) {
            int size = 8*8/4+1;
            stream->writeInt ( size );
            for ( int a = 0; a < 8; a++ )
               for ( int b = 0; b < 8; b++ )
                   stream->writeChar ( actmap->shareview->mode[a][b] );
            stream->writeInt ( actmap->shareview->recalculateview );
         } else {
            int size = 0;
            stream->writeInt ( size );
         }
      }
      if ( action == rpl_alliancechange ) {
         stream->writeChar ( action );
         int size = sizeof ( actmap->alliances ) / sizeof ( int );
         stream->writeInt ( size );
         for ( int a = 0; a < 8; a++ )
            for ( int b = 0; b < 8; b++ )
               stream->writeChar ( actmap->alliances[a][b] );
      }
      if ( action == rpl_refuel || action == rpl_refuel2 ) {
         int x =  va_arg ( paramlist, int );
         int y =  va_arg ( paramlist, int );
         int nwid = va_arg ( paramlist, int );
         int pos = va_arg ( paramlist, int );
         int amnt = va_arg ( paramlist, int );
         stream->writeChar ( action );
         stream->writeInt ( action==rpl_refuel ? 5 : 6 );
         stream->writeInt ( x );
         stream->writeInt ( y );
         stream->writeInt ( nwid );
         stream->writeInt ( pos );
         stream->writeInt ( amnt );
         if ( action == rpl_refuel2 ) {
             int old = va_arg ( paramlist, int );
             stream->writeInt( old );
         }
      }
      if ( action == rpl_refuel3 ) {
         int id =  va_arg ( paramlist, int );
         int resource = va_arg ( paramlist, int );
         int delta = va_arg ( paramlist, int );
         stream->writeChar ( action );
         stream->writeInt ( 3 );
         stream->writeInt ( id );
         stream->writeInt ( resource );
         stream->writeInt ( delta );
      }
      if ( action == rpl_bldrefuel ) {
         int x =  va_arg ( paramlist, int );
         int y =  va_arg ( paramlist, int );
         int pos = va_arg ( paramlist, int );
         int amnt = va_arg ( paramlist, int );
         stream->writeChar ( action );
         int size = 4;
         stream->writeInt ( size );
         stream->writeInt ( x );
         stream->writeInt ( y );
         stream->writeInt ( pos );
         stream->writeInt ( amnt );
      }
      if ( action == rpl_moveUnitUpDown ) {
         int x =  va_arg ( paramlist, int );
         int y =  va_arg ( paramlist, int );
         int nwid_from = va_arg ( paramlist, int );
         int nwid_to = va_arg ( paramlist, int );
         int nwid_moving = va_arg ( paramlist, int );
         stream->writeChar ( action );
         int size = 5;
         stream->writeInt ( size );
         stream->writeInt ( x );
         stream->writeInt ( y );
         stream->writeInt ( nwid_from );
         stream->writeInt ( nwid_to );
         stream->writeInt ( nwid_moving );
      }
      if ( action == rpl_productionResourceUsage ) {
         int en = va_arg ( paramlist, int );
         int ma = va_arg ( paramlist, int );
         int fu = va_arg ( paramlist, int );
         int x = va_arg ( paramlist, int );
         int y = va_arg ( paramlist, int );
         stream->writeChar ( action );
         int size = 5;
         stream->writeInt ( size );
         stream->writeInt ( en );
         stream->writeInt ( ma );
         stream->writeInt ( fu );
         stream->writeInt ( x );
         stream->writeInt ( y );
      }
      if ( action == rpl_repairUnit ) {
         int nwid = va_arg ( paramlist, int );
         int destnwid = va_arg ( paramlist, int );
         int amount = va_arg ( paramlist, int );
         int matremain = va_arg ( paramlist, int );
         int fuelremain = va_arg ( paramlist, int );

         stream->writeChar ( action );
         int size = 5;
         stream->writeInt ( size );
         stream->writeInt ( nwid );
         stream->writeInt ( destnwid );
         stream->writeInt ( amount );
         stream->writeInt ( matremain );
         stream->writeInt ( fuelremain );
      }
      if ( action == rpl_repairUnit2 ) {
         int x = va_arg ( paramlist, int );
         int y = va_arg ( paramlist, int );
         int destnwid = va_arg ( paramlist, int );
         int amount = va_arg ( paramlist, int );

         stream->writeChar ( action );
         int size = 4;
         stream->writeInt ( size );
         stream->writeInt ( x );
         stream->writeInt ( y );
         stream->writeInt ( destnwid );
         stream->writeInt ( amount );
      }
      if ( action == rpl_produceAmmo ) {
         int x = va_arg ( paramlist, int );
         int y = va_arg ( paramlist, int );
         int type = va_arg ( paramlist, int );
         int amount = va_arg ( paramlist, int );
         stream->writeChar ( action );
         int size = 4;
         stream->writeInt ( size );
         stream->writeInt ( x );
         stream->writeInt ( y );
         stream->writeInt ( type );
         stream->writeInt ( amount );
      }


      va_end ( paramlist );
   }
}


trunreplay :: trunreplay ( void ) : gui ( replayGuiHost ) 
{
   status = -1;
   movenum = 0;
}

int    trunreplay :: removeunit ( pvehicle eht, int nwid )
{
   if ( !eht )
      return 0;

    for ( int i = 0; i < 32; i++ )
       if ( eht->loading[i] )
          if ( eht->loading[i]->networkid == nwid ) {
             delete eht->loading[i];
             eht->loading[i] = NULL;
             return 1;
          } else {
             int ld = removeunit ( eht->loading[i], nwid );
             if ( ld )
                return ld;
          }
   return 0;
}

int  trunreplay :: removeunit ( int x, int y, int nwid )
{
   pfield fld  = getfield ( x, y );
   if ( !fld->vehicle )
      if ( fld->building ) {
         for ( int i = 0; i < 32; i++ ) {
            if ( fld->building->loading[i] ) {
               if ( fld->building->loading[i]->networkid == nwid ) {
                  delete fld->building->loading[i];
                  fld->building->loading[i] = NULL;
                  return 1;
               } else {
                  int ld = removeunit ( fld->building->loading[i], nwid );
                  if ( ld )
                     return ld;
               }
            }
         }
         return 0;
      } else
         return 0;
   else
      if ( fld->vehicle->networkid == nwid ) {
         delete fld->vehicle ;
         fld->vehicle = NULL;
         return 1;
      } else
         return removeunit ( fld->vehicle, nwid );
}


void trunreplay :: wait ( int t )
{
//   if ( fieldvisiblenow ( getactfield(), actmap->playerView ))
    while ( ticker < t + CGameOptions::Instance()->replayspeed  && !keypress()) {
       /*
       tkey input;
       while (keypress ()) {
           input = r_key ( );
           if ( input == ct_'+' )
       }
       */
       releasetimeslice();
    }
}

void trunreplay :: wait ( MapCoordinate pos, int t )
{
   if ( fieldvisiblenow ( actmap->getField ( pos ), actmap->playerView ))
      wait();
}

void trunreplay :: wait ( MapCoordinate pos1, MapCoordinate pos2, int t )
{
   if ( fieldvisiblenow ( actmap->getField ( pos1 ), actmap->playerView ) || fieldvisiblenow ( actmap->getField ( pos2 ), actmap->playerView ))
      wait();
}


/*
void trunreplay :: setcursorpos ( int x, int y )
{
   int i = fieldvisiblenow ( getfield ( x, y ), actmap->playerview );
   cursor.gotoxy ( x, y, i );
   if( i ) {
      lastvisiblecursorpos.x = x;
      lastvisiblecursorpos.y = y;
   }
}
*/


void trunreplay :: displayActionCursor ( int x1, int y1, int x2, int y2, int secondWait )
{
    ReplayMapDisplay rmd( &defaultMapDisplay );
    rmd.setCursorDelay ( CGameOptions::Instance()->replayspeed );
    rmd.displayActionCursor ( x1, y1, x2, y2, secondWait );
}

void trunreplay :: removeActionCursor ( void )
{
    ReplayMapDisplay rmd( &defaultMapDisplay );
    rmd.removeActionCursor (  );
}


void trunreplay :: execnextreplaymove ( void )
{
   if ( verbosity >= 8 )
     printf("executing replay move %d\n", movenum );

   displaymessage2("executing replay move %d\n", movenum );
   movenum++;
   int actaction = nextaction;
   if ( nextaction != rpl_finished ) {
      switch ( nextaction ) {
      case rpl_move: {
                        stream->readInt(); // size
                        int x1 = stream->readInt();
                        int y1 = stream->readInt();
                        int x2 = stream->readInt();
                        int y2 = stream->readInt();
                        int nwid = stream->readInt();
                        readnextaction();

                        pvehicle eht = actmap->getUnit ( x1, y1, nwid );
                        if ( eht ) {
                           ReplayMapDisplay rmd ( &defaultMapDisplay );
                           VehicleMovement vm ( &rmd, NULL );
                           vm.execute ( eht, -1, -1, 0 , -1, -1 );

                           int t = ticker;
                           vm.execute ( NULL, x2, y2, 2, -1, -1 );
                           wait( MapCoordinate(x1,y1), MapCoordinate(x2,y2), t );
                           vm.execute ( NULL, x2, y2, 3, -1, -1 );

                           if ( vm.getStatus() != 1000 )
                              eht = NULL;
                        }

                        if ( !eht )
                           displaymessage("severe replay inconsistency:\nno vehicle for move1 command !", 1);
                     }
         break;
      case rpl_move4:
      case rpl_move3:
      case rpl_move2: {
                        stream->readInt(); // size
                        int x1 = stream->readInt();
                        int y1  = stream->readInt();
                        int x2 = stream->readInt();
                        int y2 = stream->readInt();
                        int nwid = stream->readInt();
                        int height = stream->readInt();
                        int noInterrupt;
                        if ( nextaction == rpl_move3 || nextaction == rpl_move4 )
                           noInterrupt = stream->readInt();
                        else
                           noInterrupt = -1;

                        readnextaction();

                        pvehicle eht = actmap->getUnit ( x1, y1, nwid );
                        if ( eht ) {
                           ReplayMapDisplay rmd( &defaultMapDisplay );
                           BaseVehicleMovement vm ( vat_move, NULL, &rmd );
                           int t = ticker;
                           vm.execute ( eht, x2, y2, 0, height, -1 );
                           wait( MapCoordinate(x1,y1), MapCoordinate(x2,y2), t );
                           vm.execute ( NULL, x2, y2, 3, height, noInterrupt );

                           if ( vm.getStatus() != 1000 )
                              eht = NULL;
                        }

                        if ( !eht )
                           displaymessage("severe replay inconsistency:\nno vehicle for move2 command !", 1);
                     }
         break;
      case rpl_attack: {
                          stream->readInt();  // size
                          int x1 = stream->readInt();
                          int y1 = stream->readInt();
                          int x2 = stream->readInt();
                          int y2 = stream->readInt();
                          int ad1 = stream->readInt();
                          int ad2 = stream->readInt();
                          int dd1 = stream->readInt();
                          int dd2 = stream->readInt();
                          int wpnum = stream->readInt();
                          readnextaction();

                          pfield fld = getfield ( x1, y1 );
                          pfield targ = getfield ( x2, y2 );
                          int attackvisible = fieldvisiblenow ( fld, actmap->playerView ) || fieldvisiblenow ( targ, actmap->playerView );
                          if ( fld && targ && fld->vehicle ) {
                             if ( targ->vehicle ) {
                                tunitattacksunit battle ( fld->vehicle, targ->vehicle, 1, wpnum );
                                battle.av.damage = ad1;
                                battle.dv.damage = dd1;
                                if ( attackvisible ) {
                                   displayActionCursor ( x1, y1, x2, y2, 0 );
                                   battle.calcdisplay ( );
                                   removeActionCursor();
                                } else {
                                   battle.calc ();
                                }
                                if ( battle.av.damage < ad2 || battle.dv.damage > dd2 )
                                   displaymessage("severe replay inconsistency:\nresult of attack differ !\nexpected target damage: %d ; recorded target damage: %d\nexpected attacker damage: %d ; recorded attacker damage: %d\n", 1, battle.av.damage,ad2 ,battle.dv.damage, dd2);
                                battle.setresult ();
                                dashboard.x = 0xffff;
                             } else
                             if ( targ->building ) {
                                tunitattacksbuilding battle ( fld->vehicle, x2, y2 , wpnum );
                                battle.av.damage = ad1;
                                battle.dv.damage = dd1;
                                if ( attackvisible ) {
                                   displayActionCursor ( x1, y1, x2, y2, 0 );
                                   battle.calcdisplay (  );
                                   removeActionCursor();
                                } else {
                                   battle.calc ();
                                   /*battle.av.damage = ad2;
                                   battle.dv.damage = dd2; */
                                }
                                if ( battle.av.damage != ad2 || battle.dv.damage != dd2 )
                                   displaymessage("severe replay inconsistency:\nresult of attack differ !\nexpected target damage: %d ; recorded target damage: %d\nexpected attacker damage: %d ; recorded attacker damage: %d\n", 1, battle.av.damage,ad2 ,battle.dv.damage, dd2);
                                battle.setresult ();
                                dashboard.x = 0xffff;
                             } else
                             if ( !targ->objects.empty() ) {
                                tunitattacksobject battle ( fld->vehicle, x2, y2, wpnum );
                                if ( attackvisible ) {
                                   displayActionCursor ( x1, y1, x2, y2, 0 );
                                   battle.calcdisplay (  );
                                   removeActionCursor();
                                } else {
                                   battle.calc ();
                                   //battle.av.damage = ad2;
                                   //battle.dv.damage = dd2;
                                }
                                if ( battle.av.damage != ad2 || battle.dv.damage != dd2 )
                                   displaymessage("severe replay inconsistency:\nresult of attack differ !\nexpected target damage: %d ; recorded target damage: %d\nexpected attacker damage: %d ; recorded attacker damage: %d\n", 1, battle.av.damage,ad2 ,battle.dv.damage, dd2);
                                battle.setresult ();
                                dashboard.x = 0xffff;
                             }
                             computeview( actmap );
                             displaymap();
                          } else
                             displaymessage("severe replay inconsistency:\nno vehicle for attack command !", 1);

                      }
         break;
      case rpl_changeheight2:
      case rpl_changeheight: {
                        stream->readInt();  // size
                        int x1 = stream->readInt();
                        int y1 = stream->readInt();
                        int x2 = stream->readInt();
                        int y2 = stream->readInt();
                        int nwid = stream->readInt();
                        int oldheight = stream->readInt();
                        int newheight = stream->readInt();
                        int noInterrupt = -1;

                        if ( nextaction == rpl_changeheight2 )
                           noInterrupt = stream->readInt();


                        readnextaction();

                        pvehicle eht = actmap->getUnit ( x1, y1, nwid );
                        if ( eht ) {
                           ReplayMapDisplay rmd( &defaultMapDisplay );
                           VehicleAction* va;
                           if ( newheight > oldheight )
                              va = new IncreaseVehicleHeight ( &rmd, NULL );
                           else
                              va = new DecreaseVehicleHeight ( &rmd, NULL );

                           va->execute ( eht, -1, -1, 0 , newheight, -1 );

                           int t = ticker;
                           va->execute ( NULL, x2, y2, 2, -1, -1 );
                           wait( MapCoordinate(x1,y1), MapCoordinate(x2,y2), t );
                           va->execute ( NULL, x2, y2, 3, -1, noInterrupt );

                           if ( va->getStatus() != 1000 )
                              eht = NULL;

                           delete va;
                        }

                        if ( !eht )
                           displaymessage("severe replay inconsistency:\nno vehicle for changeheight command !", 1);


                     }
         break;
      case rpl_convert: {
                           stream->readInt();  // size
                           int x = stream->readInt();
                           int y = stream->readInt();
                           int col = stream->readInt();
                           readnextaction();


                           pfield fld = getfield ( x, y );
                           if ( fld ) {
                              displayActionCursor ( x, y );
                              if ( fld->vehicle )
                                 fld->vehicle->convert ( col );
                              else
                                 if ( fld->building )
                                    fld->building->convert ( col );

                              computeview( actmap );
                              displaymap();
                              wait( MapCoordinate(x,y) );
                              removeActionCursor();
                           } else
                              displaymessage("severe replay inconsistency:\nno vehicle for convert command !", 1);


                       }
         break;
      case rpl_remobj:
      case rpl_buildobj: 
      case rpl_remobj2:
      case rpl_buildobj2: {
                           stream->readInt();  // size
                           int x = stream->readInt();
                           int y = stream->readInt();
                           int id = stream->readInt();
                           int unit = -1;
                           if ( actaction == rpl_remobj2 || actaction == rpl_buildobj2 )
                              unit = stream->readInt();

                           readnextaction();

                           pobjecttype obj = getobjecttype_forid ( id );

                           pfield fld = getfield ( x, y );
                           if ( obj && fld ) {
                              displayActionCursor ( x, y );

                              Resources cost;
                              
                              if ( actaction == rpl_remobj || actaction == rpl_remobj2 ) {
                                 cost = obj->removecost;
                                 fld->removeobject ( obj );
                              } else {
                                 cost = obj->buildcost;
                                 fld->addobject ( obj );
                              }
                             
                              if ( unit > 0 ) {
                                 Vehicle* veh = actmap->getUnit(unit);
                                 if ( veh ) {
                                    Resources res2 =  static_cast<ContainerBase*>(veh)->getResource( cost, 0, 1  );
                                    for ( int r = 0; r < 3; r++ )
                                       if ( res2.resource(r) < cost.resource(r)  && cost.resource(r) > 0 )
                                          displaymessage("Resource mismatch: not enough resources to construct/remove object !", 1 );
                                       

                                 } else
                                    displaymessage("replay inconsistency:\nCannot find Unit to build/remove Object !", 1 );
                              }
                             
                             
                              computeview( actmap );
                              displaymap();
                              wait(MapCoordinate(x,y));
                              removeActionCursor();
                           } else
                              displaymessage("severe replay inconsistency:\nCannot find Object to build/remove !", 1 );

                       }
         break;
      case rpl_buildtnk:
      case rpl_buildtnk2:
      case rpl_buildtnk3:
      case rpl_buildtnk4: {
                           stream->readInt();  // size
                           int x = stream->readInt();
                           int y = stream->readInt();
                           int id = stream->readInt();
                           int col = stream->readInt();
                           int nwid = -1;
                           int constx = -1;
                           int consty = -1;
                           int height = -1;
                           if ( nextaction == rpl_buildtnk2 )
                              nwid = stream->readInt();

                           if ( nextaction == rpl_buildtnk3 || nextaction == rpl_buildtnk4 ) {
                              constx = stream->readInt();
                              consty = stream->readInt();
                              if ( nextaction == rpl_buildtnk4 )
                                 height = stream->readInt();
                           }

                           readnextaction();

                           pfield fld = getfield ( x, y );

                           pvehicletype tnk = getvehicletype_forid ( id );

                           if ( fld && tnk && !fld->vehicle ) {
                              displayActionCursor ( x, y );
                              pvehicle v = new Vehicle ( tnk, actmap, col );
                              v->xpos = x;
                              v->ypos = y;
                              fld->vehicle = v;
                              if ( height >= 0 )
                                 v->height = height;

                              if ( constx >= 0 && consty >= 0 ) {
                                 pfield constructorField = getfield(constx, consty );
                                 if ( constructorField->vehicle ) {
                                    Resources r ( 0, tnk->productionCost.material, tnk->productionCost.energy );
                                    Resources rr = constructorField->getContainer()->getResource( r, 0 );
                                    if ( rr < r ) {
                                       displayActionCursor ( x, y );
                                       displaymessage("severe replay inconsistency: \nNot enough resources to produce unit %s !\nRequired: %d/%d/%d ; Available: %d/%d/%d", 1, v->typ->description.c_str(), r.energy, r.material, r.fuel, rr.energy, rr.material, rr.fuel);
                                    }
                                 } else
                                    displaymessage("severe replay inconsistency: could not find constructor !", 1);

                              }

                              computeview( actmap );
                              displaymap();
                              wait(MapCoordinate(x,y) );
                              removeActionCursor();
                           } else
                              displaymessage("severe replay inconsistency:\nCannot find Vehicle to build !", 1 );

                       }
         break;
      case rpl_putbuilding : {
                               stream->readInt();  // size
                               int x = stream->readInt();
                               int y = stream->readInt();
                               int id = stream->readInt();
                               int color = stream->readInt();
                               readnextaction();

                               pfield fld = getfield ( x, y );

                               pbuildingtype bld = getbuildingtype_forid ( id );

                               if ( bld && fld ) {
                                  displayActionCursor ( x, y );
                                  putbuilding2( MapCoordinate(x, y), color, bld );
                                  computeview( actmap );
                                  displaymap();
                                  wait(MapCoordinate(x,y));
                                  removeActionCursor();
                               } else
                                  displaymessage("severe replay inconsistency:\nCannot find building to build/remove !", 1 );
                            }
         break;
      case rpl_putmine: {
                           stream->readInt();  // size
                           int x = stream->readInt();
                           int y = stream->readInt();
                           int col = stream->readInt();
                           int typ = stream->readInt();
                           int strength = stream->readInt();
                           readnextaction();

                           pfield fld = getfield ( x, y );
                           if ( fld ) {
                              displayActionCursor ( x, y );
                              fld -> putmine ( col, typ, strength );
                              computeview( actmap );
                              if ( fieldvisiblenow ( actmap->getField(x,y), actmap->playerView )) {
                                 displaymap();
                                 wait();
                              }
                              removeActionCursor();
                           } else
                              displaymessage("severe replay inconsistency:\nno field for putmine command !", 1);

                       }
         break;
      case rpl_removemine: {
                           stream->readInt();  // size
                           int x = stream->readInt();
                           int y = stream->readInt();
                           readnextaction();

                           pfield fld = getfield ( x, y );
                           if ( fld ) {
                              displayActionCursor ( x, y );
                              fld -> removemine ( -1 );
                              computeview( actmap );
                              if ( fieldvisiblenow ( actmap->getField(x,y), actmap->playerView )) {
                                 displaymap();
                                 wait();
                              }
                              removeActionCursor ( );
                           } else
                              displaymessage("severe replay inconsistency:\nno field for remove mine command !", 1);

                       }
         break;
      case rpl_removebuilding: {
                           stream->readInt();  // size
                           int x = stream->readInt();
                           int y = stream->readInt();
                           readnextaction();

                           pfield fld = getfield ( x, y );
                           if ( fld && fld->building ) {
                              displayActionCursor ( x, y );
                              pbuilding bb = fld->building;
                              if ( bb->getCompletion() ) {
                                 bb->setCompletion( bb->getCompletion()-1);
                              } else {
                                 delete bb;
                              }
                              computeview( actmap );
                              displaymap();
                              wait();
                              removeActionCursor();
                           } else
                              displaymessage("severe replay inconsistency:\nno building for removebuilding command !", 1);

                       }
         break;
      case  rpl_produceunit : {
                                 stream->readInt();  // size
                                 int id = stream->readInt();
                                 int col = stream->readInt();
                                 int x = stream->readInt();
                                 int y = stream->readInt();
                                 int cl = stream->readInt();
                                 int nwid = stream->readInt();
                                 readnextaction();

                                 pfield fld = getfield ( x, y );

                                 pvehicletype tnk = getvehicletype_forid ( id );
                                 if ( tnk && fld) {
                                 
                                    #if 0
                                    printf("produced unit: pos %d / %d; nwid %d; typ id %d; typ %s \n", x,y,nwid,id,tnk->description.c_str() );
                                    #endif

                                    pvehicle eht = new Vehicle ( tnk, actmap, col / 8 );
                                    eht->klasse = cl;
                                    eht->xpos = x;
                                    eht->ypos = y;
                                    eht->setup_classparams_after_generation ();
                                    eht->networkid = nwid;

                                    if ( fld->building ) {
                                       Resources r = fld->building->getResource( tnk->productionCost, 0 );
                                       if ( r < tnk->productionCost ) {
                                          displayActionCursor ( x, y );
                                          displaymessage("severe replay inconsistency: \nNot enough resources to produce unit %s !\nRequired: %d/%d/%d ; Available: %d/%d/%d", 1, eht->typ->description.c_str(), tnk->productionCost.energy, tnk->productionCost.material, tnk->productionCost.fuel, r.energy, r.material, r.fuel);
                                       }
                                       int i = 0;
                                       while ( fld->building->loading[i])
                                          i++;
                                       fld->building->loading[i] = eht;
                                    } else {
                                       displayActionCursor ( x, y );
                                       fld->vehicle = eht;
                                       computeview( actmap );
                                       displaymap();
                                       wait( MapCoordinate(x,y) );
                                       removeActionCursor();
                                    }
                                 } else
                                    displaymessage("severe replay inconsistency:\nCannot find vehicle to build/remove !", 1 );

                              }
         break;
      case rpl_removeunit : {
                                 stream->readInt();  // size
                                 int x = stream->readInt();
                                 int y = stream->readInt();
                                 int nwid = stream->readInt();
                                 readnextaction();
                                 pfield fld = getfield(x,y);
                                 if ( !fld->vehicle || fld->vehicle->networkid != nwid && fld->building ) {
                                    cbuildingcontrols bc;
                                    bc.init ( fld->building );
                                    pvehicle veh = actmap->getUnit( nwid );
                                    bc.recycling.recycle( veh );
                                 } else
                                    if ( !removeunit ( x, y, nwid ))
                                       displaymessage ( "severe replay inconsistency:\nCould not remove unit %d!", 1, nwid );
                              }
         break;
      case rpl_trainunit:{
                                 stream->readInt();  // size
                                 int x = stream->readInt();
                                 int y = stream->readInt();
                                 int exp = stream->readInt();
                                 int nwid = stream->readInt();
                                 readnextaction();

                                 pvehicle eht = actmap->getUnit ( x, y, nwid );
                                 if ( eht ) {
                                    eht->experience = exp;
                                 } else
                                    displaymessage("severe replay inconsistency:\nno vehicle for trainunit command !", 1);

                              }
         break;
      case rpl_shareviewchange: {
                                 int size = stream->readInt();
                                 tmap::Shareview* sv = new tmap::Shareview;
                                 if ( size ) {
                                    for ( int a = 0; a < 8; a++ )
                                       for ( int b = 0; b < 8; b++)
                                           sv->mode[a][b] = stream->readChar();
                                    sv->recalculateview = stream->readInt();

                                    if ( actmap->shareview )
                                       delete actmap->shareview;
                                    actmap->shareview = sv;
                                    actmap->shareview->recalculateview = 0;
                                 } else
                                    if ( actmap->shareview ) {
                                       delete actmap->shareview;
                                       actmap->shareview = NULL;
                                    }

                                 readnextaction();
                                 computeview( actmap );
                                 displaymap();
                              }
         break;
      case rpl_alliancechange: {
                                 stream->readInt();  // size
                                 for ( int a = 0; a < 8; a++ )
                                    for ( int b = 0; b < 8; b++ )
                                       actmap->alliances[a][b] = stream->readChar();
                                 readnextaction();
                                 dashboard.x = 0xffff;
                              }
         break;
      case rpl_refuel :
      case rpl_refuel2 : {
                                 stream->readInt();  // size
                                 int x = stream->readInt();
                                 int y = stream->readInt();
                                 int nwid = stream->readInt();
                                 int pos = stream->readInt();
                                 int amnt = stream->readInt();
                                 int old = -2;
                                 if ( nextaction == rpl_refuel2 )
                                    old = stream->readInt();

                                 readnextaction();

                                 pvehicle eht = actmap->getUnit ( x, y, nwid );
                                 if ( eht ) {
                                    if ( pos < 16 )
                                       eht->ammo[pos] = amnt;
                                     else {
                                        switch ( pos ) {
                                        case 1000: if ( eht->tank.energy != old && old >= 0 )
                                                      displaymessage("severe replay inconsistency:\nthe resources of unit not matching. \nrecorded: %d , expected: %d !", 1, old, eht->tank.energy);
                                                   eht->tank.energy = amnt;
                                           break;
                                        case 1001: if ( eht->tank.material != old && old >= 0 )
                                                      displaymessage("severe replay inconsistency:\nthe resources of unit not matching. \nrecorded: %d , expected: %d !", 1, old, eht->tank.material);
                                                   eht->tank.material = amnt;
                                           break;
                                        case 1002: if ( eht->tank.fuel != old && old >= 0 )
                                                      displaymessage("severe replay inconsistency:\nthe resources of unit not matching. \nrecorded: %d , expected: %d !", 1, old, eht->tank.fuel);
                                                   eht->tank.fuel = amnt;
                                           break;
                                        } /* endswitch */
                                     }
                                 } else
                                    displaymessage("severe replay inconsistency:\nno vehicle for refuel-unit command !", 1);
                              }
         break;
      case rpl_refuel3 : {
                                 stream->readInt();  // size
                                 int id = stream->readInt();
                                 int type = stream->readInt();
                                 int delta = stream->readInt();
                                 readnextaction();

                                 ContainerBase* cb = actmap->getContainer ( id );
                                 if ( cb ) {
                                    int d = cb->getResource(delta, type-1000, false);
                                    if ( d != delta )
                                       displaymessage("severe replay inconsistency:\nthe resources of container not matching. \nrequired: %d , available: %d !", 1, delta, d);
                                 } else
                                    displaymessage("severe replay inconsistency:\nno vehicle for refuel3 command !", 1);
                              }
         break;
      case rpl_bldrefuel : {
                                 stream->readInt();  // size
                                 int x = stream->readInt();
                                 int y = stream->readInt();
                                 int pos = stream->readInt();
                                 int amnt = stream->readInt();
                                 readnextaction();

                                 pbuilding bld = actmap->getField(x,y)->building;
                                 if ( bld ) {
                                    if ( pos < 16 )
                                        bld->ammo[pos] = amnt;
                                     else
                                        bld->getResource ( amnt, pos-1000, 0 );
                                 } else
                                    displaymessage("severe replay inconsistency:\nno building for refuel-unit command !", 1);
                              }
         break;
      case rpl_moveUnitUpDown: {
                                 stream->readInt();  // size
                                 int x =  stream->readInt();
                                 int y =  stream->readInt();
                                 int nwid_from = stream->readInt();
                                 int nwid_to = stream->readInt();
                                 int nwid_moving = stream->readInt();

                                 readnextaction();

                                 pvehicle eht = actmap->getUnit ( x, y, nwid_moving );

                                 ContainerBase* from;
                                 if ( nwid_from >= 0 )
                                    from = actmap->getUnit ( x, y, nwid_from );
                                 else
                                    from = actmap->getField ( x, y )->building;

                                 ContainerBase* to;
                                 if ( nwid_to >= 0 )
                                    to = actmap->getUnit ( x, y, nwid_to );
                                 else
                                    to = actmap->getField ( x, y )->building;

                                 if ( eht && from && to ) {
                                    int i = 0;
                                    while ( from->loading[i] != eht )
                                       i++;

                                    if ( i >= 32 ) {
                                       displaymessage("severe replay inconsistency: container for moveUpDown 2 !", 1);
                                       return;
                                    }

                                    from->loading[i] = NULL;

                                    while ( to->loading[i]  )
                                       i++;

                                    if ( i >= 32 ) {
                                       displaymessage("severe replay inconsistency: container for moveUpDown 3 !", 1);
                                       return;
                                    }

                                    to->loading[i] = eht;

                                 } else
                                    displaymessage("severe replay inconsistency: container for moveUpDown !", 1);
                              }
                              break;
      case rpl_repairUnit : {
                                 stream->readInt();  // size
                                 int nwid = stream->readInt();
                                 int destnwid = stream->readInt();
                                 int amount = stream->readInt();
                                 int matremain  = stream->readInt();
                                 int fuelremain = stream->readInt();

                                 readnextaction();

                                 pvehicle eht = actmap->getUnit ( nwid );
                                 pvehicle dest = actmap->getUnit ( destnwid );
                                 if ( eht && dest ) {
                                    eht->repairItem ( dest, amount );
                                    if ( eht->tank.fuel != fuelremain || eht->tank.material != matremain )
                                         displaymessage("severe replay inconsistency:\nthe resources of unit not matching for repair operation!", 1);
                                 } else
                                    displaymessage("severe replay inconsistency:\nno vehicle for repair-unit command !", 1);
                              }
         break;
      case rpl_repairUnit2 : {
                                 stream->readInt();  // size
                                 int x = stream->readInt();
                                 int y = stream->readInt();
                                 int destnwid = stream->readInt();
                                 int amount = stream->readInt();

                                 readnextaction();

                                 pbuilding bld = getfield(x,y)->building;
                                 pvehicle dest = actmap->getUnit ( destnwid );
                                 if ( bld && dest ) {
                                    bld->repairItem ( dest, amount );
                                 } else
                                    displaymessage("severe replay inconsistency:\nno vehicle for repair-unit command !", 1);
                              }
         break;
      case rpl_produceAmmo : {
                                 stream->readInt();  // size
                                 int x = stream->readInt();
                                 int y = stream->readInt();
                                 int type = stream->readInt();
                                 int amount = stream->readInt();
                                 readnextaction();
                                 pbuilding bld = getfield(x,y)->building;
                                 if ( bld ) {
                                    cbuildingcontrols bc;
                                    bc.init ( bld );
                                    bc.produceammunition.produce( type, amount );
                                 } else
                                    displaymessage("severe replay inconsistency:\nno building for produce ammo command !", 1);

                              }
         break;



      default:{
                 int size = stream->readInt();
                 for ( int i = 0; i< size; i++ )
                    stream->readInt();

                 readnextaction();

              }
        break;
      } /* endswitch */

   } else {
      status = 10;
      dashboard.x = -1;
   }

}

void trunreplay :: readnextaction ( void )
{
      if ( stream->dataavail () )
         nextaction = stream->readChar();
      else
         nextaction = rpl_finished;
}


preactionfire_replayinfo trunreplay::getnextreplayinfo ( void )
{
   if ( nextaction == rpl_reactionfire ) {
      preactionfire_replayinfo reac = new treactionfire_replayinfo;
      stream->readInt(); // size

      reac->x1 = stream->readInt();
      reac->y1 = stream->readInt( );
      reac->x2 = stream->readInt( );
      reac->y2 = stream->readInt( );
      reac->ad1 = stream->readInt( );
      reac->ad2 = stream->readInt( );
      reac->dd1 = stream->readInt( );
      reac->dd2 = stream->readInt( );
      reac->wpnum = stream->readInt( );

      readnextaction();

      return reac;
   } else
      return NULL;

}


int  trunreplay :: run ( int player )
{
   if ( status < 0 )
      firstinit ( );


   cursor.hide();
   movenum = 0;

   actplayer = actmap->actplayer;

   orgmap = actmap;
   actmap = loadreplay ( orgmap->replayinfo->map[player]  );

   actmap->playerView = actplayer;

   tmemorystream guidatastream ( orgmap->replayinfo->guidata [ player ], tnstream::reading );
   stream = &guidatastream;

   if ( stream->dataavail () )
      nextaction = stream->readChar();
   else
      nextaction = rpl_finished;

//   orgmap.replayinfo->actmemstream = stream;

   npush (actgui);
   actgui = &gui;
   actgui->restorebackground();

   actmap->xpos = orgmap->cursorpos.position[ actplayer ].sx;
   actmap->ypos = orgmap->cursorpos.position[ actplayer ].sy;

   cursor.gotoxy ( orgmap->cursorpos.position[ actplayer ].cx, orgmap->cursorpos.position[ actplayer ].cy , 0);
   // lastvisiblecursorpos.x = orgmap.cursorpos.position[ actplayer ].cx;
   // lastvisiblecursorpos.y = orgmap.cursorpos.position[ actplayer ].cy;


   if ( stream->dataavail () )
      status = 1;
   else
      status = 11;

   computeview( actmap );
   displaymap ();

   dashboard.x = 0xffff;
   mousevisible( true );
//   cursor.show();

   cursor.checkposition( getxpos(), getypos() );
   bool resourcesCompared = false;
   do {
       if ( status == 2 ) {
          execnextreplaymove ( );
         /*
          if ( getxpos () != lastvisiblecursorpos.x || getypos () != lastvisiblecursorpos.y )
             setcursorpos ( lastvisiblecursorpos.x, lastvisiblecursorpos.y );
         */
       } else
          releasetimeslice();

       if (nextaction == rpl_finished  || status != 2) {
          if ( !cursor.an )
             cursor.show();
          if ( nextaction == rpl_finished && !resourcesCompared ) {
             actmap->endTurn();
             actmap->nextPlayer();
             resourcesCompared = true;
             ASCString resourceComparisonResult;
             tmap* comparisonMap = NULL;
             tmap* nextPlayerMap = NULL;
             if ( actmap->actplayer == orgmap->actplayer )
                comparisonMap = orgmap;
             else
                comparisonMap = nextPlayerMap = loadreplay ( orgmap->replayinfo->map[actmap->actplayer]  );

             if ( comparisonMap ) {
                if ( comparisonMap->compareResources( actmap, player, &resourceComparisonResult)) {
                   tviewanytext vat;
                   vat.init ( "warning", resourceComparisonResult.c_str() );
                   vat.run();
                   vat.done();
                }
             } else
                displaymessage("Replay: no map to compare to!", 1 );

             delete nextPlayerMap;
          }
       } else
          if ( cursor.an )
             cursor.hide();


       tkey input;
       while (keypress ()) {
           mainloopgeneralkeycheck ( input );
       }

       mainloopgeneralmousecheck ();

   } while ( status > 0  &&  status < 100 ) ;

   actgui->restorebackground();

   delete actmap;
   actmap = orgmap;

   npop ( actgui );

   int st = status;
   status = 0;

   cursor.gotoxy ( orgmap->cursorpos.position[ actplayer ].cx, orgmap->cursorpos.position[ actplayer ].cy );
   dashboard.x = 0xffff;

   if ( st == 101 )
      return 1;
   else
      return 0;
}

void trunreplay :: firstinit ( void )
{
    gui.init ( hgmp->resolutionx, hgmp->resolutiony );
    gui.starticonload();
    status = 0;
}
