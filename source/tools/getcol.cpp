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
#include <dos.h> 
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fstream.h>
#include <math.h>


#include "tpascal.inc"
#include "typen.h"
#include "vesa.h"
#include "misc.h"
#include "sgstream.h"
#include "stack.h"
#include "loadpcx.h"
#include "mousehnd.h"
#include <conio.h>


dacpalette256 pal;

main(int argc, char *argv[], char *envp[])
{
   if ( argc <= 1 ) {
      printf("zuwenig parameter. Datei erwartet.");
      return 1;
   }
      
   
   initsvga ( 0x103 );
   bar( 0, 0, 799, 599, 255 );
   loadpcxxy( argv[1], 1, 20, 20 );

   void* p = malloc ( 100000 );
   initmousehandler();
   mousevisible( true );

   int col = -1;
   do {

      if ( mouseparams.taste == 1 ) {
        mousevisible(false);
        col = getpixel ( mouseparams.x, mouseparams.y );
        mousevisible(true);
      }

   } while ( col < 0  ); /* enddo */

   settextmode(3);
   printf ( "color: %d \n", col );
   return 0;
}

