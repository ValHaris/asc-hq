

#ifndef luastateH
#define luastateH

struct lua_State;

class LuaState {
   private:
      lua_State *myLuaState; 
   public:
      LuaState();
      lua_State* get() { return myLuaState; };
      ~LuaState();
   
};


#endif
