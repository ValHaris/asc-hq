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


dacpalette256 pal;


char     richtung[2][11]  = {"0 ø", "45 ø"}; 
char     mode50 = 1;
#define  la   80

int offset = 1560;

pfont fnt;

int lastid;


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

   pobjecttype ft = new tobjecttype;
 
   char filename[260];
   filename[0] = 0;

   {
      tnfilestream stream ( "USABLACK.FNT", 1 );
      fnt = loadfont  ( &stream );
   }
   activefontsettings.font = fnt; 
   activefontsettings.color = black; 
   activefontsettings.background = white; 
   activefontsettings.length = 600;
   activefontsettings.justify = lefttext; 

   settxt50mode ();

   printf ("\n    enter filename (without extension)\n");
   stredit2 ( filename, 9, 255,255);


   loadpalette();


   printf ( "\n   create based on existent object ? \n");
   char existent = 0;
   yn_switch (" YES", " NO ", 1, 0, existent );

   if ( existent ) {

      tfile datfile;
      clearscreen ();
      if ( argc < 2 ) 
         fileselect("*.obl", _A_NORMAL, datfile);
      else
         strcpy ( datfile.name, argv[1] );

      clearscreen ();
      ft = loadobjecttype(datfile.name);

      char* temp = ft->name;
      ft->name = new char [ 100 ] ;
      strcpy ( ft->name, temp );

   } else {
      ft->name = new char [ 100 ] ;
      ft->name[0] = 0;

   ft->terrain_and = -1;
   ft->terrain_or = 0;
   ft->attackbonus_plus = 0;
   ft->attackbonus_abs = -1;
   ft->defensebonus_plus = 0;
   ft->defensebonus_abs = -1;
   ft->basicjamming_plus = 0;
   ft->basicjamming_abs = -1;
   ft->armor = 2000;
   ft->height = 3;
   ft->buildcost.material = 300;
   ft->buildcost.fuel = 150;
   ft->remove_movecost = 15;
   ft->build_movecost = 30;
   ft->removecost.material = 0;
   ft->removecost.fuel = 100;
   ft->movemalus_plus_count = 0;
   ft->movemalus_abs_count = 0;
   }


   {
      tn_file_buf_stream stream ( "object.id", 1 );
      stream.readdata2 ( lastid );
   }



   printf ("\n    last ID : \n");
   num_ed ( lastid , minint, maxint);

   printf ("\n    name of object \n");
   stredit (ft->name, 21, 255,255);

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

   printf ("\n    production cost movement : \n");
   num_ed ( ft->build_movecost, 0, maxint);

   printf ("\n    removal cost material : \n");
   num_ed ( ft->removecost.material , 0, maxint);

   printf ("\n    removal cost  fuel : \n");
   num_ed ( ft->removecost.fuel, 0, maxint);

   printf ("\n    removal cost  movement : \n");
   num_ed ( ft->remove_movecost, 0, maxint);


   {
      printf ("\n    link automatically with neighbouring fields? \n");
      char c = ft->no_autonet;
      yn_switch (" yes ", " no ", 0, 1, c );
      ft->no_autonet = c;
   }

   char contin = 1;

   ft->buildcost.energy = 0;
   ft->removecost.energy = 0;

   {
       for (int j=0; j< cmovemalitypenum; j ++)
          printf(" %i : %s \n", j, cmovemalitypes[j] );

       printf ("\n  movemalus_plus_count" );
       num_ed (ft->movemalus_plus_count, 0, cmovemalitypenum );

       ft->movemalus_plus = (char*) calloc ( ft->movemalus_plus_count, 1 );

       printf ( " 0 for default \n");
       for (j=0; j< ft->movemalus_plus_count; j++) {
          printf(" %i : %s \n", j, cmovemalitypes[j] );
          
          num_ed ( ft->movemalus_plus[j], 0, 255 );
       } /* endfor */

   }{
       for (int j=0; j< cmovemalitypenum; j ++)
          printf(" %i : %s \n", j, cmovemalitypes[j] );

       int oldnum = ft->movemalus_abs_count;
       printf ("\n  movemalus_abs_count  " );
       num_ed (ft->movemalus_abs_count, 0, cmovemalitypenum );

       char* nw = new char [ ft->movemalus_abs_count ];
       for ( int k = 0; k < oldnum; k++ )
          nw[k] = ft->movemalus_abs[k];
       ft->movemalus_abs = nw;

       printf ( " 0 for default \n");
       for (j=0; j< ft->movemalus_abs_count; j++) {
          printf(" %i : %s \n", j, cmovemalitypes[j] );
          
          num_ed ( ft->movemalus_abs[j], 0, 255 );
       } /* endfor */
   }

   do {


      initsvga (0x101);

      ft->pictnum = 1;
      ft->picture = new thexpic[1];

      setvgapalette256 ( pal );

      ft->picture[0].flip = 0;

      bar ( 0, 0, 639, 479, 0 );

      void* pic;
      int num;
      getbi3pict_double ( &ft->picture[0].bi3pic, &ft->picture[0].picture );

      closesvga();
      settxt50mode();

      lastid++;
      ft->id = lastid;

      {
          tn_file_buf_stream mainstream ( getnextfilenumname ( filename, "obl", ft->id ), 2 );
          writeobject ( ft, &mainstream , 0 );
      }

      printf("  ID was %d \n", lastid );
      {
         tn_file_buf_stream stream ( "object.id", 2 );
         stream.writedata2 ( lastid );
      }


      printf(" \n continue ? \n\n");
      yn_switch (" continue ", " Exit ", 1, 0, contin );

   } while ( contin ); /* enddo */

   return 0;
}
