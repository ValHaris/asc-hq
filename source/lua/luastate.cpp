


extern "C"
{
   #include "lua.h"
   #include <lauxlib.h>

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
}


LuaState::~LuaState()
{
   lua_close(myLuaState);
}
