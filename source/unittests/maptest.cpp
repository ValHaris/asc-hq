/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <iostream>

#include "../actions/jumpdrivecommand.h"
#include "../loaders.h"
#include "../itemrepository.h"
#include "unittestutil.h"
#include "../gamemap.h"
#include "../lua/luarunner.h"
#include "../lua/luastate.h"
#include "../spfst-legacy.h"


void runScript( const ASCString& script ) 
{
   LuaState state;
   LuaRunner runner( state );
   runner.runFile( script );
   assertOrThrow( runner.getErrors().empty() );
}

void testMaps() 
{
   auto_ptr<GameMap> game ( startMap("kam005.map"));
   actmap = game.get();
   runScript( "kam005.lua" );
   actmap = NULL;
}

