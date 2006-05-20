/***************************************************************************
                           sg.cpp  -  description
                             -------------------
    begin                : a long time ago...
    copyright            : (C) 1994-2003 by Martin Bickel
    email                : bickel@asc-hq.org
 ***************************************************************************/

/*! \file sg.cpp
    \brief THE main program: ASC
*/


/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


/*!
   \mainpage 

   \section A short walk through the source
 
   THE central class of ASC is #tmap in gamemap.h . 
   It is the anchor where nearly all elements of ASC are chained to. The global 
   variable #actmap is a pointer to the active map. There can be a maximum of
   8 #Player on a map, plus neutral units (which are handled like a 9th #Player).
   Hence the array of 9 #tmap::player classes in #tmap.
   
   Each player has units and buildings, which are stored in the lists 
   #Player::vehicleList and #Player::buildingList .
   The terms units and vehicles are used synonymously in ASC. Since unit was a 
   reserved word in Borland Pascal (the language that ASC was originally written in),
   we decided to use the term vehicle instead. But now, with ASC written in C++, 'unit' is also used.
   
   Every building and unit is of a certain 'type': Vehicletype and BuildingType .
   These are stored in the data files which are loaded on startup and are globally 
   available. They are not modified during runtime in any way and are referenced
   by the instances of Vehicle and Building. The Vehicletype has information that are shared
   by all vehicles of this 'type', like speed, weapon systems, accessable
   terrain etc, while the vehicle stores things like remaining movement for this
   turn, ammo, fuel and cargo.
   
   The primary contents of a map are its fields ( #tfield ). Each field has again a pointer 
   to a certain weather of a TerrainType. Each TerrainType has up to 5 
   different weathers ("dry (standard)","light rain", "heavy rain", "few snow",
   "lot of snow"). If there is a unit or a building standing on a field, the field
   has a pointer to it: tfield::vehicle and tfield::building .
   
   On the field can be several instances of Object. Objects are another central class of 
   ASC. Roads, pipleines, trenches and woods are examples of objects. #Vehicle and #Building
   are NOT objects.

   \section The basic design of ASC
   
   <p><img src="ClassDiagram1.png">

   \section Paragui

   ASC uses the Paragui as widget engine. The documentation is available at
   <A HREF="http://terdon.asc-hq.org/asc/paragui-doc/html/">terdon.asc-hq.org</A>.
 
*/

#include "global.h"

#include <stdio.h>
#include <stdlib.h>
#include <new>
#include <cstdlib>
#include <ctype.h>
#include <algorithm>
#include <memory>
#include <SDL_image.h>
#include <SDL_mixer.h>

#include "paradialog.h"

#include "vehicletype.h"
#include "buildingtype.h"
#include "ai/ai.h"
#include "basegfx.h"
#include "misc.h"
#include "newfont.h"
#include "events.h"
#include "typen.h"
#include "spfst.h"
#include "loaders.h"
#include "dlg_box.h"
#include "stack.h"
#include "controls.h"
#include "dlg_box.h"
#include "dialog.h"
#include "pd.h"
#include "strtmesg.h"
#include "gamedlg.h"
#include "sg.h"
#include "gameoptions.h"
#include "loadimage.h"
#include "astar2.h"
#include "errors.h"
#include "dialogs/pwd_dlg.h"
#include "viewcalculation.h"
#include "replay.h"
#include "graphicset.h"
#include "loadbi3.h"
#include "itemrepository.h"
#include "music.h"
#include "messagedlg.h"
#include "statisticdialog.h"
#include "clipboard.h"
#include "guifunctions.h"
#include "iconrepository.h"
#include "dashboard.h"
#include "gamedialog.h"
#include "weathercast.h"
#include "asc-mainscreen.h"
#include "dialogs/unitinfodialog.h"
#include "messaginghub.h"
#include "cannedmessages.h"
#include "memorycheck.cpp"
#include "networkinterface.h"
#include "resourcenet.h"
#include "mapimageexport.h"
#include "loadpcx.h"
#include "gameeventsystem.h"
#include "sdl/sound.h"
#include "soundList.h"
#include "turncontrol.h"
#include "network.h"

#include "dialogs/newgame.h"
#include "dialogs/soundsettings.h"
#include "dialogs/alliancesetup.h"
#include "dialogs/unitcounting.h"
#include "dialogs/editgameoptions.h"
#include "dialogs/nextcampaignmap.h"
#include "dialogs/terraininfo.h"
#include "stdio-errorhandler.h"

#ifdef WIN32
# include "win32/win32-errormsg.h"
# include  "win32/msvc/mdump.h"
 MiniDumper miniDumper( "main" );

#include <direct.h>
#include <stdlib.h>
#include <stdio.h>

#endif

tfield*        getSelectedField(void)
{
   return actmap->getField( actmap->getCursor() ); 
} 


#define mmaintainence

bool maintainencecheck( void )
{
   int res = 0;
   if ( res )
      return true;

#ifdef maintainence
   int num = 0;

   for ( int i = 0; i < 8; i++ )
      if ( actmap->player[i].stat == Player::human  && actmap->player[i].exist())
         num++;

   if ( actmap->campaign )
      num++;

   if ( actmap->network )
      num++;

   if ( num <= 1 )
      return 1;
   else
      return 0;

#else
   return false;
#endif
}

void positionCursor( Player& player )
{
   getDefaultMapDisplay().displayPosition( player.getParentMap()->getCursor() );
   
}

void viewcomp( Player& player )
{
   computeview( player.getParentMap() );
}

void hookGuiToMap( GameMap* map )
{
   if ( !map->getGuiHooked() ) {

      map->sigPlayerUserInteractionBegins.connect( SigC::slot( &viewcomp ) );
      map->sigPlayerUserInteractionBegins.connect( SigC::hide<Player&>( repaintMap.slot() ));
      
      map->sigPlayerUserInteractionBegins.connect( SigC::slot( &positionCursor ));
      map->sigPlayerUserInteractionBegins.connect( SigC::slot( &viewunreadmessages ));
      map->sigPlayerUserInteractionBegins.connect( SigC::slot( &researchCheck ));
      map->sigPlayerUserInteractionBegins.connect( SigC::slot( &checkJournal ));
      map->sigPlayerUserInteractionBegins.connect( SigC::hide<Player&>( SigC::slot( &checkforreplay )));
      map->sigPlayerUserInteractionBegins.connect( SigC::slot( &checkUsedASCVersions ));
      map->sigPlayerUserInteractionBegins.connect( SigC::hide<Player&>( updateFieldInfo.slot() ));
      
      map->sigPlayerUserInteractionEnds.connect( SigC::slot( viewOwnReplay));

      
      map->guiHooked();
   }
}



bool loadGame()
{
   ASCString s1 = selectFile( savegameextension, true );

   if ( !s1.empty() ) {
      StatusMessageWindowHolder smw = MessagingHub::Instance().infoMessageWindow( "loading " + s1 );
      loadgame( s1 );
      if ( !actmap || actmap->xsize == 0 || actmap->ysize == 0 )
         throw  NoMapLoaded();

      computeview( actmap );
      hookGuiToMap ( actmap );
      
      updateFieldInfo();
      positionCursor( actmap->getCurrentPlayer() );
      getDefaultMapDisplay().displayPosition( actmap->getCursor() );
      displaymap();

      moveparams.movestatus = 0;
      return true;
   } else
      return false;
}


void saveGame( bool as )
{
   ASCString s1;

   int nameavail = 0;
   if ( !actmap->preferredFileNames.savegame[actmap->actplayer].empty() )
      nameavail = 1;

   if ( as || !nameavail ) {
      s1 = selectFile( savegameextension, false);
   } else
      s1 = actmap->preferredFileNames.savegame[actmap->actplayer];

   if ( !s1.empty() ) {
      actmap->preferredFileNames.savegame[actmap->actplayer] = s1;

      StatusMessageWindowHolder smw = MessagingHub::Instance().infoMessageWindow( "saving " + s1 );
      savegame( s1 );
   }
}






void loadmap( const ASCString& name, bool campaign )
{
   GameMap* m = mapLoadingExceptionChecker( name, MapLoadingFunction( tmaploaders::loadmap ));
   delete actmap;
   actmap = m;
   computeview( actmap );
   hookGuiToMap( actmap );
   if ( !campaign )
      actmap->campaign.avail = false;
}

void loadStartupMap ( const char *gameToLoad=NULL )
{
   if ( gameToLoad && gameToLoad[0] ) {
      try {
         if ( patimat ( ASCString("*")+tournamentextension, gameToLoad )) {

            if( validateemlfile( gameToLoad ) == 0 )
               fatalError( "Email gamefile %s is invalid. Aborting.", gameToLoad );

            try {
               tnfilestream gamefile ( gameToLoad, tnstream::reading );
               tnetworkloaders nwl;
               nwl.loadnwgame( &gamefile );
            } catch ( tfileerror ) {
               fatalError ( "%s is not a legal email game.", gameToLoad );
            }
         } else if( patimat ( savegameextension, gameToLoad )) {
            if( validatesavfile( gameToLoad ) == 0 )
               fatalError ( "The savegame %s is invalid. Aborting.", gameToLoad );

            try {
               loadgame( gameToLoad );
               computeview( actmap );
            } catch ( tfileerror ) {
               fatalError ( "%s is not a legal savegame. ", gameToLoad );
            }

         } else if( patimat ( mapextension, gameToLoad )) {
            if( validatemapfile( gameToLoad ) == 0 )
               fatalError ( "Mapfile %s is invalid. Aborting.", gameToLoad );

            try {
               loadmap( gameToLoad, false );
            } catch ( tfileerror ) {
               fatalError ( "%s is not a legal map. ", gameToLoad );
            }
         } else
            fatalError ( "Don't know how to handle the file %s ", gameToLoad );

      }
      catch ( InvalidID err ) {
         displaymessage( err.getMessage(), 2 );
      } /* endcatch */
      catch ( tinvalidversion err ) {
         if ( err.expected < err.found )
            displaymessage( "File/module %s has invalid version.\nExpected version %d\nFound version %d\nPlease install the latest version from www.asc-hq.org", 2, err.getFileName().c_str(), err.expected, err.found );
         else
            displaymessage( "File/module %s has invalid version.\nExpected version %d\nFound version %d\nThis is a bug, please report it!", 2, err.getFileName().c_str(), err.expected, err.found );
      }
   } else {  // resort to loading defaults

      ASCString s = CGameOptions::Instance()->startupMap; 

      if ( s.empty() )
         s = "asc001.map";


      int maploadable;
      {
         tfindfile ff ( s );
         string filename = ff.getnextname();
         maploadable = validatemapfile ( filename );
      }

      if ( !maploadable ) {

         tfindfile ff ( mapextension );
         string filename = ff.getnextname();

         if ( filename.empty() )
            displaymessage( "unable to load startup-map",2);

         while ( !validatemapfile ( filename ) ) {
            filename = ff.getnextname();
            if ( filename.empty() )
               displaymessage( "unable to load startup-map",2);

         }
         s = filename;
      }

      loadmap( s, true );
      displayLogMessage ( 6, "done\n" );
   }
}


void         startnextcampaignmap( int id)
{
   GameMap* map = nextCampaignMap( id );
   delete actmap;
   actmap = map;

   if ( actmap ) {
      computeview( actmap );
      hookGuiToMap( actmap );
      repaintMap();
   }
}


void benchgame ( int mode )
{
   int t2;
   int t = ticker;
   int n = 0;
   do {
      if ( mode <= 1 ) {
         if ( mode == 1 )
            computeview( actmap );
         displaymap();
      } else
         copy2screen();

      n++;
      t2 = ticker;
   } while ( t + 1000 > t2 ); /* enddo */
   double d = 100 * n;
   d /= (t2-t);
   char buf[100];
   sprintf ( buf, "%3.1f", d );
   displaymessage2 ( " %s fps ", buf );
}


void showSearchPath()
{

      ASCString s;
      for ( int i = 0; i < getSearchPathNum(); ++i )
         s += getSearchPath ( i ) + "\n"; 

      s += "\n";
      s += "Configuration file used: \n";
      s += getConfigFileName();

#ifdef WIN32
     char buffer[_MAX_PATH];

     if( _getcwd( buffer, _MAX_PATH ) ) {
         s += "\nCurrent working directory: \n";
         s += buffer;
     }
#endif



      tviewanytext vat ;
      vat.init ( "Search Path", s.c_str(), 20, -1 , 450, 480 );
      vat.run();
      vat.done();
}



// user actions using the old event system
void execuseraction ( tuseractions action )
{
   switch ( action ) {
      case ua_repainthard  :
      case ua_repaint      :
         repaintDisplay();
         break;

      case ua_help         :
         help(20);
         break;

      case ua_howtostartpbem :
         help(21);
         break;

      case ua_howtocontinuepbem :
         help(22);
         break;

/*
      case ua_mntnc_morefog:
         if (actmap->weather.fog < 255   && maintainencecheck() ) {
            actmap->weather.fog++;
            computeview( actmap );
            displaymessage2("fog intensity set to %d ", actmap->weather.fog);
            displaymap();
         }
         break;

      case ua_mntnc_lessfog:
         if (actmap->weather.fog  && maintainencecheck()) {
            actmap->weather.fog--;
            computeview( actmap );
            displaymessage2("fog intensity set to %d ", actmap->weather.fog);
            displaymap();
         }
         break;

      case ua_mntnc_morewind:
         if ((actmap->weather.windSpeed < 254) &&  maintainencecheck()) {
            actmap->weather.windSpeed+=2;
            displaywindspeed (  );
            updateFieldInfo();
         }
         break;

      case ua_mntnc_lesswind:
         if ((actmap->weather.windSpeed > 1)  && maintainencecheck() ) {
            actmap->weather.windSpeed-=2;
            displaywindspeed (  );
            updateFieldInfo();
         }
         break;

      case ua_mntnc_rotatewind:
         if ( maintainencecheck() ) {
            if (actmap->weather.windDirection < sidenum-1 )
               actmap->weather.windDirection++;
            else
               actmap->weather.windDirection = 0;
            displaymessage2("wind dir set to %d ", actmap->weather.windDirection);
            updateFieldInfo();
            displaymap();
         }
         break;
*/
      case ua_changeresourceview:
         if ( mainScreenWidget ) 
            mainScreenWidget->toggleMapLayer( "resources");
         displaymap();
         break;

      case ua_benchgamewov:
         benchMapDisplay();
         // benchgame( 0 );
         break;

      case ua_benchgamewv :
         benchgame( 1 );
         break;

      case ua_writescreentopcx:
         {
            ASCString name = getnextfilenumname ( "screen", "pcx", 0 );
            Surface s ( PG_Application::GetScreen() );
            writepcx ( name, s);
            displaymessage2( "screen saved to %s", name.c_str() );
         }
         break;

      case ua_changepassword:
         {
            bool success;
            do {
               Password oldpwd = actmap->player[actmap->actplayer].passwordcrc;
               actmap->player[actmap->actplayer].passwordcrc.reset();
               success = enterpassword ( actmap->player[actmap->actplayer].passwordcrc, true, true );
               if ( !success )
                  actmap->player[actmap->actplayer].passwordcrc = oldpwd;
            } while ( actmap->player[actmap->actplayer].passwordcrc.empty() && success && viewtextquery ( 910, "warning", "~e~nter password", "~c~ontinue without password" ) == 0 ); /* enddo */
         }
         break;

      case ua_bi3preferences:
         bi3preferences();
         break;

      case ua_settribute :
         settributepayments ();
         break;

      case ua_giveunitaway:
         if ( actmap && actmap->getgameparameter( cgp_disableUnitTransfer ) == 0 )
            giveunitaway ( actmap->getField( actmap->getCursor() ));
         else
            displaymessage("Sorry, this function has been disabled when starting the map!", 1 );
         break;
         
      case ua_researchinfo:
         researchinfo ();
         break;

      case ua_newmessage:
         newmessage();
         break;

      case ua_viewqueuedmessages:
         viewmessages( "queued messages", actmap->unsentmessage, 1, 0 );
         break;

      case ua_viewsentmessages:
         viewmessages( "sent messages", actmap->player[ actmap->actplayer ].sentmessage, 0, 0 );
         break;

      case ua_viewreceivedmessages:
         viewmessages( "received messages", actmap->player[ actmap->actplayer ].oldmessage, 0, 1 );
         break;

      case ua_viewjournal:
         viewjournal();
         break;

      case ua_editjournal:
         editjournal();
         break;

      case ua_viewaboutmessage:
         {
            help(30);
            tviewanytext vat;
            ASCString s = getstartupmessage();

#ifdef _SDL_
            char buf[1000];
            SDL_version compiled;
            SDL_VERSION(&compiled);
            sprintf(buf, "\nCompiled with SDL version: %d.%d.%d\n", compiled.major, compiled.minor, compiled.patch);
            s += buf;

            sprintf(buf, "Linked with SDL version: %d.%d.%d\n", SDL_Linked_Version()->major, SDL_Linked_Version()->minor, SDL_Linked_Version()->patch);
            s += buf;
#endif

            vat.init ( "about", s.c_str() );
            vat.run();
            vat.done();
         }
         break;

      case ua_toggleunitshading:
         CGameOptions::Instance()->units_gray_after_move = !CGameOptions::Instance()->units_gray_after_move;
         CGameOptions::Instance()->setChanged();
         displaymap();
         while ( mouseparams.taste )
            releasetimeslice();

         if ( CGameOptions::Instance()->units_gray_after_move )
            displaymessage("units that can not move will now be displayed gray", 3);
         else
            displaymessage("units that can not move and cannot shoot will now be displayed gray", 3);
         break;

      case ua_computerturn:
         if ( maintainencecheck() ) {
            displaymessage("This function is under development and for programmers only\n"
                           "unpredictable things may happen ...",3 ) ;

            if (choice_dlg("do you really want to start the AI?","~y~es","~n~o") == 1) {

               if ( !actmap->player[ actmap->actplayer ].ai )
                  actmap->player[ actmap->actplayer ].ai = new AI ( actmap, actmap->actplayer );

               savegame ( "aistart.sav" );
               actmap->player[ actmap->actplayer ].ai->run();
            }
         }
         break;
      case ua_setupnetwork:
      /*
         if ( actmap->network )
            setupnetwork ( actmap->network );
         else
            displaymessage("This map is not played across a network",3 );
            */
            displaymessage("Not implemented yet",3 );
         break;
      case ua_UnitSetInfo:
         viewUnitSetinfo();
         break;
      case ua_GameParameterInfo:
         showGameParameters();
         break;
      case ua_viewunitweaponrange:
         mainScreenWidget->showWeaponRange( actmap, actmap->getCursor() );
         break;

      case ua_viewunitmovementrange:
         mainScreenWidget->showMovementRange( actmap, actmap->getCursor() );
         break;

      case ua_aibench:
         if ( maintainencecheck() && 0 ) {
            if ( !actmap->player[ actmap->actplayer ].ai )
               actmap->player[ actmap->actplayer ].ai = new AI ( actmap, actmap->actplayer );

            if ( AI* ai = dynamic_cast<AI*>( actmap->player[ actmap->actplayer ].ai )) {
               savegame ( "ai-bench-start.sav" );
               ai->run( true );
            }
         }
         break;

      case ua_selectPlayList:
         selectPlayList();
         break;
      case ua_statisticdialog:
         statisticDialog();
         break;

      case ua_togglesound:
         if ( !SoundSystem::getInstance()->isOff() ) {
            bool on = !SoundSystem::getInstance()->areEffectsMuted();
            SoundSystem::getInstance()->setEffectsMute( on );
            if ( on )
               SoundSystem::getInstance()->pauseMusic();
            else
               SoundSystem::getInstance()->resumeMusic();
         }
         break;
      case ua_showPlayerSpeed:
         showPlayerTime();
         break;
      case  ua_cancelResearch:
         if ( actmap->player[actmap->actplayer].research.activetechnology ) {
            ASCString s = "do you really want to cancel the current research project ?\n";
            // s += strrr ( actmap->player[actmap->actplayer].research.progress );
            // s += " research points will be lost.";
            if (choice_dlg(s.c_str(),"~y~es","~n~o") == 1) {
               actmap->player[actmap->actplayer].research.progress = 0;
               actmap->player[actmap->actplayer].research.activetechnology = NULL;
            }
         } else
            displaymessage("you are not researching anything", 3);
         break;
      case ua_showResearchStatus: {
            ASCString s;
            s += "Current technology:\n";
            if ( actmap->player[actmap->actplayer].research.activetechnology )
               s += actmap->player[actmap->actplayer].research.activetechnology->name;
            else
               s += " - none - ";
            s += "\n\n";

            s += "Research Points: \n";
            s += strrr( actmap->player[actmap->actplayer].research.progress );
            if ( actmap->player[actmap->actplayer].research.activetechnology )
               s += ASCString(" / ") + strrr ( actmap->player[actmap->actplayer].research.activetechnology->researchpoints );
            s += "\n\n";

            s+= "Research Points Plus \n";
            int p = 0;
            for ( Player::BuildingList::iterator i = actmap->player[actmap->actplayer].buildingList.begin(); i != actmap->player[actmap->actplayer].buildingList.end(); ++i )
               p += (*i)->researchpoints;


            s += strrr ( p );

            s += "\n\n";

            s+= "Developed Technologies: \n";
            for ( vector<int>::iterator i = actmap->player[actmap->actplayer].research.developedTechnologies.begin(); i != actmap->player[actmap->actplayer].research.developedTechnologies.end(); ++i ) {
               Technology* t = technologyRepository.getObject_byID( *i );
               if ( t )
                  s += t->name + "\n";
            }

            tviewanytext vat ;
            vat.init ( "Research Status", s.c_str(), 20, -1 , 450, 480 );
            vat.run();
            vat.done();
         }
         break;
      case ua_exportUnitToFile:
         if ( getSelectedField()->vehicle && getSelectedField()->vehicle->getOwner() == actmap->actplayer ){
            if (choice_dlg( "do you really want to cut this unit from the game?", "~y~es","~n~o") == 1) {
               Vehicle* veh = getSelectedField()->vehicle;
               ClipBoard::Instance().clear();
               ClipBoard::Instance().addUnit( veh );

               ASCString filename = selectFile( clipboardFileExtension, false );
               if ( !filename.empty() ) {
                  tnfilestream stream ( filename, tnstream::writing );
                  ClipBoard::Instance().write( stream );
                  logtoreplayinfo ( rpl_cutFromGame, veh->networkid );
                  veh->prepareForCleanRemove();
                  delete veh;
                  computeview( actmap );
                  displaymap();
               }
            }
         }
         break;
      case ua_showsearchdirs: showSearchPath();
         break;

      default:;
      };
}

bool continueAndStartMultiplayerGame()
{
   if ( continuenetworkgame()) {
      hookGuiToMap(actmap);
      displaymap();
      return true;
   } else
      return false;
}



// user actions using the new event system
void execuseraction2 ( tuseractions action )
{
   switch ( action ) {
   
      case ua_unitweightinfo:
         if ( fieldvisiblenow  ( getSelectedField() )) {
            Vehicle* eht = getSelectedField()->vehicle;
            if ( eht && actmap->player[actmap->actplayer].diplomacy.getState( eht->getOwner()) >= PEACE_SV )
               displaymessage(" weight of unit: \n basic: %d\n+cargo:%d\n= %d",1 ,eht->typ->weight, eht->cargoWeight(), eht->weight() );
         }
         break;
      case ua_GameStatus:
         displaymessage ( "Current game time is:\n turn %d , move %d ", 3, actmap->time.turn(), actmap->time.move() );
         break;
      case ua_soundDialog:
          soundSettings( NULL );
         break;
      case ua_reloadDlgTheme:
             getPGApplication().reloadTheme();
             MessagingHub::Instance().message( MessagingHubBase::InfoMessage, "Theme reloaded" );
             // soundSettings( NULL );
         break;
      case ua_viewButtonPanel:  mainScreenWidget->spawnPanel( ASC_MainScreenWidget::ButtonPanel );
         break;
      case ua_viewWindPanel:     mainScreenWidget->spawnPanel( ASC_MainScreenWidget::WindInfo );
         break;
      case ua_clearImageCache:  IconRepository::clear();
         break;
      case ua_viewUnitInfoPanel: mainScreenWidget->spawnPanel( ASC_MainScreenWidget::UnitInfo );
         break;
      case ua_viewOverviewMapPanel: mainScreenWidget->spawnPanel( ASC_MainScreenWidget::OverviewMap );
         break;
      case ua_viewMapControlPanel: mainScreenWidget->spawnPanel( ASC_MainScreenWidget::MapControl );
         break;
      case ua_vehicleinfo: unitInfoDialog();
         break;
      case ua_weathercast: weathercast();
         break;
      case ua_newGame: 
         startMultiplayerGame();
         hookGuiToMap(actmap);
         break;
      case ua_continuenetworkgame:
         continueAndStartMultiplayerGame();
         break;
      case ua_loadgame: loadGame();
         break;
      case ua_savegame: saveGame( true );
         break;
      case ua_setupalliances:
         if ( setupalliances( actmap, actmap->getCurrentPlayer().stat == Player::supervisor ) ) {
            if ( computeview( actmap ))
               displaymap();
         }
         updateFieldInfo();
         break;
      case ua_mainmenu:
         /*
         if (choice_dlg("do you really want to close the current game ?","~y~es","~n~o") == 1) {
            delete actmap;
            actmap = NULL;
            throw NoMapLoaded();
         }
         */
         GameDialog::gameDialog();
         break;
      case ua_viewterraininfo:
         if ( fieldvisiblenow( actmap->getField( actmap->getCursor())))
            viewterraininfo( actmap, actmap->getCursor(), fieldVisibility( actmap->getField( actmap->getCursor())) == visible_all );
         break;
      case ua_testMessages:
         MessagingHub::Instance().message( MessagingHubBase::InfoMessage, "This is an informational message" );
         MessagingHub::Instance().message( MessagingHubBase::Warning,     "This is an warning message" );
         MessagingHub::Instance().message( MessagingHubBase::Error,       "This is an error message" );
         MessagingHub::Instance().message( MessagingHubBase::FatalError,  "This is an fatal error message. Game will be exited." );
         break;
      case ua_writemaptopcx :
         writemaptopcx ( actmap );
         break;
      case ua_exitgame:
         if (choice_dlg("do you really want to quit ?","~y~es","~n~o") == 1)
            getPGApplication().Quit();
         break;
      case ua_cargosummary: {
            tfield* fld = getSelectedField();
            if ( fld && fld->vehicle && fld->vehicle->getOwner() == actmap->actplayer ) 
               showUnitCargoSummary( fld->vehicle );
         }
         break;
      case ua_unitsummary: showUnitSummary( actmap );
         break;
      case ua_gamepreferences:
         editGameOptions();
         break;
      case ua_increase_zoom:
         if ( mainScreenWidget && mainScreenWidget->getMapDisplay() ) {
            mainScreenWidget->getMapDisplay()->changeZoom( 10 );
            viewChanged();
            repaintMap();
         }
         break;
      case ua_decrease_zoom:
         if ( mainScreenWidget && mainScreenWidget->getMapDisplay() ) {
            mainScreenWidget->getMapDisplay()->changeZoom( -10 );
            viewChanged();
            repaintMap();
         }
         break;
      case ua_selectgraphicset:
         selectgraphicset();
         break;
      case ua_networksupervisor:
         networksupervisor();
         displaymap();
         break;
         
      default:
         break;
   }

}




void execUserAction_ev( tuseractions action )
{
   getPGApplication().enableLegacyEventHandling ( true );
   execuseraction( action );
   getPGApplication().enableLegacyEventHandling ( false );
   execuseraction2( action );
}




bool mainloopidle(  )
{
   if ( actmap ) {
      while ( actmap->player[ actmap->actplayer ].queuedEvents )
         checkevents( &getDefaultMapDisplay() );

      checktimedevents( &getDefaultMapDisplay() );

      checkforvictory();
   }
   return false;
}



pfont load_font ( const char* name )
{
   tnfilestream stream ( name , tnstream::reading );
   return loadfont ( &stream );
}



void loaddata( int resolx, int resoly, const char *gameToLoad=NULL )
{

   schriften.smallarial = load_font("smalaril.fnt");
   schriften.large = load_font("usablack.fnt");
   schriften.arial8 = load_font("arial8.fnt");
   schriften.smallsystem = load_font("msystem.fnt");
   schriften.guifont = load_font("gui.fnt");
   schriften.guicolfont = load_font("guicol.fnt");
   schriften.monogui = load_font("monogui.fnt");

   dataLoaderTicker();

   GraphicSetManager::Instance().loadData();

   dataLoaderTicker();
   
   registerDataLoader ( new PlayListLoader() );
   registerDataLoader ( new BI3TranslationTableLoader() );
   
   dataLoaderTicker();
   
   loadAllData();
  
   
   activefontsettings.markfont = schriften.guicolfont;
   shrinkfont ( schriften.guifont, -1 );
   shrinkfont ( schriften.guicolfont, -1 );
   shrinkfont ( schriften.monogui, -1 );
   pulldownfont = schriften.smallarial ;

   dataLoaderTicker();

   SoundList::init();

   dataLoaderTicker();

   loadpalette();
   
   dataLoaderTicker();
   
   loadmessages();

   dataLoaderTicker();

   loadUnitSets();

   loadStartupMap( gameToLoad );

   dataLoaderTicker();

   displayLogMessage ( 6, "done\n" );

   dataLoaderTicker();

 
   registerGuiFunctions( GuiFunctions::primaryGuiIcons );

   hookReplayToSystem();
}






class GameThreadParams: public SigC::Object
{
   private:
      bool exit() { exitMainloop = true; return true; };
   public:   
      ASCString filename;
      ASC_PG_App& application;
      bool exitMainloop;
      GameThreadParams( ASC_PG_App& app ) : application ( app ), exitMainloop(false) 
      {
         app.sigQuit.connect( SigC::slot( *this, &GameThreadParams::exit ));
      };
};

int gamethread ( void* data )
{
   GameThreadParams* gtp = (GameThreadParams*) data;

   loadpalette();

   int resolx = agmp->resolutionx;
   int resoly = agmp->resolutiony;
   virtualscreenbuf.init();

   {
      StartupScreen sus( "title.jpg", dataLoaderTicker );
   
      try {
         loaddata( resolx, resoly, gtp->filename.c_str() );
      }
      catch ( ParsingError err ) {
         fatalError ( "Error parsing text file " + err.getMessage() );
      }
      catch ( tfileerror err ) {
         fatalError ( "Error loading file " + err.getFileName() );
      }
      catch ( ASCexception ) {
         fatalError ( "loading of game failed" );
      }
      catch ( ThreadExitException ) {
         displayLogMessage(0, "caught thread exiting exception, shutting down");
         return -1;
      }
      catch ( ... ) {
         fatalError ( "caught undefined exception" );
      }
      
      displayLogMessage ( 5, "loaddata completed successfully.\n" );
      dataLoaderTicker();
      
      displayLogMessage ( 5, "starting music..." );
      startMusic();
      displayLogMessage ( 5, " done \n" );
      dataLoaderTicker();
      
      repaintDisplay.connect( repaintMap );
      
      mainScreenWidget = new ASC_MainScreenWidget( getPGApplication());
      dataLoaderTicker();
   }
   
   displayLogMessage ( 5, "entering outer main loop.\n" );
   do {
      try {
         if ( !actmap || actmap->xsize <= 0 || actmap->ysize <= 0 ) {
            displayLogMessage ( 8, "gamethread :: starting main menu.\n" );
            GameDialog::gameDialog();
         } else {
            if ( actmap->actplayer == -1 ) {
               displayLogMessage ( 8, "gamethread :: performing next_turn..." );
               next_turn();
               displayLogMessage ( 8, "done.\n" );
            }

            moveparams.movestatus = 0;

            updateFieldInfo();

            displayLogMessage ( 4, "Spawning MainScreenWidget\n ");

            mainScreenWidget->Show();

            displayLogMessage ( 7, "Entering main event loop\n");
   
            getPGApplication().Run();
            displayLogMessage ( 7, "mainloop exited\n");
         }
      } /* endtry */
      catch ( NoMapLoaded ) { 
         delete actmap;
         actmap = NULL;
      } /* endcatch */
      catch ( ShutDownMap ) { 
         delete actmap;
         actmap = NULL;
      }
      catch ( LoadNextMap lnm ) {
         if ( actmap->campaign.avail ) {
            startnextcampaignmap( lnm.id );
         } else {
           viewtext2(904);
           if (choice_dlg("Do you want to continue playing ?","~y~es","~n~o") == 2) {
              delete actmap;
              actmap = NULL;
           } else {
              actmap->continueplaying = 1;
              if ( actmap->replayinfo ) {
                 delete actmap->replayinfo;
                 actmap->replayinfo = NULL;
              }
           }
         }
      }
   } while ( !gtp->exitMainloop );
   return 0;
}


void deployMapPlayingHooks ( GameMap* map )
{
   map->sigPlayerTurnBegins.connect( SigC::slot( initReplayLogging ));
   map->sigPlayerTurnBegins.connect( SigC::slot( transfer_all_outstanding_tribute ));   
}




// including the command line parser, which is generated by genparse
#include "clparser/asc.cpp"


int main(int argc, char *argv[] )
{
   // setenv( "DISPLAY", "192.168.0.61:0", 1 );

   assert ( sizeof(PointerSizedInt) == sizeof(int*));


   // we should think about replacing clparser with libpopt
   Cmdline* cl = NULL;
   try {
      cl = new Cmdline ( argc, argv );
   } catch ( string s ) {
      cerr << s;
      exit(1);
   }
   auto_ptr<Cmdline> apcl ( cl );

   if ( cl->v() ) {
      ASCString msg = getstartupmessage();
      printf( msg.c_str() );
      exit(0);
   }

   if ( cl->w() )
      fullscreen = SDL_FALSE;

   if ( cl->f() )
      fullscreen = SDL_TRUE;

   MessagingHub::Instance().setVerbosity( cl->r() );
   StdIoErrorHandler stdIoErrorHandler;
   MessagingHub::Instance().exitHandler.connect( SigC::bind( SigC::slot( exit_asc ), -1 ));

#ifdef WIN32
   Win32IoErrorHandler* win32ErrorDialogGenerator = new Win32IoErrorHandler;
#endif


   displayLogMessage( 1, getstartupmessage() );

   initFileIO( cl->c() );  // passing the filename from the command line options

   try {
      checkDataVersion();
      // check_bi3_dir ();
   } catch ( tfileerror err ) {
      displaymessage ( "unable to access file %s \n", 2, err.getFileName().c_str() );
   }
   catch ( ... ) {
      displaymessage ( "loading of game failed during pre graphic initializing", 2 );
   }

   if ( CGameOptions::Instance()->forceWindowedMode && !cl->f() )  // cl->f == force fullscreen command line param
      fullscreen = SDL_FALSE;

   int xr = 1024;
   int yr = 768;
   // determining the graphics resolution
   
   if ( CGameOptions::Instance()->xresolution != 1024 )
      xr = CGameOptions::Instance()->xresolution;
   if ( cl->x() != 1024 )
      xr = cl->x();

   if ( CGameOptions::Instance()->yresolution != 768 )
      yr = CGameOptions::Instance()->yresolution;
   if ( cl->y() != 768 )
      yr = cl->y();

   
   SoundSystem soundSystem ( CGameOptions::Instance()->sound.muteEffects, CGameOptions::Instance()->sound.muteMusic, cl->q() || CGameOptions::Instance()->sound.off );

   soundSystem.setMusicVolume ( CGameOptions::Instance()->sound.musicVolume );
   soundSystem.setEffectVolume ( CGameOptions::Instance()->sound.soundVolume );


   
   tspfldloaders::mapLoaded.connect( SigC::slot( deployMapPlayingHooks ));

   ASC_PG_App app ( "asc2_dlg" );

   app.sigAppIdle.connect ( SigC::slot( mainloopidle ));

   cursorMoved.connect( updateFieldInfo );

//   int flags = SDL_HWSURFACE; 
   int flags = SDL_SWSURFACE;
   if ( fullscreen )
      flags |= SDL_FULLSCREEN;


   app.setIcon( "program-icon.png" );
   app.InitScreen( xr, yr, 32, flags);
  
#ifdef WIN32
   delete win32ErrorDialogGenerator;
#endif


   setWindowCaption ( "Advanced Strategic Command" );
      
   GameThreadParams gtp ( app );
   gtp.filename = cl->l();

   if ( cl->next_param() < argc )
      for ( int i = cl->next_param(); i < argc; i++ )
         gtp.filename = argv[i];

   int returncode = 0;
   try {
      // this starts the gamethread procedure, whichs will run the entire game
      returncode = initializeEventHandling ( gamethread, &gtp );
   }
   catch ( bad_alloc ) {
      fatalError ("Out of memory");
   }

   delete actmap;
   actmap = NULL;
   
   writegameoptions ( );

   return( returncode );
}

