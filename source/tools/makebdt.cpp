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
#include "..\basegfx.h"

#ifdef HEXAGON
#include "..\loadbi3.h"
#endif

void        loadpcx2(char *       filestring, pwterraintype bdt, int i);


const char* bildnr[8] = { " 0   ( up ) ",
                          " 1   ( right ) ",
                          " 2   ( down ) ",
                          " 3   ( left ) ",
                          " 4   ( up mirrored ) ",
                          " 5   ( left  mirrored ) ",
                          " 6   ( down mirrored ) ",
                          " 7   ( right mirrored ) " };


// const char* direcs[8] = { "oben","rechts oben","rechts","rechts unten","unten","links unten","links","links oben" };


int main(int argc, char *argv[] )
{
   t_carefor_containerstream cfcst;

   try {
      loadpalette();
   
      settxt50mode (); 
   
      pterraintype bdt;
      tfile          datfile;
      char           pict = YES;       // Bilder einlesen
      char           dif = 1;          // Anzahl der Bilder
   
      char           maxmovement = 0;
    
      bdt = new ( tterraintype ) ;
      memset ( bdt, 0, sizeof ( *bdt ));
   
      for (int i = 0; i < cwettertypennum; i++) bdt->weather[i] = NULL;
      strcpy (datfile.name, "");
   
      _settextcolor (7);
      _setbkcolor (0);
      clearscreen ();
   
   
      char   creat_edit = 0;
      if ( argc < 2) {
         printf ("\n    Create a new terrain or edit an existent one : \n");
         yn_switch (" Create ", " Edit ", 0, 1, creat_edit);
      } else {
         creat_edit = 1;
      } /* endif */
   
      if (creat_edit==0) {                                            // creat new tank
         clearscreen ();
         printf ("\n    Enter filename (without extension)\n");
         stredit2 (datfile.name, 9, 255,255);
   
         strcat (datfile.name, ".trr");
   
         if (exist(datfile.name)) {
            sound (800);
            clearscreen(); 
            _settextcolor (15);
            _setbkcolor (red);
            _settextposition (4, 5);
            char s[100];
            sprintf ( s, " !! Filename <%s> already exists !! \n", datfile.name );
            _outtext ( s );
            _wait ();
            nosound ();
            exit (0);
         } 
   
      } 
      else {                                                         // edit existing tank
   
         if ( argc < 2) {
            clearscreen ();
            fileselect("*.trr", _A_NORMAL, datfile);
            clearscreen ();
         } else
            strcpy ( datfile.name, argv[1] );
   
         bdt = loadterraintype(datfile.name);
   
         
      } 
   
   
   
         printf ("\n    ID : \n");
         num_ed ( bdt->id , 0, 65534);
   
         printf ("\n    name of terrain \n");
         stredit (bdt->name, 21, 255,255);
   
   
         int weather = 1;
         for (i = 0; i < cwettertypennum ;i++ )
            if ( bdt->weather[i] != NULL )
               weather |= ( 1 << i );
   
   
         printf ("\n    for which weather should the terrain be available \n");
         bitselect (weather, cwettertypen, cwettertypennum);
   
         for (i = 0; i< cwettertypennum ;i++ ) 
            if (weather & ( 1 << i)) {
               clearscreen();
               printf( " %s \n", cwettertypen[i] );
               if (bdt->weather[i] == NULL) {
                   bdt->weather[i] = new ( twterraintype );
                   memset ( bdt->weather[i], 0, sizeof ( *bdt->weather[i] ));
               }
   
                if ( creat_edit ) {                                            // creat new tank
                   printf ("\n    new picture ? \n");
                   yn_switch ("", "", NO, YES, pict);
                } else
                   pict = 0;
   
                if (pict == 0) {
   
                   tfile    pictfile;
             
                   char  ok=1;
                   int pics = 0;
                  #ifndef HEXAGON
                   for ( int m = 0 ; m < 8 ; m++ )
                      if ( bdt->weather[i]->picture[m] )
                         pics |= ( 1 << m );
                   /*
                     if ( pics == 0 ) 
                      pics = 1;
                   */
                  #else
                   pics = 1;
                  #endif
   
                   do { 
                      #ifndef HEXAGON
                      printf ("\n    which pictures shall be changed ?\n");
                      bitselect (pics, bildnr, 8);
                      #endif
   
                      int   j = 0;
                      int num = 0;
             
                      do { 
                         if ( pics & ( 1 << j ) ) {
   
                           #ifdef HEXAGON
                            printf ("\n    Read own picture or one out of Battle Isle = \n");
   
                            char src = 1;
                            yn_switch ("seperate", "BI3", NO, YES, src );
                           #else
                            char src = 0;
                            printf ("\n    select picturfile %s \n\n", bildnr[j] );
                           #endif
   
                            if ( src != 1 ) {
   
                               fileselect ("*.PCX", _A_NORMAL, pictfile);
                                                         
                               initgraphics ( 640, 480, 8 );
                               loadpcx2(pictfile.name, bdt->weather[i], j);
                               #ifdef HEXAGON
                               bdt->weather[i]->bi_picture[j] = -1;
                               #endif
                               num++;
                               settxt50mode (); 
   
                            } else {
                              #ifdef HEXAGON
                               initgraphics ( 640, 480, 8 );
                               getbi3pict ( &bdt->weather[i]->bi_picture[j], &bdt->weather[i]->picture[j] ); 
                               num++;
                               settxt50mode (); 
                              #endif
                            }
                         }
                         j++;
                         
                      }  while (j < 8 );
             
                      if ( (pics & 1) == 0 )
                         ok = 0;
                      else {
                         printf ("\n    number of pictures      :  %i", num );
                         printf ("\n    is everything correct ?\n");
                         yn_switch (" yes ", " no, repeat pictureselection ", 1,0, ok);
                      }
                   }  while (!ok);
                  
                } else { 
                   initgraphics ( 640, 480, 8 );
             
                   for (int j = 0; j < 8; j++)
                     if (bdt->weather[i]->picture[j])
                        putspriteimage(40 + j * 40 , 40 , bdt->weather[i]->picture[j]);
             
                   setvgapalette256(pal);
                   _wait();
                   settxt50mode (); 
                }
   
   
   
   
   
   
                printf ("\n    terrain type: \n");
                bitselect ( bdt->weather[i]->art, cbodenarten, cbodenartennum );
   
   
                printf ("\n    defensebonus ");
                num_ed (bdt->weather[i]->defensebonus, 0, 65536 );
   
                printf ("\n    attackbonus ");
                num_ed (bdt->weather[i]->attackbonus, 0, 65536 );
   
                printf ("\n    basic jamming");
                num_ed (bdt->weather[i]->basicjamming, 0, 255 );
   
                for (int j=0; j< cmovemalitypenum; j ++)
                   printf(" %i : %s \n", j, cmovemalitypes[j] );
   
   
                int oldnum = bdt->weather[i]->movemaluscount;
                printf ("\n    number of movemali" );
                num_ed (bdt->weather[i]->movemaluscount, 1, cmovemalitypenum );
   
                if (creat_edit == 0) {
                   bdt->weather[i]->movemalus = (char*) calloc ( bdt->weather[i]->movemaluscount, 1 );
                } else {
                   bdt->weather[i]->movemalus = (char*) realloc ( bdt->weather[i]->movemalus , bdt->weather[i]->movemaluscount );
                };
   
                printf ( " 0 for default \n");
                for (j=0; j< bdt->weather[i]->movemaluscount; j++) {
                   printf(" %i : %s \n", j, cmovemalitypes[j] );
                   
                   num_ed ( bdt->weather[i]->movemalus[j], 0, 255 );
                } /* endfor */
                
   
                #ifndef HEXAGON
   
                pwterraintype pgbt = bdt->weather[i];
                for (j=0; j<8 ;j++ ) {
                   if ( !pgbt->picture[j] ) {
                     if (j == 1) {
            
                        pgbt->picture[j]   = malloc( fieldsize );
                        pgbt->direcpict[j] = malloc ( fielddirecpictsize );
                        pgbt->direcpict[j] = NULL;
   
                        npush ( *agmp );
            
                        agmp->windowstatus = 100;
                        agmp->byteperpix = 1;
                        agmp->resolutionx = 50;
                        agmp->resolutiony = 50;
                        agmp->bytesperscanline = 50;
                        agmp->scanlinelength = 50;
            
                        agmp->linearaddress = (int) malloc ( agmp->resolutionx * agmp->resolutiony );
                        memset ( (void*) agmp->linearaddress, 255, agmp->resolutionx * agmp->resolutiony );
                        putrotspriteimage90 ( 10, 10, pgbt->picture[0] , 0);
                        putrotspriteimage90 ( 11, 10, pgbt->picture[0] , 0);
            
            
                        
                        getimage(10,10, 10 + fieldxsize-1, 10 + fieldysize-1, pgbt->picture[j] );
            
                        char *b = (char*) pgbt->direcpict[j];
            
                        for (int t = 1; t < 20; t++) {
                           for (int u = 20-t; u < 20+t; u++) {
                              *b = getpixel(  10 + u, 9 + t);
                              b++;
                           }
                        }
                        
                        for (t = 20; t > 0; t-- ) {
                           for (int u =20-t; u<= 19 + t; u++) {
                              *b = getpixel(  10 + u, 10 + 39 - t );
                              b++;
                           }
                        }
            
            
                        free ( (void*) agmp->linearaddress );
                        npop  ( *agmp );
            
                     } else
                        if (j >= 2 && j <= 3) {
            
                           char *b1, *b2;
                           int k;
            
                           pgbt->picture[j] = malloc ( fieldsize );
                           b1 = (char*) pgbt->picture[j-2];
                           b2 = (char*) pgbt->picture[j];
                           for (k=0; k<4; k++,b1++,b2++)
                              *b2 = *b1;
            
                           b2 = (char*) pgbt->picture[j] + fieldsize - 1;
            
                           for (k=4; k<fieldsize; k++) {
                              *b2 = *b1;
                              b1++;
                              b2--;
                           }
            
                           pgbt->direcpict[j] = malloc ( fielddirecpictsize );
                           b1 = (char*) pgbt->direcpict[j-2];
                           b2 = (char*) pgbt->direcpict[j] + fielddirecpictsize - 1;
            
                           for (k=0; k<fielddirecpictsize; k++) {
                              *b2 = *b1;
                              b1++;
                              b2--;
                           }
            
                        }  else
                             if ( j <= 7 ) {
                                pgbt->picture[j]   = malloc ( fieldsize );
                                pgbt->direcpict[j] = malloc ( fielddirecpictsize );
                                flippict( pgbt->picture[j-4], pgbt->picture[j] );
                                generatedirecpict  ( pgbt->picture[j] , pgbt->direcpict[j] );
                             }
                   }
                } /* endfor */
             
                pgbt->quickview = generateaveragecol ( pgbt );
   
                #endif
              
   
            } else
              bdt->weather[i] = NULL;
   
   
   /*
      printf ("\n    Sollen die IDs der Nachbarfielder editiert werden ? : \n");
      char   nb_edit = 0;
      yn_switch (" JA ", " NE ", 1, 0, nb_edit);
      if ( nb_edit ) {
         for (i = 0; i< 8 ;i++ ) {
            printf(" \n %s \n",direcs[i] );
            num_ed ( bdt->neighbouringfield[i] , 0, 65530);
         }
      }
   
    
      printf ("\n    Sollen die Farbwerte fÅr die öberviewskarte editiert werden ? : \n");
      nb_edit = 0;
      yn_switch (" JA ", " NE ", 1, 0, nb_edit);
      if ( nb_edit ) {
         int weather = 0;
         initsvga ( 0x101 );
         for ( i = 0; i < 8; i++ ) {
         
            putimage ( 20 + i*80, 10, bdt->weather[ weather ]->picture[i] );
            bar ( 20 + i * 80, 100, 60 + i * 80, 140, bdt->weather[ weather ]->quickview->dir[ i ].p1 );
   
            for ( int y = 0; y < 3; y ++ ) {
               if ( y < 1 ) {
                  int x = 1;
                  bar ( 20 + i * 80 + x * 13, 200 + y * 13, 20 + i * 80 + ( x + 1 ) * 13, 200 + ( y + 1 ) * 13, bdt->weather[ weather ]->quickview->dir[ i ].p3[x][y] );
               } else {
                  for (int x = 1 - ( 2 - y ) ; x <= 1 + ( 2 - y ) ; x++ ) 
                     bar ( 20 + i * 80 + x * 13, 200 + y * 13, 20 + i * 80 + ( x + 1 ) * 13, 200 + ( y + 1 ) * 13, bdt->weather[ weather ]->quickview->dir[ i ].p3[x][y] );
               }
            }
   
            for (  y = 0; y < 5; y ++ ) {
               if ( y < 3 ) {
                  for (int x = 2 - y ; x <= 2 + y  ; x++ ) 
                    bar ( 20 + i * 80 + x * 8, 300 + y * 8, 20 + i * 80 + ( x + 1 ) * 8, 300 + ( y + 1 ) * 8, bdt->weather[ weather ]->quickview->dir[ i ].p5[x][y] );
               } else {
                  for (int x = 2 - ( 4 - y ) ; x <= 2 + ( 4 - y ) ; x++ ) 
                     bar ( 20 + i * 80 + x * 8, 300 + y * 8, 20 + i * 80 + ( x + 1 ) * 8, 300 + ( y + 1 ) * 8, bdt->weather[ weather ]->quickview->dir[ i ].p5[x][y] );
               }
            }
         }
   
         setvgapalette256(pal);
         _wait();
         settxt50mode (); 
         return 0;
      }
         */
   
   
   
   
   
      {
         tn_file_buf_stream mainstream (datfile.name,2);
         writeterrain ( bdt, &mainstream );
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



void        loadpcx2(char *       filestring, pwterraintype bdt, int i)
{      
  void         *p = NULL;




   initgraphics ( 640, 480, 8 );
   loadpcxxy ( filestring, 1, 0,0); 

   bdt->picture[i] = malloc( fieldsize );
   #ifndef HEXAGON
   bdt->direcpict[i] = malloc ( fielddirecpictsize );
   b = (char*) bdt->direcpict[i];
   #else
   bdt->direcpict[i] = NULL;
   #endif

   getimage(0,0,fieldxsize-1,fieldysize-1, bdt->picture[i] );
   putimage(10,10, bdt->picture[i] );

   #ifndef HEXAGON
   for (int t = 1; t < 20; t++) {
      for (int u = 20-t; u < 20+t; u++) {
         *b = getpixel(  10 + u, 9 + t);
         b++;
      }
   }
   
   for (t = 20; t > 0; t-- ) {
      for (int u =20-t; u<= 19 + t; u++) {
         *b = getpixel(  10 + u, 10 + 39 - t );
         b++;
      }
   }
   #endif


   _wait(); 

} 


