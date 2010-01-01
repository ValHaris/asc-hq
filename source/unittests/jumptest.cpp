/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <iostream>

#include "../actions/jumpdrivecommand.h"
#include "../loaders.h"
#include "../itemrepository.h"
#include "unittestutil.h"
#include "../gamemap.h"


void testJumpdrive() 
{
   auto_ptr<GameMap> game ( startMap("testjump.map"));
   
   Vehicle* veh = game->getField(5,14)->vehicle;
   assertOrThrow( veh != NULL );
   assertOrThrow( veh->damage == 0 );
   
   JumpDriveCommand* jdc = new JumpDriveCommand( veh );
   
   jdc->setDestination( MapCoordinate( 5, 4));
   ActionResult res = jdc->execute( createTestingContext( game.get() ));
   
   assertOrThrow( veh->attacked );
   assertOrThrow( veh->damage > 0 );
   
}

