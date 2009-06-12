/*
    This file is part of Advanced Strategic Command; http://www.asc-hq.de
    Copyright (C) 1994-1999  Martin Bickel  and  Marc Schellenberger

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

#include "ed_mapcomponent.h"
#include "edglobal.h"
#include "spfst.h"
#include "spfst-legacy.h"

#include "lua/luastate.h"
#include "lua/luarunner.h"

int MapComponent::currentPlayer = 0;
bool MapComponent::initialized = false;

void MapComponent::setPlayer( int player )
{
   currentPlayer = player;
};

void VehicleItem :: display( Surface& s, const SPoint& pos ) const { 
   if ( actmap ) {
      int player = selection.getPlayer();
      if ( player > 7 || player < 0 )
         player = 0;
      item->paint ( s, pos, actmap->getPlayer( player ).getPlayerColor()); 
   } else
      item->paint ( s, pos ); 
};


MapComponent::MapComponent( const MapItemType* item ) : mapItem( item )
{
   if ( !initialized ) {
      initialized = true;
      selection.playerChanged.connect( SigC::slot( &MapComponent::setPlayer ));
         
   }
}

LuaBrush::LuaBrush( const ASCString& filename ) : script(filename)
{
   
}

int LuaBrush :: place( const MapCoordinate& mc ) const
{
   LuaState state;
   LuaRunner runner( state );
   runner.runFile( script );
   if ( !runner.getErrors().empty() )
      errorMessage( runner.getErrors() );
   updateFieldInfo();
   return 1;
}

LuaBrush* LuaBrush::clone() const
{
	return new LuaBrush( script );
}




      
