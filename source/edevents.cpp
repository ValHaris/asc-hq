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
#include "edmisc.h";                      
#include "edevents.h"
#include "edselfnt.h"

// õS TStringselect

void         tstringselect::init(void)
{ 
   tdialogbox::init();
   xsize = 370;
   ysize = 320; 
   x1 = ( 640 -xsize ) / 2;
   y1 = ( 480 -ysize ) / 2;
   sy = 45;
   
   sx = 20;
   title = "Text-Box";
   windowstyle = windowstyle ^ dlg_in3d; 
   lnshown = 10;
   numberoflines = 0; 
   firstvisibleline = 0; 
   redline = 0; 
   startpos = 0;
   ey = ysize - 50;
   ex = xsize - 30;
   setup(); 

   if (startpos >= numberoflines ) startpos = numberoflines-1;
   if (startpos > lnshown -1 ) {
      firstvisibleline = startpos - ( lnshown - 1 );
      redline = startpos;
   } else {
      redline = startpos;
   } /* endif */

   dk = 0; 
   action = 0; 
   dx = (ey - sy) / lnshown;
   if (numberoflines > lnshown) { 
      scrollbarvisible = true; 
      addscrollbar(ex + 10 ,sy ,ex + 20,ey ,&numberoflines,lnshown,&firstvisibleline,1,0);
   } 
   else scrollbarvisible = false; 
   buildgraphics(); 
   rahmen(true,x1 + sx , y1 + sy,x1 + ex,y1 + ey);
   mousevisible(true); 
   activefontsettings.font = schriften.smallarial; 
   activefontsettings.color = black; 
   activefontsettings.justify = lefttext; 
   activefontsettings.background = lightgray;
   activefontsettings.height = 15;
   viewtext(); 
} 


void         tstringselect::setup(void)
{ 
} 


void         tstringselect::buttonpressed(byte         id)
{ 
   tdialogbox::buttonpressed(id);
   if (id == 1) { 
      if (firstvisibleline > redline) redline = firstvisibleline; 
      if (firstvisibleline + lnshown - 1 < redline) redline = firstvisibleline + lnshown - 1; 
      viewtext(); 
   } 
} 


void         tstringselect::run(void)
{ 
  boolean      view; 
  integer      my; 
  byte         ms;

   tdialogbox::run();
   if (numberoflines > 0) { 
      msel = 0; 
      if (getmousestatus() == 2) { 
         if ((( ms = mouseparams.taste ) == 0) && (dk == 1)) dk = 2;
         if ((mouseparams.x > x1 + 10) & (mouseparams.x < x1 + xsize - 40) && (ms != 0)) {
            my = mouseparams.y - y1 - sy; 
            my = my / dx;
            if ((my >= 0) && (my <= lnshown - 1) && (my <= numberoflines - 1)) { 
               mouseselect = firstvisibleline + my; 
               if ((mouseselect == redline) && (dk == 2)) { 
                  msel = ms;
                  dk = 0; 
               } 
               else { 
                  redline = mouseselect; 
                  dk = 1; 
                  ms =0;
                  viewtext(); 
               } 
            } 
         } 
      } 
      switch (taste) {
         
         case ct_up:   { 
                   view = true; 
                   if (redline > 0) redline--; 
                   else view = false; 
                   if ((redline < firstvisibleline) && (firstvisibleline > 0)) {
                      firstvisibleline--;
                      showbutton( 1 );
                   }
                   if (view) viewtext(); 
                } 
         break; 
         case ct_pos1:   {
                   view = false; 
                   if  ( (redline > 0) || (firstvisibleline > 0) ) {
                      view = true;
                      redline = 0;
                      firstvisibleline = 0;
                   }
                   if (view) viewtext(); 
                } 
         break; 
   
         case ct_ende:   {
                   view = false; 
                   if (redline < numberoflines -1 ) {
                      view = true;
                      redline = numberoflines -1 ;
                      firstvisibleline = numberoflines - lnshown;
                   }
                   if (view) viewtext(); 
                } 
         break; 
   
         case ct_down:   { 
                     view = true; 
                     if (redline < numberoflines - 1) redline++; 
                     else view = false; 
                     if ((redline > firstvisibleline + lnshown - 1) && (firstvisibleline + lnshown - 1 <= numberoflines)) {
                        firstvisibleline++;
                        showbutton( 1 );
                     }
                     if (view) viewtext(); 
                  } 
      break; 
      } 
   }
   else redline = 255;
} 


void         tstringselect::resettextfield(void)
{ 
   bar(x1 + sx,y1 + sy,x1 + ex,y1 + ey,lightgray);
   rahmen(true,x1 + sx ,y1 + sy,x1 + ex,y1 + ey);
} 

void   tstringselect::gettext(word nr) //gibt in txt den string zurck
{
  strcpy(txt,"");
  nr = 0;
}

void tstringselect::scrollbar_on(void)
{
   scrollbarvisible = true; 
   addscrollbar(ex + 10 ,sy - 10,ex + 30,ey + 10,&numberoflines,lnshown,&firstvisibleline,1,0);
}


void         tstringselect::viewtext(void)
{ 
  char         s1[200];
  word         yp; 
  integer      l; 

   mousevisible(false); 
   //showbutton(1);
   npush(activefontsettings.length);
   activefontsettings.length = ex - sx - 10;
   yp = y1 + sy + 5;
   l = firstvisibleline; 
   if (numberoflines > 0) { 
      { 
         while ((l<numberoflines) && (l-firstvisibleline < lnshown)) {
            gettext(l);
            strcpy(s1,txt);
            if (l == redline ) activefontsettings.color=red;
            else activefontsettings.color=lightblue;
            showtext2(s1,x1+ sx + 5,yp+( l-firstvisibleline ) * dx );
            l++;
         } /* endwhile */
         }
   } 
   else showtext2("No text available !",x1 + 50,yp + 50);
  
   //rahmen(true,x1  + sx ,y1 + sy,x1  + ex ,y1 + ey );
   npop(activefontsettings.length);
   mousevisible(true); 
} 


void         tstringselect::done(void)
{ 
   tdialogbox::done();
} 



// õS TplayerSel


class   tplayersel : public tstringselect {
           public :
                 int lastchoice;
                 virtual void setup(void);
                 virtual void buttonpressed(byte id);
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


void         tplayersel::buttonpressed(byte         id)
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


byte         playerselect(int lc)
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
                 virtual void buttonpressed(byte id);
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


void         verknuepfsel::buttonpressed(byte         id)
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


byte         verknuepfselect(void)
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
              byte action;
              byte bkgcolor;
              int playerbit;
              void init(void);
              virtual void run(void);
              virtual void buttonpressed(byte id);
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

   for (int i=0;i<8 ;i++ ) {
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

   for (i=0;i<8 ;i++ ) bar(x1 + 170,y1 + 60 + i*30 ,x1 + 190 ,y1 + 70 + i * 30,20 + ( i << 3 ));

   anzeige();
   
   mousevisible(true); 
} 

void         tplayerselall::anzeige(void)
{
   mousevisible(false); 
   for (int i=0;i<8 ;i++ ) {
      if ( ( playerbit & ( 1 <<  ( i+2 ) ) ) > 0 ) rectangle (x1 + 16,y1+51+i*30,x1+154,y1+79+i*30, 20 );
      else rectangle (x1 + 16,y1+51+i*30,x1+154,y1+79+i*30, bkgcolor );
   }
   mousevisible(true); 
}


void         tplayerselall::run(void)
{ 

   do { 
      tdialogbox::run(); 
      if (taste == ct_f1) help ( 1060 );
   }  while (!((taste == ct_esc) || ((action == 1) || (action ==2)))); 
   if ((action == 1) || (taste == ct_esc)) playerbit = 1;
   else {
      playerbit = playerbit | 2; //bit 1 setzen;
      playerbit = playerbit &  (65535 ^  1); // bit 0 l”schen;
   }
} 


void         tplayerselall::buttonpressed(byte         id)
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
         if ( ( playerbit & ( 1 << ( id-4 ) ) ) > 0 ) {
            playerbit = playerbit &  (65535 ^ ( 1 << ( id-4 ) ) ) ; // bit x l”schen ^ = XOR
         } else {
            playerbit = playerbit | ( 1 << ( id-4 ) );
         } /* endif */
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
                 virtual void buttonpressed(byte id);
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


void         teventtype::buttonpressed(byte         id)
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


byte         eventtypeselect(int lc)
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
                 byte lastchoice;
                 virtual void setup(void);
                 virtual void buttonpressed(byte id);
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


void         treason::buttonpressed(byte         id)
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


byte         getreason(byte lc)
{ 
  treason      sm; 

   sm.lastchoice = lc;
   sm.init(); 
   sm.run(); 
   sm.done(); 
   return sm.redline; 
} 

// õS GetID

class  tgetid : public tdialogbox {
          public :
              byte action;
              int mid;
              char nt[200];
              void init(void);
              int max,min;
              virtual void run(void);
              virtual void buttonpressed(byte id);
              };

void         tgetid::init(void)
{ 
   tdialogbox::init(); 
   title = nt; 
   x1 = 200; 
   xsize = 220; 
   y1 = 150; 
   ysize = 140; 
   action = 0; 

   if ((mid < min) || (mid > max)) mid = 10;   /* ! */ 

   windowstyle = windowstyle ^ dlg_in3d; 


   addbutton("~D~one",20,ysize - 40,100,ysize - 20,0,1,1,true); 
   addkey(1,ct_enter); 
   addbutton("~C~ancel",120,ysize - 40,200,ysize - 20,0,1,2,true); 
   addbutton("~I~D",20,60,xsize - 20,80,2,1,3,true); 
   addeingabe(3,&mid,min,max);

   buildgraphics(); 

   mousevisible(true); 
} 


void         tgetid::run(void)
{ 
   pbutton pb = firstbutton;
   while ( pb &&  (pb->id != 3)) 
      pb = pb->next;

   if ( pb )
      if ( pb->id == 3 )
         execbutton( pb , false );

   do { 
      tdialogbox::run(); 
   }  while (!((taste == ct_esc) || ((action == 1) || (action == 2)))); 
   if ((action == 2) || (taste == ct_esc)) mid = 0;
} 


void         tgetid::buttonpressed(byte         id)
{ 
   tdialogbox::buttonpressed(id); 
   switch (id) {
      
      case 1:   
      case 2:   action = id; 
   break; 
   } 
} 


int      getid(char        tit[200],int lval,int min,int max)

{ tgetid     gi;

   strcpy(gi.nt,tit);
   gi.max = max;
   gi.min = min;
   gi.mid = lval;
   gi.init();
   gi.run();
   gi.done();
   return gi.mid;
} 

// õS GetXYSel


class  tgetxy : public tdialogbox {
          public :
              byte action;
              int x,y;
              void init(void);
              virtual void run(void);
              virtual byte condition(void);
              virtual void buttonpressed(byte id);
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

byte         tgetxy::condition(void)
{
   return 1;
}

void         tgetxy::buttonpressed(byte         id)
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

// õS GetXYUnitSel

class  tgetxyunit : public tgetxy {
          public :
              virtual byte condition(void);
              };


byte         tgetxyunit::condition(void)
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
              virtual byte condition(void);
              };


// õS GetXYBuildingSel

byte         tgetxybuilding::condition(void)
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
              byte action;
              int x,y;
              void init(void);
              virtual void run(void);
              virtual void buttonpressed(byte id);
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


void         tgettm::buttonpressed(byte         id)
{ 
   tdialogbox::buttonpressed(id); 
   switch (id) {
      
      case 1:   
      case 2:   action = id; 
   break; 
   } 
} 


boolean      gettm(signed short *       x, signed short *       y)  /* True : Erfolgreich ausgefhrt */
{ tgettm       ce; 
  boolean b;

   ce.x = *x; 
   ce.y = *y; 
   ce.init(); 
   ce.run(); 
   b = false;
   if (ce.x != 50000) { 
      *x = ce.x; 
      *y = ce.y; 
      b = true;
   } 
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
                 virtual void buttonpressed(byte id);
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


void         tgeteventid::buttonpressed(byte         id)
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
      switch (taste) {
      } /* endswitch */
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
                 virtual void buttonpressed(byte id);
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
                        

void         tgettechnologyid::buttonpressed(byte         id)
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
   int count = 0;

   do { 
      tstringselect::run();
      if ( (msel == 1 ) || (taste == ct_enter ) ) {
         ttechnologyid = gettechnology_forpos(redline)->id;
         showbutton(5);
      }                            
      switch (taste) {
      } /* endswitch */
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
                byte         action,tsintensity,abort,activeheight;
                int            tdirect[3],tinten[3];
                pevent      we;
                void          init(void);
                void          showdir_and_height(void);
                virtual void        run(void);
                virtual void        buttonpressed(byte         id);
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
                              
      int x = dirx + radius * sin ( 2 * pi * (float) i / (float) sidenum );
      int y = diry - radius * cos ( 2 * pi * (float) i / (float) sidenum );

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


void         twindchange::buttonpressed(byte         id)
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


byte         changewind(pevent acte)
{ 
   twindchange        wc;

   wc.we = acte;
   wc.init();
   wc.run();
   wc.done();
   return wc.abort;
} 



// õS SelectGameParameter

class   tgameparamsel : public tstringselect {
           public :
                 int lastchoice;
                 virtual void setup(void);
                 virtual void buttonpressed(byte id);
                 virtual void run(void);
                 virtual void gettext(word nr);
                 };

void         tgameparamsel ::setup(void)
{ 

   action = 0;
   title = "Select Gameparameter";
   numberoflines = gameparameternum;
   ey = ysize - 60; 
   startpos = lastchoice;
   addbutton("~D~one",20,ysize - 40,170,ysize - 20,0,1,2,true); 
   addkey(2,ct_enter); 
   addbutton("~C~ancel",190,ysize - 40,340,ysize - 20,0,1,3,true); 
} 


void         tgameparamsel ::buttonpressed(byte         id)
{ 
   tstringselect::buttonpressed(id);
   switch (id) {
      
      case 2:   
      case 3:   action = id; 
   break; 
   } 
} 


void         tgameparamsel ::gettext(word nr)
{ 
   strcpy(txt,gameparametername[nr] );
} 


void         tgameparamsel ::run(void)
{ 
   do { 
      tstringselect::run(); 
   }  while ( ! ( (taste == ct_esc) || ( (action == 2) || (action == 3) ) || (msel == 1)) );
   if ( (action == 3) || (taste == ct_esc) ) redline = 255;
} 


byte selectgameparameter( int lc )
{ 
  tgameparamsel  gps;

   gps.lastchoice = lc;
   gps.init();
   gps.run();
   gps.done();
   return gps.redline; 
} 


// õS CreateEvent

class  tcreateevent : public tdialogbox {
          public :
              pevent ae, orgevent;
              int reasonst;
              byte action;
              boolean build;
              void init(void);
              virtual void run(void);
              virtual void buttonpressed(byte id);
              virtual void showreason(byte nr);
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

void tcreateevent::showreason(byte nr)
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
                    if ( building->name )
                       displaymessage ( building->name, 1 );
                    else
                       displaymessage ( building->typ->name, 1 );

               }
               if ( ae->trigger[i] == ceventt_unitconquered  ||
                    ae->trigger[i] == ceventt_unitlost  ||
                    ae->trigger[i] == ceventt_specific_unit_enters_polygon  ||
                    ae->trigger[i] == ceventt_unitdestroyed ) {
                    pvehicle vehicle;
                    if ( ae->trigger[i] == ceventt_specific_unit_enters_polygon )
                       vehicle = ae->trigger_data[i]->unitpolygon->vehicle;
                    else
                       vehicle = ae->trigger_data[i]->vehicle;

                    if ( vehicle->name )
                       displaymessage ( vehicle->name, 1 );
                    else
                       if ( vehicle->typ->name )
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
       if ( actmap->player[pp].firstvehicle )
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
             pf = getfield(x,y);
             abb = false;
             if ( pf && pf->vehicle ) 
                abb = true;
          }
       }  while (!((x == 50000) || abb)); 
       if ((x != 50000) && abb) 
          return pf->vehicle;
       else 
          return NULL;
    } else {
       displaymessage("no vehicles on map !", 1 );
       return NULL;
    }
}


// There is a memory leak at the unit_enters_polygon trigger; but it is not large and should be fixed by a general rewrite of the trigger functions
// I just want to get this trigger working....


void         tcreateevent::buttonpressed(byte         id)
{   int           nid, nr, rnr;
    pfield        pf; 
    boolean    abb; 
    char         s[200];
    word        i;
    int           *puffer= NULL;
    int           x,y;
    ppolygon  poly = NULL;

    teventtrigger_polygonentered etpe;


   const byte         stringpriority[7]     = {32, 64, 1, 2, 4, 8, 16};
   const byte         stringprioritynumber[7]     = {5, 6, 0, 1, 2, 3, 4};
 
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
                    fileselectsvga("*.pal", s , 1);
                    mousevisible(true); 
                    freedata();
                    if (s[0] != 0 ) {
                       ae->datasize = strlen (s) +1;
                       ae->chardata = new char [ ae->datasize ];
                       strcpy( ae->chardata, s );
                    } 
                    else {
                       ae->a.action = 255;
                       ae->datasize = 0;
                    }
                }
                break;
                case cerunscript: {
                    mousevisible(false); 
                    fileselectsvga("*.scr", s , 1);
                    mousevisible(true); 
                    freedata();
                    if (s[0] != 0 ) {
                       ae->datasize = strlen(s)+1;
                       ae->chardata = new char [ ae->datasize ];
                       strcpy( ae->chardata, s );
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
                      switch (nr) {
                         case cgp_fahrspur:
                         case cgp_eis: {
                               int newval = getid("Parameter Val",gameparameterdefault[nr],0,255); // !!!
                               freedata();
                               ae->intdata = new int[2];
                               ae->datasize = 2 * sizeof ( int );
                               ae->intdata[0] = nr;
                               ae->intdata[1] = newval;
                            }
                            break;
                      } /* endswitch */
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
                            abb = gettm(&ae->trigger_data[nid]->time.a.turn,&ae->trigger_data[nid]->time.a.move);
                            if (abb == false) return;
                            ae->trigger[nid] = rnr; 
                         } 
                         break;
                     
                     case ceventt_buildingconquered:
                     case ceventt_buildinglost:
                     case ceventt_building_seen:
                     case ceventt_buildingdestroyed:   {
                                int cnt = 0;
                                for ( int pp = 0; pp < 9; pp++ )
                                   if ( actmap->player[pp].firstbuilding )
                                      cnt++;
                                if ( cnt ) {
                                   if ( ae->trigger_data[nid]->building ) {
                                      x = ae->trigger_data[nid]->building->xpos;
                                      y = ae->trigger_data[nid]->building->ypos;
                                   } else {
                                      x = 0;
                                      y = 0;
                                   }
                                   do { 
                                       getxy_building(&x,&y); 
                                       if ((x != 50000)) { 
                                          pf = getfield(x,y);
                                          abb = false;
                                          if (pf != NULL) if (pf->building != NULL) abb = true;
                                       }
                                   }  while (!((x == 50000) || abb)); 
                                   redraw();
                                   if ((x != 50000) && abb) { 
                                      ae->trigger_data[nid]->building = pf->building; 
                                      ae->trigger[nid] = rnr; 
                                   } 
                                   else ae->trigger[nid] = 0; 
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
                                 ae->trigger_data[nid]->vehicle = selectunit( ae->trigger_data[nid]->vehicle );
                                 if ( ae->trigger_data[nid]->vehicle )
                                    ae->trigger[nid] = rnr;
                                 else
                                    ae->trigger[nid] = 0;
                                 redraw();
                             } 
                             break;
                  case ceventt_energytribute :
                  case ceventt_materialtribute :
                  case ceventt_fueltribute : {
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
                                 etpe.vehicle = selectunit( ae->trigger_data[nid]->vehicle );
                                 redraw();
                                 if ( !etpe.vehicle )
                                    break;

                  case ceventt_any_unit_enters_polygon:
                                 if ( !etpe.vehicle ) {
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
                                    ae->trigger_data[nid]->unitpolygon = new teventtrigger_polygonentered ( etpe );
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


boolean      createevent(pevent ae, pevent orgevent )  /* True=Erfolgreich ausgefhrt */
{ tcreateevent ce; 
  int i;
  boolean b;

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
                 byte oi;
                 void rebuildlines(void);
                 virtual void setup(void);
                 virtual void buttonpressed(byte id);
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
   xsize = 440; 
   ysize = 360; 
   x1 = 100;
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


void         teventsel::buttonpressed(byte         id)
{    pevent       e, ne;
     byte         i; 
     boolean      b; 
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


void setmapparameters ( void )
{
   int param;
   do {
      param = selectgameparameter( -1 );
      if ( param >= 0 && param < gameparameternum ) 
         actmap->gameparameter[ param ] = getid("Parameter Val",actmap->gameparameter[ param ],0,maxint);
   } while ( param >= 0 && param < gameparameternum );      
}

