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

#include "gui.h"
#include "controls.h"

/*! \file replay.h
    \brief Interface for recording and playing replays
*/


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
                    rpl_setResourceProcessingAmount };

extern void logtoreplayinfo ( trpl_actions action, ... );

class trunreplay {
            ASCString lastErrorMessage;
         protected:
            int movenum;
            treplayguihost& gui;
            void execnextreplaymove ( void );
            pmap orgmap;
            pmemorystream stream;
            int removeunit ( int x, int y, int nwid );
            int removeunit ( pvehicle eht, int nwid );
            void wait ( int t = ticker );
            void wait ( MapCoordinate pos, int t = ticker );
            void wait ( MapCoordinate pos1, MapCoordinate pos2, int t = ticker );
            int actplayer;

            void error( const char* message, ... );

            char nextaction;

            void readnextaction ( void );
            void displayActionCursor ( int x1, int y1, int x2 = -1, int y2 = -1, int secondWait = 0 );
            void removeActionCursor( void );

         public:

            preactionfire_replayinfo getnextreplayinfo ( void );

            trunreplay ( void );
            int status;
            void firstinit ( void );
            int  run ( int player, int viewingplayer );
   };

class LockReplayRecording {
        tmap::ReplayInfo& ri;
     public:
        LockReplayRecording ( tmap::ReplayInfo& _ri );
        ~LockReplayRecording();
};

extern trunreplay runreplay;

//! a hack to enable the replay in a game that was started without replays
extern int startreplaylate;

//! checks if there is replay information and -if there is any- runs the replay
extern void checkforreplay ( void );

//! Initialized the replay logging at the beginning of a players or the ai's turn.
extern void initReplayLogging();

//! Close the replay logging at the end of a players or the ai's turn.
extern void closeReplayLogging();

//! runs the replay for the given player
extern void runSpecificReplay( int player, int viewingplayer );


#endif
