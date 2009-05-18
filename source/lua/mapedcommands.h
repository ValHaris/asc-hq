
#ifndef mapedcommandsH
#define mapedcommandsH

#include "../typen.h"

class GameMap;
class ObjectType;
class BuildingType;
class Vehicletype;

extern GameMap* getActiveMap();

extern MapCoordinate getCursorPosition( const GameMap* gamemap );
      
extern void clearField( GameMap* map, const MapCoordinate& pos );

extern const ObjectType* getObjectType( int id );

extern bool placeObject( GameMap* map, const MapCoordinate& pos, const ObjectType* obj, bool force = false );

extern const BuildingType* getBuildingType( int id );
extern const Vehicletype* getUnitType( int id );

extern bool placeBuilding( GameMap* map, const MapCoordinate& pos, const BuildingType* bld, int owner );

extern bool placeUnit( GameMap* map, const MapCoordinate& pos, const Vehicletype* veh, int owner );

      
#endif