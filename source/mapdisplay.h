/***************************************************************************
                          mapdisplay.h  -  description
                             -------------------
    begin                : Wed Jan 24 2001
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

#ifndef mapdisplayH
 #define mapdisplayH

#include "libs/loki/Functor.h"

 
#include "typen.h"
#include "vehicle.h"
#include "basegfx.h"
#include "events.h"




class MapDisplayInterface {
         public:
           typedef Loki::Functor<void, TYPELIST_1(int) > SoundStartCallback; 
           virtual int displayMovingUnit ( const MapCoordinate3D& start, const MapCoordinate3D& dest, Vehicle* vehicle, int fieldnum, int totalmove, SoundStartCallback startSound ) = 0;
           virtual void displayMap ( void ) = 0;
           virtual void displayMap ( Vehicle* additionalVehicle ) = 0;
           virtual void displayPosition ( int x, int y ) = 0;
           virtual void resetMovement ( void ) = 0;
           virtual void startAction ( void ) = 0;
           virtual void stopAction ( void ) = 0;
           virtual void cursor_goto ( const MapCoordinate& pos ) = 0;
           virtual void displayActionCursor ( int x1, int y1, int x2, int y2 ) = 0;
           virtual void removeActionCursor ( void ) = 0;
           virtual void updateDashboard () = 0;
           virtual void repaintDisplay () = 0;
           virtual ~MapDisplayInterface () {};
       };

class MapDisplay : public MapDisplayInterface {
           dynamic_array<int> cursorstat;
           int cursorstatnum;
         public:
           int displayMovingUnit ( const MapCoordinate3D& start, const MapCoordinate3D& dest, Vehicle* vehicle, int fieldnum, int totalmove, SoundStartCallback soundStart );
           void displayMap ( void );
           void displayMap ( Vehicle* additionalVehicle ) {};
           void displayPosition ( int x, int y );
           void resetMovement ( void );
           void startAction ( void );
           void stopAction ( void );
           void displayActionCursor ( int x1, int y1, int x2, int y2 ) {};
           void removeActionCursor ( void ) {};
           void updateDashboard ();
           void repaintDisplay ();
    };


extern MapDisplayInterface& getDefaultMapDisplay();

class tlockdispspfld {
      public:
        tlockdispspfld ( void );
        ~tlockdispspfld ();
      };



//! Determines if fields that have a temp value != 0 are being marked when displaying the map
extern bool tempsvisible;

extern int showresources;

#endif
