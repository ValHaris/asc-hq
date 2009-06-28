%module asc
%include "std_vector.i"
%include "std_string.i"
%{
#include <string>
#include "../gamemap.h"
#include "../mapalgorithms.h"
#include "../util/messaginghub.h"
#include "../textfiletags.h"
#include "common.h"
%}

class Resources {
   public:
      %immutable;
      
      int energy;
      int material;
      int fuel;
      
      %mutable;

      Resources();
      Resources( int energy, int material, int fuel );
          
};

class MapCoordinate {
   protected:
      MapCoordinate();
   public:       
      MapCoordinate(int x, int y );
      bool valid();
};


class MapCoordinate3D : public MapCoordinate {
   protected:
      MapCoordinate3D();
   public:       
      MapCoordinate3D(int x, int y, int bitmappedHeight );
      bool valid();
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

class TerrainType {
   protected:
      TerrainType();
   public:
      int getID() const;
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
      
      int getOwner();
      
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
      int getWeather();
      
      int getMineralMaterial() const;
      int getMineralFuel() const;
    
      void setMineralMaterial( int material );
      void setMineralFuel( int material );
      
      %extend 
      {
         bool hasProperty ( std::string bitName ) // see textfiletags.cpp for value
         {
            for ( int i = 0; i < terrainPropertyNum; ++i )
               if ( terrainProperties[i] == bitName )
                  return $self->bdt.test(i);
            return false;
         }
      }
#ifdef mapeditor
      void setWeather( int weather );
      bool removeObject ( const ObjectType* obj, bool force = false );
#endif      
     Object* checkForObject ( const ObjectType*  o );
   protected:
      tfield();
};


class Properties {      
   public:
      std::string getValue( const std::string& key );
      void setValue( const std::string& key, const std::string& value );
};


class GameMap {
   public:
#ifdef mapeditor
      // in ASC, the scripts are not able to access all fields, as they are running in a user context
      tfield* getField( int x, int y );
      tfield* getField( const MapCoordinate& pos );
#endif      
      int width() const;
      int height() const;
      Player& getPlayer( int p );
      
      Properties& getProperties();
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


class StringArray {
   public:
      StringArray();
      void add( const std::string& s );
      std::string getItem( int n ); //!< 1 based, in best LUA tradition 
      int size(); //
};


class PropertyDialog  {
   public:
      PropertyDialog( const std::string& title );
      void addBool( const std::string& name, bool defaultValue );
      void addInteger( const std::string& name, int defaultValue );
      
      // returns and integer, so query result with getInteger
      void addIntDropdown ( const std::string& name, const StringArray& names, int defaultValue );
      
      void addString( const std::string& name, const std::string& defaultValue );
      
      std::string getString( const std::string& name );
      int getInteger( const std::string& name );
      bool getBool( const std::string& name );
      bool run();
};

int selectString ( const std::string& title, const StringArray& entries, int defaultEntry = -1 );

/* this is a very special function for usage in conjunction with setLocalizedEventMessage , to get the map that 
   is being loaded before it is set active. Only for translation scripts */
GameMap* getLoadingMap();

void setLocalizedEventMessage( GameMap* map, int eventID, const std::string& message );


