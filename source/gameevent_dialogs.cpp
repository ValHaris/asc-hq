/*! \file gameevent_dialogs.cpp
    \brief dialogs for the game event system
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

#include "global.h"
#include "buildingtype.h"
#include "vehicletype.h"
#include "newfont.h"
#include "typen.h"
#include "basegfx.h"
#include "dlg_box.h"
#include "newfont.h"
#include "vehicletype.h"
#include "buildingtype.h"
#include "spfst.h"

#include "gameevents.h"
#include "events.h"
#include "stack.h"
#include "gameoptions.h"
#include "loadimage.h"
#include "errors.h"
#include "itemrepository.h"
#include "gameevent_dialogs.h"
#include "dialog.h"


#ifdef karteneditor
#include "edmisc.h"
#include "edselfnt.h"
extern int  selectfield(int * cx ,int  * cy);
#else
int  selectfield(int * cx ,int  * cy)
{
  return 0;
}
void selweather( tkey ench ){};
void editpolygon (Poly_gon& poly) {};
#endif

// õS GetXYSel

bool chooseWeather( int& weather )
{
#ifdef karteneditor
  auswahlw = weather;
  selweather( ct_invvalue );
  weather = auswahlw;
#endif
  return true;
}

bool chooseTerrain( int& terrainID )
{
#ifdef karteneditor
  auswahl = actmap->getterraintype_byid( terrainID );
  selterraintype( ct_invvalue );
  terrainID = auswahl->id;
#endif
  return true;
}

bool chooseObject( int& objectID )
{
#ifdef karteneditor
  actobject  = actmap->getobjecttype_byid( objectID );
  selobject( ct_invvalue );
  objectID = actobject->id;
#endif
  return true;
}

bool chooseVehicleType( int& vehicleTypeID )
{
#ifdef karteneditor
  auswahlf  = actmap->getvehicletype_byid( vehicleTypeID );
  selvehicletype( ct_invvalue );
  vehicleTypeID = auswahlf->id;
#endif
  return true;
}


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
                     if ( !selectfield(&x,&y) )
                        break;
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


void selectFields( FieldAddressing::Fields& fields )
{
   typedef vector<MapCoordinate>::iterator It;
   vector<MapCoordinate> backup = fields;

   actmap->cleartemps();
   for ( It i = fields.begin(); i != fields.end(); ++i )
      actmap->getField(*i)->a.temp = 1;

   displaymap();
   displaymessage("use space to select the fields\nfinish the selection by pressing enter",3);
   int res ;
   do {
      int x,y;
      res = selectfield(&x,&y);
      if ( res == 2 ) {
         It i = find ( fields.begin(), fields.end(), MapCoordinate (x,y) );
         if ( i != fields.end() ) {
            actmap->getField(*i)->a.temp = 0;
            fields.erase( i ); 
         } else {
            actmap->getField(x,y)->a.temp = 1;
            fields.push_back ( MapCoordinate(x,y));
         }
         displaymap();
      }

   } while ( res == 2 ); /* enddo */

   if ( res == 0 )
      fields = backup;

   actmap->cleartemps();
   displaymap();
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



NewVehicleTypeDetection::NewVehicleTypeDetection(  )
{
   buf = new bool[ vehicleTypeRepository.getNum() ];

   for ( int i=0; i < vehicleTypeRepository.getNum() ; i++ )
      buf[i] = actmap->player[ actmap->actplayer ].research.vehicletypeavailable ( vehicleTypeRepository.getObject_byPos ( i ) );
}



void    NewVehicleTypeDetection::evalbuffer( void )
{
   int num = 0;
   for ( int i=0; i < vehicleTypeRepository.getNum() ;i++ ) {
      if (buf[i] == 0) {
          buf[i] = actmap->player[ actmap->actplayer ].research.vehicletypeavailable ( vehicleTypeRepository.getObject_byPos ( i ) );
          if ( buf[i] )
             num++;
      } else
          buf[i] = 0;

   }

   if ( num ) {
      tshownewtanks snt;
      snt.init ( buf );
      snt.run  ();
      snt.done ();
   }

}

NewVehicleTypeDetection::~NewVehicleTypeDetection()
{
   delete[] buf ;
}

#if 0

 class   tshowtechnology : public tdialogbox {
               public:
                  const Technology*       tech;
                  void              init( const Technology* acttech );
                  virtual void      run ( void );
                  void              showtec ( void );
         };


void         tshowtechnology::init(  const Technology* acttech  )
{ 
   tdialogbox::init();
   title = "new technology";
   buildgraphics();
   tech = acttech;
} 


void         tshowtechnology::showtec(void)
{ 
   const char         *wort1, *wort2;
   const char         *pc, *w2;
   word         xp, yp, w;

   activefontsettings.font = schriften.large; 
   activefontsettings.justify = centertext;
   activefontsettings.length = xsize - 40;
   showtext2(tech->name,x1 + 20,y1 + starty + 10); 
   activefontsettings.justify = lefttext; 

   yp = 60; 

   if (tech->icon) {
      int xs,ys;
      getpicsize ( (trleheader*) tech->icon, xs, ys );
      putimage ( x1 + ( xsize - xs) / 2 , y1 + starty + 45 , tech->icon );
      yp += ys + 10;
   }



   wort1 = new char[100];
   wort2 = new char[100];
   strcpy( wort1, "research points: " );
   itoa ( tech->researchpoints, wort2, 10 );
   strcat( wort1, wort2 );

   activefontsettings.font = schriften.smallarial; 
   showtext2(wort1, x1 + 30,y1 + yp);

   if ( !tech->infotext.empty()) {
      activefontsettings.color = black; 
      xp = 0; 
      pc = tech->infotext.c_str();
      while (*pc ) {
         w2 = wort1;
         while ( *pc  && *pc != ' ' && *pc != '-' ) {
            *w2 = *pc;
            w2++;
            pc++;
         };
         *w2 = *pc;
         if (*pc) {
            w2++;
            pc++;
         }
         *w2=0;
         
         w = gettextwdth(wort1,NULL);
         if (xp + w > xsize - 40) { 
            yp = yp + 5 + activefontsettings.font->height; 
            xp = 0; 
         } 
         showtext2(wort1,x1 + xp + 20,y1 + starty + yp);
         xp += w;
      }
   }
   delete[] wort1 ;
   delete[] wort2 ;
}



void         tshowtechnology::run(void)
{
   showtec();
   do {
      tdialogbox::run();
   }  while ( (taste != ct_esc) && (taste != ct_space) && (taste != ct_enter) );
}

#endif


void         showtechnology(const Technology*  tech )
{
   if ( tech ) {
   #if 0
      if ( tech->pictfilename ) {
         mousevisible(false);
         bar ( 0,0, agmp->resolutionx-1, agmp->resolutiony-1, black );
         activefontsettings.length = agmp->resolutionx - 40;
         activefontsettings.justify = centertext;
         activefontsettings.background = 255;
         activefontsettings.height = 0;
         activefontsettings.font = schriften.large;
         activefontsettings.color = white;
         showtext2 ( "A new technology" , 20, 200 );
         showtext2 ( "has been discovered", 20, 280 );
         int t = ticker;
         while ( mouseparams.taste )
            releasetimeslice();
         do {
            releasetimeslice();
         } while ( t + 200 > ticker  &&  !keypress()  && !mouseparams.taste);

         int abrt = 0;
         while ( keypress() )
           r_key();


         int fs = loadFullscreenImage ( tech->pictfilename );
         if ( fs ) {

            t = ticker;
            while ( mouseparams.taste )
               releasetimeslice();

            do {
               releasetimeslice();
            } while ( t + 600 > ticker  &&  !keypress()  && !mouseparams.taste && !abrt ); /* enddo */

            closeFullscreenImage();
         }
         activefontsettings.length = agmp->resolutionx - 40;
         activefontsettings.justify = centertext;
         activefontsettings.background = 255;
         activefontsettings.height = 0;
         activefontsettings.font = schriften.large;
         activefontsettings.color = white;
         bar ( 0, 0, agmp->resolutionx-1, agmp->resolutiony-1, 0 );
         showtext2 ( tech->name, 20, 20 );

         if ( tech->infotext ) {
            tviewtext vt;
            vt.setparams ( 20, 50, agmp->resolutionx - 20, agmp->resolutiony - 20, tech->infotext, white, black );
            vt.tvt_dispactive = 0;
            vt.displaytext ();

            int textsizeycomplete = vt.tvt_yp;
            int textsizey = agmp->resolutiony - 70 ;

            vt.tvt_dispactive = 1;
            vt.displaytext ();
   
            abrt = 0;
            int scrollspeed = 10;
            do {
               tkey taste = r_key();

               if ( textsizeycomplete > textsizey ) {
                  int oldstarty = vt.tvt_starty;
                  if ( taste == ct_down ) 
                     if ( vt.tvt_starty + textsizey + scrollspeed < textsizeycomplete )
                        vt.tvt_starty += scrollspeed;
                     else
                         vt.tvt_starty = textsizeycomplete - textsizey;

                  if ( taste == ct_up ) 
                     if ( vt.tvt_starty - scrollspeed > 0 )
                        vt.tvt_starty -= scrollspeed;
                     else
                         vt.tvt_starty = 0;

                  if ( oldstarty != vt.tvt_starty )
                      vt.displaytext();

               }

               if ( taste == ct_esc || taste == ct_enter || taste == ct_space )
                  abrt = 1;

            } while ( !abrt ); /* enddo */

            // repaintdisplay();
         }
      } else {
   #endif
         //ASCString text = "A new technology has been researched:\n#font02#";
         ASCString text = "#font02#";
         text += tech->name;
         text += "#font01#\n";
         text += tech->infotext;
       tviewanytext vat;
       vat.init ( "new technology", text.c_str() );
       vat.run();
       vat.done();
//      }
   }
}


void  tshownewtanks :: init ( bool*      buf2 )
{
   tdialogbox::init();

   title = "new units available";
   xsize = 400;
   x1 = 120;
   y1 = 100;
   ysize = 280;

   addbutton("~o~k", 10, ysize - 35, xsize - 10, ysize - 10, 0, 1, 1 , true );
   addkey(1, ct_enter);
   addkey(1, ct_space);

   buildgraphics();

   activefontsettings.font = schriften.smallarial;
   activefontsettings.color = black;
   activefontsettings.background = 255;
   activefontsettings.justify = lefttext;
   activefontsettings.length = xsize - 80;

   buf = buf2;
   int i, num = 0;
   for (i=0; i < vehicleTypeRepository.getNum() ;i++ ) {
      if ( buf[i] ) {
         pvehicletype tnk = vehicleTypeRepository.getObject_byPos ( i );
         if ( tnk ) {
            bar ( x1 + 25, y1 + 45 + num * 50, x1 + 65, y1 + 85 + num * 50, dblue );
            putrotspriteimage (  x1 + 30, y1 + 50 + num * 50, tnk -> picture[0] , actmap->actplayer * 8);
            showtext2( tnk -> name, x1 + 70, y1 + 45 + num * 50 );
            showtext2( tnk -> description, x1 + 70, y1 + 45 + 40 + num * 50 - activefontsettings.font->height );
            num++;
         }
      }

   } /* endfor */
}

void  tshownewtanks :: run ( void )
{
   status = 0;
   mousevisible(true);
   do {
      tdialogbox :: run ();
   } while ( status == 0 ); /* enddo */
}

void  tshownewtanks :: buttonpressed ( int id )
{
   if (id == 1)
      status = 1;
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

       pfield pf2;

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

//   addbutton("~A~ll not allied",20,ysize - 40,170,ysize - 20,0,1,1,true);
//   addkey(1,ct_enter);
   addbutton("~O~K",200,ysize - 40,350,ysize - 20,0,1,2,true);

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
      if ( playerbit & ( 1 << i ) ) 
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
      // if (taste == ct_f1) help ( 1060 );
   }  while (!((taste == ct_esc) || ((action == 1) || (action ==2))));
   if ((action == 1) || (taste == ct_esc))
      playerbit = 1;
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
            playerbit ^=  1 << ( id-6 ) ;
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


