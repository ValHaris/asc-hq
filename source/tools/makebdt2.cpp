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
#include "..\misc.h"
#include "..\stack.h"

// const char* direcs[8] = { "oben","rechts oben","rechts","rechts unten","unten","links unten","links","links oben" };


int lastid = 78;

char movemalus = 10;

int main(int argc, char *argv[] )
{
   t_carefor_containerstream cfcst;

   loadpalette();

   try {
      tnfilestream stream ( "terrain.id", 1 );
      stream.readdata2 ( lastid );
   }
   catch ( tfileerror ) {
   } /* endcatch */

    settxt50mode (); 

   char cont;

   printf ("\n    last ID : \n");
   num_ed ( lastid , 0, 65534);

   char* name = new char[100];
   name[0] = 0;

   printf ("\n    name of terrain \n");
   stredit ( name, 21, 255,255);


   tterrainbits trn = 0;

   printf ("\n    terrain type: \n");
   bitselect ( trn, cbodenarten, cbodenartennum );

   do {

      settxt50mode (); 
   
      pterraintype bdt;
      tfile          datfile;
      char           pict = YES;       // Bilder einlesen
      char           dif = 1;          // Anzahl der Bilder
   
      char           maxmovement = 0;
    
      bdt = new ( tterraintype ) ;
      memset ( bdt, 0, sizeof ( *bdt ));
   
      for (int i = 0; i < cwettertypennum; i++) 
         bdt->weather[i] = NULL;
   
      _settextcolor (7);
      _setbkcolor (0);
      clearscreen ();
   
      lastid++;
      bdt->id = lastid;
   
      bdt->name = "BI3 import";
   
      bdt->weather[0] = new twterraintype;
      memset ( bdt->weather[0], 0, sizeof ( *bdt->weather[i] ));
   
      initsvga (0x101);
      getbi3pict_double ( &bdt->weather[0]->bi_picture[0], &bdt->weather[0]->picture[0] ); 
      closesvga();
      settxt50mode (); 
   
      bdt->weather[0]->art = trn;
   
      bdt->weather[0]->defensebonus =  0;
   
      bdt->weather[0]->attackbonus = 0;
   
      bdt->weather[0]->basicjamming = 0;
   
      bdt->weather[0]->movemaluscount = 1;
   
      bdt->weather[i]->movemalus = &movemalus;
   
   
      {
         tn_file_buf_stream mainstream ( getnextfilenumname ( name, "trr", bdt->id), 2 );
         writeterrain( bdt, &mainstream );
      }

      printf("  ID was %d \n", lastid );
      {
         tnfilestream stream ( "terrain.id", 2 );
         stream.writedata2 ( lastid );
      }

      yn_switch (" continue ", " Exit ", 0, 1, cont);
   } while ( cont==0 ); /* enddo */

   return 0;
}
