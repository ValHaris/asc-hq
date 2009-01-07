/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <pgtooltiphelp.h>

#include "../gamemap.h"
#include "../containercontrols.h"
#include "../cannedmessages.h"
#include "../gameoptions.h"
#include "../dialog.h"

#include "vehicleproductionselection.h"

VehicleProduction_SelectionItemFactory::Container& VehicleProduction_SelectionItemFactory::filterVehicleTypes( const Container& vehicles, const ContainerBase* productionplant )
{
   produceables = new Container();
   for ( Container::const_iterator i = vehicles.begin(); i != vehicles.end(); ++i ) {
      ContainerConstControls cc ( productionplant );
      if ( !( cc.unitProductionPrerequisites( *i ) & ~(1+2+4)))  // we are filtering out the bits for lack of resource, so that these units will still be displayed
         produceables->push_back( *i );
   }

   return *produceables;
}

void VehicleProduction_SelectionItemFactory::vehicleTypeSelected( const Vehicletype* type, bool mouse )
{
   sigVehicleTypeSelected( type, mouse );
}

void VehicleProduction_SelectionItemFactory::itemMarked( const SelectionWidget* widget, bool mouse )
{
   if ( !widget )
      return;

   const VehicleTypeBaseWidget* fw = dynamic_cast<const VehicleTypeBaseWidget*>(widget);
   assert( fw );
   sigVehicleTypeMarked( fw->getVehicletype() );
}

const VehicleProduction_SelectionItemFactory::Container& VehicleProduction_SelectionItemFactory::getOriginalItems()
{
   delete produceables;
   filterVehicleTypes( plant->getProduction(), plant );
   return *produceables;
};

VehicleProduction_SelectionItemFactory::VehicleProduction_SelectionItemFactory( Resources plantResources, const ContainerBase* productionplant )
      : VehicleTypeSelectionItemFactory( plantResources, filterVehicleTypes( productionplant->getProduction(), productionplant), productionplant->getMap()->getCurrentPlayer() ),
      fillResources(true),
      fillAmmo(true),
      plant(productionplant)
{
};


bool VehicleProduction_SelectionItemFactory::getAmmoFilling()
{
   if ( plant->baseType->hasFunction(ContainerBaseType::AmmoProduction))
      return fillAmmo;
   else
      return false;
}

bool VehicleProduction_SelectionItemFactory::setAmmoFilling( bool value )
{
   fillAmmo = value;
   reloadAllItems();
   return true;
}

bool VehicleProduction_SelectionItemFactory::getResourceFilling()
{
   return fillResources;
}

bool VehicleProduction_SelectionItemFactory::setResourceFilling( bool value )
{
   fillResources = value;
   reloadAllItems();
   return true;
}


Resources VehicleProduction_SelectionItemFactory::getCost( const Vehicletype* type )
{
   Resources cost = plant->getProductionCost( type );
   if ( fillResources )
      cost += Resources( 0, type->getStorageCapacity(plant->getMap()->_resourcemode).material, type->getStorageCapacity(plant->getMap()->_resourcemode).fuel );

   if ( fillAmmo )
      for ( int w = 0; w < type->weapons.count; ++w )
         if ( type->weapons.weapon[w].requiresAmmo() ) {
            int wt = type->weapons.weapon[w].getScalarWeaponType();
            cost += Resources( cwaffenproduktionskosten[wt][0], cwaffenproduktionskosten[wt][1], cwaffenproduktionskosten[wt][2] ) * type->weapons.weapon[w].count;
         }
   return cost;
};

VehicleProduction_SelectionItemFactory::~VehicleProduction_SelectionItemFactory()
{
   delete produceables;
}


AddProductionLine_SelectionItemFactory::AddProductionLine_SelectionItemFactory( ContainerBase* my_plant, const Container& types ) : VehicleTypeSelectionItemFactory( my_plant->getResource(Resources(maxint,maxint,maxint), true), types, my_plant->getMap()->getCurrentPlayer() ), plant(my_plant)
{

};

void AddProductionLine_SelectionItemFactory::vehicleTypeSelected( const Vehicletype* type, bool mouse )
{
   ContainerControls cc( plant );
   int res = cc.buildProductionLine( type );
   if ( res < 0 )
      errorMessage( getmessage ( -res ));
}

Resources AddProductionLine_SelectionItemFactory::getCost( const Vehicletype* type )
{
   ContainerControls cc ( plant );
   return cc.buildProductionLineResourcesNeeded( type );
};


void VehicleProduction_SelectionWindow::vtMarked( const Vehicletype* vt )
{
   vtSelected( vt, true );
}

void VehicleProduction_SelectionWindow::vtSelected( const Vehicletype* vt, bool mouse )
{
   selected = vt;

   if ( !mouse ) // enter pressed
      produce();
};

bool VehicleProduction_SelectionWindow::produce()
{
   if ( selected ) {
      finallySelected = selected;
      quitModalLoop(0);
      return true;
   } else
      return false;
}


bool VehicleProduction_SelectionWindow::closeWindow()
{
   selected = NULL;
   return ASC_PG_Dialog::closeWindow();
}

bool VehicleProduction_SelectionWindow::quitSignalled()
{
   selected = NULL;
   QuitModal();
   return true;
};

void VehicleProduction_SelectionWindow::reLoadAndUpdate()
{
   factory->setAvailableResource(my_plant->getResource(Resources(maxint,maxint,maxint)));
   isw->reLoad( true );
}

bool VehicleProduction_SelectionWindow::eventKeyDown(const SDL_KeyboardEvent* key)
{
   int mod = SDL_GetModState() & ~(KMOD_NUM | KMOD_CAPS | KMOD_MODE);
   if ( mod )
      return false;

   if ( key->keysym.sym == SDLK_ESCAPE ) {
      closeWindow();
      return true;
   }

   return false;
}


VehicleProduction_SelectionWindow::VehicleProduction_SelectionWindow( PG_Widget *parent, const PG_Rect &r, ContainerBase* plant ) : ASC_PG_Dialog( parent, r, "Choose Vehicle Type" ), selected(NULL), finallySelected(NULL), isw(NULL), factory(NULL), my_plant( plant )
{
   factory = new VehicleProduction_SelectionItemFactory( plant->getResource(Resources(maxint,maxint,maxint), true), plant );

   factory->setResourceFilling ( CGameOptions::Instance()->unitProduction.fillResources );


   factory->sigVehicleTypeSelected.connect ( SigC::slot( *this, &VehicleProduction_SelectionWindow::vtSelected ));
   factory->sigVehicleTypeMarked.connect ( SigC::slot( *this, &VehicleProduction_SelectionWindow::vtMarked ));

   isw = new ItemSelectorWidget( this, PG_Rect(10, GetTitlebarHeight(), r.Width() - 20, r.Height() - GetTitlebarHeight() - 40), factory );
   isw->sigQuitModal.connect( SigC::slot( *this, &VehicleProduction_SelectionWindow::quitSignalled));

   factory->reloadAllItems.connect( SigC::slot( *this, &VehicleProduction_SelectionWindow::reLoadAndUpdate ));


   int y = GetTitlebarHeight() + isw->Height();
   PG_CheckButton* fillRes = new PG_CheckButton( this, PG_Rect( 10, y + 2, r.Width() / 2 - 50, 20), "Fill with Resources" );


   if ( factory->getResourceFilling() )
      fillRes->SetPressed();
   fillRes->sigClick.connect( SigC::slot( *factory, &VehicleProduction_SelectionItemFactory::setResourceFilling ));

   if ( plant->baseType->hasFunction(ContainerBaseType::AmmoProduction)) {
      factory->setAmmoFilling( CGameOptions::Instance()->unitProduction.fillAmmo );
      PG_CheckButton* fillAmmo = new PG_CheckButton( this, PG_Rect( 10, y + 20, r.Width() / 2 - 50, 20), "Fill with Ammo" );
      if ( factory->getAmmoFilling() )
         fillAmmo->SetPressed();
      fillAmmo->sigClick.connect( SigC::slot( *factory, &VehicleProduction_SelectionItemFactory::setAmmoFilling ));
   } else
      factory->setAmmoFilling( false );

   PG_Rect rr ( r.Width() / 2 + 10, y + 2, (r.Width() - 20) - (r.Width() / 2 + 10) , 35);
   PG_Button* b  = new PG_Button( this, PG_Rect( rr.x + rr.h + 5, rr.y, rr.w - 40, rr.h ) , "Produce" );
   b->sigClick.connect( SigC::slot( *this,&VehicleProduction_SelectionWindow::produce ));

   if ( !plant->baseType->hasFunction(ContainerBaseType::NoProductionCustomization)) {
      PG_Button* b2 = new PG_Button( this, PG_Rect( rr.x, rr.y, rr.h, rr.h ), "+" );
      b2->sigClick.connect( SigC::slot( *this, &VehicleProduction_SelectionWindow::addProductionLine ));
      new PG_ToolTipHelp( b2, "Add production line");

      PG_Button* b3 = new PG_Button( this, PG_Rect( rr.x - rr.h - 5, rr.y, rr.h, rr.h ), "-" );
      b3->sigClick.connect( SigC::slot( *this, &VehicleProduction_SelectionWindow::removeProductionLine ));
      new PG_ToolTipHelp( b3, "Remove production line");
   }


   SetTransparency(0);
};

bool VehicleProduction_SelectionWindow::addProductionLine()
{
   ContainerControls cc ( my_plant );
   {
      ItemSelectorWindow isw( NULL, PG_Rect( 100, 150, 400, 400 ),  "choose production line", new AddProductionLine_SelectionItemFactory( my_plant, cc.productionLinesBuyable() ));
      isw.Show();
      isw.RunModal();
   }
   reLoadAndUpdate();
   return true;
}

bool VehicleProduction_SelectionWindow::removeProductionLine()
{
   if ( selected && choice_dlg("do you really want to remove this production line ?","~y~es","~n~o") == 1) {
      ContainerControls cc ( my_plant );
      cc.removeProductionLine( selected );
      reLoadAndUpdate();
   }
   return true;
}


bool VehicleProduction_SelectionWindow::fillWithAmmo()
{
   return factory->getAmmoFilling();
}

bool VehicleProduction_SelectionWindow::fillWithResources()
{
   return factory->getResourceFilling();
}

