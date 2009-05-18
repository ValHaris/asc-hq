%module asc
%{
#include <string>
#include "commands.h"
#include "../gamemap.h"
%}
GameMap* getActiveMap();

%rename(loadGame) loadGameLua;
GameMap* loadGameLua( const char* filename );

%rename(unitAttack) attackCommandFunc;
int attackCommandFunc( GameMap* actmap, int veh, int x, int y );

%rename(unitMove) moveCommandFunc;
int moveCommandFunc( GameMap* actmap, int veh, int x, int y, int height );