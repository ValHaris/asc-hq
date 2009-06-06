%module asc
%include "std_vector.i"
%include "std_string.i"
%{
#include <string>
#include "mapedcommands.h"
#include "../gamemap.h"
#include "../mapalgorithms.h"
#include "../util/messaginghub.h"
%}

%include "common.i"


MapCoordinate getCursorPosition( const GameMap* gamemap );
      
void clearField( GameMap* map, const MapCoordinate& pos );

MapCoordinate  getNeighbouringFieldCoordinate( const MapCoordinate& pos, int direc);

bool placeObject( GameMap* map, const MapCoordinate& pos, const ObjectType* obj, bool force = false );
bool placeBuilding( GameMap* map, const MapCoordinate& pos, const BuildingType* bld, int owner );
bool placeUnit( GameMap* map, const MapCoordinate& pos, const Vehicletype* veh, int owner );
bool placeTerrain( GameMap* map, const MapCoordinate& pos, const TerrainType* terrain, int weather = 0 );

int selectPlayer( GameMap* map );


