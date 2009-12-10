
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




bool Mine :: attacksunit ( const Vehicle* veh ) const
{
   #ifndef converter
   if  (!( ( veh->typ->hasFunction( ContainerBaseType::ImmuneToMines  ) ) ||
              ( veh->height > chfahrend ) ||
              ( veh->getMap()->getPlayer(player).diplomacy.getState(veh->getOwner()) >= PEACE ) ||
              ( (veh->typ->movemalustyp ==  cmm_trooper) && (type != cmantipersonnelmine)) || 
              ( veh->height <= chgetaucht && type != cmmooredmine ) || 
              ( veh->height == chschwimmend && type != cmfloatmine ) ||
              ( veh->height == chfahrend && type != cmantipersonnelmine  && type != cmantitankmine )
            ))
         return true;
#endif
     return false;
}



void Mine::paint( Surface& surf, SPoint pos ) const
{
   MineType::paint( type, player, surf, pos );   
}



Mine::Mine( MineTypes type, int strength, int player, GameMap* gamemap )
{
   this->type = type;
   this->strength = strength;
   this->player = player;
   this->identifier = gamemap->idManager.getNewNetworkID();
   #ifndef converter
   lifetimer = gamemap->getgameparameter( GameParameter(cgp_antipersonnelmine_lifetime + type - 1));
   #endif
}

Mine::Mine( MineTypes type, int strength, int player, GameMap* gamemap, int identifier )
{
   this->type = type;
   this->strength = strength;
   this->player = player;
   this->identifier = identifier;
   #ifndef converter
   lifetimer = gamemap->getgameparameter( GameParameter(cgp_antipersonnelmine_lifetime + type - 1));
   #endif
}

const int mineVersion = 1;
void Mine::read ( tnstream& stream )
{
   int version = stream.readInt();
   if ( version < 1 || version > mineVersion ) 
      throw tinvalidversion ( "Mine", mineVersion, version );
      
   identifier = stream.readInt();
   type = MineTypes( stream.readInt() );
   strength = stream.readInt();
   player = stream.readInt();
}

void Mine::write ( tnstream& stream ) const
{
   stream.writeInt( mineVersion );
   stream.writeInt( identifier );
   stream.writeInt( type );
   stream.writeInt( strength );
   stream.writeInt( player );
}

Mine::Mine()
{
}

Mine Mine::newFromStream ( tnstream& stream )
{
   Mine m;
   m.read( stream );
   return m;  
}
