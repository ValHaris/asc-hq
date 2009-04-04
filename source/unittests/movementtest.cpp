/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "../actions/moveunitcommand.h"
#include "../loaders.h"
#include "unittestutil.h"


void testCargoMovement( Vehicle* veh, int movement )
{
   for ( Vehicle::Cargo::const_iterator i = veh->getCargo().begin(); i != veh->getCargo().end(); ++i )
      if ( *i ) {
         Vehicle* cargo1 = *i;
         assertOrThrow( cargo1->getMovement() == movement );
         
         testCargoMovement( cargo1, movement );
      }
}

void testCargoMovementMax( Vehicle* veh, int movement )
{
   for ( Vehicle::Cargo::const_iterator i = veh->getCargo().begin(); i != veh->getCargo().end(); ++i )
      if ( *i ) {
         Vehicle* cargo1 = *i;
         assertOrThrow( cargo1->getMovement() <= movement );
            
         testCargoMovementMax( cargo1, movement );
      }
}

void move( Vehicle* veh, const MapCoordinate& dest )
{
   auto_ptr<MoveUnitCommand> muc ( new MoveUnitCommand( veh ));
   muc->setDestination( dest );
   ActionResult res = muc->execute( createTestingContext( veh->getMap() ));
   if ( res.successful() )
      muc.release();
   else
      throw ActionResult(res);
}

void testMovement() 
{
   auto_ptr<GameMap> game ( startMap("unittest-movement.map"));
   
   Vehicle* veh = game->getField(0,0)->vehicle;
   assertOrThrow( veh->getMovement() == 100 );
   
   move( veh, MapCoordinate(5,10));
   
   assertOrThrow( veh->getMovement() == 0 );
   testCargoMovement( veh, 50 );
      
   game->actions.undo( createTestingContext( game.get() ) );  
      
   assertOrThrow( veh->getMovement() == 100 );
   testCargoMovement( veh, 100 );
   
   move(veh, MapCoordinate(0,1));   
   move(veh, MapCoordinate(1,2));   
   move(veh, MapCoordinate(1,3));   
   move(veh, MapCoordinate(2,4));   
   move(veh, MapCoordinate(2,5));   
   move(veh, MapCoordinate(3,6));   
   move(veh, MapCoordinate(3,7));   
   move(veh, MapCoordinate(4,8));   
   move(veh, MapCoordinate(4,9));   
   move(veh, MapCoordinate(5,10));   
   
   assertOrThrow( veh->getMovement() == 0 );
   testCargoMovement( veh, 50 );
   
}