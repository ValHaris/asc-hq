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
#include "../actions/repairunitcommand.h"
#include "../loaders.h"
#include "../itemrepository.h"
#include "unittestutil.h"
#include "repairtest.h"

/* NextTurnStrategy_Abort will delete the map if the game cannot be continued.
 * To avoid double deallocation, we must intercept the event and release the auto_ptr
 */
class MapHolder : public SigC::Object {
		auto_ptr<GameMap> game;
	public:
		MapHolder( GameMap* gamemap ) : game( gamemap ){
			   GameMap::sigMapDeletion.connect( SigC::slot( *this, &MapHolder::reset ));
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

   for ( int i = 0; i < 10; ++i) {
	   // std::cout << "experience in turn " << i << " is " << veh->getExperience_offensive_raw() << "\n";
	   next_turn( game.get(), NextTurnStrategy_Abort(), NULL, -1 );
   }

   assertOrThrow ( veh->damage == 10 );
   
}


void testManualRepair()
{
   MapHolder game ( startMap("unittest-repair.map"));

   Vehicle* carrier = game->getField(1,9)->vehicle;
   assertOrThrow( carrier != NULL );

   Vehicle* aircraft = carrier->getCargo(0);
   assertOrThrow( aircraft != NULL );
   assertOrThrow ( aircraft->getExperience_offensive() == 10 );
   assertOrThrow ( aircraft->getExperience_defensive() == 10 );
   assertOrThrow ( aircraft->damage == 50 );

   assertOrThrow( RepairUnitCommand::avail(carrier) );
   auto_ptr<RepairUnitCommand> ruc ( new RepairUnitCommand( carrier ));

   assertOrThrow( ruc->getInternalTargets().size() == 1 );
   assertOrThrow( ruc->getInternalTargets()[0] == aircraft );

   ruc->setTarget(aircraft);

   ActionResult res = ruc->execute( createTestingContext( game.get() ));
   if ( res.successful() )
      ruc.release();
   else
      throw ActionResult(res);

   assertOrThrow ( aircraft->damage == 0 );
   assertOrThrow ( aircraft->getExperience_offensive() == 9 );
   assertOrThrow ( aircraft->getExperience_defensive() == 9 );

}


void testUnitRepair() {
	testAutoRepair();
	testManualRepair();
}
