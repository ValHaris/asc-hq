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


#include "..\vesa.h"
#include "..\loadpcx.h"
#include "..\typen.h"
#include "..\sgstream.h"
#include "krkr.h"
#include "..\misc.h"

typedef char tmixbuf[256][256];

tmixbuf mixbuf;
tpixelxlattable hell,dunkel1,dunkel2,dunkel3,dunkel05;
char bi2asc[256];


int     abs( int a ) 
{
   if ( a > 0 )
      return a;
   else
      return -a;
}
#pragma intrinsic ( abs );

#define sqr(a) (a)*(a)
#define cub(a) abs ((a)*(a)*(a))


int calcdiff ( int r1, int r2, int g1, int g2, int b1, int b2 )
{
 //return cub( r1  - r2 ) + cub( g1 -  g2 ) + cub( b1 - b2 );
 return sqr( r1  - r2 ) + sqr( g1 -  g2 ) + sqr( b1 - b2 );
 //return abs( r1  - r2 ) + abs( g1 -  g2 ) + abs( b1 - b2 );
}


int main(int argc, char *argv[], char *envp[])
{
   if ( argc < 3) {
      printf("Syntax:  makepal <ascpic.pcx> <bi2pic.pcx> \n");
      return 1;
   }

   int modenum8;

   pavailablemodes avm = searchformode ( 800, 600, 8 );

   if (avm->num > 0) {
       modenum8 = avm->mode[0].num;
       initsvga( modenum8 );
   } else {
      printf("\nno vesa mode found!\n");
      return 1;
   }
   loadpcxxy( argv[1], 1,0,0);
   getch();
   settextmode ( 3 );




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

      int diff2 = 0xFFFFFFF;
      int actdif2;

      int r2 = (*activepalette256)[i][0] * 8 / 10;
      int g2 = (*activepalette256)[i][1] * 8 / 10;
      int b2 = (*activepalette256)[i][2] * 8 / 10;

      for (k=0;k<256 ;k++ ) {

         // actdif2 = sqr( (*activepalette256)[k][0] - r2 ) + sqr( (*activepalette256)[k][1] -  g2 ) + sqr( (*activepalette256)[k][2] - b2 );
         actdif2 = calcdiff( (*activepalette256)[k][0] , r2 ,
                            (*activepalette256)[k][1] , g2 ,
                            (*activepalette256)[k][2] , b2 );
         if (actdif2 < diff2) {
            diff2 = actdif2;
            dunkel1[i] = k;
         }
      } 

      diff2 = 0xFFFFFFF;

      r2 = (*activepalette256)[i][0] * 6 / 10;
      g2 = (*activepalette256)[i][1] * 6 / 10;
      b2 = (*activepalette256)[i][2] * 6 / 10;

      for (k=0;k<256 ;k++ ) {

         // actdif2 = sqr( (*activepalette256)[k][0] - r2 ) + sqr( (*activepalette256)[k][1] -  g2 ) + sqr( (*activepalette256)[k][2] - b2 );
         actdif2 = calcdiff( (*activepalette256)[k][0] , r2 ,
                            (*activepalette256)[k][1] , g2 ,
                            (*activepalette256)[k][2] , b2 );
         if (actdif2 < diff2) {
            diff2 = actdif2;
            dunkel2[i] = k;
         }
      } 

// Dunkel 05

      diff2 = 0xFFFFFFF;

      r2 = (*activepalette256)[i][0] * 9 / 10;
      g2 = (*activepalette256)[i][1] * 9 / 10;
      b2 = (*activepalette256)[i][2] * 9 / 10;

      for (k=0;k<256 ;k++ ) {

         // actdif2 = sqr( (*activepalette256)[k][0] - r2 ) + sqr( (*activepalette256)[k][1] -  g2 ) + sqr( (*activepalette256)[k][2] - b2 );
         actdif2 = calcdiff( (*activepalette256)[k][0] , r2 ,
                             (*activepalette256)[k][1] , g2 ,
                             (*activepalette256)[k][2] , b2 );
         if (actdif2 < diff2) {
            diff2 = actdif2;
            dunkel05[i] = k;
         }
      } 



      diff2 = 0xFFFFFFF;

      r2 = (*activepalette256)[i][0] * 4 / 10;
      g2 = (*activepalette256)[i][1] * 4 / 10;
      b2 = (*activepalette256)[i][2] * 4 / 10;

      for (k=0;k<256 ;k++ ) {

         // actdif2 = sqr( (*activepalette256)[k][0] - r2 ) + sqr( (*activepalette256)[k][1] -  g2 ) + sqr( (*activepalette256)[k][2] - b2 );
         actdif2 = calcdiff( (*activepalette256)[k][0] , r2 ,
                             (*activepalette256)[k][1] , g2 ,
                             (*activepalette256)[k][2] , b2 );
         if (actdif2 < diff2) {
            diff2 = actdif2;
            dunkel3[i] = k;
         }
      } 



      int diff3 = 0xFFFFFFF;
      int actdif3;

      int r3 = (*activepalette256)[i][0] * 13 / 10;
      int g3 = (*activepalette256)[i][1] * 13 / 10;
      int b3 = (*activepalette256)[i][2] * 13 / 10;

      for (k=0;k<256 ;k++ ) {

         // actdif3 = sqr( (*activepalette256)[k][0] - r3 ) + sqr( (*activepalette256)[k][1] -  g3 ) + sqr( (*activepalette256)[k][2] - b3 );
         actdif3 = calcdiff( (*activepalette256)[k][0] , r3 ,
                            (*activepalette256)[k][1] , g3 ,
                            (*activepalette256)[k][2] , b3 );
         if (actdif3 < diff3) {
            diff3 = actdif3;
            hell[i] = k;
         }
      } 

   } /* endfor */

   memcpy ( pal, activepalette256, sizeof ( pal ));

   {
         for ( int r = 0; r < 64; r++ ) {
            printf("Generating truecolor2pal table %d \n", r);
            for ( int g = 0; g < 64; g++ )
               for ( int b = 0; b < 64; b++ ) {
                     int sml = r  + ( g << 6) + ( b << 12 );
      
                     int diff = 0xFFFFFFF;
                     int pix1;
            
                     for ( int k=0;k<256 ;k++ ) {
                        int actdif = sqr( pal[k][0]  - r ) + sqr( pal[k][1]  - g ) + sqr( pal[k][2]  - b );
            
                        if (actdif < diff) {
                           diff = actdif;
                           pix1 = k;
                        }
                     } 
                    truecolor2pal_table[sml] = pix1;
               }
          }
   }



   initsvga( modenum8 );
   loadpcxxy( argv[2], 1,0,0);
   getch();
   settextmode ( 3 );

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
//      bi2asc[255] = 255;
      printf(".");
      fflush ( stdout );
   } /* endfor */

      bi2asc[0] = 255;
      for ( k = 1; k < 8; k++ )
         bi2asc[k] = 24-k;


   { 
      tn_file_buf_stream mainstream ("palette.pal",2);
      mainstream.writedata( pal, sizeof ( pal ));
      mainstream.writedata( mixbuf, sizeof ( mixbuf ));
      mainstream.writedata( dunkel05, sizeof ( dunkel05 ));
      mainstream.writedata( dunkel1, sizeof ( dunkel1 ));
      mainstream.writedata( dunkel2, sizeof ( dunkel2 ));
      mainstream.writedata( dunkel3, sizeof ( dunkel3 ));
      mainstream.writedata( hell, sizeof ( hell ));
      mainstream.writedata( truecolor2pal_table, sizeof ( truecolor2pal_table ));
      mainstream.writedata( &bi2asc, 256 );
   }

   return 0;
}
