/*! \file mapfield.cpp
    \brief The fields on which the action takes place
*/

/***************************************************************************
                          mapfield.cpp  -  description
                             -------------------
    begin                : Tue May 21 2005
    copyright            : (C) 2000 by Martin Bickel
    email                : bickel@asc-hq.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <algorithm>
#include <ctime>
#include <cmath>

#include "global.h"
#include "typen.h"
#include "vehicletype.h"
#include "buildingtype.h"
#include "spfst.h"
#include "actions/context.h"
#include "actions/removeobject.h"

void MapField::init ()
{
   bdt.set ( 0 );
   typ = NULL;
   vehicle = NULL;
   secondvehicle = NULL;
   building = NULL;
   visible = 0;
   fuel = 0;
   material = 0;
   resourceview = NULL;
   connection = 0;
   gamemap = NULL;
   viewbonus = 0;
}

void MapField::setupNeighboringFields() {
   for (int d = 0; d < 6; ++d) {
      neighboringFields[d] = gamemap->getField(getx() + getnextdx(d, gety()), gety() + getnextdy(d));
   }
}

void MapField::setaTemp (char temp) {
   gamemap->temp[index] = temp;
};
char MapField::getaTemp () {
   return gamemap->temp[index];
};

void MapField::setaTemp2 (char temp2) {
   gamemap->temp2[index] = temp2;
};
char MapField::getaTemp2 () {
   return gamemap->temp2[index];
};

void MapField::setTempw (Uint16 tempw) {
   gamemap->temp[index] = tempw>>8;
   gamemap->temp2[index] = tempw & 0xFF;
};
Uint16 MapField::getTempw () {
   return gamemap->temp[index]<<8 | gamemap->temp2[index];
};

void MapField::setTemp3 (char temp3) {
   gamemap->temp3[index] = temp3;
};
char MapField::getTemp3 () {
   return gamemap->temp3[index];
};

void MapField::setTemp4 (char temp4) {
   gamemap->temp4[index] = temp4;
};
char MapField::getTemp4 () {
   return gamemap->temp4[index];
};

int MapField::getMineralMaterial() const
{
   return material;
}

int MapField::getMineralFuel() const
{
   return fuel;
}
   
void MapField::setMineralMaterial( int material )
{
   if ( material < 0 )
      this->material = 0;
   else
      if ( material > 255 )
         this->material = 255;
      else
         this->material = material;
}
void MapField::setMineralFuel( int fuel )
{
   if ( fuel < 0 )
      this->fuel = 0;
   else
      if ( fuel > 255 )
         this->fuel = 255;
   else
      this->fuel = fuel;
}

void MapField::Resourceview::setview( int player, int material, int fuel )
{
   visible |= 1 << player;
   materialvisible[player] = material;
   fuelvisible[player] = fuel;
}

void MapField::Resourceview::resetview( int player )
{
   visible &= ~(1<<player);
}


void MapField::endRound( int turn )
{
   bool recalc = false;
   for ( ObjectContainer::iterator i = objects.begin(); i != objects.end(); ) {
      if ( AgeableItem::age( *i )) {
         i = objects.erase(i);
         recalc = true;
      } else
         ++i;
   }
   // remove_if( objects.begin(), objects.end(), Object::age );

   for ( MineContainer::iterator i = mines.begin(); i != mines.end(); ) {
      if ( AgeableItem::age( *i )) {
         i = mines.erase(i);
         recalc = true;
      } else
         ++i;
   }
   // remove_if( mines.begin(), mines.end(), Object::age );

   if ( recalc )
      setparams();
}


void MapField::operator= ( const MapField& f )
{
   typ = f.typ;
   fuel = f.fuel;
   material = f.material;
   visible = f.visible;
   vehicle = f.vehicle;
   building = f.building;
   if ( f.resourceview ) {
      resourceview = new Resourceview;
      *resourceview = *f.resourceview;
   } else
      resourceview = NULL;
   mines = f.mines;
   objects = f.objects;
   connection = f.connection;
   bdt = f.bdt;
   for ( int i = 0; i < 8; i++ )
      view[i] = f.view[i];
}


const TerrainType* MapField::getTerrainType() const 
{
   return typ->terraintype;
}

void MapField::changeTerrainType( const TerrainType* terrain )
{
   int weather = getWeather();
   if ( terrain->weather[weather] )
      typ = terrain->weather[weather];
   else
      typ = terrain->weather[0];
   
   setparams ();
}


int MapField :: mineattacks ( const Vehicle* veh )
{
   int i = 1;
   for ( MineContainer::iterator m = mines.begin(); m != mines.end(); m++, i++ )
      if ( m->attacksunit ( veh ))
         return i;

   return 0;
}

Mine& MapField::getMine ( int n )
{
  int c = 0;
  MineContainer::iterator i;
  for ( i = mines.begin(); c < n; i++,c++ );
  return *i;
}

bool  MapField :: addobject( const ObjectType* obj, int dir, bool force, MapField::ObjectRemovalStrategy* objectRemovalStrategy )
{
   if ( !obj )
      return false;

   Object* i = checkForObject ( obj );
   if ( !i ) {
     int buildable = obj->buildable ( this );
     if ( !buildable )
          if ( force )
             buildable = 1;

     if ( buildable ) {
         Object o ( obj );
         if ( dir != -1 )
            o.dir = dir;
         else
            o.dir = 0;

         objects.push_back ( o );

         sortobjects();
         if ( dir == -1 )
            calculateobject( this, true, obj, gamemap );

         if ( objectRemovalStrategy )
            setparams( objectRemovalStrategy );
         else
            setparams();
         return true;
     } else
        return false;
     
   } else {
      if ( dir != -1 )
         i->dir |= dir;

      i->lifetimer = obj->lifetime;
      sortobjects();
      return true;
   }
}


bool MapField :: removeObject( const ObjectType* obj, bool force, ObjectRemovalStrategy* objectRemovalStrategy )
{
   if ( !force && building )
      return false;

   #ifndef karteneditor
   if ( !force )
      if ( vehicle )
         if ( vehicle->getOwner() != gamemap->actplayer )
           return false;
   #endif

   bool removed = false;
   
   if ( !obj ) {
      if ( objects.size() ) {
         obj = objects.rbegin()->typ;
         objects.pop_back();
         removed = true;
      }
   } else
      for ( ObjectContainer::iterator o = objects.begin(); o != objects.end(); )
         if ( o->typ == obj ) {
            o = objects.erase( o );
            removed = true;
         } else
            o++;

   if ( objectRemovalStrategy )
      setparams( objectRemovalStrategy );
   else
      setparams();
   
   if ( obj )
      calculateobject( this, true, obj, gamemap );
   
   return removed;
}

void MapField :: deleteeverything ( void )
{
   if ( vehicle ) {
      delete vehicle;
      vehicle = NULL;
   }

   if ( building ) {
      delete building;
      building = NULL;
   }

   setparams();
}


bool MapField :: unitHere ( const Vehicle* veh )
{
   if ( vehicle == veh )
      return true;

   if ( vehicle && veh && vehicle->networkid == veh->networkid )
      return true;
   return false;
}

Building* MapField::getBuildingEntrance()
{
   if ( building &&  (bdt & getTerrainBitType(cbbuildingentry)).any() )
      return building;
   else
      return NULL;
}



ContainerBase* MapField :: getContainer()
{
   if ( vehicle )
      return vehicle;
   else
      return building;
}

const ContainerBase* MapField :: getContainer() const
{
   if ( vehicle )
      return vehicle;
   else
      return building;
}


void MapField :: removeBuilding() 
{
   if ( building )
      delete building;
}

void MapField :: removeUnit()
{
   if ( vehicle ) {
      delete vehicle;
      vehicle = NULL;
   }
}


int MapField :: getWeather ( void )
{
   if ( !typ )
      return 0;
   for ( int w = 0; w < cwettertypennum; w++ )
      if ( typ == typ->terraintype->weather[w] )
         return w;
   return -1;
}

void MapField :: setWeather ( int weather )
{
   if ( weather < 0 || weather >= cwettertypennum )
      return;
   
     if (typ->terraintype->weather[ weather ] ) {
        typ = typ->terraintype->weather[ weather ];
        setparams();
     } else {
        if ( weather == 2 )
           setWeather(1);
        else
           if ( weather == 5 )
              setWeather(4);
           else
              if (weather==4)
                 setWeather(3);
              else {
                 typ = typ->terraintype->weather[ 0 ];
                 setparams();
              }
     }
}

void MapField::setVisibility ( VisibilityStates valtoset, int actplayer ) 
{
      int newval = (valtoset ^ 3) << ( 2 * actplayer );
      int oneval = 3 << ( 2 * actplayer );

      visible |= oneval;
      visible ^= newval;
};

void MapField::resetView( GameMap* gamemap, int playersToReset )
{
   int mask = 0;
   for ( int i = 0; i < gamemap->getPlayerCount(); ++i )
      if ( !(playersToReset & (1 << i)))
         mask |= 3 << (2*i);

   int l = 0;
   for ( int y = 0; y < gamemap->ysize; ++y )
      for ( int x = 0; x < gamemap->xsize; ++x ) {
         MapField& fld = gamemap->field[l++];
         fld.visible &= mask;
      }
        
}



bool compareObjectHeight ( const Object& o1, const Object& o2 )
{
   return o1.typ->imageHeight < o2.typ->imageHeight;
}

void MapField :: sortobjects ( void )
{
   sort ( objects.begin(), objects.end(), compareObjectHeight );
}

bool  MapField :: putmine( int owner, MineTypes typ, int strength )
{
   if ( mineowner() >= 0  && mineowner() != owner )
      return false;

   if ( mines.size() >= gamemap->getgameparameter ( cgp_maxminesonfield ))
      return false;

   Mine mymine ( typ, strength, owner, gamemap );
   mines.push_back ( mymine );
   return true;
}

int MapField :: mineowner( void )
{
   if ( mines.empty() )
      return -1;
   else
      return mines.begin()->player;
}


void MapField :: removemine( int num )
{ 
   if ( num == -1 )
      num = mines.size() - 1;

   int i = 0;
   for ( MineContainer::iterator m = mines.begin(); m != mines.end(); i++)
      if ( i == num )
         m = mines.erase ( m );
      else
          m++;
}


int MapField :: getx( void )
{
   int n = this - gamemap->field;
   return n % gamemap->xsize;
}

int MapField :: gety( void )
{
   int n = this - gamemap->field;
   return n / gamemap->xsize;
}


int MapField :: getattackbonus ( void )
{
   int a = typ->attackbonus;
   for ( ObjectContainer::iterator o = objects.begin(); o != objects.end(); o++ ) {
      if ( o->typ->attackbonus_abs != -1 )
         a = o->typ->attackbonus_abs;
      else
         a += o->typ->attackbonus_plus;
   }

   if ( a > -8 )
      return a;
   else
      return -7;
}

int MapField :: getdefensebonus ( void )
{
   int a = typ->defensebonus;
   for ( ObjectContainer::iterator o = objects.begin(); o != objects.end(); o++ ) {
      if ( o->typ->defensebonus_abs != -1 )
         a = o->typ->defensebonus_abs;
      else
         a += o->typ->defensebonus_plus;
   }

   if ( a > -8 )
      return a;
   else
      return -7;
}

ASCString MapField :: getName()
{
   ASCString a = typ->terraintype->name;
   for ( ObjectContainer::iterator o = objects.begin(); o != objects.end(); o++ ) {
      if ( o->typ->namingMethod == ObjectType::ReplaceTerrain )
         a = o->typ->getName();
      else
         if ( o->typ->namingMethod == ObjectType::AddToTerrain )
            a += ", " + o->typ->getName();
   }
   
   return a;
}


int MapField :: getjamming ( void )
{
   int a = typ->basicjamming;
   for ( ObjectContainer::iterator o = objects.begin(); o != objects.end(); o++ ) {
      if ( o->typ->basicjamming_abs >= 0 )
         a = o->typ->basicjamming_abs;
      else
         a += o->typ->basicjamming_plus;
   }
   if ( a > 0 )
      return a;
   else
      return 0;
}

int MapField :: getmovemalus ( const Vehicle* veh )
{
   int mnum = mines.size();
   if ( mnum ) {
      int movemalus = __movemalus.at(veh->typ->movemalustyp);
      int col = mineowner();
      if ( veh->color == col*8 )
         movemalus += movemalus * mine_movemalus_increase * mnum / 100;

      if ( movemalus < minmalq )
         fatalError ( "invalid movemalus for terraintype ID %d used on field %d / %d" , typ->terraintype->id, getx(), gety() );

      return movemalus;
   } else {
      int mm = __movemalus.at(veh->typ->movemalustyp);
      if ( mm < minmalq )
         fatalError ( "invalid movemalus for terraintype ID %d used on field %d / %d" , typ->terraintype->id, getx(), gety() );
      return mm;
   }
}

int MapField :: getmovemalus ( int type )
{
  return __movemalus.at(type);
}

MapCoordinate MapField :: getPosition()
{
   return MapCoordinate( getx(), gety() );  
}


class SimpleObjectRemoval : public MapField::ObjectRemovalStrategy {
   public:
      virtual void removeObject( MapField* fld, const ObjectType* obj )
      {
         for ( MapField::ObjectContainer::iterator o = fld->objects.begin(); o != fld->objects.end();  ) {
            if ( o->typ == obj )
               o = fld->objects.erase( o );
            else
               ++o ;
         }
      }
};



void MapField :: setparams (  )
{
   SimpleObjectRemoval sor;
   setparams( &sor );     
}

void MapField :: setparams ( ObjectRemovalStrategy* objectRemovalStrategy )
{
   int i;
   bdt = typ->art;

   for ( i = 0; i < cmovemalitypenum; i++ )   {
      __movemalus.at(i) = typ->move_malus[i];
      if ( __movemalus[i] < minmalq )
         fatalError ( "invalid movemalus for terraintype ID %d used on field %d / %d" , typ->terraintype->id, getx(), gety() );
   }

   viewbonus = 0;

   for ( ObjectContainer::iterator o = objects.begin(); o != objects.end(); o++ ) {
      if ( gamemap->getgameparameter ( cgp_objectsDestroyedByTerrain ))
         if ( o->typ->getFieldModification(getWeather()).terrainaccess.accessible( bdt ) == -1 ) {
            objectRemovalStrategy->removeObject( this, o->typ );
            setparams( objectRemovalStrategy );
            return;
         }

      bdt  &=  o->typ->getFieldModification(getWeather()).terrain_and;
      bdt  |=  o->typ->getFieldModification(getWeather()).terrain_or;

      for ( i = 0; i < cmovemalitypenum; i++ ) {
         __movemalus[i] += o->typ->getFieldModification(getWeather()).movemalus_plus[i];
         if ( (o->typ->getFieldModification(getWeather()).movemalus_abs[i] != 0) && (o->typ->getFieldModification(getWeather()).movemalus_abs[i] != -1) )
            __movemalus[i] = o->typ->getFieldModification(getWeather()).movemalus_abs[i];
         if ( __movemalus[i] < minmalq )
            __movemalus[i] = minmalq;
      }

      viewbonus += o->typ->viewbonus_plus;
      if ( o->typ->viewbonus_abs != -1 )
         viewbonus = o->typ->viewbonus_plus;
   }

   if ( building ) 
      if ( this == building->getField( building->typ->entry ))
         bdt |= getTerrainBitType(cbbuildingentry);
   
}

Object* MapField :: checkForObject ( const ObjectType*  o )
{
   for ( ObjectContainer::iterator i = objects.begin(); i != objects.end(); i++ )
      if ( i->typ == o )
         return &(*i);

   return NULL;
}


MapField::Resourceview :: Resourceview ( void )
{
   visible = 0;
   memset ( &fuelvisible, 0, sizeof ( fuelvisible ));
   memset ( &materialvisible, 0, sizeof ( materialvisible ));
}


int MapField :: getMemoryFootprint() const
{
   int size = sizeof(*this);
   return size;
}


MapField :: ~MapField()
{
   if ( resourceview ) {
      delete resourceview;
      resourceview = NULL;
   }
}


