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

#include <pgimage.h>
#include <pgtooltiphelp.h>
#include "cargodialog.h"
#include "cargowidget.h"
#include "vehicletypeselector.h"

#include "../containerbase.h"
#include "../paradialog.h"
#include "../messaginghub.h"
#include "../iconrepository.h"
#include "../vehicle.h"
#include "../gamemap.h"
#include "../containercontrols.h"

#include "../asc-mainscreen.h"
#include "../guiiconhandler.h"
#include "../guifunctions.h"
#include "../guifunctions-interface.h"
#include "../unitctrl.h"
#include "../cannedmessages.h"
#include "../replay.h"
#include "../dashboard.h"
#include "../dialog.h"
#include "../containerbase-functions.h"
#include "../resourcenet.h"

#include "selectionwindow.h"
#include "ammotransferdialog.h"

// #include "cargowidget.cpp"

const Vehicletype* selectVehicletype( ContainerBase* plant, const vector<Vehicletype*>& items );



class CargoDialog;

class SubWindow: public SigC::Object
{
   protected:
      CargoDialog* cargoDialog;
      PG_Widget* widget;
      
      SubWindow() : cargoDialog(NULL), widget(NULL) {};
      ContainerBase* container(); 
   public:
      virtual bool available( CargoDialog* cd ) = 0;
      virtual ASCString getASCTXTname() = 0;
      virtual void registerSubwindow( CargoDialog* cd );
      virtual void registerChilds( CargoDialog* cd );
      virtual void update() = 0;
      virtual ~SubWindow() {} ;
};


class SubWinButton : public PG_Button
{
   public:
      static const int buttonwidth = 47;
      static const int buttonheight = 23;

      SubWinButton( PG_Widget *parent, const SPoint& pos, SubWindow* subWindow ) : PG_Button( parent, PG_Rect( pos.x, pos.y, buttonwidth, buttonheight ), "", -1, "SubWinButton")
      {
         SetBackground( PRESSED, IconRepository::getIcon("cargo-buttonpressed.png").getBaseSurface() );
         SetBackground( HIGHLITED, IconRepository::getIcon("cargo-buttonhighlighted.png").getBaseSurface() );
         SetBackground( UNPRESSED, IconRepository::getIcon("cargo-buttonunpressed.png").getBaseSurface() );
         SetBorderSize(0,0,0);
         SetIcon( IconRepository::getIcon(ASCString("cargo-") + subWindow->getASCTXTname() + ".png" ).getBaseSurface() );
      };
};


namespace CargoGuiFunctions {
   
   class MovementDestination : public GuiFunctions::Movement {
         PG_Widget& parent;
      public:
         MovementDestination( PG_Widget& masterParent ) : parent( masterParent)  {};
         
         void execute( const MapCoordinate& pos, ContainerBase* subject, int num )
         {
            GuiFunctions::Movement::execute( pos, subject, num );
            parent.QuitModal();
         }
   };

   class CancelMovement : public GuiFunctions::Cancel {
         PG_Widget& parent;
      public:
         CancelMovement ( PG_Widget& masterParent ) : parent( masterParent)  {};
         
         void execute( const MapCoordinate& pos, ContainerBase* subject, int num )
         {
            GuiFunctions::Cancel::execute( pos, subject, num );
            parent.QuitModal();
         }
   };
   

   class Movement : public GuiFunction
   {
         CargoDialog& parent;
      public:
         Movement( CargoDialog& masterParent ) : parent( masterParent)  {};
         bool available( const MapCoordinate& pos, ContainerBase* subject, int num );
         void execute( const MapCoordinate& pos, ContainerBase* subject, int num );
         bool checkForKey( const SDL_KeyboardEvent* key, int modifier );
         Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num );
         ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num );
   };


   class UnitProduction : public GuiFunction
   {
         CargoDialog& parent;
      public:
         UnitProduction( CargoDialog& masterParent ) : parent( masterParent)  {};
         bool available( const MapCoordinate& pos, ContainerBase* subject, int num );
         void execute( const MapCoordinate& pos, ContainerBase* subject, int num );
         bool checkForKey( const SDL_KeyboardEvent* key, int modifier );
         Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num );
         ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num );
   };
   
   class UnitTraining : public GuiFunction
   {
         CargoDialog& parent;
      public:
         UnitTraining( CargoDialog& masterParent ) : parent( masterParent)  {};
         bool available( const MapCoordinate& pos, ContainerBase* subject, int num );
         void execute( const MapCoordinate& pos, ContainerBase* subject, int num );
         bool checkForKey( const SDL_KeyboardEvent* key, int modifier );
         Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num );
         ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num );
   };

   class RefuelUnit : public GuiFunction
   {
         CargoDialog& parent;
      public:
         RefuelUnit( CargoDialog& masterParent ) : parent( masterParent)  {};
         bool available( const MapCoordinate& pos, ContainerBase* subject, int num );
         void execute( const MapCoordinate& pos, ContainerBase* subject, int num );
         bool checkForKey( const SDL_KeyboardEvent* key, int modifier );
         Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num );
         ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num );
   };
   
   class RefuelUnitDialog : public GuiFunction
   {
      CargoDialog& parent;
      public:
         RefuelUnitDialog( CargoDialog& masterParent ) : parent( masterParent)  {};
         bool available( const MapCoordinate& pos, ContainerBase* subject, int num );
         void execute( const MapCoordinate& pos, ContainerBase* subject, int num );
         bool checkForKey( const SDL_KeyboardEvent* key, int modifier );
         Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num );
         ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num );
   };

   class MoveUnitUp : public GuiFunction
   {
         CargoDialog& parent;
      public:
         MoveUnitUp ( CargoDialog& masterParent ) : parent( masterParent)  {};
         bool available( const MapCoordinate& pos, ContainerBase* subject, int num );
         void execute( const MapCoordinate& pos, ContainerBase* subject, int num );
         bool checkForKey( const SDL_KeyboardEvent* key, int modifier );
         Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num );
         ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num );
   };

   class OpenContainer: public GuiFunction
   {
         CargoDialog& parent;
      public:
         OpenContainer( CargoDialog& masterParent ) : parent( masterParent)  {};
         bool available( const MapCoordinate& pos, ContainerBase* subject, int num );
         void execute( const MapCoordinate& pos, ContainerBase* subject, int num );
         bool checkForKey( const SDL_KeyboardEvent* key, int modifier );
         Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num );
         ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num );
   };
   
   
}; // namespace CargoGuiFunctions


class DamageControlWindow;

class CargoDialog : public Panel
{

      ContainerControls containerControls;
   
      GuiIconHandler guiIconHandler;
      
      ContainerBase* container;
      bool setupOK;
      Surface infoImage;

      SigC::Signal0<void>  sigCargoChanged;

      typedef vector<SubWindow*> Activesubwindows;
      Activesubwindows activesubwindows;
      
      typedef deallocating_vector<SubWindow*> Subwindows;
      Subwindows subwindows;

      CargoWidget* cargoWidget;
      SubWindow* researchWindow;
      SubWindow* matterWindow;
      DamageControlWindow* damageControlWindow;

      bool eventKeyDown(const SDL_KeyboardEvent* key)
      {
         if ( key->keysym.sym == SDLK_ESCAPE ) {
            QuitModal();
            return true;
         }
         return false;
      };



      void registerSpecialDisplay( const ASCString& name )
      {
         SpecialDisplayWidget* sdw = dynamic_cast<SpecialDisplayWidget*>( FindChild( name, true ) );
         if ( sdw )
            sdw->display.connect( SigC::slot( *this, &CargoDialog::painter ));
      };

      void registerSpecialInput( const ASCString& name )
      {
         SpecialInputWidget* siw = dynamic_cast<SpecialInputWidget*>( FindChild( name, true ) );
         if ( siw )
            siw->sigMouseButtonDown.connect( SigC::slot( *this, &CargoDialog::onClick ));
      };

      bool onClick ( PG_MessageObject* obj, const SDL_MouseButtonEvent* event )
      {
         PG_Widget* w = dynamic_cast<PG_Widget*>(obj);
         if ( w ) {
            // click( w->GetName() );
            return true;
         }
         return false;
      };

      void painter ( const PG_Rect &src, const ASCString& name, const PG_Rect &dst)
      {
         Surface screen = Surface::Wrap( PG_Application::GetScreen() );

         if ( name == "unitpad_unitsymbol" ) {
            // if ( vt )
            //   vt->paint( screen, SPoint( dst.x, dst.y ), veh ? veh->getOwner() : 0 );

         }
      };

      bool activate_i( int pane )
      {
         if ( pane >= 0 && pane < activesubwindows.size() ) {
            activate( activesubwindows[pane]->getASCTXTname() );
            activesubwindows[pane]->update();
         }
         return true;
      }

      void activate( const ASCString& pane )
      {
         PG_Application::SetBulkMode();
         for ( int i = 0; i < activesubwindows.size(); ++i )
            if ( activesubwindows[i]->getASCTXTname() != pane )
               hide( activesubwindows[i]->getASCTXTname() );

         for ( int i = 0; i < activesubwindows.size(); ++i )
            if ( activesubwindows[i]->getASCTXTname() == pane )
               show( activesubwindows[i]->getASCTXTname() );
         PG_Application::SetBulkMode(false);
         Update();
      };

      void updateResourceDisplay()
      {
         setLabelText( "energyavail", container->getResource(maxint, 0, true ) );
         setLabelText( "materialavail", container->getResource(maxint, 1, true ) );
         setLabelText( "fuelavail", container->getResource(maxint, 2, true ) );
      }


      void registerGuiFunctions( GuiIconHandler& handler )
      {
         registerCargoGuiFunctions( handler );
         handler.registerUserFunction( new CargoGuiFunctions::Movement( *this ) );
         handler.registerUserFunction( new CargoGuiFunctions::RefuelUnit( *this ) );
         handler.registerUserFunction( new CargoGuiFunctions::RefuelUnitDialog( *this ) );
         handler.registerUserFunction( new CargoGuiFunctions::UnitProduction( *this ));
         handler.registerUserFunction( new CargoGuiFunctions::UnitTraining( *this ));
         handler.registerUserFunction( new CargoGuiFunctions::MoveUnitUp( *this ));
         handler.registerUserFunction( new CargoGuiFunctions::OpenContainer( *this ));
      }

      void checkStoringPosition( Vehicle* unit )
      {
         if ( mainScreenWidget&& mainScreenWidget->getGuiHost() ) {
            mainScreenWidget->getGuiHost()->eval( container->getPosition(), unit );
         }
         
         if ( mainScreenWidget && mainScreenWidget->getUnitInfoPanel() )
            mainScreenWidget->getUnitInfoPanel()->showUnitData( unit, NULL, true );
      }

      void updateLoadMeter()
      {
         if ( container->baseType->maxLoadableWeight > 0 ) 
            setBargraphValue ( "LoadingMeter", float( container->cargoWeight()) / container->baseType->maxLoadableWeight );
      }
      
      
      
   public:


      CargoDialog (PG_Widget *parent, ContainerBase* cb );

      void addAvailableSubwin( SubWindow* w )
      {
         activesubwindows.push_back( w );
      };
      
      void cargoChanged()
      {
         //unitHighLight.setNew( unitHighLight.getMark() );
         if ( cargoWidget ) {
            cargoWidget->redrawAll();
            if ( mainScreenWidget && mainScreenWidget->getUnitInfoPanel() )
               mainScreenWidget->getUnitInfoPanel()->showUnitData( cargoWidget->getMarkedUnit(), NULL, true );
            
            checkStoringPosition( cargoWidget->getMarkedUnit() );
         }
         sigCargoChanged();
         updateResourceDisplay();
         showAmmo();
         updateLoadMeter();
      }
      
      int RunModal()
      {
         if ( setupOK )
            return Panel::RunModal();
         return 0;
      }

      bool ProcessEvent ( const SDL_Event *   event,bool   bModal = false  )
      {
         if ( !Panel::ProcessEvent( event, bModal )) {
            if ( mainScreenWidget ) {
               if ( mainScreenWidget->getGuiHost() ) {
                  if ( mainScreenWidget->getGuiHost()->ProcessEvent( event, bModal ))
                     return true;

                  if ( mainScreenWidget->getUnitInfoPanel() ) {
                     if ( mainScreenWidget->getUnitInfoPanel()->ProcessEvent( event, bModal ))
                        return true;
                  }
               }
            }

         } else
            return true;

         return false;
      }

      void showAmmo()
      {
         setLabelText( "cmmun", container->getAmmo( cwcruisemissile, maxint, true ));
         setLabelText( "minemun", container->getAmmo( cwminen, maxint, true ));
         setLabelText( "bombmun", container->getAmmo( cwbombn, maxint, true ));
         setLabelText( "lmmun", container->getAmmo( cwlargemissilen, maxint, true ));
         setLabelText( "smmun", container->getAmmo( cwsmallmissilen, maxint, true ));
         setLabelText( "mgmun", container->getAmmo( cwmachinegunn, maxint, true ));
         setLabelText( "canmun", container->getAmmo( cwcannonn, maxint, true ));
         setLabelText( "torpmun", container->getAmmo( cwtorpedon, maxint, true ));
      }

      void userHandler( const ASCString& label, PropertyReadingContainer& pc, PG_Widget* parent, WidgetParameters widgetParams );

      ContainerBase* getContainer() { return container; };
      ContainerControls& getControls() { return containerControls; };

      ~CargoDialog()
      {
         if ( setupOK )
            NewGuiHost::popIconHandler();

      }

};





void cargoDialog( ContainerBase* cb )
{
   CargoDialog cd ( NULL, cb );
   cd.Show();
   cd.RunModal();
}





class VehicleProduction_SelectionItemFactory: public VehicleTypeSelectionItemFactory  {
      bool fillResources;
      bool fillAmmo;
   protected:
      void vehicleTypeSelected( const Vehicletype* type )
      {
         sigVehicleTypeSelected( type );
      }

      void itemMarked( const SelectionWidget* widget, bool mouse )
      {
         if ( !widget )
            return;
            
         const VehicleTypeResourceWidget* fw = dynamic_cast<const VehicleTypeResourceWidget*>(widget);
         assert( fw );
         sigVehicleTypeMarked( fw->getVehicletype() );
      }

      
   public:
      VehicleProduction_SelectionItemFactory( Resources plantResources, const Container& types )
         : VehicleTypeSelectionItemFactory( plantResources, types, actmap->actplayer ), fillResources(true), fillAmmo(true)
      {
      };
      

      bool getAmmoFilling()
      {
         return fillAmmo;
      }
      
      bool setAmmoFilling( bool value )
      {
         fillAmmo = value;
         reloadAllItems();
         return true;
      }
      
      bool getResourceFilling()
      {
         return fillResources;
      }
      
      bool setResourceFilling( bool value )
      {
         fillResources = value;
         reloadAllItems();
         return true;
      }

     
      Resources getCost( const Vehicletype* type )
      {
         Resources cost = type->productionCost;
         if ( fillResources )
            cost += Resources( 0, type->getStorageCapacity(actmap->_resourcemode).material, type->getStorageCapacity(actmap->_resourcemode).fuel );

         if ( fillAmmo )
            for ( int w = 0; w < type->weapons.count; ++w )
               if ( type->weapons.weapon[w].requiresAmmo() ) {
                  int wt = type->weapons.weapon[w].getScalarWeaponType();
                  cost += Resources( cwaffenproduktionskosten[wt][0], cwaffenproduktionskosten[wt][1], cwaffenproduktionskosten[wt][2] );
               }
         return cost;
      };

      SigC::Signal1<void,const Vehicletype* > sigVehicleTypeSelected;
      SigC::Signal1<void,const Vehicletype* > sigVehicleTypeMarked;
      
};


class AddProductionLine_SelectionItemFactory: public VehicleTypeSelectionItemFactory  {
      ContainerBase* plant;
   public:
      AddProductionLine_SelectionItemFactory( ContainerBase* my_plant, const Container& types ) : VehicleTypeSelectionItemFactory( my_plant->getResource(Resources(maxint,maxint,maxint), true), types, actmap->actplayer ), plant(my_plant)
      {
         
      };
      
      void vehicleTypeSelected( const Vehicletype* type )
      {
         ContainerControls cc( plant );
         int res = cc.buildProductionLine( type );
         if ( res < 0 )
            errorMessage( getmessage ( res ));
      }
      
      Resources getCost( const Vehicletype* type )
      {
         ContainerControls cc ( plant );
         return cc.buildProductionLineResourcesNeeded( type );
      };

};


class VehicleProduction_SelectionWindow : public ASC_PG_Dialog {
      const Vehicletype* selected;
      ItemSelectorWidget* isw;
      VehicleProduction_SelectionItemFactory* factory;
      ContainerBase* my_plant;
   protected:
      void vtSelected( const Vehicletype* filename )
      {
         selected = filename;
         // quitModalLoop(0);
      };

      bool produce()
      {
         if ( selected ) {
            quitModalLoop(0);
            return true;
         } else
            return false;
      }

      void reLoadAndUpdate()
      {
         isw->reLoad( true );
      }
      
   public:
      VehicleProduction_SelectionWindow( PG_Widget *parent, const PG_Rect &r, ContainerBase* plant, const vector<Vehicletype*>& items ) : ASC_PG_Dialog( parent, r, "Choose Vehicle Type" ), selected(NULL), isw(NULL), factory(NULL), my_plant( plant )
      {
         factory = new VehicleProduction_SelectionItemFactory( plant->getResource(Resources(maxint,maxint,maxint), true), items );
         factory->sigVehicleTypeSelected.connect ( SigC::slot( *this, &VehicleProduction_SelectionWindow::vtSelected ));
         factory->sigVehicleTypeMarked.connect ( SigC::slot( *this, &VehicleProduction_SelectionWindow::vtSelected ));

         isw = new ItemSelectorWidget( this, PG_Rect(10, GetTitlebarHeight(), r.Width() - 20, r.Height() - GetTitlebarHeight() - 40), factory );
         isw->sigQuitModal.connect( SigC::slot( *this, &ItemSelectorWindow::QuitModal));

         factory->reloadAllItems.connect( SigC::slot( *this, &VehicleProduction_SelectionWindow::reLoadAndUpdate ));
         
         
         int y = GetTitlebarHeight() + isw->Height();
         PG_CheckButton* fillRes = new PG_CheckButton( this, PG_Rect( 10, y + 2, r.Width() / 2 - 50, 20), "Fill with Resources" );
         fillRes->SetPressed();
         fillRes->sigClick.connect( SigC::slot( *factory, &VehicleProduction_SelectionItemFactory::setResourceFilling ));
         
         PG_CheckButton* fillAmmo = new PG_CheckButton( this, PG_Rect( 10, y + 20, r.Width() / 2 - 50, 20), "Fill with Ammo" );
         fillAmmo->SetPressed();
         fillAmmo->sigClick.connect( SigC::slot( *factory, &VehicleProduction_SelectionItemFactory::setAmmoFilling ));

         PG_Rect rr ( r.Width() / 2 + 10, y + 2, (r.Width() - 20) - (r.Width() / 2 + 10) , 35);
         PG_Button* b  = new PG_Button( this, PG_Rect( rr.x + rr.h + 5, rr.y, rr.w - 40, rr.h ) , "Produce" );
         b->sigClick.connect( SigC::slot( *this,&VehicleProduction_SelectionWindow::produce ));
         
         PG_Button* b2 = new PG_Button( this, PG_Rect( rr.x, rr.y, rr.h, rr.h ), "+" );
         b2->sigClick.connect( SigC::slot( *this, &VehicleProduction_SelectionWindow::addProductionLine ));
         new PG_ToolTipHelp( b2, "Add production line");

         PG_Button* b3 = new PG_Button( this, PG_Rect( rr.x - rr.h - 5, rr.y, rr.h, rr.h ), "-" );
         b3->sigClick.connect( SigC::slot( *this, &VehicleProduction_SelectionWindow::removeProductionLine ));
         new PG_ToolTipHelp( b3, "Remove production line");

         
         SetTransparency(0);
      };

      bool addProductionLine()
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

      bool removeProductionLine()
      {
         if ( selected && choice_dlg("do you really want to remove this production line ?","~y~es","~n~o") == 1) {
            ContainerControls cc ( my_plant );
            cc.removeProductionLine( selected );
            reLoadAndUpdate();
         }
         return true;
      }

      
      bool fillWithAmmo()
      {
         return factory->getAmmoFilling();
      }

      bool fillWithResources()
      {
         return factory->getResourceFilling();
      }
      

      const Vehicletype* getVehicletype() { return selected; };
};


/*
const Vehicletype* selectVehicletype( ContainerBase* plant, const vector<Vehicletype*>& items )
{
   VehicleTypeSelectionWindow fsw( NULL, PG_Rect( 10, 10, 400, 500 ), plant, items );
   fsw.Show();
   fsw.RunModal();
   const Vehicletype* v = fsw.getVehicletype();
   return v;
}

*/


void SubWindow::registerSubwindow( CargoDialog* cd )
{
   cargoDialog = cd;
   cd->addAvailableSubwin( this );
}

void SubWindow::registerChilds( CargoDialog* cd )
{
   widget = cd->FindChild( getASCTXTname(), true);
   if ( !widget )
      warning( "Could not find widget with name " + getASCTXTname() );
}


ContainerBase* SubWindow::container()
{
   return cargoDialog->getContainer();
}

/*
const int subWindowNum = 11;
static const char* subWindowName[subWindowNum] =
{ "ammotransfer", "ammoproduction", "info", "cargoinfo", "conventionalpower", "mining", "netcontrol", "research", "resourceinfo", "solarpower", "windpower"
};
*/



class SolarPowerWindow : public SubWindow {
  
   public:
      bool available( CargoDialog* cd )
      {
         return cd->getContainer()->baseType->hasFunction( ContainerBaseType::SolarPowerPlant  );
      };
      
      ASCString getASCTXTname()
      {
         return "solarpower";
      };
      
      void update()
      {
         cargoDialog->setLabelText( "MaxPower", container()->maxplus.energy, widget );
      
         SolarPowerplant solarPowerPlant ( container() );
         Resources plus = solarPowerPlant.getPlus();
         cargoDialog->setLabelText( "CurrentPower", plus.energy, widget );
      }
};

class WindPowerWindow : public SubWindow {
   public:
      bool available( CargoDialog* cd )
      {
         return cd->getContainer()->baseType->hasFunction( ContainerBaseType::WindPowerPlant  );
      };
      
      ASCString getASCTXTname()
      {
         return "windpower";
      };
      
      void update()
      {
         cargoDialog->setLabelText( "MaxPower", container()->maxplus.energy, widget );
      
         WindPowerplant windPowerPlant ( container() );
         Resources plus = windPowerPlant.getPlus();
         cargoDialog->setLabelText( "CurrentPower", plus.energy, widget );
      }
};

class DamageControlWindow : public SubWindow {
   public:
      SigC::Signal0<void> damageChanged;

   
      void registerChilds( CargoDialog* cd )
      {
         SubWindow::registerChilds( cd );

         if ( widget ) {
            PG_Button* b = dynamic_cast<PG_Button*>( widget->FindChild( "RepairButton", true ) );
            if ( b )
               b->sigClick.connect( SigC::slot( *this, &DamageControlWindow::repair ));
         }
      }

      bool repair()
      {
         container()->repairItem(container(), 0 );
         
         damageChanged();
         update();
         
         if  ( widget )
            widget->Update();
         
         return true;
      }
            
      
      bool available( CargoDialog* cd )
      {
         Building* bld = dynamic_cast<Building*>(cd->getContainer() );
         if ( bld && bld->damage > 0 )
            return true;
         else
            return false;
      };
      
      ASCString getASCTXTname()
      {
         return "damagecontrol";
      };
      
      void update()
      {
         Resources res;
         container()->getMaxRepair ( container(), 0, res  );
         cargoDialog->setLabelText( "EnergyCost", res.energy, widget );
         cargoDialog->setLabelText( "MaterialCost", res.material, widget );
         cargoDialog->setLabelText( "FuelCost", res.fuel, widget );
      }
};


class ResourceInfoWindow : public SubWindow {

   int  getvalue ( int resourcetype, int y, int scope )
   {
      switch ( y ) {
         case 0:
         {  // avail
            GetResource gr ( container()->getMap() );
            return gr.getresource ( container()->getPosition().x, container()->getPosition().y, resourcetype, maxint, 1, container()->getMap()->actplayer, scope );
         }
         case 1:
         {  // tank
            GetResourceCapacity grc ( container()->getMap() );
            return grc.getresource ( container()->getPosition().x, container()->getPosition().y, resourcetype, maxint, 1, container()->getMap()->actplayer, scope );
         }
         case 2:
         {  // plus
            GetResourcePlus grp ( container()->getMap() );
            return grp.getresource ( container()->getPosition().x, container()->getPosition().y, resourcetype, container()->getMap()->actplayer, scope );
         }
         case 3:
         {  // usage
            GetResourceUsage gru( container()->getMap() );
            return gru.getresource ( container()->getPosition().x, container()->getPosition().y, resourcetype, container()->getMap()->actplayer, scope );
         }
      } /* endswitch */
      return -1;
   }
   
   public:
      bool available( CargoDialog* cd )
      {
         return dynamic_cast<Building*>( cd->getContainer() ) != NULL;
      };
      
      ASCString getASCTXTname()
      {
         return "resourceinfo";
      };
      
      void update()
      {
         int value[3][3][4];
         int mx = 0;

         for ( int c = 0; c < 3; c++ )
            for ( int x = 0; x < 3; x++ )
               for ( int y = 0; y < 4; y++ ) {
                  value[c][x][y] = getvalue ( x, y, c );
                  if ( y != 1 )
                     if ( value[c][x][y] > mx )
                        mx = value[c][x][y];
               }



         for ( int c = 0; c < 3; c++ )
            for ( int x = 0; x < 3; x++ )
               for ( int y = 0; y < 4; y++ ) {
                  char xx = 'A' + (c * 3 + x);
                  char yy = '1' + y;
                  ASCString label = "Res";
                  label += xx;
                  label += yy;
                  cout << label << "\n";
                  if ( (y != 1 || value[c][x][y] < mx*10 || value[c][x][y] < 1000000000 ) && ( !actmap->isResourceGlobal(x) || y!=0 ||c ==2))   // don't show extremely high numbers
                     cargoDialog->setLabelText( label, value[c][x][y] );
                  else
                     cargoDialog->setLabelText( label, "-" );
               }
      }
      
};


class GraphWidget : public PG_Widget {
      map<int,int> verticalLines;
      vector<int> curves;
      typedef vector< pair<int,int> > Bars;
      Bars bars;
   protected:
      int xrange;
      int yrange;
      virtual int getPoint( int curve, int x ) { return 0; };
      virtual int getBarHeight( int bar ) { return 0; };
      virtual void click( int x, int button ) {};

      bool   eventMouseMotion (const SDL_MouseMotionEvent *motion)
      {
         PG_Point p = ScreenToClient ( motion->x, motion->y );
         if ( motion->type == SDL_MOUSEMOTION && (motion->state == SDL_BUTTON(1)))
            click ( p.x, 1 );
         
         if ( motion->type == SDL_MOUSEMOTION && (motion->state == SDL_BUTTON(3)))
            click( p.x, 3 );
         
         return true;
      }
      
      bool   eventMouseButtonDown (const SDL_MouseButtonEvent *button)
      {
         PG_Point p = ScreenToClient ( button->x, button->y );
         if ( button->type == SDL_MOUSEBUTTONDOWN && (button->button == SDL_BUTTON_LEFT))
            click ( p.x, 1 );
         
         if ( button->type == SDL_MOUSEBUTTONDOWN && (button->button == SDL_BUTTON_RIGHT))
            click ( p.x, 3 );
         
         return true;
      }

      int mapColor( int col )
      {
         PG_Color color = col;
         return color.MapRGBA( PG_Application::GetScreen()->format, 255-GetTransparency());
      }
      
   public:
      GraphWidget( PG_Widget *parent, const PG_Rect& rect ) : PG_Widget( parent, rect ), xrange(1), yrange(1) {};
      void setRange( int x, int y )
      {
         xrange = max( x, 1);
         yrange = max( y, 1);
      }

      int addCurve( int color )
      {
         curves.push_back( color );
         return curves.size();
      }

      int addBar( int pos, int color )
      {
         bars.push_back ( make_pair( pos, color ));
         return bars.size();
      }
      
      
      void addVerticalLine ( int x, int color )
      {
         verticalLines[x] = color;
      }

      void clearVerticalLines()
      {
         verticalLines.clear();
      }
      
      void eventBlit (SDL_Surface *surface, const PG_Rect &src, const PG_Rect &dst)
      {
         Surface s = Surface::Wrap( PG_Application::GetScreen() );
         
         for ( int c = 0; c < curves.size(); ++c ) {
            int realcol = mapColor( curves[c] );

            for ( int x = 0; x < Width(); ++x ) {
               int y = getPoint( c, x * xrange / Width() ) * Height() / yrange;
               if ( y < 0 )
                  y = 0;
               if ( y >= Height() )
                  y = Height() -1 ;
               PG_Point pos = ClientToScreen( x, Height() - y );
               s.SetPixel( pos.x, pos.y, realcol );
            }
         }

         for ( map<int,int>::iterator v = verticalLines.begin(); v != verticalLines.end(); ++v ) {
            int realcol = mapColor( v->second );
            
            int x = v->first * Width() / xrange;
            for ( int y = 0; y < Height(); ++y ) {
               PG_Point pos = ClientToScreen( x, y );
               s.SetPixel( pos.x, pos.y, realcol );
            }
         }
         int barNum = 0;
         for ( Bars::iterator b = bars.begin(); b != bars.end(); ++b ) {
            int x = b->first * Width() / xrange;
            int x2 = (b->first + 1) * Width() / xrange - 1;
            if ( x2 <= x )
               x2 = x + 1;

            int y = getBarHeight( barNum ) * Height() / yrange;
            if ( y < 0 )
               y = 0;
            if ( y >= Height() )
               y = Height() -1 ;
            
            PG_Point pos = ClientToScreen( x, Height() - y );
            paintFilledRectangle<4>( s, SPoint( pos.x, pos.y), x2-x, y, ColorMerger_ColoredOverwrite<4>( mapColor( b->second ) ) );

            ++barNum;
         }
      };
};

class ResearchGraph : public GraphWidget {
      ContainerBase* cont;
   protected:
      int getPoint( int curve, int x )
      {
         Resources cost = returnResourcenUseForResearch( cont, x );
         return cost.energy;
      }


      void addSecondaryLab( ContainerBase* lab )
      {
         if ( lab->baseType->nominalresearchpoints && lab->baseType->hasFunction( ContainerBaseType::Research  ) ) {
            int rel = lab->researchpoints * cont->baseType->nominalresearchpoints / lab->baseType->nominalresearchpoints;
            addVerticalLine( rel, 0xd9d9d9 );
         }
      }
      
      void recalc()
      {
         clearVerticalLines();

         Player& player = cont->getMap()->player[ cont->getOwner() ];
         for ( Player::BuildingList::iterator i = player.buildingList.begin(); i != player.buildingList.end(); ++i )
            addSecondaryLab( *i );
         
         for ( Player::VehicleList::iterator i = player.vehicleList.begin(); i != player.vehicleList.end(); ++i )
            addSecondaryLab( *i );
         
         addVerticalLine( cont->researchpoints, 0xd5d200 );
      }

      bool setResearch ( ContainerBase* lab, int x )
      {
         if ( cont->baseType->nominalresearchpoints ) {
            int res = x * xrange / Width() * lab->baseType->nominalresearchpoints / cont->baseType->nominalresearchpoints;
            int old = lab->researchpoints;
            lab->researchpoints = res;
            return res != old;
         } else
            return false;
      }


      void click( int x, int button )
      {
         setResearch( x, button == 3 );
      }
      
      void setResearch( int x, bool global = false )
      {
         if ( global ) {
            Player& player = cont->getMap()->player[ cont->getOwner() ];
            for ( Player::BuildingList::iterator i = player.buildingList.begin(); i != player.buildingList.end(); ++i )
               setResearch( *i, x );
         
            for ( Player::VehicleList::iterator i = player.vehicleList.begin(); i != player.vehicleList.end(); ++i )
               setResearch( *i, x );
         }
         
         if ( setResearch( cont, x ) || global ) {
            sigChange();
            recalc();
            Update();
         }
      }
      
   public:
      ResearchGraph( PG_Widget *parent, const PG_Rect& rect, ContainerBase* container ) : GraphWidget( parent, rect ), cont( container )
      {
         setRange( cont->maxresearchpoints, returnResourcenUseForResearch( cont, cont->maxresearchpoints ).energy );
         addCurve( 0x00ff00 );
         recalc();
      }

      SigC::Signal0<void> sigChange;

};


class ResearchWindow : public SubWindow {
   public:
      bool available( CargoDialog* cd )
      {
         return cd->getContainer()->baseType->hasFunction( ContainerBaseType::Research  );
      };
      
      ASCString getASCTXTname()
      {
         return "research";
      };
      
      void update()
      {
         Player& player = container()->getMap()->player[ container()->getOwner() ];
               
         cargoDialog->setLabelText( "ResPerTurnLocal", container()->researchpoints, widget );
         cargoDialog->setLabelText( "ResPerTurnGlobal", player.research.getResearchPerTurn(), widget );

         Resources cost = returnResourcenUseForResearch( container() );
         cargoDialog->setLabelText( "CostLocal", cost.energy, widget );

         Resources globalCost;
         for ( Player::BuildingList::iterator i = player.buildingList.begin(); i != player.buildingList.end(); ++i )
            globalCost += returnResourcenUseForResearch( *i );
         
         for ( Player::VehicleList::iterator i = player.vehicleList.begin(); i != player.vehicleList.end(); ++i )
            globalCost += returnResourcenUseForResearch( *i );
         
         cargoDialog->setLabelText( "CostGlobal", globalCost.energy, widget );
         cargoDialog->setLabelText( "AvailGlobal", player.research.currentTechAvailableIn(), widget );
         
         if ( player.research.activetechnology )
            cargoDialog->setLabelText( "CurrentTech", player.research.activetechnology->name, widget );
         else
            cargoDialog->setLabelText( "CurrentTech", "-", widget );
      }
};



class MatterAndMiningBaseWindow : public SubWindow {
   PG_Slider* slider;
   bool first;

   protected:
      virtual bool invertSlider() { return false; };
   private:
   
   bool scrollTrack( long pos )
   {
      if ( invertSlider() )
         pos = 100 - pos;
      setnewpower( pos );
      update();
      return true;
   };

   void setnewpower ( ContainerBase* c, int power )
   {
      if ( hasFunction( c ) ) {
         for ( int r = 0; r < 3; r++ )
            c->plus.resource(r) = c->maxplus.resource(r) * power/100;
      
         logtoreplayinfo( rpl_setResourceProcessingAmount, c->getPosition().x, c->getPosition().y, c->plus.energy, c->plus.material, c->plus.fuel );
      }
   }
      
   void setnewpower ( int pwr )
   {
      if ( pwr < 0 )
         pwr = 0;

      if ( pwr > 100 )
         pwr = 100;
         
      bool allbuildings = false;
         
      Player& player = container()->getMap()->player[ container()->getOwner() ];
         
      if ( allbuildings ) {
         for ( Player::BuildingList::iterator bi = player.buildingList.begin(); bi != player.buildingList.end(); bi++ )
            setnewpower( *bi, pwr );
         for ( Player::VehicleList::iterator bi = player.vehicleList.begin(); bi != player.vehicleList.end(); bi++ )
            setnewpower( *bi, pwr );
      } else {
         setnewpower( container(), pwr );
      }
   }

   virtual bool hasFunction( const ContainerBase* container ) = 0;
   
   bool available( CargoDialog* cd )
   {
      return hasFunction( cd->getContainer() );
   };
   
      
   public:
      MatterAndMiningBaseWindow () : slider(NULL), first(true) {};
      
      void update()
      {
         if ( widget )
            slider = dynamic_cast<PG_Slider*>( widget->FindChild( "PowerSlider", true ));
         if ( first && slider ) {
            first = false;
            slider->SetRange( 0, 100 );
            slider->sigScrollPos.connect( SigC::slot( *this, &MatterAndMiningBaseWindow::scrollTrack ));
            slider->sigScrollTrack.connect( SigC::slot( *this, &MatterAndMiningBaseWindow::scrollTrack ));

            for ( int r = 0; r < 3; ++r )
               if ( container()->maxplus.resource(r) ) {
                  slider->SetPosition( 100 * container()->plus.energy / container()->maxplus.energy );
                  break;
               }
         }

         for ( int r = 0; r < 3; ++r ) {
            ASCString s = Resources::name(r);
            int amount = container()->plus.resource(r);
            if ( container()->maxplus.resource(r) < 0 ) {
               s += "In";
               amount  = -amount;
            } else {
               s += "Out";
            }
            if ( container()->maxplus.resource(r) != 0 )
               cargoDialog->setLabelText( s, amount, widget );
         }
      }
};

class MatterConversionWindow : public MatterAndMiningBaseWindow {
   protected:
      bool hasFunction( const ContainerBase* container )
      {
         return container->baseType->hasFunction( ContainerBaseType::MatterConverter );
      };
   public:
      
      ASCString getASCTXTname()
      {
         return "conventionalpower";
      };
      
};



class MiningGraph : public GraphWidget {
      ContainerBase* cont;
      GetMiningInfo::MiningInfo mininginfo;
   protected:
      
      int getBarHeight( int bar )
      {
         int r = (bar % 4) - 1;
         switch ( bar % 4 ) {
            case 0:
            case 1:
               return 100;
            case 2:
            case 3:
               return 100 * mininginfo.avail[bar/4].resource(r) / mininginfo.max[bar/4].resource(r);
         };
         return 0;
      };
      
   public:
      MiningGraph( PG_Widget *parent, const PG_Rect& rect, ContainerBase* container ) : GraphWidget( parent, rect ), cont( container )
      {
         setRange( (maxminingrange+1)*3+1, 110 );

         GetMiningInfo gmi ( container );
         mininginfo = gmi.getMiningInfo();

         int neutralColor = 0x666666;

         for ( int i = 0; i <= min( mininginfo.nextMiningDistance + 1, maxminingrange); ++i ) {
            addBar( i * 3, neutralColor );
            addBar( i * 3+1, neutralColor );
            addBar( i * 3, Resources::materialColor );
            addBar( i * 3+1, Resources::fuelColor );
         }
         
      }
};


class MiningWindow : public MatterAndMiningBaseWindow {
   protected:
      bool hasFunction( const ContainerBase* container )
      {
         return container->baseType->hasFunction( ContainerBaseType::MiningStation );
      };
      
      bool invertSlider() { return true; };
      
   public:
     
      ASCString getASCTXTname()
      {
         return "mining";
      };
      
      void update()
      {
         MatterAndMiningBaseWindow::update();
      }
};



class DamageBarWidget : public PG_ThemeWidget {
   private:
      DI_Color color;
      ContainerBase* container;
   public:
      DamageBarWidget (PG_Widget *parent, const PG_Rect &rect, DI_Color color, ContainerBase* container ) : PG_ThemeWidget( parent, rect, false )
      {
         this->container = container;
         this->color = color;
      };


      void repaint()
      {
         Update();
      }
      
      void eventBlit (SDL_Surface *surface, const PG_Rect &src, const PG_Rect &dst)
      {
         PG_Rect r = dst;
         r.w = (100 - container->damage ) * dst.w / 100;

         Resources cost;
         int w2 = (100 - container->damage + container->repairableDamage() ) * dst.w / 100;

         Uint32 c = color.MapRGBA( PG_Application::GetScreen()->format, 255-GetTransparency());
         if ( w > 0 ) {
            SDL_FillRect(PG_Application::GetScreen(), &r, c );
         }

         if ( w2 > 0 ) {
            int h = dst.h / 4;
            Surface s = Surface::Wrap( PG_Application::GetScreen() );
            ColorMerger_Set<4> cm ( c );
            drawLine<4> ( s, cm, SPoint(dst.x, dst.y + h), SPoint( dst.x + w2, dst.y + h));
            drawLine<4> ( s, cm, SPoint(dst.x, dst.y + dst.h - h), SPoint( dst.x + w2, dst.y + dst.h - h));
            drawLine<4> ( s, cm, SPoint( dst.x + w2, dst.y), SPoint( dst.x + w2, dst.y + dst.h));
         }
      
/*         Uint32 c = color.MapRGBA( PG_Application::GetScreen()->format, 255-GetTransparency());
         for ( Colors::iterator i = colors.begin(); i != colors.end(); ++i)
            if ( fraction < i->first ) {
            PG_Color col = i->second;
            c = col.MapRGBA( PG_Application::GetScreen()->format, 255-GetTransparency());
            }
*/      
      
      
      }

};




//*****************************************************************************************************
//*****************************************************************************************************
//
//  Cargo Dialog
//
//*****************************************************************************************************
//*****************************************************************************************************



CargoDialog ::CargoDialog (PG_Widget *parent, ContainerBase* cb )
   : Panel( parent, PG_Rect::null, "cargodialog", false ), containerControls( cb ), container(cb), setupOK(false), cargoWidget(NULL), researchWindow( NULL ), matterWindow(NULL)
{
   sigClose.connect( SigC::slot( *this, &CargoDialog::QuitModal ));

   registerGuiFunctions( guiIconHandler );

   subwindows.push_back( new SolarPowerWindow );
   subwindows.push_back( new WindPowerWindow );
   subwindows.push_back( new MiningWindow );
   subwindows.push_back( new ResourceInfoWindow );

   damageControlWindow = new DamageControlWindow;
   subwindows.push_back( damageControlWindow );
   
   researchWindow = new ResearchWindow;
   subwindows.push_back( researchWindow );
   
   matterWindow = new MatterConversionWindow;
   subwindows.push_back ( matterWindow );
   
   for ( Subwindows::iterator i = subwindows.begin(); i != subwindows.end(); ++i )
      if ( (*i)->available( this ))
         (*i)->registerSubwindow( this );
   
         

         // cb->resourceChanged.connect( SigC::slot( *this, &CargoDialog::updateResourceDisplay ));
         // cb->ammoChanged.connect( SigC::slot( *this, &CargoDialog::showAmmo ));

         
   try {
      if ( !setup() )
         return;
   } catch ( ParsingError err ) {
      errorMessage( err.getMessage() );
      return;
   } catch ( ... ) {
      errorMessage( "unknown exception" );
      return;
   }

   

   for ( Subwindows::iterator i = subwindows.begin(); i != subwindows.end(); ++i ) {
      if ( (*i)->available( this ))
         (*i)->registerChilds( this );
      hide( (*i)->getASCTXTname() );
   }


   PG_Widget* unitScrollArea = FindChild( "UnitScrollArea", true );
   if ( unitScrollArea ) {
      cargoWidget = new CargoWidget( unitScrollArea, PG_Rect( 1, 1, unitScrollArea->Width() -2 , unitScrollArea->Height() -2 ), cb );
      cargoWidget->unitMarked.connect( SigC::slot( *this, &CargoDialog::checkStoringPosition ));
   }
               

   if ( !cb->baseType->infoImageFilename.empty() && exist( cb->baseType->infoImageFilename )) {
      PG_Image* img = dynamic_cast<PG_Image*>(FindChild( "container_3dpic", true ));
      if ( img ) {
         tnfilestream stream ( cb->baseType->infoImageFilename, tnstream::reading );
         infoImage.readImageFile( stream );
         img->SetDrawMode( PG_Draw::STRETCH );
         img->SetImage( infoImage.getBaseSurface(), false );
         img->SizeWidget( img->GetParent()->w, img->GetParent()->h );
      }
   }


   setLabelText( "UnitName", cb->getName() );
   if ( cb->getName() != cb->baseType->name )
      setLabelText( "UnitClass", cb->baseType->name );

   showAmmo();
         /*
   registerSpecialDisplay( "unitpad_unitsymbol");
   registerSpecialDisplay( "unitpad_weapon_diagram");
   registerSpecialDisplay( "unitpad_transport_transporterlevel");
   registerSpecialDisplay( "unitpad_transport_unitlevel");
   registerSpecialDisplay( "unitpad_transport_leveldisplay");
         */

   updateResourceDisplay();
   updateLoadMeter();

   activate_i(0);
   Show();
   NewGuiHost::pushIconHandler( &guiIconHandler );
   setupOK = true;

};


void CargoDialog::userHandler( const ASCString& label, PropertyReadingContainer& pc, PG_Widget* parent, WidgetParameters widgetParams )
{
   if ( label == "ButtonPanel" ) {
      int x = 0;
      for ( int i = 0; i < activesubwindows.size(); ++i ) {
         SubWinButton* button = new SubWinButton( parent, SPoint( x, 0 ), activesubwindows[i] );
         button->sigClick.connect( SigC::bind( SigC::slot( *this, &CargoDialog::activate_i  ), i));
         x += SubWinButton::buttonwidth;
      }
   }

   if ( label == "ResourceTable" ) {
      int gap = 2;
      int cellwidth = (parent->Width() - 8 * gap) / 9;
      int cellHeight = (parent->Height() - 3 * gap ) / 4;
      for ( int x = 0; x < 9; ++x )
         for ( int y = 0; y < 4; ++y ) {
            PG_Rect r ( x * parent->Width() / 9, y * parent->Height() / 4, cellwidth, cellHeight );
            PG_Label* l = new PG_Label ( parent, r, PG_NULLSTR );
            widgetParams.assign( l );
            ASCString label = "Res";
            char xx = 'A' + x;
            char yy = '1' + y;
            label += xx;
            label += yy;
            l->SetName( label );
         }
   }

   if ( label == "ResearchGraph" ) {
      ResearchGraph* graph = new ResearchGraph( parent, PG_Rect( 0, 0, parent->Width(), parent->Height() ), container );
      graph->sigChange.connect( SigC::slot( *researchWindow, &SubWindow::update ));
   }
   
   if ( label == "MiningGraph" ) 
      new MiningGraph( parent, PG_Rect( 0, 0, parent->Width(), parent->Height() ), container );
   
   if ( label == "DamageBar" ) {
      int color;
      pc.addInteger("Color", color );
      DamageBarWidget* dbw = new DamageBarWidget( parent, PG_Rect( 0, 0, parent->Width(), parent->Height() ), color, container );
      damageControlWindow->damageChanged.connect( SigC::slot( *dbw, &DamageBarWidget::repaint ));
   }
            /*
   int yoffset = 0;
   for ( int i = 0; i < vt->heightChangeMethodNum; ++i ) {
   int srcLevelCount = 0;
   for ( int j = 0; j < 8; ++j )
   if ( vt->height & vt->heightChangeMethod[i].startHeight & (1 << j))
   ++srcLevelCount;

   pc.openBracket( "LineWidget" );
   PG_Rect r = parseRect( pc, parent);
   r.y += yoffset;
   r.my_height *= (srcLevelCount-1) / 3 + 1;
   widgetParams.runTextIO( pc );

   SpecialInputWidget* sw = new SpecialInputWidget ( parent, r );
   parsePanelASCTXT( pc, sw, widgetParams );
   pc.closeBracket();
   yoffset += sw->Height();



   int counter = 0;
   for ( int j = 0; j < 8; ++j )
   if ( vt->height & vt->heightChangeMethod[i].startHeight & (1 << j))  {
   ASCString filename = "height-a" + ASCString::toString(j) + ".png";
   int xoffs = 3 + IconRepository::getIcon(filename).w() * (counter % 3 );
   int yoffs = 2 + IconRepository::getIcon(filename).h() * (counter / 3 );
   new PG_Image( sw, PG_Point( xoffs, yoffs ), IconRepository::getIcon(filename).getBaseSurface(), false );
   ++counter;
}

   ASCString delta = ASCString::toString( vt->heightChangeMethod[i].heightDelta );
   if ( vt->heightChangeMethod[i].heightDelta > 0 )
   delta = "+" + delta;
   setLabelText( "unitpad_move_changeheight_change", delta, sw );
                  
   setLabelText( "unitpad_move_changeheight_movepoints", vt->heightChangeMethod[i].moveCost, sw );
   setLabelText( "unitpad_move_changeheight_distance", vt->heightChangeMethod[i].dist, sw );
}
            */
         
};


//*****************************************************************************************************
//*****************************************************************************************************
//
//  GUI FUNCTIONS
//
//*****************************************************************************************************
//*****************************************************************************************************






namespace CargoGuiFunctions {
   

   bool Movement::checkForKey( const SDL_KeyboardEvent* key, int modifier )
   {
      return ( key->keysym.sym == SDLK_SPACE );
   };

   Surface& Movement::getImage( const MapCoordinate& pos, ContainerBase* subject, int num )
   {
      return IconRepository::getIcon("movement.png");
   };
   ASCString Movement::getName( const MapCoordinate& pos, ContainerBase* subject, int num )
   {
      return "move unit";
   };

   bool Movement::available( const MapCoordinate& pos, ContainerBase* subject, int num )
   {
      if ( pendingVehicleActions.actionType == vat_nothing ) {
         Vehicle* unit = dynamic_cast<Vehicle*>(subject);
         if ( unit && unit->getOwner() == unit->getMap()->actplayer )
            return unit->canMove();

      }
      return false;
      
   }

   void Movement::execute( const MapCoordinate& pos, ContainerBase* subject, int num )
   {
      if ( !mainScreenWidget )
         return;
         
      Vehicle* unit = dynamic_cast<Vehicle*>(subject);
      if ( !unit )
         return;
         
      bool simpleMode = false;
      if (  skeypress( ct_lshift ) ||  skeypress ( ct_rshift ))
         simpleMode = true;
   
      VehicleMovement* vehicleMovement = ContainerControls::movement ( unit, simpleMode );
      if ( vehicleMovement ) {
   
         vehicleMovement->registerPVA ( vat_move, &pendingVehicleActions );
         for ( int i = 0; i < vehicleMovement->reachableFields.getFieldNum(); i++ )
            vehicleMovement->reachableFields.getField( i ) ->a.temp = 1;
   
            // if ( !CGameOptions::Instance()->dontMarkFieldsNotAccessible_movement )
         for ( int j = 0; j < vehicleMovement->reachableFieldsIndirect.getFieldNum(); j++ )
            vehicleMovement->reachableFieldsIndirect.getField( j ) ->a.temp2 = 2;
   
         repaintMap();
            
         GuiIconHandler guiIconHandler;
         guiIconHandler.registerUserFunction( new MovementDestination( *mainScreenWidget ) );
         guiIconHandler.registerUserFunction( new CancelMovement( *mainScreenWidget ) );
   
         NewGuiHost::pushIconHandler( &guiIconHandler );
   
         parent.Hide();
            
         mainScreenWidget->Update();
         mainScreenWidget->RunModal();
         actmap->cleartemps(7);
   
         NewGuiHost::popIconHandler();
         parent.cargoChanged();
         parent.Show();
            
         if ( pendingVehicleActions.move )
            delete pendingVehicleActions.move;
   
      } else
         infoMessage( getmessage( 107 ) );
   
   }


   //////////////////////////////////////////////////////////////////////////////////////////////
   

   bool UnitProduction::available( const MapCoordinate& pos, ContainerBase* subject, int num )
   {
      return parent.getContainer()->baseType->hasFunction( ContainerBaseType::InternalVehicleProduction );
   }


   bool UnitProduction::checkForKey( const SDL_KeyboardEvent* key, int modifier )
   {
      return ( key->keysym.sym == 'p' );
   };

   Surface& UnitProduction::getImage( const MapCoordinate& pos, ContainerBase* subject, int num )
   {
      return IconRepository::getIcon("unitproduction.png");
   };
   ASCString UnitProduction::getName( const MapCoordinate& pos, ContainerBase* subject, int num )
   {
      return "produce unit";
   };


   void UnitProduction::execute( const MapCoordinate& pos, ContainerBase* subject, int num )
   {
      bool refillAmmo;
      bool refillResources;
      const Vehicletype* v;
      {
         VehicleProduction_SelectionWindow fsw( NULL, PG_Rect( 10, 10, 450, 550 ), parent.getContainer(), parent.getContainer()->unitProduction );
         fsw.Show();
         fsw.RunModal();
         v = fsw.getVehicletype();
         refillAmmo = fsw.fillWithAmmo();
         refillResources = fsw.fillWithResources();
      }
      if ( v ) {
         ContainerControls cc( parent.getContainer() );
         cc.produceUnit( v, refillAmmo, refillResources );
         parent.cargoChanged();
      }
   }


   //////////////////////////////////////////////////////////////////////////////////////////////
   

   bool UnitTraining::available( const MapCoordinate& pos, ContainerBase* subject, int num )
   {
      if ( !subject )
         return false;
      
      Vehicle* veh = dynamic_cast<Vehicle*>(subject);
      if ( !veh )
         return false;
      
      return parent.getContainer()->baseType->hasFunction( ContainerBaseType::TrainingCenter ) && parent.getControls().unitTrainingAvailable( veh );
   }


   bool UnitTraining::checkForKey( const SDL_KeyboardEvent* key, int modifier )
   {
      return ( key->keysym.sym == 't' );
   };

   Surface& UnitTraining::getImage( const MapCoordinate& pos, ContainerBase* subject, int num )
   {
      return IconRepository::getIcon("training.png");
   };
   
   ASCString UnitTraining::getName( const MapCoordinate& pos, ContainerBase* subject, int num )
   {
      return "train unit";
   };


   void UnitTraining::execute( const MapCoordinate& pos, ContainerBase* subject, int num )
   {
      if ( !subject )
         return;
      
      Vehicle* veh = dynamic_cast<Vehicle*>(subject);
      if ( !veh )
         return;
      
      parent.getControls().trainUnit( veh );
      parent.cargoChanged();
   }
   
   //////////////////////////////////////////////////////////////////////////////////////////////
   

   bool RefuelUnit::available( const MapCoordinate& pos, ContainerBase* subject, int num )
   {
      if ( !subject )
         return false;

      Vehicle* veh = dynamic_cast<Vehicle*>(subject);
      if ( !veh )
         return false;

      for ( int r = 1; r < 3; ++r )
         if ( veh->getTank().resource(r) < veh->getStorageCapacity().resource(r))
            return true;

      for ( int w = 0; w < veh->typ->weapons.count; ++w)
         if ( veh->typ->weapons.weapon[w].requiresAmmo() )
            if ( veh->typ->weapons.weapon[w].count > veh->ammo[w] )
               return true;

      return false;
   }


   bool RefuelUnit::checkForKey( const SDL_KeyboardEvent* key, int modifier )
   {
      return ( key->keysym.sym == 'f' );
   };

   Surface& RefuelUnit::getImage( const MapCoordinate& pos, ContainerBase* subject, int num )
   {
      return IconRepository::getIcon("refuel.png");
   };
   
   ASCString RefuelUnit::getName( const MapCoordinate& pos, ContainerBase* subject, int num )
   {
      return "refuel unit";
   };


   void RefuelUnit::execute( const MapCoordinate& pos, ContainerBase* subject, int num )
   {
      if ( !subject )
         return;
      
      Vehicle* veh = dynamic_cast<Vehicle*>(subject);
      if ( !veh )
         return;
      
      parent.getControls().refilleverything( veh );
      parent.cargoChanged();
   }

   //////////////////////////////////////////////////////////////////////////////////////////////
   

   bool MoveUnitUp::available( const MapCoordinate& pos, ContainerBase* subject, int num )
   {
      if ( !subject )
         return false;
      
      Vehicle* veh = dynamic_cast<Vehicle*>(subject);
      if ( !veh )
         return false;
      
      return parent.getControls().moveUnitUpAvail( veh );
   }


   bool MoveUnitUp::checkForKey( const SDL_KeyboardEvent* key, int modifier )
   {
      return ( key->keysym.sym == 'u' );
   };

   Surface& MoveUnitUp::getImage( const MapCoordinate& pos, ContainerBase* subject, int num )
   {
      return IconRepository::getIcon("moveunitup.png");
   };
   
   ASCString MoveUnitUp::getName( const MapCoordinate& pos, ContainerBase* subject, int num )
   {
      return "move unit to upper transport";
   };


   void MoveUnitUp::execute( const MapCoordinate& pos, ContainerBase* subject, int num )
   {
      if ( !subject )
         return;


      Vehicle* veh = dynamic_cast<Vehicle*>(subject);
      if ( !veh )
         return;
      
      parent.getControls().moveUnitUp( veh );
      parent.cargoChanged();
   }


   //////////////////////////////////////////////////////////////////////////////////////////////
   
   bool RefuelUnitDialog :: available( const MapCoordinate& pos, ContainerBase* subject, int num )
   {
      if ( !subject )
         return false;

      Vehicle* veh = dynamic_cast<Vehicle*>(subject);
      if ( !veh )
         return false;

      for ( int r = 1; r < 3; ++r )
         if ( veh->getStorageCapacity().resource(r) )
            return true;

      for ( int w = 0; w < veh->typ->weapons.count; ++w)
         if ( veh->typ->weapons.weapon[w].requiresAmmo() )
            return true;

      return false;
   };

   void RefuelUnitDialog :: execute( const MapCoordinate& pos, ContainerBase* subject, int num )
   {
      if ( !subject )
         return;
      
      ammoTransferWindow( parent.getContainer(), subject );
      parent.cargoChanged();
   }

   Surface& RefuelUnitDialog :: getImage( const MapCoordinate& pos, ContainerBase* subject, int num )
   {
      return IconRepository::getIcon("refuel-dialog.png");
   };

   bool RefuelUnitDialog :: checkForKey( const SDL_KeyboardEvent* key, int modifier )
   {
      return ( key->keysym.unicode == 'd' );
   };

   ASCString RefuelUnitDialog :: getName( const MapCoordinate& pos, ContainerBase* subject, int num )
   {
      return "Refuel Dialog";
   };

   //////////////////////////////////////////////////////////////////////////////////////////////
   
   bool OpenContainer :: available( const MapCoordinate& pos, ContainerBase* subject, int num )
   {
      if ( !subject )
         return false;
      
      Vehicle* veh = dynamic_cast<Vehicle*>(subject);
      if ( !veh )
         return false;
      
      Player& player = veh->getMap()->player[veh->getOwner()];
      if ( veh->typ->maxLoadableUnits && player.diplomacy.isAllied( actmap->actplayer)  )
         return true;
      
      return false;
   };

   void OpenContainer :: execute( const MapCoordinate& pos, ContainerBase* subject, int num )
   {
      if ( !subject )
         return;
      
      cargoDialog( subject );
      parent.cargoChanged();
   }

   Surface& OpenContainer :: getImage( const MapCoordinate& pos, ContainerBase* subject, int num )
   {
      return IconRepository::getIcon("container.png");
   };

   bool OpenContainer :: checkForKey( const SDL_KeyboardEvent* key, int modifier )
   {
      return ( key->keysym.unicode == 'l' );
   };

   ASCString OpenContainer :: getName( const MapCoordinate& pos, ContainerBase* subject, int num )
   {
      return "open transport ";
   };
   
}
