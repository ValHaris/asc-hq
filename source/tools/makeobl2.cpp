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

#include "tpascal.inc"
#include "typen.h"
#include "vesa.h"
#include "loadpcx.h"
#include "krkr.h"
#include "sgstream.h"
#include "misc.h"


dacpalette256 pal;


char     richtung[2][11]  = {"0 ¯", "45 ¯"}; 
char     mode50 = 1;
#define  la   80





const char* bildnr[8] = { " 0   ( oben ) ",
                          " 1   ( rechts oben ) ",
                          " 2   ( rechts ) ",
                          " 3   ( rechts unten ) ",
                          " 4   ( unten ) ",
                          " 5   ( links unten ) ",
                          " 6   ( links ) ",
                          " 7   ( links oben) " };



void *       loadpcx2(char *       filestring);










main (int argc, char *argv[] )
{ 

   Object*type ft;
   tfile          datfile;
   char           pict = YES;       // Bilder einlesen
   char           dif = 1;          // Anzahl der Bilder

   char           mehrfielderschuetze = 0;
   char           maxmovement = 0;
 
   ft = new tobjecttype;
   memset ( ft, 0, sizeof (tobjecttype) ) ;

   strcpy (datfile.name, "");

   if (mode50) settxt50mode ();
   _settextcolor (7);
   _setbkcolor (0);
   clearscreen ();
   char   creat_edit ;
   if ( argc < 2 ) {
      printf ("\n    Object erstellen oder editieren : \n");
      creat_edit = 0;
      yn_switch (" Create ", " Edit ", 0, 1, creat_edit);
   } else
      creat_edit = 1;

   if (creat_edit==0) {                                            // creat new object
      clearscreen ();
      printf ("\n    Welchen Dateinamen soll das Object bekommen ? (ohne Endung)\n");
      stredit2 (datfile.name, 9, 255,255);
      strcat (datfile.name, ".obl");

      ft->terrain_and = -1;
      ft->pictnum = 8;
      ft->picture = new pointer[256];
      ft->attackbonus_abs = -1;
      ft->defensebonus_abs = -1;
      ft->basicjamming_abs = -1;

      if (exist(datfile.name)) {
         sound (800);
         clearscreen(); 
         _settextcolor (15);
         _setbkcolor (red);
         _settextposition (4, 5);
         char s[100];
         sprintf ( s, " !! Attention :  Filename <%s> already exists !! \n", datfile.name );
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

      printf ("\n    Soll ein neues Bild eingelesen werden \n");
      yn_switch ("", "", YES, NO, pict);
      
   } 


   printf ("\n    terrain AND : \n");
   bitselect ( ft->terrain_and, cbodenarten, cbodenartennum);

   printf ("\n    terrain OR : \n");
   bitselect ( ft->terrain_or , cbodenarten, cbodenartennum);

   ft->xoffs = 0;
   ft->yoffs = 0;

   {
      initsvga(0x101);
      tnfilestream strm2 ( "D:\\WATCOM\\SG\\SINGLE\\STREETS\\fahrspr4.raw", 1 );
      int w;

      for ( int n = 0; n < ft->pictnum; n++ ) {
         strm2.readrlepict ( &ft->picture[n], false, &w );
         putimage( 10 + n * 60 ,10, ft->picture[n] );
      }

      getch();
      settextmode(3);
   } 

   printf ("\n    ID : \n");
   num_ed ( ft->id , 0, maxint);

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
   num_ed ( ft->armor , minint, maxint);

  {
             for (int j=0; j< cmovemalitypenum; j ++)
                printf(" %i : %s \n", j, cmovemalitypes[j] );

             printf ("\n  Movemalus_plus_count  anzahl ( = hîchster+1 ) der Movemali" );
             num_ed (ft->movemalus_plus_count, 0, cmovemalitypenum );

             ft->movemalus_plus = (char*) calloc ( ft->movemalus_plus_count, 1 );

             printf ( " 0 fÅr default \n");
             for (j=0; j< ft->movemalus_plus_count; j++) {
                printf(" %i : %s \n", j, cmovemalitypes[j] );
                
                num_ed ( ft->movemalus_plus[j], 0, 255 );
             } /* endfor */

  }{
             for (int j=0; j< cmovemalitypenum; j ++)
                printf(" %i : %s \n", j, cmovemalitypes[j] );

             printf ("\n  Movemalus_abs_count  anzahl ( = hîchster+1 ) der Movemali" );
             num_ed (ft->movemalus_abs_count, 0, cmovemalitypenum );

             ft->movemalus_abs = (char*) calloc ( ft->movemalus_abs_count, 1 );

             printf ( " 0 fÅr default \n");
             for (j=0; j< ft->movemalus_abs_count; j++) {
                printf(" %i : %s \n", j, cmovemalitypes[j] );
                
                num_ed ( ft->movemalus_abs[j], 0, 255 );
             } /* endfor */
   }

   tn_file_buf_stream mainstream (datfile.name,2);
   mainstream.writedata2 ( *ft );

   if ( ft->pictnum )  
      for ( int i = 0; i < ft->pictnum; i++ )
         mainstream.writedata( ft->picture[i], imagesize ( 55,15,104,64 ) );

    if ( ft->movemalus_plus_count ) 
       mainstream.writedata ( ft->movemalus_plus, ft->movemalus_plus_count );

    if ( ft->movemalus_abs_count ) 
       mainstream.writedata ( ft->movemalus_abs, ft->movemalus_abs_count );

    if ( ft->name )
       mainstream.writepchar ( ft->name );

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
      wait(); 
   } 
   return p; 

} 


