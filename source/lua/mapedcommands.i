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

MapCoordinate  getNeighbouringFieldCoordinate( const MapCoordinate& pos, int direc);


bool placeObject( GameMap* map, const MapCoordinate& pos, const ObjectType* obj, bool force = false );
bool placeBuilding( GameMap* map, const MapCoordinate& pos, const BuildingType* bld, int owner );
bool placeUnit( GameMap* map, const MapCoordinate& pos, const Vehicletype* veh, int owner );
