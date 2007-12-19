/***************************************************************************
                          cargodialog.cpp  -  description
                             -------------------
    begin                : Tue Oct 24 2000
    copyright            : (C) 2000 by Martin Bickel
    email                : bickel@asc-hq.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef buildingtypeselectorH
#define buildingtypeselectorH

#include <pgimage.h>
#include "selectionwindow.h"
#include "../buildingtype.h"
#include "../paradialog.h"






class BuildingTypeBaseWidget: public SelectionWidget  {
      const BuildingType* vt;
      static Surface clippingSurface;
      Surface& getClippingSurface() { return clippingSurface; };
      const Player& actplayer;
   public:
      BuildingTypeBaseWidget( PG_Widget* parent, const PG_Point& pos, int width, const BuildingType* BuildingType, const Player& player );
      ASCString getName() const;
      const BuildingType* getBuildingType() const { return vt; };
   protected:
      void display( SDL_Surface * surface, const PG_Rect & src, const PG_Rect & dst );
      static int getBuildingHeight( const BuildingType* type );
};

class BuildingTypeResourceWidget: public BuildingTypeBaseWidget  {
   public:
      BuildingTypeResourceWidget( PG_Widget* parent, const PG_Point& pos, int width, const BuildingType* BuildingType, int lackingResources, const Resources& cost, const Player& player );
};


class BuildingTypeCountWidget: public BuildingTypeBaseWidget  {
   public:
      BuildingTypeCountWidget( PG_Widget* parent, const PG_Point& pos, int width, const BuildingType* BuildingType, const Player& player, int number );
};


class BuildingTypeSelectionItemFactory: public SelectionItemFactory, public SigC::Object  {
      Resources plantResources;
      const Player& actplayer;
   public:
      typedef vector<const BuildingType*> Container;

   protected:
      Container::iterator it;
      Container items;

      virtual void BuildingTypeSelected( const BuildingType* type ) = 0;
      
      virtual Resources getCost( const BuildingType* type );
   
   private:
      const Container& original_items;
      
   public:
      BuildingTypeSelectionItemFactory( Resources plantResources, const Container& types, const Player& player );
      
      SigC::Signal0<void> reloadAllItems;

      void restart();
   
      void setAvailableResource( const Resources& plantResources ) { this->plantResources = plantResources; };
      
      
      SelectionWidget* spawnNextItem( PG_Widget* parent, const PG_Point& pos );
      
      void itemSelected( const SelectionWidget* widget, bool mouse );
};

bool BuildingComp ( const BuildingType* v1, const BuildingType* v2 );

#endif
