
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
#include <signal.h>
#include <algorithm>
#include <memory>
#include <SDL_image.h>
#include <SDL_mixer.h>

#include "paradialog.h"

#include "basegfx.h"
#include "misc.h"
#include "sg.h"
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


ASC_MainScreenWidget*  mainScreenWidget = NULL ;



class Menu : public PG_MenuBar {

    PG_PopupMenu* currentMenu;
    typedef list<PG_PopupMenu*> Categories;
    Categories categories;

   public:
      Menu ( PG_Widget *parent, const PG_Rect &rect=PG_Rect::null);
      ~Menu();
      
   protected:
      void setup();   
      bool execAction  (PG_PopupMenu::MenuItem* menuItem );

   private:
      void addbutton(const char* name, int id );
      void addfield ( const char* name );
     
};







Menu::~Menu()
{
}

bool Menu::execAction  (PG_PopupMenu::MenuItem* menuItem )
{
   execUserAction_ev( tuseractions( menuItem->getId() ) );

   return true;
}


void Menu::addfield( const char* name )
{
   ASCString s = name;
   while ( s.find ( "~") != ASCString::npos )
      s.erase( s.find( "~"),1 );
      
   currentMenu = new PG_PopupMenu( NULL, -1, -1, "" );
   categories.push_back ( currentMenu );
   Add ( s, currentMenu );
   currentMenu->sigSelectMenuItem.connect( SigC::slot( *this, &Menu::execAction ));

}

void Menu::addbutton( const char* name, int id )
{
   ASCString s = name;
   while ( s.find ( "~") != ASCString::npos )
      s.erase( s.find( "~"),1 );
      
   currentMenu->addMenuItem( s, id );
}


void Menu::setup()
{
   addfield ( "Glo~b~al" );
   addbutton ( "toggle ~R~esourceview\t1", ua_changeresourceview );
   addbutton ( "toggle unit shading\t2", ua_toggleunitshading );
   currentMenu->addSeparator();
   addbutton ( "~O~ptions", ua_gamepreferences );
   addbutton ( "~M~ouse options", ua_mousepreferences );
   addbutton ( "~S~ound options", ua_soundDialog );
   currentMenu->addSeparator();
   addbutton ( "E~x~it\tctrl-x", ua_exitgame );


   addfield ("~G~ame");
//   addbutton ( "New ~C~ampaign", ua_newcampaign);
//   addbutton ( "~N~ew single Level\tctrl-n", ua_startnewsinglelevel );
   addbutton ( "New Game", ua_newGame );
   currentMenu->addSeparator();
   addbutton ( "~L~oad game\tctrl-l", ua_loadgame );
   addbutton ( "~S~ave game\tctrl-s", ua_savegame );
   currentMenu->addSeparator();
   addbutton ( "Continue network game\tF3", ua_continuenetworkgame);
   addbutton ( "setup Net~w~ork", ua_setupnetwork );
   addbutton ( "Change Passw~o~rd", ua_changepassword );
   addbutton ( "supervise network game", ua_networksupervisor );
   currentMenu->addSeparator();
   addbutton ( "~D~iplomacy", ua_setupalliances);
   addbutton ( "transfer ~U~nit control", ua_giveunitaway );
   addbutton ( "~r~ename unit/building", ua_renameunit );
   addbutton ( "~T~ransfer resources", ua_settribute);
   addbutton ( "~C~ancel Research", ua_cancelResearch );

   addfield ( "~I~nfo" );
   addbutton ( "~V~ehicle types", ua_vehicleinfo );
   addbutton ( "Unit ~w~eapon range\t3", ua_viewunitweaponrange );
   addbutton ( "Unit ~m~ovement range\t4", ua_viewunitmovementrange );
   addbutton ( "~G~ame Time\t5", ua_GameStatus );
   addbutton ( "unit ~S~et information\t6", ua_UnitSetInfo );
   addbutton ( "~T~errain\t7", ua_viewterraininfo );
   addbutton ( "~U~nit weight\t8", ua_unitweightinfo );
   addbutton ( "show ~P~ipeline net\t9", ua_viewPipeNet );
   currentMenu->addSeparator();
   addbutton ( "~R~esearch", ua_researchinfo );
   addbutton ( "~P~lay time", ua_showPlayerSpeed );
   addbutton ( "~C~argo Summary", ua_cargosummary );
   // addbutton ( "~R~esearch status", ua_showResearchStatus );

   // addbutton ( "vehicle ~I~mprovement\tF7", ua_dispvehicleimprovement);
   // addbutton ( "show game ~P~arameters", ua_GameParameterInfo );


   addfield ( "~S~tatistics" );
   addbutton ( "~U~nits", ua_unitstatistics );
   addbutton ( "~B~uildings", ua_buildingstatistics );
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
   addbutton ( "Button Panel", ua_viewButtonPanel );
   addbutton ( "Wind Panel", ua_viewWindPanel );
   addbutton ( "Unit Info Panel", ua_viewUnitInfoPanel );
   addbutton ( "Overview Map Panel", ua_viewOverviewMapPanel );
   addbutton ( "Map Control Panel", ua_viewMapControlPanel );
   addbutton("Weathercast", ua_weathercast);
   currentMenu->addSeparator();
   addbutton ( "clear image cache", ua_clearImageCache );
   addbutton ( "reload dialog theme", ua_reloadDlgTheme );
   addbutton ( "test messages", ua_testMessages );


   addfield ( "~H~elp" );
   addbutton ( "HowTo ~S~tart email games", ua_howtostartpbem );
   addbutton ( "HowTo ~C~ontinue email games", ua_howtocontinuepbem );
   addbutton ( "Show ASC search ~P~ath", ua_showsearchdirs );
   currentMenu->addSeparator();
   addbutton ( "~K~eys", ua_help );

   addbutton ( "~A~bout", ua_viewaboutmessage );
}


Menu::Menu ( PG_Widget *parent, const PG_Rect &rect)
    : PG_MenuBar( parent, rect, "MenuBar"),
      currentMenu(NULL)
{
   setup();
   
}  






ASC_MainScreenWidget::ASC_MainScreenWidget( PG_Application& application )
   : MainScreenWidget( application ), guiHost(NULL), menu(NULL), unitInfoPanel(NULL)
{

   setup( true );

   displayLogMessage ( 7, "done\n  Menu ");
   menu = new Menu(this, PG_Rect(15,0,Width()-200,20));

   dataLoaderTicker();
   SetID( ASC_PG_App::mainScreenID );

   displayLogMessage ( 7, "done\n  ButtonPanel ");
   spawnPanel ( ButtonPanel );

   dataLoaderTicker();
   // displayLogMessage ( 7, "done\n  WindInfo ");
   // spawnPanel ( WindInfo );

   displayLogMessage ( 7, "done\n  UnitInfo ");
   dataLoaderTicker();
   spawnPanel ( UnitInfo );

   displayLogMessage ( 7, "done\n  OverviewMap ");
   dataLoaderTicker();
   spawnPanel ( OverviewMap );

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




void ASC_MainScreenWidget::spawnPanel ( Panels panel )
{
   if ( panel == WindInfo ) {
      WindInfoPanel* wi = new WindInfoPanel( this, PG_Rect(Width()-170, 480, 170, 114));
      wi->Show();
   }
   if ( panel == UnitInfo ) {
      unitInfoPanel = new UnitInfoPanel( this, PG_Rect(Width()-170, 160, 170, 320));
      unitInfoPanel->Show();
   }
   if ( panel == ButtonPanel ) {
      guiHost = new NewGuiHost( this, mapDisplay, PG_Rect(Width()-170, Height()-200, 170, 200));
      guiHost->pushIconHandler( &GuiFunctions::primaryGuiIcons );
      guiHost->Show();
   }
   
   if ( panel == OverviewMap ) 
      spawnOverviewMapPanel();
   
   if ( panel == MapControl ) {
      MapInfoPanel* mcp = new MapInfoPanel( this, PG_Rect(Width()-170, 0, 170, 160), mapDisplay );
      mcp->Show();
   }
}

bool ASC_MainScreenWidget::eventKeyDown(const SDL_KeyboardEvent* key)
{
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

            case SDLK_F11: 
               {
                  ASCString s;
                  for ( int i = 0; i < 20; ++i )
                     s += ASCString::toString(i) + "\n";
                  
                  displaymessage( s, 1 );
               }
            return true;

            case SDLK_1:
               execUserAction_ev ( ua_changeresourceview );
               return true;

            case SDLK_2:
               execUserAction_ev ( ua_toggleunitshading );
               return true;

            case SDLK_3:
               // viewunitweaponrange ( getSelectedField()->vehicle, SDLK_3 );
               return true;

            case SDLK_4:
               // viewunitmovementrange ( getSelectedField()->vehicle, SDLK_4 );
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
               // viewPipeNet ( SDLK_9 );
               return true;

            case SDLK_0: execUserAction_ev( ua_writescreentopcx );
               return true;
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

            case SDLK_x:
               execUserAction_ev ( ua_exitgame );
               return true;
            default:;
      }
   }
   return false;
}
