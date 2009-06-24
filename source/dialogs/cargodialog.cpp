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
#include "../util/messaginghub.h"
#include "../iconrepository.h"
#include "../vehicle.h"
#include "../gamemap.h"
#include "../containercontrols.h"

#include "../asc-mainscreen.h"
#include "../guiiconhandler.h"
#include "../guifunctions.h"
#include "../guifunctions-interface.h"
#include "../cannedmessages.h"
#include "../dashboard.h"
#include "../dialog.h"
#include "../containerbase-functions.h"
#include "../resourcenet.h"

#include "../containerbase-functions.h"
#include "../gameoptions.h"

#include "../actions/moveunitcommand.h"
#include "../actions/cargomovecommand.h"
#include "../actions/servicecommand.h"
#include "../actions/recycleunitcommand.h"
#include "../actions/repairunitcommand.h"
#include "../actions/trainunitcommand.h"
#include "../actions/repairbuildingcommand.h"
#include "../actions/transfercontrolcommand.h"
#include "../actions/setresourceprocessingratecommand.h"

#include "selectionwindow.h"
#include "ammotransferdialog.h"
#include "unitinfodialog.h"
#include "vehicleproductionselection.h"
#include "../sg.h"
#include "../spfst.h"

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
      virtual ASCString getFullName() = 0;
      virtual void registerSubwindow( CargoDialog* cd );
      virtual void registerChilds( CargoDialog* cd );
      virtual void update() = 0;
      virtual ~SubWindow() {} ;
};


class SubWinButton : public PG_Button
{

      static int calcWidth() {
         if ( buttonwidth == -1 ) {
            buttonwidth = IconRepository::getIcon("cargo-buttonpressed.png").w();
         }
         return buttonwidth;
      }

      static int calcHeight() {
         if ( buttonheight == -1 ) {
            buttonheight = IconRepository::getIcon("cargo-buttonpressed.png").h();
         }
         return buttonheight;
      }

   public:
      static int buttonwidth;
      static int buttonheight;


      SubWinButton( PG_Widget *parent, const SPoint& pos, SubWindow* subWindow ) : PG_Button( parent, PG_Rect( pos.x, pos.y, calcWidth(), calcHeight() ), "", -1, "SubWinButton") {
         SetBackground( PRESSED, IconRepository::getIcon("cargo-buttonpressed.png").getBaseSurface() );
         SetBackground( HIGHLITED, IconRepository::getIcon("cargo-buttonhighlighted.png").getBaseSurface() );
         SetBackground( UNPRESSED, IconRepository::getIcon("cargo-buttonunpressed.png").getBaseSurface() );
         SetBorderSize(0,0,0);
         ASCString filename = ASCString("cargo-") + subWindow->getASCTXTname();
         SetIcon( IconRepository::getIcon( filename + ".png" ).getBaseSurface(),
                  IconRepository::getIcon( filename + "-pressed.png" ).getBaseSurface(),
                  IconRepository::getIcon( filename + ".png" ).getBaseSurface() );
         SetToggle( true );
         new PG_ToolTipHelp ( this, subWindow->getFullName() );
      };
};

int SubWinButton::buttonwidth = -1;
int SubWinButton::buttonheight = -1;



namespace CargoGuiFunctions
{

class MovementDestination : public GuiFunctions::Movement
{
      PG_Widget& parent;
   public:
      MovementDestination( PG_Widget& masterParent ) : parent( masterParent)  {};

      void execute( const MapCoordinate& pos, ContainerBase* subject, int num ) {
         GuiFunctions::Movement::execute( pos, subject, num );
         if ( !NewGuiHost::pendingCommand )
            parent.QuitModal();
      }
};

class CancelMovement : public GuiFunctions::Cancel
{
      PG_Widget& parent;
   public:
      CancelMovement ( PG_Widget& masterParent ) : parent( masterParent)  {};

      void execute( const MapCoordinate& pos, ContainerBase* subject, int num ) {
         GuiFunctions::Cancel::execute( pos, subject, num );
         parent.QuitModal();
      }
};


class CloseDialog : public GuiFunction
{
      CargoDialog& parent;
   public:
      CloseDialog( CargoDialog& masterParent ) : parent( masterParent)  {};
      bool available( const MapCoordinate& pos, ContainerBase* subject, int num );
      void execute( const MapCoordinate& pos, ContainerBase* subject, int num );
      bool checkForKey( const SDL_KeyboardEvent* key, int modifier, int num );
      Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num );
      ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num );
};


class Movement : public GuiFunction
{
      CargoDialog& parent;
   public:
      Movement( CargoDialog& masterParent ) : parent( masterParent)  {};
      bool available( const MapCoordinate& pos, ContainerBase* subject, int num );
      void execute( const MapCoordinate& pos, ContainerBase* subject, int num );
      bool checkForKey( const SDL_KeyboardEvent* key, int modifier, int num );
      Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num );
      ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num );
};


class UnitProduction : public GuiFunction, public SigC::Object
{
      CargoDialog& parent;
      ConstructUnitCommand::Producables producables;
      ConstructUnitCommand* constructUnitCommand;
      void productionLinesChanged();
   public:
      UnitProduction( CargoDialog& masterParent ) : parent( masterParent), constructUnitCommand(NULL)  {};
      bool available( const MapCoordinate& pos, ContainerBase* subject, int num );
      void execute( const MapCoordinate& pos, ContainerBase* subject, int num );
      bool checkForKey( const SDL_KeyboardEvent* key, int modifier, int num );
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
      bool checkForKey( const SDL_KeyboardEvent* key, int modifier, int num );
      Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num );
      ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num );
};

class RefuelUnitCommand : public GuiFunction
{
      CargoDialog& parent;
   public:
      RefuelUnitCommand( CargoDialog& masterParent ) : parent( masterParent)  {};
      bool available( const MapCoordinate& pos, ContainerBase* subject, int num );
      void execute( const MapCoordinate& pos, ContainerBase* subject, int num );
      bool checkForKey( const SDL_KeyboardEvent* key, int modifier, int num );
      Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num );
      ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num );
};
class RefuelUnitDialogCommand : public GuiFunction
{
      CargoDialog& parent;
   public:
      RefuelUnitDialogCommand( CargoDialog& masterParent ) : parent( masterParent)  {};
      bool available( const MapCoordinate& pos, ContainerBase* subject, int num );
      void execute( const MapCoordinate& pos, ContainerBase* subject, int num );
      bool checkForKey( const SDL_KeyboardEvent* key, int modifier, int num );
      Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num );
      ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num );
};

class RepairUnit : public GuiFunction
{
      CargoDialog& parent;
   public:
      RepairUnit( CargoDialog& masterParent ) : parent( masterParent)  {};
      bool available( const MapCoordinate& pos, ContainerBase* subject, int num );
      void execute( const MapCoordinate& pos, ContainerBase* subject, int num );
      bool checkForKey( const SDL_KeyboardEvent* key, int modifier, int num );
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
      bool checkForKey( const SDL_KeyboardEvent* key, int modifier, int num );
      Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num );
      ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num );
};

class MoveUnitIntoInnerContainer : public GuiFunction
{
      CargoDialog& parent;
   public:
      MoveUnitIntoInnerContainer ( CargoDialog& masterParent ) : parent( masterParent)  {};
      bool available( const MapCoordinate& pos, ContainerBase* subject, int num );
      void execute( const MapCoordinate& pos, ContainerBase* subject, int num );
      bool checkForKey( const SDL_KeyboardEvent* key, int modifier, int num );
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
      bool checkForKey( const SDL_KeyboardEvent* key, int modifier, int num );
      Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num );
      ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num );
};

class RecycleUnitCommandButton : public GuiFunction
{
      ASCString getCommandName( const ContainerBase* carrier );
      CargoDialog& parent;
   public:
      RecycleUnitCommandButton( CargoDialog& masterParent ) : parent( masterParent)  {};
      bool available( const MapCoordinate& pos, ContainerBase* subject, int num );
      void execute( const MapCoordinate& pos, ContainerBase* subject, int num );
      bool checkForKey( const SDL_KeyboardEvent* key, int modifier, int num );
      Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num );
      ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num );
};


class UnitInfo : public GuiFunction
{
      CargoDialog& parent;
   public:
      UnitInfo( CargoDialog& masterParent ) : parent( masterParent)  {};
      bool available( const MapCoordinate& pos, ContainerBase* subject, int num );
      void execute( const MapCoordinate& pos, ContainerBase* subject, int num );
      bool checkForKey( const SDL_KeyboardEvent* key, int modifier, int num );
      Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num );
      ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num );
};

class TransferUnitControl : public GuiFunction
{
      CargoDialog& parent;
   public:
      TransferUnitControl( CargoDialog& masterParent ) : parent( masterParent)  {};
      bool available( const MapCoordinate& pos, ContainerBase* subject, int num );
      void execute( const MapCoordinate& pos, ContainerBase* subject, int num );
      Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num );
      ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num );
};



}; // namespace CargoGuiFunctions


class BuildingControlWindow;
class CargoInfoWindow;

class CargoDialog : public Panel
{

      ContainerControls containerControls;

      GuiIconHandler guiIconHandler;

      ContainerBase* container;
      bool setupOK;
      Surface infoImage;

      typedef vector<SubWindow*> Activesubwindows;
      Activesubwindows activesubwindows;

      typedef deallocating_vector<SubWindow*> Subwindows;
      Subwindows subwindows;

      vector<SubWinButton*> subWinButtons;

      CargoWidget* cargoWidget;
      SubWindow* researchWindow;
      SubWindow* matterWindow;
      BuildingControlWindow* buildingControlWindow;

      bool shutdownImmediately;

      void containerDestroyed() {
         shutdownImmediately = true;
      }

      CargoInfoWindow* ciw;

      bool eventKeyDown(const SDL_KeyboardEvent* key) {
         int mod = SDL_GetModState() & ~(KMOD_NUM | KMOD_CAPS | KMOD_MODE);

         if ( !mod  ) {
            if ( key->keysym.sym == SDLK_ESCAPE ) {
               QuitModal();
               return true;
            }
         }

         if ( (mod & KMOD_SHIFT) && (mod & KMOD_CTRL)) {
            switch ( key->keysym.unicode ) {
               case 26: // Z
                  getContainer()->getMap()->actions.redo( createContext( getContainer()->getMap() ) );
                  cargoChanged();
                  return true;
            }
         }

         if ( (mod & KMOD_CTRL) &&  !(mod & KMOD_SHIFT)) {
            switch ( key->keysym.unicode ) {
               case 26: // Z
                  getContainer()->getMap()->actions.undo( createContext( getContainer()->getMap() ) );
                  if ( shutdownImmediately ) {
                     QuitModal();
                     return true;
                  }

                  cargoChanged();
                  for ( Activesubwindows::iterator i =  activesubwindows.begin(); i != activesubwindows.end(); ++i )
                     (*i)->update();
                  return true;
            }
         }

         return false;
      };



      void registerSpecialDisplay( const ASCString& name ) {
         SpecialDisplayWidget* sdw = dynamic_cast<SpecialDisplayWidget*>( FindChild( name, true ) );
         if ( sdw )
            sdw->display.connect( SigC::slot( *this, &CargoDialog::painter ));
      };

      void registerSpecialInput( const ASCString& name ) {
         SpecialInputWidget* siw = dynamic_cast<SpecialInputWidget*>( FindChild( name, true ) );
         if ( siw )
            siw->sigMouseButtonDown.connect( SigC::slot( *this, &CargoDialog::onClick ));
      };

      bool onClick ( PG_MessageObject* obj, const SDL_MouseButtonEvent* event ) {
         PG_Widget* w = dynamic_cast<PG_Widget*>(obj);
         if ( w ) {
            // click( w->GetName() );
            return true;
         }
         return false;
      };

      void onUnitClick ( Vehicle* veh,SPoint pos, bool first ) {
         // if ( veh )
         if ( mainScreenWidget&& mainScreenWidget->getGuiHost() && (!first || CGameOptions::Instance()->mouse.singleClickAction) ) {
            SPoint iconPos = pos;
            if ( veh ) {
               iconPos.x -= smallGuiIconSizeX/2;
               iconPos.y -= smallGuiIconSizeY/2;
            } else {
               iconPos.x += 2;
               iconPos.y += 2;
            }

            mainScreenWidget->getGuiHost()->showSmallIcons( this, iconPos, false );
         }

      };

      void clearSmallIcons( ) {
         if ( mainScreenWidget && mainScreenWidget->getGuiHost() )
            mainScreenWidget->getGuiHost()->clearSmallIcons();
      }



      void painter ( const PG_Rect &src, const ASCString& name, const PG_Rect &dst) {
         Surface screen = Surface::Wrap( PG_Application::GetScreen() );

         if ( name == "unitpad_unitsymbol" ) {
            Vehicle* v = dynamic_cast<Vehicle*>(container);
            if ( v )
               v->paint( screen, SPoint( dst.x, dst.y ), 0);
         }
      };

      bool activate_i( int pane ) {
         if ( pane >= 0 && pane < activesubwindows.size() ) {
            activate( activesubwindows[pane]->getASCTXTname() );
            activesubwindows[pane]->update();
         }
         return true;
      }

      void activate( const ASCString& pane ) {
         BulkGraphicUpdates bgu( this );
         for ( int i = 0; i < activesubwindows.size(); ++i )
            if ( activesubwindows[i]->getASCTXTname() != pane )
               hide( activesubwindows[i]->getASCTXTname() );

         for ( int i = 0; i < activesubwindows.size(); ++i )
            if ( activesubwindows[i]->getASCTXTname() == pane )
               show( activesubwindows[i]->getASCTXTname() );

         for ( int i = 0; i< subWinButtons.size(); ++i )
            subWinButtons[i]->SetPressed( activesubwindows[i]->getASCTXTname() == pane );
      };


      void registerGuiFunctions( GuiIconHandler& handler ) {
         handler.registerUserFunction( new CargoGuiFunctions::Movement( *this ) );
         handler.registerUserFunction( new CargoGuiFunctions::RefuelUnitCommand( *this ) );
         handler.registerUserFunction( new CargoGuiFunctions::RefuelUnitDialogCommand( *this ) );
         handler.registerUserFunction( new CargoGuiFunctions::RepairUnit( *this ) );
         handler.registerUserFunction( new CargoGuiFunctions::UnitProduction( *this ));
         handler.registerUserFunction( new CargoGuiFunctions::UnitTraining( *this ));
         handler.registerUserFunction( new CargoGuiFunctions::MoveUnitUp( *this ));
         handler.registerUserFunction( new CargoGuiFunctions::MoveUnitIntoInnerContainer( *this ));
         handler.registerUserFunction( new CargoGuiFunctions::OpenContainer( *this ));
         handler.registerUserFunction( new CargoGuiFunctions::RecycleUnitCommandButton( *this ));
         handler.registerUserFunction( new CargoGuiFunctions::UnitInfo( *this ));
         handler.registerUserFunction( new CargoGuiFunctions::TransferUnitControl( *this ));
         handler.registerUserFunction( new CargoGuiFunctions::CloseDialog( *this ));
      }

      void checkStoringPosition( Vehicle* unit ) {
         if ( mainScreenWidget&& mainScreenWidget->getGuiHost() ) {
            mainScreenWidget->getGuiHost()->eval( container->getPosition(), unit );
         }

         if ( mainScreenWidget && mainScreenWidget->getUnitInfoPanel() )
            mainScreenWidget->getUnitInfoPanel()->showUnitData( unit, NULL, NULL, true );
      }

      void updateLoadMeter() {
         if ( container->baseType->maxLoadableWeight > 0 )
            setBargraphValue ( "LoadingMeter", float( container->cargoWeight()) / container->baseType->maxLoadableWeight );
      }

      void dragUnitToInnerContainer( Vehicle* draggedUnit, Vehicle* targetUnit ) {
         if ( !draggedUnit )
            return;

         if ( draggedUnit != targetUnit ) {
            if ( targetUnit && CargoMoveCommand::moveInAvail(draggedUnit, targetUnit) ) {
               auto_ptr<CargoMoveCommand> cargomove ( new CargoMoveCommand( draggedUnit ));
               cargomove->setMode( CargoMoveCommand::moveInwards );
               cargomove->setTargetCarrier( targetUnit );
               ActionResult res = cargomove->execute ( createContext( getContainer()->getMap() ));
               if ( res.successful() )
                  cargomove.release();
            } else {
               if ( container->getCarrier() ) {
                  auto_ptr<CargoMoveCommand> cargomove ( new CargoMoveCommand( draggedUnit ));
                  cargomove->setMode( CargoMoveCommand::moveOutwards );
                  ActionResult res = cargomove->execute ( createContext( getContainer()->getMap() ));
                  if ( res.successful() )
                     cargomove.release();
               } else {

               }
            }
         }
         cargoChanged();
      }

      bool dragUnitToInnerContainerAvail( Vehicle* draggedUnit, Vehicle* targetUnit ) {
         return CargoMoveCommand::moveInAvail( draggedUnit, targetUnit );
      }

      void dragInProcess() {
         if ( mainScreenWidget && mainScreenWidget->getGuiHost() )
            mainScreenWidget->getGuiHost()->clearSmallIcons();
      }

      void dragAborted() {
         // to redraw everything
         cargoChanged();
      }

   public:
      int  getResourceValue ( int resourcetype, int y, int scope ) {
         int player = container->getMap()->actplayer;
         if ( container->getMap()->getCurrentPlayer().stat == Player::supervisor )
            player = container->getOwner();

         switch ( y ) {
            case 0: { // avail
               GetResource gr ( container->getMap() );
               return gr.getresource ( container->getPosition().x, container->getPosition().y, resourcetype, maxint, 1, player, scope );
            }
            case 1: { // tank
               GetResourceCapacity grc ( container->getMap() );
               return grc.getresource ( container->getPosition().x, container->getPosition().y, resourcetype, maxint, 1, player, scope );
            }
            case 2: { // plus
               GetResourcePlus grp ( container->getMap() );
               return grp.getresource ( container->getPosition().x, container->getPosition().y, resourcetype, player, scope );
            }
            case 3: { // usage
               GetResourceUsage gru( container->getMap() );
               return gru.getresource ( container->getPosition().x, container->getPosition().y, resourcetype, player, scope );
            }
         } /* endswitch */
         return -1;
      }


   public:

      CargoDialog (PG_Widget *parent, ContainerBase* cb );

      void addAvailableSubwin( SubWindow* w ) {
         activesubwindows.push_back( w );
      };

      void updateResourceDisplay() {
         setLabelText( "energyavail", container->getResource(maxint, 0, true ) );
         setLabelText( "materialavail", container->getResource(maxint, 1, true ) );
         setLabelText( "fuelavail", container->getResource(maxint, 2, true ) );
         setLabelText( "numericaldamage", container->damage );
      }


      Vehicle* getMarkedUnit() {
         if ( cargoWidget )
            return cargoWidget->getMarkedUnit();
         else
            return NULL;
      }

      void cargoChanged() {
         //unitHighLight.setNew( unitHighLight.getMark() );
         if ( cargoWidget ) {
            cargoWidget->redrawAll();
            if ( mainScreenWidget && mainScreenWidget->getUnitInfoPanel() )
               mainScreenWidget->getUnitInfoPanel()->showUnitData( cargoWidget->getMarkedUnit(), NULL, NULL, true );

            checkStoringPosition( cargoWidget->getMarkedUnit() );
         }
         sigCargoChanged();
         updateResourceDisplay();
         showAmmo();
         updateLoadMeter();
      }

      int RunModal() {
         if ( setupOK )
            return Panel::RunModal();
         return 0;
      }

      bool ProcessEvent ( const SDL_Event *   event,bool   bModal = false  ) {
         // the unit info panel must get the events first. If a unit is renamed and a character entered, this
         // key event shall go to the renaming - and not to the unit actions.
         if ( mainScreenWidget && mainScreenWidget->getUnitInfoPanel() )
            if ( mainScreenWidget->getUnitInfoPanel()->ProcessEvent( event, bModal ))
               return true;

         if ( mainScreenWidget && mainScreenWidget->getGuiHost() )
            if ( mainScreenWidget->getGuiHost()->ProcessEvent( event, bModal ))
               return true;

         if ( Panel::ProcessEvent( event, bModal ))
            return true;

         return false;
      }

      void showAmmo() {
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

      ContainerBase* getContainer() {
         return container;
      };

      GameMap* getMap() {
         if ( container ) return container->getMap();
         else return NULL;
      };

      ContainerControls& getControls() {
         return containerControls;
      };

      SigC::Signal0<void>  sigCargoChanged;

      void updateVariables( ) {
         PG_Widget* widget = this;
         // from solarpower
         {
            setLabelText( "MaxSolarPower", container->maxplus.energy );

            static const char* weathernames[] = {"terrain_weather_dry.png",
                                                 "terrain_weather_lightrain.png",
                                                 "terrain_weather_heavyrain.png",
                                                 "terrain_weather_lightsnow.png",
                                                 "terrain_weather_heavysnow.png",
                                                 "terrain_weather_ice.png"
                                                };


            SolarPowerplant solarPowerPlant ( container );
            Resources plus = solarPowerPlant.getPlus();
            setLabelText( "CurrentSolarPower", plus.energy );
            setImage( "Weather", weathernames[ container->getMap()->getField( container->getPosition() )->getWeather() ], widget );
         }

         // from windpower
         {
            WindPowerplant windPowerPlant ( container );
            Resources plus = windPowerPlant.getPlus();
            setLabelText( "CurrentWindPower", plus.energy, widget );

#ifdef WEATHERGENERATOR
            if ( container->getMap()->weatherSystem )
               setLabelText( "Weather", container->getMap()->weatherSystem->getCurrentWindSpeed(), widget );
#else
            setLabelText( "Weather", container->getMap()->weather.windSpeed, widget );
#endif
         }

         // from cargo
         {
            ASCString s;
            s.format( "%d / %d", container->cargoWeight(), container->baseType->maxLoadableWeight );
            setLabelText( "CargoUsage", s, widget );


            class MoveMalusType
            {
               public:
                  enum {  deflt,
                          light_tracked_vehicle,
                          medium_tracked_vehicle,
                          heavy_tracked_vehicle,
                          light_wheeled_vehicle,
                          medium_wheeled_vehicle,
                          heavy_wheeled_vehicle,
                          trooper,
                          rail_vehicle,
                          medium_aircraft,
                          medium_ship,
                          structure,
                          light_aircraft,
                          heavy_aircraft,
                          light_ship,
                          heavy_ship,
                          helicopter,
                          hoovercraft
                    };
            };


            if ( getMarkedUnit() ) {
               setLabelText( "CurrentCargo", getMarkedUnit()->weight(), widget );
               setImage( "TypeImage", moveMaliTypeIcons[getMarkedUnit()->typ->movemalustyp], widget );
               show( "TypeImage" );
            } else {
               setLabelText( "CurrentCargo", "-" , widget );
               hide( "TypeImage" );
            }

            if ( container->baseType->maxLoadableWeight > 0 )
               setBargraphValue ( "LoadingMeter2", float( container->cargoWeight()) / container->baseType->maxLoadableWeight, widget );

            if ( container->baseType->maxLoadableWeight >= 1000000 ) {
               setLabelText ( "FreeWeight", "unlimited", widget );
               setLabelText ( "MaxWeight",  "unlimited", widget );
            } else {
               setLabelText ( "FreeWeight", container->baseType->maxLoadableWeight - container->cargoWeight(), widget );
               setLabelText ( "MaxWeight",  container->baseType->maxLoadableWeight, widget );
            }

            setLabelText ( "FreeSlots", container->baseType->maxLoadableUnits - container->vehiclesLoaded(), widget );
            setLabelText ( "MaxSlots",  container->baseType->maxLoadableUnits , widget );
            setLabelText ( "MaxUnitSize",  container->baseType->maxLoadableUnitSize, widget );
         }

         // from buildingcontrol
         {
            Building* bld = dynamic_cast<Building*>( container );
            if ( bld ) {
               if ( RepairBuildingCommand::avail( bld )) {
                  RepairBuildingCommand rbc( bld );
                  RepairBuildingCommand::RepairData repairData = rbc.getCost();
                  setLabelText( "RepairCostLabel", "Cost for repairing " + ASCString::toString( repairData.damageDelta ) + "%", widget );

                  setLabelText( "EnergyCost", repairData.cost.energy, widget );
                  setLabelText( "MaterialCost", repairData.cost.material, widget );
                  setLabelText( "FuelCost", repairData.cost.fuel, widget );

               } else {
                  setLabelText( "RepairCostLabel", "No repair possible", widget );

                  setLabelText( "EnergyCost", "-", widget );
                  setLabelText( "MaterialCost", "-", widget );
                  setLabelText( "FuelCost", "-", widget );
               }
            }
            setLabelText( "Jamming", container->baseType->jamming , widget );
            setLabelText( "View", container->baseType->view, widget );
            setLabelText( "Armor", container->getArmor(), widget );
         }

      }


      ~CargoDialog() {
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
      warningMessage( "Could not find widget with name " + getASCTXTname() );
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



class SolarPowerWindow : public SubWindow
{

   public:
      bool available( CargoDialog* cd ) {
         if ( !cd->getMap()->getCurrentPlayer().diplomacy.isAllied( cd->getContainer() ))
            return false;

         return cd->getContainer()->baseType->hasFunction( ContainerBaseType::SolarPowerPlant  );
      };

      ASCString getASCTXTname() {
         return "solarpower";
      };

      ASCString getFullName() {
         return "Solar Power Generation";
      };

      void update() {
         cargoDialog->updateVariables();
      }
};

class WindPowerWindow : public SubWindow
{
   public:
      bool available( CargoDialog* cd ) {
         if ( !cd->getMap()->getCurrentPlayer().diplomacy.isAllied( cd->getContainer() ))
            return false;

         return cd->getContainer()->baseType->hasFunction( ContainerBaseType::WindPowerPlant  );
      };

      ASCString getASCTXTname() {
         return "windpower";
      };

      ASCString getFullName() {
         return "Wind Power Generation";
      };


      void update() {
         cargoDialog->updateVariables();
      }
};

class NetControlWindow : public SubWindow
{

   private:

      bool click( PG_Button* b, int x, int y ) {
         return true;
      }

      PG_Button* findButton( int x, int y ) {
         char c = 'a' + x;
         ASCString buttonName = "Button" + ASCString::toString(y) + c;
         return dynamic_cast<PG_Button*>( widget->FindChild( buttonName, true ) );
      }

   public:
      bool available( CargoDialog* cd ) {
         if ( !cd->getMap()->getCurrentPlayer().diplomacy.isAllied( cd->getContainer() ))
            return false;

         return dynamic_cast<Building*>(cd->getContainer()) != NULL;
      };

      ASCString getASCTXTname() {
         return "netcontrol";
      };

      ASCString getFullName() {
         return "Resource Network Control";
      };


      void registerChilds( CargoDialog* cd ) {
         SubWindow::registerChilds( cd );

         if ( widget ) {
            for ( int x = 0; x < 3; ++x ) {
               for ( int y = 0; y < 4; ++y ) {
                  PG_Button* b = findButton( x, y );
                  if ( b ) {
                     b->sigClick.connect( SigC::bind( SigC::bind( SigC::slot( *this, &NetControlWindow::click ), x), y));
                     if ( y >= 2)
                        b->SetToggle( true );
                  }

               }
            }

         }
      }

      void update() {
         cargoDialog->updateVariables();
      }
};


class CargoInfoWindow : public SubWindow
{
   public:
      bool available( CargoDialog* cd ) {
         return true;
      };

      ASCString getASCTXTname() {
         return "cargoinfo";
      };

      ASCString getFullName() {
         return "Cargo Info";
      };

      void registerSubwindow( CargoDialog* cd ) {
         SubWindow::registerSubwindow( cd );
         cargoDialog->sigCargoChanged.connect( SigC::slot( *this, &CargoInfoWindow::update ));
      };


      void registerChilds( CargoDialog* cd ) {
         SubWindow::registerChilds( cd );

      }

      void update() {
         cargoDialog->updateVariables();
      }
};


class BuildingControlWindow : public SubWindow
{
   public:
      SigC::Signal0<void> damageChanged;


      void registerChilds( CargoDialog* cd ) {
         SubWindow::registerChilds( cd );

         if ( widget ) {
            PG_Button* b = dynamic_cast<PG_Button*>( widget->FindChild( "RepairButton", true ) );
            if ( b )
               b->sigClick.connect( SigC::slot( *this, &BuildingControlWindow::repair ));
         }
      }

      bool repair() {
         Building* bld = dynamic_cast<Building*>(container() );
         if ( !RepairBuildingCommand::avail( bld ))
            return false;

         auto_ptr<RepairBuildingCommand> rbc ( new RepairBuildingCommand( bld ));
         ActionResult res = rbc->execute( createContext( container()->getMap() ));
         if  ( res.successful() )
            rbc.release();
         else
            displayActionError( res );

         cargoDialog->updateResourceDisplay();

         damageChanged();
         update();

         if  ( widget )
            widget->Update();

         return true;
      }


      bool available( CargoDialog* cd ) {
         if ( !cd->getMap()->getCurrentPlayer().diplomacy.isAllied( cd->getContainer() ))
            return false;

         Building* bld = dynamic_cast<Building*>(cd->getContainer());
         if ( !bld )
            return false;

         return true;

         // return RepairBuildingCommand::avail(  ));
      };

      ASCString getASCTXTname() {
         return "damagecontrol";
      };

      ASCString getFullName() {
         return "Building Control";
      };


      void update() {
         cargoDialog->updateVariables();

         damageChanged(); // it didn't really change, but we are triggering a redraw
      }
};


class ResourceInfoWindow : public SubWindow
{


   public:
      bool available( CargoDialog* cd ) {
         if ( !cd->getMap()->getCurrentPlayer().diplomacy.isAllied( cd->getContainer() ) && cd->getMap()->getCurrentPlayer().stat != Player::supervisor )
            return false;

         return dynamic_cast<Building*>( cd->getContainer() ) != NULL;
      };

      ASCString getASCTXTname() {
         return "resourceinfo";
      };

      ASCString getFullName() {
         return "Resource Network Information";
      };


      void update() {

         ContainerBase* activeContainer= container();

         int value[3][3][4];
         int mx = 0;

         for ( int c = 0; c < 3; c++ )
            for ( int x = 0; x < 3; x++ )
               for ( int y = 0; y < 4; y++ ) {
                  value[c][x][y] = cargoDialog->getResourceValue ( x, y, c );
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
                  if ( (y != 1 || value[c][x][y] < mx*10 || value[c][x][y] < 1000000000 ) && ( !activeContainer->getMap()->isResourceGlobal(x) || y!=0 ||c ==2))   // don't show extremely high numbers
                     cargoDialog->setLabelText( label, value[c][x][y] );
                  else
                     cargoDialog->setLabelText( label, "-" );
               }

         // from researchwindow
         Player& player = activeContainer->getMap()->player[ activeContainer->getOwner() ];

         cargoDialog->setLabelText( "ResPerTurnLocal", activeContainer->researchpoints *  player.research.getMultiplier(), widget );
         cargoDialog->setLabelText( "ResPerTurnGlobal", player.research.getResearchPerTurn(), widget );

         Resources cost = returnResourcenUseForResearch( activeContainer );
         for ( int r = 0; r < 3; ++r)
            cargoDialog->setLabelText( "CostLocal" + ASCString::toString(r), cost.resource(r), widget );

         Resources globalCost;
         for ( Player::BuildingList::iterator i = player.buildingList.begin(); i != player.buildingList.end(); ++i )
            globalCost += returnResourcenUseForResearch( *i );

         for ( Player::VehicleList::iterator i = player.vehicleList.begin(); i != player.vehicleList.end(); ++i )
            globalCost += returnResourcenUseForResearch( *i );

         for ( int r = 0; r < 3; ++r)
            cargoDialog->setLabelText( "CostGlobal" + ASCString::toString(r), globalCost.resource(r), widget );

         int availIn = player.research.currentTechAvailableIn();
         if ( availIn >= 0 )
            cargoDialog->setLabelText( "AvailGlobal", availIn, widget );
         else
            cargoDialog->setLabelText( "AvailGlobal", "-", widget );

         if ( player.research.activetechnology )
            cargoDialog->setLabelText( "CurrentTech", player.research.activetechnology->name, widget );
         else
            cargoDialog->setLabelText( "CurrentTech", "-", widget );

      }

};


class GraphWidget : public PG_Widget
{
      map<int,int> verticalLines;
      vector<int> curves;
      typedef vector< pair<int,int> > Bars;
      Bars bars;
   protected:
      int xrange;
      int yrange;
      virtual int getPoint( int curve, int x ) {
         return 0;
      };
      virtual int getBarHeight( int bar ) {
         return 0;
      };
      virtual void click( int x, int button ) {};

      bool   eventMouseMotion (const SDL_MouseMotionEvent *motion) {
         PG_Point p = ScreenToClient ( motion->x, motion->y );
         if ( motion->type == SDL_MOUSEMOTION && (motion->state == SDL_BUTTON(1)))
            click ( p.x, 1 );

         if ( motion->type == SDL_MOUSEMOTION && (motion->state == SDL_BUTTON(3)))
            click( p.x, 3 );

         return true;
      }

      bool   eventMouseButtonDown (const SDL_MouseButtonEvent *button) {
         PG_Point p = ScreenToClient ( button->x, button->y );
         if ( button->type == SDL_MOUSEBUTTONDOWN && (button->button == SDL_BUTTON_LEFT))
            click ( p.x, 1 );

         if ( button->type == SDL_MOUSEBUTTONDOWN && (button->button == SDL_BUTTON_RIGHT))
            click ( p.x, 3 );

         return true;
      }

      int mapColor( int col ) {
         PG_Color color = col;
         return color.MapRGBA( PG_Application::GetScreen()->format, 255-GetTransparency());
      }

   public:
      GraphWidget( PG_Widget *parent, const PG_Rect& rect ) : PG_Widget( parent, rect ), xrange(1), yrange(1) {};
      void setRange( int x, int y ) {
         xrange = max( x, 1);
         yrange = max( y, 1);
      }

      int addCurve( int color ) {
         curves.push_back( color );
         return curves.size();
      }

      int addBar( int pos, int color ) {
         bars.push_back ( make_pair( pos, color ));
         return bars.size();
      }


      void addVerticalLine ( int x, int color ) {
         verticalLines[x] = color;
      }

      void clearVerticalLines() {
         verticalLines.clear();
      }

      void eventBlit (SDL_Surface *surface, const PG_Rect &src, const PG_Rect &dst) {
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
            int x = b->first * Width() / (xrange+1);
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

class ResearchGraph : public GraphWidget
{
      ContainerBase* cont;
   protected:
      int getPoint( int curve, int x ) {
         Resources cost = returnResourcenUseForResearch( cont, x );
         return cost.energy;
      }


      void addSecondaryLab( ContainerBase* lab ) {
         if ( lab->baseType->nominalresearchpoints && lab->baseType->hasFunction( ContainerBaseType::Research  ) ) {
            int rel = lab->researchpoints * cont->baseType->nominalresearchpoints / lab->baseType->nominalresearchpoints;
            addVerticalLine( rel, 0xd9d9d9 );
         }
      }

      void recalc() {
         clearVerticalLines();

         Player& player = cont->getMap()->player[ cont->getOwner() ];
         for ( Player::BuildingList::iterator i = player.buildingList.begin(); i != player.buildingList.end(); ++i )
            addSecondaryLab( *i );

         for ( Player::VehicleList::iterator i = player.vehicleList.begin(); i != player.vehicleList.end(); ++i )
            addSecondaryLab( *i );

         addVerticalLine( cont->researchpoints, 0xd5d200 );
      }

      bool setResearch ( ContainerBase* lab, int x ) {
         if ( cont->baseType->nominalresearchpoints ) {
            int res;

            if ( returnResourcenUseForResearch ( lab, lab->maxresearchpoints ) == Resources(0,0,0))
               res = lab->maxresearchpoints;
            else
               res = x * xrange / Width() * lab->baseType->nominalresearchpoints / cont->baseType->nominalresearchpoints;

            if ( res > lab->maxresearchpoints )
               res = lab->maxresearchpoints;
            int old = lab->researchpoints;
            lab->researchpoints = res;
            return res != old;
         } else
            return false;
      }


      void click( int x, int button ) {
         setResearch( x, button == 3 );
      }

      void setResearch( int x, bool global = false ) {
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
      ResearchGraph( PG_Widget *parent, const PG_Rect& rect, ContainerBase* container ) : GraphWidget( parent, rect ), cont( container ) {
         setRange( cont->maxresearchpoints+1, returnResourcenUseForResearch( cont, cont->maxresearchpoints ).energy );
         addCurve( 0x00ff00 );
         recalc();
      }

      SigC::Signal0<void> sigChange;

};


class ResearchWindow : public SubWindow
{
   public:
      bool available( CargoDialog* cd ) {
         if ( !cd->getMap()->getCurrentPlayer().diplomacy.isAllied( cd->getContainer() ))
            return false;

         return cd->getContainer()->baseType->hasFunction( ContainerBaseType::Research  );
      };

      ASCString getASCTXTname() {
         return "research";
      };

      ASCString getFullName() {
         return "Research Lab";
      };


      void update() {
         cargoDialog->updateVariables();
      }
};



class MatterAndMiningBaseWindow : public SubWindow
{
      PG_Slider* slider;
      bool first;

   protected:
      virtual bool invertSlider() {
         return false;
      };
   private:

      bool scrollPos( long pos ) {
         if ( invertSlider() )
            pos = 100 - pos;

         setnewpower( pos );
         update();
         return true;
      };


      bool scrollTrack( long pos ) {

         if ( invertSlider() )
            pos = 100 - pos;

         showResourceTable( getOutput( pos ));

         return true;
      };

      void setnewpower ( ContainerBase* c, int power ) {
         if ( hasFunction( c ) ) {
            auto_ptr<SetResourceProcessingRateCommand> srprc ( new SetResourceProcessingRateCommand( c, power ));

            ActionResult res = srprc->execute( createContext( c->getMap() ));
            if ( res.successful() )
               srprc.release();
            else
               displayActionError( res );
         }
      }

      void setnewpower ( int pwr ) {
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

      bool available( CargoDialog* cd ) {
         if ( !cd->getMap()->getCurrentPlayer().diplomacy.isAllied( cd->getContainer() ))
            return false;

         return hasFunction( cd->getContainer() );
      };

      virtual Resources getOutput() = 0;
      virtual Resources getOutput( int rate ) = 0;

   private:
      void showResourceTable( const Resources& res ) {
         for ( int r = 0; r < 3; ++r ) {
            ASCString s = Resources::name(r);
            int amount = res.resource(r);
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

   public:
      MatterAndMiningBaseWindow () : slider(NULL), first(true) {};


      void update() {
         cargoDialog->updateVariables();

         if ( widget )
            slider = dynamic_cast<PG_Slider*>( widget->FindChild( "PowerSlider", true ));

         if ( first && slider ) {
            first = false;
            slider->SetRange( 0, 100 );
            slider->sigScrollPos.connect( SigC::slot( *this, &MatterAndMiningBaseWindow::scrollPos ));
            slider->sigScrollTrack.connect( SigC::slot( *this, &MatterAndMiningBaseWindow::scrollTrack ));
         }

         for ( int r = 0; r < 3; ++r )
            if ( container()->maxplus.resource(r) ) {
               slider->SetPosition( 100 * container()->plus.resource(r) / container()->maxplus.resource(r) );
               break;
            }

         showResourceTable( getOutput() );
      }
};

class MatterConversionWindow : public MatterAndMiningBaseWindow
{
   protected:
      bool hasFunction( const ContainerBase* container ) {
         return container->baseType->hasFunction( ContainerBaseType::MatterConverter );
      };

      Resources getOutput() {
         return container()->plus;
      }

      Resources getOutput( int rate ) {
         Resources r;
         for ( int i = 0; i < Resources::count; ++i )
            r.resource(i) = rate * container()->maxplus.resource(i) / 100;
         return r;
      }


   public:

      ASCString getASCTXTname() {
         return "conventionalpower";
      };

      ASCString getFullName() {
         return "Power Generation";
      };

};



class MiningGraph : public GraphWidget
{
      ContainerBase* cont;
      GetMiningInfo::MiningInfo mininginfo;
   protected:

      int getBarHeight( int bar ) {
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
      MiningGraph( PG_Widget *parent, const PG_Rect& rect, ContainerBase* container ) : GraphWidget( parent, rect ), cont( container ) {
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


class MiningWindow : public MatterAndMiningBaseWindow
{
   protected:
      bool hasFunction( const ContainerBase* container ) {
         return container->baseType->hasFunction( ContainerBaseType::MiningStation );
      };

      bool invertSlider() {
         return false;
      };

      Resources getOutput() {
         Resources r;
         MiningStation miningStation ( container(), true );

         for ( int i = 0; i <3; ++i ) {
            Resources plus = miningStation.getPlus();
            if ( plus.resource(i) > 0 )
               r.resource(i) = plus.resource(i);

            Resources usage = miningStation.getUsage();
            if ( usage.resource(i) > 0 )
               r.resource(i) = -usage.resource(i);
         }
         return r;
      }

      Resources getOutput( int rate ) {
         ContainerBase* c = container();
         Resources temp = c->plus;

         for ( int r = 0; r < Resources::count; r++ )
            c->plus.resource(r) = c->maxplus.resource(r) * rate/100;


         Resources r;
         MiningStation miningStation ( container(), true );

         for ( int i = 0; i <3; ++i ) {
            Resources plus = miningStation.getPlus();
            if ( plus.resource(i) > 0 )
               r.resource(i) = plus.resource(i);

            Resources usage = miningStation.getUsage();
            if ( usage.resource(i) > 0 )
               r.resource(i) = -usage.resource(i);
         }

         c->plus = temp;

         return r;
      }

   public:

      ASCString getASCTXTname() {
         return "mining";
      };

      ASCString getFullName() {
         return "Mining Facility";
      };


      void update() {
         MatterAndMiningBaseWindow::update();
      }
};



class DamageBarWidget : public PG_ThemeWidget
{
   private:
      ContainerBase* container;
   public:
      DamageBarWidget (PG_Widget *parent, const PG_Rect &rect, ContainerBase* container ) : PG_ThemeWidget( parent, rect, false ) {
         this->container = container;
      };


      void repaint() {
         Update();
      }

      void eventBlit (SDL_Surface *surface, const PG_Rect &src, const PG_Rect &dst) {
         DI_Color color( container->damage * 255 / 100, (100-container->damage) * 255/100, 0  );

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
            drawLine<4> ( s, cm, SPoint(dst.x, dst.y + h+1), SPoint( dst.x + w2, dst.y + h+1));
            drawLine<4> ( s, cm, SPoint(dst.x, dst.y + dst.h - h-1), SPoint( dst.x + w2, dst.y + dst.h - h-1));
            drawLine<4> ( s, cm, SPoint(dst.x, dst.y + dst.h - h), SPoint( dst.x + w2, dst.y + dst.h - h));
            drawLine<4> ( s, cm, SPoint( dst.x + w2, dst.y), SPoint( dst.x + w2, dst.y + dst.h));
            drawLine<4> ( s, cm, SPoint( dst.x-1 + w2, dst.y), SPoint( dst.x-1 + w2, dst.y + dst.h));
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
   shutdownImmediately = false;

   sigClose.connect( SigC::slot( *this, &CargoDialog::QuitModal ));

   registerGuiFunctions( guiIconHandler );

   cb->destroyed.connect( SigC::slot( *this, &CargoDialog::containerDestroyed ));

   ciw = new CargoInfoWindow;

   subwindows.push_back( ciw );
   subwindows.push_back( new SolarPowerWindow );
   subwindows.push_back( new WindPowerWindow );
   subwindows.push_back( new MiningWindow );
   subwindows.push_back( new ResourceInfoWindow );
   // subwindows.push_back( new NetControlWindow );

   buildingControlWindow = new BuildingControlWindow;
   subwindows.push_back( buildingControlWindow );

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

      // to not block the weapon info on 800*600 screens

      if ( my_xpos + Width() > PG_Application::GetScreenWidth() - 100 )
         MoveWidget( 0, 0, false);


   } catch ( ParsingError err ) {
      errorMessage( err.getMessage() );
      return;
   } catch ( ... ) {
      errorMessage( "unknown exception" );
      return;
   }

   registerSpecialDisplay( "unitpad_unitsymbol" );


   for ( Subwindows::iterator i = subwindows.begin(); i != subwindows.end(); ++i ) {
      if ( (*i)->available( this ))
         (*i)->registerChilds( this );
      hide( (*i)->getASCTXTname() );
   }



   if ( !cb->baseType->infoImageFilename.empty() && exist( cb->baseType->infoImageFilename )) {
      PG_Image* img = dynamic_cast<PG_Image*>(FindChild( "ContainerImage", true ));
      if ( img ) {
         tnfilestream stream ( cb->baseType->infoImageFilename, tnstream::reading );
         infoImage.readImageFile( stream );
         img->SetDrawMode( PG_Draw::STRETCH );
         img->SetImage( infoImage.getBaseSurface(), false );
         img->SizeWidget( img->GetParent()->w, img->GetParent()->h );
      }
   } else {
      PG_Image* img = dynamic_cast<PG_Image*>(FindChild( "ContainerImage", true ));
      Vehicle* v = dynamic_cast<Vehicle*>(cb);
      if ( img && v )
         img->SetImage( infoImage.getBaseSurface(), false );

   }



   setLabelText( "UnitName", cb->getName() );
   if ( cb->getName() != cb->baseType->name )
      setLabelText( "UnitClass", cb->baseType->name );


   NewGuiHost::pushIconHandler( &guiIconHandler );

   activate_i(0);
   cargoChanged();
   Show();
   setupOK = true;

};


void CargoDialog::userHandler( const ASCString& label, PropertyReadingContainer& pc, PG_Widget* parent, WidgetParameters widgetParams )
{
   if ( label == "ButtonPanel" ) {
      int x = 0;
      int y = 0;
      for ( int i = 0; i < activesubwindows.size(); ++i ) {
         SubWinButton* button = new SubWinButton( parent, SPoint( x, y ), activesubwindows[i] );
         button->sigClick.connect( SigC::bind( SigC::slot( *this, &CargoDialog::activate_i  ), i));
         if ( x + 2*SubWinButton::buttonwidth < parent->Width() )
            x += SubWinButton::buttonwidth;
         else {
            x = 0;
            y += SubWinButton::buttonheight;
         }
         subWinButtons.push_back( button );
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

   if ( label == "MiningGraph" ) {
      MiningGraph* mg = new MiningGraph( parent, PG_Rect( 0, 0, parent->Width(), parent->Height() ), container );
      new PG_ToolTipHelp( mg, "Horizontal: distance from building ; vertical: amount of resources(M/F)");

   }

   if ( label == "DamageBar" ) {
      DamageBarWidget* dbw = new DamageBarWidget( parent, PG_Rect( 0, 0, parent->Width(), parent->Height() ), container );
      buildingControlWindow->damageChanged.connect( SigC::slot( *dbw, &DamageBarWidget::repaint ));
   }

   if ( label == "ScrollArea" ) {
      PG_Widget* unitScrollArea = parent;
      if ( unitScrollArea ) {
         cargoWidget = new CargoWidget( unitScrollArea, PG_Rect( 1, 1, unitScrollArea->Width() -2 , unitScrollArea->Height() -2 ), container, false );
         cargoWidget->enableDragNDrop( true );
         cargoWidget->sigDragDone.connect( SigC::slot( *this, &CargoDialog::dragUnitToInnerContainer ));
         cargoWidget->sigDragAvail.connect( SigC::slot( *this, &CargoDialog::dragUnitToInnerContainerAvail ));
         cargoWidget->sigDragInProcess.connect( SigC::slot( *this, &CargoDialog::dragInProcess ));
         cargoWidget->sigDragAborted.connect( SigC::slot( *this, &CargoDialog::dragAborted ));

         vector<StoringPosition*> storingPositionVector;

         int x = 0;
         int y = 0;

         int posNum = container->baseType->maxLoadableUnits;
         if ( container->getCargo().size() > posNum )
            posNum = container->getCargo().size();

         int unitColumnCount = 0;
         for ( int i = 0; i < posNum; ++i ) {
            pc.openBracket( "UnitSlot" );

            int unitposx, unitposy;
            pc.addInteger( "unitposx", unitposx );
            pc.addInteger( "unitposy", unitposy );

            StoringPosition* sp = new StoringPosition( cargoWidget, PG_Point( x, y), PG_Point(unitposx, unitposy), cargoWidget->getHighLightingManager(), container->getCargo(), i, container->baseType->maxLoadableUnits >= container->getCargo().size(), cargoWidget );
            storingPositionVector.push_back( sp );
            x += sp->Width();
            if ( x + sp->Width() >= parent->Width() - 20 ) {
               if ( !unitColumnCount )
                  unitColumnCount = i + 1;
               x = 0;
               y += sp->Height();
            }


            widgetParams.runTextIO( pc );
            parsePanelASCTXT( pc, sp, widgetParams );
            pc.closeBracket();

         }

         cargoWidget->registerStoringPositions( storingPositionVector, unitColumnCount );
         cargoWidget->sigScrollTrack.connect( SigC::slot( *this, &CargoDialog::clearSmallIcons ));

         cargoWidget->unitMarked.connect( SigC::slot( *this, &CargoDialog::checkStoringPosition ));
         cargoWidget->unitMarked.connect( SigC::hide<Vehicle*>( SigC::slot( *ciw, &CargoInfoWindow::update )));
         if ( mainScreenWidget && mainScreenWidget->getGuiHost() )
            cargoWidget->unitMarked.connect( SigC::hide<Vehicle*>( SigC::slot( *this, &CargoDialog::clearSmallIcons )));

         cargoWidget->unitClicked.connect ( SigC::slot( *this, &CargoDialog::onUnitClick ));

         container->cargoChanged.connect( SigC::slot( *cargoWidget, &CargoWidget::redrawAll ));
      }
   }
   if ( label == "UnitTypeList" ) {
      int y = 0;
      parent->SetTransparency(255);
      for ( int i = 0; i < cmovemalitypenum; ++i ) {
         if ( container->baseType->vehicleCategoriesStorable & (1<<i)) {
            PG_Image* img = new PG_Image( parent, PG_Point(0, y),IconRepository::getIcon(moveMaliTypeIcons[i] ).getBaseSurface(), false);
            new PG_ToolTipHelp( img, cmovemalitypes[i] );
            y += img->Height();
         }
      }

   }

};


//*****************************************************************************************************
//*****************************************************************************************************
//
//  GUI FUNCTIONS
//
//*****************************************************************************************************
//*****************************************************************************************************






namespace CargoGuiFunctions
{


bool Movement::checkForKey( const SDL_KeyboardEvent* key, int modifier, int num )
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
   Vehicle* unit = dynamic_cast<Vehicle*>(subject);
   if ( unit && unit->getOwner() == unit->getMap()->actplayer )
      return unit->canMove();
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


   MoveUnitCommand* move = new MoveUnitCommand( unit );

   ActionResult res = move->searchFields ();
   if ( !res.successful() ) {
      dispmessage2 ( res.getCode(), NULL );
      delete move;
      return;
   }

   for ( set<MapCoordinate3D>::const_iterator i = move->getReachableFields().begin(); i != move->getReachableFields().end(); ++i )
      unit->getMap()->getField( *i)->a.temp = 1;

   for ( set<MapCoordinate3D>::const_iterator i = move->getReachableFieldsIndirect().begin(); i != move->getReachableFieldsIndirect().end(); ++i )
      unit->getMap()->getField( *i)->a.temp2 = 2;

   repaintMap();
   NewGuiHost::pendingCommand = move;

   GuiIconHandler guiIconHandler;
   guiIconHandler.registerUserFunction( new MovementDestination( *mainScreenWidget ) );
   guiIconHandler.registerUserFunction( new CancelMovement( *mainScreenWidget ) );

   NewGuiHost::pushIconHandler( &guiIconHandler );

   parent.Hide();

   mainScreenWidget->Update();
   mainScreenWidget->RunModal();
   parent.getMap()->cleartemps(7);

   NewGuiHost::popIconHandler();
   parent.cargoChanged();
   parent.Show();

}


//////////////////////////////////////////////////////////////////////////////////////////////


bool UnitProduction::available( const MapCoordinate& pos, ContainerBase* subject, int num )
{
   if ( parent.getContainer()->getOwner() == parent.getContainer()->getMap()->actplayer )
      return ConstructUnitCommand::internalConstructionAvail( parent.getContainer() );

   return false;
}


bool UnitProduction::checkForKey( const SDL_KeyboardEvent* key, int modifier, int num )
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

void UnitProduction::productionLinesChanged()
{
   if (constructUnitCommand ) {
      producables.clear();
      ConstructUnitCommand::Producables temp = constructUnitCommand->getProduceableVehicles();
      producables.insert( producables.end(), temp.begin(), temp.end () );
   }
}

void UnitProduction::execute( const MapCoordinate& pos, ContainerBase* subject, int num )
{
   bool refillAmmo;
   bool refillResources;

   auto_ptr<ConstructUnitCommand> production ( new ConstructUnitCommand( parent.getContainer() ));
   constructUnitCommand = production.get();

   const Vehicletype* v;
   {
      production->setMode( ConstructUnitCommand::internal );
      producables = production->getProduceableVehicles();
      VehicleProduction_SelectionWindow fsw( NULL, PG_Rect( 10, 10, 450, 550 ), parent.getContainer(), producables, true );
      fsw.reloadProducebles.connect( SigC::slot( *this, &UnitProduction::productionLinesChanged ));
      fsw.SetTransparency(0);
      fsw.Show();
      fsw.RunModal();
      v = fsw.getVehicletype();
      refillAmmo = fsw.fillWithAmmo();
      refillResources = fsw.fillWithResources();
   }
   if ( v ) {

      for ( ConstructUnitCommand::Producables::const_iterator i = producables.begin(); i != producables.end(); ++i )
         if ( i->type == v ) {
            if ( i->prerequisites.getValue() & ( ConstructUnitCommand::Lack::Energy  | ConstructUnitCommand::Lack::Material | ConstructUnitCommand::Lack::Fuel )) {
               warningMessage("Not enough resources to build unit");
               return;
            }

            if ( i->prerequisites.getValue() & ( ConstructUnitCommand::Lack::Movement )) {
               warningMessage("Not enough movement to build unit");
               return;
            }

            if ( i->prerequisites.getValue() & ( ConstructUnitCommand::Lack::Research )) {
               warningMessage("This unit has not been researched yet");
               return;
            }
         }

      production->setVehicleType( v );
      ActionResult res = production->execute ( createContext( parent.getContainer()->getMap() ) );

      if ( !res.successful() )
         throw res;

      production.release();

      Vehicle* newUnit = constructUnitCommand->getProducedUnit();
      if ( !newUnit )
         throw ActionResult( 21804 );


      if ( refillAmmo || refillResources ) {
         auto_ptr<ServiceCommand> ser ( new ServiceCommand( parent.getContainer() ));
         ser->setDestination( newUnit );
         TransferHandler& trans = ser->getTransferHandler();
         if ( refillAmmo )
            trans.fillDestAmmo();

         if ( refillResources )
            trans.fillDestResource();

         ser->saveTransfers();
         ActionResult res = ser->execute( createContext(  newUnit->getMap() ));
         if ( res.successful() )
            ser.release();
         else
            displayActionError(res);

      }
      if ( CGameOptions::Instance()->unitProduction.fillAmmo != refillAmmo ) {
         CGameOptions::Instance()->unitProduction.fillAmmo = refillAmmo;
         CGameOptions::Instance()->setChanged();
      }
      if ( CGameOptions::Instance()->unitProduction.fillResources != refillResources ) {
         CGameOptions::Instance()->unitProduction.fillResources = refillResources;
         CGameOptions::Instance()->setChanged();
      }

   }
   parent.cargoChanged(); // we need to update the resources because new production lines may have been build
   constructUnitCommand = NULL;
}


//////////////////////////////////////////////////////////////////////////////////////////////


bool UnitTraining::available( const MapCoordinate& pos, ContainerBase* subject, int num )
{
   if ( !subject )
      return false;

   Vehicle* veh = dynamic_cast<Vehicle*>(subject);
   if ( !veh )
      return false;

   GameMap* map = parent.getContainer()->getMap();
   if ( map->actplayer == veh->getOwner() || map->actplayer == parent.getContainer()->getOwner() )
      if ( map->player[map->actplayer].diplomacy.isAllied( veh->getOwner() ))
         return TrainUnitCommand::avail( parent.getContainer(), veh);

   return false;
}


bool UnitTraining::checkForKey( const SDL_KeyboardEvent* key, int modifier, int num )
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

   auto_ptr<TrainUnitCommand> tuc ( new TrainUnitCommand( parent.getContainer() ));
   tuc->setUnit( veh );
   ActionResult res = tuc->execute( createContext( veh->getMap()));
   if ( res.successful() ) {
      tuc.release();
      parent.cargoChanged();
   } else
      displayActionError( res );
}


//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////


bool RefuelUnitCommand::available( const MapCoordinate& pos, ContainerBase* subject, int num )
{
   if ( !subject )
      return false;

   Vehicle* veh = dynamic_cast<Vehicle*>(subject);
   if ( !veh )
      return false;

   ServiceCommand sc( parent.getContainer() );
   const ServiceTargetSearcher::Targets& dest  = sc.getDestinations();

   return find( dest.begin(), dest.end(), subject )  != dest.end();
}


bool RefuelUnitCommand::checkForKey( const SDL_KeyboardEvent* key, int modifier, int num )
{
   return ( key->keysym.sym == 'f' );
};

Surface& RefuelUnitCommand::getImage( const MapCoordinate& pos, ContainerBase* subject, int num )
{
   return IconRepository::getIcon("refuel.png");
};

ASCString RefuelUnitCommand::getName( const MapCoordinate& pos, ContainerBase* subject, int num )
{
   return "refuel unit";
};


void RefuelUnitCommand::execute( const MapCoordinate& pos, ContainerBase* subject, int num )
{
   if ( !subject )
      return;

   Vehicle* veh = dynamic_cast<Vehicle*>(subject);
   if ( !veh )
      return;

   auto_ptr<ServiceCommand> ser ( new ServiceCommand( parent.getContainer() ));
   ser->setDestination( subject );
   ser->getTransferHandler().fillDest();
   ser->saveTransfers();
   ActionResult res = ser->execute( createContext(  subject->getMap() ));
   if ( res.successful() )
      ser.release();
   else
      displayActionError(res);
   parent.cargoChanged();
}
//////////////////////////////////////////////////////////////////////////////////////////////

bool RepairUnit::available( const MapCoordinate& pos, ContainerBase* subject, int num )
{
   if ( !subject )
      return false;

   if ( !RepairUnitCommand::availInternally( parent.getContainer() ) )
      return false;

   Vehicle* veh = dynamic_cast<Vehicle*>(subject);
   if ( !veh )
      return false;


   return veh->damage > 0;
}


bool RepairUnit::checkForKey( const SDL_KeyboardEvent* key, int modifier, int num )
{
   return ( key->keysym.sym == 'r' );
};

Surface& RepairUnit::getImage( const MapCoordinate& pos, ContainerBase* subject, int num )
{
   return IconRepository::getIcon("repair.png");
};

ASCString RepairUnit::getName( const MapCoordinate& pos, ContainerBase* subject, int num )
{
   Vehicle* veh = dynamic_cast<Vehicle*>(subject);
   if ( veh && parent.getContainer() ) {
      Resources r;
      parent.getContainer()->getMaxRepair ( veh, 0, r);
      return "~r~epair unit (cost: " + r.toString() + ")";
   }

   return "repair unit";
};


void RepairUnit::execute( const MapCoordinate& pos, ContainerBase* subject, int num )
{
   if ( !subject )
      return;

   Vehicle* veh = dynamic_cast<Vehicle*>(subject);
   if ( !veh )
      return;

   auto_ptr<RepairUnitCommand> rp ( new RepairUnitCommand( parent.getContainer() ));

   if ( !rp->validTarget( veh ) )
      return;

   rp->setTarget( veh );
   ActionResult res = rp->execute( createContext ( veh->getMap() ));
   if ( res.successful() )
      rp.release();

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

   return CargoMoveCommand::moveOutAvail( veh );
}


bool MoveUnitUp::checkForKey( const SDL_KeyboardEvent* key, int modifier, int num )
{
   return ( key->keysym.sym == 'o' );
};

Surface& MoveUnitUp::getImage( const MapCoordinate& pos, ContainerBase* subject, int num )
{
   return IconRepository::getIcon("moveunitup.png");
};

ASCString MoveUnitUp::getName( const MapCoordinate& pos, ContainerBase* subject, int num )
{
   return "move unit to ~o~uter transport";
};


void MoveUnitUp::execute( const MapCoordinate& pos, ContainerBase* subject, int num )
{
   if ( !subject )
      return;


   Vehicle* veh = dynamic_cast<Vehicle*>(subject);
   if ( !veh )
      return;

   auto_ptr<CargoMoveCommand> cargomove ( new CargoMoveCommand( veh ));
   cargomove->setMode( CargoMoveCommand::moveOutwards );
   ActionResult res = cargomove->execute ( createContext( veh->getMap() ));
   if ( res.successful() )
      cargomove.release();

   parent.cargoChanged();
}

//////////////////////////////////////////////////////////////////////////////////////////////


bool CloseDialog::available( const MapCoordinate& pos, ContainerBase* subject, int num )
{
   return true;
}


bool CloseDialog::checkForKey( const SDL_KeyboardEvent* key, int modifier, int num )
{
   return ( key->keysym.sym == 'x' );
};

Surface& CloseDialog::getImage( const MapCoordinate& pos, ContainerBase* subject, int num )
{
   return IconRepository::getIcon("closecargodialog.png");
};

ASCString CloseDialog::getName( const MapCoordinate& pos, ContainerBase* subject, int num )
{
   return "close dialog";
};


void CloseDialog::execute( const MapCoordinate& pos, ContainerBase* subject, int num )
{
   parent.QuitModal();
}

//////////////////////////////////////////////////////////////////////////////////////////////


bool UnitInfo::available( const MapCoordinate& pos, ContainerBase* subject, int num )
{
   if ( !subject )
      return false;

   Vehicle* veh = dynamic_cast<Vehicle*>(subject);
   return veh;
}


bool UnitInfo::checkForKey( const SDL_KeyboardEvent* key, int modifier, int num )
{
   return ( key->keysym.sym == 'i' );
};

Surface& UnitInfo::getImage( const MapCoordinate& pos, ContainerBase* subject, int num )
{
   return IconRepository::getIcon("unitinfo.png");
};

ASCString UnitInfo::getName( const MapCoordinate& pos, ContainerBase* subject, int num )
{
   return "show unit ~i~nformation";
};


void UnitInfo::execute( const MapCoordinate& pos, ContainerBase* subject, int num )
{
   Vehicle* veh = dynamic_cast<Vehicle*>(subject);
   if ( veh )
      unitInfoDialog( veh->typ );
}

//////////////////////////////////////////////////////////////////////////////////////////////


bool TransferUnitControl::available( const MapCoordinate& pos, ContainerBase* subject, int num )
{
   if ( !subject )
      return false;

   if ( subject->getOwner() != subject->getMap()->actplayer )
      return false;

   Vehicle* veh = dynamic_cast<Vehicle*>(subject);
   if ( !veh )
      return false;

   return TransferControlCommand::avail( subject );

}


Surface& TransferUnitControl::getImage( const MapCoordinate& pos, ContainerBase* subject, int num )
{
   return IconRepository::getIcon("transferunitcontrol.png");
};

ASCString TransferUnitControl::getName( const MapCoordinate& pos, ContainerBase* subject, int num )
{
   return "transfer unit control";
};


void TransferUnitControl::execute( const MapCoordinate& pos, ContainerBase* subject, int num )
{
   Vehicle* veh = dynamic_cast<Vehicle*>(subject);
   if ( veh ) {

      auto_ptr<TransferControlCommand> tcc ( new TransferControlCommand( subject ));
      TransferControlCommand::Receivers rec = tcc->getReceivers();

      vector<ASCString> entries;
      for ( TransferControlCommand::Receivers::iterator i = rec.begin(); i != rec.end(); ++i )
         entries.push_back ( (*i)->getName() );

      if ( !entries.size() ) {
         infoMessage("you don't have any allies!");
         return;
      }

      int result = chooseString ( "Choose player", entries );
      if ( result >= 0 ) {
         tcc->setReceiver( rec[result] );
         ActionResult res = tcc->execute( createContext( subject->getMap() ));
         if ( res.successful() ) {
            tcc.release();
            parent.cargoChanged();
         } else
            displayActionError( res );
      }


   }
}

//////////////////////////////////////////////////////////////////////////////////////////////



bool MoveUnitIntoInnerContainer::available( const MapCoordinate& pos, ContainerBase* subject, int num )
{
   if ( !subject )
      return false;

   Vehicle* veh = dynamic_cast<Vehicle*>(subject);
   if ( !veh )
      return false;

   CargoMoveCommand cmc( veh );
   return cmc.getTargetCarriers().size() > 0 ;
}


bool MoveUnitIntoInnerContainer::checkForKey( const SDL_KeyboardEvent* key, int modifier, int num )
{
   return ( key->keysym.sym == 'u' );
};

Surface& MoveUnitIntoInnerContainer::getImage( const MapCoordinate& pos, ContainerBase* subject, int num )
{
   return IconRepository::getIcon("container-in.png");
};

ASCString MoveUnitIntoInnerContainer::getName( const MapCoordinate& pos, ContainerBase* subject, int num )
{
   return "move ~u~nit into neighboring transport";
};



class VehicleWidget: public VehicleTypeBaseWidget
{
   private:
      Vehicle* veh;
   public:
      VehicleWidget( PG_Widget* parent, const PG_Point& pos, int width, Vehicle* unit )
            : VehicleTypeBaseWidget( parent, pos, width, unit->typ, unit->getMap()->getPlayer(unit) ) {
         veh = unit;
      };

      Vehicle* getUnit() const {
         return veh;
      };
};

class VehicleSelectionFactory: public SelectionItemFactory, public SigC::Object
{
   public:
      typedef vector<Vehicle*> Container;
   protected:
      Container::iterator it;
      Container& items;

   public:
      VehicleSelectionFactory( Container& units ) : items ( units ), selected(NULL) {};
      void restart() {
         it = items.begin();
      };

      SelectionWidget* spawnNextItem( PG_Widget* parent, const PG_Point& pos ) {
         if ( it != items.end() ) {
            Vehicle* v = *(it++);
            return new VehicleWidget( parent, pos, parent->Width() - 15, v );
         } else
            return NULL;
      };

      void itemSelected( const SelectionWidget* widget, bool mouse ) {
         if ( !widget )
            return;

         const VehicleWidget* vw = dynamic_cast<const VehicleWidget*>(widget);
         assert( vw );
         selected = vw->getUnit();
      }

      Vehicle* selected;
};


Vehicle* selectVehicle( vector<Vehicle*> targets )
{
   Vehicle* target = NULL;
   VehicleSelectionFactory* vsf = new VehicleSelectionFactory( targets );
   ItemSelectorWindow isw ( NULL, PG_Rect( 50, 50, 300, 400), "Please choose target unit", vsf );
   isw.Show();
   isw.RunModal();
   target = vsf->selected;
   return target;
}




void MoveUnitIntoInnerContainer::execute( const MapCoordinate& pos, ContainerBase* subject, int num )
{
   if ( !subject )
      return;


   Vehicle* veh = dynamic_cast<Vehicle*>(subject);
   if ( !veh )
      return;

   auto_ptr<CargoMoveCommand> cargomove ( new CargoMoveCommand( veh ));
   cargomove->setMode( CargoMoveCommand::moveInwards );

   vector<Vehicle*> targets = cargomove->getTargetCarriers();

   choiceDialog("You can also use Drag'n'Drop to move units in the Cargo Dialog\nMouse button: " + CGameOptions::Mouse::getButtonName( CGameOptions::Instance()->mouse.dragndropbutton ), "OK", "", "dragndropinfo");

   Vehicle* target = selectVehicle( targets );
   if ( target ) {
      cargomove->setTargetCarrier( target );
      ActionResult res = cargomove->execute ( createContext( veh->getMap() ));
      if ( res.successful() )
         cargomove.release();
   }
   parent.cargoChanged();
}



//////////////////////////////////////////////////////////////////////////////////////////////

bool RefuelUnitDialogCommand :: available( const MapCoordinate& pos, ContainerBase* subject, int num )
{
   if ( !subject )
      return false;

   Vehicle* veh = dynamic_cast<Vehicle*>(subject);
   if ( !veh )
      return false;

   ServiceCommand sc( parent.getContainer() );
   const ServiceTargetSearcher::Targets& dest  = sc.getDestinations();

   return find( dest.begin(), dest.end(), subject )  != dest.end();
};

void RefuelUnitDialogCommand :: execute( const MapCoordinate& pos, ContainerBase* subject, int num )
{
   if ( !subject )
      return;

   auto_ptr<ServiceCommand> ser ( new ServiceCommand( parent.getContainer() ));
   ser->setDestination( subject );
   ammoTransferWindow( parent.getContainer(), subject, ser.get() );
   parent.cargoChanged();
   if ( ser->getState() == Command::Completed )
      ser.release();
}

Surface& RefuelUnitDialogCommand :: getImage( const MapCoordinate& pos, ContainerBase* subject, int num )
{
   return IconRepository::getIcon("refuel-dialog.png");
};

bool RefuelUnitDialogCommand :: checkForKey( const SDL_KeyboardEvent* key, int modifier, int num )
{
   return ( key->keysym.unicode == 'd' );
};

ASCString RefuelUnitDialogCommand :: getName( const MapCoordinate& pos, ContainerBase* subject, int num )
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
   if ( veh->typ->maxLoadableUnits && player.diplomacy.isAllied( veh->getMap()->actplayer)  )
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

bool OpenContainer :: checkForKey( const SDL_KeyboardEvent* key, int modifier, int num )
{
   return ( key->keysym.unicode == 'l' );
};

ASCString OpenContainer :: getName( const MapCoordinate& pos, ContainerBase* subject, int num )
{
   return "open transport ";
};




//////////////////////////////////////////////////////////////////////////////////////////////


bool RecycleUnitCommandButton :: available( const MapCoordinate& pos, ContainerBase* subject, int num )
{
   if ( !subject )
      return false;

   Vehicle* veh = dynamic_cast<Vehicle*>(subject);
   if ( !veh )
      return false;

   if ( !RecycleUnitCommand::avail( parent.getContainer(), veh))
      return false;

   Player& player = veh->getMap()->player[veh->getOwner()];
   if ( player.diplomacy.isAllied( veh->getMap()->actplayer)  )
      return true;
   else
      return false;
};

ASCString RecycleUnitCommandButton :: getCommandName( const ContainerBase* carrier )
{
   if ( carrier->baseType->hasFunction( ContainerBaseType::RecycleUnits ))
      return "recycle";
   else
      return "salvage";
}


void RecycleUnitCommandButton :: execute( const MapCoordinate& pos, ContainerBase* subject, int num )
{
   if ( !subject )
      return;

   Vehicle* veh = dynamic_cast<Vehicle*>(subject);
   if ( !veh )
      return;

   ASCString msg = "do you really want to " + getCommandName( parent.getContainer() ) + " this unit ?";

   if (choice_dlg( msg ,"~y~es","~n~o") == 1) {
      auto_ptr<RecycleUnitCommand> command ( new RecycleUnitCommand( parent.getContainer() ));
      command->setUnit( veh );
      ActionResult res = command->execute( createContext( parent.getContainer()->getMap() ));
      if ( res.successful() )
         command.release();
      else
         displayActionError( res );
      parent.cargoChanged();
   }
}


Surface& RecycleUnitCommandButton :: getImage( const MapCoordinate& pos, ContainerBase* subject, int num )
{
   return IconRepository::getIcon("recycle.png");
};

bool RecycleUnitCommandButton :: checkForKey( const SDL_KeyboardEvent* key, int modifier, int num )
{
   return false;
};

ASCString RecycleUnitCommandButton :: getName( const MapCoordinate& pos, ContainerBase* subject, int num )
{
   if ( !subject )
      return "";

   Vehicle* veh = dynamic_cast<Vehicle*>(subject);
   if ( !veh )
      return "";

   ContainerControls cc ( parent.getContainer() );

   ASCString s = getCommandName( parent.getContainer() ) + " unit - ";

   Resources res = RecycleUnitCommand::getOutput( parent.getContainer(), veh );

   bool cost = false;
   for ( int r = 0; r < 3; ++r ) {
      if ( res.resource(r) < 0 ) {
         if ( !cost ) {
            s += "Cost: ";
            cost = true;
         }
         s += ASCString::toString( -res.resource( r ));
         s += " ";
         s += Resources::name( r );
      }
   }

   bool gain = false;
   for ( int r = 0; r < 3; ++r ) {
      if ( res.resource(r) > 0 ) {
         if ( !gain ) {
            s += "Gain: ";
            cost = true;
         }
         s += ASCString::toString( res.resource( r ));
         s += " ";
         s += Resources::name( r );
      }
   }

   return s;

};

}

