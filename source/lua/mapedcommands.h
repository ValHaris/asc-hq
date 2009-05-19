
#ifndef mapedcommandsH
#define mapedcommandsH

#include "../typen.h"

class GameMap;
class ObjectType;
class BuildingType;
class Vehicletype;
class TerrainType;

extern GameMap* getActiveMap();

extern MapCoordinate getCursorPosition( const GameMap* gamemap );
      
extern void clearField( GameMap* map, const MapCoordinate& pos );

extern const ObjectType* getObjectType( int id );
extern const BuildingType* getBuildingType( int id );
extern const Vehicletype* getUnitType( int id );
extern const TerrainType* getTerrainType( int id );

extern bool placeBuilding( GameMap* map, const MapCoordinate& pos, const BuildingType* bld, int owner );
extern bool placeUnit( GameMap* map, const MapCoordinate& pos, const Vehicletype* veh, int owner );
extern bool placeObject( GameMap* map, const MapCoordinate& pos, const ObjectType* obj, bool force = false );
extern bool placeTerrain( GameMap* map, const MapCoordinate& pos, const TerrainType* terrain, int weather = 0 );
extern int selectPlayer( GameMap* map );

extern void errorMessage ( const char* string );
extern void warningMessage ( const char* string );
extern void infoMessage ( const char* string );
      
      
#endif