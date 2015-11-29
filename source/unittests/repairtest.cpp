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
#include "repairtest.h"

/* NextTurnStrategy_Abort will delete the map if the game cannot be continued.
 * To avoid double deallocation, we must intercept the event and release the auto_ptr
 */
class MapHolder : public sigc::trackable {
		auto_ptr<GameMap> game;
	public:
		MapHolder( GameMap* gamemap ) : game( gamemap ){
			   GameMap::sigMapDeletion.connect( sigc::mem_fun( *this, &MapHolder::reset ));
		}

		GameMap* get() {
			return game.get();
		}

	     GameMap* operator->() const
	      {
	    	 return game.operator->();
	      }

	private:
		void reset( GameMap& gamemap ) {
			if ( &gamemap == game.get() )
				game.release();
		}
};

void testAutoRepair()
{
   MapHolder game ( startMap("unittest-repair.map"));
   
   Vehicle* veh = game->getField(3,6)->vehicle;
   assertOrThrow( veh != NULL );

   assertOrThrow ( veh->damage == 40 );

   next_turn( game.get(), NextTurnStrategy_Abort(), NULL, -1 );
   
   assertOrThrow ( veh->damage == 35 );

   for ( int i = 0; i < 10; ++i)
	   next_turn( game.get(), NextTurnStrategy_Abort(), NULL, -1 );

   assertOrThrow ( veh->damage == 10 );
   
}

void testUnitRepair() {
	testAutoRepair();
}
