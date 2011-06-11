%module asc
%{
#include <string>
#include "commands.h"
#include "../gamemap.h"
#include "../actions/actionresult.h"
#include "../dialog.h"
%}

%include "common.i"


%rename(loadGame) loadGameLua;
GameMap* loadGameLua( const char* filename );



class ActionResult {
   protected:
      ActionResult();
   public:
      bool successful();
};



extern void displayActionError( const ActionResult& result, const std::string& additionalInfo = "" );


ActionResult unitAttack( GameMap* actmap, int veh, const MapCoordinate& destination, int weapon = -1 );
ActionResult unitMovement( GameMap* actmap, int unitID, const MapCoordinate& destination, int destinationHeigth );
ActionResult unitMovement( GameMap* actmap, int unitID, const MapCoordinate& destination );

%rename(unitPutMine) putMineFunc;
extern ActionResult putMineFunc( GameMap* actmap, int veh, const MapCoordinate& destination, int mineType );

%rename(unitRemoveMine) removeMineFunc;
extern ActionResult removeMineFunc( GameMap* actmap, int veh, const MapCoordinate& destination );

extern ActionResult unitPutObject   ( GameMap* actmap, int veh, const MapCoordinate& destination, int objectID );
extern ActionResult unitRemoveObject( GameMap* actmap, int veh, const MapCoordinate& destination, int objectID );
extern ActionResult unitDestructBuilding( GameMap* actmap, int veh, const MapCoordinate& destination );
extern ActionResult transferControl( GameMap* actmap, int containerID, int newOwner );
extern ActionResult trainUnit( GameMap* actmap, int containerID, int unitID );
extern ActionResult constructUnit( GameMap* actmap, int containerID, const MapCoordinate& position, int unitID );
extern ActionResult constructBuilding( GameMap* actmap, int unitID, const MapCoordinate& position, int buildingTypeID );
extern ActionResult serviceCommand( GameMap* actmap, int providingContainerID, int receivingContainerID, int type, int amount );
extern ActionResult repairUnit( GameMap* actmap, int repairerID, int damagedUnitID );
extern ActionResult unitReactionFireEnable( GameMap* actmap, int unitID, int enabled );
extern ActionResult unitPowerGenerationEnable( GameMap* actmap, int unitID, int enabled );
extern ActionResult unitJump( GameMap* actmap, int veh, const MapCoordinate& destination );
extern ActionResult selfDestruct( GameMap* actmap, int containerID );
extern ActionResult recycleUnit( GameMap* actmap, int containerID, int unitID );
extern ActionResult buildProductionLine( GameMap* actmap, int containerID, int vehicleTypeID );
extern ActionResult removeProductionLine( GameMap* actmap, int containerID, int vehicleTypeID );
extern ActionResult repairBuilding( GameMap* actmap, int buildingID );
extern ActionResult setResourceProcessingRate( GameMap* actmap, int containerID, int amount );
extern ActionResult cargoUnitMove( GameMap* actmap, int unitID, int targetContainerID );
extern ActionResult setDiplomacy( GameMap* actmap, int actingPlayer, int towardsPlayer, bool sneak, int newState );
extern ActionResult cancelResearch( GameMap* actmap, int actingPlayer );
extern ActionResult setResearchGoal( GameMap* actmap, int actingPlayer, int techID );
extern ActionResult renameContainer( GameMap* actmap, int unitID, const std::string& publicName, const std::string& privateName );


extern void endTurn();
extern void endTurn_headLess( GameMap* gamemap );

/** returns the unit under the cursor, provided that there is a unit and it belongs to the active player
    return NULL otherwise
*/
extern Vehicle* getSelectedUnit( GameMap* map );

//! throw an AssertionException if the result indicates failure
extern void assertSuccess( const ActionResult& result );
