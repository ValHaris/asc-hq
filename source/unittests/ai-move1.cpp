/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <iostream>

#include "ai-move1.h"
#include "../loaders.h"
#include "unittestutil.h"
#include "../turncontrol.h"
#include "../gamemap.h"

void testAiMovement1() 
{
   auto_ptr<GameMap> game ( startMap("unittest-ai-simpleattack.map"));
   
   Vehicle* veh = game->getField(3,9)->vehicle;
   
   assertOrThrow( veh != NULL );
   assertOrThrow( veh->damage == 0 );
   
   next_turn( game.get(), NextTurnStrategy_Abort(), NULL, -1 );
   
   assertOrThrow( veh->damage > 0 );
}


void testAiMovement2() 
{
   
   auto_ptr<GameMap> game ( startMap("unittest-ai-mam.map"));
   
   Vehicle* veh = game->getField(3,9)->vehicle;
   
   assertOrThrow( veh != NULL );
   assertOrThrow( veh->damage == 0 );
   
   next_turn( game.get(), NextTurnStrategy_Abort(), NULL, -1 );
   
   /*  a single unit will not make this much damage.
       The point of this test is that the AI should move MoveAttackMove - capable units 
       out of the way to continue attacking with further units */
   
   assertOrThrow( veh->damage >= 35 );
}

void testAiMovement3() 
{
   
   auto_ptr<GameMap> game ( startMap("unittest-ai-hemming.map"));
   
   Vehicle* veh = game->getField(3,9)->vehicle;
   
   assertOrThrow( veh != NULL );
   assertOrThrow( veh->damage == 0 );
   
   next_turn( game.get(), NextTurnStrategy_Abort(), NULL, -1 );
   
   /*  a single unit will not make this much damage.
   The point of this test is that the AI should move MoveAttackMove - capable units 
   out of the way to continue attacking with further units */
   
   assertOrThrow( veh->damage >= 64 );
}


/** the AI should disembark all helicopters from the carrier and attack with them.
    The carrier itself is not capable to attack the submarine */
void testAiMovement4() 
{
   
   auto_ptr<GameMap> game ( startMap("unittest-ai-carrier.map"));
   
   Vehicle* veh = game->getField(2,7)->vehicle;
   
   assertOrThrow( veh != NULL );
   assertOrThrow( veh->damage == 0 );
   
   next_turn( game.get(), NextTurnStrategy_Abort(), NULL, -1 );
   
   assertOrThrow( veh->damage >= 70 );
}


/** the trooper should conquer the building , but try to evade the view of the
    player (if he steps into visiblity, the Artillery will attack) */
void testAiMovement5() 
{
   
   auto_ptr<GameMap> game ( startMap("unittest-ai-conquer.map"));
   
   Vehicle* veh = game->getField(8,4)->vehicle;
   Building* bld = game->getField( 14,9 )->building;
   
   assertOrThrow( veh != NULL );
   assertOrThrow( veh->damage == 0 );
   
   assertOrThrow( bld != NULL );
   assertOrThrow( bld->getOwner() == 0 );
   
   next_turn( game.get(), NextTurnStrategy_Abort(), NULL, -1 );
   next_turn( game.get(), NextTurnStrategy_Abort(), NULL, -1 );
   next_turn( game.get(), NextTurnStrategy_Abort(), NULL, -1 );
   next_turn( game.get(), NextTurnStrategy_Abort(), NULL, -1 );
   
   assertOrThrow( bld->getOwner() == 1 );
}


/** the trooper should not conquer this building, because it would immediately be 
    conquered back*/
void testAiMovement6() 
{
   
   auto_ptr<GameMap> game ( startMap("unittest-ai-conquer2.map"));
   
   Vehicle* veh = game->getField(3,5)->vehicle;
   Building* bld = game->getField(4,8)->building;
   
   assertOrThrow( veh != NULL );
   
   assertOrThrow( bld != NULL );
   assertOrThrow( bld->getOwner() == 0 );
   
   next_turn( game.get(), NextTurnStrategy_Abort(), NULL, -1 );
   
   assertOrThrow( bld->getOwner() == 0 );
}


/** the trooper should conquer this building, because the contents
    of tte building are more worth than the lost unit*/
void testAiMovement7() 
{
   auto_ptr<GameMap> game ( startMap("unittest-ai-conquer3.map"));
   
   Vehicle* veh = game->getField(3,5)->vehicle;
   Building* bld = game->getField(4,8)->building;
   
   assertOrThrow( veh != NULL );
   
   assertOrThrow( bld != NULL );
   assertOrThrow( bld->getOwner() == 0 );
   
   next_turn( game.get(), NextTurnStrategy_Abort(), NULL, -1 );
   
   assertOrThrow( bld->getOwner() == 1 );
}

/** the helicopter should attack from a height where the tank can't retaliate*/
void testAiHeliMovement1()
{
   auto_ptr<GameMap> game ( startMap("unittest-ai-heli.map"));
   
   Vehicle* heli = game->getField(7,10)->vehicle;
   
   assertOrThrow( heli != NULL );
   
   next_turn( game.get(), NextTurnStrategy_Abort(), NULL, -1 );
   next_turn( game.get(), NextTurnStrategy_Abort(), NULL, -1 );
   
   // at least one attack
   assertOrThrow( heli->getExperience_offensive() >= 1  );
   
   // attacked from flying level
   assertOrThrow( heli->damage == 0  );
  
}

void testAiMovement() 
{
   testAiMovement1();
   testAiMovement2();
   testAiMovement3();
   testAiMovement4();
   testAiMovement5();
   testAiMovement6();
   testAiMovement7();
   testAiHeliMovement1();
}
