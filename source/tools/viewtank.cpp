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

                                
/*
 * Generated by TPTC - Translate Pascal to C
 *     Version 1.7 03/26/88   (C) 1988 S.H.Smith
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <dos.h>

#include "tpascal.inc"
#include "typen.h"
#include "misc.h"
#include "newfont.h"
#include "vesa.h"
#include "sgstream.h"
#include "basestrm.h"
#include "stack.h"

  #define palpath "d:\\watcom\\sg\\palette.pal"
  #define la   80  
  #define SColor 174  
  #define RColor 223  
  #define PowColor 73  
  #define PipColor 164  

               dacpalette256 pal;


#define mntd(parm1,parm2)   sprintf( buf, "parm1: %d ", parm2 -> parm1 )
#define mnts(parm1,parm2)   sprintf( buf, "parm1: %s ", parm2 -> parm1 )


void loadpalette ( void )
{
   tnfilestream stream ( palpath, 1 );
   stream.readdata ( &pal, sizeof ( pal ));
   setvgapalette256 ( pal );
}

   int grm = 0;

void setgraph ( void )
{
   if (grm != 103) {
      initsvga(0x103);
      grm = 103;
   } 
   loadpalette ();
   bar(0,0,799,600,white);
}

main(int argc, char *argv[], char *envp[])
{

   initmisc();
   char buf[256];

   if (argc == 0) {
      printf("Syntax: VIEWTANK dateiname \n"); 
      return (1);
   } 

   pfont fnt;
   {
      tnfilestream stream ( "smalaril.fnt", 1 );
      fnt = loadfont  ( &stream );
   }
   activefontsettings.background = 255;
   activefontsettings.color = black;
   activefontsettings.length = 0;
   activefontsettings.font = fnt;


   for (int par = 1; par < argc; par++) {

       find_t  fileinfo;
       unsigned rc;        /* return code */
     
       rc = _dos_findfirst( argv[ par ], _A_NORMAL, &fileinfo );
       if ( rc ) {
           printf("Syntax: VIEWTANK dateiname \n"); 
           return (1);
        } 
        while( rc == 0 ) {
         char* suffix = fileinfo.name;
         while ( *suffix != '.' && *suffix )
            suffix++;

         if (strcmpi(suffix,".TNK") == 0) {
            pvehicletype tnk = loadvehicletypetype(fileinfo.name);
            if (loaderror == 0) { 
               setgraph();

               for (int i = 0; i <= 8; i++) {
                  putrotspriteimage(la + 50 * (i),20,tnk->picture[0],i * 8); 
                  putrotspriteimage(la + 50 * (i),70,tnk->picture[1],i * 8); 

                  putrotspriteimage90(la + 50 * (i),120,tnk->picture[0],i * 8); 
                  putrotspriteimage90(la + 50 * (i),170,tnk->picture[1],i * 8); 

                  putrotspriteimage180(la + 50 * (i),220,tnk->picture[0],i * 8); 
                  putrotspriteimage180(la + 50 * (i),270,tnk->picture[1],i * 8); 

                  putrotspriteimage270(la + 50 * (i),320,tnk->picture[0],i * 8); 
                  putrotspriteimage270(la + 50 * (i),370,tnk->picture[1],i * 8); 
               } 
               if (tnk->name) {
                  sprintf( buf, "Name: %s ", tnk->name );
                  showtext2( buf, 20,420 );
               }
               if (tnk->description) {
                  sprintf( buf, "Descr.: %s ", tnk->description );
                  showtext2( buf, 20,450 );
               }
               mntd ( id, tnk );
               showtext2( buf, 400, 420 );
               mntd ( armor, tnk );
               showtext2( buf, 400, 450 );
               mntd ( view,tnk );
               showtext2( buf, 400, 480 );
               mntd ( jamming ,tnk);
               showtext2( buf, 400, 510 );

               int  ch = getch();
            } 
         } 

         if (strcmpi( suffix,".bdt") == 0) {
            pterraintype bdt = loadbodentyp(fileinfo.name);
            if (loaderror == 0) { 
               setgraph();
               for (int i = 0; i < cwettertypennum; i++)
                 if (bdt->weather[i]) {
                   for (int j = 0; j <= 7; j++)
                     if (bdt->weather[i]->picture[j])
                        putspriteimage(40 + i * 50,40 + j * 50, bdt->weather[i]->picture[j]);
                   itoa ( bdt->weather[i]->art, buf, 16 );
                   activefontsettings.length = 50;
                   showtext2 ( buf, 40 + i * 50, 450 );
                 }
               activefontsettings.length = 0;
               if (bdt->name) {
                  sprintf( buf, "Name: %s ", bdt->name );
                  showtext2( buf, 490,20 );
               }
               sprintf( buf, "ID: %d ", bdt->id );
               showtext2( buf, 490, 50 );

               int pos = 0;
               activefontsettings.length = 130;
               showtext2 ( "ART f�r Wetter 0:", 490, 190 );
               for ( i = 0; i < cbodenartennum;i++ )   
                  if ( bdt->weather[0]->art & ( 1 << i )) {
                     showtext2 ( cbodenarten[i], 500, 220 + pos * 30 );
                     pos++;
                  }
               activefontsettings.length = 0;

               int ch = getch();
            } 
         } 
         if (strcmpi(suffix,".bld") == 0) {
            pbuildingtype bld = loadbuildingtype( fileinfo.name );
            if (loaderror == 0) { 
               setgraph();
               for (int k = 0; k < bld->construction_steps; k++)
                  for (int d = 0; d <= 5; d++)
                     for (int c = 0; c <= 3; c++)
                        if (bld->picture[k][c][d] ) 
                           putspriteimage(20 + k * 150 + 40 * c + ((d & 1) * 20),20 + 20 * d,bld->picture[k][c][d]); 

               if (bld->name) {
                  sprintf( buf, "Name: %s ", bld->name );
                  showtext2( buf, 20,420 );
               }
               mntd ( id , bld);
               showtext2( buf, 400, 420 );
               mntd ( armor , bld);
               showtext2( buf, 400, 450 );
               mntd ( view , bld);
               showtext2( buf, 400, 480 );
               mntd ( jamming, bld);
               showtext2( buf, 400, 510 );

               mntd ( tank.energy , bld);
               showtext2( buf, 20, 450 );
               mntd ( tank.material , bld);
               showtext2( buf, 20, 480 );
               mntd ( tank.fuel , bld);
               showtext2( buf, 20, 510 );

               int ch = getch();
            } 
         } 
         if (strcmpi(suffix,".raw") == 0) {
            void* p;
            tnfilestream stream ( fileinfo.name , 1);
            int i;
            stream.readrlepict( &p, false, &i);
            if (grm != 101) {
               initsvga( 0x101 );
               grm = 101;
            } 
            loadpalette ();
            bar(0,0,639,480, white);

            putspriteimage(10,10,p); 
            showtext2(fileinfo.name,40,350);
            int ch = getch();
             
         } 
/*         if (strcmpi(suffix,".icn") == 0) {
            pguiicon icn = loadguiicon(fileinfo.name);
            if (loaderror == 0) { 
               setgraph();
               for (int j = 0; j <= 1; j++)
                  putspriteimage(40 ,40 + j * 60,icn->picture[j]); 
               showtext2(icn->txt,40,200);
               int ch = getch();
            } 
         } */
         
         if (strcmpi(suffix,".tec") == 0) { 
            ptechnology tec = loadtechnology(fileinfo.name); 
            if (loaderror == 0) { 
               if (grm != 101) { 
                  initsvga(0x101); 
                  grm = 101; 
               } 
               loadpalette(); 
               bar(0,0,639,479,white); 
               if ( tec->icon  )
                 putspriteimage(20,20,tec->icon); 

               showtext2(tec->name,40,120); 
               showtext2(strr ( tec->id ),40,200 ); 
               /*
                  showtext(scat("researchpoints: %s",strr(researchpoints)),fnt,40,240,black); 
                  showtext("infotext: ",fnt,40,260,black); 
                  strlcopy(infotext,infotext,240); 
                  strcpy(s1,strpas(infotext)); 
                  for (i = 0; i <= 2; i++) { 
                     strcpy(s2,copy(s1,1 + (i * 80),80)); 
                     showtext(s2,fnt,80,280 + (i * 20),black); 
                  } 
                  } 
                */
               int ch = getch(); 
            } 
         } 
         if (strcmpi(suffix,".pal") == 0) {
               setgraph();
               tnfilestream stream ( fileinfo.name, 1);
               stream.readdata ( &pal, sizeof ( pal ));
               setvgapalette256( pal );
               int x = 16;
               int y = 16;
               for (int i = 0; i <= 255; i++) {
                  bar(10 + (i / x) * 40,10 + (i % x) * y,15 + (i / x) * 40,15 + (i % x) * y,i); 
               } 
               int ch = getch();
             
         } 

         rc = _dos_findnext( &fileinfo );
      } 
   } 
   if (grm != 0) 
      settextmode(3);


   return 0;
}



