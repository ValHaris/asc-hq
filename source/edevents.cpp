/*! \file edevents.cpp
    \brief The event editing in the mapeditor
*/

//     $Id: edevents.cpp,v 1.31 2002-04-14 17:21:17 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.30  2001/10/11 10:22:49  mbickel
//      Some cleanup and fixes for Visual C++
//
//     Revision 1.29  2001/10/02 14:06:28  mbickel
//      Some cleanup and documentation
//      Bi3 import tables now stored in .asctxt files
//      Added ability to choose amoung different BI3 import tables
//      Added map transformation tables
//
//     Revision 1.28  2001/08/19 12:31:26  mbickel
//      Fixed several bugs in event and campaign handling
//
//     Revision 1.27  2001/08/19 10:48:49  mbickel
//      Fixed display problems in event dlg in mapeditor
//      Fixed error when starting campaign with AI as first player
//
//     Revision 1.26  2001/08/09 10:28:22  mbickel
//      Fixed AI problems
//      Mapeditor can edit a units AI parameter
//
//     Revision 1.25  2001/07/28 11:19:10  mbickel
//      Updated weaponguide
//      moved item repository from spfst to itemrepository
//
//     Revision 1.24  2001/07/15 21:00:25  mbickel
//      Some cleanup in the vehicletype class
//
//     Revision 1.23  2001/05/21 12:46:19  mbickel
//      Fixed infinite loop in AI::strategy
//      Fixed bugs in mapeditor - event editing
//      Fixed bugs in even loading / writing
//      Fixed wrong build order AI <-> main program
//
//     Revision 1.22  2001/03/23 16:02:56  mbickel
//      Some restructuring;
//      started rewriting event system
//
//     Revision 1.21  2001/02/26 12:35:09  mbickel
//      Some major restructuing:
//       new message containers
//       events don't store pointers to units any more
//       tfield class overhauled
//
//     Revision 1.20  2001/02/11 11:39:31  mbickel
//      Some cleanup and documentation
//
//     Revision 1.19  2001/02/01 22:48:36  mbickel
//      rewrote the storing of units and buildings
//      Fixed bugs in bi3 map importing routines
//      Fixed bugs in AI
//      Fixed bugs in mapeditor
//
//     Revision 1.18  2001/01/31 14:52:35  mbickel
//      Fixed crashes in BI3 map importing routines
//      Rewrote memory consistency checking
//      Fileselect dialog now uses ASCStrings
//
//     Revision 1.17  2001/01/28 14:04:12  mbickel
//      Some restructuring, documentation and cleanup
//      The resource network functions are now it their own files, the dashboard
//       as well
//      Updated the TODO list
//
//     Revision 1.16  2001/01/25 23:44:56  mbickel
//      Moved map displaying routins to own file (mapdisplay.cpp)
//      Wrote program to create pcx images from map files (map2pcx.cpp)
//      Fixed bug in repair function: too much resource consumption
//      AI improvements and bug fixes
//      The BI3 map import function now evaluates the player status (human/
//       computer)
//
//     Revision 1.15  2001/01/04 15:13:43  mbickel
//      configure now checks for libSDL_image
//      AI only conquers building that cannot be conquered back immediately
//      tfindfile now returns strings instead of char*
//
//     Revision 1.14  2000/11/08 19:31:03  mbickel
//      Rewrote IO for the tmap structure
//      Fixed crash when entering damaged building
//      Fixed crash in AI
//      Removed item CRCs
//
//     Revision 1.13  2000/10/18 14:14:04  mbickel
//      Rewrote Event handling; DOS and WIN32 may be currently broken, will be
//       fixed soon.
//
//     Revision 1.12  2000/10/11 14:26:29  mbickel
//      Modernized the internal structure of ASC:
//       - vehicles and buildings now derived from a common base class
//       - new resource class
//       - reorganized exceptions (errors.h)
//      Split some files:
//        typen -> typen, vehicletype, buildingtype, basecontainer
//        controls -> controls, viewcalculation
//        spfst -> spfst, mapalgorithm
//      bzlib is now statically linked and sources integrated
//
//     Revision 1.11  2000/08/12 12:52:45  mbickel
//      Made DOS-Version compile and run again.
//
//     Revision 1.10  2000/08/06 11:39:02  mbickel
//      New map paramter: fuel globally available
//      Mapeditor can now filter buildings too
//      Fixed unfreed memory in fullscreen image loading
//      Fixed: wasted cpu cycles in building
//      map parameters can be specified when starting a map
//      map parameters are reported to all players in multiplayer games
//
//     Revision 1.9  2000/07/05 09:24:00  mbickel
//      New event action: change building damage
//
//     Revision 1.8  2000/05/23 20:40:43  mbickel
//      Removed boolean type
//
//     Revision 1.7  2000/05/10 19:55:47  mbickel
//      Fixed empty loops when waiting for mouse events
//
//     Revision 1.6  2000/05/07 12:53:58  mbickel
//      some minor adjustments
//
//     Revision 1.5  2000/05/05 21:15:02  mbickel
//      Added Makefiles for mount/demount and mapeditor
//      mapeditor can now be compiled for linux, but is not running yet
//
//     Revision 1.4  2000/04/27 16:25:20  mbickel
//      Attack functions cleanup
//      New vehicle categories
//      Rewrote resource production in ASC resource mode
//      Improved mine system: several mines on a single field allowed
//      Added unitctrl.* : Interface for vehicle functions
//        currently movement and height change included
//      Changed timer to SDL_GetTicks
//
//     Revision 1.3  2000/03/16 14:06:54  mbickel
//      Added unitset transformation to the mapeditor
//
//     Revision 1.2  1999/11/16 03:41:28  tmwilson
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

#include <math.h>
#include "vehicletype.h"
#include "buildingtype.h"

#include "edmisc.h"
#include "edevents.h"
#include "edselfnt.h"
#include "mapdisplay.h"
#include "itemrepository.h"

// õS TplayerSel


class   tplayersel : public tstringselect {
           public :
                 int lastchoice;
                 virtual void setup(void);
                 virtual void buttonpressed(int id);
                 virtual void run(void);
                 virtual void gettext(word nr);
                 };

void         tplayersel::setup(void)
{ 

   action = 0;
   title = "Select Player"; 
   numberoflines = 9;
   ey = ysize - 60; 
   startpos = lastchoice;
   addbutton("~D~one",20,ysize - 40,170,ysize - 20,0,1,2,true); 
   addkey(2,ct_enter); 
   addbutton("~C~ancel",190,ysize - 40,340,ysize - 20,0,1,3,true); 
} 


void         tplayersel::buttonpressed(int         id)
{ 
   tstringselect::buttonpressed(id);
   switch (id) {
      
      case 2:   
      case 3:   action = id; 
   break; 
   } 
} 


void         tplayersel::gettext(word nr)
{ 
   if (nr == 8) strcpy(txt,"Neutral");
   else {
      if ((nr>=0) && (nr<=7)) {
         strcpy(txt,"Player ");
         strcat(txt,strrr(nr+1));
      } else {
         strcpy(txt,"");
      } /* endif */
  }
} 


void         tplayersel::run(void)
{ 
   do { 
      tstringselect::run(); 
   }  while ( ! ( (taste == ct_esc) || ( (action == 2) || (action == 3) ) || (msel == 1)) );
   if ( (action == 3) || (taste == ct_esc) ) redline = 255;
} 


int         playerselect(int lc)
{ 
  tplayersel  sm; 

   sm.lastchoice = lc;
   sm.init(); 
   sm.run(); 
   sm.done(); 
   return sm.redline; 
} 


// õS VerknuepfSel

class   verknuepfsel : public tstringselect {
           public :
                 virtual void setup(void);
                 virtual void buttonpressed(int id);
                 virtual void run(void);
                 virtual void gettext(word nr);
                 };

void         verknuepfsel::setup(void)
{ 

   action = 0;
   title = "Select Connection";
   numberoflines = 8;
   ey = ysize - 60; 
   addbutton("~D~one",20,ysize - 40,170,ysize - 20,0,1,2,true); 
   addkey(2,ct_enter); 
   addbutton("~C~ancel",190,ysize - 40,340,ysize - 20,0,1,3,true); 
} 


void         verknuepfsel::buttonpressed(int         id)
{ 
   tstringselect::buttonpressed(id);
   switch (id) {
      
      case 2:   
      case 3:   action = id; 
   break; 
   } 
} 


void         verknuepfsel::gettext(word nr)
{ 
   strcpy(txt,ceventtriggerconn[nr]);
} 


void         verknuepfsel::run(void)
{ 
   do { 
      tstringselect::run(); 
   }  while ( ! ( (taste == ct_esc) || ( (action == 2) || (action == 3) ) || (msel == 1)) );
   if ( (action == 3) || (taste == ct_esc) ) redline = 255;
} 


int         verknuepfselect(void)
{ 
  verknuepfsel  vm;

   vm.init();
   vm.run();
   vm.done();
   return vm.redline;
} 

// õS  playerselect bei allenemy *.*

class  tplayerselall : public tdialogbox {
          public :
              int action;
              int bkgcolor;
              int playerbit;
              void init(void);
              virtual void run(void);
              virtual void buttonpressed(int id);
              void anzeige(void);
              };




void         tplayerselall::init(void)
{ 
   char *s1;

   tdialogbox::init(); 
   title = "Player Select";
   x1 = 50;
   xsize = 370; 
   y1 = 50; 
   ysize = 380; 
   action = 0; 
   bkgcolor = lightgray;

   windowstyle = windowstyle ^ dlg_in3d; 

   int i;
   for (i=0;i<8 ;i++ ) {
      s1 = new(char[12]);
      strcpy(s1,"Player ~");
      strcat(s1,strrr(i+1));
      strcat(s1,"~");
      addbutton(s1,20,55+i*30,150,75+i*30,0,1,6+i,true); 
      addkey(1,ct_1+i); 
   }  

   addbutton("~A~ll not allied",20,ysize - 40,170,ysize - 20,0,1,1,true); 
   addkey(1,ct_enter); 
   addbutton("~M~y selection",200,ysize - 40,350,ysize - 20,0,1,2,true); 

   buildgraphics(); 

   for ( i=0; i<8 ;i++ )
      bar(x1 + 170,y1 + 60 + i*30 ,x1 + 190 ,y1 + 70 + i * 30,20 + ( i << 3 ));

   anzeige();
   
   mousevisible(true); 
} 

void         tplayerselall::anzeige(void)
{
   mousevisible(false); 
   for (int i=0;i<8 ;i++ ) {
      if ( playerbit & ( 1 <<  ( i+2 ) ) )
         rectangle (x1 + 16,y1+51+i*30,x1+154,y1+79+i*30, 20 );
      else
         rectangle (x1 + 16,y1+51+i*30,x1+154,y1+79+i*30, bkgcolor );
   }
   mousevisible(true); 
}


void         tplayerselall::run(void)
{ 

   do { 
      tdialogbox::run(); 
      if (taste == ct_f1) help ( 1060 );
   }  while (!((taste == ct_esc) || ((action == 1) || (action ==2)))); 
   if ((action == 1) || (taste == ct_esc))
      playerbit = 1;
   else {
      playerbit = playerbit | 2; //bit 1 setzen;
      playerbit = playerbit &  (65535 ^  1); // bit 0 l”schen;
   }
} 


void         tplayerselall::buttonpressed(int         id)
{ 
   tdialogbox::buttonpressed(id); 
   switch (id) {
      
      case 1:   
      case 2:   action = id; 
        break;
      case 6:
      case 7:
      case 8:
      case 9:
      case 10:
      case 11:
      case 12:
      case 13: {
            playerbit ^=  1 << ( id-4 ) ;
         anzeige();
      }
   break; 
   } 
} 


void playerselall( int *playerbitmap)  
{ tplayerselall       sc; 

   sc.playerbit = *playerbitmap;
   sc.init(); 
   sc.run(); 
   sc.done(); 
   *playerbitmap = sc.playerbit;
} 


// õS EventTypeSel

class   teventtype : public tstringselect {
           public :
                 int lastchoice;
                 virtual void setup(void);
                 virtual void buttonpressed(int id);
                 virtual void run(void);
                 virtual void gettext(word nr);
                 };

void         teventtype::setup(void)
{ 

   action = 0; 
   title = "Select EventType"; 
   numberoflines = ceventactionnum;
   ey = ysize - 60;
   if (lastchoice == 255) lastchoice = 0;
   startpos = lastchoice;
   addbutton("~D~one",20,ysize - 40,170,ysize - 20,0,1,4,true);
   addkey(4,ct_enter);
   addbutton("~C~ancel",190,ysize - 40,340,ysize - 20,0,1,5,true);
} 

void teventtype::gettext(word nr)
{
   strcpy(txt,ceventactions[nr]);
}


void         teventtype::buttonpressed(int         id)
{ 
   tstringselect::buttonpressed(id); 
   switch (id) {
      case 4:
      case 5:   action = id-2;
   break; 
   } 
} 


void         teventtype::run(void)
{ 
   do { 
      tstringselect::run();
   }  while (!((taste == ct_esc) || ((action == 2) || (action == 3)) || (msel == 1)));
   if ((action == 3) || (taste == ct_esc)) redline = 255;
} 


int         eventtypeselect(int lc)
{ 
  teventtype   sm; 

   sm.lastchoice = lc;
   sm.init(); 
   sm.run(); 
   sm.done();
   return sm.redline; 
} 


// õS ReasonSel

class   treason : public tstringselect {
           public :
                 int lastchoice;
                 virtual void setup(void);
                 virtual void buttonpressed(int id);
                 virtual void run(void);
                 virtual void gettext(word nr);
                 };

void         treason::setup(void)
{ 

   action = 0; 
   title = "Select Reason"; 
   numberoflines = ceventtriggernum;
   ey = ysize - 60;
   startpos = lastchoice;
   addbutton("~D~one",20,ysize - 40,170,ysize - 20,0,1,4,true); 
   addkey(4,ct_enter); 
   addbutton("~C~ancel",190,ysize - 40,340,ysize - 20,0,1,5,true); 
} 


void         treason::gettext(word nr)
{ 
   strcpy(txt,ceventtrigger[nr]);
} 


void         treason::buttonpressed(int         id)
{ 
   tstringselect::buttonpressed(id); 
   switch (id) {
      case 4:   
      case 5:   action = id; 
   break; 
   } 
} 


void         treason::run(void)
{ 
   do { 
      tstringselect::run(); 
   }  while (!((taste == ct_esc) || ((action == 4) || (action == 5)) || (msel == 1))); 
   if ((action == 5) || (taste == ct_esc)) redline = 255;
} 


int         getreason(int lc)
{ 
  treason      sm; 

   sm.lastchoice = lc;
   sm.init(); 
   sm.run(); 
   sm.done(); 
   return sm.redline; 
} 

// õS GetXYSel


class  tgetxy : public tdialogbox {
          public :
              int action;
              int x,y;
              void init(void);
              virtual void run(void);
              virtual int condition(void);
              virtual void buttonpressed(int id);
              };

void         tgetxy::init(void)
{ 
   tdialogbox::init(); 
   title = "X/Y-Pos"; 
   x1 = 150;
   xsize = 320; 
   y1 = 150; 
   ysize = 140; 
   action = 0; 

   windowstyle = windowstyle ^ dlg_in3d; 


   addbutton("~D~one",20,ysize - 40,100,ysize - 20,0,1,1,true);
   addkey(1,ct_enter); 
   addbutton("~C~ancel",120,ysize - 40,200,ysize - 20,0,1,2,true);
   addbutton("~M~ap",220,ysize - 40,300,ysize - 20,0,1,3,true);
   addbutton("~X~-Pos",20,60,150,80,2,1,4,true);
   addeingabe(4,&x,0,actmap->xsize - 1);
   addbutton("~Y~-Pos",170,60,300,80,2,1,5,true);
   addeingabe(5,&y,0,actmap->ysize - 1);

   buildgraphics(); 

   mousevisible(true); 
} 


void         tgetxy::run(void)
{ 

   do { 
      tdialogbox::run(); 
   }  while (! ( (taste == ct_esc) || (action == 1) || (action == 2) ) );
   if ((action == 2) || (taste == ct_esc)) x = 50000; 
} 

int         tgetxy::condition(void)
{
   return 1;
}

void         tgetxy::buttonpressed(int         id)
{ 

   tdialogbox::buttonpressed(id); 
   switch (id) {
      
      case 1:   
      case 2:   action = id; 
   break; 
      case 3: {
                  mousevisible(false);
                  x = getxpos(); 
                  y = getypos(); 
                  displaymap(); 
                  mousevisible(true);
                  do {
                     selectfield(&x,&y);
                  } while ( condition() != 1 ); /* enddo */
                  mousevisible(false);
                  redraw();
                  enablebutton(4);
                  enablebutton(5);
                  mousevisible(true);
                  
                }
   break;
   } 
} 

void         getxy(word *x,word *y)
{ tgetxy       ce;

   ce.x = *x ; 
   ce.y = *y ; 
   ce.init(); 
   ce.run(); 
   *x = ce.x; 
   *y = ce.y; 
   ce.done(); 
} 

void         getxy( int *x, int *y)
{ tgetxy       ce;

   ce.x = *x ;
   ce.y = *y ;
   ce.init();
   ce.run();
   *x = ce.x;
   *y = ce.y;
   ce.done();
}


// õS GetXYUnitSel

class  tgetxyunit : public tgetxy {
          public :
              virtual int condition(void);
              };


int         tgetxyunit::condition(void)
{
   if ( getfield( x, y ) )
      if ( getfield(x,y)->vehicle ) 
          return 1;
   return 0;
}

void         getxy_unit(int *x,int *y)
{ tgetxyunit       gu;

   gu.x = *x;
   gu.y = *y;
   gu.init();
   gu.run();
   *x = gu.x;
   *y = gu.y;
   gu.done();
} 

class  tgetxybuilding : public tgetxy {
          public :
              virtual int condition(void);
              };


// õS GetXYBuildingSel

int         tgetxybuilding::condition(void)
{
   if ( getfield( x, y ) )
      if ( getfield(x,y)->building ) 
          return 1;
   return 0;
}

void         getxy_building(int *x,int *y)
{ tgetxybuilding       gb;

   gb.x = *x ;
   gb.y = *y ;
   gb.init();
   gb.run();
   *x = gb.x;
   *y = gb.y;
   gb.done();
} 

// õS GetTurn/MoveSel

class  tgettm : public tdialogbox {
          public :
              int action;
              int x,y;
              void init(void);
              virtual void run(void);
              virtual void buttonpressed(int id);
              };




void         tgettm::init(void)
{ 
   tdialogbox::init(); 
   title = "Turn/Move";
   x1 = 150;
   xsize = 320; 
   y1 = 150; 
   ysize = 140; 
   action = 0; 
   if ((x < 0) || (x > 100000)) x = 0;   /* ! */ 
   if ((y < 0) || (y > 100000)) y = 0;   /* ! */ 

   windowstyle = windowstyle ^ dlg_in3d; 


   addbutton("~D~one",20,ysize - 40,150,ysize - 20,0,1,1,true); 
   addkey(1,ct_enter); 
   addbutton("~C~ancel",170,ysize - 40,300,ysize - 20,0,1,2,true); 
   addbutton("~T~urn",20,60,150,80,2,1,3,true); 
   addeingabe(3,&x,0,100000);   /* ! */ 
   addbutton("~M~ove",170,60,300,80,2,1,4,true); 
   addeingabe(4,&y,-100,100000);   /* ! */

   buildgraphics(); 

   mousevisible(true); 
} 


void         tgettm::run(void)
{ 

   do { 
      tdialogbox::run(); 
   }  while (!((taste == ct_esc) || ((action == 1) || (action ==2)))); 
   if ((action == 2) || (taste == ct_esc)) x = 50000; 
} 


void         tgettm::buttonpressed(int         id)
{ 
   tdialogbox::buttonpressed(id); 
   switch (id) {
      
      case 1:   
      case 2:   action = id; 
   break; 
   } 
} 


bool gettm( GameTime& time  )  /* True : Erfolgreich ausgefhrt */
{ tgettm       ce; 
  char b;

  ce.x = time.turn();
  ce.y = time.move();

   ce.init();
   ce.run();
   if (ce.x != 50000) {
      time.set ( ce.x , ce.y );
      b = true;
   }  else
      b = false;
   ce.done();
   return b;
} 

// õS GetEventID

class   tgeteventid : public tstringselect {
           public :        
                 int lastid;
                 int teventid;
                 int mapid;
                 void rebuildlines(void);
                 virtual void setup(void);
                 virtual void buttonpressed(int id);
                 virtual void gettext(word nr);
                 virtual void run(void);
                 };


void         tgeteventid::setup(void)
{ 
  pevent e;

   title = "Select Event";
   numberoflines = 0;
   teventid = 0;
   mapid = 0;

   e = actmap->firsteventtocome; 
   while (e != NULL) { 
      if (e->id == lastid  && !mapid ) {
         startpos = numberoflines;
         teventid = e->id;
      } 
      e = e->next; 
      numberoflines++;
   } 
   xsize = 440; 
   ysize = 360; 
   sy = 120; 
   ey = ysize - 20; 
   addbutton("~E~vent ID",20,50,100,70,2,1,5,true);
   addeingabe(5,&teventid,1,2147483647);

   addbutton("~M~ap ID",120,50,200,70,2,1,9,true);
   addeingabe(9,&mapid,0,2147483647);

   addbutton("~D~one",220,40,310,70,0,1,7,true);
   addbutton("~C~ancel",330,40,420,70,0,1,8,true);
} 

void   tgeteventid::gettext(word nr) //gibt in txt den string zurck
{
   word    count;
   pevent  e;

   count=0;
   e = actmap->firsteventtocome; 
   while ((e != NULL) && (count<nr)) {
      count++;
      e = e->next; 
   }      
   if (e != NULL) {
      strcpy(txt,ceventactions[e->a.action]);
      strcat(txt,"  ");
      strcat(txt,e->description);
   }
}


void         tgeteventid::buttonpressed(int         id)
{    
   tstringselect::buttonpressed(id);
   switch (id) {
   case 7:  if ( teventid != 0 ) 
               action = 1;
      break;
   case 8: {
               action = 1;
               teventid = 0;
              }
      break;
   } 
} 


void         tgeteventid::run(void)
{ 
   int count = 0;
   pevent e;

   do { 
      tstringselect::run();
      if ( (msel == 1 ) || ( taste == ct_enter ) ){
         e = actmap->firsteventtocome; 
         while ((e != NULL) && (count<redline)) {
            count++;
            e = e->next; 
         }      
         if (e != NULL) {
            teventid = e->id;
            showbutton(5);
         }
      } 
   }  while ((taste != ct_esc) && (action != 1));
   if (taste == ct_esc) teventid = 0;
} 

void  geteventid( int* eid, int* mid )
{ 
   tgeteventid    tg;

   tg.lastid = *eid;
   tg.mapid = *mid;

   tg.init();
   tg.run();
   *eid = tg.teventid;
   *mid = tg.mapid;
   tg.done();
} 


// õS gettechnologyid

class   tgettechnologyid : public tstringselect {
           public :        
                 int lastid;
                 int ttechnologyid;
                 void rebuildlines(void);
                 virtual void setup(void);
                 virtual void buttonpressed(int id);
                 virtual void gettext(word nr);
                 virtual void run(void);
                 };


void         tgettechnologyid::setup(void)
{ 
   title = "Select Technology";
   numberoflines = technologynum;
   ttechnologyid = 0;
   for (int i=0;i< technologynum;i++) {
      if (gettechnology_forpos(i)->id == lastid ) {
         startpos = i;
         ttechnologyid = gettechnology_forpos(i)->id;
         i = technologynum;
      } 
   } /* endfor */
   xsize = 440; 
   ysize = 360; 
   sy = 120; 
   ey = ysize - 20; 
   addbutton("~I~D",20,50,200,70,2,1,5,true);
   addeingabe(5,&ttechnologyid,1,maxint);
   addbutton("~D~one",220,40,310,70,0,1,7,true);
   addbutton("~C~ancel",330,40,420,70,0,1,8,true);
} 

void   tgettechnologyid::gettext(word nr) //gibt in txt den string zurck
{
   strcpy(txt,gettechnology_forpos(nr)->name);
}
                        

void         tgettechnologyid::buttonpressed(int         id)
{    
   tstringselect::buttonpressed(id);
   switch (id) {
   case 7:  if ( ttechnologyid )
               action = 1;
      break;
   case 8: {

               action = 1;
               ttechnologyid = 0;
              }
      break;
   } 
} 


void         tgettechnologyid::run(void)
{ 
   do {
      tstringselect::run();
      if ( (msel == 1 ) || (taste == ct_enter ) ) {
         ttechnologyid = gettechnology_forpos(redline)->id;
         showbutton(5);
      }                            
   }  while ((taste != ct_esc) && (action != 1));
} 


int         gettechnologyid(int lid)
{ tgettechnologyid    tt;

   tt.lastid = lid;
   tt.init();
   tt.run();
   tt.done();
   ch = 0;
   return tt.ttechnologyid;
} 

// õS ChangeWind

     class twindchange: public tdialogbox {
              public:
                word        heightxs,w2,dirx,diry;
                int         action,tsintensity,abort,activeheight;
                int            tdirect[3],tinten[3];
                pevent      we;
                void          init(void);
                void          showdir_and_height(void);
                virtual void        run(void);
                virtual void        buttonpressed(int         id);
        };


void         twindchange::init(void)
{ word         w;
  int            i;

   tdialogbox::init();
   action = 0; 
   title = "Wind Change";
   x1 = 70;
   xsize = 500;
   y1 = 40;
   ysize = 250;
   w = (xsize - 60) / 2; 
   dirx= 370;
   diry= 110;
   action = 0; 
 
   activeheight = 0;

   if ( (we->datasize == 24) && we->rawdata  ) {
      int* puffer = we->intdata;
      for ( int i=0 ;i<3 ;i++ ) {
          tinten[i] = puffer[i];
          tdirect[i] = puffer[i+3];
      } /* endfor */
   } else {
      for (i=0;i<=2;i++ ) {
         tdirect[i] = 0;
         tinten[i] = 0;
      } 
   } /* endif */

   tsintensity = tinten[activeheight];

   windowstyle = windowstyle ^ dlg_in3d; 

   addbutton("~I~ntensity",50,60,250,80,2,1,1,true);
   addeingabe(1,&tsintensity,0,255);

   heightxs = 120;
   w2 = 57;
   for (i=0;i<=2 ;i++) {
       addbutton("",40+( i * w2),heightxs,20 + w2 * ( i +1 ),heightxs+24,0,1,i+2,true);
       addkey(i+2,ct_1+i);
   } // Buttons 2-4
            
   // 8 im Kreis bis 7



   addbutton("~D~one",20,ysize - 40,20 + w,ysize - 10,0,1,30,true);
   addkey(30,ct_enter);
   addbutton("~C~ancel",40 + w,ysize - 40,40 + 2 * w,ysize - 10,0,1,31,true);

   buildgraphics(); 


   float radius = 50;
   float pi = 3.14159265;
   for ( i = 0; i < sidenum; i++ ) {
                              
      int x = (int) (dirx + radius * sin ( 2 * pi * (float) i / (float) sidenum ));
      int y = (int) (diry - radius * cos ( 2 * pi * (float) i / (float) sidenum ));

      addbutton("", x-10, y - 10, x + 10, y + 10,0,1,14+i,true);
      enablebutton ( 14+i);
      void* pic = rotatepict ( icons.pfeil2[0], directionangle[i] );
      int h,w;
      getpicsize ( pic, w, h );
      putspriteimage ( x1 + x - w/2, y1 + y - h/2, pic );
      delete pic;
  }

   showdir_and_height();
 
   mousevisible(false);
   for (i=0;i<=2 ;i++) {
      putimage(x1 + 43+( i * w2), y1 + heightxs + 2 ,icons.height[i+4]);
   } 

   mousevisible(true); 
} 

void         twindchange::run(void)
{ 
   do { 
      tdialogbox::run();
   }  while (!((taste == ct_esc) || (action == 1))); 
} 

void         twindchange::showdir_and_height(void)
{
    bar(x1 + 40, y1 + heightxs-7, x1 + 240 , y1 + heightxs-4,lightgray);
    bar(x1 + 45+( activeheight * w2), y1 + heightxs-7, x1 + 15 + w2 * ( activeheight +1 ), y1 + heightxs-4,red);
    bar(dirx+40,diry+40,dirx+72,diry+72,lightgray);


      void* pic = rotatepict ( icons.pfeil2[0], directionangle[tdirect[activeheight]] );
      int h,w;
      getpicsize ( pic, w, h );
      bar ( x1 + dirx - 20, y1 + diry - 20, x1 + dirx + 20, y1 + diry + 20, dblue );
      putspriteimage( x1 + dirx - w/2 , y1 + diry - h/2, pic );
      delete pic;


}


void         twindchange::buttonpressed(int         id)
{ 
   switch (id) {
      case 2:
      case 3:
      case 4: 
         {
              if (id-2 != activeheight ) {
                 tinten[activeheight] = tsintensity;
                 activeheight = id-2;
                 showdir_and_height();
                 tsintensity = tinten[activeheight];
                 showbutton(1);
              } 
         }
         break;
      case 14 : 
      case 15 :  
      case 16 : 
      case 17 : 
      case 18 : 
      case 19 : 
      case 20 : 
      case 21 : {
                   tdirect[activeheight] = id-14;
                   showdir_and_height();
                }
          break;
      case 30:
         {
            tinten[activeheight] = tsintensity;
            action = 1; 
            abort = 0;
            we->datasize = 24;
            int* puffer = new int[6];
            for ( int i=0 ;i<3 ;i++ ) {
                puffer[i] = tinten[i];
                puffer[i+3] = tdirect[i];
            } /* endfor */
            if ( we->rawdata ) 
               asc_free( we->rawdata );
            we->intdata = puffer;
       } 
       break;
    case 31 : {
            action = 1;
            abort = 1;
         }
         break;
   } /* endswitch */
} 


int         changewind(pevent acte)
{ 
   twindchange        wc;

   wc.we = acte;
   wc.init();
   wc.run();
   wc.done();
   return wc.abort;
} 



// õS SelectGameParameter



// õS CreateEvent

class  tcreateevent : public tdialogbox {
          public :
              pevent ae, orgevent;
              int reasonst;
              int action;
              char build;
              void init(void);
              virtual void run(void);
              virtual void buttonpressed(int id);
              virtual void showreason(int nr);
              virtual void showplayer(void);
              virtual void showeventtype(void);
              virtual void showeventid ( void );
              void freedata ( void ); // setzt den speicher in data frei / data = 0 / datasize = 0
            protected:
              int getcapabilities ( void ) { return 1; };
              void redraw ( void );
              void draw_additional_graphics( void );
            private:
              int reasonx;
              };


void tcreateevent::showplayer(void)
{
   char s[200];

   activefontsettings.length = 84;
   bar(x1 + xsize - 110,y1 + 90,x1 + xsize- 20,y1 + 110,lightgray);
   rahmen(true,x1 + xsize - 110,y1 + 90,x1 + xsize- 20,y1 + 110);
   if (ae->player == 8) showtext2("Neutral",x1 + xsize - 105,y1 + 92);
   else {
      strcpy(s,"Player ");
      showtext2(strcat(s,strrr(ae->player + 1)),x1 + xsize -105,y1 + 92);
   }
}

void tcreateevent::showeventtype(void)
{
   activefontsettings.length = 200;
   bar(x1 + 110,y1 + 40,x1 + xsize - 80 ,y1 + 60,lightgray);
   rahmen(true,x1 + 110,y1 + 40,x1 + xsize - 80 ,y1 + 60);
   if (ae->a.action != 255) showtext2( ceventactions[ae->a.action],x1 + 115,y1 + 42);

}

void tcreateevent::showreason(int nr)
{
   npush ( activefontsettings );
   activefontsettings.font = schriften.smallarial; 
   activefontsettings.justify = lefttext; 
   activefontsettings.color = red;
   activefontsettings.markcolor = lightred;
   activefontsettings.background = lightgray;

   int showv1 = 20;
   int showr = 115;
   int showv2 = 300;
   char s[300];

   bar(x1 + showv1,y1 + reasonst + nr * 40, x1 + xsize - 250,y1 + reasonst + 20 + nr * 40,lightgray);

   strcpy(s,"");
   if ( ae->triggerconnect[nr] & ceventtrigger_and ) strcat(s,ceventtriggerconn[0]);
   if ( ae->triggerconnect[nr] & ceventtrigger_or ) strcat(s,ceventtriggerconn[1]);
   if ( ae->triggerconnect[nr] & ceventtrigger_not ) strcat(s,ceventtriggerconn[2]);
   if ( ae->triggerconnect[nr] & ceventtrigger_klammerauf ) strcat(s,ceventtriggerconn[3]);
   if ( ae->triggerconnect[nr] & ceventtrigger_2klammerauf ) strcat(s,ceventtriggerconn[4]);

   activefontsettings.length = showr - showv1- 5;
   showtext2(s,x1+showv1+2,y1+reasonst + 2 + nr * 40);

   strcpy(s,"");
   strcat(s,ceventtrigger[ae->trigger[nr]]);

   activefontsettings.length = showv2 - showr - 5;
   showtext2(s,x1+showr+2,y1+reasonst + 2 + nr * 40);

   strcpy(s,"");
   if ( ae->triggerconnect[nr] & ceventtrigger_2klammerzu ) strcat(s,ceventtriggerconn[5]);
   if ( ae->triggerconnect[nr] & ceventtrigger_klammerzu ) strcat(s,ceventtriggerconn[6]);

   activefontsettings.length = ( xsize - 250 ) - showv2;
   showtext2(s,x1+showv2+2,y1+reasonst + 2 + nr * 40);

   npop ( activefontsettings );
}

void         tcreateevent::showeventid ( void )
{
   npush ( activefontsettings );

   int reasonx = xsize - 230;
   activefontsettings.font = schriften.smallarial; 
   activefontsettings.justify = lefttext; 
   activefontsettings.color = red;
   activefontsettings.markcolor = lightred;
   activefontsettings.background = lightgray;
   activefontsettings.length = 150;

   showtext2 ( strrr ( ae->id ), x1 + reasonx - 170, y1 + reasonst + 175 );

   npop ( activefontsettings );
}

void tcreateevent::draw_additional_graphics ( void )
{
   activefontsettings.font = schriften.smallarial; 
   activefontsettings.justify = lefttext; 
   activefontsettings.color = red;
   activefontsettings.markcolor = lightred;
   activefontsettings.background = lightgray;
   activefontsettings.length = 0;

   showtext2 ( "ID:", x1 + reasonx - 200, y1 + reasonst + 175 );

   showeventid();
   showeventtype();
   showplayer();

   for ( int i=0 ;i < 4 ;i++ ) 
      showreason(i);
}

void tcreateevent::redraw ( void )
{
   tdialogbox::redraw();
   npush ( activefontsettings );
   draw_additional_graphics();
   npop ( activefontsettings );
}


void         tcreateevent::init(void)
{

   tdialogbox::init(); 
   title = "New Event"; 
   x1 = 30;
   xsize = 580; 
   y1 = 50;
   ysize = 380;
   action = 0; 

   windowstyle = windowstyle ^ dlg_in3d; 

   addbutton("~D~one",20,ysize - 40,280,ysize - 20,0,1,1,true); 
   addkey(1,ct_enter); 
   addbutton("~C~ancel",300,ysize - 40,560,ysize - 20,0,1,2,true); 

   addbutton("~E~venttype",20,40,100 ,60,0,1,3,true);
/*
   addbutton("~N~um",xsize-60,40,xsize-20,60,2,1,14,true);
   addeingabe(14,&ae->num,0,255);
*/

   addbutton("De~s~cription",20,90,xsize - 200,110,1,1,20,true);
   addeingabe(20,&ae->description,0,19);

   addbutton("~P~layer",xsize - 180,90,xsize - 120,110,0,1,4,true);

   reasonst = 140; //Start fr Eventreasonanzeige y
   reasonx = xsize - 230;

   addbutton("Reason ~1~",reasonx,reasonst,reasonx+90,reasonst+20,0,1,5,true);
   addbutton("Reason ~2~",reasonx,reasonst+40,reasonx+90,reasonst+60,0,1,6,true);
   addbutton("Reason ~3~",reasonx,reasonst+80,reasonx+90,reasonst+100,0,1,7,true);
   addbutton("Reason ~4~",reasonx,reasonst+120,reasonx+90,reasonst+140,0,1,8,true);

   addbutton("~T~urn offset", reasonx, reasonst + 175, reasonx + 90, reasonst + 195, 2, 1, 25, true );
   addeingabe( 25, &ae->delayedexecution.turn, 0, 100000 );

   addbutton("~M~ove offset", reasonx+100, reasonst + 175, reasonx + 190, reasonst + 195, 2, 1, 26, true );
   addeingabe( 26, &ae->delayedexecution.move, -1000, 100000 );



   int verkx = xsize -120;
 
   int i;
   for ( i=0;i<4 ;i++ ) {
        static char* bname[4] = { "Connection ~F1~", "Connection ~F2~", "Connection ~F3~", "Connection ~F4~" };

        addbutton(bname[i],verkx ,reasonst + i * 40 ,verkx + 100 ,reasonst + 20 + i * 40,0,1,9 + i,true);
        addkey(9 + i,ct_f1 + i);
   } /* endfor */

   buildgraphics(); 

   mousevisible(true); 
} 


void         tcreateevent::run(void)
{ 
   do { 
      tdialogbox::run(); 
      for ( int i = 0; i < 4; i++ )
         if ( mouseparams.taste == 2 )
            if ( mouseinrect ( x1 + 117, y1 + reasonst + 2 + i * 40, x1 + 117 + 20, y1 + reasonst + 2 + i * 40 + 20 )) {
               if ( ae->trigger[i] == ceventt_event ) {
                  pevent ev = actmap->firsteventtocome;
                  while ( ev ) {
                     if ( ev->id == ae->trigger_data[i]->id  && ((actmap->campaign && actmap->campaign->id == ae->trigger_data[i]->mapid) || (ae->trigger_data[i]->mapid == 0 )))
                        displaymessage ( ev->description, 1 );
                     ev = ev->next;
                  }
               }   
               if ( ae->trigger[i] == ceventt_buildingconquered  ||
                    ae->trigger[i] == ceventt_buildinglost  ||
                    ae->trigger[i] == ceventt_building_seen  ||
                    ae->trigger[i] == ceventt_buildingdestroyed ) {
                    pbuilding building = ae->trigger_data[i]->building;
                    displaymessage ( building->getName().c_str(), 1 );

               }
               if ( ae->trigger[i] == ceventt_unitconquered  ||
                    ae->trigger[i] == ceventt_unitlost  ||
                    ae->trigger[i] == ceventt_specific_unit_enters_polygon  ||
                    ae->trigger[i] == ceventt_unitdestroyed ) {
                    pvehicle vehicle;
                    if ( ae->trigger[i] == ceventt_specific_unit_enters_polygon )
                       vehicle = actmap->getUnit( ae->trigger_data[i]->unitpolygon->vehiclenetworkid );
                    else
                       vehicle = actmap->getUnit( ae->trigger_data[i]->networkid );

                    if ( !vehicle->name.empty() )
                       displaymessage ( vehicle->name.c_str(), 1 );
                    else
                       if ( !vehicle->typ->name.empty() )
                          displaymessage ( vehicle->typ->name, 1 );
                       else
                          displaymessage ( vehicle->typ->description, 1 );
   

               }
            }
   }  while (!((taste == ct_esc) || ((action == 1) || (action == 2))));
} 

void tcreateevent::freedata( void )
{
   if (ae->rawdata ) {
      asc_free( ae->rawdata );
      ae->rawdata = NULL;
      ae->datasize = 0;
   }
}


pvehicle selectunit ( pvehicle unit )
{
    int x, y;
    int cnt = 0;
    int abb = 1;
    for ( int pp = 0; pp < 9; pp++ )
       if ( !actmap->player[pp].vehicleList.empty() )
          cnt++;
    if ( cnt ) {
       if ( unit ) {
          x = unit->xpos;
          y = unit->ypos;
       } else {
          x = 0;
          y = 0;
       }

       do { 
          getxy_unit(&x,&y); 
          if ((x != 50000)) { 
             pf2 = getfield(x,y);
             abb = false;
             if ( pf2 && pf2->vehicle ) 
                abb = true;
          }
       }  while (!((x == 50000) || abb)); 
       if ((x != 50000) && abb) 
          return pf2->vehicle;
       else 
          return NULL;
    } else {
       displaymessage("no vehicles on map !", 1 );
       return NULL;
    }
}

int selectunit ( int unitnetworkid )
{
  pvehicle v = actmap->getUnit ( unitnetworkid );
  v = selectunit ( v );
  if ( v )
     return v->networkid;
  else
     return 0;
}



// There is a memory leak at the unit_enters_polygon trigger; but it is not large and should be fixed by a general rewrite of the trigger functions
// I just want to get this trigger working....


void         tcreateevent::buttonpressed(int         id)
{   int           nid, nr, rnr;
    pfield        pf2; 
    char    abb; 
    word        i;
    int           *puffer= NULL;
    int           x,y;
    ppolygon  poly = NULL;

    tevent::LargeTriggerData::PolygonEntered etpe;


   // const int         stringpriority[7]     = {32, 64, 1, 2, 4, 8, 16};
   // const int         stringprioritynumber[7]     = {5, 6, 0, 1, 2, 3, 4};
 
   switch (id) {
      
      case 1: {  
                  int dup;
                  do {
                     dup = 0;

                     pevent ev = actmap->firsteventtocome;
                     while ( ev ) {
                        if ( ev != orgevent )
                           if ( ae->id == ev->id ) {
                              dup++;
                              ae->a.num++;
                           }
                        ev = ev->next;
                     }
                  } while ( dup );

                  int trignum = 0;
                  for ( int ii = 0; ii < 4; ii++ )
                     if ( ae->trigger [ii] != 0 )
                        trignum++;
                  if ( !trignum ) {
                     displaymessage ( "no trigger defined !", 1 );
                     return;
                  }
                  if ( ae->a.action == 255 ) {
                     displaymessage ( "no action defined !", 1 );
                     return;
                  }

              }
      case 2:   action = id; 
      break; 
      
      case 3:   { 
             nr = eventtypeselect(ae->a.action);
             if ((nr >= 0) && (nr <= ceventactionnum-1)) 
                ae->a.action = nr;
             else 
                return;

             activefontsettings.font = schriften.smallarial;
             switch (ae->a.action) {
                case cemessage:   {
                       freedata();
                       nr = ( word ) getid("Message ID",ae->a.saveas,0,maxint);
                       if (nr != 0) ae->a.saveas = nr;
                       else ae->a.action = 255;
                    } 
                break; 
                case cepalettechange: {
                    mousevisible(false);
                    ASCString filename;
                    fileselectsvga("*.pal", filename , true);
                    mousevisible(true); 
                    freedata();
                    if ( !filename.empty() ) {
                       ae->datasize = filename.length() + 1;
                       ae->chardata = new char [ ae->datasize ];
                       strcpy( ae->chardata, filename.c_str() );
                    } 
                    else {
                       ae->a.action = 255;
                       ae->datasize = 0;
                    }
                }
                break;
                case cerunscript: {
                    mousevisible(false); 
                    ASCString filename;
                    fileselectsvga("*.scr", filename , true);
                    mousevisible(true);
                    freedata();
                    if ( !filename.empty() ) {
                       ae->datasize = filename.length() + 1;
                       ae->chardata = new char [ ae->datasize ];
                       strcpy( ae->chardata, filename.c_str() );
                       nr = getid("Map ID",ae->a.saveas,0,maxint);
                       if (nr != 0) ae->a.saveas = nr;
                       else {
                          ae->a.action = 255;
                          freedata();
                       }
                    } 
                    else ae->a.action = 255;
                }
                break;
                case ceweatherchange: 
                case cemapchange:
                {
                        createpolygon(&poly,0,ae->a.action);

                        if (ae->a.action == ceweatherchange ) {
                           npush ( lastselectiontype );
                           npush ( auswahlw );

                           lastselectiontype = cselweather;

                           selweather( ct_invvalue );
                           redraw();

                           freedata();

                           ae->datasize = poly->vertexnum * sizeof ( poly->vertex[0] ) + 16;
                           puffer = new int [ ae->datasize / sizeof ( int ) ];
                           puffer[0]=auswahlw; //Wettertyp
                           puffer[1]=0;
                           puffer[2]=1;
                           puffer[3]=poly->vertexnum;
                           for (i=0;i<=poly->vertexnum-1;i++ ) {
                              puffer[4+i * 2]=poly->vertex[i].x;
                              puffer[5+i * 2]=poly->vertex[i].y;
                           } /* endfor */
                           ae->intdata = puffer;

                           npop ( auswahlw );
                           npop ( lastselectiontype );
                        } 
                        else {
                           npush ( lastselectiontype );
                           npush ( auswahl );

                           lastselectiontype = cselbodentyp;
                           selterraintype( ct_invvalue );
                           redraw();

                           ae->datasize = poly->vertexnum * sizeof ( poly->vertex[0] ) + 20;
                           puffer = new int [ ae->datasize / sizeof ( int ) ];
                           puffer[0]=auswahl->id;
                           puffer[1]=0; //direction
                           puffer[2]=1;
                           puffer[3]=1;
                           puffer[4]=poly->vertexnum;
                           for (i=0;i<=poly->vertexnum-1;i++ ) {
                              puffer[5+i * 2]=poly->vertex[i].x;
                              puffer[6+i * 2]=poly->vertex[i].y;
                           } /* endfor */
                           ae->intdata = puffer;

                           npop ( auswahl );
                           npop ( lastselectiontype );
                        } /* endif */
                        
                        activefontsettings.color = red;

                        redraw();
                }
                break;
                case cenewtechnology:
                case cenewtechnologyresearchable:   {
                         freedata();
                         nr = gettechnologyid(ae->a.saveas);
                         if (nr != 0) ae->a.saveas = nr;
                         else ae->a.action = 255;
                      } 
                break; 
             
                case ceeraseevent:   {

                       int eventid;
                       int mapid;
                       if ( ae->rawdata == NULL ) {
                          eventid = 0;
                          mapid = 0;
                       } else {
                          eventid = ae->intdata[0] ;
                          mapid = ae->intdata[1] ;
                       }

                       ae->a.saveas = 0;

                       geteventid( &eventid, &mapid );
                       if ( eventid != 0) { 
                          freedata();
                          ae->datasize = 8;
                          ae->intdata = new int ;
                          ae->intdata[0] = eventid;
                          ae->intdata[1] = mapid;
                       } else  {
                          ae->a.action = 255;
                          freedata();
                       }
                    }
             break; 
             case cenextmap:   {
                          freedata();
                          nr = getid("Map ID",ae->a.saveas,0,maxint); 
                          if (nr != 0) ae->a.saveas = nr;
                          else ae->a.action = 255;
                       } 
                break; 
             case cewindchange: {
                       if (changewind( ae ) != 0) ae->a.action = 255;
                       }
                break;
             case cegameparamchange: {
                   int oldval;
                   if ( !ae->rawdata ) 
                      oldval = 0;
                   else 
                      oldval = ae->intdata[0] ;
                   int nr = selectgameparameter( oldval );
                   if ( (nr >= 0) && ( nr < gameparameternum) ) {
                      int newval = getid("Parameter Val",gameparameterdefault[nr],minint,maxint); // !!!
                      freedata();
                      ae->intdata = new int[2];
                      ae->datasize = 2 * sizeof ( int );
                      ae->intdata[0] = nr;
                      ae->intdata[1] = newval;
                   } else ae->a.action = 255;
                }
                break;
             case ceellipse: {
                      if ( ae->intdata && ae->datasize == 6 * sizeof ( int )) {
                         ae->intdata[0] = getid("x1",ae->intdata[0],0,639); 
                         ae->intdata[1] = getid("y1",ae->intdata[1],0,479); 
                         ae->intdata[2] = getid("x2",ae->intdata[2],0,639); 
                         ae->intdata[3] = getid("y2",ae->intdata[3],0,479); 
                         ae->intdata[4] = choice_dlg("x orientation","~l~eft","~r~ight") - 1;
                         ae->intdata[5] = choice_dlg("y orientation","~t~op","~b~ottom") - 1;
                      } else {
                         freedata();
                         ae->intdata = new int[6];
                         ae->datasize = 6 * sizeof ( int );
                         ae->intdata[0] = getid("x1",0,0,639); 
                         ae->intdata[1] = getid("y1",0,0,479); 
                         ae->intdata[2] = getid("x2",0,0,639); 
                         ae->intdata[3] = getid("y2",0,0,479); 
                         ae->intdata[4] = choice_dlg("x orientation","~l~eft","~r~ight") - 1;
                         ae->intdata[5] = choice_dlg("y orientation","~t~op","~b~ottom") - 1;
                      }
                }
                break;
             case cechangebuildingdamage: {
                      if ( !ae->intdata || ae->datasize != 3 * sizeof ( int )) {
                         freedata();
                         ae->intdata = new int[3];
                         ae->datasize = 3 * sizeof ( int );
                         ae->intdata[0] = 0;
                         ae->intdata[1] = 0;
                         ae->intdata[2] = 0;
                      }
                      getxy_building(&ae->intdata[0],&ae->intdata[1]); 
                      ae->intdata[2] = getid("damage",ae->intdata[2],0,100); 
                }
                break;
             } 
             activefontsettings.font = schriften.smallarial;
             mousevisible(false); 
             showeventtype();
             showeventid();
             mousevisible(true);
          }  
      break; 
      
      case 4:   { 
             nr = playerselect(ae->player); 
             if ((nr >= 0) && (nr <= 8)) ae->player = nr; 
             mousevisible(false); 
             showplayer();
             mousevisible(true);
          } 
      break;                 
      
      case 5:   
      case 6:   
      case 7:   
      case 8:   { 
                  nid = id - 5; 
                  rnr = getreason(ae->trigger[nid]);
                  switch (rnr) {
                     case ceventt_turn:   {
                            abb = gettm( ae->trigger_data[nid]->time );
                            if (abb == false)
                               return;
                            ae->trigger[nid] = rnr; 
                         } 
                         break;
                     
                     case ceventt_buildingconquered:
                     case ceventt_buildinglost:
                     case ceventt_building_seen:
                     case ceventt_buildingdestroyed:   {
                                int cnt = 0;
                                for ( int pp = 0; pp < 9; pp++ )
                                   if ( !actmap->player[pp].buildingList.empty() )
                                      cnt++;
                                if ( cnt ) {
                                   if ( ae->trigger_data[nid] && ae->trigger_data[nid]->building ) {
                                      MapCoordinate mc = ae->trigger_data[nid]->building->getEntry();
                                      x = mc.x;
                                      y = mc.y;
                                   } else {
                                      x = 0;
                                      y = 0;
                                   }
                                   do { 
                                       getxy_building(&x,&y); 
                                       if ((x != 50000)) { 
                                          pf2 = getfield(x,y);
                                          abb = false;
                                          if (pf2 != NULL) if (pf2->building != NULL) abb = true;
                                       }
                                   }  while (!((x == 50000) || abb)); 
                                   redraw();
                                   if ((x != 50000) && abb) {
                                      if ( !ae->trigger_data[nid] )
                                         ae->trigger_data[nid] = new tevent::LargeTriggerData;
                                      ae->trigger_data[nid]->building = pf2->building; 
                                      ae->trigger[nid] = rnr; 
                                   } 
                                   else
                                      ae->trigger[nid] = 0;
                                } else {
                                   displaymessage("no buildings on map !", 1 );
                                   ae->trigger[nid] = 0;
                                }
                             } 
                             break;
                     
                     case ceventt_technologyresearched:   {
                            nr = gettechnologyid( ae->trigger_data[nid]->id );
                            if (nr != 0) { 
                               ae->trigger_data[nid]->id = nr; 
                               ae->trigger[nid] = rnr; 
                            } 
                            else ae->a.action = 255;
                         } 
                         break;
                     
                     case ceventt_event:   {
                            int mapid = 0;
                            int eventid = ae->trigger_data[nid]->id;
                            geteventid( &eventid, &mapid );
                            if ( eventid != 0) { 
                               ae->trigger_data[nid]->id = eventid; 
                               ae->trigger_data[nid]->mapid = mapid; 
                               ae->trigger[nid] = rnr; 
                            } 
                            else 
                               ae->a.action = 255;
                         } 
                         break;
                     
                     case ceventt_unitlost:
                     case ceventt_unitconquered:
                     case ceventt_unitdestroyed:   {
                                 ae->trigger_data[nid]->networkid = selectunit ( ae->trigger_data[nid]->networkid );
                                 if ( ae->trigger_data[nid]->networkid )
                                    ae->trigger[nid] = rnr;
                                 else
                                    ae->trigger[nid] = 0;
                                 redraw();
                             } 
                             break;
                  case ceventt_energytribute :
                  case ceventt_materialtribute :
                  case ceventt_fueltribute : displaymessage(" event not supported at the moment !", 1 );
                     /*{
                        nr = playerselect( ae->trigger_data[nid]->time.a.move );
                        if ((nr >= 0) && (nr <= 7)) {
                           ae->trigger[nid] = rnr; 
                           ae->trigger_data[nid]->time.a.move = nr;
                           nr = getid("Tribute",ae->trigger_data[nid]->time.a.turn,0,maxint);
                           if (nr == 0 ) ae->trigger[nid] = 0;
                           ae->trigger_data[nid]->time.a.turn = nr;
                        }
                        else ae->trigger[nid] = 0; 
                     }
                     */
                     break;
                  case 0:
                  case ceventt_allenemyunitsdestroyed:
                  case ceventt_allunitslost:
                  case ceventt_allenemybuildingsdestroyed:
                  case ceventt_allbuildingslost: {
                        ae->trigger[nid] = rnr;
                        playerselall(&ae->trigger_data[nid]->id);
                     }
                     break;
                  case ceventt_specific_unit_enters_polygon:
                                 etpe.vehiclenetworkid = selectunit( ae->trigger_data[nid]->networkid );
                                 redraw();
                                 if ( !etpe.vehiclenetworkid )
                                    break;

                  case ceventt_any_unit_enters_polygon:
                                 if ( !etpe.vehiclenetworkid ) {
                                    int col = 0;
                                    playerselall ( &col );
                                    etpe.color = (col >> 2) & 255;
                                 }

                                 createpolygon(&poly,0,ae->a.action);
                                 redraw();
                                 if ( poly && poly->vertexnum ) {
                                    int datasize = 2 + 2*poly->vertexnum;
                                    etpe.size = sizeof ( etpe ) + datasize * sizeof ( int );
                                    etpe.data = new int[ datasize ];
                                    etpe.data[0] = 1;    // one polygon
                                    etpe.data[1] = poly->vertexnum;
                                    for ( i=0; i < poly->vertexnum; i++ ) {
                                       etpe.data[2 + i * 2] = poly->vertex[i].x;
                                       etpe.data[3 + i * 2] = poly->vertex[i].y;
                                    } /* endfor */
                                    ae->trigger[nid] = rnr; 
                                    if ( ae->trigger_data[nid]->unitpolygon )
                                       delete ae->trigger_data[nid]->unitpolygon;
                                    ae->trigger_data[nid]->unitpolygon = new tevent::LargeTriggerData::PolygonEntered ( etpe );
                                 } else 
                                    ae->trigger[nid] = 0; 
   

                     break;
                  } 
                  mousevisible(false); 
                  if ((rnr < 0) && (rnr > ceventtriggernum -1 )) return;

                  showreason(nid);
                  mousevisible(true); 
               } 
      break; 
      
      case 9:   
      case 10:   
      case 11:   
      case 12:    
      case 13:   {
                    nr = verknuepfselect();
                    if ((nr >= 0) && (nr <= 7)) {
                       mousevisible(false);
                       if ( nr == 7 ) ae->triggerconnect[id-9] = 0;
                       else if  ( ( id-9 == 0 ) && ( nr <= 2  ) ) ;
                          else {
                             if ( nr < 2 ) ae->triggerconnect[id-9] = ae->triggerconnect[id-9] & clear_and_or; //* And/Or l”schen
                             ae->triggerconnect[id-9] = ae->triggerconnect[id-9] | (1 << nr );
                          }
                       showreason(id-9);
                       mousevisible(true);
                    }
            } 
   break; 
   case 14: showeventid();
   break;
   } 
} 


char      createevent(pevent ae, pevent orgevent )  /* True=Erfolgreich ausgefhrt */
{ tcreateevent ce; 
  int i;
  char b;

   ce.ae = ae;
   ce.orgevent = orgevent;
   ce.init();
   ce.run();
   ce.done();
   b = false;
   for (i=0;i<=3 ;i++ ) {
      if (ce.ae->trigger[i] != 0) b = true;
   } /* endfor */
   if (ce.ae->a.action == 255) b = false;
   if ((ce.taste == ct_esc) || (ce.action == 2) || ( ce.action == 255 ) || (b == false ) ) return false;
   else return true;
}

// õS EventSel

class   teventsel : public tstringselect {
           public :        
                 pevent pe,ae;
                 int oi;
                 void rebuildlines(void);
                 void setup(void);
                 virtual void buttonpressed(int id);
                 virtual void gettext(word nr);
                 virtual void run(void);
                 };


void         teventsel::setup(void)
{
  pevent       e;

   title = "Events";
   numberoflines = 0;
   e = actmap->firsteventtocome;
   while (e != NULL) {
      numberoflines++;
      e = e->next;
   }
   numberoflines++;
   pe = NULL;
   xsize = 570;
   ysize = 360;
   x1 = 35;
   y1 = 60;

   sy = 120;
   ey = ysize - 20;
   addbutton("~N~ew",20,40,90,70,0,1,4,true);
   addbutton("~E~dit",110,40,180,70,0,1,5,true);
   addbutton("~C~hange",200,40,260,70,0,1,6,true);
   addbutton("~D~elete",280,40,340,70,0,1,7,true);
   addbutton("E~x~it",360,40,420,70,0,1,8,true);
}

void   teventsel::gettext(word nr) //gibt in txt den string zurck
{
   word    count;
   pevent  e;

   count=0;
   if (nr+1 == numberoflines ) {
      strcpy( txt, "Empty" );
      return;
   }
   e = actmap->firsteventtocome;
   while ((e != NULL) && (count<nr)) {
      count++;
      e = e->next; 
   }      
   if (e != NULL) {
      strcpy(txt,ceventactions[e->a.action]);
      strcat(txt,"  ");
      strcat(txt,e->description);
   }
}


void         teventsel::buttonpressed(int         id)
{    pevent       e, ne;
     int         i; 
     char      b; 
//     char         s[200];
     tevent         *en, *pen;

   tstringselect::buttonpressed(id);
   switch (id) {
      
      case 4:   { 
             ae = new tevent ;
             b = createevent(ae, NULL);
             if (!b) {
                delete ae;
                return;
             }
             i = 0;
             e = actmap->firsteventtocome;
             ne = NULL;
             while (i != redline) {
                ne = e; 
                if (e->next != NULL) e = e->next; 
                i++;
             } 
             numberoflines++; 
             if ((numberoflines > lnshown) && (scrollbarvisible == false)) { 
                scrollbar_on();
                enablebutton(1);                                   
                enablebutton(2); 
                enablebutton(3); 
             } 

             if (ne != NULL) { 
                   ne->next = ae;
                   ae->next = e;
             } 
             else {
                   ae->next = actmap->firsteventtocome; 
                   actmap->firsteventtocome=ae;
                }
             //ae->rawdata = NULL;
             //ae->datasize = 0;
             viewtext();
          } 
      break; 
      case 5:   { 
             int i = 0; 
             pevent e = actmap->firsteventtocome; 
             while (i != redline) { 
                if ( e->next ) 
                   e = e->next; 
                i++; 
             } 
             if (i == numberoflines-1 ) 
               return;

             ae = new tevent ( *e );
             int b = createevent(ae, e);
             if (b && (ae->a.action != 255)) {
                if ( i == 0 )
                   actmap->firsteventtocome = ae;
                else {
                  pevent e = actmap->firsteventtocome; 
                  for ( int j = 0; j < i-1; j++ )
                     e = e->next;
                  delete e->next;
                  e->next = ae;
                }
             } else
                delete ae;

             viewtext();
          }  
      break; 
      
      case 6:   { 
             i = 0;
             e = actmap->firsteventtocome; 
             while (i != redline) { 
                if (e->next != NULL) e = e->next; 
                i++;  
             } 
             if (i == numberoflines-1 ) return;
             if (pe == NULL) {
                pe = e;
                mousevisible(false); 
                activefontsettings.length = 300;
                showtext2( (char *) ceventactions[pe->a.action],x1 + 65,y1 + 90);
                mousevisible(true); 
             } 
             else { 
                ne = new ( tevent ); 
                en = e->next;
                pen = pe->next;  
                memcpy(ne,e,sizeof( tevent ));
                memcpy(e,pe,sizeof( tevent ));
                memcpy(pe,ne,sizeof( tevent ));
                e->next = en; 
                pe->next = pen;
                asc_free(ne);
                pe = NULL; 
                viewtext(); 
                activefontsettings.length = 300;
                mousevisible(false); 
                showtext2("                                                   ",x1 + 65,y1 + 90);
                mousevisible(true); 
             } 
          } 
      break; 
      
      case 7:   { 
             i = 0; 
             e = actmap->firsteventtocome; 
             ne = e;
             while (i != redline) { 
                ne = e; 
                if (e->next != NULL) e = e->next; 
                i++; 
             } 
             if (i == numberoflines-1 ) return;
             if (ne == e) {
                actmap->firsteventtocome = e->next; 
             } 
             else {
                ne->next = e->next; 
                asc_free(e);
             }  
             numberoflines--;
             resettextfield(); 
             viewtext(); 
          } 
      break; 
      
      case 8:   action = 255; 
   break; 
   } 
} 


void         teventsel::run(void)
{ 
   mousevisible(false); 
   showtext2("Puffer :",x1 + 10,y1 + 90);
   mousevisible(true); 
   do { 
      tstringselect::run();
      switch (taste) {
      case ct_up : {
 
           }
         break;
      case ct_down : {
         }
         break;
      default:
        break;
      } /* endswitch */
   }  while (!((taste == ct_esc) || (action == 255))); 
} 


void         event(void)
{ teventsel    te;

   cursor.hide();
   te.init();
   te.run(); 
   te.done();
   ch = 0;
   cursor.show();
} 




