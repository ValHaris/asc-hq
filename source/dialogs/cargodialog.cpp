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

#include "selectionwindow.h"

class HighLightingManager
{
      int marked;
   public:
      HighLightingManager() : marked(-1)
      {}
      ;
      int getMark()
      {
         return marked;
      };
      SigC::Signal2<void,int,int> markChanged;
      void setNew(int pos )
      {
         int old = marked;
         marked = pos;
         markChanged(old,pos);
      };

      SigC::Signal0<void> redrawAll;
};

const Vehicletype* selectVehicletype( ContainerBase* plant, const vector<Vehicletype*>& items );


typedef vector<Vehicle*> StorageVector;

class StoringPosition : public PG_Widget
{
      static Surface clippingSurface;
      HighLightingManager& highlight;
      StorageVector& storage;
      int num;
      bool regular;
   protected:
      void markChanged(int old, int mark)
      {
         if ( num == old || num == mark )
            Update();
      }

      bool eventMouseButtonDown (const SDL_MouseButtonEvent *button)
      {
         if ( button->type == SDL_MOUSEBUTTONDOWN && button->button == SDL_BUTTON_LEFT ) {
            highlight.setNew( num );
            return true;
         }
         return false;
      }

   public:

      static const int spWidth = 64;
      static const int spHeight = 64;

      StoringPosition( PG_Widget *parent, const PG_Point &pos, HighLightingManager& highLightingManager, StorageVector& storageVector, int number, bool regularPosition  )
            : PG_Widget ( parent, PG_Rect( pos.x, pos.y, spWidth, spHeight)), highlight( highLightingManager ), storage( storageVector), num(number), regular(regularPosition)
      {
         if ( !clippingSurface.valid() )
            clippingSurface = Surface::createSurface( spWidth + 10, spHeight + 10, 32, 0 );

         highlight.markChanged.connect( SigC::slot( *this, &StoringPosition::markChanged ));
         highlight.redrawAll.connect( SigC::bind( SigC::slot( *this, &StoringPosition::Update), true));
      }




      void eventBlit (SDL_Surface *surface, const PG_Rect &src, const PG_Rect &dst)
      {
         clippingSurface.Fill(0);

         ASCString background = "hexfield-bld-";
         background += regular ? "1" : "2";
         if (  num == highlight.getMark() )
            background += "h";
         background += ".png";

         Surface& icon = IconRepository::getIcon( background );

         MegaBlitter<4,4,ColorTransform_None, ColorMerger_AlphaOverwrite> blitter;
         blitter.blit( icon, clippingSurface, SPoint(0,0));

         if ( num < storage.size() && storage[num] ) {
            int ypos;
            if ( num == highlight.getMark() )
               ypos = 0;
            else
               ypos = 1;

            if( storage[num]->getMovement() > 0  )
               storage[num]->typ->paint( clippingSurface, SPoint(0,ypos), storage[num]->getOwner() );
            else
               storage[num]->typ->paint( clippingSurface, SPoint(0,ypos), storage[num]->getMap()->getNeutralPlayerNum() );
         }

         PG_Draw::BlitSurface( clippingSurface.getBaseSurface(), src, PG_Application::GetScreen(), dst);
      }
};

Surface StoringPosition::clippingSurface;





const int subWindowNum = 11;
static const char* subWindowName[subWindowNum] =
   { "ammotransfer", "ammoproduction", "info", "cargoinfo", "conventionalpower", "mining", "netcontrol", "research", "resourceinfo", "solarpower", "windpower"
   };

class SubWinButton : public PG_Button
{
   public:
      static const int buttonwidth = 47;
      static const int buttonheight = 23;

      SubWinButton( PG_Widget *parent, const SPoint& pos, int subWindow ) : PG_Button( parent, PG_Rect( pos.x, pos.y, buttonwidth, buttonheight ), "", -1, "SubWinButton")
      {
         SetBackground( PRESSED, IconRepository::getIcon("cargo-buttonpressed.png").getBaseSurface() );
         SetBackground( HIGHLITED, IconRepository::getIcon("cargo-buttonhighlighted.png").getBaseSurface() );
         SetBackground( UNPRESSED, IconRepository::getIcon("cargo-buttonunpressed.png").getBaseSurface() );
         SetBorderSize(0,0,0);
         SetIcon( IconRepository::getIcon(ASCString("cargo-") + subWindowName[subWindow] + ".png" ).getBaseSurface() );
      };
};


class CargoDialog;

class SubWindow
{
   public:
      virtual bool available( CargoDialog* cd ) = 0;
      virtual void registerSubwindow( CargoDialog* cd )
      {}
      ;
      virtual ~SubWindow()
      {}
      ;
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

}; // namespace CargoGuiFunctions


class CargoDialog : public Panel
{

      ContainerControls containerControls;
   
      GuiIconHandler guiIconHandler;
      
      ContainerBase* container;
      bool setupOK;
      int unitColumnCount;
      Surface infoImage;

      SigC::Signal0<void>  sigCargoChanged;
      
      vector<StoringPosition*> storingPositionVector;

      HighLightingManager unitHighLight;

      deallocating_vector<SubWindow*> subwindows;

      StorageVector& loadedUnits;

      void moveSelection( int delta )
      {
         int newpos = unitHighLight.getMark() + delta;

         if ( newpos < 0 )
            newpos = 0;

         if ( newpos >= storingPositionVector.size() )
            newpos = storingPositionVector.size() -1 ;

         if ( newpos != unitHighLight.getMark() )
            unitHighLight.setNew( newpos );
      }

      bool eventKeyDown(const SDL_KeyboardEvent* key)
      {
         if ( key->keysym.sym == SDLK_ESCAPE ) {
            QuitModal();
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
            moveSelection(-unitColumnCount);
            return true;
         }
         if ( key->keysym.sym == SDLK_DOWN )  {
            moveSelection(unitColumnCount);
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
         activate( subWindowName[pane] );
         return true;
      }

      void activate( const ASCString& pane )
      {
         PG_Application::SetBulkMode();
         for ( int i = 0; i < subWindowNum; ++i )
            if ( ASCString( subWindowName[i]) != pane )
               hide( subWindowName[i] );

         for ( int i = 0; i < subWindowNum; ++i )
            if ( ASCString( subWindowName[i]) == pane )
               show( subWindowName[i] );
         PG_Application::SetBulkMode(false);
         Update();
      };

      void updateResourceDisplay()
      {
         setLabelText( "energyavail", container->getResource(maxint, 0, true ) );
         setLabelText( "materialavail", container->getResource(maxint, 1, true ) );
         setLabelText( "fuelavail", container->getResource(maxint, 2, true ) );
      }



      void checkStoringPosition( int oldpos, int newpos )
      {
         PG_ScrollWidget* unitScrollArea = dynamic_cast<PG_ScrollWidget*>(FindChild( "UnitScrollArea", true ));
         if ( unitScrollArea )
            if ( newpos < storingPositionVector.size() && newpos >= 0 )
               unitScrollArea->ScrollToWidget( storingPositionVector[newpos] );

         if ( mainScreenWidget&& mainScreenWidget->getGuiHost() ) {
            mainScreenWidget->getGuiHost()->eval( container->getPosition(), getMarkedUnit() );
         }
         
         if ( mainScreenWidget && mainScreenWidget->getUnitInfoPanel() )
            mainScreenWidget->getUnitInfoPanel()->showUnitData( getMarkedUnit(), NULL, true );

      }

      Vehicle* getMarkedUnit()
      {
         int pos = unitHighLight.getMark();
         if ( pos < 0 || pos >= loadedUnits.size() )
            return NULL;
         else
            return loadedUnits[pos];
      }

      void registerGuiFunctions( GuiIconHandler& handler )
      {
         registerCargoGuiFunctions( handler );
         handler.registerUserFunction( new CargoGuiFunctions::Movement( *this ) );
         handler.registerUserFunction( new CargoGuiFunctions::RefuelUnit( *this ) );
         handler.registerUserFunction( new CargoGuiFunctions::UnitProduction( *this ));
         handler.registerUserFunction( new CargoGuiFunctions::UnitTraining( *this ));
      }

      
   public:


      CargoDialog (PG_Widget *parent, ContainerBase* cb )
         : Panel( parent, PG_Rect::null, "cargodialog", false ), containerControls( cb ), container(cb), setupOK(false), unitColumnCount(0), loadedUnits ( cb->cargo )
      {
         sigClose.connect( SigC::slot( *this, &CargoDialog::QuitModal ));

         registerGuiFunctions( guiIconHandler );
         

         // cb->resourceChanged.connect( SigC::slot( *this, &CargoDialog::updateResourceDisplay ));
         // cb->ammoChanged.connect( SigC::slot( *this, &CargoDialog::showAmmo ));
         NewGuiHost::pushIconHandler( &guiIconHandler );

         
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


         
         unitHighLight.markChanged.connect( SigC::slot( *this, &CargoDialog::checkStoringPosition ));

         /*
         for ( int i = 0; i< 32; ++i )
            if ( cb->loading[i] )
               loadedUnits.push_back (  cb->loading[i] );
               */

         PG_Widget* unitScrollArea = FindChild( "UnitScrollArea", true );
         if ( unitScrollArea ) {
            int x = 0;
            int y = 0;
            int posNum = cb->baseType->maxLoadableUnits;
            if ( loadedUnits.size() > posNum )
               posNum = loadedUnits.size();

            for ( int i = 0; i < posNum; ++i ) {
               storingPositionVector.push_back( new StoringPosition( unitScrollArea, PG_Point( x, y), unitHighLight, loadedUnits, i, cb->baseType->maxLoadableUnits >= loadedUnits.size() ));
               x += StoringPosition::spWidth;
               if ( x + StoringPosition::spWidth >= unitScrollArea->Width() - 20 ) {
                  if ( !unitColumnCount )
                     unitColumnCount = i + 1;
                  x = 0;
                  y += StoringPosition::spHeight;
               }
            }
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

         activate_i(0);
         Show();
         setupOK = true;

      };

      void cargoChanged()
      {
         //unitHighLight.setNew( unitHighLight.getMark() );
         unitHighLight.redrawAll();
         sigCargoChanged();
         updateResourceDisplay();
         showAmmo();
         if ( mainScreenWidget && mainScreenWidget->getUnitInfoPanel() )
            mainScreenWidget->getUnitInfoPanel()->showUnitData( getMarkedUnit(), NULL, true );
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

      void userHandler( const ASCString& label, PropertyReadingContainer& pc, PG_Widget* parent, WidgetParameters widgetParams )
      {
         if ( label == "ButtonPanel" ) {
            int x = 0;
            for ( int i = 0; i < subWindowNum; ++i ) {
               SubWinButton* button = new SubWinButton( parent, SPoint( x, 0 ), i);
               button->sigClick.connect( SigC::bind( SigC::slot( *this, &CargoDialog::activate_i  ), i));
               x += SubWinButton::buttonwidth;
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
         }
      };

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
            cost += Resources( 0, type->tank.material, type->tank.fuel );

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
         if ( veh->getTank().resource(r) < veh->typ->tank.resource(r))
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

   
}
