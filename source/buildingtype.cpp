/***************************************************************************
                          buildingtype.cpp  -  description
                             -------------------
    begin                : Fri Sep 29 2000
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

#include "vehicletype.h"
#include "buildingtype.h"

#include "viewcalculation.h"
#include "errors.h"

#ifdef sgmain
 #include "spfst.h"
#endif



const char*  cbuildingfunctions[cbuildingfunctionnum]  =
              { "HQ",
                "training",
                "unused (was: refinery)",
                "vehicle production",
                "ammunition production",
                "unused (was: energy prod)",
                "unused (was: material prod)",
                "unused (was: fuel prod)",
                "repair facility",
                "recycling",
                "research",
                "sonar",
                "wind power plant",
                "solar power plant",
                "matter converter (was: power plant)",
                "mining station",
                "external loading",
                "construct units that cannot move out" };



void*   Buildingtype :: getpicture ( int x, int y )
{
   #ifdef HEXAGON
   return w_picture[0][0][x][y];
   #else
   return w_picture[0][x][y];
   #endif
}


void  Buildingtype :: getfieldcoordinates ( int          bldx,
                                             int          bldy,
                                             int          x,
                                             int          y,
                                             int     *    xx,
                                             int     *    yy)

{

   int      orgx, orgy;
   int      dx;

   orgx=bldx - entry.x;
   orgy=bldy - entry.y;

   dx =orgy & 1;
   orgx = orgx + (dx & (~ entry.y));

   *yy = orgy + y;
   *xx = orgx + x - (dx & *yy);
}

int    Buildingtype :: vehicleloadable ( pvehicletype fzt ) const
{
   if ( special & cgproduceAllUnitsB )
      return 1;

   if (    (loadcapacity >= fzt->maxsize()  &&  ((unitheightreq & fzt->height) || !unitheightreq) && !(unitheight_forbidden & fzt->height)  && (loadcapability & fzt->height))
        || ( fzt->functions & cf_trooper ) )
        return 1;

   return 0;
}





const float repairEfficiencyBuilding[resourceTypeNum*resourceTypeNum] = { 1./3., 0,     0,
                                                                          0,     1./3., 0,
                                                                          0,     0,     1. / 3. };

Building :: Building ( pmap actmap )
           : ContainerBase ( NULL ), repairEfficiency ( repairEfficiencyBuilding )
{
   int i;
   for ( i = 0; i < 8; i++ )
      aiparam[i] = NULL;
   color = 0;
   completion = 0;
   connection = 0;

   lastenergyavail = 0;
   lastmaterialavail = 0;
   lastfuelavail = 0;
   lastmineddist= 0;
   maxresearchpoints = 0;
   for ( i = 0; i < waffenanzahl; i++ ) {
      munition[i] = 0;
      munitionsautoproduction[i] = 0;
   }
   name = NULL;
   netcontrol = 0;
   next = NULL;
   prev = NULL;

   for ( i = 0; i< 32; i++ ) {
      production[i] = 0;
      productionbuyable[i] = 0;
      loading[i] = 0;
   }

   repairedThisTurn = 0;
   researchpoints = 0;
   typ = NULL;
   visible = 1;
   xpos = -1;
   ypos = -1;

   chainToMap ( actmap );
}

Building :: Building ( pbuilding src, tmap* actmap )
           : ContainerBase ( NULL ), repairEfficiency ( repairEfficiencyBuilding )
{
   chainToMap ( actmap );

   work = src->work;
   lastenergyavail = src->lastenergyavail;
   lastmaterialavail = src->lastmaterialavail;
   lastfuelavail = src->lastfuelavail;
   typ = src->typ;
   memcpy ( munitionsautoproduction , src->munitionsautoproduction, sizeof ( munitionsautoproduction ) );
   color = src->color;
   memcpy ( production , src->production, sizeof ( production ));
   damage = src->damage;

   plus = src->plus;
   maxplus = src->maxplus;

   actstorage = src->actstorage;

   memcpy ( munition ,  src->munition, sizeof ( munition ));

   maxresearchpoints = src->maxresearchpoints;
   researchpoints = src->researchpoints;

   if ( src->name )
      name = strdup ( src->name );
   else
      name = NULL;

   xpos = src->xpos;
   ypos = src->ypos;

   completion = src->completion;
   netcontrol = src->netcontrol;
   connection = src->connection;
   visible = src->visible;
   memcpy ( productionbuyable , src->productionbuyable, sizeof ( productionbuyable ));

   bi_resourceplus = src->bi_resourceplus;

   lastmineddist = src->lastmineddist;
   repairedThisTurn = src->repairedThisTurn;

   for ( int i = 0; i < 32; i++ )
     if ( src->loading[i] )
        loading[i] = new tvehicle ( src->loading[i], actmap );
     else
        loading[i] = NULL;

   if ( actmap )
      actmap->chainbuilding ( this );

}

bool Building::canRepair ( void )
{
   return typ->special & cgrepairfacilityn;
}




#ifdef converter
//int Building :: vehicleloadable ( pvehicle vehicle, int uheight )
//{
//   return 0;
//}

//void* Building :: getpicture ( int x, int y )
//{
//   return NULL;
//}

#endif






void Building :: convert ( int col )
{

   if (col > 8)
      fatalError("convertbuilding: \n color muá im bereich 0..8 sein ");

   int oldcol = color >> 3;

   #ifdef sgmain
   if ( oldcol == 8 )
      for ( int r = 0; r < 3; r++ )
         if ( gamemap->isResourceGlobal( r )) {
            gamemap->bi_resource[col].resource(r) += actstorage.resource(r);
            actstorage.resource(r) = 0;
         }

   #endif

   if ( !prev && !next ) {
      gamemap->player[oldcol].firstbuilding = NULL;
      gamemap->player[oldcol].queuedEvents++;
   }
   else {

      if ( prev )
         prev->next = next;
      else
         gamemap->player[oldcol].firstbuilding = next;

      if ( next )
         next->prev = prev;

   }
   color = col << 3;

   pbuilding pe = gamemap->player[col].firstbuilding;
   gamemap->player[ col ].firstbuilding = this;
   prev = NULL;
   next = pe;
   if ( pe )
      pe->prev = this;

   for ( int i = 0; i < 32; i++)
      if ( loading[i] ) {
         loading[i]->convert ( col );
//         loading[i]->cmpchecked = 0;
      }

      if ( connection & cconnection_conquer )
         gamemap->player[oldcol].queuedEvents++;
         // releaseevent(NULL,this,cconnection_conquer);

      if ( connection & cconnection_lose )
         gamemap->player[oldcol].queuedEvents++;
         // releaseevent(NULL,this,cconnection_lose);

}



void* Building :: getpicture ( int x, int y )
{
//                      if ( bld->typ->id == 8 ) {          // Windkraftwerk

   pfield fld = getField ( x, y );
   if ( fld ) {
      int w = fld->getweather();

      #ifdef HEXAGON
       if ( typ->w_picture[w][completion][x][y] )
          return typ->w_picture[w][completion][x][y];
       else
          return typ->w_picture[0][completion][x][y];
      #else
       return typ->picture[completion][x][y];
      #endif
   } else
      return NULL;
}


int Building :: vehicleloadable ( pvehicle vehicle, int uheight ) const
{
   if ( uheight == -1 )
      uheight = vehicle->height;

   if ( vehicle->functions & cf_trooper )
      if ( uheight & (chschwimmend | chfahrend ))
         uheight |= (chschwimmend | chfahrend );  //these heights are effectively the same

   if ( completion ==  typ->construction_steps - 1 )
      if ( typ->loadcapability & uheight ) {
         if ( (( typ->loadcapacity >= vehicle->size())               // the unit is physically able to get "through the door"
           && ( vehiclesLoaded()+1 < maxloadableunits )
           && (( typ->unitheightreq & vehicle->typ->height ) || !typ->unitheightreq)
           && !( typ->unitheight_forbidden & vehicle->typ->height) )
                   ||
             ( vehicle->functions & cf_trooper )
           ) {
         //  && ( (uheight == typ->buildingheight)  || (typ->buildingheight >= chschwimmend && hgt == chfahrend) ))) {

         #ifdef karteneditor
              return 2;
         #else
              if ( color == gamemap->actplayer * 8)
                 return 2;
              else
                if ( !vehicle->attacked ) {
                   if ( color == (8 << 3) )      // neutral building can be conquered by any unit
                      return 2;
                   else
                      if ( (vehicle->functions & cf_conquer)  || ( damage >= mingebaeudeeroberungsbeschaedigung))
                         return 2;
                }
         #endif
         }
      }

/*
&&
         (
         (( color == actmap->actplayer * 8)                              // ganz regul„r: eigenes geb„ude

         || (( vehicle->functions & cftrooper )                // JEDES Geb„ude muá sich mit Fusstruppen erobern lassen
         && (( uheight == typ->height ) || (typ->height >= chschwimmend && hgt == chfahrend))
         && ( !vehicle->attacked ))
         // && color != (8 << 3)) )
         //&& ( typ->loadcapability & hgt ))
         ||

         ( (( damage >= mingebaeudeeroberungsbeschaedigung) || ( vehicle->functions & cfconquer ))    // bei Besch„digung oder cfconquer jedes Geb„ude mit fahrenden vehicle
         && (vehicle->height == chfahrend)
         // && ( color != (8 << 3))
         && ( !vehicle->attacked )
         && ( typ->loadcapability & hgt )
         && ( typ->height & vehicle->typ->height ))
         ||

         (( color == )                               // neutrale Geb„ude lassen sich immer erobern
         // && (vehicle->height == chfahrend)
         && ( !vehicle->attacked )
         && ( typ->loadcapability & hgt )
         && ( typ->height & vehicle->typ->height ) ))
       )

          return 2;
      else
           return 0;
*/
   return 0;
}


#ifndef sgmain
void Building :: changecompletion ( int d ) {}
int  Building :: getresourceplus ( int mode, Resources* plus, int queryonly ) { return 0;};
void Building :: execnetcontrol ( void ) {}
int  Building :: processmining ( int res, int abbuchen ) { return 0; }
void Building :: getresourceusage ( Resources* usage ) {  usage->energy = 0;
                                                           usage->material =  0;
                                                           usage->fuel = 0;
                                                         }
int Building :: putResource ( int amount, int resourcetype, int queryonly, int scope ) { return 0; };
int Building :: getResource ( int amount, int resourcetype, int queryonly, int scope ) { return 0; };
#else
void Building :: changecompletion ( int d )
{
  completion += d;
  resetPicturePointers ( );
}

#endif




int  Building :: chainbuildingtofield ( int x, int y )
{

   int orgx = x - typ->entry.x - (typ->entry.y & ~y & 1 );
   int orgy = y - typ->entry.y;

   int dx = orgy & 1;


   int a;
   for ( a = orgx; a <= orgx + 3; a++)
      for ( int b = orgy; b <= orgy + 5; b++)
         if ( typ->getpicture ( a - orgx, b - orgy )) {
            pfield f = gamemap->getField(a + compensatebuildingcoordinatex, b );
            if ( !f )
               return 1;

            if ( f->building )
               return 1;
         }

   for ( a = orgx; a <= orgx + 3; a++)
      for ( int b = orgy; b <= orgy + 5; b++)
         if ( typ->getpicture ( a - orgx, b - orgy )) {
            pfield field = gamemap->getField(a + compensatebuildingcoordinatex, b );
            if ( field->object ) {
               for ( int n = 0; n < field->object->objnum; n++ )
                  delete field->object->object[n];
               field->object->objnum = 0;
            }

            if ( field->vehicle ) {
               delete field->vehicle;
               field->vehicle = NULL;
            }

            field = gamemap->getField(a + compensatebuildingcoordinatex, b );
            field->building = this;

            // field->picture = gbde->typ->picture[compl][a - orgx][b - orgy];
            field->bdt &= ~( cbstreet | cbrailroad | cbpipeline | cbpowerline );
           }

   xpos = x;
   ypos = y;

   for ( int i = 0; i < 32; i++ )
      if ( loading[i] )
         loading[i]->setnewposition ( x, y );


   pfield field = getField( typ->entry.x, typ->entry.y );
   if ( field )
      field->bdt |= cbbuildingentry ;

   resetPicturePointers ();

   if ( gamemap )
      gamemap->calculateAllObjects();


   return 0;
}


int  Building :: unchainbuildingfromfield ( void )
{
   int set = 0;
   for (int i = 0; i <= 3; i++)
      for (int j = 0; j <= 5; j++)
         if ( typ->getpicture ( i, j ) ) {
            pfield fld = getField( i, j );
            if ( fld && fld->building == this ) {
               set = 1;
               fld->building = NULL;
               fld->picture = fld->typ->picture[0];
               // if ( fld->vehicle )
               //   removevehicle( &fld->vehicle );

               tterrainbits t1 = cbstreet | cbbuildingentry | cbrailroad | cbpowerline | cbpipeline;
               tterrainbits t2 = ~t1;

               fld->bdt &= t2;

               #ifdef sgmain
                if ( typ->destruction_objects[i][j] )
                   fld->addobject ( getobjecttype_forid ( typ->destruction_objects[i][j] ), -1, 1 );

               #endif

            }
         }
   return set;
}


void Building :: addview ( void )
{
   tcomputebuildingview bes ( gamemap );
   bes.init( this, +1 );
   bes.startsuche();
}

void Building :: removeview ( void )
{
   tcomputebuildingview bes ( gamemap );
   bes.init( this, -1 );
   bes.startsuche();
}


int Building :: getArmor ( void )
{
   return typ->_armor * gamemap->getgameparameter( cgp_buildingarmor ) / 100;
}

int Building :: gettank ( int resource )
{
   if ( gamemap && gamemap->_resourcemode == 1 )
      return typ->_bi_maxstorage.resource(resource);
   else
      return typ->_tank.resource(resource);
}



pfield        Building :: getField( int  x, int y)
{
  int      x1, y1;
  getFieldCoordinates ( x, y, x1, y1 );
  return gamemap->getField(x1,y1);
}


pfield        Building :: getEntryField( )
{
  return getField ( typ->entry.x, typ->entry.y );
}

MapCoordinate Building :: getEntry( )
{
  MapCoordinate e;
  getFieldCoordinates ( typ->entry.x, typ->entry.y, e.x, e.y );
  return e;
}



void         Building :: getFieldCoordinates( int x, int y, int &xx, int &yy)
{
  int      orgx, orgy;
  int     dx;

   orgx = xpos - typ->entry.x;
   orgy = ypos - typ->entry.y;

   dx = orgy & 1;

   orgx += (dx & (~ typ->entry.y));

   yy=orgy+y;
   xx=orgx+x-(dx & yy);
}

void        Building :: resetPicturePointers ( void )
{
   if ( visible )
      for (int x = 0; x < 4; x++)
         for ( int y = 0; y < 6; y++ )
            if ( getpicture ( x, y ) )
                getField ( x, y )->picture = getpicture ( x, y );
}



void    Building :: produceAmmo ( int type, int num )
{
   num = ((num +4) / 5)*5;
   Resources res;
   for( int j = 0; j< resourceTypeNum; j++ )
      res.resource(j) = cwaffenproduktionskosten[type][j] * num / 5;

   ContainerBase* cb = this;  // Really strange. Building is derived from Containerbase, but getResource doesn't work here
   Resources res2 = cb->getResource ( res, 1 );
   int perc = 100;
   for ( int i = 0; i< resourceTypeNum; i++ )
       perc = min ( perc, 100 * res2.resource(i) / res.resource(i) );
   int produceable = num * perc / 100 ;
   int produceablePackages = produceable / 5;

   for( int k = 0; k< resourceTypeNum; k++ )
      res.resource(k) = cwaffenproduktionskosten[type][k] * produceablePackages;

   cb->getResource ( res, 0 );

   munition[type] += produceablePackages * 5;
}
