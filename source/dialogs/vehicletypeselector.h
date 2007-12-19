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
      const Vehicletype* vt;
      static Surface clippingSurface;
      Surface& getClippingSurface() { return clippingSurface; };
      const Player& actplayer;
      bool info();
   public:
      VehicleTypeBaseWidget( PG_Widget* parent, const PG_Point& pos, int width, const Vehicletype* vehicletype, const Player& player );
      ASCString getName() const;
      const Vehicletype* getVehicletype() const { return vt; };
   protected:
      void display( SDL_Surface * surface, const PG_Rect & src, const PG_Rect & dst );
      static int buttonXPos( int width, int num );  
};

class VehicleTypeResourceWidget: public VehicleTypeBaseWidget  {
   public:
      VehicleTypeResourceWidget( PG_Widget* parent, const PG_Point& pos, int width, const Vehicletype* vehicletype, int lackingResources, const Resources& cost, const Player& player );
};


class VehicleTypeCountWidget: public VehicleTypeBaseWidget  {
   public:
      VehicleTypeCountWidget( PG_Widget* parent, const PG_Point& pos, int width, const Vehicletype* vehicletype, const Player& player, int number );
};

class VehicleTypeCountLocateWidget: public VehicleTypeCountWidget  {
   private:
      bool locate();
   public:
      VehicleTypeCountLocateWidget( PG_Widget* parent, const PG_Point& pos, int width, const Vehicletype* vehicletype, const Player& player, int number );
      
      SigC::Signal1<void,const Vehicletype*> locateVehicles;
};


class VehicleTypeSelectionItemFactory: public SelectionItemFactory, public SigC::Object  {
      Resources plantResources;
      const Player& actplayer;
      bool showResourcesForUnit;
   public:
      typedef vector<const Vehicletype*> Container;

      static SigC::Signal1<void,const Vehicletype*> showVehicleInfo;
      
   protected:
      Container::iterator it;
      Container items;

      virtual void vehicleTypeSelected( const Vehicletype* type, bool mouse ) {};

      virtual const Container& getOriginalItems() { return original_items; };
      
   private:
      const Container& original_items;
      
   public:
      VehicleTypeSelectionItemFactory( Resources plantResources, const Container& types, const Player& player );
      VehicleTypeSelectionItemFactory( const Container& types, const Player& player );
      
      SigC::Signal0<void> reloadAllItems;

      void restart();
   
      void setAvailableResource( const Resources& plantResources ) { this->plantResources = plantResources; };
      
      virtual Resources getCost( const Vehicletype* type ) {return Resources(); };
      
      SelectionWidget* spawnNextItem( PG_Widget* parent, const PG_Point& pos );
      
      void itemSelected( const SelectionWidget* widget, bool mouse );
};


#endif
