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

#ifndef edselfntH
#define edselfntH

#include <algorithm>

#include <paragui.h>
#include <pgwidget.h>
#include <pgwidgetlist.h>
#include <pgwindow.h>
#include <pgapplication.h>

#include "events.h"
#include "itemrepository.h"


#define selfontyanf 40       						//Auswahlanzeige Yanf
#define selfontxsize 280 						//Breite des Auswahlbalkens am Rand
#define selfontxanf ( agmp->resolutionx - selfontxsize - 10 )  	//Auswahlanzeige Xanf

extern void selterraintype( tkey ench );
extern pvehicletype selvehicletype(tkey ench );
extern void selcolor( tkey ench );
extern void selobject( tkey ench );
extern void selmine( tkey ench );
extern void selweather( tkey ench );
extern void selbuilding ( tkey ench );
extern void selcargo( ContainerBase* container );
extern void selbuildingproduction( Building* eht );



class MapComponent;

extern SigC::Signal0<void> filtersChangedSignal;

class SelectionHolder : public SigC::Object {
     const MapComponent* currentItem;
     int actplayer;
     int currentWeather;
     
  public:
     SelectionHolder() : currentItem(NULL), actplayer(0), currentWeather(0),brushSize(1) {};
 
     int getPlayer() { return actplayer; };
     void setPlayer( int player );
     
     void setWeather( int weather );
     int getWeather() { return currentWeather; };
     
     int brushSize;
     const MapComponent* getSelection();
     void setSelection( const MapComponent* component ) ;
     void pickup ( pfield fld );
     
     SigC::Signal1<void,const MapComponent*> selectionChanged;
   
};
 
extern SelectionHolder selection;


class MapComponent {
    protected:
       virtual Surface& getClippingSurface() const = 0;
    public:  
       static const int fontHeight = 20;
       virtual ASCString getName() const = 0;
       virtual int displayWidth() const = 0;
       virtual int displayHeight() const = 0;
       virtual MapComponent* clone() const = 0;
       virtual int place( const MapCoordinate& mc ) const = 0;
       //! just a wrapper so we have a function return void
       void vPlace( const MapCoordinate& mc ) const { place( mc ); };  
       virtual void display( Surface& s, const SPoint& pos ) const = 0;
       virtual int getID() const = 0;
       virtual bool supportMultiFieldPlacement() const { return true; };
       virtual void display( PG_Widget* parent, SDL_Surface * surface, const PG_Rect & src, const PG_Rect & dst ) const;
       virtual ~MapComponent() {};
};


template<class Item> class BasicItem : public MapComponent {
    protected:
       const Item* item;
       static Surface clippingSurface;
       Surface& getClippingSurface() const { return clippingSurface; };
    public:  
       BasicItem( const Item* i ) : item( i ) {};
       ASCString getName() const { return item->getName(); };
       virtual int displayWidth() const { return Width(); };
       static  int Width() { return fieldsizex; };
       virtual int displayHeight() const { return Height(); };
       static int Height() { return fieldsizey; };
       virtual int getID() const { return item->id; };
};

template<class C> class ItemTypeSelector {};

class VehicleItem : public BasicItem<Vehicletype> {
    public:  
       VehicleItem( const Vehicletype* vehicle ) : BasicItem<Vehicletype>( vehicle ) {};
       virtual int place( const MapCoordinate& mc ) const ;
       virtual void display( Surface& s, const SPoint& pos ) const { item->paint ( s, pos, selection.getPlayer() ); };
       virtual MapComponent* clone() const { return new VehicleItem( item ); };
};
template<> class ItemTypeSelector<Vehicletype> {
   public:
      typedef VehicleItem type;
};



class BuildingItem : public MapComponent {
       const BuildingType* bld;
       static Surface clippingSurface;
       static Surface fullSizeImage;
    protected:
       Surface& getClippingSurface() const { return clippingSurface; };
    public:  
       BuildingItem( const BuildingType* building ) : bld( building ) {};
       ASCString getName() const { return bld->getName(); };
       virtual int displayWidth() const { return Width(); };
       static  int Width() { return (fieldsizex+(4-1)*fielddistx+fielddisthalfx)/2; };
       virtual int displayHeight() const { return Height(); };
       static int Height() { return (fieldsizey+(6-1)*fielddisty)/2; };
       virtual bool supportMultiFieldPlacement() const { return false; };
       virtual int place( const MapCoordinate& mc ) const ;
       virtual void display( Surface& s, const SPoint& pos ) const;
       virtual MapComponent* clone() const { return new BuildingItem( bld ); };
       virtual int getID() const { return bld->id; };
};
template<> class ItemTypeSelector<BuildingType> {
   public:
      typedef BuildingItem type;
};


class ObjectItem : public BasicItem<ObjectType> {
    public:  
       ObjectItem( const ObjectType* object ) : BasicItem<ObjectType>( object ) {};
       virtual int place( const MapCoordinate& mc ) const;
       virtual void display( Surface& s, const SPoint& pos ) const { item->display (s, pos); };
       virtual MapComponent* clone() const { return new ObjectItem( item ); };
};
template<> class ItemTypeSelector<ObjectType> {
   public:
      typedef ObjectItem type;
};


class TerrainItem : public BasicItem<TerrainType> {
    public:  
       TerrainItem( const TerrainType* object ) : BasicItem<TerrainType>( object ) {};
       virtual int place( const MapCoordinate& mc ) const;
       virtual void display( Surface& s, const SPoint& pos ) const { item->weather[0]->paint (s, pos); };
       virtual MapComponent* clone() const { return new TerrainItem( item ); };
};

template<> class ItemTypeSelector<TerrainType> {
   public:
      typedef TerrainItem type;
};



extern void sortItems( vector<Vehicletype*>& vec );
extern void sortItems( vector<BuildingType*>& vec );
extern void sortItems( vector<ObjectType*>& vec );
extern void sortItems( vector<TerrainType*>& vec );




class SingleItemWidget : public PG_Widget {
      PG_Window* my_window;
      const MapComponent* it;
      static const int labelHeight = 15;
   public:
      SingleItemWidget( const MapComponent* item, PG_Widget* parent, SPoint pos, PG_Window* window = NULL ) : PG_Widget( parent, PG_Rect( pos.x, pos.y, item->displayWidth(), item->displayHeight() + MapComponent::fontHeight ) ),  my_window(window), it(item)
      {
      }

      int ItemHeight() { 
         return it->displayHeight() + labelHeight; 
      };
      
      void set( const MapComponent* item) { it = item; };

      ~SingleItemWidget() {
         delete it;
      };   
      
      ASCString getItemName() {
         return it->getName();
      }
      
      bool nameMatch( const ASCString& name )
      {
         ASCString a = name;
         a.toLower();
         ASCString b = getItemName().toLower();
         if ( a.length() > 0 && b.length() > 0 )
            if ( b.find ( a ) == 0 ) 
               return true;
         return false;
      };   
           
   protected:
      
      bool eventMouseButtonUp (const SDL_MouseButtonEvent *button) {
         if ( my_window ) 
            if ( button->type == SDL_MOUSEBUTTONUP && button->button == SDL_BUTTON_LEFT ) {
               my_window->Hide();
               my_window->QuitModal();
               selection.setSelection( it );
               return true;
            }   
         return false;
      };
      
      void eventBlit ( SDL_Surface * surface, const PG_Rect & src, const PG_Rect & dst )
      {
         it->display( this, PG_Application::GetScreen(), src, dst );
      };

};



template <class ItemType> class ItemSelector : public PG_Window {

      const ItemRepository<ItemType>& repository;
      vector<ItemType*> items;
      typedef vector<SingleItemWidget*> WidgetList;
      WidgetList widgets;
      int rowCount;
      int columnCount;
      PG_ScrollWidget* scrollWidget;
      static const int gapWidth = 5;
      PG_Label* nameSearch;
   protected:   
      bool eventKeyDown(const SDL_KeyboardEvent* key) {
         if ( key->keysym.sym == SDLK_BACKSPACE ) {
            ASCString s = nameSearch->GetText();
            if ( s.length() > 0 ) {
               s.erase( s.length() - 1 );
               nameSearch->SetText( s );
            }
            return true;
         } 
         
         if ( key->keysym.unicode <= 255 ) {
            nameSearch->SetText( nameSearch->GetText() + char ( key->keysym.unicode ));
            locateObject( nameSearch->GetText() );
            return true;
         }   
         return false;
      };
      
      void locateObject( const ASCString& name ) {
         for ( WidgetList::iterator i = widgets.begin(); i != widgets.end(); ++i ) {
            if ( (*i)->nameMatch( name )  ) {
               scrollWidget->ScrollToWidget( *i );
               return;
            }   
         }
      }
      
   public:
      ItemSelector( PG_Widget *parent, const PG_Rect &r , const ItemRepository<ItemType>& itemRepository ) : PG_Window( parent,r,"Item Selector"), repository( itemRepository), scrollWidget( NULL) {
         columnCount = r.w / (ItemTypeSelector<ItemType>::type::Width()  + gapWidth);
         reLoad();
         setupWidgets();
         filtersChangedSignal.connect( SigC::slot( *this, &ItemSelector<ItemType>::filtersChanged ));
         nameSearch = new PG_Label ( this, PG_Rect( 5, Height() - 25, Width() - 10, 20 ));
      };

      int RunModal()
      {
         nameSearch->SetText( "" );
         return PG_Window::RunModal();
      }
      
      virtual bool isFiltered( const ItemType* item) {
         return ItemFiltrationSystem::isFiltered( item );
      };   

      void filtersChanged() {
         reLoad();
         setupWidgets();
      }         
           
      void reLoad() {
         items.clear();
         for ( size_t i = 0; i < repository.getNum(); ++i ) {
            ItemType* item = repository.getObject_byPos(i);
            if ( item && !isFiltered(item)) 
               items.push_back(item);
         }
         sortItems( items );
         // sort( items.begin(), items.end(), itemComp );      
         // sort( items.begin(), items.end() );      
      };
      
      void setupWidgets()
      {
         delete scrollWidget;
         scrollWidget = new PG_ScrollWidget( this , PG_Rect( 0, GetTitlebarHeight () + 2, Width(), Height()- GetTitlebarHeight ()- 30 ));
         widgets.clear();
         /*
	widgetList->SetDirtyUpdate(false);
	widgetList->SetTransparency(0);
	widgetList->SetBackground("default/wnd_close.bmp", PG_Draw::TILE, 0xFF);
	widgetList->SetBackgroundBlend(0);
			
	widgetList->EnableScrollBar(true, PG_ScrollBar::VERTICAL);
	widgetList->EnableScrollBar(true, PG_ScrollBar::HORIZONTAL);
        */
         int x = 0;
         int y = 0;
         for ( size_t i = 0; i < items.size(); ++i ) {
            typedef typename ItemTypeSelector<ItemType>::type itt ;
            widgets.push_back( new SingleItemWidget ( new itt( items[i] ), scrollWidget, SPoint( x * (ItemTypeSelector<ItemType>::type::Width() + gapWidth), y * (ItemTypeSelector<ItemType>::type::Height() + MapComponent::fontHeight + gapWidth) ), this) );
            // new PG_Button( widgetList, PG_Rect( 10 + x , 10+y , 40,20), items[i]->getName() );
            ++x;
            if ( x >= columnCount ) {
               x = 0; 
               ++y;
            }
         }   
      }
         
};

#endif
