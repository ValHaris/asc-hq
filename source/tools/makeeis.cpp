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

#include <malloc.h>
#include <stdio.h>
#include <i86.h>
#include <graph.h>

#include "tpascal.inc"
#include "typen.h"
#include "vesa.h"
#include "loadpcx.h"
#include "krkr.h"
#include "sgstream.h"
#include "misc.h"


struct timage {
                int size;
                char data[51][51];
              };
 

void* schabl[8];
timage* img[8];


main( void )
{
   initsvga ( 0x101 );
   char* c = strdup ( "eis-#.pcx");

   for (int i = 0; i < 8; i++) {
      c[4] = digit[i][0];
      loadpcxxy ( c, 1, 0, 0 );
      schabl[i] = malloc ( 20000 );
      img[i] = (timage*) schabl[i];
      getimage ( 0, 0, 50, 50, schabl[i] );
//      getch();
   } /* endfor */

   initsvga ( 0x103 );
   loadpcxxy ( "watr1.pcx",1, 0, 0 );

   for ( i = 0; i < 8; i++ )
      putimage ( 400, i * 60, schabl[i] );

   void* buf = malloc ( 100000 );

   tn_file_buf_stream stream ( "eisbr1.raw", 2 );

/*
   for ( i = 0; i < 256; i++ ) {
      bar ( 500, 100, 600, 200, 255 );
      for ( int y = 0; y < 50; y++ )
         for ( int x = 0; x < 50; x++ ) {
            int b = getpixel ( 35 + x, 35 + y );
            int c = 255;
            for ( int j = 0; j < 8; j++ )
               if ( i & ( 1 << j ) )
//                  if ( img[8]->data[y][x] == 1 ) 
                  if ( getpixel ( 400 + x, j * 60 + y ) == 1 )
                     c = b;

            putpixel ( 500 + x, 100 + y, c );

         }
      getimage ( 500, 100, 550, 150, buf );
      stream.writerlepict ( buf );
//      getch();
   } */



   for ( i = 0; i < 8; i++ ) {
      bar ( 500, 100, 600, 200, 255 );
      for ( int y = 0; y < 50; y++ )
         for ( int x = 0; x < 50; x++ ) {
            int b = getpixel ( 35 + x, 35 + y );
            int c = 255;
               if ( getpixel ( 400 + x, i * 60 + y ) == 1 )
                  c = b;

            putpixel ( 500 + x, 100 + y, c );

         }
      getimage ( 500, 100, 550, 150, buf );
      stream.writerlepict ( buf );
   } 



   settextmode ( 3 );
}




