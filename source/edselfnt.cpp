/*! \file edselfnt.cpp
    \brief Selecting units, buildings, objects, weather etc. in the mapeditor
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


#include "vehicletype.h"
#include "buildingtype.h"

#include "edmisc.h"
#include "edselfnt.h"
#include "mapdisplay.h"
#include "itemrepository.h"
#include "edglobal.h"

#include "graphics/blitter.h"
#include "paradialog.h"


SigC::Signal0<void> filtersChangedSignal;




bool vehicleComp( const Vehicletype* v1, const Vehicletype* v2 )
{
   int id1 = getUnitSetID(v1);
   int id2 = getUnitSetID(v2);
   return (id1 <  id2) ||
          (id1 == id2 && v1->movemalustyp  < v2->movemalustyp ) ||
          (id1 == id2 && v1->movemalustyp == v2->movemalustyp && v1->name < v2->name);
}

bool buildingComp( const BuildingType* v1, const BuildingType* v2 )
{
   int id1 = getUnitSetID(v1);
   int id2 = getUnitSetID(v2);
   return (id1 <  id2) ||
          (id1 == id2 && v1->name < v2->name);
}

bool objectComp( const ObjectType* v1, const ObjectType* v2 )
{
   return v1->name < v2->name;
}

bool terrainComp( const TerrainType* v1, const TerrainType* v2 )
{
   if (  v1->weather[0] && v2->weather[0] )
      return v1->weather[0]->art.to_ulong()  < v2->weather[0]->art.to_ulong();
   else
      return v1->name < v2->name;
}

bool mineComp( const MineType* v1, const MineType* v2 )
{
   return v1->id < v2->id;
}


void sortItems( vector<Vehicletype*>& vec )
{
   sort( vec.begin(), vec.end(), vehicleComp );
}

void sortItems( vector<BuildingType*>& vec )
{
   sort( vec.begin(), vec.end(), buildingComp );
}

void sortItems( vector<ObjectType*>& vec )
{
   sort( vec.begin(), vec.end(), objectComp );
}

void sortItems( vector<TerrainType*>& vec )
{
   sort( vec.begin(), vec.end(), terrainComp );
}

void sortItems( vector<MineType*>& vec )
{
   sort( vec.begin(), vec.end(), mineComp );
}


void MapComponent::displayClip( PG_Widget* parent, SDL_Surface * surface, const PG_Rect & src, const PG_Rect & dst ) const
{
   if ( !getClippingSurface().valid() )
      getClippingSurface() = Surface::createSurface( displayWidth() + 10, displayHeight() + fontHeight + 10, 32, 0 );
      
   getClippingSurface().Fill(0);   
   display( getClippingSurface(), SPoint( 0, 0 ) );
   
   static PG_ThemeWidget* fontProvidingWidget = NULL;
   if ( !fontProvidingWidget  )
      fontProvidingWidget = new PG_ThemeWidget( NULL );
      
   SDL_Rect      blitRect;
   blitRect.x = 0;
   blitRect.y = displayHeight() + 2;
   blitRect.w = displayWidth();
   blitRect.h = displayHeight() - blitRect.y;
   PG_FontEngine::RenderText( getClippingSurface().getBaseSurface(), blitRect, blitRect.x, blitRect.y+fontProvidingWidget ->GetFontAscender(), getItemType()->getName(), fontProvidingWidget ->GetFont() );
   
   PG_Draw::BlitSurface( getClippingSurface().getBaseSurface(), src, surface, dst);
}




// template<class Vehicletype> Surface BasicItem<Vehicletype>::clippingSurface;
int VehicleItem::place( const MapCoordinate& mc ) const
{
   pfield fld = actmap->getField(mc);
   if ( !fld )
      return -1;

   const Vehicletype* veh = item;
   
   if ( !veh )
      return -2;
     
   if ( selection.getPlayer()  == 8 )
      return -3;

   bool accessible = veh->terrainaccess.accessible ( fld->bdt );
   if ( veh->height >= chtieffliegend )
         accessible = true;

   if ( fld->building  || ( !accessible && !actmap->getgameparameter( cgp_movefrominvalidfields)) ) 
      return -3;
   
   
   if ( fld->vehicle ) {
      if ( fld->vehicle->typ != veh ) {
         delete fld->vehicle;
         fld->vehicle = NULL;
      } else {
         fld->vehicle->convert( selection.getPlayer()  );
         return 1;
      }
   }
   fld->vehicle = new Vehicle ( veh, actmap, selection.getPlayer()  );
   fld->vehicle->setnewposition ( mc );
   fld->vehicle->fillMagically();
   
   for ( int i = 0; i < 9; ++i ) {
      if ( fld->vehicle->typ->height & (1 << i )) {
         fld->vehicle->height = 1 << i;
         if ( terrainaccessible( fld, fld->vehicle ) == 2  || actmap->getgameparameter( cgp_movefrominvalidfields) )
            break;
      }
      if ( i == 8 ) { // no height found
         delete fld->vehicle;
         fld->vehicle = NULL;
         return -3;
      }   
   }
   fld->vehicle->resetMovement();
   return 1;
   
}


Surface BuildingItem::clippingSurface;
Surface BuildingItem::fullSizeImage;
int BuildingItem::place( const MapCoordinate& mc ) const
{
   int f = 0;
   for ( int x = 0; x < 4; x++ )
      for ( int y = 0; y < 6; y++ )
         if ( bld->fieldExists ( BuildingType::LocalCoordinate(x, y) )) {
            MapCoordinate mc = bld->getFieldCoordinate ( actmap->getCursor() , BuildingType::LocalCoordinate (x, y) );
            if ( !actmap->getField (mc) )
               return -1;

            if ( bld->terrainaccess.accessible ( actmap->getField (mc)->bdt ) <= 0 )
               f++;
         }
   if ( f ) {
      // if (choice_dlg("Invalid terrain for building !","~i~gnore","~c~ancel") == 2)
         return -1 ;
   }      

   putbuilding( actmap->getCursor(), selection.getPlayer()  * 8, bld, bld->construction_steps );
   return 0;
}

void BuildingItem::display( Surface& s, const SPoint& pos ) const 
{ 
   if ( !fullSizeImage.valid() )
      fullSizeImage = Surface::createSurface( displayWidth()*2 + 10, displayHeight()*2 + 30, 32, 0 );
   fullSizeImage.FillTransparent();
   bld->paint ( fullSizeImage, SPoint(0,0), selection.getPlayer() ); 
   
   MegaBlitter<colorDepth,colorDepth,ColorTransform_None,ColorMerger_AlphaOverwrite,SourcePixelSelector_DirectZoom,TargetPixelSelector_Valid> blitter;
   blitter.setZoom( 0.5 );
   blitter.blit( fullSizeImage, s, pos );
}



// template<> Surface BasicItem<ObjectType>::clippingSurface;
int ObjectItem::place( const MapCoordinate& mc ) const
{
   actmap->getField(mc)->addobject( item );
   return 0;
}


template<typename T> Surface BasicItem<T>::clippingSurface;
int TerrainItem::place( const MapCoordinate& mc ) const
{
   pfield fld = actmap->getField(mc);
   fld->typ = item->weather[0]; 
   fld->setweather( selection.getWeather() );
   fld->setparams();
   return 0;
}


int MineItem::place( const MapCoordinate& mc ) const
{
   actmap->getField(mc)->putmine( selection.getPlayer(), item->id, MineBasePunch[item->id-1] );
   return 0;
}













int actplayer = 0;
int currentWeather = 0;
int brushSize = 1;
















class CargoItemFactory: public MapItemTypeWidgetFactory<MapItemTypeWidget< Vehicletype > > {
      typedef MapItemTypeWidgetFactory<MapItemTypeWidget< Vehicletype > > Parent;
      typedef MapItemTypeWidget< Vehicletype > WidgetType;
       ContainerBase* container;
   protected:
      bool isFiltered( const ItemType& item ) {
         if ( Parent::isFiltered( item ))
            return false;
      
         bool result = false;
         Vehicle* unit = new Vehicle ( &item, actmap, container->getOwner() );
         if ( container->vehicleFit ( unit ))
            result = true;
         delete unit;
         return result;
      };

   public:
      CargoItemFactory(  ContainerBase* container_ )  : Parent(vehicleTypeRepository), container( container_ ) {};
      
      void itemSelected( const SelectionWidget* widget, bool mouse )
      {
         if ( !widget )
            return;
            
         const WidgetType* mapItemWidget = dynamic_cast<const WidgetType*>(widget);
         assert( mapItemWidget );
         if ( mapItemWidget->getItem() ) {
            Vehicle* unit = new Vehicle ( mapItemWidget->getItem(), actmap, container->getOwner() );
            unit->fillMagically();
            unit->setnewposition ( container->getPosition() );
            unit->tank.material = 0;
            unit->tank.fuel = 0;
            if ( container->vehicleFit ( unit )) {
               unit->tank.material = unit->typ->tank.material;
               unit->tank.fuel = unit->typ->tank.fuel;
      
               if ( !container->vehicleFit ( unit )) {
                  unit->tank.material = 0;
                  unit->tank.fuel = 0;
                  displaymessage("Warning:\nThe unit you just set could not be loaded with full material and fuel\nPlease set these values manually",1);
               }
               container->addToCargo( unit );
      
            } else {
               delete unit;
               displaymessage("The unit could not be loaded !",1);
            }
         }
      }
     
};


void selcargo( PG_Window* parentWindow, ContainerBase* container )
{
   ItemSelectorWindow isw( parentWindow, PG_Rect( 100, 100, 280, 600), new CargoItemFactory( container ) );
   isw.Show();
   isw.RunModal();
}




/////////////////////////////////////// CARGO //////////////////////////////////////








class SelectAnythingBase {
            public:
                virtual void showactiteminfos ( int x1, int y1, int x2, int y2 ) = 0;
                virtual int getiteminfoheight( void ) { return 60; };
                virtual ~SelectAnythingBase ( ) {};
        };


#define vect vector


template<class T> 
class SelectAnything : public SelectAnythingBase {
                    protected:
                       tmouserect position;
                       int maxx;  int maxy;
                       int winstartx;  int winstarty;
                       int winsizey;
                       int actitemx;   int actitemy;

                       virtual bool isavailable ( T item ) = 0;
                       virtual void displaysingleitem ( T item, int x, int y ) = 0;
                       virtual void _displaysingleitem ( T item, int x, int y );
                       virtual void displayItem ( int itemx, int itemy, int picx, int picy );
                       virtual void displayItem ( int itemx, int itemy );
                       virtual int getxposforitempos ( int itemx );
                       virtual int getyposforitempos ( int itemy );
                       virtual int getitemsizex ( void ) = 0;
                       virtual int getitemsizey ( void ) = 0;
                       virtual ASCString getItemName ( T item ) = 0;
                       virtual int getxgap ( void ) { return 5; };
                       virtual int getygap ( void ) { return 5; };
                       virtual void _showiteminfos ( T item, int x1, int y1, int x2, int y2 );
                       virtual void showiteminfos ( T item, int x1, int y1, int x2, int y2 ) = 0;
                       void showiteminfos ( T item );
                       virtual void showactiteminfos ( void );
                       vect<T> itemsavail;

                    public:
                       virtual void showactiteminfos ( int x1, int y1, int x2, int y2 );
                       T selectitem ( T previtem, tkey neutralkey = 0 );
                       void init ( vect<T> &v, int x1, int y1, int x2, int y2 ); 
                       void init ( vect<T> &v ); 
                       void display ( void );
                       void setnewselection ( T item );
                  };


template<class T> void SelectAnything<T> :: init ( vect<T> &v ) 
{
   init ( v, selfontxanf, selfontyanf, selfontxanf + selfontxsize, agmp->resolutiony - 20 );
}


template<class T> void SelectAnything<T> :: init ( vect<T> &v, int x1, int y1, int x2, int y2 ) 
{
   itemsavail.clear();

   for ( int i = 0; i < v.size(); i++ )
      if ( isavailable ( v[i] ) )
         itemsavail.push_back (  v[i] );
     
  int num = itemsavail.size() + 1; 
  if ( num <= 0 ) {
     displaymessage("no items available", 1 );
     // return;
  } 


  position.x1 = x1;
  if ( x1 + getitemsizex() > x2 ) {
     if ( x1 + getitemsizex() >= agmp->resolutionx ) 
        position.x1 = agmp->resolutionx - getitemsizex();
     position.x2 = position.x1 + getitemsizex();
  } else
     position.x2 = x2;
  position.y1 = y1 + getiteminfoheight();
  position.y2 = y2;

  maxx = (position.x2 - position.x1) / ( getitemsizex() + getxgap() );
  if ( maxx < 1 )
     maxx = 1;

  maxy = (num + maxx -1) / maxx;

  actitemx = 0;
  actitemy = 0;
  winstartx = 0;
  winstarty = 0;
  winsizey = ( position.y2 - position.y1 ) / ( getitemsizey() + getygap() );
  if ( winsizey > maxy )
     winsizey = maxy;
}


template<class T> void SelectAnything<T> :: _showiteminfos ( T item, int x1, int y1, int x2, int y2 )
{
   collategraphicoperations cgo ( x1, y1, x2, y2 );
   showiteminfos ( item, x1, y1, x2, y2 );
}


template<class T> void SelectAnything<T> :: showactiteminfos ( int x1, int y1, int x2, int y2 )
{
   int pos = actitemx + actitemy * maxx;
   if ( pos < itemsavail.size() )
      _showiteminfos ( itemsavail[ pos ], x1, y1, x2, y2 );
   else
      _showiteminfos ( NULL, x1, y1, x2, y2 );

}

template<class T> void SelectAnything<T> :: showactiteminfos ( void )
{
   int pos = actitemx + actitemy * maxx;
   if ( pos < itemsavail.size() )
      showiteminfos ( itemsavail[ pos ] );
   else
      showiteminfos ( NULL );
}


template<class T> void SelectAnything<T> :: showiteminfos ( T item )
{
   _showiteminfos ( item, position.x1, position.y1 - getiteminfoheight(), position.x2, position.y1 - 5 );
}







template<class T> void SelectAnything<T> :: _displaysingleitem ( T item, int x, int y )
{
   collategraphicoperations cgo ( x, y, x + getitemsizex(), y + getitemsizey() );
   displaysingleitem ( item, x, y );
}


template<class T> void SelectAnything<T> :: displayItem ( int itemx, int itemy, int picx, int picy )
{
   int pos = itemx + itemy * maxx;
   if ( pos < itemsavail.size() )
      _displaysingleitem ( itemsavail[ pos ], picx, picy );
   else
      _displaysingleitem ( NULL, picx, picy );

   if ( itemx == actitemx  &&  itemy == actitemy ) 
      rectangle ( picx, picy, picx + getitemsizex(), picy + getitemsizey(), white );
   
}

template<class T> int SelectAnything<T> :: getxposforitempos ( int itemx )
{
   int p = position.x1 + itemx * getitemsizex();
   if ( itemx > 0 )
      p +=( itemx ) * getxgap();
   return p;
}

template<class T> int SelectAnything<T> :: getyposforitempos ( int itemy )
{
   int p = position.y1 + (itemy - winstarty) * getitemsizey();
   if ( itemy-winstarty > 0 )
      p +=  ( itemy-winstarty ) * getygap();
   return p;
}


template<class T> void SelectAnything<T> :: displayItem ( int itemx, int itemy )
{
   displayItem ( itemx, itemy, getxposforitempos ( itemx ), getyposforitempos ( itemy ) );
}

template<class T> void SelectAnything<T> ::  display ( void ) 
{
   for ( int y = winstarty; y < winstarty + winsizey; y++ ) 
      for ( int x = 0; x < maxx; x++ )
         displayItem ( x, y );
      
}


template<class T> void SelectAnything<T> :: setnewselection ( T item )
{
   int num = itemsavail.size(); 
   if ( num <= 0 )
      return;

   if ( !item )
      return;

   int i = 0;
   int found = 0;
   while ( i < num && !found ) {
      if ( itemsavail[i] == item )
         found = 1;
      else
         i++;
   }
   if ( found ) {
      actitemx = i % maxx;
      actitemy = i / maxx;
   }

}



template<class T> T SelectAnything<T> :: selectitem( T previtem, tkey neutralkey )
{
   int num = itemsavail.size() ; 
   if ( num <= 0 )
      return NULL;

   bar ( position.x1, position.y1-getiteminfoheight(), position.x2, position.y2, black );

   if ( previtem ) 
      for ( int i = 0; i < itemsavail.size(); i++ )
         if ( itemsavail[ i ] == previtem ) {
            actitemx = i % maxx;
            actitemy = i / maxx;
         }
   
   display();
   showactiteminfos();
   int finished = 0;
   int mousepressed = 0;
   int lastscroll = ticker;
   int mousescrollspeed = 8;
   do {
      int oldx = actitemx;
      int oldy = actitemy;
      int oldwiny = winstarty;
      if ( keypress() ) {
         int prntkey;
         tkey ch;
         getkeysyms ( &ch, &prntkey );
         switch ( ch ) {
         
               case ct_up:
               case ct_8k:   actitemy--;
                             break;
                             
               case ct_left:
               case ct_4k:   actitemx--;
                             break;
                             
               case ct_right:
               case ct_6k:   actitemx++;
                             break;
                             
               case ct_down:
               case ct_2k:   actitemy++;
                  break;
                  
               case ct_pos1: actitemy=0;
                  break;
               case ct_ende: actitemy = maxy-1;
                  break;
                  
               case ct_f2:
               case ct_space:
               case ct_enter: finished = 1;
                  break;
               case ct_esc: finished = 2;
                  break;
                  
               case ct_f3:
               case ct_f4:
               case ct_f5:
               case ct_f6:
               case ct_f7:
               case ct_f8:
               case ct_f9: if ( ch != neutralkey && neutralkey ) {
                              finished = 2;
                              ::ch = ch;
                           }
                  break;
         } 
         if ((prntkey > ' ') && (prntkey < 256)) {   /* spedsearc */
            int key = tolower ( prntkey );

            int ax = actitemx;
            int ay = actitemy;
            bool finished  = false;
            do {
               ax++;
               while ( ax >= maxx ) {
                  ax -= maxx;
                  ay ++;
               }
               if ( ay >= maxy )
                  ay = 0;

               if ( ax == actitemx && ay == actitemy ) {
                  finished = true;
               } else {
                  int pos = ax + ay * maxx;
                  if ( pos < itemsavail.size() )
                     if ( itemsavail[pos] ) {
                        ASCString s = getItemName( itemsavail[pos] );
                        if ( !s.empty() )
                           if ( tolower (s[0]) == key ) {
                              actitemx = ax;
                              actitemy = ay;
                              finished = true;
                           }
                     }
               }
            } while ( !finished );
         }
      }

      if ( mouseparams.taste == 1 ) {
         for ( int x = 0; x < maxx; x++ )
            for ( int y = winstarty; y < winstarty + winsizey; y++ )
               if ( mouseinrect ( getxposforitempos ( x ), getyposforitempos ( y ),
                                  getxposforitempos ( x ) + getitemsizex(), getyposforitempos ( y ) + getitemsizey()) )
                  {
                     actitemx = x;
                     actitemy = y;
                     mousepressed = 1;
                  }
         if ( mouseparams.x >= position.x1 && mouseparams.x <= position.x2 ) {
            if ( mouseparams.y > position.y2 ) 
               if ( winstarty + winsizey < maxy -1 )
                  if ( ticker > lastscroll + mousescrollspeed ) {
                     winstarty ++;
                     lastscroll = ticker;
                     actitemy ++;
                  }
            if ( mouseparams.y < position.y1 && mouseparams.y > position.y1 - getiteminfoheight() ) 
               if ( winstarty > 0 )
                  if ( ticker > lastscroll + mousescrollspeed ) {
                     winstarty --;
                     lastscroll = ticker;
                     actitemy --;
                  }

         }
      } else
         if ( mousepressed == 1 )
            finished = 1;
/*
      if ( selfntmousecurs != 8 ) {
         if ( selfntmousecurs == 0 )
            if ( winstarty > 0 )
               if ( ticker > lastscroll + mousescrollspeed ) {
                  winstarty --;
                  lastscroll = ticker;
                  actitemy --;
               }

         if ( selfntmousecurs == 4 )
            if ( winstarty + winsizey < maxy -1 )
               if ( ticker > lastscroll + mousescrollspeed ) {
                  winstarty ++;
                  lastscroll = ticker;
                  actitemy ++;
               }
      }
*/
      while ( actitemx >= maxx ) {
         actitemx -= maxx;
         actitemy ++;
      }
      while ( actitemx < 0 ) {
         actitemx += maxx;
         actitemy --;
      }
      if ( actitemy < 0 )
         actitemy = 0;
      if ( actitemy >= maxy )
         actitemy = maxy -1;

      if ( winstarty > actitemy )
         winstarty = actitemy;

      if ( winstarty + winsizey <= actitemy )
         winstarty = actitemy - winsizey +1;

      if ( oldwiny != winstarty ) 
         display();
      else 
         if ( oldx != actitemx  || oldy != actitemy ) {
            displayItem ( oldx, oldy );
            displayItem ( actitemx, actitemy );
         }
      
      if ( oldx != actitemx  || oldy != actitemy ) 
         showactiteminfos();

      releasetimeslice();
   } while ( !finished );

   if ( finished == 1 ) {
      int pos = actitemx + actitemy * maxx;
      if ( pos < itemsavail.size() )
         return itemsavail[ pos ];
      else
         return previtem;
   } else
      return previtem;
}


class SelectVehicleType : public SelectAnything< Vehicletype* > {
                    protected:
                       virtual bool isavailable ( Vehicletype* item );
                       virtual void displaysingleitem ( Vehicletype* item, int x, int y );
                       virtual int getitemsizex ( void ) { return fieldsizex; } ;
                       virtual int getitemsizey ( void ) { return fieldsizey; } ;
                       virtual void showiteminfos ( Vehicletype* item, int x1, int y1, int x2, int y2 );
                       virtual ASCString getItemName ( Vehicletype* item );
};


bool SelectVehicleType :: isavailable ( Vehicletype* item )
{
   /*
   if ( farbwahl == 8 ) {
      displaymessage("no neutral units allowed on map !\nswitching to red player!", 1 );
      farbwahl = 0;
   }
   */

   return !ItemFiltrationSystem::isFiltered( ItemFiltrationSystem::Vehicle, item->id );
}


void SelectVehicleType :: displaysingleitem ( Vehicletype* item, int x, int y )
{
   bar ( x, y, x + getitemsizex(), y + getitemsizey(), black );
   if ( item )
      // putrotspriteimage ( x, y, item->picture[0], farbwahl*8 );
      item->paint( getActiveSurface(), SPoint(x,y), farbwahl, 0 );
}

ASCString SelectVehicleType :: getItemName ( Vehicletype* item )
{
   if ( !item->name.empty()  )
      return item->name;
   else
      if ( !item->description.empty() )
         return item->description;
      else
         return "-NONE-";
}


void SelectVehicleType :: showiteminfos ( Vehicletype* item, int x1, int y1, int x2, int y2 )
{ 
   rectangle ( x1, y1, x2, y2, lightgray );
   bar ( x1+1, y1+1, x2-1, y2-1, black );
   if ( item ) {
      displaysingleitem ( item, x1 + 10, (y1 + y2 - getitemsizey() )/2 );
      npush ( activefontsettings );
      activefontsettings.font = schriften.smallarial;
      activefontsettings.background  = 255;
      activefontsettings.justify = lefttext;
      activefontsettings.color = lightgray;
      activefontsettings.height = 0;
      activefontsettings.length = x2 - x1 - 20 - getitemsizex();

      showtext4 ( "Name: %s", x1 + 20 + getitemsizex(), y1 + 10, getItemName (item).c_str() );

      showtext4 ( "ID: %d", x1 + 20 + getitemsizex(), y1 + 30, item->id );
      npop ( activefontsettings );
   }
}





class SelectTerrainType : public SelectAnything< pterraintype > {
                    protected:
                       virtual bool isavailable ( pterraintype item );
                       virtual void displaysingleitem ( pterraintype item, int x, int y );
                       virtual int getitemsizex ( void ) { return fieldsizex; } ;
                       virtual int getitemsizey ( void ) { return fieldsizey; } ;
                       virtual void showiteminfos ( pterraintype item, int x1, int y1, int x2, int y2 );
                       virtual ASCString getItemName ( pterraintype item );
};


bool SelectTerrainType :: isavailable ( pterraintype item )
{
   return !ItemFiltrationSystem::isFiltered( ItemFiltrationSystem::Terrain, item->id );
}

void SelectTerrainType :: displaysingleitem ( pterraintype item, int x, int y )
{
   bar ( x, y, x + getitemsizex(), y + getitemsizey(), black );
   if ( auswahlw >= cwettertypennum || auswahlw < 0 )
      auswahlw = 0;
/*
   if ( item )
      if ( item->weather[auswahlw] )
         item->weather[auswahlw]->paint ( SPoint(x, y) );
      else
         item->weather[0]->paint ( SPoint(x, y) );
         */
}

ASCString SelectTerrainType :: getItemName ( pterraintype item )
{
   if ( !item->name.empty() )
      return item->name;
   else
      return "-NONE-";
}


void SelectTerrainType :: showiteminfos ( pterraintype item, int x1, int y1, int x2, int y2 )
{ 
   rectangle ( x1, y1, x2, y2, lightgray );
   bar ( x1+1, y1+1, x2-1, y2-1, black );
   if ( item ) {
      displaysingleitem ( item, x1 + 10, (y1 + y2 - getitemsizey() )/2 );
      npush ( activefontsettings );
      activefontsettings.font = schriften.smallarial;
      activefontsettings.background  = 255;
      activefontsettings.justify = lefttext;
      activefontsettings.color = lightgray;
      activefontsettings.height = 0;
      activefontsettings.length = x2 - x1 - 20 - getitemsizex();

      showtext4 ( "Name: %s", x1 + 20 + getitemsizex(), y1 + 10, getItemName(item).c_str() );

      showtext4 ( "ID: %d", x1 + 20 + getitemsizex(), y1 + 30, item->id );
      npop ( activefontsettings );
   }
}





class SelectObjectType : public SelectAnything< ObjectType* > {
                    protected:
                       virtual bool isavailable ( ObjectType* item );
                       virtual void displaysingleitem ( ObjectType* item, int x, int y );
                       virtual int getitemsizex ( void ) { return fieldsizex; } ;
                       virtual int getitemsizey ( void ) { return fieldsizey; } ;
                       virtual void showiteminfos ( ObjectType* item, int x1, int y1, int x2, int y2 );
                       virtual ASCString getItemName ( ObjectType* item );
};

bool SelectObjectType :: isavailable ( ObjectType* item )
{
  return !ItemFiltrationSystem::isFiltered( ItemFiltrationSystem::Object, item->id );
}


void SelectObjectType :: displaysingleitem ( ObjectType* item, int x, int y )
{
   bar ( x, y, x + getitemsizex(), y + getitemsizey(), black );
   if ( item )
      item->display ( getActiveSurface(), SPoint(x, y) );
}

ASCString SelectObjectType :: getItemName ( ObjectType* item )
{
   if ( !item->name.empty() )
      return item->name;
   else
      return "-NONE-";
}


void SelectObjectType :: showiteminfos ( ObjectType* item, int x1, int y1, int x2, int y2 )
{ 
   rectangle ( x1, y1, x2, y2, lightgray );
   bar ( x1+1, y1+1, x2-1, y2-1, black );
   if ( item ) {
      displaysingleitem ( item, x1 + 10, (y1 + y2 - getitemsizey() )/2 );
      npush ( activefontsettings );
      activefontsettings.font = schriften.smallarial;
      activefontsettings.background  = 255;
      activefontsettings.justify = lefttext;
      activefontsettings.color = lightgray;
      activefontsettings.height = 0;
      activefontsettings.length = x2 - x1 - 20 - getitemsizex();

      showtext4 ( "Name: %s", x1 + 20 + getitemsizex(), y1 + 10, getItemName(item).c_str() );

      showtext4 ( "ID: %d", x1 + 20 + getitemsizex(), y1 + 30, item->id );
      npop ( activefontsettings );
   }
}





class SelectBuildingType : public SelectAnything< BuildingType* > {
                       int buildingfieldsdisplayedx, buildingfieldsdisplayedy;
                    protected:
                       virtual bool isavailable ( BuildingType* item );
                       virtual void displaysingleitem ( BuildingType* item, int x, int y );
                       virtual int getitemsizex ( void ) { return fieldsizex+(buildingfieldsdisplayedx-1)*fielddistx+fielddisthalfx; } ;
                       virtual int getitemsizey ( void ) { return fieldsizey+(buildingfieldsdisplayedy-1)*fielddisty; } ;
                       virtual void showiteminfos ( BuildingType* item, int x1, int y1, int x2, int y2 );
                       virtual ASCString getItemName ( BuildingType* item );
                    public:
                       SelectBuildingType( void ) { buildingfieldsdisplayedx = 4; buildingfieldsdisplayedy = 6; };
};

bool SelectBuildingType :: isavailable ( BuildingType* item )
{
  return !ItemFiltrationSystem::isFiltered( ItemFiltrationSystem::Building, item->id );
}

ASCString SelectBuildingType :: getItemName ( BuildingType* item )
{
   if ( item->name.empty() )
      return "-NONE-";
   else
      return item->name;
}


void SelectBuildingType :: displaysingleitem ( BuildingType* item, int x, int y )
{
   bar ( x, y, x + getitemsizex(), y + getitemsizey(), black );
   if ( item )
      item->paint( getActiveSurface(), SPoint(x,y), farbwahl );
}

void SelectBuildingType :: showiteminfos ( BuildingType* item, int x1, int y1, int x2, int y2 )
{ 
   rectangle ( x1, y1, x2, y2, lightgray );
   bar ( x1+1, y1+1, x2-1, y2-1, black );
   if ( item ) {
      item->paintSingleField( getActiveSurface(), SPoint(x1 + 10, y1 + (y2 - y1 - fieldsizey )/2), item->entry , farbwahl );
      npush ( activefontsettings );
      activefontsettings.font = schriften.smallarial;
      activefontsettings.background  = 255;
      activefontsettings.justify = lefttext;
      activefontsettings.color = lightgray;
      activefontsettings.height = 0;
      activefontsettings.length = x2 - x1 - 20 - fieldsizex;

      showtext4 ( "Name: %s", x1 + 20 + fieldsizex, y1 + 10, getItemName(item).c_str() );

      showtext4 ( "ID: %d", x1 + 20 + fieldsizex, y1 + 30, item->id );
      npop ( activefontsettings );
   }
}


class tweathertype {
         public:
            int num;
            char* name;
       };
typedef tweathertype* pweathertype;
vector< pweathertype > weathervector;


class tcolortype {
         public:
            int col;
            int getcolor ( void ) { return 20 + 8 * col; };
       };
typedef tcolortype* pcolortype;
vector< pcolortype > colorvector;


class tminetype {
         public:
            int type;
            char* name;
            void paint ( int x, int y ) { 
            MineType::paint( MineTypes(type+1), farbwahl, getActiveSurface(), SPoint(x,y) ); 
            };
       };
typedef tminetype* pminetype;
vector< pminetype > minevector;



class SelectColor : public SelectAnything< pcolortype > {
                    protected:
                       virtual bool isavailable ( pcolortype item ) { return 1; };
                       virtual void displaysingleitem ( pcolortype item, int x, int y );
                       virtual int getitemsizex ( void ) { return fieldsizex; } ;
                       virtual int getitemsizey ( void ) { return fieldsizey; } ;
                       virtual void showiteminfos ( pcolortype item, int x1, int y1, int x2, int y2 );
                       virtual int getiteminfoheight( void ) { return 20; };
                       virtual ASCString getItemName ( pcolortype item );
};

ASCString SelectColor :: getItemName ( pcolortype item )
{
  return strrr ( item->col );
}


void SelectColor :: displaysingleitem ( pcolortype item, int x, int y )
{
   if ( item ) {
      bar ( x, y, x + getitemsizex(), y + getitemsizey(), item->getcolor() );
      npush ( activefontsettings );
      activefontsettings.font = schriften.arial8;
      activefontsettings.background  = 255;
      activefontsettings.justify = centertext;
      activefontsettings.color = black;
      activefontsettings.height = 0;
      activefontsettings.length = getitemsizex();
      showtext2 ( getItemName(item).c_str(), x, y + (getitemsizey() - activefontsettings.font->height) / 2 );
      npop ( activefontsettings );
   } else
      bar ( x, y, x + getitemsizex(), y + getitemsizey(), black );
}

void SelectColor :: showiteminfos ( pcolortype item, int x1, int y1, int x2, int y2 )
{ 
    rectangle ( x1, y1, x2, y2, lightgray );
    if ( item )
       bar ( x1+1, y1+1, x2-1 , y2-1, item->getcolor() );
    else
       bar ( x1+1, y1+1, x2-1 , y2-1, 0 );
}





class SelectWeather : public SelectAnything< pweathertype > {
                    protected:
                       virtual bool isavailable ( pweathertype item ) { return 1; };
                       virtual void displaysingleitem ( pweathertype item, int x, int y );
                       virtual int getitemsizex ( void ) { return 120; } ;
                       virtual int getitemsizey ( void ) { return 25; } ;
                       virtual void showiteminfos ( pweathertype item, int x1, int y1, int x2, int y2 );
                       virtual ASCString getItemName ( pweathertype  item ) ;
};

ASCString SelectWeather :: getItemName ( pweathertype  item )
{
  return item->name;
}

void SelectWeather :: displaysingleitem ( pweathertype item, int x, int y )
{
   bar ( x, y, x + getitemsizex(), y + getitemsizey(), black );
   if ( item ) {
      npush ( activefontsettings );
      activefontsettings.font = schriften.smallarial;
      activefontsettings.background  = 255;
      activefontsettings.justify = lefttext;
      activefontsettings.color = white;
      activefontsettings.height = 0;
      activefontsettings.length = getitemsizex() - 10;
      showtext2 ( getItemName( item ).c_str(), x+5, y + (getitemsizey() - activefontsettings.font->height) / 2 );
      npop ( activefontsettings );
   }
}

void SelectWeather :: showiteminfos ( pweathertype item, int x1, int y1, int x2, int y2 )
{ 
   rectangle ( x1, y1, x2, y2, lightgray );
   bar ( x1+1, y1+1, x2-1, y2-1, black );
   if ( item && auswahl ) 
      if ( auswahl->weather[ item->num ] ) {
         // auswahl->weather[item->num]->paint ( SPoint(x1 + 10, (y1 + y2 - fieldsizey )/2) );
      } else {

         // auswahl->weather[0]->paint ( SPoint(x1 + 10, (y1 + y2 - fieldsizey )/2) );

         npush ( activefontsettings );
         activefontsettings.font = schriften.smallarial;
         activefontsettings.background  = 255;
         activefontsettings.justify = lefttext;
         activefontsettings.color = lightgray;
         activefontsettings.height = 0;
         activefontsettings.length = x2 - x1 - 20 - fieldsizex;
   
         showtext4 ( "the terrain '%s'", x1 + 20 + fieldsizex, y1 + 10, auswahl->name.c_str() );
         showtext2 ( "does not have this weather", x1 + 20 + fieldsizex, y1 + 30 );
   
         npop ( activefontsettings );
      }
   
}



class SelectMine : public SelectAnything< pminetype > {
                    protected:
                       virtual bool isavailable ( pminetype item ) { return 1; };
                       virtual void displaysingleitem ( pminetype item, int x, int y );
                       virtual int getitemsizex ( void ) { return fieldsizex; } ;
                       virtual int getitemsizey ( void ) { return fieldsizey; } ;
                       virtual void showiteminfos ( pminetype item, int x1, int y1, int x2, int y2 );
                       virtual ASCString getItemName ( pminetype  item );
};

void SelectMine :: displaysingleitem ( pminetype item, int x, int y )
{
   bar ( x, y, x + getitemsizex(), y + getitemsizey(), black );
   if ( item ) 
      item->paint ( x, y );
}

ASCString SelectMine :: getItemName ( pminetype  item )
{
   if ( item->name && item->name[0] )
      return item->name;
   else
      return "-NONE-";
}


void SelectMine :: showiteminfos ( pminetype item, int x1, int y1, int x2, int y2 )
{ 
   rectangle ( x1, y1, x2, y2, lightgray );
   bar ( x1+1, y1+1, x2-1, y2-1, black );
   if ( item ) {
      displaysingleitem ( item, x1 + 10, (y1 + y2 - getitemsizey() )/2 );
      npush ( activefontsettings );
      activefontsettings.font = schriften.smallarial;
      activefontsettings.background  = 255;
      activefontsettings.justify = lefttext;
      activefontsettings.color = lightgray;
      activefontsettings.height = 0;
      activefontsettings.length = x2 - x1 - 20 - getitemsizex();

      showtext4 ( "Name: %s", x1 + 20 + getitemsizex(), y1 + 10, getItemName(item).c_str() );

      npop ( activefontsettings );
   }
}





class SelectItemContainer {
             SelectTerrainType*   selectterraintype;
             SelectVehicleType*   selectvehicletype;
             SelectColor*         selectcolor;
             SelectObjectType*    selectobjecttype;
             SelectMine*          selectmine;
             SelectWeather*       selectweather;
             SelectBuildingType*  selectbuildingtype;

             struct {
                SelectAnythingBase*  selector;
                char*                name;
                char*                keyname;
                tmouserect           pos;
             } selector[10];
             int                  paintallselections_initialized;
             int                  selectornum;

             void addselector ( SelectAnythingBase* _selector, char* name, char* keyname ) {
                      selector[selectornum].selector = _selector;
                      selector[selectornum].name     = name;
                      selector[selectornum].keyname  = keyname;
                      selector[selectornum].pos.x1 = -1;
                      selector[selectornum].pos.x2 = -1;
                      selector[selectornum].pos.y1 = -1;
                      selector[selectornum].pos.y2 = -1;
                      selectornum++;
             };

             int                  selectionypos;
             void paintselections ( int num, int act );

         public:
             SelectItemContainer ( void ) {
                  selectterraintype = NULL;
                  selectvehicletype = NULL;
                  selectcolor = NULL;
                  selectobjecttype = NULL;
                  selectmine = NULL;
                  selectweather = NULL;
                  selectbuildingtype = NULL;
                  paintallselections_initialized = 0;
             };

             SelectTerrainType*   getterrainselector ( void ) {  
                  if ( !selectterraintype ) {
                     selectterraintype = new SelectTerrainType;
                     selectterraintype->init( terrainTypeRepository.getVector() );
                  }
                  return selectterraintype;
             };

             SelectBuildingType*   getbuildingselector ( void ) {
                  if ( !selectbuildingtype ) {
                     selectbuildingtype = new SelectBuildingType;
                     selectbuildingtype->init( buildingTypeRepository.getVector() );
                  }
                  return selectbuildingtype;
             };

             SelectVehicleType*   getvehicleselector ( void ) {
                  if ( !selectvehicletype ) {
                     selectvehicletype = new SelectVehicleType;
                     selectvehicletype->init( vehicleTypeRepository.getVector() );
                  }
                  return selectvehicletype;
             };

             SelectObjectType*   getobjectselector ( void ) {
                  if ( !selectobjecttype ) {
                     selectobjecttype = new SelectObjectType;
                     selectobjecttype->init( objectTypeRepository.getVector() );
                  }
                  return selectobjecttype;
             };

             SelectWeather*   getweatherselector ( void ) {
                  if ( !selectweather ) {
                     for ( int i = 0; i < cwettertypennum; i++ ) {
                        pweathertype wt = new tweathertype;
                        wt->num = i;
                        wt->name = strdup ( cwettertypen[i] );
                        weathervector.push_back ( wt );
                     }
                     selectweather = new SelectWeather;
                     selectweather->init( weathervector );
                  }
                  return selectweather;
             };

             SelectMine*   getmineselector ( void ) {
                  if ( !selectmine ) {
                     for ( int i = 0; i < cminenum; i++ ) {
                        pminetype mt = new tminetype;
                        mt->type = i;
                        minevector.push_back( mt );
                        mt->name = strdup ( MineNames[ i ] );
                     }
                     selectmine = new SelectMine;
                     selectmine->init( minevector );
                  }
                  return selectmine;
             };

             SelectColor*   getcolorselector ( void ) {
                  if ( !selectcolor ) {
                     for ( int i = 0; i < 9; i++ ) {
                        pcolortype ct = new tcolortype;
                        ct->col = i;
                        colorvector.push_back( ct );
                     }
                     selectcolor = new SelectColor;
                     selectcolor->init( colorvector );
                  }
                  return selectcolor;
             };


             void checkformouse ( void );
      } selectitemcontainer;


void SelectItemContainer :: paintselections ( int num, int act )
{

   int backgroundcol;
   if ( num == act ) 
      backgroundcol = white;
   else 
      backgroundcol = lightgray;


   int freespace = 20;

   npush ( activefontsettings );
   activefontsettings.font = schriften.smallarial;
   activefontsettings.background  = backgroundcol;
   activefontsettings.justify = centertext;
   activefontsettings.color = black;
   activefontsettings.height = 0;
   activefontsettings.length = selfontxsize;

   int keywidth = 20;

   int x1 = selfontxanf;
   int y1 = selectionypos;
   int x2 = selfontxanf + selfontxsize - keywidth;
   int y2 = selectionypos + activefontsettings.font->height + selector[num].selector->getiteminfoheight();

   selector[num].pos.x1 = x1;
   selector[num].pos.x2 = x2 + keywidth;
   selector[num].pos.y1 = y1;
   selector[num].pos.y2 = y2;

   if ( y2 < agmp->resolutiony ) {
      showtext2 ( selector[num].name, x1, y1 );

      y1 += activefontsettings.font->height;


      selector[num].selector->showactiteminfos ( x1, y1, x2, y2 );
      bar ( x2, y1, x2 + keywidth, y2, backgroundcol );

      selectionypos += selector[num].selector->getiteminfoheight() + freespace;
      if (y2 + freespace < agmp->resolutiony )
         bar ( x1, y2+1, x2 + keywidth, y2 + freespace, black );
      activefontsettings.background  = 255;
      activefontsettings.color = black;
      activefontsettings.length = keywidth;
      showtext2 ( selector[num].keyname, x2, y1 + ( y2 - y1 - activefontsettings.font->height ) / 2);
   }
   npop ( activefontsettings );
}


void SelectItemContainer :: checkformouse ( void ) 
{
   if ( mouseparams.taste == 1 ) {
      int found = -1;
      for ( int i = 0; i < selectornum; i++ )
         if ( mouseinrect ( &selector[i].pos )) {
            found = i;
            while ( mouseparams.taste & 1 )
               releasetimeslice();
            break;   
         }

      switch ( found ) {
         case 0: if ( skeypress ( ct_lstrg ) || skeypress ( ct_rstrg ) || skeypress ( ct_lshift ) || skeypress ( ct_rshift ))
                    selterraintype ( ct_invvalue );
                 else
                    execaction_ev(act_switchmaps );
            break;
         case 1: selvehicletype ( ct_invvalue );
            break;
         case 2: selcolor ( ct_invvalue );
            break;
         case 3: selbuilding ( ct_invvalue );
            break;
         case 4: if ( skeypress ( ct_lstrg ) || skeypress ( ct_rstrg ) || skeypress ( ct_lshift ) || skeypress ( ct_rshift ))
                    selobject ( ct_invvalue );
                 else
                    execaction_ev(act_switchmaps );
            break;
         case 5: selmine ( ct_invvalue );
            break;
         case 6: selweather ( ct_invvalue );
            break;
      } /* endswitch */
   }
}


void selterraintype( tkey ench )
{  
   auswahl = selectitemcontainer.getterrainselector()->selectitem( auswahl, ench );
   lastselectiontype = cselbodentyp;
}

Vehicletype* selvehicletype(tkey ench )
{
   auswahlf = selectitemcontainer.getvehicleselector()->selectitem( auswahlf, ench );
   lastselectiontype = cselunit;
   return auswahlf;
}

void selcolor( tkey ench )
{
   int oldsel = farbwahl;
   farbwahl = selectitemcontainer.getcolorselector()->selectitem( colorvector[farbwahl], ench )->col;
   if ( farbwahl > 8 )
      farbwahl = oldsel;
       
   if ( lastselectiontype != cselunit && lastselectiontype != cselbuilding && lastselectiontype != cselmine )
      lastselectiontype = cselunit;
}

void selobject( tkey ench )
{  
   actobject = selectitemcontainer.getobjectselector()->selectitem( actobject, ench );
   lastselectiontype = cselobject;
}

void selmine( tkey ench )
{  
   auswahlm = selectitemcontainer.getmineselector()->selectitem( minevector[auswahlm], ench )->type;
   lastselectiontype = cselmine;
}

void selweather( tkey ench )
{  
   auswahlw = selectitemcontainer.getweatherselector()->selectitem( weathervector[auswahlw], ench )->num;
   lastselectiontype = cselbodentyp;
}

void selbuilding ( tkey ench )
{
   auswahlb = selectitemcontainer.getbuildingselector()->selectitem( auswahlb, ench );
   lastselectiontype = cselbuilding;
}

  /* Nr
 1:terrain
 2:vehicletypeen
 3:Farben
 4:Buildings
 5:(Keine Auswahl) SPezielle Darstellung f�r vehicle beim beladen (mit Gewicht)
 6:object
 7:Fahrzeuge f�r BuildingCargo
 8:Fahrzeuge f�r vehicleCargo
 9:Minen
 10: Weathersel
 */ 



void checkselfontbuttons(void)
{
   selectitemcontainer.checkformouse( );
}




void setnewvehicleselection ( Vehicletype* v )
{
   selectitemcontainer.getvehicleselector()->setnewselection ( v );
   selectitemcontainer.getcolorselector()->setnewselection( colorvector[farbwahl] );
//   showallchoices();
}

void resetvehicleselector ( void )
{
   selectitemcontainer.getvehicleselector()->init( vehicleTypeRepository.getVector() );
}

void resetbuildingselector ( void )
{
   selectitemcontainer.getbuildingselector()->init( buildingTypeRepository.getVector() );
}

void resetterrainselector ( void )
{
   selectitemcontainer.getterrainselector()->init( terrainTypeRepository.getVector() );
}

void resetobjectselector ( void )
{
   selectitemcontainer.getobjectselector()->init( objectTypeRepository.getVector() );
}


void setnewterrainselection ( pterraintype t )
{
   selectitemcontainer.getterrainselector()->setnewselection( t );
}

void setnewobjectselection  ( ObjectType* o )
{
   selectitemcontainer.getobjectselector()->setnewselection( o );
}

void setnewbuildingselection ( BuildingType* v )
{
   selectitemcontainer.getbuildingselector()->setnewselection ( v );
   selectitemcontainer.getcolorselector()->setnewselection( colorvector[farbwahl] );
//   showallchoices();
}







class SelectCargoVehicleType : public SelectVehicleType {
      public:
          void showiteminfos ( Vehicletype* item, int x1, int y1, int x2, int y2 );
     };

void SelectCargoVehicleType :: showiteminfos ( Vehicletype* item, int x1, int y1, int x2, int y2 )
{
   rectangle ( x1, y1, x2, y2, lightgray );
   bar ( x1+1, y1+1, x2-1, y2-1, black );
   if ( item ) {
      displaysingleitem ( item, x1 + 10, (y1 + y2 - getitemsizey() )/2 );
      npush ( activefontsettings );
      activefontsettings.font = schriften.smallarial;
      activefontsettings.background  = 255;
      activefontsettings.justify = lefttext;
      activefontsettings.color = lightgray;
      activefontsettings.height = 0;
      activefontsettings.length = x2 - x1 - 20 - getitemsizex();

      if ( !item->name.empty() )
         showtext4 ( "Name: %s", x1 + 20 + getitemsizex(), y1 + 10, item->name.c_str() );
      else
         if ( !item->description.empty()  )
            showtext4 ( "Name: %s", x1 + 20 + getitemsizex(), y1 + 10, item->description.c_str() );
         else
            showtext4 ( "Name: NONE", x1 + 20 + getitemsizex(), y1 + 10 );

      showtext4 ( "weight: %d", x1 + 20 + getitemsizex(), y1 + 30, item->maxsize() );
      npop ( activefontsettings );
   }
}



class SelectVehicleTypeForContainerCargo : public SelectCargoVehicleType {
         ContainerBase* container;
      public:
         SelectVehicleTypeForContainerCargo ( ContainerBase* _container ) { container = _container; };
         bool isavailable ( Vehicletype* item );
     };

bool SelectVehicleTypeForContainerCargo :: isavailable ( Vehicletype* item )
{
   bool result = false;
   if ( SelectVehicleType::isavailable ( item ) ) {
      Vehicle* unit = new Vehicle ( item, actmap, container->getOwner() );
      if ( container->vehicleFit ( unit ))
         result = true;
      delete unit;
   }
   return result;
}

class SelectVehicleTypeForBuildingProduction : public SelectCargoVehicleType {
         Building* building;
      public:
         SelectVehicleTypeForBuildingProduction ( Building* _building ) { building = _building; };
         bool isavailable ( Vehicletype* item ) {  
            for ( int i = 0; i < building->unitProduction.size(); i++ )
               if ( building->unitProduction[i] == item )
                  return 0;
            return building->typ->vehicleFit ( item )
                   && SelectVehicleType::isavailable ( item )
                  &&  ( building->vehicleUnloadable(item) || (building->typ->hasFunction( ContainerBaseType::InternalVehicleProduction  ) ));
         };
     };

//* �S Fahrzeuge f�r Unit-Beloading





//* �S Fahrzeuge f�r Geb�ude-Production

void selbuildingproduction( Building* bld )
{  
   SelectVehicleTypeForBuildingProduction svtfbc ( bld );
   svtfbc.init( vehicleTypeRepository.getVector() );
   Vehicletype* newcargo = svtfbc.selectitem ( NULL );
   if ( newcargo )
      bld->unitProduction.push_back( newcargo );
}

void selcargo( ContainerBase* container )
{
   SelectVehicleTypeForContainerCargo svtftc ( container );
   svtftc.init( vehicleTypeRepository.getVector() );
   Vehicletype* newcargo = svtftc.selectitem ( NULL );

   if ( newcargo ) {
      Vehicle* unit = new Vehicle ( newcargo, actmap, container->getOwner() );
      unit->fillMagically();
      unit->setnewposition ( container->getPosition() );
      unit->tank.material = 0;
      unit->tank.fuel = 0;
      if ( container->vehicleFit ( unit )) {
         unit->tank.material = unit->typ->tank.material;
         unit->tank.fuel = unit->typ->tank.fuel;

         if ( !container->vehicleFit ( unit )) {
            unit->tank.material = 0;
            unit->tank.fuel = 0;
            displaymessage("Warning:\nThe unit you just set could not be loaded with full material and fuel\nPlease set these values manually",1);
         }
         container->addToCargo( unit );

      } else {
        delete unit;
        displaymessage("The unit could not be loaded !",1);
      }
   }
}


