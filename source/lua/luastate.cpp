


extern "C"
{
   #include <lua.h>
   #include <lauxlib.h>
   #include <lualib.h>

   int luaopen_asc(lua_State* L); // declare the wrapped module
}

#include "luastate.h"


class LuaStatePrivate {
   public:
      LuaStatePrivate() : myLuaState( NULL ) {};
      lua_State *myLuaState; 
};


LuaState::LuaState()
{
   myLuaState = lua_open();
   
   luaopen_asc(myLuaState); // declare the wrapped module   
   luaL_openlibs(myLuaState); 
}


LuaState::~LuaState()
{
   lua_close(myLuaState);
}
