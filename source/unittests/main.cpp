

#include "ai/ai.h"
#include "loaders.h"
#include "dlg_box.h"
#include "controls.h"
#include "dlg_box.h"
#include "strtmesg.h"
#include "sg.h"
#include "viewcalculation.h"
#include "replay.h"
#include "graphicset.h"
#include "loadbi3.h"
#include "messagedlg.h"
#include "guifunctions.h"
#include "unitset.h"

#include "cannedmessages.h"
#include "resourcenet.h"
#include "mapimageexport.h"
#include "soundList.h"
#include "turncontrol.h"

#include "stdio-errorhandler.h"

#include "autotraining.h"


#ifdef WIN32
 MiniDumper miniDumper( "main" );
#endif

#include "movementtest.h"
#include "attacktest.h"
                
tfield*        getSelectedField(void)
{
   return actmap->getField( actmap->getCursor() ); 
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



void diplomaticChange( GameMap* gm,int p1,int p2)
{
   if ( p1 == gm->getPlayerView() || p2 == gm->getPlayerView() ) {
      computeview( gm );
      mapChanged( gm );
      repaintMap();
   }
}


int runTester ( )
{
   loadpalette();

   int resolx = 1000;
   int resoly = 1000;
   virtualscreenbuf.init();

   try {
      loaddata( resolx, resoly );
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
      return -1;
   }
#endif

   GameMap::sigMapDeletion.connect( SigC::slot( &resetActions ));
   GameMap::sigPlayerTurnEndsStatic.connect( SigC::slot( automaticTrainig ));

   suppressMapTriggerExecution = false;

   DiplomaticStateVector::shareViewChanged.connect( SigC::slot( &diplomaticChange ));
   

   testMovement();   
   testAttack();
   
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


void execuseraction ( tuseractions action ) {};
void execUserAction_ev ( tuseractions action ) {};
bool continueAndStartMultiplayerGame( bool mostRecent = false ) { return true;};
bool loadGame( bool mostRecent ) { return true;};
void saveGame( bool mostRecent ) {};


int main(int argc, char *argv[] )
{
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

#ifdef WIN32
   Win32IoErrorHandler* win32ErrorDialogGenerator = new Win32IoErrorHandler;
#endif


   displayLogMessage( 1, getstartupmessage() );

   ConfigurationFileLocator::Instance().setExecutableLocation( argv[0] );
   initFileIO( cl->c() );  // passing the filename from the command line options

   SoundSystem soundSystem ( true, true, true );
   
   try {
      checkDataVersion();
      // check_bi3_dir ();
   } catch ( tfileerror err ) {
      displaymessage ( "unable to access file %s \n", 2, err.getFileName().c_str() );
   }
   catch ( ... ) {
      displaymessage ( "loading of game failed during pre graphic initializing", 2 );
   }

   tspfldloaders::mapLoaded.connect( SigC::slot( deployMapPlayingHooks ));

   PG_FileArchive archive( argv[0] );

   try {
      runTester();
   }
   catch ( bad_alloc ) {
      fatalError ("Out of memory");
      return 1;
   }
   catch ( ASCmsgException e ) {
      cerr << e.getMessage() << "\n";
      return 2;
   }
   catch ( ActionResult ar ) {
      cerr << "ActionResult failed:" << ar.getCode() << " : " << ar.getMessage() << "\n";
      return 2;
   }
   catch ( ... ) {
      cerr << "caught exception\n";
      return 2;  
   }

   cout << "exiting successfully \n";
   return 0;
}
