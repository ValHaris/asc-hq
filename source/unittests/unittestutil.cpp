/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include "../actions/moveunitcommand.h"
#include "../actions/attackcommand.h"
#include "../loaders.h"
#include "../viewcalculation.h"
#include "../turncontrol.h"

#include "unittestutil.h"

bool NextTurnStrategy_Abort::continueWhenLastPlayer() const { 
   return false;
};  

bool NextTurnStrategy_Abort::authenticate( GameMap* actmap) const
{
   return true;  
}


void move( Vehicle* veh, const MapCoordinate& dest )
{
   auto_ptr<MoveUnitCommand> muc ( new MoveUnitCommand( veh ));
   muc->setDestination( dest );
   ActionResult res = muc->execute( createTestingContext( veh->getMap() ));
   if ( res.successful() )
      muc.release();
   else
      throw ActionResult(res);
}

void attack( Vehicle* veh, const MapCoordinate& target )
{
   auto_ptr<AttackCommand> muc ( new AttackCommand( veh ));
   muc->setTarget( target );
   ActionResult res = muc->execute( createTestingContext( veh->getMap() ));
   if ( res.successful() )
      muc.release();
   else
      throw ActionResult(res);
}


GameMap* startMap( const ASCString& filename )
{
   GameMap* game = mapLoadingExceptionChecker( filename, MapLoadingFunction( tmaploaders::loadmap  ));
   if ( !game )
      throw TestFailure("could not load map " + filename);;

   game->levelfinished = false;

   if ( game->replayinfo ) {
      delete game->replayinfo;
      game->replayinfo = NULL;
   }
   
   computeview( game );
         
   if ( game && game->actplayer == -1 ) 
      next_turn(game, NextTurnStrategy_Abort(), NULL, -1);
   
   return game;
   
}

Context createTestingContext( GameMap* gamemap )
{
   Context context;
   
   context.gamemap = gamemap;
   context.actingPlayer = &gamemap->getPlayer( gamemap->actplayer );
   context.parentAction = NULL;
   context.display = NULL;
   context.viewingPlayer = gamemap->getPlayerView(); 
   context.actionContainer = &gamemap->actions;
   return context;   
}

void testCargoMovement( Vehicle* veh, int movement )
{
   for ( Vehicle::Cargo::const_iterator i = veh->getCargo().begin(); i != veh->getCargo().end(); ++i )
      if ( *i ) {
         Vehicle* cargo1 = *i;
         assertOrThrow( cargo1->getMovement() == movement );
            
         testCargoMovement( cargo1, movement );
      }
}

void testCargoMovementMax( Vehicle* veh, int movement )
{
   for ( Vehicle::Cargo::const_iterator i = veh->getCargo().begin(); i != veh->getCargo().end(); ++i )
      if ( *i ) {
         Vehicle* cargo1 = *i;
         assertOrThrow( cargo1->getMovement() <= movement );
               
         testCargoMovementMax( cargo1, movement );
      }
}

Vehicle* getFirstCargo( ContainerBase* carrier )
{
   for ( Vehicle::Cargo::const_iterator i = carrier->getCargo().begin(); i != carrier->getCargo().end(); ++i )
      if ( *i ) 
         return *i;
   return NULL;
}
