//     $Id: dialog.cpp,v 1.32 2000-07-02 21:04:12 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.31  2000/06/04 21:39:20  mbickel
//      Added OK button to ViewText dialog (used in "About ASC", for example)
//      Invalid command line parameters are now reported
//      new text for attack result prediction
//      Added constructors to attack functions
//
//     Revision 1.30  2000/05/30 18:39:23  mbickel
//      Added support for multiple directories
//      Moved DOS specific files to a separate directory
//
//     Revision 1.29  2000/05/23 20:40:40  mbickel
//      Removed boolean type
//
//     Revision 1.28  2000/05/22 15:40:34  mbickel
//      Included patches for Win32 version
//
//     Revision 1.27  2000/05/10 19:55:45  mbickel
//      Fixed empty loops when waiting for mouse events
//
//     Revision 1.26  2000/05/07 12:12:14  mbickel
//      New mouse option dialog
//      weapon info can now be displayed by clicking on a unit
//
//     Revision 1.25  2000/04/27 17:59:21  mbickel
//      Updated Kdevelop project file
//      Fixed some graphical errors
//
//     Revision 1.24  2000/04/27 16:25:19  mbickel
//      Attack functions cleanup
//      New vehicle categories
//      Rewrote resource production in ASC resource mode
//      Improved mine system: several mines on a single field allowed
//      Added unitctrl.* : Interface for vehicle functions
//        currently movement and height change included
//      Changed timer to SDL_GetTicks
//
//     Revision 1.23  2000/03/29 09:58:42  mbickel
//      Improved memory handling for DOS version
//      Many small changes I can't remember ;-)
//
//     Revision 1.22  2000/03/11 19:51:12  mbickel
//      Removed file name length limitation under linux
//      No weapon sound for attacked units any more (only attacker)
//
//     Revision 1.21  2000/02/03 21:15:33  mbickel
//      Fixed a crash in the new file date routines
//
//     Revision 1.20  2000/02/03 20:54:38  mbickel
//      Some cleanup
//      getfiletime now works under Linux too
//
//     Revision 1.19  2000/01/31 16:34:43  mbickel
//      now standard hotkeys in dialog boxes
//
//     Revision 1.18  2000/01/25 19:28:12  mbickel
//      Fixed bugs:
//        invalid mouse buttons reported when moving the mouse
//        missing service weapon in weapon information
//        invalid text displayed in ammo production
//        undamaged units selected in repair vehicle function
//
//      speed up when playing weapon sound
//
//     Revision 1.17  2000/01/24 17:35:42  mbickel
//      Added dummy routines for sound under DOS
//      Cleaned up weapon specification
//
//     Revision 1.16  2000/01/19 22:14:19  mbickel
//      Fixed:
//        - crash in replay
//        - invalid character highliting in showtext3
//
//     Revision 1.15  2000/01/19 22:03:34  mbickel
//      Fixed a bug in the DOS keyboard code
//
//     Revision 1.14  2000/01/06 11:19:11  mbickel
//      Worked on the Linux-port again...
//
//     Revision 1.13  2000/01/04 19:43:49  mbickel
//      Continued Linux port
//
//     Revision 1.12  2000/01/02 20:23:37  mbickel
//      Improved keyboard handling in dialog boxes under DOS
//
//     Revision 1.11  2000/01/02 19:47:05  mbickel
//      Continued Linux port
//      Fixed crash at program exit
//
//     Revision 1.10  1999/12/30 20:30:27  mbickel
//      Improved Linux port again.
//
//     Revision 1.9  1999/12/28 21:02:48  mbickel
//      Continued Linux port
//      Added KDevelop project files
//
//     Revision 1.8  1999/12/27 12:59:48  mbickel
//      new vehicle function: each weapon can now be set to not attack certain
//                            vehicles
//
//     Revision 1.7  1999/12/07 22:13:15  mbickel
//      Fixed various bugs
//      Extended BI3 map import tables
//
//     Revision 1.6  1999/11/23 21:07:26  mbickel
//      Many small bugfixes
//
//     Revision 1.5  1999/11/22 18:27:06  mbickel
//      Restructured graphics engine:
//        VESA now only for DOS
//        BASEGFX should be platform independant
//        new interface for initialization
//      Rewrote all ASM code in C++, but it is still available for the Watcom
//        versions
//      Fixed bugs in RLE decompression, BI map importer and the view calculation
//
//     Revision 1.4  1999/11/18 17:31:05  mbickel
//      Improved BI-map import translation tables
//      Moved macros to substitute Watcom specific routines into global.h
//
//     Revision 1.3  1999/11/16 17:04:00  mbickel
//     Made ASC compilable for DOS again :-)
//     Merged all the bug fixes in that I did last week
//
//     Revision 1.2  1999/11/16 03:41:20  tmwilson
//     	Added CVS keywords to most of the files.
//     	Started porting the code to Linux (ifdef'ing the DOS specific stuff)
//     	Wrote replacement routines for kbhit/getch for Linux
//     	Cleaned up parts of the code that gcc barfed on (char vs unsigned char)
//     	Added autoconf/automake capabilities
//     	Added files used by 'automake --gnu'
//
//
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


#include "config.h"                   
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#ifdef _DOS_
#include <dos.h>
#include <conio.h>
#endif

#include <stdarg.h>

#include "tpascal.inc"
#include "typen.h"
#include "keybp.h"
#include "basegfx.h"
#include "newfont.h"
#include "spfst.h"
#include "loaders.h"
#include "misc.h"
#include "mousehnd.h"
#include "loadpcx.h"
#include "stack.h"
#include "dlg_box.h"
#include "dialog.h"
#include "sgstream.h"
#include "timer.h"

#ifndef karteneditor
#include "network.h"
#include "gamedlg.h"
#endif


#define markedlettercolor red  



void         doubleline(word         x1,
                        word         y1,
                        word         x2,
                        word         y2)
{ 
   line(x1,y1,x2,y2,white);          
   line(x1 + 1,y1 + 1,x2 + 1,y2 + 1,dbluedark); 
} 


class  tstatistics : public tdialogbox {
              public:
                   word         linelength;
                   char         categories;
                   int          l[3][4];
                   int          m[3][9];
                   char         paintmode;
                   char         color1,color2,color3,color4;
                   char         action;

                   void         init ( void );
                   void         paintgraph ( void );
                   virtual void buttonpressed( char id );
                };
                                    
class  tstatisticbuildings : public tstatistics {
                public:
                   void         init ( void );
                   virtual void run ( void );
                   void         count ( void );
               };

class  tstatisticarmies : public tstatistics {
                   char         jk;
                public:
                   void         init ( void );
                   virtual void run ( void );
                   void         count ( void );
                   void         newknopfdruck ( integer      xx1,
                                                integer      yy1,
                                                integer      xx2,
                                                integer      yy2 );
               };




void         tstatisticarmies ::newknopfdruck(integer      xx1,
                           integer      yy1,
                           integer      xx2,
                           integer      yy2)
{ 
    char*      p; 
    char      kn; 
    integer      mt; 
    {
        collategraphicoperations cgo ( xx1, yy1, xx2, yy2 );
        mt = mouseparams.taste;
        mousevisible(false);
        p = new char [ imagesize ( xx1 + 1,yy1 + 1,xx2 - 2,yy2 - 2 )];
        getimage(xx1 + 1,yy1 + 1,xx2 - 2,yy2 - 2, p);
        putimage(xx1 + 3,yy1 + 3, p);

        line(xx1 - 1,yy1 - 1,xx2 + 1,yy1 - 1, 8);
        line(xx1 - 1,yy1 - 1,xx1 - 1,yy2 + 1, 8);

        line(xx2 + 1,yy1 - 1,xx2 + 1,yy2 + 1, 15);
        line(xx1 - 1,yy2 + 1,xx2 + 1,yy2 + 1, 15);

        line(xx1,yy1,xx2,yy1, 8);
        line(xx1,yy1,xx1,yy2, 8);

        line(xx2,yy1,xx2,yy2, 15);
        line(xx1,yy2,xx2,yy2, 15);
        mousevisible(true);

        knopfsuccessful = true;
        kn = true;
     }
     do { 
           if ((mouseparams.x > xx2) || (mouseparams.x < xx1) || (mouseparams.y > yy2) || (mouseparams.y < yy1)) { 
              knopfsuccessful = false; 
              kn = false; 
           } 
           if (mouseparams.taste != mt)
              kn = false;

           releasetimeslice();
     }  while (kn == true);

     {
        collategraphicoperations cgo ( xx1, yy1, xx2, yy2 );

        mousevisible(false);
        putimage(xx1 + 1,yy1 + 1,p);
        delete[] p  ;

        line(xx1 - 1,yy1 - 1,xx2 + 1,yy1 - 1, 8);
        line(xx1 - 1,yy1 - 1,xx1 - 1,yy2 + 1, 8);

        line(xx2 + 1,yy1 - 1,xx2 + 1,yy2 + 1, 15);
        line(xx1 - 1,yy2 + 1,xx2 + 1,yy2 + 1, 15);

        line(xx1,yy1,xx2,yy1, 15);
        line(xx1,yy1,xx1,yy2, 15);

        line(xx2,yy1,xx2,yy2, 8);
        line(xx1,yy2,xx2,yy2, 8);

        mousevisible(true);
     }
} 



void         tstatistics::init(void)
{ 
   tdialogbox::init();
   paintmode = 1; 
   color1 = 23 + actmap->actplayer * 8; 
   color2 = 19 + actmap->actplayer * 8; 
   color3 = 14; 
   color4 = darkgray; 
   x1 = 70; 
   xsize = 640 - 2 * x1; 
   linelength = (xsize - 95 - 20 - 10); 

   memset( l, 0, sizeof(l) );
   memset( m, 0, sizeof(m) );

   windowstyle = windowstyle ^ dlg_in3d; 
} 


void         tstatisticarmies::init(void)
{ 
  byte         i;

   tstatistics::init();
   categories = 3; 
   y1 = 25; 
   ysize = 490 - 2 * y1; 
   title = "army statistics";
   action = 0; 
   jk = 0; 

   addbutton("e~x~it",10,ysize - 35,xsize - 10,ysize - 10,0,1,2,true); 
   addbutton("~c~hange display mode",10,ysize - 65,xsize - 10,ysize - 40,0,1,1,true); 
   
   buildgraphics();
   rahmen(true,x1 + 10,y1 + 50,x1 + xsize - 10,y1 + ysize - 70); 
   for (i = 0; i <= 2; i++) 
      putimage(x1 + 20,y1 + 75 + i * 90,icons.statarmy[i]); 

} 


void         tstatisticbuildings::init(void)
{ 

   tstatistics::init();
   categories = 1; 
   y1 = 115; 
   ysize = 490 - 2 * y1; 
   title = "building statistics";

   action = 0; 

   addbutton("e~x~it",10,ysize - 35,xsize - 10,ysize - 10,0,1,2,true); 
   addbutton("~c~hange display mode",10,ysize - 65,xsize - 10,ysize - 40,0,1,1,true); 

   buildgraphics();
   rahmen(true,x1 + 10,y1 + 50,x1 + xsize - 10,y1 + ysize - 70); 
} 



void         tstatisticarmies::count(void)
{ 
  integer      j, b; 
  pvehicle     actvehicle; 

   for (j = 0; j <= 8; j++) { 
      if (j == 8) 
         b = 3; 
      else 
         if (j == actmap->actplayer) 
            b = 0; 
         else 
            if (getdiplomaticstatus(j * 8) == capeace) 
               b = 1; 
            else 
               b = 2; 
      actvehicle = actmap->player[j].firstvehicle; 
      while (actvehicle != NULL) { 
         if (godview || fieldvisiblenow(getfield(actvehicle->xpos,actvehicle->ypos))) { 
            if (actvehicle->typ->height & (chtieffliegend | chfliegend | chhochfliegend | chsatellit)) { 
               l[0][b]++; 
               m[0][j]++; 
            } 
            else 
               if (actvehicle->typ->height & (chschwimmend | chgetaucht | chtiefgetaucht) ) { 
                  l[2][b]++; 
                  m[2][j]++; 
               } 
               else { 
                  l[1][b]++; 
                  m[1][j]++; 
               } 
         } 
         actvehicle = actvehicle->next; 
      } 
   } 
} 


void         tstatisticbuildings::count(void)
{ 
  integer      j, b; 
  pbuilding    actbuilding; 

   for (j = 0; j <= 8; j++) { 
      if (j == 8) 
         b = 3; 
      else 
         if (j == actmap->actplayer) 
            b = 0; 
         else 
            if (getdiplomaticstatus(j * 8) == capeace) 
               b = 1; 
            else 
               b = 2; 
      actbuilding = actmap->player[j].firstbuilding; 
      while (actbuilding != NULL) { 
         if (godview || fieldvisiblenow(getfield(actbuilding->xpos,actbuilding->ypos))) { 
            l[0][b]++; 
            m[0][j]++; 
         } 
         actbuilding = actbuilding->next; 
      } 
   } 
} 


void         tstatistics::paintgraph(void)
{ 
   collategraphicoperations cgo ( x1, y1, x1 + xsize, y1 + ysize );
   int    linestart = x1 + 95;

  int      max; 
  integer      b, i, j, y; 

   max = 0; 
   if (paintmode == 0) { 
      for (i = 0; i <= categories - 1; i++) 
         for (j = 0; j <= 3; j++) 
            if (l[i][j] > max) max = l[i][j]; 
   } 
   else 
      for (i = 0; i <= categories - 1; i++) 
         for (j = 0; j <= 8; j++) 
            if (m[i][j] > max) max = m[i][j]; 

   if (max > 0) 
      for (i = 0; i <= categories - 1; i++) { 
         y = y1 + 60 + i * 90; 

         rahmen(true,x1 + 90,y,x1 + xsize - 20,y + 60); 
         if (paintmode == 0) { 
            bar(linestart,y + 5,linestart + linelength * l[i][0] / max,y + 15,color1); 
            bar(linestart,y + 17,linestart + linelength * l[i][1] / max,y + 27,color2); 
            bar(linestart,y + 29,linestart + linelength * l[i][2] / max,y + 39,color3); 
            bar(linestart,y + 41,linestart + linelength * l[i][3] / max,y + 51,color4); 
         } 
         else 
            for (j = 0; j <= 8; j++) 
               bar(linestart,y + 3 + j * 6,linestart + linelength * m[i][j] / max,y + 8 + j * 6,21 + j * 8); 

      } 
   else 
      y = y1 + 60 + (categories - 1) * 90; 

   doubleline(linestart,y + 80,linestart + linelength,y + 80); 
   activefontsettings.font = schriften.smallsystem;
   activefontsettings.length = 10;
   
   if ((max & 3) == 0)
     b = 4;
   else 
      if ((max % 3) == 0)
        b = 3;
      else 
        b = 2;

   activefontsettings.background = 255; 
   activefontsettings.color = color1; 
   for (i = 0; i <= b; i++) { 
      doubleline(linestart + linelength * i / b,y + 75,linestart + linelength * i / b,y + 85); 
      showtext2(strr(max * i / b),linestart + linelength * i / b - 5,y + 87); 
   } 
   activefontsettings.font = schriften.smallarial;
   activefontsettings.length = 80; 
   activefontsettings.color = white; 
   activefontsettings.background = color1; 
           
   if (paintmode == 0) { 
      showtext2("own units",linestart,y + 110);

      activefontsettings.background = color2; 
      showtext2("allies", linestart + linelength / 4,y + 110);

      activefontsettings.background = color3; 
      activefontsettings.color = black; 
      showtext2("enemys", linestart + linelength * 2 / 4,y + 110);

      activefontsettings.background = color4; 
      activefontsettings.color = white; 
      showtext2("neutral", linestart + linelength * 3 / 4,y + 110);
   } 
   else 
      paintsurface2(linestart,y + 110,linestart + linelength,y + 110 + schriften.smallarial->height);
} 


void         tstatisticarmies::run(void)
{ 
   count(); 
   paintgraph(); 
   mousevisible(true); 
   do { 
      tstatistics::run();
      if (taste == ct_esc) 
         action = 2; 
      if (taste == ct_f1) 
         help(11); 
      if ((mouseparams.taste == 1) && (mouseparams.x > x1 + 10) && (mouseparams.y > y1 + 50) && (mouseparams.x < x1 + xsize - 10) && (mouseparams.y < y1 + ysize - 70)) { 
         jk++; 
         newknopfdruck( (int) (x1 + 10) , (int) (y1 + 50) , (int) (x1 + xsize - 10) , (int) (y1 + ysize - 70)); 
         if (jk > 3) { 
            displaymessage("that makes fun, doesn't it ?  \n this will be released as a seperate game soon ! ",1);
         } 
      } 
   }  while (action < 2);
} 


void         tstatisticbuildings::run(void)
{ 
   count(); 
   paintgraph(); 
   mousevisible(true); 
   do { 
      tstatistics::run();
      if (taste == ct_esc) 
         action = 2; 
      if (taste == ct_f1) 
         help(10); 
   }  while (action < 2);
} 


void         tstatistics::buttonpressed( char id )
{ 
   if (id == 1) { 
      paintmode = 1 - paintmode; 
      mousevisible(false); 
      paintsurface ( 81, 51, xsize - 11, ysize - 72 );
      paintgraph(); 
      mousevisible(true); 
   } 
   if (id == 2) 
      action = 2; 
} 


void         statisticarmies(void)
{ 
  tstatisticarmies sta; 

   sta.init(); 
   sta.run(); 
   sta.done(); 
} 


void         statisticbuildings(void)
{ 
  tstatisticbuildings stb; 

   stb.init(); 
   stb.run(); 
   stb.done(); 
} 




class  tweaponinfo : public tdialogbox {
              public:
                   pvehicletype        aktvehicle;
                   char                weapnum;
                   char                backgrnd2;
                   void                init(pvehicletype eht, char num );
                   virtual void        run ( void );
                };


class  tvehicleinfo : public tdialogbox , public tviewtextwithscrolling {
              public:
                   char         zahlcolor;
                   char         textcol;
                   char         background;
                   char         randunten;
                   pvehicletype aktvehicle;
                   signed char  markweap;
                   char         backgrnd2;
                   word         wepx, wepy;
                   char         category;
                   char         action;
                   integer      i,j;

                   void         init ( pvehicletype type );
                   virtual void run  ( void );
                   void         lines( integer xp1, integer yp1, integer xp2, integer yp2, char* st);
                   void         zeigevehicle ( void );
                   void         showgeneralinfos( void );
                   void         showgeneralinfovariables( void );
                   void         showweapons ( void );
                   void         showweaponsvariables( void );
                   void         showclasses ( void );
                   void         showinfotext ( void );
                   void         paintmarkweap ( void );
                   virtual void buttonpressed( char id ); 
                   void         checknextunit ( void );

                   void         repaintscrollbar ( void );
                };



void         tweaponinfo::init( pvehicletype eht, char num  )
{ 
   tdialogbox::init();
   x1 = 40; 
   xsize = 640 - 2 * x1; 
   y1 = 75; 
   ysize = 480 - 2 * y1; 
   title = "weapon info";
   buildgraphics(); 
   backgrnd2 = dblue + 1;
   aktvehicle = eht; 
   weapnum = num - 1; 
   mousevisible(false); 
} 





void         tweaponinfo::run(void)
{ 
  char* suffix;
  word         i; 
  integer      xa, ya; 
  char*        strng;
  char*        strng2;


   strng = new char [ 200 ];
   strng2= new char [ 200 ];
   activefontsettings.color = black; 
   activefontsettings.background = 255; 
   activefontsettings.justify = lefttext; 
   showtext2("type: ",x1 + 30,y1 + starty + 20); 
   showtext2("maxstrenght: ",x1 + 30,y1 + starty + 75); 
   showtext2("count: ",x1 + 30,y1 + starty + 130); 
   showtext2("shootable from which heights:",x1 + 300,y1 + starty + 20); 
   showtext2("targets:",x1 + 300,y1 + starty + 75); 
   showtext2("unit can shoot:",x1 + 30,y1 + starty + 185); 
   showtext2("unit can refuel other units:",x1 + 30,y1 + starty + 240); 


   if (aktvehicle->weapons->weapon[weapnum].maxstrength > 0) 
      if (aktvehicle->weapons->weapon[weapnum].getScalarWeaponType() >= 0 ) { 

         line(x1 + 300,y1 + starty + 130,x1 + 300,y1 + starty + 260,black); 
         putspriteimage(x1 + 293,y1 + starty + 130,icons.weapinfo.pfeil1); 
         line(x1 + 297,y1 + starty + 140,x1 + 303,y1 + starty + 140,black); 
         line(x1 + 297,y1 + starty + 260,x1 + 303,y1 + starty + 260,black); 

         line(x1 + 310,y1 + starty + 260,x1 + 520,y1 + starty + 260,black); 
         putrotspriteimage90(x1 + 516,y1 + starty + 253,icons.weapinfo.pfeil1,0); 

         line(x1 + 325,y1 + starty + 257,x1 + 325,y1 + starty + 263,black); 
         line(x1 + 510,y1 + starty + 257,x1 + 510,y1 + starty + 263,black); 


         /*
         showtext(strrd8(aktvehicle->weapons->weapon[weapnum].mindistance),schriften.smallsystem,x1 + 320,y1 + starty + 270,black);
         showtext(strrd8(aktvehicle->weapons->weapon[weapnum].maxdistance),schriften.smallsystem,x1 + 500,y1 + starty + 270,black); 
         */
         activefontsettings.justify = centertext; 

         showtext2("distance",x1 + 410,y1 + starty + 270);
         activefontsettings.direction = 90; 

         showtext2("strength", x1 + 285,y1 + starty + 200);
         activefontsettings.direction = 0; 
         activefontsettings.justify = righttext; 
         itoa(aktvehicle->weapons->weapon[weapnum].maxstrength, strng, 10 );
         showtext2(strng, x1 + 292,y1 + starty + 137);
         showtext2("0", x1 + 292,y1 + starty + 257);

            for (i = 0; i <= 255; i++) { 
               xa = x1 + 325 + (510 - 325) * i / 256;
               ya = y1 + starty + 260 - (260 - 140) * 
                   weapdist->getweapstrength(&aktvehicle->weapons->weapon[weapnum], aktvehicle->weapons->weapon[weapnum].mindistance + ( aktvehicle->weapons->weapon[weapnum].maxdistance - aktvehicle->weapons->weapon[weapnum].mindistance) * i / 256, -1, -1 )
                   / 256;
               putpixel(xa,ya,darkgray); 
             
         } 
      } 
      


   activefontsettings.color = black; 
   activefontsettings.background = backgrnd2; 
   activefontsettings.justify = lefttext; 
   activefontsettings.length = 170;

   if (aktvehicle->height >= chtieffliegend) 
      suffix = "air to ";
   else 
      suffix = "ground to ";

   if (!(aktvehicle->weapons->weapon[weapnum].getScalarWeaponType() == cwgroundmissilen || 
         aktvehicle->weapons->weapon[weapnum].getScalarWeaponType() == cwairmissilen ))
      suffix = "";                         
   activefontsettings.color = black; 
   if ( !aktvehicle->weapons->weapon[weapnum].shootable() )
      showtext2("no",x1 + 50,y1 + starty + 205); 
   else { 
      showtext2("yes",x1 + 50,y1 + starty + 205); 
   } 

   if ( aktvehicle->weapons->weapon[weapnum].canRefuel() ) { 
      activefontsettings.length = 10;
      showtext2("yes",x1 + 50,y1 + starty + 260); 
   } 
   else 
      showtext2("no",x1 + 50,y1 + starty + 260); 

   activefontsettings.length = 170;
   strcpy ( strng, suffix );
   int k = aktvehicle->weapons->weapon[weapnum].getScalarWeaponType();
      if ( k < cwaffentypennum) {
         strcat( strng, cwaffentypen[k] );
         showtext2( strng, x1 + 50,y1 + starty + 40);
      };

   itoa(aktvehicle->weapons->weapon[weapnum].maxstrength , strng, 10 );
   showtext2(strng ,x1 + 50,y1 + starty + 90);

   itoa(aktvehicle->weapons->weapon[weapnum].count , strng, 10 );
   showtext2(strng ,x1 + 50,y1 + starty + 145);


   bar(x1 + 330,y1 + starty + 40,x1 + 550,y1 + starty + 70,dblue);   /* ## */ 
   k = 0; 
   for (i = 0; i <= 7; i++) 
      if (aktvehicle->weapons->weapon[weapnum].sourceheight & (1 << i)) { 
         putimage(x1 + 330 + k * 35,y1 + starty + 40,icons.height[i]); 
         k++; 
      } 

   bar(x1 + 330,y1 + starty + 95,x1 + 550,y1 + starty + 125,dblue);   /* ## */ 
   k = 0; 
   for (i = 0; i <= 7; i++) 
      if (aktvehicle->weapons->weapon[weapnum].targ & (1 << i) ) { 
         putimage(x1 + 330 + k * 35,y1 + starty + 95,icons.height[i]); 
         k++; 
      } 

   delete[]  strng ;
   delete[]  strng2;

   do {
      releasetimeslice();
   }  while (!((mouseparams.taste == 0) || keypress()) );
   mousevisible(true); 
   k = 0;                                  

   do { 
      tdialogbox::run();
   }  while (!((taste != ct_invvalue) || (mouseparams.taste == 1))); 
   if (taste == ct_invvalue) 
      do {
        releasetimeslice();
      }  while (!(mouseparams.taste == 0)); 

   mousevisible(false); 

} 





void         tvehicleinfo::init( pvehicletype type )
{ 

   aktvehicle = type;
   tdialogbox::init();
   category = 0;
   x1 = 20; 
   xsize = 600; 
   y1 = 15; 
   ysize = 460;
   action = 0; 
   randunten = 35; 
   windowstyle = windowstyle ^ dlg_in3d; 
   title = "vehicle info";
   addbutton("e~x~it",50,ysize - randunten + 5,xsize - 50,ysize - 5,0,1,1,true); 
   addbutton("",10,ysize - randunten + 5,40,ysize - 5,0,2,2,true); 
   addkey(2,ct_left); 
   addbutton("",xsize - 40,ysize - randunten + 5,xsize - 10,ysize - 5,0,2,3,true); 
   addbutton("general infos", 10, ysize - randunten- 25, (xsize - 50) / 4 + 10 , ysize - randunten - 5, 0,1,4,true);
   addbutton("weapons", (xsize - 50) / 4 + 20, ysize - randunten- 25, (xsize - 50) * 2 / 4 + 20 , ysize - randunten - 5, 0,1,5,true);
   addbutton("classes", (xsize - 50) * 2 / 4 + 30, ysize - randunten- 25, (xsize - 50) * 3 / 4 + 30 , ysize - randunten - 5, 0,1,6,true);
   addbutton("info",    (xsize - 50) * 3 / 4 + 40, ysize - randunten- 25, xsize - 10 , ysize - randunten - 5, 0,1,7,true);
   addkey(3,ct_right); 

   textsizey = ( ysize - 65) - ( starty + 55 );

   #ifdef NEWKEYB
   addscrollbar(xsize - 35, starty + 55 , xsize - 20,ysize - 65, &textsizeycomplete, textsizey, &tvt_starty,8,0);
   #else
   addscrollbar(xsize - 35, starty + 55 , xsize - 20,ysize - 65, &textsizeycomplete, textsizey, &tvt_starty,8,1);
   #endif
   setscrollspeed( 8 , 1 );

   hidebutton ( 8 );


   buildgraphics();



   line( x1 + 15, y1 + starty + 45, x1 + xsize - 15, y1 + starty + 45, darkgray);
   line( x1 + 15, y1 + starty + 46, x1 + xsize - 15, y1 + starty + 46, white);
   

   rahmen(true,x1 + 5,y1 + starty,x1 + xsize - 5,y1 + ysize - randunten); 
   putrotspriteimage(x1 + 565,y1 + starty + 395,icons.weapinfo.pfeil2, actmap->actplayer * 8);
   putrotspriteimage180(x1 + 12,y1 + starty + 395,icons.weapinfo.pfeil2, actmap->actplayer * 8);

   background= dblue;
   textcol = black;
   backgrnd2 = dblue + 1;


   wepx = x1 + 30; 
   wepy = y1 + starty + 70; 
   markweap = 0; 
} 


void         tvehicleinfo::lines(integer      xp1,
                                 integer      yp1,
                                 integer      xp2,
                                 integer      yp2,
                                 char *       st)
{ 
  collategraphicoperations cgo ( xp1, yp1, xp2, yp2);
   rahmen(true,xp1,yp1,xp2,yp2);
   rahmen(false,xp1 + 1,yp1 + 1,xp2 - 1,yp2 - 1); 
} 


  #define wepshown 6  
  #define movespeed 20  


#define column2x ( x1 + 420 - 20 )
#define column1x ( x1 + 50 - 20 )


#define productioncostx ( column1x )
#define productioncosty ( y1 + starty + 55 )


#define graphx1 (x1 + 330)
#define graphy1 (wepy )
#define graphx2 (x1 + 580)
#define graphy2 (wepy + 160)
 


void         tvehicleinfo::paintmarkweap(void)
{ 
  collategraphicoperations cgo ( x1, y1, x1 + xsize, y1 + ysize );
  byte         mas;
  int ii;
  char strng[100];

   mas = getmousestatus(); 
   if (mas == 2) 
      mousevisible(false); 

   activefontsettings.font = schriften.smallarial; 
   activefontsettings.color = textcol; 
   activefontsettings.justify = lefttext;

   if (aktvehicle->weapons->count ) 

      for (ii = 0; ii < aktvehicle->weapons->count ; ii++) { 

         if (ii == markweap)
            activefontsettings.background = white;
         else
            activefontsettings.background = dblue;

         if (aktvehicle->height >= chtieffliegend) 
            strcpy (strng,  "air to ");
         else 
            strcpy(strng, "ground to ");

         if (!(aktvehicle->weapons->weapon[ii].getScalarWeaponType() == cwgroundmissilen || 
               aktvehicle->weapons->weapon[ii].getScalarWeaponType() == cwairmissilen ))
            strng[0] = 0;

         activefontsettings.color = textcol; 
         if ( !aktvehicle->weapons->weapon[ii].shootable() )
            activefontsettings.color = darkgray; 

         activefontsettings.length = 190;
         if ( aktvehicle->weapons->weapon[ii].getScalarWeaponType() >= 0 )
            strcat( strng, cwaffentypen[ aktvehicle->weapons->weapon[ii].getScalarWeaponType() ] );
         else
            if ( aktvehicle->weapons->weapon[ii].service() )
               strcat( strng, cwaffentypen[ cwservicen ] );

         showtext2( strng, wepx + 20, wepy + ii * 20);


         activefontsettings.length = 40;
         itoa(aktvehicle->weapons->weapon[ii].maxstrength , strng, 10 );
         showtext2(strng ,wepx + 210, wepy + ii * 20);
      
         itoa(aktvehicle->weapons->weapon[ii].count , strng, 10 );
         showtext2(strng ,wepx + 250, wepy + ii * 20);

      } 
    
   for (ii = aktvehicle->weapons->count; ii <= 7; ii++)
       paintsurface2(wepx + 20 ,wepy + ii * 20,wepx + 290,wepy + 20 + ii * 20);


   showweaponsvariables();


   if (mas == 2) 
      mousevisible(true); 


} 

void         tvehicleinfo::repaintscrollbar ( void )
{
   enablebutton ( 8 );
}

extern dacpalette256 pal;

void         tvehicleinfo::zeigevehicle(void)
{ 
  collategraphicoperations cgo ( x1, y1, x1 + xsize, y1 + ysize );;

      activefontsettings.font = schriften.smallarial; 
      activefontsettings.length = 240;
      activefontsettings.background = backgrnd2; 
      activefontsettings.color = textcol; 
   activefontsettings.justify = righttext;
   
   if ( aktvehicle->name )
      showtext2(aktvehicle->name,x1 + 20,y1 + starty + 15);
   else
      showtext2("",x1 + 20,y1 + starty + 15);

   activefontsettings.justify = lefttext; 
   if ( aktvehicle->description )
      showtext2(aktvehicle->description, x1 + xsize - 260, y1 + starty + 15);
   else
      showtext2("", x1 + xsize - 260, y1 + starty + 15);

   bar(x1 + xsize/2 - 20 ,y1 + starty + 5, x1 + xsize/2 + 20, y1 + starty + 40,backgrnd2);

   #ifdef HEXAGON
   putimageprt ( x1 + xsize/2 - 20 ,y1 + starty + 5, x1 + xsize/2 + 20, y1 + starty + 40, aktvehicle->picture[0], (fieldxsize - 40) / 2, (fieldysize - 40) / 2 );
/*   TrueColorImage* zimg = zoomimage ( aktvehicle->picture[0], 30, 30, pal, 0 );
   void* pic = convertimage ( zimg, pal ) ;
   putrotspriteimage(x1 + xsize/2 - 15 ,y1 + starty + 7, pic, actmap->actplayer * 8);
   delete pic;
   delete zimg;*/
   #else
   putrotspriteimage(x1 + xsize/2 - 15 ,y1 + starty + 7,aktvehicle->picture[0],actmap->actplayer * 8);
   #endif
                         
}

void tvehicleinfo::showgeneralinfos ( void ) 
{
  collategraphicoperations cgo ( x1, y1, x1 + xsize, y1 + ysize );;
 char mss = getmousestatus();
 if (mss == 2)
    mousevisible(false);
 
  paintsurface ( 11, starty + 48, xsize - 11, ysize - randunten- 30  );




   activefontsettings.color = textcol; 
   activefontsettings.background = 255; 
   activefontsettings.font = schriften.smallarial; 
   activefontsettings.markcolor = red; 
 
   activefontsettings.justify = lefttext;
   activefontsettings.length = 200;
   showtext2("production cost", productioncostx , productioncosty );
   showtext2("material:", productioncostx + 10, productioncosty + 20);
   showtext2("energy:",   productioncostx + 10, productioncosty + 40);

   showtext2("transportation ", column1x,  productioncosty + 70);
   showtext2("material:", column1x + 10, productioncosty + 90);
   showtext2("energy:", column1x + 10, productioncosty + 110);
   showtext2("fuel:", column1x + 10, productioncosty + 130);
   showtext2("units (mass):", column1x + 10, productioncosty + 150);
   showtext2("units (height):", column1x + 10, productioncosty + 170);

   showtext2("movement ", column1x,  productioncosty + 200);
   showtext2("height:", column1x + 10,  productioncosty + 220 );
   showtext2("distance:", column1x + 10,  productioncosty + 240 );
   showtext2("fuel usage:", column1x + 10,  productioncosty + 260 );
   showtext2("ascent :", column1x + 10, productioncosty + 280 );


   showtext2("sight", column2x, productioncosty );
   showtext2("view:", column2x+10, productioncosty + 20);
   showtext2("jamming:", column2x+10, productioncosty + 40);

   showtext2("masses", column2x, productioncosty + 70 );
   showtext2("armor:", column2x + 10, productioncosty + 90 );


   showtext2("category:", column2x - 50, productioncosty + 260 );





  showgeneralinfovariables( );

 if (mss == 2)
    mousevisible(true);

}

void  tvehicleinfo::showgeneralinfovariables( void )
{
  collategraphicoperations cgo ( x1, y1, x1 + xsize, y1 + ysize );;
 int          ii, kk;
 void*      q; 
 char         strng[100];

      activefontsettings.length = 50;
      activefontsettings.background = backgrnd2; 
      activefontsettings.justify = lefttext; 
      activefontsettings.color = textcol; 
   

   activefontsettings.length = 30;

   for (ii = 7; ii >= 0; ii--) { 
      if (aktvehicle->height & (1 << ii) ) 
         q = icons.height[ii];
      else 
         q = xlatpict(xlatpictgraytable,icons.height[ii]);

      putimage(x1 + 155 + ii * 37,y1 + starty + 273,q);

      strrd8d(aktvehicle->movement[ii], strng );
      showtext2(strng, x1 + 155 + ii * 37,y1 + starty + 297);

   } 


   for (ii = 7; ii >= 0; ii--) { 
      if (aktvehicle->loadcapability & (1 << ii) )
         q = icons.height[ii];
      else 
         q = xlatpict(xlatpictgraytable,icons.height[ii]);

      putimage(x1 + 155 + ii * 37,y1 + starty + 225,q);

   } 


   activefontsettings.length = 150;
   activefontsettings.justify = centertext;

   kk=0;
   for (ii = 0; ii < cvehiclefunctionsnum; ii++)
      if ((aktvehicle->functions & cfvehiclefunctionsanzeige) & (1 << ii ) ) { 
         showtext2( cvehiclefunctions[ii], x1 + xsize/2 - activefontsettings.length/2, y1 + starty + 60 + kk * 20);
         kk++;
      } 

   paintsurface ( xsize/2 - activefontsettings.length/2, starty + 60 + kk * 20, xsize/2 + activefontsettings.length/2, starty + 60 + 6 * 20 );


   activefontsettings.justify = righttext;
   activefontsettings.length = 70;
   showtext2( strrr(aktvehicle->production.material), productioncostx + 100, productioncosty + 20);
   showtext2( strrr(aktvehicle->production.energy),   productioncostx + 100, productioncosty + 40);

   showtext2( strrr(aktvehicle->material), column1x + 100, productioncosty + 90);
   showtext2( strrr(aktvehicle->energy), column1x + 100, productioncosty + 110);
   showtext2( strrr(aktvehicle->tank), column1x + 100, productioncosty + 130);
   showtext2( strrr(aktvehicle->loadcapacity), column1x + 100, productioncosty + 150);
   showtext2( strrr(aktvehicle->fuelconsumption), column1x + 100,  productioncosty + 260 );
   showtext2( strrr(aktvehicle->steigung), column1x + 100, productioncosty + 280 );


   showtext2( strrr(aktvehicle->view), column2x + 100, productioncosty + 20);
   showtext2( strrr(aktvehicle->jamming), column2x + 100, productioncosty + 40);

   showtext2( strrr(aktvehicle->armor), column2x + 100, productioncosty + 90 );
   showtext2( strrr(aktvehicle->weight), column2x + 100, productioncosty + 110 );

   activefontsettings.length = 70+50;
   showtext2( cmovemalitypes[aktvehicle->movemalustyp], column2x + 50, productioncosty + 260 );


   npush ( activefontsettings );

   activefontsettings.background = lightgray;
   activefontsettings.color = black;
   activefontsettings.justify = lefttext;
   activefontsettings.length = 70;

   if (aktvehicle->height >= chfahrend) 
      showtext2("weight :",column2x + 10, productioncosty + 110);
   else 
      showtext2("draught :",column2x + 10, productioncosty + 110);

   npop ( activefontsettings );
   return;




//   strng = (char*) malloc ( 200 );
/*         
   itoa ( aktvehicle->production.energy, strng, 10 );
   showtext2(strng, x1 + 490,y1 + starty + 5);

   itoa ( aktvehicle->production.material, strng, 10 );
   showtext2(strng, x1 + 490,y1 + starty + 25);

   itoa ( aktvehicle->tank, strng, 10 );
   showtext2(strng, x1 + 490,y1 + starty + 70);

   itoa ( aktvehicle->fuelconsumption, strng, 10 );
   showtext2(strng, x1 + 490,y1 + starty + 90);
   itoa ( aktvehicle->steigung, strng, 10 );
   showtext2(strng, x1 + 490,y1 + starty + 110);

   strrd8d( aktvehicle->view, strng );
   showtext2(strng, x1 + 490,y1 + starty + 140);

   strrd8d( aktvehicle->jamming, strng );
   showtext2(strng, x1 + 490,y1 + starty + 160);

   itoa ( aktvehicle->armor, strng, 10 );
   showtext2(strng, x1 + 490,y1 + starty + 190);

   if (aktvehicle->wait) 
      showtext2("yes",x1 + 490,y1 + starty + 230); 
   else 
      showtext2("no",x1 + 490,y1 + starty + 230); 

   push(activefontsettings,sizeof(activefontsettings)); 
      activefontsettings.color = textcol; 
      activefontsettings.background = dblue; 
      activefontsettings.font = schriften.smallarial; 
      activefontsettings.justify = righttext; 
      activefontsettings.length = 60;
   
   if (aktvehicle->height >= chfahrend) 
      showtext2("weight :",wepx + 390, y1 + starty + 210);
   else 
      showtext2("draught :",wepx + 390, y1 + starty + 210);
   pop(activefontsettings,sizeof(activefontsettings)); 

   itoa ( aktvehicle->weight, strng, 10 );
   showtext2(strng, x1 + 490,y1 + starty + 210);

   itoa ( aktvehicle->loadcapacity, strng, 10 );
   showtext2(strng, x1 + 490,y1 + starty + 300);

   kk = 0; 
   bar(x1 + 130,y1 + starty + 265,x1 + 350,y1 + starty + 295,background); 
   if (aktvehicle->loadcapacity ) { 
      for (ii = 0; ii <= 7; ii++) 
         if (aktvehicle->loadcapability & (1 << ii) ) { 
            putimage(x1 + 130 + kk * 35,y1 + starty + 265,icons.height[ii]); 
            kk++; 
         } 
   } 

   itoa ( aktvehicle->energy, strng, 10 );
   showtext2(strng, x1 + 490,y1 + starty + 260);

   itoa ( aktvehicle->material, strng, 10 );
   showtext2(strng, x1 + 490,y1 + starty + 280);



   activefontsettings.length = 150;
   kk = 0; 

   bar(wepx + 10,y1 + starty + 320,wepx + 330,y1 + starty + 360,background); 




   activefontsettings.justify = lefttext; 
   if (aktvehicle->weapons->count == 0) 
      bar(wepx + 10,wepy,wepx + 340,wepy + 20 + wepshown * 20,background); 
   else { 
      kk = aktvehicle->weapons->count - 1; 
      if (kk > wepshown) kk = wepshown; 
      for (ii = 0; ii <= kk; ii++) { 
         if (aktvehicle->height >= chtieffliegend) 
            strcpy (strng,  "air to ");
         else 
            strcpy(strng, "ground to ");
         if ((aktvehicle->weapons->weapon[ii].typ & (cwgroundmissileb | cwairmissileb)) == 0)
            strng[0] = 0;
         jj = aktvehicle->weapons->weapon[ii].typ; 
         activefontsettings.color = textcol; 
         if ((jj & cwshootableb) == 0)
            activefontsettings.color = darkgray; 
         else 
            jj &= ~cwshootableb;

         if (jj & cwammunitionb ) { 
            jj &= ~cwammunitionb;
            activefontsettings.length = 10;
            showtext2( letter[0], wepx + 120,wepy + ii * 20);
         } 
         activefontsettings.length = 170;
         for (ll = 0; ll <= 15; ll++) 
            if (jj & (1 << ll) ) 
               if ( ll < cwaffentypennum) {
                  strcat( strng, cwaffentypen[ ll ] );
                  showtext2( strng, wepx + 20, wepy + ii * 20);
               }
         activefontsettings.length =  30;

         itoa ( aktvehicle->weapons->weapon[ii].maxstrength, strng , 10);
         showtext2(strng, wepx + 195,wepy + ii * 20);

         strrd8u ( aktvehicle->weapons->weapon[ii].mindistance, strng );
         showtext2(strng, wepx + 230, wepy + ii * 20);

         strrd8d ( aktvehicle->weapons->weapon[ii].maxdistance, strng );
         showtext2(strng, wepx + 265, wepy + ii * 20);

         itoa ( aktvehicle->weapons->weapon[ii].count, strng ,10);
         showtext2(strng, wepx + 300,wepy + ii * 20);
      } 
      if (aktvehicle->weapons->count < 8) 
         for (ii = aktvehicle->weapons->count; ii <= wepshown; ii++) 
            bar(wepx + 10,wepy + ii * 20,wepx + 340,wepy + 20 + ii * 20,dblue); 
   } 
  */
} 




void tvehicleinfo::showweapons ( void )
{                             
  collategraphicoperations cgo ( x1, y1, x1 + xsize, y1 + ysize );;

 char mss = getmousestatus();
 if (mss == 2)
    mousevisible(false);

  paintsurface ( 11, starty + 48, xsize - 11, ysize - randunten- 30  );

  activefontsettings.color = textcol; 
  activefontsettings.background = 255;
  activefontsettings.font = schriften.smallarial; 
  activefontsettings.justify = lefttext;
  activefontsettings.length = 200;

/*   showtext2("maxstrenght: ",wepx + 10, wepy + 160);
   showtext2("count: ",wepx + 10,y1 + starty + 130); */

   showtext2("sourceheight:", wepx + 10, y1 + starty + 305);
   showtext2("targheight:", wepx + 10 ,y1 + starty + 330);

   showtext2("unit can shoot:",wepx + 10, wepy + 160);
   showtext2("unit can refuel other units:", wepx + 10, wepy + 180);


  paintmarkweap();

 if (mss == 2)
    mousevisible(true);

}


void tvehicleinfo::showweaponsvariables( void )
{
  collategraphicoperations cgo ( x1, y1, x1 + xsize, y1 + ysize );;

   char strng[100];
   int xa,ya,ii;
      
   activefontsettings.color = textcol; 
   activefontsettings.background = dblue; 
   activefontsettings.font = schriften.smallarial; 
   activefontsettings.justify = righttext; 
   activefontsettings.length = 60;


   if (aktvehicle->weapons->count) {

      void *q;
   
      for (ii = 7; ii >= 0; ii--) { 
         if (aktvehicle->weapons->weapon[markweap].sourceheight & (1 << ii) )
            q = icons.height[ii];
         else 
            q = xlatpict(xlatpictgraytable,icons.height[ii]);
   
         putimage(wepx + 210 + ii * 35, y1 + starty + 300,q);
      } 
   
      for (ii = 7; ii >= 0; ii--) { 
         if (aktvehicle->weapons->weapon[markweap].targ & (1 << ii) )
            q = icons.height[ii];
         else 
            q = xlatpict(xlatpictgraytable,icons.height[ii]);
   
         putimage(wepx + 210 + ii * 35, y1 + starty + 325, q);
      } 
   
   
      paintsurface2 ( graphx1, graphy1, graphx2, graphy2 );
      rectangle ( graphx1, graphy1, graphx2, graphy2 , lightblue);
   
      if (aktvehicle->weapons->weapon[markweap].maxstrength > 0)
         if ( aktvehicle->weapons->weapon[markweap].getScalarWeaponType() >= 0 ) {
   
          /*  line(x1 + 300,y1 + starty + 130,x1 + 300,y1 + starty + 260,black);
            putspriteimage(x1 + 293,y1 + starty + 130,icons.weapinfo.pfeil1); 
            line(x1 + 297,y1 + starty + 140,x1 + 303,y1 + starty + 140,black); 
            line(x1 + 297,y1 + starty + 260,x1 + 303,y1 + starty + 260,black); 
   
            line(x1 + 310,y1 + starty + 260,x1 + 520,y1 + starty + 260,black); 
            putrotspriteimage90(x1 + 516,y1 + starty + 253,icons.weapinfo.pfeil1,0); 
   
            line(x1 + 325,y1 + starty + 257,x1 + 325,y1 + starty + 263,black); 
            line(x1 + 510,y1 + starty + 257,x1 + 510,y1 + starty + 263,black); */
   
            activefontsettings.font = schriften.smallsystem;
            activefontsettings.background = dblue;
            activefontsettings.length = 20;
            activefontsettings.justify = lefttext;
   
            strrd8u(aktvehicle->weapons->weapon[markweap].mindistance, strng);
            showtext2( strng, graphx1, graphy2 + 5);
   
            activefontsettings.justify = righttext;
            strrd8d(aktvehicle->weapons->weapon[markweap].maxdistance, strng);
            showtext2( strng, graphx2 - 20, graphy2 + 5);
            
            activefontsettings.justify = centertext; 
            activefontsettings.length = gettextwdth("distance",NULL);
   
            showtext2("distance",graphx1 + (graphx2 - graphx1  - activefontsettings.length ) / 2, graphy2 + 5);
   
   /*         activefontsettings.direction = 90;
   
            showtext2("strength", x1 + 285,y1 + starty + 200);
            activefontsettings.direction = 0; 
            activefontsettings.justify = righttext; 
            itoa(aktvehicle->weapons->weapon[weapnum].maxstrength, strng, 10 );
            showtext2(strng, x1 + 292,y1 + starty + 137);
            showtext2("0", x1 + 292,y1 + starty + 257); */
                     
            int dx = graphx2 - graphx1;
            int dy = graphy2 - graphy1;
   
            for (ii = 0; ii <= 255; ii++) {
                xa = graphx1 + ii * dx / 255;
                ya = graphy2 - dy *
                      weapdist->getweapstrength(&aktvehicle->weapons->weapon[markweap], 0, -1, -1, ii )
                      / 256;
                putpixel(xa,ya,14); 
             
            } 
         } 
   
      activefontsettings.font = schriften.smallarial;
   
      /*
   
      activefontsettings.color = black; 
      activefontsettings.background = backgrnd2; 
      activefontsettings.justify = lefttext; 
      activefontsettings.length = 170;
   
      if (aktvehicle->height >= chtieffliegend) 
         suffix = "air - ";
      else 
         suffix = "ground - ";
   
      if ((aktvehicle->weapons->weapon[markweap].typ & (cwgroundmissileb | cwairmissileb)) == 0)
         suffix = "";
      */
   
      activefontsettings.color = black; 
      activefontsettings.length = 25;
      activefontsettings.justify = lefttext;
   
      if ( aktvehicle->weapons->weapon[markweap].shootable() ) {
         showtext2("yes", wepx + 210, wepy + 160);
      } else { 
         showtext2("no", wepx + 210, wepy + 160);
      } 
   
   
      if ( aktvehicle->weapons->weapon[markweap].canRefuel() ) { 
         showtext2("yes",wepx + 210, wepy + 180);
      } 
      else 
         showtext2("no",wepx + 210, wepy + 180);
   
   
   /*
      activefontsettings.length = 170;
      strcpy ( strng, suffix );
      for (k = 0; k <= 15; k++) 
         if (j & (1 << k) ) 
            if ( k < cwaffentypennum) {
               strcat( strng, cwaffentypen[k] );
               showtext2( strng, x1 + 50,y1 + starty + 40);
            };                             
   
      itoa(aktvehicle->weapons->weapon[markweap].maxstrength , strng, 10 );
      showtext2(strng ,x1 + 50,y1 + starty + 90);
   
      itoa(aktvehicle->weapons->weapon[markweap].count , strng, 10 );
      showtext2(strng ,x1 + 50,y1 + starty + 145);
   
   
      bar(x1 + 330,y1 + starty + 40,x1 + 550,y1 + starty + 70,dblue);
      k = 0; 
      for (i = 0; i <= 7; i++) 
         if (aktvehicle->weapons->weapon[markweap].sourceheight & (1 << i)) { 
            putimage(x1 + 330 + k * 35,y1 + starty + 40,icons.height[i]); 
            k++; 
         } 
   
      bar(x1 + 330,y1 + starty + 95,x1 + 550,y1 + starty + 125,dblue);   
      k = 0; 
      for (i = 0; i <= 7; i++) 
         if (aktvehicle->weapons->weapon[markweap].targ & (1 << i) ) { 
            putimage(x1 + 330 + k * 35,y1 + starty + 95,icons.height[i]); 
            k++; 
         } 
      */
   
   
   /*
            activefontsettings.length =  30;
   
            itoa ( aktvehicle->weapons->weapon[ii].maxstrength, strng , 10);
            showtext2(strng, wepx + 195,wepy + ii * 20);
   
            strrd8 ( aktvehicle->weapons->weapon[ii].mindistance, strng );
            showtext2(strng, wepx + 230, wepy + ii * 20);
   
            strrd8 ( aktvehicle->weapons->weapon[ii].maxdistance, strng );
            showtext2(strng, wepx + 265, wepy + ii * 20);
   
            itoa ( aktvehicle->weapons->weapon[ii].count, strng ,10);
            showtext2(strng, wepx + 300,wepy + ii * 20);
   */
   
   } else {
      paintsurface2 ( graphx1, graphy1, graphx2, graphy2 );
      rectangle ( graphx1, graphy1, graphx2, graphy2 , lightblue);
   }
}


void         tvehicleinfo::showinfotext( void )
{
  collategraphicoperations cgo ( x1, y1, x1 + xsize, y1 + ysize );;

   char *b;
   if (aktvehicle->infotext)
      b = aktvehicle->infotext;
   else
      b = "No information available ";
   
   if (category < 21)
      paintsurface ( xsize - 44, starty + 55, xsize - 20, ysize - 65 );
   setparams ( x1 + 20, y1 + starty + 55, x1 + xsize - 45, y1 + ysize - 65, b, black, backgrnd2);
   tvt_dispactive = 0;
   displaytext(  );
   textsizeycomplete = tvt_yp;
   tvt_dispactive = 1;


   tvt_starty = 0; 

   if (category < 21  &&   textsizeycomplete >= textsizey) 
      showbutton ( 8 );
                                                                                          

   if (category == 21  &&   textsizeycomplete < textsizey) {
      hidebutton ( 8 );
      paintsurface(xsize - 35, starty + 55 , xsize - 20,ysize - 65);
   }

   if (textsizeycomplete >= textsizey)
      category = 21;
   else
      category = 20;

   displaytext(  );

}

void         tvehicleinfo::showclasses( void )
{
  collategraphicoperations cgo ( x1, y1, x1 + xsize, y1 + ysize );;

  int i, j;

   if ( aktvehicle->classnum) {
      if (category == 10) 
         paintsurface ( 40, starty + 60, 170, starty + 80);
      else
         if (category < 9)
            paintsurface( 20, starty + 55, xsize - 20, ysize - 65 );
      activefontsettings.length = 70;
      activefontsettings.justify = righttext;
      showtext2 ( "weapon:", x1 + 170 , y1 + starty + 60 );
      activefontsettings.length = 20;
      for (j = 0 ; j < aktvehicle->weapons->count  ; j++ ) 
          showtext2 (  letter[ j ], x1 + 250 + j * 30, y1 + starty + 60 );

        activefontsettings.length = 40;
      showtext2 ( "armor", x1 + 250 + j * 30, y1 + starty + 60 );
      doubleline ( x1 + 25, y1 + starty + 80, x1 + xsize - 25, y1 + starty + 80 );

      for (i = 0; i<aktvehicle->classnum ;i++ ) {
         activefontsettings.background = backgrnd2;
         activefontsettings.color = black;
         activefontsettings.length = 200;
         activefontsettings.justify = lefttext;
         showtext2 ( aktvehicle->classnames[i], x1 + 40, y1 + starty + 85 + i * 25 );
         activefontsettings.justify = righttext;
         activefontsettings.length = 20;
         for (j = 0 ; j < aktvehicle->weapons->count  ; j++ )
            if ( aktvehicle->weapons->weapon[j].getScalarWeaponType() >= 0 )
               showtext2 ( strrr ( aktvehicle->weapons->weapon[j].maxstrength * aktvehicle->classbound[i].weapstrength[ aktvehicle->weapons->weapon[j].getScalarWeaponType() ] / 1024 ), x1 + 250 + j * 30, y1 + starty + 85 + i * 25 );

        activefontsettings.length = 40;
         showtext2 ( strrr ( aktvehicle->armor * aktvehicle->classbound[i].armor / 1024 ), x1 + 250 + j * 30, y1 + starty + 85 + i * 25 );
      } /* endfor */
      paintsurface ( 25, starty + 85 + i*25, xsize - 25, ysize - 65 );
      paintsurface ( 291 + j * 30, starty + 60, xsize - 25, starty + 85 + i*25 );

      category = 9;
   } else 
     if (category != 10) {
        paintsurface( 20, starty + 55, xsize - 20, ysize - 65);
        activefontsettings.length = 0;
        npush ( activefontsettings.background );
        activefontsettings.background = 255;
        showtext2 ( "unit has no classes", x1 + 40, y1 + starty + 60 );
        npop ( activefontsettings.background );
        category = 10;
     }
   activefontsettings.justify = lefttext;
}                    



void         tvehicleinfo::buttonpressed( char id )
{ 
   tdialogbox::buttonpressed ( id );
   if (id == 1) 
      action = 20; 

   if (( id == 2 ) || ( id == 3)) {
      pvehicletype type;
      if (id == 2) { 
         do {
            if ( i > 0 )
               i--;
            else
               i = vehicletypenum - 1;
            type = getvehicletype_forpos ( i );
         } while ( !type || !isUnitNotFiltered ( type->id ) ); /* enddo */
         markweap = 0; 
      } 
             
      if (id == 3) { 
         do {
            if ( i < vehicletypenum - 1 )
               i++;
            else
               i = 0;
            type = getvehicletype_forpos ( i );
         } while ( !type || !isUnitNotFiltered ( type->id ) ); /* enddo */

         markweap = 0;
      }                     
      checknextunit(); 
   } 
   if (id == 5 && category != 1) {
      if (category == 21)
         hidebutton ( 8 );
      category = 1;
      markweap = 0;
      showweapons();
   }
   if (id == 4 && category ) {
      if (category == 21)
         hidebutton ( 8 );
      category = 0;
      showgeneralinfos();
   }
   if (id == 6 && (category < 3 || category >= 20)) {
      if (category == 21)
         hidebutton ( 8 );
      category = 3;
      showclasses ();
   }

   if (id == 7 && category < 20 ) {
      showinfotext ();
   }

   if (id == 8  && category >= 20)
      displaytext(  );

} 


void         tvehicleinfo::checknextunit(void)
{ 
   if (i != j) { 
      mousevisible(false); 
      aktvehicle = getvehicletype_forpos ( i ); 
      zeigevehicle();
      if ( category == 0 ) {
         showgeneralinfovariables();
      } else 
         if ( category == 1) {
            markweap = 0;
            paintmarkweap();
         } else
           if (category < 20)
              showclasses ();
           else 
              showinfotext ();


      mousevisible(true); 
      j = i; 
   } 
} 




void         tvehicleinfo::run(void)
{ 
   tweaponinfo  weapinf; 
   byte         k; 
   byte         drk; 

   i = 0; 
   if ( !aktvehicle ) {
      pvehicle eht = getactfield()->vehicle;
      if ( !fieldvisiblenow(getactfield()) )
         eht = NULL; 
      if ( eht ) { 
         while (eht->typ != getvehicletype_forpos ( i )) 
            i++;
      } 
   } else {
      while ( aktvehicle != getvehicletype_forpos ( i )) 
        i++;
   }

   pvehicletype type = getvehicletype_forpos ( i );
   while ( !type || !isUnitNotFiltered ( type->id ) ) {
      if ( i < vehicletypenum - 1 )
         i++;
      else {
         displaymessage ( "no vehicle type to display; check unitset filters ", 1);
         return;
      }
      type = getvehicletype_forpos ( i );
   } ;


   j = i;
   drk = 0; 

   aktvehicle = getvehicletype_forpos ( i ); 
   zeigevehicle();
   showgeneralinfos();

   mousevisible(true); 
   do { 

      tdialogbox::run();

      if (category == 1) {
         if (mouseparams.taste == 1) 
            if ((mouseparams.x > wepx + 10) && (mouseparams.x < wepx + 340))
               if (aktvehicle->weapons->count > 0) 
                  for (k = 0; k < aktvehicle->weapons->count ; k++) 
                     if ((mouseparams.y > wepy + k * 20 - 3) && (mouseparams.y < wepy + 17 + k * 20)) {
                        if (k != markweap ) { 
                           markweap = k ; 
                           mousevisible(false); 
                           paintmarkweap(); 
                           mousevisible(true); 
                        } 
                     } 


         if (taste == ct_up) 
            if (aktvehicle->weapons->count > 0) { 
               markweap--;
               if (markweap < 0)
                  markweap = aktvehicle->weapons->count-1;
               paintmarkweap(); 
            } 
         if (taste == ct_down) 
            if (aktvehicle->weapons->count ) { 
               markweap++;
               if (markweap >= aktvehicle->weapons->count)
                  markweap = 0;
               paintmarkweap(); 
            } 
      }
      if ( category == 4 )
         checkscrolling ( );


   }  while ( (taste != ct_esc) && (action < 20));
} 

void         vehicle_information( pvehicletype type )
{ 
  tvehicleinfo eif; 
   eif.init( type ); 
   eif.run(); 
   eif.done(); 
} 




  struct tmessagestrings { 
                       word         number; 
                       struct { 
                                      word      id;
                                      char*     txt;
                               } data[100];
                    }; 




tmessagestrings messagestrings; 





   #define klickconst 100  
   #define delayconst 10  






#include <malloc.h>

void   loadsinglemessagefile ( char* name )
{
   tnfilestream stream ( name, 1 );

   char* s1;
   char* s2;
   stream.readpnchar ( &s1 );
   stream.readpnchar ( &s2 );

   while ( s1 && s2 ) {
     int w = atoi ( s1 );

      char* t = strdup ( s2 );

      messagestrings.data[ messagestrings.number ].id  = w;
      messagestrings.data[ messagestrings.number ].txt = t;
      messagestrings.number++;

      delete[] s1;
      delete[] s2;
      stream.readpnchar ( &s1 );
      stream.readpnchar ( &s2 );
   } 
   if ( s1 )
      delete[] s1;
   if ( s2 )
      delete[] s2;
}


void         loadmessages(void)
{ 

   messagestrings.number = 0; 
 
   {
      tfindfile ff ( "message?.txt" );
      char* filename = ff.getnextname();
   
      while( filename ) {
         loadsinglemessagefile ( filename );
         filename = ff.getnextname();
      } 
   }

} 


char*        getmessage(word         id)
{ 
  word         w; 

   if ( messagestrings.number == 0 ) 
      return NULL;
   w = 0;
   while ((messagestrings.number > w) && (messagestrings.data[w].id != id)) 
      w++;
   if (messagestrings.data[w].id == id) 
      return messagestrings.data[w].txt; 
   return NULL;
} 



int          dispmessage2(int          id,
                          char *       st)
{ 
   char*        sp;
   char         *s1, *s2;

   sp = getmessage(id); 
   s1 = sp;
   if (sp != NULL) { 
      char s[200];
      s2 = s;
      while (*s1 !=0) {
        if ( *s1 != 35 /* # */ ) {
           *s2 = *s1;
           s2++;
           s1++;
        } else {
           if (st != NULL) {
              while (*st != 0) {
                 *s2 = *st;
                 s2++;
                 st++;
              } /* endwhile */
           } /* endif */
        } /* endif */
      } /* endwhile */
      *s2 = 0;

      return displaymessage2(s); 
   } else
     return -1;
    
} 


  /* type
  tmemoryinfo = object(tdialogbox)
                   procedure init;
                   procedure run;
                end;

procedure tmemoryinfoinit;
var
  l:int;
begin
  x1:=80;
  y1:=50;
  xsize:=640-2*x1;
  ysize:=480-2*y1;
  buildgraphics;
  activefontsettings.font:=schriften.smallarial;
  activefontsettings.color:=lightblue;
  activefontsettings.background:=255;
  activefontsettings.justify:=lefttext;
  showtext2('vehicletypeen : '+strr(sizeof(tvehicletype)*vehicletypeenanz+
end;  */ 




class  tchoice_dlg : public tdialogbox {
               public:
                  byte      result;
                  void      init( char* a, char* b, char* c );
                  virtual void buttonpressed( byte id );
                  virtual void run( void );
                };


void         tchoice_dlg::init( char* a, char* b, char* c )
{
  tdialogbox::init();
  windowstyle |= dlg_notitle;
  int wdth = gettextwdth ( a, schriften.arial8 ) ;
  if ( wdth > 280 )
     xsize = wdth + 20;
  else
     xsize = 300; 

  ysize = 100; 
  addbutton(b,10,60,xsize/2-5,90,0,1,1,true); 
  addbutton(c,xsize/2+5,60,xsize-10,90,0,1,2,true);

  x1 = (640 - xsize) / 2;
  y1 = (480 - ysize) / 2;

  buildgraphics();
  activefontsettings.font = schriften.arial8; 
  activefontsettings.justify = centertext; 
  activefontsettings.length = xsize - 20;
  activefontsettings.background = 255; 
  activefontsettings.color = actmap->actplayer * 8 + 20; 
  mousevisible ( false );
  showtext2(a,x1 + 10,y1 + 15);
  mousevisible ( true );
  result = 0; 
} 


void         tchoice_dlg::buttonpressed( byte id )
{ 
  if (id == 1) result = 1; 
  if (id == 2) result = 2; 
} 


void         tchoice_dlg::run(void)
{ 
  while ( mouseparams.taste )
     releasetimeslice();
  mousevisible(true); 
  do { 
    tdialogbox::run();
  }  while (result  == 0);
} 


byte         choice_dlg(char *       title,
                        char *       s1,
                        char *       s2, 
                        ... )
{ 

   va_list paramlist;
   va_start ( paramlist, s2 );

   char tempbuf[1000];

   int lng = vsprintf( tempbuf, title, paramlist );
   if ( lng >= 1000 )
      displaymessage ( "dialog.cpp / choice_dlg:   string to long !\nPlease report this error",1 );

  tchoice_dlg  c; 
  byte a;

  c.init(tempbuf,s1,s2); 
  c.run(); 
  a = c.result;
  c.done(); 
  return a;
} 



/*********************************************************************************************/
/* unpack_date :  entpackt das datum (von zb dateien)                                        */
/*********************************************************************************************/
void        unpack_date (unsigned short packed, int &day, int &month, int &year)
{
   year = packed >> 9;
   month = (packed - year*512) >> 5;
   day = packed - year*512 - month*32;
   year +=1980;
}




/*********************************************************************************************/
/* unpack_time :  entpackt die zeit (von zb dateien)                                         */
/*********************************************************************************************/
void        unpack_time (unsigned short packed, int &sec, int &min, int &hour)
{
   hour = packed >> 11;
   min = (packed - hour*2048) >> 5;
   sec = packed - hour*2048 - min*32;       // sekunden sind noch nicht getestet !!!
}





#define shownfilenumber 15
#define entervalue ct_space  
#define pfiledata filedata* 

const int fileswithdescrptionnum = 1;
const char* fileswithdescrption[fileswithdescrptionnum] =  {"bla.bla"}; // { mapextension, savegameextension, tournamentextension };

 struct tfiledata {
                char*        name;
                char*        description;
                char*        sdate;
                int          time;

             } ;



class   tfileselectsvga : public tdialogbox {
                   public:
                        tfiledata       *files;
                        
                        char         ausgabeaborted;
                        char            searchstring[20];
                        int             searchsize;
                        int             numberoffiles;
                        char            mousebuttonreleased;
                        int             lastscrollbarposition;

                        int             actdispc[shownfilenumber];
                        int             actdispn[shownfilenumber];
                                                 
                        char            abrt;
                        int             actshownfilenum;
                        int             firstshownfile;
                        int             markedfile;

                        char            wildcard[100];
                        char*           descrip;
                        char            swtch;           /* 1: load        2: save */
                        char*           result;
                        char         sort_name;
                        char         sort_time;

                        void            init( char sw );
                        void            setdata( const char* _wildcard,   char* b, char* description );   // result
                        
                        void            readdirectory ( void );
                        void            fileausgabe( char force , int dispscrollbar);
                        void            sortentries ( void );
                        virtual void    run ( void );
                        
                        char         speedsearch( char input );
                        void            checkfsf( char lock);   
                        void            displayspeedsearch ( void );

                        virtual void    buttonpressed( char id);
                };


void         tfileselectsvga::init( char sw  )
{ 

   tdialogbox::init();
   swtch = sw; 
   if (sw == 1)
      title = "load";
   else
      title = "save";
   x1 = 5;
   xsize = 630;
   y1 = 30; 
   ysize = 420;
   sort_name = false; 
   sort_time = true; 
   searchsize = 0;
   lastscrollbarposition = 0;

   windowstyle &= ~dlg_in3d;
   if (swtch == 1) 
      addbutton("load",10,ysize - 45,110,ysize - 10,0,1,1,true);
   else 
      addbutton("save",10,ysize - 45,110,ysize - 10,0,1,1,true);

   addbutton("cancel",120,ysize - 45,210,ysize - 10,0,1,2,true);
  // addkey ( 2, cto_esc );

   addbutton("sort by name",430,ysize - 45, xsize - 10, ysize - 30,3,1,3,true);
   addeingabe(3, &sort_name, black, dblue);

   addbutton("sort by time",430,ysize - 25, xsize - 10, ysize - 10,3,1,4,true);
   addeingabe(4, &sort_time, black, dblue);
                                                                                                
   actshownfilenum = shownfilenumber;
   addscrollbar(xsize - 30,starty + 10,xsize - 10,ysize - 60, &numberoffiles, actshownfilenum, &firstshownfile, 5, 0);
   hidebutton ( 5 );

   buildgraphics();

   rahmen(true,x1 + 10,y1 + starty + 10, x1 + xsize - 35,y1 + ysize - 57);   /* fileausgabe */

   if (sw == 1)
      rahmen3("SpeedSearch:", x1 + 220, y1 + ysize - 40, x1 + 420, y1 + ysize - 10 , 1);
   else
      rahmen3("new filename:", x1 + 220, y1 + ysize - 40, x1 + 420, y1 + ysize - 10 , 1);

   memset ( actdispc, -1, sizeof ( actdispc ));
   memset ( actdispn, -1, sizeof ( actdispn ));
} 




void         tfileselectsvga::buttonpressed(byte         id)
{ 
   tdialogbox::buttonpressed(id);
   switch (id) {
      
      case 1:   abrt = 2; 
      break; 
      
      case 2:   abrt = 1; 
      break; 
      
      case 3:   { 
            collategraphicoperations cgo ( x1, y1, x1 + xsize, y1 + ysize );;
            sort_time = (char) !sort_name;
            enablebutton(4); 
            sortentries(); 
            fileausgabe(false,1);
         } 
      break; 
      
      case 4:   { 
            collategraphicoperations cgo ( x1, y1, x1 + xsize, y1 + ysize );;
            sort_name = (char) !sort_time;
            enablebutton(3); 
            sortentries(); 
            fileausgabe(false,1);
         } 
      break; 
      
      case 5:   { 
            checkfsf(0); 
            fileausgabe(false,0);
         } 
   break; 
   } 
} 





void         tfileselectsvga::readdirectory(void)
{ 
   numberoffiles = 0; 

  char fls[200];
//  strcpy ( fls, gamepath );
//  strcat ( fls, wildcard );
  strcpy ( fls, wildcard );


  {
     tfindfile ff ( fls );

     char* filename = ff.getnextname();
     while( filename ) {
        numberoffiles++;
        filename = ff.getnextname();
      }

   } 

   files = new tfiledata [ numberoffiles ];
   memset ( files, 0, numberoffiles *  sizeof ( tfiledata ));

   numberoffiles = 0; 

   int searchfordescription = 0;
   for (int i = 0; i < fileswithdescrptionnum; i++ ) 
      if ( strcmpi ( &wildcard[ strlen( wildcard ) - strlen (fileswithdescrption[i]) ] , fileswithdescrption[i] ) == 0 )
         searchfordescription++;

   char* description = NULL;


   tfindfile ff ( fls );
   char* filename = ff.getnextname();

   while( filename ) {
      if ( searchfordescription && !keypress()) {
   
            try {
               char tmp[200];
               if ( strchr ( wildcard, '\\' )) {
                  char* a = wildcard;
                  char* b = wildcard;
                  while ( *a ) {
                     if ( *a == '\\' )
                        b = a;
                     a++;
                  }
                  
                  int ps = 0;
                  a = wildcard;
                  while ( a <= b ) {
                     tmp[ps++] = *a;
                     a++;
                  }

                  tmp[ps] = 0;

                  strcat ( tmp, filename );

               } else
                  strcpy ( tmp, filename );

               tnfilestream stream ( tmp, 1 );
               stream.readpchar ( &description, 200 );
               word w;
               stream.readdata2 ( w );
               if ( w != fileterminator )
                  throw tinvalidversion ( filename, fileterminator, w );
   
               files[numberoffiles].name = strdup ( filename );
               files[numberoffiles].description = NULL;
               if ( description )
                  if ( description[0] )
                      files[numberoffiles].description = strdup ( description );
   
               time_t tdate = stream.get_time();
               if ( tdate != -1 ) {
                  files[numberoffiles].time = tdate;
                  files[numberoffiles].sdate = strdup ( ctime ( &tdate ) );
               }
   
   
               numberoffiles++;
   
            } /* endtry */
   
            catch ( tfileerror ) {
            } /* endcatch */
            
            if ( description ) {
               delete[]  description ;
               description = NULL;
            }
   
          
      } else {
          files[numberoffiles].name = strdup ( filename );
          files[numberoffiles].description = NULL;
                 
          time_t tdate = get_filetime( filename );

          if ( tdate != -1 )
             files[numberoffiles].sdate = strdup ( ctime ( &tdate ) );
          else
             files[numberoffiles].sdate = NULL;

          files[numberoffiles].time = tdate;

          numberoffiles++;
      }


      filename = ff.getnextname();
   } 
   firstshownfile = 0;

   if (numberoffiles > shownfilenumber) 
      showbutton(5); 
} 




void         tfileselectsvga::fileausgabe(char     force , int dispscrollbar)
{ 

   if (numberoffiles == 0)
      return;

   collategraphicoperations cgo ( x1, y1, x1 + xsize, y1 + ysize );;

   mousevisible(false);

   ausgabeaborted = true; 
   if (dispscrollbar)
      if (numberoffiles > actshownfilenum) 
         if ( firstshownfile != lastscrollbarposition) {
            showbutton ( 5 );
            lastscrollbarposition = firstshownfile;
         }
   activefontsettings.font = schriften.smallarial;
   activefontsettings.background = dblue; 
   
   int lastshownfile = firstshownfile + actshownfilenum;
   int emptylines;
   if (lastshownfile >= numberoffiles) {
      emptylines = lastshownfile - numberoffiles+1;
      lastshownfile = numberoffiles - 1;
   } else
      emptylines = 0;

   int jj = 0;
   for ( int ii = firstshownfile; ii < firstshownfile + actshownfilenum; ii++, jj++) {
      if (keypress() && (!force)) 
         return;
      if ( ii < numberoffiles ) {

         if (ii == markedfile)
            activefontsettings.color = lightred; 
         else 
            activefontsettings.color = black; 

         if ( ii != actdispn[jj]    ||    activefontsettings.color != actdispc[jj] ) {
            actdispn[jj] = ii;
            actdispc[jj] = activefontsettings.color;

            activefontsettings.length = 120;
            showtext2( files[ii].name,x1 + 15,y1 + starty + jj * 20 + 20);

            activefontsettings.length = 200;
            activefontsettings.justify = righttext;
            if ( files[ii].sdate ) 
              showtext2( files[ii].sdate, x1 + 145,y1 + starty + jj * 20 + 20);
            else 
              bar ( x1 + 145,y1 + starty + jj * 20 + 20, x1 + 145 + activefontsettings.length, y1 + starty + jj * 20 + 20 + activefontsettings.font->height, activefontsettings.background );

            activefontsettings.justify = lefttext;
            activefontsettings.length = 230;
            if ( files[ii].description ) {
               showtext2( files[ii].description, x1 + 360,y1 + starty + jj * 20 + 20);
            } else
               bar ( x1 + 360,y1 + starty + jj * 20 + 20, x1 + 360 + activefontsettings.length, y1 + starty + jj * 20 + 20 + activefontsettings.font->height, activefontsettings.background );
         } 
      } 
      else { 
         bar(x1 + 15,          y1 + starty + (jj+1) * 20 ,
             x1 + xsize - 50,  y1 + starty + (jj+2) * 20, dblue);
         actdispn[jj] = -1;
         actdispc[jj] = -1;
      } 
   } 


   ausgabeaborted = false; 
   mousevisible(true); 
} 




void         tfileselectsvga::sortentries(void)
{ 
    int      ii;

   if ( numberoffiles == 0)
      return;

   memset ( actdispc, -1, sizeof ( actdispc ));
   memset ( actdispn, -1, sizeof ( actdispn ));

   for (ii=0; ii < numberoffiles-1; ) {
      if (((sort_name == true ) && ( strcmp( files[ii].name , files[ii+1].name ) > 0)) ||
          ((sort_name == false) && ( files[ii].time < files[ii+1].time))) {
             tfiledata temp = files[ii];
             files[ii] = files[ii+1];
             files[ii+1] = temp;

             if (ii == markedfile)
                markedfile++;
             else
               if (ii+1 == markedfile)
                  markedfile--;

             if (ii > 0)
                ii--;
      } 
      else 
         ii++;
   } 

} 



char      tfileselectsvga::speedsearch(char         input)
{ 
    char      sps; 


   searchstring[searchsize] = toupper( input );
   searchsize++;
   searchstring[searchsize] = 0;

   sps = false; 
   int ii = markedfile;
   if ( ii >= 0    &&   strnicmp ( searchstring, files[ii].name, searchsize ) == 0) {
      sps = true; 
   } 
   else { 
      ii = 0;
      while (ii < numberoffiles  &&  !sps ) {
         if (strnicmp ( searchstring, files[ii].name, searchsize ) == 0) {
            sps = true; 
            markedfile = ii;
         } 
         ii++;
      } 
   } 

   if (!sps   &&   swtch == 1) {
     searchsize--;
     searchstring[searchsize] = 0;
   }


   displayspeedsearch(); 

   return sps;
} 


void         tfileselectsvga::displayspeedsearch(void)
{ 
  collategraphicoperations cgo ( x1 + 225, y1 + ysize - 30, x1 + 225 + 190, y1 + ysize );

   mousevisible(false);
   npush ( activefontsettings );
   activefontsettings.length = 190;
   showtext2(searchstring,x1 + 225, y1 + ysize - 30);
   npop ( activefontsettings );
   mousevisible(true); 
} 



void         tfileselectsvga::checkfsf( char lock )   
{ 
   if ( numberoffiles > actshownfilenum) {
      if ( firstshownfile + actshownfilenum > numberoffiles)
         firstshownfile = numberoffiles - actshownfilenum;
   } else
      firstshownfile = 0;

   if (markedfile >= numberoffiles)
      markedfile = numberoffiles - 1;

   if (lock) {
      if ( firstshownfile + actshownfilenum <= markedfile )
         firstshownfile = markedfile - actshownfilenum+1;
      if (markedfile < firstshownfile)
         firstshownfile = markedfile;
   }
}



void         tfileselectsvga::run(void)
{ 
   #ifdef _DOS_
    #ifdef NEWKEYB
     closekeyb();
    #endif
   #endif

   activefontsettings.background = dblue; 
   searchstring[0] = 0;

   readdirectory(); 
   sortentries(); 

   firstshownfile = 0;
   markedfile = -1;

   activefontsettings.justify = lefttext;

   mousebuttonreleased = 0; 
   abrt = 0; 

   if (numberoffiles == 0) {
      activefontsettings.font = schriften.smallarial;
      activefontsettings.justify = centertext;
      activefontsettings.length = xsize - 60;
      showtext2( " no files found", x1 + 30, 150 );
      activefontsettings.justify = lefttext;
      if (swtch == 1) {
         disablebutton ( 1 );
         addkey ( 2, ct_enter );
         mousevisible ( true );
         do {
            tdialogbox::run ();
         } while ( abrt == 0 ); /* enddo */
         mousevisible ( false );
         abrt = 1; 
      } 
   } 

   if (abrt == 0) { 

      fileausgabe( true ,1 );
      mousevisible( true );
      do { 
         tdialogbox::run();

         if ( numberoffiles ) {
            switch ( taste ) {
            case cto_down:  markedfile++;
                            checkfsf( 1 );
                            fileausgabe(false,1);
               break;

            case cto_up:    if ( markedfile > 0) {
                               markedfile--;
                               checkfsf( 1 );
                               fileausgabe(false,1);
                            }
               break;

            case cto_pdown: markedfile+=actshownfilenum - 1;
                            checkfsf( 1 );
                            fileausgabe(false,1); 
               break;

            case cto_pup:    if ( markedfile > actshownfilenum - 1)
                               markedfile-= actshownfilenum - 1;
                            else
                               markedfile = 0;
                            checkfsf( 1 );
                            fileausgabe(false,1);
               break;

            case cto_pos1:  markedfile = 0;
                            checkfsf( 1 );
                            fileausgabe(false,1);
               break;
            case cto_ende:  markedfile = numberoffiles - 1;
                            checkfsf( 1 );
                            fileausgabe(false,1);
               break;

            } /* endswitch */

            if (ausgabeaborted) 
               fileausgabe( false ,1);
         }

         if (prntkey == cto_enter) {
            if ( (swtch == 1 && ( markedfile >= 0) )   ||   ( swtch == 0) && searchstring[0]  )
               abrt = 2;
            else
              if  ( ( swtch == 0) && (markedfile >= 0))
                 abrt = 3;
         }

         if ((prntkey >= ' ') && (prntkey < 256)) {   /* spedsearc */ 
            if (swtch == 1) {
               if (speedsearch(prntkey)) { 
                  checkfsf( 1 );
                  fileausgabe(false,1); 
               } 
            } 
            else { 
               if ( searchsize < maxfilenamelength ) {
                  searchstring[searchsize] = prntkey;
                  searchsize++;
                  searchstring[searchsize] = 0;
                  displayspeedsearch(); 
               }
            } 
         } 
         if ((prntkey == cto_bspace) && (searchstring[0] != 0)) {   /* l”schen des letzten zeichens von searchstring */
            searchsize--;
            searchstring[searchsize] = 0;
            displayspeedsearch(); 
         } 
         if (prntkey == cto_esc ) {   /*  abbrechen / searchstring zurcksetze  */
            if ((swtch == 1 ) || (searchstring[0] == 0))
               abrt = 1; 
            else
               if ( searchstring[0] ) {
                  searchstring[0] = 0;
                  searchsize = 0;
               }

         } 
         if (ausgabeaborted) 
            fileausgabe(false,1);

         if ((mouseparams.x >= x1 + 15) && (mouseparams.x <= x1 + xsize - 50) && 
             (mouseparams.y >= y1 + starty + 20) && (mouseparams.y <= y1 + starty + (shownfilenumber + 1) * 20)) {

            int i = firstshownfile + (mouseparams.y - (y1 + starty + 20)) / 20;
            if (mouseparams.taste == 1   && ( mousebuttonreleased == 0 || i != markedfile)) {
               if ( i < numberoffiles){
                  markedfile = i;
                  fileausgabe(false,1);
                  mousebuttonreleased = 1;
               }
            } else
               if ( i == markedfile   &&  mousebuttonreleased == 1   &&   mouseparams.taste == 0)
                  mousebuttonreleased = 2;
               else
                 if ( i == markedfile   &&  mousebuttonreleased == 2   &&   mouseparams.taste & 1 )
                    mousebuttonreleased = 3;
         }
         if (mousebuttonreleased == 3) {
            abrt = 3;
            while (mouseparams.taste & 1)
              releasetimeslice();
         }
      }  while ( abrt == 0);
      mousevisible(false); 
      if (abrt > 1) { 
         if ( ( markedfile >= 0) || searchstring[0] ) {
            if ( (swtch == 1)  ||   (abrt == 3)   ||  (!searchstring[0]) ) {  // load
               strcpy ( result , files[markedfile].name );
               if ( descrip ) 
                  if ( files[markedfile].description )
                     strcpy ( descrip, files[markedfile].description );
                  else
                     descrip[0] = 0;
            } else {
               char* aa = searchstring;
               while (*aa != 0  && *aa != '.') 
                  aa++;
               *aa = 0;

               aa = wildcard;
               while ( *aa != 0 && *aa != '*' )
                  aa++;

               if ( *aa ) 
                  aa++;

               strcat( searchstring, aa );
   
               strcpy( result , searchstring );
            }
         } else
             result[0] = 0;
      } else
        result[0] = 0;

      mousevisible(true); 

   } else
     result[0] = 0;

  #ifdef _DOS_
   #ifdef NEWKEYB
   initkeyb();
   #endif
  #endif


   for (int ii=0; ii < numberoffiles ;ii++ ) {
      if ( files[ii].name )
         delete[]  files[ii].name ;
      if ( files[ii].sdate )
         delete[]  files[ii].sdate ;
      if ( files[ii].description )
         delete[]  files[ii].description ;
   } /* endfor */
   delete[] files ;

} 




void         tfileselectsvga::setdata( const char * _wildcard, char *   b, char* description)
{ 
   strcpy ( wildcard , _wildcard );
   result = b;
   descrip = description;
   if ( descrip ) 
      descrip[0] = 0;

} 


void         fileselectsvga( const char*       ext,
                            char*       filename,
                            char        swtch,
                            char *      description )
{                           
   tfileselectsvga tss; 

   tss.init(swtch); 
   tss.setdata( ext, filename, description );
   tss.run();          
   tss.done(); 
} 

void testdisptext ( void )
{
   tnfilestream stream ("tst.txt",1 );
    char *cct;
    stream.readpnchar ( &cct );

    if ( cct ) {
       tviewtext vt;
       vt.setparams ( 10, 10, 630, 470, cct, white, black );
       vt.tvt_dispactive = 1;
       vt.displaytext ();
   
   
       delete[] cct;
    }
}

void tenterfiledescription::init ( char* descrip )
{

   tdialogbox::init();
   xsize = 350;
   ysize = 145;
   x1 = 170;
   y1 = 100;
   title = "enter description";
   description =  new char [ 100 ];

   if ( descrip )
      strcpy ( description, descrip );
   else
      description[0] = 0;

   addbutton( NULL, 15, starty + 10, xsize - 15, starty + 35 , 1, 0, 1, true );
   addeingabe ( 1, (void*) description, 0, 100 );
   addbutton( "~O~K", 15, ysize - 35, xsize - 15, ysize - 10, 0,1,2,true);
   addkey( 2, ct_enter);

   buildgraphics();
   status = 0;
}

void tenterfiledescription::buttonpressed ( char id )
{
   tdialogbox::buttonpressed ( id );
   if (id == 2)
      status = 1;
};


void tenterfiledescription::run ( void )
{
   tdialogbox::run();
   pbutton pb = firstbutton;
   while (pb->id != 1) 
      pb = pb->next;
   execbutton( pb , false );

   /*
   mousevisible(true);
   do {
      tdialogbox::run();
   } while ( status == 0 ); 
   */

}



extern dacpalette256 pal;
#define sqr(a) (a)*(a)

char mix3colors ( int p1, int p2, int p3 )
{
    int diff = 0xFFFFFFF;
    int actdif;
    char pix1;
 
    int r = pal[p1][0] + pal[p2][0] + pal[p3][0]  ;
    int g = pal[p1][1] + pal[p2][1] + pal[p3][1]  ;
    int b = pal[p1][2] + pal[p2][2] + pal[p3][2]  ;

    for (int k=0; k<256 ; k++ ) {
       actdif = sqr( pal[k][0]  * 2 - r ) + sqr( pal[k][1] * 2 -  g ) + sqr( pal[k][2] * 2 - b );
       if (actdif < diff) {
          diff = actdif;
          pix1 = k;
       }
    } 
    return pix1;

}

char mix2colors ( int a, int b )
{
   return (*colormixbuf) [ a ] [ b ];
}

char mix4colors ( int a, int b, int c, int d )
{
   return mix2colors ( mix2colors ( a, b ), mix2colors ( c, d ) );
}



class tshowmap : public tdialogbox, public tbasicshowmap {
          public :
            void init ( void );
            void run ( void );
};

void  tshowmap::init ( void )
{

   tdialogbox::init();
   windowstyle ^= dlg_notitle;
   xsize = 450;
   ysize = 300;
   buffer = NULL;
   buildgraphics();

   tbasicshowmap :: init ( x1+20, y1+30, xsize-2*20, ysize-2*30 );

}

tbasicshowmap :: tbasicshowmap ( void )
{
   buffer = NULL;
   bufsizex = 0;
   bufsizey = 0;
}

#ifdef HEXAGON
#define FREEZOOM
#endif

void tbasicshowmap :: init ( int x1, int y1, int xsize, int ysize )
{
   txsize = xsize;
   tysize = ysize;
   xp1 = x1;
   yp1 = y1;

   lastmapxsize = 0;
   lastmapysize = 0;
   xofs = 0;
   yofs = 0;
   dispxpos = actmap->xpos;
   dispypos = actmap->ypos;
   border = 0;
   lastmapposx = 0;
   lastmapposy = 0;
   lastmaptick = ticker;
   maxmapscrollspeed = 10;

   #ifdef FREEZOOM
   maxzoom = 10;


   int zz1 = txsize / actmap->xsize;
   int zz2 = tysize / (actmap->ysize * fielddisty / fielddistx);
   if ( zz1 > zz2 )
      zoom = zz2;
   else
      zoom = zz1;

   if ( zoom < 2 )   // ##
      zoom = 2;
   if ( zoom > maxzoom )
      zoom = maxzoom;


   #else
   maxzoom = 3;
   #endif


}




int  tbasicshowmap::generatemap ( int autosize )
{
 #ifdef FREEZOOM
                   
   generatemap_var ();

#else

   if (buffer == NULL) {
      if ( autosize ) 
         zoom = 4;
      int size;
      do {
         do {
            if ( autosize ) 
               zoom--;
            switch (zoom) {
            case 0: return 1;
            case 1: mysize = actmap->ysize + 2*border;
                    mxsize = actmap->xsize * 2 + 2*border;
               break;
            case 2: mysize = actmap->ysize * 2 + 1 + 2*border;
                    mxsize = actmap->xsize * 4 + 1 + 2*border;
               break;
            case 3: mysize = actmap->ysize * 3 + 2 + 2*border;
                    mxsize = actmap->xsize * 6 + 2 + 2*border;
               break;
            } 
   
         } while ( (mxsize > txsize - 20) && ( zoom > 1 ) && autosize ); 
   
         size = mxsize * mysize + 4;
   
         buffer = new char [ size + 1000 ] ;
         if (buffer == NULL) {
            displaymessage ( "Not enoigh memory for displaybuffer !", 1 );
            if ( autosize == 0 )
               zoom--;
         }
      } while ( buffer == NULL );

      memset ( buffer, 255, ( size + 1000 ));
    }
    switch (zoom) {
    case 1: generatemap1();
       break;
    case 2: generatemap2();
       break;
    case 3: generatemap3();
       break;
    } 
   interpolatemap ();

#endif
   return 0;
}


void tbasicshowmap::freebuf ( void )
{
   if ( buffer ) {
      delete[]  buffer ;
      buffer= NULL;
   }

}

void tbasicshowmap::interpolatemap ( void )
{
   int col[4];
   int pnum;
   for (int y = 0; y < mysize; y++) 
      for (int x = 0; x < mxsize; x++) {
         if ( buffer[ getbufpos( x, y ) ] == 255 ) {
            pnum = 0;
            if ( y > 0 ) {
               col[ pnum ] = buffer[ getbufpos( x, y-1 ) ];
               if ( col[ pnum ] != 255 ) 
                  pnum++;
             }
            if ( x > 0 ) {
               col[ pnum ] = buffer[ getbufpos( x-1, y ) ];
               if ( col[ pnum ] != 255 ) 
                  pnum++;
             }
            if ( y < ( mysize-1 ) ) {
               col[ pnum ] = buffer[ getbufpos( x, y+1 ) ];
               if ( col[ pnum ] != 255 ) 
                  pnum++;
             }
            if ( x < ( mxsize-1 ) ) {
               col[ pnum ] = buffer[ getbufpos( x+1, y ) ];
               if ( col[ pnum ] != 255 ) 
                  pnum++;
             }
             /*if (pnum == 1)
                buffer[ getbufpos( x, y ) ] = col[0];
             else
                if (pnum == 2) 
                   buffer[ getbufpos( x, y ) ] = mix2colors ( col[0], col[1] );
                else
                  if (pnum == 3) 
                     buffer[ getbufpos( x, y ) ] = mix3colors ( col[0], col[1], col[2] );
                  else*/
                     if (pnum == 4) 
                        buffer[ getbufpos( x, y ) ] = mix4colors ( col[0], col[1], col[2], col[3]  );
                     else
                        buffer[ getbufpos( x, y ) ] = dblue;
         }
      } /* endfor */
}


#define visiblenotcol darkgray
#define buildingcoloroffset 3+16
#define unitcoloroffset     6+16

void tbasicshowmap::generatemap1 ( void )
{
   // mxsize = actmap->xsize * 2;

   word* k = (word*) buffer;
   *k = mxsize - 1;
   k++;
   *k = mysize - 1;

   int l,i,j;
   char c;


   buffer2 = buffer + 4;

   l = 0; 
   for (j = 0; j < actmap->ysize ; j++) {
      for (i = 0; i < actmap->xsize ; i++) { 
         pfield fld1 = getfield ( i, j ) ;
         c = fld1->typ->quickview->dir[fld1->direction].p1;

         int v ;
         if ( godview )
            v = visible_all;
         else
            v = (fld1->visible >> (actmap->playerview*2)) & 3;

         if (j & 1) {
            if ( v == visible_not )
               buffer2[ j * mxsize + 1 + i*2] = visiblenotcol;
            else
               if ( v == visible_ago)
                  buffer2[ j * mxsize + 1 + i*2] = xlattables.a.dark1[ c ];
               else 
                  if ( fld1->building && fieldvisiblenow ( fld1, actmap->playerview ) && ( fld1->building->visible || fld1->building->color == actmap->playerview*8 || godview ) )
                     buffer2[ j * mxsize + 1 + i*2] = fld1->building->color + buildingcoloroffset;
                  else
                     if ( fld1->vehicle && ((v == visible_all) || ((fld1->vehicle->height >= chschwimmend) && (fld1->vehicle->height <= chhochfliegend))))
                        buffer2[ j * mxsize + 1 + i*2] = fld1->vehicle->color + unitcoloroffset;
                     else
                        buffer2[ j * mxsize + 1 + i*2] = c;
         } else  {
            if ( v == visible_not )
               buffer2[ j * mxsize + i*2] = visiblenotcol;
            else
               if ( v == visible_ago)
                  buffer2[ j * mxsize + i*2] = xlattables.a.dark1[ c ];
               else 
                  if ( fld1->building && fieldvisiblenow ( fld1, actmap->playerview ) && ( fld1->building->visible || fld1->building->color == actmap->playerview*8 || godview )) 
                     buffer2[ j * mxsize + i*2] = fld1->building->color + buildingcoloroffset;
                  else
                     if ( fld1->vehicle && ((v == visible_all) || ((fld1->vehicle->height >= chschwimmend) && (fld1->vehicle->height <= chhochfliegend))))
                        buffer2[ j * mxsize + i*2] = fld1->vehicle->color + unitcoloroffset;
                     else
                        buffer2[ j * mxsize + i*2] = c;
         } 
         l++;
      } 
   } 
}

void tbasicshowmap::generatemap2 ( void )
{
   // mxsize = actmap->xsize * 4 + 4;

   word* k = (word*) buffer;
   *k = mxsize - 1;
   k++;
   *k = mysize - 1;


   int l,i,j;

   buffer2 = buffer + 4;
   pfield fld1;

   l = 0; 
   for (j = 0; j < actmap->ysize ; j++) {
      for (i = 0; i < actmap->xsize ; i++) { 
         fld1 = getfield ( i, j ) ;

         int v ;
         if ( godview )
            v = visible_all;
         else
            v = (fld1->visible >> (actmap->playerview*2)) & 3;

         if (j & 1) {
                    if ( v == visible_not ) {
                       buffer2 [ (j * 2 + 0) * mxsize + (i * 4) + 3 ] = visiblenotcol;
                       buffer2 [ (j * 2 + 1) * mxsize + (i * 4) + 2 ] = visiblenotcol;
                       buffer2 [ (j * 2 + 1) * mxsize + (i * 4) + 3 ] = visiblenotcol;
                       buffer2 [ (j * 2 + 1) * mxsize + (i * 4) + 4 ] = visiblenotcol;
                       buffer2 [ (j * 2 + 2) * mxsize + (i * 4) + 3 ] = visiblenotcol;
                    } else 
                       if ( v == visible_ago) {
                          buffer2 [ (j * 2 + 0) * mxsize + (i * 4) + 3 ] = xlattables.a.dark1[ fld1->typ->quickview->dir[fld1->direction].p3[1][0] ];
                          buffer2 [ (j * 2 + 1) * mxsize + (i * 4) + 2 ] = xlattables.a.dark1[ fld1->typ->quickview->dir[fld1->direction].p3[0][1] ];
                          buffer2 [ (j * 2 + 1) * mxsize + (i * 4) + 3 ] = xlattables.a.dark1[ fld1->typ->quickview->dir[fld1->direction].p3[1][1] ];
                          buffer2 [ (j * 2 + 1) * mxsize + (i * 4) + 4 ] = xlattables.a.dark1[ fld1->typ->quickview->dir[fld1->direction].p3[2][1] ];
                          buffer2 [ (j * 2 + 2) * mxsize + (i * 4) + 3 ] = xlattables.a.dark1[ fld1->typ->quickview->dir[fld1->direction].p3[1][2] ];
                        } else {
                          if ( fld1->building && fieldvisiblenow ( fld1, actmap->playerview ) && ( fld1->building->visible || fld1->building->color == actmap->playerview*8 || godview ) ) {
                             buffer2 [ (j * 2 + 0) * mxsize + (i * 4) + 3 ] = fld1->building->color + buildingcoloroffset;
                             buffer2 [ (j * 2 + 1) * mxsize + (i * 4) + 2 ] = fld1->building->color + buildingcoloroffset;
                             buffer2 [ (j * 2 + 1) * mxsize + (i * 4) + 3 ] = fld1->building->color + buildingcoloroffset;
                             buffer2 [ (j * 2 + 1) * mxsize + (i * 4) + 4 ] = fld1->building->color + buildingcoloroffset;
                             buffer2 [ (j * 2 + 2) * mxsize + (i * 4) + 3 ] = fld1->building->color + buildingcoloroffset;
                          } else
                             if ( fld1->vehicle && ((v == visible_all) || ((fld1->vehicle->height >= chschwimmend) && (fld1->vehicle->height <= chhochfliegend)))) {
                                buffer2 [ (j * 2 + 0) * mxsize + (i * 4) + 3 ] = fld1->vehicle->color + unitcoloroffset;
                                buffer2 [ (j * 2 + 1) * mxsize + (i * 4) + 2 ] = fld1->vehicle->color + unitcoloroffset;
                                buffer2 [ (j * 2 + 1) * mxsize + (i * 4) + 3 ] = fld1->vehicle->color + unitcoloroffset;
                                buffer2 [ (j * 2 + 1) * mxsize + (i * 4) + 4 ] = fld1->vehicle->color + unitcoloroffset;
                                buffer2 [ (j * 2 + 2) * mxsize + (i * 4) + 3 ] = fld1->vehicle->color + unitcoloroffset;
                             } else {
                                buffer2 [ (j * 2 + 0) * mxsize + (i * 4) + 3 ] = fld1->typ->quickview->dir[fld1->direction].p3[1][0];
                                buffer2 [ (j * 2 + 1) * mxsize + (i * 4) + 2 ] = fld1->typ->quickview->dir[fld1->direction].p3[0][1];
                                buffer2 [ (j * 2 + 1) * mxsize + (i * 4) + 3 ] = fld1->typ->quickview->dir[fld1->direction].p3[1][1];
                                buffer2 [ (j * 2 + 1) * mxsize + (i * 4) + 4 ] = fld1->typ->quickview->dir[fld1->direction].p3[2][1];
                                buffer2 [ (j * 2 + 2) * mxsize + (i * 4) + 3 ] = fld1->typ->quickview->dir[fld1->direction].p3[1][2];
                             }
                        }
         } else {
                    if ( v == visible_not ) {
                       buffer2 [ (j * 2 + 0) * mxsize + (i * 4) + 1 ] = visiblenotcol;
                       buffer2 [ (j * 2 + 1) * mxsize + (i * 4) + 0 ] = visiblenotcol;
                       buffer2 [ (j * 2 + 1) * mxsize + (i * 4) + 1 ] = visiblenotcol;
                       buffer2 [ (j * 2 + 1) * mxsize + (i * 4) + 2 ] = visiblenotcol;
                       buffer2 [ (j * 2 + 2) * mxsize + (i * 4) + 1 ] = visiblenotcol;
                    } else 
                       if ( v == visible_ago) {
                          buffer2 [ (j * 2 + 0) * mxsize + (i * 4) + 1 ] = xlattables.a.dark1[ fld1->typ->quickview->dir[fld1->direction].p3[1][0] ];
                          buffer2 [ (j * 2 + 1) * mxsize + (i * 4) + 0 ] = xlattables.a.dark1[ fld1->typ->quickview->dir[fld1->direction].p3[0][1] ];
                          buffer2 [ (j * 2 + 1) * mxsize + (i * 4) + 1 ] = xlattables.a.dark1[ fld1->typ->quickview->dir[fld1->direction].p3[1][1] ];
                          buffer2 [ (j * 2 + 1) * mxsize + (i * 4) + 2 ] = xlattables.a.dark1[ fld1->typ->quickview->dir[fld1->direction].p3[2][1] ];
                          buffer2 [ (j * 2 + 2) * mxsize + (i * 4) + 1 ] = xlattables.a.dark1[ fld1->typ->quickview->dir[fld1->direction].p3[1][2] ];
                        } else {
                          if ( fld1->building && fieldvisiblenow ( fld1, actmap->playerview ) && ( fld1->building->visible || fld1->building->color == actmap->playerview*8 || godview ) ) {
                             buffer2 [ (j * 2 + 0) * mxsize + (i * 4) + 1 ] = fld1->building->color + buildingcoloroffset;
                             buffer2 [ (j * 2 + 1) * mxsize + (i * 4) + 0 ] = fld1->building->color + buildingcoloroffset;
                             buffer2 [ (j * 2 + 1) * mxsize + (i * 4) + 1 ] = fld1->building->color + buildingcoloroffset;
                             buffer2 [ (j * 2 + 1) * mxsize + (i * 4) + 2 ] = fld1->building->color + buildingcoloroffset;
                             buffer2 [ (j * 2 + 2) * mxsize + (i * 4) + 1 ] = fld1->building->color + buildingcoloroffset;
                          } else
                             if ( fld1->vehicle && ((v == visible_all) || ((fld1->vehicle->height >= chschwimmend) && (fld1->vehicle->height <= chhochfliegend)))) {
                                buffer2 [ (j * 2 + 0) * mxsize + (i * 4) + 1 ] = fld1->vehicle->color + unitcoloroffset;
                                buffer2 [ (j * 2 + 1) * mxsize + (i * 4) + 0 ] = fld1->vehicle->color + unitcoloroffset;
                                buffer2 [ (j * 2 + 1) * mxsize + (i * 4) + 1 ] = fld1->vehicle->color + unitcoloroffset;
                                buffer2 [ (j * 2 + 1) * mxsize + (i * 4) + 2 ] = fld1->vehicle->color + unitcoloroffset;
                                buffer2 [ (j * 2 + 2) * mxsize + (i * 4) + 1 ] = fld1->vehicle->color + unitcoloroffset;
                             } else {
                                buffer2 [ (j * 2 + 0) * mxsize + (i * 4) + 1 ] = fld1->typ->quickview->dir[fld1->direction].p3[1][0];
                                buffer2 [ (j * 2 + 1) * mxsize + (i * 4) + 0 ] = fld1->typ->quickview->dir[fld1->direction].p3[0][1];
                                buffer2 [ (j * 2 + 1) * mxsize + (i * 4) + 1 ] = fld1->typ->quickview->dir[fld1->direction].p3[1][1];
                                buffer2 [ (j * 2 + 1) * mxsize + (i * 4) + 2 ] = fld1->typ->quickview->dir[fld1->direction].p3[2][1];
                                buffer2 [ (j * 2 + 2) * mxsize + (i * 4) + 1 ] = fld1->typ->quickview->dir[fld1->direction].p3[1][2];
                             }
                        }
         }
      }
   }
}


void tbasicshowmap::generatemap3 ( void )
{
   // mxsize = actmap->xsize * zoom*2 + zoom+2;

   word* k = (word*) buffer;
   *k = mxsize - 1;
   k++;
   *k = mysize - 1;


   int l,i,j,m,n;

   buffer2 = buffer + 4;
   pfield fld1;

   l = 0; 
   for (j = 0; j < actmap->ysize ; j++) {
      for (i = 0; i < actmap->xsize ; i++) { 
         fld1 = getfield ( i, j ) ;
         for (l = 0; l < 5 ; l++ ) {
             if (l < zoom)
                m = (zoom-1)-l;
             else
                m = l-(zoom-1) ;

             int v ;
             if ( godview )
                v = visible_all;
             else
                v = (fld1->visible >> (actmap->playerview*2)) & 3;

             if (j & 1) {
                for (n = m ; n < 5-m ; n++ )
                    if ( v == visible_not )
                       buffer2 [ (j * 3 + 0 + l) * mxsize + (i * 6) + 3 + n ] = visiblenotcol;
                    else
                       if ( v == visible_ago)
                          buffer2 [ (j * 3 + 0 + l) * mxsize + (i * 6) + 3 + n ] = xlattables.a.dark1[ fld1->typ->quickview->dir[fld1->direction].p5[n][l] ];
                       else
                          if ( fld1->building && fieldvisiblenow ( fld1, actmap->playerview ) && ( fld1->building->visible || fld1->building->color == actmap->playerview*8 || godview ) )
                             buffer2 [ (j * 3 + 0 + l) * mxsize + (i * 6) + 3 + n ]= fld1->building->color + buildingcoloroffset;
                          else
                             if ( fld1->vehicle && ((v == visible_all) || ((fld1->vehicle->height >= chschwimmend) && (fld1->vehicle->height <= chhochfliegend))))
                                buffer2 [ (j * 3 + 0 + l) * mxsize + (i * 6) + 3 + n ] = fld1->vehicle->color + unitcoloroffset;
                             else
                                buffer2 [ (j * 3 + 0 + l) * mxsize + (i * 6) + 3 + n ] = fld1->typ->quickview->dir[fld1->direction].p5[n][l];

             } else {
                for (n = m ; n < 5-m  ; n++ )
                    if ( v == visible_not )
                       buffer2 [ (j * 3 + 0 + l) * mxsize + (i * 6) + 0 + n ] = visiblenotcol;
                    else
                       if ( v == visible_ago)
                          buffer2 [ (j * 3 + 0 + l) * mxsize + (i * 6) + 0 + n ] = xlattables.a.dark1[ fld1->typ->quickview->dir[fld1->direction].p5[n][l] ];
                       else
                          if ( fld1->building && fieldvisiblenow ( fld1, actmap->playerview ) && ( fld1->building->visible || fld1->building->color == actmap->playerview*8 || godview ) )
                             buffer2 [ (j * 3 + 0 + l) * mxsize + (i * 6) + 0 + n ] = fld1->building->color + buildingcoloroffset;
                          else
                             if ( fld1->vehicle && ((v == visible_all) || ((fld1->vehicle->height >= chschwimmend) && (fld1->vehicle->height <= chhochfliegend))))
                                buffer2 [ (j * 3 + 0 + l) * mxsize + (i * 6) + 0 + n ] = fld1->vehicle->color + unitcoloroffset;
                             else
                                buffer2 [ (j * 3 + 0 + l) * mxsize + (i * 6) + 0 + n ] = fld1->typ->quickview->dir[fld1->direction].p5[n][l];
             }
         }
      }
   }
}

   
int hexfieldform[] = { 16,16,14,14,14,14,12,12,10,10,8,8,8,8,6,6,4,4,2,2,2,2,0,0 };

void tbasicshowmap::generatemap_var ( void )
{
   // mxsize = actmap->xsize * zoom*2 + zoom+2;

   mxsize = actmap->xsize * zoom ;
   mysize = actmap->ysize * zoom  * fielddisty / fielddistx;

   if ( mxsize > bufsizex || mysize > bufsizey || !buffer ) {
      if ( buffer ) 
         delete[] buffer;

      if ( mxsize > bufsizex )
         bufsizex = mxsize;
      if ( mysize > bufsizey )
         bufsizey = mysize;
      int size = bufsizex * bufsizey + 4;
  
      buffer = new unsigned char [ size + 1000 ] ;
   }

   word* k = (word*) buffer;
   *k = mxsize - 1;
   k++;
   *k = mysize - 1;

   int mapx = actmap->xsize * fielddistx;
   int mapy = fieldysize + (actmap->ysize - 1) * fielddisty;
   
   buffer2 = buffer + 4;
   pfield fld1;
   
   for ( int j = 0; j < mysize ; j++) {
      for ( int i = 0; i < mxsize ; i++) { 

         int line = j * mapy / mysize;
         int row =  i * mapx / mxsize;

         int yd = line / fielddisty;
         int xd = (row - ( yd & 1) * fielddisthalfx) / fielddistx ;

         int xo = row -  xd*fielddistx - ( yd & 1) * fielddisthalfx + fieldxsize / (zoom * 2);
         int yo = line - yd*fielddisty + fieldysize / (zoom * 2);

         int pix = getpixelfromimage ( icons.fieldshape, xo, yo );
         if ( (pix == -1  || pix == 255) &&   yd > 0  ) {
            if ( yo < fieldysize/2 )
               yd--;
            else
               yd++;
            xd = (row - ( yd & 1) * fielddisthalfx) / fielddistx ;
            xo = row -  xd*fielddistx - ( yd & 1) * fielddisthalfx + fieldxsize / (zoom * 2);
            yo = line - yd*fielddisty + fieldysize / (zoom * 2);
            pix = getpixelfromimage ( icons.fieldshape, xo, yo );

         }

         int cnt = 0;
         while ( (pix == -1  || pix == 255) && ( cnt < 10 )) {
            cnt++;
            if ( xo < fieldsizex/2 )
               xo+= 5;
            else
               xo-=5;

            if ( yo < fieldsizey/2 )
               yo+=5;
            else
               yo-=5;

            pix = getpixelfromimage ( icons.fieldshape, xo, yo );
         }

         unsigned char* b = &buffer2[ j * mxsize + i ];
         if ( !(pix == -1  || pix == 255) ) {
             fld1 = getfield ( xd, yd );
             if ( fld1 ) {
                int v ;
                if ( godview )
                   v = visible_all;
                else
                   v = (fld1->visible >> (actmap->playerview*2)) & 3;
   
   
                if ( v == visible_not )
                   *b = visiblenotcol;
                else
                   if ( v == visible_ago)
                      *b = xlattables.a.dark1[ fld1->typ->quickview->dir[fld1->direction].p5[ xo * 5 / fieldxsize ][ yo * 5 / fieldysize ] ];
                   else
                      if ( fld1->building && fieldvisiblenow ( fld1, actmap->playerview ) && ( fld1->building->visible || fld1->building->color == actmap->playerview*8 || godview ) )
                         *b = fld1->building->color + buildingcoloroffset;
                      else
                         if ( fld1->vehicle && ((v == visible_all) || ((fld1->vehicle->height >= chschwimmend) && (fld1->vehicle->height <= chhochfliegend))))
                            *b = fld1->vehicle->color + unitcoloroffset;
                         else {
                            *b = fld1->typ->quickview->dir[fld1->direction].p5[ xo * 5 / fieldxsize ][ yo * 5 / fieldysize ];
                         }
    
            } else
               *b = dblue;
         } else
            *b = dblue;
      }
   }
}


int tbasicshowmap::getfieldposx ( int c )
{
   #ifdef FREEZOOM
   return mxsize * c / actmap->xsize;
   #else
   return c * zoom * 2;
   #endif 
}

int tbasicshowmap::getfieldposy ( int c )
{
   #ifdef FREEZOOM
   return mysize * c / actmap->ysize;
   #else
   return c * zoom;
   #endif 
}


int tbasicshowmap::getposfieldx ( int c )
{
   #ifdef FREEZOOM
   return c * actmap->xsize / mxsize ;
   #else
   return c / (zoom * 2);
   #endif 
}

int tbasicshowmap::getposfieldy ( int c )
{
   #ifdef FREEZOOM
   return c * actmap->ysize / mysize ;
   #else
   return c / zoom;
   #endif 
}


void tbasicshowmap::dispimage ( void )
{
   int xsize;
   int ysize;
   int x1 = xp1;
   int y1 = yp1;

   if ( mxsize > txsize )
      xsize = txsize;
   else
      xsize = mxsize;

   if ( mysize > tysize )
      ysize = tysize;
   else
      ysize = mysize;

   setinvisiblemouserectanglestk ( xp1, yp1, xp1 + txsize, yp1 + tysize );
   collategraphicoperations cgo ( xp1, yp1, xp1 + txsize, yp1 + tysize );;

   mapwindow.x1 = (txsize - xsize) / 2;
   mapwindow.y1 = (tysize - ysize) / 2;
   mapwindow.x2 = mapwindow.x1 + xsize;
   mapwindow.y2 = mapwindow.y1 + ysize;

   if ((lastmapxsize > xsize) && (lastmapysize > ysize))
      bar ( x1 + (txsize - lastmapxsize) / 2 - 5, y1 + (tysize - lastmapysize) / 2 - 5, x1 + (txsize + lastmapxsize) / 2 + 5, y1 + (tysize + lastmapysize) / 2 + 5, dblue );
   else
     if ( lastmapxsize > xsize )
        bar ( x1 + (txsize - lastmapxsize) / 2 - 5, y1 + (tysize - ysize) / 2 - 5, x1 + (txsize + lastmapxsize) / 2 + 5, y1 + (tysize + ysize) / 2 + 5, dblue );
     else
        if (lastmapysize > ysize )
          bar ( x1 + (txsize - xsize) / 2 - 5, y1 + (tysize - lastmapysize) / 2 - 5, x1 + (txsize + xsize) / 2 + 5, y1 + (tysize + lastmapysize) / 2 + 5, dblue );

   scrxsize = idisplaymap.getscreenxsize();
   scrysize = idisplaymap.getscreenysize();


// Bildausschnitt soll in Fenster komplett drin sein

   if ( getfieldposx ( dispxpos ) - xofs < 0 )
      xofs = getfieldposx ( dispxpos );
      
   if ( getfieldposy ( dispypos ) - yofs < 0 )
      yofs = getfieldposy ( dispypos ) ;
                                  

   if ( getfieldposx ( dispxpos + scrxsize ) - xofs > xsize)
      xofs = getfieldposx ( dispxpos + scrxsize ) - xsize;

/*
   if ( ( dispxpos + scrxsize ) * zoom * 2 + zoom - xofs > xsize)
      xofs = ( dispxpos + scrxsize ) * zoom * 2 + zoom - xsize;
*/

   if ( getfieldposy ( dispypos + scrysize ) - yofs > ysize)
      yofs = getfieldposy ( dispypos + scrysize ) - ysize;
                                                                            



   if ( xsize + xofs >= mxsize )
      xofs = mxsize - xsize - 1 ;

   if ( ysize + yofs >= mysize )
      yofs = mysize - ysize - 1 ;

   if ( xofs < 0)
      xofs = 0;

   if ( yofs < 0)
      yofs = 0;


   if ( (mxsize > txsize ) || ( mysize > tysize )) 
      putimageprt ( x1 + mapwindow.x1 , y1 + mapwindow.y1, x1 + mapwindow.x2, y1 + mapwindow.y2, buffer, xofs, yofs );
   else
      putspriteimage ( x1 + mapwindow.x1 , y1 + mapwindow.y1,  buffer );
      
   rectangle ( x1 + mapwindow.x1 + getfieldposx ( dispxpos ) - xofs,                y1 + mapwindow.y1 + getfieldposy ( dispypos ) - yofs, 
               x1 + mapwindow.x1 + getfieldposx ( dispxpos + scrxsize ) - xofs - 1, y1 + mapwindow.y1 + getfieldposy ( dispypos + scrysize ) - yofs - 1, 14 );

   lastmapxsize = xsize;
   lastmapysize = ysize;

   if ( lastmapposx != xofs  ||  lastmapposy != yofs ) 
      while ( ticker < lastmaptick + maxmapscrollspeed )
         releasetimeslice();

   lastmaptick = ticker;
   lastmapposx = xofs;
   lastmapposy = yofs;


   getinvisiblemouserectanglestk();
}


void tbasicshowmap :: saveimage ( void )
{
   word* pw = (word*) buffer;
   int width = pw[0] + 1;
   int height = pw[1] + 1;

   tvirtualdisplay vd ( width + 10 , height + 10 );

   putimage ( 0, 0, buffer );
   writepcx ( getnextfilenumname ( "smap", "pcx" ), 0, 0, width-1, height-1, pal );

}

void tbasicshowmap :: checkformouse ( void )
{
    if ( mouseinrect ( mapwindow.x1 + xp1, mapwindow.y1 + yp1, mapwindow.x2 + xp1, mapwindow.y2 + yp1 )) {
       int oldx = dispxpos;
       int oldy = dispypos;

       dispxpos = getposfieldx ( mouseparams.x + xofs - xp1 - mapwindow.x1) - scrxsize / 2; 
       dispypos = getposfieldy ( mouseparams.y + yofs - yp1 - mapwindow.y1) - scrysize / 2;

       if ( dispxpos < 0 )
          dispxpos = 0;
       if ( dispxpos + scrxsize > actmap->xsize )
          dispxpos = actmap->xsize - scrxsize;

       if ( dispypos < 0 )
          dispypos = 0;
       if ( dispypos + scrysize > actmap->ysize )
          dispypos = actmap->ysize - scrysize;

       if ( dispypos & 1 )
          dispypos--;


       if ( oldx != dispxpos  ||  oldy != dispypos )
          dispimage();

    } else
       if ( mouseinrect ( xp1 - 5 , yp1 - 5, xp1 + txsize + 5, yp1 + tysize + 5 )) {

          int oldx = dispxpos;
          int oldy = dispypos;

         if ( mouseinrect ( xp1 + mapwindow.x2, yp1, xp1 + txsize, yp1 + tysize )) 
            if ( dispxpos + scrxsize < actmap->xsize ) 
               dispxpos++;

         if ( mouseinrect ( xp1 , yp1, xp1 + mapwindow.x1 , yp1 + tysize )) 
           if ( dispxpos > 0) 
              dispxpos--;

         if ( mouseinrect ( xp1 , yp1 + mapwindow.y2, xp1 + txsize , yp1 + tysize )) 
            if ( dispypos + scrysize + 1 < actmap->ysize )  
               dispypos+=2;

         if ( mouseinrect ( xp1 , yp1, xp1 + txsize , yp1 + mapwindow.y1 )) 
           if ( dispypos > 1) 
              dispypos-=2;

         if ( oldx != dispxpos  ||  oldy != dispypos )
            dispimage();

       }

}

void tbasicshowmap :: setmapposition ( void )
{
         actmap->xpos = dispxpos;
         actmap->ypos = dispypos;
         cursor.posx = scrxsize / 2;
         cursor.posy = scrysize / 2;
}

void tshowmap::run ( void )                             
{
   if ( generatemap( 1 ) )
      return;

   dispimage();

   mousevisible ( true );

   int mousepressed = 0;

   do {

      tdialogbox::run ( );

      #ifdef karteneditor
      if ( taste == ct_a + ct_stp )
         saveimage();
      #endif

      int oldzoom = zoom;
      if ( (taste == ct_plus || taste == ct_plusk ) && (zoom < maxzoom) ) 
         zoom++;
      if ( (taste == ct_minus || taste == ct_minusk) && (zoom > 1) ) 
         zoom--;

      if ( (taste == ct_right)) 
         if ( dispxpos + scrxsize < actmap->xsize ) {
            dispxpos++;
            dispimage();
         }
      if ( (taste == ct_left)) 
         if ( dispxpos > 0) {
            dispxpos--;
            dispimage();
         }

      if ( (taste == ct_down)) 
         if ( dispypos + scrysize + 1 < actmap->ysize )  {
            dispypos+=2;
            dispimage();
         }
      if ( (taste == ct_up)) 
         if ( dispypos > 1) {
            dispypos-=2;
            dispimage();
         }

      if (oldzoom != zoom) {
         freebuf();
         if ( generatemap( 0 ) )
            return;
         dispimage();
      } /* endif */

      if ( mouseparams.taste == 0  && mousepressed == 1 )
         taste = ct_enter;

      if ( taste == ct_enter ) 
         setmapposition();

      if ( mouseparams.taste & 2 )
         mousepressed = 0;


      if ( mouseparams.taste == 1 ) {
         mousepressed = 1;
         checkformouse();
      }
   } while ( taste != ct_esc && taste != ct_enter ); /* enddo */
   delete[]  buffer ;
   buffer = NULL;
}



void showmap ( void )
{
   mousevisible( false );

   tshowmap sm;
   sm.init();
   sm.run();
   sm.done();

   mousevisible( true );

}












typedef char* tnamestrings[8];
typedef tnamestrings* pnamestrings ;




class  tenternamestrings : public tdialogbox {
                     typedef tdialogbox inherited;
                  public:
                       char             status;
                       char             playerexist;
                       pnamestrings     orgnames;
                       char             names[8][100];
                       char             buttonnames[8][100];

                       void             init( pnamestrings namestrings, char plyexistyte, char md);
                       virtual void     run ( void );
                       virtual void     buttonpressed( char id );
                       void             done ( void );
                       virtual int      getcapabilities ( void );
                    };


class  tsetalliances : public tdialogbox {
                     typedef tdialogbox inherited;
               protected:
                     virtual void redraw ( void );       
                     tshareview          sv;
               public:
                     char                status;
                     // char                alliances[8];
                     char                alliancedata[8][8];
                     tnamestrings        computername;
                     tnamestrings        computerplayername;
                     tnamestrings        humanplayername;
                     // tnamestrings        alliancename;
                     char                location[8];
                     int                 playerpos[8];  // Beispiel: Es existieren Spieler 0 und Spieler 6; dann ist playerpos[0] = 0 und playerpos[1] = 6
                     int                 playermode[8];    /*  0: player
                                                               1: ai
                                                               2: off  */

                     char                playernum;
                     char                playerexist;
                     char                lastplayer;
                     int                 oninit;
                     int                 supervisor;
                     shortint            xp,yp,xa,ya,bx;  /* position der tastatur-markierungsrechtecke  */

                     void                init ( int supervis );
                     virtual void        buttonpressed( char id);
                     virtual void        run ( void );
                     virtual int         getcapabilities ( void );
                     void                click( char bxx, char x, char y);
                     void                paintkeybar ( void );
                     void                buildhlgraphics ( void );
                     void                displayplayernamesintable( void );
                     #ifndef karteneditor
                     void                checkfornetwork ( void );
                     #endif
                     void                setparams ( void );
                };

static const char*    cens[]  = { "player", "alliance", "computer" };


void         tenternamestrings::init(  pnamestrings namestrings, char plyexist, char md )
{ 

  char         i, j;

  tdialogbox::init();
  playerexist = plyexist;
  j = 0; 
  for (i = 0; i <= 7; i++) 
    if (playerexist & (1 << i)) 
      j++;

  title = new char [ 100 ];
  strcpy ( title, "enter " );
  strcat ( title, cens[md] );
  strcat ( title, " names" );

  xsize = 350; 
  x1 = (640 - xsize) / 2; 
  ysize = 140 + j * 45; 
  y1 = (480 - ysize) / 2; 


  for (i=0;i<8 ;i++ ) {
     if ( (*namestrings)[i] ) 
        strcpy ( names[i], (*namestrings)[i] );
     else 
        names[i][0] = 0;

     strcpy( buttonnames [i], cens[md] );
     strcat( buttonnames [i], digit[i] );
  } /* endfor */

  orgnames =  namestrings ;

  j = 0; 
  for (i = 0; i <= 7; i++) 
    if ( (playerexist & (1 << i)) && names[i] ) { 
      j++;
      addbutton(buttonnames[i],20,30 + j * 45,xsize - 20,52 + j * 45,1,0,i + 1,true);
      addeingabe(i + 1, names[i], 1, 99);
    } 
  addbutton("~o~k",20,ysize - 40,xsize / 2 - 5,ysize - 10,0,1,30,true); 
  addbutton("~c~ancel",xsize / 2 + 5,ysize - 40,xsize - 20,ysize - 10,0,1,31,true); 
  buildgraphics(); 
  status = 0; 
} 

void         tenternamestrings::done ( void ) 
{
  delete[] title;
  tdialogbox::done();
}


void         tenternamestrings::buttonpressed( char id )
{ 
  inherited::buttonpressed(id);
  if (id == 30) status = 2; 
  if (id == 31) status = 1; 
} 


void         tenternamestrings::run(void)
{ 
  mousevisible(true); 
  do { 
    tdialogbox::run();
  }  while (status == 0);

  if (status == 2) { 
     for (int i=0;i<8 ;i++ ) {
        if ( (*orgnames)[i] ) {
           if ( names[i] )
              strcpy ( (*orgnames)[i] , names[i] );
           else
              (*orgnames)[i][0] = 0;
        } /* endif */
     } /* endfor */
  } 
} 



int               tenternamestrings::getcapabilities ( void )
{
  return 1;
}


  #define tsa_namelength 100  
  #define ply_x1 25  
  #define ply_y1 55  
  #define ply_lineheight 22  
  #define ali_x1 30 + tsa_namelength  
  #define ali_y1 275  

const char*  playermodenames[]  = {"player ", "ai ", "off "};
const char   alliancecolors[]  = {0, 1, 2, 3, 4, 5, 6, 7};


void         tsetalliances::init( int supervis )
{ 
  #ifdef karteneditor
  oninit = 1;
  supervisor = 1;
  #else
  if ( actmap->actplayer == -1 ) {
     oninit = 1;
     supervisor = 1;
  } else {
    oninit = 0;
    supervisor = 0;
  }
  #endif

  if ( supervis )
     supervisor = 1;

   tdialogbox::init();
   title = "set alliances";
   status = 0; 
   x1 = 30; 
   y1 = 10; 
   xsize = 580; 
   ysize = 460; 
   addbutton("~o~k",400,240,xsize - 20,280,0,1,1,true); 
   addkey(1,ct_enter); 

   addbutton("~h~elp",400,45,xsize - 20,70,0,1,2,true); 
   addkey ( 2, ct_f1 );

   addbutton("~p~layer names",400,80,xsize - 20,105,0,1,3,true); 
   #ifndef karteneditor
   addbutton("~c~omputer names",400,115,xsize - 20,140,0,1,4,true); 
   addbutton("~n~etwork",400,160,xsize - 20,190,0,1,5, actmap->network != NULL ); 
   #endif
   if ( !supervisor ) 
     addbutton("~s~upervisor",400,200,xsize - 20,230,0,1,6, actmap->supervisorpasswordcrc != 0 ); 

   // buildgraphics(); 

   int lastcomppos = 0;
   int plnum = 0;
   int i, j;

   for (i = 0; i <= 7; i++) { 

     if ( actmap->player[i].firstvehicle || actmap->player[i].firstbuilding )
        actmap->player[i].existent = true;
     else
        actmap->player[i].existent = false;

      playermode[i] = (enum tplayerstat) actmap->player[i].stat;

      if ( actmap->player[i].existent )  // ((playermode[i] == ps_human) || (playermode[i] == ps_computer)) &&
         playerpos[plnum++] = i;

      humanplayername[i] = new char [100];
      strcpy ( humanplayername[i] , actmap->humanplayername[i] ); 

      computerplayername[i] = new char [ 100 ];
      strcpy ( computerplayername[i] , actmap->computerplayername[i] ); 

      computername[i] = new char [ 100 ];
      if ( actmap->network && actmap->network->computer[i].name )
         strcpy ( computername[i] , actmap->network->computer[i].name ); 
      else {
         strcpy ( computername[i] , "computer " ); 
         strcat ( computername[i] , digit[i] ); 
      }

      if ( actmap->player[i].existent && actmap->network ) {
        location[i] = actmap->network->player[i].compposition; 
        lastcomppos = location[i];
      } else
        location[i] = lastcomppos;

      if (actmap->player[i].existent) 
        lastplayer = i; 

      for (j = 0; j < 8 ; j++) 
         if ( actmap->shareview ) 
            sv.mode[i][j] = actmap->shareview->mode[i][j];
         else
            sv.mode[i][j] = sv_none;
   } 


   for (i = 0; i < 8; i++) 
      for ( char j = 0; j < 8; j++) 
         alliancedata[i][j] = actmap->alliances[i][j] ;

   if ( oninit )
      if ( actmap->shareview ) 
         for ( i = 0; i < 8; i++ )
            for (int j = 0; j < 8 ; j++) 
               if ( sv.mode[i][j] == sv_shareview ) {
                  alliancedata[i][j] = capeace_with_shareview;
                  alliancedata[j][i] = capeace_with_shareview;
               }


   #ifdef karteneditor   
   for (i = 0; i <= 7; i++) {
         if ((actmap->player[i].firstvehicle != NULL) || (actmap->player[i].firstbuilding != NULL))  
            actmap->player[i].existent = true; 
         else 
           actmap->player[i].existent = false;
   } 
   #endif   


   playernum = 0; 
   playerexist = 0; 

   xp = 0; 
   yp = 0; 
   xa = 0; 
   ya = 0; 
   bx = -1; 

   buildgraphics ( );
} 



int          tsetalliances::getcapabilities ( void )
{
   return 1;
}


void          tsetalliances::redraw ( void )
{
  collategraphicoperations cgo ( x1, y1, x1 + xsize, y1 + ysize );

   inherited::redraw();

   rahmen3("player",x1 + 10,y1 + 45,x1 + 380,y1 + 230,1); 
   rahmen3("diplomacy",x1 + 10,y1 + 240,x1 + 380,y1 + ysize - 10,1); 

   activefontsettings.font = schriften.smallarial; 
   activefontsettings.justify = lefttext; 
   activefontsettings.height = (schriften.smallarial)->height;

   buildhlgraphics();
}


void         tsetalliances::displayplayernamesintable( void )
{
   collategraphicoperations cgo ( x1, y1, x1 + xsize, y1 + ysize );;
   npush ( activefontsettings );
   activefontsettings.background = dblue; 
   activefontsettings.length = ali_x1 - 41 ;
   for (int j = 0; j < playernum ; j++) {   /*  y */ 
      activefontsettings.color = 20 + 8 * playerpos[j]; 
      if ( playermode[playerpos[j]] == ps_human )
         showtext2( humanplayername[playerpos[j]], x1 + 40,y1 + ali_y1 + j * 22);
      else
         if ( playermode[playerpos[j]] == ps_computer )
            showtext2( computerplayername[playerpos[j]], x1 + 40,y1 + ali_y1 + j * 22);
         else
            showtext2( "off", x1 + 40,y1 + ali_y1 + j * 22);
   }
   npop ( activefontsettings );
}

char* shareview_modenames[2] = { "none", "share view" };


void         tsetalliances::buildhlgraphics(void)
{ 
   collategraphicoperations cgo ( x1, y1, x1 + xsize, y1 + ysize );;

  byte         i, j;
  byte         mss; 

   mss = getmousestatus(); 
   if (mss == 2)
     mousevisible(false); 
   paintkeybar(); 
   playernum = 0; 
   for (i = 0; i <= 7; i++) { 
      if (actmap->player[i].existent) { 
         activefontsettings.color = 23 + i * 8; 
         activefontsettings.background = 17 + i * 8; 
         activefontsettings.length = tsa_namelength;

         char* c;
         switch ( playermode[i] ) {
            case 0: c = humanplayername[i];
                    break;
            case 1: c = computerplayername[i];
                    break;
            case 2: c = "off";
                    break;
         } /* endswitch */

         showtext2( c, x1 + ply_x1,y1 + ply_y1 + i * 22); 

         activefontsettings.background = dblue; 
         showtext2("(at)",x1 + ply_x1 + tsa_namelength,y1 + ply_y1 + i * 22);

         activefontsettings.color = alliancecolors[location[i]]; 
         showtext2(computername[location[i]],x1 + ply_x1 + 30 + tsa_namelength,y1 + ply_y1 + i * 22); 

         #ifndef kartened
         if ( actmap->actplayer >= 0   &&  !oninit )
            if ( i != actmap->actplayer )
               showtext2(shareview_modenames[sv.mode[actmap->actplayer][i]],x1 + 40 + ply_x1 + 2 * tsa_namelength,y1 + ply_y1 + i * 22); 

         #endif

         playernum++; 
         playerexist |= 1 << i;
      } 
   } 

   activefontsettings.background = dblue; 
   for (i = 0; i < playernum ; i++) { 
      activefontsettings.color = 20 + 8 * playerpos[i]; 
      showtext2( letter[i], x1 + ali_x1 + i * 30,y1 + ali_y1 - 25);
   } 


   for (j = 0; j < playernum ; j++) {   /*  y */ 

      activefontsettings.color = 20 + 8 * playerpos[j]; 
      showtext2( letter[j] ,x1 + 25,y1 + ali_y1 + j * 22);

      if ( playermode[playerpos[j]] != 2 )
         for (i = 0; i < playernum ; i++) {   /*  x  */ 
              /* if i=j then
               nputimage(x1+ali_x1+i*30,y1+ali_y1+j*22,icons.diplomaticstatusinv)
            else */ 
            if (i != j) 
              if ( playermode[playerpos[i]] != 2 )
                  putimage(x1 + ali_x1 + i * 30,y1 + ali_y1 + j * 22,icons.diplomaticstatus[alliancedata[playerpos[i]][playerpos[j]]]); 
         } 
   } 
   displayplayernamesintable ( );
   paintkeybar(); 
   if (mss == 2) 
     mousevisible(true); 
} 



void         tsetalliances::paintkeybar(void)
{ 
  collategraphicoperations cgo ( x1, y1, x1 + xsize, y1 + ysize );
 int          xx1, xx2;

   if (bx == 0) { 
      xx1 = x1 + ply_x1 - 3; 
      xorrectangle(xx1,y1 + ply_y1 - 3 + yp * 22,xx1 + 48 + tsa_namelength * 3,y1 + ply_y1 - 3 + (yp + 1) * 22,14); 
      if (xp > 0) 
        xx1 = xx1 + 20; 
      xx2 = xx1 + 17 + (xp + 1) * tsa_namelength; 
      if (xp > 1) 
        xx2 = x1 + ply_x1 - 3 + 48 + tsa_namelength * 3; 
      xorrectangle(xx1 + xp * tsa_namelength,y1 + ply_y1 - 3,xx2,y1 + ply_y1 - 3 + (lastplayer + 1) * 22,100); 
   } 
   if (bx == 1) { 
      xorrectangle(x1 + ali_x1 - 5,y1 + ali_y1 - 3 + ya * 22,x1 + ali_x1 + (playernum) * 30 - 12,y1 + ali_y1 - 5 + (ya + 1) * 22,14); 
      xorrectangle(x1 + ali_x1 - 5 + xa * 30,y1 + ali_y1 - 3,x1 + ali_x1 + (xa + 1) * 30 - 12,y1 + ali_y1 - 5 + playernum * 22,100); 
   } 
} 


#ifndef karteneditor
void          tsetalliances::checkfornetwork ( void )
{
  int cmp = 0;
  int i;

  for (i = 0; i < 8; i++) 
     if ( actmap->player[i].existent )
        cmp |= 1 << location[i];

  int num = 0;
  for ( i = 1; i < 256; i<<=1 ) 
     if ( cmp & i )
        num++;

  if ( num >= 2 ) {
     if ( actmap->network == NULL ) {
        actmap->network = new ( tnetwork );
        // memset ( actmap->network, 0, sizeof ( *actmap->network ));
        enablebutton ( 5 );
     }
  } else {
     if ( actmap->network ) {
        for (i = 0; i < 8; i++) 
           if ( actmap->network->computer[i].name )
              delete[]  actmap->network->computer[i].name ;

        delete  actmap->network ;
        actmap->network = NULL;
        disablebutton( 5 );
     }
  }
}
#endif

void         tsetalliances::setparams ( void )
{
  #ifdef logging
  char tmpcbuf[300];
  #endif

  int i, j;

  if ( actmap->network ) {
     for ( int i = 0; i < 8; i++ )
         actmap->network->computer[i].existent = 0;
  }
  for (i = 0; i < 8; i++) {
      
      #ifdef logging
      logtofile ( "6/dialog / tsetalliances::setparams / alliancedata" );
      #endif
         
      for (j = 0; j < 8; j++) 
         actmap->alliances[i][j] = alliancedata[i][j];
//      actmap->player[i].alliance = alliances[i];

      #ifdef logging
      sprintf(tmpcbuf,"6/dialog / tsetalliances::setparams / humanplayername; old address is %x, source address is %x",actmap->humanplayername[i], humanplayername[i] );
      logtofile ( tmpcbuf );
      #endif
      delete[] actmap->humanplayername[i];
      actmap->humanplayername[i] = strdup ( humanplayername[i] );
     
      #ifdef logging
      sprintf(tmpcbuf,"6/dialog / tsetalliances::setparams / humanplayername; new address is %x",actmap->humanplayername[i] );
      logtofile ( tmpcbuf );
      #endif


      #ifdef logging
      sprintf(tmpcbuf,"6/dialog / tsetalliances::setparams / computerplayername; old address is %x, source address is %x",actmap->computerplayername[i], computerplayername[i] );
      logtofile ( tmpcbuf );
      #endif
      delete[] actmap->computerplayername[i];
      actmap->computerplayername[i] = strdup ( computerplayername[i] ); 
      #ifdef logging
      sprintf(tmpcbuf,"6/dialog / tsetalliances::setparams / computerplayername; new address is %x",actmap->computerplayername[i] );
      logtofile ( tmpcbuf );
      #endif

      /*
      #ifdef logging
      logtofile ( "6/dialog / tsetalliances::setparams / alliancename" );
      #endif

      delete actmap->alliancenames[i];
      actmap->alliancenames[i] = strdup ( alliancename[i] ); 
      */


      if ( actmap->player[i].existent ) {
         char* c;
         switch ( playermode[i] ) {
            case 0: c = actmap->humanplayername[i];
                    break;
            case 1: c = actmap->computerplayername[i];
                    break;
            case 2: c = NULL;
                    break;
         } /* endswitch */

         actmap->player[i].name = c;

         if ( actmap->network )
            actmap->network->computer[ location[i] ].existent = 1;
      }

      #ifdef logging
      logtofile ( "6/dialog / tsetalliances::setparams / network" );
      #endif
      if ( actmap->network ) {
         if ( actmap->network->computer[i].name )
            delete[]  actmap->network->computer[i].name ;
         actmap->network->computer[i].name = strdup ( computername [ i ] );
         actmap->network->player[i].compposition = location[i];
      }

      actmap->player[i].stat = playermode[i];

   } /* endfor */

  if ( actmap->network ) {
      actmap->network->computernum = 0;
      for ( i = 0; i < 8; i++ )
         if ( actmap->network->computer[i].existent )
            actmap->network->computernum++;
  }
  for ( i = 0; i < 8; i++ )
     for (j = 0; j < 8; j++ ) {
        if ( actmap->alliances[i][j] == canewsetwar1 ) {
           actmap->alliances[i][j] = canewsetwar2;
           if ( actmap->shareview ) {
              if ( sv.mode[i][j] != sv_none )
                 sv.recalculateview = 1;
              sv.mode[j][i] = sv_none;
              sv.mode[i][j] = sv_none;
           }
        }
        if ( oninit ) {
           if ( actmap->alliances[i][j] == capeace_with_shareview ) {
              actmap->alliances[i][j] = capeace;
              sv.mode[j][i] = sv_shareview;
              sv.mode[i][j] = sv_shareview;
           } else {
              sv.mode[j][i] = sv_none;
              sv.mode[i][j] = sv_none;
           }
        }


      #ifdef logging
      logtofile ( "6/dialog / tsetalliances::setparams / returning" );
      #endif
    }
  int svnum = sv.recalculateview;
  for ( i = 0; i < 8; i++ )
     for ( int j = 0; j < 8; j++ )
        if ( sv.mode[i][j] != sv_none )
           svnum++;

  if ( actmap->shareview && !svnum ) {
     delete actmap->shareview;
     actmap->shareview = NULL;
  } else
    if ( actmap->shareview || svnum ) {
       if ( !actmap->shareview )
          actmap->shareview = new tshareview;

       *actmap->shareview = sv;
    }

}


void         tsetalliances::click(byte         bxx,
                                byte         x,
                                byte         y)
{ 

   mousevisible(false);
   activefontsettings.color = 23 + y * 8; 
   activefontsettings.length = tsa_namelength; 
   activefontsettings.background = dblue; 
   if (bxx == 0) { 
      if (x == 0  && ( y == actmap->actplayer || supervisor ) ) { 
        #ifdef __WATCOM_CPLUSPLUS__
         playermode[y]++;
        #else
         playermode[y] += 1;
        #endif
         if ( actmap->actplayer == -1 ) {
            if (playermode[y] > 2) 
               playermode[y] = ps_human; 
         } else {
            if (playermode[y] > 1) 
               playermode[y] = ps_human; 
         }

         char* c;
         switch ( playermode[y] ) {
            case 0: c = humanplayername[y];
                    break;
            case 1: c = computerplayername[y];
                    break;
            case 2: c = "off";
                    break;
         } /* endswitch */

         activefontsettings.background = 17 + y * 8;
         showtext2( c, x1 + ply_x1,y1 + ply_y1 + y * 22); 
         displayplayernamesintable(  );
      } 
      #ifndef karteneditor

      int nextplayer = actmap->actplayer;
      do {
         nextplayer++;
         if ( nextplayer > 7 )
           nextplayer = 0;
      } while ( !actmap->player[nextplayer].existent ); /* enddo */

      if (x == 1 && ( y == actmap->actplayer || y == nextplayer || supervisor ) ) { 
         location[y]++;
         if (location[y] >= playernum) 
            location[y] = 0; 
         activefontsettings.color = alliancecolors[location[y]]; 
         showtext2(computername[location[y]],x1 + ply_x1 + 30 + tsa_namelength,y1 + ply_y1 + y * 22); 
         checkfornetwork();
      } 
      #endif
      
      #ifndef kartened
      if (x == 2  &&  ( y != actmap->actplayer ) && actmap->actplayer>=0  &&  !oninit ) 
         if ( (actmap->alliances[actmap->actplayer][y] == capeace && actmap->alliances[y][actmap->actplayer] == capeace) ||  sv.mode[actmap->actplayer][y] == sv_shareview ) {
                                                 
            if ( sv.mode[actmap->actplayer][y] == sv_shareview )
               sv.mode[actmap->actplayer][y] = sv_none;
            else {
             #ifdef __WATCOM_CPLUSPLUS__
              sv.mode[actmap->actplayer][y] ++;
             #else
              sv.mode[actmap->actplayer][y] += 1;
             #endif
            }
   
            activefontsettings.color = 23 + y * 8; 
            activefontsettings.background = dblue; 
            activefontsettings.length = tsa_namelength;
   
            activefontsettings.color = alliancecolors[location[y]]; 
            showtext2(shareview_modenames[sv.mode[actmap->actplayer][y]],x1 + 40 + ply_x1 + 2 * tsa_namelength,y1 + ply_y1 + y * 22); 
   
         } 
      #endif
      
   } 
   else { 
      if (x != y) { 
         if ( oninit ) {
            if (x < playernum  && y < playernum) {
               if ( playermode[playerpos[x]] != 2  &&  playermode[playerpos[y]] != 2 ) {

                  if ( alliancedata[playerpos[x]][playerpos[y]] == capeace )
                     alliancedata[playerpos[x]][playerpos[y]] = capeace_with_shareview;
                  else 
                     if ( alliancedata[playerpos[x]][playerpos[y]] == cawar )
                        alliancedata[playerpos[x]][playerpos[y]] = capeace;
                     else
                        alliancedata[playerpos[x]][playerpos[y]] = cawar;
   
                  alliancedata[playerpos[y]][playerpos[x]] = alliancedata[playerpos[x]][playerpos[y]]; 
   
                  putimage(x1 + ali_x1 + x * 30,y1 + ali_y1 + y * 22,icons.diplomaticstatus[alliancedata[playerpos[x]][playerpos[y]]]); 
                  putimage(x1 + ali_x1 + y * 30,y1 + ali_y1 + x * 22,icons.diplomaticstatus[alliancedata[playerpos[x]][playerpos[y]]]); 
               }
            } /* endif */
         } else {
            if ( x < playernum  && y < playernum && playermode[playerpos[x]] != 2  &&  playermode[playerpos[y]] != 2 ) 
               if ( playerpos[y] == actmap->actplayer  &&  x < playernum ) {
                  char* c = &alliancedata[playerpos[x]][playerpos[y]];
                  if ( *c == capeaceproposal ||  *c == cawar ) {
                     if ( *c == capeaceproposal )
                        *c = cawar;
                     else
                        *c = capeaceproposal;
                  } else 
                     if ( *c != canewsetwar2 ) {
                        if ( *c == capeace )
                           *c = cawarannounce;
                        else
                           if ( *c == cawarannounce )
                              *c = canewsetwar1;
                           else
                              *c = capeace;
                     }
                  putimage(x1 + ali_x1 + x * 30,y1 + ali_y1 + y * 22,icons.diplomaticstatus[ *c ] ); 
               } /* endif */

         }
      }
   } 
   mousevisible(true); 
} 


#ifndef karteneditor


int taskforsupervisorpassword :: checkforreask ( int crc )
{
   if ( crc == 0 || actmap->actplayer == -1 )
      return 1;
   else 
      return 0;

}

void taskforsupervisorpassword :: init ( int* crc, int mode )
{
   tenterpassword::init ( crc, mode, "supervisor password" );
}


#endif

void         tsetalliances::buttonpressed( char id )
{ 
  int i;

   switch (id) {
      
      case 1:   status = 10; 
      break; 
      case 2: if ( oninit )
                 help ( 61 );
              else
                 help ( 60 );
      break; 
/*      
      case 2: {   
                tenternamestrings   enternamestrings;
                int allianceexist = 0;
                for (int i = 0; i < playernum; i++ ) 
                   allianceexist |= ( 1 << i );
                   
                enternamestrings.init( &alliancename, allianceexist, 1);
                enternamestrings.run(); 
                enternamestrings.done(); 
                buildhlgraphics(); 
              }
              break; 
*/      

      case 3: {
                 tenternamestrings   enternamestrings;
                 tnamestrings names;
                 for (i = 0; i < 8; i++ ) {
                     char* c;
                     switch ( playermode[i] ) {
                        case 0: c = humanplayername[i];
                                break;
                        case 1: c = computerplayername[i];
                                break;
                        case 2: c = NULL;
                                break;
                     } /* endswitch */
                     names[i] = c;
                 }
                 int plx;
                 if ( oninit || supervisor || actmap->actplayer == -1)
                    plx = playerexist;
                 else
                    plx = 1 << actmap->actplayer;
                 enternamestrings.init( &names, plx ,0);
                 enternamestrings.run(); 
                 enternamestrings.done(); 
                 for ( i = 0; i < 8; i++ ) {
                     char* c = names[i];
                     switch ( playermode[i] ) {
                        case 0: humanplayername[i] = c;
                                break;
                        case 1: computerplayername[i] = c;
                                break;
                     } /* endswitch */
                 }
                 buildhlgraphics(); 
           }
      break; 
      
      case 4:  {  
                 int plx = 0;
                 for ( int j = 0; j < 8; j++ )
                    if ( actmap->player[j].existent )
                       plx = plx * 2 + 1;
                 /*
                 if ( oninit || supervisor || actmap->actplayer == -1) {
                    plx = playerexist;
                 } else
                    plx = 1 << actmap->actplayer;
                  */
                  tenternamestrings   enternamestrings;
                  enternamestrings.init( &computername, plx ,2);
                  enternamestrings.run(); 
                  enternamestrings.done(); 
                  buildhlgraphics(); 
               }
      break; 
      
      #ifndef karteneditor
      case 5: setparams();
              setupnetwork ( actmap->network );
              break;
              
      case 6: {
                 if ( actmap->supervisorpasswordcrc ) {
                    taskforsupervisorpassword afsp;
                    int a;
                    afsp.init ( &actmap->supervisorpasswordcrc, 1 );
                    afsp.run( &a );
                    afsp.done();
   
                    if ( a == 1 ) {
                      supervisor = 1;
                      disablebutton ( 6 );
                    }
                 }
              }
      break;
      #endif
   } 
} 


void         tsetalliances::run(void)
{ 
  byte         i, j; 

   mousevisible(true); 
   do { 
      tdialogbox::run();
      if (mouseparams.taste == 1) { 
         for (i = 0; i <= 7; i++) { 
            if (actmap->player[i].existent) { 
               if ((mouseparams.x > x1 + ply_x1) && (mouseparams.x <= x1 + ply_x1 + tsa_namelength) && (mouseparams.y > y1 + ply_y1 + i * ply_lineheight) && (mouseparams.y <= y1 + ply_y1 + (i + 1) * ply_lineheight)) {
                  click(0,0,i); 
                  while ( mouseparams.taste != 0 )
                     releasetimeslice();
               }

               if ((mouseparams.x > x1 + ply_x1 + tsa_namelength + 30) && (mouseparams.x <= x1 + 30 + ply_x1 + 2 * tsa_namelength) && (mouseparams.y > y1 + ply_y1 + i * ply_lineheight) && (mouseparams.y <= y1 + ply_y1 + (i + 1) * ply_lineheight)) {
                  click(0,1,i); 
                  while ( mouseparams.taste != 0 )
                     releasetimeslice();
               } 

               if ((mouseparams.x > x1 + 40 + ply_x1 + 2 * tsa_namelength) && (mouseparams.x <= x1 + 40 + ply_x1 + 3 * tsa_namelength) && (mouseparams.y > y1 + ply_y1 + i * ply_lineheight) && (mouseparams.y <= y1 + ply_y1 + (i + 1) * ply_lineheight)) {
                  click(0,2,i); 
                  while ( mouseparams.taste != 0 )
                     releasetimeslice();
               } 
            } 
         } 
         for (i = 0; i <= 7; i++) 
            for (j = 0; j <= 7; j++) 
               if ((mouseparams.x > x1 + ali_x1 - 5 + i * 30) && (mouseparams.x <= x1 + ali_x1 - 5 + (i + 1) * 30) && (mouseparams.y > y1 + ali_y1 + j * 22) && (mouseparams.y <= y1 + ali_y1 + (j + 1) * 22)) {
                  click(1,i,j); 
                  while ( mouseparams.taste != 0 )
                     releasetimeslice();
               }




      } 

      if ((taste == ct_down) || (taste == ct_up) || (taste == ct_left) || (taste == ct_right)) { 
         mousevisible(false); 
         paintkeybar(); 
         if (taste == ct_down) { 
            if (bx == -1) { 
               bx++;
            } 
            else 
               if (bx == 0) { 
                  if (yp < lastplayer) { 
                     do { 
                       yp++; 
                     }  while ( (playerexist & (1 << yp)) == 0);
                  } 
                  else { 
                     bx++; 
                     ya = 0; 
                  } 
               } 
               else 
                  if (bx == 1) { 
                     if (ya < playernum - 1) 
                        ya++; 
                     else { 
                        bx--; 
                        yp = 0; 
                     } 
                  } 
         } 
         if (taste == ct_up) { 
            if (bx == 0) { 
               do { 
                 if (yp > 0)
                   yp--;
                 else {
                   bx++;
                   yp = playernum - 1;
                 }
               }  while (   (bx != 1) && ((playerexist & (1 << yp)) == 0));

            } 
            else 
               if (bx == 1) { 
                  if (ya > 0) 
                     ya--; 
                  else { 
                     bx--; 
                     yp = lastplayer; 
                  } 
               } 
         } 
         if (taste == ct_left) { 
            if (bx == 0) { 
               if (xp > 0) 
                  xp--; 
               else 
                  xp = 2; 
            } 
            if (bx == 1) { 
               if (xa > 0) 
                  xa--; 
               else 
                  xa = playernum - 1; 
            } 
         } 
         if (taste == ct_right) { 
            if (bx == 0) { 
               if (xp < 2) 
                  xp++; 
               else 
                  xp = 0; 
            } 
            if (bx == 1) { 
               if (xa < playernum - 1) 
                  xa++; 
               else 
                  xa = 0; 
            } 
         } 
         paintkeybar(); 
         mousevisible(true); 
      } 
      if (taste == ct_space) { 
        if (bx == 0) { 
          click(bx,xp,yp); 
        } 
        if (bx == 1) { 
          click(bx,xa,ya); 
        } 
      } 

   }  while ( status < 10 );
   
   #ifdef logging
   logtofile ( "dialog / tsetalliances::run / setparams ");
   #endif
   setparams();
   for ( i = 0; i < 8; i++ ) {
      #ifdef logging
      logtofile ( "dialog / tsetalliances::run / humanplayername ");
      #endif
      delete[]  humanplayername[i] ;

     /*
      #ifdef logging
      logtofile ( "dialog / tsetalliances::run / alliancename ");
      #endif
      delete  alliancename[i] ;
    */

      #ifdef logging
      logtofile ( "dialog / tsetalliances::run / computerplayername ");
      #endif
      delete[]  computerplayername[i] ;

      #ifdef logging
      logtofile ( "dialog / tsetalliances::run / computername ");
      #endif
      delete[]  computername[i] ;
   }

  #ifdef logging
  logtofile ( "dialog / tsetalliances::run / returning ");
  #endif
} 


void         setupalliances( int supervisor )
{ 
   /*
   #ifndef karteneditor
   if ( actmap->actplayer == -1  && !supervisor ) {
        taskforsupervisorpassword afsp;
        afsp.init ( &actmap->supervisorpasswordcrc, 0 );
        afsp.run( NULL );
        afsp.done();
   }
   #endif
   */

   tsetalliances sua; 

   sua.init( supervisor ); 
   sua.run(); 
   sua.done(); 
}  




/*




void         tviewanytext::setup( void )
{
   addbutton ( "e~x~it", 10, ysize - 30, xsize - 10, ysize - 10, 0, 1, 5, 1 );
   addkey ( 5, ct_esc );
}

void         tviewanytext::init( char* titlet, char* texttoview )
{ 

   // dialogbox initialisieren

   tdialogbox::init();
   x1 = 60; 
   y1 = 90; 
   xsize = 520; 
   ysize = 300; 
   textstart = 42; 
   textsizey = (ysize - textstart - 40); 
   starty = starty + 10; 
   title = titlet;
   windowstyle ^= dlg_in3d;
   action=0;



   txt = texttoview;

   setup();                    

   setparams ( x1 + 13, y1 + textstart, x1 + xsize - 41, y1 + ysize - 40, txt, black, dblue);
   tvt_dispactive = 0;
   displaytext(  );
   textsizeycomplete = tvt_yp;
   tvt_dispactive = 1;


   if (textsizeycomplete >= textsizey) {
      scrollbarvisible = true; 
      #ifdef NEWKEYB
      addscrollbar(xsize - 30,starty,xsize - 15,ysize - 40,&textsizeycomplete, textsizey, &tvt_starty,1,0);
      #else
      addscrollbar(xsize - 30,starty,xsize - 15,ysize - 40,&textsizeycomplete, textsizey, &tvt_starty,1,1);
      #endif
      setscrollspeed ( 1 , 1 );

   }                                                                                       
   else 
      scrollbarvisible = false; 

   tvt_starty = 0; 
   
} 

void         tviewanytext::repaintscrollbar( void )
{
   enablebutton( 1 );
}


void         tviewanytext::buildgraphics(void)
{ 
   tdialogbox::buildgraphics();
   rahmen(true,x1 + 10,y1 + textstart - 2,x1 + xsize - 40,y1 + textstart + textsizey + 2); 

   mousevisible(true);

   activefontsettings.font = schriften.smallarial; 
   activefontsettings.color = black; 
   activefontsettings.justify = lefttext; 
   activefontsettings.background = dblue; 
   activefontsettings.height = textsizey / linesshown; 

} 


void         tviewanytext::buttonpressed( char id )
{ 
   tdialogbox::buttonpressed(id);
   if (id == 1) 
      displaytext();

   if ( id == 5 )
      action = 11;
} 


void         tviewanytext::run(void)
{ 
   displaytext(); 
   do { 
      tdialogbox::run();
      checkscrolling();

   }  while (action < 10);
} 


*/



void         tviewanytext:: init( char* _title, const char* text , int xx1 , int yy1  , int xxsize , int yysize  )
{ 

   // dialogbox initialisieren

   tdialogbox::init();
   x1 = xx1; 
   y1 = yy1; 
   xsize = xxsize; 
   ysize = yysize; 
   textstart = 42 ; 
   textsizey = (ysize - textstart - 40); 
   starty = starty + 10; 
   title = _title;
   windowstyle ^= dlg_in3d;
   action=0;


   txt = text;
                       
   setparams ( x1 + 13, y1 + textstart, x1 + xsize - 41, y1 + ysize - 40, txt, black, dblue);
   addbutton ( "~O~K", 10, ysize - 30, xsize - 10, ysize - 10, 0, 1, 10, true );
   addkey ( 10, ct_space );
   addkey ( 10, ct_enter );
   addkey ( 10, ct_esc );
   tvt_dispactive = 0;
   displaytext(  );
   textsizeycomplete = tvt_yp;
   tvt_dispactive = 1;


   if (textsizeycomplete >= textsizey) {
      scrollbarvisible = true; 

      #ifdef NEWKEYB
      addscrollbar(xsize - 30,starty,xsize - 15,ysize - 35,&textsizeycomplete, textsizey, &tvt_starty,1,0);
      #else
      addscrollbar(xsize - 30,starty,xsize - 15,ysize - 35,&textsizeycomplete, textsizey, &tvt_starty,1,1);
      #endif
      setscrollspeed ( 1 , 1 );

      rightspace = 40;
   }                                                                                       
   else {
      rightspace = 10;
      scrollbarvisible = false; 
   }

   tvt_starty = 0; 
   buildgraphics();
} 


void         tviewanytext::repaintscrollbar ( void )
{
   enablebutton( 1 );
}


void         tviewanytext::redraw(void)
{ 
  collategraphicoperations cgo ( x1, y1, x1 + xsize, y1 + ysize);

   tdialogbox::redraw();
   rahmen(true,x1 + 10,y1 + textstart - 2,x1 + xsize - rightspace,y1 + textstart + textsizey + 2); 


   activefontsettings.font = schriften.smallarial; 
   activefontsettings.justify = lefttext; 
   activefontsettings.background = dblue; 
   activefontsettings.length =0;

   activefontsettings.color = black; 
   activefontsettings.height = textsizey / linesshown; 

   int xd = 15 - (rightspace - 10) / 2;

   setpos ( x1 + 13 + xd, y1 + textstart, x1 + xsize - 41 + xd, y1 + ysize - 40 );
   displaytext(); 

} 


void         tviewanytext::buttonpressed( char id )
{     
   tdialogbox::buttonpressed(id);
   if (id == 1) 
      displaytext();
   if ( id== 10 )
      action = 11;
} 


void         tviewanytext::run(void)
{ 
   mousevisible(true);
   do { 
      tdialogbox::run();
      checkscrolling ( );
   }  while (action < 10);
} 





class tbi3preferences : public tdialogbox {
                        tgameoptions actoptions;
                        int status;
                        tmouserect delta, r1, r2, r3, r4, r5, r6, r7, r8;
                        tmouserect dlgoffset;

                     public:
                        void init ( void );
                        void buttonpressed ( char id );
                        void run ( void );
                    };


void tbi3preferences :: init ( void )
{
   tdialogbox::init();
   title = "BattleIsle options";

   xsize = 600;
   ysize = 460;

   x1 = -1;
   y1 = -1;

   actoptions = gameoptions;

   addbutton ( "~O~K", 10, ysize - 35, xsize / 2 - 5, ysize - 10, 0, 1, 1, true );
   addbutton ( "~C~ancel", xsize / 2 + 5, ysize - 35, xsize - 10, ysize - 10, 0, 1, 2, true );

   r1.x1 = xsize - 150;
   r1.y1 = starty + 20;
   r1.x2 = xsize - 20;
   r1.y2 = starty + 35;

   delta.x1 = 0;
   delta.x2 = 0;
   delta.y1 = 30;
   delta.y2 = 30;

   addbutton ( "", r1, 3, 0, 3, true );
   addeingabe ( 3, &actoptions.bi3.interpolate.terrain, 0, dblue );

   r2 = r1 + delta;

   addbutton ( "", r2,  3, 0, 4, true  );
   addeingabe ( 4, &actoptions.bi3.interpolate.units, 0, dblue );

   r3 = r2 + delta;

   addbutton ( "", r3,  3, 0, 5, true  );
   addeingabe ( 5, &actoptions.bi3.interpolate.buildings, 0, dblue );
   
   r4 = r3 + delta;

   addbutton ( "", r4,  3, 0, 6, true  );
   addeingabe ( 6, &actoptions.bi3.interpolate.objects, 0, dblue );

   buildgraphics(); 

   activefontsettings.font = schriften.smallarial; 
   activefontsettings.justify = lefttext; 
   activefontsettings.length = 0;
   activefontsettings.background = 255;

   dlgoffset.x1 = x1;
   dlgoffset.y1 = y1;
   dlgoffset.x2 = x1;
   dlgoffset.y2 = y1;

   showtext2 ( "interpolate terrain",   x1 + 25, y1 + r1.y1 );
   showtext2 ( "interpolate units",     x1 + 25, y1 + r2.y1 );
   showtext2 ( "interpolate buildings", x1 + 25, y1 + r3.y1 );
   showtext2 ( "interpolate objects",   x1 + 25, y1 + r4.y1 );

   status = 0;


}

void tbi3preferences :: buttonpressed ( char id )
{
   tdialogbox :: buttonpressed ( id );

   if ( id == 1 ) {
      gameoptions = actoptions;
      gameoptions.changed = 1;
      status = 10;
   }

   if ( id == 2 ) 
      status = 10;

}


void tbi3preferences :: run ( void )
{
   mousevisible ( true );
   do {
      tdialogbox::run();
   } while ( status < 10 ); /* enddo */
}

void bi3preferences  ( void )
{
   tbi3preferences prefs;
   prefs.init();
   prefs.run();
   prefs.done();
}




class ResizeMap : public tdialogbox {
                        int status;
                        int top, bottom, left, right;

                     public:
                        void init ( void );
                        void buttonpressed ( char id );
                        void run ( void );
                        char checkvalue( byte         id, void*      p);
                    };


char ResizeMap :: checkvalue(byte         id, void*      p)  
{
   if ( id >= 3 && id <= 6 ) {
      int* wp = (int*) p;

      if ( id == 3 && id == 6 ) 
         if ( *wp & 1 ) {
            displaymessage ( "values must be even !", 1 );
            return 0;
         }
      if ( (id == 3   &&  - ( *wp + bottom ) > actmap->ysize - idisplaymap.getscreenysize(1) ) ||
           (id == 6   &&  - ( top + *wp )    > actmap->ysize - idisplaymap.getscreenysize(1) ) ||
           (id == 4   &&  - ( *wp + right  ) > actmap->xsize - idisplaymap.getscreenxsize(1) ) ||
           (id == 5   &&  - ( left + *wp   ) > actmap->xsize - idisplaymap.getscreenxsize(1) ) ) {
         displaymessage ( "remaining size too small !", 1 );
         return 0;
      }

      if ( (id == 3   &&   ( *wp + bottom ) + actmap->ysize > 32000 ) ||
           (id == 6   &&   ( top + *wp )    + actmap->ysize > 32000 ) ||
           (id == 4   &&   ( *wp + right  ) + actmap->xsize > 32000 ) ||
           (id == 5   &&   ( left + *wp   ) + actmap->xsize > 32000 ) ) {
         displaymessage ( "new map too large !", 1 );
         return 0;
      }

   }
   return 1;
}

void ResizeMap :: init ( void )
{

   top = 0;
   bottom = 0;
   left = 0;
   right = 0;

   tdialogbox::init();
   title = "Resize Map";

   xsize = 400;
   ysize = 300;

   x1 = -1;
   y1 = -1;

   addbutton ( "~O~K", 10, ysize - 35, xsize / 2 - 5, ysize - 10, 0, 1, 1, true );
   addkey ( 1, ct_enter );

   addbutton ( "~C~ancel", xsize / 2 + 5, ysize - 35, xsize - 10, ysize - 10, 0, 1, 2, true );
   addkey ( 2, ct_esc );

   addbutton ( "~T~op", xsize/2 - 30, 70, xsize/2 + 30, 90, 2, 0, 3, true );
   addeingabe ( 3, &top, -actmap->ysize, 70000 );
   addkey ( 3, ct_t );

   addbutton ( "~L~eft", 20, 120, 80, 140, 2, 0, 4, true );
   addeingabe ( 4, &left, -actmap->xsize, 70000 );
   addkey ( 4, ct_l );

   addbutton ( "~R~ight", xsize - 80, 120, xsize - 20, 140, 2, 0, 5, true );
   addeingabe ( 5, &right, -actmap->xsize, 70000 );
   addkey ( 5, ct_r );

   addbutton ( "~B~ottom", xsize/2 - 30, 150, xsize/2 + 30, 170, 2, 0, 6, true );
   addeingabe ( 6, &bottom, -actmap->ysize, 70000 );
   addkey ( 6, ct_b );

   buildgraphics(); 

   activefontsettings.font = schriften.smallarial; 
   activefontsettings.justify = lefttext; 
   activefontsettings.length = 0;
   activefontsettings.background = 255;

   showtext2 ( "positive values: enlarge map",   x1 + 25, y1 + 220 );
   showtext2 ( "negative values: shrink map",   x1 + 25, y1 + 240 );

   status = 0;
}

void ResizeMap :: buttonpressed ( char id )
{
   tdialogbox :: buttonpressed ( id );

   if ( id == 1 ) 
      status = 12;

   if ( id == 2 ) 
      status = 10;

}


void ResizeMap :: run ( void )
{
   mousevisible ( true );
   do {
      tdialogbox::run();
   } while ( status < 10 ); /* enddo */
   if ( status >= 12 ) {
      int result = resizemap ( top, bottom, left, right );
      if ( result )
        displaymessage ( "resizing failed" , 1 );
   }
}

void resizemap ( void )
{
   ResizeMap rm;
   rm.init();
   rm.run();
   rm.done();
   displaymap();
}











void tprogressbar :: start ( int _color, int _x1, int _y1, int _x2, int _y2, pnstream stream )
{
  ended = 0;
   x1 = _x1;
   y1 = _y1;
   x2 = _x2;
   y2 = _y2;
   color = _color;

   if ( stream ) {
      int ver;
      stream->readdata2 ( ver );
      stream->readdata2 ( groupnum );
      for ( int i = 0; i < groupnum; i++ ) {
         stream->readdata2 ( group[i].orgnum );
         stream->readdata2 ( group[i].time );
         stream->readdata2 ( group[i].newtime );
         stream->readdata2 ( group[i].timefromstart );

         /*
         for ( int j = 0; j < group[i].num; j++ )
           stream->readdata2 ( group[i].point[j] );
         */

      }
      stream->readdata2 ( time );
      first = 0;
   } else
      first  = 1;

  actgroupnum = -1;

  starttick = ticker;
  lastpaintedpos = -1;

  startgroup();
}

void tprogressbar :: startgroup ( void )
{
   int t = ticker;
   if ( actgroupnum >= 0 ) {
      group[ actgroupnum ].time = t - group[ actgroupnum ].newtime;
      group[ actgroupnum ].orgnum = group[ actgroupnum ].num;
   }
      
   actgroupnum++;
   group[ actgroupnum ].num = 0;
   group[ actgroupnum ].newtime = t;
   group[ actgroupnum ].timefromstart = t - starttick;
   point();
}


void tprogressbar :: point ( void )
{
   int t = ticker - group[ actgroupnum ].newtime;
   if ( !first &&  group[ actgroupnum ].num < group[ actgroupnum ].orgnum ) {
      if ( group[ actgroupnum ].time ) { 
         float fgtime = (float) group[ actgroupnum ].time;

         float gpos = (float) t  /  fgtime;
         if ( gpos > 1 )
            gpos = 1;

         float gstart = group[ actgroupnum ].timefromstart;

         float pos = (float)(gstart + fgtime * gpos) / time;
         if ( pos > 1 )
            pos = 1;
         lineto ( pos );
         
      }

   }
   // group[ actgroupnum ].point [ group[ actgroupnum ].num++ ] = t;
   group[ actgroupnum ].num++ ;
}

void tprogressbar :: lineto ( float pos )
{
   if ( pos > 1 )
      pos = 1;

   int newpos = (int)(x1 + ( x2 - x1 ) * pos);
   if ( newpos > lastpaintedpos ) {

      collategraphicoperations cgo ( lastpaintedpos+1, y1, newpos, y2 );
      for ( int x = lastpaintedpos+1; x <= newpos; x++ )
         for ( int y = y1; y <= y2; y++ )
            putpixel ( x, y, color );

      lastpaintedpos = newpos;
   }
}


void tprogressbar :: end ( void )
{
   time = ticker - starttick;
   ended = 1;
   groupnum = actgroupnum;
}

void tprogressbar :: writetostream ( pnstream stream )
{
   if ( !ended )
      end();

   int ver = 1;
   stream->writedata2 ( ver );
   stream->writedata2 ( groupnum );
   for ( int i = 0; i < groupnum; i++ ) {
      stream->writedata2 ( group[i].orgnum );
      stream->writedata2 ( group[i].time );
      stream->writedata2 ( group[i].newtime );
      stream->writedata2 ( group[i].timefromstart );

      /*
      for ( int j = 0; j < group[i].num; j++ )
        stream->writedata2 ( group[i].point[j] );
      */
   }
   stream->writedata2 ( time );
}

             
#ifdef FREEMAPZOOM
class tchoosezoomlevel : public tdialogbox {
             protected:
                int zoom; 
                int ok;
             public: 
                void init ( void );
                void buttonpressed ( char id );
                void run ( void );
         };

void tchoosezoomlevel :: init ( void )
{
   tdialogbox :: init ( );
   title = "choose zoomlevel";
   xsize = 200;
   ysize = 150;
   zoom = zoomlevel.getzoomlevel();
   ok = 0;

   addbutton ( "~O~k", 10, ysize - 30, xsize - 10, ysize - 10 , 0, 1, 1, true );
   addkey ( 1, ct_enter );

   addbutton ( "", 10, 60, xsize - 10, 85                     , 2, 1, 2, true );
   addeingabe ( 2, &zoom, zoomlevel.getminzoom(), zoomlevel.getmaxzoom() );
   addkey ( 1, ct_space );

   buildgraphics();
}

void tchoosezoomlevel :: buttonpressed ( char id )
{
   tdialogbox:: buttonpressed ( id );
   if ( id == 1 )
      ok = 1;
}

void tchoosezoomlevel :: run ( void )
{
   editfield ( 2 );
   ok = 1;
   mousevisible ( true );
   do {
      tdialogbox::run();
   } while ( !ok ); 
   zoomlevel.setzoomlevel ( zoom );

};

void choosezoomlevel ( void )
{
   tchoosezoomlevel ctl;
   ctl.init();
   ctl.run();
   ctl.done();
   displaymap();
}
#endif






    struct tvweapon { 
                  byte         typ;   /*  position in cwaffentypen  */ 
                  byte         sourcepos;   /*  position im munitionsarray des containers; cenergy .. gltig */ 
                  byte         destpos;   /*  position im munitionsarray des gel. fahrzeugs  */ 
                  int          sourceamount;
                  int          maxsourceamount;
                  int          destamount;
                  int          maxdestamount;
                  int          newdestamount;
               }; 


    struct tvweapons { 
                   byte         count; 
                   byte         mode; 
                   tvweapon     weap[10]; 
                } ;                 
                
#define entervalue ct_space  

    class  tmunitionsbox : public tdialogbox {
                  public:
                       char          backgrnd;
                       int           maxproduction;
                       char*         strng2;
                       char*         strr ( int a );
                       void          init ( void );
                       void          done ( void );
                    };


    class  tverlademunition : public tmunitionsbox {
                          int abstand;
                          int firstliney;
                          int startx;
                          int llength;
                          int numlength;
                 public:
                          tvweapons     wp;
                          int           oldpos[10];
                          int           j;
                          integer       mx, my;
                          char          mp, op;
                          char       frst;
                          
                          // int*          ammunition;
                          int           sprit,energy,material;
                          pvehicle      vehicle;
                          pvehicle      vehicle2;
                          pbuilding     building;
                          char          fast;
                          char       abbruch;
                          
                          void          setvariables( pvehicle svehicle, pvehicle svehicle2, pbuilding sbuilding, char sfast);
                          
                          void          init ( void );
                          virtual void  run ( void );
                          virtual void  buttonpressed( char id ); 
                          void          zeichneschieberegler( char b);
                          void          checkpossible( char b );
                          void          setloading ( void );
                       };

#define wpnum 8


void tmunitionsbox::init ( void ) {
   tdialogbox::init();
   strng2 = new char[100];
}

void tmunitionsbox::done ( void ) {
   delete[] strng2 ;
   tdialogbox::done();
}


char* tmunitionsbox::strr ( int a ) {
   itoa( a, strng2, 10 );
   return strng2;
}

   #define firstweapon 0

    #define cenergy 100  
    #define cmaterial 101  
    #define csprit 102  
    #define maxloading 0xffff  



void         tverlademunition::init(void)
{ 

   tmunitionsbox::init();
   x1 = 40;
   xsize = 560;
   y1 = 55;

   title = "transfer ammunition and resources";
   backgrnd = dblue;
   windowstyle = windowstyle ^ dlg_in3d;

   firstliney = 120;
   abstand = 25;
   startx = 230;
   llength = xsize - startx - 90;
   numlength = 40;
} 


void         tverlademunition::buttonpressed( char id)
{ 
   if (id == 1) { 
      setloading(); 
      abbruch = true; 
   } 
} 


#define yoffset 105
void         tverlademunition::run(void)
{ 
   word         i; 
   integer      k, l; 

   if (vehicle == NULL) return;

   wp.count = 0; 
   frst = true; 
   op = 20; 
   mp = 0; 
   memset( (void*) &wp, 0, sizeof(wp));

   if (vehicle2 == NULL) { 
    /*
      if (vehicle->typ->tank > 0) { 
            wp.weap[wp.count].typ = csprit; 
            wp.weap[wp.count].sourcepos = csprit; 
            wp.weap[wp.count].destpos = csprit; 
            wp.weap[wp.count].sourceamount = sprit; 
            wp.weap[wp.count].destamount = vehicle->fuel; 
            wp.weap[wp.count].maxsourceamount = maxloading; 
            wp.weap[wp.count].maxdestamount = vehicle->typ->tank; 
            wp.weap[wp.count].newdestamount = wp.weap[wp.count].destamount; 
         
         wp.count++;
      } 
      if (vehicle->typ->energy > 0) { 
            wp.weap[wp.count].typ = cenergy; 
            wp.weap[wp.count].sourcepos = cenergy; 
            wp.weap[wp.count].destpos = cenergy; 
            wp.weap[wp.count].sourceamount = energy; 
            wp.weap[wp.count].destamount = vehicle->energy; 
            wp.weap[wp.count].maxsourceamount = maxloading; 
            wp.weap[wp.count].maxdestamount = vehicle->typ->energy;
            wp.weap[wp.count].newdestamount = wp.weap[wp.count].destamount; 
         
         wp.count++; 
      } 
      if (vehicle->typ->material > 0) { 
            wp.weap[wp.count].typ = cmaterial; 
            wp.weap[wp.count].sourcepos = cmaterial; 
            wp.weap[wp.count].destpos = cmaterial; 
            wp.weap[wp.count].sourceamount = material; 
            wp.weap[wp.count].destamount = vehicle->material; 
            wp.weap[wp.count].maxsourceamount = maxloading; 
            wp.weap[wp.count].maxdestamount = vehicle->typ->material; 
            wp.weap[wp.count].newdestamount = wp.weap[wp.count].destamount; 
         
         wp.count++; 
      } 

      if (vehicle->typ->weapons->count > 0) 
         for (i = 0; i <= vehicle->typ->weapons->count - 1; i++) { 
            if (vehicle->typ->weapons->weapon[i].typ & (cwweapon | cwmineb)) {   

                  wp.weap[wp.count].typ = log2(vehicle->typ->weapons->weapon[i].typ & (cwweapon | cwmineb)); 
                  wp.weap[wp.count].sourcepos = wp.weap[wp.count].typ; 
                  wp.weap[wp.count].sourceamount = ammunition[wp.weap[wp.count].typ]; 

                  wp.weap[wp.count].destpos = i; 
                  wp.weap[wp.count].destamount = vehicle->ammo[i]; 
                  wp.weap[wp.count].maxsourceamount = maxloading; 
                  wp.weap[wp.count].maxdestamount = vehicle->typ->weapons->weapon[i].count; 
                  wp.weap[wp.count].newdestamount = wp.weap[wp.count].destamount; 
               
               wp.count++; 
            } 
         } 
     */
   } 
   else { 
      /*
      if ((fast & 2 ) == 0)
         mx = 0; 
      else 
      */
      mx = vehicle2->typ->weapons->count; 
      for (i = 0; i < mx; i++) { 
         if ( vehicle2->typ->weapons->weapon[i].service()  /* || ((fast & 2) == 0)  */ ) {
            if ((vehicle->typ->tank > 0) && (vehicle2->typ->tank > 0) && ( vehicle2->functions & cffuelref)) { 
                  wp.weap[wp.count].typ = csprit; 
                  wp.weap[wp.count].sourcepos = csprit; 
                  wp.weap[wp.count].destpos = csprit; 
                  wp.weap[wp.count].sourceamount = vehicle2->fuel; 
                  wp.weap[wp.count].maxsourceamount = vehicle2->typ->tank; 
                  wp.weap[wp.count].destamount = vehicle->fuel; 
                  wp.weap[wp.count].maxdestamount = vehicle->typ->tank; 
                  wp.weap[wp.count].newdestamount = wp.weap[wp.count].destamount; 
               
               wp.count++;
            } 
            /*
            if ((vehicle->typ->energy > 0) && (vehicle2->typ->energy > 0)) { 
                  wp.weap[wp.count].typ = cenergy; 
                  wp.weap[wp.count].sourcepos = cenergy; 
                  wp.weap[wp.count].destpos = cenergy; 
                  wp.weap[wp.count].sourceamount = vehicle2->energy; 
                  wp.weap[wp.count].maxsourceamount = vehicle2->typ->energy; 
                  wp.weap[wp.count].destamount = vehicle->energy; 
                  wp.weap[wp.count].maxdestamount = vehicle->typ->energy; 
                  wp.weap[wp.count].newdestamount = wp.weap[wp.count].destamount; 
               
               wp.count++;
            } 
            */
            if ((vehicle2->typ->material > 0) && (vehicle->typ->material > 0) && ( vehicle2->functions & cfmaterialref )) { 
                  wp.weap[wp.count].typ = cmaterial; 
                  wp.weap[wp.count].sourcepos = cmaterial; 
                  wp.weap[wp.count].destpos = cmaterial; 
                  wp.weap[wp.count].sourceamount = vehicle2->material; 
                  wp.weap[wp.count].maxsourceamount = vehicle2->typ->material; 
                  wp.weap[wp.count].destamount = vehicle->material; 
                  wp.weap[wp.count].maxdestamount = vehicle->typ->material; 
                  wp.weap[wp.count].newdestamount = wp.weap[wp.count].destamount; 
               
               wp.count++;
            } 
         } 
      } 
      if (vehicle->typ->weapons->count > 0) 
         for (i = 0; i < vehicle->typ->weapons->count ; i++) { 
            l = 255; 
            for (k = 0; k < vehicle2->typ->weapons->count ; k++) 
               if (vehicle2->typ->weapons->weapon[k].getScalarWeaponType() == vehicle->typ->weapons->weapon[i].getScalarWeaponType()  &&  vehicle->typ->weapons->weapon[i].getScalarWeaponType() >= 0) 
                  if (vehicle2->typ->weapons->weapon[k].canRefuel() ) 
                     l = k; 
            if (l != 255) {                                                                                          
                  wp.weap[wp.count].typ = vehicle->typ->weapons->weapon[i].getScalarWeaponType() ; 
                  wp.weap[wp.count].sourcepos = l; 
                  wp.weap[wp.count].destpos = i; 
                  wp.weap[wp.count].sourceamount = vehicle2->ammo[l]; 
                  wp.weap[wp.count].maxsourceamount = vehicle2->typ->weapons->weapon[l].count; 

                  wp.weap[wp.count].destamount = vehicle->ammo[i]; 
                  wp.weap[wp.count].maxdestamount = vehicle->typ->weapons->weapon[i].count; 
                  wp.weap[wp.count].newdestamount = wp.weap[wp.count].destamount; 
               
               wp.count++;
            } 
         } 
   } 

   y1 = 75 + (8 - wp.count) * 25 / 2 - 20;
   ysize = 480 - 2 * y1; 

   if ((fast & 1) == 0) {
      {
         for (i = 0; i <= wp.count - 1; i++) {
            oldpos[i] = wp.weap[i].newdestamount;
         }

         addbutton("~l~oad",10,ysize - 40,xsize - 10,ysize - 10,0,1,1,true);
         addkey(1,ct_enter);
         buildgraphics();

         rahmen(true,x1 + 10,y1 + 50,x1 + xsize - 10,y1 + ysize - 50);

         activefontsettings.justify = lefttext;
         activefontsettings.font = schriften.smallarial;
         activefontsettings.length = 100;

         int ix, iy;
         getpicsize ( vehicle2->typ->picture[0], ix, iy );

         if ( vehicle2 )
            putrotspriteimage(x1 + 30 , y1 + 55, vehicle2->typ->picture[0], actmap->actplayer * 8);
         else
            showtext2( building->typ->name , x1 + 50, y1 + 55);

         putrotspriteimage(x1 + xsize - 30 - ix , y1 + 55, vehicle->typ->picture[0], actmap->actplayer * 8);

         for (i = 0; i < wp.count ; i++) {
            activefontsettings.length = (x1 + startx - numlength - 10 ) - (x1 + 20 );
            if ((wp.weap[i].typ >= 100) && (wp.weap[i].typ <= 110))
               showtext2( resourceNames[wp.weap[i].typ - 100],x1 + 20, y1 + firstliney + i * abstand);
            else
               showtext2( cwaffentypen[wp.weap[i].typ],x1 + 20, y1 + firstliney + i * abstand);

            rahmen(true, x1 + startx - 11, y1 + firstliney - 2 + i * abstand, x1 + startx + llength + 11,y1 + firstliney + 18 + i * abstand);
            zeichneschieberegler(i);
         }
      }

      mousevisible(true);
      if (wp.count > 0) {
         abbruch = false; 
         do { 
            tmunitionsbox::run();
            switch (taste) {
               
               case ct_up:   { 
                         op = mp; 
                         if (mp > 0) 
                            mp--;
                         else 
                            mp = wp.count - 1;
                      } 
               break; 
               
               case ct_down:   { 
                           op = mp; 
                           if (mp >= wp.count - 1) 
                              mp = 0;
                           else 
                              mp++;
                        } 
               break; 
               
               case ct_right:   { 

                               oldpos[mp] = wp.weap[mp].newdestamount; 

                               if (( wp.weap[mp].maxdestamount >= 1) && (wp.weap[mp].maxdestamount <= 20))
                                  i = 1;
                               if (( wp.weap[mp].maxdestamount >= 21) && (wp.weap[mp].maxdestamount <= 50))
                                  i = 2;
                               if (( wp.weap[mp].maxdestamount >= 51) && (wp.weap[mp].maxdestamount <= 100))
                                  i = 5;
                               if (( wp.weap[mp].maxdestamount >= 101) && (wp.weap[mp].maxdestamount <= 200))
                                  i = 10;
                               if (( wp.weap[mp].maxdestamount >= 201) && (wp.weap[mp].maxdestamount <= 500))
                                  i = 20;
                               if (( wp.weap[mp].maxdestamount >= 501) && (wp.weap[mp].maxdestamount <= 2000))
                                  i = 100;
                               if (( wp.weap[mp].maxdestamount >= 2001) && (wp.weap[mp].maxdestamount <= 10000))
                                  i = 200;
                               if ( wp.weap[mp].maxdestamount >= 10001)
                                  i = 500;
                                  
                               wp.weap[mp].newdestamount = wp.weap[mp].newdestamount + i; 
                               checkpossible(mp); 
                               if (oldpos[mp] != wp.weap[mp].newdestamount) 
                                  zeichneschieberegler(mp); 
                            
                         } 
               break; 
               
               case ct_left:   { 
                               oldpos[mp] = wp.weap[mp].newdestamount; 

                               if (( wp.weap[mp].maxdestamount >= 1) && (wp.weap[mp].maxdestamount <= 20))
                                  i = 1;
                               if (( wp.weap[mp].maxdestamount >= 21) && (wp.weap[mp].maxdestamount <= 50))
                                  i = 2;
                               if (( wp.weap[mp].maxdestamount >= 51) && (wp.weap[mp].maxdestamount <= 100))
                                  i = 5;
                               if (( wp.weap[mp].maxdestamount >= 101) && (wp.weap[mp].maxdestamount <= 200))
                                  i = 10;
                               if (( wp.weap[mp].maxdestamount >= 201) && (wp.weap[mp].maxdestamount <= 500))
                                  i = 20;
                               if (( wp.weap[mp].maxdestamount >= 501) && (wp.weap[mp].maxdestamount <= 2000))
                                  i = 100;
                               if (( wp.weap[mp].maxdestamount >= 2001) && (wp.weap[mp].maxdestamount <= 10000))
                                  i = 200;
                               if ( wp.weap[mp].maxdestamount >= 10001)
                                  i = 500;

                               if (wp.weap[mp].newdestamount > i)
                                  wp.weap[mp].newdestamount-=i;
                               else
                                  wp.weap[mp].newdestamount = 0;

                               checkpossible(mp); 
                               if (oldpos[mp] != wp.weap[mp].newdestamount) 
                                  zeichneschieberegler(mp); 
                           
                        } 
               break;

            } 

            if (taste == ct_esc) {
                abbruch = true;
            }

            if (taste == entervalue) {

               mousevisible(false); 
               oldpos[mp] = wp.weap[mp].newdestamount; 
               bar(x1 + xsize - 60, y1 + firstliney + mp * abstand,x1 + xsize - 17,y1 + firstliney + 15 + mp * abstand, backgrnd);
               intedit( &wp.weap[mp].newdestamount , x1 + xsize - 60, y1 + firstliney + mp * abstand, 43, 0, 65535);
               checkpossible(mp); 
               zeichneschieberegler(mp); 
               mousevisible(true); 
            } 
            if ((op != 20) && (mp != op)) { 

               mousevisible(false);
               if (!frst) 
                  xorrectangle(x1 + 15,y1 + firstliney - 5 + op * abstand,x1 + xsize - 15,y1 + firstliney - 5 + (op + 1) * abstand,14);
               frst = false; 
               op = mp; 
               xorrectangle(x1 + 15,y1 + firstliney - 5 + op * abstand,x1 + xsize - 15,y1 + firstliney - 5 + (op + 1) * abstand,14);
               mousevisible(true); 
            } 
            mx = mouseparams.x; 
            my = mouseparams.y; 
            for (i = 0; i < wp.count ; i++) {
               if ((mouseparams.taste == 1) && mouseinrect ( x1 + startx - 10, y1 + firstliney + i * abstand, x1 + startx + llength + 10, y1 + firstliney + i * abstand + 20 )) {
                  oldpos[i] = wp.weap[i].newdestamount; 
                  j = wp.weap[i].maxdestamount * (mx - (startx + x1)) / llength;
                  if (j < 0) j = 0; 
                  if (j > wp.weap[i].maxdestamount) j = wp.weap[i].maxdestamount; 
                  wp.weap[i].newdestamount = j; 

                  checkpossible(i); 

                  if (oldpos[i] != wp.weap[i].newdestamount) { 
                     mousevisible(false); 
                     zeichneschieberegler(i); 
                     mousevisible(true); 
                     oldpos[i] = wp.weap[i].newdestamount; 
                  } 
               } 
            } 

         }  while ( abbruch==0 );
      } 
      else 
         wait(); 
      mousevisible(false); 
   } 
   else { 
      for (i = 0; i < wp.count ; i++) { 
         wp.weap[i].newdestamount = wp.weap[i].maxdestamount; 
         checkpossible(i); 
      } 
      setloading(); 
   } 
} 



void         tverlademunition::setvariables(  pvehicle svehicle, pvehicle svehicle2, pbuilding sbuilding, char sfast )
{ 
   vehicle = svehicle; 
   vehicle2 = svehicle2; 
   building = sbuilding; 
   if ( building ) { 
      displaymessage("obsolete function called : tverlademunition::setvariables / building ",2);
      sprit = building->actstorage.a.fuel; 
      energy = building->actstorage.a.energy; 
      material = building->actstorage.a.material; 
   } 
   else { 
      sprit = vehicle2->fuel; 
      energy = vehicle2->energy; 
      material = vehicle2->material; 
   } 

   fast = sfast; 
} 




void         tverlademunition::zeichneschieberegler(byte         b)
{ 
//  collategraphicoperations cgo;

   int      l;
   byte         r;

   r = getmousestatus(); 
   if (r == 2) mousevisible(false); 
   bar(x1 + xsize - 60, y1 + firstliney + b * abstand, x1 + xsize - 17,y1 + firstliney + 15 + b * abstand,backgrnd);

   activefontsettings.justify = lefttext; 
   activefontsettings.background = dblue;
   activefontsettings.length = numlength;

   showtext2(strrr(wp.weap[b].newdestamount),x1 + startx + llength + 15, y1 + firstliney + b * abstand);

   l = x1 + startx + llength * oldpos[b] / wp.weap[b].maxdestamount;

   rectangle(l - 10, y1 + firstliney - 1 + b * abstand,l + 10,y1 + firstliney - 1 + b * abstand + 18,backgrnd);

   l = x1 + startx + llength * wp.weap[b].newdestamount / wp.weap[b].maxdestamount;
   rahmen(false, l - 10,y1 + firstliney - 1 + b * abstand,l + 10,y1 + firstliney - 1 + b * abstand + 18);

//   bar(x1 + 116,y1 + 100 - 35 + b * abstand,startx - 12,y1 + 115 - 35 + b * abstand,backgrnd);

   showtext2(strrr(wp.weap[b].sourceamount - (wp.weap[b].newdestamount - wp.weap[b].destamount)), x1 + startx - numlength -15 ,y1 + firstliney + b * abstand);

   if (r == 2) mousevisible(true); 
} 


void         tverlademunition::checkpossible(byte         b)
{ 
      if (wp.weap[b].newdestamount - wp.weap[b].destamount > wp.weap[b].sourceamount) 
         wp.weap[b].newdestamount = wp.weap[b].destamount + wp.weap[b].sourceamount; 
      if (wp.weap[b].newdestamount > wp.weap[b].maxdestamount) 
         wp.weap[b].newdestamount = wp.weap[b].maxdestamount; 
      if (wp.weap[b].sourceamount - (wp.weap[b].newdestamount - wp.weap[b].destamount) > wp.weap[b].maxsourceamount) 
         wp.weap[b].newdestamount = wp.weap[b].sourceamount + wp.weap[b].destamount - wp.weap[b].maxsourceamount; 

}



void         tverlademunition::setloading(void)
{ 
   byte         b; 

   if (wp.count > 0) 
      for (b = 0; b < wp.count ; b++) {
            if (wp.weap[b].sourcepos == csprit) { 
               vehicle->fuel = wp.weap[b].newdestamount; 
               if (building) 
                  building->actstorage.a.fuel = wp.weap[b].sourceamount - (wp.weap[b].newdestamount - wp.weap[b].destamount);
               else
                  vehicle2->fuel = wp.weap[b].sourceamount - (wp.weap[b].newdestamount - wp.weap[b].destamount);
            } 
            else 
               if (wp.weap[b].sourcepos == cmaterial) { 
                  vehicle->material = wp.weap[b].newdestamount; 
                  material = wp.weap[b].sourceamount - (wp.weap[b].newdestamount - wp.weap[b].destamount); 
                  if (building) 
                     building->actstorage.a.material = wp.weap[b].sourceamount - (wp.weap[b].newdestamount - wp.weap[b].destamount);
                  else
                     vehicle2->material = wp.weap[b].sourceamount - (wp.weap[b].newdestamount - wp.weap[b].destamount);
               } 
               else 
                  if (wp.weap[b].sourcepos == cenergy) { 
                     vehicle->energy = wp.weap[b].newdestamount; 
                     energy = wp.weap[b].sourceamount - (wp.weap[b].newdestamount - wp.weap[b].destamount); 
                     if (building) 
                        building->actstorage.a.energy = wp.weap[b].sourceamount - (wp.weap[b].newdestamount - wp.weap[b].destamount);
                     else
                        vehicle2->energy = wp.weap[b].sourceamount - (wp.weap[b].newdestamount - wp.weap[b].destamount);
                  } 
                  else { 
                     if (vehicle2 == NULL) { 
                        /*
                        vehicle->munition[wp.weap[b].destpos] = wp.weap[b].newdestamount; 
                        (*munition)[wp.weap[b].sourcepos] = wp.weap[b].sourceamount - (wp.weap[b].newdestamount - wp.weap[b].destamount); 
                        */
                     } 
                     else { 
                        vehicle->ammo[wp.weap[b].destpos] = wp.weap[b].newdestamount; 
                        vehicle2->ammo[wp.weap[b].sourcepos] = wp.weap[b].sourceamount - (wp.weap[b].newdestamount - wp.weap[b].destamount);
                     } 
                  } 
      }
/*
  //recordunitparams(vehicle); 
   if (fast >= 2) 
      //recordbuildingparams(building);
   else 
      //recordunitparams(vehicle2);
*/
} 


void         verlademunition(pvehicle     vehicle,
                             pvehicle     vehicle2,
                             pbuilding    building,
                             byte         fast)
{ 
   tverlademunition vlm; 

   vlm.setvariables(vehicle,vehicle2,building,fast); 
   vlm.init(); 
   vlm.run(); 
   vlm.done(); 
     /*  0: innen dialog  */ 
     /*  1: innen fast  */ 
     /*  2: auáen dialog  */ 
     /*  3: auáen fast  */ 
} 

void showbdtbits( void )
{
   pfield fld = getactfield();
   char m[200];
   m[0] = 0;
   for (int i = 0; i < cbodenartennum ; i++) {
      tterrainbits bts;
      if ( i < 32 )
         bts.set ( 1 << i, 0 );
      else
         bts.set ( 0, 1 << ( i - 32));

      if (fld->bdt & bts) {
         strcat ( m, cbodenarten[i] );
         strcat ( m, "\n" );
      }
   } /* endfor */
   displaymessage( m, 3 );
}

void viewterraininfo ( void )
{
   if ( fieldvisiblenow  ( getactfield() )) {
      displaymessage(" field->direction: %d \n field->id: %d ", 1, getactfield()->direction,getactfield()->typ->terraintype->id);
      displaymessage(" defense bonus: %d/8 \n attack bonus: %d/8 ", 1, getactfield()->getdefensebonus(), getactfield()->getattackbonus());
      showbdtbits();
      char c[1000];
      c[0] = 0;
      for ( int i = 0; i < cmovemalitypenum; i++ ) {
        strcat ( c, strrr( getactfield()->getmovemalus(i) ));
        strcat ( c, " / " );
      }
      displaymessage( c, 1 );
      if  ( getactfield()->vehicle ) {

         pvehicletype typ = getactfield()->vehicle->typ;
         int mx = cbodenartennum;
         if ( cbodenartennum > 32 ) 
            mx = 32;

         int i;
         strcpy ( c, "the unit can drive onto the following fields:\n" );
         for ( i = 0; i < cbodenartennum; i++ ) {
            tterrainbits bts;
            if ( i < 32 )
               bts.set ( 1 << i, 0 );
            else
               bts.set ( 0, 1 << ( i - 32 ));

            if ( typ->terrainaccess->terrain & bts) {
               strcat ( c, cbodenarten[i] );
               strcat ( c, "\n" );
            }
         }

         displaymessage( c, 3 );

         strcpy ( c, "these bits must be set:\n" );
         for ( i = 0; i < cbodenartennum; i++ ) {
            tterrainbits bts;
            if ( i < 32 )
               bts.set ( 1 << i, 0 );
            else
               bts.set ( 0, 1 << ( i - 32 ));
            if ( typ->terrainaccess->terrainreq & bts) {
               strcat ( c, cbodenarten[i] );
               strcat ( c, "\n" );
            }
         }
         displaymessage( c, 3 );

         strcpy ( c, "these bits must NOT be set:\n" );
         for ( i = 0; i < cbodenartennum; i++ ) {
            tterrainbits bts;
            if ( i < 32 )
               bts.set ( 1 << i, 0 );
            else
               bts.set ( 0, 1 << ( i - 32 ));
            if ( typ->terrainaccess->terrainnot & bts) {
               strcat ( c, cbodenarten[i] );
               strcat ( c, "\n" );
            }
         }
         displaymessage( c, 3 );

         strcpy ( c, "the unit ist killed by:\n" );
         for ( i = 0; i < cbodenartennum; i++ ) {
            tterrainbits bts;
            if ( i < 32 )
               bts.set ( 1 << i, 0 );
            else
               bts.set ( 0, 1 << ( i - 32 ));
            if ( typ->terrainaccess->terrainkill & bts) {
               strcat ( c, cbodenarten[i] );
               strcat ( c, "\n" );
            }
         }
         displaymessage( c, 3 );
      }
      displaymessage(" basejamming: %d ", 1, getactfield()->getjamming());
   }
}


int encodepassword ( char* pw )
{
   if ( !pw )
      return 0;

   int len = strlen ( pw );

   if ( len )
      return crc32buf( pw, len+1 );
   else
      return 0;

   /*
   char* p = pw;
   int cr = 0;
   while ( pw[0] ) {
      cr+= ( pw[0] ^ pw[1] );
      cr = rol ( cr, pw[1]+1 );
      cr^= pw[0];
      pw++;
   } 

   if ( *p && !cr )
      cr = 1;

   return cr;

   */
}



int   tenterpassword :: gettextwdth_stredit ( char* txt, pfont font )
{
  char* ss2 = strdup ( txt );

  int i = 0;
  while ( ss2[i] )
     ss2[i++] = '*';
  
  ss2 [ i ] = 0;

  i = gettextwdth(ss2, font);
  asc_free ( ss2 );

  return i;
}


int   tenterpassword :: getcapabilities ( void )
{
   return 0;
}


void         tenterpassword ::lne(int          x1,
                 int          y1,
                 char *       s,
                 int          position,
                 char      einfuegen)
{
  char* ss2 = strdup ( s );

  int i = 0;
  while ( ss2[i] )
     ss2[i++] = '*';
  
  ss2 [ position ] = 0;
  i = x1 + gettextwdth(ss2,activefontsettings.font);
  int j = y1; 
  int k = y1 + activefontsettings.font->height; 
  collategraphicoperations cgo ( i-1, j, i+1, k );
  xorline(i,j,i,k,3);
  if (einfuegen == false) { 
     xorline(i + 1,j,i + 1,k,3); 
     xorline(i - 1,j,i - 1,k,3); 
  }  
  asc_free ( ss2 );
} 

void tenterpassword :: dispeditstring ( char* st, int x1, int y1 )
{
   char* ss2 = strdup ( st );

   int i = 0;
   while ( ss2[i] )
     ss2[i++] = '*';

   showtext2(ss2,x1,y1); 

  asc_free ( ss2 );
}

int tenterpassword :: checkforreask ( int crc )
{
   if ( crc == 0 || actmap->network && actmap->network->globalparams.reaskpasswords )
      return 1;
   else 
      return 0;
}

void tenterpassword :: init ( int* crc, int mode, char* ttl  )
{
   tdialogbox::init();
   xsize = 200;
   if ( *crc )
     ysize = 150;
   else
     ysize = 180;
   y1 = -1;
   x1 = -1;
   cr = crc;


   reask = checkforreask( *crc );

   windowstyle ^= dlg_in3d;
   if ( mode == 0 ) {
      if ( *crc == 0   &&  gameoptions.defaultpassword ) {
         addbutton ( "~O~k", 10, ysize - 35, xsize / 2 - 5, ysize - 10, 0, 1, 1, reask );
         addbutton ( "~D~efault", xsize / 2 + 5, ysize - 35, xsize - 10, ysize - 10, 0, 1,7, true );
      } else {
         addbutton ( "~O~k", 10, ysize - 35, xsize / 2 - 5, ysize - 10, 0, 1, 1, reask );
         addbutton ( "e~x~it", xsize / 2 + 5, ysize - 35, xsize - 10, ysize - 10, 0, 1,8, true );
      }

   } else {
      if ( *crc == 0   &&  gameoptions.defaultpassword ) {
         addbutton ( "~O~k", 10, ysize - 35, xsize / 3 - 5, ysize - 10, 0, 1, 1, reask );
         addbutton ( "~D~efault", xsize / 3 + 5,   ysize - 35, 2 * xsize / 3 - 5, ysize - 10, 0, 1,7, true );
         addbutton ( "~C~ancel", 2 * xsize / 3 + 5, ysize - 35, xsize - 10, ysize - 10, 0, 1, 6, true );
      } else {
         addbutton ( "~O~k", 10, ysize - 35, xsize / 2 - 5, ysize - 10, 0, 1, 1, reask );
         addbutton ( "~C~ancel", xsize / 2 + 5, ysize - 35, xsize - 10, ysize - 10, 0, 1, 6, true );
      }
   }

   addkey ( 1, ct_enter );
   addkey ( 1, ct_enterk );


   if ( !ttl )
      title = "enter password";
   else 
      title = ttl;

   

   addbutton ( "password:", 10, 50, xsize - 10, 75, 1, 0, 2, true );
   addeingabe ( 2, strng1, 0, 39 );

   if ( *crc == 0 ) {
      addbutton ( "confirmation:", 10, 95, xsize - 10, 120, 1, 0, 3, true );
      addeingabe ( 3, strng2, 0, 39 );
      confirm = 1;
   } else
      confirm = 0;

   strng1[0] = 0;
   strng2[0] = 0;
   status = 0;

   buildgraphics();

};


void tenterpassword :: buttonpressed ( char id )
{
   tdialogbox::buttonpressed ( id );
   if ( id == 1 )
      status = 1;
   else
      if ( id == 6 )
         status = 10;
      else
         if ( id == 7 ) {
            *cr = gameoptions.defaultpassword;
            status = 2;
         } else
            if ( id == 8 ) {
               erasemap();
               throw tnomaploaded();
            } else
               if ( reask == 0 )
                  if ( encodepassword ( strng1 ) == *cr )
                     enablebutton ( 1 );
                  else
                     disablebutton ( 1 );
               else
                  if ( strcmp ( strng1, strng2 ) == 0)
                     enablebutton ( 1 );
                  else
                     disablebutton ( 1 );
}

void tenterpassword :: run ( int* result )
{
   tdialogbox::run ();

   pbutton pb = firstbutton;
   while ( pb &&  (pb->id != 2)) 
      pb = pb->next;

   if ( pb )
      if ( pb->id == 2 )
         execbutton( pb , false );

   if ( strng1[0] ) {
      pb = firstbutton;
      while ( pb &&  (pb->id != 3)) 
         pb = pb->next;
   
      if ( pb )
         if ( pb->id == 3 )
            execbutton( pb , false );
   }

   mousevisible ( true );
   do {
      tdialogbox::run ();
   } while ( status == 0 ); /* enddo */

   if ( status == 1 )
      *cr = encodepassword ( strng1 );

   if ( result )
      *result = status;
}

void enterpassword ( int* cr )
{
   tenterpassword epw;
   epw.init ( cr, 0 );
   epw.run ( NULL );
   epw.done ();
}

