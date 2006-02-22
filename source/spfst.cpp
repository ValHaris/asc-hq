/*! \file spfst.cpp
    \brief map accessing and usage routines used by ASC and the mapeditor
*/

/*
    This file is part of Advanced Strategic Command; http://www.asc-hq.de
    Copyright (C) 1994-2005  Martin Bickel  and  Marc Schellenberger

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
#include <SDL_image.h>


#include "vehicletype.h"
#include "buildingtype.h"

#include "basestrm.h"
#include "misc.h"
#include "basegfx.h"
#include "typen.h"
#include "spfst.h"
#include "events.h"
#include "loaders.h"
#include "attack.h"
#include "itemrepository.h"

#include "dialog.h"
#include "mapalgorithms.h"
#include "vehicle.h"
#include "buildings.h"
#include "mapfield.h"

SigC::Signal0<void> repaintMap;
SigC::Signal0<void> repaintDisplay;
SigC::Signal0<void> updateFieldInfo;
SigC::Signal0<void> cursorMoved;
SigC::Signal1<void,ContainerBase*> showContainerInfo;
SigC::Signal1<void,Vehicletype*> showVehicleTypeInfo;
SigC::Signal0<void> viewChanged;
SigC::Signal1<void,GameMap*> mapChanged;
SigC::Signal0<bool> idleEvent;



void displaymap()
{
   repaintMap();
}   

  
   GameMap*    actmap = NULL;

   Schriften schriften;



int          terrainaccessible ( const tfield*        field, const Vehicle*     vehicle, int uheight )
{
   int res  = terrainaccessible2 ( field, vehicle, uheight );
   if ( res < 0 )
      return 0;
   else
      return res;
}

int          terrainaccessible2 ( const tfield*        field, const Vehicle*     vehicle, int uheight )
{
   if ( uheight == -1 )
      uheight = vehicle->height;

   if ( !(uheight & vehicle->typ->height))
      return 0;


   return terrainaccessible2( field, vehicle->typ->terrainaccess, uheight );
}

int          terrainaccessible2 ( const tfield*        field, const TerrainAccess& terrainAccess, int uheight )
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



int         fieldAccessible( const tfield*        field,
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
      int c = fieldVisibility ( field, vehicle->color/8 );

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
           if ( terrainaccessible ( field, vehicle, uheight ) )
              if (vehicleplattfahrbar(vehicle,field))
                 return 2;
              else
                 if ((field->vehicle->height != vehicle->height) || (attackpossible28(field->vehicle,vehicle) == false) ||  actmap->player[actmap->actplayer].diplomacy.getState( field->vehicle->getOwner()) >= PEACE )
                    return 1;
      }
      else {   // building
        if ((field->bdt & getTerrainBitType(cbbuildingentry) ).any() && field->building->vehicleLoadable ( vehicle, uheight, attacked ))
           return 2;
        else
           if (uheight >= chtieffliegend || (field->building->typ->buildingheight <= chgetaucht && uheight >=  chschwimmend ))
              return 1;
           else
              return 0;
      }
   }
   return 0;
}




void         generatemap( TerrainType::Weather*   bt,
                               int                xsize,
                               int                ysize)
{ 
   delete actmap;
   actmap = new GameMap;
   for (int k = 1; k < 8; k++)
      actmap->player[k].stat = Player::computer;

   actmap->maptitle = "new map";

   actmap->allocateFields(xsize, ysize);

   if ( actmap->field== NULL)
      displaymessage ( "Could not generate map !! \nProbably out of enough memory !",2);

   for ( int l = 0; l < xsize*ysize; l++ ) {
      actmap->field[l].typ = bt;
      actmap->field[l].setparams();
      actmap->field[l].setMap( actmap );
   }

   actmap->_resourcemode = 1;
   actmap->playerView = 0;
}




tfield*        getfield(int          x,
                     int          y)
{ 
   if ((x < 0) || (y < 0) || (x >= actmap->xsize) || (y >= actmap->ysize))
      return NULL; 
   else
      return (   &actmap->field[y * actmap->xsize + x] );
}



void         putbuilding( const MapCoordinate& entryPosition,
                         int          color,
                         const BuildingType* buildingtyp,
                         int          completion,
                         int          ignoreunits )
{ 
   if ( color & 7 )
      displaymessage("putbuilding muss eine farbe aus 0,8,16,24,.. uebergeben werden !",2); 

   for ( int a = 0; a < 4; a++)
      for ( int b = 0; b < 6; b++ )
         if ( buildingtyp->fieldExists ( BuildingType::LocalCoordinate( a, b ) ) ) {
            tfield* field = actmap->getField( buildingtyp->getFieldCoordinate( entryPosition, BuildingType::LocalCoordinate(a,b) ));
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


void         putbuilding2( const MapCoordinate& entryPosition,
                           int         color,
                           BuildingType* buildingtyp)
{ 
   if ( color & 7 )
      displaymessage("putbuilding muss eine farbe aus 0,8,16,24,.. uebergeben werden !",2);

   for ( int a = 0; a < 4; a++)
      for ( int b = 0; b < 6; b++ )
         if ( buildingtyp->fieldExists ( BuildingType::LocalCoordinate( a, b ) ) ) {
            tfield* field = actmap->getField( buildingtyp->getFieldCoordinate( entryPosition, BuildingType::LocalCoordinate(a,b) ));
            if (field == NULL)
               return ;
            else {
               if ( field->vehicle )
                  return;
            }
         }

   if ( !actmap->getField(entryPosition)->building ) {
      Building* gbde = new Building ( actmap, entryPosition, buildingtyp, color/8 );

      Resources maxplus;
      Resources actplus;
      Resources biplus;
      /*
      int maxresearch = 0;
      bool found = false;
      for ( GameMap::Player::BuildingList::iterator i = actmap->player[color/8].buildingList.begin(); i != actmap->player[ color/8].buildingList.end(); i++ ) {
         Building* bld = *i;
         if ( bld->typ == gbde->typ  && bld != gbde ) {
            found = true;

            for ( int r = 0; r < 3; r++ ) {
               if ( bld->maxplus.resource(r) > maxplus.resource(r) )
                 maxplus  = bld->maxplus;

               if ( bld->bi_resourceplus.resource(r) > biplus.resource(r) )
                  biplus = bld->bi_resourceplus;

               if ( bld->plus.resource(r) > actplus.resource(r) )
                  actplus = bld->plus;
            }

            if ( bld->maxresearchpoints > maxresearch )
               maxresearch = bld->maxresearchpoints;
         }
      }

      gbde->damage = 0;
      if ( found ) {
         if ( actmap->_resourcemode == 1 ) {
            gbde->plus.energy = biplus.energy;
            gbde->plus.material = biplus.material;
            gbde->plus.fuel = biplus.fuel;
         } else {
            gbde->plus.energy = maxplus.energy;
            gbde->plus.material = maxplus.material;
            gbde->plus.fuel = maxplus.fuel;
         }
         gbde->maxplus = maxplus;
         gbde->bi_resourceplus = biplus;
      } else {
         gbde->plus = gbde->defaultProduction;
         gbde->maxplus = gbde->defaultProduction;
         gbde->bi_resourceplus = gbde->defaultProduction;
      }
      */
      
      gbde->plus = gbde->typ->defaultProduction;
      gbde->maxplus = gbde->typ->defaultProduction;
      gbde->bi_resourceplus = gbde->typ->defaultProduction;

      gbde->actstorage.fuel = 0;
      gbde->actstorage.material = 0;
      gbde->actstorage.energy = 0;
      gbde->netcontrol = 0;
      gbde->connection = 0;
      gbde->visible = true;
      gbde->setCompletion ( 0 );
   }
   else {
      Building* gbde = actmap->getField(entryPosition)->building;
      if (gbde->getCompletion() < gbde->typ->construction_steps-1)
         gbde->setCompletion( gbde->getCompletion()+1 );

   }
}






void checkobjectsforremoval ( void )
{
   for ( int y = 0; y < actmap->ysize; y++ )
      for ( int x = 0; x < actmap->xsize; x++ ) {
         tfield* fld = getfield ( x, y );
         for ( tfield::ObjectContainer::iterator i = fld->objects.begin(); i != fld->objects.end();  )
            if ( i->typ->getFieldModification(fld->getweather()).terrainaccess.accessible ( fld->bdt ) < 0 ) {
               fld->removeobject ( i->typ, true );
               i = fld->objects.begin();
            } else
               i++;
      }
}

void  checkunitsforremoval ( void )
{
   for ( int c=0; c<=8 ;c++ ) {
      ASCString msg;
      for ( Player::VehicleList::iterator i = actmap->player[c].vehicleList.begin(); i != actmap->player[c].vehicleList.end();  ) {

          Vehicle* eht = *i;
          tfield* field = getfield(eht->xpos,eht->ypos);
          bool erase = false;

          ASCString reason;
          if (field->vehicle == eht) {
             if ( eht->height <= chfahrend )
                if ( eht->typ->terrainaccess.accessible ( field->bdt ) < 0 ) {
                   erase = true;
                   reason = "was swallowed by the ground";
                }
           //  if ( eht )
/*                if ( getmaxwindspeedforunit( eht ) < actmap->weather.windSpeed*maxwindspeed ) {
                   reason = "was blown away by the wind";
                   erase = true;
                }*/
          }
          if ( erase ) {
             ASCString ident = "The unit " + (*i)->getName() + " at position ("+strrr((*i)->getPosition().x)+"/";
             ident += strrr((*i)->getPosition().y)+ASCString(") ");

             msg += ident + reason;
             msg += "\n\n";

             Vehicle* pv = *i;
             actmap->player[c].vehicleList.erase ( i );
             delete pv;



             /* if the unit was a transport and had other units loaded, these units have been deleted as well.
                We don't know which elements of the container are still valid, so we start from the beginning again. */
             i = actmap->player[c].vehicleList.begin();
          } else
             i++;
      }

      if ( !msg.empty() )
         new Message ( msg, actmap, 1<<c);
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
   tfield* field = getfield(eht->xpos,eht->ypos);
   if ( field->vehicle == eht) {
      if (eht->height >= chtieffliegend && eht->height <= chhochfliegend ) //    || ((eht->height == chfahrend) && ( field->typ->art & cbwater ))) ) 
         return eht->typ->movement[log2(eht->height)] * 256 ;

      if ( (field->bdt & getTerrainBitType(cbfestland)).none() && eht->height <= chfahrend && eht->height >= chschwimmend && (field->bdt & getTerrainBitType(cbharbour)).none() && (field->bdt & getTerrainBitType(cbwater0)).none())
         return eht->typ->maxwindspeedonwater * maxwindspeed;
   }
   return maxint;
}





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


int getheightdelta ( const ContainerBase* c1, const ContainerBase* c2 )
{
   return getheightdelta( log2(c1->getHeight()), log2(c2->getHeight() ));
}


int getheightdelta ( int height1, int height2 )
{
   int ah = height1;
   int dh = height2;
   int hd = dh - ah;

   if ( ah >= 3 && dh <= 2 )
      hd++;
   if (dh >= 3 && ah <= 2 )
      hd--;

   return hd;
}

bool fieldvisiblenow( const tfield* pe, int player )
{
  if ( player < 0 ) {
     #ifdef karteneditor
     return true;
     #else
     return false;
     #endif
  }

  if ( pe ) { 
      int c = (pe->visible >> ( player * 2)) & 3;
      #ifdef karteneditor
         c = visible_all;
      #endif

      if ( c < actmap->getInitialMapVisibility( player ) )
         c = actmap->getInitialMapVisibility( player );

      if (c > visible_ago) { 
         if ( pe->vehicle ) { 
            if ((c == visible_all) || (pe->vehicle->color / 8 == player ) || ((pe->vehicle->height >= chschwimmend) && (pe->vehicle->height <= chhochfliegend)))
               return true; 
         } 
         else 
            if (pe->building != NULL) { 
               if ((c == visible_all) || (pe->building->typ->buildingheight >= chschwimmend) || (pe->building->color == player*8)) 
                  return true; 
            } 
            else
               return true; 
      } 
   }
   return false; 
} 



VisibilityStates fieldVisibility( const tfield* pe, int player )
{
   return fieldVisibility( pe, player, actmap );
}

VisibilityStates fieldVisibility( const tfield* pe, int player, GameMap* gamemap )
{
  if ( pe && player >= 0 ) {
      VisibilityStates c = VisibilityStates((pe->visible >> ( player * 2)) & 3);
      #ifdef karteneditor
         c = visible_all;
      #endif

      if ( c < gamemap->getInitialMapVisibility( player ) )
         c = gamemap->getInitialMapVisibility( player );

      return c;
   } else
      return visible_not;
}



int getUnitSetID( const Vehicletype* veh )
{
   for ( UnitSets::iterator i = unitSets.begin(); i != unitSets.end(); ++i)
      if ( (*i)->isMember( veh->id, SingleUnitSet::unit) )
         return (*i)->ID;
         
   return -1;
}

int getUnitSetID( const BuildingType* bld )
{
   for ( UnitSets::iterator i = unitSets.begin(); i != unitSets.end(); ++i)
      if ( (*i)->isMember( bld->id, SingleUnitSet::building) )
         return (*i)->ID;
         
   return -1;
}


void         calculateobject( int       x,
                              int       y,
                              bool      mof,
                              const ObjectType* obj,
                              GameMap* actmap )
{
   if ( obj->netBehaviour & ObjectType::SpecialForest ) {
      // ForestCalculation::calculateforest( actmap, obj );
      return;
   }

   tfield* fld = actmap->getField(x,y) ;
   Object* oi2 = fld-> checkforobject (  obj  );

   int c = 0;
   for ( int dir = 0; dir < sidenum; dir++) {
      int a = x;
      int b = y;
      getnextfield( a, b, dir );
      tfield* fld2 = actmap->getField(a,b);

      if ( fld2 ) {
         for ( int oj = -1; oj < int(obj->linkableObjects.size()); oj++ ) {
            Object* oi;
            if ( oj == -1 )
               oi = fld2->checkforobject ( obj );
            else
               oi = fld2->checkforobject ( actmap->getobjecttype_byid ( obj->linkableObjects[oj] ) );

            if ( oi ) {
               c |=  1 << dir ;
               if ( mof )
                  calculateobject ( a, b, false, oi->typ, actmap );

            }
         }
         for ( unsigned int t = 0; t < obj->linkableTerrain.size(); t++ )
            if ( fld2->typ->terraintype->id == obj->linkableTerrain[t] )
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
         tfield* fld2 = actmap->getField(a,b);
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
     oi2->setdir ( c );
     fld->setparams();
   }

}




void         calculateallobjects( GameMap* actmap )
{
   // vector<ObjectType*> forestObjects;
   for ( int y = 0; y < actmap->ysize ; y++)
      for ( int x = 0; x < actmap->xsize ; x++) {
         tfield* fld = actmap->getField(x,y);

         for ( tfield::ObjectContainer::iterator i = fld->objects.begin(); i != fld->objects.end(); i++ )
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



