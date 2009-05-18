

extern "C"
{
                   
   #include <lua.h>
   #include <lualib.h>
   #include <lauxlib.h>
                   
}

               
#include "luarunner.h"
#include "luastate.h"
#include "../ascstring.h"
#include "../basestrm.h"

              
LuaRunner::LuaRunner( LuaState& luaState ) : state ( luaState )
{
     
}
               
               
void LuaRunner::runFile( const ASCString& filename )
{
   try {
      tnfilestream stream ( filename, tnstream::reading );
      ASCString line = stream.readString(true);
      while ( line.size() ) {
         
         int errorCode = luaL_loadbuffer(state.get(), line.c_str(), line.size(), stream.getLocation().c_str());
         if ( !errorCode )
            errorCode = lua_pcall(state.get(), 0, 0, 0);
        
         if (errorCode) {
            errors += lua_tostring(state.get(), -1);
            errors += "\n";
            lua_pop(state.get(), 1);  /* pop error message from the stack */
         }
         
         line = stream.readString(true);
      }
   } catch ( treadafterend err ) {
   }
}

void LuaRunner::runCommand( const ASCString& command )
{
   int errorCode = luaL_loadbuffer(state.get(), command.c_str(), command.size(), command.c_str());
   if ( !errorCode )
      errorCode = lua_pcall(state.get(), 0, 0, 0);
        
   if (errorCode) {
      errors += lua_tostring(state.get(), -1);
      errors += "\n";
      lua_pop(state.get(), 1);  /* pop error message from the stack */
   }
}

const ASCString& LuaRunner::getErrors()
{
   return errors;
}

