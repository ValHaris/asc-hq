
/*! \file commands.cpp
    \brief The interface for accessing the game commands from Lua.
 */


/*
     This file is part of Advanced Strategic Command; http://www.asc-hq.de
     Copyright (C) 1994-2008  Martin Bickel  

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


#include "../sg.h"
#include "../ascstring.h"
#include "../vehicle.h"
#include "../gamemap.h"
#include "../spfst.h"
#include "../spfst-legacy.h"
#include "../itemrepository.h"
               
#include "../actions/attackcommand.h"
#include "../actions/moveunitcommand.h"
#include "../actions/putminecommand.h"
#include "../actions/putobjectcommand.h"
#include "../actions/destructbuildingcommand.h"
#include "../actions/transfercontrolcommand.h"
#include "../actions/trainunitcommand.h"
#include "../actions/constructunitcommand.h"
#include "../actions/constructbuildingcommand.h"
#include "../actions/servicecommand.h"
#include "../actions/repairunitcommand.h"
#include "../actions/reactionfireswitchcommand.h"
#include "../actions/jumpdrivecommand.h"
#include "../actions/destructunitcommand.h"
#include "../actions/recycleunitcommand.h"
#include "../actions/buildproductionlinecommand.h"
#include "../actions/removeproductionlinecommand.h"
#include "../actions/powergenerationswitchcommand.h"
#include "../actions/repairbuildingcommand.h"
#include "../actions/setresourceprocessingratecommand.h"
#include "../actions/cargomovecommand.h"
#include "../actions/diplomacycommand.h"
#include "../actions/cancelresearchcommand.h"
#include "../actions/directresearchcommand.h"

GameMap* loadGameLua( const char* filename )
{
   loadGameFromFile( filename );  
   repaintMap();
   return actmap;
}
         
ActionResult unitAttack( GameMap* actmap, int veh, const MapCoordinate& target, int weapon )
{
   if ( !actmap )
      return ActionResult( 23500 );
   
   Vehicle* unit = actmap->getUnit( veh );
   if ( !unit  )
      return ActionResult(120);
   
   if ( !AttackCommand::avail( unit ))   
      return ActionResult(202);
   
   auto_ptr<AttackCommand> ac( new AttackCommand(unit) );
   ac->setTarget( target, weapon);
   ActionResult res = ac->execute( createContext( actmap ) );
   if ( res.successful() ) 
      ac.release();
      
   return res;
}
         
         
ActionResult unitMovement( GameMap* actmap, int unitID, const MapCoordinate& destination, int destinationHeigth )
{
   if ( !actmap )
      return ActionResult( 23500 );
   
   Vehicle* unit = actmap->getUnit( unitID );
   if ( !unit )
      return ActionResult(120);
   
   if ( !MoveUnitCommand::avail( unit ))   
      return ActionResult(23411);
   
   auto_ptr<MoveUnitCommand> ac( new MoveUnitCommand(unit) );
   if ( destinationHeigth < 0 )
      ac->setDestination( destination );
   else
      ac->setDestination( MapCoordinate3D( destination, 1 << destinationHeigth ));
   ActionResult res = ac->execute( createContext( actmap ) );
   if ( res.successful() ) 
      ac.release();
   
   return res;
}
         
ActionResult unitMovement( GameMap* actmap, int unitID, const MapCoordinate& destination )
{
   return unitMovement( actmap, unitID, destination, -1 );
}

ActionResult putMineFunc( GameMap* actmap, int veh, const MapCoordinate& destination, int mineType )
{
   if ( !actmap )
      return ActionResult( 23500 );
   
   Vehicle* unit = actmap->getUnit( veh );
   if ( !unit )
      return ActionResult(120);
   
   if ( !PutMineCommand::avail( unit ))   
      return ActionResult(23411);
   
   auto_ptr<PutMineCommand> ac( new PutMineCommand(unit) );
   ac->setCreationTarget( destination, (MineTypes)mineType );
   ActionResult res = ac->execute( createContext( actmap ) );
   if ( res.successful() ) 
      ac.release();
   
   return res;
}

ActionResult removeMineFunc( GameMap* actmap, int veh, const MapCoordinate& destination )
{
   if ( !actmap )
      return ActionResult( 23500 );
   
   Vehicle* unit = actmap->getUnit( veh );
   if ( !unit )
      return ActionResult(120);
   
   if ( !PutMineCommand::avail( unit ))   
      return ActionResult(23411);
   
   auto_ptr<PutMineCommand> ac( new PutMineCommand(unit) );
   ac->setRemovalTarget( destination );
   ActionResult res = ac->execute( createContext( actmap ) );
   if ( res.successful() ) 
      ac.release();
   
   return res;
}



ActionResult unitPutObject( GameMap* actmap, int veh, const MapCoordinate& destination, int objectID )
{
   if ( !actmap )
      return ActionResult( 23500 );
   
   Vehicle* unit = actmap->getUnit( veh );
   if ( !unit )
      return ActionResult(120);
   
   if ( !PutObjectCommand::avail( unit ))   
      return ActionResult(23411);
   
   auto_ptr<PutObjectCommand> ac( new PutObjectCommand(unit) );
   ac->setTarget( destination, objectID );
   ActionResult res = ac->execute( createContext( actmap ) );
   if ( res.successful() ) 
      ac.release();
   
   return res;
}

ActionResult unitRemoveObject( GameMap* actmap, int veh, const MapCoordinate& destination, int objectID )
{
   return unitPutObject( actmap, veh, destination, objectID );
}


ActionResult unitDestructBuilding( GameMap* actmap, int veh, const MapCoordinate& destination )
{
   if ( !actmap )
      return ActionResult( 23500 );
   
   Vehicle* unit = actmap->getUnit( veh );
   if ( !unit )
      return ActionResult(120);
   
   if ( !DestructBuildingCommand::avail( unit ))   
      return ActionResult(23411);
   
   auto_ptr<DestructBuildingCommand> ac( new DestructBuildingCommand(unit) );
   ac->setTargetPosition( destination );
   ActionResult res = ac->execute( createContext( actmap ) );
   if ( res.successful() ) 
      ac.release();
   
   return res;
}

ActionResult transferControl( GameMap* actmap, int containerID, int newOwner )
{
   if ( !actmap )
      return ActionResult( 23500 );
   
   ContainerBase* c = actmap->getContainer( containerID);
   if ( !c )
      return ActionResult(23410);
   
   if ( !TransferControlCommand::avail( c ))   
      return ActionResult(23411);
   
   if ( newOwner < 0 || newOwner > 7 )
      return ActionResult(22801);
   
   
   auto_ptr<TransferControlCommand> ac( new TransferControlCommand(c) );
   ac->setReceiver( &actmap->getPlayer( newOwner) );
   ActionResult res = ac->execute( createContext( actmap ) );
   if ( res.successful() ) 
      ac.release();
   
   return res;
}

ActionResult trainUnit( GameMap* actmap, int containerID, int unitID )
{
   if ( !actmap )
      return ActionResult( 23500 );
   
   ContainerBase* c = actmap->getContainer( containerID);
   if ( !c )
      return ActionResult(23410);
     
   Vehicle* v = actmap->getUnit( unitID );
   if ( !v )
      return ActionResult( 21001 );
   
   if ( !TrainUnitCommand::avail( c, v ))   
      return ActionResult(23411);
   
   
   auto_ptr<TrainUnitCommand> ac( new TrainUnitCommand(c) );
   ac->setUnit( v );
   ActionResult res = ac->execute( createContext( actmap ) );
   if ( res.successful() ) 
      ac.release();
   
   return res;
}

ActionResult constructUnit( GameMap* actmap, int containerID, const MapCoordinate& position, int unitID )
{
   if ( !actmap )
      return ActionResult( 23500 );
   
   ContainerBase* c = actmap->getContainer( containerID );
   if ( !c )
      return ActionResult(23410);
     
   Vehicletype* v = vehicleTypeRepository.getObject_byID( unitID );
   if ( !v )
      return ActionResult( 21701 );
   
   if ( !ConstructUnitCommand::avail( c ))   
      return ActionResult(23411);
   
   
   auto_ptr<ConstructUnitCommand> ac( new ConstructUnitCommand(c) );
   ac->setVehicleType( v );
   
   if ( position.valid() ) {
      if ( position != c->getPosition())
         ac->setMode( ConstructUnitCommand::external );
      else
         ac->setMode( ConstructUnitCommand::internal );
      
      ac->setTargetPosition( position );
   } else
      ac->setMode( ConstructUnitCommand::internal );
   
   ActionResult res = ac->execute( createContext( actmap ) );
   if ( res.successful() ) 
      ac.release();
   
   return res;
}


ActionResult constructBuilding( GameMap* actmap, int unitID, const MapCoordinate& position, int buildingTypeID )
{
   if ( !actmap )
      return ActionResult( 23500 );
   
   Vehicle* unit = actmap->getUnit( unitID );
   if ( !unit )
      return ActionResult(120);
   
   if ( !ConstructBuildingCommand::avail( unit ))   
      return ActionResult(23411);
     
   BuildingType* v = buildingTypeRepository.getObject_byID( buildingTypeID );
   if ( !v )
      return ActionResult( 21701 );
   
  
   auto_ptr<ConstructBuildingCommand> ac( new ConstructBuildingCommand(unit) );
   ac->setBuildingType( v );
   ac->setTargetPosition( position );

   ActionResult res = ac->execute( createContext( actmap ) );
   if ( res.successful() ) 
      ac.release();

   return res;
}

ActionResult serviceCommand( GameMap* actmap, int providingContainerID, int receivingContainerID, int type, int amount )
{
   if ( !actmap )
      return ActionResult( 23500 );
   
   ContainerBase* pro = actmap->getContainer( providingContainerID );
   ContainerBase* rec = actmap->getContainer( receivingContainerID );
   if ( !pro || !rec )
      return ActionResult(23410);
   
   auto_ptr<ServiceCommand> ac( new ServiceCommand(pro) );
   ac->setDestination( rec );
   
   TransferHandler::Transfers& transfers = ac->getTransferHandler().getTransfers();
   
   for ( TransferHandler::Transfers::iterator i = transfers.begin(); i != transfers.end(); ++i )
      if ( (*i)->getID() == type )
         (*i)->setAmount( rec, amount );
   
   ac->saveTransfers();
   ActionResult res = ac->execute( createContext( actmap ) );
   if ( res.successful() ) 
      ac.release();

   return res;
}

ActionResult repairUnit( GameMap* actmap, int repairerID, int damagedUnitID )
{
   if ( !actmap )
      return ActionResult( 23500 );
   
   ContainerBase* c = actmap->getContainer( repairerID);
   if ( !c )
      return ActionResult(23410);
     
   Vehicle* v = actmap->getUnit( damagedUnitID );
   if ( !v )
      return ActionResult( 21001 );
   
   if ( !RepairUnitCommand::avail( c ))   
      return ActionResult(23411);
   
   
   auto_ptr<RepairUnitCommand> ac( new RepairUnitCommand(c) );
   ac->setTarget( v );
   ActionResult res = ac->execute( createContext( actmap ) );
   if ( res.successful() ) 
      ac.release();
   
   return res;
}

ActionResult unitReactionFireEnable( GameMap* actmap, int unitID, bool enabled )
{
   if ( !actmap )
      return ActionResult( 23500 );
   
   Vehicle* unit = actmap->getUnit( unitID );
   if ( !unit )
      return ActionResult(120);
   
   if ( !ReactionFireSwitchCommand::avail( unit, enabled ))   
      return ActionResult(23411);
   
   auto_ptr<ReactionFireSwitchCommand> ac( new ReactionFireSwitchCommand(unit) );
   ac->setNewState( enabled );
   ActionResult res = ac->execute( createContext( actmap ) );
   if ( res.successful() ) 
      ac.release();
   
   return res;
}

ActionResult unitPowerGenerationEnable( GameMap* actmap, int unitID, int enabled )
{
   if ( !actmap )
      return ActionResult( 23500 );
   
   Vehicle* unit = actmap->getUnit( unitID );
   if ( !unit )
      return ActionResult(120);

   if ( !PowerGenerationSwitchCommand::avail( unit, enabled ))   
      return ActionResult(23411);

   auto_ptr<PowerGenerationSwitchCommand> ac( new PowerGenerationSwitchCommand(unit) );
   ac->setNewState( enabled );
   ActionResult res = ac->execute( createContext( actmap ) );
   if ( res.successful() ) 
      ac.release();

   return res;
}

ActionResult unitJump( GameMap* actmap, int veh, const MapCoordinate& destination )
{
   if ( !actmap )
      return ActionResult( 23500 );
   
   Vehicle* unit = actmap->getUnit( veh );
   if ( !unit )
      return ActionResult(120);
   
   if ( !JumpDriveCommand::avail( unit ))   
      return ActionResult(23411);
   
   auto_ptr<JumpDriveCommand> ac( new JumpDriveCommand(unit) );
   ac->setDestination( destination );
   ActionResult res = ac->execute( createContext( actmap ) );
   if ( res.successful() ) 
      ac.release();
   
   return res;
}

ActionResult selfDestruct( GameMap* actmap, int containerID )
{
   if ( !actmap )
      return ActionResult( 23500 );
   
   ContainerBase* c = actmap->getContainer( containerID );
   if ( !c )
      return ActionResult(23410);
     
   if ( !DestructUnitCommand::avail( c ))   
      return ActionResult(23411);
   
   auto_ptr<DestructUnitCommand> ac( new DestructUnitCommand(c) );
   ActionResult res = ac->execute( createContext( actmap ) );
   if ( res.successful() ) 
      ac.release();
   
   return res;
}

ActionResult recycleUnit( GameMap* actmap, int containerID, int unitID )
{
   if ( !actmap )
      return ActionResult( 23500 );
   
   ContainerBase* c = actmap->getContainer( containerID );
   if ( !c )
      return ActionResult(23410);
     
   Vehicle* v = actmap->getUnit( unitID );
   if ( !v )
      return ActionResult( 21001 );
   
   if ( !RecycleUnitCommand::avail( c, v ))   
      return ActionResult(23411);
   
   
   auto_ptr<RecycleUnitCommand> ac( new RecycleUnitCommand(c) );
   ac->setUnit( v );
   ActionResult res = ac->execute( createContext( actmap ) );
   if ( res.successful() ) 
      ac.release();
   
   return res;
}

ActionResult buildProductionLine( GameMap* actmap, int containerID, int vehicleTypeID )
{
   if ( !actmap )
      return ActionResult( 23500 );
   
   ContainerBase* c = actmap->getContainer( containerID );
   if ( !c )
      return ActionResult(23410);
     
   Vehicletype* v = vehicleTypeRepository.getObject_byID( vehicleTypeID );
   if ( !v )
      return ActionResult( 21701 );
   
   if ( !BuildProductionLineCommand::avail( c ))   
      return ActionResult(23411);
   
   
   auto_ptr<BuildProductionLineCommand> ac( new BuildProductionLineCommand(c) );
   ac->setProduction( v );
   ActionResult res = ac->execute( createContext( actmap ) );
   if ( res.successful() ) 
      ac.release();
   
   return res;
}

ActionResult removeProductionLine( GameMap* actmap, int containerID, int vehicleTypeID )
{
   if ( !actmap )
      return ActionResult( 23500 );
   
   ContainerBase* c = actmap->getContainer( containerID );
   if ( !c )
      return ActionResult(23410);
     
   Vehicletype* v = vehicleTypeRepository.getObject_byID( vehicleTypeID );
   if ( !v )
      return ActionResult( 21701 );
   
   if ( !RemoveProductionLineCommand::avail( c ))   
      return ActionResult(23411);
   
   auto_ptr<RemoveProductionLineCommand> ac( new RemoveProductionLineCommand(c) );
   ac->setRemoval( v );
   ActionResult res = ac->execute( createContext( actmap ) );
   if ( res.successful() ) 
      ac.release();
   
   return res;
}

ActionResult repairBuilding( GameMap* actmap, int buildingID )
{
   if ( !actmap )
      return ActionResult( 23500 );
   
   ContainerBase* c = actmap->getContainer( buildingID );
   if ( !c )
      return ActionResult(23410);
   
   Building* b = dynamic_cast<Building*>(c);
   if ( !b )
      return ActionResult(23410);
     
   if ( !RepairBuildingCommand::avail( b ))   
      return ActionResult(23411);
   
   auto_ptr<RepairBuildingCommand> ac( new RepairBuildingCommand(b) );
   ActionResult res = ac->execute( createContext( actmap ) );
   if ( res.successful() ) 
      ac.release();
   
   return res;
}

ActionResult setResourceProcessingRate( GameMap* actmap, int containerID, int amount )
{
   if ( !actmap )
      return ActionResult( 23500 );
   
   ContainerBase* c = actmap->getContainer( containerID );
   if ( !c )
      return ActionResult(23410);
     
   if ( !SetResourceProcessingRateCommand::avail( c ))   
      return ActionResult(23411);
   
   auto_ptr<SetResourceProcessingRateCommand> ac( new SetResourceProcessingRateCommand(c, amount ) );
   ActionResult res = ac->execute( createContext( actmap ) );
   if ( res.successful() ) 
      ac.release();
   
   return res;
}

ActionResult cargoUnitMove( GameMap* actmap, int unitID, int targetContainerID )
{
   if ( !actmap )
      return ActionResult( 23500 );
   
   Vehicle* unit = actmap->getUnit( unitID );
   if ( !unit )
      return ActionResult(120);
   
   auto_ptr<CargoMoveCommand> ac( new CargoMoveCommand(unit) );
   
   Vehicle* target = NULL;
   if ( targetContainerID != -1 ) {
      target = actmap->getUnit( targetContainerID );
      if ( !target )
         return ActionResult(23410);
      
      if ( !CargoMoveCommand::moveInAvail( unit, target ))   
         return ActionResult(23411);
      
      ac->setMode( CargoMoveCommand::moveInwards );
      ac->setTargetCarrier( target );
      
   } else {
      if ( !CargoMoveCommand::moveOutAvail( unit ))   
         return ActionResult(23411);
      
      ac->setMode( CargoMoveCommand::moveOutwards );
   }
   
   ActionResult res = ac->execute( createContext( actmap ) );
   if ( res.successful() ) 
      ac.release();
   
   return res;
}

ActionResult setDiplomacy( GameMap* actmap, int actingPlayer, int towardsPlayer, bool sneak, int newState )
{
   if ( !actmap )
      return ActionResult( 23500 );
   
   if ( actingPlayer < 0 || actingPlayer >= actmap->getPlayerCount() )
      return ActionResult( 23100 );
   
   if ( towardsPlayer < 0 || towardsPlayer >= actmap->getPlayerCount() )
      return ActionResult( 23100 );
   
   
   
   auto_ptr<DiplomacyCommand> ac( new DiplomacyCommand(actmap->getPlayer(actingPlayer)) );
   if ( sneak )
      ac->sneakAttack( actmap->getPlayer(towardsPlayer) );
   else {
      if ( newState < 0 || newState >= diplomaticStateNum )
         return ActionResult( 23501 );
      
      DiplomaticStates state = (DiplomaticStates) newState;
      ac->newstate( state, actmap->getPlayer(towardsPlayer));
   }
   ActionResult res = ac->execute( createContext( actmap ) );
   if ( res.successful() ) 
      ac.release();
   
   return res;
}

ActionResult cancelResearch( GameMap* actmap, int actingPlayer )
{
   if ( !actmap )
      return ActionResult( 23500 );
   
   if ( actingPlayer < 0 || actingPlayer >= actmap->getPlayerCount() )
      return ActionResult( 23100 );
   
   auto_ptr<CancelResearchCommand> ac( new CancelResearchCommand( actmap ) );
   ac->setPlayer( actmap->getPlayer( actingPlayer ));
   ActionResult res = ac->execute( createContext( actmap ) );
   if ( res.successful() ) 
      ac.release();
   
   return res;

}

ActionResult setResearchGoal( GameMap* actmap, int actingPlayer, int techID )
{
   if ( !actmap )
      return ActionResult( 23500 );
   
   if ( actingPlayer < 0 || actingPlayer >= actmap->getPlayerCount() )
      return ActionResult( 23100 );
   
   Player& p = actmap->getPlayer( actingPlayer );
   
   if ( !DirectResearchCommand::available( p ) )
      return ActionResult(23411);
   
   Technology* t = technologyRepository.getObject_byID( techID );
   if ( !t )
      return ActionResult( 23205 );
   
   auto_ptr<DirectResearchCommand> ac( new DirectResearchCommand( p ) );
   ac->setTechnology( t );
   ActionResult res = ac->execute( createContext( actmap ) );
   if ( res.successful() ) 
      ac.release();
   
   return res;

}
