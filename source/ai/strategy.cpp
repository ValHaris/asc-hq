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


#include "ai_common.h"



void AI::findStratPath ( vector<MapCoordinate>& path, pvehicle veh, int x, int y )
{
  StratAStar stratAStar ( this, veh );
  stratAStar.findPath ( AStar::HexCoord ( veh->xpos, veh->ypos ), AStar::HexCoord ( x, y ), path );
}

AI::AiResult AI::strategy( void )
{
   AiResult result;

   int stratloop = 0;
   AiResult localResult;
   do {
      localResult.unitsMoved = 0;
      localResult.unitsWaiting = 0;
      stratloop++;

      Player::VehicleList::iterator nvi;
      for ( Player::VehicleList::iterator vi = getPlayer().vehicleList.begin(); vi != getPlayer().vehicleList.end();  ) {
         nvi = vi;
         ++nvi;

         pvehicle veh = *vi;
         if ( veh->aiparam[ getPlayerNum() ]->getJob() == AiParameter::job_fight ) {
            if ( veh->weapexist() && veh->aiparam[ getPlayerNum() ]->getTask() != AiParameter::tsk_tactics
                                  && veh->aiparam[ getPlayerNum() ]->getTask() != AiParameter::tsk_serviceRetreat ) {
               /*
               int orgmovement = veh->getMovement();
               int orgxpos = veh->xpos ;
               int orgypos = veh->ypos ;
               */

               VehicleMovement vm ( mapDisplay, NULL );
               if ( vm.available ( veh )) {
                  MapCoordinate3D dest;

                  AI::Section* sec = sections.getBest ( 0, veh, &dest, true );
                  if ( sec ) {
                     int nwid = veh->networkid;
                     int movement = veh->getMovement();
                     moveUnit ( veh, dest, false);

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
            runUnitTask ( veh );
         }

         displaymessage2("strategy loop %d ; moved unit %d ... ", stratloop, localResult.unitsMoved );

         checkKeys();
         vi = nvi;

      }
      result += localResult;
   } while ( localResult.unitsMoved );

   displaymessage2("strategy completed ... ");

   return result;
}
