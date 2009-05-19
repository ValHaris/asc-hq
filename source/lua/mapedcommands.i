%module asc
%{
#include <string>
#include "mapedcommands.h"
#include "../gamemap.h"
#include "../mapalgorithms.h"
%}
GameMap* getActiveMap();

MapCoordinate getCursorPosition( const GameMap* gamemap );
      
void clearField( GameMap* map, const MapCoordinate& pos );

const ObjectType* getObjectType( int id );
const BuildingType* getBuildingType( int id );
const Vehicletype* getUnitType( int id );
const TerrainType* getTerrainType( int id );

MapCoordinate  getNeighbouringFieldCoordinate( const MapCoordinate& pos, int direc);

bool placeObject( GameMap* map, const MapCoordinate& pos, const ObjectType* obj, bool force = false );
bool placeBuilding( GameMap* map, const MapCoordinate& pos, const BuildingType* bld, int owner );
bool placeUnit( GameMap* map, const MapCoordinate& pos, const Vehicletype* veh, int owner );
bool placeTerrain( GameMap* map, const MapCoordinate& pos, const TerrainType* terrain, int weather = 0 );

int selectPlayer( GameMap* map );

extern void errorMessage ( const char* string );
extern void warningMessage ( const char* string );
extern void infoMessage ( const char* string );


