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
#include "unitctrl.h"
#include "replaymapdisplay.h"
#include "asc-mainscreen.h"
#include "loaders.h"
#include "turncontrol.h"
#include "widgets/textrenderer.h"
#include "actions/jumpdrive.h"
#include "reactionfire.h"
#include "gameeventsystem.h"
#include "sdl/graphicsqueue.h"
#include "video/videorecorder.h"
#include "iconrepository.h"
#include "dialogs/replayrecorder.h"

trunreplay runreplay;

int startreplaylate = 0;


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
      
      void start( const ASCString& filename, bool append )
      {
         lastFilename = filename;
         movieModeStorage = CGameOptions::Instance()->replayMovieMode;
         CGameOptions::Instance()->replayMovieMode = true;
         ASCString newFilename = constructFileName( 0, "", filename );
         if ( !rec || !append || newFilename != rec->getFilename()  ) {
            delete rec;
            rec = new VideoRecorder( newFilename, PG_Application::GetScreen());
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
        if ( replayRecorder )
           filename = replayRecorder->getLastFilename();
        
        ReplayRecorderDialog rrd( filename );
        rrd.Show();
        rrd.RunModal();
        rrd.Hide();
         
        if ( !replayRecorder )
           replayRecorder = new ReplayRecorder();
        replayRecorder->start( rrd.getFilename(), rrd.getAppend() );
        
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

#ifndef _DEBUG
       catch ( ... ) {     // this will catch NullPointer-Exceptions and stuff like that, which we want to pass to the debugger  in debug mode
#else
       catch ( GameMap ) {  // will never be thrown, but we need catch statement for the try block
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

void logtoreplayinfo ( trpl_actions _action, ... )
{
   char action = _action;
   if ( actmap->replayinfo && actmap->replayinfo->actmemstream && !actmap->replayinfo->stopRecordingActions) {
      pnstream stream = actmap->replayinfo->actmemstream;

      va_list paramlist;
      va_start ( paramlist, _action );

      if ( action == rpl_attack || action == rpl_reactionfire ) {
         int x1 =  va_arg ( paramlist, int );
         int y1 =  va_arg ( paramlist, int );
         int x2 =  va_arg ( paramlist, int );
         int y2 =  va_arg ( paramlist, int );
         int ad1 =  va_arg ( paramlist, int );
         int ad2 =  va_arg ( paramlist, int );
         int dd1 =  va_arg ( paramlist, int );
         int dd2 =  va_arg ( paramlist, int );
         int wpnum =  va_arg ( paramlist, int );
         int size = 9;
         stream->writeChar ( action );
         stream->writeInt ( size );
         stream->writeInt ( x1 );
         stream->writeInt ( y1 );
         stream->writeInt ( x2 );
         stream->writeInt ( y2 );
         stream->writeInt ( ad1 );
         stream->writeInt ( ad2 );
         stream->writeInt ( dd1 );
         stream->writeInt ( dd2 );
         stream->writeInt ( wpnum );
      }

      if ( action == rpl_move ) {
         int x1 =  va_arg ( paramlist, int );
         int y1 =  va_arg ( paramlist, int );
         int x2 =  va_arg ( paramlist, int );
         int y2 =  va_arg ( paramlist, int );
         int nwid = va_arg ( paramlist, int );
         stream->writeChar ( action );
         int size = 5;
         stream->writeInt ( size );
         stream->writeInt ( x1 );
         stream->writeInt ( y1 );
         stream->writeInt ( x2 );
         stream->writeInt ( y2 );
         stream->writeInt ( nwid );
      }
      if ( action == rpl_move2 || action == rpl_move3 || action == rpl_move4 || action==rpl_move5) {
         int x1 =  va_arg ( paramlist, int );
         int y1 =  va_arg ( paramlist, int );
         int x2 =  va_arg ( paramlist, int );
         int y2 =  va_arg ( paramlist, int );
         int nwid = va_arg ( paramlist, int );
         int height = va_arg ( paramlist, int );

         stream->writeChar ( action );
         if ( action == rpl_move5 ) {
            int size = 8;
            stream->writeInt( size );
         } else 
            if ( action == rpl_move2 ) {
               int size = 6;
               stream->writeInt ( size );
            } else {
               int size = 7;
               stream->writeInt ( size );
            }
         stream->writeInt ( x1 );
         stream->writeInt ( y1 );
         stream->writeInt ( x2 );
         stream->writeInt ( y2 );
         stream->writeInt ( nwid );
         stream->writeInt ( height );
         if ( action == rpl_move3 || action == rpl_move4 || action==rpl_move5) {
            int nointerrupt = va_arg ( paramlist, int );
            stream->writeInt ( nointerrupt );
            if ( action==rpl_move5 ) {
               int destDamage = va_arg( paramlist, int );
               stream->writeInt( destDamage );
            }
         }
      }

      if ( action == rpl_changeheight || action == rpl_changeheight2 ) {
         int x1 =  va_arg ( paramlist, int );
         int y1 =  va_arg ( paramlist, int );
         int x2 =  va_arg ( paramlist, int );
         int y2 =  va_arg ( paramlist, int );
         int nwid = va_arg ( paramlist, int );
         int oldheight = va_arg ( paramlist, int );
         int newheight = va_arg ( paramlist, int );
         stream->writeChar ( action );
         if ( action == rpl_changeheight ) {
            int size = 7;
            stream->writeInt ( size );
         } else {
            int size = 8;
            stream->writeInt ( size );
         }
         stream->writeInt ( x1 );
         stream->writeInt ( y1 );
         stream->writeInt ( x2 );
         stream->writeInt ( y2 );
         stream->writeInt ( nwid );
         stream->writeInt ( oldheight );
         stream->writeInt ( newheight );
         if ( action == rpl_changeheight2 ) {
            int noInterrupt = va_arg ( paramlist, int );
            stream->writeInt ( noInterrupt );
         }
      }
      if ( action == rpl_convert  ) {
         int x =  va_arg ( paramlist, int );
         int y =  va_arg ( paramlist, int );
         int col =  va_arg ( paramlist, int );
         stream->writeChar ( action );
         int size = 3;
         stream->writeInt ( size );
         stream->writeInt ( x );
         stream->writeInt ( y );
         stream->writeInt ( col );
      }
      if ( action == rpl_convert2  ) {
         int x =  va_arg ( paramlist, int );
         int y =  va_arg ( paramlist, int );
         int col =  va_arg ( paramlist, int );
         int nwid =  va_arg ( paramlist, int );
         stream->writeChar ( action );
         int size = 4;
         stream->writeInt ( size );
         stream->writeInt ( x );
         stream->writeInt ( y );
         stream->writeInt ( col );
         stream->writeInt ( nwid );
      }
      if ( action == rpl_buildobj || action == rpl_remobj ) {
         int x =  va_arg ( paramlist, int );
         int y =  va_arg ( paramlist, int );
         int id =  va_arg ( paramlist, int );
         stream->writeChar ( action );
         int size = 3;
         stream->writeInt ( size );
         stream->writeInt ( x );
         stream->writeInt ( y );
         stream->writeInt ( id );
      }
      if ( action == rpl_buildobj2 || action == rpl_remobj2 ) {
         int x =  va_arg ( paramlist, int );
         int y =  va_arg ( paramlist, int );
         int id =  va_arg ( paramlist, int );
         int unit = va_arg ( paramlist, int );
         stream->writeChar ( action );
         int size = 4;
         stream->writeInt ( size );
         stream->writeInt ( x );
         stream->writeInt ( y );
         stream->writeInt ( id );
         stream->writeInt ( unit );
      }
      if ( action == rpl_buildtnk || action == rpl_buildtnk3 || action == rpl_buildtnk4 ) {
         int x =  va_arg ( paramlist, int );
         int y =  va_arg ( paramlist, int );
         int id =  va_arg ( paramlist, int );
         int col =  va_arg ( paramlist, int );
         stream->writeChar ( action );
         int size;
         if ( action == rpl_buildtnk )
            size = 4;
         else
            if ( action == rpl_buildtnk4 )
               size = 7;
            else
               size = 6;

         stream->writeInt ( size );
         stream->writeInt ( x );
         stream->writeInt ( y );
         stream->writeInt ( id );
         stream->writeInt ( col );
         if ( action == rpl_buildtnk3 || action == rpl_buildtnk4 ) {
            stream->writeInt ( va_arg ( paramlist, int ) ); // constructor x
            stream->writeInt ( va_arg ( paramlist, int ) ); // constructor x
            if ( action == rpl_buildtnk4  )
               stream->writeInt ( va_arg ( paramlist, int ) ); // unit height
         }
      }
      if ( action == rpl_buildtnk2 ) {
         stream->writeChar ( action );
         int size = 5;
         stream->writeInt ( size );
         stream->writeInt ( va_arg ( paramlist, int ) );
         stream->writeInt ( va_arg ( paramlist, int ) );
         stream->writeInt ( va_arg ( paramlist, int ) );
         stream->writeInt ( va_arg ( paramlist, int ) );
         stream->writeInt ( va_arg ( paramlist, int ) );
      }
      if ( action == rpl_putbuilding || action == rpl_putbuilding2 ) {
         int x =  va_arg ( paramlist, int );
         int y =  va_arg ( paramlist, int );
         int id = va_arg ( paramlist, int );
         int col =  va_arg ( paramlist, int );
         stream->writeChar ( action );
         if ( action == rpl_putbuilding )
            stream->writeInt ( 4 );
         else
            stream->writeInt ( 5 );

         stream->writeInt ( x );
         stream->writeInt ( y );
         stream->writeInt ( id );
         stream->writeInt ( col );
         if ( action == rpl_putbuilding2 )
            stream->writeInt ( va_arg ( paramlist, int ));
      }
      if ( action == rpl_putmine || action == rpl_putmine2 ) {
         int x =  va_arg ( paramlist, int );
         int y =  va_arg ( paramlist, int );
         int col =  va_arg ( paramlist, int );
         int typ = va_arg ( paramlist, int );
         int strength = va_arg ( paramlist, int );
         int nwid = -1; 

         stream->writeChar ( action );
         int size;
         if ( action == rpl_putmine ) {
            size = 5;
         } else {
            nwid = va_arg ( paramlist, int );
            size = 6;
         }
         stream->writeInt ( size );
         stream->writeInt ( x );
         stream->writeInt ( y );
         stream->writeInt ( col );
         stream->writeInt ( typ );
         stream->writeInt ( strength );

         if ( action == rpl_putmine2 ) 
            stream->writeInt ( nwid );
         
      }
      if ( action == rpl_removemine ) {
         int x =  va_arg ( paramlist, int );
         int y =  va_arg ( paramlist, int );
         stream->writeChar ( action );
         int size = 2;
         stream->writeInt ( size );
         stream->writeInt ( x );
         stream->writeInt ( y );
      }
      if ( action == rpl_removebuilding ) {
         int x =  va_arg ( paramlist, int );
         int y =  va_arg ( paramlist, int );
         stream->writeChar ( action );
         int size = 2;
         stream->writeInt ( size );
         stream->writeInt ( x );
         stream->writeInt ( y );
      }
      if ( action == rpl_removebuilding2 || action == rpl_removebuilding3) {
         int x =  va_arg ( paramlist, int );
         int y =  va_arg ( paramlist, int );
         int nwid = va_arg ( paramlist, int );
         stream->writeChar ( action );
         if ( action == rpl_removebuilding2 ) {
            int size = 3;
            stream->writeInt ( size );
            stream->writeInt ( x );
            stream->writeInt ( y );
            stream->writeInt( nwid );
         } else {
            int size = 6;
            stream->writeInt ( size );
            stream->writeInt ( x );
            stream->writeInt ( y );
            stream->writeInt( nwid );
            int e =  va_arg ( paramlist, int );
            int m =  va_arg ( paramlist, int );
            int f =  va_arg ( paramlist, int );
            stream->writeInt( e );
            stream->writeInt( m );
            stream->writeInt( f );
         }
      }

      if ( action == rpl_produceunit ) {
         int id = va_arg ( paramlist, int );
         int col = va_arg ( paramlist, int );
         int x =  va_arg ( paramlist, int );
         int y =  va_arg ( paramlist, int );
         int cl = va_arg ( paramlist, int );
         int nwid = va_arg ( paramlist, int );
         stream->writeChar ( action );
         int size = 6;
         stream->writeInt ( size );
         stream->writeInt ( id );
         stream->writeInt ( col );
         stream->writeInt ( x );
         stream->writeInt ( y );
         stream->writeInt ( cl );
         stream->writeInt ( nwid );
      }
      if ( action == rpl_removeunit ) {
         int x =  va_arg ( paramlist, int );
         int y =  va_arg ( paramlist, int );
         int nwid = va_arg ( paramlist, int );
         stream->writeChar ( action );
         int size = 3;
         stream->writeInt ( size );
         stream->writeInt ( x );
         stream->writeInt ( y );
         stream->writeInt ( nwid );
      }
      if ( action == rpl_trainunit ) {
         int x =  va_arg ( paramlist, int );
         int y =  va_arg ( paramlist, int );
         int exp =  va_arg ( paramlist, int );
         int nwid = va_arg ( paramlist, int );
         stream->writeChar ( action );
         int size = 4;
         stream->writeInt ( size );
         stream->writeInt ( x );
         stream->writeInt ( y );
         stream->writeInt ( exp );
         stream->writeInt ( nwid );
      }
      /*
      not supported any more because of new diplomatic model
      if ( action == rpl_shareviewchange ) {
         stream->writeChar ( action );
         if ( actmap->shareview ) {
            int size = 8*8/4+1;
            stream->writeInt ( size );
            for ( int a = 0; a < 8; a++ )
               for ( int b = 0; b < 8; b++ )
                   stream->writeChar ( actmap->shareview->mode[a][b] );
            stream->writeInt ( actmap->shareview->recalculateview );
         } else {
            int size = 0;
            stream->writeInt ( size );
         }
      }
      */
      if ( action == rpl_alliancechange2 ) {
         stream->writeChar ( action );
         stream->writeInt( 3 ); // size

         int actingPlayer =  va_arg ( paramlist, int );
         int targetPlayer =  va_arg ( paramlist, int );
         int state = va_arg ( paramlist, int );
         stream->writeInt( actingPlayer );
         stream->writeInt( targetPlayer );
         stream->writeInt( state );
      }
      if ( action == rpl_refuel || action == rpl_refuel2 ) {
         int x =  va_arg ( paramlist, int );
         int y =  va_arg ( paramlist, int );
         int nwid = va_arg ( paramlist, int );
         int pos = va_arg ( paramlist, int );
         int amnt = va_arg ( paramlist, int );
         stream->writeChar ( action );
         stream->writeInt ( action==rpl_refuel ? 5 : 6 );
         stream->writeInt ( x );
         stream->writeInt ( y );
         stream->writeInt ( nwid );
         stream->writeInt ( pos );
         stream->writeInt ( amnt );
         if ( action == rpl_refuel2 ) {
             int old = va_arg ( paramlist, int );
             stream->writeInt( old );
         }
      }
      if ( action == rpl_refuel3 ) {
         int id =  va_arg ( paramlist, int );
         int resource = va_arg ( paramlist, int );
         int delta = va_arg ( paramlist, int );
         stream->writeChar ( action );
         stream->writeInt ( 3 );
         stream->writeInt ( id );
         stream->writeInt ( resource );
         stream->writeInt ( delta );
      }
      if ( action == rpl_bldrefuel ) {
         int x =  va_arg ( paramlist, int );
         int y =  va_arg ( paramlist, int );
         int pos = va_arg ( paramlist, int );
         int amnt = va_arg ( paramlist, int );
         stream->writeChar ( action );
         int size = 4;
         stream->writeInt ( size );
         stream->writeInt ( x );
         stream->writeInt ( y );
         stream->writeInt ( pos );
         stream->writeInt ( amnt );
      }
      if ( action == rpl_moveUnitUpDown ) {
         int x =  va_arg ( paramlist, int );
         int y =  va_arg ( paramlist, int );
         int nwid_from = va_arg ( paramlist, int );
         int nwid_to = va_arg ( paramlist, int );
         int nwid_moving = va_arg ( paramlist, int );
         stream->writeChar ( action );
         int size = 5;
         stream->writeInt ( size );
         stream->writeInt ( x );
         stream->writeInt ( y );
         stream->writeInt ( nwid_from );
         stream->writeInt ( nwid_to );
         stream->writeInt ( nwid_moving );
      }
      if ( action == rpl_moveUnitUp ) {
         int x =  va_arg ( paramlist, int );
         int y =  va_arg ( paramlist, int );
         int nwid = va_arg ( paramlist, int );
         stream->writeChar ( action );
         int size = 3;
         stream->writeInt ( size );
         stream->writeInt ( x );
         stream->writeInt ( y );
         stream->writeInt ( nwid );
      }
      if ( action == rpl_productionResourceUsage ) {
         int en = va_arg ( paramlist, int );
         int ma = va_arg ( paramlist, int );
         int fu = va_arg ( paramlist, int );
         int x = va_arg ( paramlist, int );
         int y = va_arg ( paramlist, int );
         stream->writeChar ( action );
         int size = 5;
         stream->writeInt ( size );
         stream->writeInt ( en );
         stream->writeInt ( ma );
         stream->writeInt ( fu );
         stream->writeInt ( x );
         stream->writeInt ( y );
      }
      if ( action == rpl_repairUnit ) {
         int nwid = va_arg ( paramlist, int );
         int destnwid = va_arg ( paramlist, int );
         int amount = va_arg ( paramlist, int );
         int matremain = va_arg ( paramlist, int );
         int fuelremain = va_arg ( paramlist, int );

         stream->writeChar ( action );
         int size = 5;
         stream->writeInt ( size );
         stream->writeInt ( nwid );
         stream->writeInt ( destnwid );
         stream->writeInt ( amount );
         stream->writeInt ( matremain );
         stream->writeInt ( fuelremain );
      }
      if ( action == rpl_repairUnit2 || action == rpl_repairBuilding ) {
         int x = va_arg ( paramlist, int );
         int y = va_arg ( paramlist, int );
         int destnwid = va_arg ( paramlist, int );
         int amount = va_arg ( paramlist, int );

         stream->writeChar ( action );
         int size = 4;
         stream->writeInt ( size );
         stream->writeInt ( x );
         stream->writeInt ( y );
         stream->writeInt ( destnwid );
         stream->writeInt ( amount );
      }
      if ( action == rpl_repairUnit3  ) {
         int serviceNWID = va_arg ( paramlist, int );
         int destnwid = va_arg ( paramlist, int );
         int amount = va_arg ( paramlist, int );

         stream->writeChar ( action );
         int size = 3;
         stream->writeInt ( size );
         stream->writeInt ( serviceNWID );
         stream->writeInt ( destnwid );
         stream->writeInt ( amount );
      }
      if ( action == rpl_produceAmmo ) {
         int x = va_arg ( paramlist, int );
         int y = va_arg ( paramlist, int );
         int type = va_arg ( paramlist, int );
         int amount = va_arg ( paramlist, int );
         stream->writeChar ( action );
         int size = 4;
         stream->writeInt ( size );
         stream->writeInt ( x );
         stream->writeInt ( y );
         stream->writeInt ( type );
         stream->writeInt ( amount );
      }
      if ( action == rpl_buildProdLine || action == rpl_removeProdLine) {
         int building = va_arg ( paramlist, int );
         int id = va_arg ( paramlist, int );
         stream->writeChar ( action );
         int size = 2;
         stream->writeInt ( size );
         stream->writeInt ( building );
         stream->writeInt ( id );
      }

      if ( action == rpl_recycleUnit ) {
         int building = va_arg ( paramlist, int );
         int id = va_arg ( paramlist, int );
         stream->writeChar ( action );
         int size = 2;
         stream->writeInt ( size );
         stream->writeInt ( building );
         stream->writeInt ( id );
      }

      if ( action == rpl_techResearched ) {
         int tech = va_arg ( paramlist, int );
         int player = va_arg ( paramlist, int );
         stream->writeChar ( action );
         int size = 2;
         stream->writeInt ( size );
         stream->writeInt ( tech );
         stream->writeInt ( player );
      }
      if ( action == rpl_setGeneratorStatus ) {
         int vehid = va_arg ( paramlist, int );
         int status = va_arg ( paramlist, int );
         stream->writeChar ( action );
         int size = 2;
         stream->writeInt ( size );
         stream->writeInt ( vehid );
         stream->writeInt ( status );
      }
      if ( action == rpl_cutFromGame ) {
         int vehid = va_arg( paramlist, int );
         Resources res = getUnitResourceCargo ( actmap->getUnit ( vehid ));
         stream->writeChar ( action );
         int size = 4;
         stream->writeInt ( size );
         stream->writeInt ( vehid );
         stream->writeInt ( res.energy );
         stream->writeInt ( res.material );
         stream->writeInt ( res.fuel );
      }
      if ( action == rpl_setResearch ) {
         int bldid = va_arg ( paramlist, int );
         int amount = va_arg ( paramlist, int );
         stream->writeChar ( action );
         int size = 2;
         stream->writeInt ( size );
         stream->writeInt ( bldid );
         stream->writeInt ( amount );
      }
      if ( action == rpl_setResourceProcessingAmount ) {
         int x = va_arg ( paramlist, int );
         int y = va_arg ( paramlist, int );
         int e = va_arg ( paramlist, int );
         int m = va_arg ( paramlist, int );
         int f = va_arg ( paramlist, int );
         stream->writeChar ( action );
         int size = 5;
         stream->writeInt ( size );
         stream->writeInt ( x );
         stream->writeInt ( y );
         stream->writeInt ( e );
         stream->writeInt ( m );
         stream->writeInt ( f );
      }
      if ( action == rpl_netcontrol ) {
         int x = va_arg ( paramlist, int );
         int y = va_arg ( paramlist, int );
         int cat = va_arg ( paramlist, int );
         int stat = va_arg ( paramlist, int );
         stream->writeChar ( action );
         int size = 4;
         stream->writeInt ( size );
         stream->writeInt ( x );
         stream->writeInt ( y );
         stream->writeInt ( cat );
         stream->writeInt ( stat );
      }
      if ( action == rpl_jump ) {
         int nwid = va_arg ( paramlist, int );
         int x = va_arg ( paramlist, int );
         int y = va_arg ( paramlist, int );
         stream->writeChar ( action );
         int size = 3;
         stream->writeInt ( size );
         stream->writeInt ( nwid );
         stream->writeInt ( x );
         stream->writeInt ( y );
      }
      if ( action == rpl_transferTribute ) {
         stream->writeChar( action );
         stream->writeInt( 1 );
         int player = va_arg( paramlist, int );
         stream->writeInt( player );
      }

      if ( action == rpl_reactionFireOn || action == rpl_reactionFireOff ) {
         stream->writeChar( action );
         stream->writeInt( 1 );
         int nwid = va_arg( paramlist, int );
         stream->writeInt( nwid );
      }

      va_end ( paramlist );
   }
}


trunreplay :: trunreplay ( void )  
{
   status = -1;
   movenum = 0;
}

void trunreplay::error( const MapCoordinate& pos, const ASCString& message )
{
   error( message );
}


void trunreplay::error( const MapCoordinate& pos, const char* message, ... )
{
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
                           VehicleMovement vm ( &rmd, NULL );
                           vm.execute ( eht, -1, -1, 0 , -1, -1 );

                           int t = ticker;
                           vm.execute ( NULL, x2, y2, 2, -1, -1 );
                           wait( MapCoordinate(x1,y1), MapCoordinate(x2,y2), t );
                           vm.execute ( NULL, x2, y2, 3, -1, -1 );

                           if ( vm.getStatus() != 1000 )
                              eht = NULL;
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
                           ReplayMapDisplay rmd( &getDefaultMapDisplay() );
                           VehicleMovement vm ( &rmd );
                           int t = ticker;
                           vm.execute ( eht, x2, y2, 0, -2, 0 );
                           vm.execute ( eht, x2, y2, 2, height, -1 );
                           wait( MapCoordinate(x1,y1), MapCoordinate(x2,y2), t );
                           vm.execute ( NULL, x2, y2, 3, height, noInterrupt > 0 ? VehicleMovement::NoInterrupt : 0 );

                           if ( vm.getStatus() != 1000 ) {
                              if ( CGameOptions::Instance()->replayMovieMode ) {

                                 tfield* fld = eht->getMap()->getField(x1,y1);
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

                                 tfield* fld2 = eht->getMap()->getField(x2,y2);
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

                           tfield* fld = getfield ( x1, y1 );
                           tfield* targ = getfield ( x2, y2 );
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
                        int oldheight = stream->readInt();
                        int newheight = stream->readInt();
                        int noInterrupt = -1;

                        if ( nextaction == rpl_changeheight2 )
                           noInterrupt = stream->readInt();


                        readnextaction();

                        Vehicle* eht = actmap->getUnit ( x1, y1, nwid );
                        if ( eht ) {
                           ReplayMapDisplay rmd( &getDefaultMapDisplay() );
                           VehicleAction* va;
                           if ( newheight > oldheight )
                              va = new IncreaseVehicleHeight ( &rmd, NULL );
                           else
                              va = new DecreaseVehicleHeight ( &rmd, NULL );

                           va->execute ( eht, -1, -1, 0 , newheight, -1 );

                           int t = ticker;
                           va->execute ( NULL, x2, y2, 2, -1, -1 );
                           wait( MapCoordinate(x1,y1), MapCoordinate(x2,y2), t );
                           va->execute ( NULL, x2, y2, 3, -1, noInterrupt );

                           if ( va->getStatus() != 1000 )
                              eht = NULL;

                           delete va;
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

                              tfield* fld = getfield ( x, y );
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

                           tfield* fld = getfield ( x, y );
                           if ( obj && fld ) {
                              displayActionCursor ( x, y );

                              Resources cost;
                              int movecost;

                              RecalculateAreaView rav ( actmap, MapCoordinate(x,y), maxViewRange / maxmalq + 1 );
                              
                              bool objectAffectsVisibility = obj->basicjamming_plus || obj->viewbonus_plus || obj->viewbonus_abs != -1 || obj->basicjamming_abs != -1;
                              if ( objectAffectsVisibility )
                                 rav.removeView();


                              if ( actaction == rpl_remobj || actaction == rpl_remobj2 ) {
                                 cost = obj->removecost;
                                 fld->removeobject ( obj );
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

                              // if ( fieldvisiblenow ( fld, actmap->getPlayerView() ))
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

                           tfield* fld = getfield ( x, y );

                           Vehicletype* tnk = vehicleTypeRepository.getObject_byID ( id );

                           if ( fld && tnk && !fld->vehicle ) {
                              displayActionCursor ( x, y );
                              Vehicle* v = new Vehicle ( tnk, actmap, col );
                              v->xpos = x;
                              v->ypos = y;
                              fld->vehicle = v;
                              if ( height >= 0 )
                                 v->height = height;

                              if ( constx >= 0 && consty >= 0 ) {
                                 tfield* constructorField = getfield(constx, consty );
                                 if ( constructorField->vehicle ) {
                                    Resources r ( 0, tnk->productionCost.material, tnk->productionCost.energy );
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
                               int color = stream->readInt();
                               int networkid = 0;
                               if ( size == 5 )
                                   networkid = stream->readInt();

                               readnextaction();

                               tfield* fld = getfield ( x, y );

                               BuildingType* bld = buildingTypeRepository.getObject_byID ( id );

                               if ( bld && fld ) {
                                  displayActionCursor ( x, y );
                                  putbuilding2( MapCoordinate(x, y), color, bld );
                                  computeview( actmap );
                                  if ( networkid ) {
                                     Vehicle* veh = actmap->getUnit( networkid );
                                     if ( veh ) {
                                       int mf = actmap->getgameparameter ( cgp_building_material_factor );
                                       int ff = actmap->getgameparameter ( cgp_building_fuel_factor );
                                       if ( mf <= 0 )
                                          mf = 100;

                                       if ( ff <= 0 )
                                          ff = 100;

                                       Resources res ( 0, bld->productionCost.material * mf / 100, bld->productionCost.fuel * ff / 100 );
                                       Resources got = static_cast<ContainerBase*>(veh)->getResource( res, 0 );
                                       if ( got < res )
                                          error(MapCoordinate(x,y), "severe replay inconsistency:\nnot enough resources to build/remove building !");

                                     } else
                                        error(MapCoordinate(x,y), "severe replay inconsistency:\nCannot find vehicle to build/remove building !");
                                  }

                                  displaymap();
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

                           tfield* fld = getfield ( x, y );
                           if ( fld ) {
                              displayActionCursor ( x, y );
                              fld -> putmine ( col, typ, strength );
                              if ( nwid >= 0 ) {
                                 Vehicle* veh = actmap->getUnit( nwid );
                                 if ( veh ) {
                                    ContainerControls cc( veh );
                                    if ( cc.getammunition( cwminen, 1, true, true ) != 1 )
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

                           tfield* fld = getfield ( x, y );
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

                           tfield* fld = getfield ( x, y );
                           if ( fld && fld->building ) {
                              displayActionCursor ( x, y );
                              Building* bb = fld->building;
                              if ( nwid >=  0 ) {
                                 Vehicle* veh = actmap->getUnit( nwid );
                                 if ( veh ) {
                                    Resources r = getDestructionCost ( bb, veh );
                                    if ( !veh || veh->getResource(r,false) != res )
                                       error(MapCoordinate(x,y), "severe replay inconsistency:\nfailed to obtain vehicle resources for removebuilding command !");

                                 } else
                                   error(MapCoordinate(x,y), "severe replay inconsistency:\nfailed to obtain vehicle for removebuilding command !");
                              }

                              if ( bb->getCompletion() ) {
                                 bb->setCompletion( bb->getCompletion()-1);
                              } else {
                                 bb->netcontrol = cnet_stopenergyinput + (cnet_stopenergyinput << 1) + (cnet_stopenergyinput << 2);
                                 Resources put = bb->putResource( bb->actstorage, false );
                                 delete bb;
                              }
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

                                 tfield* fld = getfield ( x, y );

                                 Vehicletype* tnk = vehicleTypeRepository.getObject_byID ( id );
                                 if ( tnk && fld) {
                                 
                                    #if 0
                                    printf("produced unit: pos %d / %d; nwid %d; typ id %d; typ %s \n", x,y,nwid,id,tnk->description.c_str() );
                                    #endif

                                    Vehicle* eht = new Vehicle ( tnk, actmap, col / 8 );
                                    eht->xpos = x;
                                    eht->ypos = y;
                                    eht->networkid = nwid;

                                    if ( fld->building ) {
                                       Resources r = fld->building->getResource( tnk->productionCost, 0 );
                                       if ( r < tnk->productionCost ) {
                                          displayActionCursor ( x, y );
                                          error(MapCoordinate(x,y), "severe replay inconsistency: \nNot enough resources to produce unit %s !\nRequired: %d/%d/%d ; Available: %d/%d/%d", eht->typ->description.c_str(), tnk->productionCost.energy, tnk->productionCost.material, tnk->productionCost.fuel, r.energy, r.material, r.fuel);
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
                                 tfield* fld = getfield(x,y);
                                 if ( !fld->vehicle || fld->vehicle->networkid != nwid && fld->building ) {
                                    ContainerControls cc ( fld->building );
                                    cc.destructUnit( actmap->getUnit( nwid ) );
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
                                    ContainerControls cc( bld );
                                    if ( cc.unitTrainingAvailable(eht) )
                                       cc.trainUnit( eht );
                                    else
                                       error(MapCoordinate(x,y), "severe replay inconsistency:\nno vehicle for trainunit command !");
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
                                    int got;
                                    if ( type >= 1000 ) {
                                       got = cb->getResource(delta, type-1000, false);
                                    } else {
                                       ContainerControls cc( cb );
                                       got = cc.getammunition( type, delta, true, true );
                                    }
                                    if ( got != delta )
                                       error("severe replay inconsistency:\nthe resources of container not matching. \nrequired: %d , available: %d !", delta, got);
                                       
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
                                    ContainerBase* from = eht->getCarrier();
                                    ContainerControls cc ( from );
                                    if ( !cc.moveUnitUp( eht ))
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
                                 int type = stream->readInt();
                                 int amount = stream->readInt();
                                 readnextaction();
                                 Building* bld = getfield(x,y)->building;
                                 if ( bld ) {
                                    ContainerControls cc( bld );
                                    cc.produceAmmo( type, amount );
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
                                 Vehicletype* veh = actmap->getvehicletype_byid ( vehicleid );
                                 if ( bld && veh ) {
                                    if ( veh->techDependency.available( actmap->player[ bld->getOwner()].research )) {
                                       ContainerControls cc( bld );
                                       int result = cc.buildProductionLine( veh );
                                       if ( result < 0)
                                          error("severe replay inconsistency:\ncould not build production line!\n%s !", getmessage(result));
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
                                 Vehicletype* veh = actmap->getvehicletype_byid ( vehicleid );
                                 if ( bld && veh ) {
                                    ContainerControls cc( bld );
                                    int result = cc.removeProductionLine( veh );
                                    if ( result < 0)
                                       error("severe replay inconsistency:\ncould not remove production line!\n%s !", getmessage(result));

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
               JumpDrive jd;
               if ( jd.available( veh )) {
                  displayActionCursor ( veh->getPosition().x , veh->getPosition().x, x, y, 0 );
                  ReplayMapDisplay rmd ( &getDefaultMapDisplay() );
                  if ( !jd.jump(veh, MapCoordinate(x,y), &rmd ))
                     error(MapCoordinate(x,y), "Unit cannot jump to this position");
                  
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
                                    ContainerControls cc ( bld );
                                    cc.destructUnit( veh );
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


treactionfire_replayinfo* trunreplay::getnextreplayinfo ( void )
{
   if ( nextaction == rpl_reactionfire ) {
      treactionfire_replayinfo* reac = new treactionfire_replayinfo;
      stream->readInt(); // size

      reac->x1 = stream->readInt();
      reac->y1 = stream->readInt( );
      reac->x2 = stream->readInt( );
      reac->y2 = stream->readInt( );
      reac->ad1 = stream->readInt( );
      reac->ad2 = stream->readInt( );
      reac->dd1 = stream->readInt( );
      reac->dd2 = stream->readInt( );
      reac->wpnum = stream->readInt( );

      readnextaction();

      return reac;
   } else
      return NULL;

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
          checktimedevents( &getDefaultMapDisplay() );
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


void logAllianceChanges( GameMap* map, int player1, int player2, DiplomaticStates s)
{
   if ( map == actmap && actmap )
      logtoreplayinfo ( rpl_alliancechange2, player1, player2, int(s) );
}

void hookReplayToSystem()
{
   DiplomaticStateVector::anyStateChanged.connect( SigC::slot( &logAllianceChanges ));
}
