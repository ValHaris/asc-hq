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
      actmap->replayinfo = new treplayinfo;
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


int  ReplayMapDisplay :: displayMovingUnit ( int x1,int y1, int x2, int y2, pvehicle vehicle, int height1, int height2, int fieldnum, int totalmove, SoundLoopManager* slc )
{
   if ( actmap->playerView < 0 )
      return 0;

   if ( fieldvisiblenow ( getfield ( x1, y1 ), actmap->playerView) || fieldvisiblenow ( getfield ( x2, y2 ), actmap->playerView)) {
      if ( checkMapPosition  ( x1, y1 ))
         displayMap();

      int fc = mapDisplay->displayMovingUnit ( x1, y1, x2, y2, vehicle, height1, height2, fieldnum, totalmove, slc );
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




void logtoreplayinfo ( trpl_actions _action, ... )
{
   char action = _action;
   if ( actmap->replayinfo && actmap->replayinfo->actmemstream ) {
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
      if ( action == rpl_move2 || action == rpl_move3 ) {
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
         if ( action == rpl_move3 ) {
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
      if ( action == rpl_buildtnk ) {
         int x =  va_arg ( paramlist, int );
         int y =  va_arg ( paramlist, int );
         int id =  va_arg ( paramlist, int );
         int col =  va_arg ( paramlist, int );
         stream->writeChar ( action );
         int size = 4;
         stream->writeInt ( size );
         stream->writeInt ( x );
         stream->writeInt ( y );
         stream->writeInt ( id );
         stream->writeInt ( col );
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
      if ( action == rpl_refuel ) {
         int x =  va_arg ( paramlist, int );
         int y =  va_arg ( paramlist, int );
         int nwid = va_arg ( paramlist, int );
         int pos = va_arg ( paramlist, int );
         int amnt = va_arg ( paramlist, int );
         stream->writeChar ( action );
         int size = 5;
         stream->writeInt ( size );
         stream->writeInt ( x );
         stream->writeInt ( y );
         stream->writeInt ( nwid );
         stream->writeInt ( pos );
         stream->writeInt ( amnt );
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
   if ( fieldvisiblenow ( getactfield(), actmap->playerView ))
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
                           wait( t );
                           vm.execute ( NULL, x2, y2, 3, -1, -1 );

                           if ( vm.getStatus() != 1000 )
                              eht = NULL;
                        }

                        if ( !eht )
                           displaymessage("severe replay inconsistency:\nno vehicle for move1 command !", 1);
                     }
         break;
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
                        if ( nextaction == rpl_move3 )
                           noInterrupt = stream->readInt();
                        else
                           noInterrupt = -1;

                        readnextaction();

                        pvehicle eht = actmap->getUnit ( x1, y1, nwid );
                        if ( eht ) {
                           ReplayMapDisplay rmd( &defaultMapDisplay );
                           VehicleMovement vm ( &rmd, NULL );
                           vm.execute ( eht, -1, -1, 0 , height, -1 );

                           int t = ticker;
                           vm.execute ( NULL, x2, y2, 2, -1, -1 );
                           wait( t );
                           vm.execute ( NULL, x2, y2, 3, -1, noInterrupt );

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
                                   battle.calcdisplay ( ad2, dd2 );
                                   removeActionCursor();
                                } else {
                                   battle.calc ();
                                   battle.av.damage = ad2;
                                   battle.dv.damage = dd2;
                                }
                                battle.setresult ();
                                dashboard.x = 0xffff;
                             } else
                             if ( targ->building ) {
                                tunitattacksbuilding battle ( fld->vehicle, x2, y2 , wpnum );
                                battle.av.damage = ad1;
                                battle.dv.damage = dd1;
                                if ( attackvisible ) {
                                   displayActionCursor ( x1, y1, x2, y2, 0 );
                                   battle.calcdisplay ( ad2, dd2 );
                                   removeActionCursor();
                                } else {
                                   battle.calc ();
                                   battle.av.damage = ad2;
                                   battle.dv.damage = dd2;
                                }
                                battle.setresult ();
                                dashboard.x = 0xffff;
                             } else
                             if ( !targ->objects.empty() ) {
                                tunitattacksobject battle ( fld->vehicle, x2, y2, wpnum );
                                if ( attackvisible ) {
                                   displayActionCursor ( x1, y1, x2, y2, 0 );
                                   battle.calcdisplay ( ad2, dd2 );
                                   removeActionCursor();
                                } else {
                                   battle.calc ();
                                   battle.av.damage = ad2;
                                   battle.dv.damage = dd2;
                                }
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
                           wait( t );
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
                              wait();
                              removeActionCursor();
                           } else
                              displaymessage("severe replay inconsistency:\nno vehicle for convert command !", 1);


                       }
         break;
      case rpl_remobj:
      case rpl_buildobj: {
                           stream->readInt();  // size
                           int x = stream->readInt();
                           int y = stream->readInt();
                           int id = stream->readInt();
                           readnextaction();

                           pobjecttype obj = getobjecttype_forid ( id );

                           pfield fld = getfield ( x, y );
                           if ( obj && fld ) {
                              displayActionCursor ( x, y );

                              if ( actaction == rpl_remobj )
                                 fld->removeobject ( obj );
                              else
                                 fld->addobject ( obj );

                              computeview( actmap );
                              displaymap();
                              wait();
                              removeActionCursor();
                           } else
                              displaymessage("severe replay inconsistency:\nCannot find Object to build/remove !", 1 );

                       }
         break;
      case rpl_buildtnk: {
                           stream->readInt();  // size
                           int x = stream->readInt();
                           int y = stream->readInt();
                           int id = stream->readInt();
                           int col = stream->readInt();
                           readnextaction();

                           pfield fld = getfield ( x, y );

                           pvehicletype tnk = getvehicletype_forid ( id );

                           if ( fld && tnk && !fld->vehicle ) {
                              displayActionCursor ( x, y );
                              pvehicle v = new Vehicle ( tnk, actmap, col );
                              v->xpos = x;
                              v->ypos = y;
                              fld->vehicle = v;

                              computeview( actmap );
                              displaymap();
                              wait();
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
                                  wait();
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
                              displaymap();
                              wait();
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
                              displaymap();
                              wait();
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
                                    pvehicle eht = new Vehicle ( tnk, actmap, col / 8 );
                                    eht->klasse = cl;
                                    eht->xpos = x;
                                    eht->ypos = y;
                                    eht->setup_classparams_after_generation ();
                                    eht->tank.fuel = eht->typ->tank.fuel;
                                    eht->networkid = nwid;

                                    if ( fld->building ) {
                                       int i = 0;
                                       while ( fld->building->loading[i])
                                          i++;
                                       fld->building->loading[i] = eht;
                                    } else {
                                       displayActionCursor ( x, y );
                                       fld->vehicle = eht;
                                       computeview( actmap );
                                       displaymap();
                                       wait();
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

                                    // stream->readdata2 ( *sv );
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
      case rpl_refuel : {
                                 stream->readInt();  // size
                                 int x = stream->readInt();
                                 int y = stream->readInt();
                                 int nwid = stream->readInt();
                                 int pos = stream->readInt();
                                 int amnt = stream->readInt();
                                 readnextaction();

                                 pvehicle eht = actmap->getUnit ( x, y, nwid );
                                 if ( eht ) {
                                    if ( pos < 16 )
                                       eht->ammo[pos] = amnt;
                                     else {
                                        switch ( pos ) {
                                        case 1000: eht->tank.energy = amnt;
                                           break;
                                        case 1001: eht->tank.material = amnt;
                                           break;
                                        case 1002: eht->tank.fuel = amnt;
                                           break;
                                        } /* endswitch */
                                     }
                                 } else
                                    displaymessage("severe replay inconsistency:\nno vehicle for refuel-unit command !", 1);
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
   actmap = NULL;

   loadreplay ( orgmap->replayinfo->map[player]  );

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
   do {
       if ( status == 2 ) {
          execnextreplaymove ( );
         /*
          if ( getxpos () != lastvisiblecursorpos.x || getypos () != lastvisiblecursorpos.y )
             setcursorpos ( lastvisiblecursorpos.x, lastvisiblecursorpos.y );
         */
       }

       if (nextaction == rpl_finished  || status != 2) {
          if ( !cursor.an )
             cursor.show();
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
