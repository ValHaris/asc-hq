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

#include "..\tpascal.inc"
#include "..\typen.h"
#include "..\vesa.h"
#include "..\loadpcx.h"
#include "krkr.h"
#include "..\sgstream.h"
#include "..\newfont.h"
#include "..\mousehnd.h"
#include "..\misc.h"
#include "..\keybp.h"
#include "..\basegfx.h"

#ifdef HEXAGON
#include "..\loadbi3.h"
#endif


int xdiff = 0, ydiff = 0;

pfont fnt;
void* rast;
void* rast2;
void* haken;

   char bilder[4][6];
   pbuildingtype  bld;
 
void* mousepntr;

      int weather = 0;

char getbuildingfield ( int& x, int& y )
{
   void* fieldshape;
   {
      int w;
      tnfilestream stream ("hexfeld.raw", 1);
      stream.readrlepict ( &fieldshape, false, &w );
   }


   void* kontur;
   {
      int w;
      tnfilestream stream ("hexcont.raw", 1);
      stream.readrlepict ( &kontur, false, &w );
   }

   mousevisible ( true );
   char ok = 1;
   do {

      int i,j, xp, yp,k;
      if (mouseparams.taste == 1 ) 
        for (i=0; i < 4; i++ ) {
           for (j=0; j < 6 ; j++ ) {
             if ( j & 1)
                xp = i * fielddistx + 100 + fielddisthalfx;
             else
                xp = i * fielddistx + 100 ;
   
            yp = 100 + j * fielddisty ;
   
            if ( mouseparams.x >= xp && mouseparams.x < xp+fieldxsize && mouseparams.y >= yp && mouseparams.y < yp + fieldysize) {
   
               int xd = mouseparams.x - xp;
               int yd = mouseparams.y - yp;
               k = 0;
              
               int px = getpixelfromimage ( fieldshape, xd, yd );
               if (  px >= 0 && px < 255 )
                  k++;
              
              /*
               if ( pw[0] >= xd  && pw[1] >= yd )
                  for ( int xxx = 0; xxx <= pw[0]; xxx++ )
                     for ( int yyy = 0; yyy <= pw[1]; yyy++ )
                        if ( pc[ yyy * ( pw[0] + 1) + xxx] != 255 )
                           putpixel ( xp + xxx, yp + yyy, i * j * 16 );
              
               */
   
               if ( k ) {
                  ok = 0;
                  x = i;
                  y = j;
                  putspriteimage ( xp, yp, kontur );
               } /* endif */
   
            } /* endif */
   
            
         } /* endfor */
      } /* endfor */

      if ( kbhit () )
         ok = 0;

   } while ( ok ); /* enddo */

   char c = 0;
   while ( kbhit() ) {
      c = getch ();
   }
  while ( mouseparams.taste );

  mousevisible(false);
   return c;
}


char getbuildingfield ( shortint& x, shortint& y )
{
   int x1 = x;
   int y1 = y;
   char c = getbuildingfield ( x1, y1 );
   x = x1;
   y = y1;
   return c;
}


void mount ( char* p1, char* p2, int size )
{
   for (int i = 0; i < size ;i++, p1++, p2++ ) {
      if (*p2 == 255)
         *p1 = 255;
   } /* endfor */
}


void loadpicture ( char* filename , int step ) {
   bar(0,0,639,480,255);
   putpixel ( 0,0,255);
   loadpcxxy ( filename, 0, 0, 0);
   void* buf = malloc ( imagesize ( 0, 0, 400, 350 ) );
   setvgapalette256 ( pal );
   getimage ( 0, 0, 400, 350, buf );
   bar(0,0,639,480,255);
   putimage ( 100, 100, buf );

   
   activefontsettings.font = fnt; 
   activefontsettings.color = black; 
   activefontsettings.background = white; 
   activefontsettings.length = 600;
   activefontsettings.justify = lefttext; 
   

   char ch;
   if ( step == 0 ) {
      showtext2 ( "move building into the upper left fields ", 10, 10 );
      showtext2 ( "and press enter", 20, 40 );
      do { 
         char strng[40];
   
         if (! kbhit() ) 
            putspriteimage(100 , 100 , rast2);
         ch = getch();
         if (ch == 'K') xdiff++;
         if (ch == 'M') xdiff--;
         if (ch == 'H') ydiff++;
         if (ch == 'P') ydiff--;
         if (! kbhit() ) {
            bar(20,70,400,300,255); 
            putimage(100 - xdiff ,100 - ydiff, buf );
            sprintf ( strng, "x: %d / y: %d" , xdiff, ydiff );
            showtext2( strng, 100,400);
         } 
      }  while ( ch != '\r' );
   }

   bar(20,20,400,300,255); 
   putimage(100 - xdiff ,100 - ydiff, buf );


   activefontsettings.color = blue;

   int  c, x, y;
   if (step == 0) {
      showtext2 ( "which fields are covered by the building ?", 10, 10 );
      showtext2 ( "press enter when finished", 20, 40 );
   
      memset ( bilder, 0, sizeof ( bilder ));
   
      do {
         c = getbuildingfield ( x, y );
         if ( c == 0) {
            bilder[x][y] = 1;
            putimage ( 110 + x * fielddistx + (y & 1) * fielddisthalfx, 110 + y * fielddisty, haken );
         }
      } while ( c != 13  ); /* enddo */

   }

   bar(00,00,639,480,255);
   putimage(100 - xdiff ,100 - ydiff, buf );

   void* bilder2[4][6];
   char *p1, *p2;

   void* maske;
   {
      int w;
      tnfilestream stream ("hexfeld.raw", 1);
      stream.readrlepict ( &maske, false, &w );
   }


   p2 = (char*) maske;
   p2 += 4;
   int size = imagesize ( 0, 0, fieldsizex-1, fieldsizey-1 );
   for (y = 0; y <= 5; y++)
      for (x = 0; x <= 3; x++)
         if (bilder[x][y] ) {
            bilder2[x][y] = malloc ( size );
            p1 = (char*) bilder2[x][y];
            p1 += 4;

            getimage(100 + fielddistx * x + (y & 1) * fielddisthalfx             , 100 + fielddisty * y , 
                      99 + fielddistx * x + (y & 1) * fielddisthalfx + fieldsizex,  99 + fielddisty * y + fieldsizey, bilder2[x][y]);
            mount (p1, p2, size - 4 );
         } 
         else bilder2[x][y] = NULL;

   for (y = 0; y <= 5; y++)
      for (x = 0; x <= 3; x++)
         if (bilder2[x][y] ) {
            putspriteimage(250 + fielddistx * x + (y & 1) * fielddisthalfx, 250 + fielddisty * y , bilder2[x][y] );
            bld->w_picture[weather][step][x][y] = bilder2[x][y];
            bld->bi_picture[weather][step][x][y] = -1;
            if ( step == 0 ) 
               getch();
         } 
   if ( step != 0 ) 
      getch();
}


void showbld ( void )
{
   bar ( 0, 40, 639, 480, 255 );
  
   for (int y = 0; y <= 5; y++)
      for (int x = 0; x <= 3; x++)
         if (bld->w_picture[weather][0][x][y] ) 
            putspriteimage ( 100 + fielddistx * x + fielddisthalfx * ( y & 1), 100 + fielddisty * ( y ), bld->w_picture[weather][0][x][y] );
}




main ()
{ 
   t_carefor_containerstream cfcst;


   settxt50mode ();
   int i;

   try {

      {
         tnfilestream mainstream ( "hexrast.raw", 1);          
         mainstream.readrlepict ( &rast, false, &i );
      }

      {
         tnfilestream mainstream ( "hxraster.raw", 1);          
         mainstream.readrlepict ( &rast2, false, &i );
      }
   
      {
         tnfilestream mainstream ( "allianc2.raw", 1);
         mainstream.readrlepict ( &haken, false, &i );
      }
   
      {
         tnfilestream mainstream ("mausi.raw",1); 
         mainstream.readrlepict ( &mousepntr, false, &i );
      }

      loadpalette();
      loadbi3graphics();

      {
         tnfilestream stream ( "USABLACK.FNT", 1 );
         fnt = loadfont  ( &stream );
      }
      if ( !fnt ) {
         printf("error loading file USABLACK.FNT \n" );
         return 1;
      }
   
   
      activefontsettings.font = fnt; 
      activefontsettings.color = black; 
      activefontsettings.background = white; 
      activefontsettings.justify = lefttext; 
      activefontsettings.length = 600;
   
   
      char           pict = YES;       // Bilder einlesen
      char           dif = 1;          // Anzahl der Bilder
      tfile          datfile;
   
      _settextcolor (7);
      _setbkcolor (0);
      clearscreen ();
   
      datfile.name[0] = 0;
      printf ("\n    create a new building or edit an exisitent one ? \n");
      char   creat_edit = 0;
      yn_switch (" create ", " edit ", 0, 1, creat_edit);
      if (creat_edit==0) {                                            // creat new tank
         bld = new tbuildingtype;
         memset ( bld, 0, sizeof ( *bld ));
         bld->terrain_access = &bld->terrainaccess;
         clearscreen ();
         printf ("\n    enter filename (without extension)\n");
         stredit2 (datfile.name, 9, 255,255);
         strcat (datfile.name, ".bld");
   
         if (exist(datfile.name)) {
            sound (800);
            clearscreen(); 
            _settextcolor (15);
            _setbkcolor (red);
            _settextposition (4, 5);
            char s[100];
            sprintf ( s, "  filename <%s> already exists !! \n", datfile.name );
            _outtext ( s );
            wait ();
            nosound ();
            settxt50mode ();
            exit (0);
         } 
   
      } 
      else {                                                         // edit existing building
   
         clearscreen ();
         fileselect("*.bld", _A_NORMAL, datfile);
         clearscreen ();
         bld = loadbuildingtype(datfile.name);
   
         printf ("\n    read a new picture ?\n");
         yn_switch ("", "", YES, NO, pict);
         
      } 
   
      int pictnum = 1;
      char battleisle = 1;
   
      if (pict) {
   
         int wt = 1;
         for (i = 0; i < cwettertypennum ;i++ ) {
            int f = 0;
            for (int x = 0; x < 4; x++) 
               for (int y = 0; y < 6; y++) 
                  if ( bld->w_picture[i][0][x][y] )
                     f++;
   
            if ( f )
               wt |= ( 1 << i );
         }
   
         printf ("\n    number of construction steps\n");
         num_ed ( bld->construction_steps , 1, maxbuildingpicnum );
   
         printf ("\n    number of different pictures (usually 1 * construction steps ) ?\n");
         num_ed ( pictnum , bld->construction_steps, maxbuildingpicnum );
   
         printf ("\n    for which weather should the building be available \n");
         bitselect (wt, cwettertypen, cwettertypennum);
   
         memset ( bld->bi_picture, 0, sizeof ( bld->bi_picture ));
         memset ( bld->w_picture, 0, sizeof ( bld->w_picture ));

/*
         for ( int pic = 0; pic < pictnum; pic++ ) {
            printf("\n\n picture # %d \n", pic );
*/

            for (weather = 0; weather< cwettertypennum ;weather++ ) 
               if (wt & ( 1 << weather )) {
                 printf ("\n\n weather: %s\n", cwettertypen[weather] );
                 printf ("\n    use pictures of Battle Isle or own picture ?\n");
                 yn_switch ("BI", "seperate picture" , 1, 0, battleisle);
           
                 if ( battleisle ) {
                    int   i ;
                    initgraphics ( 640, 480, 8 );
                    setvgapalette256 ( pal );
                    for ( i=0; i < pictnum; i++ ) {
                       int end = 0;
                       do {
                          bar ( 0, 0, 639, 479, 0 );
                          putimage ( 0, 0, rast );
      
                          showtext2 ( "select field ", 10, 210 );
      
                          initmousehandler( mousepntr );
                          mousevisible ( true );
                          int col = -1;
                          do {
                              if ( mouseparams.taste == 1 ) {
                                 mousevisible ( false );
                                 col = getpixel ( mouseparams.x, mouseparams.y );
                              }
                 
                          } while ( col == -1 ); /* enddo */
                          bar ( 0, 0, 639, 479, 0 );
                          getbi3pict_double ( &bld->bi_picture[weather][i][col % 8][col / 8], &bld->w_picture[weather][i][col % 8][col / 8] ); 
                          bar ( 0, 0, 639, 479, 0 );
                          showbld();
                          showtext2 ( "more pictures ?", 10, 10 );
                          switch ( r_key() ) {
                            case ct_n:
                            case ct_esc: end = 1;
                          } /* endswitch */
                      } while ( !end );
                    }
           
                    closegraphics();
                    settxt50mode ();
           
                 } else {
                    int   i ;
                    for ( i=0; i < pictnum; i++ ) {
              
                       tfile pictfile;
                       fileselect ("*.PCX", _A_NORMAL, pictfile);
                                                 
                       initgraphics ( 640, 480, 8 );
                       initmousehandler( mousepntr );
                       mousevisible ( false );
              
                       loadpicture ( pictfile.name , i);
                       mousevisible(false);
                       removemousehandler ();
                       settxt50mode ();
                    } 
                 }
               }
   
         initgraphics ( 640, 480, 8 );
         setvgapalette256 ( pal );
   
         weather = 0;
   
         initmousehandler( mousepntr );
         mousevisible ( false );
   
         showtext2 ( "position of entrance: ", 10, 10 );
         showbld ();
         getbuildingfield ( bld->entry.x, bld->entry.y );
   
       /*
         showtext2 ( "position of powerline connector", 10, 10 );
         showbld ();
         getbuildingfield ( bld->powerlineconnect.x, bld->powerlineconnect.y );
   
         showtext2 ( "position of pipeline connector : ", 10, 10 );
         showbld ();
         getbuildingfield ( bld->pipelineconnect.x, bld->pipelineconnect.y );
       */

         mousevisible(false);
         removemousehandler ();
         settxt50mode ();
        
      } else { 
         pictnum = maxbuildingpicnum;
         initgraphics ( 800, 600, 8 );
         showbld ();
         setvgapalette256(pal);
   
         wait();
   
         settxt50mode ();
         printf ("\n    everything correct ? \n");
         char  c=1;
         yn_switch (" yes, continue ", " no, exit ", 1,0, c);
         if (c==0) exit(0);
      } 
   
                   
   
      {
   
         printf ("\n    id : \n");
         num_ed ( bld->id , 0, 65534);
   
         printf ("\n    name \n");
         stredit ( bld->name, 26, 255,255);
   
         printf ("\n    production costs : \n");
         printf ("\n       material :\n");
         num_ed (bld->produktionskosten.material, 0, 65535);
         printf ("\n       fuel : \n" );
         num_ed (bld->produktionskosten.sprit, 0, 65535);
   
   
         printf ("\n    armor  ");
         num_ed (bld->armor, 0, 65535);
   
         printf ("\n    max size of loadable units \n");
         num_ed (bld->loadcapacity, 0, 65535);
   
         if ( bld->loadcapacity ) {
            int lc = bld->loadcapability;
            printf ("\n    loadable units (height of entering units)\n");
            bitselect (lc , choehenstufen, choehenstufennum);
            bld->loadcapability = lc;
         } 
   
         int a = bld->unitheightreq;
         printf ("\n    unitheightreq ( only units that are able to reach this height may enter)\n");
         bitselect (a, choehenstufen, choehenstufennum);
         bld->unitheightreq = a;
   
         printf ("\n    Units that may NOT enter\n");
         bitselect (bld->unitheight_forbidden, choehenstufen, choehenstufennum);
   
         printf ("\n    height of the building \n");
         bitselect (bld->buildingheight, choehenstufen, choehenstufennum);
   
   
         printf ("\n    terrain on which the building can be constructed \n");
         terrainaccess_ed ( bld->terrain_access, "building" );
   
   
                                                                                     
         printf ("\n  ASC resource mode - fuel tank    \n");
         num_ed (bld->_tank.a.fuel, 0, maxint );
                                                                       
         printf ("\n  ASC resource mode - material tank    \n");
         num_ed (bld->_tank.a.material, 0, maxint );
                                                                       
         printf ("\n  ASC resource mode - energy tank    \n");
         num_ed (bld->_tank.a.energy, 0, maxint );
                                                                       
   
         printf ("\n  BI resource mode - fuel tank    \n");
         num_ed (bld->_bi_maxstorage.a.fuel, 0, maxint );
                                                                       
         printf ("\n  BI resource mode - material tank    \n");
         num_ed (bld->_bi_maxstorage.a.material, 0, maxint );
                                                                       
         printf ("\n  BI resource mode - energy tank    \n");
         num_ed (bld->_bi_maxstorage.a.energy, 0, maxint );
                                                                       
   
         printf ("\n    fuel maxplus    \n");
         num_ed (bld->maxplus.a.fuel, minint, maxint );
                                                                       
         printf ("\n    material maxplus    \n");
         num_ed (bld->maxplus.a.material, minint, maxint );
                                                                       
         printf ("\n    energy maxplus    \n");
         num_ed (bld->maxplus.a.energy, minint, maxint );
   
   
         printf ("\n    efficiency material    \n");
         num_ed (bld->efficiencymaterial, 0, maxint );
                                                                       
         printf ("\n    efficiency fuel    \n");
         num_ed (bld->efficiencyfuel, 0, maxint );
   
   
         a = bld->technologylevel;
         printf ("\n    technology level \n ");
         num_ed (a, 0, 255);
         bld->technologylevel = a;
   
   /*
         printf ("\n    research id \n ");
         num_ed (bld->researchid, 0, 255);
   */
   
         printf ("\n    max research per turn \n ");
         num_ed (bld->maxresearchpoints, 0, 65534 );
   
         clearscreen(); 
   
         printf ("\n    funktions \n");
         bitselect (bld->special, cbuildingfunctions, cbuildingfunctionnum);
   
         if ( bld->special & cgexternalloadingb ) {
            printf ("\n    height of units that can be loaded externally \n");
            bitselect (bld->externalloadheight, choehenstufen, choehenstufennum);
         } else
           bld->externalloadheight = 0;

         printf ("\n    jamming  \n" );
         num_ed (bld->jamming, 0, 255);
   
         printf ("\n    view  \n");
         num_ed (bld->view, 0, 255);
   
      } 
   
   
   
      {
         tn_file_buf_stream mainstream (datfile.name,2);
         writebuildingtype ( bld, &mainstream  );
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


   clearscreen(); 

   return 0;
}

