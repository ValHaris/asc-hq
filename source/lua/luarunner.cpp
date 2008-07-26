

extern "C"
{
                   
   #include <lua.h>
   #include <lualib.h>
   #include <lauxlib.h>
                   
}



#include <luabind/luabind.hpp>
#include <luabind/function.hpp>
#include <luabind/class.hpp>
               
#include "luarunner.h"
#include "luastate.h"
#include "../ascstring.h"
#include "../basestrm.h"

void executeFile( LuaState& state, const ASCString& filename )
{
   try {
   tnfilestream stream ( filename, tnstream::reading );
   ASCString line = stream.readString(true);
   while ( line.size() ) {
      
        int error = luaL_loadbuffer(state.get(), line.c_str(), line.size(), stream.getLocation().c_str());
        if ( !error )
           error = lua_pcall(state.get(), 0, 0, 0);
        
        if (error) {
          fprintf(stderr, "Lua error: %s\n", lua_tostring(state.get(), -1));
          lua_pop(state.get(), 1);  /* pop error message from the stack */
        }
        line = stream.readString(true);
    }
   } catch ( treadafterend err ) {
   }

}

void executeCommand( LuaState& state, const ASCString& command )
{
 
 
   // Define a lua function that we can call
   // luaL_dostring( state.get(), command.c_str() );
 
}


