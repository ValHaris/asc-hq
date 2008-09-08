/** \file edmisc.cpp
    \brief various functions for the mapeditor
*/

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

// #include <QClipbrd.hpp>

#include <string>
#include <iostream>
#include <math.h>

#include "sdl/graphics.h"
#include "vehicletype.h"
#include "buildingtype.h"
#include "edmisc.h"
#include "loadbi3.h"
#include "edgen.h"
#include "edselfnt.h"
#include "edglobal.h"
#include "dialogs/pwd_dlg.h"
#include "mapdisplay.h"
#include "graphicset.h"
#include "itemrepository.h"
#include "textfileparser.h"
#include "textfile_evaluation.h"
#include "textfiletags.h"
#include "clipboard.h"
#include "dialogs/cargowidget.h"
#include "dialogs/fieldmarker.h"
#include "dialogs/newmap.h"
#include "stack.h"

#include "unitset.h"
#include "maped-mainscreen.h"
#include "gameevents.h"
#include "gameevent_dialogs.h"
#include "dialogs/fileselector.h"
#include "pgeventsupplier.h"

   bool       mapsaved;



void placeCurrentItem()
{
   if ( selection.getSelection() && actmap->getCursor().valid() ) {
      if ( selection.getSelection()->supportMultiFieldPlacement()  && selection.brushSize > 1 ) {
         circularFieldIterator( actmap, actmap->getCursor(), 0, selection.brushSize-1, FieldIterationFunctor( selection.getSelection(), &MapComponent::vPlace ));
      } else {
         selection.getSelection()->place( actmap->getCursor() );
      }
      mapChanged( actmap );
   }    
}

bool removeCurrentItem()
{
   if ( selection.getSelection() && actmap->getCursor().valid() ) 
      return selection.getSelection()->remove( actmap->getCursor() );
   else
      return false;
}

   
bool mousePressedOnField( const MapCoordinate& pos, const SPoint& mousePos, bool cursorChanged, int button, int prio )
{
   if ( prio > 1 )
      return false;
   
   if ( button == 1 ) {
      execaction_ev( act_primaryAction );
      return true;
   } else
      return false;
}

bool mouseDraggedToField( const MapCoordinate& pos, const SPoint& mousePos, bool cursorChanged, int prio )
{
   if ( prio > 1 )
      return false;
   execaction_ev( act_primaryAction );
   return true;
}
   
   
// � Checkobject

char checkobject(tfield* pf)
{
   return !pf->objects.empty();
}



// � PutResource

void tputresources :: init ( int sx, int sy, int dst, int restype, int resmax, int resmin )
{
   centerPos = MapCoordinate(sx,sy);
   initsearch( centerPos, dst, 0);
   resourcetype = restype;
   maxresource = resmax;
   minresource = resmin;
   maxdst = dst;
   startsearch();
}

void tputresources :: testfield ( const MapCoordinate& mc )
{
   int dist = beeline ( mc, centerPos ) / 10;
   int m = maxresource - dist * ( maxresource - minresource ) / maxdst;

   tfield* fld = gamemap->getField ( mc );
   if ( resourcetype == 1 )
      fld->material = min( 255, fld->material + m);
   else
      fld->fuel = min( 255, fld->fuel + m );
}

void tputresourcesdlg :: init ( void )
{
   resourcetype = 1;
   restp2 = 0;
   status = 0;
   tdialogbox::init();
   title = "set resources";
   xsize = 400;
   ysize = 300;
   maxresource = 200;
   minresource = 0;
   dist = 10;

   addbutton ( "~O~k", 10, ysize - 30, xsize/2 - 5, ysize - 10 , 0, 1, 1, true );
   addkey ( 1, ct_enter );
   addbutton ( "~C~ancel", xsize/2 + 5, ysize - 30, xsize-10 - 5, ysize - 10 , 0, 1, 2, true );
   addkey ( 2, ct_esc );

   addbutton ( "ma~x~ resources ( at center )", xsize/2 + 5, 80, xsize-20 - 5, 100 , 2, 1, 3, true );
   addeingabe ( 3, &maxresource, 0, 255 );

   addbutton ( "mi~n~ resources ", xsize/2 + 5, 120, xsize-20 - 5, 140 , 2, 1, 4, true );
   addeingabe ( 4, &minresource, 0, 255 );

   addbutton ( "max ~d~istance", 20, 120, xsize/2-5, 140 , 2, 1, 5, true );
   addeingabe ( 5, &dist, 1, 20 );

   addbutton ( "~m~aterial", 20, 160, xsize/2-5, 180, 3, 10, 6, true );
   addeingabe ( 6, &resourcetype, black, dblue );

   addbutton ( "~f~uel", xsize/2 + 5, 160, xsize - 20, 180, 3, 10, 7, true );
   addeingabe ( 7, &restp2, black, dblue );

   buildgraphics(); 

}

void tputresourcesdlg :: buttonpressed ( int id )
{
   tdialogbox :: buttonpressed ( id );

   switch ( id ) {
      case 1: status = 11;
         break;
      case 2: status = 10;
         break;
      case 6: restp2 = 0;
              enablebutton ( 7 );
         break;
      case 7: resourcetype = 0;
              enablebutton ( 6 );
         break;
   } /* endswitch */

}

void tputresourcesdlg :: run ( void )
{
   mousevisible ( true );
   do {
      tdialogbox :: run ( );
   } while ( status < 10 ); /* enddo */
   if ( status == 11 ) {
      tputresources pr ( actmap );
      pr.init ( actmap->getCursor().x, actmap->getCursor().y, dist, resourcetype ? 1 : 2, maxresource, minresource );
   }
}




// � Lines

void lines(int x1,int y1,int x2,int y2)
{
   line(x1,y1,x2,y1,white);
   line(x2,y1,x2,y2,darkgray);
   line(x1,y2,x2,y2,darkgray);
   line(x1,y1,x1,y2,white);
} 




// � PlayerChange

/* class   tcolorsel : public tstringselect {
           public :
                 virtual void setup(void);
                 virtual void buttonpressed(int id);
                 virtual void run(void);
                 virtual void get_text(int nr);
                 };

void         tcolorsel::setup(void)
{

   action = 0;
   title = "Select Color";
   numberoflines = 9;
   xsize = 250;
   ex = xsize - 100;
   ey = ysize - 60;
   addbutton("~D~one",20,ysize - 40,120,ysize - 20,0,1,2,true);
   addkey(2,ct_enter);
   addbutton("~C~ancel",130,ysize - 40,230,ysize - 20,0,1,3,true);
}


void         tcolorsel::buttonpressed(int         id)
{
   tstringselect::buttonpressed(id);
   switch (id) {

      case 2:
      case 3:   action = id;
   break;
   }
}


void         tcolorsel::get_text(int nr)
{
   if (nr == 8) strcpy(txt,"Neutral");
   else {
      if ((nr>=0) && (nr<=7)) {
         strcpy(txt,"Color ");
         strcat(txt,strrr(nr+1));
      } else {
         strcpy(txt,"");
      }
   }
}


void         tcolorsel::run(void)
{
   mousevisible(false);
   for (i=0;i<=8 ;i++ ) bar(x1 + 160,y1 + 52 + i*21 ,x1 + 190 ,y1 + 62 + i * 21,20 + i * 8);
   mousevisible(true);
   do {
      tstringselect::run();
   }  while ( ! ( (taste == ct_esc) || ( (action == 2) || (action == 3) ) || (msel == 1)) );
   if ( (action == 3) || (taste == ct_esc) ) redline = 255;
}


int         colorselect(void)
{
  tcolorsel  sm;

   sm.init();
   sm.run();
   sm.done();
   return sm.redline;
} */


class  tplayerchange : public tdialogbox {
          public :
              int action;
              int sel1,sel2;
              int bkgcolor;
              void init(void);
              virtual void run(void);
              virtual void buttonpressed(int id);
              void anzeige(void);
              };




void         tplayerchange::init(void)
{
   char *s1;

   tdialogbox::init();
   title = "Player Change";
   x1 = 50;
   xsize = 370;
   y1 = 50;
   ysize = 410;
   action = 0;
   sel1 = 255;
   sel2 = 255;
   bkgcolor = lightgray;

   windowstyle = windowstyle ^ dlg_in3d;

   for ( int i=0;i<=8 ;i++ ) {
      s1 = new(char[12]);
      if (i == 8) {
         strcpy(s1,"~N~eutral");
      } else {
         strcpy(s1,"Player ~");
         strcat(s1,strrr(i+1));
         strcat(s1,"~");
      }
      addbutton(s1,20,55+i*30,150,75+i*30,0,1,6+i,true);
      addkey(1,ct_1+i);
   }

   //addbutton("~C~hange",20,ysize - 70,80,ysize - 50,0,1,3,true);
   addbutton("X~c~hange",20,ysize - 70,170,ysize - 50,0,1,4,true);
   addbutton("~M~erge",200,ysize - 70,350,ysize - 50,0,1,5,true);

   addbutton("~D~one",20,ysize - 40,170,ysize - 20,0,1,1,true);
   addkey(1,ct_enter);
   addbutton("E~x~it",200,ysize - 40,350,ysize - 20,0,1,2,true);

   buildgraphics();

   for ( int i=0;i<=8 ;i++ ) bar(x1 + 170,y1 + 60 + i*30 ,x1 + 190 ,y1 + 70 + i * 30,20 + ( i << 3 ));

   anzeige();

   mousevisible(true);
}

void         tplayerchange::anzeige(void)
{
   int e,b,m[9];
   for ( int i=0;i<=8 ;i++ )
      m[i] =0;
   
   for ( int i =0;i < actmap->xsize * actmap->ysize ;i++ ) {
      int color = actmap->field[i].mineowner();
      if ( color >= 0 && color < 8 )
         m[color]++;
   }
   activefontsettings.length = 40;
   activefontsettings.background = lightgray;
   activefontsettings.color = red;
   activefontsettings.justify = lefttext;
   activefontsettings.font = schriften.smallarial;
   mousevisible(false);
   showtext2("Units",x1+210,y1+35);
   showtext2("Build.",x1+260,y1+35);
   showtext2("Mines",x1+310,y1+35);
   for ( int i=0;i<=8 ;i++ ) {
      if (i == sel1 ) rectangle (x1 + 16,y1+51+i*30,x1+154,y1+79+i*30, 20 );
      else if ( i == sel2 ) rectangle (x1 + 16,y1+51+i*30,x1+154,y1+79+i*30, 28 );
      else rectangle (x1 + 16,y1+51+i*30,x1+154,y1+79+i*30, bkgcolor );
      e = actmap->player[i].vehicleList.size();
      b = actmap->player[i].buildingList.size();
      activefontsettings.justify = righttext;
      showtext2(strrr(e),x1+200,y1+56+i*30);
      showtext2(strrr(b),x1+255,y1+56+i*30);
      showtext2(strrr(m[i]),x1+310,y1+56+i*30);
   } /* endfor */
   mousevisible(true);
}


void         tplayerchange::run(void)
{

   do {
      tdialogbox::run();
      if (taste == ct_f1) help ( 1050 );
   }  while (!((taste == ct_esc) || ((action == 1) || (action ==2))));
 // ????  if ((action == 2) || (taste == ct_esc)) ;
}


void         tplayerchange::buttonpressed(int         id)
{
   tdialogbox::buttonpressed(id);
   switch (id) {

      case 1:
      case 2:   action = id;
        break;
/*     case 3: {
           int sel = colorselect();
           if ( ( sel != 255) && ( sel != sel1 ) && ( sel1 != 255 ) &&
              ( actmap->player[sel].firstvehicle == NULL ) && ( actmap->player[sel].firstbuilding == NULL )   ) {

           }
        }
        break; */
     case 4: {
           if ( ( sel1 != 255) && ( sel2 != sel1 ) && ( sel2 != 255 ) && ( sel1 != 8) && ( sel2 != 8 ) ) {
              actmap->player[sel1].swap ( actmap->player[sel2] );
              anzeige();
           }
        }
        break;
     case 5: {
           if ( ( sel1 != 255) && ( sel2 != sel1 ) && ( sel2 != 255 ) ) {

              // adding everything from player sel2 to sel1

              if ( sel1 != 8 )
                 for ( Player::VehicleList::iterator i = actmap->player[sel2].vehicleList.begin(); i != actmap->player[sel2].vehicleList.end(); ) {
                    (*i)->color = sel1*8;
                    actmap->player[sel1].vehicleList.push_back ( *i );
                    i = actmap->player[sel2].vehicleList.erase( i );
                 }

              Player::BuildingList bl = actmap->player[sel2].buildingList;
              for ( Player::BuildingList::iterator i = bl.begin(); i != bl.end(); ++i)
                 (*i)->convert( sel1 );

              for (int i =0;i < actmap->xsize * actmap->ysize ;i++ ) {
                 tfield* fld = &actmap->field[i];
                 for ( tfield::MineContainer::iterator i = fld->mines.begin(); i != fld->mines.end(); i++ )
                    if ( i->player == sel2 && sel1 != 8)
                       i->player = sel1;

              } /* endfor */
              anzeige();
           }
        }
        break;
      case 6:
      case 7:
      case 8:
      case 9:
      case 10:
      case 11:
      case 12:
      case 13:
      case 14: if ( id-6 != sel1 ) {
         sel2 = sel1;
         sel1 = id-6;
         anzeige();
      }
   break;
   }
}


void playerchange(void)
{ tplayerchange       sc;

   sc.init();
   sc.run();
   sc.done();
   mapChanged(actmap);
}




void         exchg(int *       a1,
                   int *       a2)
{ int ex;

   ex = *a1;
   *a1 = *a2;
   *a2 = ex;
}




// � TCDPlayer

/*

class   tcdplayer : public tstringselect {
           public :
                 virtual void setup(void);
                 virtual void buttonpressed(int id);
                 virtual void run(void);
                 virtual void get_text(int nr);
                 };

void         tcdplayer ::setup(void)
{

   action = 0;
   xsize = 400;
   ysize = 400;
   x1 = 100;
   title = "CD- Player";
   numberoflines = cdrom.cdinfo.lasttrack;
   ey = ysize - 60;
   ex = xsize - 120;
   addbutton("~R~CD",320,50,390,70,0,1,11,true);
   addbutton("~P~lay",320,90,390,110,0,1,12,true);
   addbutton("~S~top",320,130,390,150,0,1,13,true);
   addbutton("E~x~it",10,ysize - 40,290,ysize - 20,0,1,10,true);
}


void         tcdplayer ::buttonpressed(int         id)
{
   tstringselect::buttonpressed(id);
   switch (id) {

   case 10:   action = 3;
      break;
   case 11:  {
      cdrom.readcdinfo();
      redline = 0;
      firstvisibleline = 0;
      numberoflines = cdrom.cdinfo.lasttrack;
      viewtext();
      }
      break;
   case 12: {
         cdrom.playtrack(redline);
      }
      break;
   case  13: cdrom.stopaudio ();
      break;
   }
}


void         tcdplayer ::get_text(int nr)
{
    if (cdrom.cdinfo.track[nr]->type > 3) {
       strcpy(txt,"Data Track ");
       strcat(txt,strrr(nr+1));
    } else {
       strcpy(txt,"");
       strcat(txt,strrr(cdrom.cdinfo.track[nr]->min));
       strcat(txt," : ");
       strcat(txt,strrr(cdrom.cdinfo.track[nr]->sec));
       strcat(txt," : ");
       strcat(txt,strrr(cdrom.cdinfo.track[nr]->frame));
       strcat(txt,"   Track ");
       strcat(txt,strrr(nr+1));
    }
}


void         tcdplayer ::run(void)
{
   do {
      tstringselect::run();
      if ( (msel == 1)  || ( taste == ct_enter ) ) cdrom.playtrack(redline);
   }  while ( ! ( (taste == ct_esc) || (action == 3) ) );
}


void cdplayer( void )
{
   tcdplayer cd;

   cd.init();
   cd.run();
   cd.done();
}

*/


#ifndef pbpeditor

void         k_savemap(char saveas)
{
   ASCString filename;

   int nameavail = 0;
   if ( !actmap->preferredFileNames.mapname[0].empty() ) {
      nameavail = 1;
      filename = actmap->preferredFileNames.mapname[0];;
   }

   if ( saveas || !nameavail ) {
      filename = selectFile(mapextension, false);
   }
   if ( !filename.empty() ) {
      mapsaved = true;
      actmap->preferredFileNames.mapname[0] = filename;
      savemap( filename.c_str(), actmap );
      displaymap();
   }
}


void         k_loadmap(void)
{
   ASCString s1 = selectFile( mapextension, true );
   if ( !s1.empty() ) {
      StatusMessageWindowHolder smw = MessagingHub::Instance().infoMessageWindow( "loading map " + s1 );
      GameMap* mp = mapLoadingExceptionChecker( s1, MapLoadingFunction( tmaploaders::loadmap ));
      if ( !mp )
         return;
         
      delete actmap;
      actmap =  mp;

      if ( actmap->campaign.avail && !actmap->campaign.directaccess && !actmap->codeWord.empty() ) {
         tlockdispspfld ldsf;
         removemessage();
         Password pwd;
         pwd.setUnencoded ( actmap->codeWord );
         enterpassword ( pwd );
      } else
         removemessage();


      displaymap();
      mapsaved = true;
   }
}

#else

#include "pbpeditor.cpp"

#endif


void selectUnitFromMap ( GameMap* gamemap, MapCoordinate& pos )
{
   SelectFromMap::CoordinateList list;
   list.push_back( pos );

   SelectFromMap sfm( list, gamemap );
   sfm.Show();
   sfm.RunModal();

   if ( list.empty() )
      pos = MapCoordinate( -1, -1 );
   else
      pos = *list.begin();
}



void         setstartvariables(void)
{
   activefontsettings.font = schriften.smallarial;
   activefontsettings.color = 1;
   activefontsettings. background = 0;

   mapsaved = true;

}

#if 0

int  selectfield(int * cx ,int  * cy)
{
   // int oldposx = getxpos();
   // int oldposy = getypos();
   actmap->getCursor() = MapCoordinate( *cx, *cy );
   displaymap();
   tkey ch = ct_invvalue;
   do {
      if (keypress()) {
         ch = r_key();
         // movecursor( ch );
      } else
         releasetimeslice();
   } while ( ch!=ct_enter  &&  ch!=ct_space  && ch!=ct_esc); /* enddo */
   *cx=actmap->getCursor().x;
   *cy=actmap->getCursor().y;
   if ( ch == ct_enter )
      return 1;
   else
      if ( ch == ct_esc )
         return 0;
      else
         return 2;
}



Vehicle*  selectUnitFromMap()
{
   displaymap();
   tkey ch = ct_invvalue;
   Vehicle* veh = NULL;
   do {
      if (keypress()) {
         ch = r_key();
         // movecursor( ch );
         if( ch == 'c' &&  getactfield()->getContainer()  )
            veh = selectUnitFromContainer( getactfield()->getContainer() );

         if ( ch ==ct_enter  ||  ch==ct_space ) {
            if ( getactfield()->vehicle )
               veh = getactfield()->vehicle;
         }
      } else
         releasetimeslice();


   } while ( !veh  && ch!=ct_esc); /* enddo */

   if ( ch == ct_enter || ct_space )
      return veh;
   else
      return NULL;
}

#endif

//* � FillPolygonevent



//* � FillPolygonbdt
/*
void tfillpolygonbodentyp::setpointabs    ( int x,  int y  )
{
       tfield* ffield = getfield ( x , y );
       if (ffield) {
           ffield->a.temp = tempvalue;
           if ( auswahl->weather[auswahlw] )
              ffield->typ = auswahl->weather[auswahlw];
           else
              ffield->typ = auswahl->weather[0];
           ffield->direction = auswahld;
           ffield->setparams();
           if (ffield->vehicle != NULL)
              if ( terrainaccessible(ffield,ffield->vehicle) == false ) {
                 delete ffield->vehicle;
                 ffield->vehicle = NULL;
              }
       }
}

void tfillpolygonbodentyp::initevent ( void )
{
}

// � FillPolygonunit

void tfillpolygonunit::setpointabs    ( int x,  int y  )
{
       tfield* ffield = getfield ( x , y );
       if (ffield) {
          if ( terrainaccessible(ffield,ffield->vehicle) )
               {
                  if (ffield->vehicle != NULL) {
                     delete ffield->vehicle;
                     ffield->vehicle = NULL;
                  }
                  if (auswahlf != NULL) {
                     ffield->vehicle = new Vehicle ( auswahlf,actmap, farbwahl );
                     ffield->vehicle->fillMagically();
                     ffield->vehicle->height=1;
                     while ( ! ( ( ( ( ffield->vehicle->height & ffield->vehicle->typ->height ) > 0) && (terrainaccessible(ffield,ffield->vehicle) == 2) ) ) && (ffield->vehicle->height != 0) )
                        ffield->vehicle->height = ffield->vehicle->height * 2;
                     for (i = 0; i <= 31; i++) ffield->vehicle->loading[i] = NULL;
                     if (ffield->vehicle->height == 0 ) {
                        delete ffield->vehicle;
                        ffield->vehicle = NULL;
                     }
                     else ffield->vehicle->setMovement ( ffield->vehicle->typ->movement[log2(ffield->vehicle->height)] );
                     ffield->vehicle->direction = auswahld;
                  }
            }
            else
               if (auswahlf == NULL)
                  if (ffield->vehicle != NULL) {
                     delete ffield->vehicle;
                     ffield->vehicle=NULL;
                  }
            ffield->a.temp = tempvalue;
       }
}

void tfillpolygonunit::initevent ( void )
{
}


// � ChangePoly
*/


class  ShowPolygonUsingTemps : public PolygonPainterSquareCoordinate {
   protected:
      virtual void setpointabs ( int x,  int y  ) {
         tfield* ffield = getfield ( x , y );
         if (ffield)
            ffield->a.temp2 = 1;
      };
   public:
      bool paintPolygon   (  const Poly_gon& poly ) {
         bool res = PolygonPainterSquareCoordinate::paintPolygon ( poly );
         for ( int i = 0; i < poly.vertex.size(); ++i ) {
            tfield* ffield = actmap->getField ( poly.vertex[i] );
            if (ffield)
               ffield->a.temp = 1;
         }
         return res;
      };
};


class PolygonEditor : public SelectFromMap {
   protected:
      void showFieldMarking( const CoordinateList& coordinateList )
      {
         Poly_gon poly;
         for ( CoordinateList::const_iterator i = coordinateList.begin(); i != coordinateList.end(); ++i ) 
            poly.vertex.push_back( MapCoordinate(*i) );

         ShowPolygonUsingTemps sput;
         if ( !sput.paintPolygon ( poly ) )
            displaymessage("Invalid Polygon !",1 );

         repaintMap();
      }
      
   public:
      PolygonEditor( CoordinateList& list, GameMap* map ) : SelectFromMap( list, map ) {};
};



void editpolygon(Poly_gon& poly)
{
   savemap ( "_backup_polygoneditor.map", actmap );

   PolygonEditor::CoordinateList list;
   
   for ( Poly_gon::VertexIterator  i = poly.vertex.begin(); i != poly.vertex.end(); ++i )
      list.push_back( *i );
   
   PolygonEditor cp ( list, actmap );
   cp.Show();
   cp.RunModal();

   poly.vertex.clear();
   for ( PolygonEditor::CoordinateList::iterator i = list.begin(); i != list.end(); ++i )
      poly.vertex.push_back( *i );
}



void         newmap(void)
{
   GameMap* map = createNewMap();
   if ( map ) {
      delete actmap;
      actmap = map;
      // displaymap();
      mapChanged( actmap );
      // tspfldloaders::mapLoaded( actmap );
   }
}


//* � MapVals



     class BuildingValues : public tdialogbox {
               int action;
               Building& gbde;
               int rs,mrs;
               Resources plus,mplus, biplus,storage;
               int col;
               int damage;
               char tvisible;
               char name[260];
               int ammo[waffenanzahl];
               virtual void buttonpressed(int id);
               int lockmaxproduction;
               TemporaryContainerStorage buildingBackup; // if the editing is cancelled

           public :
               BuildingValues ( Building& building ) : gbde ( building ), buildingBackup ( &building ) {};
               void init(void);
               virtual void run(void);
            };


void         BuildingValues::init(void)
{
  char      b;

  for ( int i = 0; i< waffenanzahl; i++ )
     ammo[i] = gbde.ammo[i];

   lockmaxproduction = 1;

   tdialogbox::init();
   action = 0;
   title = "Building Values";
   x1 = 0;
   xsize = 640;
   y1 = 10;
   ysize = 480;
   // int w = (xsize - 60) / 2;
   action = 0;

   windowstyle = windowstyle ^ dlg_in3d;

   storage = gbde.actstorage;
   plus = gbde.plus;
   mplus = gbde.maxplus;
   rs = gbde.researchpoints;
   mrs = gbde.maxresearchpoints;
   tvisible = gbde.visible;
   biplus = gbde.bi_resourceplus;
   col = gbde.color / 8;
   damage = gbde.damage;

   strcpy( name, gbde.name.c_str() );

   addbutton("~N~ame",15,50,215,70,1,1,10,true);
   addeingabe(10,&name[0],0,25);

   addbutton("~E~nergy-Storage",15,90,215,110,2,1,1,true);
   addeingabe(1,&storage.energy,0,gbde.getStorageCapacity().resource(0));

   addbutton("~M~aterial-Storage",15,130,215,150,2,1,2,true);
   addeingabe(2,&storage.material,0,gbde.getStorageCapacity().resource(1));

   addbutton("~F~uel-Storage",15,170,215,190,2,1,3,true);
   addeingabe(3,&storage.fuel,0,gbde.getStorageCapacity().resource(2));

   if ( gbde.typ->hasFunction( ContainerBaseType::MatterConverter  ) ||
        gbde.typ->hasFunction( ContainerBaseType::SolarPowerPlant  ) ||
        gbde.typ->hasFunction( ContainerBaseType::WindPowerPlant  ) ||
        gbde.typ->hasFunction( ContainerBaseType::MiningStation  ) ||
        gbde.typ->hasFunction( ContainerBaseType::ResourceSink  ))
      b = true;
   else
      b = false;

   addbutton("Energy-Max-Plus",230,50,430,70,2,1,13,b);
   addeingabe(13,&mplus.energy,0,gbde.typ->maxplus.energy);

   if ( gbde.typ->hasFunction( ContainerBaseType::MatterConverter  ) ||
        gbde.typ->hasFunction( ContainerBaseType::MiningStation  ) ||
        gbde.typ->hasFunction( ContainerBaseType::ResourceSink  ))
      b = true;
   else
      b = false;
   

   addbutton("Energ~y~-Plus",230,90,430,110,2,1,4,b);
   addeingabe(4,&plus.energy,0,mplus.energy);

   if ( ((gbde.typ->hasFunction( ContainerBaseType::MatterConverter) || gbde.typ->hasFunction( ContainerBaseType::MiningStation)) && gbde.typ->efficiencymaterial )
          || gbde.typ->hasFunction( ContainerBaseType::ResourceSink  ) )
      b = true;
   else
      b = false;
   
   addbutton("Material-Max-Plus",230,130,430,150,2,1,14,b);
   addeingabe(14,&mplus.material,0,gbde.typ->maxplus.material);

   addbutton("M~a~terial-Plus",230,170,430,190,2,1,5,b);
   addeingabe(5,&plus.material,0,mplus.material);

   if ( ((gbde.typ->hasFunction( ContainerBaseType::MatterConverter) || gbde.typ->hasFunction( ContainerBaseType::MiningStation)) && gbde.typ->efficiencyfuel )
          || gbde.typ->hasFunction( ContainerBaseType::ResourceSink  ) )
      b = true;
   else
      b = false;

   addbutton("Fuel-Max-Plus",230,210,430,230,2,1,15,b);
   addeingabe(15,&mplus.fuel,0,gbde.typ->maxplus.fuel);

   addbutton("F~u~el-Plus",230,250,430,270,2,1,6,b);
   addeingabe(6,&plus.fuel, 0, mplus.fuel);

   if ( gbde.typ->hasFunction( ContainerBaseType::Research) || ( gbde.typ->maxresearchpoints > 0))
      b = true;
   else
      b = false;

   addbutton("~R~esearch-Points",15,210,215,230,2,1,9,b);
   if ( gbde.typ->hasFunction(ContainerBaseType::Research))
      addeingabe(9,&rs,0,gbde.typ->maxresearchpoints);
   else
      addeingabe(9,&rs,gbde.typ->maxresearchpoints,gbde.typ->maxresearchpoints);

   addbutton("Ma~x~research-Points",15,250,215,270,2,1,11,b);
   if ( gbde.typ->hasFunction(ContainerBaseType::Research))
      addeingabe(11,&mrs,0,gbde.typ->maxresearchpoints);
   else
      addeingabe(11,&mrs,gbde.typ->maxresearchpoints,gbde.typ->maxresearchpoints);


   addbutton("~V~isible",15,290,215,300,3,1,12,true);
   addeingabe(12,&tvisible,0,lightgray);

   addbutton("~L~ock MaxPlus ratio",230,290,430,300,3,1,120,true);
   addeingabe(120,&lockmaxproduction,0,lightgray);


   addbutton("~C~olor",230,370,430,390,2,1,104,true);
   addeingabe(104,&col,0,8);

   addbutton("~D~amage",230,410,430,430,2,1,105,true);
   addeingabe(105,&damage,0,99);


   addbutton("BI energy plus",15,330,215,350,2,1,101,1);
   addeingabe(101,&biplus.energy,0,maxint);

   addbutton("BI material plus",230,330,430,350,2,1,102,1);
   addeingabe(102,&biplus.material,0,maxint);

   addbutton("BI fuel plus",15,370,215,390,2,1,103,1);
   addeingabe(103,&biplus.fuel,0,maxint);


   addbutton("~S~et Values",10,ysize - 40, xsize/3-5,ysize - 10,0,1,7,true);
   addkey(7,ct_enter);

   addbutton("Help (~F1~)", xsize/3+5, ysize - 40, xsize/3*2-5, ysize-10, 0, 1, 110, true );
   addkey(110, ct_f1 );

   addbutton("~C~ancel",xsize/3*2+5,ysize - 40,xsize-10,ysize - 10,0,1,8,true);
   addkey(8, ct_esc );


   for ( int i = 0; i < waffenanzahl; i++ ) {
      addbutton ( cwaffentypen[i], 460, 90+i*40, 620, 110+i*40, 2, 1, 200+i, 1 );
      addeingabe(200+i, &ammo[i], 0, maxint );
   }

   buildgraphics();
   line(x1+450, y1+45, x1+450, y1+55+9*40, darkgray );
   activefontsettings.length =0;
   activefontsettings.font = schriften.smallarial;
   showtext2("ammunition:", x1 + 460, y1 + 50 );

   mousevisible(true);
}


void         BuildingValues::run(void)
{
   do {
      tdialogbox::run();
   }  while (!((taste == ct_esc) || (action == 1)));
}


void         BuildingValues::buttonpressed(int         id)
{
   switch(id) {
   case 4:            // energy, material & fuel plus
   case 5:
   case 6: {
              int changed_resource = id - 4;
              for ( int r = 0; r < 3; r++ )
                 if ( r != changed_resource )
                    if ( mplus.resource(changed_resource) ) {
                       int a = mplus.resource(r) * plus.resource(changed_resource) / mplus.resource(changed_resource);
                       if ( a != plus.resource(r) ) {
                          plus.resource(r) = a;
                          showbutton ( 4 + r );
                       }
                    }
            }
      break;
   case 13:        // energy, material and fuel maxplus
   case 14:
   case 15: {
              int changed_resource = id - 13;
              if ( lockmaxproduction )
                 for ( int r = 0; r < 3; r++ )
                    if ( r != changed_resource )
                       if ( gbde.typ->maxplus.resource(changed_resource) ) {
                          int a = gbde.typ->maxplus.resource(r) * mplus.resource(changed_resource) / gbde.typ->maxplus.resource(changed_resource);
                          if ( a != mplus.resource(r) ) {
                             mplus.resource(r) = a;
                             showbutton ( 13 + r );
                          }
                       }

              for ( int r = 0; r < 3; r++ ) {
                 if ( (mplus.resource(r) >= 0 && plus.resource(r) > mplus.resource(r) ) ||
                      (mplus.resource(r) <  0 && plus.resource(r) < mplus.resource(r) )) {
                         plus.resource(r) = mplus.resource(r);
                         showbutton ( 4 + r );
                 }
                 addeingabe(4+r, &plus.resource(r), 0, mplus.resource(r) );
              }
            }
      break;
     case 7: {
           mapsaved = false;
           action = 1;
           gbde.actstorage = storage;

           gbde.plus = plus;
           gbde.maxplus = mplus;
           if ( col != gbde.color/8 )
              gbde.convert ( col );

           gbde.researchpoints = rs;
           gbde.maxresearchpoints = mrs;
           gbde.visible = tvisible;
           gbde.bi_resourceplus = biplus;
           gbde.name = name;
           gbde.damage = damage;
           for ( int i = 0; i< waffenanzahl; i++ )
               gbde.ammo[i] = ammo[i];

        }
        break;
     case 8: action = 1;
        break;

     /*

     case 13: {
                     addeingabe(4,&plus.energy,0,mplus.energy);
                     if (mplus.energy < plus.energy ) {
                        plus.energy = mplus.energy;
                        enablebutton(4);
                     }
                  }
        break;
     case 14: {
                   addeingabe(5,&plus.material,0,mplus.material);
                      if (mplus.material < plus.material) {
                         plus.material =mplus.material;
                         enablebutton(5);
                      }
                   }

        break;
     case 15: {
                   addeingabe(6,&plus.fuel,0,mplus.fuel);
                      if (plus.fuel > mplus.fuel) {
                         plus.fuel = mplus.fuel;
                         enablebutton(6);
                      }
                   }
        break;
     */
  case 110: help ( 41 );
     break;

  }
}


void         changebuildingvalues( Building& b )
{
   BuildingValues bval ( b );
   bval.init();
   bval.run();
   bval.done();

   displaymap();
}

// � Class-Change


// � Polygon-Management
/*
class tpolygon_managementbox: public tstringselect {
              public:
                 ppolygon poly;
                 virtual void setup(void);
                 virtual void buttonpressed(int id);
                 virtual void run(void);
                 virtual void get_text(int nr);
                 };


tpolygon_management::tpolygon_management(void)
{
   polygonanz = 0;
   lastpolygon = firstpolygon;
}

void         tpolygon_managementbox::setup(void)
{
   action = 0;
   title = "Choose Polygon";

   x1 = 70;
   y1 = 40;
   xsize = 500;
   ysize = 400;
   lnshown = 10;
   numberoflines = polymanage.polygonanz;
   activefontsettings.length = xsize - 30;
   addbutton("~D~one",20,ysize - 40,170,ysize - 20,0,1,1,true);
   addkey(1,ct_enter);
   addbutton("~C~ancel",190,ysize - 40,340,ysize - 20,0,1,2,true);
}


void         tpolygon_managementbox::buttonpressed(int         id)
{
   tstringselect::buttonpressed(id);
   switch (id) {
      case 1:
      case 2:   action = id;
   break;
   }
}


void         tpolygon_managementbox::get_text(int nr)
{
   char s[200];
   ppolystructure pps;
   int i,vn;

   const int showmaxvertex = 5;

   pps = polymanage.firstpolygon;
   for (i=0 ;i<nr; i++ )
       pps =pps->next;
   strcpy(s,"");
   if ( pps->poly->vertexnum> showmaxvertex ) vn=5;
   else vn = pps->poly->vertexnum;
   switch (pps->place) {
   case 0: {
      strcat(s,"Events: ");
      strcat(s,ceventactions[pps->id]);
      }
      break;
   }
   for (i=0;i <= vn-1;i++ ) {
       strcat(s,"(");
       strcat(s,strrr(pps->poly->vertex[i].x));
       strcat(s,"/");
       strcat(s,strrr(pps->poly->vertex[i].y));
       strcat(s,") ");
   }
   if (vn < pps->poly->vertexnum )  strcat(s,"...");
   strcpy(txt,s);
}


void         tpolygon_managementbox::run(void)
{
   do {
      tstringselect::run();
      if (taste ==ct_f1) help (1030);
   }  while ( ! ( (taste == ct_enter ) || (taste == ct_esc) || ( (action == 1) || (action == 2) ) ) );
   if ( ( ( taste == ct_enter ) || (action == 1 ) ) && (redline != 255 ) ) {
      ppolystructure pps;

      pps = polymanage.firstpolygon;
      for (i=0 ;i<redline; i++ ) pps =pps->next;
      poly = pps->poly;
   }
}

void tpolygon_management::addpolygon(ppolygon *poly, int place, int id)
{
   (*poly) = new(tpolygon);

   (*poly)->vertexnum = 0;
   (*poly)->vertex    = (tpunkt*) malloc ( 1 * sizeof ( (*poly)->vertex[0] ) );
   if (polygonanz == 0 ) {
      lastpolygon = new ( tpolystructure );
      firstpolygon = lastpolygon;
   }
   else {
      lastpolygon->next = new ( tpolystructure );
      lastpolygon = lastpolygon->next;
   }
   lastpolygon->poly = (*poly);
   lastpolygon->id = id;
   lastpolygon->place = place;
   polygonanz++;
}

void tpolygon_management::deletepolygon(ppolygon *poly)
{
   asc_free( (*poly)->vertex );
   asc_free(*poly);
}


int        getpolygon(ppolygon *poly) //return Fehlerstatus
{
   tpolygon_managementbox polymanagebox;

   polymanagebox.poly = (*poly);
   polymanagebox.init();
   polymanagebox.run();
   polymanagebox.done();
   (*poly) = polymanagebox.poly;
   if ( (polymanagebox.action == 2) || (polymanagebox.taste == ct_esc ) ) return 1;
   else return 0;
}
*/
// � Unit-Values


class   StringSelector : public tstringselect {
                 const char** text;
           public :
                 int lastchoice;
                 StringSelector ( const char* title_, const char** text_, int itemNum_ ) : text ( text_ )  { lastchoice = 0; numberoflines = itemNum_; title = title_; };
                 virtual void setup(void);
                 virtual void buttonpressed(int id);
                 virtual void run(void);
                 virtual void get_text(int nr);
                 };

void         StringSelector ::setup(void)
{
   action = 0;
   ey = ysize - 90;
   startpos = lastchoice;
   addbutton("~O~k",20,ysize - 50,xsize-20,ysize - 20,0,1,13,true);
   addkey ( 13, ct_enter );
}


void         StringSelector ::buttonpressed(int         id)
{
   tstringselect::buttonpressed(id);
   switch (id) {

      case 12:  if ( redline >= 0 )
                   action = id-10;
                break;

      case 13:   action = id-10;
                 break;
   }
}


void         StringSelector ::get_text(int nr)
{
   strcpy(txt, text[nr] );
}


void         StringSelector ::run(void)
{
   do {
      tstringselect::run();
      if ( taste == ct_enter ) {
         if ( redline >= 0 )
            action = 2;
         else
            action = 3;
      }
   }  while ( action == 0 );
}


int selectString( int lc, const char* title, const char** text, int itemNum )
{
   StringSelector  ss ( title, text, itemNum );
   ss.lastchoice = lc;
   ss.init();
   ss.run();
   ss.done();
   return ss.redline;
}


class EditAiParam : public tdialogbox {
           Vehicle* unit;
           TemporaryContainerStorage tus;
           int action;
           AiParameter& aiv;
           int z;
        public:
           EditAiParam ( Vehicle* veh, int player ) : unit ( veh ), tus ( veh ), aiv ( *veh->aiparam[player] ) {};
           void init ( );
           void run ( );
           void buttonpressed ( int id );
           int  getcapabilities ( void ) { return 1; };
};


void         EditAiParam::init(  )
{
   tdialogbox::init();
   action = 0;
   title = "Unit AI Values";

   windowstyle = windowstyle ^ dlg_in3d;

   x1 = 20;
   xsize = 600;
   y1 = 40;
   ysize = 400;
   int w = (xsize - 60) / 2;
   action = 0;

   addbutton("dest ~X~",50,80,250,100,2,1,1,true);
   addeingabe(1, &aiv.dest.x, minint, maxint);

   addbutton("dest ~Y~",50,120,250,140,2,1,2,true);
   addeingabe(2, &aiv.dest.y, minint, maxint);

   z = aiv.dest.getNumericalHeight();
   addbutton("dest ~Z~",50,160,250,180,2,1,3,true);
   addeingabe(3, &z, minint, maxint);

   addbutton("dest ~N~WID",50,200,250,220,2,1,4,true);
   addeingabe(4, &aiv.dest_nwid, minint, maxint);

   addbutton("~V~alue",410,80,570,100,2,1,10,true);
   addeingabe( 10, &aiv.value, minint, maxint );

   addbutton("~A~dded value",410,120,570,140,2,1,11,true);
   addeingabe( 11, &aiv.addedValue, minint, maxint );

   addbutton("~R~eset task after completion",410,160,570,180,3,1,12,true);
   addeingabe( 12, &aiv.resetAfterJobCompletion, black, dblue );


   addbutton("~S~elect Dest",50,240,250,260,0,1,22,true);

   addbutton("~T~ask",20,ysize - 70,20 + w,ysize - 50,0,1,20,true);
   addbutton("~J~ob",40 + w,ysize - 70,40 + 2 * w,ysize - 50,0,1,21,true);

   addbutton("~S~et Values",20,ysize - 40,20 + w,ysize - 10,0,1,30,true);
   addkey(30,ct_enter );
   addbutton("~C~ancel",40 + w,ysize - 40,40 + 2 * w,ysize - 10,0,1,31,true);
   addkey(31,ct_esc );

   buildgraphics();
   mousevisible(true);
}


void         EditAiParam::run(void)
{
   do {
      tdialogbox::run();
   }  while ( !action );
}


void         EditAiParam::buttonpressed(int         id)
{
   switch (id) {
   case 20 : aiv.setTask( AiParameter::Task( selectString ( aiv.getTask(), "Select Task", AItasks , AiParameter::taskNum) ));
             break;
   case 21 : {
                AiParameter::JobList j;
                j.push_back ( AiParameter::Job( selectString ( aiv.getJob(), "Select Job", AIjobs , AiParameter::jobNum) ) );
                aiv.setJob ( j );
             }
             break;
             /*
   case 22 : getxy ( &aiv.dest.x, &aiv.dest.y );
             aiv.dest.setnum ( aiv.dest.x, aiv.dest.y, -2 );
             z = -2;
             redraw();
             break;
             */
   case 30 : action = 1;
             aiv.dest.setnum ( aiv.dest.x, aiv.dest.y, z );
             break;
   case 31 : action = 1;
              tus.restore();
        break;
   } /* endswitch */
}




     class UnitPropertyEditor: public tdialogbox {
                TemporaryContainerStorage tus;
                int        dirx,diry;
                int        action;
                Vehicle*    unit;
                int         w2, heightxs;
                char        namebuffer[1000];
                char        reactionfire;
                int owner;
              public:
               // char     checkvalue( char id, char* p );
                UnitPropertyEditor ( Vehicle* v ) : tus ( v ), unit ( v ) {};
                void        init( void );
                void        run( void );
                void        buttonpressed( int id );
     };


void         UnitPropertyEditor::init(  )
{
   int         w;
   char *weaponammo;

   tdialogbox::init();
   action = 0;
   title = "Unit-Values";

   windowstyle = windowstyle ^ dlg_in3d;

   x1 = 20;
   xsize = 600;
   y1 = 0;
   ysize = 600;
   w = (xsize - 60) / 2;
   w2 = (xsize - 40) / 8;
   dirx= x1 + 300;
   diry= y1 + 100;
   action = 0;

   owner = unit->getOwner();


   strcpy ( namebuffer, unit->name.c_str() );

   addbutton("~N~ame",50,80,250,100,1,1,29,true);
   addeingabe(29, namebuffer, 0, 100);

   addbutton("E~x~p of Unit",50,120,250,140,2,1,1,true);
   addeingabe(1, &unit->experience, 0, maxunitexperience);

   addbutton("~D~amage of Unit",50,160,250,180,2,1,2,true);
   addeingabe(2, &unit->damage, 0, 100 );

   addbutton("~F~uel of Unit",50,200,250,220,2,1,3,true);
   addeingabe( 3, &unit->tank.fuel, 0, unit->getStorageCapacity().fuel );

   addbutton("~M~aterial",50,240,250,260,2,1,12,true);
   addeingabe(12,&unit->tank.material, 0, unit->getStorageCapacity().material );

   addbutton("AI Parameter", 50, 280, 250, 300, 0, 1, 115, true );

   addbutton("Unit owner",50,320,250,340,2,1,116,true);
   addeingabe(116, &owner, 0, 7 );

   int unitheights = 0;
   heightxs = 520;
   tfield* fld = getfield ( unit->xpos, unit->ypos);
   if ( fld && fld->vehicle == unit ) {
      npush ( unit->height );
      for ( int i=0;i<=7 ;i++) {
          unit->height = 1 << i;
          if (( ( unit->height & unit->typ->height ) > 0) && ( terrainaccessible( fld, unit ) == 2)) {
             addbutton("",20+( i * w2),heightxs,w2 * (i +1 ),heightxs+24,0,1,i+4,true);
             addkey(i+4,ct_0+i);
          }
          unitheights |= 1 << i;
      } /* endfor  Buttons 4-11*/
      npop ( unit->height );
   }

   if ( unit->typ->hasFunction( ContainerBaseType::NoReactionfire  ))
      unit->reactionfire.disable();

   addbutton("~R~eactionfire",dirx-50,250,dirx+50,260,3,1,22,(unit->typ->hasFunction( ContainerBaseType::NoReactionfire  )) == 0 );
   reactionfire = unit->reactionfire.getStatus();
   addeingabe(22, &reactionfire, 0, lightgray);

   addbutton("~S~et Values",20,ysize - 40,20 + w,ysize - 10,0,1,30,true);
   addkey(30,ct_enter );
   addbutton("~C~ancel",40 + w,ysize - 40,40 + 2 * w,ysize - 10,0,1,31,true);
   addkey(31,ct_esc );

   const int maxeditable = 10;

   for( int i =0;i < unit->typ->weapons.count;i++) {
     if (i < maxeditable) {
        weaponammo = new(char[25]);
        strcpy(weaponammo,"Wpn Ammo ");
        strcat(weaponammo,strrr(i+1));
        addbutton(weaponammo,410,80+i*40,570,100+i*40,2,1,33+i,true);
        addeingabe( 33+i, &unit->ammo[i], 0, unit->typ->weapons.weapon[i].count );
   	} else
   	    showtext2("6 weapons max at the moment",x1+410,y1+80+maxeditable*40);

   }

   buildgraphics();

   mousevisible(false);


   static bool heightIconsLoaded = false;
   static void* iconsheight[8];
   static void* iconspfeil[8];

   
   if ( !heightIconsLoaded ) {
      {
         tnfilestream stream ("height.raw",tnstream::reading);
         int w;
         for ( int i = 0; i <= 7; i++)
            stream.readrlepict( &iconsheight[i],false,&w);
      }
      {
         tnfilestream stream ("pfeil-a0.raw", tnstream::reading);
         for ( int i=0;i<8 ;i++ )
             stream.readrlepict( &iconspfeil[i], false, &w);
      }
   }
         
   
   
   if ( unitheights ) 
       for ( int i=0;i<=7 ;i++) {
           if ( unit->height == (1 << i) )
              bar(x1 + 25+( i * w2),y1 + heightxs-5,x1 + w2 * (i +1 ) - 5,y1 + heightxs-3,red);

           if ( unitheights & ( 1 << i ))
              putimage(x1 + 28+( i * w2), y1 + heightxs + 2 ,iconsheight[i]);
       }

   // 8 im Kreis bis 7
   float radius = 50;
   float pi = 3.14159265;
   for ( int i = 0; i < sidenum; i++ ) {

      int x = (int) (dirx + radius * sin ( 2 * pi * (float) i / (float) sidenum ));
      int y = (int) (diry - radius * cos ( 2 * pi * (float) i / (float) sidenum ));

      addbutton("", x-10, y - 10, x + 10, y + 10,0,1,14+i,true);
      enablebutton ( 14 + i );
      void* pic = rotatepict ( iconspfeil[0], directionangle[i] );
      int h,w;
      getpicsize ( pic, w, h );
      putspriteimage ( x1 + x - w/2, y1 + y - h/2, pic );
      asc_free( pic );
  } /*Buttons 14 - 14 +sidenum*/

   // unit->paint( getActiveSurface(), SPoint( dirx + x1 - fieldsizex/2 ,diry + y1 - fieldsizey/2));
   mousevisible(true);
}

/*
char      UnitPropertyEditor::checkvalue( char id, void* p)
{
   if ( id == 3 ) {
   }
   addbutton("~F~uel of Unit",50,200,250,220,2,1,3,true);
   addeingabe( 3, &unit->fuel, 0, unit->getmaxfuelforweight() );

   addbutton("~M~aterial",50,240,250,260,2,1,12,true);
   addeingabe(12,&unit->material, 0, unit->getmaxmaterialforweight() );
   return  true;
}
*/

void         UnitPropertyEditor::run(void)
{
   do {
      tdialogbox::run();
   }  while ( !action );
}


void         UnitPropertyEditor::buttonpressed(int         id)
{
   int ht;

   switch (id) {
   case 3: /* temp = unit->tank.material;
            unit->tank.material = 0;
            storage = unit->putResource(maxint, Resources::Material, 1 );
            addeingabe(12,&unit->tank.material, 0, storage );
            unit->tank.material = min ( storage, temp );*/
      break;

   case 4 :
   case 5 :
   case 6 :
   case 7 :
   case 8 :
   case 9 :
   case 10 :
   case 11 :
   {
     int h = 1 << ( id - 4 );
     bar(x1 +20,y1 + heightxs-5,x1 + 480,y1 + heightxs-3,lightgray);
     for (int i=0;i<=7 ;i++) {
        ht = 1 << i;
        if ( ht == h ) bar(x1 + 25+( i * w2),y1 + heightxs-5,x1 + w2 * (i +1 ) - 5,y1 + heightxs-3,red);
     } /* endfor */
     unit->height = h;
     unit->setMovement ( unit->typ->movement[ log2 ( unit->height ) ], 0 );
   }
   break;
   case 12: /* temp = unit->tank.fuel;
            unit->tank.fuel = 0;
            storage = unit->putResource(maxint, Resources::Fuel, 1 );
            addeingabe( 3, &unit->tank.fuel, 0, storage );
            unit->tank.fuel = min ( storage, temp );*/
      break;

   case 14 :
   case 15 :
   case 16 :
   case 17 :
   case 18 :
   case 19 :
   case 20 :
   case 21 :  {
                  unit->direction = id-14;
                  bar(dirx + x1 -fieldsizex/2, diry + y1 - fieldsizey/2 ,dirx + x1 + fieldsizex/2 ,diry + y1 +fieldsizey/2,lightgray);
                  // unit->paint( getActiveSurface(), SPoint(dirx + x1 - fieldsizex/2 ,diry + y1 - fieldsizey/2));
               }
         break;
   case 30 : {
         mapsaved = false;
         action = 1;
         if ( reactionfire )  {
            unit->reactionfire.status = Vehicle::ReactionFire::ready;
         } else
            unit->reactionfire.status =  Vehicle::ReactionFire::off;

         unit->name = namebuffer;
         if ( owner != unit->getOwner() )
            unit->convert( owner );
        }
        break;
    case 31 : action = 1;
              tus.restore();
        break;
    case 115: {
                 static int player = 0;
                 player = getid("player",player,0,7);
                 if ( !unit->aiparam[player] )
                    unit->aiparam[player] = new AiParameter ( unit );

                 EditAiParam eap ( unit, player );
                 eap.init();
                 eap.run();
              }
   } /* endswitch */
}


void         changeunitvalues(Vehicle* ae)
{
   if ( !ae )
      return;

   UnitPropertyEditor units ( ae );
   units.init();
   units.run();
   units.done();
}


//* � Resource

     class tres: public tdialogbox {
        tfield* pf2;
            public :
                int action;
                int fuel,material;
                void init(void);
                virtual void run(void);
                virtual void buttonpressed(int id);
                };



void         tres::init(void)
{ int w;

   tdialogbox::init();
   action = 0;
   title = "Resources";
   x1 = 170;
   xsize = 200;
   y1 = 70;
   ysize = 200;
   w = (xsize - 60) / 2;
   action = 0;
   pf2 = getactfield();
   fuel = pf2->fuel;
   material = pf2->material;

   windowstyle = windowstyle ^ dlg_in3d;

   activefontsettings.length = 200;

   addbutton("~F~uel (0-255)",50,70,150,90,2,1,1,true);
   addeingabe(1,&fuel,0,255);
   addbutton("~M~aterial (0-255)",50,110,150,130,2,1,2,true);
   addeingabe(2,&material,0,255);

   addbutton("~S~et Vals",20,ysize - 40,20 + w,ysize - 10,0,1,7,true);
   addkey(7,ct_enter);
   addbutton("~C~ancel",40 + w,ysize - 40,40 + 2 * w,ysize - 10,0,1,8,true);

   buildgraphics();

   mousevisible(true);
}


void         tres::run(void)
{
   do {
      tdialogbox::run();
   }  while (!((taste == ct_esc) || (action == 1)));
}


void         tres::buttonpressed(int         id)
{
   if (id == 7) {
      mapsaved = false;
      action = 1;
      pf2->fuel = fuel;
      pf2->material = material;
   }
   if (id == 8) action = 1;
}


void         changeresource(void)
{
   tres         resource;

   resource.init();
   resource.run();
   resource.done();
}

//* � MineStrength

     class tminestrength: public tdialogbox {
               tfield* pf2;
            public :
                int action;
                int strength;
                void init(void);
                virtual void run(void);
                virtual void buttonpressed(int id);
                };



void         tminestrength::init(void)
{ int w;

   tdialogbox::init();
   action = 0;
   title = "Minestrength";
   x1 = 170;
   xsize = 200;
   y1 = 70;
   ysize = 160;
   w = (xsize - 60) / 2;
   action = 0;
   pf2 = getactfield();
   strength = pf2->mines.begin()->strength;

   windowstyle = windowstyle ^ dlg_in3d;

   activefontsettings.length = 200;

   addbutton("~S~trength (0-255)",30,70,170,90,2,1,1,true);
   addeingabe(1,&strength,0,255);

   addbutton("~S~et Vals",20,ysize - 40,20 + w,ysize - 10,0,1,7,true);
   addkey(7,ct_enter);
   addbutton("~C~ancel",40 + w,ysize - 40,40 + 2 * w,ysize - 10,0,1,8,true);

   buildgraphics();

   mousevisible(true);
}


void         tminestrength::run(void)
{
   do {
      tdialogbox::run();
   }  while (!((taste == ct_esc) || (action == 1)));
}


void         tminestrength::buttonpressed(int         id)
{
   if (id == 7) {
      mapsaved = false;
      action = 1;
      pf2->mines.begin()->strength = strength;
   }
   if (id == 8) action = 1;
}


void         changeminestrength(void)
{
   if ( getactfield()->mines.empty() )
      return;

   tminestrength  ms;
   ms.init();
   ms.run();
   ms.done();
}


#if 0
//* � Laderaum Unit-Cargo

class tladeraum : public tdialogbox {
               protected:
                    int maxusable;
                    int itemsperline;
                    int action;
                    virtual const char* getinfotext ( int pos );
                    void displayinfotext ( void );

                    virtual void additem ( void ) = 0;
                    virtual void removeitem ( int pos ) = 0;
                    virtual void checkforadditionalkeys ( tkey ch );
                    void displayallitems ( void );
                    int mouseoverfield ( int pos );
                    virtual void displaysingleitem ( int pos, int x, int y ) = 0;
                    void displaysingleitem ( int pos );
                    int cursorpos;
                    virtual void finish ( int cancel ) = 0;
                    int getcapabilities ( void ) { return 1; };
                    virtual void redraw ( void );
               public :
                    void init( char* ttl );
                    virtual void run(void);
                    virtual void buttonpressed(int id);
                    void done(void);
                 };

void tladeraum::redraw ( void )
{
   tdialogbox::redraw();
   displayallitems();
}


void         tladeraum::init( char* ttl )
{
  maxusable = 18;

   itemsperline = 8;

   npush ( farbwahl );
   cursorpos = 0;

   tdialogbox::init();
   title = ttl;
   x1 = 20;
   xsize = 600;
   y1 = 50;
   ysize = 400;
   int w = (xsize - 60) / 2;
   action = 0;

   windowstyle = windowstyle ^ dlg_in3d;

   addbutton("~N~ew",20,40,100,70,0,1,1,true);
   addbutton("~E~rase",120,40,220,70,0,1,2,true);

   addbutton("~O~K",20,ysize - 40,20 + w,ysize - 10,0,1,7,true);
   addkey(7,ct_enter);
   addbutton("Cancel",40 + w,ysize - 40,40 + 2 * w,ysize - 10,0,1,8,true);
   addkey(8,ct_esc );

   buildgraphics();

   displayallitems();
   displayinfotext();

}

void tladeraum :: displayallitems ( void )
{
   for ( int i = 0; i < 32; i++ )
      displaysingleitem ( i );
}


void tladeraum :: displaysingleitem ( int pos )
{
   int x = x1 + 28 + (pos % itemsperline) * fielddistx;
   int y = y1 + 88 + (pos / itemsperline) * fielddisty * 3;
   putspriteimage( x, y, icons.stellplatz );
   if ( pos >= maxusable )
      putspriteimage( x, y, icons.X );

   displaysingleitem ( pos, x, y );
}

int tladeraum :: mouseoverfield ( int pos )
{
   int x = x1 + 28 + (pos % itemsperline) * fielddistx;
   int y = y1 + 88 + (pos / itemsperline) * fielddisty * 3;
   return mouseinrect ( x, y, x+fieldsizex, y + fieldsizey );
}

const char* tladeraum :: getinfotext( int pos )
{
   return NULL;
}

void tladeraum :: checkforadditionalkeys ( tkey ch )
{
}

void         tladeraum::displayinfotext ( void )
{
   npush ( activefontsettings );
   activefontsettings.background = lightgray;
   activefontsettings.color = red;
   activefontsettings.length = 185;
   activefontsettings.font = schriften.smallarial;
   const char* it = getinfotext( cursorpos );
   if ( it )
      showtext2(it, x1 + 240, y1 + 50);
   npop ( activefontsettings );
}

void         tladeraum::run(void)
{
   mousevisible( true );
   do {
      tdialogbox::run();

      checkforadditionalkeys ( taste );
      int oldpos = cursorpos;
      switch (taste) {
         case ct_up:
         case ct_8k:   cursorpos -= itemsperline;
            break;
         case ct_left:
         case ct_4k:   cursorpos--;
            break;
         case ct_right:
         case ct_6k:   cursorpos++;
            break;
         case ct_down:
         case ct_2k:   cursorpos += itemsperline;
            break;
      }
      if ( cursorpos < 0 )
         cursorpos = 0;
      if ( cursorpos >= 32 )
         cursorpos = 31;

      if ( mouseparams.taste == 1)
         for ( int i = 0; i < 32; i++ )
            if ( mouseoverfield ( i ))
               cursorpos = i;

      if ( cursorpos != oldpos ) {
         displaysingleitem ( oldpos );
         displaysingleitem ( cursorpos );
         displayinfotext();
      }

   }  while ( !action );
   finish ( action == 1 );

}


void         tladeraum::buttonpressed( int id )
{
   if (id == 1) {
      additem ();
      redraw();
   }
   if (id == 2) {
      removeitem ( cursorpos );
      displayallitems();
   }
   if (id == 7) {
      mapsaved = false;
      action = 2;
   }
   if (id == 8)
      action = 1;

}


void         tladeraum::done(void)
{
   tdialogbox::done();
   npop ( farbwahl );
   ch = 0;
}
#endif
/*
class UnitProductionLimitation : public tladeraum {
              GameMap::UnitProduction::IDsAllowed ids;
              GameMap::UnitProduction& up;
         protected:
              virtual const char* getinfotext ( int pos );
              virtual void additem ( void );
              virtual void removeitem ( int pos );
              void displaysingleitem ( int pos, int x, int y );
              virtual void finish ( int cancel );
          public:
              UnitProductionLimitation ( GameMap::UnitProduction& _up ) : up ( _up ) { ids = up.idsAllowed;  };
              void init (  );
};


const char* UnitProductionLimitation:: getinfotext ( int pos )
{
   if ( ids.size() > pos ) {
      Vehicletype* vt = actmap->getvehicletype_byid( ids[pos] );
      if ( vt )
         return vt->name.c_str();
   }
   return NULL;
}


void UnitProductionLimitation :: init (  )
{
   tladeraum::init ( "Allowed Production" );
}

void UnitProductionLimitation :: displaysingleitem ( int pos, int x, int y )
{
   if ( ids.size() > pos ) {
      Vehicletype* vt = actmap->getvehicletype_byid( ids[pos] );
      if ( vt )
         vt->paint( getActiveSurface(), SPoint ( x, y), farbwahl );
   }
}

void UnitProductionLimitation :: additem  ( void )
{
   Vehicletype* vt = selvehicletype ( ct_invvalue );
   if ( vt ) {
      for ( GameMap::UnitProduction::IDsAllowed::iterator i = ids.begin(); i != ids.end(); i++ )
         if ( *i == vt->id )
            return;

      ids.push_back ( vt->id );
   }
}

void UnitProductionLimitation :: removeitem ( int pos )
{
   if ( pos < ids.size() )
      ids.erase( ids.begin()+pos );
}


void UnitProductionLimitation :: finish ( int cancel )
{
   if ( !cancel ) 
      up.idsAllowed = ids;
}


void unitProductionLimitation(  )
{
   UnitProductionLimitation upl( actmap->unitProduction );
   upl.init();
   upl.run();
   upl.done();

   static bool warned = false;
   if ( warned == false && actmap->getgameparameter(cgp_forbid_unitunit_construction) != 2 ) {
      displaymessage(ASCString("Please note that the limition will only be used \n if the GameParameter 5 (")+ gameparametername[cgp_forbid_unitunit_construction]+") is set to 2 !",3 );
      warned = true;
   }
}
*/


#if 0



class tvehiclecargo : public tladeraum {
                    TemporaryContainerStorage tus;
               protected:
                    Vehicle* transport;
                    virtual const char* getinfotext ( int pos );
                    virtual void additem ( void );
                    virtual void removeitem ( int pos );
                    virtual void checkforadditionalkeys ( tkey ch );
                    void displaysingleitem ( int pos, int x, int y );
                    virtual void finish ( int cancel );

                public:
                    tvehiclecargo ( Vehicle* unit ) : tus ( unit, true ), transport ( unit ) {};
                    void init (  );


};


const char* tvehiclecargo :: getinfotext ( int pos )
{
   if ( transport->cargo.size() <= pos )
      return NULL;
      
   Vehicle* veh = transport->cargo[pos];
   if ( !veh )
      return NULL;
      
   if ( !veh->name.empty() )
      return veh->name.c_str();
   else
      if ( !veh->typ->name.empty() )
         return veh->typ->name.c_str();
      else
         return veh->typ->description.c_str();
}


void tvehiclecargo :: init (  )
{
   tladeraum::init ( "cargo" );
}

void tvehiclecargo :: displaysingleitem ( int pos, int x, int y )
{
   if ( transport->cargo.size() > pos && transport->cargo[pos] )
      transport->cargo[ pos ]->typ->paint( getActiveSurface(), SPoint ( x, y), farbwahl );
}

void tvehiclecargo :: additem  ( void )
{
 //  selcargo ( transport );
}

void tvehiclecargo :: removeitem ( int pos )
{
   if ( transport->cargo.size() > pos && transport->cargo[pos] ) {
      delete transport->cargo[ pos ] ;
      transport->cargo[ pos ] = NULL;
   }
}

void tvehiclecargo :: checkforadditionalkeys ( tkey ch )
{
   if ( transport->cargo.size() > cursorpos && transport->cargo[cursorpos] ) {
   
       if ( ch == ct_p )
          changeunitvalues( transport->cargo[ cursorpos ] );
       if ( ch == ct_c )
          cargoEditor( transport->cargo[ cursorpos ] );

       if ( ch == ct_c + ct_stp )
          if ( transport->cargo[ cursorpos ] ) {
             ClipBoard::Instance().clear();
             ClipBoard::Instance().addUnit( transport->cargo[ cursorpos ] );
          }
   }
   if ( ch == ct_v + ct_stp ) {
      Vehicle* veh = ClipBoard::Instance().pasteUnit();
      if ( transport->vehicleFit( veh )) {
         transport->addToCargo( veh );
         redraw();
      } else   
         delete veh;
   }
}


void tvehiclecargo :: finish ( int cancel )
{
   if ( cancel )
      tus.restore();
}


#endif

#if 0

class SelectFromContainer : public tladeraum {
               protected:
                    ContainerBase* transport;
                    virtual const char* getinfotext ( int pos );
                    virtual void additem ( void );
                    virtual void removeitem ( int pos );
                    virtual void checkforadditionalkeys ( tkey ch );
                    void displaysingleitem ( int pos, int x, int y );
                    virtual void finish ( int cancel );

                public:
                    SelectFromContainer ( ContainerBase* unit ) : transport ( unit ), unit(NULL) {};
                    void init (  );
                    Vehicle* unit;


};


const char* SelectFromContainer :: getinfotext ( int pos )
{
   if ( transport->loading[ pos ] )
      if ( !transport->loading[ pos ]->name.empty() )
         return transport->loading[ pos ]->name.c_str();
      else
         if ( !transport->loading[ pos ]->typ->name.empty() )
            return transport->loading[ pos ]->typ->name.c_str();
         else
            return transport->loading[ pos ]->typ->description.c_str();
   return NULL;
}


void SelectFromContainer :: init (  )
{
   tladeraum::init ( "select unit" );
}

void SelectFromContainer :: displaysingleitem ( int pos, int x, int y )
{
   if ( transport->loading[ pos ] )
      transport->loading[ pos ]->typ->paint( getActiveSurface(), SPoint( x, y), farbwahl );
}

void SelectFromContainer :: additem  ( void )
{
}

void SelectFromContainer :: removeitem ( int pos )
{
}

void SelectFromContainer :: checkforadditionalkeys ( tkey ch )
{
   if ( transport->loading[ cursorpos ] ) {
       if ( ch == ct_c )
          unit = selectUnitFromContainer( transport->loading[ cursorpos ] );

       if ( ch == ct_enter || ch == ct_space  )
          if ( transport->loading[ cursorpos ] )
             unit = transport->loading[ cursorpos ];

       if ( unit )
          action = 1;
   }
}


void SelectFromContainer :: finish ( int cancel )
{
}

#endif

Vehicle* selectUnitFromContainer( ContainerBase* container )
{
#if 0
   SelectFromContainer sfc ( container );
   sfc.init();
   sfc.run();
   sfc.done();
   return sfc.unit;
   #endif
   return NULL;
}



//* � Laderaum2 Building-Cargo

#if 0

class tbuildingcargoprod : public tladeraum {
                    TemporaryContainerStorage tus;
               protected:
                    Building* building;
                    void finish ( int cancel );
                public:
                    tbuildingcargoprod ( Building* bld ) : tus ( bld, true ), building ( bld ) {};
       };


void tbuildingcargoprod :: finish ( int cancel )
{
   if ( cancel )
      tus.restore();
}
#endif
#if 0
class tbuildingcargo : public tbuildingcargoprod {
               protected:
                    virtual const char* getinfotext ( int pos );
                    virtual void additem ( void );
                    virtual void removeitem ( int pos );
                    virtual void checkforadditionalkeys ( tkey ch );
                    void displaysingleitem ( int pos, int x, int y );
               public:
                    tbuildingcargo ( Building* bld ) : tbuildingcargoprod ( bld ) {};
   };



void tbuildingcargo :: displaysingleitem ( int pos, int x, int y )
{
   if ( building->loading[ pos ] )
      building->loading[ pos ]->typ->paint( getActiveSurface(), SPoint( x, y), farbwahl );
}

void tbuildingcargo :: additem  ( void )
{
   selcargo ( building );
}

void tbuildingcargo :: removeitem ( int pos )
{
   if ( building->loading[ pos ] ) {
      delete building->loading[ pos ] ;
      building->loading[ pos ] = NULL;
   }
}

void tbuildingcargo :: checkforadditionalkeys ( tkey ch )
{
   if ( building->loading[ cursorpos ] ) {
       if ( ch == ct_p )
          changeunitvalues( building->loading[ cursorpos ] );
       if ( ch == ct_c )
          unit_cargo( building->loading[ cursorpos ] );

       if ( ch == ct_c + ct_stp )
          if ( building->loading[ cursorpos ] ) {
             ClipBoard::Instance().clear();
             ClipBoard::Instance().addUnit( building->loading[ cursorpos ] );
          }
   }
   if ( ch == ct_v + ct_stp ) {
      Vehicle* veh = ClipBoard::Instance().pasteUnit();
      if ( building->vehicleFit( veh ))
         for ( int i = 0; i < 32; i++ )
            if ( !building->loading[i] ) {
               veh->convert( log2(building->color) );
               building->loading[i] = veh;
               redraw();
               return;
            }
      delete veh;
   }
}

const char* tbuildingcargo :: getinfotext ( int pos )
{
   if ( building->loading[ pos ] )
      if ( !building->loading[ pos ]->name.empty() )
         return building->loading[ pos ]->name.c_str();
      else
         if ( !building->loading[ pos ]->typ->name.empty() )
            return building->loading[ pos ]->typ->name.c_str();
         else
            return building->loading[ pos ]->typ->description.c_str();
   return NULL;
}


void         building_cargo( Building* bld )
{
   if ( bld  ) {
      tbuildingcargo laderaum ( bld );
      laderaum.init( "cargo" );
      laderaum.run();
      laderaum.done();
   }
}


//* � Production Building-Production


class tbuildingproduction : public tbuildingcargoprod {
               protected:
                    virtual const char* getinfotext ( int pos );
                    virtual void additem ( void );
                    virtual void removeitem ( int pos );
                    void displaysingleitem ( int pos, int x, int y );
               public:
                    tbuildingproduction ( Building* bld ) : tbuildingcargoprod ( bld ) {};
   };

void tbuildingproduction :: displaysingleitem ( int pos, int x, int y )
{
   if ( building->unitProduction[ pos ] )
      building->unitProduction[ pos ]->paint( getActiveSurface(), SPoint ( x, y), farbwahl );
}

void tbuildingproduction :: additem  ( void )
{
   selbuildingproduction ( building );
}

void tbuildingproduction :: removeitem ( int pos )
{
   building->unitProduction[ pos ] = NULL;
}

const char* tbuildingproduction :: getinfotext ( int pos )
{
   if ( building->unitProduction[ pos ] )
      if ( !building->unitProduction[ pos ]->name.empty() )
         return building->unitProduction[ pos ]->name.c_str();
      else
         return building->unitProduction[ pos ]->description.c_str();
   return NULL;
}

bool isNull(const Vehicletype* v ) { return !v; };
#endif


void movebuilding ( void )
{
   warning("sorry, not implemented yet in ASC2!");
#if 0
   mapsaved = false;
   tfield* fld = getactfield();
   if ( fld->vehicle ) {
      Vehicle* v = fld->vehicle;
      fld->vehicle = NULL;

      int x = v->xpos;
      int y = v->ypos;
      int res2 = -1;
      do {
         res2 = selectfield ( &x, &y );
         if ( res2 > 0 ) {
            if ( getfield(x,y)->vehicle || getfield(x,y)->vehicle  || !terrainaccessible(getfield(x,y),v)) {
               displaymessage ( "you cannot place the vehicle here", 1 );
               res2 = -1;
            } else {
               getfield(x,y)->vehicle = v;
               v->setnewposition( x, y );
            }
         }
      } while ( res2 < 0  );
      displaymap();

   }
   if ( fld->building ) {
      Building* bld = fld->building;

      bld->unchainbuildingfromfield ();

      MapCoordinate mc = bld->getEntry();
      MapCoordinate oldPosition = mc;
      int res2 = -1;
      do {
         res2 = selectfield ( &mc.x, &mc.y );
         if ( res2 > 0 ) {
            int res = bld->chainbuildingtofield ( mc );
            if ( res ) {
               displaymessage ( "you cannot place the building here", 1 );
               res2 = -1;
            }
         }
      } while ( res2 < 0  );
      if ( res2 == 0 ) {   // operation canceled
         if ( bld->chainbuildingtofield ( oldPosition ))
            displaymessage ( "severe inconsistency in movebuilding !", 1 );
      }
      displaymap();
   }
#endif
}


#if 0
class SelectUnitSet : public tdialogbox {
               int* active;
               int action;
           public :
               SelectUnitSet() : active(NULL) {};
               void init(void);
               virtual void run(void);
               virtual void buttonpressed(int id);
               ~SelectUnitSet() { delete[] active; };
           };


void         SelectUnitSet::init(void)
{

   tdialogbox::init();
   action = 0;
   title = "Disable Items";
   x1 = 90;
   xsize = 445;
   ysize = 580;
   int w = (xsize - 60) / 2;
   action = 0;

   windowstyle = windowstyle ^ dlg_in3d;


   active = new int[ItemFiltrationSystem::itemFilters.size()];

   for ( int i = 0; i < ItemFiltrationSystem::itemFilters.size(); i++ ) {
     active[i]=  ItemFiltrationSystem::itemFilters[i]->isActive() ;

     addbutton ( ItemFiltrationSystem::itemFilters[i]->name.c_str(), 30, 60 + i * 20, xsize - 50, 80 + i * 20, 3, 0, 10 + i, 1 );
     addeingabe ( 10 + i, &active[i], black, dblue );
   }

   addbutton("~O~k",20,ysize - 40,20 + w,ysize - 10,0,1,7,true);
   addkey(7,ct_enter );

   addbutton("~C~ancel",40 + w,ysize - 40,40 + 2 * w,ysize - 10,0,1,8,true);
   addkey(8, ct_esc );

   buildgraphics();

   mousevisible(true);
}


void         SelectUnitSet::run(void)
{
   do {
      tdialogbox::run();
   }  while ( action == 0 );
}


void         SelectUnitSet::buttonpressed(int         id)
{
   switch(id) {
       case 7: {
                  action = 1;
                  for ( int i = 0; i < ItemFiltrationSystem::itemFilters.size(); i++ )
                      ItemFiltrationSystem::itemFilters[i]->setActive( active[i] );

                  resetvehicleselector();
                  resetbuildingselector();
                  resetterrainselector();
                  resetobjectselector();
          }
          break;
       case 8: action = 2;
          break;
  }
}
#endif

void selectunitsetfilter ( void )
{
   if ( ItemFiltrationSystem::itemFilters.size() > 0 ) {

      vector<ASCString> buttons;
      buttons.push_back ( "~H~ide set" );
      buttons.push_back ( "~S~how set" );
      buttons.push_back ( "~S~how set only");
      buttons.push_back ( "~S~how all sets");
      buttons.push_back ( "~O~k" );

      pair<int,int> playerRes;
      do {
         vector<ASCString> filter;

         for ( int i = 0; i < ItemFiltrationSystem::itemFilters.size(); i++ ) {
            ASCString s;
            if ( ItemFiltrationSystem::itemFilters[i]->isActive() )
               s = "! ";

            s += ItemFiltrationSystem::itemFilters[i]->name;

            filter.push_back ( s );
         }

         playerRes = chooseString ( "Filter", filter, buttons );
         if ( playerRes.first == 0 && playerRes.second >= 0)
            ItemFiltrationSystem::itemFilters[playerRes.second]->setActive( true );

         if ( playerRes.first == 1 && playerRes.second >= 0)
            ItemFiltrationSystem::itemFilters[playerRes.second]->setActive( false );

         if ( playerRes.first == 2 && playerRes.second >= 0)
            for ( int i = 0; i < ItemFiltrationSystem::itemFilters.size(); i++ )
               if ( i == playerRes.second )
                  ItemFiltrationSystem::itemFilters[i]->setActive(false);
               else
                  ItemFiltrationSystem::itemFilters[i]->setActive(true);

         if ( playerRes.first == 3 )
            for ( int i = 0; i < ItemFiltrationSystem::itemFilters.size(); i++ )
               ItemFiltrationSystem::itemFilters[i]->setActive(false);


      } while ( playerRes.first != 4 );

      filtersChangedSignal();
   } else
      displaymessage ( " no Filters defined !", 1 );
}


class UnitTypeTransformation {

              class   UnitSetSelection : public tstringselect {
                       public :
                             virtual void setup(void);
                             virtual void buttonpressed(int id);
                             virtual void run(void);
                             virtual void get_text(int nr);
                         };
              class   TranslationTableSelection : public tstringselect {
                              int unitsetnum;
                         public :
                               virtual void setup( void );
                               void setup2 ( int _unitset ) { unitsetnum = _unitset; };
                               virtual void buttonpressed(int id);
                               virtual void run(void);
                               virtual void get_text(int nr);
                           };

                int unitstransformed;
                int unitsnottransformed;

                Vehicletype* transformvehicletype ( const Vehicletype* type, int unitsetnum, int translationnum );
                void transformvehicle ( Vehicle* veh, int unitsetnum, int translationnum );
                dynamic_array<int> vehicleTypesNotTransformed;
                int vehicleTypesNotTransformedNum ;
             public:
                 void run ( void );
      } ;


void         UnitTypeTransformation :: UnitSetSelection::setup(void)
{
   action = 0;
   title = "Select UnitSet";
   numberoflines = unitSets.size();
   ey = ysize - 60;
   addbutton("~D~one",20,ysize - 40,170,ysize - 20,0,1,202,true);
   addkey(202,ct_enter);
   addbutton("~C~ancel",190,ysize - 40,340,ysize - 20,0,1,203,true);
   addkey(203,ct_esc);
}

void         UnitTypeTransformation :: UnitSetSelection::buttonpressed(int         id)
{
   tstringselect::buttonpressed(id);
   switch (id) {
      case 202:
      case 203:   action = id - 200;
                break;
   }
}

void         UnitTypeTransformation :: UnitSetSelection::get_text(int nr)
{
   strcpy(txt,unitSets[nr]->name.c_str() );
}

void         UnitTypeTransformation :: UnitSetSelection::run(void)
{
   do {
      tstringselect::run();
   }  while ( ! ( action > 0 || (msel == 1)) );

   if ( action == 3)
      redline = -1;
}



void         UnitTypeTransformation :: TranslationTableSelection::setup( void )
{
   action = 0;
   title = "Select Transformation Table";
   numberoflines = unitSets[unitsetnum]->transtab.size();
   ey = ysize - 60;
   addbutton("~D~one",20,ysize - 40,170,ysize - 20,0,1,202,true);
   addkey(202,ct_enter);
   addbutton("~C~ancel",190,ysize - 40,340,ysize - 20,0,1,203,true);
   addkey(203,ct_esc);
}

void         UnitTypeTransformation :: TranslationTableSelection::buttonpressed(int         id)
{
   tstringselect::buttonpressed(id);
   switch (id) {
      case 202:
      case 203:   action = id-200;
                break;
   }
}

void         UnitTypeTransformation :: TranslationTableSelection::get_text(int nr)
{ 
   strcpy(txt, unitSets[unitsetnum]->transtab[nr]->name.c_str() );
} 

void         UnitTypeTransformation :: TranslationTableSelection::run(void)
{ 
   do { 
      tstringselect::run(); 
   }  while ( ! ( action > 0 || (msel == 1)) );

   if ( action == 3) 
      redline = -1;
} 

Vehicletype* UnitTypeTransformation :: transformvehicletype ( const Vehicletype* type, int unitsetnum, int translationnum )
{
   for ( int i = 0; i < unitSets[unitsetnum]->transtab[translationnum]->translation.size(); i++ )
      if ( unitSets[unitsetnum]->transtab[translationnum]->translation[i].from == type->id ) {
         Vehicletype* tp = vehicleTypeRepository.getObject_byID ( unitSets[unitsetnum]->transtab[translationnum]->translation[i].to );
         if ( tp ) 
            return tp;
      }

   int fnd = 0;
   for ( int j = 0; j < vehicleTypesNotTransformedNum; j++ )
       if ( vehicleTypesNotTransformed[j] == type->id )
          fnd ++;

   if ( !fnd ) 
      vehicleTypesNotTransformed[vehicleTypesNotTransformedNum++] = type->id;

   return NULL;
}

void  UnitTypeTransformation ::transformvehicle ( Vehicle* veh, int unitsetnum, int translationnum )
{
   for ( ContainerBase::Cargo::const_iterator i = veh->getCargo().begin(); i != veh->getCargo().end(); ++i )
      if ( *i )
         transformvehicle ( *i, unitsetnum, translationnum );

   Vehicletype* nvt = transformvehicletype ( veh->typ, unitsetnum, translationnum );
   if ( !nvt ) {
      unitsnottransformed++;
      return;
   }
   veh->transform ( nvt );

   unitstransformed++;
}


void UnitTypeTransformation :: run ( void )
{
   vehicleTypesNotTransformedNum = 0;

   UnitSetSelection uss;
   uss.init();
   uss.run();
   uss.done();

   int unitsetnum = uss.redline;
   if ( unitsetnum == -1 )
      return;

   TranslationTableSelection tss;
   tss.setup2( unitsetnum );
   tss.init();
   tss.run();
   tss.done();

   int translationsetnum = tss.redline;
   if ( translationsetnum == -1 || translationsetnum == 255 )
      return;

   unitstransformed = 0;
   unitsnottransformed = 0;


   for ( int y = 0; y < actmap->ysize; y++ )
      for ( int x = 0; x < actmap->xsize; x++ ) {
         tfield* fld = getfield ( x, y );
         if ( fld->vehicle )
            transformvehicle ( fld->vehicle, unitsetnum, translationsetnum );
         if ( fld->building && (fld->bdt & getTerrainBitType(cbbuildingentry) ).any() ) {
            for ( ContainerBase::Cargo::const_iterator i = fld->building->getCargo().begin(); i != fld->building->getCargo().end(); ++i )
               if ( *i )
                  transformvehicle ( *i, unitsetnum, translationsetnum );
                  
            ContainerBase::Production prod = fld->building->getProduction();
            for ( ContainerBase::Production::iterator j = prod.begin(); j != prod.end(); ++j )
               if ( *j && transformvehicletype ( *j, unitsetnum, translationsetnum ) ) {
                  *j = transformvehicletype ( *j, unitsetnum, translationsetnum );
                  ++unitstransformed;
               }
               
            fld->building->setProductionLines( prod );
         }   
      }

    if ( vehicleTypesNotTransformedNum ) {
       ASCString s = "The following vehicles could not be transformed: ";
       for ( int i = 0; i < vehicleTypesNotTransformedNum; i++ ) {
          s += "\n ID ";
          s += strrr ( vehicleTypesNotTransformed[i] );
          s += " : ";
          Vehicletype* vt = vehicleTypeRepository.getObject_byID ( vehicleTypesNotTransformed[i] );
          if ( !vt-> name.empty() )
             s += vt->name;
          else
             s += vt->description;
       }
       
       s += "\n";
       s += strrr ( unitstransformed );
       s += " units were transformed\n";
       s += strrr ( unitsnottransformed );
       s += " units were NOT transformed\n (production included)";

       tviewanytext vat;
       vat.init ( "warning", s.c_str() );
       vat.run();
       vat.done();
    } else 
       displaymessage ( "All units were transformed !\ntotal number: %d", 3, unitstransformed );


}

void unitsettransformation( void )
{
   UnitTypeTransformation utt;
   utt.run();
}


void MapSwitcher::deleteMaps()
{
   delete actmap;
   actmap = NULL;
   toggle();
   delete actmap;
   actmap = NULL;
}

void MapSwitcher :: toggle ( )
{
   maps[active].map = actmap;
   maps[active].changed = !mapsaved;
   
   if ( getMainScreenWidget() ) {
      MapDisplayPG* md = getMainScreenWidget()->getMapDisplay();
      if ( md )
         maps[active].windowpos = md->upperLeftCorner();
   }
   
   active = !active;

   

   actmap = maps[active].map;
   mapsaved = !maps[active].changed;


   if ( getMainScreenWidget() ) {
      MapDisplayPG* md = getMainScreenWidget()->getMapDisplay();
      if ( md )
         md->cursor.goTo( actmap->getCursor(), maps[active].windowpos );
   }

   displaymap();
   viewChanged();

   int x,y;
   SDL_Event ev;
   while ( PG_Application::GetEventSupplier()->GetMouseState(x,y))
      PG_Application::GetEventSupplier()->WaitEvent (&ev);
}

string MapSwitcher :: getName ()
{
  const char* name[2] = { "primary", "secondary" };
  string s;
  s = name[active];

  if ( actmap )
     if ( !actmap->preferredFileNames.mapname[0].empty() ) {
        s += " ( ";
        s += actmap->preferredFileNames.mapname[0];
        s += " ) ";
     }

  return s;
}

MapSwitcher::Action MapSwitcher :: getDefaultAction ( )
{
   if ( active )
      return select;
   else
      return set;
}


MapSwitcher mapSwitcher;


Vehicletype* transform( int id, const vector<int>& translation )
{
   for ( int i = 0; i < translation.size()/2; i++ )
      if ( id == translation[i*2] ) 
         return vehicleTypeRepository.getObject_byID ( translation[i*2+1] );
   return NULL;
}

void transformMap ( )
{

   ASCString filename = selectFile( "*.ascmapxlat", true );

   if ( filename.empty() )
      return;


   vector<int> terraintranslation;
   vector<int> objecttranslation;
   vector<int> terrainobjtranslation;
   vector<int> buildingtranslation;
   vector<int> vehicletranslation;
   try {
      tnfilestream s ( filename, tnstream::reading );

      TextFormatParser tfp ( &s );
      TextPropertyGroup* tpg = tfp.run();

      PropertyReadingContainer pc ( "maptranslation", tpg );

      pc.addIntegerArray ( "TerrainTranslation", terraintranslation );
      pc.addIntegerArray ( "TerrainObjTranslation", terrainobjtranslation );
      pc.addIntegerArray ( "ObjectTranslation", objecttranslation );
      pc.addIntegerArray ( "BuildingTranslation", buildingtranslation );
      if ( pc.find( "VehicleTranslation" ))
         pc.addIntegerArray ( "VehicleTranslation", vehicletranslation );

      delete tpg;
   }
   catch ( ParsingError err ) {
      displaymessage ( "Error parsing file : " + err.getMessage() , 1 );
      return;
   }
   catch ( ASCexception ) {
      displaymessage ( "Error reading file " + filename , 1 );
      return;
   }

   if ( terraintranslation.size() % 2 ) {
      displaymessage ( "Map Translation : terraintranslation - Invalid number of entries ", 1);
      return;
   }

   if ( terrainobjtranslation.size() % 3 ) {
      displaymessage ( "Map Translation : terrainobjtranslation - Invalid number of entries ", 1);
      return;
   }

   if ( objecttranslation.size() % 2 ) {
      displaymessage ( "Map Translation : objecttranslation - Invalid number of entries ", 1);
      return;
   }

   if ( buildingtranslation.size() % 2 ) {
      displaymessage ( "Map Translation : buildingtranslation - Invalid number of entries ", 1);
      return;
   }

   if ( vehicletranslation.size() % 2 ) {
      displaymessage ( "Map Translation : buildingtranslation - Invalid number of entries ", 1);
      return;
   }


   for ( int y = 0; y < actmap->ysize; y++ )
      for ( int x = 0; x < actmap->xsize; x++ ) {
          tfield* fld = actmap->getField ( x, y );
          for ( int i = 0; i < terraintranslation.size()/2; i++ )
             if ( fld->typ->terraintype->id == terraintranslation[i*2] ) {
                TerrainType* tt = terrainTypeRepository.getObject_byID ( terraintranslation[i*2+1] );
                if ( tt ) {
                   TerrainType::Weather* tw = tt->weather[fld->getweather()];
                   if ( !tw )
                      tw = tt->weather[0];
                   fld->typ = tw;
                   fld->setparams();
                }
             }

          for ( int i = 0; i < terrainobjtranslation.size()/3; i++ )
             if ( fld->typ->terraintype->id == terrainobjtranslation[i*3] ) {
                TerrainType* tt = terrainTypeRepository.getObject_byID ( terrainobjtranslation[i*3+1] );
                if ( tt ) {
                   TerrainType::Weather* tw = tt->weather[fld->getweather()];
                   if ( !tw )
                      tw = tt->weather[0];
                   fld->typ = tw;
                   fld->addobject ( objectTypeRepository.getObject_byID ( terrainobjtranslation[i*3+2] ), -1, true );
                   fld->setparams();
                }
             }


          for ( int j = 0; j < fld->objects.size(); ++j ) 
             for ( int i = 0; i < objecttranslation.size() / 2; i++ )
                if ( fld->objects[j].typ->id == objecttranslation[i*2] ) {
                   ObjectType* ot = objectTypeRepository.getObject_byID ( objecttranslation[i*2+1] );
                   if ( ot ) {
                      fld->objects[j].typ = ot;
                      fld->sortobjects();
                      fld->setparams();
                      j = -1; // restarting the outer loop
                      break;
                   } else {
                      fld->removeobject( fld->objects[j].typ, true );
                      j = -1;
                      break;
                   }
                }
          for ( int b = 0; b < buildingtranslation.size()/2; ++b )
             if ( (fld->bdt & getTerrainBitType(cbbuildingentry)).any() && fld->building && fld->building->typ->id == buildingtranslation[b*2] ) {
               BuildingType* newtype = buildingTypeRepository.getObject_byID ( buildingtranslation[b*2+1] );
               if ( newtype ) {
                  Building* bld  = fld->building;
                  const BuildingType* orgtype = fld->building->typ;
                  MapCoordinate orgpos = bld->getEntry();
                  MapCoordinate pos = orgpos;
                  bld->unchainbuildingfromfield();
                  bld->typ = newtype;
                  int iteration = 0;
                  enum { trying, success, failed } state = trying;
                  do {
                     if ( bld->chainbuildingtofield( pos )) {
                        pos = getNeighbouringFieldCoordinate( orgpos, iteration );
                     } else
                        state = success;

                     ++iteration;

                     if ( iteration > 6 && state != success ) {
                        state = failed;
                        bld->typ = orgtype;
                        bld->chainbuildingtofield( orgpos );
                     }
                  } while ( state == trying );

               } else {
                  delete fld->building;
               }
             }

   }

   for( int p = 0; p < actmap->getPlayerCount(); ++p ) {
      Player::VehicleList::iterator di = actmap->getPlayer(p).vehicleList.begin();
      while ( di != actmap->getPlayer(p).vehicleList.end() ) {
         bool found = false;
         for ( int i = 0; i < vehicletranslation.size()/2; ++i )
            if ( (*di)->typ->id == vehicletranslation[i*2] && vehicletranslation[i*2+1] <= 0 ) {
               delete *di;
               found = true;
               break;
            }
         if ( found )
            di = actmap->getPlayer(p).vehicleList.begin();
         else
            ++di;
      }
         


      for ( Player::VehicleList::iterator i = actmap->getPlayer(p).vehicleList.begin(); i != actmap->getPlayer(p).vehicleList.end(); ++i) {
         (*i)->transform( transform( (*i)->typ->id, vehicletranslation));
         
         ContainerBase::Production prod = (*i)->getProduction();
         for ( ContainerBase::Production::iterator j = prod.begin(); j != prod.end(); ++j )
            if ( *j && transform((*j)->id, vehicletranslation) ) 
               *j = transform((*j)->id, vehicletranslation);
            
         (*i)->setProductionLines( prod );
      }
      for ( Player::BuildingList::iterator i = actmap->getPlayer(p).buildingList.begin(); i != actmap->getPlayer(p).buildingList.end(); ++i) {
         ContainerBase::Production prod = (*i)->getProduction();
         for ( ContainerBase::Production::iterator j = prod.begin(); j != prod.end(); ++j )
            if ( *j && transform((*j)->id, vehicletranslation) ) 
               *j = transform((*j)->id, vehicletranslation);
            
         (*i)->setProductionLines( prod );
      }
   }

   // for ( int i = 0; i < vehicleTypeRepository.getNum(); ++i )

      /*
   tn_file_buf_stream s ( "out.txt", tnstream::writing );
   for ( int j = 0; j < vehicletranslation.size()/2; j++ ) {
      Vehicletype* vt1 = vehicleTypeRepository.getObject_byID ( vehicletranslation[j*2] );
      Vehicletype* vt2 = vehicleTypeRepository.getObject_byID ( vehicletranslation[j*2+1] );
      ASCString st = ASCString::toString( vehicletranslation[j*2] ) + " " + 
                     ASCString::toString( vehicletranslation[j*2+1] );
      

      if ( vt1 ) {
         st += " ; " + vt1->name;
         if ( vt2 ) 
            st += " -> " + vt2->name;
      }

      s.writeString( st + "\n" );
   }
      */

   calculateallobjects();
   displaymap();
}


class EditArchivalInformation : public tdialogbox {
         GameMap* gamemap;
         char maptitle[10000];
         char author[10000];
         ASCString description;
         char tags[10000];
         char requirements[10000];
         int action;
       public:
         EditArchivalInformation ( GameMap* map );
         void init();
         void run();
         void buttonpressed ( int id );
};


EditArchivalInformation :: EditArchivalInformation ( GameMap* map ) : gamemap ( map )
{
  strcpy ( maptitle, map->maptitle.c_str() );
  strcpy ( author, map->archivalInformation.author.c_str() );
  description = map->archivalInformation.description;
  strcpy ( tags, map->archivalInformation.tags.c_str() );
  strcpy ( requirements, map->archivalInformation.requirements.c_str() );
}

void EditArchivalInformation::init()
{
   tdialogbox::init();
   title = "Edit Archival Information";
   xsize = 600;
   ysize = 410;
   action = 0;


   addbutton("~T~itle",20,70,xsize - 20,90,1,1,10,true);
   addeingabe(10,maptitle,0,10000);

   addbutton("~A~uthor",20,120,xsize - 20,140,1,1,11,true);
   addeingabe(11,author,0,10000);

   addbutton("Ta~g~s",20,170,xsize - 20,190,1,1,12,true);
   addeingabe(12,tags,0,10000);

   addbutton("~R~equirements ( see http://asc-hq.sf.net/req.php for possible values)",20,220,xsize - 20,240,1,1,13,true);
   addeingabe(13,requirements,0,10000);


   addbutton("Edit ~D~escription",20,ysize - 70,170,ysize - 50,0,1,14,true);

   addbutton("~O~k",20,ysize - 40,xsize/2-10,ysize - 20,0,1,1,true);
   addkey(1,ct_enter);
   addbutton("~C~ancel",xsize/2+10,ysize - 40,xsize-20,ysize - 20,0,1,2,true);
   addkey(2,ct_esc);

   buildgraphics();
}

void EditArchivalInformation :: buttonpressed ( int id )
{
   switch ( id ) {
      case 14: {
                  MultilineEdit mle ( description, "Map Description" );
                  mle.init();
                  mle.run();
                  mle.done();
                  break;
                }
      case 1: gamemap->archivalInformation.author = author;
              gamemap->archivalInformation.tags   = tags;
              gamemap->archivalInformation.requirements   = requirements;
              gamemap->archivalInformation.description   = description;
              gamemap->maptitle = maptitle;
              mapsaved = false;
      case 2: action = id;
   }
}

void EditArchivalInformation :: run ()
{
   do {
      tdialogbox::run();
   } while ( action == 0);
}


void editArchivalInformation()
{
  EditArchivalInformation eai ( actmap );
  eai.init();
  eai.run();
  eai.done();
}

void resourceComparison ( )
{
   ASCString s;
   for ( int i = 0; i < 9; i++ ) {
      s += ASCString("player ") + strrr ( i ) + " : ";
      Resources plus;
      Resources have;
      for ( Player::BuildingList::iterator b = actmap->player[i].buildingList.begin(); b != actmap->player[i].buildingList.end(); ++b ) {
         if ( actmap->_resourcemode == 0 )
            plus += (*b)->plus;
         else
            plus += (*b)->bi_resourceplus;

         have += (*b)->actstorage;
      }
      for ( int r = 0; r < resourceTypeNum; r++ ) {
         s += ASCString(resourceNames[r]) + " " + strrr ( have.resource(r)) + " +";
         s += strrr(plus.resource(r));
         s += "; ";
      }

      s += "\n\n";
   }

   Resources res;
   for ( int x = 0; x < actmap->xsize; x++)
      for ( int y = 0; y < actmap->ysize; y++ ) {
          res.material += actmap->getField(x,y)->material;
          res.fuel     += actmap->getField(x,y)->fuel;
      }

   s += "Mineral Resources: ";
   s += strrr(res.material );
   s += " Material, ";
   s += strrr(res.material );
   s += " Fuel";

   displaymessage ( s, 3 );
}




void readClipboard()
{
   ASCString filename = selectFile(clipboardFileExtension, true);
   if ( !filename.empty() ) {
      tnfilestream stream ( filename, tnstream::reading );
      ClipBoard::Instance().read( stream );
   }
}

void saveClipboard()
{
   ASCString filename = selectFile( clipboardFileExtension, false);
   if ( !filename.empty() ) {
      tnfilestream stream ( filename, tnstream::writing );
      ClipBoard::Instance().write( stream );
   }
}

void setweatherall ( int weather  )
{
   for (int y=0; y < actmap->ysize; y++)
     for (int x=0; x < actmap->xsize; x++)
        actmap->getField(x,y)->setweather( weather );
}


ASCString printTech( int id )
{
   ASCString s;
   s.format ( "%7d ", id );
   const Technology* t = technologyRepository.getObject_byID ( id );
   if ( t )
      s += t->name;

   return s;
}

void editResearch()
{
   vector<ASCString> buttons;
   buttons.push_back ( "~A~dd" );
   buttons.push_back ( "~R~emove" );
   buttons.push_back ( "~C~lose" );

   vector<ASCString> buttons2;
   buttons2.push_back ( "~A~dd" );
   buttons2.push_back ( "~C~ancel" );

   vector<ASCString> buttonsP;
   buttonsP.push_back ( "~E~dit" );
   buttonsP.push_back ( "~C~lose" );

   pair<int,int> playerRes;
   do {
      vector<ASCString> player;
      for ( int i = 0; i < 8; ++i )
         player.push_back ( ASCString ( strrr(i)) + " " + actmap->player[i].getName());

      playerRes = chooseString ( "Choose Player", player, buttonsP );
      if ( playerRes.first == 0 && playerRes.second >= 0 ) {
         int player = playerRes.second;

         pair<int,int> res;
         do {
            vector<int>& devTech = actmap->player[player].research.developedTechnologies;
            vector<ASCString> techs;
            map<ASCString,int> techIds;
            for ( int i = 0; i < devTech.size(); ++i ) {
               int id = devTech[i];
               ASCString s = printTech(id );
               techs.push_back ( s );
               techIds[s] = id;
            }
            sort (techs.begin(), techs.end() );
            res = chooseString ( "Available Technologies", techs, buttons );
            if ( res.first == 0 ) {
               vector<ASCString> techs;
               vector<int> techIds;
               for ( int i = 0; i < technologyRepository.getNum(); ++i ) {
                  const Technology* t = technologyRepository.getObject_byPos(i);
                  if ( find ( devTech.begin(), devTech.end(), t->id ) == devTech.end() )
                     if ( !ItemFiltrationSystem::isFiltered( ItemFiltrationSystem::Technology, t->id )) {
                        techs.push_back ( printTech ( t->id ));
                        techIds.push_back ( t->id );
                     }
               }

               if ( techIds.empty() ) {
                  infoMessage("No technologies found. Please review Item Filters");
               } else {
                  // sort (techs.begin(), techs.end() );
                  pair<int,int> r = chooseString ( "Unresearched Technologies", techs, buttons2 );
                  if ( r.first == 0 && r.second >= 0 ) {
                     devTech.push_back ( techIds[r.second] );
                  }
               }
            } else
            if ( res.first == 1 && res.second >= 0 ) {
               vector<int>::iterator p = find ( devTech.begin(), devTech.end(), techIds[techs[res.second]]);
               devTech.erase ( p );
            }
         } while ( res.first != 2 );
      }
   } while ( playerRes.first != 1 );
}

void editResearchPoints()
{
   vector<ASCString> buttonsP;
   buttonsP.push_back ( "~E~dit" );
   buttonsP.push_back ( "~C~lose" );

   pair<int,int> playerRes;
   do {
      vector<ASCString> player;
      for ( int i = 0; i < 8; ++i ) {
         ASCString s = strrr(i);
         player.push_back ( s + " " + actmap->player[i].getName() + " (" + strrr(actmap->player[i].research.progress) + " Points)" );
      }

      playerRes = chooseString ( "Choose Player", player, buttonsP );
      if ( playerRes.first == 0 && playerRes.second >= 0 )
         actmap->player[playerRes.second].research.progress = editInt ( "Points", actmap->player[playerRes.second].research.progress );

   } while ( playerRes.first != 1 );
}


void generateTechTree()
{
   ASCString filename = selectFile("*.dot", false);
   if ( !filename.empty() ) {

      map<ASCString,int> temptechs;
      for ( int i = 0; i < technologyRepository.getNum(); ++i ) {
         const Technology* t = technologyRepository.getObject_byPos(i);
         if ( !ItemFiltrationSystem::isFiltered( ItemFiltrationSystem::Technology, t->id )) {
            temptechs[t->name] = t->id;
         }
      }

      vector<ASCString> techs;
      vector<int> techIds;
      for ( map<ASCString,int>::iterator i = temptechs.begin(); i != temptechs.end(); ++i ) {
          techs.push_back ( printTech ( i->second ));
          techIds.push_back ( i->second );
      }



      /*
      for ( int i = 1; i < techIds.size();  )
         if ( technologyRepository.getObject_byID( techIds[i-1] )->name > technologyRepository.getObject_byID( techIds[i] )->name ) {
            int a = techIds[i];
            int b = techIds[i-1];
            techs[i] = printTech ( b );
            techs[i-1] = printTech ( a );
            techIds[i] = b;
            techIds[i-1] = a;
            if ( i > 1 )
               --i;
         } else
            ++i;
         */


      vector<ASCString> buttons2;
      buttons2.push_back ( "~O~k" );
      buttons2.push_back ( "~C~ancel" );

      bool leavesUp = choice_dlg ( "tree direction", "leaves up", "root up" ) == 1;

      if ( leavesUp )  {

         pair<int,int> r = chooseString ( "Choose Base Technology", techs, buttons2 );
         if ( r.first == 0 ) {


            bool reduce = choice_dlg ( "generate sparce tree ?", "~y~es", "~n~o" ) == 1;

            tn_file_buf_stream f ( filename, tnstream::writing );

            f.writeString ( "digraph \"ASC Technology Tree\" { \nnode [color=gray]\n", false );

            for ( int i = 0; i < technologyRepository.getNum(); ++i ) {
               const Technology* t  = technologyRepository.getObject_byPos(i);
               if ( !ItemFiltrationSystem::isFiltered( ItemFiltrationSystem::Technology, t->id )) {
                  vector<int> stack;
                  if ( t->techDependency.findInheritanceLevel( techIds[r.second], stack, techs[r.second] ) > 0 )
                     t->techDependency.writeTreeOutput( t->name, f, reduce );
               }

            }

            ASCString stn2 = technologyRepository.getObject_byID(techIds[r.second])->name;
            while ( stn2.find ( "\"" ) != ASCString::npos )
               stn2.erase ( stn2.find ( "\"" ),1 );

            f.writeString ( "\"" + stn2 + "\" [shape=doublecircle] \n", false );
            f.writeString ( "}\n", false );
         }
      } else {
         pair<int,int> r = chooseString ( "Choose Tip Technology", techs, buttons2 );
         if ( r.first == 0 ) {

            tn_file_buf_stream f ( filename, tnstream::writing );

            f.writeString ( "digraph \"ASC Technology Tree\" { \nnode [color=gray]\n", false );

            const Technology* t  = technologyRepository.getObject_byID(techIds[r.second]);
            vector<int> history;
            vector<pair<int,int> > blockedPrintList;
            if ( t )
               t->techDependency.writeInvertTreeOutput( t->name, f, history, blockedPrintList );

            ASCString stn2 = technologyRepository.getObject_byID(techIds[r.second])->name;
            while ( stn2.find ( "\"" ) != ASCString::npos )
               stn2.erase ( stn2.find ( "\"" ),1 );

            f.writeString ( "\"" + stn2 + "\" [shape=doublecircle] \n", false );
            f.writeString ( "}\n", false );
         }
      }

   }
}



void editTechAdapter()
{
   vector<ASCString> buttons;
   buttons.push_back ( "~A~dd" );
   buttons.push_back ( "~R~emove" );
   buttons.push_back ( "~C~lose" );

   vector<ASCString> buttons2;
   buttons2.push_back ( "~A~dd" );
   buttons2.push_back ( "~C~ancel" );

   vector<ASCString> buttonsP;
   buttonsP.push_back ( "~E~dit" );
   buttonsP.push_back ( "~C~lose" );

   pair<int,int> playerRes;
   do {
      vector<ASCString> player;
      for ( int i = 0; i < 8; ++i )
         player.push_back ( ASCString ( strrr(i)) + " " + actmap->player[i].getName());

      playerRes = chooseString ( "Choose Player", player, buttonsP );
      if ( playerRes.first == 0 && playerRes.second >= 0) {
         int player = playerRes.second;

         pair<int,int> res;
         do {

            vector<ASCString>& ta = actmap->player[player].research.predefinedTechAdapter;
            res = chooseString ( "Registered TechAdapter", ta, buttons );
            if ( res.first == 0 ) {
               ASCString s = editString( "enter TechAdapter" );
               if ( !s.empty() ) {
                  s.toLower();
                  ta.push_back ( s );
               }
            } else
               if ( res.first == 1 && res.second >= 0 )
                  ta.erase ( ta.begin() + res.second );

         } while ( res.first != 2 );
      }
   } while ( playerRes.first != 1 );
}

/*
void resetPlayerData()
{
   
   vector<ASCString> buttonsP;
   buttonsP.push_back ( "~V~iew" );
   buttonsP.push_back ( "r~E~search" );
   buttonsP.push_back ( "~P~roduction" );
   buttonsP.push_back ( "~U~nits" );
   buttonsP.push_back ( "~B~uildings" );
   buttonsP.push_back ( "~R~esource" );
   buttonsP.push_back ( "~T~ribute" );
   buttonsP.push_back ( "~c~lose" );

   pair<int,int> playerRes;
   playerRes.second = -1;
   do {
      vector<ASCString> player;
      for ( int i = 0; i < 8; ++i ) {
         ASCString s = strrr(i);
         player.push_back ( s + " " + actmap->player[i].getName() );
      }
      player.push_back ( "all" );
      player.push_back ( "all except " + actmap->player[0].getName() );

      playerRes = chooseString ( "Choose Player", player, buttonsP, playerRes.second );

      for ( int player = 0; player < 8; ++player )
         if ( playerRes.second == player || playerRes.second == 8 || (playerRes.second == 9 && player != 0 )) {
            if ( playerRes.first == 0 ) {
                 for ( int x = 0; x < actmap->xsize; x++ )
                    for ( int y = 0; y < actmap->ysize; y++ ) {
                       tfield* fld = actmap->getField(x,y);
                       fld->setVisibility( visible_not, player );
                       if ( fld->resourceview )
                          fld->resourceview->visible &= ~(1<<player);
                    }
            }
            if ( playerRes.first == 1 ) {
               actmap->player[player].research.progress = 0;
               actmap->player[player].research.activetechnology = NULL;
               actmap->player[player].research.developedTechnologies.clear();
            }

            if ( playerRes.first == 2 ) {
               for ( Player::BuildingList::iterator i = actmap->player[player].buildingList.begin(); i != actmap->player[player].buildingList.end(); ++i )
                  (*i)->unitProduction.clear();
            }

            if ( playerRes.first == 3 ) {
               while ( actmap->player[player].vehicleList.begin() != actmap->player[player].vehicleList.end() )
                  delete *actmap->player[player].vehicleList.begin();
            }

            if ( playerRes.first == 4 ) {
               while ( actmap->player[player].buildingList.begin() != actmap->player[player].buildingList.end() )
                  delete *actmap->player[player].buildingList.begin();
            }

            if ( playerRes.first == 5 ) {
               for ( Player::BuildingList::iterator i = actmap->player[player].buildingList.begin(); i != actmap->player[player].buildingList.end(); ++i ) {
                  for ( int j = 0; j < waffenanzahl; ++j )
                     (*i)->ammo[j] = 0;

                  (*i)->actstorage = Resources();
               }
               actmap->bi_resource[player] = Resources();
            }

            if ( playerRes.first == 5 || playerRes.first == 6 ) {
               for ( int j = 0; j< 8; ++j ) {
                  actmap->tribute.avail[player][j] = Resources();
                  actmap->tribute.avail[j][player]= Resources();
                  actmap->tribute.paid[player][j] = Resources();
                  actmap->tribute.paid[j][player]= Resources();
               }
            }
         }


   } while ( playerRes.first != 7 );
   
}
*/

tfield*        getactfield(void)
{
   return actmap->getField( actmap->getCursor() );; 
} 


class ItemLocator : public ASC_PG_Dialog {

      PG_LineEdit* idField;
      DropDownSelector* typeSelector;


      bool findUnit( ContainerBase* c, int id )
      {
         if ( !c )
            return false;

         for ( ContainerBase::Cargo::const_iterator i = c->getCargo().begin(); i != c->getCargo().end(); ++i ) {
            if ( (*i) ) {
               if ( (*i)->typ->id == id )
                  return true;
               if ( findUnit( *i, id ))
                  return true;
            }
         }
         return false;
      }

      bool ok()
      {
         int id = atoi ( idField->GetText().c_str() );
         if ( id != 0 ) {
            for ( int y = 0; y < actmap->ysize; ++y )
               for ( int x = 0; x < actmap->xsize; ++x ) {
                  tfield* fld = actmap->getField(x,y);
                  if ( fld ) {
                     bool found = false;
                     switch ( typeSelector->GetSelectedItemIndex () ) {
                        case 0: if ( fld->typ->terraintype->id == id )
                                   found = true;
                           break;
                        case 1: if ( fld->vehicle && fld->vehicle->typ->id == id )
                                   found = true;
                                 if ( findUnit( fld->vehicle, id ))
                                    found = true;
                           break;
                        case 2: if ( fld->building && fld->building->typ->id == id )
                                   found = true;
                                 if ( findUnit( fld->building, id ))
                                    found = true;
                           break;
                        case 3: if ( fld->checkforobject( objectTypeRepository.getObject_byID( id )) )
                                   found = true;
                           break;
                     }


                     if ( found ) {
                         getMainScreenWidget()->getMapDisplay()->cursor.goTo(MapCoordinate(x,y));
                        QuitModal();
                        return true;
                     }
                  }
               }

         } 
         return false;
      }

      bool cancel()
      {
         QuitModal();
         return true;
      }
          
   public:
      ItemLocator() : ASC_PG_Dialog(NULL, PG_Rect(-1,-1,400,200 ), "Item Locator")
      {
         AddStandardButton("OK")->sigClick.connect( SigC::slot(*this, &ItemLocator::ok ));
         AddStandardButton("cancel")->sigClick.connect( SigC::slot(*this, &ItemLocator::cancel ));

         typeSelector = new DropDownSelector(this, PG_Rect(20, 60, 150, 25 ));
         typeSelector->AddItem("Terrain");
         typeSelector->AddItem("Vehicle");
         typeSelector->AddItem("Building");
         typeSelector->AddItem("Object");

         new PG_Label ( this, PG_Rect(20, 100, 50, 25), "ID: " );
         idField = new PG_LineEdit( this, PG_Rect( 70, 100, 150, 25 ));
      }

};


void locateItemByID()
{
   ItemLocator il;
   il.Show();
   il.RunModal();
}


/********************************************************************/
/********************************************************************/
/****** mirrorMap and utitity methods                          ******/
/********************************************************************/
/********************************************************************/


   void copyVehicleData( Vehicle* source, Vehicle* target, GameMap* targetMap, int* playerTranslation )
   {
       
      target->height = source->height;
      target->tank = source->getTank();
      target->name = source->name;
      target->experience = source->experience;
      target->damage = source->damage;
      
      
      if( source->reactionfire.getStatus() == Vehicle::ReactionFire::off )
         target->reactionfire.disable();
      else
         target->reactionfire.enable();
      
      // for( int i=0; i<8; i++ ) target->aiparam[ i ] = source->aiparam[ i ];
      for( int i=0; i<16; i++ ) target->ammo[ i ] = source->ammo[ i ];
      
      // containing units
      ContainerBase::Cargo sourceCargo = source->getCargo();
      for( int i=0; i<sourceCargo.size(); i++ )
      {
         int playerID = playerTranslation[ sourceCargo[ i ]->getOwner() ];
         Vehicle* cargo = new Vehicle( sourceCargo[ i ]->typ, targetMap, playerID );
         copyVehicleData( sourceCargo[ i ], cargo, targetMap, playerTranslation );
         target->addToCargo( cargo );
      }
         
   }
   
   
   void copyBuildingData( Building* source, Building* target, GameMap* targetMap, int *playerTranslation, bool mirrorUnits )
   {
      for( int i=0; i<waffenanzahl; i++ ) 
         target->ammo[ i ] = source->ammo[ i ];
      target->name = source->name;
      target->netcontrol = source->netcontrol;
      target->visible = source->visible;
      /*
      for( int i=0; i<8; i++ ) 
         target->aiparam[ i ] = source->aiparam[ i ];
      */
      target->lastmineddist = source->lastmineddist;
      target->actstorage = source->actstorage;
      target->damage = source->damage;

      if( mirrorUnits )
      {
         // containing units
         ContainerBase::Cargo sourceCargo = source->getCargo();
         for( int i=0; i<sourceCargo.size(); i++ )
         {
            int playerID = playerTranslation[ sourceCargo[ i ]->getOwner() ];
            Vehicle* cargo = new Vehicle( sourceCargo[ i ]->typ, targetMap, playerID );
            copyVehicleData( sourceCargo[ i ], cargo, targetMap, playerTranslation );
            target->addToCargo( cargo );
         }
      }
      
   }
   
   void copyFieldStep1( tfield* sourceField, tfield* targetField, bool mirrorTerrain, bool mirrorResources, bool mirrorWeather )
   {
      targetField->deleteeverything();
      while( targetField->objects.size() > 0 )
         targetField->removeobject( targetField->objects[ 0 ].typ, true );
      
         if( mirrorTerrain )
         {
            targetField->typ = sourceField->typ; 
            targetField->bdt = sourceField->bdt;
         }
      
      if( mirrorResources )
      {
         targetField->fuel = sourceField->fuel; 
         targetField->material = sourceField->material;
      }
      if( mirrorWeather ) targetField->setweather( sourceField->getweather() ); 
   }
   
   void copyFieldStep2( tfield* sourceField, tfield* targetField, GameMap* targetMap, int *directionTranslation, int *playerTranslation, bool mirrorObjects, bool mirrorBuildings, bool mirrorUnits, bool mirrorMines )
   {
      if( mirrorObjects )
      {
         for( int i=0; i<sourceField->objects.size(); i++ )
            targetField->addobject( sourceField->objects[ i ].typ, -1, true );
      }
      
      if( mirrorBuildings )
      {
         if( sourceField->building != NULL && sourceField->building->getEntryField() == sourceField )
         {
            int playerID = playerTranslation[ sourceField->building->getOwner() ];
            Building *newBuilding = new Building( targetMap, MapCoordinate( targetField->getx(), targetField->gety() ), sourceField->building->typ, playerID );
            copyBuildingData( sourceField->building, newBuilding, targetMap, playerTranslation, mirrorUnits );
         }
      }
      
      if( mirrorUnits )
      {
         if( sourceField->vehicle != NULL )
         {
            int playerID = playerTranslation[ sourceField->vehicle->getOwner() ];
            targetField->vehicle = new Vehicle( sourceField->vehicle->typ, targetMap, playerID );
            copyVehicleData( sourceField->vehicle, targetField->vehicle, targetMap, playerTranslation );
            targetField->vehicle->setnewposition( targetField->getx(), targetField->gety() );
            targetField->vehicle->direction = directionTranslation[ int(sourceField->vehicle->direction) ];
         }
      }
      
      if( mirrorMines )
      {
         for( int i=0; i<sourceField->mines.size(); i++ )
         {
            Mine sourceMine = sourceField->getMine( i );
            targetField->putmine( playerTranslation[ sourceMine.player ], sourceMine.type, sourceMine.strength );
         }
      }
   }
 
   class MirrorMap : public tdialogbox
   {
      int mirrorUnits;
      int mirrorBuildings;
      int mirrorObjects;
      int mirrorMines;
      int mirrorResources;
      int mirrorWeather;
      
      int playerTranslation[ 9 ];
      
      bool doneMirrorMap;
      
      public:
         void init( void );
         void buttonpressed ( int id );
         void run ( void );
         char checkvalue( int id, void* p );
         
      protected:
      
         
         void mirrorX();
         void mirrorY();
   };

   char MirrorMap::checkvalue( int id, void* p )
   {
      return 1;
   }
   
   void MirrorMap::buttonpressed( int id )
   {
      tdialogbox::buttonpressed( id );
      
      if ( id == 1 ) 
         doneMirrorMap = true;
      
      if ( id == 2 )
      {
         mirrorX();
         doneMirrorMap = true;
      }
   
      if ( id == 3 )
      {
         mirrorY();
         doneMirrorMap = true;
      }
   
   }
   
   void MirrorMap::init( void )
   {
      doneMirrorMap = false;
      
      mirrorUnits = true;
      mirrorBuildings = true;
      mirrorObjects = true;
      mirrorMines = true;
      mirrorResources = true;
      mirrorWeather = true;
      for( int i=0; i<8; i++ )
         playerTranslation[ i ] = 7-i;
      playerTranslation[ 8 ] = 8;

      tdialogbox::init();
      title = "Mirror Map";
      
      xsize = 400;
      ysize = 450;
      
      x1 = -1;
      y1 = -1;
      
      addbutton( "~C~ancel", 10, ysize - 35, xsize / 3 - 5, ysize - 10, 0, 1, 1, true );
      addkey ( 1, ct_esc );
      
      addbutton( "Mirror ~X~", xsize / 3 + 5, ysize - 35, xsize / 3 * 2 - 5, ysize - 10, 0, 1, 2, true );
      addkey ( 2, ct_x );
      
      addbutton ( "Mirror ~Y~", xsize / 3 * 2 + 5, ysize - 35, xsize - 10, ysize - 10, 0, 1, 3, true );
      addkey ( 3, ct_y );
      
      addbutton( "~O~bjects", xsize - 100, 50, xsize - 10, 65, 3, 0, 4, true );
      addeingabe( 4, &mirrorObjects, 0, lightgray );
      addkey ( 4, ct_o );

      addbutton( "~U~nits", xsize - 100, 70, xsize - 10, 85, 3, 0, 5, true );
      addeingabe( 5, &mirrorUnits, 0, lightgray );
      addkey ( 5, ct_u );

      addbutton( "~B~uildings", xsize - 100, 90, xsize - 10, 105, 3, 0, 6, true );
      addeingabe( 6, &mirrorBuildings, 0, lightgray );
      addkey ( 6, ct_b );

      addbutton( "~M~ines", xsize - 100, 110, xsize - 10, 125, 3, 0, 7, true );
      addeingabe( 7, &mirrorMines, 0, lightgray );
      addkey ( 7, ct_m );

      addbutton( "~R~esources", xsize - 100, 130, xsize - 10, 145, 3, 0, 8, true );
      addeingabe( 8, &mirrorResources, 0, lightgray );
      addkey ( 8, ct_r );

      addbutton( "~W~eather", xsize - 100, 150, xsize - 10, 165, 3, 0, 9, true );
      addeingabe( 9, &mirrorWeather, 0, lightgray );
      addkey ( 9, ct_w );
      
      addbutton( "Player Conversions:", 30, 50, xsize - 120, 70, 0, 0, 10, true );
      addbutton( "Player 0:", 30, 80, 130, 95, 2, 0, 12, true );
      addeingabe( 12, &playerTranslation[ 0 ], 0, 8 );
      addbutton( "Player 1:", 30, 110, 130, 125, 2, 0, 13, true );
      addeingabe( 13, &playerTranslation[ 1 ], 0, 8 );
      addbutton( "Player 2:", 30, 140, 130, 155, 2, 0, 14, true );
      addeingabe( 14, &playerTranslation[ 2 ], 0, 8 );
      addbutton( "Player 3:", 30, 170, 130, 185, 2, 0, 15, true );
      addeingabe( 15, &playerTranslation[ 3 ], 0, 8 );
      addbutton( "Player 4:", 30, 200, 130, 215, 2, 0, 16, true );
      addeingabe( 16, &playerTranslation[ 4 ], 0, 8 );
      addbutton( "Player 5:", 150, 80, xsize - 120, 95, 2, 0, 17, true );
      addeingabe( 17, &playerTranslation[ 5 ], 0, 8 );
      addbutton( "Player 6:", 150, 110, xsize - 120, 125, 2, 0, 18, true );
      addeingabe( 18, &playerTranslation[ 6 ], 0, 8 );
      addbutton( "Player 7:", 150, 140, xsize - 120, 155, 2, 0, 19, true );
      addeingabe( 19, &playerTranslation[ 7 ], 0, 8 );
      addbutton( "Player 8:", 150, 170, xsize - 120, 185, 2, 0, 20, true );
      addeingabe( 20, &playerTranslation[ 8 ], 0, 8 );

      buildgraphics(); 
      
      activefontsettings.font = schriften.smallarial; 
      activefontsettings.justify = lefttext; 
      activefontsettings.length = 0;
      activefontsettings.background = 255;
      
      showtext2( "Warnings:",   x1 + 25, y1 + 220 );
      showtext2( "right&lower border might have to be manually fixed", x1 + 25, y1 + 260 );
      showtext2( "mountains, battle isle graphics coasts and similar", x1 + 25, y1 + 280 );
      showtext2( "terrain needs manual adaption", x1 + 25, y1 + 300 );
      showtext2( "building directions might be weird, resulting in:", x1 + 25, y1 + 330 );
      showtext2( "- pipeline net might be broken due to building directions", x1 + 25, y1 + 350 );
      showtext2( "- buildings might be missing due to terrain", x1 + 25, y1 + 370 );
      showtext2( "- units might be missing due to building locations", x1 + 25, y1 + 390 );
      
   }


   void MirrorMap::run ( void )
   {
       mousevisible ( true );
       do {
            tdialogbox::run();
       } while ( !doneMirrorMap ); /* enddo */
   }

  
   void MirrorMap::mirrorX()
   {
      if( actmap->xsize % 2 == 1 ) actmap->resize ( 0, 0, 0, 1 );
      int maxX = actmap->xsize/2;
      int xOffset = 2;
      int directionTranslation[ 6 ] = { 0, 5, 4, 3, 2, 1 };
   
      // run 1: object removal and terrain copy
      for( int x=0; x<maxX; x++ )
      {
         for( int y=0; y<actmap->ysize; y++ )
         {
            int targetX = actmap->xsize - x - xOffset;
            if( y%2 == 0 ) targetX++;
            if( targetX >= actmap->xsize || targetX == x ) continue;
            
            tfield *targetField = actmap->getField( targetX, y );
            tfield *sourceField = actmap->getField( x, y );
            
            copyFieldStep1( sourceField, targetField, true, mirrorResources, mirrorWeather );
         }
      }
      
      // run 2: object copy
      for( int x=0; x<maxX; x++ )
      {
         for( int y=0; y<actmap->ysize; y++ )
         {
            int targetX = actmap->xsize - x - xOffset;
            if( y%2 == 0 ) targetX++;
            if( targetX >= actmap->xsize || targetX == x ) continue;
            
            tfield *targetField = actmap->getField( targetX, y );
            tfield *sourceField = actmap->getField( x, y );
            
            copyFieldStep2( sourceField, targetField, actmap, directionTranslation, playerTranslation, mirrorObjects, mirrorBuildings, mirrorUnits, mirrorMines );
         }
      }
      doneMirrorMap = true;
   }

   void MirrorMap::mirrorY()
   {
      int yOffset = 2;
      int maxY = actmap->ysize/2;
      int directionTranslation[ 6 ] = { 3, 2, 1, 0, 5, 4 };
   
      // run 1: object removal and terrain copy
      for( int x=0; x<actmap->xsize; x++ )
      {
         for( int y=0; y<maxY; y++ )
         {
            int targetY = actmap->ysize - y - yOffset;
            
            tfield *targetField = actmap->getField( x, targetY );
            tfield *sourceField = actmap->getField( x, y );
            
            copyFieldStep1( sourceField, targetField, true, mirrorResources, mirrorWeather );
         }
      }

      // run 2: object copy
      for( int x=0; x<actmap->xsize; x++ )
      {
         for( int y=0; y<maxY; y++ )
         {
            int targetY = actmap->ysize - y - yOffset;
            
            tfield *targetField = actmap->getField( x, targetY );
            tfield *sourceField = actmap->getField( x, y );
            
            copyFieldStep2( sourceField, targetField, actmap, directionTranslation, playerTranslation, mirrorObjects, mirrorBuildings, mirrorUnits, mirrorMines );
         }
      }
      doneMirrorMap = true;
   }
   
   void mirrorMap()
   {
      MirrorMap mm;
      mm.init();
      mm.run();
      mm.done();
      mapChanged( actmap );
      displaymap();
   }

/********************************************************************/
/********************************************************************/
/****** mirrorMap and utitity methods END                      ******/
/********************************************************************/
/********************************************************************/

/********************************************************************/
/********************************************************************/
/****** copy area and utitity methods START                    ******/
/********************************************************************/
/********************************************************************/

class CopyMap : public FieldAddressing, public ASC_PG_Dialog
{
   private:
      GameMap *map;
      bool* fieldCopied;

      int mapStartX, mapStartY, mapEndX, mapEndY, sizeX, sizeY;
      int copyStep;
      
      int *directionTranslation; 
      int *playerTranslation; 

      PG_CheckButton* mirrorTerrain;
      PG_CheckButton* mirrorResources;
      PG_CheckButton* mirrorWeather;
      PG_CheckButton* mirrorObjects;
      PG_CheckButton* mirrorBuildings;
      PG_CheckButton* mirrorUnits;
      PG_CheckButton* mirrorMines;

      PG_CheckButton* mirrorX;
      PG_CheckButton* mirrorY;
      
      PG_CheckButton* autoIncreaseMapSize;
      
      PG_LineEdit* playerTranslation0;
      PG_LineEdit* playerTranslation1;
      PG_LineEdit* playerTranslation2;
      PG_LineEdit* playerTranslation3;
      PG_LineEdit* playerTranslation4;
      PG_LineEdit* playerTranslation5;
      PG_LineEdit* playerTranslation6;
      PG_LineEdit* playerTranslation7;
      PG_LineEdit* playerTranslation8;
      
      
   public:
      CopyMap();
      ~CopyMap();
      bool paste();
      void copy();
      void selectArea();
      
   protected:
      virtual void fieldOperator( const MapCoordinate& point );
};

CopyMap::CopyMap() : FieldAddressing( actmap ), ASC_PG_Dialog( NULL, PG_Rect( 30, 30, 550, 400 ), "Paste Options" )
{
   addressingMode = poly;
   mapStartX = -1;
   mapStartY = -1;
   mapEndX = -1;
   mapEndY = -1;
   map = NULL;
   fieldCopied = NULL;

   directionTranslation = new int[ 6 ];
   for( int i=0;i<6; i++ ) directionTranslation[ i ] = i;
   playerTranslation = new int[ 9 ];
   for( int i=0;i<9; i++ ) playerTranslation[ i ] = i;
   
   int dialogLine = 0;
   int lineHeight = 20;
   int lineSpacing = 10;
   
   int startX = 20;
   int startY = 20;
   int startX2 = 200;
   int startX3 = 250;
   int startX4 = 400;
   int xSpacer = 20;
   
   PG_Label* label = new PG_Label ( this, PG_Rect( startX, startY + dialogLine*(lineHeight+lineSpacing) , startX2 - xSpacer - startX, lineHeight ) );
   label->SetAlignment( PG_Label::LEFT );
   label->SetText( "Terrain" );
   mirrorTerrain = new PG_CheckButton( this, PG_Rect( startX2, startY + dialogLine*(lineHeight+lineSpacing) , startX3 - xSpacer - startX2, lineHeight ) );
   mirrorTerrain->SetPressed();
   dialogLine++;
   
   label = new PG_Label ( this, PG_Rect( startX, startY + dialogLine*(lineHeight+lineSpacing) , startX2 - xSpacer - startX, lineHeight ) );
   label->SetAlignment( PG_Label::LEFT );
   label->SetText( "Resources" );
   mirrorResources = new PG_CheckButton( this, PG_Rect( startX2, startY + dialogLine*(lineHeight+lineSpacing) , startX3 - xSpacer - startX2, lineHeight ) );
   mirrorResources->SetPressed();
   dialogLine++;
   
   label = new PG_Label ( this, PG_Rect( startX, startY + dialogLine*(lineHeight+lineSpacing) , startX2 - xSpacer - startX, lineHeight ) );
   label->SetAlignment( PG_Label::LEFT );
   label->SetText( "Weather" );
   mirrorWeather = new PG_CheckButton( this, PG_Rect( startX2, startY + dialogLine*(lineHeight+lineSpacing) , startX3 - xSpacer - startX2, lineHeight ) );
   mirrorWeather->SetPressed();
   dialogLine++;
   
   label = new PG_Label ( this, PG_Rect( startX, startY + dialogLine*(lineHeight+lineSpacing) , startX2 - xSpacer - startX, lineHeight ) );
   label->SetAlignment( PG_Label::LEFT );
   label->SetText( "Objects" );
   mirrorObjects = new PG_CheckButton( this, PG_Rect( startX2, startY + dialogLine*(lineHeight+lineSpacing) , startX3 - xSpacer - startX2, lineHeight ) );
   mirrorObjects->SetPressed();
   dialogLine++;
   
   label = new PG_Label ( this, PG_Rect( startX, startY + dialogLine*(lineHeight+lineSpacing) , startX2 - xSpacer - startX, lineHeight ) );
   label->SetAlignment( PG_Label::LEFT );
   label->SetText( "Buildings" );
   mirrorBuildings = new PG_CheckButton( this, PG_Rect( startX2, startY + dialogLine*(lineHeight+lineSpacing) , startX3 - xSpacer - startX2, lineHeight ) );
   mirrorBuildings->SetPressed();
   dialogLine++;
   
   label = new PG_Label ( this, PG_Rect( startX, startY + dialogLine*(lineHeight+lineSpacing) , startX2 - xSpacer - startX, lineHeight ) );
   label->SetAlignment( PG_Label::LEFT );
   label->SetText( "Units" );
   mirrorUnits = new PG_CheckButton( this, PG_Rect( startX2, startY + dialogLine*(lineHeight+lineSpacing) , startX3 - xSpacer - startX2, lineHeight ) );
   mirrorUnits->SetPressed();
   dialogLine++;
   
   label = new PG_Label ( this, PG_Rect( startX, startY + dialogLine*(lineHeight+lineSpacing) , startX2 - xSpacer - startX, lineHeight ) );
   label->SetAlignment( PG_Label::LEFT );
   label->SetText( "Mines" );
   mirrorMines = new PG_CheckButton( this, PG_Rect( startX2, startY + dialogLine*(lineHeight+lineSpacing) , startX3 - xSpacer - startX2, lineHeight ) );
   mirrorMines->SetPressed();
   dialogLine++;
   
   label = new PG_Label ( this, PG_Rect( startX, startY + dialogLine*(lineHeight+lineSpacing) , startX2 - xSpacer - startX, lineHeight ) );
   label->SetAlignment( PG_Label::LEFT );
   label->SetText( "X" );
   mirrorX = new PG_CheckButton( this, PG_Rect( startX2, startY + dialogLine*(lineHeight+lineSpacing) , startX3 - xSpacer - startX2, lineHeight ) );
   dialogLine++;
   
   label = new PG_Label ( this, PG_Rect( startX, startY + dialogLine*(lineHeight+lineSpacing) , startX2 - xSpacer - startX, lineHeight ) );
   label->SetAlignment( PG_Label::LEFT );
   label->SetText( "Y" );
   mirrorY = new PG_CheckButton( this, PG_Rect( startX2, startY + dialogLine*(lineHeight+lineSpacing) ,startX3 - xSpacer - startX2, lineHeight ) );
   dialogLine++;
   
   label = new PG_Label ( this, PG_Rect( startX, startY + dialogLine*(lineHeight+lineSpacing) , startX2 - xSpacer - startX, lineHeight ) );
   label->SetAlignment( PG_Label::LEFT );
   label->SetText( "autoIncreaseMapSize" );
   autoIncreaseMapSize = new PG_CheckButton( this, PG_Rect( startX2, startY + dialogLine*(lineHeight+lineSpacing) , startX3 - xSpacer - startX2, lineHeight ) );
   autoIncreaseMapSize->SetPressed();
   dialogLine++;
   
   
   PG_Button* ok = new PG_Button( this, PG_Rect( startX, startY + dialogLine*(lineHeight+lineSpacing) , startX2 - xSpacer - startX, lineHeight ), "Paste" );
   ok->sigClick.connect( SigC::slot( *this, &CopyMap::paste ));
   
   dialogLine = 0;
   
   label = new PG_Label ( this, PG_Rect( startX3, startY + dialogLine*(lineHeight+lineSpacing) , startX4 - xSpacer - startX, lineHeight ) );
   label->SetAlignment( PG_Label::LEFT );
   label->SetText( "playerTranslation0" );
   playerTranslation0 = new PG_LineEdit( this, PG_Rect( startX4, startY + dialogLine*(lineHeight+lineSpacing) , this->Width() - xSpacer - startX4, lineHeight ) );
   playerTranslation0->SetText( "0" );
   dialogLine++;

   label = new PG_Label ( this, PG_Rect( startX3, startY + dialogLine*(lineHeight+lineSpacing) , startX4 - xSpacer - startX, lineHeight ) );
   label->SetAlignment( PG_Label::LEFT );
   label->SetText( "playerTranslation1" );
   playerTranslation1 = new PG_LineEdit( this, PG_Rect( startX4, startY + dialogLine*(lineHeight+lineSpacing) , this->Width() - xSpacer - startX4, lineHeight ) );
   playerTranslation1->SetText( "1" );
   dialogLine++;

   label = new PG_Label ( this, PG_Rect( startX3, startY + dialogLine*(lineHeight+lineSpacing) , startX4 - xSpacer - startX, lineHeight ) );
   label->SetAlignment( PG_Label::LEFT );
   label->SetText( "playerTranslation2" );
   playerTranslation2 = new PG_LineEdit( this, PG_Rect( startX4, startY + dialogLine*(lineHeight+lineSpacing) , this->Width() - xSpacer - startX4, lineHeight ) );
   playerTranslation2->SetText( "2" );
   dialogLine++;

   label = new PG_Label ( this, PG_Rect( startX3, startY + dialogLine*(lineHeight+lineSpacing) , startX4 - xSpacer - startX, lineHeight ) );
   label->SetAlignment( PG_Label::LEFT );
   label->SetText( "playerTranslation3" );
   playerTranslation3 = new PG_LineEdit( this, PG_Rect( startX4, startY + dialogLine*(lineHeight+lineSpacing) , this->Width() - xSpacer - startX4, lineHeight ) );
   playerTranslation3->SetText( "3" );
   dialogLine++;

   label = new PG_Label ( this, PG_Rect( startX3, startY + dialogLine*(lineHeight+lineSpacing) , startX4 - xSpacer - startX, lineHeight ) );
   label->SetAlignment( PG_Label::LEFT );
   label->SetText( "playerTranslation4" );
   playerTranslation4 = new PG_LineEdit( this, PG_Rect( startX4, startY + dialogLine*(lineHeight+lineSpacing) , this->Width() - xSpacer - startX4, lineHeight ) );
   playerTranslation4->SetText( "4" );
   dialogLine++;

   label = new PG_Label ( this, PG_Rect( startX3, startY + dialogLine*(lineHeight+lineSpacing) , startX4 - xSpacer - startX, lineHeight ) );
   label->SetAlignment( PG_Label::LEFT );
   label->SetText( "playerTranslation5" );
   playerTranslation5 = new PG_LineEdit( this, PG_Rect( startX4, startY + dialogLine*(lineHeight+lineSpacing) , this->Width() - xSpacer - startX4, lineHeight ) );
   playerTranslation5->SetText( "5" );
   dialogLine++;

   label = new PG_Label ( this, PG_Rect( startX3, startY + dialogLine*(lineHeight+lineSpacing) , startX4 - xSpacer - startX, lineHeight ) );
   label->SetAlignment( PG_Label::LEFT );
   label->SetText( "playerTranslation6" );
   playerTranslation6 = new PG_LineEdit( this, PG_Rect( startX4, startY + dialogLine*(lineHeight+lineSpacing) , this->Width() - xSpacer - startX4, lineHeight ) );
   playerTranslation6->SetText( "6" );
   dialogLine++;

   label = new PG_Label ( this, PG_Rect( startX3, startY + dialogLine*(lineHeight+lineSpacing) , startX4 - xSpacer - startX, lineHeight ) );
   label->SetAlignment( PG_Label::LEFT );
   label->SetText( "playerTranslation7" );
   playerTranslation7 = new PG_LineEdit( this, PG_Rect( startX4, startY + dialogLine*(lineHeight+lineSpacing) , this->Width() - xSpacer - startX4, lineHeight ) );
   playerTranslation7->SetText( "7" );
   dialogLine++;

   label = new PG_Label ( this, PG_Rect( startX3, startY + dialogLine*(lineHeight+lineSpacing) , startX4 - xSpacer - startX, lineHeight ) );
   label->SetAlignment( PG_Label::LEFT );
   label->SetText( "playerTranslation8" );
   playerTranslation8 = new PG_LineEdit( this, PG_Rect( startX4, startY + dialogLine*(lineHeight+lineSpacing) , this->Width() - xSpacer - startX4, lineHeight ) );
   playerTranslation8->SetText( "8" );
   dialogLine++;

}

CopyMap::~CopyMap()
{
   if( map != NULL )
   {
      delete map;
      delete fieldCopied;
   }
   delete directionTranslation;
   delete playerTranslation;
}

void CopyMap::selectArea()
{
   polygons.clear();
   
   Poly_gon area;
   editpolygon( area );
   setPolygon( area );
}

void CopyMap::copy()
{
   if( map != NULL )
   {
      delete map;
      delete fieldCopied;
   }
   
   mapStartX = -1;
   mapStartY = -1;
   mapEndX = -1;
   mapEndY = -1;
   copyStep = 0;
   operate();
   
   sizeX = mapEndX-mapStartX + 1;
   sizeY = mapEndY-mapStartY + 1;
   
   map = new GameMap;
   map->allocateFields( sizeX, sizeY, terrainTypeRepository.getObject_byID(30)->weather[0] );

   int* oldDirectoyTranslation = directionTranslation;
   int* oldPlayerTranslation = playerTranslation;
   
   directionTranslation = new int[ 6 ];
   for( int i=0;i<6; i++ ) directionTranslation[ i ] = i;
   playerTranslation = new int[ 9 ];
   for( int i=0;i<9; i++ ) playerTranslation[ i ] = i;
   
   fieldCopied = new bool[ sizeY * sizeX ];
   for( int i=0; i<sizeY * sizeX; i++ )
      fieldCopied[ i ] = false;
   
   copyStep = 1;
   operate();
   copyStep = 2;
   operate();

   delete directionTranslation;
   delete playerTranslation;
   directionTranslation = oldDirectoyTranslation;
   playerTranslation = oldPlayerTranslation;
}

bool CopyMap::paste()
{
   QuitModal();
   Hide();
   if( map == NULL ) return false;
   
   tfield *field = getactfield();
   if( field == NULL ) return false;
   
   int pasteStartX = field->getx();
   int pasteStartY = field->gety();
   
   if( autoIncreaseMapSize->GetPressed() )
   {
      int targetSizeX = pasteStartX + map->xsize;
      int targetSizeY = pasteStartY + map->ysize;

      if( targetSizeY%2 == 1 )
         targetSizeY++;
      
      if( targetSizeX > actmap->xsize )
      {
         actmap->resize( 0, 0, 0, targetSizeX - actmap->xsize );
      }
      
      if( targetSizeY > actmap->ysize )
      {
         actmap->resize( 0, targetSizeY - actmap->ysize, 0, 0 );
      }
   }
   
   playerTranslation[ 0 ] = atoi( playerTranslation0->GetText() );
   playerTranslation[ 1 ] = atoi( playerTranslation1->GetText() );
   playerTranslation[ 2 ] = atoi( playerTranslation2->GetText() );
   playerTranslation[ 3 ] = atoi( playerTranslation3->GetText() );
   playerTranslation[ 4 ] = atoi( playerTranslation4->GetText() );
   playerTranslation[ 5 ] = atoi( playerTranslation5->GetText() );
   playerTranslation[ 6 ] = atoi( playerTranslation6->GetText() );
   playerTranslation[ 7 ] = atoi( playerTranslation7->GetText() );
   playerTranslation[ 8 ] = atoi( playerTranslation8->GetText() );
   
   int* oldDirectoyTranslation = directionTranslation;
   
   directionTranslation = new int[ 6 ];
   for( int i=0;i<6; i++ ) directionTranslation[ i ] = i;
   
   if( mirrorY->GetPressed() )
   {
      int directionTranslationNew[ 6 ] = 
      { 
         directionTranslation[ 3 ], 
         directionTranslation[ 2 ],
         directionTranslation[ 1 ], 
         directionTranslation[ 0 ], 
         directionTranslation[ 5 ], 
         directionTranslation[ 4 ] 
      };
      
      for( int i=0;i<6; i++ ) directionTranslation[ i ] = directionTranslationNew[ i ];
   }
   
   if( mirrorX->GetPressed() )
   {
      int directionTranslationNew[ 6 ] = 
      { 
         directionTranslation[ 0 ], 
         directionTranslation[ 5 ],
         directionTranslation[ 4 ], 
         directionTranslation[ 3 ], 
         directionTranslation[ 2 ], 
         directionTranslation[ 1 ] 
      };
      
      for( int i=0;i<6; i++ ) directionTranslation[ i ] = directionTranslationNew[ i ];
   }
   
   
   
   
   for( int x=0; x<map->xsize; x++ )
   {
      for( int y=0; y<map->ysize; y++ )
      {
         if( fieldCopied[ x + y * sizeX ] )
         {
            int fieldX = pasteStartX + x;
            int fieldY = pasteStartY + y;
            
            if( pasteStartY%2 == 1 && fieldY%2 == 0 )
                  fieldX++;
            
            if( mirrorX->GetPressed() )
            {
               fieldX = pasteStartX + sizeX - x - 1;
               
               if( pasteStartY%2 == 0 && fieldY%2 == 1 )
                  fieldX--;
            }
            
            if( mirrorY->GetPressed() )
            {
               fieldY = pasteStartY + sizeY - y - 1;
            }
            
            if( fieldX >= actmap->xsize ) continue;
            if( fieldY >= actmap->ysize ) continue;
            
            tfield* target = actmap->getField( fieldX, fieldY );
            tfield* source = map->getField( x, y );
            
            copyFieldStep1( source, target, mirrorTerrain->GetPressed(), mirrorResources->GetPressed(), mirrorWeather->GetPressed() );
         }
      }
   }
   
   for( int x=0; x<map->xsize; x++ )
   {
      for( int y=0; y<map->ysize; y++ )
      {
         if( fieldCopied[ x + y * sizeX ] )
         {
            int fieldX = pasteStartX + x;
            int fieldY = pasteStartY + y;
            
            if( pasteStartY%2 == 1 && fieldY%2 == 0 )
               fieldX++;
            
            if( mirrorX->GetPressed() )
            {
               fieldX = pasteStartX + sizeX - x - 1;
               
               if( pasteStartY%2 == 0 && fieldY%2 == 1 )
                  fieldX--;
            }
            
            if( mirrorY->GetPressed() )
            {
               fieldY = pasteStartY + sizeY - y - 1;
            }
            
            if( fieldX >= actmap->xsize ) continue;
            if( fieldY >= actmap->ysize ) continue;
            
            tfield* target = actmap->getField( fieldX, fieldY );
            tfield* source = map->getField( x, y );
            
            copyFieldStep2( source, target, actmap, directionTranslation, playerTranslation, mirrorObjects->GetPressed(), mirrorBuildings->GetPressed(), mirrorUnits->GetPressed(), mirrorMines->GetPressed() );
         }
      }
   }
   
   delete directionTranslation;
   directionTranslation = oldDirectoyTranslation;
   
   return true;

}

void CopyMap::fieldOperator( const MapCoordinate& point )
{
   int mapX = point.x - mapStartX;
   int mapY = point.y - mapStartY;
   
	 if( mapStartY%2 == 1 && mapY%2 == 1 )
		 mapX--;
	 
   if( copyStep == 0 )
   {
      if( mapStartX == -1 || point.x < mapStartX )
         mapStartX = point.x;
         
      if( mapStartY == -1 || point.y < mapStartY )
         mapStartY = point.y;

      if( mapEndX == -1 || point.x > mapEndX )
         mapEndX = point.x;
         
      if( mapEndY == -1 || point.y > mapEndY )
         mapEndY = point.y;
   }else if( copyStep == 1 )
   {
      fieldCopied[ mapX + mapY * sizeX ] = true;

      tfield* source = actmap->getField( point );
      tfield* target = map->getField( mapX, mapY );
      
      
      
      copyFieldStep1( source, target, true, true, true );

   }else if( copyStep == 2 )
   {
      tfield* source = actmap->getField( point );
      tfield* target = map->getField( mapX, mapY );
      
      copyFieldStep2( source, target, map, directionTranslation, playerTranslation, true, true, true, true );

   }
}

CopyMap *copyMap = NULL;

void copyArea()
{
   if( copyMap == NULL )
   {
      copyMap = new CopyMap();
   }
   copyMap -> selectArea();
   copyMap -> copy();
}

void pasteArea()
{
   if( copyMap != NULL )
   {
      copyMap -> Show();
      copyMap -> RunModal();
      mapChanged( actmap );
      displaymap();
   }
}

/********************************************************************/
/********************************************************************/
/****** copy area and utitity methods END                      ******/
/********************************************************************/
/********************************************************************/




void testDebugFunction()
{
  // tempsvisible = true;
   actmap->cleartemps(7);
   for ( int x = 0; x < actmap->xsize; ++x )
      for ( int y = 0; y < actmap->ysize; ++y ) {
         tfield* fld = actmap->getField(x,y);
         if ( fld->getVisibility(5) >= visible_now )
            fld->a.temp = 1;
         else
            fld->a.temp = 0;
      }

   repaintMap();
}
