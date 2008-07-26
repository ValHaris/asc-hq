

extern "C"
{
    #include "lua.h"
}

#include <luabind/luabind.hpp>
#include <luabind/function.hpp>
#include <luabind/class.hpp>


#include "binder.h"
#include "luastate.h"

#include "../sg.h"
#include "../ascstring.h"
               
               

using namespace luabind;

void loadGameLua( const std::string& filename )
{
   loadGameFromFile( ASCString(filename) );  
}
         
void registerLuaFunctions( LuaState& state)
{

    module( state.get() )
    [
        def("loadgame", &loadGameLua )
    ]; 
   
}
