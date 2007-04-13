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
//#include <conio.h>
#include <graph.h>
//#include <stdio.h>
//#include <stdlib.h>

#include "tpascal.inc"
#include "typen.h"
#include "vesa.h"
#include "loadpcx.h"
#include "krkr.h"
#include "sgstream.h"
#include "misc.h"

/* USES typen,newgraph,graf2,crt,dos,krkr, gif2,pictload,loaders,pcx,vesa; */ 

      dacpalette256 pal;


char     richtung[2][11]  = {"0 ø", "45 ø"}; 
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


//char           s[STRSIZ], s1[STRSIZ], name[STRSIZ];
//text           txt;
//boolean        load, pict, muned;


//int            f;




void *       loadpcx2(char *       filestring);



void readtextfile ( char* name, pchar &buf, int allocated )
{
   int size = filesize ( name );
   if (!allocated)
     buf = (char*) malloc ( size+1 );

   char *p1, p2;
   p1 = buf;

   tnfilestream mainstream ( name , 1);
   for (int i = 0; i < size ; i++ ) {
      mainstream.readdata ( &p2, 1);
      if (p2 == 0x0D) {
        mainstream.readdata ( &p2, 1);
        i++;
      }
      *p1 = p2;
      p1++;
   } /* endfor */
   *p1 = 0;
}





main (int argc, char *argv[] )
{ 

   Vehicletype*   ft;
   tfile          datfile;
   char           pict = YES;       // Bilder einlesen
   char           dif = 1;          // Anzahl der Bilder

   char           mehrfielderschuetze = 0;
   char           maxmovement = 0;
   long           erwartungen;
 
   ft = (Vehicletype*) calloc (sizeof (tvehicletype), 1) ;
   for (int i = 0; i <= 1; i++) ft->picture[i] = NULL;
   strcpy (datfile.name, "");

   if (mode50) settxt50mode ();
   _settextcolor (7);
   _setbkcolor (0);
   clearscreen ();
   char   creat_edit ;
   if ( argc < 2 ) {
      printf ("\n    vehicle erstellen oder editieren : \n");
      creat_edit = 0;
      yn_switch (" Create ", " Edit ", 0, 1, creat_edit);
   } else
      creat_edit = 1;

   if (creat_edit==0) {                                            // creat new tank
      clearscreen ();
      printf ("\n    Welchen Dateinamen soll die vehicle bekommen ? (ohne Endung)\n");
      stredit2 (datfile.name, 9, 255,255);
      strcat (datfile.name, ".veh");

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
   else {                                                         // edit existing tank

      clearscreen ();
      if ( argc < 2 ) 
         fileselect("*.veh", _A_NORMAL, datfile);
      else
         strcpy ( datfile.name, argv[1] );

      clearscreen ();
      ft = loadvehicletypetype(datfile.name);
      if (ft->picture[0] == ft->picture[1]) { 
         ft->picture[1] = NULL;
         dif = false; 
      } 
      else 
         dif = true; 

      printf ("\n    Soll ein neues Bild eingelesen werden \n");
      yn_switch ("", "", YES, NO, pict);
      
   } 


   if (pict) {
      tfile    pictfile;

      char  ok=1;
      do { 

        int   i = 0;

        int pics = 0;
        for ( int m = 0 ; m < 8 ; m++ )
           if ( ft->picture[m] )
              pics |= ( 1 << m );

        /*
          if ( pics == 0 ) 
           pics = 1;
        */


           printf ("\n    Welche Bilder sollen neu eingelesen werden ?\n");
           bitselect (pics, bildnr, 8);

           printf ("\n    Welche Bilder sollen gel”scht werden ?\n");
           int kill = 0;
           bitselect (kill, bildnr, 8);
           for ( int n = 0; n < 8; n++ )
              if ( kill & ( 1 << n ) )
                 ft->picture[n] = NULL;

           int   j = 0;
           int num = 0;
  
           do { 
              if ( pics & ( 1 << j ) ) {
                 printf ("\n    Select picturfile %s \n", bildnr[j] );
                 wait ();

                 fileselect ("*.PCX", _A_NORMAL, pictfile);
                                           
                 initsvga (0x101);
                 ft->picture[i] = loadpcx2(pictfile.name);
                 num++; 
                 settextmode(3);
                 if (mode50) settxt50mode ();
              }
              j++;
              
           }  while (j < 8 );
  
           if ( (pics & 1) == 0   &&  creat_edit==0 )
              ok = 0;
           else {
              printf ("\n    Anzahl der Bilder       :  %i", num );
              printf ("\n    Is everything correct ?\n");
              yn_switch (" YES, Go on ", " NO, repeat pictureselection ", 1,0, ok);
           }
        }  while (!ok);
     
   } else { 
      void     *p, *q;
      initsvga (0x101);
      p = ft->picture[0]; 
      q = ft->picture[1]; 

      if (q == NULL)
         q = p; 

      for (int i = 0; i <= 8; i++) {
         putrotspriteimage(la + 50 * (i),50,p,i * 8); 
         putrotspriteimage(la + 50 * (i),100,q,i * 8); 

         putrotspriteimage90(la + 50 * (i),150,p,i * 8); 
         putrotspriteimage90(la + 50 * (i),200,q,i * 8); 

         putrotspriteimage180(la + 50 * (i),250,p,i * 8); 
         putrotspriteimage180(la + 50 * (i),300,q,i * 8); 

         putrotspriteimage270(la + 50 * (i),350,p,i * 8); 
         putrotspriteimage270(la + 50 * (i),400,q,i * 8); 
      } 

      loadpalette();
      setvgapalette256(pal);

      wait();

      //settextmode (3);  
      //if (mode50) 
      settxt50mode (); 
      printf ("\n    Everything correct ? \n");
      char  c=1;
      yn_switch (" YES ", " NO, exit ", 1,0, c);
      if (c==0) exit(0);
   } 

                

   {

      printf ("\n    Fahrzeuggattung  : ");
      for (i=0; i<cmovemalitypenum; i++) {
         printf ("\n %i.)  %s ", i, cmovemalitypes[i]);
      } /* endfor */
      num_ed (ft->movemalustyp, 0, cmovemalitypenum-1);


      printf ("\n    fielder auf die die vehicle fahren kann : \n");
      bitselect (ft->terrain, cbodenarten, cbodenartennum);

      printf ("\n    Bits, die ALLE n”tig sind, damit die vehicle aufs field fahren kann : \n");
      bitselect (ft->terrainreq, cbodenarten, cbodenartennum);

      printf ("\n    Bits, von denen min. eins n”tig ist, damit die vehicle aufs field fahren kann : \n");
      bitselect (ft->terrainreq1, cbodenarten, cbodenartennum);


      printf ("\n    Bits, die nicht gesetzt sein drfen, damit die vehicle drauf kann: \n");
      bitselect (ft->terrainnot, cbodenarten, cbodenartennum);

      printf ("\n    fielder auf denen die vehicle stirbt : \n");
      bitselect (ft->terrainkill, cbodenarten, cbodenartennum);

   }

   tn_file_buf_stream mainstream (datfile.name,2);
   mainstream.writedata ( (char*) ft, sizeof(*ft));

   if (ft->name)
      mainstream.writepchar( ft->name );
   if (ft->description)
      mainstream.writepchar( ft->description );
   if (ft->infotext)
      mainstream.writepchar( ft->infotext );
   for (i=0; i<8; i++)
      if ( ft->classnames[i] )
         mainstream.writepchar( ft->classnames[i] );

   for (i=0;i<8  ;i++ ) 
      if ( ft->picture[i] ) 
         mainstream.writedata ( (char*) ft->picture[i], tanksize );

   for ( i = 0; i < ft->objectsbuildablenum; i++ )
      mainstream.writedata2 ( ft->objectsbuildable[i] );

   if (ft->picture[1] == NULL)
       ft->picture[1] = ft->picture[0];


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
      p = malloc( 1800 ); 
      getimage(0,0,30,30,p); 
      wait(); 
   } 
   return p; 

} 


