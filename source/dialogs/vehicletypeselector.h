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

#ifndef vehicletypeselectorH
#define vehicletypeselectorH

#include <pgimage.h>
#include "selectionwindow.h"
#include "../containerbase.h"
#include "../paradialog.h"




class Player;

class VehicleTypeBaseWidget: public SelectionWidget  {
      const VehicleType* vt;
      static Surface clippingSurface;
      Surface& getClippingSurface() { return clippingSurface; };
      const Player& actplayer;
      bool info();
   public:
      VehicleTypeBaseWidget( PG_Widget* parent, const PG_Point& pos, int width, const VehicleType* vehicletype, const Player& player );
      ASCString getName() const;
      const VehicleType* getVehicletype() const { return vt; };
   protected:
      void display( SDL_Surface * surface, const PG_Rect & src, const PG_Rect & dst );
      static int buttonXPos( int width, int num );  
};

class VehicleBaseWidget: public SelectionWidget  {
      const Vehicle* v;
      static Surface clippingSurface;
      Surface& getClippingSurface() { return clippingSurface; };
      const Player& actplayer;
      bool info();
   public:
      VehicleBaseWidget( PG_Widget* parent, const PG_Point& pos, int width, const Vehicle* vehicle, const Player& player );
      ASCString getName() const;

   protected:
      void display( SDL_Surface * surface, const PG_Rect & src, const PG_Rect & dst );
      static int buttonXPos( int width, int num );
};

class VehicleTypeResourceWidget: public VehicleTypeBaseWidget  {
   public:
      VehicleTypeResourceWidget( PG_Widget* parent, const PG_Point& pos, int width, const VehicleType* vehicletype, int lackingResources, const Resources& cost, const Player& player );
};


class VehicleTypeCountWidget: public VehicleTypeBaseWidget  {
   public:
      VehicleTypeCountWidget( PG_Widget* parent, const PG_Point& pos, int width, const VehicleType* vehicletype, const Player& player, int number );
};

class VehicleTypeCountLocateWidget: public VehicleTypeCountWidget  {
   private:
      bool locate();
   public:
      VehicleTypeCountLocateWidget( PG_Widget* parent, const PG_Point& pos, int width, const VehicleType* vehicletype, const Player& player, int number );
      
      sigc::signal<void,const VehicleType*> locateVehicles;
};


class VehicleTypeSelectionItemFactory: public SelectionItemFactory, public sigc::trackable  {
      Resources plantResources;
      const Player& actplayer;
      bool showResourcesForUnit;
   public:
      typedef vector<const VehicleType*> Container;

      static sigc::signal<void,const VehicleType*> showVehicleInfo;
      
   protected:
      Container::iterator it;
      Container items;

      virtual void vehicleTypeSelected( const VehicleType* type, bool mouse ) {};

   private:
      const Container& original_items;
      
   public:
      VehicleTypeSelectionItemFactory( Resources plantResources, const Container& types, const Player& player );
      VehicleTypeSelectionItemFactory( const Container& types, const Player& player );
      
      sigc::signal<void> reloadAllItems;

      void restart();
   
      void setAvailableResource( const Resources& plantResources ) { this->plantResources = plantResources; };
      
      virtual Resources getCost( const VehicleType* type ) {return Resources(); };
      
      SelectionWidget* spawnNextItem( PG_Widget* parent, const PG_Point& pos );
      
      void itemSelected( const SelectionWidget* widget, bool mouse );
};


#endif
