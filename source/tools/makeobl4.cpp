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
#include "..\basegfx.h"
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

      {
         tnfilestream stream ( "USABLACK.FNT", 1 );
         fnt = loadfont  ( &stream );
      }
      if ( !fnt ) {
         printf("error loading file USABLACK.FNT \n" );
         return 1;
      }

      pobjecttype ft;
      tfile          datfile;
      char           pict = YES;       // Bilder einlesen
      char           dif = 1;          // Anzahl der Bilder
   
      char           mehrfielderschuetze = 0;
      char           maxmovement = 0;
    
      ft = new tobjecttype;
      memset ( ft, 0, sizeof (tobjecttype) ) ;
   
      strcpy (datfile.name, "");
   
      loadpalette();
      loadbi3graphics();
   
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
            closegraphics ();
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
         ft->pictnum = 0;
   
         char contin = 1;
         char graph = 1;
         ft->dirlistnum = 0;
            
   
         printf ("\n    for which weather should the object have different pics \n");
         bitselect (ft->weather, cwettertypen, cwettertypennum);
   
         int first = 1;
   
         for ( int ww = 0; ww < cwettertypennum; ww++ )
            if ( ft->weather & ( 1 << ww)) {
   
              printf(" \n use which graphics ? \n");
              yn_switch (" BI3 ", " own ", 1, 0, graph );


               ft->picture[ww] = new thexpic[100];
   
               if ( first ) {
                  do {
                     if ( graph == 1 ) {
                        initgraphics ( 640, 480, 8);
                         
                        loadpalette();
                        setvgapalette256 ( pal );
               
                        getbi3pict_double ( &ft->picture[ww][ft->pictnum].bi3pic, &ft->picture[ww][ft->pictnum].picture );
                        ft->picture[ww][ft->pictnum].flip = 0;
                     } else {
                        tfile    pictfile;
                       
                        fileselect ("*.PCX", _A_NORMAL, pictfile);
                                                  
                        initgraphics ( 640, 480, 8);
                        ft->picture[ww][ft->pictnum].picture = loadpcx2(pictfile.name);
                        ft->picture[ww][ft->pictnum].bi3pic = -1;
                        ft->picture[ww][ft->pictnum].flip = 0;
                     }
                     ft->pictnum++;
                                       
                     closegraphics();
                     settxt50mode();
            
                    /*
                     if ( dirlist ) {
                        printf ("\n    dir : \n");
                        int d = 0;
                        num_ed ( ft->dirlist[ ft->dirlistnum ] , 0, maxint);
                        ft->dirlistnum++;
                     }
                    */
            
                     printf(" \n include more pictures ? \n\n");
                     yn_switch (" YES ", " NO ", 1, 0, contin );
               
                  } while ( contin ); /* enddo */
                  first = 0;
               } else {
                  printf("now the pictures for %s", cwettertypen[ww] );
                  _wait();
   
                  if ( graph == 1 ) {
                     initgraphics ( 640, 480, 8);
                     tnfilestream mainstream ( "palette.pal" , 1);
                     mainstream.readdata( (char*) pal, sizeof(pal)); 
                     setvgapalette256 ( pal );

                     for ( int m = 0; m < ft->pictnum; m++ ) 
                        getbi3pict_double ( &ft->picture[ww][m].bi3pic, &ft->picture[ww][m].picture );
                     closegraphics();
                     settxt50mode();
                  } else {
                     for ( int m = 0; m < ft->pictnum; m++ ) {
                        tfile    pictfile;
                       
                        fileselect ("*.PCX", _A_NORMAL, pictfile);
                                                  
                        initgraphics ( 640, 480, 8);
                        ft->picture[ww][m].picture = loadpcx2(pictfile.name);
                        ft->picture[ww][m].bi3pic = -1;
                        ft->picture[ww][m].flip = 0;
                        closegraphics();
                        settxt50mode();
                     } 
                  }
               }
            }                                         
   
      } else { 
         initgraphics ( 640, 480, 8);
   
         putspriteimage( 50, 50, ft->picture[0][0].picture ); 
   
         {
            tnfilestream mainstream ( "palette.pal" , 1);
            mainstream.readdata( (char*) pal, sizeof(pal)); 
         }
         setvgapalette256(pal);
   
         wait();
         closegraphics();
      } 
     
      settxt50mode();
   
      printf ("\n    ID : \n");
      num_ed ( ft->id , 0, maxint);
   
      printf ("\n    name of object \n");
      stredit (ft->name, 40, 255,255);
   
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
      num_ed ( ft->buildcost.a.material , 0, maxint);
   
      printf ("\n    production cost fuel : \n");
      num_ed ( ft->buildcost.a.fuel, 0, maxint);
   
     #ifdef HEXAGON
      printf ("\n    production cost movement : \n");
      num_ed ( ft->build_movecost, 0, maxint);
     #else
      printf ("\n    production cost movement : \n");
      num_ed ( ft->movecost, 0, maxint);
     #endif
   
      printf ("\n    removal cost material : \n");
      num_ed ( ft->removecost.a.material , 0, maxint);
   
      printf ("\n    removal cost  fuel : \n");
      num_ed ( ft->removecost.a.fuel, 0, maxint);
   
     #ifdef HEXAGON
      printf ("\n    removal cost  movement : \n");
      num_ed ( ft->remove_movecost, 0, maxint);
     #endif
   
      char va = ft->visibleago;
      printf ("\n    should the object still be displayed if the field is not visible any more ?\n");
      yn_switch ("YES","NO", 1,0,  va );
      ft->visibleago = va;


      terrainaccess_ed ( &ft->terrainaccess, "object" );
      
      {
         printf ("\n    link automatically with neighbouring fields? \n");
         char c = ft->no_autonet;
         yn_switch (" yes ", " no ", 0, 1, c );
         ft->no_autonet = c;
         if ( !ft->no_autonet ) {
         
               dynamic_array<pobjecttype> obj;
               dynamic_array<pchar> name;
         
               int objectlayernum = 0; 
             
               tfindfile ff ( "*.obl" );
         
               char *c = ff.getnextname();
             
               while ( c ) {
                   obj[objectlayernum] = loadobjecttype( c ); 
                   name[objectlayernum] = strdup ( c );
                   objectlayernum++;
                   c = ff.getnextname();
                }
             
                printf("\n\n\n    the object links to the following other objects : \n\n");
                int* pi = new int[100];
                memset ( pi, 0, 400 );
                for ( int i = 0; i < ft->objectslinkablenum; i++ ) {
                   for ( int j = 0; j < objectlayernum; j++ )
                      if ( obj[j]->id == ft->objectslinkableid[i] )
                         printf("           %s\n", name[j] );
         
                   pi[i] = ft->objectslinkableid[i];
                }
                pi[i] = 0;
         
                ft->objectslinkableid = pi;
         
         
                char m = 0;
                printf("     Change ?\n");
         
                yn_switch ("no", "yes", 0, 1, m);
                if ( m ) {
                   int num = 0;
                   do {
                     printf("\n\n\n\n");
                     for ( int j = 0; j < objectlayernum; j++ ) {
                        printf("%3i %28s |    ", obj[j]->id, name[j]);
                        if (j % 2 ==0) printf("\n");
                     }
         
                     printf (" enter id of object  ( 0 to continue ): ");
                     num_ed (ft->objectslinkableid[num],0,10000);
                     num++;
                   } while ( ft->objectslinkableid[num-1] ); /* enddo */
                   ft->objectslinkablenum = num-1;
         
                }
         
            
         }
      }
   
      ft->buildcost.a.energy = 0;
      ft->removecost.a.energy = 0;
   
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

   activefontsettings.font = fnt; 
   activefontsettings.color = black; 
   activefontsettings.background = white; 
   activefontsettings.length = 600;
   activefontsettings.justify = lefttext; 


   initgraphics ( 640, 480, 8);
   b = loadpcxxy(filestring, 1, 0,0); 
   if (b == 0) { 
      p = malloc( 10000 ); 
      getimage(0,0, fieldsizex-1, fieldsizey-1, p); 

      showtext2 ( "press x and y to flip the picture", 30, 400 );
      showtext2 ( "enter to continue", 30, 440 );

      int ch;
      do {
         ch = getch();
         if ( ch == 'x' ) {
            void* buf = asc_malloc ( imagesize ( 0, 0, fieldxsize, fieldysize ) );
            flippict ( p, buf , 1 );
            p = buf;
         }
      
         if ( ch == 'y' ) {
            void* buf = asc_malloc ( imagesize ( 0, 0, fieldxsize, fieldysize ) );
            flippict ( p, buf , 2 );
            p = buf;
         }
         bar ( 0, 0, 100, 100, 0 );
         putspriteimage ( 0, 0, p );

      } while ( ch != 13 ); /* enddo */

   } 
   return p; 

} 


