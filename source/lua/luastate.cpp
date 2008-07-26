


extern "C"
{
    #include "lua.h"
}

#include <luabind/luabind.hpp>

#include "luastate.h"
#include "binder.h"


class LuaStatePrivate {
   public:
      LuaStatePrivate() : myLuaState( NULL ) {};
      lua_State *myLuaState; 
};


LuaState::LuaState()
{
   myLuaState = lua_open();
   // Connect LuaBind to this lua state
   luabind::open( myLuaState );
   registerLuaFunctions( *this );
}


LuaState::~LuaState()
{
   lua_close(myLuaState);
}
