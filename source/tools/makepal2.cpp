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


#include "vesa.h"
#include "loadpcx.h"
#include "typen.h"
#include "sgstream.h"
#include "krkr.h"
#include "misc.h"
#include "conio.h"

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


dacpalette256 orgpal;
dacpalette256 pal;

main(int argc, char *argv[], char *envp[])
{
   if ( argc < 2) {
      printf("Syntax:  makepal <filename.pcx> \n");
      return 1;
   }

   pavailablemodes avm = searchformode ( 1280, 1024, 8 );
   if (avm->num > 0) 
      initsvga( avm->mode[0].num );
   else {
      printf(" no matching graphic mode found !\n" );
      return 1;
   }

   loadpcxxy( argv[1], 1,0,0);

   {
      tnfilestream stream ("palette.pal",1); 
      stream.readdata( &orgpal, sizeof(orgpal)); 
   }


   int i,j,k;
   for (i=0;i<256 ;i++ ) {

      int diff2 = 0xFFFFFFF;
      int actdif2;

      int r2 = (*activepalette256)[i][0];
      int g2 = (*activepalette256)[i][1];
      int b2 = (*activepalette256)[i][2];

      for (k=0;k<256 ;k++ ) {

         actdif2 = calcdiff( orgpal[k][0] , r2 ,
                            orgpal[k][1] , g2 ,
                            orgpal[k][2] , b2 );
         if (actdif2 < diff2) {
            diff2 = actdif2;
            bi2asc[i] = k;
         }
      } 

   } /* endfor */
   getch();
   settextmode ( 3 );
   { 
      tn_file_buf_stream mainstream ("bi2asc.pal",2);
      mainstream.writedata( &bi2asc, 256 );
   }
   return 0;
}
