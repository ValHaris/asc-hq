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



void testView() 
{
   auto_ptr<GameMap> game ( startMap("unittest-view1.map"));
   
   Vehicle* radar = game->getField(0,4)->vehicle;
   assertOrThrow( veh->getMovement() == 100 );
   
   assertOrThrow( fieldvisiblenow(game->getField(3,5)));
   
   move( radar, MapCoordinate(5,10));
  
   
}
