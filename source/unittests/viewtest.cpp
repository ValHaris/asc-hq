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


void testView3() 
{
   auto_ptr<GameMap> game ( startMap("unittest-jammingdestroyview.map"));
   
   Vehicle* ari = game->getField(5,2)->vehicle;
   
   MapField* field = game->getField(0,12);
   MapField* field2 = game->getField(3,10);
   
   
   assertOrThrow( fieldVisibility(field) == visible_not );
   assertOrThrow( fieldVisibility(field2) == visible_not );
   
   attack( ari, MapCoordinate(3,8));
  
   assertOrThrow( fieldVisibility(field) == visible_not );
   assertOrThrow( fieldVisibility(field2) == visible_now );

   ActionResult res = game->actions.undo( createTestingContext( game.get() ) );  
   assertOrThrow( res.successful() );
   
   assertOrThrow( fieldVisibility(field) == visible_not );
   assertOrThrow( fieldVisibility(field2) == visible_not );
   
}

void testView1() 
{
   auto_ptr<GameMap> game ( startMap("unittest-view1.map"));
   
   Vehicle* radar = game->getField(0,4)->vehicle;
   
   assertOrThrow( fieldvisiblenow(game->getField(3,5)) == false );
   
   move( radar, MapCoordinate(1,4));
  
   assertOrThrow( fieldvisiblenow(game->getField(3,5)) == true );
   
   assertOrThrow( fieldvisiblenow(game->getField(4,3)) == false );
   
   attack( game->getField(1,6)->vehicle, MapCoordinate( 3,5 ) );
   
   assertOrThrow( game->getField(3,5)->vehicle == NULL );
   assertOrThrow( fieldvisiblenow(game->getField(4,3)) == true );
   
   ActionResult res = game->actions.undo( createTestingContext( game.get() ) );  
   assertOrThrow( res.successful() );
   
   assertOrThrow( fieldvisiblenow(game->getField(4,3)) == false );
   assertOrThrow( fieldvisiblenow(game->getField(3,5)) == true );
   
   res = game->actions.undo( createTestingContext( game.get() ) );  
   assertOrThrow( res.successful() );
   
   assertOrThrow( fieldvisiblenow(game->getField(3,5)) == false );
   
   
   // now we are testing the view of the BLUE player
   
   assertOrThrow( fieldvisiblenow(game->getField(0,15), 1) == true );
   attack( game->getField(2,16)->vehicle, MapCoordinate( 0,13 ) );
   
   assertOrThrow( fieldvisiblenow(game->getField(0,15), 1) == false );
   
   res = game->actions.undo( createTestingContext( game.get() ) );  
   assertOrThrow( res.successful() );
   
   assertOrThrow( fieldvisiblenow(game->getField(0,15), 1) == true );
}


void testView2() 
{
   auto_ptr<GameMap> game ( startMap("unittest-view2.map"));
   
   Vehicle* spyplane = game->getField(5,2)->vehicle;
   
   assertOrThrow( spyplane != NULL );
   assertOrThrow( spyplane->damage == 0 );
   
   assertOrThrow( fieldvisiblenow(game->getField(5,2),1 ) == false );
   
   move( spyplane, MapCoordinate(5,18));
  
   spyplane = game->getField(5,18)->vehicle;
   assertOrThrow( spyplane != NULL );
   assertOrThrow( spyplane->damage == 0 );
}


void testView()
{
   testView1();
   testView2();
   testView3();
} 
