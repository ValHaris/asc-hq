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
#include "../actions/diplomacycommand.h"


void checkSymmetry( const Player& p0, const Player& p1 )
{
   assertOrThrow( p0.diplomacy.getState( p1 ) == p1.diplomacy.getState( p0 ) );
}

void testDiplomacy1() 
{
   auto_ptr<GameMap> game ( startMap("unittest-diplomacy.map"));
   
   // there is PEACE between the player
   
   Player& p0 = game->getPlayer(0);
   Player& p1 = game->getPlayer(1);
   
   assertOrThrow( p0.diplomacy.getState( p1 ) == PEACE );
   checkSymmetry(p0,p1);
   
   assertOrThrow( p0.diplomacy.isAllied( p1 ) == false );
   assertOrThrow( p0.diplomacy.isHostile( p1 ) == false );
   
   assertOrThrow( game->actplayer == 0);
   
   assertOrThrow( !fieldvisiblenow( game->getField( MapCoordinate( 0, 18)), p1.getPosition() ));
   
   
   // Player 0 proposes ALLIANCE
   
   DiplomacyCommand* dc = new DiplomacyCommand( p0 );
   dc->newstate( ALLIANCE, p1 );
   dc->execute( createTestingContext( game.get() ));
   
   assertOrThrow( p0.diplomacy.isAllied( p1 ) == false );
   checkSymmetry(p0,p1);
   
   next_turn( game.get(), NextTurnStrategy_Abort(), NULL, -1 );
   assertOrThrow( game->actplayer == 1);
   
   assertOrThrow( p0.diplomacy.isAllied( p1 ) == false );
   checkSymmetry(p0,p1);
   
   // Player 1 accepts ALLIANCE
   
   dc = new DiplomacyCommand( p1 );
   dc->newstate( ALLIANCE, p0 );
   dc->execute( createTestingContext( game.get() ));
   assertOrThrow( p0.diplomacy.isAllied( p1 ) == true );
   assertOrThrow( p0.diplomacy.getState( p1 ) == ALLIANCE );
   
   checkSymmetry(p0,p1);
   
   // now there is ALLIANCE between the players
   
   assertOrThrow( fieldvisiblenow( game->getField( MapCoordinate( 0, 18)), p1.getPosition()));
   
   
   // Player 1 makes a sneak attack 
   
   dc = new DiplomacyCommand( p1 );
   dc->sneakAttack( p0 );
   dc->execute( createTestingContext( game.get() ));
   checkSymmetry(p0,p1);
   
   assertOrThrow( p0.diplomacy.getState( p1 ) == WAR );
   
   // NOW there is WAR between the player
   
   assertOrThrow( !fieldvisiblenow( game->getField( MapCoordinate( 0, 18)), p1.getPosition()));
   
   next_turn( game.get(), NextTurnStrategy_Abort(), NULL, -1 );
   assertOrThrow( game->actplayer == 0);
   
   checkSymmetry(p0,p1);
   assertOrThrow( p0.diplomacy.getState( p1 ) == WAR );
   
   
   // Player 0 proposes peace 
   
   dc = new DiplomacyCommand( p0 );
   dc->newstate( PEACE_SV, p1 );
   dc->execute( createTestingContext( game.get() ));
   
   checkSymmetry(p0,p1);
   assertOrThrow( p0.diplomacy.getState( p1 ) == WAR );
   
   next_turn( game.get(), NextTurnStrategy_Abort(), NULL, -1 );
   assertOrThrow( game->actplayer == 1);
   
   // Player 1 only accepts truce
   
   dc = new DiplomacyCommand( p1 );
   dc->newstate( TRUCE, p0 );
   dc->execute( createTestingContext( game.get() ));
   
   checkSymmetry(p0,p1);
   assertOrThrow( p0.diplomacy.getState( p1 ) == TRUCE );
   
   assertOrThrow( p0.diplomacy.getProposal( p1.getPosition(), NULL ) == false);
   assertOrThrow( p1.diplomacy.getProposal( p0.getPosition(), NULL ) == false);
   
   // now there is truce and now queued requests
   
   
   next_turn( game.get(), NextTurnStrategy_Abort(), NULL, -1 );
   assertOrThrow( game->actplayer == 0);
   
   checkSymmetry(p0,p1);
   assertOrThrow( p0.diplomacy.getState( p1 ) == TRUCE );
   
   // Player 0 proposes peace again
   
   dc = new DiplomacyCommand( p0 );
   dc->newstate( PEACE, p1 );
   dc->execute( createTestingContext( game.get() ));
   
   next_turn( game.get(), NextTurnStrategy_Abort(), NULL, -1 );
   assertOrThrow( game->actplayer == 1);
   
   checkSymmetry(p0,p1);
   assertOrThrow( p0.diplomacy.getState( p1 ) == TRUCE );
   
   assertOrThrow( p0.diplomacy.getProposal( p1.getPosition(), NULL ) == false );
   assertOrThrow( p1.diplomacy.getProposal( p0.getPosition(), NULL ) == true );
   
   
   // Player 1 accepts the peace and proposes to do even an alliance  
   
   dc = new DiplomacyCommand( p1 );
   dc->newstate( ALLIANCE, p0 );
   dc->execute( createTestingContext( game.get() ));
   
   checkSymmetry(p0,p1);
   assertOrThrow( p0.diplomacy.getState( p1 ) == PEACE );
   
   assertOrThrow( p0.diplomacy.getProposal( p1.getPosition(), NULL ) == true );
   assertOrThrow( p1.diplomacy.getProposal( p0.getPosition(), NULL ) == false );
   
   
   next_turn( game.get(), NextTurnStrategy_Abort(), NULL, -1 );
   assertOrThrow( game->actplayer == 0);
   
   checkSymmetry(p0,p1);
   assertOrThrow( p0.diplomacy.getState( p1 ) == PEACE );
   
   assertOrThrow( !fieldvisiblenow( game->getField( MapCoordinate( 0, 18)), p1.getPosition()));
   
   dc = new DiplomacyCommand( p0 );
   dc->newstate( ALLIANCE, p1 );
   dc->execute( createTestingContext( game.get() ));
   
   checkSymmetry(p0,p1);
   assertOrThrow( p0.diplomacy.getState( p1 ) == ALLIANCE );
   
   assertOrThrow( fieldvisiblenow( game->getField( MapCoordinate( 0, 18)), p1.getPosition()));
   
   
   assertOrThrow( p0.diplomacy.getProposal( p1.getPosition(), NULL ) == false );
   assertOrThrow( p1.diplomacy.getProposal( p0.getPosition(), NULL ) == false );
   
   
   next_turn( game.get(), NextTurnStrategy_Abort(), NULL, -1 );
   assertOrThrow( game->actplayer == 1);
   
   
   dc = new DiplomacyCommand( p1 );
   dc->newstate( TRUCE, p0 );
   dc->execute( createTestingContext( game.get() ));

   next_turn( game.get(), NextTurnStrategy_Abort(), NULL, -1 );
   assertOrThrow( game->actplayer == 0);
   
   dc = new DiplomacyCommand( p0 );
   dc->newstate( WAR, p1 );
   dc->execute( createTestingContext( game.get() ));
      
   
   checkSymmetry(p0,p1);
   assertOrThrow( p0.diplomacy.getState( p1 ) == TRUCE );
   
   next_turn( game.get(), NextTurnStrategy_Abort(), NULL, -1 );
   assertOrThrow( game->actplayer == 1);
   
   checkSymmetry(p0,p1);
   assertOrThrow( p0.diplomacy.getState( p1 ) == WAR );
}



void testDiplomacy2() 
{
   auto_ptr<GameMap> game ( startMap("unittest-diplomacy.map"));
   
   // there is PEACE between the player
   
   Player& p0 = game->getPlayer(0);
   Player& p1 = game->getPlayer(1);
   
   assertOrThrow( p0.diplomacy.getState( p1 ) == PEACE );
   checkSymmetry(p0,p1);
   
   assertOrThrow( p0.diplomacy.isAllied( p1 ) == false );
   assertOrThrow( p0.diplomacy.isHostile( p1 ) == false );
   
   // player0 declares WAR
   
   DiplomacyCommand* dc = new DiplomacyCommand( p0 );
   dc->newstate( WAR, p1 );
   dc->execute( createTestingContext( game.get() ));
   
   next_turn( game.get(), NextTurnStrategy_Abort(), NULL, -1 );
   assertOrThrow( game->actplayer == 1);
   
   // .. so on the next turn the WAR will take effect
   
   checkSymmetry(p0,p1);
   assertOrThrow( p0.diplomacy.getState( p1 ) == WAR );
   
}

void testDiplomacy3() 
{
   auto_ptr<GameMap> game ( startMap("unittest-diplomacy.map"));
   
   // there is PEACE between the player
   
   Player& p0 = game->getPlayer(0);
   Player& p1 = game->getPlayer(1);
   
   assertOrThrow( p0.diplomacy.getState( p1 ) == PEACE );
   checkSymmetry(p0,p1);
   
   assertOrThrow( p0.diplomacy.isAllied( p1 ) == false );
   assertOrThrow( p0.diplomacy.isHostile( p1 ) == false );
   
   // player0 declares WAR
   
   DiplomacyCommand* dc = new DiplomacyCommand( p0 );
   dc->newstate( WAR, p1 );
   dc->execute( createTestingContext( game.get() ));
   
   // ... and presses undo
   
   ActionResult res = game->actions.undo( createTestingContext( game.get() ) );
   assertOrThrow( res.successful() );
   
   next_turn( game.get(), NextTurnStrategy_Abort(), NULL, -1 );
   assertOrThrow( game->actplayer == 1);
   
   checkSymmetry(p0,p1);
   assertOrThrow( p0.diplomacy.getState( p1 ) == PEACE );
   
   // nothing happens
   
   next_turn( game.get(), NextTurnStrategy_Abort(), NULL, -1 );
   
   // still nothing
   
   assertOrThrow( game->actplayer == 0);
   
   checkSymmetry(p0,p1);
   assertOrThrow( p0.diplomacy.getState( p1 ) == PEACE );
   
}


void testDiplomacy() 
{
   testDiplomacy1();
   testDiplomacy2();
}
