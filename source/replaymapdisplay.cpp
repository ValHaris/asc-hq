/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "replaymapdisplay.h"
#include "spfst.h"
#include "viewcalculation.h"
#include "events.h"
#include "gameoptions.h"
#include "spfst-legacy.h"

int ReplayMapDisplay :: checkMapPosition ( int x, int y )
{
   mapDisplay->displayPosition(x,y);
   /*

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
      */
      return 0;
}


int ReplayMapDisplay :: displayMovingUnit ( const MapCoordinate3D& start, const MapCoordinate3D& dest, Vehicle* vehicle, int fieldnum, int totalmove, SoundStartCallback startSound, int duration )
{
   if ( actmap->getPlayerView() < 0 )
      return 0;

   
   bool view1 = fieldvisiblenow ( getfield ( start.x, start.y ), actmap->getPlayerView() );
   
   int view2;
   MapField* fld2 = actmap->getField ( dest );
   
   if ( actmap->player[actmap->getPlayerView()].diplomacy.sharesView( vehicle->getOwner() )) 
      view2 = fieldVisibility ( fld2, actmap->getPlayerView() );
   else {
      // This is a workaround to estimate if the target field will be visible or not once the units is there 
      view2 = calcvisibilityfield ( actmap, fld2, actmap->getPlayerView(), -1, actmap->getgameparameter ( cgp_initialMapVisibility ), vehicle->typ->jamming );
   }

   /*
   if ( vehicle->height >= chschwimmend && vehicle->height <= chhochfliegend ) {
      visibility = fieldvisiblenow ( getfield ( start.x, start.y ), actmap->playerView) || fieldvisiblenow ( getfield ( dest.x, dest.y ), actmap->playerView);
   } else {
      // visibility = (fieldVisibility ( getfield ( start.x, start.y ), actmap->playerView) >= visible_all) && (fieldVisibility ( getfield ( dest.x, dest.y ), actmap->playerView) >= visible_now);
      visibility = fieldvisiblenow ( getfield ( start.x, start.y ), actmap->playerView) || fieldvisiblenow ( getfield ( dest.x, dest.y ), actmap->playerView);
   }
   */
   
   if ( view1 || (view2 >= visible_now ) ) {
      if ( checkMapPosition  ( start.x, start.y ))
         displayMap();

      int fc = mapDisplay->displayMovingUnit ( start, dest, vehicle, fieldnum, totalmove, startSound, duration );
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
   if ( fieldvisiblenow ( getfield ( x, y ), actmap->getPlayerView() )) {
      checkMapPosition  ( x, y );
      mapDisplay->displayPosition ( x, y );
   }
}

void ReplayMapDisplay :: removeActionCursor ( void )
{
   mapDisplay->removeActionCursor();
 
}

void ReplayMapDisplay :: displayActionCursor ( int x1, int y1, int x2, int y2, int secondWait )
{
   if ( x1 >= 0 && y1 >= 0 ) {
      int i = fieldvisiblenow ( getfield ( x1, y1 ), actmap->getPlayerView() );
      if( i ) {
         cursor_goto( MapCoordinate( x1, y1 ));
         
         if ( x2 >= 0 && y2 >= 0 )
            wait( 30 );
         else
            wait();
      }
   }

   if ( x2 >= 0 && y2 >= 0 ) {
      int i = fieldvisiblenow ( getfield ( x2, y2 ), actmap->getPlayerView() );
      if( i ) {
         cursor_goto( MapCoordinate( x2, y2 ));
         if ( secondWait )
            wait();
      }
   }
}

void ReplayMapDisplay :: wait ( int minTime )
{
   int t = SDL_GetTicks();
   while ( SDL_GetTicks() < t + max ( cursorDelay, minTime ) )
      releasetimeslice();
}

void ReplayMapDisplay :: playPositionalSound( const MapCoordinate& pos, Sound* snd )
{
   if ( fieldvisiblenow ( actmap->getField ( pos ), actmap->getPlayerView() )) {
      mapDisplay->playPositionalSound( pos, snd );
   }
}


int ReplayMapDisplay :: getUnitMovementDuration() const 
{ 
   int speed = CGameOptions::Instance()->movespeed;
   int factor = max( 10, CGameOptions::Instance()->replayMoveSpeedFactor );
   return speed * 100 / factor;
}

