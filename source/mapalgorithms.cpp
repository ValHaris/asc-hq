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


tdrawgettempline :: tdrawgettempline ( int _freefields, pmap _gamemap )
{
   gamemap = _gamemap;
   tempsum = 0;
   freefields = _freefields;
   num = 0;
}

void tdrawgettempline :: putpix8 ( int x, int y )
{
   if ( !gamemap->getField ( x, y ) ) {
      x = sx;
      y = sy;
      return;
   }
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
   double at = atan2 ( dy, dx );
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






tsearchfields :: tsearchfields ( void )
{
   abbruch = 0;
}

void         tsearchfields::initsuche( pmap _gamemap, int  sx, int  sy, int max, int min )
{
   gamemap = _gamemap;
   maxdistance = max;
   mindistance = min;
   if (mindistance == 0) mindistance = 1;
   if (maxdistance == 0) maxdistance = 1;
   startx = sx;
   starty = sy;
}


#ifdef HEXAGON

void         tsearchfields::startsuche(void)
{
   if ( abbruch )
      return;

   int   step;

   if (mindistance > maxdistance)
      step = -1;
   else
      step = 1;
   int strecke = mindistance;

   do {
      dist = strecke;

      xp = startx;
      yp = starty - 2*strecke;
      for ( int e = 0; e < 6; e++ ) {
         int dir = (e + 2) % sidenum;
         for ( int c = 0; c < strecke; c++) {
            if ((xp >= 0) && (yp >= 0) && (xp < gamemap->xsize) && (yp < gamemap->ysize))
               testfield();
            getnextfield ( xp, yp, dir );
         }

         if ( abbruch )
            return;
      }

      strecke += step;

   }  while (!((strecke - step == maxdistance) || abbruch));
}

#else

void         tsearchfields::startsuche(void)
{
   if ( abbruch )
      return;

   word         strecke;
   int          a, c;
   int          step;

   if (mindistance > maxdistance)
      step = -1;
   else
      step = 1;
   strecke = mindistance;

   do {
      dist = strecke;
      a = startx - strecke;

      xp = a;
      yp = starty;
      for (c = 1; c <= 2 * strecke; c++) {
         yp--;
         if ((starty & 1) == (c & 1))
            xp++;
         if ((xp >= 0) && (yp >= 0) && (xp < gamemap->xsize) && (yp < gamemap->ysize))
            testfield();
         if (abbruch) return;
      }

      xp = startx + strecke + (starty & 1);
      yp = starty;
      for (c = 0; c < 2 * strecke ; c++) {
         if ((starty & 1) != (c & 1))
            xp--;
         if ((xp >= 0) && (yp >= 0) && (xp < gamemap->xsize) && (yp < gamemap->ysize))
            testfield();
         if (abbruch)
            return;
         yp--;
      }

      yp = starty + strecke * 2 - 1;
      xp = startx - (yp & 1);
      for (c = 1; c <= 2 * strecke; c++) {   /*  ????????  */
         if ((xp >= 0) && (yp >= 0) && (xp < gamemap->xsize) && (yp < gamemap->ysize))
            testfield();
         if (abbruch)
            return;
         if ((starty & 1) == (c & 1)) xp--;

         yp--;
      }

      xp = startx;
      yp = starty + 2 * strecke;
      for (c = 0; c <= 2 * strecke - 1; c++) {
         if ((xp >= 0) && (yp >= 0) && (xp < gamemap->xsize) && (yp < gamemap->ysize))
            testfield();
         if ((starty & 1) != (c & 1))
            xp++;
         yp--;
         if (abbruch) return;
      }
      strecke += step;
   }  while (!((strecke - step == maxdistance) || abbruch));
}
#endif


#ifdef HEXAGON
int         ccmpheighchangemovedir[6]  = {0, 1, 5, 2, 4, 3 };

void         getnextfielddir(int&       x,
                             int&       y,
                             int       direc,
                             int       sdir)
{
   int newdir = ccmpheighchangemovedir[direc] + sdir;
   if ( newdir >= 6 )
      newdir -= 6;

   getnextfield( x, y, newdir );
}

#else
int         ccmpheighchangemovedir[8]  = {0, 1, 7, 2, 6, 3, 5, 4};


void         getnextfielddir(int&       x,
                             int&       y,
                             int       direc,
                             int       sdir)
{
   getnextfield( x, y, (ccmpheighchangemovedir[direc] + sdir) & 7);
}

#endif




void         getnextfield(int&       x,
                          int&       y,
                          int       direc)
{
   switch (direc) {

#ifdef HEXAGON
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

#else

      case 0:
         y-=2   ;                      /*  oben  */
         break;

      case 1:
         if ((y & 1) == 1) x+=1;        /*  rechts oben  */
         y-=1;
         break;

      case 2:
         x+=1;                        /*  rechts  */
         break;

      case 3:
         if ((y & 1) == 1) x+=1;        /*  rechts unten  */
         y+=1;
         break;

      case 4:
         y+=2;                          /*  unten  */
         break;

      case 5:
         if ((y & 1) == 0) x-=1;        /*  links unten  */
         y+=1;
         break;

      case 6:
         x-=1;                        /*  links  */
         break;

      case 7:
         if ((y & 1) == 0) x-=1;        /*  links oben  */
         y-=1;
         break;
#endif
   }
}

int          getdirection(    int      x1,
                              int      y1,
                              int      x2,
                              int      y2)

{
#ifdef HEXAGON
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


#else
   int      a;
   int      dx, dy;

   dx = (2 * x2 + (y2 & 1)) - (2 * x1 + (y1 & 1));
   dy = y2 - y1;
   if (dx < 0)
      if (dy < 0)
         a = 7;
      else
         if (dy == 0)
            a = 6;
         else
            a = 5;
   else
      if (dx > 0)
         if (dy < 0)
            a = 1;
         else
            if (dy == 0)
               a = 2;
            else
               a = 3;
      else
         if (dy < 0)
            a = 0;
         else
            a = 4;
   return a;
#endif
}


int beeline ( const pvehicle a, const pvehicle b )
{
   return beeline ( a->xpos, a->ypos, b->xpos, b->ypos );
}

int beeline ( int x1, int y1, int x2, int y2 )
{
#ifdef HEXAGON
   int num = 0;
   while ( x1 != x2  || y1 != y2 ) {
      num++;
      getnextfield ( x1, y1, getdirection ( x1, y1, x2, y2 ));
   }
   return minmalq*num;

#else
   return luftlinie8( x1, y1, x2, y2 );
#endif

}
