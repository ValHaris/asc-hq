

#include <iostream>

#include "../ascstring.h"
#include "../vehicle.h"
#include "../gamemap.h"
#include "../spfst.h"
#include "../spfst-legacy.h"
               
#include "../itemrepository.h"
#include "../dlg_box.h"
#include "mapedcommands.h"
#include "../ed_mapcomponent.h"
#include "../dialogs/fieldmarker.h"
#include "../edglobal.h"
         
         
         
         
void clearField( GameMap* map, const MapCoordinate& pos )
{
   MapField* fld = map->getField(pos);
   if ( fld ) {
      if ( fld->building )
         delete fld->building;
      
      if ( fld->vehicle )
         delete fld->vehicle;
      
      while ( !fld->objects.empty() )
         fld->removeObject( fld->objects[0].typ );
               
   }  
}
         
Object* placeObject( GameMap* map, const MapCoordinate& pos, const ObjectType* obj, bool force )
{
   MapField* fld = map->getField( pos );
   if ( fld ) {
      if ( !fld->addobject( obj, -1, force ))
         return NULL;
      else
         return fld->checkForObject(obj);
   } else
      return NULL;
}

Building* placeBuilding( GameMap* map, const MapCoordinate& pos, const BuildingType* bld, int owner )
{
   if ( map && bld && owner >= 0 && owner <= 8 ) {
      MapField* fld = map->getField(pos);
      if ( fld ) {
         putbuilding( map, pos, owner*8, bld, bld->construction_steps );
         if ( fld->building && (fld->building->typ == bld ))
            return fld->building;
      }
   }
   return NULL;
}


Vehicle* placeUnit( GameMap* map, const MapCoordinate& pos, const VehicleType* veh, int owner )
{
   if ( map && veh && owner >= 0 && owner < 8 ) {
      MapField* fld = map->getField(pos);
      if ( fld ) {
         int r = VehicleItem::place( map, pos, veh, owner );
         if ( r < 0 )
            return NULL;
         else
            return fld->vehicle;
      }
   }
   return NULL;
}


bool placeTerrain( GameMap* map, const MapCoordinate& pos, const TerrainType* terrain, int weather )
{
   if ( map && terrain ) {
      MapField* fld = map->getField(pos);
      fld->typ = terrain->weather[0]; 
      fld->setWeather( weather );
      fld->setparams( );
      for ( int d = 0; d < 6; ++d ) {
         MapCoordinate pos2 = getNeighbouringFieldCoordinate( pos, d );
         MapField* fld = map->getField( pos2 );
         if ( fld ) 
            for ( MapField::ObjectContainer::iterator i = fld->objects.begin(); i != fld->objects.end(); ++i )
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

FieldVector::FieldVector() 
{
}
      
unsigned int FieldVector::size() {
  return vector<MapCoordinate>::size();
}

MapCoordinate FieldVector::getItem( int i ) {
   return at(i-1);  
}



class LuaFieldSearcher : public SearchFields {
   public:
      FieldVector fields;
      
      LuaFieldSearcher ( GameMap* _gamemap ) : SearchFields ( _gamemap ) {
           
      };
       
   protected:
      void testfield ( const MapCoordinate& pos ) {
         fields.push_back(pos); 
      };
      
};
      
      
FieldVector getFieldsInDistance( GameMap* map, const MapCoordinate& position, int distance )
{
   LuaFieldSearcher lfs ( map );
   lfs.initsearch( position, distance, distance );
   lfs.startsearch();
   return lfs.fields;
}

MapCoordinate selectPosition()
{
   SelectFromMap::CoordinateList list;
   SelectFromMap sfm( list, actmap, true );
   sfm.Show();
   sfm.RunModal();
   if ( list.size () )
      return *list.begin();
   else
      return MapCoordinate(-1,-1);
         
}

Resources putResources( ContainerBase* container, const Resources& resources )
{
   return container->putResource( resources, false, 0, container->getOwner() );
}

void setReactionFire( Vehicle* vehicle, bool state )
{
   if ( state )
      vehicle->reactionfire.enable();
   else
      vehicle->reactionfire.disable();
}

int EditingEnvironment :: getSelectedPlayerPosition() const
{
   return selection.getPlayer();
}

int EditingEnvironment :: getBrushSize() const
{
   return selection.brushSize;
}


EditingEnvironment getEditingEnvironment() 
{
   return EditingEnvironment();
}
