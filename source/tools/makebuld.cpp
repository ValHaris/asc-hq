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

#define maxint 1147483648



int xdiff = 0, ydiff = 0;

pfont fnt;
void* rast;
void* haken;
void* maske;

   char bilder[4][6];
   tbuildingtype  bld;
 
void* mousepntr;


char getbuildingfield ( int& x, int& y )
{
   mousevisible ( true );
   char ok = 1;
   do {

      int i,j, xp, yp,k;
      if (mouseparams.taste == 1 ) 
        for (i=0; i < 4; i++ ) {
           for (j=0; j < 6 ; j++ ) {
             if ( j & 1)
                xp = i * 40 + 125 ;
             else
                xp = i * 40 + 105 ;
   
            yp = j * 20 + 105 ;
   
            if ( mouseparams.x >= xp && mouseparams.x < xp+30 && mouseparams.y >= yp && mouseparams.y < yp + 30) {
               k=0;
               for (y=0; y<10 ;y++ ) {
                     putpixel ( xp + 10 - y, yp + y , 0 );
                     putpixel ( xp + 20 + y, yp + y , 0 );
                  if (mouseparams.x >  xp + 10 - y   &&   mouseparams.x <= xp + 20 + y   &&   mouseparams.y == yp + y) {
                     k++;
                  }
               } /* endfor */
   
               for (y=10;y<20 ;y++ ) {
                     putpixel ( xp , yp + y , 0 );
                     putpixel ( xp + 30 , yp + y , 0 );
                  if (mouseparams.x >= xp  &&   mouseparams.x <= xp + 30   &&   mouseparams.y == yp + y) {
                     k++;
                  }
               } /* endfor */
   
               for (y=10; y>0 ;y-- ) {
                     putpixel ( xp + 10 - y, yp + 30 - y , 0 );
                     putpixel ( xp + 20 + y, yp + 30 - y , 0);
                  if (mouseparams.x >  xp + 10 - y   &&   mouseparams.x <= xp + 20 + y   &&   mouseparams.y == yp + 30 - y) {
                     k++;
                  }
               } /* endfor */
   
               if ( k ) {
                  ok = 0;
                  x = i;
                  y = j;
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
   void* buf = malloc ( imagesize ( 0, 0, 200, 150 ) );
   setvgapalette256 ( pal );
   getimage ( 0, 0, 200, 150, buf );
   bar(0,0,639,480,255);
   putimage ( 100, 100, buf );

   
   activefontsettings.font = fnt; 
   activefontsettings.color = black; 
   activefontsettings.background = white; 
   activefontsettings.length = 600;
   activefontsettings.justify = lefttext; 
   

   char ch;
   if ( step == 0 )
   do { 
      char strng[40];

      if (! kbhit() ) 
         putspriteimage(100 , 100 , rast);
      ch = getch();
      if (ch == 'K') xdiff--;
      if (ch == 'M') xdiff++;
      if (ch == 'H') ydiff--;
      if (ch == 'P') ydiff++;
      if (! kbhit() ) {
         bar(20,20,400,300,255); 
         putimage(100 - xdiff ,100 - ydiff, buf );
         sprintf ( strng, "x: %d / y: %d" , xdiff, ydiff );
         showtext2( strng, 100,400);
      } 
   }  while ( ch != '\r' );

   bar(20,20,400,300,255); 
   putimage(100 - xdiff ,100 - ydiff, buf );


   activefontsettings.color = blue;

   int  c, x, y;
   if (step == 0) {
      showtext2 ( "which fields are covered by the building ?", 10, 10 );
   
      memset ( bilder, 0, sizeof ( bilder ));
   
      do {
         c = getbuildingfield ( x, y );
         if ( c == 0) {
            bilder[x][y] = 1;
            putimage ( 110 + x * 40 + (y & 1) * 20, 110 + y * 20, haken );
         }
      } while ( c != 13  ); /* enddo */

   }

   bar(00,00,639,480,255);
   putimage(100 - xdiff ,100 - ydiff, buf );

   void* bilder2[4][6];
   char *p1, *p2;

   p2 = (char*) maske;
   p2 += 4;
   int size = imagesize ( 100, 100, 139, 138 );
   for (y = 0; y <= 5; y++)
      for (x = 0; x <= 3; x++)
         if (bilder[x][y] ) {
            bilder2[x][y] = malloc ( size );
            p1 = (char*) bilder2[x][y];
            p1 += 4;

            getimage(100 + 40 * x + (y & 1) * 20, 100 + 20 * y , 139 + 40 * x + (y & 1) * 20, 138 + 20 * y , bilder2[x][y]);
            mount (p1, p2, size - 4 );
         } 
         else bilder2[x][y] = NULL;

   for (y = 0; y <= 5; y++)
      for (x = 0; x <= 3; x++)
         if (bilder2[x][y] ) {
            putspriteimage(250 + 40 * x + (y & 1) * 20, 250 + 20 * y , bilder2[x][y] );
            bld.picture[step][x][y] = bilder2[x][y];
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
         if (bld.picture[0][x][y] ) 
            putspriteimage(100 + 40 * x + (y & 1) * 20, 100 + 20 * y , bld.picture[0][x][y] );
}




main ()
{ 
   t_carefor_containerstream cfcst;


   settxt50mode ();
   int i;

   try {

      {
         tnfilestream mainstream ( "rast.raw", 1);          
         mainstream.readrlepict ( &rast, false, &i );
      }
   
      {
         tnfilestream mainstream ( "allianc2.raw", 1);
         mainstream.readrlepict ( &haken, false, &i );
      }
   
      {
         tnfilestream mainstream ( "mkbld.raw", 1);
         mainstream.readrlepict ( &maske, false, &i );
      }
   
      loadpalette();

      {
         tnfilestream mainstream ("mausi.raw",1); 
         mainstream.readrlepict ( &mousepntr, false, &i );
      }


      {
         tnfilestream stream ( "USABLACK.FNT", 1 );
         fnt = loadfont  ( &stream );
      }
      if ( !fnt ) {
         printf("error loading file USABLACK.FNT \n" );
         return 1;
      }
   
   
      char           pict = YES;       // Bilder einlesen
      char           dif = 1;          // Anzahl der Bilder
      tfile          datfile;
   
      memset ( &bld, 0, sizeof ( bld ));
   
      _settextcolor (7);
      _setbkcolor (0);
      clearscreen ();
   
      datfile.name[0] = 0;
      printf ("\n    create a new building or edit an exisitent one ? \n");
      char   creat_edit = 0;
      yn_switch (" create ", " edit ", 0, 1, creat_edit);
      if (creat_edit==0) {                                            // creat new tank
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
            _wait ();
            nosound ();
            settxt50mode ();
            exit (0);
         } 
   
      } 
      else {                                                         // edit existing tank
   
         clearscreen ();
         fileselect("*.bld", _A_NORMAL, datfile);
         clearscreen ();
         pbuildingtype pbld = loadbuildingtype(datfile.name);
         memcpy ( &bld, pbld, sizeof ( bld ));
         free ( pbld );
   
         printf ("\n    read a new picture ?\n");
         yn_switch ("", "", YES, NO, pict);
         
      } 
   
      int pictnum = 1;
   
      if (pict) {
   
         printf ("\n    number of construction steps\n");
         num_ed ( bld.construction_steps , 1, maxbuildingpicnum );
   
         printf ("\n    number of different pictures (usually 1) ?\n");
         num_ed ( pictnum , 1, maxbuildingpicnum );
   
         int   i ;
         for ( i=0; i < pictnum; i++ ) {
   
            tfile pictfile;
            fileselect ("*.PCX", _A_NORMAL, pictfile);
                                      
            initsvga (0x101);
            initmousehandler( mousepntr );
            mousevisible ( false );
   
            loadpicture ( pictfile.name , i);
            mousevisible(false);
            removemousehandler ();
            settxt50mode ();
         } ;
   
         initsvga (0x101);
         setvgapalette256 ( pal );
   
         initmousehandler( mousepntr );
         mousevisible ( false );
   
         showtext2 ( "position des entrance: ", 10, 10 );
         showbld ();
         getbuildingfield ( bld.entry.x, bld.entry.y );
   
         showtext2 ( "position of powerline connector", 10, 10 );
         showbld ();
         getbuildingfield ( bld.powerlineconnect.x, bld.powerlineconnect.y );
   
         showtext2 ( "position of pipeline connector : ", 10, 10 );
         showbld ();
         getbuildingfield ( bld.pipelineconnect.x, bld.pipelineconnect.y );
   
         mousevisible(false);
         removemousehandler ();
         settxt50mode ();
        
      } else { 
         pictnum = maxbuildingpicnum;
         initsvga (0x103);
         for ( int i=0; i < maxbuildingpicnum ; i++ ) 
            for (int y = 0 ; y < 6 ; y++ ) 
               for (int x = 0; x < 4 ; x++ ) 
                  if (bld.picture[i][x][y])
                     putspriteimage(20 + 40 * x + (y & 1) * 20 + 180 * ( i % 4) , 20 + 20 * y + 400 * ( i / 4) , bld.picture[i][x][y] );
   
         setvgapalette256(pal);
   
         _wait();
   
            settxt50mode ();
         printf ("\n    everything correct ? \n");
         char  c=1;
         yn_switch (" yes ", " no, exit ", 1,0, c);
         if (c==0) exit(0);
      } 
   
                   
   
      {
   
         printf ("\n    id : \n");
         num_ed ( bld.id , 0, 65534);
   
         printf ("\n    name \n");
         stredit ( bld.name, 26, 255,255);
   
         printf ("\n    production costs : \n");
         printf ("\n       material :\n");
         num_ed (bld.produktionskosten.material, 0, 65535);
         printf ("\n       fuel : \n" );
         num_ed (bld.produktionskosten.sprit, 0, 65535);
   
   
         printf ("\n    armor  ");
         num_ed (bld.armor, 0, 65535);
   
         printf ("\n    max size of loadable units \n");
         num_ed (bld.loadcapacity, 0, 65535);
   
         if ( bld.loadcapacity ) {
            printf ("\n    loadable units (height of entering units)\n");
            bitselect (bld.loadcapability, choehenstufen, choehenstufennum);
         } 
   
         printf ("\n    unitheightreq ( only units that are able to reach this height may enter)\n");
         bitselect (bld.unitheightreq, choehenstufen, choehenstufennum);
   
         printf ("\n    Units that may NOT enter\n");
         bitselect (bld.unitheight_forbidden, choehenstufen, choehenstufennum);
   
         printf ("\n    height of the building \n");
         bitselect (bld.buildingheight, choehenstufen, choehenstufennum);
   
         printf ("\n    terrain on which the building can be constructed \n");
         terrainaccess_ed ( bld.terrain_access, "building" );
   
                                                                                     
         printf ("\n    fuel tank    \n");
         num_ed (bld._tank.fuel, 0, maxint );
                                                                       
         printf ("\n    material tank    \n");
         num_ed (bld._tank.material, 0, maxint );
                                                                       
         printf ("\n    energy tank    \n");
         num_ed (bld._tank.energy, 0, maxint );
                                                                       
   
         printf ("\n    fuel maxplus    \n");
         num_ed (bld.maxplus.fuel, 0, maxint );
                                                                       
         printf ("\n    material maxplus    \n");
         num_ed (bld.maxplus.material, 0, maxint );
                                                                       
         printf ("\n    energy maxplus    \n");
         num_ed (bld.maxplus.energy, 0, maxint );
   
   
         printf ("\n    efficiency material    \n");
         num_ed (bld.efficiencymaterial, 0, maxint );
                                                                       
         printf ("\n    efficiency fuel    \n");
         num_ed (bld.efficiencyfuel, 0, maxint );
   
   
         printf ("\n    technology level \n ");
         num_ed (bld.technologylevel, 0, 255);
   
   /*
         printf ("\n    research id \n ");
         num_ed (bld.researchid, 0, 255);
   */
   
         printf ("\n    max research per turn \n ");
         num_ed (bld.maxresearchpoints, 0, 65534 );
   
         clearscreen(); 
   
         printf ("\n    funktions \n");
         bitselect (bld.special, cbuildingfunctions, cbuildingfunctionnum);
   
         if ( bld.special & cgexternalloadingb ) {
            printf ("\n    height of units that can be loaded externally \n");
            bitselect (bld.externalloadheight, choehenstufen, choehenstufennum);
         } else
           bld.externalloadheight = 0;

         printf ("\n    jamming  \n" );
         num_ed (bld.jamming, 0, 255);
   
         printf ("\n    view  \n");
         num_ed (bld.view, 0, 255);
   
      } 
   
   
      {
         tn_file_buf_stream mainstream (datfile.name,2);
         writebuildingtype ( &bld, &mainstream );
      } 
   
      clearscreen(); 

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

