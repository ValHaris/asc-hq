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
#include <conio.h>

#include "tpascal.inc"
#include "typen.h"
#include "vesa.h"
#include "loadpcx.h"
#include "krkr.h"
#include "sgstream.h"
#include "newfont.h"
#include "mousehnd.h"
#include "misc.h"

#define maxint 1147483648

int* buf;
int* bufptr;

dacpalette256 pal;

main(int argc, char *argv[], char *envp[])
{
   if ( argc <= 1) {
      printf( " Syntax: MAKEBKGR <filename> \n" );
      return 1;
   }

   initsvga ( 0x101 );
   initmousehandler ();
   buf = (int*) malloc ( 1000000 );
   bufptr = buf;
   loadpcxxy ( argv[1], 1, 0, 0 );
   mousevisible ( true );
   do {

   } while ( mouseparams.taste == 0 ); /* enddo */
   mousevisible ( false );

   int col = getpixel ( mouseparams.x, mouseparams.y );
   int abspos = 0;
   int dist = 0;
   int mode = 0;
   int delta = 0;
   buf++;
   for (int y = 0; y < 480; y++) {
      for (int x = 0; x < 640; x++) {
         int c = getpixel ( x, y );

         if ( c == col ) 
            abspos++;

         if ( mode ) {       // letztes Pixel war col 
            if (c == col) 
               dist++;
            else {
               putpixel ( x, y, white );
               *buf = delta + dist ;
               delta = 0;
               buf++;
               dist = 1;
               mode = 0;
               abspos++;
            }
         } else {
            if (c != col) 
               dist++;
            else {
               putpixel ( x, y, white );
               *buf = delta + dist ;
               delta = 0;
               buf++;
               dist = 1;
               mode = 1;
               abspos++;
            }
         } /* endif */

      } /* endfor */

      if ( abspos==0 ) 
         dist = 0;
      else
         if ( buf > bufptr ) {
               delta = -hgmp->bytesperscanline;
               putpixel ( x, y, blue );
            }
   } /* endfor */
  
   *bufptr = (buf - bufptr - 1) / 2;

   char s[100];
   strcpy ( s, argv[1] );
   int i=0;
   while ( s[i] != '.' )
      i++;
   s[++i] = 's';
   s[++i] = 'c';
   s[++i] = 'r';
   s[++i] = 0;

   bufptr[1] = 0;

   mainstream.init();
   mainstream.openstream ( s, 2 );
   mainstream.writedata ( (char*) bufptr, 4 * ( buf - bufptr ));
   mainstream.closestream();
   mainstream.done();

   removemousehandler();
   getch();
   settextmode ( 3 );
   return 0;
}


