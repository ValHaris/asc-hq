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
#include "objects.h"


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



extern SigC::Signal0<void> filtersChangedSignal;


class MapComponentPlacer {
   public:

};


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
       virtual void display( Surface& s, const SPoint& pos ) const { item->paint ( s, pos, 0 ); };
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




class MineItem : public BasicItem<MineType> {
    public:  
       MineItem( const MineType* object ) : BasicItem<MineType>( object ) {};
       virtual int place( const MapCoordinate& mc ) const;
       virtual void display( Surface& s, const SPoint& pos ) const { item->paint(s, pos); };
       virtual MapComponent* clone() const { return new MineItem( item ); };
};

template<> class ItemTypeSelector<MineType> {
   public:
      typedef MineItem type;
};



extern void sortItems( vector<Vehicletype*>& vec );
extern void sortItems( vector<BuildingType*>& vec );
extern void sortItems( vector<ObjectType*>& vec );
extern void sortItems( vector<TerrainType*>& vec );
extern void sortItems( vector<MineType*>& vec );






class SingleItemWidget : public PG_Widget {
      PG_Window* my_window;
      const MapComponent* it;
      SingleItemWidget** my_selectedItemMarker;
      static const int labelHeight = 15;
   public:
      SingleItemWidget( const MapComponent* item, PG_Widget* parent, SPoint pos, PG_Window* window = NULL, SingleItemWidget** selectedItemMarker = NULL ) : PG_Widget( parent, PG_Rect( pos.x, pos.y, item->displayWidth(), item->displayHeight() + MapComponent::fontHeight ) ),  my_window(window), it(item), my_selectedItemMarker( selectedItemMarker )
      {
      }
      
      const MapComponent* getMapComponent() { return it; };

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

      SigC::Signal1<void,const MapComponent*> itemSelected;
                 
   protected:
      
      bool eventMouseButtonUp (const SDL_MouseButtonEvent *button) {
         if ( my_window ) 
            if ( button->type == SDL_MOUSEBUTTONUP && button->button == SDL_BUTTON_LEFT ) {
               my_window->Hide();
               my_window->QuitModal();
               if ( my_selectedItemMarker )
                  *my_selectedItemMarker = this;
               itemSelected( it );
               return true;
            }   
         return false;
      };
      
      bool eventMouseButtonDown (const SDL_MouseButtonEvent *button) {
         if ( my_window ) 
            if ( button->type == SDL_MOUSEBUTTONDOWN && button->button == SDL_BUTTON_LEFT ) {
               if ( my_selectedItemMarker )
                  *my_selectedItemMarker = this;
               Update();
               return true;
            }   
         return false;
      };
      
      void eventBlit ( SDL_Surface * surface, const PG_Rect & src, const PG_Rect & dst )
      {
         if ( my_selectedItemMarker && *my_selectedItemMarker && *my_selectedItemMarker == this)
            SDL_FillRect( PG_Application::GetScreen(), const_cast<PG_Rect*>(&dst), 0xff666666 );
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
      SingleItemWidget* selectedItem;
   protected:   
      bool moveSelection( int amount ) 
      {
         WidgetList::iterator i;
         if ( !selectedItem ) {
            i = widgets.begin();
         } else {
            i = find( widgets.begin(), widgets.end(), selectedItem );
            if ( i != widgets.end() ) {
               if ( amount > 0 ) {
                  if ( widgets.end() - i > amount )
                     i += amount;
                  else
                     i = widgets.end() - 1;
               } else
               if ( amount < 0 ) {
                  if ( i - widgets.begin() >= -amount )
                     i += amount;
                  else
                     i = widgets.begin();
               }
            }
         }
         
         if ( *i != selectedItem ) {
            selectedItem = *i;
            scrollWidget->ScrollToWidget( *i );
            Update();
            return true;
         } 
         return false;
      }
   
      bool eventKeyDown(const SDL_KeyboardEvent* key) {
         if ( key->keysym.sym == SDLK_BACKSPACE ) {
            ASCString s = nameSearch->GetText();
            if ( s.length() > 0 ) {
               s.erase( s.length() - 1 );
               nameSearch->SetText( s );
            }
            return true;
         } 
         if ( key->keysym.sym == SDLK_RIGHT )  {
            moveSelection(1);
            return true;
         }
         if ( key->keysym.sym == SDLK_LEFT )  {
            moveSelection(-1);
            return true;
         }
         if ( key->keysym.sym == SDLK_UP )  {
            moveSelection(-columnCount);
            return true;
         }
         if ( key->keysym.sym == SDLK_DOWN )  {
            moveSelection(columnCount);
            return true;
         }

         if ( key->keysym.sym == SDLK_RETURN ) {
            if ( selectedItem ) {
               itemSelected( selectedItem->getMapComponent() );
               Hide();
               QuitModal();
               return true;
            }
         } 
         
                  
         if ( key->keysym.unicode <= 255 ) {
            ASCString newtext = nameSearch->GetText() + char ( key->keysym.unicode );
            if ( locateObject( newtext ) ) 
               nameSearch->SetText( newtext );
            
            return true;
         }   
         
         
         return false;
      };
      
      bool locateObject( const ASCString& name ) {
         for ( WidgetList::iterator i = widgets.begin(); i != widgets.end(); ++i ) {
            if ( (*i)->nameMatch( name )  ) {
               selectedItem = *i;
               scrollWidget->ScrollToWidget( *i );
               Update();
               return true;
            }   
         }
         return false;
      }
      
   public:
      ItemSelector( PG_Widget *parent, const PG_Rect &r , const ItemRepository<ItemType>& itemRepository ) : PG_Window( parent,r,"Item Selector"), repository( itemRepository), scrollWidget( NULL), selectedItem(NULL) {
         columnCount = r.w / (ItemTypeSelector<ItemType>::type::Width()  + gapWidth);
         reLoad();
         setupWidgets();
         filtersChangedSignal.connect( SigC::slot( *this, &ItemSelector<ItemType>::filtersChanged ));
         nameSearch = new PG_Label ( this, PG_Rect( 5, Height() - 25, Width() - 10, 20 ));
      };
      
      SigC::Signal1<void,const MapComponent*> itemSelected;

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
      };
      
      void setupWidgets()
      {
         delete scrollWidget;
         scrollWidget = new PG_ScrollWidget( this , PG_Rect( 0, GetTitlebarHeight () + 2, Width(), Height()- GetTitlebarHeight ()- 30 ));
         widgets.clear();
         
         int x = 0;
         int y = 0;
         for ( size_t i = 0; i < items.size(); ++i ) {
            typedef typename ItemTypeSelector<ItemType>::type itt ;
            SingleItemWidget* siw = new SingleItemWidget ( new itt( items[i] ), scrollWidget, SPoint( x * (ItemTypeSelector<ItemType>::type::Width() + gapWidth), y * (ItemTypeSelector<ItemType>::type::Height() + MapComponent::fontHeight + gapWidth) ), this, &selectedItem);
            siw->itemSelected.connect( itemSelected );
            widgets.push_back( siw );
            ++x;
            if ( x >= columnCount ) {
               x = 0; 
               ++y;
            }
         }   
      }
         
};

#endif
