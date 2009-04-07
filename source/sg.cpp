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
 
   THE central class of ASC is GameMap in gamemap.h . 
   It is the anchor where nearly all elements of ASC are chained to. The global 
   variable #actmap is a pointer to the active map. There can be a maximum of
   8 #Player on a map, plus neutral units (which are handled like a 9th #Player).
   Hence the array of 9 #GameMap::Player classes in #GameMap.
   
   Each player has units and buildings, which are stored in the lists 
   #Player::vehicleList and #Player::buildingList .
   The terms units and vehicles are used synonymously in ASC. Since unit was a 
   reserved word in Borland Pascal (the language that ASC was originally written in),
   we decided to use the term vehicle instead. But now, with ASC written in C++, 'unit' is also used.
   
   Every building and unit is of a certain 'type': Vehicletype and BuildingType .
   These are stored in the data files which are loaded on startup and are globally 
   available. They are not modified during runtime in any way and are referenced
   by the indiividual instances of Vehicle and Building. 
   The Vehicletype has information that are shared  by all vehicles of this 'type', like speed, 
   weapon systems, accessable terrain etc, while the vehicle stores things like remaining movement for this
   turn, ammo, fuel and cargo.
   
   The primary contents of a GameMap are its fields ( #tfield ). Each field has again a pointer 
   to a certain weather of a TerrainType. Each TerrainType has up to 5 
   different weathers ("dry (standard)","light rain", "heavy rain", "few snow",
   "lot of snow"). If there is a #Vehicle or a #Building standing on a field, the field
   has a pointer to it: tfield::vehicle and tfield::building .
   
   On the field can be several instances of Object. Objects are another central class of 
   ASC. Roads, pipleines, trenches and woods are examples of objects. #Vehicle and #Building
   are NOT objects. Objects modify the terrain for the field, like changing movement cost or adding
   defense bonus. There is again a class #ObjectType containing immutable properties.
   
   \section Interacting Interacting with the game's artefacts
   
   \link gameinteraction See dedicated page \endlink

   \section The basic class structure of ASC
   
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
#include <fstream>

#include <boost/regex.hpp>

#include "paradialog.h"

#include "vehicletype.h"
#include "buildingtype.h"
#include "ai/ai.h"
#include "misc.h"
#include "events.h"
#include "typen.h"
#include "spfst.h"
#include "loaders.h"
#include "dlg_box.h"
#include "controls.h"
#include "dlg_box.h"
#include "dialog.h"
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
#include "unitset.h"

#ifdef WEATHERGENERATOR
# include "weathercast.h"
#endif

#include "asc-mainscreen.h"
#include "dialogs/unitinfodialog.h"
#include "util/messaginghub.h"
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
#include "networksupervisor.h"

#include "dialogs/newgame.h"
#include "dialogs/soundsettings.h"
#include "dialogs/alliancesetup.h"
#include "dialogs/unitcounting.h"
#include "dialogs/editgameoptions.h"
#include "dialogs/nextcampaignmap.h"
#include "dialogs/terraininfo.h"
#include "dialogs/editplayerdata.h"
#include "dialogs/locatefile.h"
#include "stdio-errorhandler.h"
#include "widgets/textrenderer.h"
#include "dialogs/productionanalysis.h"
#include "dialogs/fileselector.h"
#include "containerbase-functions.h"
#include "memory-measurement.h"
#include "dialogs/mailoptionseditor.h"
#include "dialogs/unitguidedialog.h"

#include "autotraining.h"

#ifdef LUAINTERFACE
# include "lua/luarunner.h"
# include "lua/luastate.h"
#endif


#ifdef WIN32
# include "win32/win32-errormsg.h"
# include  "win32/msvc/mdump.h"
 MiniDumper miniDumper( "main" );
# include <direct.h>
# include <stdlib.h>
# include <stdio.h>
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
      map->sigPlayerUserInteractionBegins.connect( SigC::hide<Player&>( SigC::slot( &checkforreplay )));
      map->sigPlayerUserInteractionBegins.connect( SigC::slot( &researchCheck ));
      map->sigPlayerUserInteractionBegins.connect( SigC::slot( &viewunreadmessages ));
      map->sigPlayerUserInteractionBegins.connect( SigC::slot( &checkJournal ));
      map->sigPlayerUserInteractionBegins.connect( SigC::slot( &checkUsedASCVersions ));
      map->sigPlayerUserInteractionBegins.connect( SigC::hide<Player&>( updateFieldInfo.slot() ));

      map->sigPlayerUserInteractionEnds.connect( SigC::slot( closePlayerReplayLogging ));

      map->sigPlayerTurnHasEnded.connect( SigC::slot( viewOwnReplay));
      map->guiHooked();
   }
}


bool loadGameFromFile( const ASCString& filename )
{
   GameMap* m = mapLoadingExceptionChecker( filename, MapLoadingFunction( tsavegameloaders::loadGameFromFile ));
   if ( !m )
      return false;

   delete actmap;
   actmap = m;
   actmap->levelfinished = false;

   if ( actmap->replayinfo ) {
      if ( actmap->replayinfo->actmemstream )
         displaymessage2( "actmemstream already open at begin of turn ",2 );

      if ( actmap->replayinfo->guidata[actmap->actplayer] )
         actmap->replayinfo->actmemstream = new tmemorystream ( actmap->replayinfo->guidata[actmap->actplayer], tnstream::appending );
      else {
         actmap->replayinfo->guidata[actmap->actplayer] = new tmemorystreambuf;
         actmap->replayinfo->actmemstream = new tmemorystream ( actmap->replayinfo->guidata[actmap->actplayer], tnstream::writing );
      }
   }
   
   computeview( actmap );
   hookGuiToMap ( actmap );
   return true;
}


bool loadGame( bool mostRecent )
{
   ASCString s1;
   if ( mostRecent ) {
      int datefound = 0;

      tfindfile ff ( savegameextension );
      tfindfile::FileInfo fi;
      while ( ff.getnextname( fi ))
         if ( fi.date > datefound ) {
            datefound = fi.date;
            s1 = fi.name;
         }
   } else {
      s1 = selectFile( savegameextension, true );
   }

   if ( !s1.empty() ) {
      StatusMessageWindowHolder smw = MessagingHub::Instance().infoMessageWindow( "loading " + s1 );

      loadGameFromFile( s1 );
      
      updateFieldInfo();
      positionCursor( actmap->getCurrentPlayer() );
      getDefaultMapDisplay().displayPosition( actmap->getCursor() );
      displaymap();

      return true;
   } else
      return false;
}


void saveGame( bool as )
{
   if ( !actmap )
      return;

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


enum MapTypeLoaded { None, Map, Savegame, Mailfile };

MapTypeLoaded loadStartupMap ( const char *gameToLoad=NULL )
{
   if ( gameToLoad && gameToLoad[0] ) {
      try {
         if ( patimat ( ASCString("*")+tournamentextension, gameToLoad )) {

            if( validateemlfile( gameToLoad ) == 0 )
               fatalError( "Email gamefile %s is invalid. Aborting.", gameToLoad );

            try {

               if ( continuenetworkgame( ASCString(gameToLoad) )) 
                  hookGuiToMap(actmap);

               return Mailfile;
            } catch ( tfileerror ) {
               fatalError ( "%s is not a legal email game.", gameToLoad );
            }
         } else if( patimat ( savegameextension, gameToLoad )) {
            if( validatesavfile( gameToLoad ) == 0 )
               fatalError ( "The savegame %s is invalid. Aborting.", gameToLoad );

            try {
               loadGameFromFile( gameToLoad );
               computeview( actmap );
               return Savegame;
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
      return Map;
      displayLogMessage ( 6, "done\n" );
   }

   return None;
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


void benchgame ( bool withViewCalc )
{
   int t2;
   int t = ticker;
   int n = 0;
   do {
      if ( withViewCalc ) 
         computeview( actmap );

      repaintMap();
            
      n++;
      t2 = ticker;
   } while ( t + 1000 > t2 ); /* enddo */
   double d = 100 * n;
   d /= (t2-t);
   char buf[100];
   sprintf ( buf, "%3.1f fps", d );
   infoMessage ( buf );
}


void showSearchPath()
{

   ASCString s = "#fontsize=17#ASC search path#fontsize=13#\n";
      for ( int i = 0; i < getSearchPathNum(); ++i )
         s += getSearchPath ( i ) + "\n"; 

      s += "\n";
      s += "Configuration file used: \n";
      s += getConfigFileName();

#ifdef WIN32
     char buffer[_MAX_PATH];

     if( _getcwd( buffer, _MAX_PATH ) ) {
        s += "\n#fontsize=17#Current working directory#fontsize=13#\n";
         s += buffer;
     }
#endif

     s += "\n\n#fontsize=17#Mounted archive files#fontsize=13#\n";
     s += listContainer();


     ViewFormattedText vft("ASC directories", s, PG_Rect( -1, -1, 400, 400 ));
     vft.Show();
     vft.RunModal();
}



void changePassword( GameMap* gamemap )
{
   if ( Player::getHumanPlayerNum( gamemap) < 2 ) {
      infoMessage ("Passwords are only used for multiplayer games");
      return;
   }

   bool success;
   do {
      Password oldpwd = gamemap->player[gamemap->actplayer].passwordcrc;
      gamemap->player[gamemap->actplayer].passwordcrc.reset();
      success = enterpassword ( gamemap->player[gamemap->actplayer].passwordcrc, true, true );
      if ( !success )
         gamemap->player[gamemap->actplayer].passwordcrc = oldpwd;
   } while ( gamemap->player[gamemap->actplayer].passwordcrc.empty() && success && viewtextquery ( 910, "warning", "~e~nter password", "~c~ontinue without password" ) == 0 ); /* enddo */
}

void showSDLInfo()
{
   ASCString s;
   s += "#fontsize=18#SDL versions#fontsize=14#\n";
   char buf[1000];
   SDL_version compiled;
   SDL_VERSION(&compiled);
   sprintf(buf, "\nCompiled with SDL version: %d.%d.%d\n", compiled.major, compiled.minor, compiled.patch);
   s += buf;

   sprintf(buf, "Linked with SDL version: %d.%d.%d\n", SDL_Linked_Version()->major, SDL_Linked_Version()->minor, SDL_Linked_Version()->patch);
   s += buf;

   s += "Byte order is ";
#if SDL_BYTEORDER==SDL_LIL_ENDIAN
   s += "little endian\n";
#else
#if SDL_BYTEORDER==SDL_BIG_ENDIAN
   s += "big endian\n";
#else
   s += "undefined\n";
#endif
#endif
  
   s += "Graphics backend: ";
   s += SDL_VideoDriverName( buf, 1000 );
   s += "\n";


   const SDL_VideoInfo* videoInfo = SDL_GetVideoInfo();
   s += "VideoInfo: \n";

   s += "Hardware surfaces available: ";
   s += videoInfo->hw_available ? "yes" : "no";


   s += "\nScreen uses hardware surface: ";
   s += PG_Application::GetScreen()->flags & SDL_HWSURFACE ? "yes" : "no";

   s += "\nWindow manager available: ";
   s += videoInfo->wm_available ? "yes" : "no";

   s += "\nhardware to hardware blits accelerated: ";
   s += videoInfo->blit_hw ? "yes" : "no";

   s += "\nhardware to hardware colorkey blits accelerated: ";
   s += videoInfo->blit_hw_CC ? "yes" : "no";

   s += "\nhardware to hardware alpha blits accelerated: ";
   s += videoInfo->blit_hw_A ? "yes" : "no";

   s += "\nsoftware to hardware blits accelerated: ";
   s += videoInfo->blit_sw ? "yes" : "no";

   s += "\nsoftware to hardware colorkey blits accelerated: ";
   s += videoInfo->blit_sw_CC ? "yes" : "no";

   s += "\nsoftware to hardware alpha blits accelerated: ";
   s += videoInfo->blit_sw_A ? "yes" : "no";

   s += "\ncolor fills accelerated: ";
   s += videoInfo->blit_fill ? "yes" : "no";

   s += "\nVideo memory: ";
   s += ASCString::toString( int(videoInfo->video_mem ));

   ViewFormattedText vft( "SDL Settings", s, PG_Rect(-1,-1,450,550));
   vft.Show();
   vft.RunModal();
}

void helpAbout()
{
   ASCString s = "#fontsize=22#Advanced Strategic Command#fontsize=14#\n";
   s += getVersionAndCompilation();

   s += "\n#fontsize=18#Credits#fontsize=14#\n";

   s += readtextmessage( 30 );
                     
   ViewFormattedText vft( "About", s, PG_Rect(-1,-1,450,550));
   vft.Show();
   vft.RunModal();
}


Context createContext( GameMap* gamemap )
{
   Context context;
   
   context.gamemap = gamemap;
   context.actingPlayer = &gamemap->getPlayer( gamemap->actplayer );
   context.parentAction = NULL;
   context.display = &getDefaultMapDisplay();
   context.viewingPlayer = gamemap->getPlayerView(); 
   context.actionContainer = &gamemap->actions;
   return context;   
}

void undo()
{
   if ( actmap ) {
      actmap->actions.undo( createContext( actmap ) );  
      displaymap();
      mapChanged(actmap);
      updateFieldInfo();
   }
}

void redo()
{
   if ( actmap ) {
      actmap->actions.redo( createContext( actmap ) );  
      displaymap();
      mapChanged(actmap);
      updateFieldInfo();
   }
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

      case ua_visibilityInfo:
         if ( mainScreenWidget ) 
            mainScreenWidget->toggleMapLayer( "visibilityvalue");
         displaymap();
         break;

      case ua_showCargoLayer:
         if ( mainScreenWidget ) 
            mainScreenWidget->toggleMapLayer( "container");
         displaymap();
         break;

      case ua_benchgamewov:
         benchgame( false );
         break;

      case ua_benchgamewv :
         benchgame( true );
         break;

      case ua_writescreentopcx:
         {
            ASCString name = getnextfilenumname ( "screen", "pcx", 0 );
            Surface s ( PG_Application::GetScreen() );
            writepcx ( name, s);
            displaymessage2( "screen saved to %s", name.c_str() );
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
            infoMessage("Sorry, this function has been disabled when starting the map!");
         break;

      case ua_newmessage:
         newmessage();
         break;

      case ua_viewqueuedmessages:
         viewmessages( "queued messages", actmap->unsentmessage, 1 );
         break;

      case ua_viewsentmessages:
         viewmessages( "sent messages", actmap->player[ actmap->actplayer ].sentmessage, 0);
         break;

      case ua_viewreceivedmessages:
         viewmessages( "received messages", actmap->player[ actmap->actplayer ].oldmessage, 0 );
         break;

      case ua_viewjournal:
         viewjournal( true );
         break;

      case ua_editjournal:
         editjournal();
         break;

      case ua_viewaboutmessage:
         helpAbout();
         break;

      case ua_viewlayerhelp:
         help(49);
         break;

      case ua_SDLinfo:
         showSDLInfo();
         break;

      case ua_toggleunitshading: 
         {
            CGameOptions::Instance()->units_gray_after_move = !CGameOptions::Instance()->units_gray_after_move;
            CGameOptions::Instance()->setChanged();
            displaymap();
            while ( mouseparams.taste )
               releasetimeslice();

            ASCString condition;
            if ( CGameOptions::Instance()->units_gray_after_move )
               condition = "- thay can't move";
            else
               condition = "- thay can't move AND\n- thay can't shoot";

            infoMessage ("units that now displayed shaded when:\n" + condition);
         }

         break;

      case ua_computerturn:
         if ( maintainencecheck() ) {
            displaymessage("This function is under development and for programmers only\n"
                           "unpredictable things may happen ...",3 ) ;

            if (choice_dlg("do you really want to start the AI?","~y~es","~n~o") == 1) {

               if ( !actmap->player[ actmap->actplayer ].ai )
                  actmap->player[ actmap->actplayer ].ai = new AI ( actmap, actmap->actplayer );

               savegame ( "aistart.sav" );
               actmap->player[ actmap->actplayer ].ai->run( &getDefaultMapDisplay() );
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
               ai->run( true, &getDefaultMapDisplay() );
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
               actmap->player[actmap->actplayer].research.cancel();
               logtoreplayinfo( rpl_cancelResearch );
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

            s += strrr ( actmap->player[actmap->actplayer].research.getResearchPerTurn() );

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
      case ua_undo:
         undo();
         break;
      case ua_redo:
         redo();
         break;

         
      default:;
      };
}

bool continueAndStartMultiplayerGame( bool mostRecent = false )
{
   if ( continuenetworkgame( mostRecent )) {
      hookGuiToMap(actmap);
      actmap->sigPlayerUserInteractionBegins( actmap->player[actmap->actplayer] );
      displaymap();
      return true;
   } else
      return false;
}


void ammoCounter( const ContainerBase* c, map<int,int>& amount )
{
   for ( int i = 0; i < weaponTypeNum; ++i )
      if ( weaponAmmo[i] )
         amount[i] += c->getAmmo(i,maxint );

   for ( int i = 1000; i < 1003; ++i )
      amount[i] += c->getAvailableResource( maxint, i-1000, 0 );

   for ( ContainerBase::Cargo::const_iterator i = c->getCargo().begin(); i != c->getCargo().end(); ++i )
      if ( *i )
         ammoCounter( *i, amount );
}


void showCargoSummary( tfield* fld )
{
   if ( !fld->vehicle ) {
      infoMessage( "Please select a unit");
      return;
   }

   if ( actmap->getCurrentPlayer().diplomacy.isAllied( fld->vehicle  )) {
      showUnitCargoSummary( fld->vehicle );
      map<int,int> ammo;
      ammoCounter( fld->vehicle, ammo );
      ASCString s;
      for ( int i = 0; i < weaponTypeNum; ++i )
         if ( weaponAmmo[i] )
            s += ASCString(cwaffentypen[i]) + ": " + ASCString::toString( ammo[i] ) + "\n";

      s += "\n";
      for ( int i = 1000; i < 1003; ++i )
         s += ASCString(Resources::name(i-1000)) + ": " + ASCString::toString( ammo[i] ) + "\n";

      ViewFormattedText vft("Ammo summary", s, PG_Rect( -1, -1, 300, 300 ));
      vft.Show();
      vft.RunModal();
   } else
      infoMessage( "The unit is not yours");
   
}


 class GotoPosition: public ASC_PG_Dialog {
      PG_LineEdit* xfield;
      PG_LineEdit* yfield;
      GameMap* gamemap;

      

      bool ok()
      {
         static boost::regex numercial("\\d+");

         if( boost::regex_match( xfield->GetText(), numercial)  &&
             boost::regex_match( yfield->GetText(), numercial)) {
               int xx = atoi( xfield->GetText() );
               int yy = atoi( yfield->GetText() );
               if ( xx >= 0 && yy >= 0 && xx < gamemap->xsize && yy < gamemap->ysize ) {
                  Hide();
                  MapDisplayPG* md = getMainScreenWidget()->getMapDisplay();
                  md->cursor.goTo( MapCoordinate( xx, yy) );
                  QuitModal();
                  return true;
               }
         }
         return false;
      }
      
      bool cancel()
      {
         QuitModal();
         return true;
      }

      static const int border  = 20;

      bool line1completed()
      {
         if ( yfield ) {
            yfield->EditBegin();
            return true;
         } else
            return false;
      }
      
   public:
      GotoPosition ( GameMap* gamemap ) : ASC_PG_Dialog( NULL, PG_Rect( -1, -1, 300, 120), "Enter Coordinates")
      {
         this->gamemap = gamemap;
         int fieldwidth = (Width()-3*border)/2;
         xfield = new PG_LineEdit( this, PG_Rect( border, 40, fieldwidth, 20));
         // xfield->SetText( ASCString::toString( gamemap->getCursor().x ));
         xfield->sigEditReturn.connect( SigC::slot( *this, &GotoPosition::line1completed ));

         yfield = new PG_LineEdit( this, PG_Rect( (Width()+border)/2, 40, fieldwidth, 20));
         // yfield->SetText( ASCString::toString( gamemap->getCursor().y ));
         yfield->sigEditReturn.connect( SigC::slot( *this, &GotoPosition::ok ));

         AddStandardButton( "~O~k" )->sigClick.connect( SigC::slot( *this, &GotoPosition::ok ));
      };

      int RunModal()
      {
         xfield->EditBegin();
         return ASC_PG_Dialog::RunModal();
      }
   };



class FontViewer : public ASC_PG_Dialog {
      static const int spacing = 30;
   public:
      FontViewer() : ASC_PG_Dialog( NULL, PG_Rect( -1, -1, 18 * spacing, 19 * spacing ), "view character set" ) 
      {
         for ( int i = 32; i < 255; ++i ) {
            ASCString s;
            s += char(i);
            new PG_Label ( this, PG_Rect( (i % 16 + 1) * spacing, (i / 16 + 2) * spacing, spacing, spacing ), s );
         }
      };
};

void viewFont()
{
   FontViewer fv;
   fv.Show();
   fv.RunModal();
}


void resourceAnalysis()
{
   ASCString s;
   Resources total;
   for ( Player::BuildingList::iterator j = actmap->player[actmap->actplayer].buildingList.begin(); j != actmap->player[actmap->actplayer].buildingList.end() ; j++ ) {
      Resources res = (*j)->getResource( Resources(maxint,maxint,maxint), true, 0 );
      MapCoordinate pos = (*j)->getPosition();
      s += (*j)->getName() + " #pos150#(" + ASCString::toString( pos.x ) + "/" + ASCString::toString( pos.y) + pos.toString() + ") : #pos300#" + ASCString::toString(res.energy) + "#pos400#" + ASCString::toString(res.material) + "#pos500#" + ASCString::toString(res.fuel) + "\n";
      total += res;
   }
   s += "\nTotal:\n";

   for ( int r = 0; r < 3; ++r)
      if ( actmap->isResourceGlobal(r))
         total.resource(r) = actmap->bi_resource[actmap->actplayer].resource(r);
   s += total.toString();

   s += "\nIncluding units:\n";
   for ( Player::VehicleList::iterator j = actmap->player[actmap->actplayer].vehicleList.begin(); j != actmap->player[actmap->actplayer].vehicleList.end() ; j++ ) 
      total += (*j)->getResource( Resources(maxint,maxint,maxint), true, 0 );

   s += total.toString();

   ViewFormattedText vft("Resource Analysis", s, PG_Rect( -1, -1, 600, 550 ));
   vft.Show();
   vft.RunModal();
}

void showUnitEndurance()
{
   
   vector<Vehicletype*> units;
   for ( int i = 0; i < vehicleTypeRepository.getNum(); ++i ) {
      Vehicletype* vt = vehicleTypeRepository.getObject_byPos(i);
      if ( vt && (vt->movemalustyp == MoveMalusType::medium_aircraft || 
                  vt->movemalustyp == MoveMalusType::light_aircraft  ||
                  vt->movemalustyp == MoveMalusType::heavy_aircraft  ||
                  vt->movemalustyp == MoveMalusType::helicopter))
         units.push_back( vt );
   }
   sort( units.begin(), units.end(), vehicleComp );
   
   ASCString s;
   for ( vector<Vehicletype*>::iterator i = units.begin(); i != units.end(); ++i )
   {
      ASCString u;
      ASCString range;
      if ( (*i)->fuelConsumption )
         range = ASCString::toString( (*i)->getStorageCapacity(0).fuel / (*i)->fuelConsumption);
      else
         range = "-";
      
      u.format( "#vehicletype=%d# %s : %d fuel ; %s fields range ; %d turns endurance \n", (*i)->id, (*i)->getName().c_str(), (*i)->getStorageCapacity(0).fuel, range.c_str(), UnitHooveringLogic::getEndurance(*i) );
      s += u;
   }
   
   ViewFormattedText vft("Unit Endurance", s, PG_Rect( -1, -1, 650, 550 ));
   vft.Show();
   vft.RunModal();
}


void showMemoryFootprint()
{
   MemoryMeasurement mm;
   mm.measureTypes();
   mm.measure( actmap );
   mm.measureIcons();
   
   ViewFormattedText vft("Memory Usage", mm.getResult(), PG_Rect( -1, -1, 750, 550 ));
   vft.Show();
   vft.RunModal();
}


void viewMiningPower()
{
   typedef map<const ContainerBaseType*,ASCString> InfoMap;
   InfoMap info;

   for ( Player::BuildingList::iterator i = actmap->getCurrentPlayer().buildingList.begin(); i != actmap->getCurrentPlayer().buildingList.end(); ++i )
      if ( (*i)->baseType->hasFunction( ContainerBaseType::MiningStation )) {
         int power;
         int output;
         MiningStation miningStation ( *i, true );
         for ( int r = 0; r < 3; ++r )
            if ( (*i)->maxplus.resource(r) ) {
               power = 100 * (*i)->plus.resource(r) / (*i)->maxplus.resource(r) ;
               break;
            }
            
         for ( int r = 0; r < 3; ++r )
            if ( miningStation.getPlus().resource(r) ) {
               output = miningStation.getPlus().resource(r);
               break;
            }
            
         ASCString txt = ASCString::toString(output) + " (" + ASCString::toString(power) + "%) " + (*i)->getPosition().toString() + " " + (*i)->baseType->name + "\n";
         info[(*i)->baseType] += txt;
      }

   ASCString fullText = "Mining Station Statistics\n\n";
   for ( InfoMap::iterator i = info.begin(); i != info.end(); ++i )
      fullText += i->second;


   ViewFormattedText vft("Mining Stations", fullText, PG_Rect( -1, -1, 750, 550 ));
   vft.Show();
   vft.RunModal();

}


void writeLuaCommands() 
{
   ASCString filename =  selectFile("*.lua", false );
   if ( !filename.empty() ) {
      tn_file_buf_stream stream ( filename, tnstream::writing );
      stream.writeString( actmap->actions.getCommands() ); 
   }
}

#ifdef LUAINTERFACE
void selectAndRunLuaScript()
{
   ASCString file = selectFile( "*.lua", true );
   if ( file.size() ) {
      LuaState state;
      executeFile( state, file );
      updateFieldInfo();
   }
}
#endif               


// user actions using the new event system
void execuseraction2 ( tuseractions action )
{
   switch ( action ) {
   
      case ua_unitweightinfo:
         if ( fieldvisiblenow  ( getSelectedField() )) {
            Vehicle* eht = getSelectedField()->vehicle;
            if ( eht && actmap->player[actmap->actplayer].diplomacy.getState( eht->getOwner()) >= PEACE_SV )
               infoMessage(" weight of unit: \n basic: " + ASCString::toString(eht->typ->weight) + "\n+cargo: " + ASCString::toString(eht->cargoWeight()) + "\n= " + ASCString::toString( eht->weight() ));
         }
         break;
      case ua_GameStatus:
         infoMessage ( "Current game time is:\n turn " + ASCString::toString( actmap->time.turn() ) + " , move " + ASCString::toString( actmap->time.move() ));
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
      case ua_viewActionPanel: mainScreenWidget->spawnPanel( ASC_MainScreenWidget::ActionInfo );
         break;
      case ua_vehicleinfo: unitInfoDialog();
         break;
#ifdef WEATHERGENERATOR
      case ua_weathercast: weathercast();
         break;
#endif
      case ua_newGame: 
         startMultiplayerGame();
         break;

      case ua_continuerecentnetworkgame:
         continueAndStartMultiplayerGame( true );
         break;

      case ua_continuenetworkgame:
         continueAndStartMultiplayerGame();
         break;
      case ua_loadgame: loadGame( false);
         break;
      case ua_loadrecentgame: loadGame ( true );
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
         writemaptopcx ( actmap, choice_dlg("Include View ?","~y~es","~n~o")==1  );
         break;
      case ua_exitgame:
         if (choiceDialog("do you really want to quit ?","~y~es","~n~o", "quitasc") == 1)
            getPGApplication().Quit();
         break;
      case ua_cargosummary: 
         showCargoSummary( getSelectedField() );
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
      case ua_researchinfo:
         researchinfo ();
         break;
      case ua_viewPipeNet:
         mainScreenWidget->getMapDisplay()->toggleMapLayer("pipes");
         repaintMap();
         break;
      case ua_showsearchdirs: showSearchPath();
         break;
      case ua_changepassword:
         changePassword( actmap );
         break;
      case ua_editPlayerData:
         editPlayerData( actmap );
         break;
      case ua_locatefile:
         locateFile();
         break;
      case ua_viewfont:
         viewFont();
         break;
      case ua_resourceAnalysis:
         resourceAnalysis();
         break;
      case ua_unitproductionanalysis:
         unitProductionAnalysis( actmap );
         break;
      case ua_gotoPosition: { 
         GotoPosition gp( actmap );
         gp.Show();
         gp.RunModal();
          break;
      };
      case ua_showTechAdapter: {
               ViewFormattedText vft("TechAdapter", actmap->getCurrentPlayer().research.listTriggeredTechAdapter(), PG_Rect( -1,-1,300,500));
               vft.Show();
               vft.RunModal();
                               };
         break;
      case ua_showUnitEndurance:  showUnitEndurance(); 
         break;
         
      case ua_getMemoryFootprint: showMemoryFootprint();
         break;

      case ua_showMiningPower: viewMiningPower();
         break;

      case ua_emailOptions: editEmailOptions();
         break;
      
      case ua_createReminder: newreminder(); 
         break;

      case ua_recompteview: 
         computeview(actmap);
         displaymap();
         break;
         
      case ua_unitGuideDialog:
         unitGuideWindow( 2);
         break;
         
      case ua_writeLuaCommands: writeLuaCommands();
         break;
         
#ifdef LUAINTERFACE 
      case ua_runLuaCommands: selectAndRunLuaScript();
         break;
      
#endif
                  
      default:
         break;
   }

}




void execUserAction_ev( tuseractions action )
{
   execuseraction( action );
   execuseraction2( action );
}




bool mainloopidle( PG_MessageObject* msgObj )
{
   if ( msgObj != PG_Application::GetApp())
      return false;

   if ( actmap ) {
      while ( actmap->player[ actmap->actplayer ].queuedEvents )
         if ( !checkevents( &getDefaultMapDisplay() ))
            return false;

      checktimedevents( &getDefaultMapDisplay() );

      checkforvictory( true );
   }
   return false;
}



pfont load_font ( const char* name )
{
   tnfilestream stream ( name , tnstream::reading );
   return loadfont ( &stream );
}


void resetActions( GameMap& map )
{
   pendingVehicleActions.reset();
   if ( NewGuiHost::pendingCommand ) {
      delete NewGuiHost::pendingCommand;
      NewGuiHost::pendingCommand = NULL;
   }
}


void loaddata( int resolx, int resoly )
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

   dataLoaderTicker();

   SoundList::init();

   dataLoaderTicker();

   loadpalette();
   
   dataLoaderTicker();
   
   loadmessages();

   dataLoaderTicker();

   loadUnitSets();

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

void diplomaticChange( GameMap* gm,int p1,int p2)
{
   if ( p1 == gm->getPlayerView() || p2 == gm->getPlayerView() ) {
      computeview( gm );
      mapChanged( gm );
      repaintMap();
   }
}


int gamethread ( void* data )
{
   GameThreadParams* gtp = (GameThreadParams*) data;

   loadpalette();

   int resolx = agmp->resolutionx;
   int resoly = agmp->resolutiony;
   virtualscreenbuf.init();

   std::auto_ptr<StartupScreen> startupScreen ( new StartupScreen( "title.jpg", dataLoaderTicker ));

   MapTypeLoaded mtl = None;

   try {
      loaddata( resolx, resoly );
      mtl = loadStartupMap( gtp->filename.c_str() );
   }
   catch ( ParsingError err ) {
      errorMessage ( "Error parsing text file " + err.getMessage() );
      return -1;
   }
   catch ( tfileerror err ) {
      errorMessage ( "Error loading file " + err.getFileName() );
      return -1;
   }
   catch ( ASCexception ) {
      errorMessage ( "loading of game failed" );
      return -1;
   }
   catch ( ThreadExitException ) {
      displayLogMessage(0, "caught thread exiting exception, shutting down");
      return -1;
   }

#ifndef _WIN32_
   // Windows/MSVC will catch access violations with this, which we don't want to, because it makes our dump files useless.
   catch ( ... ) {
      fatalError ( "caught undefined exception" );
   }
#endif

   GameMap::sigMapDeletion.connect( SigC::slot( &resetActions ));
   GameMap::sigPlayerTurnEndsStatic.connect( SigC::slot( automaticTrainig ));

   suppressMapTriggerExecution = false;
   

   displayLogMessage ( 5, "loaddata completed successfully.\n" );
   dataLoaderTicker();
   
   displayLogMessage ( 5, "starting music..." );
   startMusic();
   displayLogMessage ( 5, " done\n" );
   dataLoaderTicker();
   
   repaintDisplay.connect( repaintMap );

   DiplomaticStateVector::shareViewChanged.connect( SigC::slot( &diplomaticChange ));
   
   mainScreenWidget = new ASC_MainScreenWidget( getPGApplication());
   dataLoaderTicker();

   //! we are performing this the first time here while the startup logo is still active
   if ( actmap && actmap->actplayer == -1 ) {
      displayLogMessage ( 8, "Startup :: performing first next_turn..." );
      next_turn( actmap, NextTurnStrategy_AskUser() );
      displayLogMessage ( 8, "done.\n" );
   }

   displayLogMessage ( 5, "entering outer main loop.\n" );
   do {
      try {
         if ( !actmap || actmap->xsize <= 0 || actmap->ysize <= 0 ) {
            displayLogMessage ( 8, "gamethread :: starting main menu.\n" );
            startupScreen.reset();
            GameDialog::gameDialog();
            mtl = None;
         } else {
            if ( actmap->actplayer == -1 ) {
               displayLogMessage ( 8, "gamethread :: performing next_turn..." );
               next_turn( actmap, NextTurnStrategy_AskUser() );
               displayLogMessage ( 8, "done.\n" );
            } 

            updateFieldInfo();

            displayLogMessage ( 4, "Spawning MainScreenWidget\n ");

            mainScreenWidget->Show();
            startupScreen.reset();

            displayLogMessage ( 7, "Entering main event loop\n");
   
            if ( mtl == Mailfile ) 
               actmap->sigPlayerUserInteractionBegins( actmap->player[actmap->actplayer] );

            mtl = None;

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
            delete actmap;
            actmap = NULL;
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
   
   delete actmap;
   actmap = NULL;
   
   return 0;
}


void tributeTransfer( Player& player )
{
   logtoreplayinfo( rpl_transferTribute, player.getPosition() );
   transfer_all_outstanding_tribute( player );
}


void deployMapPlayingHooks ( GameMap* map )
{
   map->sigPlayerTurnBegins.connect( SigC::slot( initReplayLogging ));
   map->sigPlayerTurnBegins.connect( SigC::slot( tributeTransfer ));   
}




// including the command line parser, which is generated by genparse
#include "clparser/asc.cpp"


class ResourceLogger: public SigC::Object {
      ofstream s;
   public:
      ResourceLogger() {
         s.open("resource-log", ios_base::out | ios_base::trunc );
         MessagingHub::Instance().logCategorizedMessage.connect( SigC::slot( *this, &ResourceLogger::message ));
         MessagingHub::Instance().setLoggingCategory("ResourceWork", true);
      };

      void message( const ASCString& category, const ASCString& msg )
      {
         if ( category == "ResourceWork" )
            s << msg << "\n";
      }

      ~ResourceLogger() {
         s.close();
      }

};


void createUnitCostList()
{
   cout << "name;id;unitset id;new cost E;new cost M;old cost E;old cost M\n";
   for ( int i = 0; i < vehicleTypeRepository.getNum(); ++i ) {
      Vehicletype* v = vehicleTypeRepository.getObject_byPos(i);
      Resources oldc = v->calcProductionsCost2();
      Resources newc = v->calcProductionsCost();
      
      cout <<  v->getName() << ";" << v->id << ";" << getUnitSetID( v) << ";" << newc.energy << ";" << newc.material << ";" << oldc.energy << ";" << oldc.material << "\n";
   }
}


int main(int argc, char *argv[] )
{
   putenv(const_cast<char*>("SDL_VIDEO_CENTERED=1")) ;

   // putenv(const_cast<char*>("DISPLAY=192.168.0.21:0")) ;
   
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
   StdIoErrorHandler stdIoErrorHandler(false);
   MessagingHub::Instance().exitHandler.connect( SigC::bind( SigC::slot( exit_asc ), -1 ));

   // ResourceLogger rl;

#ifdef WIN32
   Win32IoErrorHandler* win32ErrorDialogGenerator = new Win32IoErrorHandler;
#endif


   displayLogMessage( 1, getstartupmessage() );

   ConfigurationFileLocator::Instance().setExecutableLocation( argv[0] );
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

   if ( CGameOptions::Instance()->graphicsDriver.compare_ci("default") != 0 ) {
      static char buf[100];
      strcpy(buf, "SDL_VIDEODRIVER=" );
      strncat( buf, CGameOptions::Instance()->graphicsDriver.c_str(), 100 - strlen(buf));
      buf[99] = 0;
      putenv( buf );
   }


   SoundSystem soundSystem ( CGameOptions::Instance()->sound.muteEffects, CGameOptions::Instance()->sound.muteMusic, cl->q() || CGameOptions::Instance()->sound.off );
   soundSystem.setMusicVolume ( CGameOptions::Instance()->sound.musicVolume );
   soundSystem.setEffectVolume ( CGameOptions::Instance()->sound.soundVolume );

   
   tspfldloaders::mapLoaded.connect( SigC::slot( deployMapPlayingHooks ));

   PG_FileArchive archive( argv[0] );
   ASC_PG_App app ( "asc2_dlg" );

   app.sigAppIdle.connect ( SigC::slot( mainloopidle ));

   cursorMoved.connect( updateFieldInfo );

   int flags = 0;

   if ( CGameOptions::Instance()->hardwareSurface )
      flags |= SDL_HWSURFACE;
   else
      flags |= SDL_SWSURFACE;

   if ( fullscreen )
      flags |= SDL_FULLSCREEN;

   app.setIcon( "program-icon.png" );
   bool initialized = false;
   if ( !app.InitScreen( xr, yr, 32, flags)) {
      if ( flags & SDL_FULLSCREEN ) {
         GetVideoModes gvm;
         if ( gvm.getList().size() > 0 ) {
            xr = gvm.getx(0);
            yr = gvm.gety(0);
            if ( app.InitScreen( xr, yr, 32, flags)) 
               initialized = true;
         }
      }
   } else
      initialized = true;

   if ( !initialized )
     fatalError( "Could not initialize video mode");


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

   writegameoptions ( );

   createUnitCostList();
   return( returncode );
}
