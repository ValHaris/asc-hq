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
#include <string.h>
#include <conio.h>

#include "..\tpascal.inc"
#include "..\typen.h"
#include "..\basestrm.h"
#include "..\misc.h"
#include "dos.h"
#include "..\sgstream.h"
#include "..\vesa.h"
#include "..\loadpcx.h"
#include "..\basegfx.h"
#include "..\loadbi3.h"

char header[2][500];
char text[200][500];
char buf[500];

int cat[100][200];

void load_palette ( void )
{
   loadpalette();
   setvgapalette256 ( pal );
}


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

main(int argc, char *argv[] )
{
   memset ( text, 0, sizeof(text));
   memset ( header, 0, sizeof(header));
   int num = 0;

   int quantity=0;   

   char* wildcard;

   if ( argc == 2 ) {
      wildcard = argv[1];
      sprintf( buf, " %s   %c%c%c\n", argv[1], 27,33,5 );
      strcat ( header[0], buf);
   } else {
      wildcard =  "*.veh";
      sprintf( buf, " *.veh %c%c%c\n",27,33,5 );
      strcat ( header[0], buf );
   }
          
   t_carefor_containerstream cfcs;
 

   tfindfile ff ( wildcard );
 
   char* cn = ff.getnextname();
   
   initgraphics ( 640, 480, 8 );
   load_palette();
   loadbi3graphics();
 
   while( cn ) { 

      pvehicletype   ft;
      ft = loadvehicletype( cn );

      bar ( 0, 0, 120, 120, 255 );
      putspriteimage(0,0,ft->picture[0]); 

      char m[100];
      strcpy ( m, cn );
      char* d = m;
      while ( *d != '.' )
         d++;

      strcpy ( d+1, "pcx" );


      int maxx = 100;
      while ( !searchline ( maxx, 0, maxx, 100 ))
         maxx--;
   
   
      int maxy = 100;
      while ( !searchline ( 0, maxy, 100, maxy ))
         maxy--;

      writepcx ( m, 0, 0, maxx+5, maxy+5, pal );
      cn = ff.getnextname();

      num++;
   }

   closegraphics ( );

   return 0;
};



