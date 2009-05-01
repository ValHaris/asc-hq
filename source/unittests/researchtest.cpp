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
#include "diplomacytest.h"
#include "unittestutil.h"
#include "spfst.h"
#include "../actions/directresearchcommand.h"
#include "itemrepository.h"
#include "../actions/buildproductionlinecommand.h"

bool vectorContains( vector<const Technology*>& v, const Technology* t )
{
   return find( v.begin(), v.end(), t) != v.end();  
}

void testresearch1() 
{
   auto_ptr<GameMap> game ( startMap("unittest-research.map"));
   
   Player& p0 = game->getPlayer(0);
   Research& r = p0.research;
   
   assertOrThrow( r.progress == 0 );
   assertOrThrow( !DirectResearchCommand::available(p0 ));
   
   next_turn( game.get(), NextTurnStrategy_Abort(), NULL, -1 );
   assertOrThrow( game->actplayer == 0);
   
   assertOrThrow( r.progress == 40 );
   assertOrThrow( DirectResearchCommand::available(p0 ));
   
   Technology* t1 = technologyRepository.getObject_byID( 1000000002  );
   assertOrThrow( t1 != NULL );
   
   Technology* t2 = technologyRepository.getObject_byID( 1000000003  );
   assertOrThrow( t2 != NULL );
   
   Technology* tjam = technologyRepository.getObject_byID( 1000000051  );
   assertOrThrow( tjam != NULL );
   
   DirectResearchCommand* drc = new DirectResearchCommand( p0 );
   vector<const Technology*> avTechs = drc->getAvailableTechnologies( true );
   
   assertOrThrow( vectorContains(avTechs, t1 ));
   assertOrThrow( vectorContains(avTechs, t2 ));
   assertOrThrow( vectorContains(avTechs, tjam ));
   
   drc->setTechnology( t1 );
   ActionResult res = drc->execute( createTestingContext( game.get() ));
   
   assertOrThrow( res.successful());
   
   
   next_turn( game.get(), NextTurnStrategy_Abort(), NULL, -1 );
   next_turn( game.get(), NextTurnStrategy_Abort(), NULL, -1 );
   next_turn( game.get(), NextTurnStrategy_Abort(), NULL, -1 );
   
   assertOrThrow( r.activetechnology == NULL );
   assertOrThrow( r.progress == 20 );
   
   drc = new DirectResearchCommand( p0 );
   avTechs = drc->getAvailableTechnologies( true );
   
   assertOrThrow( !vectorContains(avTechs, t1 ));
   assertOrThrow( vectorContains(avTechs, t2 ));
   assertOrThrow( vectorContains(avTechs, tjam ));
   
   drc->setTechnology( t2 );
   res = drc->execute( createTestingContext( game.get() ));
   assertOrThrow( res.successful());
   
   Building* bld = game->getField( MapCoordinate( 7,8 ))->building;
   assertOrThrow( bld != NULL );
   
   Vehicletype* jam = vehicleTypeRepository.getObject_byID( 1000000051 );
   assertOrThrow( jam != NULL );
   
   BuildProductionLineCommand* bplc = new BuildProductionLineCommand( bld );
   
   vector<const Vehicletype*> prods = bplc->productionLinesBuyable();
   assertOrThrow( find( prods.begin(), prods.end(), jam ) == prods.end() );
   
   drc = new DirectResearchCommand( p0 );
   drc->setTechnology( tjam );
   res = drc->execute( createTestingContext( game.get() ));
   
   assertOrThrow( r.progress == 5 );
   
   prods = bplc->productionLinesBuyable();
   assertOrThrow( find( prods.begin(), prods.end(), jam ) != prods.end() );
   
   bplc->setProduction( jam );
   res = bplc->execute( createTestingContext( game.get() ));
   
   assertOrThrow( res.successful() );
   
   game->actions.undo( createTestingContext( game.get() ) );  // build production line
   game->actions.undo( createTestingContext( game.get() ) );  // research jammer 
   game->actions.undo( createTestingContext( game.get() ) );  // research T level 2
   
   assertOrThrow( r.activetechnology == NULL );
   assertOrThrow( r.progress == 20 );
   
   
   bplc = new BuildProductionLineCommand( bld );
   prods = bplc->productionLinesBuyable();
   assertOrThrow( find( prods.begin(), prods.end(), jam ) == prods.end() );
   
}

void testresearch2() 
{
   auto_ptr<GameMap> game ( startMap("unittest-research.map"));
   
   Player& p0 = game->getPlayer(0);
   Research& r = p0.research;
   
   assertOrThrow( r.progress == 0 );
   assertOrThrow( !DirectResearchCommand::available(p0 ));
   
   next_turn( game.get(), NextTurnStrategy_Abort(), NULL, -1 );
   assertOrThrow( game->actplayer == 0);
   
   assertOrThrow( r.progress == 40 );
   assertOrThrow( DirectResearchCommand::available(p0 ));
   
   Technology* t1 = technologyRepository.getObject_byID( 1000000002  );
   assertOrThrow( t1 != NULL );
   
   Technology* t2 = technologyRepository.getObject_byID( 1000000003  );
   assertOrThrow( t2 != NULL );
   
   Technology* tjam = technologyRepository.getObject_byID( 1000000051  );
   assertOrThrow( tjam != NULL );
   
   DirectResearchCommand* drc = new DirectResearchCommand( p0 );
   vector<const Technology*> avTechs = drc->getAvailableTechnologies( true );
   
   assertOrThrow( vectorContains(avTechs, t1 ));
   assertOrThrow( vectorContains(avTechs, t2 ));
   assertOrThrow( vectorContains(avTechs, tjam ));
   
   drc->setTechnology( tjam );
   ActionResult res = drc->execute( createTestingContext( game.get() ));
   
   assertOrThrow( res.successful());
   
   
   next_turn( game.get(), NextTurnStrategy_Abort(), NULL, -1 );
   next_turn( game.get(), NextTurnStrategy_Abort(), NULL, -1 );
   next_turn( game.get(), NextTurnStrategy_Abort(), NULL, -1 );
   
   assertOrThrow( r.activetechnology == NULL );
   assertOrThrow( r.progress == 15 );
   
   drc = new DirectResearchCommand( p0 );
   avTechs = drc->getAvailableTechnologies( true );
   
   assertOrThrow( !vectorContains(avTechs, t1 ));
   assertOrThrow( vectorContains(avTechs, t2 ));
   assertOrThrow( !vectorContains(avTechs, tjam ));
   
   drc->setTechnology( t2 );
   res = drc->execute( createTestingContext( game.get() ));
   assertOrThrow( res.successful());
   
   Building* bld = game->getField( MapCoordinate( 7,8 ))->building;
   assertOrThrow( bld != NULL );
   
   Vehicletype* jam = vehicleTypeRepository.getObject_byID( 1000000051 );
   assertOrThrow( jam != NULL );
   
   BuildProductionLineCommand* bplc = new BuildProductionLineCommand( bld );
   
   vector<const Vehicletype*> prods = bplc->productionLinesBuyable();
   assertOrThrow( find( prods.begin(), prods.end(), jam ) != prods.end() );
   
}


void testResearch() 
{
   testresearch1();
   testresearch2();
}
