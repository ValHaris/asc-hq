
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

#include "vehicletypeselector.h"

class VehicleProduction_SelectionItemFactory: public VehicleTypeSelectionItemFactory
{
      bool fillResources;
      bool fillAmmo;
      const ContainerBase* plant;

      // is initialized by the constructor's call to filterVehicleTypes
      Container* produceables;

      Container& filterVehicleTypes( const Container& vehicles, const ContainerBase* productionplant );

   protected:
      void vehicleTypeSelected( const Vehicletype* type, bool mouse );
      void itemMarked( const SelectionWidget* widget, bool mouse );
      const Container& getOriginalItems();

   public:
      VehicleProduction_SelectionItemFactory( Resources plantResources, const ContainerBase* productionplant );

      bool getAmmoFilling();

      bool setAmmoFilling( bool value );

      bool getResourceFilling();

      bool setResourceFilling( bool value );

      Resources getCost( const Vehicletype* type );

      SigC::Signal2<void,const Vehicletype*, bool > sigVehicleTypeSelected;
      SigC::Signal1<void,const Vehicletype* > sigVehicleTypeMarked;

      ~VehicleProduction_SelectionItemFactory();
};


class AddProductionLine_SelectionItemFactory: public VehicleTypeSelectionItemFactory
{
      ContainerBase* plant;
   public:
      AddProductionLine_SelectionItemFactory( ContainerBase* my_plant, const Container& types );

      void vehicleTypeSelected( const Vehicletype* type, bool mouse );

      Resources getCost( const Vehicletype* type );
};


class VehicleProduction_SelectionWindow : public ASC_PG_Dialog
{
      const Vehicletype* selected;
      const Vehicletype* finallySelected;
      ItemSelectorWidget* isw;
      VehicleProduction_SelectionItemFactory* factory;
      ContainerBase* my_plant;
   protected:
      void vtMarked( const Vehicletype* vt );

      void vtSelected( const Vehicletype* vt, bool mouse );

      bool produce();

      bool closeWindow();

      bool quitSignalled();

      void reLoadAndUpdate();

      bool eventKeyDown(const SDL_KeyboardEvent* key);

   public:
      VehicleProduction_SelectionWindow( PG_Widget *parent, const PG_Rect &r, ContainerBase* plant );

      bool addProductionLine();

      bool removeProductionLine();

      bool fillWithAmmo();

      bool fillWithResources();

      const Vehicletype* getVehicletype() { return finallySelected; };
};


#endif
