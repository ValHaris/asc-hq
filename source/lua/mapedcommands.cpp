

#include "../sg.h"
#include "../ascstring.h"
#include "../vehicle.h"
#include "../gamemap.h"
#include "../spfst.h"
#include "../spfst-legacy.h"
               
#include "../itemrepository.h"
#include "../dlg_box.h"

         
         
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
   if ( map && bld && owner >= 0 && owner < 8 ) {
      tfield* fld = map->getField(pos);
      if ( fld ) {
         putbuilding( map, pos, owner*8, bld, bld->construction_steps );
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
   if ( map && veh && owner >= 0 && owner < 8 ) {
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


const TerrainType* getTerrainType( int id )
{
   return terrainTypeRepository.getObject_byID( id );   
}

bool placeTerrain( GameMap* map, const MapCoordinate& pos, const TerrainType* terrain, int weather )
{
   if ( map && terrain ) {
      tfield* fld = map->getField(pos);
      fld->typ = terrain->weather[0]; 
      fld->setweather( weather );
      fld->setparams();
      for ( int d = 0; d < 6; ++d ) {
         MapCoordinate pos2 = getNeighbouringFieldCoordinate( pos, d );
         tfield* fld = map->getField( pos2 );
         if ( fld ) 
            for ( tfield::ObjectContainer::iterator i = fld->objects.begin(); i != fld->objects.end(); ++i )
               calculateobject( pos2, false, i->typ, map );
      }
      return true;
   } else
      return false;
}

int selectPlayer( GameMap* map )
{
   vector<ASCString> buttons;
   buttons.push_back ( "~O~k" );
   buttons.push_back ( "~C~ancel" );

   vector<ASCString> player;
   for ( int i = 0; i < 8; ++i )
      player.push_back ( ASCString ( strrr(i)) + " " + map->player[i].getName());

   pair<int,int> playerRes = chooseString ( "Choose Player", player, buttons );
   if ( playerRes.first == 0 && playerRes.second >= 0) 
      return playerRes.second;
   else
      return -1;
}

void errorMessage ( const char* string )
{
   errorMessage( ASCString( string ));
}

void warningMessage ( const char* string )
{
   warning( ASCString( string ) );
}

void infoMessage ( const char* string )
{
   infoMessage( ASCString( string ) );
}
