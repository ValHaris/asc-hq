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
#include "..\krkr.h"
#include "..\sgstream.h"
#include "..\misc.h"
#include "..\stack.h"

#ifdef HEXAGON
#include "loadbi3.h"
#endif

// const char* direcs[8] = { "oben","rechts oben","rechts","rechts unten","unten","links unten","links","links oben" };


dacpalette256 pal;
int lastid = 78;
char movemalus = 10;
int weather = 3;
tfile  datfile;


int main(int argc, char *argv[] )
{
   t_carefor_containerstream cfcst;

   loadpalette();

    settxt50mode (); 

   char cont;

   int trn = 0;

   printf ("\n    wetter: \n");
   num_ed ( weather, 0, 4 );


   printf ("\n    terrain type: \n");
   bitselect ( trn, cbodenarten, cbodenartennum );


   do {

      lastid++;

      settxt50mode (); 
   


      char           pict = YES;       // Bilder einlesen
      char           dif = 1;          // Anzahl der Bilder
   
      char           maxmovement = 0;
    
      _settextcolor (7);
      _setbkcolor (0);
      clearscreen ();
   
      initsvga (0x101);
      int bi_picture;
      void* picture;
      getbi3pict_double ( &bi_picture, &picture ); 
      closesvga();
      settxt50mode (); 
   
      printf ("\n    ID : \n");
      num_ed ( lastid , 0, 65534);

      pterraintype bdt = NULL;

      char name[1000];

      {
         tfindfile ff ( "*.trr" );
      
         char *c = ff.getnextname();
      
         while( c ) {
            bdt = loadbodentyp( c );
                          
            if (  bdt->id == lastid ) {
               strcpy ( name, c );
               break;
            }

            c = ff.getnextname();
         } 
      }

      if ( bdt->id == lastid ) {

         bdt->weather[weather] = new twterraintype;
         memset ( bdt->weather[weather], 0, sizeof ( *bdt->weather[weather] ));

         bdt->weather[weather]->art = bdt->weather[0]->art ;
      
         bdt->weather[weather]->defensebonus =  0;
      
         bdt->weather[weather]->attackbonus = 0;
      
         bdt->weather[weather]->basicjamming = 0;
      
         bdt->weather[weather]->movemaluscount = 1;
      
         bdt->weather[weather]->movemalus = &movemalus;

         bdt->weather[weather]->bi_picture[0] = bi_picture;
         bdt->weather[weather]->picture[0] = picture;
      
      
asfasf         {
            tn_file_buf_stream mainstream ( name, 2 );
            mainstream.writedata ( ( char*) bdt, sizeof ( *bdt ));
            mainstream.writepchar( bdt->name );
            for (int i=0;i<cwettertypennum ;i++ ) {
              if ( bdt->weather[i] ) {
                 mainstream.writedata ( ( char*) bdt->weather[i], sizeof ( *(bdt->weather[i]) ));
                 mainstream.writedata ( bdt->weather[i]->movemalus, bdt->weather[i]->movemaluscount );
                 for ( int j = 0; j < 8; j++ ) {
                    if ( bdt->weather[i]->picture[j] && bdt->weather[i]->bi_picture[j] == -1 ) {
                       mainstream.writedata ( ( char*) bdt->weather[i]->picture[j], fieldsize );
                      #ifndef HEXAGON
                       if ( bdt->weather[i]->direcpict[j] )
                          mainstream.writedata ( ( char*) bdt->weather[i]->direcpict[j], fielddirecpictsize );
                      #endif
                    }
                 }
                 if ( bdt->weather[i]->quickview )
                    mainstream.writedata ( ( char*) bdt->weather[i]->quickview, sizeof ( *bdt->weather[i]->quickview ));
              }
            }
         }
        printf(" %s written\n", name );
      } else 
        printf("not found !\n");

      yn_switch (" continue ", " Exit ", 0, 1, cont);
   } while ( cont==0 ); /* enddo */

   return 0;
}
