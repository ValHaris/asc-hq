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

#include "..\basegfx.h"
#include "..\tpascal.inc"
#include "..\typen.h"
#include "..\loadpcx.h"
#include "krkr.h"
#include "..\basestrm.h"
#include "..\misc.h"
#include "..\mousehnd.h"
#include "..\sgstream.h"



/*

um alle technologien einzulesen und bei techrequired deren namen anzuzeigen, kopier dir aus
 loaders.cpp den ersten teil von 
loadalltechnologies raus. hinten das pointereinrichten brauchst du nicht.
nimm fÅr das icon die routine aus pcx2raw

zu den streams:
tnfilestream zum laden ( der lÑdt auch komprimierte dateien und auch aus der main.dta )
tn_file_buf_stream zum speichern, der komprimiert nÑmlich im gegensatz zu tnfilestream nicht ( damit mein main.dta montierer 
   die beste Methode raussuchen kann.

den stream in einem eigen block definieren, damit beim verlassen des blockes der destructor aufgerufen wird. wie unten.


*/

char     mode50 = 1;





void *       loadpcx2(char *       filestring);
void  selecttechnology(int num, int &id);
void  loadicon (char *filename, void **icon);



void readtextfile ( char* name, pchar &buf, int allocated )
{
   int size = filesize ( name );
   if (!allocated)
     buf = (char*) malloc ( size+1 );

   char *p1, p2;
   p1 = buf;

   tnfilestream stream ( name , 1);
   for (int i = 0; i < size ; i++ ) {
      stream.readdata ( &p2, 1);
      if (p2 == 0x0D) {
        stream.readdata ( &p2, 1);
        i++;
      }
      *p1 = p2;
      p1++;
   } /* endfor */
   *p1 = 0;
}

  
void fexisterror (char *name)
{
   sound (800);
   clearscreen(); 
   _settextcolor (15);
   _setbkcolor (red);
   _settextposition (4, 5);
   char s[100];
   sprintf ( s, " !! Attention :  Filename <%s> already exists !! \n", name );
   _outtext ( s );
   _wait ();
   nosound ();
   closegraphics ( );
   exit (0);
};




main (int argc, char *argv[] )
{ 
   t_carefor_containerstream cfcst;

   ptechnology    tech;
   tfile          datfile;
   char i;
 
   tech = (ptechnology) calloc (sizeof (ttechnology), 1) ;

   if (mode50) settxt50mode ();
   _settextcolor (7);
   _setbkcolor (0);
   clearscreen ();

   printf ("\n    create new technology or edit an existing technology ? :\n");
   char maketech = 0;
   yn_switch (" Create ", " Edit ", 0, 1, maketech);
   

   clearscreen ();
   if (maketech) {
      fileselect ("*.tec", _A_NORMAL, datfile);
      tech = loadtechnology ( datfile.name );
   } else {
      printf ("\n    enter filename for new technology (without extension) :\n");
      strcpy (datfile.name, "");
      stredit2 (datfile.name, 9, 255,255);
      strcat (datfile.name, ".tec");
      if (exist(datfile.name)) fexisterror (datfile.name);
      tech->id=0;
      tech->icon=NULL;
      tech->infotext=NULL;
      tech->name=NULL;
      tech->techlevelget=1024000000;
      for (i=0; i<waffenanzahl; i++) tech->unitimprovement.weapons[i]=1024;
      tech->unitimprovement.armor=1024;
      tech->requiretechnologyid[0]=0;
   };   


   printf ("\n    enter technology-id  : \n");
   num_ed ( tech->id , 0, 65534);


   printf ("\n  change infotext ? : \n");
   i=0;
   yn_switch ("no", "yes", 0, 1, i);
   if (i) {
      char del = 0;
      if ( tech->infotext ){
         printf ("\n  delete infotext or include a new one ? : \n");
         i=1;
         yn_switch ("new", "delete", 0, 1, del);
      }

      if ( del ) {
         free (tech->infotext);
         tech->infotext=NULL;
      } else {
         free (tech->infotext);
         tech->infotext=NULL;

         tfile infotextfile;
         fileselect("*.txt", _A_NORMAL, infotextfile);
         readtextfile (infotextfile.name, tech->infotext, 0);
      }
   }
      

   printf ("\n  change full screen picture ? : \n");
   printf("only the filename of the picture will be stored, not the picture itself\n");
   i=0;
   yn_switch ("no", "yes", 0, 1, i);
   if (i) {
      char del = 0;
      if ( tech->pictfilename ){
         printf ("\n  delete picture or include a reference to a new one ? : \n");
         yn_switch ("new", "delete", 0, 1, del);
      }

      if ( del ) {
         free (tech->pictfilename);
         tech->pictfilename =NULL;
      } else {
         free (tech->pictfilename);
         tech->pictfilename =NULL;

         tfile pictfile;
         fileselect("*.pcx", _A_NORMAL, pictfile);
         tech->pictfilename = strdup ( pictfile.name );
      }
   }


   printf ("\n  change ""small""picture ? : \n");
   i=0;
   yn_switch ("no", "yes", 0, 1, i);
   if (i) {
      char del = 0;
      if ( tech->icon ) {
         printf ("\n  delete picture or include a new one ? : \n");
         yn_switch ("new", "delete", 0, 1, del);
      }
      if ( del ) {
         free (tech->icon);
         tech->icon=NULL;
      } else {
         free (tech->icon);
         tech->icon=NULL;

         tfile iconfile;
         fileselect("*.pcx", _A_NORMAL, iconfile);
         loadicon (iconfile.name, &tech->icon);
      }
   }
      
      
   
   printf ("\n    enter researchpoints  : \n");
   num_ed (tech->researchpoints, 0 , 1024000000);


   printf ("\n    enter name  of technology: \n");
   stredit (tech->name, 25, 255, 255);


   printf ("\n    enter techlevelGET \n ( the technology will be automatically available when a level\nis started with a techlevel equal or greater than this)  : \n");
   num_ed (tech->techlevelget, 1 , 1024000000);

   printf ("\n    enter techlevelSET \n ( the techlevel will be raised to this level after the technology has been researched\nShould be much smaller then TechlevelGET\ncheck out doc/maketec.htm for further infos)  : \n");
   num_ed (tech->techlevelset, 1 , 1024000000);


   for (i=0; i<waffenanzahl; i++) {
      printf ("\n    enter weaponimprovement (basis 1024)  : %s \n", cwaffentypen[i]);
      num_ed (tech->unitimprovement.weapons[i], 0 , 65000);
   };

   
   printf ("\n    enter armorimprovement (basis 1024)  : \n");
   num_ed (tech->unitimprovement.armor, 0 , 65000);


//   boolean      requireevent; 
 
   printf ("\n    enter up to 6 technologies, which are required for researching %s   (0=no technologies) :\n", tech->name);
   _wait();
   for (i=0; i<=5 ; i++) {
      selecttechnology (i+1, tech->requiretechnologyid[i]);
      if (tech->requiretechnologyid[i]==0) {
         for ( int j = i+1; j<=5; j++) tech->requiretechnologyid[j]=0;
         break;
      }   
   };


   {
      tn_file_buf_stream stream ( datfile.name, 2 );
      writetechnology ( tech, &stream );
   }

   clearscreen(); 

   if (mode50) _setvideomode (_TEXTC80);
   return 0;
}



void *       loadpcx2(char *       filestring)
{      
  void         *p = NULL;
  byte         b; 


   initgraphics (640, 480, 8);
   b = loadpcxxy(filestring, 1, 0,0); 
   if (b == 0) { 
      p = malloc( 1800 ); 
      getimage(0,0,30,30,p); 
      _wait(); 
   } 
   return p; 

} 










void  selecttechnology(int num, int &id)
{ 
   clearscreen ();
   printf ("select technology  %i : \n", num); 

   int tanzahl=0;
   ptechnology  technology; 

   tfindfile ff ( "*.tec" );
   char *c = ff.getnextname();

   while ( c ) {
      tanzahl++;
      technology = loadtechnology( c );
      printf("%3i %28s |    ", technology->id, technology->name);
      if (tanzahl % 2 ==0) printf("\n");
      c = ff.getnextname();
   } 
   printf (" enter id of technology  : ");
   num_ed (id,0,10000);
}










void     loadicon (char *filename, void **icon)
{
   initgraphics (800, 600, 8);
   bar( 0, 0, 799, 599, 255 );
   loadpcxxy( filename, 1, 20, 20 );

   *icon = malloc ( 100000 );
   initmousehandler();
   mousevisible( true );

   do {

      if ( mouseparams.taste == 1 ) {
        mousevisible(false);
        int x = mouseparams.x;
        int y = mouseparams.y;
        int x1 = x;
        while ( getpixel ( x1-1, y ) != 255 )
           x1--;

        int x2 = x;
        while ( getpixel ( x2+1, y ) != 255 )
           x2++;

        int y1 = y;
        while ( getpixel ( x, y1-1 ) != 255 )
           y1--;

        int y2 = y;
        while ( getpixel ( x, y2+1 ) != 255 )
           y2++;
        
        getimage( x1,y1,x2,y2, *icon );


        rectangle( x1,y1,x2,y2, 14 );

        while ( mouseparams.taste );

        mousevisible(true);

      }

   } while ( !kbhit() ); /* enddo */

   getch ();
   settxt50mode (); 
};



