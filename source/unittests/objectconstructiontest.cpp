/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "../gamemap.h"
#include "../loaders.h"
#include "objectconstructiontest.h"
#include "unittestutil.h"
#include "spfst.h"
#include "../actions/spawnobject.h"
#include "itemrepository.h"


void testObjectConstruction() 
{
   auto_ptr<GameMap> game ( startMap("unittest-objectconstruction.map"));
   
   ObjectType* crystals = objectTypeRepository.getObject_byID( 2105 );
   assertOrThrow( crystals != NULL );
   
   ObjectType* road = objectTypeRepository.getObject_byID( 1 );
   assertOrThrow( road != NULL );
   
   MapCoordinate pos ( 4,8);
   MapField* fld = game->getField( pos );
   assertOrThrow( fld != NULL );
   
   assertOrThrow( fld->checkForObject( crystals ) != NULL );
   
   SpawnObject so ( game.get(), pos, 1 );
   ActionResult res = so.execute( createTestingContext( game.get() ));
   assertOrThrow( res.successful() );
   
   assertOrThrow( fld->checkForObject( crystals ) == NULL );
   assertOrThrow( fld->checkForObject( road ) != NULL );
   
   res = so.undo( createTestingContext( game.get() ) );  
   assertOrThrow( res.successful() );
   
   assertOrThrow( fld->checkForObject( crystals ) != NULL );
   assertOrThrow( fld->checkForObject( road ) == NULL );
}
