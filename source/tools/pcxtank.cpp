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
#include <string.h>

#include "../tpascal.inc"
#include "../typen.h"
#include "../basestrm.h"
#include "../misc.h"
#include "../sgstream.h"
#include "../loadpcx.h"
#include "../basegfx.h"
#include "../buildingtype.h"
#include "../vehicletype.h"
#include "../graphicset.h"
#include "../graphicselector.h"
#include "../strtmesg.h"

char header[2][500];
char text[200][500];
char buf[500];

int cat[100][200];

void load_palette ( void )
{
   loadpalette();
   #ifdef _DOS_
   setvgapalette256 ( pal );
   #endif
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


void runWildcard ( const ASCString& st )
{
         tfindfile ff ( st );

         string cn = ff.getnextname();

         while( !cn.empty() ) {

            pvehicletype   ft;
            ft = loadvehicletype( cn.c_str() );

            bar ( 0, 0, 120, 120, 255 );
            putspriteimage(0,0,ft->picture[0]);

            char m[100];
            strcpy ( m, cn.c_str() );
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

         }
}

// including the command line parser, which is generated by genparse
#include "../clparser/tank2pcx.cpp"

int main(int argc, char *argv[] )
{
   Cmdline cl ( argc, argv );

   if ( cl.v() ) {
      cout << argv[0] << " " << getVersionString() << endl;
      exit(0);
   }

   memset ( text, 0, sizeof(text));
   memset ( header, 0, sizeof(header));

   try {
      initFileIO( cl.c().c_str() );
      tvirtualdisplay vd ( 640, 480 );

      load_palette();
      loadbi3graphics();

      if ( cl.next_param()+1 < argc )
         for ( int i = cl.next_param()+1; i < argc; i++ )
            runWildcard ( argv[i] );
      else
        runWildcard ( "*.veh" );

   } /* endtry */
   catch ( tfileerror err  ) {
      printf("error accessing file %s\n", err.getFileName().c_str() );
   } /* endcatch */

   return 0;
}



