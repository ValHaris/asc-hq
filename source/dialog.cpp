/*! \file dialog.cpp
    \brief Many many dialog boxes used by the game and the mapeditor
*/

/*
    This file is part of Advanced Strategic Command; http://www.asc-hq.de
    Copyright (C) 1994-2005  Martin Bickel  and  Marc Schellenberger

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


#include <stdio.h>
#include <cstring>
#include <ctype.h>
#include <stdlib.h>
#include <stdarg.h>

#include "typen.h"
#include "buildingtype.h"
#include "vehicletype.h"
#include "basegfx.h"
#include "newfont.h"
#include "spfst.h"
#include "loaders.h"
#include "misc.h"
#include "events.h"
#include "stack.h"
#include "dlg_box.h"
#include "dialog.h"
#include "sgstream.h"
#include "attack.h"
#include "gameoptions.h"
#include "errors.h"
// #include "password_dialog.h"
#include "itemrepository.h"
#include "mapdisplay.h"
#include "graphicset.h"
#include "viewcalculation.h"
#include "paradialog.h"
#include "cannedmessages.h"

#include "dialogs/messagedialog.h"
#include "widgets/textrenderer.h"

#ifndef karteneditor
 #include "gamedlg.h"
#endif


#define markedlettercolor red


#if 0


void         doubleline(int x1, int y1, int x2, int y2)
{ 
   line(x1,y1,x2,y2,white);          
   line(x1 + 1,y1 + 1,x2 + 1,y2 + 1,dbluedark); 
} 


class  tstatistics : public tdialogbox {
              public:
                   int          linelength;
                   char         categories;
                   int          l[3][4];
                   int          m[3][9];
                   char         paintmode;
                   char         color1,color2,color3,color4;
                   char         action;

                   void         init ( void );
                   void         paintgraph ( void );
                   virtual void buttonpressed( int id );
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
    bool      kn; 
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
  int         i;

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

   for (j = 0; j <= 8; j++) { 
      if (j == 8) 
         b = 3; 
      else 
         if (j == actmap->actplayer) 
            b = 0; 
         else 
            if ( actmap->player[actmap->actplayer].diplomacy.getState(j) >= PEACE ) 
               b = 1; 
            else 
               b = 2;

      for ( Player::VehicleList::iterator i = actmap->player[j].vehicleList.begin(); i != actmap->player[j].vehicleList.end(); i++ ) {
         Vehicle* actvehicle = *i;
         if (fieldvisiblenow(getfield(actvehicle->xpos,actvehicle->ypos))) {
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
      }
   } 
} 


void         tstatisticbuildings::count(void)
{ 
   integer      j, b;
   for (j = 0; j <= 8; j++) {
      if (j == 8) 
         b = 3; 
      else 
         if (j == actmap->actplayer) 
            b = 0; 
         else 
            if ( actmap->player[actmap->actplayer].diplomacy.getState(j) >= PEACE ) 
               b = 1; 
            else 
               b = 2; 
      for ( Player::BuildingList::iterator i = actmap->player[j].buildingList.begin(); i != actmap->player[j].buildingList.end(); i++ ) {
         Building* actbuilding = *i;
         if (fieldvisiblenow( actbuilding->getEntryField() )) {
            l[0][b]++; 
            m[0][j]++; 
         } 
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


void         tstatistics::buttonpressed( int id )
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


#endif



#define klickconst 100
#define delayconst 10



void         dispmessage2(int          id,
                          char *       st)
{
   char          *s2;

   const char* sp = getmessage(id);
   const char* s1 = sp;
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

      displaymessage2(s); 
   }
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
                  int      result;
                  void      init( const char* a, const char* b, const char* c );
                  virtual void buttonpressed( int id );
                  virtual void run( void );
                };


void         tchoice_dlg::init( const char* a, const char* b, const char* c )
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
  if ( actmap && actmap->actplayer >= 0 )
     activefontsettings.color = actmap->actplayer * 8 + 20;
  else
     activefontsettings.color = 20;
  mousevisible ( false );
  showtext2(a,x1 + 10,y1 + 15);
  mousevisible ( true );
  result = 0; 
} 


void         tchoice_dlg::buttonpressed( int id )
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


int         legacy_choice_dlg( const char *       title,
                        const char *       s1,
                        const char *       s2,
                        ... )
{ 

   va_list paramlist;
   va_start ( paramlist, s2 );

   char tempbuf[1000];

   int lng = vsprintf( tempbuf, title, paramlist );
   va_end ( paramlist );
   if ( lng >= 1000 )
      displaymessage ( "dialog.cpp / choice_dlg:   string to long !\nPlease report this error",1 );

  tchoice_dlg  c;
  int a;

  c.init(tempbuf,s1,s2);
  c.run();
  a = c.result;
  c.done(); 
  return a;
} 


int  choice_dlg(const ASCString& title, const ASCString& leftButton, const ASCString& rightButton )
{
   if ( legacyEventSystemActive() ) {
      return legacy_choice_dlg( title.c_str(), leftButton.c_str(), rightButton.c_str() );
   } else {
      return new_choice_dlg( title, leftButton, rightButton );
   }
   return 0;
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






/*
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

void tenterfiledescription::buttonpressed ( int id )
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


   mousevisible(true);
   do {
      tdialogbox::run();
   } while ( status == 0 ); 

}

*/



#if 0



typedef string* tnamestrings[9];

class  tenternamestrings : public tdialogbox {
                     typedef tdialogbox inherited;
                     ASCString dlgtitle;
                  public:
                       char             status;
                       char             playerexist;
                       tnamestrings&    orgnames;
                       char             names[9][100];
                       char             buttonnames[9][100];

                       void             init( char plyexistyte, int md);
                       virtual void     run ( void );
                       virtual void     buttonpressed( int id );
                       void             done ( void );
                       virtual int      getcapabilities ( void );

                       tenternamestrings ( tnamestrings& n ) : orgnames ( n ) {};
                    };


class  tsetalliances : public tdialogbox {
                     typedef tdialogbox inherited;
               protected:
                     virtual void redraw ( void );       
                     tmap::Shareview     sv;
               public:
                     char                status;
                     char                alliancedata[8][8];
                     char                location[8];
                     int                 playerpos[8];  // Beispiel: Es existieren Spieler 0 und Spieler 6; dann ist playerpos[0] = 0 und playerpos[1] = 6
                     // int                 playermode[8];    /*  0: player
                     //                                          1: ai
                     //                                          2: off  */

                     char                playernum;
                     char                playerexist;
                     char                lastplayer;
                     int                 oninit;
                     int                 supervisor;
                     bool                mapeditor;
                     bool                  shareview_changeable;
                     int                 xp,yp,xa,ya,bx;  /* position der tastatur-markierungsrechtecke  */

                     void                init ( int supervis );
                     virtual void        buttonpressed( int id);
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


void         tenternamestrings::init(  char plyexist, int md )
{ 

  char         i, j;

  tdialogbox::init();
  playerexist = plyexist;
  j = 0; 
  for (i = 0; i <= 7; i++) 
    if (playerexist & (1 << i)) 
      j++;

  dlgtitle = "enter ";
  dlgtitle += cens[md];
  dlgtitle += " names" ;
  title = dlgtitle.c_str();

  xsize = 350; 
  x1 = (640 - xsize) / 2; 
  ysize = 140 + j * 45; 
  y1 = (480 - ysize) / 2; 


  for (i=0;i<8 ;i++ ) {
     strcpy ( names[i], orgnames[i]->c_str() );
     strcpy( buttonnames [i], cens[md] );
     strcat( buttonnames [i], digit[i] );
  } /* endfor */

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
  tdialogbox::done();
}


void         tenternamestrings::buttonpressed( int id )
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

  if (status == 2)
     for (int i=0;i<8 ;i++ )
        *orgnames[i] = names[i];

} 



int               tenternamestrings::getcapabilities ( void )
{
  return 1;
}

#endif


void viewVisibilityStatistics()
{
/*
   ASCString msg;

   tmap::Shareview* sv = actmap->shareview;
   actmap->shareview = NULL;
   computeview ( actmap );

   for ( int i = 0; i < 8; i++ )
      if ( actmap->player[i].exist() ) {
         msg += ASCString("#font02#Player ") + strrr ( i ) + "#font01#\n" ;
         int notVisible = 0;
         int fogOfWar = 0;
         int visible = 0;
         for ( int x = 0; x < actmap->xsize; x++ )
            for ( int y = 0; y < actmap->ysize; y++ ) {
                VisibilityStates vs = fieldVisibility  ( actmap->getField ( x, y ), i );
                switch ( vs ) {
                   case visible_not: ++notVisible;
                   break;
                   case visible_ago: ++fogOfWar;
                   break;
                   default: ++visible;
                }
            }
         msg += ASCString("  not visible: ") + strrr(notVisible ) + " fields\n";
         msg += ASCString("  fog of war: ") + strrr(fogOfWar ) + " fields\n";
         msg += ASCString("  visible: ") + strrr(visible ) + " fields\n\n";
      }


   actmap->shareview = sv;
   computeview ( actmap );

*/
   tviewanytext vat;
   vat.init ( "Visibility Statistics", "Sorry, not available in ASC2 " );
   vat.run();
   vat.done();
}




#if 0




  #define tsa_namelength 100
  #define ply_x1 25
  #define ply_y1 55
  #define ply_lineheight 22  
  #define ali_x1 30 + tsa_namelength  
  #define ali_y1 275  

// const char*  playermodenames[]  = {"player ", "ai ", "off "};
const char   alliancecolors[]  = {0, 1, 2, 3, 4, 5, 6, 11};


void         tsetalliances::init( int supervis )
{ 
   #ifdef karteneditor
   oninit = 1;
   supervisor = 1;
   mapeditor = true;
   #else
   mapeditor = false;
   if ( actmap->actplayer == -1 ) {
      oninit = 1;
      supervisor = 1;
   } else {
     oninit = 0;
     supervisor = 0;
   }
   #endif

   if ( supervis ) {
     supervisor = 1;
     oninit = 0;
   }

   shareview_changeable = actmap->actplayer >= 0   &&  !oninit ;

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
   addbutton("~a~i names",400,115,xsize - 20,140,0,1,4,true);
//   addbutton("~n~etwork",400,160,xsize - 20,190,0,1,5, actmap->network != NULL ); 
   #endif

   int lastcomppos = 0;
   int plnum = 0;
   int i, j;

   for (i = 0; i <= 7; i++) { 

      if ( actmap->player[i].exist() )  // ((playermode[i] == ps_human) || (playermode[i] == ps_computer)) &&
         playerpos[plnum++] = i;

/*         
      if ( actmap->player[i].exist() && actmap->network ) {
         location[i] = actmap->network->player[i].compposition;
         lastcomppos = location[i];
      } else
         location[i] = lastcomppos;
*/
      if (actmap->player[i].exist() )
         lastplayer = i;

      for (j = 0; j < 8 ; j++) 
         if ( actmap->shareview ) 
            sv.mode[i][j] = actmap->shareview->mode[i][j];
         else
            sv.mode[i][j] = false;
   }

   if ( !supervisor )
     addbutton("~s~upervisor",400,200,xsize - 20,230,0,1,6, !actmap->supervisorpasswordcrc.empty() );
   else {
      if ( !mapeditor && !oninit ) {
          if ( actmap->getgameparameter( cgp_superVisorCanSaveMap) ) {
             addbutton ("save map", 400,290,xsize - 20,320, 0, 1, 116, true );
          }
          for ( int i = 0; i < 8; i++ )
             if ( actmap->player[i].exist() ) {
                int x = x1 + 10 + ply_x1 + 2 * tsa_namelength;
                int y = y1 + ply_y1 + i * 22 - 10;
                // addbutton ("reset passw.", x, y, x+ 90, y + 15, 0, 1, 70+i, true );
                addbutton ("set passw.", x, y, x+ 90, y + 15, 0, 1, 80+i, true );
             }
      }
      if ( !mapeditor && oninit ) {
          for ( int i = 0; i < 8; i++ )
             if ( actmap->player[i].exist() ) {
                int x = x1 + 10 + ply_x1 + 2 * tsa_namelength;
                int y = y1 + ply_y1 + i * 22 - 10;
                addbutton ("set passw.", x, y, x+ 90, y + 15, 0, 1, 80+i, true );
             }
      }
   }

   for (i = 0; i < 8; i++)
      for ( char j = 0; j < 8; j++)
         alliancedata[i][j] = actmap->alliances[i][j] ;

   if ( oninit )
      if ( actmap->shareview )
         for ( i = 0; i < 8; i++ )
            for (int j = 0; j < 8 ; j++)
               if ( sv.mode[i][j] ) {
                  alliancedata[i][j] = capeace_with_shareview;
                  #ifndef pbpeditor
                  alliancedata[j][i] = capeace_with_shareview;
                  #endif
               }


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
      showtext2( actmap->player[ playerpos[j]].getName().c_str(), x1 + 40,y1 + ali_y1 + j * 22);
   }
   npop ( activefontsettings );
}

char* shareview_modenames[2] = { "don't share view", "share view" };


void         tsetalliances::buildhlgraphics(void)
{ 
   collategraphicoperations cgo ( x1, y1, x1 + xsize, y1 + ysize );;

  int         i, j;
  int         mss; 

   mss = getmousestatus(); 
   if (mss == 2)
     mousevisible(false); 
   paintkeybar(); 
   playernum = 0; 
   for (i = 0; i <= 7; i++) { 
      if (actmap->player[i].exist() ) {
         activefontsettings.color = 23 + i * 8; 
         activefontsettings.background = 17 + i * 8; 
         activefontsettings.length = tsa_namelength;

         showtext2( actmap->player[i].getName().c_str(), x1 + ply_x1,y1 + ply_y1 + i * 22);

         activefontsettings.background = dblue; 
         showtext2("(at)",x1 + ply_x1 + tsa_namelength,y1 + ply_y1 + i * 22);

         activefontsettings.color = alliancecolors[location[i]];

         char temp[1000];
         sprintf(temp, "computer %d", location[i] );
         showtext2(temp ,x1 + ply_x1 + 30 + tsa_namelength,y1 + ply_y1 + i * 22);

         if ( shareview_changeable )
            if ( i != actmap->actplayer )
               showtext2(shareview_modenames[sv.mode[actmap->actplayer][i]],x1 + 40 + ply_x1 + 2 * tsa_namelength,y1 + ply_y1 + i * 22); 

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

      if ( actmap->player[playerpos[j]].stat != 2 )
         for (i = 0; i < playernum ; i++) {   /*  x  */ 
              /* if i=j then
               nputimage(x1+ali_x1+i*30,y1+ali_y1+j*22,icons.diplomaticstatusinv)
            else */ 
            if (i != j) 
              if ( actmap->player[playerpos[i]].stat != 2 )
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
     if ( actmap->player[i].exist() )
        cmp |= 1 << location[i];

  int num = 0;
  for ( i = 1; i < 256; i<<=1 ) 
     if ( cmp & i )
        num++;
}
#endif

void         tsetalliances::setparams ( void )
{
  int i, j;

  for (i = 0; i < 8; i++) {

      for (j = 0; j < 8; j++)
         actmap->alliances[i][j] = alliancedata[i][j];

   } /* endfor */

  for ( i = 0; i < 8; i++ )
     for (j = 0; j < 8; j++ ) {
        if ( actmap->alliances[i][j] == canewsetwar1 ) {
           actmap->alliances[i][j] = canewsetwar2;
           if ( actmap->shareview ) {
              if ( sv.mode[i][j]  )
                 sv.recalculateview = 1;
              sv.mode[j][i] = false;
              sv.mode[i][j] = false;
           }
        }
        if ( oninit ) {
           if ( actmap->alliances[i][j] == capeace_with_shareview ) {
              actmap->alliances[i][j] = capeace;
              sv.mode[i][j] = true;
              #ifndef pbpeditor
              sv.mode[j][i] = true;
              #endif
           } else {
              sv.mode[i][j] = false;
              #ifndef pbpeditor
              sv.mode[j][i] = false;
              #endif
           }
        }


      #ifdef logging
      logtofile ( "6/dialog / tsetalliances::setparams / returning" );
      #endif
    }
  int svnum = sv.recalculateview;
  for ( i = 0; i < 8; i++ )
     for ( int j = 0; j < 8; j++ )
        if ( sv.mode[i][j] )
           svnum++;

  if ( actmap->shareview && !svnum ) {
     delete actmap->shareview;
     actmap->shareview = NULL;
  } else
    if ( actmap->shareview || svnum ) {
       if ( !actmap->shareview )
          actmap->shareview = new tmap::Shareview;

       *actmap->shareview = sv;
    }

}


void         tsetalliances::click(pascal_byte         bxx,
                                pascal_byte         x,
                                pascal_byte         y)
{ 
   mousevisible(false);
   activefontsettings.color = 23 + y * 8; 
   activefontsettings.length = tsa_namelength; 
   activefontsettings.background = dblue; 
   if (bxx == 0) { 
      if (x == 0  && ( y == actmap->actplayer || supervisor ) ) {
         if ( actmap->player[y].stat == Player::human )
            actmap->player[y].stat = Player::computer;
         else
            if ( actmap->actplayer == -1 || mapeditor ) {
               if (actmap->player[y].stat == Player::computer )
                  actmap->player[y].stat = Player::off;
               else
                  actmap->player[y].stat = Player::human;
            } else {
                actmap->player[y].stat = Player::human;
            }

         activefontsettings.background = 17 + y * 8;
         showtext2( actmap->player[y].getName().c_str(), x1 + ply_x1,y1 + ply_y1 + y * 22);
         displayplayernamesintable(  );
      } 
      #ifndef karteneditor

      int nextplayer = actmap->actplayer;
      do {
         nextplayer++;
         if ( nextplayer > 7 )
           nextplayer = 0;
      } while ( !actmap->player[nextplayer].exist() ); /* enddo */

      #if SDL_BYTEORDER == SDL_LIL_ENDIAN
      if (x == 1 && ( y == actmap->actplayer || y == nextplayer || supervisor ) ) {
         location[y]++;
         if (location[y] >= playernum) 
            location[y] = 0;
         activefontsettings.color = alliancecolors[location[y]]; 
         char temp[1000];
         sprintf(temp, "computer %d", location[y] );
         showtext2(temp,x1 + ply_x1 + 30 + tsa_namelength,y1 + ply_y1 + y * 22);
         checkfornetwork();
      } 
      #endif //  SDL_LIL_ENDIAN
      #endif

      if (x == 2  &&  ( y != actmap->actplayer ) && actmap->actplayer>=0  &&  !oninit && !supervisor )
         if ( (actmap->alliances[actmap->actplayer][y] == capeace && actmap->alliances[y][actmap->actplayer] == capeace) ||  sv.mode[actmap->actplayer][y] ) {

            sv.mode[actmap->actplayer][y] = ! sv.mode[actmap->actplayer][y];

            activefontsettings.color = 23 + y * 8;
            activefontsettings.background = dblue;
            activefontsettings.length = tsa_namelength;

            activefontsettings.color = alliancecolors[location[y]];
            showtext2(shareview_modenames[sv.mode[actmap->actplayer][y]],x1 + 40 + ply_x1 + 2 * tsa_namelength,y1 + ply_y1 + y * 22);

         }

   }
   else {
      if (x != y) { 
         if ( oninit ) {
            if (x < playernum  && y < playernum) {
               if ( actmap->player[playerpos[x]].stat != 2  &&  actmap->player[playerpos[y]].stat != 2 ) {

                  if ( alliancedata[playerpos[x]][playerpos[y]] == capeace )
                     alliancedata[playerpos[x]][playerpos[y]] = capeace_with_shareview;
                  else 
                     if ( alliancedata[playerpos[x]][playerpos[y]] == cawar )
                        alliancedata[playerpos[x]][playerpos[y]] = capeace;
                     else
                        alliancedata[playerpos[x]][playerpos[y]] = cawar;

                  #ifndef pbpeditor
                  alliancedata[playerpos[y]][playerpos[x]] = alliancedata[playerpos[x]][playerpos[y]];
                  putimage(x1 + ali_x1 + y * 30,y1 + ali_y1 + x * 22,icons.diplomaticstatus[alliancedata[playerpos[x]][playerpos[y]]]);
                  #endif

                  putimage(x1 + ali_x1 + x * 30,y1 + ali_y1 + y * 22,icons.diplomaticstatus[alliancedata[playerpos[x]][playerpos[y]]]);
               }
            } /* endif */
         } else {
            if ( x < playernum  && y < playernum && actmap->player[playerpos[x]].stat != 2  &&  actmap->player[playerpos[y]].stat != 2 )
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





void         tsetalliances::buttonpressed( int id )
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

      case 3: {
                 tnamestrings names;
                 for (i = 0; i < 9; i++ ) {
                     if ( actmap->player[i].stat == 0 )
                        names[i] = &actmap->player[i].humanname;
                     else
                        names[i] = &actmap->player[i].computername;
                 }

                 tenternamestrings   enternamestrings ( names );

                 int plx;
                 if ( oninit || supervisor || actmap->actplayer == -1)
                    plx = playerexist;
                 else
                    plx = 1 << actmap->actplayer;
                 enternamestrings.init( plx ,0);
                 enternamestrings.run();
                 enternamestrings.done(); 
                 buildhlgraphics();
           }
      break; 
/*
      case 4:  {  
                 int plx = 0;
                 for ( int j = 0; j < 8; j++ )
                    if ( actmap->player[j].existent )
                       plx = plx * 2 + 1;
                 //
                 //if ( oninit || supervisor || actmap->actplayer == -1) {
                 //   plx = playerexist;
                 //} else
                 //   plx = 1 << actmap->actplayer;

                  tenternamestrings   enternamestrings;
                  enternamestrings.init( &computername, plx ,2);
                  enternamestrings.run(); 
                  enternamestrings.done();
                  buildhlgraphics();
               }
      break;
*/
      #ifndef karteneditor
      case 5: setparams();
              // setupnetwork ( actmap->network );
              break;

      case 6: {
                 if ( !actmap->supervisorpasswordcrc.empty() ) {
                    bool success = enterpassword ( actmap->supervisorpasswordcrc, false, true );
                    if ( success ) {
                      supervisor = 1;
                      disablebutton ( 6 );
                    }
                 }
              }
      break;
      case 116: {
                    viewVisibilityStatistics();

                    for ( int i = 0; i < 8; ++i )
                       for ( Player::VehicleList::iterator j = actmap->player[i].vehicleList.begin(); j != actmap->player[i].vehicleList.end(); j++ )
                           (*j)->resetMovement();


                    ASCString filename = selectFile(mapextension, false);
                    if ( !filename.empty() )
                       savemap( filename.c_str() );

                    delete actmap;
                    throw NoMapLoaded();

                }
      #endif
   }
   if ( id >= 70 && id <= 77 ) {
      actmap->player[id-70].passwordcrc.reset();
      char txt[1000];
      char* sp = "The supervirsor reset the password of player #%d, %s" ;
      sprintf ( txt, sp, id-70, actmap->player[id-70].getName().c_str() );
      new Message ( txt, actmap, 255 );
   }

   if ( id >= 80 && id <= 87 )
      enterpassword ( actmap->player[id-80].passwordcrc, true, true, false );
}


void         tsetalliances::run(void)
{ 
  int         i, j; 

   mousevisible(true); 
   do { 
      tdialogbox::run();
      if (mouseparams.taste == 1) { 
         for (i = 0; i <= 7; i++) { 
            if (actmap->player[i].exist() ) {
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

               if (shareview_changeable && (mouseparams.x > x1 + 40 + ply_x1 + 2 * tsa_namelength) && (mouseparams.x <= x1 + 40 + ply_x1 + 3 * tsa_namelength) && (mouseparams.y > y1 + ply_y1 + i * ply_lineheight) && (mouseparams.y <= y1 + ply_y1 + (i + 1) * ply_lineheight)) {
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
   
   setparams();
}


void         setupalliances( int supervisor )
{ 
   tsetalliances sua;
   sua.init( supervisor );
   sua.run(); 
   sua.done(); 
}  

#endif


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


void         tviewanytext::buttonpressed( int id )
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

      addscrollbar(xsize - 30,starty,xsize - 15,ysize - 35,&textsizeycomplete, textsizey, &tvt_starty,1,0);
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


void         tviewanytext::buttonpressed( int id )
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
      if ( prntkey == 's' )
         printf("%s", txt);
   }  while (action < 10);
} 





class tbi3preferences : public tdialogbox {
                        CGameOptions actoptions;
                        int status;
                        tmouserect delta, r1, r2, r3, r4, r5, r6, r7, r8;
                        tmouserect dlgoffset;

                     public:
                        void init ( void );
                        void buttonpressed ( int id );
                        void run ( void );
                        tbi3preferences ( ) : actoptions ( *CGameOptions::Instance() ) {};
                    };


void tbi3preferences :: init ( void )
{
   tdialogbox::init();
   title = "BattleIsle options";

   xsize = 600;
   ysize = 460;

   x1 = -1;
   y1 = -1;

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
//   addeingabe ( 3, &actoptions.bi3.interpolate.terrain, 0, dblue );

   r2 = r1 + delta;

   addbutton ( "", r2,  3, 0, 4, true  );
  // addeingabe ( 4, &actoptions.bi3.interpolate.units, 0, dblue );

   r3 = r2 + delta;

   addbutton ( "", r3,  3, 0, 5, true  );
 //  addeingabe ( 5, &actoptions.bi3.interpolate.buildings, 0, dblue );
   
   r4 = r3 + delta;

   addbutton ( "", r4,  3, 0, 6, true  );
//   addeingabe ( 6, &actoptions.bi3.interpolate.objects, 0, dblue );

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

void tbi3preferences :: buttonpressed ( int id )
{
   tdialogbox :: buttonpressed ( id );

   if ( id == 1 ) {
      *CGameOptions::Instance() = actoptions;
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
                        void buttonpressed ( int id );
                        void run ( void );
                        char checkvalue( int         id, void*      p);
                    };


char ResizeMap :: checkvalue(int         id, void*      p)  
{
   if ( id >= 3 && id <= 6 ) {
      int* wp = (int*) p;

      if ( id == 3 && id == 6 ) 
         if ( *wp & 1 ) {
            displaymessage ( "values must be even !", 1 );
            return 0;
         }
         /*
      if ( (id == 3   &&  - ( *wp + bottom ) > actmap->ysize - idisplaymap.getscreenysize(1) ) ||
           (id == 6   &&  - ( top + *wp )    > actmap->ysize - idisplaymap.getscreenysize(1) ) ||
           (id == 4   &&  - ( *wp + right  ) > actmap->xsize - idisplaymap.getscreenxsize(1) ) ||
           (id == 5   &&  - ( left + *wp   ) > actmap->xsize - idisplaymap.getscreenxsize(1) ) ) {
         displaymessage ( "remaining size too small !", 1 );
         return 0;
      }    */

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

void ResizeMap :: buttonpressed ( int id )
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
      int result = actmap->resize ( top, bottom, left, right );
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








class tchoosezoomlevel : public tdialogbox {
             protected:
                int zoom;
                int ok;
             public:
                void init ( void );
                void buttonpressed ( int id );
                void run ( void );
         };

void tchoosezoomlevel :: init ( void )
{
   tdialogbox :: init ( );
   title = "choose zoomlevel";
   xsize = 200;
   ysize = 150;
//   zoom = zoomlevel.getzoomlevel();
   ok = 0;

   addbutton ( "~O~k", 10, ysize - 30, xsize - 10, ysize - 10 , 0, 1, 1, true );
   addkey ( 1, ct_enter );

   addbutton ( "", 10, 60, xsize - 10, 85                     , 2, 1, 2, true );
   // addeingabe ( 2, &zoom, zoomlevel.getminzoom(), zoomlevel.getmaxzoom() );
   addkey ( 1, ct_space );

   buildgraphics();
}

void tchoosezoomlevel :: buttonpressed ( int id )
{
   tdialogbox:: buttonpressed ( id );
   if ( id == 1 )
      ok = 1;
}

void tchoosezoomlevel :: run ( void )
{
   if ( pcgo ) {
      delete pcgo;
      pcgo = NULL;
   }
   editfield ( 2 );
   ok = 1;
   mousevisible ( true );
   do {
      tdialogbox::run();
   } while ( !ok ); 
//   zoomlevel.setzoomlevel ( zoom );

};

void choosezoomlevel ( void )
{
   tchoosezoomlevel ctl;
   ctl.init();
   ctl.run();
   ctl.done();
   displaymap();
}





void showbdtbits( void )
{
   tfield* fld = actmap->getField(actmap->getCursor());
   char m[200];
   m[0] = 0;
   for (int i = 0; i < cbodenartennum ; i++) {
      TerrainBits bts;
      bts.set ( i );

      if ( (fld->bdt & bts).any() ) {
         strcat ( m, cbodenarten[i] );
         strcat ( m, "\n" );
      }
   } /* endfor */
   displaymessage( m, 3 );
}

void appendTerrainBits ( char* text, const TerrainBits* bdt )
{
   for (int i = 0; i < cbodenartennum ; i++) {
      TerrainBits bts;
      bts.set ( i );

      if ( (*bdt & bts).any() ) {
         strcat ( text, "    " );
         strcat ( text, cbodenarten[i] );
         strcat  ( text, "\n" );
      }
   } /* endfor */
}


void viewUnitSetinfo ( void )
{
   ASCString s;
   tfield* fld = actmap->getField( actmap->getCursor() );
   if ( fld && fieldvisiblenow  ( fld ) && fld->vehicle ) {

         s += "#fontsize=18#Unit Information:#fontsize=14##aeinzug20##eeinzug20##crtp20#" ;

         const Vehicletype* typ = fld->vehicle->typ;
/*
         s += "\nreactionfire.Status: ";
         s += strrr( getactfield()->vehicle->reactionfire.status );
         s += "\nreactionfire.enemiesattackable: ";
         s += strrr ( getactfield()->vehicle->reactionfire.enemiesAttackable );
*/
         s += "Unit name: " ;
         if ( !typ->name.empty() )
            s += typ->name ;
         else
            s += typ->description;

         s += "\nUnit owner: " + ASCString::toString( fld->vehicle->getOwner() ) + " - " + actmap->getPlayer(fld->vehicle).getName();

         s += "\nUnit ID: " + ASCString::toString( typ->id );

         if ( !typ->location.empty() ) 
            s += typ->location;

         if ( unitSets.size() > 0 )
            for ( unsigned int i = 0; i < unitSets.size(); i++ )
               if ( unitSets[i]->isMember ( typ->id,SingleUnitSet::unit )) {
                  s += "Unit is part of this unit set:";
                  if ( unitSets[i]->name.length()) {
                     s += "#aeinzug20##eeinzug20#\nName: #aeinzug60##eeinzug60#\n";
                     s += unitSets[i]->name;
                  }
                  if ( unitSets[i]->maintainer.length()) {
                     s += "#aeinzug20##eeinzug20#\nMaintainer: #aeinzug60##eeinzug60#\n";
                     s += unitSets[i]->maintainer;
                  }
                  if ( unitSets[i]->information.length()) {
                     s += "#aeinzug20##eeinzug20#\nInformation: #aeinzug60##eeinzug60#\n";
                     s += unitSets[i]->information;
                  }
                  s += "#aeinzug0##eeinzug0#\n";
               }


   } else
      s += "\nNo unit selected";

   ViewFormattedText vft( "Unit information", s, PG_Rect( -1, -1, 400, 350) );
   vft.Show();
   vft.RunModal();
}






#define blocksize 256


int tparagraph :: winy1;
int tparagraph :: winy2;
int tparagraph :: winx1;
int tparagraph :: maxlinenum;

tparagraph :: tparagraph ( void )
{
   text = new char [ blocksize ];
   text[0] = 0;
   size = 1;
   allocated = blocksize;

   next = NULL;
   prev = NULL;
   cursor = -1;
   cursorstat = 0;
   ps.line1num = 0;
   linenum = -1;
   searchcursorpos = 0;
}

tparagraph :: tparagraph ( pparagraph prv )
{
   searchcursorpos = 0;
   linenum = -1;
   text = new char [ blocksize ];
   text[0] = 0;
   size = 1;
   allocated = blocksize;
   cursorstat = 0;
   cursor = -1;

   prev = prv;

   if ( prv ) {
      next = prv->next;
      if ( next )
         next->prev = this;
      prv->next = this;

      ps.line1num = prv->ps.line1num + 1 + prv->linenum;
   } else {
      next = NULL;
      ps.line1num = 0;
   }
}



tparagraph :: ~tparagraph ()
{
   if ( text ) {
      delete[] text;
      text = NULL;
   }

   if ( prev )
      prev->next = next;

   if ( next )
      next->prev = prev;

}


void tparagraph :: changesize ( int newsize )
{
   newsize = (newsize / blocksize + 1) * blocksize;
   char* temp = new char[newsize];
   if ( text ) {
      strcpy ( temp, text );
      delete[] text;
   } else
      temp[0] = 0;

   allocated = newsize;
   size = strlen ( temp )+1;
   text = temp;
}


void tparagraph :: join ( void )
{
   cursor = size-1;
   addtext ( next->text );
   delete next;
   if ( reflow() ) {
       display();
       if ( checkcursorpos() ) {
          checkscrollup();
          checkscrolldown();
       }
   }


}



void tparagraph :: addtext ( const ASCString& txt )
{
    int nsize = txt.length();
    if ( allocated < nsize + size )
       changesize ( nsize + size );

    strcat ( text, txt.c_str() );
    size = strlen ( text ) + 1;
}

void tparagraph :: checkcursor( void )
{
   if ( cursor != -1 ) {
      if ( cursor >= size )
         cursor = size-1;
      if ( cursor < 0 )
         cursor = 0;
   } else
      displaymessage ( " void tparagraph :: checkcursor ( void ) \ncursor not in paragraph !\n text is : %s", 2, text );
}



void tparagraph :: addchar ( char c )
{
   checkcursor();
   if ( size + 1 > allocated )
      changesize ( size + 1 );

    for ( int i = size; i > cursor; i--)
       text[i] = text[i-1];

    text[cursor] = c;
    cursor++;
    size++;

    if ( reflow() ) {
       display();
       checkcursorpos();
    }
}


pparagraph tparagraph :: erasechar ( int c )
{
   checkcursor();
   if ( c == 1 ) {     // backspace
      if ( cursor ) {
         for ( int i = cursor-1; i < size; i++)
            text[i] = text[i+1];
         cursor--;
         size--;
         if ( reflow() )
            display();
      } else
         if ( prev ) {
            pparagraph temp = prev;
            prev->join();
            return temp;              //  !!!###!!!   gef„llt mir eigentlich ?berhauptnicht, wird aber wohl laufen. Sonst m?áte ich halt erasechar von auáen managen
         }

   }
   if ( c == 2 ) {     // del
      if ( cursor < size-1 ) {
         for ( int i = cursor; i < size; i++)
            text[i] = text[i+1];
         size--;
         if ( reflow() )
            display();
      } else
         if ( next )
            join ( );
   }
   return this;
}

int  tparagraph :: checkcursorpos ( void )
{
   if ( cursor >= 0 ) {
      checkcursor();
      if ( ps.line1num + cursory >= maxlinenum ) {
         ps.line1num = maxlinenum - cursory - 1;
         display();
         checkscrollup();
         checkscrolldown();
         return 0;
      }

      if( ps.line1num + cursory < 0 ) {
         ps.line1num = -cursory;
         display();
         checkscrollup();
         checkscrolldown();
         return 0;
      }
   }
   return 1;
}

pparagraph tparagraph :: cut ( void )
{
   checkcursor();
   displaycursor();
   char* tempbuf = strdup ( &text[cursor] );
   text[cursor] = 0;
   size = strlen ( text ) + 1;
   reflow ( 0 );
   new tparagraph ( this );
   if ( tempbuf[0] )
      next->addtext ( tempbuf );

   asc_free ( tempbuf );
   cursor = -1;
   next->cursor = 0;
   reflow( 0 );
   display();
   if ( next->reflow() )
      next->display();

   return next;
}

void tparagraph :: checkscrollup ( void )
{
   if ( prev ) {
      if ( prev->ps.line1num + prev->linenum + 1 != ps.line1num ) {
           prev->ps.line1num = ps.line1num - 1 - prev->linenum ;
           prev->display();
           prev->checkscrollup();
      }
   }
}

void tparagraph :: checkscrolldown ( void )
{
   if ( next ) {
      if ( ps.line1num + linenum + 1 != next->ps.line1num ) {
         next->ps.line1num = ps.line1num + linenum + 1;
         next->display();
         next->checkscrolldown ();
      }
   } else {
      if ( ps.line1num + linenum < maxlinenum )
         bar ( winx1, winy1 + (ps.line1num + linenum + 1) * activefontsettings.height, winx1 + activefontsettings.length, winy1 + maxlinenum * activefontsettings.height, dblue );

   }
}


int  tparagraph :: reflow( int all  )
{
   int oldlinenum = linenum;

   pfont font = activefontsettings.font;
   linenum = 0;
   int pos = 0;
   linestart [ linenum ] = text;
   int length = 0;

   do {

     if ( font->character[int(text[pos])].size ) {
        length += font->character[int(text[pos])].width + 2 ;
        if ( pos )
           length += font->kerning[int(text[pos])][int(text[pos-1])];
     }

     if ( length > activefontsettings.length ) {
        int pos2 = pos;
        while ( ( text[pos2] != ' ' )  &&  ( &text[pos2] - linestart[linenum] )  )
           pos2--;

        if ( &text[pos2] == linestart[linenum] )
           pos2 = pos;
        else
           pos2++;

        linelength[linenum] = &text[pos2] - linestart[linenum];
        if ( linelength[linenum] > 200 ) {
           printf("\a");
        }
        linenum++;
        linestart[linenum] = &text[pos2];
        length = 0;
        pos = pos2;
     } else
        pos++;
   } while ( pos <= size ); /* enddo */

   linelength[linenum] = &text[pos] - linestart[linenum];
        if ( linelength[linenum] > 200 ) {
           printf("\a");
        }


   int oldlength = 0;
   for (int i = 0; i <= linenum; i++) {
      length = 0;
      for ( int j = 0; j < linelength[i]; j++ ) {
         pos = linestart[i] - text + j;

         if ( pos == cursor  && !searchcursorpos ) {
             cursorx = length;
             normcursorx = cursorx;
             cursory = i;
         }

         if ( searchcursorpos ) {
            if ( i == cursory )
               if ( length >= normcursorx ) {
                  cursorx = length;
                  cursor = pos;
                  searchcursorpos = 0;
               }
            if ( i > cursory ) {
               cursorx = oldlength;
               cursor = pos-1;
               searchcursorpos = 0;
            }
        }

        if ( font->character[int(text[pos])].size ) {
             length += font->character[int(text[pos])].width + 2 ;
             if ( pos )
                length += font->kerning[int(text[pos])][int(text[pos-1])];
        }

     }
     oldlength = length;
   } /* endfor */


   if ( searchcursorpos ) {
      cursorx = length;
      cursor = size-1;
      searchcursorpos = 0;
   }


   if ( all ) {
      if ( linenum != oldlinenum ) {
         display ( );
         if ( checkcursorpos() ) {
            checkscrollup();
            checkscrolldown();
         }
         return 0;
      }
   }
   return 1;

}


pparagraph tparagraph :: movecursor ( int dx, int dy )
{
   pparagraph newcursorpos = this;

   if ( cursorstat )
      displaycursor();

   if ( dx == -1 )
      if ( cursor > 0 )
         cursor--;
      else
         if ( prev ) {
            cursor = -1;
            newcursorpos = prev;
            prev->cursor = prev->size - 1;
         }
   if ( dx == 1 )
      if ( cursor < size-1 )
         cursor++;
      else
         if ( next ) {
            cursor = -1;
            next->cursor = 0;
            newcursorpos = next;
         }

   if ( dy == 1 )
      if ( cursory < linenum ) {
         cursory++;
         searchcursorpos++;
      } else
        if ( next ) {
           cursor = -1;
           next->cursor = 0;
           next->normcursorx = normcursorx;
           next->cursorx = normcursorx;
           next->cursory = 0;
           next->searchcursorpos = 1;
           newcursorpos = next;
        }

   if ( dy == -1 )
      if ( cursory > 0 ) {
         cursory--;
         searchcursorpos++;
      } else
        if ( prev ) {
           cursor = -1;
           prev->cursor = 0;
           prev->normcursorx = normcursorx;
           prev->cursorx = normcursorx;
           prev->cursory = prev->linenum;
           prev->searchcursorpos = 1;
           newcursorpos = prev;
        }

   newcursorpos->reflow( 0 );
   newcursorpos->checkcursorpos();
   newcursorpos->displaycursor();
   return newcursorpos;
}


void tparagraph :: displaycursor ( void )
{
   if ( cursor >= 0 ) {
      int starty;
      if ( ps.line1num < 0 ) {
         starty = winy1 + (ps.line1num + cursory) * activefontsettings.height;
         // startline = -ps.line1num;
      } else {
         starty = winy1 + (ps.line1num + cursory) * activefontsettings.height;
         // startline = 0;
      }

      if ( starty  < winy2 ) {
         collategraphicoperations cgo ( winx1 + cursorx-2,   starty, winx1 + cursorx+2,   starty + activefontsettings.height );
         xorline ( winx1 + cursorx,   starty+1, winx1 + cursorx,   starty + activefontsettings.height - 1, blue  );
         if ( cursorx )
           xorline ( winx1 + cursorx-1, starty+1, winx1 + cursorx-1, starty + activefontsettings.height - 1, blue  );
         cursorstat = !cursorstat;
      }
   }
}

void tparagraph :: setpos ( int x1, int y1, int y2, int linepos, int linenum  )
{
   winx1 = x1;
   winy1 = y1;
   winy2 = y2;
   ps.line1num = linepos;
   maxlinenum = linenum;
}

void tparagraph :: display ( void )
{
   if ( cursorstat )
      displaycursor();
   cursorstat = 0;

   int startline;
   int starty;
   if ( ps.line1num < 0 ) {
      starty = winy1;
      startline = -ps.line1num;
   } else {
      starty = winy1 + ps.line1num * activefontsettings.height;
      startline = 0;
   }

   while ( startline <= linenum  && starty < winy2 ) {
      char *c = &linestart[ startline ] [ linelength [ startline ]] ;
      char d = *c;
      *c = 0;

      showtext2 ( linestart[ startline ], winx1, starty );

      *c = d;
      starty += activefontsettings.height;
      startline++;
   } ; /* enddo */

   if ( cursor >= 0 )
      displaycursor();
}




tmessagedlg :: tmessagedlg ( void )
{
    firstparagraph = NULL;
    lastcursortick = 0;
    blinkspeed = 80;
}

void tmessagedlg :: inserttext ( const ASCString& txt )
{
   if ( txt.empty() )
      firstparagraph = new tparagraph;
   else {
      firstparagraph = NULL;
      actparagraph = NULL;
      int pos = 0;
      int start = 0;
      const char* c = txt.c_str();
      while ( c[pos] ) {
         int sz = 0;
         if ( strnicmp(&c[pos], "#CRT#" , 5 ) == 0 )
            sz = 5;
         if ( strnicmp(&c[pos], "\r\n" , 2 ) == 0 )
            sz = 2;
         if ( strnicmp(&c[pos], "\n" , 2 ) == 0 )
            sz = 1;
         if ( sz ) {
            ASCString s = txt;
            if ( start ) {
               s.erase ( 0, start );
               s.erase ( pos-start );
            } else
               s.erase ( pos );
            actparagraph = new tparagraph ( actparagraph );
            if ( !firstparagraph )
               firstparagraph = actparagraph;

            actparagraph->addtext ( s );
            pos+=sz;
            start = pos;
         } else
            pos++;
      } /* endwhile */

      if ( start < txt.length() ) {
         actparagraph = new tparagraph ( actparagraph );
         if ( !firstparagraph )
             firstparagraph = actparagraph;
         actparagraph->addtext ( &(txt.c_str()[start]) );
      }

   }
}

void tmessagedlg :: run ( void )
{
   tdialogbox::run ( );
   if ( prntkey != cto_invvalue ) {
      if ( prntkey == cto_bspace )
         actparagraph = actparagraph->erasechar ( 1 );
      else
      if ( taste == cto_entf )
         actparagraph->erasechar ( 2 );
      else
      if ( taste == cto_left )
         actparagraph = actparagraph->movecursor ( -1, 0 );
      else
      if ( taste == cto_right )
         actparagraph = actparagraph->movecursor ( 1, 0 );
      else
      if ( taste == cto_up )
         actparagraph = actparagraph->movecursor ( 0, -1 );
      else
      if ( taste == cto_down )
         actparagraph = actparagraph->movecursor ( 0, 1 );
      else
      if ( taste == cto_enter )
         actparagraph = actparagraph->cut ( );
      else
      if ( taste == cto_esc )
         printf("\a");
      else
      if ( prntkey > 31 && prntkey < 256 )
         actparagraph->addchar ( prntkey );
   }
   if ( lastcursortick + blinkspeed < ticker ) {
      actparagraph->displaycursor();
      lastcursortick = ticker;
   }
}

ASCString tmessagedlg :: extracttext ()
{
   tparagraph text;

   actparagraph = firstparagraph;
   while ( actparagraph ) {
      text.addtext ( actparagraph->text );
      text.addtext ( "#crt#" );
      actparagraph = actparagraph->next;
   }
   return text.text;

}

tmessagedlg :: ~tmessagedlg ( )
{
   actparagraph = firstparagraph;
   while ( actparagraph ) {
      pparagraph temp = actparagraph->next;
      delete actparagraph;
      actparagraph = temp;
   }
}

void tmessagedlg :: setup ( void )
{
   xsize = 500;
   ysize = 450;
   ok = 0;
   tx1 = 20;
   ty1 = starty + 10;
   tx2 = xsize - 40;
   ty2 = ysize - ty1 - 80;

   addbutton ( "~S~end", 10, ysize - 30, xsize / 2 - 5, ysize - 10 , 0, 1, 1, true );
   clearkey ( 1 );
   addkey ( 1, cto_stp + cto_s );

   addbutton ( "~C~ancel", xsize / 2 + 5, ysize - 30, xsize - 10, ysize - 10 , 0, 1, 2, true );
   clearkey ( 2 );
   addkey ( 2, cto_stp + cto_c );

   int num = 0;
   for ( int i = 0; i < 8; i++ ) {
      if ( actmap->player[i].exist() )
         if ( actmap->actplayer != i ) {
            int x = 20 + ( num % 2 ) * 200;
            int y = ty2 + 10 + ( num / 2 ) * 20;
            addbutton ( actmap->player[i].getName().c_str(), x, y, x+ 180, y+15, 3, 0, num+3, true );
            addeingabe ( num+3, &to[i], 0, dblue );
            num++;
         }
   }


}




void MultilineEdit :: init ( void )
{
   tdialogbox :: init ( );
   title = dlg_title.c_str();

   setup();
   buildgraphics();


   rectangle ( x1 + tx1 - 2, y1 + ty1 - 2, x1 + tx2 + 2, y1 + ty2 + 2 ,black );


   activefontsettings.font = schriften.smallarial;
   activefontsettings.length = tx2 - tx1 -1 ;
   activefontsettings.height = activefontsettings.font->height+5;
   activefontsettings.background = dblue;
   activefontsettings.color = black;
   activefontsettings.justify = lefttext;

   inserttext ( text.c_str() );
   actparagraph = firstparagraph;
   actparagraph->cursor = 0;
   actparagraph->setpos( x1 + tx1, y1 + ty1, y1 + ty2, 0, 13 );

   while ( actparagraph ) {
      actparagraph->reflow( 0 );
      actparagraph = actparagraph->next;
   }
   actparagraph = firstparagraph;
   actparagraph->checkscrolldown();
   actparagraph->display();


}


void MultilineEdit :: setup ( void )
{
   xsize = 500;
   ysize = 400;
   ok = 0;
   tx1 = 20;
   ty1 = starty + 10;
   tx2 = xsize - 40;
   ty2 = ysize - ty1 - 40;

   addbutton ( "~O~k", 10, ysize - 30, xsize / 2 - 5, ysize - 10 , 0, 1, 1, true );
   clearkey ( 1 );
   addkey ( 1, cto_stp + cto_o );

   addbutton ( "~C~ancel", xsize / 2 + 5, ysize - 30, xsize - 10, ysize - 10 , 0, 1, 2, true );
   clearkey ( 2 );
   addkey ( 2, cto_stp + cto_c );

}


void MultilineEdit :: buttonpressed ( int id )
{
   tmessagedlg :: buttonpressed ( id );
   if ( id == 1 )
      ok = 1;
   if ( id == 2 )
      ok = 2;
}


void MultilineEdit :: run ( void )
{
   mousevisible ( true );
   do {
      tmessagedlg::run ( );
   } while ( !ok );

   if ( ok == 1 ) {
      text = extracttext();
      textchanged =  true;
   }
}



void selectgraphicset ( void )
{
   ASCString filename = selectFile( "*.gfx", true );
   if ( !filename.empty() ) {
      int id = getGraphicSetIdFromFilename ( filename.c_str() );
      if ( id != actmap->graphicset ) {
         actmap->graphicset = id;
         displaymap();
      }
   }
}

int editInt( const ASCString& title, int defaultValue, int minValue, int maxValue )
{
   return getid( title.c_str(), defaultValue, minValue, maxValue );
}




