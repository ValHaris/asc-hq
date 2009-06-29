/***************************************************************************
                          strategy.cpp  -  description
                             -------------------
    begin                : Fri Mar 30 2001
    copyright            : (C) 2001 by Martin Bickel
    email                : bickel@asc-hq.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include <iostream>
#include "ai_common.h"

#include "../actions/moveunitcommand.h"


void AI::findStratPath ( vector<MapCoordinate>& path, Vehicle* veh, int x, int y )
{
  StratAStar stratAStar ( this, veh );
  stratAStar.findPath ( AStar::HexCoord ( veh->xpos, veh->ypos ), AStar::HexCoord ( x, y ), path );
}

int aiDebugInterruptHelper = -1;

AI::AiResult AI::strategy( void )
{
   AiResult result;

   /* to prevent that many units try to rush for the same spot, we are keeping track at how many units
      are going to which destination */
   map<MapCoordinate,int> destinationCounter;
   
   int stratloop = 0;
   AiResult localResult;
   do {
      localResult.unitsMoved = 0;
      localResult.unitsWaiting = 0;
      stratloop++;

      vector<int> units;
      for ( Player::VehicleList::iterator vi = getPlayer().vehicleList.begin(); vi != getPlayer().vehicleList.end(); ++vi )
         units.push_back( (*vi)->networkid );

      for ( vector<int>::iterator vi = units.begin(); vi != units.end(); ++vi ) {
         Vehicle* veh = getMap()->getUnit(*vi);
         if ( veh ) {
            if ( veh->aiparam[ getPlayerNum() ]->getJob() == AiParameter::job_fight ) {
               if ( veh->weapexist() && veh->aiparam[ getPlayerNum() ]->getTask() != AiParameter::tsk_tactics
                                    && veh->aiparam[ getPlayerNum() ]->getTask() != AiParameter::tsk_serviceRetreat ) {
                  /*
                  int orgmovement = veh->getMovement();
                  int orgxpos = veh->xpos ;
                  int orgypos = veh->ypos ;
                  */
                  
                  if ( veh->networkid == aiDebugInterruptHelper ) {
                     cout << "debug point hit with unit " << aiDebugInterruptHelper << " \n";  
                  }
   
                  if ( MoveUnitCommand::avail ( veh )) {
                     MapCoordinate3D dest;
   
                     AI::Section* sec = sections.getBest ( 0, veh, &dest, true, false, &destinationCounter );
                     if ( sec ) {
                        if( stratloop < 3 )
                           destinationCounter[dest]+=1;
                        
                        int nwid = veh->networkid;
                        int movement = veh->getMovement();
                        moveUnit ( veh, dest, false, false );
   
                        if ( getMap()->getUnit(nwid)) {
                           AiParameter& aip = *veh->aiparam[getPlayerNum()];
   
                           aip.dest = dest;
                           if ( getMap()->getUnit(nwid)->getMovement() < movement )
                              localResult.unitsMoved++;
                        } else
                           localResult.unitsMoved++;
                     }
                  }
               }
            } else {
               int nwid = veh->networkid;
               if ( runUnitTask ( veh ) )
                  if ( getMap()->getUnit( nwid ) ) {  // the unit still lives
                     if ( veh->aiparam[getPlayerNum()]->resetAfterJobCompletion )
                        veh->aiparam[getPlayerNum()]->reset( veh );
                  }
            }
   
            displaymessage2("strategy loop %d ; moved unit %d ... ", stratloop, localResult.unitsMoved );
   
            checkKeys();
         }
      }
      result += localResult;
   } while ( localResult.unitsMoved );

   displaymessage2("strategy completed ... ");

   return result;
}
