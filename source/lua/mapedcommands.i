%module asc
%include "std_vector.i"
%include "std_string.i"
%include "typemaps.i"
%{
#include <string>
#include "mapedcommands.h"
#include "../gamemap.h"
#include "../mapalgorithms.h"
#include "../util/messaginghub.h"
%}

%include "common.i"

     
void clearField( GameMap* map, const MapCoordinate& pos );

MapCoordinate  getNeighbouringFieldCoordinate( const MapCoordinate& pos, int direc);

Object* placeObject( GameMap* map, const MapCoordinate& pos, const ObjectType* obj, bool force = false );
Building* placeBuilding( GameMap* map, const MapCoordinate& pos, const BuildingType* bld, int owner );
Vehicle* placeUnit( GameMap* map, const MapCoordinate& pos, const Vehicletype* veh, int owner );
bool placeTerrain( GameMap* map, const MapCoordinate& pos, const TerrainType* terrain, int weather = 0 );

int selectPlayer( GameMap* map );

class FieldVector {
   public:
      FieldVector();
      unsigned int size();
   
      MapCoordinate getItem( int i ); 
};

FieldVector getFieldsInDistance( GameMap* map, const MapCoordinate& position, int distance );
MapCoordinate selectPosition();
Resources putResources( ContainerBase* container, const Resources& resources );
void setReactionFire( Vehicle* vehicle, bool state );

