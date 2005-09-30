
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


MainScreenWidget*  mainScreenWidget = NULL ;

/*
class tsgonlinemousehelp : public tonlinemousehelp
{
   public:
      tsgonlinemousehelp ( void );
};

tsgonlinemousehelp :: tsgonlinemousehelp ( void )
{
   helplist.num = 12;

   static tonlinehelpitem sghelpitems[12]  = {{{ 498, 26, 576, 36}, 20001 },
         {{ 498, 41, 576, 51}, 20002 },
         {{ 586, 26, 612, 51}, 20003 },
         {{ 499, 57, 575, 69}, 20004 },
         {{ 499, 70, 575, 81}, 20005 },
         {{ 577, 58, 610, 68}, 20006 },
         {{ 577, 70, 610, 80}, 20007 },
         {{ 502, 92, 531,193}, 20008 },
         {{ 465, 92, 485,194}, 20009 },
         {{ 551, 92, 572,193}, 20010 },
         {{ 586, 90, 612,195}, 20011 },
         {{ 473,agmp->resolutiony - ( 480 - 449 ), 601,agmp->resolutiony - ( 480 - 460 )}, 20016 }};

   for ( int i = 0; i< helplist.num; i++ ) {
      sghelpitems[i].rect.x1 = agmp->resolutionx - ( 640 - sghelpitems[i].rect.x1 );
      sghelpitems[i].rect.x2 = agmp->resolutionx - ( 640 - sghelpitems[i].rect.x2 );
   }

   helplist.item = sghelpitems;
}

tsgonlinemousehelp* onlinehelp = NULL;




class tsgonlinemousehelpwind : public tonlinemousehelp
{
   public:
      tsgonlinemousehelpwind ( void );
} ;

tsgonlinemousehelpwind :: tsgonlinemousehelpwind ( void )
{
   helplist.num = 3;

   static tonlinehelpitem sghelpitemswind[3]  = { {{ 501,224, 569,290}, 20013 },
         {{ 589,228, 609,289}, 20014 },
         {{ 489,284, 509,294}, 20015 }};

   for ( int i = 0; i< helplist.num; i++ ) {
      sghelpitemswind[i].rect.x1 = agmp->resolutionx - ( 640 - sghelpitemswind[i].rect.x1 );
      sghelpitemswind[i].rect.x2 = agmp->resolutionx - ( 640 - sghelpitemswind[i].rect.x2 );
   }

   helplist.item = sghelpitemswind;
}

tsgonlinemousehelpwind* onlinehelpwind = NULL;

*/





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








/*
class MainMenuPullDown : public tpulldown
{
   public:
      void init ( void );
} ;

void         MainMenuPullDown :: init ( void )
{
   alwaysOpen = true;

   addfield ( "Glo~b~al" );
   addbutton ( "~O~ptions", ua_gamepreferences );
   addbutton ( "~M~ouse options", ua_mousepreferences );
   // addbutton ( "Select Music Play ~L~ist ", ua_selectPlayList );
   addbutton ( "~S~ound options", ua_soundDialog );
   addbutton ( "seperator", -1);
   addbutton ( "E~x~it\tctrl-x", ua_exitgame );


   addfield ("~G~ame");
   addbutton ( "New ~C~ampaign", ua_newcampaign);
   addbutton ( "~N~ew single Level\tctrl-n", ua_startnewsinglelevel );

   addbutton ( "~L~oad game\tctrl-l", ua_loadgame );
   addbutton ( "Continue network game\tF3", ua_continuenetworkgame);
   addbutton ( "supervise network game", ua_networksupervisor );

   addfield ( "~H~elp" );
   addbutton ( "HowTo ~S~tart email games", ua_howtostartpbem );
   addbutton ( "HowTo ~C~ontinue email games", ua_howtocontinuepbem );
   addbutton ( "seperator", -1);
   addbutton ( "~K~eys", ua_help );

   addbutton ( "~A~bout", ua_viewaboutmessage );

   tpulldown :: init();
   setshortkeys();
}

*/


Menu::~Menu()
{
/*
   for ( Categories::iterator i = categories.begin(); i != categories.end(); ++i )
      delete *i;
*/      
      
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
   addbutton("Weathercast", ua_weathercast);
   currentMenu->addSeparator();
   addbutton ( "clear image cache", ua_clearImageCache );
   addbutton ( "reload dialog theme", ua_reloadDlgTheme );


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






MainScreenWidget::MainScreenWidget( PG_Application& application )
              : PG_Widget(NULL, PG_Rect ( 0, 0, app.GetScreen()->w, app.GetScreen()->h ), false),
              app ( application ) , messageLine(NULL), lastMessageTime(0)
{

   displayLogMessage ( 5, "MainScreenWidget: initializing panels:\n");

   dataLoaderTicker();

   displayLogMessage ( 7, "  Mapdisplay ");
   mapDisplay = new MapDisplayPG( this, PG_Rect(15,30,Width() - 200, Height() - 73));
   mapDisplay->SetID( ASC_PG_App::mapDisplayID );
   dataLoaderTicker();

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

   displayLogMessage ( 5, "done\nMainScreenWidget completed\n");
   dataLoaderTicker();


   repaintDisplay.connect ( SigC::bind( SigC::slot( *this, &MainScreenWidget::Update ), true ));
   
   buildBackgroundImage();
   dataLoaderTicker();

   PG_Application::GetApp()->sigAppIdle.connect( SigC::slot( *this, &MainScreenWidget::idleHandler ));

   mapChanged.connect( SigC::slot( OverviewMapHolder::clearmap ));
   dataLoaderTicker();
   
   MessagingHub::Instance().statusInformation.connect( SigC::slot( *this, &MainScreenWidget::displayMessage ));
   MessagingHub::Instance().messageWindowFactory.connect( SigC::slot( *this, &MainScreenWidget::createStatusWindow ));
}


bool MainScreenWidget :: idleHandler( )
{
   if ( ticker > lastMessageTime + 300 ) {
      displayMessage( "" );
      lastMessageTime = 0xfffffff;
   }   
   return true;
}



void MainScreenWidget::buildBackgroundImage()
{
   if ( !backgroundImage.valid() ) {
      backgroundImage = Surface::createSurface( Width(), Height(), 32 );
      
      Surface& source = IconRepository::getIcon("640480.pcx");
      
      MegaBlitter< 1, gamemapPixelSize, ColorTransform_None,ColorMerger_PlainOverwrite,SourcePixelSelector_DirectZoom> blitter;
      blitter.setSize( source.w(), source.h(), Width(), Height(), false );
      
      dataLoaderTicker();
      blitter.blit( source, backgroundImage, SPoint(0,0) );

      dataLoaderTicker();
            
      assert( mapDisplay );
      PG_Rect r = *mapDisplay;
      
      const int borderWidth = 5;
      for ( int i = 0; i <= borderWidth; ++i ) 
         rectangle<4> ( backgroundImage, SPoint(r.x-i, r.y-i), r.w+2*i, r.h+2*i, ColorMerger_Brightness<4>( 0.6 ), ColorMerger_Brightness<4>( 1.5 ));

      int x1 = r.x + 1;
      int y1 = r.y + 1;
      int x2 = r.x + r.Width() - 1;
      int y2 = r.y + r.Height() -1;

      blitRects[0] = PG_Rect(  0, 0, Width(), y1 ); 
      blitRects[1] = PG_Rect(  0, y1, x1,     y2 ); 
      blitRects[2] = PG_Rect( x2, y1, Width() - x2, y2 ); 
      blitRects[3] = PG_Rect( 0, y2, Width(), Height() - y2 ); 
      
      Surface& msgstart = IconRepository::getIcon("msgline1.png");
      Surface& msgmid   = IconRepository::getIcon("msgline2.png");
      Surface& msgend   = IconRepository::getIcon("msgline3.png");

      dataLoaderTicker();
      
      int mx1 = x1 - borderWidth;
      int mx2 = x2 + borderWidth;
      int my1 = y2 + 10;
      
      int msglength = mx2 - mx1;
      int midlength = msglength - msgend.w();

      backgroundImage.Blit( msgstart, SPoint( mx1, my1 ));

      int x = msgstart.w();
      while ( x + msgmid.w() < midlength ) {
         backgroundImage.Blit( msgmid, SPoint( mx1 + x, my1 ));
         x += msgmid.w();
      }
      dataLoaderTicker();
      backgroundImage.Blit( msgmid, SPoint( mx1 + msglength - msgend.w() - msgmid.w(), my1 ));
      
      dataLoaderTicker();
      backgroundImage.Blit( msgend, SPoint( mx1 + msglength - msgend.w(), my1) );

      messageLine = new PG_Label ( this, PG_Rect( mx1 + 20, my1 + 9, msglength - 30, msgend.h() - 18) );
      messageLine->SetFontSize(11);
      dataLoaderTicker();
         
   }
}

void MainScreenWidget::displayMessage( const ASCString& message )
{
   if ( messageLine ) {
      messageLine->SetText( message );
      lastMessageTime = ticker;
   }   
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



void MainScreenWidget::eventBlit (SDL_Surface *surface, const PG_Rect &src, const PG_Rect &dst) 
{
   SDL_Rect dstrect;
   Surface s = Surface::Wrap( PG_Application::GetScreen() );
   dstrect.x = blitRects[0].x;
   dstrect.y = blitRects[0].y;
   s.Blit( backgroundImage, blitRects[0], dstrect );
   
   dstrect.x = blitRects[1].x;
   dstrect.y = blitRects[1].y;
   s.Blit( backgroundImage, blitRects[1], dstrect );
   
   dstrect.x = blitRects[2].x;
   dstrect.y = blitRects[2].y;
   s.Blit( backgroundImage, blitRects[2], dstrect );
   
   dstrect.x = blitRects[3].x;
   dstrect.y = blitRects[3].y;
   s.Blit( backgroundImage, blitRects[3], dstrect );
}

StatusMessageWindow MainScreenWidget::createStatusWindow( const ASCString& msg )
{
   return StatusMessageWindow( new PG_StatusWindowData( msg ));
}


void MainScreenWidget::spawnPanel ( Panels panel )
{
   if ( panel == WindInfo ) {
      WindInfoPanel* wi = new WindInfoPanel( this, PG_Rect(Width()-170, 480, 170, 114));
      wi->Show();
   }
   if ( panel == UnitInfo ) {
      UnitInfoPanel* ui = new UnitInfoPanel( this, PG_Rect(Width()-170, 160, 170, 320));
      ui->Show();
   }
   if ( panel == ButtonPanel ) {
      guiHost = new NewGuiHost( this, mapDisplay, PG_Rect(Width()-170, Height()-200, 170, 200));
      guiHost->pushIconHandler( &GuiFunctions::primaryGuiIcons );
      guiHost->Show();
   }
   if ( panel == OverviewMap ) {
      assert( mapDisplay);
      OverviewMapPanel* smp = new OverviewMapPanel( this, PG_Rect(Width()-170, 0, 170, 160), mapDisplay );
      smp->Show();
   }
}

bool MainScreenWidget::eventKeyDown(const SDL_KeyboardEvent* key)
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
