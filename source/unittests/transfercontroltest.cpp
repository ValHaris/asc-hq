/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "../gamemap.h"
#include "../actions/transfercontrolcommand.h"
#include "../loaders.h"
#include "transfercontroltest.h"
#include "unittestutil.h"
#include "spfst.h"



void testTransferControl() 
{
   auto_ptr<GameMap> game ( startMap("unittest-transfercontrol.map"));
   
   Vehicle* veh = game->getField(4,8)->vehicle;
   assertOrThrow( veh != NULL );
   assertOrThrow( veh->getOwner() == 0 );
   
   MapField* fld = game->getField(4,0);
   assertOrThrow(  fieldvisiblenow(fld,0) );
   assertOrThrow( !fieldvisiblenow(fld,3) );
   
   auto_ptr<TransferControlCommand> tcc ( new TransferControlCommand( veh ));
   tcc->setReceiver( &game->player[3] );
   ActionResult res = tcc->execute( createTestingContext( game.get() ));
   assertOrThrow( res.successful() );
   tcc.release();
   
   assertOrThrow( veh->getOwner() == 3 );
   assertOrThrow( !fieldvisiblenow(fld,0) );
   assertOrThrow(  fieldvisiblenow(fld,3) );
   
   res = game->actions.undo( createTestingContext( game.get() ) );  
   assertOrThrow( res.successful() );
   
   assertOrThrow( veh->getOwner() == 0 );
   assertOrThrow( fieldVisibility(fld,0) == visible_now );
   assertOrThrow( fieldVisibility(fld,3) == visible_not );
   
   
   Building* bld = game->getField(14,7)->building;
   assertOrThrow( bld != NULL );
   assertOrThrow( bld->getOwner() == 0 );
   
   
   auto_ptr<TransferControlCommand> tcc2 ( new TransferControlCommand( bld));
   tcc2->setReceiver( &game->player[1] );
   ActionResult res2 = tcc2->execute( createTestingContext( game.get() ));
   assertOrThrow( res2.successful() );
   tcc2.release();
   
   assertOrThrow( bld->getOwner() == 1 );
   assertOrThrow( getFirstCargo(bld) );
   assertOrThrow( getFirstCargo(bld)->getOwner() == 1 );
   
}
