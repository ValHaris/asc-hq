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
#include "../itemrepository.h"
#include "../spfst.h"


void testAttack1() 
{
   auto_ptr<GameMap> game ( startMap("unittest-attack.map"));
   
   Vehicle* veh = game->getField(3,6)->vehicle;
   assertOrThrow( veh != NULL );
   assertOrThrow( veh->getMovement() == 100 );
   
   attack( veh, MapCoordinate( 3,5) );
   
   assertOrThrow( veh->getMovement() == 0 );
   
   testCargoMovement( veh, 50 );
   
   ActionResult res = game->actions.undo( createTestingContext( game.get() ) );  
   assertOrThrow( res.successful() );
   
   assertOrThrow( veh->getMovement() == 100 );
   testCargoMovement( veh, 100 );
   
   
   Vehicle* mam = game->getField(4,4)->vehicle;
   assertOrThrow( mam != NULL );
   assertOrThrow( mam->getMovement() == 100 );
   
   attack( mam, MapCoordinate( 3,5) );
   
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

void testAttack2()
{
   auto_ptr<GameMap> game ( startMap("unittest-objectattack.map"));
   
   Vehicle* buggy = game->getField(0,3)->vehicle;
   assertOrThrow( buggy != NULL );
   
   Vehicle* assault = game->getField(1,2)->vehicle;
   assertOrThrow( assault != NULL );
   
   MapCoordinate h ( 3, 8 );
   MapCoordinate cry( 3,9);
   
   move( buggy, h);
   attack( buggy, cry );
   move( buggy, MapCoordinate( 0,3 ));
   
   move( assault, h );
   
   ObjectType* cryst = objectTypeRepository.getObject_byID( 2105 );
   assertOrThrow( cryst != NULL );
   
   MapField* fld = game->getField( cry );   
   assertOrThrow( fld->checkForObject( cryst ) != NULL );
   
   MapField* view = game->getField( MapCoordinate( 4,11 ));
   assertOrThrow( fieldvisiblenow(view) == false ); 
   
   attack( assault, cry );
   
   assertOrThrow( fld->checkForObject( cryst ) == NULL );
   
   assertOrThrow( fieldvisiblenow(view) == true ); 
   
   ActionResult res = game->actions.undo( createTestingContext( game.get() ) );
   assertOrThrow( res.successful() );
   
   assertOrThrow( fld->checkForObject( cryst ) != NULL );
   assertOrThrow( fieldvisiblenow(view) == false ); 
   
   assertOrThrow ( game->actions.undo( createTestingContext( game.get() ) ).successful() ) ;
   assertOrThrow ( game->actions.undo( createTestingContext( game.get() ) ).successful() ) ;
   assertOrThrow ( game->actions.undo( createTestingContext( game.get() ) ).successful() ) ;
   assertOrThrow ( game->actions.undo( createTestingContext( game.get() ) ).successful() ) ;
   
} 

      
void testAttack() {
   testAttack2();
   testAttack1();
}
