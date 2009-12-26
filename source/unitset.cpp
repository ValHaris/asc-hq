/*! \file sgstream.cpp
    \brief The IO for many basic classes and structurs of ACS
   
    These routines are gradually being moved to become methods of their classes
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



#include "unitset.h"
#include "vehicletype.h"
#include "buildingtype.h"



bool SingleUnitSet :: isMember ( int id, Type type )
{
   if ( type == unit ) {
      for ( int i = 0; i < unitIds.size(); i++ )
        if ( id >= unitIds[i].from && id <= unitIds[i].to )
           return true;
   }
   if ( type == building ) {
      for ( int i = 0; i < buildingIds.size(); i++ )
        if ( id >= buildingIds[i].from && id <= buildingIds[i].to )
           return true;
   }
   return false;
}


std::vector<IntRange> SingleUnitSet::parseIDs ( const char* s )
{
   char buf[10000];

   std::vector<IntRange> res;

   if ( s && s[0] ) {

      strcpy ( buf, s);

      char* piclist = strtok ( buf, ";\r\n" );

      char* pic = strtok ( piclist, "," );
      while ( pic ) {
         int from, to;
         if ( strchr ( pic, '-' )) {
            char* a = strchr ( pic, '-' );
            *a = 0;
            from = atoi ( pic );
            to = atoi ( ++a );
         } else
            from = to = atoi ( pic );

         IntRange ir;
         ir.from = from;
         ir.to = to;
         res.push_back ( ir );

         pic = strtok ( NULL, "," );
      }
   }
   return res;
}

void SingleUnitSet::TranslationTable::parseString ( const char* s )
{
   if ( s && s[0] && strchr ( s, ';' )) {
      char buf[10000];
      if ( s[0] == '#' )
         strcpy ( buf, s+1 );
      else
         strcpy ( buf, s );

      char* tname = strtok ( buf, ";\n\r");
      if ( tname )
         name = tname;

      char* xl = strtok ( NULL, ";\n\r" );
      while ( xl ) {
         if ( strchr ( xl, ',' )) {
            char* a = strchr ( xl, ',' );
            *a = 0;
            IntRange ir;
            ir.from = atoi ( xl );
            ir.to = atoi ( ++a );

            translation.push_back ( ir );

         }
         xl = strtok ( NULL, ";\n\r" );
      }

   }
}


void SingleUnitSet::read ( tnstream* stream )
{
   if ( !stream )
      return;
   const char separator = '=';
   ASCString s;
   int data = stream->readTextString ( s );
   if ( s == "#V2#" ) {
      while ( data ) {
         ASCString s2;
         data = stream->readTextString ( s2 );

         size_t seppos = s2.find_first_of ( separator );
         if ( seppos >= 0 ) {
            ASCString b = s2.substr(0, seppos);
            ASCString e = s2.substr( seppos+1 );
            if ( b == "NAME" )
               name = e;

            if ( b == "ACTIVE" )
               active = atoi ( e.c_str() );


            if ( b == "TRANSFORMATION" ) {
               TranslationTable* tt = new TranslationTable;
               tt->parseString ( e.c_str() );
               transtab.push_back ( tt );
            }

            if ( b == "MAINTAINER" )
               maintainer = e;

            if ( b == "INFORMATION" )
               information = e;

            if ( b == "FILTERBUILDINGS" )
               filterBuildings = atoi ( e.c_str() );

            if ( b == "IDENTIFICATION" )
               ID = atoi ( e.c_str() );


            if ( b == "ID" )
               unitIds = parseIDs ( e.c_str() );

            if ( b == "BUILDINGID" )
               buildingIds = parseIDs ( e.c_str() );

         }
      }
   } else {
      size_t seppos = s.find_first_of ( ';' );
      if ( seppos >= 0 ) {
         ASCString b = s.substr(0, seppos);
         ASCString e = s.substr( seppos+1 );
         name = b;
         parseIDs ( e.c_str() );

         while ( data ) {
            ASCString s2;
            data = stream->readTextString ( s2 );
            if ( s2.length() ) {
               TranslationTable* tt = new TranslationTable;
               tt->parseString ( s2.c_str() );
               transtab.push_back ( tt );
            }
         }
      }
   }
}


void loadUnitSets ( void )
{
   displayLogMessage ( 4, "loading unit set definition files\n" );
   tfindfile ff ( "*.set" );
   string n = ff.getnextname();
   while ( !n.empty() ) {
      displayLogMessage ( 5, " loading unit set definition file %s ... ",n.c_str() );
      tnfilestream stream ( n.c_str(), tnstream::reading );

      SingleUnitSet* set = new SingleUnitSet;
      set->read ( &stream );
      unitSets.push_back ( set );

//      ItemFiltrationSystem::ItemFilter* itf = new ItemFiltrationSystem::ItemFilter ( set->name, set->ids, !set->active );
//      ItemFiltrationSystem::itemFilters.push_back ( itf );

      n = ff.getnextname();
      displayLogMessage ( 5, "done\n" );
   } /* endwhile */
}

UnitSets unitSets;




int getUnitSetID( const VehicleType* veh )
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

bool vehicleComp( const VehicleType* v1, const VehicleType* v2 )
{
   int id1 = getUnitSetID(v1);
   int id2 = getUnitSetID(v2);
   return (id1 <  id2) ||
          (id1 == id2 && v1->movemalustyp  < v2->movemalustyp ) ||
          (id1 == id2 && v1->movemalustyp == v2->movemalustyp && v1->name < v2->name);
}

