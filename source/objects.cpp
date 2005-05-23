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
#include "spfst.h"
#include "gamemap.h"


const char* MineNames[cminenum]  = {"antipersonnel mine", "antitank mine", "antisub mine", "antiship mine"};
const int MineBasePunch[cminenum]  = { 60, 120, 180, 180 };



bool Mine :: attacksunit ( const Vehicle* veh )
{
     if  (!( ( veh->typ->functions & cfmineimmune ) ||
              ( veh->height > chfahrend ) ||
              ( getdiplomaticstatus2 ( veh->color, player*8 ) == capeace ) ||
              ( (veh->typ->movemalustyp ==  cmm_trooper) && (type != cmantipersonnelmine)) || 
              ( veh->height <= chgetaucht && type != cmmooredmine ) || 
              ( veh->height == chschwimmend && type != cmfloatmine ) ||
              ( veh->height == chfahrend && type != cmantipersonnelmine  && type != cmantitankmine )
            ))
         return true;
     return false;
}


void Mine::paint( MineTypes type, int player, Surface& surf, SPoint pos )
{
   static Surface* images[5] = { NULL, NULL, NULL, NULL, NULL };
   if ( !images[type] ) {
      switch ( type ) {
         case cmantipersonnelmine: images[type] = & IconRepository::getIcon( "antipersonellmine.png" );
         break;
         case cmantitankmine: images[type] = & IconRepository::getIcon( "antitankmine.png" );
         break;
         case cmfloatmine:
         case cmmooredmine: images[type] = & IconRepository::getIcon( "antishipmine.png" );
         break;
     };
   }
   if ( images[type] ) 
      surf.Blit( *images[type], pos );
}

void Mine::paint( Surface& surf, SPoint pos )
{
   paint( type, player, surf, pos );   
}






bool AgeableItem::age( AgeableItem& obj )
{
   if ( obj.lifetimer > 0 ) {
      --obj.lifetimer;
      return obj.lifetimer==0;
   } else
      return false;
}



Mine::Mine( MineTypes type, int strength, int player, tmap* gamemap )
{
   this->type = type;
   this->strength = strength;
   this->player = player;
   lifetimer = gamemap->getgameparameter( GameParameter(cgp_antipersonnelmine_lifetime + type - 1));
}


Object :: Object ( void )
{
   typ = NULL;
   dir = 0;
   damage = 0;
}

Object :: Object ( pobjecttype t )
{
   lifetimer = t->lifetime;
   typ = t;
   dir = 0;
   damage = 0;
}


void Object :: setdir ( int direc )
{
   dir = direc;
}

int  Object :: getdir ( void )
{
   return dir;
}

void Object :: display ( Surface& surface, SPoint pos, int weather ) const
{
   typ->display ( surface, pos, dir, weather );
}

const OverviewMapImage* Object :: getOverviewMapImage( int weather )
{
   return typ->getOverviewMapImage( dir, weather );
}

