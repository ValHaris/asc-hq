/*! \file edevents.cpp
    \brief The event editing in the mapeditor
*/

//     $Id: edevents.cpp,v 1.42 2004-07-12 18:15:04 mbickel Exp $
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
#include "gameevent_dialogs.h"

// ıS TplayerSel


class   tplayersel : public tstringselect {
           public :
                 int lastchoice;
                 virtual void setup(void);
                 virtual void buttonpressed(int id);
                 virtual void run(void);
                 virtual void get_text(int nr);
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


void         tplayersel::get_text( int nr)
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


// ıS ReasonSel

class   treason : public tstringselect {
           public :
                 int lastchoice;
                 virtual void setup(void);
                 virtual void buttonpressed(int id);
                 virtual void run(void);
                 virtual void get_text( int nr);
                 };

void         treason::setup(void)
{

   action = 0;
   title = "Select Trigger";
   numberoflines = EventTriggerNum;
   ey = ysize - 60;
   startpos = lastchoice;
   addbutton("~D~one",20,ysize - 40,170,ysize - 20,0,1,4,true);
   addkey(4,ct_enter);
   addbutton("~C~ancel",190,ysize - 40,340,ysize - 20,0,1,5,true);
}


void         treason::get_text(int nr)
{
   strcpy(txt, EventTriggerName[nr]);
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
   if ((action == 5) || (taste == ct_esc))
      redline = -1;
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


// ıS GetTurn/MoveSel

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


bool gettm( GameTime& time  )  /* True : Erfolgreich ausgefÅhrt */
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


// ıS gettechnologyid
#if 0
class   tgettechnologyid : public tstringselect {
           public :
                 int lastid;
                 int ttechnologyid;
                 void rebuildlines(void);
                 virtual void setup(void);
                 virtual void buttonpressed(int id);
                 virtual void get_text(int nr);
                 virtual void run(void);
                 };


void         tgettechnologyid::setup(void)
{
   title = "Select Technology";
   numberoflines = technologynum;
   ttechnologyid = 0;
   for (int i=0;i< technologynum;i++) {
      if (technologyRepository.getObject_byPos(i)->id == lastid ) {
         startpos = i;
         ttechnologyid = technologyRepository.getObject_byPos(i)->id;
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

void   tgettechnologyid::get_text(int nr) //gibt in txt den string zurÅck
{
   strcpy(txt,technologyRepository.getObject_byPos(nr)->name.c_str());
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
         ttechnologyid = technologyRepository.getObject_byPos(redline)->id;
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

#endif

// ıS ChangeWind
#if 0
     class twindchange: public tdialogbox {
              public:
                int         heightxs,w2,dirx,diry;
                int         action,tsintensity,abort,activeheight;
                int            tdirect[3],tinten[3];
                pevent      we;
                void          init(void);
                void          showdir_and_height(void);
                virtual void        run(void);
                virtual void        buttonpressed(int         id);
        };


void         twindchange::init(void)
{ int         w;
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

#endif

// ıS SelectGameParameter



// ıS CreateEvent

class  tcreateevent : public tdialogbox {
          public :
              tcreateevent( Event* ev );
              virtual void run(void);
              virtual void buttonpressed(int id);
              virtual void showreason(int nr);
              virtual void showplayer(void);
              virtual void showeventtype(void);
              int action;
            protected:
              Event* event;
              int eventType;
              int triggerConnection;
              int getcapabilities ( void ) { return 1; };
              void redraw ( void );
              void draw_additional_graphics( void );
              char descriptionbuf[10000];
            private:
              int reasonx;
              int reasonst; // position in the dialog box
              int invertTrigger[4];
              };

tcreateevent :: tcreateevent( Event* ev )
              : event(ev)
{

   for ( int i = 0; i < 4; ++i )
      if ( event && event->trigger.size() > i && event->trigger[i] )
         invertTrigger[i] = event->trigger[i]->invert;
      else
         invertTrigger[i] = 0;

   if ( event->triggerConnection == Event::OR )
      triggerConnection = 1;
   else
      triggerConnection = 0;
      
   tdialogbox::init();
   title = "New Event";
   x1 = 10;
   xsize = 620;
   y1 = 10;
   ysize = 380;
   action = 0;

   windowstyle = windowstyle ^ dlg_in3d;

   addbutton("~D~one",20,ysize - 40,280,ysize - 20,0,1,1,true);
   addkey(1,ct_enter);
   addbutton("~C~ancel",300,ysize - 40,560,ysize - 20,0,1,2,true);

   if ( event && event->action)
      eventType = event->action->getActionID();
   else {
      eventType = 0;
      event->spawnAction( EventAction_Nothing );
   }

   addDropDown( 110, 40, xsize - 100, 60, 3, EventActionName, EventActionNum, &eventType );

   addbutton("setup",xsize-90, 40, xsize - 20, 60, 0,1,33,true);

   addbutton("De~s~cription",20,90,xsize - 200,110,1,1,20,true);
   strcpy ( descriptionbuf, event->description.c_str() );
   addeingabe(20,descriptionbuf,0,500);

   addbutton("~P~layer",xsize - 180,90,xsize - 120,110,0,1,4,true);

   reasonst = 140; //Start fÅr Eventreasonanzeige y
   reasonx = xsize - 230;

   addbutton("Trigger ~1~",reasonx,reasonst,reasonx+90,reasonst+20,0,1,5,true);
   addbutton("Trigger ~2~",reasonx,reasonst+40,reasonx+90,reasonst+60,0,1,6,true);
   addbutton("Trigger ~3~",reasonx,reasonst+80,reasonx+90,reasonst+100,0,1,7,true);
   addbutton("Trigger ~4~",reasonx,reasonst+120,reasonx+90,reasonst+140,0,1,8,true);

   for ( int i = 0; i < 4; ++i ) {
      addbutton ( "Invert", reasonx + 100,reasonst + i * 40,reasonx+180,reasonst+20 + i * 40, 3, 0, 10+i, true );
      addeingabe ( 10+i, &invertTrigger[i], 0, dblue );
   }

   addbutton("~T~urn offset", reasonx, reasonst + 175, reasonx + 90, reasonst + 195, 2, 1, 25, true );
   addeingabe( 25, &event->delayedexecution.turn, 0, 100000 );

   addbutton("~M~ove offset", reasonx+100, reasonst + 175, reasonx + 190, reasonst + 195, 2, 1, 26, true );
   addeingabe( 26, &event->delayedexecution.move, -1000, 100000 );

   addbutton("~R~earm Num", reasonx-100, reasonst + 175, reasonx - 10, reasonst + 195, 2, 1, 27, true );
   addeingabe( 27, &event->reArmNum, 0, 100000 );


   static const char* connectionNames[2] = {"all trigger must be triggered", "only one trigger must be triggered" };
   addDropDown( 20, reasonst + 175, reasonx - 110, reasonst+195, 9, connectionNames, 2, &triggerConnection );

   int verkx = xsize -120;

   buildgraphics();

   mousevisible(true);
}

void tcreateevent::draw_additional_graphics ( void )
{
   activefontsettings.font = schriften.smallarial;
   activefontsettings.justify = lefttext;
   activefontsettings.color = red;
   activefontsettings.markcolor = lightred;
   activefontsettings.background = lightgray;
   activefontsettings.length = 0;


   showtext2 ( "Action:", x1 + 40, y1 + 40 );

//   showtext2 ( "Connect Trigger using", x1 + 40, y1 + reasonst + 155 );

   showeventtype();
   showplayer();

   for ( int i=0 ;i < 4 ;i++ )
      showreason(i);
}


void tcreateevent::showplayer(void)
{
   ASCString s;

   activefontsettings.length = 84;
   activefontsettings.font = schriften.smallarial;
   bar(x1 + xsize - 110,y1 + 90,x1 + xsize- 20,y1 + 110,lightgray);
   rahmen(true,x1 + xsize - 110,y1 + 90,x1 + xsize- 20,y1 + 110);

   for ( int i = 0; i < 8; i++ )
      if ( event->playerBitmap & (1<<i))
         s += strrr(1+i);
   showtext2( s.c_str(),x1 + xsize -105,y1 + 92);
}

void tcreateevent::showeventtype(void)
{/*
   activefontsettings.length = 200;
   bar(x1 + 110,y1 + 40,x1 + xsize - 80 ,y1 + 60,lightgray);
   rahmen(true,x1 + 110,y1 + 40,x1 + xsize - 80 ,y1 + 60);
   if ( event->action )
      showtext2( EventActionName[event->action->getActionID()],x1 + 115,y1 + 42);
   else
      showtext2( "no action specified",x1 + 115,y1 + 42);
   */
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
   int showr = 50;
   int showv2 = 300;
   ASCString s2;

   bar(x1 + showv1,y1 + reasonst + nr * 40, x1 + xsize - 250,y1 + reasonst + 20 + nr * 40,lightgray);

   if ( event->trigger.size() > nr )
      s2 = event->trigger[nr]->getName();
   else
      s2 = "not defined yet";

   activefontsettings.length = showv2 - showr - 5;
   showtext2(s2, x1+showr+2,y1+reasonst + 2 + nr * 40);

   npop ( activefontsettings );
}


void tcreateevent::redraw ( void )
{
   tdialogbox::redraw();
   npush ( activefontsettings );
   draw_additional_graphics();
   npop ( activefontsettings );
}




void         tcreateevent::run(void)
{
   do {
      tdialogbox::run();
   }  while (!((taste == ct_esc) || ((action == 1) || (action == 2))));
}



void         tcreateevent::buttonpressed(int         id)
{
   switch (id) {

      case 1: {  // done button
                  if ( event->trigger.size() < 1 ) {
                     displaymessage ( "no trigger defined !", 1 );
                     return;
                  }
                  if ( !event->action ) {
                     displaymessage ( "no action defined !", 1 );
                     return;
                  }
                  if ( event->reArmNum && event->delayedexecution.turn < 1 ) {
                     displaymessage ( "without delayed execution, reArming the event makes no sense !", 1 );
                     return;
                  }
                  event->description = descriptionbuf;
              }
      case 2:  // cancel
              action = id;
      break;

      case 3:   { // event action
      /*
                   EventActionID eai = EventActionID ( eventType );
                   EventActionID eai_old;
                   if ( event->action )
                      eai = event->action->getActionID();
                   else
                      eai = EventAction_DisplayMessage;

                   int nr = eventtypeselect( int(eai) );
                   if ( nr < 0 )
                      return; // cancel pressed
                      */

                   EventActionID eai = EventActionID( eventType );


                   if ( !event->action )
                      event->spawnAction( eai );
                   else
                      if ( event->action->getActionID() != eai ) {
                         delete event->action;
                         event->action = NULL;
                         event->spawnAction( eai );
                      }

                   event->action->setup();
                   redraw();
                   showeventtype();
                 };
      break;
      case 33:  if ( event && event->action ) {
                   event->action->setup();
                   redraw();
                }
                break;

      case 4:   {
               playerselall( &event->playerBitmap );
               mousevisible(false);
               showplayer();
               mousevisible(true);
          }
      break;

      case 5:
      case 6:
      case 7:
      case 8:   {
                  int nid = id - 5;
                  int rnr;
                  if ( nid < event->trigger.size() )
                     rnr = getreason( event->trigger[nid]->getTriggerID() );
                  else
                     rnr = getreason( 0 );

                  if ( rnr >= 0 ) {
                     if ( event->trigger.size() <= nid ) {
                        event->trigger.resize(nid+1);
                        event->trigger[nid] = event->spawnTrigger( rnr );
                        event->trigger[nid]->setup();
                     } else {
                        if ( event->trigger[nid]->getTriggerID() == rnr )
                           event->trigger[nid]->setup();
                        else {
                           delete event->trigger[nid];
                           event->trigger[nid] = event->spawnTrigger( rnr );
                           event->trigger[nid]->setup();
                        }
                     }
                     redraw();
                  }
                  showreason(nid);
               }
      break;
      case 9: if ( event )
                 if ( triggerConnection )
                    event->triggerConnection = Event::OR;
                 else
                    event->triggerConnection = Event::AND;
               break;
      case 10:
      case 11:
      case 12:
      case 13: if ( event && event->trigger.size() > id - 10 && event->trigger[id-10] )
                  event->trigger[id-10]->invert = invertTrigger[id-10];
               break;
   break;
   }
}


bool   createevent( Event* event ) 
{  tcreateevent ce ( event );
   ce.run();
   ce.done();

   if ((ce.taste == ct_esc) || (ce.action == 2) || ( ce.action == 255 ))
      return false;
   else
      return true;
}

// ıS EventSel

class   teventsel : public tstringselect {
           public :
                 int oi;
                 void rebuildlines(void);
                 void setup(void);
                 virtual void buttonpressed(int id);
                 virtual void get_text(int nr);
                 virtual void run(void);
                 };


void         teventsel::setup(void)
{
   title = "Events";
   numberoflines = actmap->events.size();
   xsize = 570;
   ysize = 360;
   x1 = 35;
   y1 = 60;

   sy = 120;
   ey = ysize - 20;
   addbutton("~N~ew",20,40,90,70,0,1,4,true);
   addbutton("~E~dit",110,40,180,70,0,1,5,true);
   // addbutton("~C~hange",200,40,260,70,0,1,6,true);
   addbutton("~D~elete",280,40,340,70,0,1,7,true);
   addbutton("~O~K",360,40,420,70,0,1,8,true);
}

void   teventsel::get_text(int nr) //gibt in txt den string zurÅck
{
   int count = 0;
   for ( tmap::Events::iterator i = actmap->events.begin(); i != actmap->events.end() && count <= nr; ++i, ++count )
      if( count == nr ) {
         strcpy(txt,EventActionName[(*i)->action->getActionID()]);
         strcat(txt,"  ");
         strcat(txt,(*i)->description.c_str());
      }
}


void         teventsel::buttonpressed(int         id)
{
   tstringselect::buttonpressed(id);
   switch (id) {

      case 4:   { // new
             Event* ev = new Event(*actmap);
             if ( !createevent( ev ) ) {
                delete ev;
                return;
             }

             tmap::Events::iterator e = actmap->events.begin();
             for ( int t = 0; t < redline && e != actmap->events.end(); t++ )
                ++e;

             actmap->events.insert( e, ev );

             numberoflines++;
             if ((numberoflines > lnshown) && (scrollbarvisible == false)) {
                scrollbar_on();
                enablebutton(1);
                enablebutton(2);
                enablebutton(3);
             }

             viewtext();
          }
      break;
      case 5:  if ( actmap->events.size() ) { // edit
                   tmemorystreambuf buf;
                                              
                   tmap::Events::iterator e = actmap->events.begin();
                   for ( int t = 0; t < redline && e != actmap->events.end(); t++ )
                      ++e;

                   {
                     tmemorystream stream ( &buf, tnstream::writing );
                     (*e)->write ( stream );
                   }

                   if ( ! createevent( *e ) ) {
                     // cancel pressed, we are restoring the original event
                     tmemorystream stream ( &buf, tnstream::reading );
                     (*e)->read ( stream );
                   }

                   viewtext();
                }
      break;

      case 7:   { // delete

             tmap::Events::iterator e = actmap->events.begin();
             for ( int t = 0; t < redline && e != actmap->events.end(); t++ )
                ++e;

             delete *e;
             actmap->events.erase ( e );

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


