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

#include "dialogs/vehicletypeselector.h"
#include "unitset.h"

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
      fontProvidingWidget = new PG_ThemeWidget( NULL, PG_Rect::null , false, "MapedItemSelector" );
      
   /*
   SDL_Rect      blitRect;
   blitRect.x = 0;
   blitRect.y = displayHeight() + 2;
   blitRect.w = displayWidth();
   blitRect.h = displayHeight() - blitRect.y;

   PG_FontEngine::RenderText( getClippingSurface().getBaseSurface(), blitRect, blitRect.x, blitRect.y+fontProvidingWidget ->GetFontAscender(), getItemType()->getName(), fontProvidingWidget ->GetFont() );
   */

   PG_Draw::BlitSurface( getClippingSurface().getBaseSurface(), src, surface, dst);

   int x = dst.x;
   int y = dst.y + displayHeight() + fontProvidingWidget ->GetFontAscender() - src.y;
   
   PG_FontEngine::RenderText( surface, dst, x, y, getItemType()->getName(), fontProvidingWidget ->GetFont() );
}




// template<class Vehicletype> Surface BasicItem<Vehicletype>::clippingSurface;
int VehicleItem::place( const MapCoordinate& mc ) const
{
   tfield* fld = actmap->getField(mc);
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
         if ( terrainaccessible( fld, fld->vehicle ) == 2 ) //  || actmap->getgameparameter( cgp_movefrominvalidfields)
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
   if ( !actmap->getField(mc)->addobject( item ) )
      if ( SDL_GetKeyState(NULL)[SDLK_LCTRL] || SDL_GetKeyState(NULL)[SDLK_RCTRL] || choice_dlg("object cannot be built here\n(bypass this dialog by pressing <ctrl>)","~c~ancel","~i~gnore") == 2)
         actmap->getField(mc)->addobject( item, -1, true );

   return 0;
}


template<typename T> Surface BasicItem<T>::clippingSurface;
int TerrainItem::place( const MapCoordinate& mc ) const
{
   tfield* fld = actmap->getField(mc);
   fld->typ = item->weather[0]; 
   fld->setweather( selection.getWeather() );
   fld->setparams();
   for ( int d = 0; d < 6; ++d ) {
      MapCoordinate pos = getNeighbouringFieldCoordinate( mc, d );
      tfield* fld = actmap->getField( pos );
      if ( fld ) 
         for ( tfield::ObjectContainer::iterator i = fld->objects.begin(); i != fld->objects.end(); ++i )
            calculateobject( pos, false, i->typ, actmap );
   }
   return 0;
}


int MineItem::place( const MapCoordinate& mc ) const
{
   actmap->getField(mc)->putmine( selection.getPlayer(), item->id, MineBasePunch[item->id-1] );
   return 0;
}





















class CargoItemFactory: public MapItemTypeWidgetFactory<MapItemTypeWidget< Vehicletype > > {
      typedef MapItemTypeWidgetFactory<MapItemTypeWidget< Vehicletype > > Parent;
      typedef MapItemTypeWidget< Vehicletype > WidgetType;
       ContainerBase* container;
   protected:
      bool isFiltered( const ItemType& item ) {
         if ( Parent::isFiltered( item ))
            return true;
      
         if ( !container->baseType->vehicleFit( &item ))
            return true;
              
         bool result = true;
         Vehicle* unit = new Vehicle ( &item, actmap, container->getOwner() );
         if ( container->vehicleFit ( unit ))
            result = false;
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
               unit->tank.material = unit->getStorageCapacity().material;
               unit->tank.fuel = unit->getStorageCapacity().fuel;
      
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


void addCargo( ContainerBase* container )
{
   ItemSelectorWindow isw( NULL, PG_Rect( 100, 100, 280, 600), "cargo", new CargoItemFactory( container ) );
   isw.Show();
   isw.RunModal();
}





class ProductionItemFactory: public MapItemTypeWidgetFactory<MapItemTypeWidget< Vehicletype > > {
      typedef MapItemTypeWidgetFactory<MapItemTypeWidget< Vehicletype > > Parent;
      typedef MapItemTypeWidget< Vehicletype > WidgetType;
      ContainerBase* container;
      const Vehicletype* selectedItem;
   protected:
      bool isFiltered( const ItemType& item ) {
         if ( Parent::isFiltered( item ))
            return true;
      
         if ( !container->baseType->vehicleFit( &item ))
            return true;
         
         if ( !(container->baseType->vehicleCategoriesProduceable & (1 << item.movemalustyp)))
            return true;

         return false;
      };

   public:
      ProductionItemFactory(  ContainerBase* container_ )  : Parent(vehicleTypeRepository), container( container_ ), selectedItem(NULL) {};
      

      void itemMarked  ( const SelectionWidget* widget )
      {
         if ( !widget )
            return;
            
         const WidgetType* mapItemWidget = dynamic_cast<const WidgetType*>(widget);
         assert( mapItemWidget );
         const Vehicletype* type = mapItemWidget->getItem();
         if ( type ) {
            selectedItem = type;
         }
      };
      
      void itemSelected( const SelectionWidget* widget, bool mouse )
      {
         itemMarked( widget );
      }
      
      const Vehicletype* getSelectedVehicleType()
      {
         return selectedItem;
      }
     
};


class AvailableProductionItemFactory: public SelectionItemFactory, public SigC::Object  {
   private:
      const ContainerBase* container;
      const Vehicletype* selectedItem;
   protected:
      ContainerBase::Production::const_iterator it;
      ContainerBase::Production& production;
   public:
      AvailableProductionItemFactory(  ContainerBase* container_, ContainerBase::Production& prod )  : container( container_ ), selectedItem(NULL), production( prod )
      {
         restart();
      };
      
      void restart()
      {
         // sort( items.begin(), items.end(), VehicleComp );
         it = production.begin();
      }
      
      SelectionWidget* spawnNextItem( PG_Widget* parent, const PG_Point& pos )
      {
         if ( it != production.end() ) {
            const Vehicletype* v = *(it++);
            return new VehicleTypeBaseWidget( parent, pos, parent->Width() - 15, v, actmap->actplayer );
         } else
            return NULL;
      };
      
      void itemMarked  ( const SelectionWidget* widget )
      {
         if ( !widget )
            return;
            
         const VehicleTypeBaseWidget* mapItemWidget = dynamic_cast<const VehicleTypeBaseWidget*>(widget);
         assert( mapItemWidget );
         const Vehicletype* type = mapItemWidget->getVehicletype();
         if ( type ) {
            selectedItem = type;
         }
      }
      
      
      void itemSelected( const SelectionWidget* widget, bool mouse )
      {
         itemMarked( widget );
      }

      const Vehicletype* getSelectedVehicleType()
      {
         return selectedItem;
      }
            
};



class ProductionEditorWindow : public ASC_PG_Dialog {
   private:
      ProductionItemFactory* allTypesFactory;
      ItemSelectorWidget* allTypes;
      
      AvailableProductionItemFactory* productionFactory;
      ItemSelectorWidget* productionWidget;
      

      ContainerBase* container;

      ContainerBase::Production production;
      

      bool ok()
      {
         container->setProductionLines( production );
         QuitModal();
         return true;
      }

      bool addOne()
      {
         const Vehicletype* v = allTypesFactory->getSelectedVehicleType();
         if ( !v )
            return false;
         
         if ( find( production.begin(), production.end(), v ) == production.end() ) {
            production.push_back( v );
            productionWidget->reLoad( true );
            return true;
         }
         return false;
      }
      
      bool removeOne()
      {
         const Vehicletype* v = productionFactory->getSelectedVehicleType();
         if ( !v )
            return false;

         ContainerBase::Production::iterator i = find( production.begin(), production.end(), v );
         if ( i != production.end() ) {
            production.erase( i );
            productionWidget->reLoad( true );
            return true;
         }
         return false;
      }

      
   public:
      ProductionEditorWindow ( ContainerBase* container ) : ASC_PG_Dialog ( NULL, PG_Rect( 20,20, 700, 550 ), "Unit Production" )
      {
         this->container = container;
         production = container->getProduction();
         
         const int centerSpace = 80;
         const int border  = 10;
         
         allTypesFactory = new ProductionItemFactory( container );
         allTypes = new ItemSelectorWidget( this, PG_Rect( border, 50, (my_width- centerSpace) / 2 - 2 * border, my_height - 100 ), allTypesFactory );

         productionFactory = new AvailableProductionItemFactory( container, production );
         productionWidget = new ItemSelectorWidget( this, PG_Rect( (my_width + centerSpace) / 2 + border, 50, (my_width- centerSpace) / 2 - 2 * border, my_height - 100 ), productionFactory);


         
         PG_Button* addB = new PG_Button( this, PG_Rect( (my_width - centerSpace) / 2, 100, centerSpace, 30 ), "->" );
         addB->sigClick.connect( SigC::slot( *this, &ProductionEditorWindow::addOne ));
         
         PG_Button* removeB = new PG_Button( this, PG_Rect( (my_width - centerSpace) / 2, 140, centerSpace, 30 ), "<-" );
         removeB->sigClick.connect( SigC::slot( *this, &ProductionEditorWindow::removeOne ));

         
         PG_Button* ok = new PG_Button( this, PG_Rect( my_width - 100, my_height - 40, 90, 30 ), "OK" );
         ok->sigClick.connect( SigC::slot( *this, &ProductionEditorWindow::ok ));
      };

};
      

void editProduction( ContainerBase* container )
{
   ProductionEditorWindow pew ( container );
   pew.Show();
   pew.RunModal();
}

