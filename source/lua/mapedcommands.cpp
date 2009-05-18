

#include "../sg.h"
#include "../ascstring.h"
#include "../vehicle.h"
#include "../gamemap.h"
#include "../spfst.h"
#include "../spfst-legacy.h"
               
#include "../itemrepository.h"


         
         
GameMap* getActiveMap()
{
   return actmap;
}
         
MapCoordinate getCursorPosition( const GameMap* gamemap )
{
   return gamemap->getCursor(); 
}
         
void clearField( GameMap* map, const MapCoordinate& pos )
{
   tfield* fld = map->getField(pos);
   if ( fld ) {
      if ( fld->building )
         delete fld->building;
      
      if ( fld->vehicle )
         delete fld->vehicle;
      
      while ( !fld->objects.empty() )
         fld->removeobject( fld->objects[0].typ );
               
   }  
}
         
const ObjectType* getObjectType( int id )
{
   return objectTypeRepository.getObject_byID( id );   
}

bool placeObject( GameMap* map, const MapCoordinate& pos, const ObjectType* obj, bool force )
{
   tfield* fld = map->getField( pos );
   if ( fld )
      return fld->addobject( obj, -1, force );
   else
      return false;
}

const BuildingType* getBuildingType( int id )
{
   return buildingTypeRepository.getObject_byID( id );   
}

bool placeBuilding( GameMap* map, const MapCoordinate& pos, const BuildingType* bld, int owner )
{
   if ( map && bld ) {
      tfield* fld = map->getField(pos);
      if ( fld ) {
         putbuilding( map, pos, owner, bld, bld->construction_steps );
         if ( fld->building->typ == bld )
            return true;
      }
   }
   return false;
}

const Vehicletype* getUnitType( int id )
{
   return vehicleTypeRepository.getObject_byID( id );   
}


bool placeUnit( GameMap* map, const MapCoordinate& pos, const Vehicletype* veh, int owner )
{
   if ( map && veh ) {
      tfield* fld = map->getField(pos);
      if ( fld ) {
         fld->vehicle = new Vehicle ( veh, map, owner );
         fld->vehicle->setnewposition ( pos );
         fld->vehicle->fillMagically();
         return true;
      }
   }
   return false;
}
