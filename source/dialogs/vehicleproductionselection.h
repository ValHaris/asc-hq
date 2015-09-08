
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef vehicleproductionselectionH
#define vehicleproductionselectionH

#include <map>
#include "vehicletypeselector.h"
#include "../actions/constructunitcommand.h"

class VehicleProduction_SelectionItemFactory: public VehicleTypeSelectionItemFactory
{
      bool fillResources;
      bool fillAmmo;
      const ContainerBase* plant;

      const ConstructUnitCommand::Producables& produceables;

      Container* items; // will be initialized by the constructor's call to convertArrays
      
      static const Container& convertAndCreateArrays( const ConstructUnitCommand::Producables& from, Container** items );
      static const Container& convertArrays( const ConstructUnitCommand::Producables& from, Container& items );
      
   protected:
      void vehicleTypeSelected( const VehicleType* type, bool mouse );
      void itemMarked( const SelectionWidget* widget, bool mouse );

   public:
      VehicleProduction_SelectionItemFactory( Resources plantResources, const ContainerBase* productionplant, const ConstructUnitCommand::Producables& produceableUnits  );

      bool getAmmoFilling();

      bool setAmmoFilling( bool value );

      bool getResourceFilling();

      bool setResourceFilling( bool value );

      Resources getCost( const VehicleType* type );

      sigc::signal<void,const VehicleType*, bool > sigVehicleTypeSelected;
      sigc::signal<void,const VehicleType* > sigVehicleTypeMarked;
      
      void updateProducables();
      
      
      ~VehicleProduction_SelectionItemFactory()
      {
         delete items;
      }
};


class AddProductionLine_SelectionItemFactory: public VehicleTypeSelectionItemFactory
{
      ContainerBase* plant;
   public:
      AddProductionLine_SelectionItemFactory( ContainerBase* my_plant, const Container& types );

      void vehicleTypeSelected( const VehicleType* type, bool mouse );

      Resources getCost( const VehicleType* type );
};


class VehicleProduction_SelectionWindow : public ASC_PG_Dialog
{
      const VehicleType* selected;
      const VehicleType* finallySelected;
      ItemSelectorWidget* isw;
      VehicleProduction_SelectionItemFactory* factory;
      ContainerBase* my_plant;
      
      const ConstructUnitCommand::Producables& produceables;
   protected:
      void vtMarked( const VehicleType* vt );

      void vtSelected( const VehicleType* vt, bool mouse );

      bool produce();

      bool closeWindow();

      bool quitSignalled();

      void reLoadAndUpdate();

      bool eventKeyDown(const SDL_KeyboardEvent* key);

   public:
      VehicleProduction_SelectionWindow( PG_Widget *parent, const PG_Rect &r, ContainerBase* plant, const ConstructUnitCommand::Producables& produceableUnits, bool internally );
      
      void updateProducables();
      
      sigc::signal<void> reloadProducebles;
      
      bool addProductionLine();

      bool removeProductionLine();

      bool fillWithAmmo();

      bool fillWithResources();

      const VehicleType* getVehicletype() { return finallySelected; };
};


#endif
