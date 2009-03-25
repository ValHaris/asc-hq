/*! \file clipboard.cpp
    \brief The clipboard for ASC
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


#include "clipboard.h"
#include "vehicle.h"
#include "buildings.h"
#include "gamemap.h"
#include "spfst.h"
#include "dlg_box.h"
#include "statistics.h"


const char* clipboardFileExtension = "*.ascclipboard";


ClipBoardBase::ClipBoardBase()
{
   objectNum = 0;
//   TClipboard* cb = Clipboard();
}

void ClipBoardBase::clear()
{
   buf.clear();
   properties.clear();
   objectNum = 0;
}


void ClipBoardBase::setProperties( const ContainerBase* unit, StatisticsCalculator& stat )
{
   properties["strength"] = ASCString::toString( stat.strength() );
   properties["materialindex"] = ASCString::toString( stat.resource().material / 1000 );
   properties["name"] = unit->getName();
   properties["player"] = unit->getMap()->getPlayer( unit ).getName();
   properties["turn"] = ASCString::toString( unit->getMap()->time.turn() );
   properties["map"] = unit->getMap()->maptitle;
}

void ClipBoardBase::addUnit ( const Vehicle* unit )
{
  tmemorystream stream ( &buf, tnstream::appending );
  stream.writeInt( ClipVehicle );
  unit->write ( stream );
  objectNum++;
  
  StatisticsCalculator sc( unit );
  setProperties( unit, sc );
}

void ClipBoardBase::addBuilding ( const Building* bld )
{
  tmemorystream stream ( &buf, tnstream::appending );
  stream.writeInt( ClipBuilding );
  bld->write ( stream );
  objectNum++;
  
  StatisticsCalculator sc( bld );
  setProperties( bld, sc );

}


Vehicle* ClipBoardBase::pasteUnit( tnstream& stream )
{
   Vehicle* veh = Vehicle::newFromStream( actmap, stream );

   // veh->networkid = actmap->getNewNetworkID();

   return veh;
}

Vehicle* ClipBoardBase::pasteUnit(  )
{
  if ( !objectNum )
     return NULL;

  tmemorystream stream ( &buf, tnstream::reading );
  Type type = Type(stream.readInt());
  if ( type == ClipVehicle )
     return pasteUnit ( stream );

  return NULL;
}


void ClipBoardBase::place ( const MapCoordinate& pos )
{
  if ( !objectNum )
     return;

  tmemorystream stream ( &buf, tnstream::reading );
  Type type = Type(stream.readInt());
  if ( type == ClipVehicle ) {
     tfield* fld = actmap->getField ( pos );
     Vehicle* veh = pasteUnit ( stream );

     if ( !fieldAccessible ( fld, veh, -2, NULL, true ) && !actmap->getgameparameter( cgp_movefrominvalidfields) ) {
        delete veh;
        return;
     }

     if ( fld->vehicle )
        delete fld->vehicle;
     fld->vehicle = veh;
     veh->setnewposition( pos.x, pos.y );
  }
  if ( type == ClipBuilding ) {
     Building* bld = Building::newFromStream ( actmap, stream, false );

     for ( int x = 0; x < 4; x++ )
        for ( int y = 0; y < 6; y++ )
           if ( bld->typ->fieldExists ( BuildingType::LocalCoordinate( x , y ) )) {
              tfield* field = actmap->getField( bld->typ->getFieldCoordinate( pos, BuildingType::LocalCoordinate( x, y) ));
              if ( !field ) {
                 delete bld;
                 displaymessage("building does not fit here", 1 );
                 return;
              }

              /*
              if ( !bld->typ->terrainaccess.accessible ( field->bdt ) ) {
                 delete bld;
                 displaymessage("building does can not be build here", 1 );
                 return;
              }
              */


              if ( field->vehicle ) {
                 delete field->vehicle;
                 field->vehicle = NULL;
              }
              if ( field->building ) {
                 delete field->building;
                 field->building = NULL;
              }
           }


     bld->chainbuildingtofield( pos );
  }
}

static int clipboardVersion = 1;

void ClipBoardBase::write( tnstream& stream )
{
   stream.writeInt( clipboardVersion );
   stream.writeInt( objectNum );
   buf.writetostream ( &stream );
}

void ClipBoardBase::read( tnstream& stream )
{
   stream.readInt(); // Version
   objectNum = stream.readInt();
   buf.readfromstream ( &stream );
}

void ClipBoardBase::writeProperties( PropertyContainer& pc ) const
{
   for ( map<ASCString,ASCString>::const_iterator i = properties.begin(); i != properties.end(); ++i ) {
      ASCString temp = i->second;
      pc.addString( i->first, temp );
   }
}

