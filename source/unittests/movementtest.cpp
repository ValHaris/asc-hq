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


void testMovement1() 
{
   auto_ptr<GameMap> game ( startMap("unittest-movement.map"));
   
   Vehicle* veh = game->getField(0,0)->vehicle;
   assertOrThrow( veh->getMovement() == 100 );
   
   move( veh, MapCoordinate(5,10));
   
   assertOrThrow( veh->getMovement() == 0 );
   testCargoMovement( veh, 50 );
      
   ActionResult res = game->actions.undo( createTestingContext( game.get() ) );  
   assertOrThrow( res.successful() );
      
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

void testMovementRF() 
{
   auto_ptr<GameMap> game ( startMap("unittest-reactionfire.map"));
   
   Vehicle* veh = game->getField(4,2)->vehicle;
   assertOrThrow( veh != NULL );
   
   move( veh, MapCoordinate(4,3));
   
   assertOrThrow( veh->damage == 0 );
   
   move( veh, MapCoordinate(5,4));
   
   assertOrThrow( veh->damage > 50 );
}

void testMovementTracks() 
{
   auto_ptr<GameMap> game ( startMap("unittest-objectgeneration.map"));
   
   Vehicle* veh = game->getField(4,9)->vehicle;
   assertOrThrow( veh != NULL );
   
   move( veh, MapCoordinate(6,12));
   
   ObjectType* track = objectTypeRepository.getObject_byID( 7 );
   
   assertOrThrow( game->getField(4,9)->checkforobject( track) != NULL );
   assertOrThrow( game->getField(5,10)->checkforobject( track) == NULL );
   
   ActionResult res = game->actions.undo( createTestingContext( game.get() ) );  
   
   assertOrThrow( res.successful() );
   
   assertOrThrow( game->getField(4,9)->checkforobject( track) == NULL );
   assertOrThrow( game->getField(5,10)->checkforobject( track) == NULL );
  
}

void testHeightChangeAI()
{
   auto_ptr<GameMap> game ( startMap("unittest-heightchange.map"));
   Vehicle* veh = game->getField(4,17)->vehicle;
   assertOrThrow( veh != NULL );
   
   auto_ptr<MoveUnitCommand> muc ( new MoveUnitCommand( veh ));
   
   // this is the AI way of doing things, selecting a 3D coordinate as destination
   
   MapCoordinate3D dest( 6,10,1<<5 );
   assertOrThrow( muc->isFieldReachable3D( dest, true ));
   
   muc->setDestination( dest );
   ActionResult res = muc->execute( createTestingContext( veh->getMap() ));
   if ( res.successful() )
      muc.release();
   else
      throw ActionResult(res);
   
   assertOrThrow( veh->height == 32 );
}

void testHeightChangeGUI()
{
   auto_ptr<GameMap> game ( startMap("unittest-heightchange.map"));
   Vehicle* veh = game->getField(4,17)->vehicle;
   assertOrThrow( veh != NULL );
   
   auto_ptr<MoveUnitCommand> muc ( new MoveUnitCommand( veh ));
   
   // this is the GUI way of doing things, selecting a 2D coordinate as destination
   
   MapCoordinate dest( 6,10 );
   muc->searchFields();
   assertOrThrow( muc->isFieldReachable( dest, true ));
   
   muc->setDestination( dest );
   ActionResult res = muc->execute( createTestingContext( veh->getMap() ));
   if ( res.successful() )
      muc.release();
   else
      throw ActionResult(res);
   
   assertOrThrow( veh->height == 16 ); // because low level flight can be reached with less movement consumption
   
}


void testMovement() 
{
   testMovement1();
   testMovementRF();
   testMovementTracks();
   testHeightChangeAI();
   testHeightChangeGUI();
}
