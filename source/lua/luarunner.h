

#ifndef luaRunnerH
#define luaRunnerH

#include <list>

#include "../ascstring.h"

class LuaState;

class LuaRunner {
      LuaState& state;
      
      ASCString errors;
      
   public:
      LuaRunner( LuaState& luaState );
      
      void runFile( const ASCString& filename );
      void runCommand( const ASCString& command );

      const ASCString& getErrors();
      
};
#endif
