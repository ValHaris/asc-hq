

#include "ai/ai.h"
#include "loaders.h"
#include "dlg_box.h"
#include "dlg_box.h"
#include "strtmesg.h"
#include "sg.h"
#include "viewcalculation.h"
#include "replay.h"
#include "messagedlg.h"

#include "researchexecution.h"
#include "resourcenet.h"
#include "mapimageexport.h"
#include "soundList.h"
#include "turncontrol.h"

#include "stdio-errorhandler.h"

#include "autotraining.h"

#include "actiontest.h"
#include "movementtest.h"
#include "attacktest.h"
#include "ai-move1.h"
#include "viewtest.h"
#include "ai-service1.h"
#include "transfercontroltest.h"
#include "recyclingtest.h"                
#include "researchtest.h"
#include "applicationstarter.h"
#include "diplomacytest.h"
#include "objectconstructiontest.h"
#include "eventtest.h"
#include "gameeventsystem.h"
#include "jumptest.h"
#include "testversionidentifier.h"


void viewcomp( Player& player )
{
   computeview( player.getParentMap() );
}

void hookGuiToMap( GameMap* map )
{
   if ( !map->getGuiHooked() ) {

      map->sigPlayerUserInteractionBegins.connect( SigC::slot( &viewcomp ) );
      map->sigPlayerUserInteractionBegins.connect( SigC::hide<Player&>( repaintMap.slot() ));
      
      map->sigPlayerUserInteractionBegins.connect( SigC::hide<Player&>( SigC::slot( &checkforreplay )));
      map->sigPlayerUserInteractionBegins.connect( SigC::slot( &viewunreadmessages ));
      map->sigPlayerUserInteractionBegins.connect( SigC::slot( &checkJournal ));
      map->sigPlayerUserInteractionBegins.connect( SigC::slot( &checkUsedASCVersions ));
      map->sigPlayerUserInteractionBegins.connect( SigC::hide<Player&>( updateFieldInfo.slot() ));

      map->sigPlayerTurnHasEnded.connect( SigC::slot( viewOwnReplay));
      map->guiHooked();
   }
}

bool loadGameFromFile( const ASCString& filename )
{
   return false;
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


void testStreamEncoding()
{
   const int maxsize = 100000;
   char buffer[maxsize];

   int size;

   {
      tnfilestream stream ( "asc2_dlg.zip", tnstream::reading );
      size = stream.readdata(buffer, maxsize, false );
   }

   ASCString encodedData;

   {
      ASCIIEncodingStream stream;
      stream.writedata(buffer, size);
      encodedData = stream.getResult();
   }

   {
      char buffer2[maxsize];

      ASCIIDecodingStream stream ( encodedData );

      int size2 = stream.readdata( buffer2, size, true);
      assertOrThrow( size2 == size );

      for ( int i = 0; i < size; ++i )
         if ( buffer[i] != buffer2[i] )
            throw ASCmsgException( "ASCII Encoding/Decoding failed at offset " + ASCString::toString(i));
   }
}

void testStreamEncoding2()
{
   const int maxsize = 100000;
   char buffer[maxsize];

   int size;

   {
      tnfilestream stream ( "asc2_dlg.zip", tnstream::reading );
      size = stream.readdata(buffer, maxsize, false );
   }

   ASCString encodedData;

   {
      ASCIIEncodingStream outerStream;
      StreamCompressionFilter stream( &outerStream );
      stream.writedata(buffer, size);
      stream.close();

      encodedData = outerStream.getResult();
   }

   {
      char buffer2[maxsize];

      ASCIIDecodingStream outerStream ( encodedData );
      StreamDecompressionFilter stream( &outerStream );

     int size2 = stream.readdata( buffer2, size, true);
     assertOrThrow( size2 == size );

      for ( int i = 0; i < size; ++i )
         if ( buffer[i] != buffer2[i] )
            throw ASCmsgException( "ASCII Encoding/Decoding failed at offset " + ASCString::toString(i));
   }

}


void runUnitTests()
{
   testStreamEncoding();
   testStreamEncoding2();
   testVersionIdentifier();
   testJumpdrive();
   testEvents();
   testActions();
   testObjectConstruction();
   testResearch();
   testDiplomacy();
   testRecycling();
   testTransferControl();
   testAiService();
   testMovement();   
   testAttack();
   testAiMovement();
   testView();
}     
      

void checkGameEvents( GameMap* map,const Command& command )
{
   checktimedevents( map, NULL );
   checkevents( map, NULL );
}


int runTester ( )
{
   loadpalette();

   virtualscreenbuf.init();

   try {
      loaddata();
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

   GameMap::sigPlayerTurnEndsStatic.connect( SigC::slot( automaticTrainig ));
   //ActionContainer::postActionExecution.connect( SigC::slot( &checkGameEvents ));

   suppressMapTriggerExecution = false;

   runUnitTests();
   
   return 0;
}



static void __runResearch( Player& player ){
   runResearch( player, NULL, NULL );  
}

void deployMapPlayingHooks ( GameMap* map )
{
   map->sigPlayerTurnBegins.connect( SigC::slot( initReplayLogging ));
   map->sigPlayerTurnBegins.connect( SigC::slot( transfer_all_outstanding_tribute ));   
   map->sigPlayerTurnBegins.connect( SigC::slot( __runResearch ));
}




// including the command line parser, which is generated by genparse
#include "clparser/asc.cpp"


void execuseraction ( tuseractions action ) {};
void execUserAction_ev ( tuseractions action ) {};


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
      printf( "%s", msg.c_str() );
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

   ConfigurationFileLocator::Instance().setExecutableLocation( argv[0] );
   initFileIO( cl->c() );  // passing the filename from the command line options

   SoundSystem soundSystem ( true, true, true );

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
