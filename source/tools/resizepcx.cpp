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


#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>


#include "..\tpascal.inc"
#include "..\vesa.h"
#include "..\misc.h"
#include "..\stack.h"
#include "..\loadpcx.h"
#include "..\palette.h"



int searchline ( int x1, int y1, int x2, int y2 )
{
   if ( x1 == x2 ) {
      for ( int y = y1; y <= y2; y++ )
         if ( getpixel ( x1, y ) != 255 )
            return 1;
         
      return 0;
   } else {
      for ( int x = x1; x <= x2; x++ )
         if ( getpixel ( x, y1 ) != 255 )
            return 1;
         
      return 0;
   }
}

main(int argc, char *argv[], char *envp[])
{
   if ( argc <= 1 ) {
      printf("\nmissing parameter. filename expected.");
      return 1;
   }
      
   pavailablemodes avm = searchformode ( 800, 600, 8 );
   if (avm->num > 0) 
      initsvga( avm->mode[0].num );
   else {
      avm = searchformode ( 640, 480, 8 );
      if (avm->num > 0) 
         initsvga( avm->mode[0].num );
      else {
         printf("no vesa modes available !\n" );
         return 1;
      }

   }


   bar( 0, 0, agmp->resolutionx-1, agmp->resolutiony-1, 255 );
   loadpcxxy( argv[1], 1, 0, 0 );

   int minx = 0;
   while ( !searchline ( minx, 0, minx, agmp->resolutiony-1 ))
      minx++;

   int miny = 0;
   while ( !searchline ( 0, miny, agmp->resolutionx-1, miny ))
      miny++;

   int maxx = agmp->resolutionx-1;
   while ( !searchline ( maxx, 0, maxx, agmp->resolutiony-1 ))
      maxx--;


   int maxy = agmp->resolutiony-1;
   while ( !searchline ( 0, maxy, agmp->resolutionx-1, maxy ))
      maxy--;

   getch();
   writepcx ( argv[1], minx, miny, maxx, maxy, pal );

   rectangle ( minx, miny, maxx, maxy, black );

   getch();
   closesvga();
   settextmode(3);
   return 0;

}
