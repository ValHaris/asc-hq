
#ifndef commandsH
#define commandsH

class GameMap;

extern GameMap* getActiveMap();
extern int attackCommandFunc( GameMap* actmap, int veh, int x, int y );
extern int moveCommandFunc( GameMap* actmap, int veh, int x, int y, int height );
extern GameMap* loadGameLua( const char* filename );

      
#endif