/*
     This file is part of Advanced Strategic Command; http://www.asc-hq.de
     Copyright (C) 1994-2010  Martin Bickel  

     This program is free software; you can redistribute it and/or modify
     it under the terms of the GNU General Public License as published by
     the Free Software Foundation; either version 2 of the License, or
     (at your option) any later version.

     This program is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU General Public License for more details.

     You should have received a copy of the GNU General Public License
     along with this program; see the file COPYING. If not, write to the
     Free Software Foundation, Inc., 59 Temple Place, Suite 330,
     Boston, MA  02111-1307  USA
*/

#ifndef commandsH
#define commandsH

class GameMap;
class MapCoordinate;
class MapCoordinate3D;
class Vehicle;

#include "../actions/actionresult.h"

extern GameMap* loadGameLua( const char* filename );

extern ActionResult unitAttack( GameMap* actmap, int veh, const MapCoordinate& target, int weapon = -1 );
extern ActionResult unitMovement( GameMap* actmap, int unitID, const MapCoordinate& destination, int destinationHeigth );
extern ActionResult unitMovement( GameMap* actmap, int unitID, const MapCoordinate& destination );
extern ActionResult putMineFunc( GameMap* actmap, int veh, const MapCoordinate& destination, int mineType );
extern ActionResult removeMineFunc( GameMap* actmap, int veh, const MapCoordinate& destination );
extern ActionResult unitPutObject( GameMap* actmap, int veh, const MapCoordinate& destination, int objectID );
extern ActionResult unitRemoveObject( GameMap* actmap, int veh, const MapCoordinate& destination, int objectID );
extern ActionResult unitDestructBuilding( GameMap* actmap, int veh, const MapCoordinate& destination );
extern ActionResult transferControl( GameMap* actmap, int containerID, int newOwner );
extern ActionResult trainUnit( GameMap* actmap, int containerID, int unitID );
extern ActionResult constructUnit( GameMap* actmap, int containerID, const MapCoordinate& position, int unitID );
extern ActionResult constructBuilding( GameMap* actmap, int unitID, const MapCoordinate& position, int buildingTypeID );
extern ActionResult serviceCommand( GameMap* actmap, int providingContainerID, int receivingContainerID, int type, int amount );
extern ActionResult repairUnit( GameMap* actmap, int repairerID, int damagedUnitID );
extern ActionResult unitReactionFireEnable( GameMap* actmap, int unitID, bool enabled );
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
      
extern void endTurn();

//! ends a turn. All user interaction is avoided. Intended for unit tests and other automated tasks
extern void endTurn_headLess(GameMap* gamemap);

extern Vehicle* getSelectedUnit( GameMap* map );

#endif
