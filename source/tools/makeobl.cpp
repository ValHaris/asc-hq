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

#include "..\tpascal.inc"
#include "..\typen.h"
#include "..\newfont.h"
#include "..\vesa.h"
#include "..\loadpcx.h"
#include "krkr.h"
#include "..\sgstream.h"
#include "..\misc.h"
#include "..\keybp.h"
#include "..\loadbi3.h"


char     mode50 = 1;
#define  la   80

int offset = 1560;

pfont fnt;


const char* bildnr[8] = { " 0   ( object facing up ) ",
                          " 1   ( object facing up and right ) ",
                          " 2   ( object facing right ) ",
                          " 3   ( object facing right and down ) ",
                          " 4   ( object facing down ) ",
                          " 5   ( object facing down and left ) ",
                          " 6   ( object facing left ) ",
                          " 7   ( object facing left and up ) " };



void *       loadpcx2(char *       filestring);



char bip[24] = { 0,1,2,3,5,7,9,11,15,18,19,20,21,22,23,27,28,29,30,31,54,55,63,6};



main (int argc, char *argv[] )
{ 
   t_carefor_containerstream cfcst;

   try {
      pobjecttype ft;
      tfile          datfile;
      char           pict = YES;       // Bilder einlesen
      char           dif = 1;          // Anzahl der Bilder
   
      char           mehrfielderschuetze = 0;
      char           maxmovement = 0;
    
      ft = new tobjecttype;
      memset ( ft, 0, sizeof (tobjecttype) ) ;
   
      strcpy (datfile.name, "");
   
      {
         tnfilestream stream ( "USABLACK.FNT", 1 );
         fnt = loadfont  ( &stream );
      }
      activefontsettings.font = fnt; 
      activefontsettings.color = black; 
      activefontsettings.background = white; 
      activefontsettings.length = 600;
      activefontsettings.justify = lefttext; 
   
      if (mode50) settxt50mode ();
      _settextcolor (7);
      _setbkcolor (0);
      clearscreen ();
      char   creat_edit ;
      if ( argc < 2 ) {
         printf ("\n    create a new object or edit an existent one ? \n");
         creat_edit = 0;
         yn_switch (" create ", " edit ", 0, 1, creat_edit);
      } else
         creat_edit = 1;
   
      if (creat_edit==0) {                                            // creat new object
         clearscreen ();
         printf ("\n    enter filename (without extension)\n");
         stredit2 (datfile.name, 9, 255,255);
         strcat (datfile.name, ".obl");
   
         ft->terrain_and = -1;
         ft->pictnum = 1;
   
         if (exist(datfile.name)) {
            sound (800);
            clearscreen(); 
            _settextcolor (15);
            _setbkcolor (red);
            _settextposition (4, 5);
            char s[100];
            sprintf ( s, " Filename <%s> already exists !! \n", datfile.name );
            _outtext ( s );
            _wait ();
            nosound ();
            settextmode ( 3 );
            exit (0);
         } 
   
      } 
      else {                                                         // edit existing obl
   
         clearscreen ();
         if ( argc < 2 ) 
            fileselect("*.obl", _A_NORMAL, datfile);
         else
            strcpy ( datfile.name, argv[1] );
   
         clearscreen ();
         ft = loadobjecttype(datfile.name);
   
         printf ("\n    change picture ? \n");
         yn_switch ("", "", YES, NO, pict);
         
      } 
   
   
   
      if (pict) {
         tfile    pictfile;
        
         fileselect ("*.PCX", _A_NORMAL, pictfile);
                                   
         initsvga (0x101);
         ft->picture[0] = loadpcx2(pictfile.name);
        
            /*
   
         ft->pictnum = 64;
         ft->picture = new thexpic[64];
   
         initsvga (0x101);
         loadpalette();
         setvgapalette256 ( pal );
         
         tkey ch;
         for ( int i = 0; i < 64; i++ ) 
            do {
               ft->picture[i].flip = 0;
   
               bar ( 0, 0, 639, 479, 0 );
   
               void* pic;
               int num;
               getbi3pict_double ( &num, &pic );
   
               do {
                  bar ( 0, 0, 639, 479, 255 );
                  for ( int j = 0; j < 6; j++ ) {
                     int col;
                     if ( i & ( 1 << j ))
                        col = lightred;
                     else
                        col = red;
                     bar ( 50 + j * 20, 400, 68 + j * 20, 420, col );
                  }
   
                  putspriteimage ( 100, 100, pic );
   
                  ch = r_key();
   
                  if ( ch == ct_x ) {
                     void* buf = new char[ imagesize ( 0, 0, fieldxsize, fieldysize ) ];
                     flippict ( pic, buf , 1 );
                     pic = buf;
                     ft->picture[i].flip ^= 1;
                  }
      
                  if ( ch == ct_y ) {
                     void* buf = new char[ imagesize ( 0, 0, fieldxsize, fieldysize ) ];
                     flippict ( pic, buf , 2 );
                     pic = buf;
                     ft->picture[i].flip ^= 2;
                  }
   
               } while ( ch != ct_enter &&  ch  != ct_esc ); 
               ft->picture[i].picture = pic;
               ft->picture[i].bi3pic = num;
   
            } while ( ch != ct_enter ); 
            */
            /*
            {
               int found = 0;
               int k = i;
               int flip = 0;
               do {
                  for ( int j = 0; j < 24; j++ ) 
                     if ( bip[j] == k ) {
                       loadbi3pict_double ( offset+j, &ft->picture[i].picture );
                       ft->picture[i].bi3pic = offset+j;
                       ft->picture[i].flip = flip;
                       found = 1;
                     }
                  if ( !found  && flip == 1) {
                     int l = 0;
                     k = i;
                     if ( k & 8 ) l |= 1;
                     if ( k & 4 ) l |= 2;
                     if ( k & 2 ) l |= 4;
                     if ( k & 1 ) l |= 8;
                     if ( k & 32 ) l |= 16;
                     if ( k & 16 ) l |= 32;
                     flip <<= 1;
                     k = l;
                  } else
                  if ( !found ) {
                     int l = 0;
                     if ( k & 1 ) l |= 1;
                     if ( k & 32 ) l |= 2;
                     if ( k & 16 ) l |= 4;
                     if ( k & 8 ) l |= 8;
                     if ( k & 4 ) l |= 16;
                     if ( k & 2 ) l |= 32;
                     flip |= 1;
                     k = l;
                  }
               } while ( !found ); 
   
               loadbi3pict_double ( ft->picture[i].bi3pic, &ft->picture[i].picture );
   
               void* buf = new char[ imagesize ( 0, 0, fieldxsize, fieldysize ) ];
   
               if ( ft->picture[i].flip == 1 ) {
                  flippict ( ft->picture[i].picture, buf , 1 );
                  delete ft->picture[i].picture;
                  ft->picture[i].picture = buf;
               }
   
               if ( ft->picture[i].flip == 2 ) {
                  flippict ( ft->picture[i].picture, buf , 2 );
                  delete ft->picture[i].picture;
                  ft->picture[i].picture = buf;
               }
   
               if ( ft->picture[i].flip == 3 ) {
                  flippict ( ft->picture[i].picture, buf , 2 );
                  flippict ( buf, ft->picture[i].picture, 1 );
                  delete buf;
               }
   
               bar ( 0, 0, 639, 479, 255 );
               putimage ( 100, 100, ft->picture[i].picture );
               showtext2 ( strr ( i ), 10, 10 );
               _wait();
   
            }
            */
   
   
      } else { 
         initsvga (0x101);
   
         putspriteimage( 50, 50, ft->picture[0] ); 
   
         {
            tnfilestream mainstream ( "palette.pal" , 1);
            mainstream.readdata( (char*) pal, sizeof(pal)); 
         }
         setvgapalette256(pal);
   
         _wait();
   
      } 
   
      settxt50mode();
   
      printf ("\n    ID : \n");
      num_ed ( ft->id , 0, maxint);
   
      printf ("\n    terrain AND : \n");
      bitselect ( ft->terrain_and, cbodenarten, cbodenartennum);
   
      printf ("\n    terrain OR : \n");
      bitselect ( ft->terrain_or , cbodenarten, cbodenartennum);
   
      printf ("\n    attackbonus_plus : \n");
      num_ed ( ft->attackbonus_plus , minint, maxint);
   
      printf ("\n    attackbonus_abs : \n");
      num_ed ( ft->attackbonus_abs , minint, maxint);
   
      printf ("\n    defensebonus_plus : \n");
      num_ed ( ft->defensebonus_plus , minint, maxint);
   
      printf ("\n    defensebonus_abs : \n");
      num_ed ( ft->defensebonus_abs , minint, maxint);
   
      printf ("\n    basicjamming_plus : \n");
      num_ed ( ft->basicjamming_plus , minint, maxint);
   
      printf ("\n    basicjamming_abs : \n");
      num_ed ( ft->basicjamming_abs , minint, maxint);
   
      printf ("\n    armor : \n");
      num_ed ( ft->armor , 0, maxint);
   
      printf ("\n    height : \n");
      num_ed ( ft->height , 0, maxint);
   
      printf ("\n    production cost material : \n");
      num_ed ( ft->buildcost.material , 0, maxint);
   
      printf ("\n    production cost fuel : \n");
      num_ed ( ft->buildcost.fuel, 0, maxint);
   
     #ifdef HEXAGON
      printf ("\n    production cost movement : \n");
      num_ed ( ft->build_movecost, 0, maxint);
     #else
      printf ("\n    production cost movement : \n");
      num_ed ( ft->movecost, 0, maxint);
     #endif
   
      printf ("\n    removal cost material : \n");
      num_ed ( ft->removecost.material , 0, maxint);
   
      printf ("\n    removal cost  fuel : \n");
      num_ed ( ft->removecost.fuel, 0, maxint);
   
     #ifdef HEXAGON
      printf ("\n    removal cost  movement : \n");
      num_ed ( ft->remove_movecost, 0, maxint);
     #endif
   
      printf ("\n    name of object \n");
      stredit (ft->name, 21, 255,255);
   
      {
         printf ("\n    link automatically with neighbouring fields? \n");
         char c = ft->no_autonet;
         yn_switch (" yes ", " no ", 0, 1, c );
         ft->no_autonet = c;
      }
   
      ft->buildcost.energy = 0;
      ft->removecost.energy = 0;
   
     {
                for (int j=0; j< cmovemalitypenum; j ++)
                   printf(" %i : %s \n", j, cmovemalitypes[j] );
   
                printf ("\n  movemalus_plus_count" );
                num_ed (ft->movemalus_plus_count, 0, cmovemalitypenum );
   
                ft->movemalus_plus = (char*) realloc ( ft->movemalus_plus, ft->movemalus_plus_count );
   
                printf ( " 0 for default \n");
                for (j=0; j< ft->movemalus_plus_count; j++) {
                   printf(" %i : %s \n", j, cmovemalitypes[j] );
                   
                   num_ed ( ft->movemalus_plus[j], 0, 255 );
                } /* endfor */
   
     }{
                for (int j=0; j< cmovemalitypenum; j ++)
                   printf(" %i : %s \n", j, cmovemalitypes[j] );
   
                printf ("\n  movemalus_abs_count  " );
                num_ed (ft->movemalus_abs_count, 0, cmovemalitypenum );
   
                ft->movemalus_abs = (char*) realloc ( ft->movemalus_abs, ft->movemalus_abs_count );
   
                printf ( " 0 for default \n");
                for (j=0; j< ft->movemalus_abs_count; j++) {
                   printf(" %i : %s \n", j, cmovemalitypes[j] );
                   
                   num_ed ( ft->movemalus_abs[j], 0, 255 );
                } /* endfor */
      }
   
      {
          tn_file_buf_stream mainstream (datfile.name,2);
          writeobject ( ft, &mainstream, 0 );
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

   if (mode50) _setvideomode (_TEXTC80);
   return 0;
}



void *       loadpcx2(char *       filestring)
{      
  void         *p = NULL;
  byte         b; 


   initsvga(0x101);
   b = loadpcxxy(filestring, 1, 0,0); 
   if (b == 0) { 
      p = malloc( 10000 ); 
      getimage(0,0,49,49,p); 
      _wait(); 
   } 
   return p; 

} 


