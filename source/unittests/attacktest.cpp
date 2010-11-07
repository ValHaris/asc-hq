/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "../gamemap.h"
#include "../actions/attackcommand.h"
#include "../loaders.h"
#include "unittestutil.h"




void testAttack() 
{
   auto_ptr<GameMap> game ( startMap("unittest-attack.map"));
   
   Vehicle* veh = game->getField(3,6)->vehicle;
   assertOrThrow( veh != NULL );
   assertOrThrow( veh->getMovement() == 100 );
   
   assertOrThrow( veh->experience_offensive == 0 );
   assertOrThrow( veh->experience_defensive == 0 );
   
   attack( veh, MapCoordinate( 3,5) );
   
   assertOrThrow( veh->getMovement() == 0 );
   assertOrThrow( veh->experience_offensive == 2 );
   assertOrThrow( veh->experience_defensive == 1 );
   
   testCargoMovement( veh, 50 );
   
   ActionResult res = game->actions.undo( createTestingContext( game.get() ) );  
   assertOrThrow( res.successful() );
   
   assertOrThrow( veh->getMovement() == 100 );
   testCargoMovement( veh, 100 );
   
   
   Vehicle* mam = game->getField(4,4)->vehicle;
   assertOrThrow( mam != NULL );
   assertOrThrow( mam->getMovement() == 100 );
   assertOrThrow( mam->experience_offensive == 0 );
   assertOrThrow( mam->experience_defensive == 0 );
   
   attack( mam, MapCoordinate( 3,5) );
   assertOrThrow( mam->experience_offensive == 1 );
   assertOrThrow( mam->experience_defensive == 0 );
   
   assertOrThrow( mam->getMovement() == 100 );
   testCargoMovement( mam, 100 );
   
   Vehicle* v2 = game->getField(9,14)->vehicle;
   assertOrThrow( v2 != NULL );
   
   Vehicle* a2 = game->getField(9,17)->vehicle;
   assertOrThrow( a2 != NULL );
   
   attack( a2, MapCoordinate( 9, 14 ) );
    
   assertOrThrow( game->getField(9,14)->vehicle == NULL );
   res = game->actions.undo( createTestingContext( game.get() ) );  
   assertOrThrow( res.successful() );
   assertOrThrow( game->getField(9,14)->vehicle != NULL );
   
   
}
