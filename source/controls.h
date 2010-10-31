/*! \file controls.h
   Controlling units (which is graudally moved to vehicletype.cpp and unitctrl.cpp );
   Resource networks
   Things that are run when starting and ending someones turn   
*/

/*
    This file is part of Advanced Strategic Command; http://www.asc-hq.de
    Copyright (C) 1994-2010  Martin Bickel  and  Marc Schellenberger

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

#if defined(karteneditor) 
 #error the mapeditor should not need to use controls.h !
#endif


#ifndef controlsH
#define controlsH
#include "typen.h"
#include "astar2.h"


/*! calculates the movement cost for moving vehicle from start to dest.
    \returns : first: movement ; second: fuel consumption
*/
extern pair<int,int> calcMoveMalus( const MapCoordinate3D& start,
                                    const MapCoordinate3D& dest,
                                    const Vehicle*     vehicle,
                                    WindMovement* wm = NULL,
                                    bool*  inhibitAttack = NULL,
                                    bool container2container  = false );

//! return the distance between x1/y1 and x2/y2 using the power of the wind factors precalculated in #WindMovement
extern int windbeeline ( const MapCoordinate& start, const MapCoordinate& dest, WindMovement* wm );

extern bool checkUnitsForCrash( Player& player, ASCString& text );


#endif
