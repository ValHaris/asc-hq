/***************************************************************************
                          ai_common.h  -  description
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

#ifndef ai_common_h_included
 #define ai_common_h_included

 // This header collects all other headers used by the various AI files so
 // compilers that are able to use precompiled headers may do so.

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include <algorithm>
#include <memory>
#include <float.h>

#include "ai.h"

#include "../typen.h"
#include "../vehicletype.h"
#include "../buildingtype.h"
#include "../misc.h"
#include "../newfont.h"
#include "../events.h"
#include "../spfst.h"
#include "../dlg_box.h"
#include "../stack.h"
#include "../missions.h"
#include "../controls.h"
#include "../dialog.h"
#include "../gamedlg.h"
#include "../attack.h"
#include "../gameoptions.h"
#include "../astar2.h"
#include "../sg.h"
#include "../resourcenet.h"
#include "../itemrepository.h"
#include "../building_controls.h"
#include "../viewcalculation.h"
#include "../replay.h"
#include "../textfiletags.h"


extern const int currentServiceOrderVersion;

  //! A path finding algorithm which avoids units to jam; used by the AI's strategy module.
  class StratAStar : public AStar {
       AI* ai;
    protected:
       virtual int getMoveCost ( int x1, int y1, int x2, int y2, const pvehicle vehicle )
       {
          int cost = AStar::getMoveCost ( x1, y1, x2, y2, vehicle );
          if ( getfield ( x2, y2 )->vehicle && beeline ( vehicle->xpos, vehicle->ypos, x2, y2) < vehicle->getMovement())
             cost += 2;
          return cost;
       };
    public:
       StratAStar ( AI* _ai, pvehicle veh ) : AStar ( _ai->getMap(), veh ), ai ( _ai ) {};
 };

  //! A 3D path finding algorithm which avoids units to jam; used by the AI's strategy module.
  class StratAStar3D : public AStar3D {
       AI* ai;
    protected:
       virtual DistanceType getMoveCost ( const MapCoordinate3D& start, const MapCoordinate3D& dest, const pvehicle vehicle, bool& canStop, bool& hasAttacked )
       {
          DistanceType cost = AStar3D::getMoveCost ( start, dest, vehicle, canStop, hasAttacked );
          if ( ai->getMap()->getField ( dest )->vehicle && beeline ( vehicle->xpos, vehicle->ypos, dest.x, dest.y) < vehicle->getMovement())
             cost += 2;
          return cost;
       };
    public:
       StratAStar3D ( AI* _ai, pvehicle veh, bool markTemps_ = true ) : AStar3D ( _ai->getMap(), veh, markTemps_ ), ai ( _ai ) {};
 };


  //! A path finding algorithm which tries to keep the units hidden from view.
 class HiddenAStar : public AStar {
       AI* ai;
    protected:
       virtual int getMoveCost ( int x1, int y1, int x2, int y2, const pvehicle vehicle )
       {
          int cost = AStar::getMoveCost ( x1, y1, x2, y2, vehicle );
          int visibility = getfield ( x2, y2 )->visible;
          int visnum = 0;
          int enemynum = 0;
          for ( int i = 0; i< 8; i++ )
             if ( getdiplomaticstatus2 ( i*8, ai->getPlayerNum()*8 ) != capeace ) {
                enemynum++;
                int v = (visibility >> ( 2*i)) & 3;
                if ( v >= visible_now )
                   visnum++;
             }
          if ( enemynum )
             cost += 12 * visnum / enemynum;

          return cost;
       };
    public:
       HiddenAStar ( AI* _ai, pvehicle veh ) : AStar ( _ai->getMap(), veh ), ai ( _ai ) {};
 };


  //! A 3D path finding algorithm which tries to keep the units hidden from view.
 class HiddenAStar3D : public AStar3D {
       AI* ai;
    protected:
       virtual DistanceType getMoveCost ( const MapCoordinate3D& start, const MapCoordinate3D& dest, const pvehicle vehicle, bool& canStop, bool& hasAttacked )
       {
          DistanceType cost = AStar3D::getMoveCost ( start, dest, vehicle, canStop, hasAttacked );
          int visibility = ai->getMap()->getField ( dest )->visible;
          int visnum = 0;
          int enemynum = 0;
          for ( int i = 0; i< 8; i++ )
             if ( getdiplomaticstatus2 ( i*8, ai->getPlayerNum()*8 ) != capeace ) {
                enemynum++;
                int v = (visibility >> ( 2*i)) & 3;
                if ( v >= visible_now )
                   visnum++;
             }
          if ( enemynum )
             cost += 12 * visnum / enemynum;

          return cost;
       };
    public:
       HiddenAStar3D ( AI* _ai, pvehicle veh, bool markTemps_ = true ) : AStar3D ( _ai->getMap(), veh, markTemps_ ), ai ( _ai ) {};
 };


#endif
