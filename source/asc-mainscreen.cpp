
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "global.h"

#include <stdio.h>
#include <cstring>
#include <stdlib.h>
#include <new>
#include <cstdlib>
#include <ctype.h>
#include <algorithm>
#include <memory>
#include <SDL_image.h>
#include <SDL_mixer.h>

#include <pgpopupmenu.h>
#include <pgmenubar.h>

#include "paradialog.h"

#include "misc.h"
#include "iconrepository.h"
#include "dashboard.h"
#include "asc-mainscreen.h"
#include "guiiconhandler.h"
#include "guifunctions.h"
#include "graphics/blitter.h"
#include "graphics/drawing.h"
#include "overviewmappanel.h"
#include "ai/ai.h"
#include "itemrepository.h"
#include "mapdisplay.h"
#include "sg.h"
#include "gameoptions.h"
#include "dialog.h"

ASC_MainScreenWidget*  mainScreenWidget = NULL ;


MainScreenWidget* getMainScreenWidget()
{
   return mainScreenWidget;
}

class Menu : public PG_MenuBar {

    PG_PopupMenu* currentMenu;
    typedef list<PG_PopupMenu*> Categories;
    Categories categories;

   public:
      Menu ( PG_Widget *parent, const PG_Rect &rect=PG_Rect::null);
      
   protected:
      void setup();   
      bool execAction  (PG_PopupMenu::MenuItem* menuItem );

   private:
      void addbutton(const char* name, int id );
      void addfield ( const char* name );
     
};








bool Menu::execAction  (PG_PopupMenu::MenuItem* menuItem )
{
   execUserAction_ev( tuseractions( menuItem->getId() ) );

   return true;
}


#define HIGHLIGHT

void Menu::addfield( const char* name )
{
   currentMenu = new PG_PopupMenu( NULL, -1, -1, "" );

#ifdef HIGHLIGHT
   categories.push_back ( currentMenu );
   Add ( name, currentMenu );
#else
   ASCString s = name;
   while ( s.find ( "~") != ASCString::npos )
      s.erase( s.find( "~"),1 );
   Add ( s, currentMenu );
#endif

   currentMenu->sigSelectMenuItem.connect( SigC::slot( *this, &Menu::execAction ));
}

void Menu::addbutton( const char* name, int id )
{
#ifdef HIGHLIGHT
   currentMenu->addMenuItem( name, id );
#else
   ASCString s = name;
   while ( s.find ( "~") != ASCString::npos )
      s.erase( s.find( "~"),1 );

   currentMenu->addMenuItem( s, id );
#endif

}


void Menu::setup()
{
   addfield ( "Glo~b~al" );
   addbutton ( "~O~ptions", ua_gamepreferences );
   addbutton ( "~S~ound options", ua_soundDialog );
   currentMenu->addSeparator();
   addbutton ( "~M~ain menu\tESC", ua_mainmenu );
   currentMenu->addSeparator();
   addbutton ( "E~x~it\tctrl-x", ua_exitgame );


   addfield ("~G~ame");
   addbutton ( "~N~ew Game\tctrl-n", ua_newGame );
   currentMenu->addSeparator();
   addbutton ( "~L~oad game\tctrl-l", ua_loadgame );
   addbutton ( "~S~ave game\tctrl-s", ua_savegame );
   currentMenu->addSeparator();
   addbutton ( "Continue network game\tF3", ua_continuenetworkgame);
   // addbutton ( "setup Net~w~ork", ua_setupnetwork );
   addbutton ( "Change Passw~o~rd", ua_changepassword );
   addbutton ( "supervise network game", ua_networksupervisor );
   currentMenu->addSeparator();
   addbutton ( "~D~iplomacy", ua_setupalliances);
   addbutton ( "transfer ~U~nit control", ua_giveunitaway );
   addbutton ( "~T~ransfer resources", ua_settribute);
   addbutton ( "~C~ancel Research", ua_cancelResearch );
   currentMenu->addSeparator();
   addbutton ( "~E~dit Player Data", ua_editPlayerData);



   addfield ( "~I~nfo" );
   addbutton ( "~V~ehicle types", ua_vehicleinfo );
   addbutton ( "Unit ~w~eapon range\t3", ua_viewunitweaponrange );
   addbutton ( "Unit ~m~ovement range\t4", ua_viewunitmovementrange );
   addbutton ( "~G~ame Time\t5", ua_GameStatus );
   addbutton ( "unit ~S~et information\t6", ua_UnitSetInfo );
   addbutton ( "~T~errain\t7", ua_viewterraininfo );
   addbutton ( "~U~nit weight\t8", ua_unitweightinfo );
   currentMenu->addSeparator();
   addbutton ( "~R~esearch", ua_researchinfo );
   addbutton ( "~P~lay time", ua_showPlayerSpeed );
   addbutton ( "~C~argo Summary", ua_cargosummary );
   addbutton ( "Unit Summary", ua_unitsummary );
   // addbutton ( "~R~esearch status", ua_showResearchStatus );

   // addbutton ( "vehicle ~I~mprovement\tF7", ua_dispvehicleimprovement);
   // addbutton ( "show game ~P~arameters", ua_GameParameterInfo );


   // addfield ( "~S~tatistics" );
   
//   addbutton ( "~R~esources ", ua_statisticdialog );
   // addbutton ( "seperator");
   // addbutton ( "~H~istory");

   addfield ( "~M~essage");
   addbutton ( "~n~ew message", ua_newmessage );
   addbutton ( "view ~q~ueued messages", ua_viewqueuedmessages );
   addbutton ( "view ~s~end messages", ua_viewsentmessages );
   addbutton ( "view ~r~eceived messages", ua_viewreceivedmessages);
   currentMenu->addSeparator();
   addbutton ( "view ~j~ournal", ua_viewjournal );
   addbutton ( "~a~ppend to journal", ua_editjournal );

   addfield ( "~T~ools" );
   addbutton ( "save ~M~ap as PCX\t9", ua_writemaptopcx );
   addbutton ( "save ~S~creen as PCX", ua_writescreentopcx );
   addbutton ( "benchmark without view calc", ua_benchgamewov );
   addbutton ( "benchmark with view calc", ua_benchgamewv);
   addbutton ( "compiler benchmark (AI)", ua_aibench );
   // addbutton ( "test memory integrity", ua_heapcheck );
   currentMenu->addSeparator();
   addbutton ( "select graphic set", ua_selectgraphicset );

   addfield ( "~V~iew" );
   addbutton ( "toggle ~R~esourceview\t1", ua_changeresourceview );
   addbutton ( "toggle unit shading\t2", ua_toggleunitshading );
   addbutton ( "show ~P~ipeline net\t9", ua_viewPipeNet );
   currentMenu->addSeparator();
   addbutton ( "Button Panel", ua_viewButtonPanel );
   addbutton ( "Wind Panel", ua_viewWindPanel );
   addbutton ( "Unit Info Panel", ua_viewUnitInfoPanel );
   addbutton ( "Overview Map Panel", ua_viewOverviewMapPanel );
   addbutton ( "Map Control Panel", ua_viewMapControlPanel );
   // addbutton("Weathercast", ua_weathercast);
   currentMenu->addSeparator();
   addbutton ( "clear image cache", ua_clearImageCache );
   addbutton ( "reload dialog theme", ua_reloadDlgTheme );
   addbutton ( "test messages", ua_testMessages );
   currentMenu->addSeparator();
   addbutton ( "Increase Map Zoom\tKP+", ua_increase_zoom );
   addbutton ( "Decrease Map Zoom\tKP-", ua_decrease_zoom );


   addfield ( "~H~elp" );
   addbutton ( "HowTo ~S~tart email games", ua_howtostartpbem );
   addbutton ( "HowTo ~C~ontinue email games", ua_howtocontinuepbem );
   addbutton ( "Show ASC search ~P~ath", ua_showsearchdirs );
   addbutton ( "SDL settings", ua_SDLinfo );
   currentMenu->addSeparator();
   addbutton ( "~K~eys", ua_help );

   addbutton ( "~A~bout", ua_viewaboutmessage );
}


Menu::Menu ( PG_Widget *parent, const PG_Rect &rect)
    : PG_MenuBar( parent, rect, "MenuBar"),
      currentMenu(NULL)
{
   activateHotkey( KMOD_ALT );
   setup();
}



class UnitWeaponRangeLayer : public MapLayer {
   Surface& icon1;
   Surface& icon2;
   GameMap* gamemap;

   map<MapCoordinate,int> fields;

   void markField( const MapCoordinate& pos )
   {
      fields[pos] |= 1;
   }

      bool addUnit( Vehicle* veh )
      {
         if ( fieldvisiblenow ( getfield ( veh->xpos, veh->ypos ))) {
            int found = 0;
            for ( int i = 0; i < veh->typ->weapons.count; i++ ) {
               if ( veh->typ->weapons.weapon[i].shootable() ) {
                  circularFieldIterator( gamemap,veh->getPosition(), veh->typ->weapons.weapon[i].maxdistance/minmalq, (veh->typ->weapons.weapon[i].mindistance+maxmalq-1)/maxmalq, FieldIterationFunctor( this, &UnitWeaponRangeLayer::markField )  );
                  found++;
               }
            }
            if ( found )
               fields[veh->getPosition()] |= 2;
            
            return found;
         } else
            return false;
      };

      void reset()
      {
         fields.clear();
      }
   public:

      void operateField( GameMap* actmap, const MapCoordinate& pos )
      {
         if ( !pos.valid() )
            return;
         
         if ( gamemap && gamemap != actmap ) 
            reset();

         gamemap = actmap;
         
         if ( fields.find( pos ) != fields.end() ) {
            if ( fields[pos] & 2 ) {
               reset();
               setActive(false);
               statusMessage("Weapon range layer disabled");
               repaintMap();
               return;
            }
         }
         
         if ( actmap->getField( pos )->vehicle ) {
            if ( addUnit( actmap->getField( pos )->vehicle ) ) {
               setActive(true);
               statusMessage("Weapon range layer enabled");
               repaintMap();
            }
         }
      }
      
      UnitWeaponRangeLayer() : icon1 ( IconRepository::getIcon( "markedfield-red.png")), icon2 ( IconRepository::getIcon( "markedfield-red2.png")), gamemap(NULL) {
         // cursorMoved.connect( SigC::slot( *this, UnitWeaponRangeLayer::cursorMoved ));
      }

      bool onLayer( int layer ) { return layer == 17; };
      
      void paintSingleField( const MapRenderer::FieldRenderInfo& fieldInfo,  int layer, const SPoint& pos )
      {
         if ( fieldInfo.gamemap != gamemap && gamemap) {
            reset();
            gamemap = NULL;
            return;
         }
         
         if ( fieldInfo.visibility >= visible_ago) {
            if ( fields.find( fieldInfo.pos ) != fields.end() ) {
               int p = fields[fieldInfo.pos];
               if ( p & 1 )
                  fieldInfo.surface.Blit( icon1, pos );
               if ( p & 2 )
                  fieldInfo.surface.Blit( icon2, pos );
            }
         }
      }
};



class UnitMovementRangeLayer : public MapLayer, public SigC::Object {
   Surface& icon;
   GameMap* gamemap;

   map<MapCoordinate,int> fields;

   void markField( const MapCoordinate& pos )
   {
      fields[pos] |= 1;
   }

   bool addUnit( Vehicle* veh )
   {

      if ( fieldvisiblenow ( getfield ( veh->xpos, veh->ypos ))) {
         int counter = 0;
         VehicleMovement vm ( NULL, NULL );


         int orgMovement = veh->getMovement(false);
         veh->setMovement( veh->maxMovement(), 0 );
         if ( vm.available ( veh )) {

            vm.execute ( veh, -1, -1, 0, -1, -1 );
            if ( vm.reachableFields.getFieldNum()) {
               for  ( int i = 0; i < vm.reachableFields.getFieldNum(); i++ )
                  if ( fieldvisiblenow ( vm.reachableFields.getField ( i ) )) {
                     ++counter;
                     markField( vm.reachableFields.getFieldCoordinates(i));
                  }
               for  ( int j = 0; j < vm.reachableFieldsIndirect.getFieldNum(); j++ )
                  if ( fieldvisiblenow ( vm.reachableFieldsIndirect.getField ( j ))) {
                     markField( vm.reachableFieldsIndirect.getFieldCoordinates(j));
                     ++counter;
                  }
   
            }

         }
         veh->setMovement( orgMovement, 0 );
      
         if ( counter )
            fields[veh->getPosition()] |= 2;
            
         return counter;
      } else
         return false;
   };

   void reset()
   {
      fields.clear();
      if ( isActive() ) {
         setActive(false);
         statusMessage("Movement range layer disabled");
         repaintMap();
      }
   }
   public:

      void operateField( GameMap* actmap, const MapCoordinate& pos )
      {
         if ( !pos.valid() )
            return;
         
         if ( gamemap && gamemap != actmap )
            fields.clear();

         gamemap = actmap;
         
         if ( fields.find( pos ) != fields.end() ) {
            if ( fields[pos] & 2 ) {
               reset();
               return;
            }
         }
         
         if ( actmap->getField( pos )->vehicle ) {
            if ( addUnit( actmap->getField( pos )->vehicle ) ) {
               setActive(true);
               statusMessage("Movement range layer enabled");
               repaintMap();
            }
         }
      }
      
      UnitMovementRangeLayer() : icon ( IconRepository::getIcon( "markedfield-blue.png")), gamemap(NULL) {
         cursorMoved.connect( SigC::slot( *this, &UnitMovementRangeLayer::reset ));
      }

      bool onLayer( int layer ) { return layer == 17; };
      
      void paintSingleField( const MapRenderer::FieldRenderInfo& fieldInfo,  int layer, const SPoint& pos )
      {
         if ( fieldInfo.gamemap != gamemap && gamemap) {
            reset();
            gamemap = NULL;
            return;
         }
         
         if ( fieldInfo.visibility >= visible_ago) {
            if ( fields.find( fieldInfo.pos ) != fields.end() ) {
               int p = fields[fieldInfo.pos];
               if ( p )
                  fieldInfo.surface.Blit( icon, pos );
            }
         }
      }
};



ASC_MainScreenWidget::ASC_MainScreenWidget( PG_Application& application )
   : MainScreenWidget( application ), standardActions(true), guiHost(NULL), menu(NULL), unitInfoPanel(NULL), windInfoPanel(NULL), mapInfoPanel(NULL)
{

   setup( true );

   displayLogMessage ( 7, "done\n  Menu ");
   menu = new Menu(this, PG_Rect(15,0,Width()-200,20));

   dataLoaderTicker();
   SetID( ASC_PG_App::mainScreenID );

   
   weaponRangeLayer = new UnitWeaponRangeLayer();
   mapDisplay->addMapLayer( weaponRangeLayer, "weaprange" );

   movementRangeLayer = new UnitMovementRangeLayer();
   mapDisplay->addMapLayer( movementRangeLayer, "moverange" );

   int counter = 0;
   for ( CGameOptions::PanelDataContainer::iterator i = CGameOptions::Instance()->panelData.begin(); i != CGameOptions::Instance()->panelData.end(); ++i ) {
      if ( i->second.visible ) {
         ++counter;
         spawnPanel( i->first );
      }
   }

   if ( !counter ) {
      spawnPanel( UnitInfo );
      spawnPanel( ButtonPanel );
      spawnPanel( OverviewMap );
   }
      
}

void ASC_MainScreenWidget :: enableStandardAction( bool enable )
{
   if ( menu )
      if ( enable )
         menu->Show();
      else
         menu->Hide();

      standardActions = enable;
}





void displaymessage2( const char* formatstring, ... )
{

   ASCString s;
   
    std::va_list arg_ptr;
    va_start ( arg_ptr, formatstring );

    s.vaformat( formatstring, arg_ptr );

    va_end ( arg_ptr );

    if ( mainScreenWidget )
       mainScreenWidget->displayMessage(s);
}



void ASC_MainScreenWidget::spawnPanel ( const ASCString& panelName )
{
   if ( panelName == "WindInfo" )
      spawnPanel ( WindInfo );

   if ( panelName == "UnitInfo" )
      spawnPanel ( UnitInfo );
   
   if ( panelName == "GuiIcons" )
      spawnPanel ( ButtonPanel );

   if ( panelName == "OverviewMap" )
      spawnPanel ( OverviewMap );

   if ( panelName == "MapInfo" )
      spawnPanel ( MapControl );

}

void ASC_MainScreenWidget::spawnPanel ( Panels panel )
{
   if ( panel == WindInfo ) {
      if ( !windInfoPanel || !CGameOptions::Instance()->cacheASCGUI ) {
         delete windInfoPanel;
         windInfoPanel = new WindInfoPanel( this, PG_Rect(Width()-170, 480, 170, 114));
      }
      windInfoPanel->Show();
   }
   if ( panel == UnitInfo ) {
      if ( !unitInfoPanel || !CGameOptions::Instance()->cacheASCGUI ) {
         delete unitInfoPanel;
         unitInfoPanel = new UnitInfoPanel( this, PG_Rect(Width()-170, 160, 170, 320));
      }
      unitInfoPanel->Show();
   }
   if ( panel == ButtonPanel ) {
      if ( !guiHost || !CGameOptions::Instance()->cacheASCGUI ) {
         delete guiHost;
         guiHost = new NewGuiHost( this, mapDisplay, PG_Rect(Width()-170, Height()-200, 170, 200));
         guiHost->pushIconHandler( &GuiFunctions::primaryGuiIcons );
      }
      guiHost->Show();
   }
   
   if ( panel == OverviewMap ) 
      spawnOverviewMapPanel();
   
   if ( panel == MapControl ) {
      if ( !mapInfoPanel || !CGameOptions::Instance()->cacheASCGUI ) {
         delete mapInfoPanel;
         mapInfoPanel = new MapInfoPanel( this, PG_Rect(Width()-170, 0, 170, 160), mapDisplay );
      }
      mapInfoPanel->Show();
   }
}


void ASC_MainScreenWidget::showMovementRange( GameMap* gamemap, const MapCoordinate& pos )
{
   movementRangeLayer->operateField( gamemap, pos );
}

void ASC_MainScreenWidget::showWeaponRange( GameMap* gamemap, const MapCoordinate& pos )
{
   weaponRangeLayer->operateField( gamemap, pos );
}



bool ASC_MainScreenWidget::eventKeyDown(const SDL_KeyboardEvent* key)
// bool Menu::eventKeyDown(const SDL_KeyboardEvent* key)
{
   if ( !standardActions )
      return false;
   
   int mod = SDL_GetModState() & ~(KMOD_NUM | KMOD_CAPS | KMOD_MODE);

   if ( !mod  ) {
      switch ( key->keysym.sym ) {
            case SDLK_ESCAPE:
               execUserAction_ev( ua_mainmenu );
               return true;
               
            case SDLK_F1:
               execUserAction_ev ( ua_help );
               return true;

            case SDLK_F3:
               execUserAction_ev ( ua_continuenetworkgame );
               return true;

            case SDLK_F4:
               execUserAction_ev ( ua_computerturn );
               return true;

            case SDLK_F8:
               {
                  int color = actmap->actplayer;
                  for ( int p = 0; p < 8; p++ )
                     if ( actmap->player[p].stat == Player::computer && actmap->player[p].exist() )
                        color = p;

                  if ( actmap->player[color].ai ) {
                     AI* ai = (AI*) actmap->player[color].ai;
                     ai->showFieldInformation ( actmap->getCursor().x, actmap->getCursor().y );
                  }
               }
               return true;

            case SDLK_1:
               execUserAction_ev ( ua_changeresourceview );
               return true;

            case SDLK_2:
               execUserAction_ev ( ua_toggleunitshading );
               return true;

            case SDLK_3:
               execUserAction_ev( ua_viewunitweaponrange );
               return true;

            case SDLK_4:
               execUserAction_ev( ua_viewunitmovementrange );
               return true;

            case SDLK_5:
               execUserAction_ev ( ua_GameStatus );
               return true;

            case SDLK_6:
               execUserAction_ev ( ua_UnitSetInfo );
               return true;

            case SDLK_7:
               execUserAction_ev ( ua_viewterraininfo );
               return true;

            case SDLK_8:
               execUserAction_ev ( ua_unitweightinfo );
               return true;

            case SDLK_9:
               execUserAction_ev ( ua_viewPipeNet );
               return true;

            case SDLK_0: execUserAction_ev( ua_writescreentopcx );
               return true;

            case SDLK_PLUS:   
            case SDLK_KP_PLUS: execUserAction_ev( ua_increase_zoom );
               return true;

            case SDLK_MINUS:   
            case SDLK_KP_MINUS: execUserAction_ev( ua_decrease_zoom );
               return true;
               
               case SDLK_F10: {
                  
                  PG_ScrollBar* sb = new PG_ScrollBar( NULL, PG_Rect( 20, 50, 15, 400), PG_ScrollBar::VERTICAL, -1, "DLGScrollbar" );
                  sb->SetPageSize(10);
                  sb->SetRange(0, 100);
                  sb->Show();
                  
               }
            // testText();
            break;
               
            default:;
      }
   }

   if ( mod & KMOD_CTRL ) {
      switch ( key->keysym.sym ) {
            case SDLK_l:
               execUserAction_ev ( ua_loadgame );
               return true;

            case SDLK_s:
               execUserAction_ev ( ua_savegame );
               return true;


            case SDLK_n:
               execUserAction_ev ( ua_newGame );
               return true;

            case SDLK_F12:
                  execUserAction_ev ( ua_exportUnitToFile );
               return true;

            case SDLK_F11:
               if ( mod & KMOD_SHIFT ) {
                  if (choice_dlg("Do you really want to crash ASC ?","~y~es","~n~o") == 1) {
                     char* c = NULL;
                     *c = 1;
                  }
               } 
            return true;

            case SDLK_F10: {
               SDL_Surface* s = PG_Application::GetScreen();
               SDL_LockSurface( s );
               int* i = (int*) (s->pixels);
               for ( int x = 0; x < 100; ++x )
                  i[x * s->pitch/4 + x] = 0xffffff;
               SDL_UnlockSurface( s );
               SDL_UpdateRect(s,0,0,0,0);
                           }

            return true;
            

            case SDLK_x:
               execUserAction_ev ( ua_exitgame );
               return true;
            default:;
      }
   }

   if ( mod & KMOD_ALT ) {
      switch ( key->keysym.sym ) {
            case SDLK_RETURN:
               getPGApplication().toggleFullscreen();
               return true;

            default:;
      }
   }

   return false;
}
