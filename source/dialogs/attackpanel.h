/***************************************************************************
                          cargowdiget.h  -  description
                             -------------------
    begin                : Tue Feb 17 2001
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


#ifndef attackpanelH
#define attackpanelH

class tfight;
class GameMap;
 
/** Performs the calculation of the attack and displays the result on screen.
    The result is not written to the involved units, this must be done separately.

    \param battle  The fight executer
    \param ad  Attacker damage override (used for replays)
    \param dd  Target damage override (used for replays)
*/
extern void showAttackAnimation( tfight& battle, GameMap* actmap, int ad = -1, int dd = -1 );

#endif

