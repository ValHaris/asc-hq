/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef unittestutilH
#define unittestutilH

#include "../ascstring.h"
#include "../errors.h"
#include "../actions/context.h"
#include "../vehicle.h"
#include "../turncontrol.h"

extern GameMap* startMap( const ASCString& filename );

class TestFailure : public ASCmsgException {
   public:
      TestFailure( const ASCString& message ) : ASCmsgException (message ){
      }
};

extern Context createTestingContext( GameMap* gamemap );
extern void testCargoMovement( Vehicle* veh, int movement );
extern void testCargoMovementMax( Vehicle* veh, int movement );
extern Vehicle* getFirstCargo( ContainerBase* carrier );
extern void move( Vehicle* veh, const MapCoordinate& dest );
extern void move( Vehicle* veh, const MapCoordinate3D& dest );
extern void attack( Vehicle* veh, const MapCoordinate& target );


#endif
