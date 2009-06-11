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
#include "../itemrepository.h"
#include "unittestutil.h"


void testLoseMap() 
{
   auto_ptr<GameMap> game ( startMap("unittest-eventlose.map"));
   
   Vehicle* veh = game->getField(5,4)->vehicle;
   assertOrThrow( veh != NULL );
   
   attack( veh, MapCoordinate( 6,6 ));
   
   next_turn( game.get(), NextTurnStrategy_Abort(), NULL, -1 );
   
   assertOrThrow ( game->actplayer == 2 );
   
}

void testEvents() 
{
  //  testLoseMap();
}
