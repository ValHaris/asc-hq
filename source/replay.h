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

#ifndef replay_h_included
 #define replay_h_included

#include "gui.h"
#include "controls.h"


enum trpl_actions { rpl_attack, rpl_move, rpl_changeheight, rpl_convert, rpl_remobj, rpl_buildobj, rpl_putbuilding,
                    rpl_removebuilding, rpl_putmine, rpl_removemine, rpl_produceunit, rpl_removeunit, rpl_trainunit,
                    rpl_reactionfire, rpl_finished, rpl_shareviewchange, rpl_alliancechange, rpl_move2, rpl_buildtnk,
                    rpl_refuel, rpl_bldrefuel };

extern void logtoreplayinfo ( trpl_actions action, ... );

class trunreplay {
         protected:
            int movenum;
            treplayguihost gui;
            void execnextreplaymove ( void );
            pmap orgmap;
            pmemorystream stream;
            int removeunit ( int x, int y, int nwid );
            int removeunit ( pvehicle eht, int nwid );
            void wait ( int t = ticker );
            int actplayer;

            char nextaction;

            void readnextaction ( void );
            void displayActionCursor ( int x1, int y1, int x2 = -1, int y2 = -1, int secondWait = 0 );
            void removeActionCursor( void );

         public:

            preactionfire_replayinfo getnextreplayinfo ( void );

            trunreplay ( void );
            int status;
            void firstinit ( void );
            int  run ( int player );

   };
extern trunreplay runreplay;

extern int startreplaylate;

//! checks if there is replay information and -if there is any- runs the replay
extern void checkforreplay ( void );

#endif