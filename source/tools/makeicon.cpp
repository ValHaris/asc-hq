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
#include "misc.h"
#include "basegui.h"


void        loadpcx2(char *       filestring, pwterraintype bdt, int i);

      dacpalette256 pal;

const char* direcs[8] = { "oben","rechts oben","rechts","rechts unten","unten","links unten","links","links oben" };



main ()
{ 


   tnguiiconfiledata  data;

   memset ( &data, 0, sizeof ( data ) );

   tfile          datfile;

   char filename[20];
   filename[0] = 0;

   char           pict = YES;       // Bilder einlesen

   settxt50mode ();
   printf ("\n    Icon erstellen oder editieren : \n");
   char   creat_edit = 0;
   yn_switch (" Create ", " Edit ", 0, 1, creat_edit);
   if (creat_edit==0) {                                            // creat new tank
      clearscreen ();
      printf ("\n    Welchen Dateinamen soll die vehicle bekommen ? (ohne Endung)\n");
      printf ("\n    Dateiname des GUIICS \n");
      stredit2 (filename, 8, 255,255);
      strcat ( filename, ".nic" );
  } else {
      clearscreen ();
      fileselect("*.nic", _A_NORMAL, datfile);
      clearscreen ();
      strcpy ( filename, datfile.name );

      tnfilestream mainstream ( datfile.name, 1 );
      mainstream.readdata ( (char*) &data, sizeof ( data ));
      if ( data.infotext )
         mainstream.readpchar ( &data.infotext );
   
      for (int i = 0; i < 8; i++) {
         int  w;
   
         if ( data.picture[i] )
            mainstream.readrlepict ( &data.picture[i], false, &w );
   
         if ( data.picturepressed[i] )
            mainstream.readrlepict ( &data.picturepressed[i], false, &w );
      } 

      printf ("\n    Soll ein neues Bild eingelesen werden \n");
      yn_switch ("", "", YES, NO, pict);
      
   } 



   clearscreen ();

   if ( pict ) {
      int picsize = imagesize ( 0, 0, guiiconsizex, guiiconsizey );
   
      fileselect("gui\\*.pcx", _A_NORMAL, datfile);
   
      char fn[30];
      strcpy ( fn, "gui\\" );
      strcat ( fn, datfile.name );
   
      initsvga ( 0x101 );
      loadpcxxy ( fn, 1, 0, 0 );
      for (int i = 0; i < 8 ; i++ ) {
         int p = getpixel ( 0, i*50 );
         if ( p && p < 255 ) {
            data.picture[i] = malloc ( picsize );
            getimage ( 0, i * 50, guiiconsizex, i * 50 + guiiconsizey, data.picture[i] );
            rectangle( 0, i * 50, guiiconsizex, i * 50 + guiiconsizey, 14 );
         } else
            data.picture[i] = NULL;
   
         p = getpixel ( 100, i*50 );
         if ( p && p < 255 ) {
            data.picturepressed[i] = malloc ( picsize );
            getimage ( 100, i * 50, 100 + guiiconsizex, i * 50 + guiiconsizey, data.picture[i] );
            rectangle( 100, i * 50, 100 + guiiconsizex, i * 50 + guiiconsizey, 14 );
         } else
            data.picturepressed[i] = NULL;
   
      } /* endfor */
      
   
      wait();
      settxt50mode ();
   } else {
      initsvga ( 0x101 );
      for (int i = 0; i < 8 ; i++ ) {
         if ( data.picture[i] )
            putimage ( 20, 20 + 50 * i, data.picture[i] );
         if ( data.picturepressed[i] )
            putimage ( 20, 20 + 50 * i, data.picturepressed[i] );
      }
      wait();
      settxt50mode ();
   }

   printf ("\n    Infotext des GUIICS \n");
   stredit (data.infotext, 70, 255,255);

   printf ("\n    priority \n");
   num_ed (data.priority, 0, 65536 );

   printf (" \n   keys: \n");
   int j = 0;
   while (   data.keys[0][j]  ) 
      printf( "  %c   ", data.keys[0][j++] );

   char keyedit = NO;
   printf ("\n    Sollen die Tasten ge„ndert werden ? \n");
   yn_switch ("", "", YES, NO, keyedit);

   if ( keyedit == YES ) {
      j = 0;
       printf ("\n    keys \n");
       while ( data.keys[0][j] != 'ø' ) {
          data.keys[0][j] = getche ();
          if ( data.keys[0][j] != 'ø' )
             j++;
       }
       data.keys[0][j] = 0;

   }


   if ( data.infotext && data.infotext[0] == 0 )
      data.infotext = NULL;

   char save = YES;
   printf ("\n    abspeichern ? \n");
   yn_switch ("", "", YES, NO, save );
   if ( save ) {
   
      tnfilestream mainstream(filename,2);
   
      mainstream.writedata ( ( char*) &data, sizeof ( data ));
      if ( data.infotext )
         mainstream.writepchar ( data.infotext );
      for (int i=0; i<8 ;i++ ) {
         if ( data.picture[i] )
            mainstream.writerlepict ( data.picture[i] );
         if ( data.picturepressed[i] )
            mainstream.writerlepict ( data.picturepressed[i] );
      }
   
   }
   return 0;
} 

