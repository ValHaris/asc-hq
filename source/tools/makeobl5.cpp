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
      Object*type ft;
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
      loadpalette();
   
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
            wait ();
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
   
   
   
      ft->xoffs = 0;
      ft->yoffs = 0;
   
   
      if (pict) {
         initsvga ( 0x101 );
   
         for ( int i = 0; i < ft->pictnum; i++ ) {
            int m = ft->picture[0][i].bi3pic - 1560 + 1;
            char tmp[100];
            strcpy ( tmp, "f:\\asc\\objects\\icebreak\\");
            strcat ( tmp, strrr ( m ));
            strcat ( tmp, ".pcx");
            ft->picture[0][i].picture = loadpcx2 ( tmp );
            ft->picture[0][i].bi3pic = -1;
         }
         

   
   
      } else { 
         initsvga (0x101);
   
         putspriteimage( 50, 50, ft->picture[0][0].picture ); 
   
         {
            tnfilestream mainstream ( "palette.pal" , 1);
            mainstream.readdata( (char*) pal, sizeof(pal)); 
         }
         setvgapalette256(pal);
   
         wait();
   
      } 
   
      settxt50mode();
   
      {
          tn_file_buf_stream mainstream (datfile.name, 2 );
          writeobject  ( ft, &mainstream, 0 );
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


   b = loadpcxxy(filestring, 1, 0,0); 
   if (b == 0) { 
      for ( int x = 0; x < 100; x++ )
         for ( int y = 0; y < 100; y++ )
            if ( getpixel ( x, y ) == 94 )
               putpixel ( x, y , 255 );



      p = malloc( 10000 ); 
      getimage(0,0,fieldsizex-1,fieldsizey-1,p); 
      wait(); 
   } 
   return p; 

} 


