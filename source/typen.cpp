/*
    This file is part of Advanced Strategic Command; http://www.asc-hq.de
    Copyright (C) 1994-2003  Martin Bickel  and  Marc Schellenberger

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

#include "global.h"
#include "misc.h"
#include "typen.h"
#include "graphicset.h"
#include "basegfx.h" 

#ifndef converter
#include "spfst.h"
#endif

#ifdef sgmain
 #include "network.h"
 #include "gameoptions.h"
#endif

#include "vehicletype.h"
#include "buildingtype.h"
#include "textfileparser.h"
#include "textfile_evaluation.h"

//! The different levels of height
const char*  choehenstufen[choehenstufennum] = {"deep submerged", "submerged", "floating", "ground level", "low-level flight", "flight", "high-level flight", "orbit"}; 

//! The different connections of something, be it unit, building, or field, to an event.
const char* cconnections[6]  = {"destroyed", "conquered", "lost", "seen", "area entererd by any unit", "area entered by specific unit"};

const char* ceventtriggerconn[8]  = {"AND ", "OR ", "NOT ", "( ", "(( ", ")) ", ") ", "Clear "}; 

//! All actions that can be performed by events
const char* ceventactions[ceventactionnum]  = {"message", "weather change", "new technology discovered", "lose campaign", "run script + next map",
                                               "new technology researchable", "map change", "discarded [ was erase event ]", "end campaign", "next map",
                                               "reinforcement","weather change completed", "new vehicle developed","palette change",
                                               "alliance change","wind change", "nothing", "change game parameter","paint ellipse","remove ellipse",
                                               "change building damage"};

const char* ceventtrigger[ceventtriggernum]  = {"*NONE*", "turn/move >=", "building conquered", "building lost",
                                                 "building destroyed", "unit lost", "technology researched",
                                                 "event", "unit conquered", "unit destroyed", "all enemy units destroyed",
                                                 "all units lost", "all enemy buildings destroyed/captured", "all buildings lost", 
                                                 "energy tribute <", "material tribute <", "fuel tribute <",
                                                 "any unit enters polygon", "specific unit enters polygon", "building is seen", "irrelevant (used internally)"};



const char*  cwaffentypen[cwaffentypennum]  = {"cruise missile", "mine",    "bomb",       "air - missile", "ground - missile", "torpedo", "machine gun",
                                               "cannon",         "service", "ammunition refuel", "laser (not implemented yet!)", "shootable"};

const char*  cmovemalitypes[cmovemalitypenum] = { "default",
                                                 "light tracked vehicle", "medium tracked vehicle", "heavy tracked vehicle",
                                                 "light wheeled vehicle", "medium wheeled vehicle", "heavy wheeled vehicle",
                                                 "trooper",               "rail vehicle",           "medium aircraft",
                                                 "medium ship",           "building / turret / object", "light aircraft",
                                                 "heavy aircraft",        "light ship",             "heavy ship",  "helicopter",
                                                 "hoovercraft"  };

const char* cnetcontrol[cnetcontrolnum] = { "store energy",           "store material",           "store fuel",           
                                            "move out all energy",           "move out all material",           "move out all fuel", 
                                            "stop storing energy", "stop storing material", "stop storing fuel", 
                                            "stop energy extraction", "stop material extraction", "stop fuelextraction" };
const char* cgeneralnetcontrol[4] = {       "store",  "move out", "stop storing", "stop using" };
                                          // Functionen in Geb„uden ....

const char*  cwettertypen[cwettertypennum] = {"dry (standard)","light rain", "heavy rain", "few snow", "lot of snow", "lot of snow + ice"};
const char*  resourceNames[3]  = {"energy", "material", "fuel"}; 
const int  cwaffenproduktionskosten[cwaffentypennum][3]    = {{20, 15, 10}, {2, 2, 0}, {3, 2, 0}, {3, 3, 2}, {3, 3, 2}, {4, 3, 2},
                                                              {1, 1, 0},    {1, 2, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}; // jeweils f?r weaponpackagesize Pack !

//! when repairing a unit, the experience of the unit is decreased by one when passing each of these damage levels
const int experienceDecreaseDamageBoundaries[experienceDecreaseDamageBoundaryNum] = { 80, 60, 40, 20 };

const int directionangle [ sidenum ] = 
 { 0, -53, -127, -180, -180 -53 , -180 -127 };

const int resourceWeight[ resourceTypeNum ] = { 0, 10, 4 };




const int csolarkraftwerkleistung[cwettertypennum] = { 1024, 512, 256, 756, 384, 384 }; // 1024 ist Maximum







ResourceMatrix :: ResourceMatrix ( const float* f )
{
   int num = 0;
   for ( int i = 0; i < resourceTypeNum; i++ )
      for ( int j = 0; j < resourceTypeNum; j++ )
         e[i][j] = f[num++];
}


Resources ResourceMatrix :: operator* ( const Resources& r ) const
{
   Resources res;
   for ( int i = 0; i < resourceTypeNum; i++ )
      for ( int j = 0; j < resourceTypeNum; j++ )
         res.resource(i) += int( e[i][j] * r.resource(j));

   return res;
}


void Resources :: read ( tnstream& stream )
{
   for ( int i = 0; i< resourceTypeNum; i++ )
      resource(i) = stream.readInt();
}

void Resources :: write ( tnstream& stream ) const
{
   for ( int i = 0; i< resourceTypeNum; i++ )
      stream.writeInt( resource(i) );
}


Resources operator- ( const Resources& res1, const Resources& res2 )
{
   Resources res = res1;
   res -= res2;
   return res;
}

Resources operator+ ( const Resources& res1, const Resources& res2 )
{
   Resources res = res1;
   res += res2;
   return res;
}

Resources operator* ( const Resources& res1, float a )
{
   Resources res = res1;
   for ( int r = 0; r < resourceTypeNum; r++ )
      res.resource(r) = int( res.resource(r) * a );
   return res;
}

Resources operator/ ( const Resources& res1, float a )
{
   Resources res = res1;
   for ( int r = 0; r < resourceTypeNum; r++ )
      res.resource(r) = int( res.resource(r) / a );
   return res;
}

Resources Resources::operator* ( double d )
{
   Resources rs;
   for ( int i = 0; i < resourceTypeNum; i++ )
      rs.resource(i) = int( resource(i)*d );
   return rs;
}


void Resources::runTextIO ( PropertyContainer& pc )
{
   pc.addInteger  ( "Energy", energy );
   pc.addInteger  ( "Material", material );
   pc.addInteger  ( "fuel", fuel );
}

void Resources::runTextIO ( PropertyContainer& pc, const Resources& defaultValue )
{
   pc.addInteger  ( "Energy", energy, defaultValue.energy );
   pc.addInteger  ( "Material", material, defaultValue.material );
   pc.addInteger  ( "fuel", fuel, defaultValue.fuel );
}


////////////////////////////////////////////////////////////////////



tevent::LargeTriggerData::PolygonEntered :: PolygonEntered ( void )
{
   dataSize = 0;
   vehiclenetworkid = 0;
   data = NULL;
   tempnwid = 0;
   tempxpos = 0;
   tempypos = 0;
   color = 0;
   memset ( reserved, 0, sizeof ( reserved ));
}

tevent::LargeTriggerData::PolygonEntered :: PolygonEntered ( const tevent::LargeTriggerData::PolygonEntered& poly )
{
   dataSize = poly.dataSize;
   vehiclenetworkid = poly.vehiclenetworkid;
   if ( poly.data ) {
      data = new int [dataSize];
      for ( int i = 0; i < dataSize; i++ )
         data[i] = poly.data[i];
   } else
      data = NULL;
   tempnwid = poly.tempnwid;
   tempxpos = poly.tempxpos;
   tempypos = poly.tempypos;
   color = poly.color;
   memcpy ( reserved, poly.reserved, sizeof ( reserved ));
}

tevent::LargeTriggerData::PolygonEntered :: ~PolygonEntered ( )
{
   if ( data ) {
      delete[] data;
      data = NULL;
   }
}


tevent::LargeTriggerData :: LargeTriggerData ( void )
{
   time.abstime = 0;
   xpos = -1;
   ypos = -1;
   building = NULL;
   mapid = 0;
   id = -1;
   unitpolygon = NULL;
   networkid = 0;
   memset ( reserved, 0, sizeof ( reserved ));
}

tevent::LargeTriggerData :: LargeTriggerData ( const LargeTriggerData& data )
{
   time = data.time;
   xpos = data.xpos;
   ypos = data.ypos;
   networkid = data.networkid;
   building = data.building;
   mapid = data.mapid;
   id = data.id;
   if ( data.unitpolygon ) {
      unitpolygon = new tevent::LargeTriggerData::PolygonEntered ( *data.unitpolygon );
   } else
      unitpolygon = NULL;
}

tevent::LargeTriggerData :: ~LargeTriggerData ( )
{
   if ( unitpolygon ) {
      delete unitpolygon;
      unitpolygon = NULL;
   }
}

tevent :: tevent ( void )
{
   a.action= 255;
   a.saveas = 0;
   a.num = 0;
   player = 0; 
   rawdata = NULL;
   next = NULL;
   datasize = 0;
   conn = 0;
   for ( int i = 0; i < 4; i++) { 
      trigger[i] = 0;
      triggerstatus[i] = 0;
      triggerconnect[i] = 0; 
      triggertime.set( -1, -1 );
      trigger_data[i] = new LargeTriggerData;
   }
   delayedexecution.turn = 0;
   delayedexecution.move = 0;
   description[0] = 0;
   triggertime.abstime = -1;
}

tevent :: tevent ( const tevent& event )
{
   id = event.id;
   player = event.player;
   strcpy ( description, event.description );
   rawdata = event.rawdata;
   datasize  = event.datasize;
   if ( datasize && event.rawdata ) {
      chardata = new char [ datasize ];
      memcpy ( rawdata, event.rawdata, datasize );
   } else
      datasize = 0;
   next = event.next;
   conn = event.conn;
   for ( int i = 0; i < 4; i++ ) {
      trigger[i] = event.trigger[i];
      if ( event.trigger_data[i] )
         trigger_data[i] = new LargeTriggerData ( *event.trigger_data[i] );
      else
         trigger_data[i] = NULL;

      triggerconnect[i] = event.triggerconnect[i];
      triggerstatus[i] = event.triggerstatus[i]; 
   }
   triggertime = event.triggertime;
   delayedexecution = event.delayedexecution;
}


tevent :: ~tevent ()
{
   for ( int i = 0; i < 4; i++ )
      if ( trigger_data[i] ) {
        delete trigger_data[i];
        trigger_data[i] = NULL;
      }

   if ( chardata ) {
      delete[]  chardata ;
      chardata = NULL;
   }
}




