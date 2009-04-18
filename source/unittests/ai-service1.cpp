/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <iostream>

#include "ai-service1.h"
#include "../loaders.h"
#include "unittestutil.h"
#include "../turncontrol.h"
#include "../gamemap.h"

void testAiService1() 
{
   auto_ptr<GameMap> game ( startMap("unittest-ai-refuel1.map"));
   
   Vehicle* veh = game->getField(2,10)->vehicle;
   
   assertOrThrow( veh != NULL );
   assertOrThrow( veh->damage == 0 );
   
   next_turn( game.get(), NextTurnStrategy_Abort(), NULL, -1 );
   
   assertOrThrow( veh->damage > 0 );
   int damage = veh->damage;
   
   /* the ammo of the attacking unit is exhausted. 
      The AI should service it with the service unit nearby */
   next_turn( game.get(), NextTurnStrategy_Abort(), NULL, -1 );
   next_turn( game.get(), NextTurnStrategy_Abort(), NULL, -1 );
   
   assertOrThrow( veh->damage > damage );
   
}


void testAiService2() 
{
   auto_ptr<GameMap> game ( startMap("unittest-ai-refuel2.map"));
   
   Vehicle* veh = game->getField(4,6)->vehicle;
   
   assertOrThrow( veh != NULL );
   assertOrThrow( veh->getResource( Resources(0,0,100)).fuel <= 1 );
   
   next_turn( game.get(), NextTurnStrategy_Abort(), NULL, -1 );
   next_turn( game.get(), NextTurnStrategy_Abort(), NULL, -1 );
   
   assertOrThrow( veh->getResource( Resources(0,0,100)).fuel >= 100 );
   
}


void testAiService() 
{
   testAiService1();
   testAiService2();
}
