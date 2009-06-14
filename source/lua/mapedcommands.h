
#ifndef mapedcommandsH
#define mapedcommandsH

#include "../typen.h"

class GameMap;
class ObjectType;
class BuildingType;
class Vehicletype;
class TerrainType;
class Building;
class Vehicle;
class Object;

extern MapCoordinate getCursorPosition( const GameMap* gamemap );
      
extern void clearField( GameMap* map, const MapCoordinate& pos );

extern Building* placeBuilding( GameMap* map, const MapCoordinate& pos, const BuildingType* bld, int owner );
extern Vehicle* placeUnit( GameMap* map, const MapCoordinate& pos, const Vehicletype* veh, int owner );
extern Object* placeObject( GameMap* map, const MapCoordinate& pos, const ObjectType* obj, bool force = false );
extern bool placeTerrain( GameMap* map, const MapCoordinate& pos, const TerrainType* terrain, int weather = 0 );
extern int selectPlayer( GameMap* map );
extern MapCoordinate selectPosition();


class FieldVector : public vector<MapCoordinate> {
   public:
      FieldVector();
      unsigned int size();
   
      MapCoordinate getItem( int i );  // i is 1..N in best LUA tradition...
};


extern FieldVector getFieldsInDistance( GameMap* map, const MapCoordinate& position, int distance );

      
#endif
