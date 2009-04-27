/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "../gamemap.h"
#include "../actions/recycleunitcommand.h"
#include "../loaders.h"
#include "recyclingtest.h"
#include "unittestutil.h"
#include "spfst.h"



void testRecycling() 
{
   auto_ptr<GameMap> game ( startMap("unittest-recycle.map"));
   
   Building* bld = game->getField(5,8)->building;
   Vehicle* veh = getFirstCargo( bld );
   assertOrThrow( veh != NULL );
   assertOrThrow( bld != NULL );
   
   RecycleUnitCommand* ruc = new RecycleUnitCommand( bld );
   ruc->setUnit( veh );
   ActionResult res = ruc->execute( createTestingContext( game.get() ));
   assertOrThrow( res.successful() );
   
}
