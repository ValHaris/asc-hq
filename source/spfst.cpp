/*! \file spfst.cpp
    \brief map accessing and usage routines used by ASC and the mapeditor
*/

/*
    This file is part of Advanced Strategic Command; http://www.asc-hq.de
    Copyright (C) 1994-2010  Martin Bickel  and  Marc Schellenberger

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; see the file COPYING. If not, write to the 
    Free Software Foundation, Inc., 59 Temple Place, Suite 330, 
    Boston, MA  02111-1307  USA
*/

#include <stdio.h>
#include <cstring>
#include <utility>
#include <map>


#include "vehicletype.h"
#include "buildingtype.h"

#include "typen.h"
#include "spfst.h"
#include "attack.h"

#include "mapalgorithms.h"
#include "vehicle.h"
#include "buildings.h"
#include "mapfield.h"

#include "spfst-legacy.h"

SigC::Signal0<void> repaintMap;
SigC::Signal0<void> repaintDisplay;
SigC::Signal0<void> updateFieldInfo;
SigC::Signal0<void> cursorMoved;
SigC::Signal1<void,ContainerBase*> showContainerInfo;
SigC::Signal1<void,VehicleType*> showVehicleTypeInfo;
SigC::Signal0<void> viewChanged;
SigC::Signal1<void,GameMap*> mapChanged;
SigC::Signal0<bool> idleEvent;
SigC::Signal1<void, const Context&> fieldCrossed;



void displaymap()
{
   repaintMap();
}   

  
   GameMap*    actmap = NULL;

   Schriften schriften;



int          terrainaccessible ( const MapField*        field, const Vehicle*     vehicle, int uheight )
{
   int res  = terrainaccessible2 ( field, vehicle, uheight );
   if ( res < 0 )
      return 0;
   else
      return res;
}

int          terrainaccessible2 ( const MapField*        field, const Vehicle*     vehicle, int uheight )
{
   if ( uheight == -1 )
      uheight = vehicle->height;

   if ( !(uheight & vehicle->typ->height))
      return 0;


   return terrainaccessible2( field, vehicle->typ->terrainaccess, uheight );
}

int          terrainaccessible2 ( const MapField*        field, const TerrainAccess& terrainAccess, int uheight )
{
   if ( uheight >= chtieffliegend)
      return 2;
   else {
        if ( uheight == chtiefgetaucht )
           if ( (field->bdt & getTerrainBitType(cbwater3) ).any() )
              return 2;
           else
              return -1;
        else
           if ( uheight == chgetaucht )
              if ( (field->bdt & ( getTerrainBitType(cbwater3) | getTerrainBitType(cbwater2 )) ).any() )
                 return 2;
              else
                 return -2;
           else {
              if ( terrainAccess.accessible ( field->bdt ) > 0 ) {
                 if ( uheight == chschwimmend ) {
                    if ( (field->bdt & getTerrainBitType(cbwater)).any() )
                       return 2;
                    else
                       return -3;
                 } else
                    return 2;
              } else
                 return -3;
            }
   }
}



int         fieldAccessible( const MapField*        field,
                            const Vehicle*     vehicle,
                            int  uheight,
                            const bool* attacked,
                            bool ignoreVisibility )
{
   if ( !field || !vehicle )
      return 0;

   if ( uheight == -2 )
      uheight = vehicle->height;

   if ( !ignoreVisibility ) {
      int c = fieldVisibility ( field, vehicle->getOwner() );

      if (field == NULL)
        return 0;

      if (c == visible_not)
         return 0;
   }

/*
   if ( c == visible_all)
      if ( field->mines.size() )
         for ( int i = 0; i < field->mines.size(); i++ )
            if ( field->getMine(i).attacksunit( vehicle ))
               return 0;
*/


   if ( (!field->vehicle || field->vehicle == vehicle) && !field->building ) {
      if ( vehicle->typ->height & uheight )
         return terrainaccessible ( field, vehicle, uheight );
      else
         return 0;
   } else {
      if (field->vehicle) {
         if ( vehicle->getMap()->getPlayer(vehicle).diplomacy.isAllied( field->vehicle->getOwner()) ) {
            if ( field->vehicle->vehicleLoadable ( vehicle, uheight ) )
               return 2;
            else
               if ( terrainaccessible ( field, vehicle, uheight ))
                  return 1;
               else
                  return 0;
         }
         else   ///////   keine eigene vehicle
           if ( terrainaccessible ( field, vehicle, uheight ) ) {
              if (vehicleplattfahrbar(vehicle,field))
                 return 2;
               else {
                  if ( getheightdelta(getFirstBit(field->vehicle->height), getFirstBit(vehicle->height)) || (attackpossible28(field->vehicle,vehicle) == false) || ( vehicle->getMap()->getPlayer(vehicle).diplomacy.getState( field->vehicle->getOwner()) >= PEACE ))
                    return 1;
               }
           }
      }
      else {   // building
        if ((field->bdt & getTerrainBitType(cbbuildingentry) ).any() && field->building->vehicleLoadable ( vehicle, uheight, attacked ))
           return 2;
        else
           if (uheight >= chtieffliegend || (field->building->typ->height <= chgetaucht && uheight >=  chschwimmend ))
              return 1;
           else
              return 0;
      }
   }
   return 0;
}






MapField*        getfield(int          x,
                     int          y)
{ 
   if ((x < 0) || (y < 0) || (x >= actmap->xsize) || (y >= actmap->ysize))
      return NULL; 
   else
      return (   &actmap->field[y * actmap->xsize + x] );
}



void         putbuilding( GameMap* actmap,
                          const MapCoordinate& entryPosition,
                         int          color,
                         const BuildingType* buildingtyp,
                         int          completion,
                         int          ignoreunits )
{ 
   if ( color & 7 )
      fatalError("putbuilding muss eine farbe aus 0,8,16,24,.. uebergeben werden !",2);

   for ( int a = 0; a < 4; a++)
      for ( int b = 0; b < 6; b++ )
         if ( buildingtyp->fieldExists ( BuildingType::LocalCoordinate( a, b ) ) ) {
            MapField* field = actmap->getField( buildingtyp->getFieldCoordinate( entryPosition, BuildingType::LocalCoordinate(a,b) ));
            if (field == NULL) 
               return ;
            else {
               if ( field->vehicle && (!ignoreunits ) ) 
                  return;
               if (field->building != NULL)
                  return;
            }
         } 


   Building* gbde = new Building ( actmap , entryPosition, buildingtyp, color/8 );

   if (completion >= buildingtyp->construction_steps)
      completion = buildingtyp->construction_steps - 1;

   gbde->setCompletion ( completion );
}





void checkobjectsforremoval ( GameMap* gamemap )
{
   for ( int y = 0; y < gamemap->ysize; y++ )
      for ( int x = 0; x < gamemap->xsize; x++ ) {
         MapField* fld = getfield ( x, y );
         for ( MapField::ObjectContainer::iterator i = fld->objects.begin(); i != fld->objects.end();  )
            if ( i->typ->getFieldModification(fld->getWeather()).terrainaccess.accessible ( fld->bdt ) < 0 ) {
               fld->removeObject ( i->typ, true );
               i = fld->objects.begin();
            } else
               i++;
      }
}

void  checkunitsforremoval ( GameMap* gamemap )
{
   ASCString messages[playerNum];
   for ( int y = 0; y < gamemap->ysize; y++ )
      for ( int x = 0; x < gamemap->xsize; x++ ) {
         MapField* fld = gamemap->getField ( x, y );
         if ( (fld->building && fld->building->typ->terrainaccess.accessible( fld->bdt ) < 0) && (fld->building->typ->height <= chfahrend) ) {
            messages[fld->building->getOwner()] += getBuildingReference( fld->building ) + " was destroyed \n\n";
            delete fld->building;
         }
      }


   for ( int c=0; c<=8 ;c++ ) {
      ASCString msg = messages[c];
      for ( Player::VehicleList::iterator i = gamemap->player[c].vehicleList.begin(); i != gamemap->player[c].vehicleList.end();  ) {

          Vehicle* eht = *i;
          MapField* field = gamemap->getField(eht->xpos,eht->ypos);
          bool erase = false;

          ASCString reason;
          if (field->vehicle == eht) {
             if ( eht->height <= chfahrend )
                if ( eht->typ->terrainaccess.accessible ( field->bdt ) < 0 ) {
                   erase = true;
                   reason = "was swallowed by the ground";
                }
             if ( eht )
                if ( getmaxwindspeedforunit( eht ) < gamemap->weather.windSpeed*maxwindspeed ) {
                   reason = "was blown away by the wind";
                   erase = true;
                }
          }
          if ( erase ) {
             msg += getUnitReference( eht ) + reason;
             msg += "\n\n";

             Vehicle* pv = *i;
             gamemap->player[c].vehicleList.erase ( i );
             delete pv;



             /* if the unit was a transport and had other units loaded, these units have been deleted as well.
                We don't know which elements of the container are still valid, so we start from the beginning again. */
             i = gamemap->player[c].vehicleList.begin();
          } else
             i++;
      }

      if ( !msg.empty() )
         new Message ( msg, gamemap, 1<<c);
   }
}


int  getwindheightforunit ( const Vehicle* eht, int uheight )
{
   if ( uheight == -1 )
      uheight = eht->height;

   if ( uheight == chfliegend )
      return 1;
   else
      if ( uheight == chhochfliegend )
         return 2;
      else
         return 0;
}

int  getmaxwindspeedforunit ( const Vehicle* eht )
{
   MapField* field = eht->getMap()->getField(eht->xpos,eht->ypos);
   if ( field->vehicle == eht) {
      if (eht->height >= chtieffliegend && eht->height <= chhochfliegend ) //    || ((eht->height == chfahrend) && ( field->typ->art & cbwater ))) ) 
         return eht->typ->movement[getFirstBit(eht->height)] * 256 ;

      if ( (field->bdt & getTerrainBitType(cbfestland)).none() && eht->height <= chfahrend && eht->height >= chschwimmend && (field->bdt & getTerrainBitType(cbharbour)).none() && (field->bdt & getTerrainBitType(cbwater0)).none())
         return eht->typ->maxwindspeedonwater * maxwindspeed;
   }
   return maxint;
}


/*


void tdrawline8 :: start ( int x1, int y1, int x2, int y2 )
{
   x1 += x1 + (y1 & 1);
   x2 += x2 + (y2 & 1);
   tdrawline::start ( x1, y1, x2, y2 );
}

void tdrawline8 :: putpix ( int x, int y )
{
       if ( (x & 1) == (y & 1) )
          putpix8( x/2, y );
}
*/
/*
void EllipseOnScreen :: paint ( void )
{
   if ( active )
      ellipse ( x1, y1, x2, y2, color, precision );
}

void EllipseOnScreen :: read( tnstream& stream )
{
  x1 = stream.readInt();
  y1 = stream.readInt();
  x2 = stream.readInt();
  y2 = stream.readInt();
  color = stream.readInt();
  precision = stream.readFloat();
  active = stream.readInt();
}

void EllipseOnScreen :: write ( tnstream& stream )
{
   stream.writeInt( x1 );
   stream.writeInt( y1 );
   stream.writeInt( x2 );
   stream.writeInt( y2 );
   stream.writeInt( color );
   stream.writeFloat( precision );
   stream.writeInt( active );
}

*/

int getheightdelta ( const ContainerBase* c1, const ContainerBase* c2 )
{
   return getheightdelta( getFirstBit(c1->getHeight()), getFirstBit(c2->getHeight() ));
}


bool fieldvisiblenow( const MapField* pe, Vehicle* veh, int player )
{
   GameMap* gamemap = pe->getMap();
   if ( player == -1 )
      return true;

   if ( player < -1 )
      return false;

   if ( !gamemap )
      return false;
  
   if ( pe ) {
      int c = (pe->visible >> ( player * 2)) & 3;

      if ( c < gamemap->getInitialMapVisibility( player ) )
         c = gamemap->getInitialMapVisibility( player );

      if (c > visible_ago) {
         if ( !veh )
            veh = pe->vehicle;
         
         if ( veh ) {
            if ((c == visible_all) || (veh->color / 8 == player ) || ((veh->height >= chschwimmend) && (veh->height <= chhochfliegend)))
               return true;
         }
         else
            if (pe->building != NULL) {
            if ((c == visible_all) || (pe->building->typ->height >= chschwimmend) || (pe->building->color == player*8))
               return true;
            }
            else
               return true;
      }
   }
   return false;
}




VisibilityStates fieldVisibility( const MapField* pe )
{
   return fieldVisibility( pe, pe->getMap()->actplayer );
}

VisibilityStates fieldVisibility( const MapField* pe, int player )
{
   GameMap* gamemap = pe->getMap();
   
   if ( player < 0 )
      return visible_all;

   if ( pe ) {
      VisibilityStates c = VisibilityStates((pe->visible >> ( player * 2)) & 3);
      if ( c < gamemap->getInitialMapVisibility( player ) )
         c = gamemap->getInitialMapVisibility( player );

      return c;
   } else
     return visible_not;
}
      

void  calculateobject( const MapCoordinate& pos, 
                             bool mof,
                             const ObjectType* obj,
                             GameMap* gamemap  )
{
   calculateobject( pos.x, pos.y, mof, obj, gamemap );
}


void         calculateobject( int       x,
                              int       y,
                              bool      mof,
                              const ObjectType* obj,
                              GameMap* actmap )
{
   if ( obj->netBehaviour & ObjectType::KeepOrientation ) 
      return;
   
   if ( obj->netBehaviour & ObjectType::SpecialForest ) {
      // ForestCalculation::calculateforest( actmap, obj );
      return;
   }

   MapField* fld = actmap->getField(x,y) ;
   Object* oi2 = fld-> checkForObject (  obj  );

   int c = 0;
   for ( int dir = 0; dir < sidenum; dir++) {
      int a = x;
      int b = y;
      getnextfield( a, b, dir );
      MapField* fld2 = actmap->getField(a,b);

      if ( fld2 ) {
         if ( obj->netBehaviour & ObjectType::NetToSelf )
            if ( fld2->checkForObject ( obj )) {
               c |=  1 << dir ;
               if ( mof )
                  calculateobject ( a, b, false, obj, actmap );
            }


         for ( int oj = 0; oj < int(obj->linkableObjects.size()); oj++ ) {
            for ( int id = obj->linkableObjects[oj].from; id <= obj->linkableObjects[oj].to; ++id ) {
               Object* oi = fld2->checkForObject ( actmap->getobjecttype_byid ( id ) );
               if ( oi ) {
                  c |=  1 << dir ;
                  if ( mof )
                     calculateobject ( a, b, false, oi->typ, actmap );
               }
            }
         }

         for ( unsigned int t = 0; t < obj->linkableTerrain.size(); t++ )
            for ( int id = obj->linkableTerrain[t].from; id <= obj->linkableTerrain[t].to; ++id ) 
               if ( fld2->typ->terraintype->id == id )
                  c |=  1 << dir ;

         if ( fld2->building && !fld2->building->typ->hasFunction( ContainerBaseType::NoObjectChaining  ) ) {
            if ( (obj->netBehaviour & ObjectType::NetToBuildingEntry)  &&  (fld2->bdt & getTerrainBitType(cbbuildingentry) ).any() )
               c |= 1 << dir;

            if ( obj->netBehaviour & ObjectType::NetToBuildings )
               c |= 1 << dir;
         }

      }
      else {
         if ( obj->netBehaviour & ObjectType::NetToBorder )
            c |= 1 << dir;
      }
   }

   if ( obj->netBehaviour & ObjectType::AutoBorder ) {
      int autoborder = 0;
      int count = 0;
      for ( int dir = 0; dir < sidenum; dir++) {
         int a = x;
         int b = y;
         getnextfield( a, b, dir );
         MapField* fld2 = actmap->getField(a,b);
         if ( !fld2 ) {
            // if the field opposite of the border field is connected to, make a straight line out of the map.
            if ( c & (1 << ((dir+sidenum/2) % sidenum ))) {
               autoborder |= 1 << dir;
               count++;
            }
         }
      }
      if ( count == 1 )
         c |= autoborder;
   }

   if ( oi2 ) {
     oi2->setDir ( c );
   }

}




void         calculateallobjects( GameMap* actmap )
{
   // vector<ObjectType*> forestObjects;
   for ( int y = 0; y < actmap->ysize ; y++)
      for ( int x = 0; x < actmap->xsize ; x++) {
         MapField* fld = actmap->getField(x,y);

         for ( MapField::ObjectContainer::iterator i = fld->objects.begin(); i != fld->objects.end(); i++ )
             // if ( !(i->typ->netBehaviour & ObjectType::SpecialForest) )
                calculateobject( x, y, false, i->typ, actmap );
                #if 0
             else
                if ( find ( forestObjects.begin(), forestObjects.end(), i->typ ) == forestObjects.end())
                   forestObjects.push_back ( i->typ );
                   #endif

         fld->setparams();
      }
#if 0
   for ( vector<ObjectType*>::iterator i = forestObjects.begin(); i != forestObjects.end(); i++ )
      ForestCalculation::calculateforest( actmap, *i );
#endif      
}



