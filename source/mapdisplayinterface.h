
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef mapdisplayinterfaceH
 #define mapdisplayinterfaceH


 #include "loki/Functor.h"

class Sound;
 
#include "typen.h"
#include "vehicle.h"
#include "attack.h"


class MapDisplayInterface {
         public:
           typedef Loki::Functor<void, LOKI_TYPELIST_1(int) > SoundStartCallback; 
           virtual int displayMovingUnit ( const MapCoordinate3D& start, const MapCoordinate3D& dest, Vehicle* vehicle, int fieldnum, int totalmove, SoundStartCallback startSound, int duration ) = 0;
           virtual void displayMap ( void ) = 0;
           virtual void displayMap ( Vehicle* additionalVehicle ) = 0;
           virtual void displayPosition ( int x, int y ) = 0;
           void displayPosition ( const MapCoordinate& pos ) { displayPosition(pos.x,pos.y ); };
           virtual void resetMovement ( void ) = 0;
           virtual void startAction ( void ) = 0;
           virtual void stopAction ( void ) = 0;
           virtual void cursor_goto ( const MapCoordinate& pos ) = 0;
           virtual void displayActionCursor ( int x1, int y1, int x2, int y2 ) = 0;
           virtual void displayActionCursor ( const MapCoordinate& pos1, const MapCoordinate& pos2 ) { displayActionCursor(pos1.x,pos1.y, pos2.x,pos2.y); };
           virtual void removeActionCursor ( void ) = 0;
           virtual void updateDashboard () = 0;
           virtual void repaintDisplay () = 0;
           virtual void setTempView( bool view ) = 0;
           virtual void showBattle( tfight& battle ) = 0;
           virtual void playPositionalSound( const MapCoordinate& pos, Sound* snd ) = 0;
           virtual int getUnitMovementDuration() const = 0;
           virtual ~MapDisplayInterface () {};
       };

       extern MapDisplayInterface& getDefaultMapDisplay();

       
#endif
