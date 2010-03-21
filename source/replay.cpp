/***************************************************************************
                          replay.cpp  -  description
                             -------------------
    begin                : Sun Jan 21 2001
    copyright            : (C) 2001 by Martin Bickel
    email                : bickel@asc-hq.org
 ***************************************************************************/

/*! \file replay.cpp
    \brief Everything for recording and playing replays

    Replays being the moves of other players or the AI, which can be viewed 
    later (especially useful in email games)
*/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <stdarg.h>
#include "replay.h"
#include "typen.h"
#include "buildingtype.h"
#include "vehicletype.h"
#include "spfst.h"
#include "controls.h"
#include "dialog.h"
#include "gameoptions.h"
#include "viewcalculation.h"
#include "itemrepository.h"
#include "containercontrols.h"
#include "resourcenet.h"
#include "guiiconhandler.h"
#include "guifunctions.h"
#include "cannedmessages.h"
#include "spfst-legacy.h"
#include "replaymapdisplay.h"
#include "asc-mainscreen.h"
#include "loaders.h"
#include "turncontrol.h"
#include "widgets/textrenderer.h"
#include "actions/jumpdrivecommand.h"
#include "reactionfire.h"
#include "gameeventsystem.h"
#include "sdl/graphicsqueue.h"
#include "video/videorecorder.h"
#include "iconrepository.h"
#include "dialogs/replayrecorder.h"
#include "sg.h"
#include "actions/action.h"
#include "actions/cargomovecommand.h"
#include "actions/constructbuildingcommand.h"
#include "actions/destructbuildingcommand.h"
#include "actions/destructunitcommand.h"
#include "actions/recycleunitcommand.h"
#include "actions/trainunitcommand.h"
#include "actions/moveunitcommand.h"
#include "actions/consumeammo.h"
#include "actions/consumeresource.h"
#include "actions/buildproductionlinecommand.h"
#include "actions/removeproductionlinecommand.h"
#include "actions/cancelresearchcommand.h"
#include "researchexecution.h"

trunreplay runreplay;

int startreplaylate = 0;


enum trpl_actions { rpl_attack,
   rpl_move, 
   rpl_changeheight, 
   rpl_convert, 
   rpl_remobj, 
   rpl_buildobj, 
   rpl_putbuilding,
   rpl_removebuilding, 
   rpl_putmine, 
   rpl_removemine,
   rpl_produceunit, 
   rpl_removeunit,
   rpl_trainunit,
   rpl_reactionfire, 
   rpl_finished, 
   rpl_shareviewchange, 
   rpl_alliancechange,
   rpl_move2, 
   rpl_buildtnk,
   rpl_refuel, 
   rpl_bldrefuel, 
   rpl_move3, 
   rpl_changeheight2,
   rpl_buildtnk2,
   rpl_moveUnitUpDown,
   rpl_move4,
   rpl_productionResourceUsage,
   rpl_buildtnk3,
   rpl_refuel2,
   rpl_buildobj2,
   rpl_remobj2,
   rpl_repairUnit,
   rpl_repairUnit2,
   rpl_refuel3,
   rpl_produceAmmo,
   rpl_buildtnk4,
   rpl_buildProdLine,
   rpl_removeProdLine,
   rpl_setResearch,
   rpl_techResearched,
   rpl_putbuilding2,
   rpl_setGeneratorStatus,
   rpl_cutFromGame,
   rpl_removebuilding2,
   rpl_setResourceProcessingAmount,
   rpl_removebuilding3,
   rpl_netcontrol,
   rpl_move5,
   rpl_alliancechange2,
   rpl_moveUnitUp,
   rpl_jump,
   rpl_repairBuilding,
   rpl_recycleUnit,
   rpl_convert2,
   rpl_putmine2,
   rpl_repairUnit3,
   rpl_transferTribute,
   rpl_reactionFireOn,
   rpl_reactionFireOff,
   rpl_selfdestruct,
   rpl_cancelResearch,
   rpl_runCommandAction };



class ReplayRecorder;

class ReplayRecorderWatcherGlobal {
      ReplayRecorder* recorder;
   public:
      ReplayRecorderWatcherGlobal()  : recorder( NULL ) {};
      void set(  ReplayRecorder* rec  ) {
         recorder = rec;
      }
      
      ~ReplayRecorderWatcherGlobal() ;
} replayRecorderWatcherGlobal;


 class ReplayRecorder : public SigC::Object {
   
   VideoRecorder* rec;
   SigC::Connection connection;
   bool movieModeStorage;
   ASCString lastFilename;
   
   public:
      ReplayRecorder() : rec (NULL)
      {
         movieModeStorage = CGameOptions::Instance()->replayMovieMode;
         replayRecorderWatcherGlobal.set( this );
      }
      
      void start( const ASCString& filename, bool append, int framerate, int quality )
      {
         lastFilename = filename;
         movieModeStorage = CGameOptions::Instance()->replayMovieMode;
         CGameOptions::Instance()->replayMovieMode = true;
         ASCString newFilename = constructFileName( 0, "", filename );
         if ( !rec || !append || newFilename != rec->getFilename()  ) {
            delete rec;
            rec = new VideoRecorder( newFilename, PG_Application::GetScreen(), framerate, quality );
         }
         
         if ( !connection.connected() )
            connection = postScreenUpdate.connect( SigC::slot( *this, &ReplayRecorder::screenUpdate ));
      }
      
      void pause()
      {
         if ( connection.connected() )
            connection.disconnect();
         CGameOptions::Instance()->replayMovieMode = movieModeStorage;
      }
      
      void close()
      {
         pause();
         delete rec;
         rec = NULL;
      }
      
      bool isRunning()
      {
         return connection.connected() && rec;
      }
      
      bool isOpen()
      {
         return rec != NULL;  
      }
      
      ASCString getLastFilename()
      {
         return lastFilename;
      }
   
   private:
      void screenUpdate( const SDL_Surface* surf )
      {
         if ( rec )
            rec->storeFrame( surf );
      }
};

class ReplayRecorderWatcherLocal {
      ReplayRecorder* recorder;
   public:
      ReplayRecorderWatcherLocal( ReplayRecorder* rec )  : recorder( rec ) {};
      ~ReplayRecorderWatcherLocal() 
      { 
         if ( recorder ) 
            recorder->pause();
      }
};


ReplayRecorderWatcherGlobal::~ReplayRecorderWatcherGlobal() 
{ 
   if ( recorder ) 
      recorder->pause();
}


ReplayRecorder* replayRecorder = NULL;

 

namespace ReplayGuiFunctions {

class ReplayPlay : public GuiFunction
{
   public:
      bool available( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         if ( runreplay.status == 1 )
            return true;

         return false;
      };

      void execute( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
        runreplay.status = 2;
        updateFieldInfo();
      }

      Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         return IconRepository::getIcon("replay-play.png");
      };

      ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         return "start re~p~lay";
      };

      bool checkForKey( const SDL_KeyboardEvent* key, int modifier, int num )
      {
         return ( key->keysym.unicode == 'p' );
      };
      
};


class ReplayPause : public GuiFunction
{
   public:
      bool available( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         if ( runreplay.status == 2 )
            return true;

         return false;
      };

      void execute( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
        runreplay.status = 1;
        updateFieldInfo();
      }

      Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         return IconRepository::getIcon("replay-pause.png");
      };

      ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         return "~p~ause replay";
      };
      
      bool checkForKey( const SDL_KeyboardEvent* key, int modifier, int num )
      {
         return ( key->keysym.unicode == 'p' );
      };
      
};



class ReplayFaster : public GuiFunction
{
   public:
      bool available( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         if ( runreplay.status == 2 )
            if ( CGameOptions::Instance()->replayspeed > 0 )
              return true;

         return false;
      };

      void execute( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         if ( CGameOptions::Instance()->replayspeed > 20 )
            CGameOptions::Instance()->replayspeed -= 20;
         else
            CGameOptions::Instance()->replayspeed = 0;

         CGameOptions::Instance()->setChanged ( 1 );
         displaymessage2 ( "delay set to %d / 100 sec", CGameOptions::Instance()->replayspeed );
         updateFieldInfo();
      }

      Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         return IconRepository::getIcon("replay-faster.png");
      };

      ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         return "increase replay speed (~+~)";
      };

      bool checkForKey( const SDL_KeyboardEvent* key, int modifier, int num )
      {
         return ( key->keysym.unicode == '+' || key->keysym.sym == SDLK_KP_PLUS);
      };
      
};


class ReplaySlower : public GuiFunction
{
   public:
      bool available( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         if ( runreplay.status == 2 )
            return true;

         return false;
      };

      void execute( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         CGameOptions::Instance()->replayspeed += 20;
         CGameOptions::Instance()->setChanged ( 1 );
         displaymessage2 ( "delay set to %d / 100 sec", CGameOptions::Instance()->replayspeed );
         updateFieldInfo();
      }

      Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         return IconRepository::getIcon("replay-slow.png");
      };

      ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         return "decrease replay speed (~-~)";
      };

      bool checkForKey( const SDL_KeyboardEvent* key, int modifier, int num )
      {
         return ( key->keysym.unicode == '-' || key->keysym.sym == SDLK_KP_MINUS);
      };
      
};


class ReplayRewind : public GuiFunction
{
   public:
      bool available( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         if ( runreplay.status == 1  ||  runreplay.status == 10 )
              return true;

         return false;
      };

      void execute( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         runreplay.status = 101;
         updateFieldInfo();
      }

      Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         return IconRepository::getIcon("replay-back.png");
      };

      ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         return "~r~estart replay";
      };

      bool checkForKey( const SDL_KeyboardEvent* key, int modifier, int num )
      {
         return ( key->keysym.unicode == 'r' );
      };
      
};


class ReplayExit : public GuiFunction
{
   public:
      bool available( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         if ( runreplay.status == 1 || runreplay.status == 10 || runreplay.status == 11 )
              return true;

         return false;
      };

      void execute( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         runreplay.status = 100;
         updateFieldInfo();
      }

      Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         return IconRepository::getIcon("replay-exit.png");
      };

      ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         return "e~x~it replay";
      };

      bool checkForKey( const SDL_KeyboardEvent* key, int modifier, int num )
      {
         return ( key->keysym.unicode == 'x' );
      };
      
};

class ReplayRecord : public GuiFunction
{
   public:
      bool available( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         if ( runreplay.status == 1 )
            return true;

         return false;
      };

      void execute( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
        ASCString filename;
        bool open = false;
        if ( replayRecorder ) {
           filename = replayRecorder->getLastFilename();
           open = replayRecorder->isOpen();
        }
        
        ReplayRecorderDialog rrd( filename, open );
        rrd.Show();
        rrd.RunModal();
        rrd.Hide();
         
        if ( !replayRecorder )
           replayRecorder = new ReplayRecorder();
        replayRecorder->start( rrd.getFilename(), rrd.getAppend(), rrd.getFramerate(), rrd.getQuality() );
        
        runreplay.status = 2;
        updateFieldInfo();
      }

      Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         return IconRepository::getIcon("replay-record.png");
      };

      ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         return "record to ~v~ideo";
      };

      bool checkForKey( const SDL_KeyboardEvent* key, int modifier, int num )
      {
         return ( key->keysym.unicode == 'v' );
      };
      
};

class ReplayRecordExit : public GuiFunction
{
   public:
      bool available( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         if ( replayRecorder && replayRecorder->isOpen() && (runreplay.status == 1 || runreplay.status == 10 || runreplay.status == 11) )
              return true;

         return false;
      };

      void execute( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         replayRecorder->close();
         runreplay.status = 100;
         updateFieldInfo();
      }

      Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         return IconRepository::getIcon("replay-record-stop.png");
      };

      ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num )
      {
         return "exit replay and ~c~lose recording";
      };

      bool checkForKey( const SDL_KeyboardEvent* key, int modifier, int num )
      {
         return ( key->keysym.unicode == 'c' );
      };
      
};


}

void registerReplayGuiFunctions( GuiIconHandler& handler )
{
   handler.registerUserFunction( new ReplayGuiFunctions::ReplayPlay() );
#ifdef XVIDEXPORT   
   handler.registerUserFunction( new ReplayGuiFunctions::ReplayRecord() );
#endif   
   handler.registerUserFunction( new ReplayGuiFunctions::ReplayPause() );
   handler.registerUserFunction( new ReplayGuiFunctions::ReplayFaster() );
   handler.registerUserFunction( new ReplayGuiFunctions::ReplaySlower() );
   handler.registerUserFunction( new ReplayGuiFunctions::ReplayRewind() );
#ifdef XVIDEXPORT   
   handler.registerUserFunction( new ReplayGuiFunctions::ReplayRecordExit() );
#endif
   handler.registerUserFunction( new ReplayGuiFunctions::ReplayExit() );
}



class ReplayGuiIconHandleHandler {
   static GuiIconHandler* replayIconHandler;
   bool active;
   public:
      ReplayGuiIconHandleHandler()
      {
         if ( !replayIconHandler ) {
            replayIconHandler = new GuiIconHandler();
            registerReplayGuiFunctions( *replayIconHandler );
         }
         
         if ( NewGuiHost::getIconHandler() != replayIconHandler ) {
            active = true;
            NewGuiHost::pushIconHandler( replayIconHandler );
         } else
            active = false;
      };

      ~ReplayGuiIconHandleHandler()
      {
         if ( active )
            NewGuiHost::popIconHandler();
      };
};
GuiIconHandler* ReplayGuiIconHandleHandler::replayIconHandler = NULL;





void runSpecificReplay( int player, int viewingplayer, bool performEndTurnOperations )
{
    if ( actmap->replayinfo->map[player] && actmap->replayinfo->guidata[player] ) {
       try {
         int t;
         do {
            t = runreplay.run ( player, viewingplayer, performEndTurnOperations );
         } while ( t ); /* enddo */
       }

       catch ( ActionResult res ) {
          // displayActionError( res );
          delete actmap;
          actmap = NULL;
          throw NoMapLoaded();
       }
       
#ifndef ASC_DEBUG
       catch ( ... ) {     // this will catch NullPointer-Exceptions and stuff like that, which we want to pass to the debugger  in debug mode
#else
       catch ( GameMap m ) {  // will never be thrown, but we need catch statement for the try block
#endif
          errorMessage("An unrecognized error occured during the replay");
          delete actmap;
          actmap = NULL;
          throw NoMapLoaded();
       }
    }
}

void viewOwnReplay( Player& player )
{
   if ( player.stat == Player::human || player.stat == Player::supervisor )
      if ( CGameOptions::Instance()->debugReplay && player.getParentMap()->replayinfo )
         if (choice_dlg("run replay of your turn ?","~y~es","~n~o") == 1) {
            // cursor.gotoxy( actmap->cursorpos.position[oldplayer].cx, actmap->cursorpos.position[oldplayer].cy );
            runSpecificReplay ( player.getPosition(), player.getPosition(), false );
         }
}


void checkforreplay ( void )
{
   if ( !actmap->replayinfo )
      return;

   int rpnum  = 0;
   int s = actmap->actplayer + 1;
   if ( s >= 8 )
       s = 0;
   while ( s != actmap->actplayer ) {
       if ( actmap->replayinfo->map[s] && actmap->replayinfo->guidata[s] )
          rpnum++;

       if ( s < 7 )
          s++;
       else
          s = 0;
   }


   if ( actmap->replayinfo  &&  rpnum  &&  (actmap->player[ actmap->actplayer ].stat == Player::human || actmap->player[ actmap->actplayer ].stat == Player::supervisor) )
      if (choice_dlg("run replay of last turn ?","~y~es","~n~o") == 1) {
      
         MainScreenWidget::StandardActionLocker locker( mainScreenWidget, MainScreenWidget::LockOptions::Menu );
         ReplayGuiIconHandleHandler guiIconHandler;
         
         int s = actmap->actplayer + 1;
         if ( s >= 8 )
            s = 0;
         while ( s != actmap->actplayer ) {
            if ( s >= 8 )
               s = 0;

             runSpecificReplay(s, actmap->actplayer );

             if ( s < 7 )
                 s++;
              else
                 s = 0;
          }


      }
}


void initReplayLogging( Player& player )
{
   GameMap* gamemap = player.getParentMap();
   
   if ( startreplaylate ) {
      gamemap->replayinfo = new GameMap::ReplayInfo;
      startreplaylate = 0;
   }

   if ( gamemap->replayinfo && player.stat != Player::off ) {
      if ( gamemap->replayinfo->actmemstream )
         fatalError( "actmemstream already open at begin of turn " );

      if ( gamemap->replayinfo->guidata[ player.getPosition() ] ) {
         delete gamemap->replayinfo->guidata[ player.getPosition() ];
         gamemap->replayinfo->guidata[ player.getPosition() ] = NULL;
      }

      savereplay ( gamemap, player.getPosition() );

      gamemap->replayinfo->guidata[ player.getPosition() ] = new tmemorystreambuf;
      gamemap->replayinfo->actmemstream = new tmemorystream ( gamemap->replayinfo->guidata[ player.getPosition() ], tnstream::writing );
   }
}


LockReplayRecording::LockReplayRecording( GameMap::ReplayInfo& _ri )
                    : ri ( _ri )
{
   ri.stopRecordingActions++;
}

LockReplayRecording::~LockReplayRecording()
{
   ri.stopRecordingActions--;
}


Resources getUnitResourceCargo ( Vehicle* veh )
{
   Resources res = veh->getTank();
   for ( ContainerBase::Cargo::const_iterator i = veh->getCargo().begin(); i != veh->getCargo().end(); ++i )
      if ( *i )
         res += getUnitResourceCargo ( *i );
   return res;
}

class LogActionIntoReplayInfo  {
      GameMap* gamemap;
   public:
      LogActionIntoReplayInfo( GameMap* map ) : gamemap( map ) {

      };

      void saveCommand( const Command& cmd )
      {
         if ( gamemap->replayinfo && gamemap->replayinfo->actmemstream && !gamemap->replayinfo->stopRecordingActions) {
            tnstream* stream = gamemap->replayinfo->actmemstream;
         
            stream->writeChar( rpl_runCommandAction );
            
            tmemorystreambuf buff;
            {
               tmemorystream stream2( &buff, tnstream::writing );
               cmd.write( stream2 );
            }
            
            // size is counted in 4 Byte chunks, so we need padding bytes
            int size = (buff.getSize()+3)/4;
            stream->writeInt( size + 1 );
            
            int padding = size*4 - buff.getSize();
            stream->writeInt( padding );
            
            buff.writetostream( stream );
            for ( int i = 0; i < padding;++i )
               stream->writeChar( 255-i );
         }
      }
};

static void logActionToReplay( GameMap* map, Command& command)
{
   LogActionIntoReplayInfo lairi( map );
   lairi.saveCommand( command );
}


Context trunreplay::createReplayContext()
{
   Context context;
   
   context.gamemap = actmap;
   context.actingPlayer = &actmap->getPlayer( actmap->actplayer );
   context.parentAction = NULL;
   
   if ( !replayMapDisplay ) 
      replayMapDisplay = new ReplayMapDisplay( &getDefaultMapDisplay() );
   
   context.display = replayMapDisplay;
   context.viewingPlayer = actmap->getPlayerView(); 
   context.actionContainer = &actmap->actions;
   return context;   
}




trunreplay :: trunreplay ()  
{
   replayMapDisplay = NULL;
   status = -1;
   movenum = 0;
}

trunreplay :: ~trunreplay()
{
   delete replayMapDisplay;  
}

void trunreplay::error( const ActionResult& res )
{
   error( ASCString(getmessage(res.getCode() )) + res.getMessage() );
}

void trunreplay::error( const MapCoordinate& pos, const ASCString& message )
{
   error( message );
}


void trunreplay::error( const MapCoordinate& pos, const char* message, ... )
{
   if ( CGameOptions::Instance()->replayMovieMode )
      return;
   
   va_list paramlist;
   va_start ( paramlist, message );
   char tempbuf[1000];
   int lng = vsprintf( tempbuf, message, paramlist );

   assert(lng < 1000);
   error( tempbuf );
   // error( message + "\nPosition: " + pos.toString() );
}

void trunreplay::error( const char* message, ... )
{
   if ( CGameOptions::Instance()->replayMovieMode )
      return;
   
   if ( message != lastErrorMessage ) {
      va_list paramlist;
      va_start ( paramlist, message );

      char tempbuf[1000];

      int lng = vsprintf( tempbuf, message, paramlist );
      if ( lng >= 1000 )
         displaymessage ( "trunreplay::error: String to long !\nPlease report this error", 1 );

      va_end ( paramlist );
      
      displaymessage(tempbuf, 1 );
      lastErrorMessage = message;
   }
}

void trunreplay::error( const ASCString& message )
{
   if ( CGameOptions::Instance()->replayMovieMode )
      return;

   if ( message != lastErrorMessage ) {
      displaymessage(message.c_str(), 1 );
      lastErrorMessage = message;
   }
}




void trunreplay :: wait ( int t )
{
//   if ( fieldvisiblenow ( getactfield(), actmap->playerView ))
    while ( ticker < t + CGameOptions::Instance()->replayspeed  && !keypress()) {
       /*
       tkey input;
       while (keypress ()) {
           input = r_key ( );
           if ( input == ct_'+' )
       }
       */
       releasetimeslice();
    }
}

void trunreplay :: wait ( MapCoordinate pos, int t )
{
   if ( fieldvisiblenow ( actmap->getField ( pos ), actmap->getPlayerView() ))
      wait();
}

void trunreplay :: wait ( MapCoordinate pos1, MapCoordinate pos2, int t )
{
   if ( fieldvisiblenow ( actmap->getField ( pos1 ), actmap->getPlayerView() ) || fieldvisiblenow ( actmap->getField ( pos2 ), actmap->getPlayerView() ))
      wait();
}


/*
void trunreplay :: setcursorpos ( int x, int y )
{
   int i = fieldvisiblenow ( getfield ( x, y ), actmap->playerview );
   cursor.gotoxy ( x, y, i );
   if( i ) {
      lastvisiblecursorpos.x = x;
      lastvisiblecursorpos.y = y;
   }
}
*/


void trunreplay :: displayActionCursor ( int x1, int y1, int x2, int y2, int secondWait )
{
    ReplayMapDisplay rmd( &getDefaultMapDisplay() );
    rmd.setCursorDelay ( CGameOptions::Instance()->replayspeed );
    rmd.displayActionCursor ( x1, y1, x2, y2, secondWait );
}

void trunreplay :: removeActionCursor ( void )
{
    ReplayMapDisplay rmd( &getDefaultMapDisplay() );
    rmd.removeActionCursor (  );
}


void trunreplay :: execnextreplaymove ( void )
{
   displayLogMessage( 8, "executing replay move %d\n", movenum );

   if ( !replayRecorder || !replayRecorder->isRunning())
      displaymessage2("executing replay move %d\n", movenum );
   
   movenum++;
   int actaction = nextaction;
   if ( nextaction != rpl_finished ) {
      switch ( nextaction ) {
      case rpl_move: {
                        stream->readInt(); // size
                        int x1 = stream->readInt();
                        int y1 = stream->readInt();
                        int x2 = stream->readInt();
                        int y2 = stream->readInt();
                        int nwid = stream->readInt();
                        readnextaction();

                        Vehicle* eht = actmap->getUnit ( x1, y1, nwid );
                        if ( eht ) {
                           ReplayMapDisplay rmd ( &getDefaultMapDisplay() );
                           
                           
                           auto_ptr<MoveUnitCommand> muc ( new MoveUnitCommand( eht ));
                           muc->setDestination( MapCoordinate(x2,y2) );

                           int t = ticker;
                           wait( MapCoordinate(x1,y1), MapCoordinate(x2,y2), t );
                           
                           ActionResult res = muc->execute( createReplayContext() );
                           if ( res.successful() )
                              muc.release();
                           else
                              error("severe replay inconsistency:\nerror for move1 command at " + MapCoordinate(x1,y1).toString() );
                        }

                        if ( !eht )
                           error("severe replay inconsistency:\nno vehicle for move1 command at " + MapCoordinate(x1,y1).toString() );
                     }
         break;
      case rpl_move5:
      case rpl_move4:
      case rpl_move3:
      case rpl_move2: {
                        stream->readInt(); // size
                        int x1 = stream->readInt();
                        int y1  = stream->readInt();
                        int x2 = stream->readInt();
                        int y2 = stream->readInt();
                        int nwid = stream->readInt();
                        int height = stream->readInt();
                        int noInterrupt;
                        if ( nextaction == rpl_move3 || nextaction == rpl_move4 || nextaction == rpl_move5 )
                           noInterrupt = stream->readInt();
                        else
                           noInterrupt = -1;

                        int destDamage;
                        if ( nextaction == rpl_move5 )
                           destDamage = stream->readInt();
                        else
                           destDamage = -1;

                        readnextaction();

                        Vehicle* eht = actmap->getUnit ( x1, y1, nwid );
                        if ( eht ) {
                           auto_ptr<MoveUnitCommand> muc ( new MoveUnitCommand( eht ));
                           
                           int t = ticker;
                           wait( MapCoordinate(x1,y1), MapCoordinate(x2,y2), t );
                           
                           MapCoordinate3D dest ( x2,y2, 0 );
                           dest.setNumericalHeight(height);
                           muc->setDestination( dest );
                           
                           ActionResult res = muc->execute( createReplayContext());
                           if ( res.successful() )
                              muc.release();
                           else {
                              if ( CGameOptions::Instance()->replayMovieMode ) {

                                 MapField* fld = eht->getMap()->getField(x1,y1);
                                 if ( fld->vehicle == eht ) {
                                    fld->vehicle = NULL;
                                 } else {
                                    if ( fld->getContainer() )
                                       fld->getContainer()->removeUnitFromCargo( eht, true );
                                 }

                                 if ( eht->isViewing() )
                                    eht->removeview();

                                 
                                 eht->setnewposition(x2,y2);
                                 if ( height >= 0 )
                                    eht->height = 1 << height;

                                 MapField* fld2 = eht->getMap()->getField(x2,y2);
                                 if ( !fld2->getContainer() ) {
                                    fld2->vehicle = eht;
                                    eht->addview();
                                 } else
                                    fld2->getContainer()->addToCargo( eht );
                              }
                              eht = NULL;

                           }

                           if ( destDamage >= 0 ) {
                              int realDamage;
                              Vehicle* veh = actmap->getUnit( nwid );
                              if ( veh )
                                 realDamage = veh->damage;
                              else
                                 realDamage = 100;

                              if ( destDamage != realDamage )
                                 error( MapCoordinate(x1,y1), "severe replay inconsistency:\ndamage after movement differs: recorded=%d, actual=%d", destDamage, realDamage );
                           }
                        }

                        if ( !eht )
                           error("severe replay inconsistency:\nno vehicle for move2 command at " + MapCoordinate(x1,y1).toString() );
                     }
         break;
      case rpl_attack: {
                           stream->readInt();  // size
                           int x1 = stream->readInt();
                           int y1 = stream->readInt();
                           int x2 = stream->readInt();
                           int y2 = stream->readInt();
                           int ad1 = stream->readInt();
                           int ad2 = stream->readInt();
                           int dd1 = stream->readInt();
                           int dd2 = stream->readInt();
                           int wpnum = stream->readInt();
                           readnextaction();

                           MapField* fld = getfield ( x1, y1 );
                           MapField* targ = getfield ( x2, y2 );
                           int attackvisible = fieldvisiblenow ( fld, actmap->getPlayerView() ) || fieldvisiblenow ( targ, actmap->getPlayerView() );
                           if ( fld && targ && fld->vehicle ) {
                              if ( fieldvisiblenow ( targ, fld->vehicle->getOwner() )) {
                                 if ( targ->vehicle ) {
                                    tunitattacksunit battle ( fld->vehicle, targ->vehicle, 1, wpnum );
                                    battle.av.damage = ad1;
                                    battle.dv.damage = dd1;
                                    if ( attackvisible ) {
                                       displayActionCursor ( x1, y1, x2, y2, 0 );
                                       ReplayMapDisplay rmd( &getDefaultMapDisplay() );
                                       rmd.showBattle( battle );
                                       removeActionCursor();
                                    } else {
                                       battle.calc ();
                                    }
                                    if ( battle.av.damage < ad2 || battle.dv.damage > dd2 )
                                       error(MapCoordinate(x2,y2), "severe replay inconsistency:\nresult of attack differ !\nexpected target damage: %d ; recorded target damage: %d\nexpected attacker damage: %d ; recorded attacker damage: %d", battle.av.damage,ad2 ,battle.dv.damage, dd2);
                                    battle.setresult ();

                                    if ( battle.av.damage >= 100 || battle.dv.damage >= 100 )
                                       computeview( actmap );

                                    updateFieldInfo();

                                 } else
                                 if ( targ->building ) {
                                    tunitattacksbuilding battle ( fld->vehicle, x2, y2 , wpnum );
                                    battle.av.damage = ad1;
                                    battle.dv.damage = dd1;
                                    if ( attackvisible ) {
                                       displayActionCursor ( x1, y1, x2, y2, 0 );
                                       ReplayMapDisplay rmd( &getDefaultMapDisplay() );
                                       rmd.showBattle( battle );
                                       removeActionCursor();
                                    } else {
                                       battle.calc ();
                                       /*battle.av.damage = ad2;
                                       battle.dv.damage = dd2; */
                                    }
                                    if ( battle.av.damage != ad2 || battle.dv.damage != dd2 )
                                       error(MapCoordinate(x2,y2), "severe replay inconsistency:\nresult of attack differ !\nexpected target damage: %d ; recorded target damage: %d\nexpected attacker damage: %d ; recorded attacker damage: %d", battle.av.damage,ad2 ,battle.dv.damage, dd2);
                                    battle.setresult ();

                                    if ( battle.av.damage >= 100 || battle.dv.damage >= 100 )
                                       computeview( actmap );
                                    updateFieldInfo();
                                 } else
                                 if ( !targ->objects.empty() ) {
                                    tunitattacksobject battle ( fld->vehicle, x2, y2, wpnum );
                                    if ( attackvisible ) {
                                       displayActionCursor ( x1, y1, x2, y2, 0 );
                                       ReplayMapDisplay rmd( &getDefaultMapDisplay() );
                                       rmd.showBattle( battle );
                                       removeActionCursor();
                                    } else {
                                       battle.calc ();
                                       //battle.av.damage = ad2;
                                       //battle.dv.damage = dd2;
                                    }
                                    if ( battle.av.damage != ad2 || battle.dv.damage != dd2 )
                                       error(MapCoordinate(x2,y2), "severe replay inconsistency:\nresult of attack differ !\nexpected target damage: %d ; recorded target damage: %d\nexpected attacker damage: %d ; recorded attacker damage: %d", battle.av.damage,ad2 ,battle.dv.damage, dd2);
                                    battle.setresult ();

                                    if ( battle.av.damage >= 100 || battle.dv.damage >= 100 )
                                       computeview( actmap );

                                    updateFieldInfo();
                                 }
                                 displaymap();
                             } else
                                error(MapCoordinate(x2,y2), "severe replay inconsistency:\nthe attacking unit can't view the target field!" );

                           } else
                              error(MapCoordinate(x1,y1), "severe replay inconsistency:\nno vehicle for attack command !" );

                      }
         break;
      case rpl_changeheight2:
      case rpl_changeheight: {
                        stream->readInt();  // size
                        int x1 = stream->readInt();
                        int y1 = stream->readInt();
                        int x2 = stream->readInt();
                        int y2 = stream->readInt();
                        int nwid = stream->readInt();
                        stream->readInt(); // oldheight
                        int newheight = stream->readInt();
                        int noInterrupt = -1;

                        if ( nextaction == rpl_changeheight2 )
                           noInterrupt = stream->readInt();


                        readnextaction();

                        Vehicle* eht = actmap->getUnit ( x1, y1, nwid );
                        if ( eht ) {
                           ReplayMapDisplay rmd( &getDefaultMapDisplay() );
                           
                           auto_ptr<MoveUnitCommand> muc ( new MoveUnitCommand( eht ));
                           muc->setDestination( MapCoordinate3D( x2,y2, newheight ));
                           
                           wait( MapCoordinate(x1,y1), MapCoordinate(x2,y2) );
                           
                           ActionResult res = muc->execute( createReplayContext() );
                           
                           if ( res.successful() )
                              muc.release();
                           else
                              eht = NULL;
                           
                        }

                        if ( !eht )
                           error(MapCoordinate(x1,y1), "severe replay inconsistency:\nno vehicle for changeheight command !");


                     }
         break;
      case rpl_convert: 
      case rpl_convert2: {
                           stream->readInt();  // size
                           int x = stream->readInt();
                           int y = stream->readInt();
                           int col = stream->readInt();
                           if ( nextaction == rpl_convert2 ) {
                              int nwid = stream->readInt();
                              readnextaction();

                              Vehicle* veh = actmap->getUnit( x,y, nwid );
                              if ( veh ) 
                                 veh->convert(col);
                              else
                                 error(MapCoordinate(x,y), "severe replay inconsistency:\nno vehicle for convert command !");
                           } else {
                              readnextaction();

                              MapField* fld = getfield ( x, y );
                              if ( fld ) {
                                 displayActionCursor ( x, y );
                                 if ( fld->vehicle )
                                    fld->vehicle->convert ( col );
                                 else
                                    if ( fld->building )
                                       fld->building->convert ( col );

                                 computeview( actmap );
                                 displaymap();
                                 wait( MapCoordinate(x,y) );
                                 removeActionCursor();
                              } else
                                 error(MapCoordinate(x,y), "severe replay inconsistency:\nno vehicle for convert command !");
                           }
                       }
         break;
      case rpl_remobj:
      case rpl_buildobj: 
      case rpl_remobj2:
      case rpl_buildobj2: {
                           stream->readInt();  // size
                           int x = stream->readInt();
                           int y = stream->readInt();
                           int id = stream->readInt();
                           int unit = -1;
                           if ( actaction == rpl_remobj2 || actaction == rpl_buildobj2 )
                              unit = stream->readInt();

                           readnextaction();

                           ObjectType* obj = objectTypeRepository.getObject_byID ( id );

                           MapField* fld = getfield ( x, y );
                           if ( obj && fld ) {
                              displayActionCursor ( x, y );

                              Resources cost;
                              int movecost;

                              RecalculateAreaView rav ( actmap, MapCoordinate(x,y), maxViewRange / maxmalq + 1, NULL );
                              
                              bool objectAffectsVisibility = obj->basicjamming_plus || obj->viewbonus_plus || obj->viewbonus_abs != -1 || obj->basicjamming_abs != -1;
                              if ( objectAffectsVisibility )
                                 rav.removeView();


                              if ( actaction == rpl_remobj || actaction == rpl_remobj2 ) {
                                 cost = obj->removecost;
                                 fld->removeObject ( obj );
                                 movecost = obj->remove_movecost;
                              } else {
                                 cost = obj->buildcost;
                                 fld->addobject ( obj );
                                 movecost = obj->build_movecost;
                              }

                              if ( objectAffectsVisibility )
                                 rav.addView();


                              if ( unit > 0 ) {
                                 Vehicle* veh = actmap->getUnit(unit);
                                 if ( veh ) {
                                    if ( veh->getMovement() < movecost )
                                       error(MapCoordinate(x,y), "not enough movement to construct/remove object !");
                                    veh->decreaseMovement( movecost );
                                    Resources res2 =  static_cast<ContainerBase*>(veh)->getResource( cost, 0, 1  );
                                    for ( int r = 0; r < 3; r++ )
                                       if ( res2.resource(r) < cost.resource(r)  && cost.resource(r) > 0 )
                                          error("Resource mismatch: not enough resources to construct/remove object !\nPosition: " + MapCoordinate(x,y).toString());

                                 } else
                                    error(MapCoordinate(x,y), "replay inconsistency:\nCannot find Unit to build/remove Object !");
                              }

                              if ( fieldvisiblenow ( fld, actmap->getPlayerView() ))
                                 displaymap();
                              
                              wait(MapCoordinate(x,y));
                              removeActionCursor();
                           } else
                              error(MapCoordinate(x,y), "severe replay inconsistency:\nCannot find Object to build/remove !");

                       }
         break;
      case rpl_buildtnk:
      case rpl_buildtnk2:
      case rpl_buildtnk3:
      case rpl_buildtnk4: {
                           stream->readInt();  // size
                           int x = stream->readInt();
                           int y = stream->readInt();
                           int id = stream->readInt();
                           int col = stream->readInt();
                           int nwid = -1;
                           int constx = -1;
                           int consty = -1;
                           int height = -1;
                           if ( nextaction == rpl_buildtnk2 )
                              nwid = stream->readInt();

                           if ( nextaction == rpl_buildtnk3 || nextaction == rpl_buildtnk4 ) {
                              constx = stream->readInt();
                              consty = stream->readInt();
                              if ( nextaction == rpl_buildtnk4 )
                                 height = stream->readInt();
                           }

                           readnextaction();

                           MapField* fld = getfield ( x, y );

                           VehicleType* tnk = vehicleTypeRepository.getObject_byID ( id );

                           if ( fld && tnk && !fld->vehicle ) {
                              displayActionCursor ( x, y );
                              Vehicle* v = new Vehicle ( tnk, actmap, col );
                              v->xpos = x;
                              v->ypos = y;
                              fld->vehicle = v;
                              if ( height >= 0 )
                                 v->height = height;

                              if ( constx >= 0 && consty >= 0 ) {
                                 MapField* constructorField = getfield(constx, consty );
                                 if ( constructorField->vehicle ) {
                                    Resources r ( 0, tnk->productionCost.material, tnk->productionCost.energy ); //  = constructorField->vehicle->getProductionCost(tnk );
                                    Resources rr = constructorField->getContainer()->getResource( r, 0 );
                                    if ( rr < r ) {
                                       displayActionCursor ( x, y );
                                       error(MapCoordinate(x,y), "severe replay inconsistency: \nNot enough resources to produce unit %s !\nRequired: %d/%d/%d ; Available: %d/%d/%d", v->typ->description.c_str(), r.energy, r.material, r.fuel, rr.energy, rr.material, rr.fuel);
                                    }
                                 } else
                                    error(MapCoordinate(x,y), "severe replay inconsistency: could not find constructor !");

                              }

                              computeview( actmap );
                              displaymap();
                              wait(MapCoordinate(x,y) );
                              removeActionCursor();
                           } else
                              error(MapCoordinate(x,y), "severe replay inconsistency:\nCannot find Vehicle to build !");

                       }
         break;
      case rpl_putbuilding2:
      case rpl_putbuilding : {
                               int size = stream->readInt();  // size
                               int x = stream->readInt();
                               int y = stream->readInt();
                               int id = stream->readInt();
                               stream->readInt(); // color ; we are using the vehicle's color
                               int networkid = 0;
                               if ( size == 5 )
                                   networkid = stream->readInt();

                               readnextaction();

                               MapField* fld = getfield ( x, y );

                               BuildingType* bld = buildingTypeRepository.getObject_byID ( id );

                               if ( bld && fld && networkid && actmap->getUnit( networkid ) ) {
                                  displayActionCursor ( x, y );
                                  auto_ptr<ConstructBuildingCommand> cbc ( new ConstructBuildingCommand( actmap->getUnit( networkid ) ));
                                  cbc->setBuildingType( bld );
                                  cbc->setTargetPosition( MapCoordinate(x,y));
                                  ActionResult res = cbc->execute( createReplayContext());
                                  if ( res.successful() )
                                     cbc.release();
                                  else
                                     error( MapCoordinate(x,y), "severe replay inconsistency, could not construct building; Code=" + ASCString::toString( res.getCode() ));
                                  
                                  wait(MapCoordinate(x,y));
                                  removeActionCursor();
                               } else
                                  error(MapCoordinate(x,y), "severe replay inconsistency:\nCannot find building to build/remove building!" );
                            }
         break;
      case rpl_putmine:
      case rpl_putmine2: {
                           stream->readInt();  // size
                           int x = stream->readInt();
                           int y = stream->readInt();
                           int col = stream->readInt();
                           int typ = stream->readInt();
                           int strength = stream->readInt();
                           int nwid = -1;
                           if ( nextaction == rpl_putmine2) 
                              nwid = stream->readInt();

                           readnextaction();

                           MapField* fld = getfield ( x, y );
                           if ( fld ) {
                              displayActionCursor ( x, y );
                              fld -> putmine ( col, MineTypes(typ), strength );
                              if ( nwid >= 0 ) {
                                 Vehicle* veh = actmap->getUnit( nwid );
                                 if ( veh ) {
                                    ConsumeAmmo ca ( veh, cwminen, -1, 1 );
                                    ActionResult res = ca.execute( createReplayContext());
                                    if ( !res.successful() )
                                       error(MapCoordinate(x,y), "could not obtain ammo for mine placement");

                                 } else 
                                    error(MapCoordinate(x,y), "could not find unit for mine placement");
                              }
                              computeview( actmap );
                              if ( fieldvisiblenow ( actmap->getField(x,y), actmap->getPlayerView() )) {
                                 displaymap();
                                 wait();
                              }
                              removeActionCursor();
                           } else
                              error(MapCoordinate(x,y), "severe replay inconsistency:\nno field for putmine command !");

                       }
         break;
      case rpl_removemine: {
                           stream->readInt();  // size
                           int x = stream->readInt();
                           int y = stream->readInt();
                           readnextaction();

                           MapField* fld = getfield ( x, y );
                           if ( fld ) {
                              displayActionCursor ( x, y );
                              fld -> removemine ( -1 );
                              computeview( actmap );
                              if ( fieldvisiblenow ( actmap->getField(x,y), actmap->getPlayerView() )) {
                                 displaymap();
                                 wait();
                              }
                              removeActionCursor ( );
                           } else
                              error(MapCoordinate(x,y), "severe replay inconsistency:\nno field for remove mine command !");

                       }
         break;
      case rpl_removebuilding2:
      case rpl_removebuilding3:
      case rpl_removebuilding: {
                           stream->readInt();  // size
                           int x = stream->readInt();
                           int y = stream->readInt();
                           int nwid = -1;
                           if ( nextaction == rpl_removebuilding2 || nextaction == rpl_removebuilding3 )
                              nwid = stream->readInt();

                           Resources res;
                           if ( nextaction == rpl_removebuilding3 ) {
                              res.energy = stream->readInt();
                              res.material = stream->readInt();
                              res.fuel = stream->readInt();
                           }

                           readnextaction();

                           MapField* fld = getfield ( x, y );
                           if ( fld && fld->building ) {
                              displayActionCursor ( x, y );
                              if ( nwid >= 0 ) {
                                 Vehicle* veh = actmap->getUnit( nwid );
                                 if ( veh ) {
                                    DestructBuildingCommand* dbc = new DestructBuildingCommand( veh );
                                    dbc->setTargetPosition( MapCoordinate( x,y ));
                                    dbc->execute( createReplayContext());
                                    
                                 } else
                                   error(MapCoordinate(x,y), "severe replay inconsistency:\nfailed to obtain vehicle for removebuilding command !");
                              } else
                                 error(MapCoordinate(x,y), "no vehicle for removebuilding command !");

                              computeview( actmap );
                              displaymap();
                              wait();
                              removeActionCursor();
                           } else
                              error(MapCoordinate(x,y), "severe replay inconsistency:\nno building for removebuilding command !");

                       }
         break;
      case  rpl_produceunit : {
                                 stream->readInt();  // size
                                 int id = stream->readInt();
                                 int col = stream->readInt();
                                 int x = stream->readInt();
                                 int y = stream->readInt();
                                 stream->readInt(); // class
                                 int nwid = stream->readInt();
                                 readnextaction();

                                 MapField* fld = getfield ( x, y );

                                 VehicleType* tnk = vehicleTypeRepository.getObject_byID ( id );
                                 if ( tnk && fld) {
                                 
                                    #if 0
                                    printf("produced unit: pos %d / %d; nwid %d; typ id %d; typ %s \n", x,y,nwid,id,tnk->description.c_str() );
                                    #endif

                                    Vehicle* eht = new Vehicle ( tnk, actmap, col / 8 );
                                    eht->xpos = x;
                                    eht->ypos = y;
                                    eht->networkid = nwid;

                                    if ( fld->building ) {
                                       Resources cost  = fld->building->getProductionCost(tnk);
                                       Resources r = fld->building->getResource( cost, 0 );
                                       if ( r < cost ) {
                                          displayActionCursor ( x, y );
                                          error(MapCoordinate(x,y), "severe replay inconsistency: \nNot enough resources to produce unit %s !\nRequired: %d/%d/%d ; Available: %d/%d/%d", eht->typ->description.c_str(), cost.energy, cost.material, cost.fuel, r.energy, r.material, r.fuel);
                                       }
                                       fld->building->addToCargo( eht );
                                    } else {
                                       displayActionCursor ( x, y );
                                       fld->vehicle = eht;
                                       computeview( actmap );
                                       displaymap();
                                       wait( MapCoordinate(x,y) );
                                       removeActionCursor();
                                    }
                                 } else
                                    error(MapCoordinate(x,y), "severe replay inconsistency:\nCannot find vehicle to build/remove !");

                              }
         break;
      case rpl_removeunit : {
                                 stream->readInt();  // size
                                 int x = stream->readInt();
                                 int y = stream->readInt();
                                 int nwid = stream->readInt();
                                 readnextaction();
                                 MapField* fld = getfield(x,y);
                                 if ( (!fld->vehicle || fld->vehicle->networkid != nwid) && fld->building ) {
                                    auto_ptr<RecycleUnitCommand> ruc ( new RecycleUnitCommand( fld->building ));
                                    ruc->setUnit( actmap->getUnit( nwid ) );
                                    ActionResult res = ruc->execute( createReplayContext());
                                    if ( res.successful() )
                                       ruc.release();
                                    else
                                       displayActionError(res);
                                 } else
                                    if ( !fld->getContainer() || !fld->getContainer()->removeUnitFromCargo ( nwid, true ))
                                       displaymessage ( "severe replay inconsistency:\nCould not remove unit %d!", 1, nwid );
                              }
         break;
      case rpl_trainunit:{
                                 stream->readInt();  // size
                                 int x = stream->readInt();
                                 int y = stream->readInt();
                                 stream->readInt(); // experience
                                 int nwid = stream->readInt();
                                 readnextaction();

                                 Vehicle* eht = actmap->getUnit ( x, y, nwid );
                                 Building* bld = actmap->getField ( x, y )->building;
                                 if ( eht && bld ) {
                                    auto_ptr<TrainUnitCommand> tuc ( new TrainUnitCommand( bld ));
                                    tuc->setUnit( eht );
                                    ActionResult res = tuc->execute( createReplayContext());
                                    if ( res.successful())
                                       tuc.release();
                                    else
                                       displayActionError(res);
                                 } else
                                    error(MapCoordinate(x,y), "severe replay inconsistency:\nno vehicle for trainunit command !");


                              }
         break;
         /*
      case rpl_shareviewchange: {
                                 int size = stream->readInt();
                                 GameMap::Shareview* sv = new GameMap::Shareview;
                                 if ( size ) {
                                    for ( int a = 0; a < 8; a++ )
                                       for ( int b = 0; b < 8; b++)
                                           sv->mode[a][b] = stream->readChar();
                                    sv->recalculateview = stream->readInt();

                                    if ( actmap->shareview )
                                       delete actmap->shareview;
                                    actmap->shareview = sv;
                                    actmap->shareview->recalculateview = 0;
                                 } else
                                    if ( actmap->shareview ) {
                                       delete actmap->shareview;
                                       actmap->shareview = NULL;
                                    }

                                 readnextaction();
                                 computeview( actmap );
                                 displaymap();
                              }
         break;
         */
      case rpl_alliancechange2: {
                                 stream->readInt();  // size
                                 int actingPlayer = stream->readInt();
                                 int targetPlayer = stream->readInt();
                                 int state = stream->readInt();
                                 actmap->player[actingPlayer].diplomacy.setState( targetPlayer, DiplomaticStates( state ));
                                 
                                 readnextaction();
                              }
         break;
      case rpl_refuel :
      case rpl_refuel2 : {
                                 stream->readInt();  // size
                                 int x = stream->readInt();
                                 int y = stream->readInt();
                                 int nwid = stream->readInt();
                                 int pos = stream->readInt();
                                 int amnt = stream->readInt();
                                 int old = -2;
                                 if ( nextaction == rpl_refuel2 )
                                    old = stream->readInt();

                                 readnextaction();

                                 Vehicle* eht = actmap->getUnit ( x, y, nwid );
                                 if ( eht ) {
                                    if ( pos < 16 ) {
                                       if ( old >= 0 && old != eht->ammo[pos] )
                                          error(MapCoordinate(x,y), "severe replay inconsistency:\nthe ammo of unit not matching. \nrecorded: %d , expected: %d !", old, eht->ammo[pos] );
                                       eht->ammo[pos] = amnt;
                                    } else {
                                        int res = pos - 1000;
                                        int avl = eht->getTank().resource(res);
                                        if ( avl != old && old >= 0 )
                                            error(MapCoordinate(x,y), "severe replay inconsistency:\nthe resources of unit not matching. \nrecorded: %d , expected: %d !", old, avl);
                                        eht->getResource ( avl - amnt, res, false );
                                     }
                                 } else
                                    error(MapCoordinate(x,y), "severe replay inconsistency:\nno vehicle for refuel-unit command !");
                              }
         break;
      case rpl_refuel3 : {
                                 stream->readInt();  // size
                                 int id = stream->readInt();
                                 int type = stream->readInt();
                                 int delta = stream->readInt();
                                 readnextaction();

                                 ContainerBase* cb = actmap->getContainer ( id );
                                 if ( cb ) {
                                    if ( type >= 1000 ) {
                                       Resources res;
                                       res.resource(type-1000) = delta;
                                       ConsumeResource cr ( cb, res );
                                       ActionResult result = cr.execute( createReplayContext() );
                                       if ( !result.successful())
                                          error("severe replay inconsistency:\nthe resources of container not matching. ");
                                          
                                    } else {
                                       ConsumeAmmo ca ( cb, type, -1, delta );
                                       ActionResult result = ca.execute( createReplayContext() );
                                       if ( !result.successful())
                                          error("severe replay inconsistency:\nthe resources of container not matching. ");
                                    }
                                 } else
                                    error("severe replay inconsistency:\nno vehicle for refuel3 command !");
                              }
         break;
      case rpl_bldrefuel : {
                                 stream->readInt();  // size
                                 int x = stream->readInt();
                                 int y = stream->readInt();
                                 int pos = stream->readInt();
                                 int amnt = stream->readInt();
                                 readnextaction();

                                 Building* bld = actmap->getField(x,y)->building;
                                 if ( bld ) {
                                    if ( pos < 16 )
                                        bld->ammo[pos] = amnt;
                                     else
                                        bld->getResource ( amnt, pos-1000, 0 );
                                 } else
                                    error(MapCoordinate(x,y), "severe replay inconsistency:\nno building for refuel-unit command !");
                              }
         break;
         case rpl_moveUnitUpDown: {
            stream->readInt();  // size
            int x =  stream->readInt();
            int y =  stream->readInt();
            stream->readInt(); // dummy(basegfx_0)
            int nwid_to = stream->readInt();
            int nwid_moving = stream->readInt();

            readnextaction();

            ContainerBase* b = actmap->getField(x,y)->getContainer();
            if ( b ) {
               b->removeUnitFromCargo( nwid_moving, true );

               Vehicle* veh_targ = actmap->getUnit( nwid_to );
               Vehicle* veh_moving = actmap->getUnit( nwid_moving );
               if ( veh_targ && veh_moving )
                  veh_targ->addToCargo( veh_moving );
               else
                  error ( MapCoordinate(x,y), "Could not locate unit for MoveToInnerTransport");
            } else
               error( MapCoordinate(x,y), "severe replay inconsistency in MoveUnitUp !");
                                 
         }
         break;
         case rpl_moveUnitUp: {
                                 stream->readInt();  // size
                                 int x =  stream->readInt();
                                 int y =  stream->readInt();
                                 int nwid = stream->readInt();

                                 readnextaction();

                                 Vehicle* eht = actmap->getUnit ( x, y, nwid );
                                 if ( eht ) {
                                    CargoMoveCommand* cmc = new CargoMoveCommand( eht );
                                    cmc->setMode( CargoMoveCommand::moveOutwards );
                                    ActionResult res = cmc->execute( createReplayContext() );
                                    if ( !res.successful())
                                       error(MapCoordinate(x,y), "severe replay inconsistency in MoveUnitUp !");
                                 } else
                                    error(MapCoordinate(x,y), "Unit not found for MoveUnitUp !");
                                 
                              }
                              break;
      case rpl_repairUnit : {
                                 stream->readInt();  // size
                                 int nwid = stream->readInt();
                                 int destnwid = stream->readInt();
                                 int amount = stream->readInt();
                                 int matremain  = stream->readInt();
                                 int fuelremain = stream->readInt();

                                 readnextaction();

                                 Vehicle* eht = actmap->getUnit ( nwid );
                                 Vehicle* dest = actmap->getUnit ( destnwid );
                                 if ( eht && dest ) {
                                    eht->repairItem ( dest, amount );
                                    if ( eht->getTank().fuel != fuelremain || eht->getTank().material != matremain )
                                         error("severe replay inconsistency:\nthe resources of unit not matching for repair operation!");
                                 } else
                                    error("severe replay inconsistency:\nno vehicle for repair-unit command !");
                              }
         break;
      case rpl_repairUnit2 : {
                                 stream->readInt();  // size
                                 int x = stream->readInt();
                                 int y = stream->readInt();
                                 int destnwid = stream->readInt();
                                 int amount = stream->readInt();

                                 readnextaction();

                                 ContainerBase* bld = getfield(x,y)->getContainer();
                                 Vehicle* dest = actmap->getUnit ( destnwid );
                                 if ( bld && dest ) {
                                    bld->repairItem ( dest, amount );
                                 } else
                                    error(MapCoordinate(x,y), "severe replay inconsistency:\nno vehicle for repair-unit command !");
                              }
         break;
      case rpl_repairUnit3 : {
                                 stream->readInt();  // size
                                 int serviceNWID = stream->readInt();
                                 int destnwid = stream->readInt();
                                 int amount = stream->readInt();

                                 readnextaction();

                                 ContainerBase* bld = actmap->getContainer( serviceNWID );
                                 Vehicle* dest = actmap->getUnit ( destnwid );
                                 if ( bld && dest ) {
                                    bld->repairItem ( dest, amount );
                                 } else
                                    error("severe replay inconsistency:\nno vehicle for repair-unit command !");
                              }
         break;
      case rpl_repairBuilding : {
                                 stream->readInt();  // size
                                 int x = stream->readInt();
                                 int y = stream->readInt();
                                 stream->readInt(); // int destnwid = 
                                 int amount = stream->readInt();

                                 readnextaction();

                                 ContainerBase* bld = getfield(x,y)->getContainer();
                                 if ( bld ) {
                                    bld->repairItem ( bld, amount );
                                 } else
                                    error(MapCoordinate(x,y), "severe replay inconsistency:\nno building for repair-building command !");
                              }
         break;
      case rpl_produceAmmo : {
                                 stream->readInt();  // size
                                 int x = stream->readInt();
                                 int y = stream->readInt();
                                 int weaptype = stream->readInt();
                                 int n = stream->readInt();
                                 readnextaction();
                                 Building* bld = getfield(x,y)->building;
                                 if ( bld ) {
                                    
                                    bld->getResource ( Resources( ammoProductionCost[weaptype][0] * n, ammoProductionCost[weaptype][1] * n, ammoProductionCost[weaptype][2] * n ), false );
                                    bld->putAmmo ( weaptype, n, false );
                                    
                                 } else
                                    error(MapCoordinate(x,y), "severe replay inconsistency:\nno building for produce ammo command !");

                              }
         break;
      case rpl_buildProdLine : {
                                 stream->readInt();  // size
                                 int building = stream->readInt();
                                 int vehicleid = stream->readInt();
                                 readnextaction();
                                 Building* bld = dynamic_cast<Building*>( actmap->getContainer(building));
                                 VehicleType* veh = actmap->getvehicletype_byid ( vehicleid );
                                 if ( bld && veh ) {
                                    if ( veh->techDependency.available( actmap->player[ bld->getOwner()].research )) {
                                       auto_ptr<BuildProductionLineCommand> bplc ( new BuildProductionLineCommand( bld ));
                                       bplc->setProduction( veh );
                                       ActionResult res = bplc->execute( createReplayContext() );
                                       if ( res.successful() )
                                          bplc.release();
                                       else
                                          error("severe replay inconsistency:\ncould not build production line!");
                                    } else
                                       error("severe replay inconsistency:\ntechnology for building production line not available!");

                                 } else
                                    error("severe replay inconsistency:\nno building for build production line command !");

                              }
         break;
      case rpl_removeProdLine : {
                                 stream->readInt();  // size
                                 int building = stream->readInt();
                                 int vehicleid = stream->readInt();
                                 readnextaction();
                                 Building* bld = dynamic_cast<Building*>( actmap->getContainer(building));
                                 VehicleType* veh = actmap->getvehicletype_byid ( vehicleid );
                                 if ( bld && veh ) {
                                    auto_ptr<RemoveProductionLineCommand> rplc ( new RemoveProductionLineCommand( bld ));
                                    rplc->setRemoval( veh );
                                    ActionResult res = rplc->execute( createReplayContext());
                                    if ( res.successful()) 
                                       rplc.release();
                                    else
                                       error("severe replay inconsistency:\ncould not remove production line!");

                                 } else
                                    error("severe replay inconsistency:\nno building for remove production line command !");

                              }
         break;
      case rpl_setResearch : {
                                 stream->readInt();  // size
                                 int building = stream->readInt();
                                 int amount = stream->readInt();
                                 readnextaction();
                                 Building* bld = dynamic_cast<Building*>( actmap->getContainer(building));
                                 if ( bld )
                                    bld->researchpoints = amount;
                                 else
                                    error("severe replay inconsistency:\nno building for set research command !");

                              }
         break;
      case rpl_techResearched: {
                                 stream->readInt();  // size
                                 int techID = stream->readInt();
                                 int player = stream->readInt();
                                 readnextaction();
                                 Technology* tech = technologyRepository.getObject_byID( techID );
                                 if ( tech ) {
                                    actmap->player[player].research.addanytechnology( tech );
                                    actmap->player[player].research.progress -= tech->researchpoints;
                                 } else
                                    error("severe replay inconsistency:\nno technology for tech researched command !");
                                }
         break;
      case rpl_setGeneratorStatus : {
                                 stream->readInt();
                                 int nwid = stream->readInt();
                                 int status = stream->readInt();

                                 readnextaction();

                                 Vehicle* eht = actmap->getUnit ( nwid );
                                 if ( eht )
                                    eht->setGeneratorStatus( status );
                                 else
                                    error("severe replay inconsistency:\nvehicle for generator switching not found !");
                              }


         break;
         case rpl_cutFromGame: {
                              stream->readInt();
                              int vehid = stream->readInt();
                              Vehicle* veh = actmap->getUnit ( vehid );
                              Resources res;
                              res.energy = stream->readInt();
                              res.material = stream->readInt();
                              res.fuel = stream->readInt();

                              readnextaction();

                              if ( veh && res == getUnitResourceCargo ( veh )) {
                                 veh->prepareForCleanRemove();
                                 delete veh;
                                 computeview( actmap );
                                 displaymap();
                              } else
                                 error ( "resource mismatch at cut unit operation! ");
                           }
         break;
         case rpl_setResourceProcessingAmount: {
                 stream->readInt();
                 int x = stream->readInt();
                 int y = stream->readInt();
                 Resources p;
                 p.energy = stream->readInt();
                 p.material = stream->readInt();
                 p.fuel = stream->readInt();
                 readnextaction();
                 Building* bld = actmap->getField(x,y)->building;
                 if ( bld ) {
                    for ( int r = 0; r< 3; ++r )
                       if ( abs(p.resource(r)) > abs(bld->typ->maxplus.resource(r)) )
                          error (MapCoordinate(x,y), "Building can not produ ");
                     bld->plus = p;
                 } else
                    error (MapCoordinate(x,y), "Building not found on for rpl_setResourceProcessingAmount ");
         }
         break;
         case rpl_netcontrol: {
                 stream->readInt();
                 int x = stream->readInt();
                 int y = stream->readInt();
                 stream->readInt(); // int cat = 
                 stream->readInt(); // int stat = 
                 readnextaction();

                 Building* bld = actmap->getField(x,y)->building;
                 if ( bld ) {
                    // not supported
                    /*
                    cbuildingcontrols bc;
                    bc.init( bld );
                    bc.netcontrol.setnetmode( cat, stat );
                    bld->execnetcontrol();
                    */
                 } else
                    error (MapCoordinate(x,y), "Building not found on for rpl_setResourceProcessingAmount ");
         }
         break;
         case rpl_jump: {
            stream->readInt();
            int nwid = stream->readInt();
            int x = stream->readInt();
            int y = stream->readInt();
            readnextaction();

            Vehicle* veh = actmap->getUnit(nwid);
            if ( veh ) {
               if ( JumpDriveCommand::avail( veh )) {
                  auto_ptr<JumpDriveCommand> jd( new JumpDriveCommand(veh) );
                  displayActionCursor ( veh->getPosition().x , veh->getPosition().x, x, y, 0 );
                  jd->setDestination( MapCoordinate(x,y));
                  ActionResult res = jd->execute( createReplayContext() );
                  if ( !res.successful() )
                     error(MapCoordinate(x,y), "Unit cannot jump to this position");
                  else
                     jd.release();
                  
               } else
                  error(MapCoordinate(x,y), "Unit cannot jump");
            } else
               error (MapCoordinate(x,y), "Unit not found for Jump ");
            
            
         }
         break;
      case rpl_recycleUnit : {
                                 stream->readInt();  // size
                                 int building = stream->readInt();
                                 int vehicleid = stream->readInt();
                                 readnextaction();
                                 Building* bld = dynamic_cast<Building*>( actmap->getContainer(building));
                                 Vehicle* veh = actmap->getUnit ( vehicleid );
                                 if ( bld && veh ) {
                                    auto_ptr<RecycleUnitCommand> ruc ( new RecycleUnitCommand( bld ));
                                    ruc->setUnit( veh  );
                                    ActionResult res = ruc->execute( createReplayContext() );
                                    if ( res.successful() )
                                       ruc.release();
                                    else
                                       displayActionError(res);
                                 } else
                                    error("severe replay inconsistency:\nno unit for recycle command !");
                              }
         break;
      case rpl_transferTribute: {
         stream->readInt();
         int player = stream->readInt();
         readnextaction();
         transfer_all_outstanding_tribute( actmap->getPlayer( player ) );
                                }
         break;

      case rpl_reactionFireOn: 
      case rpl_reactionFireOff: {
            stream->readInt();
            int nwid = stream->readInt();
            readnextaction();
            Vehicle* v = actmap->getUnit( nwid );
            if ( v ) {
               if ( actaction == rpl_reactionFireOn ) {
                  int res = v->reactionfire.enable();
                  if ( res < 0 )
                     error("severe enabling reactionfire for unit !");
               } else
                  v->reactionfire.disable();
            } else
               error("severe replay inconsistency:\nno unit for reactionfire command !");
         }
         break;
      case rpl_selfdestruct: {
             stream->readInt();
             int nwid = stream->readInt();
             readnextaction();
             ContainerBase* c = actmap->getContainer( nwid );
             if ( DestructUnitCommand::avail( c )) {
                auto_ptr<DestructUnitCommand> duc ( new DestructUnitCommand( c ));
                ActionResult res = duc->execute( createReplayContext() );
                if ( !res.successful() )
                   error("severe replay inconsistency:\nno container for selfdestruct command !");
                else
                   duc.release();
             } else
                error("severe replay inconsistency:\nno container for selfdestruct command !");
            }
            break;
      case rpl_cancelResearch : 
            {
               stream->readInt();
               readnextaction();
               
               auto_ptr<CancelResearchCommand> crc ( new CancelResearchCommand( actmap ));
               crc->setPlayer( actmap->player[actmap->actplayer] );
               ActionResult res = crc->execute( createReplayContext() );
               if ( res.successful() )
                  crc.release();
               else
                  error( res );
            }
            break;

      case rpl_runCommandAction:
         {
            stream->readInt();
            int padding = stream->readInt();
            tmemorystreambuf buffer;
            buffer.readfromstream( stream );
            
            tmemorystream memstream( &buffer, tnstream::reading );
            
            auto_ptr<GameAction> readaction ( GameAction::readFromStream( memstream, actmap ));
            
            Command* a = dynamic_cast<Command*> ( readaction.get() );
            
            for ( int i = 0; i < padding;++i ) {
               char c = stream->readChar();
               if ( c != 255-i )
                  error("invalid padding bytes in command action storage buffer");
            }
            readnextaction();
            
            if ( a ) {
               try {
               ActionResult res = a->redo( createReplayContext() );
               if ( !res.successful() )
                  error("action " + a->getDescription() + " failed\n" + getmessage(res.getCode()));
               } catch ( ActionResult res ) {
                  error("action " + a->getDescription() + " failed\n" + getmessage(res.getCode()));
                  throw res;
               }

            } else
               error("could not read Command action from replay stream" );
         }
         break;
               
      default:{
                 int size = stream->readInt();
                 for ( int i = 0; i< size; i++ )
                    stream->readInt();

                 readnextaction();

              }
        break;
      } /* endswitch */

   } else {
      status = 10;
      updateFieldInfo();
   }

}

void trunreplay :: readnextaction ( void )
{
      if ( stream->dataavail () )
         nextaction = stream->readChar();
      else
         nextaction = rpl_finished;
}



int  trunreplay :: run ( int player, int viewingplayer, bool performEndTurnOperations )
{
   ReplayRecorderWatcherLocal rrw( replayRecorder );
  
   if ( status < 0 )
      firstinit ( );

   lastErrorMessage = "";

   movenum = 0;

   actplayer = actmap->actplayer;

   orgmap = actmap;
   actmap = loadreplay ( orgmap->replayinfo->map[player]  );
   if ( !actmap ) {
      displaymessage("error loading replay", 1 );
      actmap = orgmap;
      return 0;
   }
   actmap->state = GameMap::Replay;

   actmap->setPlayerView ( viewingplayer );
   actmap->getCursor() = orgmap->getCursor();

   SuppressTechPresentation stp;
   actmap->sigPlayerTurnBegins( actmap->getPlayer( player ));
   
   tmemorystream guidatastream ( orgmap->replayinfo->guidata [ player ], tnstream::reading );
   stream = &guidatastream;

   if ( stream->dataavail () )
      nextaction = stream->readChar();
   else
      nextaction = rpl_finished;

//   orgmap.replayinfo->actmemstream = stream;


   ReplayGuiIconHandleHandler guiIconHandler;

   if ( stream->dataavail () ) {
      if ( CGameOptions::Instance()->replayMovieMode ) 
         status = 2;
      else
         status = 1;
   } else
      status = 11;

   // force completion of overview map rendering
   actmap->overviewMapHolder.getOverviewMap( );

   computeview( actmap );
   displaymap ();

   updateFieldInfo();

   MainScreenWidget::StandardActionLocker locker( mainScreenWidget, MainScreenWidget::LockOptions::Menu );
   
//   cursor.show();

//   cursor.checkposition( getxpos(), getypos() );

   bool resourcesCompared = false;
   do {
       if ( status == 2 ) {
          execnextreplaymove ( );
          checktimedevents( actmap, &getDefaultMapDisplay() );
         /*
          if ( getxpos () != lastvisiblecursorpos.x || getypos () != lastvisiblecursorpos.y )
             setcursorpos ( lastvisiblecursorpos.x, lastvisiblecursorpos.y );
         */
       }  else {
          PG_Application::GetApp()->sigAppIdle( PG_Application::GetApp() );
          releasetimeslice();
       }

       if (nextaction == rpl_finished  || status != 2 ) {
          if ( CGameOptions::Instance()->replayMovieMode && status != 1 && !(replayRecorder && replayRecorder->isRunning()) ) 
             status = 100;
          else {
            if ( nextaction == rpl_finished && !resourcesCompared ) {

               if ( replayRecorder )
                  replayRecorder->pause();
               
               displaymessage2("running final comparison" );

               int replayedplayer  = actmap->actplayer; 
               actmap->endTurn();
               int nextplayer = findNextPlayer( actmap );
               if ( nextplayer < actmap->actplayer && performEndTurnOperations )
                  actmap->endRound();
                
               actmap->getCursor() = orgmap->getCursor();
               
               resourcesCompared = true;
               ASCString resourceComparisonResult;
               GameMap* comparisonMap = NULL;
               GameMap* nextPlayerMap = NULL;

               if ( replayedplayer == orgmap->actplayer )
                  comparisonMap = orgmap;
               else
                  comparisonMap = nextPlayerMap = loadreplay ( orgmap->replayinfo->map[nextplayer]  );

               if ( !CGameOptions::Instance()->replayMovieMode ) {
                  if ( comparisonMap ) {
                     if ( compareMapResources( comparisonMap, actmap, player, &resourceComparisonResult)) {
                        ViewFormattedText vft( "warning", resourceComparisonResult, PG_Rect( -1, -1, 500, 550 ) );
                        vft.Show();
                        vft.RunModal();
                     }

                  } else
                     error("Replay: no map to compare to!");
               }

               delete nextPlayerMap;
            }
          }
       }

       for ( int i = 0; i < 5; ++i )
          getPGApplication().processEvent();

   } while ( status > 0  &&  status < 100 ) ;

   delete actmap;
   actmap = orgmap;

   int st = status;
   status = 0;

   updateFieldInfo();

   if ( st == 101 )
      return 1;
   else
      return 0;
}

void trunreplay :: firstinit ( void )
{
    status = 0;
}


void hookReplayToSystem()
{
   ActionContainer::commitCommand.connect( SigC::slot( &logActionToReplay ));
}
