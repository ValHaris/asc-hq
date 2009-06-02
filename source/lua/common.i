%module asc
%include "std_vector.i"
%include "std_string.i"
%{
#include <string>
#include "../gamemap.h"
#include "../mapalgorithms.h"
#include "../util/messaginghub.h"
#include "common.h"
%}

class MapCoordinate {
   protected:
      MapCoordinate();
   public:       
      MapCoordinate(int x, int y );
};


class MapCoordinate3D : public MapCoordinate {
   protected:
      MapCoordinate3D();
   public:       
      MapCoordinate3D(int x, int y, int bitmappedHeight );
};

GameMap* getActiveMap();

const ObjectType* getObjectType( int id );
const BuildingType* getBuildingType( int id );
const Vehicletype* getUnitType( int id );
const TerrainType* getTerrainType( int id );

void errorMessage ( std::string s );
void warningMessage ( std::string s );
void infoMessage ( std::string s );


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
      
      std::string getName();
      
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
      Player& getPlayer( int p );
};

class Player {
   protected:
      Player();
   public:        
      int getPosition();
      bool exist() const;
      bool isHuman() const;
#ifdef mapeditor
      void setName( const char* name );
#endif      
      Research& getResearch();
};


class Research {
   protected:
      Research();
   public:
#ifdef mapeditor
      void addPredefinedTechAdapter( const std::string& techAdapter );
#endif      
         
};