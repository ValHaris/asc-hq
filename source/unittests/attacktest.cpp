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
   
   auto_ptr<AttackCommand> muc ( new AttackCommand( veh ));
   muc->setTarget( MapCoordinate( 3,5) );
   ActionResult res = muc->execute( createTestingContext( veh->getMap() ));
   if ( res.successful() )
      muc.release();
   else
      throw ActionResult(res);
   
   assertOrThrow( veh->getMovement() == 0 );
   
   testCargoMovement( veh, 50 );
   
   game->actions.undo( createTestingContext( game.get() ) );  
   
   assertOrThrow( veh->getMovement() == 100 );
   testCargoMovement( veh, 100 );
   
   
   Vehicle* mam = game->getField(4,4)->vehicle;
   assertOrThrow( mam != NULL );
   assertOrThrow( mam->getMovement() == 100 );
   
   auto_ptr<AttackCommand> muc2 ( new AttackCommand( mam ));
   muc2->setTarget( MapCoordinate( 3,5) );
   res = muc2->execute( createTestingContext( veh->getMap() ));
   if ( res.successful() )
      muc2.release();
   else
      throw ActionResult(res);
   
   assertOrThrow( mam->getMovement() == 100 );
   testCargoMovement( mam, 100 );
   
}
