/***************************************************************************
                          replay.h  -  description
                             -------------------
    begin                : Sun Jan 21 2001
    copyright            : (C) 2001 by Martin Bickel
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

#ifndef replayH
 #define replayH

#include "events.h"
#include "ascstring.h"
#include "gamemap.h"
#include "basestreaminterface.h"

/*! \file replay.h
    \brief Interface for recording and playing replays
*/


class ReplayMapDisplay;

class trunreplay {
            ASCString lastErrorMessage;
            ReplayMapDisplay* replayMapDisplay;
         protected:
            int movenum;
            void execnextreplaymove ( void );
            GameMap* orgmap;
            tmemorystream* stream;
            void wait ( int t = ticker );
            void wait ( MapCoordinate pos, int t = ticker );
            void wait ( MapCoordinate pos1, MapCoordinate pos2, int t = ticker );
            int actplayer;

            void error( const char* message, ... );
            void error( const MapCoordinate& pos, const char* message, ... );
            void error( const ASCString& message );
            void error( const MapCoordinate& pos, const ASCString& message );
            void error( const ActionResult& res );

            char nextaction;

            Context createReplayContext();
            
            void readnextaction ( void );
            void displayActionCursor ( int x1, int y1, int x2 = -1, int y2 = -1, int secondWait = 0 );
            void removeActionCursor( void );

         public:
            trunreplay();
            int status;
            void firstinit();
            int  run ( int player, int viewingplayer, bool performEndTurnOperations );
            ~trunreplay();
   };

class LockReplayRecording {
        GameMap::ReplayInfo& ri;
     public:
        LockReplayRecording ( GameMap::ReplayInfo& _ri );
        ~LockReplayRecording();
};

extern trunreplay runreplay;

//! a hack to enable the replay in a game that was started without replays
extern int startreplaylate;

//! checks if there is replay information and -if there is any- runs the replay
extern void checkforreplay ( void );

//! Initialized the replay logging at the beginning of a players or the ai's turn.
extern void initReplayLogging( Player& player );

//! runs the replay for the given player
extern void runSpecificReplay( int player, int viewingplayer, bool performEndTurnOperations = true );

//! runs the replay of the current player. This is used primarily for debugging the replay system
extern void viewOwnReplay( Player& player );

//! initialized the replay system at program startup
extern void hookReplayToSystem();

#endif
