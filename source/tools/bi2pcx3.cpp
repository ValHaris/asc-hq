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


   loadpalette();

   memset ( bipict, 0, sizeof ( 0 ) );
   try {
      {
         for ( int i = 0; i < getterraintranslatenum(); i++ ) {
            const int* p = getterraintranslate(i);
            int n = p[0];
            int t = bipict[n].textnum++;
            sprintf ( bipict [ n ].entry[ t ] .text, "=> %d", p[1] );
            bipict [ n ].entry[ t ].color = 1;
         }
      }
   
      {
         printf("\nLoading terrain:\n");
         tfindfile ff ( "*.trr" );
         char* c = ff.getnextname();
         while ( c ) { 
            if ( verbosity )
               printf("loading %s\n", c ) ;
            else
               printf(".");
            fflush ( stdout );
   
   
            pterraintype bdt = loadterraintype ( c );
            for ( int i = 0; i< cwettertypennum; i++ )
               if ( bdt->weather[i] )
                  for ( int j = 0; j < 6; j++ )
                     if ( bdt->weather[i]->picture[j] ) 
                        if ( bdt->weather[i]->bi_picture[j] >= 0 ) {
                           int n = bdt->weather[i]->bi_picture[j];
                           int t = bipict[n].textnum++;
                           if ( (i == 0)  ||  (bdt->weather[0]->bi_picture[0] < 0) ) {
                              sprintf ( bipict [ n ].entry[ t ] .text, "%s (%d)", c, bdt->id );
                              bipict [ n ].entry[ t ].color = 1;
                           } else {
                              sprintf ( bipict [ n ].entry[ t ] .text, "-> %d ; w=%d", bdt->weather[0]->bi_picture[0],i );
                              bipict [ n ].entry[ t ].color = 1;
                           }
                        }
   
            c = ff.getnextname();
         }
      }
   
      {
         for ( int i = 0; i < getobjectcontainertranslatenum(); i++ ) {
            const int* p = getobjectcontainertranslate(i);
            for ( int j = 1; j < 5; j++ ) 
               if ( p[j] >= 0 ) {
                  int n = p[0];
                  int t = bipict[n].textnum++;
                  sprintf ( bipict [ n ].entry[ t ] .text, "=> %d", p[j] );
                  bipict [ n ].entry[ t ].color = 2;
               }
         }
      }
      {
         printf("\nLoading objects:\n");
         tfindfile ff ( "*.obl" );
         char* c = ff.getnextname();
         while ( c ) { 
            if ( verbosity )
               printf("loading %s\n", c ) ;
            else
               printf(".");
            fflush ( stdout );
   
            Object*type obj = loadobjecttype ( c );
            for ( int w = 0; w < cwettertypennum; w++ )
               if ( (obj->weather & ( 1 << w)) && obj->picture[w] )
                  for ( int i = 0; i< obj->pictnum ; i++ )
                      if ( obj->picture[w][i].bi3pic >= 0 ) {
                         int n = obj->picture[w][i].bi3pic;
                         int t = bipict[n].textnum;
                         bipict[n].textnum += 2;
         
                         sprintf ( bipict [ n ].entry[ t ] .text, "%s (%d)", c, obj->id );
                         sprintf ( bipict [ n ].entry[ t+1 ] .text, "    #%d ", i );
                         if ( obj->picture[w][i].flip & 1 )
                            strcat ( bipict [ n ].entry[ t+1 ] .text, "H" );
                         if ( obj->picture[w][i].flip & 2 )
                            strcat ( bipict [ n ].entry[ t+1 ] .text, "V" );
         
                         bipict [ n ].entry[ t ].color = 2;
                         bipict [ n ].entry[ t+1 ].color = 2;
                      }
   
            c = ff.getnextname();
         }
      }
   
      {
         printf("\nLoading buildings:\n");
         tfindfile ff ( "*.bld" );
         char* c = ff.getnextname();
         while ( c ) { 
            if ( verbosity )
               printf("loading %s\n", c ) ;
            else
               printf(".");
            fflush ( stdout );
   
            BuildingType* bld = loadbuildingtype ( c );
   
            for ( int i = 0; i< cwettertypennum ; i++ )
               for ( int j = 0; j < maxbuildingpicnum; j++ )
                  for ( int k = 0; k < 4; k++ )
                     for ( int l = 0; l < 6; l++ )
                        if ( bld->w_picture[i][j][k][l] )
                           if ( bld->bi_picture[i][j][k][l] >= 0 ) {
                              int n = bld->bi_picture[i][j][k][l];
                              int t = bipict[n].textnum;
                              bipict[n].textnum += 2;
              
                              sprintf ( bipict [ n ].entry[ t ] .text, "%s (%d)", c, bld->id );
                              sprintf ( bipict [ n ].entry[ t+1 ] .text, "    W=%d #%d X=%d Y=%d", i, j, k, l );
              
                              bipict [ n ].entry[ t ].color = 3;
                              bipict [ n ].entry[ t+1 ].color = 3;
                           }
   
            c = ff.getnextname();
         }
      }
   
      {
         printf("\nLoading vehicles:\n");
            tfindfile ff ( "*.veh" );
            char* c = ff.getnextname();
            while ( c ) { 
               if ( verbosity )
                  printf("loading %s\n", c ) ;
               else
                  printf(".");
               fflush ( stdout );
      
               Vehicletype* tnk = loadvehicletype ( c );
      
               if ( tnk->bipicture > 0 ) {
                  int n = tnk->bipicture;
                  int t = bipict[n].textnum;
                  bipict[n].textnum += 1;
       
                  sprintf ( bipict [ n ].entry[ t ] .text, "%s (%d)", c, tnk->id );
       
                  bipict [ n ].entry[ t ].color = 4;
               }
      
               c = ff.getnextname();
            }
         
      }
   
   
   
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
   
   
   //   initsvga(0x101);
   
      int ys;
      int colnum;
      if ( wide ) {
         ys = 1600;
         colnum = 10;
      } else {
         ys = 800;
         colnum = 5;
      }
   
      printf("\nallocating buffer \n" ) ;
      fflush ( stdout );
   
      tvirtualdisplay vdp ( ys, 30000 / ( wide + 1 ), 255 );
   
      int colxl[] = {0, black, blue, green, red };
   
      int x0 = 30;
      int xd = ( agmp->resolutionx - 2 * x0 ) / colnum ;
   
   
      int y0 = 10;
      int yd = fieldsizey/2 + 5;
   
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
            showtext2 ( strrr ( i ), 1, yp + ( fieldsizey/2 - activefontsettings.font->height) / 2 );
   
            if ( verbosity )
               printf ( "line %d\n", i / colnum );
            else
               printf(".");
            fflush ( stdout );
         }
   
         if ( v ) {
            putimage ( x0 + (i % colnum) * xd, yp, v );
            for ( int j = 0; j < bipict[i].textnum; j++ ) {
               activefontsettings.color = colxl[bipict[i].entry[j].color]; 
               showtext2 ( bipict[i].entry[j].text, x0 + (i % colnum) * xd + fieldsizex/2 + 5 , yp + j * (activefontsettings.font->height + 2 ));
   
               if ( lastlinenum < j+1 )
                  lastlinenum = j+1;
            }
   
   
            delete v;
   
         }
      }
   
      printf ( "\nwriting pcx\n", i / colnum );
      char* outputfilename = "bi_graph.pcx";
      writepcx ( outputfilename, 0, 0, agmp->resolutionx-1, yp + fieldsizey/2 + (activefontsettings.font->height + 2 ) * (lastlinenum + 1), pal );
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

   } /* endtry */
   catch ( tfileerror err ) {
      printf("\nfatal error accessing file %s \n", err.filename );
      return 1;
   } /* endcatch */
   catch ( terror ) {
      printf("\na fatal exception occured\n" );
      return 2;
   } /* endcatch */


   return 0;

}
