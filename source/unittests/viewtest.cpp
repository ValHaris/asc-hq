/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "../actions/moveunitcommand.h"
#include "../actions/attackcommand.h"
#include "../loaders.h"
#include "../viewcalculation.h"
#include "../spfst.h"
#include "unittestutil.h"


void testView() 
{
   auto_ptr<GameMap> game ( startMap("unittest-view1.map"));
   
   Vehicle* radar = game->getField(0,4)->vehicle;
   
   assertOrThrow( fieldvisiblenow(game->getField(3,5)) == false );
   
   move( radar, MapCoordinate(1,4));
  
   assertOrThrow( fieldvisiblenow(game->getField(3,5)) == true );
   
   assertOrThrow( fieldvisiblenow(game->getField(4,3)) == false );
   
   attack( game->getField(1,6)->vehicle, MapCoordinate( 3,5 ) );
   
   assertOrThrow( fieldvisiblenow(game->getField(4,3)) == true );
   
   game->actions.undo( createTestingContext( game.get() ) );  
   
   assertOrThrow( fieldvisiblenow(game->getField(4,3)) == false );
   assertOrThrow( fieldvisiblenow(game->getField(3,5)) == true );
   
   game->actions.undo( createTestingContext( game.get() ) );  
   
   assertOrThrow( fieldvisiblenow(game->getField(3,5)) == false );
   
   
   // now we are testing the view of the BLUE player
   
   assertOrThrow( fieldvisiblenow(game->getField(0,15), 1) == true );
   attack( game->getField(2,16)->vehicle, MapCoordinate( 0,13 ) );
   
   assertOrThrow( fieldvisiblenow(game->getField(0,15), 1) == false );
   
   game->actions.undo( createTestingContext( game.get() ) );  
   
   assertOrThrow( fieldvisiblenow(game->getField(0,15), 1) == true );
}
