/*! \file dialog.cpp
    \brief Many many dialog boxes used by the game and the mapeditor
*/

/*
    This file is part of Advanced Strategic Command; http://www.asc-hq.de
    Copyright (C) 1994-2003  Martin Bickel  and  Marc Schellenberger

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

#include "tpascal.inc"
#include "typen.h"
#include "buildingtype.h"
#include "vehicletype.h"
#include "basegfx.h"
#include "newfont.h"
#include "spfst.h"
#include "loaders.h"
#include "misc.h"
#include "events.h"
#include "loadpcx.h"
#include "stack.h"
#include "dlg_box.h"
#include "dialog.h"
#include "sgstream.h"
#include "attack.h"
#include "gameoptions.h"
#include "errors.h"
#include "password_dialog.h"
#include "itemrepository.h"
#include "mapdisplay.h"
#include "networkdata.h"
#include "graphicset.h"
#include "viewcalculation.h"

#ifndef karteneditor
 #include "gamedlg.h"
#endif


#define markedlettercolor red  



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
            if (getdiplomaticstatus(j * 8) == capeace) 
               b = 1; 
            else 
               b = 2;

      for ( tmap::Player::VehicleList::iterator i = actmap->player[j].vehicleList.begin(); i != actmap->player[j].vehicleList.end(); i++ ) {
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
            if (getdiplomaticstatus(j * 8) == capeace) 
               b = 1; 
            else 
               b = 2; 
      for ( tmap::Player::BuildingList::iterator i = actmap->player[j].buildingList.begin(); i != actmap->player[j].buildingList.end(); i++ ) {
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
                   int          wepx, wepy;
                   char         category;
                   char         action;
                   integer      i,j;

                   void         init ( const Vehicletype* type );
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
                   virtual void buttonpressed( int id ); 
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
   backgrnd2 = lightgray; // dblue + 1;
   aktvehicle = eht; 
   weapnum = num - 1; 
   mousevisible(false); 
} 





void         tweaponinfo::run(void)
{ 
  int          i;
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


   if (aktvehicle->weapons.weapon[weapnum].maxstrength > 0)
      if (aktvehicle->weapons.weapon[weapnum].getScalarWeaponType() >= 0 ) {

         line(x1 + 300,y1 + starty + 130,x1 + 300,y1 + starty + 260,black);
         putspriteimage(x1 + 293,y1 + starty + 130,icons.weapinfo.pfeil1);
         line(x1 + 297,y1 + starty + 140,x1 + 303,y1 + starty + 140,black);
         line(x1 + 297,y1 + starty + 260,x1 + 303,y1 + starty + 260,black);

         line(x1 + 310,y1 + starty + 260,x1 + 520,y1 + starty + 260,black);
         putrotspriteimage90(x1 + 516,y1 + starty + 253,icons.weapinfo.pfeil1,0);

         line(x1 + 325,y1 + starty + 257,x1 + 325,y1 + starty + 263,black);
         line(x1 + 510,y1 + starty + 257,x1 + 510,y1 + starty + 263,black);


         /*
         showtext(strrd8(aktvehicle->weapons.weapon[weapnum].mindistance),schriften.smallsystem,x1 + 320,y1 + starty + 270,black);
         showtext(strrd8(aktvehicle->weapons.weapon[weapnum].maxdistance),schriften.smallsystem,x1 + 500,y1 + starty + 270,black);
         */
         activefontsettings.justify = centertext;

         showtext2("distance",x1 + 410,y1 + starty + 270);
         activefontsettings.direction = 90;

         showtext2("strength", x1 + 285,y1 + starty + 200);
         activefontsettings.direction = 0;
         activefontsettings.justify = righttext;
         itoa(aktvehicle->weapons.weapon[weapnum].maxstrength, strng, 10 );
         showtext2(strng, x1 + 292,y1 + starty + 137);
         showtext2("0", x1 + 292,y1 + starty + 257);

         for (i = 0; i <= 255; i++) {
            xa = x1 + 325 + (510 - 325) * i / 256;
            ya = int( y1 + starty + 260 - (260 - 140) * weapDist.getWeapStrength(&aktvehicle->weapons.weapon[weapnum], aktvehicle->weapons.weapon[weapnum].mindistance + ( aktvehicle->weapons.weapon[weapnum].maxdistance - aktvehicle->weapons.weapon[weapnum].mindistance) * i / 256, -1, -1 ) );
            putpixel(xa,ya,darkgray);
         }
      }



   activefontsettings.color = black;
   activefontsettings.background = backgrnd2;
   activefontsettings.justify = lefttext;
   activefontsettings.length = 170;

   activefontsettings.color = black;
   if ( !aktvehicle->weapons.weapon[weapnum].shootable() )
      showtext2("no",x1 + 50,y1 + starty + 205);
   else {
      showtext2("yes",x1 + 50,y1 + starty + 205);
   }

   if ( aktvehicle->weapons.weapon[weapnum].canRefuel() ) {
      activefontsettings.length = 10;
      showtext2("yes",x1 + 50,y1 + starty + 260);
   }
   else
      showtext2("no",x1 + 50,y1 + starty + 260);

   activefontsettings.length = 170;
   strng[0] = 0;
   int k = aktvehicle->weapons.weapon[weapnum].getScalarWeaponType();
      if ( k < cwaffentypennum) {
         strcat( strng, cwaffentypen[k] );
         showtext2( strng, x1 + 50,y1 + starty + 40);
      };

   itoa(aktvehicle->weapons.weapon[weapnum].maxstrength , strng, 10 );
   showtext2(strng ,x1 + 50,y1 + starty + 90);

   itoa(aktvehicle->weapons.weapon[weapnum].count , strng, 10 );
   showtext2(strng ,x1 + 50,y1 + starty + 145);


   bar(x1 + 330,y1 + starty + 40,x1 + 550,y1 + starty + 70,dblue);   /* ## */
   k = 0;
   for (i = 0; i <= 7; i++)
      if (aktvehicle->weapons.weapon[weapnum].sourceheight & (1 << i)) {
         putimage(x1 + 330 + k * 35,y1 + starty + 40,icons.height[i]);
         k++;
      }

   bar(x1 + 330,y1 + starty + 95,x1 + 550,y1 + starty + 125,dblue);   /* ## */
   k = 0;
   for (i = 0; i <= 7; i++)
      if (aktvehicle->weapons.weapon[weapnum].targ & (1 << i) ) {
         putimage(x1 + 330 + k * 35,y1 + starty + 95,icons.height[i]);
         k++;
      }

   delete[]  strng ;
   delete[]  strng2;

   do {
      releasetimeslice();
   }  while (!((mouseparams.taste == 0) || keypress()) );
   mousevisible(true);

   do {
      tdialogbox::run();
   }  while (!((taste != ct_invvalue) || (mouseparams.taste == 1)));
   if (taste == ct_invvalue)
      do {
        releasetimeslice();
      }  while (!(mouseparams.taste == 0));

   mousevisible(false);

}





void         tvehicleinfo::init( const Vehicletype* type )
{

   if ( type )
      aktvehicle = actmap->getvehicletype_byid ( type->id);
   else
      aktvehicle = NULL;

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

   addscrollbar(xsize - 35, starty + 55 , xsize - 20,ysize - 65, &textsizeycomplete, textsizey, &tvt_starty,8,0);
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
   backgrnd2 = lightgray; // dblue + 1;


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
  int         mas;
  int ii;
  char strng[100];

   mas = getmousestatus();
   if (mas == 2)
      mousevisible(false);

   activefontsettings.font = schriften.smallarial;
   activefontsettings.color = textcol;
   activefontsettings.justify = lefttext;

   if (aktvehicle->weapons.count )

      for (ii = 0; ii < aktvehicle->weapons.count ; ii++) {

         if (ii == markweap)
            activefontsettings.background = white;
         else
            activefontsettings.background = dblue;

         strng[0] = 0;

         activefontsettings.color = textcol;
         if ( !aktvehicle->weapons.weapon[ii].shootable() )
            activefontsettings.color = darkgray;

         activefontsettings.length = 190;
         if ( aktvehicle->weapons.weapon[ii].getScalarWeaponType() >= 0 )
            strcat( strng, cwaffentypen[ aktvehicle->weapons.weapon[ii].getScalarWeaponType() ] );
         else
            if ( aktvehicle->weapons.weapon[ii].service() )
               strcat( strng, cwaffentypen[ cwservicen ] );

         showtext2( strng, wepx + 20, wepy + ii * 20);


         activefontsettings.length = 40;
         itoa(aktvehicle->weapons.weapon[ii].maxstrength , strng, 10 );
         showtext2(strng ,wepx + 210, wepy + ii * 20);

         itoa(aktvehicle->weapons.weapon[ii].count , strng, 10 );
         showtext2(strng ,wepx + 250, wepy + ii * 20);

      }

   for (ii = aktvehicle->weapons.count; ii <= 7; ii++)
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

   if ( !aktvehicle->name.empty() )
      showtext2(aktvehicle->name, x1 + 20,y1 + starty + 15);
   else
      showtext2("",x1 + 20,y1 + starty + 15);

   activefontsettings.justify = lefttext;
   if ( !aktvehicle->description.empty() )
      showtext2(aktvehicle->description, x1 + xsize - 260, y1 + starty + 15);
   else
      showtext2("", x1 + xsize - 260, y1 + starty + 15);

   bar(x1 + xsize/2 - 20 ,y1 + starty + 5, x1 + xsize/2 + 20, y1 + starty + 40,backgrnd2);

   aktvehicle->paint( getActiveSurface(), SPoint( x1 + xsize/2 - 20 ,y1 + starty + 5 ), 0 );
/*   TrueColorImage* zimg = zoomimage ( aktvehicle->picture[0], 30, 30, pal, 0 );
   void* pic = convertimage ( zimg, pal ) ;
   putrotspriteimage(x1 + xsize/2 - 15 ,y1 + starty + 7, pic, actmap->actplayer * 8);
   delete pic;
   delete zimg;*/
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
   // showtext2("ascent :", column1x + 10, productioncosty + 280 );


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


   /* ####TRANS
   for (ii = 7; ii >= 0; ii--) {
      if (aktvehicle->loadcapability & (1 << ii) )
         q = icons.height[ii];
      else
         q = xlatpict(xlatpictgraytable,icons.height[ii]);

      putimage(x1 + 155 + ii * 37,y1 + starty + 225,q);

   }
   */


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
   showtext2( strrr(aktvehicle->productionCost.material), productioncostx + 100, productioncosty + 20);
   showtext2( strrr(aktvehicle->productionCost.energy),   productioncostx + 100, productioncosty + 40);

   showtext2( strrr(aktvehicle->tank.material), column1x + 100, productioncosty + 90);
   showtext2( strrr(aktvehicle->tank.energy), column1x + 100, productioncosty + 110);
   showtext2( strrr(aktvehicle->tank.fuel), column1x + 100, productioncosty + 130);
   // ####TRANS showtext2( strrr(aktvehicle->loadcapacity), column1x + 100, productioncosty + 150);
   showtext2( strrr(aktvehicle->fuelConsumption), column1x + 100,  productioncosty + 260 );
   // showtext2( strrr(aktvehicle->steigung), column1x + 100, productioncosty + 280 );


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

   itoa ( aktvehicle->fuelConsumption, strng, 10 );
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
   if (aktvehicle->weapons.count == 0)
      bar(wepx + 10,wepy,wepx + 340,wepy + 20 + wepshown * 20,background);
   else {
      kk = aktvehicle->weapons.count - 1;
      if (kk > wepshown) kk = wepshown;
      for (ii = 0; ii <= kk; ii++) {
         if (aktvehicle->height >= chtieffliegend)
            strcpy (strng,  "air to ");
         else
            strcpy(strng, "ground to ");
         if ((aktvehicle->weapons.weapon[ii].typ & (cwgroundmissileb | cwairmissileb)) == 0)
            strng[0] = 0;
         jj = aktvehicle->weapons.weapon[ii].typ;
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

         itoa ( aktvehicle->weapons.weapon[ii].maxstrength, strng , 10);
         showtext2(strng, wepx + 195,wepy + ii * 20);

         strrd8u ( aktvehicle->weapons.weapon[ii].mindistance, strng );
         showtext2(strng, wepx + 230, wepy + ii * 20);

         strrd8d ( aktvehicle->weapons.weapon[ii].maxdistance, strng );
         showtext2(strng, wepx + 265, wepy + ii * 20);

         itoa ( aktvehicle->weapons.weapon[ii].count, strng ,10);
         showtext2(strng, wepx + 300,wepy + ii * 20);
      }
      if (aktvehicle->weapons.count < 8)
         for (ii = aktvehicle->weapons.count; ii <= wepshown; ii++)
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


   if (aktvehicle->weapons.count) {

      void *q;

      for (ii = 7; ii >= 0; ii--) {
         if (aktvehicle->weapons.weapon[markweap].sourceheight & (1 << ii) )
            q = icons.height[ii];
         else
            q = xlatpict(xlatpictgraytable,icons.height[ii]);

         putimage(wepx + 210 + ii * 35, y1 + starty + 300,q);
      }

      for (ii = 7; ii >= 0; ii--) {
         if (aktvehicle->weapons.weapon[markweap].targ & (1 << ii) )
            q = icons.height[ii];
         else
            q = xlatpict(xlatpictgraytable,icons.height[ii]);

         putimage(wepx + 210 + ii * 35, y1 + starty + 325, q);
      }


      paintsurface2 ( graphx1, graphy1, graphx2, graphy2 );
      rectangle ( graphx1, graphy1, graphx2, graphy2 , lightblue);

      if (aktvehicle->weapons.weapon[markweap].maxstrength > 0)
         if ( aktvehicle->weapons.weapon[markweap].getScalarWeaponType() >= 0 ) {

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

            strrd8u(aktvehicle->weapons.weapon[markweap].mindistance, strng);
            showtext2( strng, graphx1, graphy2 + 5);

            activefontsettings.justify = righttext;
            strrd8d(aktvehicle->weapons.weapon[markweap].maxdistance, strng);
            showtext2( strng, graphx2 - 20, graphy2 + 5);

            activefontsettings.justify = centertext;
            activefontsettings.length = gettextwdth("distance",NULL);

            showtext2("distance",graphx1 + (graphx2 - graphx1  - activefontsettings.length ) / 2, graphy2 + 5);

   /*         activefontsettings.direction = 90;

            showtext2("strength", x1 + 285,y1 + starty + 200);
            activefontsettings.direction = 0;
            activefontsettings.justify = righttext;
            itoa(aktvehicle->weapons.weapon[weapnum].maxstrength, strng, 10 );
            showtext2(strng, x1 + 292,y1 + starty + 137);
            showtext2("0", x1 + 292,y1 + starty + 257); */

            int dx = graphx2 - graphx1;
            int dy = graphy2 - graphy1;

            for (ii = 0; ii <= 255; ii++) {
                xa = graphx1 + ii * dx / 255;
                ya = int ( graphy2 - dy * weapDist.getWeapStrength(&aktvehicle->weapons.weapon[markweap], 0, -1, -1, ii ));
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

      if ((aktvehicle->weapons.weapon[markweap].typ & (cwgroundmissileb | cwairmissileb)) == 0)
         suffix = "";
      */

      activefontsettings.color = black;
      activefontsettings.length = 25;
      activefontsettings.justify = lefttext;

      if ( aktvehicle->weapons.weapon[markweap].shootable() ) {
         showtext2("yes", wepx + 210, wepy + 160);
      } else {
         showtext2("no", wepx + 210, wepy + 160);
      }


      if ( aktvehicle->weapons.weapon[markweap].canRefuel() ) {
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

      itoa(aktvehicle->weapons.weapon[markweap].maxstrength , strng, 10 );
      showtext2(strng ,x1 + 50,y1 + starty + 90);

      itoa(aktvehicle->weapons.weapon[markweap].count , strng, 10 );
      showtext2(strng ,x1 + 50,y1 + starty + 145);


      bar(x1 + 330,y1 + starty + 40,x1 + 550,y1 + starty + 70,dblue);
      k = 0;
      for (i = 0; i <= 7; i++)
         if (aktvehicle->weapons.weapon[markweap].sourceheight & (1 << i)) {
            putimage(x1 + 330 + k * 35,y1 + starty + 40,icons.height[i]);
            k++;
         }

      bar(x1 + 330,y1 + starty + 95,x1 + 550,y1 + starty + 125,dblue);
      k = 0;
      for (i = 0; i <= 7; i++)
         if (aktvehicle->weapons.weapon[markweap].targ & (1 << i) ) {
            putimage(x1 + 330 + k * 35,y1 + starty + 95,icons.height[i]);
            k++;
         }
      */


   /*
            activefontsettings.length =  30;

            itoa ( aktvehicle->weapons.weapon[ii].maxstrength, strng , 10);
            showtext2(strng, wepx + 195,wepy + ii * 20);

            strrd8 ( aktvehicle->weapons.weapon[ii].mindistance, strng );
            showtext2(strng, wepx + 230, wepy + ii * 20);

            strrd8 ( aktvehicle->weapons.weapon[ii].maxdistance, strng );
            showtext2(strng, wepx + 265, wepy + ii * 20);

            itoa ( aktvehicle->weapons.weapon[ii].count, strng ,10);
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

   const char *b;
   if ( !aktvehicle->infotext.empty() )
      b = aktvehicle->infotext.c_str();
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


  #if 0
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
      for (j = 0 ; j < aktvehicle->weapons.count  ; j++ )
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
         for (j = 0 ; j < aktvehicle->weapons.count  ; j++ )
            if ( aktvehicle->weapons.weapon[j].getScalarWeaponType() >= 0 )
               showtext2 ( strrr ( aktvehicle->weapons.weapon[j].maxstrength * aktvehicle->classbound[i].weapstrength[ aktvehicle->weapons.weapon[j].getScalarWeaponType() ] / 1024 ), x1 + 250 + j * 30, y1 + starty + 85 + i * 25 );

        activefontsettings.length = 40;
         showtext2 ( strrr ( aktvehicle->armor * aktvehicle->classbound[i].armor / 1024 ), x1 + 250 + j * 30, y1 + starty + 85 + i * 25 );
      } /* endfor */
      paintsurface ( 25, starty + 85 + i*25, xsize - 25, ysize - 65 );
      paintsurface ( 291 + j * 30, starty + 60, xsize - 25, starty + 85 + i*25 );

      category = 9;
   } else
   #endif
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

const bool vehicleInfoFilterUnits = false;

void         tvehicleinfo::buttonpressed( int id )
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
               i = vehicleTypeRepository.getNum() - 1;
            type = vehicleTypeRepository.getObject_byPos ( i );
         } while ( !type || (ItemFiltrationSystem::isFiltered( ItemFiltrationSystem::Vehicle, type->id ) && vehicleInfoFilterUnits )); /* enddo */
         markweap = 0;
      }

      if (id == 3) {
         do {
            if ( i < vehicleTypeRepository.getNum() - 1 )
               i++;
            else
               i = 0;
            type = vehicleTypeRepository.getObject_byPos ( i );
         } while ( !type || (ItemFiltrationSystem::isFiltered( ItemFiltrationSystem::Vehicle, type->id ) && vehicleInfoFilterUnits )); /* enddo */

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
      aktvehicle = actmap->getvehicletype_bypos ( i );
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
   int         k;

   i = 0;
   if ( !aktvehicle ) {
      pfield fld = actmap->getField( actmap->getCursor());
      if ( !fld )
         return;
      Vehicle* eht = fld->vehicle;
      if ( !fieldvisiblenow( fld ))
         eht = NULL;
      if ( eht ) {
         while (eht->typ != actmap->getvehicletype_bypos ( i ))
            i++;
      }
   } else {
      while ( aktvehicle != actmap->getvehicletype_bypos ( i ))
        i++;
   }

   pvehicletype type = actmap->getvehicletype_bypos ( i );
   while ( !type || (ItemFiltrationSystem::isFiltered( ItemFiltrationSystem::Vehicle, type->id ) && vehicleInfoFilterUnits )) {
      if ( i < vehicleTypeRepository.getNum() - 1 )
         i++;
      else {
         displaymessage ( "no vehicle type to display; check unitset filters ", 1);
         return;
      }
      type = actmap->getvehicletype_bypos ( i );
   } ;


   j = i;

   aktvehicle = actmap->getvehicletype_bypos ( i );
   zeigevehicle();
   showgeneralinfos();

   mousevisible(true);
   do {

      tdialogbox::run();

      if (category == 1) {
         if (mouseparams.taste == 1)
            if ((mouseparams.x > wepx + 10) && (mouseparams.x < wepx + 340))
               if (aktvehicle->weapons.count > 0)
                  for (k = 0; k < aktvehicle->weapons.count ; k++)
                     if ((mouseparams.y > wepy + k * 20 - 3) && (mouseparams.y < wepy + 17 + k * 20)) {
                        if (k != markweap ) {
                           markweap = k ;
                           mousevisible(false);
                           paintmarkweap();
                           mousevisible(true);
                        }
                     }


         if (taste == ct_up)
            if (aktvehicle->weapons.count > 0) {
               markweap--;
               if (markweap < 0)
                  markweap = aktvehicle->weapons.count-1;
               paintmarkweap();
            }
         if (taste == ct_down)
            if (aktvehicle->weapons.count ) {
               markweap++;
               if (markweap >= aktvehicle->weapons.count)
                  markweap = 0;
               paintmarkweap(); 
            } 
      }
      if ( category == 4 )
         checkscrolling ( );


   }  while ( (taste != ct_esc) && (action < 20));
} 

void         vehicle_information( const Vehicletype* type )
{ 
  tvehicleinfo eif; 
   eif.init( type ); 
   eif.run(); 
   eif.done(); 
}


map<int,ASCString> messageStrings;


#define klickconst 100
#define delayconst 10


void   loadsinglemessagefile ( const char* name )
{
   try {
      tnfilestream stream ( name, tnstream::reading );

      ASCString s1, s2;
      s1 = stream.readString();
      s2 = stream.readString();

      while ( !s1.empty() && !s2.empty() ) {
         int w = atoi ( s1.c_str() );

         messageStrings[w] = s2;

         s1 = stream.readString();
         s2 = stream.readString();
      }
   }
   catch ( treadafterend ) {
   }
}


void         loadmessages(void)
{
   tfindfile ff ( "message?.txt" );
   ASCString filename = ff.getnextname();

   while( !filename.empty() ) {
      loadsinglemessagefile ( filename.c_str() );
      filename = ff.getnextname();
   }
}


const char*        getmessage( int id)
{
   if ( messageStrings.find ( id ) != messageStrings.end() )
         return messageStrings[id].c_str();

   static const char* notfound = "message not found";
   return notfound;
}



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


int         choice_dlg( const char *       title,
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




class   tfileselectsvga : public tdialogbox {
                   public:
                       tfileselectsvga() : markedfile(-1) {};
                       
                       class tfiledata {
                                   public:
                                      ASCString        name;
                                      ASCString        sdate;
                                      int              time;
                                      ASCString        location;
                                   } ;

                        vector<tfiledata>  files;
                        
                        char            ausgabeaborted;
                        ASCString       searchstring;
                        int             numberoffiles;
                        char            mousebuttonreleased;
                        int             lastscrollbarposition;

                        int             actdispc[shownfilenumber];
                        int             actdispn[shownfilenumber];
                                                 
                        char            abrt;
                        int             actshownfilenum;
                        int             firstshownfile;
                        int             markedfile;

                        ASCString       wildcard;
                        char            swtch;           /* 1: load        2: save */
                        ASCString*      result;
                        bool         sort_name;
                        bool         sort_time;

                        void            init( char sw );
                        void            setdata( const ASCString& _wildcard, ASCString* _result );   // result
                        
                        void            readdirectory ( void );
                        void            fileausgabe( char force , int dispscrollbar);
                        void            sortentries ( void );
                        virtual void    run ( void );
                        
                        char            speedsearch( char input );
                        void            checkfsf( char lock);   
                        void            displayspeedsearch ( void );

                        virtual void    buttonpressed(int id);
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




void         tfileselectsvga::buttonpressed( int id)
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

   tfindfile ff ( wildcard );

   ASCString location;
   string filename = ff.getnextname(NULL, NULL, &location );

   while( !filename.empty() ) {
      tfiledata f;
      f.name = filename.c_str();

      time_t tdate = get_filetime( filename.c_str() );
      f.time = tdate;
      if ( tdate != -1 )
         f.sdate = ctime ( &tdate );

      f.location = location;

      files.push_back ( f );

      numberoffiles++;

      filename = ff.getnextname(NULL, NULL, &location );
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
   if (lastshownfile >= numberoffiles)
      lastshownfile = numberoffiles - 1;

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
            showtext2( files[ii].name.c_str() ,x1 + 15,y1 + starty + jj * 20 + 20);

            activefontsettings.length = 200;
            activefontsettings.justify = righttext;
            if ( !files[ii].sdate.empty() )
               showtext2( files[ii].sdate.c_str(), x1 + 145,y1 + starty + jj * 20 + 20);
            else 
               bar ( x1 + 145,y1 + starty + jj * 20 + 20, x1 + 145 + activefontsettings.length, y1 + starty + jj * 20 + 20 + activefontsettings.font->height, activefontsettings.background );

            activefontsettings.justify = lefttext;
            activefontsettings.length = 230;
            // if ( files[ii].location ) {
              showtext2( files[ii].location.c_str(), x1 + 360,y1 + starty + jj * 20 + 20);
            ///} else
            //   bar ( x1 + 360,y1 + starty + jj * 20 + 20, x1 + 360 + activefontsettings.length, y1 + starty + jj * 20 + 20 + activefontsettings.font->height, activefontsettings.background );
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
      if ((sort_name == true  && files[ii].name > files[ii+1].name ) ||
          (sort_name == false && files[ii].time < files[ii+1].time )) {
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
   searchstring += toupper( input );

   bool sps = false;
   int ii = markedfile;
   if ( ii >= 0  && searchstring.compare_ci ( 0, searchstring.length(), files[ii].name ) == 0) {
      sps = true;
   }
   else {
      ii = 0;
      while (ii < numberoffiles  &&  !sps ) {
         if ( files[ii].name.compare_ci ( 0, searchstring.length(), searchstring ) == 0) {
            sps = true;
            markedfile = ii;
         } 
         ii++;
      } 
   } 

   if ( !sps   &&   swtch == 1)
     searchstring.erase ( searchstring.length()-1, 1 );

   displayspeedsearch();

   return sps;
} 


void         tfileselectsvga::displayspeedsearch(void)
{ 
   collategraphicoperations cgo ( x1 + 225, y1 + ysize - 30, x1 + 225 + 190, y1 + ysize );

   mousevisible(false);
   npush ( activefontsettings );
   activefontsettings.length = 190;
   showtext2(searchstring.c_str(),x1 + 225, y1 + ysize - 30);

   if ( ticker & 128 ) {
      int x = gettextwdth ( searchstring.c_str(), activefontsettings.font );
      line ( x1 + 225 + x, y1 + ysize - 30, x1 + 225 + x, y1 + ysize - 30 + activefontsettings.font->height-1, black);
   }


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

   activefontsettings.background = dblue; 

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
         addkey ( 2, ct_esc );
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
            if (  ( swtch == 1 && ( markedfile >= 0) ) || ( swtch == 0) && !searchstring.empty()  )
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
            else
               if ( prntkey != '.' ) {
                  searchstring += prntkey;
                  displayspeedsearch();
               }
         } 
         if ( prntkey == cto_bspace  &&  !searchstring.empty() ) {   /* l”schen des letzten zeichens von searchstring */
            searchstring.erase ( searchstring.length()-1 , 1);
            displayspeedsearch(); 
         } 
         if (prntkey == cto_esc ) {   /*  abbrechen / searchstring zur?cksetze  */
            if ((swtch == 1 ) || searchstring.empty() )
               abrt = 1; 
            else
               if ( !searchstring.empty() )
                  searchstring.erase();

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
         if ( ticker & 64 )
            displayspeedsearch();
      }  while ( abrt == 0);
      mousevisible(false); 
      if (abrt > 1) { 
         if ( ( markedfile >= 0) || !searchstring.empty() ) {
            if ( (swtch == 1)  ||   (abrt == 3)   || searchstring.empty() ) {  // load
               *result = files[markedfile].name ;
            } else {
               ASCString extension = wildcard;
               extension.erase ( 0, 1 );
               *result = searchstring + extension;
            }
         } else
             result->erase();
      } else
          result->erase();

      mousevisible(true); 

   } else
      result->erase();

}




void         tfileselectsvga::setdata( const ASCString& _wildcard, ASCString* _result )
{ 
   wildcard = _wildcard;
   result = _result;
} 


void         fileselectsvga( const ASCString& ext, ASCString& filename, bool load )
{                           
   tfileselectsvga tss; 

   tss.init( load );
   tss.setdata( ext, &filename );
   tss.run();          
   tss.done(); 
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

                       void             init( char plyexistyte, char md);
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


void         tenternamestrings::init(  char plyexist, char md )
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




void viewVisibilityStatistics()
{
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


   tviewanytext vat;
   vat.init ( "Visibility Statistics", msg.c_str() );
   vat.run();
   vat.done();
}









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
   addbutton("~n~etwork",400,160,xsize - 20,190,0,1,5, actmap->network != NULL ); 
   #endif

   int lastcomppos = 0;
   int plnum = 0;
   int i, j;

   for (i = 0; i <= 7; i++) { 

      if ( actmap->player[i].exist() )  // ((playermode[i] == ps_human) || (playermode[i] == ps_computer)) &&
         playerpos[plnum++] = i;

      if ( actmap->player[i].exist() && actmap->network ) {
         location[i] = actmap->network->player[i].compposition;
         lastcomppos = location[i];
      } else
         location[i] = lastcomppos;

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
  int i, j;

  if ( actmap->network ) {
     for ( int i = 0; i < 8; i++ )
         actmap->network->computer[i].existent = 0;
  }
  for (i = 0; i < 8; i++) {

      for (j = 0; j < 8; j++)
         actmap->alliances[i][j] = alliancedata[i][j];

      if ( actmap->player[i].exist() ) {
         if ( actmap->network )
            actmap->network->computer[ location[i] ].existent = 1;
      }

      if ( actmap->network ) {
         if ( actmap->network->computer[i].name )
            delete[]  actmap->network->computer[i].name ;

         string s = "computer ";
         s += strrr( i );
         actmap->network->computer[i].name = strdup ( s.c_str() );
         actmap->network->player[i].compposition = location[i];
      }

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
              setupnetwork ( actmap->network );
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


                    ASCString filename;
                    fileselectsvga(mapextension, filename, false);
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











void tprogressbar :: start ( int _color, int _x1, int _y1, int _x2, int _y2, pnstream stream )
{
  ended = 0;
   x1 = _x1;
   y1 = _y1;
   x2 = _x2;
   y2 = _y2;
   color = _color;

   if ( stream ) {
      int ver  = stream->readInt();
      groupnum = stream->readInt();
      for ( int i = 0; i < groupnum; i++ ) {
         group[i].orgnum = stream->readInt();
         group[i].time   = stream->readInt();
         group[i].newtime = stream->readInt();
         group[i].timefromstart = stream->readInt();
      }
      time = stream->readInt();
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
      /*
      for ( int x = lastpaintedpos+1; x <= newpos; x++ )
         for ( int y = y1; y <= y2; y++ )
            putpixel ( x, y, color );
*/
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
   stream->writeInt ( ver );
   stream->writeInt ( groupnum );
   for ( int i = 0; i < groupnum; i++ ) {
      stream->writeInt ( group[i].orgnum );
      stream->writeInt ( group[i].time );
      stream->writeInt ( group[i].newtime );
      stream->writeInt ( group[i].timefromstart );
   }
   stream->writeInt ( time );
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
   pfield fld = actmap->getField(actmap->getCursor());
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

void viewterraininfo ( void )
{
   pfield fld = actmap->getField( actmap->getCursor() );
   if ( fld && fieldvisiblenow  ( fld )) {
      const char* terraininfo = "#font02#Field Information (%d,%d)#font01##aeinzug20##eeinzug20##crtp10#"
                                            "ID: %d\n"
                                            "attack bonus: %.1f\n"
                                            "defense bonus: %.1f\n"
                                            "base jamming: %d\n"
                                            "terrain filename: %s#aeinzug0##eeinzug0#\n\n"
                                            "#font02#Terrain properties:#font01##aeinzug20##eeinzug20##crtp10#";

      char text[10000];


      float ab = fld->getattackbonus();
      float db = fld->getdefensebonus();

      sprintf(text, terraininfo, actmap->getCursor().x, actmap->getCursor().y, fld->typ->terraintype->id, ab/8, db/8, fld->getjamming(), fld->typ->terraintype->location.c_str() );

      appendTerrainBits ( text, &fld->bdt );

      strcat ( text, "#aeinzug0##eeinzug0#\n\n"
                     "#font02#Movemali:#font01##aeinzug20##eeinzug20##crtp10#" );

      for ( int i = 0; i < cmovemalitypenum; i++ ) {
         char t2[1000];
         if ( fld->vehicle && fld->vehicle->typ->movemalustyp == i )
            strcat ( text, "#color1#");
         sprintf(t2, "%s: %d\n",  cmovemalitypes[i], fld->getmovemalus(i) );
         strcat ( text, t2 );
         if ( fld->vehicle && fld->vehicle->typ->movemalustyp == i )
            strcat ( text, "#color0#");
      }

      int mines[4] = { 0, 0, 0, 0 };
      int mineDissolve[4] = { maxint, maxint, maxint, maxint };

      for ( tfield::MineContainer::iterator m = fld->mines.begin(); m != fld->mines.end(); ++m )
         if ( m->player == actmap->actplayer || fieldVisibility  ( fld ) == visible_all ) {
            mines[m->type-1]++;
            int lifetime = actmap->getgameparameter( GameParameter(cgp_antipersonnelmine_lifetime + m->type-1 ));
            if ( lifetime > 0)
               mineDissolve[m->type-1] = min( m->lifetimer, mineDissolve[m->type-1]);
         }

      if ( mines[0] || mines[1] || mines[2] || mines[3] ) {
         strcat ( text, "#aeinzug0##eeinzug0#\n\n"
                        "#font02#Mine Information:#font01##aeinzug20##eeinzug20##crtp10#"
                        "On this field are #aeinzug50# \n" );

         for ( int i = 0; i < 4; i++ ) {
            strcat ( text, " " );
            strcat ( text, strrr ( mines[i] ));
            strcat ( text, " ");
            strcat ( text, MineNames[i] );
            strcat ( text, "(s). " );
            if ( mineDissolve[i] >= 0 && mineDissolve[i] < maxint ) {
               strcat ( text, "Next mine will dissolve in " );
               strcat ( text, strrr( mineDissolve[i]));
               strcat ( text, " turns.");
            }
            strcat ( text, "\n" );
         }
         strcat ( text, "#aeinzug0#\n");
      }

      if  ( fld->vehicle ) {
         strcat ( text, "#aeinzug0##eeinzug0#\n\n"
                        "#font02#Vehicle Information:#font01##aeinzug20##eeinzug20##crtp10#" );

         const Vehicletype* typ = fld->vehicle->typ;


         strcat ( text, "Unit name: " );
         if ( !typ->name.empty() )
            strcat( text, typ->name.c_str() );
         else
            if ( !typ->description.empty() )
               strcat( text, typ->description.c_str() );

         char t3[1000];
         sprintf(t3, "\nUnit ID: %d \n", typ->id );
         strcat ( text, t3 );

         sprintf(t3, "\nInternal Identification: %d \n", fld->vehicle->networkid );
         strcat ( text, t3 );


         if ( !typ->filename.empty() ) {
            sprintf(t3, "file name: %s\n", typ->filename.c_str() );
            strcat ( text, t3 );
         }

         strcat ( text, "the unit can drive onto the following fields:\n" );
         appendTerrainBits ( text, &typ->terrainaccess.terrain );

         strcat ( text, "\n\nthese bits must be set:\n" );
         appendTerrainBits ( text, &typ->terrainaccess.terrainreq );

         strcat ( text, "\n\nthese bits must NOT be set:\n" );
         appendTerrainBits ( text, &typ->terrainaccess.terrainnot );

         strcat ( text, "\n\nthe unit ist killed by:\n" );
         appendTerrainBits ( text, &typ->terrainaccess.terrainkill );
      }

      if ( !fld->objects.empty() )
         strcat ( text, "#aeinzug0##eeinzug0#\n\n"
                        "#font02#Object Information:#font01##aeinzug20##eeinzug20##crtp10#" );

      for ( tfield::ObjectContainer::iterator i = fld->objects.begin(); i != fld->objects.end(); i++ ) {
         strcat ( text, i->typ->location.c_str() );
         strcat ( text, "\n" );
      }


      if ( fld->building ) {
         strcat ( text, "#aeinzug0##eeinzug0#\n\n"
                        "#font02#Building Information:#font01##aeinzug20##eeinzug20##crtp10#" );
         strcat ( text, fld->building->typ->location.c_str() );
         strcat ( text, "#crtp10#owner: " );
         if ( fld->building->getOwner() < 8 )
            strcat ( text, actmap->player[fld->building->getOwner()].getName().c_str() );
         else
            strcat ( text, "neutral" );
      }


      tviewanytext vat ;
      vat.init ( "Field information", text, 20, -1 , 450, 480 );
      vat.run();
      vat.done();

   }
}

void viewUnitSetinfo ( void )
{
   ASCString s;
   pfield fld = actmap->getField( actmap->getCursor() );
   if ( fld && fieldvisiblenow  ( fld ) && fld->vehicle ) {

         s += "#aeinzug0##eeinzug0#\n"
              "#font02#Unit Information:#font01##aeinzug20##eeinzug20##crtp10#" ;

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

         s += "\nUnit owner: ";
         s += strrr ( fld->vehicle->color / 8 );
         s += " - ";
         s += actmap->getPlayerName ( fld->vehicle->color/8 );

         char t3[1000];
         sprintf(t3, "\nUnit ID: %d \n", typ->id );
         s += t3;

         if ( !typ->location.empty() ) {
            sprintf(t3, "file name: %s\n\n", typ->location.c_str() );
            s += t3;
         }

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

   while ( s.find ( "@" ) != std::string::npos )
      s.replace ( s.find ( "@" ), 1, "(at)"); // the default font has not @ character

   tviewanytext vat;
   vat.init ( "Unit information", s.c_str() );
   vat.run();
   vat.done();
}







class   tgameparamsel : public tstringselect {
           public :
                 int lastchoice;
                 virtual void setup(void);
                 virtual void buttonpressed(int id);
                 virtual void run(void);
                 virtual void get_text( int nr);
                 };

void         tgameparamsel ::setup(void)
{

   action = 0;
   title = "Select Gameparameter";
   numberoflines = gameparameternum;
   ey = ysize - 90;
   startpos = lastchoice;
   addbutton("~O~k",20,ysize - 50,xsize-20,ysize - 20,0,1,13,true);
   addbutton("~E~dit Selected",20,ysize - 80,xsize/3-5,ysize - 60,0,1,12,true);
   addbutton("~D~escription",  xsize/3+5,ysize - 80,xsize/3*2-5,ysize - 60,0,1,14,true);
   addbutton("~R~eset all",xsize/3*2+5,ysize - 80,xsize-20,ysize - 60,0,1,17,true);
   addkey ( 13, ct_esc );
   addkey ( 14, ct_f1 );
}


void         tgameparamsel ::buttonpressed(int         id)
{
   tstringselect::buttonpressed(id);
   switch (id) {

      case 12:  if ( redline >= 0 )
                   action = id-10;
                break;

      case 13:   action = id-10;
                 break;

      case 14:   if ( redline >= 0)
                    help ( 800+redline);
                 else
                    displaymessage ( "Please select an entry first", 3);
                 break;
      case 17:   delete actmap->game_parameter;
                 actmap->game_parameter = NULL;
                 actmap->gameparameter_num = 0;
                 viewtext();
                 break;
   }
}


void         tgameparamsel ::get_text( int nr)
{
   strcpy(txt,gameparametername[nr] );
   strcat(txt, " (" );
   strcat(txt, strrr ( actmap->getgameparameter( GameParameter(nr) )));
   strcat(txt,") ");
}


void         tgameparamsel ::run(void)
{
   do {
      tstringselect::run();
      if ( taste == ct_enter )
         if ( redline >= 0 )
            action = 2;
         else
            action = 3;
   }  while ( action == 0 );
   if ( action == 3 )
      redline = -1;
}


int selectgameparameter( int lc )
{
  tgameparamsel  gps;

   gps.lastchoice = lc;
   gps.init();
   gps.run();
   gps.done();
   return gps.redline;
}


void setmapparameters ( void )
{
   int param;
   do {
      param = selectgameparameter( -1 );
      if ( param >= 0 && param < gameparameternum )
         actmap->setgameparameter( GameParameter(param) , getid("Parameter Val",actmap->getgameparameter( GameParameter(param) ), gameParameterLowerLimit[param], gameParameterUpperLimit[param] ));
   } while ( param >= 0 && param < gameparameternum );
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
   assert ( c >= 0 );
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

     if ( font->character[text[pos]].size ) {
        length += font->character[text[pos]].width + 2 ;
        if ( pos )
           length += font->kerning[text[pos]][text[pos-1]];
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

        if ( font->character[text[pos]].size ) {
             length += font->character[text[pos]].width + 2 ;
             if ( pos )
                length += font->kerning[text[pos]][text[pos-1]];
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
   ASCString filename;
   fileselectsvga("*.gfx", filename, true );
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


void longWarning ( const ASCString& string )
{
   tviewanytext vat ;
   vat.init ( "Warning", string.c_str(), 20, -1 , 450, 480 );
   vat.run();
   vat.done();
}

