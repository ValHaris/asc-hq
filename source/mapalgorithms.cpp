/*! \file mapalgorithms.cpp
    \brief Routines for working with hexagonal grids
*/

/***************************************************************************
                          mapalgorithms.cpp  -  description
                             -------------------
    begin                : Thu Oct 5 2000
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

#include <math.h>
#include "mapalgorithms.h"
#include "typen.h"
#include "vehicletype.h"
#include "errors.h"
#include "gamemap.h"


tdrawgettempline :: tdrawgettempline ( int _freefields, pmap _gamemap )
{
   gamemap = _gamemap;
   tempsum = 0;
   freefields = _freefields;
   num = 0;
}

void tdrawgettempline :: putpix8 ( int x, int y )
{
   if ( !gamemap->getField ( x, y ) )
      return;
   
   if ( num >= freefields )
      tempsum += gamemap->getField ( x, y )->getjamming();
   num++;
   /*
                           getfield( x,y )->temp = 100;
                           displaymap();
                           cursor.gotoxy ( x , y );
                                                   */
}

int tdrawgettempline :: winkelcomp ( double w1, double w2 )
{
   double pi = 3.141592654;
   double delta = w2 - w1;
   if ( delta > -0.0001 && delta < 0.0001 )
      return 0;

   if ( delta > 0 ) {
      if ( delta <= pi )
         return 1;
      else
         return -1;
   }

   if ( delta < 0 ) {
      if ( delta < -pi )
         return 1;
      else
         return -1;
   }

   return 0;
}


int tdrawgettempline :: initialized = 0;
double tdrawgettempline :: dirs[ sidenum ];
double tdrawgettempline :: offset;

void tdrawgettempline :: init ( void )
{
   if ( !initialized ) {
      offset = 0;
      sx = 10;
      sy = 10;
      int i;

      for ( i = 0; i < sidenum; i++ ) {
         sx = 10;
         sy = 10;
         getnextfield ( sx, sy, i );
         dirs[i] = winkel ( 10, 10 );
      }
      offset = dirs[0];

      for ( i = 0; i < sidenum; i++ ) {
         sx = 10;
         sy = 10;
         getnextfield ( sx, sy, i );
         dirs[i] = winkel ( 10, 10 );
      }

      initialized = 1;
   }
}


double tdrawgettempline :: winkel ( int x, int y )
{
   int xp2 = sx * fielddistx + (sy & 1) * fielddisthalfx;
   int yp2 = sy * fielddisty;

   int xp1 = x * fielddistx + (y & 1) * fielddisthalfx;
   int yp1 = y * fielddisty;

   int dx = xp2-xp1;
   int dy = yp2-yp1;
   double at = atan2 ( double(dy), double(dx) );
   // printf("%d / %d / %f \n", dx, dy, at);
   at -= offset;
   while ( at < 0 )
      at += 2 * 3.14159265;

   // printf("%f \n", at);
   return at;
}

#define checkvisibility

void tdrawgettempline :: start ( int x1, int y1, int x2, int y2 )
{
   init();

   sx = x2;
   sy = y2;

   if ( y1 == y2  && x1 == x2 )
      return;

   int x = x1;
   int y = y1;

   double w = winkel ( x1, y1 );

   int dir = -1;
   double mindiff = 10000;
   for ( int i = 0; i < sidenum; i++ ) {
      double nd = fabs ( dirs[i] - w );
      if ( nd < mindiff ) {
         dir = i;
         mindiff = nd;
      }
   }

#ifdef checkvisibility
   int ldist = beeline ( x1, y1, x2, y2 );
#endif

   int lastdir = winkelcomp ( w, dirs[dir] );
   /*
      if ( x1 == 18 && y1 == 24 && x2 == 18 && y2 == 9 ) {
         printf("blurb");
      }
   */

   getnextfield( x, y, dir );
   while ( x != x2 || y != y2 ) {
#ifdef checkvisibility
      int ldist2 = beeline ( x, y, x2, y2 );
      if ( ldist2 > ldist ) {
         fatalError ( "inconsistency in tdrawgettempline :: start ; parameters are %d/%d ; %d/%d ", 1, x1, y1, x2, y2 );
         return;
      }
#endif

      putpix8 ( x, y );
      double w2 = winkel ( x, y );
      // printf("%f \n", w2);
      if ( lastdir > 0 ) {
         if ( winkelcomp ( w2,  w ) == 1 ) {
            dir--;
            lastdir = -1;
         }
      } else {
         if ( winkelcomp ( w2 , w ) == -1 ) {
            dir++;
            lastdir = 1;
         }
      }
      if ( dir < 0 )
         dir += sidenum;

      if ( dir >= sidenum )
         dir = dir % sidenum;

      getnextfield ( x, y, dir );
   }
   putpix8 ( x, y );
}






SearchFields :: SearchFields ( pmap _gamemap )
{
   gamemap = _gamemap;
   cancelSearch = false;
}

void         SearchFields::initsearch( const MapCoordinate& startPosition, int _firstDistance, int _lastDistance )
{
   cancelSearch = false;
   startPos = startPosition;
   firstDistance = _firstDistance;
   lastDistance = _lastDistance;
}



void         SearchFields::startsearch(void)
{
   if ( cancelSearch )
      return;

   int   step;

   if (firstDistance > lastDistance)
      step = -1;
   else
      step = 1;

   dist = firstDistance;

   do {
      MapCoordinate mc ( startPos.x, startPos.y - 2 * dist );
      if ( dist == 0 ) {
         if ((mc.x >= 0) && (mc.y >= 0) && (mc.x < gamemap->xsize) && (mc.y < gamemap->ysize))
            testfield( mc );

         if ( cancelSearch )
            return;

      } else {
         for ( int e = 0; e < 6; e++ ) {
            int dir = (e + 2) % sidenum;
            for ( int c = 0; c < dist; c++) {
               if ((mc.x >= 0) && (mc.y >= 0) && (mc.x < gamemap->xsize) && (mc.y < gamemap->ysize)) {
                  testfield( mc );
                  if ( cancelSearch )
                     return;
               }
               getnextfield ( mc.x, mc.y, dir );
            }

         }
      }

      dist += step;

   }  while (!((dist - step == lastDistance) || cancelSearch));
}

int         ccmpheighchangemovedir[6]  = {0, 1, 5, 2, 4, 3 };


MapCoordinate3D getNeighbouringFieldCoordinate( const MapCoordinate3D& pos, int direc)
{
  MapCoordinate3D mc = pos;
  getnextfield ( mc.x, mc.y, direc );
  return mc;
}

MapCoordinate getNeighbouringFieldCoordinate( const MapCoordinate& pos, int direc)
{
  MapCoordinate mc = pos;
  getnextfield ( mc.x, mc.y, direc );
  return mc;
}


void         getnextfield(int&       x,
                          int&       y,
                          int       direc)
{
   switch (direc) {

      case 0:
         y-=2   ;                      /*  oben  */
         break;

      case 1:
         if ((y & 1) == 1) x+=1;        /*  rechts oben  */
         y-=1;
         break;

      case 2:
         if ((y & 1) == 1) x+=1;        /*  rechts unten  */
         y+=1;
         break;

      case 3:
         y+=2;                          /*  unten  */
         break;

      case 4:
         if ((y & 1) == 0) x-=1;        /*  links unten  */
         y+=1;
         break;

      case 5:
         if ((y & 1) == 0) x-=1;        /*  links oben  */
         y-=1;
         break;

   }
}


int getdirection( const MapCoordinate& start, const MapCoordinate& dest )
{
   return getdirection(start.x, start.y, dest.x, dest.y );
}

int          getdirection(    int      x1,
                              int      y1,
                              int      x2,
                              int      y2)
{
   int a;
   int dx = (2 * x2 + (y2 & 1)) - (2 * x1 + (y1 & 1));
   int dy = y2 - y1;

   if (dx < 0)
      if (dy < 0)
         a = 5;
      else
         a = 4;
   else
      if (dx > 0)
         if (dy < 0)
            a = 1;
         else
            a = 2;
      else  // dx is 0
         if (dy < 0)
            a = 0;
         else
            if ( dy > 0 )
               a = 3;
            else
               a = -1;
   return a;
}


int beeline ( const pvehicle a, const pvehicle b )
{
   return beeline ( a->xpos, a->ypos, b->xpos, b->ypos );
}

int beeline ( const MapCoordinate& a, const MapCoordinate& b )
{
   return beeline ( a.x, a.y, b.x, b.y );
}


int beeline ( int x1, int y1, int x2, int y2 )
{
   int xdist = abs ( (x1 * 2 + (y1 & 1 )) - ( x2 * 2 + ( y2 & 1)) );
   int ydist = abs ( y2 - y1 );
   int num2;
   if ( ydist > xdist )
      num2 = (ydist - xdist) / 2 + xdist;
   else
      num2 = max ( xdist, ydist );
/*
   int num = 0;
   while ( x1 != x2  || y1 != y2 ) {
      num++;
      getnextfield ( x1, y1, getdirection ( x1, y1, x2, y2 ));
   }

   if ( num != num2 )
      printf("beeline inconsistent\n" );
*/
   return minmalq*num2;
}


int square ( int i )
{
   return i*i;
}

inline float square ( float i )
{
   return i*i;
}


WindMovement::WindMovement ( const Vehicle* vehicle )
{
   for ( int i = 0; i < sidenum; i++ )
      wm[i] = 0;

   int movement = 0;
   for ( int height = 4; height <= 6; height++ )
      if ( vehicle->typ->movement[height] )
         if ( vehicle->typ->movement[height] > movement )
            movement = vehicle->typ->movement[height];


   if ( movement ) {
      int wmn[7];
      
      int lastDir = 0;
      
      float abswindspeed = float( vehicle->getMap()->weather.windSpeed) * maxwindspeed / 255;
      
      for ( float direc = 0; direc < 360; direc++) {
         static const float pi = 3.14159265;
         float unitspeedx = movement * sin(direc/180*pi);
         float unitspeedy = movement * cos(direc/180*pi);

         float angle = atan2( unitspeedx, unitspeedy + abswindspeed );
         if ( angle < 0 )
            angle += 2 * pi;

         if ( angle >= 60 * float(lastDir) * (2*pi) / 360 ) {
            float absspeed = sqrt ( square ( unitspeedy + abswindspeed)+ square ( unitspeedx) );
            wmn[lastDir] = int( 10 - 10*movement/absspeed );
            ++lastDir;
         }
      }
      
      for ( int i = 0; i <= 3; i++ ) {
         wm[(i+vehicle->getMap()->weather.windDirection)%6] = wmn[i];
         if ( i > 0 )
            wm[(6-i+vehicle->getMap()->weather.windDirection)%6] = wmn[i];
      }
   }
}



int WindMovement::getDist ( int dir )
{
   return wm[dir];
}



