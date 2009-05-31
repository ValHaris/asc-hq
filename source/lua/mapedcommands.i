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

void errorMessage ( const char* string );
void warningMessage ( const char* string );
void infoMessage ( const char* string );

class ContainerBaseType {
   public: 
      int getID() const;
   protected:
      ContainerBaseType();
};

class BuildingType : public ContainerBaseType {
};

class Vehicletype : public ContainerBaseType {
};

class ContainerBase {
   public:         
      void deleteProductionLine( const Vehicletype* type );
      void deleteAllProductionLines();
      void addProductionLine( const Vehicletype* type  );
      
      int getCargoCount();
      
      // warning: the cargo may have items which are NULL
      Vehicle* getCargo( int i );
      
   protected:
      ContainerBase();
};

class Building : public ContainerBase {
   protected:
      Building();
   public:
      const BuildingType* getType();
};

class Vehicle : public ContainerBase {
   protected:       
      Vehicle();
   public:
      const Vehicletype* getType();
};

class tfield {
   public:
      Building* getBuildingEntrance();
      Vehicle* getVehicle();
   protected:
      tfield();
};

class GameMap {
   public:
      tfield* getField( int x, int y );
      int width() const;
      int height() const;
      
};
