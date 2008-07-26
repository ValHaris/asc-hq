

#ifndef luaRunnerH
#define luaRunnerH

class LuaState;
class ASCString;

extern void executeFile( LuaState& state, const ASCString& filename );
extern void executeCommand( LuaState& state, const ASCString& command );

#endif
