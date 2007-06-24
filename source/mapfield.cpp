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



tfield :: tfield ( GameMap* gamemap_ )
{
  init();
  setMap( gamemap_ );
}

tfield :: tfield (  )
{
  init();
}


void tfield::init ()
{
   bdt.set ( 0 );
   typ = NULL;
   vehicle = NULL;
   building = NULL;
   a.temp = 0;
   a.temp2 = 0;
   temp3 = 0;
   temp4 = 0;
   visible = 0;
   fuel = 0;
   material = 0;
   resourceview = NULL;
   connection = 0;
   gamemap = NULL;
   viewbonus = 0;
}


void tfield::Resourceview::setview( int player, int material, int fuel )
{
   visible |= 1 << player;
   materialvisible[player] = material;
   fuelvisible[player] = fuel;
}

void tfield::Resourceview::resetview( int player )
{
   visible &= ~(1<<player);
}


void tfield::endRound( int turn )
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


void tfield::operator= ( const tfield& f )
{
   typ = f.typ;
   fuel = f.fuel;
   material = f.material;
   visible = f.visible;
   tempw = f.tempw;
   temp3 = f.temp3;
   temp4 = f.temp4;
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



int tfield :: mineattacks ( const Vehicle* veh )
{
   int i = 1;
   for ( MineContainer::iterator m = mines.begin(); m != mines.end(); m++, i++ )
      if ( m->attacksunit ( veh ))
         return i;

   return 0;
}

Mine& tfield::getMine ( int n )
{
  int c = 0;
  MineContainer::iterator i;
  for ( i = mines.begin(); c < n; i++,c++ );
  return *i;
}

bool  tfield :: addobject( const ObjectType* obj, int dir, bool force )
{
   if ( !obj )
      return false;

   Object* i = checkforobject ( obj );
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

         if ( dir == -1 )
            calculateobject( getx(), gety(), true, obj, gamemap );

         sortobjects();
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


void tfield :: removeobject( const ObjectType* obj, bool force)
{
   if ( !force && building )
      return;

   #ifndef karteneditor
   if ( !force )
      if ( vehicle )
         if ( vehicle->color != gamemap->actplayer << 3)
           return;
   #endif

   if ( !obj ) {
      if ( objects.size() ) {
         obj = objects.rbegin()->typ;
         objects.pop_back();
      }
   } else
      for ( ObjectContainer::iterator o = objects.begin(); o != objects.end(); )
         if ( o->typ == obj )
            o = objects.erase( o );
         else
            o++;

   setparams();
   if ( obj )
      calculateobject( getx(), gety(), true, obj );
}

void tfield :: deleteeverything ( void )
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


bool tfield :: unitHere ( const Vehicle* veh )
{
   if ( vehicle == veh )
      return true;

   if ( vehicle && veh && vehicle->networkid == veh->networkid )
      return true;
   return false;
}

ContainerBase* tfield :: getContainer()
{
   if ( vehicle )
      return vehicle;
   else
      return building;
}


int tfield :: getweather ( void )
{
   if ( !typ )
      return 0;
   for ( int w = 0; w < cwettertypennum; w++ )
      if ( typ == typ->terraintype->weather[w] )
         return w;
   return -1;
}

void tfield :: setweather ( int weather )
{
     if (typ->terraintype->weather[ weather ] ) {
        typ = typ->terraintype->weather[ weather ];
        setparams();
     } else {
        typ = typ->terraintype->weather[ 0 ];
        setparams();
     }
}

bool compareObjectHeight ( const Object& o1, const Object& o2 )
{
   return o1.typ->imageHeight < o2.typ->imageHeight;
}

void tfield :: sortobjects ( void )
{
   sort ( objects.begin(), objects.end(), compareObjectHeight );
}

bool  tfield :: putmine( int col, int typ, int strength )
{
   if ( mineowner() >= 0  && mineowner() != col )
      return false;

   if ( mines.size() >= gamemap->getgameparameter ( cgp_maxminesonfield ))
      return false;

   MineTypes mt =  MineTypes(typ);
   Mine mymine ( mt, strength, col, gamemap );
   mines.push_back ( mymine );
   return true;
}

int tfield :: mineowner( void )
{
   if ( mines.empty() )
      return -1;
   else
      return mines.begin()->player;
}


void tfield :: removemine( int num )
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


int tfield :: getx( void )
{
   int n = this - gamemap->field;
   return n % gamemap->xsize;
}

int tfield :: gety( void )
{
   int n = this - gamemap->field;
   return n / gamemap->xsize;
}


int tfield :: getattackbonus ( void )
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

int tfield :: getdefensebonus ( void )
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

ASCString tfield :: getName()
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


int tfield :: getjamming ( void )
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

int tfield :: getmovemalus ( const Vehicle* veh )
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

int tfield :: getmovemalus ( int type )
{
  return __movemalus.at(type);
}

void tfield :: setparams ( void )
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
         if ( o->typ->getFieldModification(getweather()).terrainaccess.accessible( bdt ) == -1 ) {
            objects.erase(o);
            setparams();
            return;
         }

      bdt  &=  o->typ->getFieldModification(getweather()).terrain_and;
      bdt  |=  o->typ->getFieldModification(getweather()).terrain_or;

      for ( i = 0; i < cmovemalitypenum; i++ ) {
         __movemalus[i] += o->typ->getFieldModification(getweather()).movemalus_plus[i];
         if ( (o->typ->getFieldModification(getweather()).movemalus_abs[i] != 0) && (o->typ->getFieldModification(getweather()).movemalus_abs[i] != -1) )
            __movemalus[i] = o->typ->getFieldModification(getweather()).movemalus_abs[i];
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

Object* tfield :: checkforobject ( const ObjectType*  o )
{
   for ( ObjectContainer::iterator i = objects.begin(); i != objects.end(); i++ )
      if ( i->typ == o )
         return &(*i);

   return NULL;
}


tfield::Resourceview :: Resourceview ( void )
{
   visible = 0;
   memset ( &fuelvisible, 0, sizeof ( fuelvisible ));
   memset ( &materialvisible, 0, sizeof ( materialvisible ));
}


int tfield :: getMemoryFootprint() const
{
   int size = sizeof(*this);
   return size;
}


tfield :: ~tfield()
{
   if ( resourceview ) {
      delete resourceview;
      resourceview = NULL;
   }
}


