/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <iostream>

#include "../actions/moveunitcommand.h"
#include "../loaders.h"
#include "../itemrepository.h"
#include "unittestutil.h"
#include "../tasks/taskcontainer.h"

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
   
   assertOrThrow( game->getField(4,9)->checkForObject( track) != NULL );
   assertOrThrow( game->getField(5,10)->checkForObject( track) == NULL );
   
   ActionResult res = game->actions.undo( createTestingContext( game.get() ) );  
   
   assertOrThrow( res.successful() );
   
   assertOrThrow( game->getField(4,9)->checkForObject( track) == NULL );
   assertOrThrow( game->getField(5,10)->checkForObject( track) == NULL );
  
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


void testMovementFieldsReachable()
{
   auto_ptr<GameMap> game ( startMap("unittest-moveback.map"));
   Vehicle* veh = game->getField(15,31)->vehicle;
   assertOrThrow( veh != NULL );
   
   auto_ptr<MoveUnitCommand> muc ( new MoveUnitCommand( veh ));
   
   muc->searchFields();
   const set<MapCoordinate3D>& fields = muc->getReachableFields();
   
   for ( set<MapCoordinate3D>::const_iterator i = fields.begin(); i != fields.end(); ++i ) {
      auto_ptr<MoveUnitCommand> m2 ( new MoveUnitCommand( veh ));
      m2->setDestination( *i );
      ActionResult res = m2->execute( createTestingContext( veh->getMap() ));
      assertOrThrow( res.successful() );
      m2.release();
      
      res = game->actions.undo( createTestingContext( game.get() ));
      assertOrThrow( res.successful() );
   }
}


static void runTasks( GameMap* gamemap )
{
   if ( gamemap->tasks ) {
      TaskContainer* tc = dynamic_cast<TaskContainer*>( gamemap->tasks );
      if ( tc ) {
         while ( tc->pendingCommands.begin() != tc->pendingCommands.end() ) {
            Command* c = tc->pendingCommands.front();
            TaskInterface* ti = dynamic_cast<TaskInterface*>( c );
            
            if ( ti->operatable() ) {
               ActionResult res = c->execute( createTestingContext( gamemap ));
               if ( !res.successful() )
                  throw res;
            }
            
            tc->pendingCommands.erase( tc->pendingCommands.begin() );
         }
      }
   }
}


static void moveUnitTest( GameMap* game, Vehicle* veh, const MapCoordinate& destination, int turns )
{
   assertOrThrow( veh != NULL );
   assertOrThrow( game != NULL );
   auto_ptr<MoveUnitCommand> muc ( new MoveUnitCommand( veh ));
   muc->searchFields();
   muc->setDestination( destination );
   ActionResult res = muc->execute( createTestingContext( veh->getMap() ));
   if ( res.successful() )
      muc.release();
   else
      throw ActionResult(res);
   
   for ( int i = 0; i < turns; ++i ) {
      next_turn( game, NextTurnStrategy_Abort(), NULL, -1 );
      runTasks( game );
   }
   
   assertOrThrow( destination == veh->getPosition() );
   
   TaskContainer* tc = dynamic_cast<TaskContainer*>( game->tasks );
   assertOrThrow( tc != NULL );
   
   assertOrThrow( tc->pendingCommands.begin() == tc->pendingCommands.end() );

}

void testLongDistMovement()
{
   auto_ptr<GameMap> game ( startMap("unittest-moveland.map"));
   Vehicle* veh;
   MapCoordinate airport(0,7);
   MapCoordinate carrier(0,0);
   
   veh = game->getField(9,18)->vehicle;
   moveUnitTest(  game.get(), veh, airport, 1 );
   
   veh = game->getField(9,19)->vehicle;
   moveUnitTest(  game.get(), veh, carrier, 1 );
   
   veh = game->getField(9,17)->vehicle;
   moveUnitTest(  game.get(), veh, airport, 3 );
}


void testMapResizeWithMovement()
{
   auto_ptr<GameMap> game ( startMap("unittest-moveland.map"));
   Vehicle* veh = game->getField(9,16)->vehicle;
  
   assertOrThrow( veh != NULL );
   
   auto_ptr<MoveUnitCommand> muc ( new MoveUnitCommand( veh ));
   muc->searchFields();
   muc->setDestination( MapCoordinate(3,2) );
   ActionResult res = muc->execute( createTestingContext( veh->getMap() ));
   if ( res.successful() )
      muc.release();
   else
      throw ActionResult(res);
   
   game->resize(10,10,10,10);
   
   next_turn( game.get(), NextTurnStrategy_Abort(), NULL, -1 );
   runTasks( game.get() );
   
   next_turn( game.get(), NextTurnStrategy_Abort(), NULL, -1 );
   runTasks( game.get() );
   
   assertOrThrow( MapCoordinate( 13,12 ) == veh->getPosition() );
  
}

void testPathFinding()
{
   auto_ptr<GameMap> game ( startMap("unittest-pathfinding.map"));
   Vehicle* buggy = game->getField(12,19)->vehicle;
   assertOrThrow( buggy );

   {
	   auto_ptr<MoveUnitCommand> muc ( new MoveUnitCommand( buggy ));
	   muc->searchFields();
	   //cout <<  muc->getReachableFields().size() << "\n";
	   assertOrThrow(  muc->getReachableFields().size() == 157 );

	   for ( set<MapCoordinate3D>::iterator i = muc->getReachableFields().begin(); i != muc->getReachableFields().end(); ++i ) {
		   muc->setDestination( *i );
		   muc->calcPath();
		   assertOrThrow( !muc->getPath().empty() );
	   }
   }



   {
	   Vehicle* sub = game->getField(0,15)->vehicle;
	   assertOrThrow( sub );

	   AStar3D ast( game.get(), sub, false, sub->getMovement());
	   vector<MapCoordinate3D> fields;
	   ast.findAllAccessibleFields( &fields );

	   // make entries unique - there shouldn't be any duplicates, but at the time of writing this test
	   // findAllAccessibleFields does return some duplicates
	   set<MapCoordinate3D> s( fields.begin(), fields.end() );

	   /*
	   cout << "s.size: " << s.size() << "\n";
	   for ( vector<MapCoordinate3D>::iterator i = fields.begin(); i!= fields.end(); ++i ) {
		   cout << "assertOrThrow ( s.find( MapCoordinate3D(" << i->x << "," << i->y << "," << i->getBitmappedHeight() << ")) != s.end() );\n";
	   }
	   */


	   assertOrThrow( s.size() == 16 );
	   // activate this test once the duplicate-bug is fixed
	   // assertOrThrow( s.size() == fields.size() );

	   assertOrThrow ( s.find( MapCoordinate3D(0,15,4)) != s.end() );
	   assertOrThrow ( s.find( MapCoordinate3D(1,16,4)) != s.end() );
	   assertOrThrow ( s.find( MapCoordinate3D(0,14,4)) != s.end() );
	   assertOrThrow ( s.find( MapCoordinate3D(1,16,2)) != s.end() );
	   assertOrThrow ( s.find( MapCoordinate3D(1,17,2)) != s.end() );
	   assertOrThrow ( s.find( MapCoordinate3D(2,18,2)) != s.end() );
	   assertOrThrow ( s.find( MapCoordinate3D(2,18,4)) != s.end() );
	   assertOrThrow ( s.find( MapCoordinate3D(2,19,4)) != s.end() );
	   assertOrThrow ( s.find( MapCoordinate3D(2,19,2)) != s.end() );
	   assertOrThrow ( s.find( MapCoordinate3D(3,20,2)) != s.end() );
	   assertOrThrow ( s.find( MapCoordinate3D(3,18,2)) != s.end() );
	   assertOrThrow ( s.find( MapCoordinate3D(2,21,2)) != s.end() );
	   assertOrThrow ( s.find( MapCoordinate3D(1,16,1)) != s.end() );
	   assertOrThrow ( s.find( MapCoordinate3D(2,18,1)) != s.end() );
	   assertOrThrow ( s.find( MapCoordinate3D(1,17,1)) != s.end() );
	   assertOrThrow ( s.find( MapCoordinate3D(2,19,1)) != s.end() );

	   // test the test :-)
	   assertOrThrow ( s.find( MapCoordinate3D(99,16,1)) == s.end() );

	   AStar3D::Path path;
	   AStar3D ast2( game.get(), sub, false, sub->getMovement());
	   ast2.findPath(path, MapCoordinate3D(2,19,4));

	   /*
	   cout << path.size() << "\n";
	   int cnt = 0;
	   for ( AStar3D::Path::iterator i = path.begin(); i != path.end(); ++i) {
		   cout << "assertOrThrow( path["<< cnt++ <<"] == MapCoordinate3D(" << i->x << "," << i->y << "," << i->getBitmappedHeight() << "));\n";
	   }
	   */


	   assertOrThrow( path.size() == 7 );
	   assertOrThrow( path[0] == MapCoordinate3D(0,15,4));
	   assertOrThrow( path[1] == MapCoordinate3D(1,16,4));
	   assertOrThrow( path[2] == MapCoordinate3D(1,16,2));
	   assertOrThrow( path[3] == MapCoordinate3D(1,17,2));
	   assertOrThrow( path[4] == MapCoordinate3D(2,18,2));
	   assertOrThrow( path[5] == MapCoordinate3D(2,19,2));
	   assertOrThrow( path[6] == MapCoordinate3D(2,19,4));


	   move( sub, MapCoordinate3D(2,19,4));

	   assertOrThrow( sub->getMovement() == 19 );

   }

   {

	   Vehicle* air = game->getField(19,2)->vehicle ;
	   assertOrThrow( air );

	   AStar3D ast( game.get(), air, false, air->getMovement()*2);
	   AStar3D::Path path;
	   ast.findPath(path, MapCoordinate3D(19,12,8));

	   assertOrThrow( path.size() == 6 );

	   assertOrThrow( path[0] == MapCoordinate3D( 19, 2,    8) );
	   assertOrThrow( path[1] == MapCoordinate3D( 18, 4, 0x10) );
	   assertOrThrow( path[2] == MapCoordinate3D( 18, 5, 0x10) );
	   assertOrThrow( path[3] == MapCoordinate3D( 19, 6, 0x10) );
	   assertOrThrow( path[4] == MapCoordinate3D( 19, 8, 0x10) );
	   assertOrThrow( path[5] == MapCoordinate3D( 19,12,    8) );


	   vector<MapCoordinate3D> fields;
	   ast.findAllAccessibleFields(&fields);

	   assertOrThrow( fields.size() == 31 );
   }

}



void testMovement() 
{
   testMapResizeWithMovement();
   testLongDistMovement();
   testMovementFieldsReachable();
   testMovement1();
   testMovementRF();
   testMovementTracks();
   testHeightChangeAI();
   testHeightChangeGUI();
   testPathFinding();
}
