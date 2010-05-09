/*! \file controls.h
   Controlling units (which is graudally moved to vehicletype.cpp and unitctrl.cpp );
   Resource networks
   Things that are run when starting and ending someones turn   
*/

/*
    This file is part of Advanced Strategic Command; http://www.asc-hq.de
    Copyright (C) 1994-1999  Martin Bickel  and  Marc Schellenberger

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; see the file COPYING. If not, write to the 
    Free Software Foundation, Inc., 59 Temple Place, Suite 330, 
    Boston, MA  02111-1307  USA
*/

#if defined(karteneditor) 
# error the mapeditor should not need to use turncontrol.h !
#endif


#ifndef turncontrolH
#define turncontrolH

#include "ascstring.h"

class Player;
class GameMap;
class MapDisplayInterface;

/** describes the behavier that next_turn() shall do when it finds out that the current player is the last one available */
class NextTurnStrategy {
   public:
      virtual bool continueWhenLastPlayer() const = 0;  
      virtual bool authenticate( GameMap* actmap ) const = 0;
      virtual ~NextTurnStrategy() {};
};

class NextTurnStrategy_AskUser : public NextTurnStrategy {
   public:
      bool continueWhenLastPlayer() const;  
      bool authenticate( GameMap* actmap  ) const;  
} ;

class NextTurnStrategy_Abort : public NextTurnStrategy {
   public:
      bool continueWhenLastPlayer() const;
      bool authenticate( GameMap* actmap) const;
} ;



/*! Ends the turn of the current player and runs AI until a player is human again
    \param playerView -2 = detect automatically; -1 = don't display anything; 0-7 = this player is watching
*/
extern void next_turn ( GameMap* gameMap, const NextTurnStrategy& nextTurnStrategy, MapDisplayInterface* display, int playerView = -2 );

//! checks if the current player has terminated another player or even won
extern void  checkforvictory ( GameMap* gamemap, bool hasTurnControl );


//! continues a PBeM game; the current map is deleted
extern GameMap* continueNetworkGame ( bool mostRecent = false );

extern GameMap* continueNetworkGame ( const ASCString& filename );


//! this checks if one player has been using a new ASC version than the current player. If this is the case, the current player is notified and asked to upgrade
extern void checkUsedASCVersions( Player& currentPlayer );

class AbstractPlayerProcessing {
   public:
       virtual void playerSkipped( Player& player ) = 0;
       virtual ~AbstractPlayerProcessing(){};
};
    
extern int findNextPlayer( GameMap* actmap, AbstractPlayerProcessing* playerProcessor = NULL );

//! skips the next player. This is for administrative use, for example if a player does not react
extern void skipTurn( GameMap* gamemap );



#endif
