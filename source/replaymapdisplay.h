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
 #error the mapeditor should not need to use controls.h !
#endif


#ifndef replaymapdisplayH
#define replaymapdisplayH
#include "typen.h"
#include "mapdisplayinterface.h"

class ReplayMapDisplay : public MapDisplayInterface {
           MapDisplayInterface* mapDisplay;
           int cursorDelay;
           void wait ( int minTime = 0 );
         public:
           ReplayMapDisplay ( MapDisplayInterface* md ) { mapDisplay = md; cursorDelay = 20; };
           int displayMovingUnit ( const MapCoordinate3D& start, const MapCoordinate3D& dest, Vehicle* vehicle, int fieldnum, int totalmove, SoundStartCallback startSound );
           void displayPosition ( int x, int y );
           void displayMap ( Vehicle* additionalVehicle ) { mapDisplay->displayMap( additionalVehicle ); };
           void displayMap ( void ) { mapDisplay->displayMap(); };
           void resetMovement ( void ) { mapDisplay->resetMovement(); };
           void startAction ( void ) { mapDisplay->startAction(); };
           void stopAction ( void ) { mapDisplay->stopAction(); };
           void cursor_goto( const MapCoordinate& pos ) { mapDisplay->cursor_goto(pos);};
           void displayActionCursor ( int x1, int y1, int x2 , int y2 , int secondWait );
           void displayActionCursor ( int x1, int y1 ) { displayActionCursor ( x1, y1, -1, -1, 0 ); };
           void displayActionCursor ( int x1, int y1, int x2 , int y2 ) { displayActionCursor ( x1, y1, x2, y2, 0 ); };
           void removeActionCursor ( void );
           int checkMapPosition ( int x, int y );
           void setCursorDelay  ( int time ) { cursorDelay = time; };
           void updateDashboard() { mapDisplay->updateDashboard(); };
           void repaintDisplay () { mapDisplay->repaintDisplay(); };
           void setTempView( bool view )  { mapDisplay->setTempView( view ); };
           void showBattle( tfight& battle ) { mapDisplay->showBattle( battle ); };

    };

#endif
