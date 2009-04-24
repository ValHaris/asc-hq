/***************************************************************************
                          autotraining.h  -  description
                             -------------------
    copyright            : (C) 2009 by Martin Bickel
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



#ifndef autotrainingH
#define autotrainingH
 class Player;
 class GameMap;
 /** trains all units that are in training centers, which increases their experience at the cost
     of time (units lose all movement) and ammo */
 extern void automaticTrainig( GameMap* gameMap, Player& player );
#endif
