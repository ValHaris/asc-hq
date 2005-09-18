/*! \file objects.cpp
    \brief Mine and Objects which can be placed on a map
*/

/***************************************************************************
                          gamemap.cpp  -  description
                             -------------------
    begin                : Tue May 21 2005
    copyright            : (C) 2005 by Martin Bickel
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

#include <algorithm>
#include <ctime>
#include <cmath>

#include "global.h"
#include "misc.h"
#include "typen.h"
#include "vehicletype.h"
#include "buildingtype.h"
#include "itemrepository.h"
#include "graphics/blitter.h"
#include "iconrepository.h"
#include "objects.h"
#include "graphics/blitter.h"

#include "spfst.h"
#include "gamemap.h"




bool Mine :: attacksunit ( const Vehicle* veh )
{
     if  (!( ( veh->typ->functions & cfmineimmune ) ||
              ( veh->height > chfahrend ) ||
              ( actmap->getPlayer(player).diplomacy.getState(veh->getOwner()) >= PEACE ) ||
              ( (veh->typ->movemalustyp ==  cmm_trooper) && (type != cmantipersonnelmine)) || 
              ( veh->height <= chgetaucht && type != cmmooredmine ) || 
              ( veh->height == chschwimmend && type != cmfloatmine ) ||
              ( veh->height == chfahrend && type != cmantipersonnelmine  && type != cmantitankmine )
            ))
         return true;
     return false;
}



void Mine::paint( Surface& surf, SPoint pos )
{
   MineType::paint( type, player, surf, pos );   
}



Mine::Mine( MineTypes type, int strength, int player, tmap* gamemap )
{
   this->type = type;
   this->strength = strength;
   this->player = player;
   #ifndef converter
   lifetimer = gamemap->getgameparameter( GameParameter(cgp_antipersonnelmine_lifetime + type - 1));
   #endif
}

