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


#include "..\loadbi3.h"
#include "string.h"
#include "..\vesa.h"
#include "..\keybp.h"
#include "..\loadpcx.h"
#include "..\newfont.h"
#include "..\misc.h"

int background = 255;

struct tbipictparam {
   int textnum;
   struct {
     char text[40];
     int color;
   } entry[20];
} bipict[4000];


int main(int argc, char *argv[] )
{
   keeporiginalpalette = 1;

   t_carefor_containerstream cfcst;


  int index = 0;
  int wide = 0;
  int verbosity = 0;

   for (int i = 1; i<argc; i++ ) {
      if ( argv[i][0] == '/'  ||  argv[i][0] == '-' ) {
         if ( strcmpi ( &argv[i][1], "WIDE" ) == 0 ) 
            wide = 1;
         else 
            if ( strcmpi ( &argv[i][1], "INDEX" ) == 0 )
               index = 1;
            else
               if ( strcmpi ( &argv[i][1], "verbosity" ) == 0 )
                  verbosity = 1;
               else
                 if ( ( strcmpi ( &argv[i][1], "?" ) == 0 ) || ( strcmpi ( &argv[i][1], "h" ) == 0 ) ){
                    printf( " Parameters: \n"
                            "     /h          this page\n"
                            "     /index      output index\n"
                            "     /verbosity  increase verbosity level\n"
                            "     /wide       use wide output ( 10 columns ) \n\n"     );
                    return (0);
   
                 } else {
                     printf ( "\nInvalid command line parameter: %s \n", argv[i] );
                     return (1);
                 }
       } else {
           printf ( "\nInvalid command line parameter: %s \n", argv[i] );
           return (1);
       }
   } /* endfor */

   initsvga(0x101);
   loadpcxxy ("bi2\\dump0000.pcx", 1, 0, 0 );
   closesvga();
   settextmode(3);

      int ys;
      int colnum;
      if ( wide ) {
         ys = 1600;
         colnum = 40;
      } else {
         ys = 400;
         colnum = 10;
      }
   
      printf("\nallocating buffer \n" ) ;
      fflush ( stdout );
   
      tvirtualdisplay vdp ( ys, 30000 / ( wide + 1 ), 0 );
   
      int colxl[] = {0, black, blue, green, red };
   
      int x0 = 30;
      int xd = ( agmp->resolutionx - 2 * x0 ) / colnum ;
   
   
      int y0 = 10;
      int yd = fieldsizey/2 + 5;
   
      char* name = "monogui.fnt";
      pfont fnt;
      {
         tnfilestream stream ( name, 1 );
         fnt = loadfont  ( &stream );
      }
      if ( !fnt ) {
         printf("error loading file %s \n", name );
         return 1;
      }


      activefontsettings.font = fnt; 
      activefontsettings.color = black; 
      activefontsettings.background = 255; 
      activefontsettings.length = xd;
      activefontsettings.justify = lefttext; 
      activefontsettings.height = 0; 
   
   
      int yp = y0;
      int lastlinenum = 0;
   
      if ( wide )
         line ( agmp->resolutionx / 2 - 3, 0, agmp->resolutionx / 2 - 3, agmp->resolutiony -1, black );
   
      loadbi3graphics();

      printf("Generating image: \n" ) ;
      fflush ( stdout );
   
      for ( i = 0; i < bi3graphnum; i++ ) {
         void* v;
         loadbi3pict ( i, &v );
         if ( !(i % colnum) ) {
            if ( i ) {
              if ( yd > (activefontsettings.font->height + 2 )* lastlinenum ) 
                 yp += yd;
              else
                 yp += (activefontsettings.font->height + 2 ) * (lastlinenum + 1);
            }
            lastlinenum = 0;
            activefontsettings.color = black; 
            activefontsettings.justify = lefttext;
            showtext2 ( strrr ( i ), 1, yp + ( fieldsizey/2 - activefontsettings.font->height) / 2 );
   
            if ( verbosity )
               printf ( "line %d\n", i / colnum );
            else
               printf(".");
            fflush ( stdout );
         }
   
         if ( v ) {
            activefontsettings.color = white;
            activefontsettings.justify = centertext;
            activefontsettings.length = fieldsizex/2;

            showtext2 ( strrr ( i ), x0 + (i % colnum) * xd - 1, yp + ( fieldsizey/2 - activefontsettings.font->height) / 2 );

         }
      }
   
      printf ( "\nwriting pcx\n", i / colnum );
      char* outputfilename = "biograph.pcx";
      writepcx ( outputfilename, 0, 0, agmp->resolutionx-1, yp + fieldsizey/2 + (activefontsettings.font->height + 2 ) * (lastlinenum + 1), *activepalette256 );
      printf ( "%s written\n", outputfilename );
   
      if ( index ) {
         tnfilestream strm ( "bi_index.raw", 1 );
         int w;
         void* p;
         strm.readrlepict ( &p, false, &w );
         putimage ( 0, 0, p );
         char* outputfilename2 = "INDEX.PCX";
         writepcx ( outputfilename2, 0, 0, 639, 479, pal );
         printf ( "\nIndex file %s written\n", outputfilename2 );
      }

   return 0;

}
