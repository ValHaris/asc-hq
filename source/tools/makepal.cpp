/*
    This file is part of Advanced Strategic Command; http://www.asc-hq.de
    Copyright (C) 1994-1999  Martin Bickel  and  Marc Schellenberger

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


#include "..\basegfx.h"
#include "..\loadpcx.h"
#include "..\typen.h"
#include "..\basestrm.h"
#include "..\sgstream.h"
#include "..\misc.h"

typedef char tmixbuf[256][256];

tmixbuf mixbuf;
char bi2asc[256];

#ifdef WIN32
#undef main
#endif


int     abs( int a )
{
   if ( a > 0 )
      return a;
   else
      return -a;
}

#define sqr(a) (a)*(a)
#define cub(a) abs ((a)*(a)*(a))


int calcdiff ( int r1, int r2, int g1, int g2, int b1, int b2 )
{
 //return cub( r1  - r2 ) + cub( g1 -  g2 ) + cub( b1 - b2 );
 return sqr( r1  - r2 ) + sqr( g1 -  g2 ) + sqr( b1 - b2 );
 //return abs( r1  - r2 ) + abs( g1 -  g2 ) + abs( b1 - b2 );
}


int main(int argc, char *argv[] )
{
   if ( argc < 3) {
      printf("Syntax:  makepal <ascpic.pcx> <bi2pic.pcx> \n");
      return 1;
   }

   tvirtualdisplay vd ( 1200, 1200 );
   loadpcxxy( argv[1], 1,0,0);

   tn_file_buf_stream mainstream ("palette.pal", tnstream::writing);
   memcpy ( pal, activepalette256, sizeof ( pal ));
   mainstream.writedata( pal, sizeof ( pal ));


   int i,j,k;
   for (i=0;i<256 ;i++ ) {
      printf("Generating color %d \n", i);
      mixbuf[i][i] = i;
      for (j=i+1;j<256 ;j++ ) {
         int diff = 0xFFFFFFF;
         int actdif;
         char pix1;

         int r = ((*activepalette256)[i][0] + (*activepalette256)[j][0] ) ;
         int g = ((*activepalette256)[i][1] + (*activepalette256)[j][1] ) ;
         int b = ((*activepalette256)[i][2] + (*activepalette256)[j][2] ) ;

         for (k=0;k<256 ;k++ ) {
            actdif = calcdiff( (*activepalette256)[k][0]  * 2 , r ,
                               (*activepalette256)[k][1]  * 2 , g ,
                               (*activepalette256)[k][2]  * 2 , b );
            if (actdif < diff) {
               diff = actdif;
               pix1 = k;
            }
         }

        mixbuf[i][j] = pix1;
        mixbuf[j][i] = pix1;

      }
   }

   mainstream.writedata( mixbuf, sizeof ( mixbuf ));


   const int shadings[8] = { 9, 8, 6, 4, 11, 12, 14, 17 };
   for ( int sh = 0; sh < 8; sh++ ) {
      tpixelxlattable xl;
      for (i=0;i<256 ;i++ ) {

         int diff2 = 0xFFFFFFF;
         int actdif2;

         int r2 = (*activepalette256)[i][0] * shadings[sh] / 10;
         int g2 = (*activepalette256)[i][1] * shadings[sh] / 10;
         int b2 = (*activepalette256)[i][2] * shadings[sh] / 10;

         for (k=0;k<256 ;k++ ) {
            actdif2 = calcdiff( (*activepalette256)[k][0] , r2 ,
                               (*activepalette256)[k][1] , g2 ,
                               (*activepalette256)[k][2] , b2 );
            if (actdif2 < diff2) {
               diff2 = actdif2;
               xl[i] = k;
            }
         }
      }

      xl[255] = 255;
      mainstream.writedata( xl, sizeof ( xl ));
   }


   for ( int r = 0; r < 64; r++ ) {
      printf("Generating truecolor2pal table %d \n", r);
      for ( int g = 0; g < 64; g++ )
         for ( int b = 0; b < 64; b++ ) {
            int sml = r  + ( g << 6) + ( b << 12 );

            int diff = 0xFFFFFFF;
            int pix1;

            for ( int k=0;k<256 ;k++ ) {
               int actdif = calcdiff( pal[k][0]  , r , pal[k][1]  , g , pal[k][2]  , b );

               if (actdif < diff) {
                  diff = actdif;
                  pix1 = k;
               }
            }
            truecolor2pal_table[sml] = pix1;
         }
   }
   mainstream.writedata( truecolor2pal_table, sizeof ( truecolor2pal_table ));

   loadpcxxy( argv[2], 1,0,0);

   printf("Generating BattleIsle Translation table:\n");

   for (i=0;i<256 ;i++ ) {

      int diff2 = 0xFFFFFFF;
      int actdif2;

      int r2 = (*activepalette256)[i][0];
      int g2 = (*activepalette256)[i][1];
      int b2 = (*activepalette256)[i][2];

      for (k=0;k<255 ;k++ ) {

         actdif2 = calcdiff( pal[k][0] , r2 ,
                             pal[k][1] , g2 ,
                             pal[k][2] , b2 );
         if (actdif2 < diff2) {
            diff2 = actdif2;
            bi2asc[i] = k;
         }
      }
      printf(".");
      fflush ( stdout );
   } /* endfor */

   bi2asc[0] = 255;
   for ( k = 1; k < 8; k++ )
      bi2asc[k] = 24-k;

   mainstream.writedata( &bi2asc, 256 );

   return 0;
}
