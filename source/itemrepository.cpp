/***************************************************************************
                          itemrepository.cpp   -  description
                             -------------------
    begin                : Thu Jul 28 2001
    copyright            : (C) 2001 by Martin Bickel
    email                : bickel@asc-hq.org
 ***************************************************************************/
/*! \file itemrepository.cpp
    \brief Storage facility for all buildingtypes, vehicletypes etc 
*/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <map>
#include <vector>
#include "typen.h"
#include "itemrepository.h"
#include "textfileparser.h"
#include "sgstream.h"
#include "textfile_evaluation.h"

#ifndef converter
#  include "dialog.h"
#  include "sg.h"
#else
 class ProgressBar {
    public:
       void point() {};
 };
ProgressBar* actprogressbar = NULL;
#endif 

pobjecttype streetobjectcontainer = NULL;
pobjecttype pathobject = NULL;
pobjecttype railroadobject = NULL;
pobjecttype runwayobject = NULL;
pobjecttype eisbrecherobject = NULL;
pobjecttype fahrspurobject = NULL;




  TerrainTypeVector terrain;
  TerrainTypeVector& getterraintypevector ( void ) 
  {
     return terrain;
  }

  VehicleTypeVector vehicletypes; 
  VehicleTypeVector& getvehicletypevector ( void ) 
  {
     return vehicletypes;
  }

  BuildingTypeVector buildingtypes;
  BuildingTypeVector& getbuildingtypevector ( void ) 
  {
     return buildingtypes;
  }

  ObjectTypeVector objecttypes;
  ObjectTypeVector& getobjecttypevector ( void ) 
  {
     return objecttypes;
  }

  dynamic_array<ptechnology> technology; 



  int terraintypenum = 0;
  int vehicletypenum = 0;
  int buildingtypenum = 0;
  int technologynum = 0;
  int objecttypenum = 0;


   typedef map< int, pterraintype>  TerrainMap;
   TerrainMap terrainmap;

   typedef map< int, pobjecttype>  ObjectMap;
   ObjectMap objectmap;

   typedef map< int, pvehicletype>  VehicleMap;
   VehicleMap vehiclemap;

   typedef map< int, pbuildingtype>  BuildingMap;
   BuildingMap buildingmap;

   map< int, ptechnology>  technologymap;

pterraintype getterraintype_forid ( int id )
{
   return terrainmap[id];
}
pobjecttype getobjecttype_forid ( int id )
{
   return objectmap[id];
}
pvehicletype getvehicletype_forid ( int id )
{
   return vehiclemap[id];
}
pbuildingtype getbuildingtype_forid ( int id )
{
   return buildingmap[id];
}
ptechnology gettechnology_forid ( int id )
{
   return technologymap[id];
}


pterraintype getterraintype_forpos ( int pos )
{
   if ( pos < terraintypenum )
      return terrain[pos];
   else
      return NULL;
}
pobjecttype getobjecttype_forpos ( int pos )
{
   if ( pos < objecttypenum )
      return objecttypes[pos];
   else
      return NULL;
}
pvehicletype getvehicletype_forpos ( int pos )
{
   if ( pos < vehicletypenum )
      return vehicletypes[pos];
   else
      return NULL;
}
pbuildingtype getbuildingtype_forpos ( int pos )
{
   if ( pos < buildingtypenum )
      return buildingtypes[pos];
   else
      return NULL;
}

ptechnology gettechnology_forpos ( int pos )
{
   if ( pos < technologynum )
      return technology[pos];
   else
      return NULL;
}


void duplicateIDError ( const ASCString& itemtype, int id, const ASCString& file1, const ASCString& name1, const ASCString&  file2, const ASCString& name2 )
{
   fatalError ( "Conflicting IDs !\n"
                "These two " + itemtype + "s use both the ID of " + strrr ( id ) + "\n" +
                " " + name1 + " from file " + file1 + "\n" +
                " " + name2 + " from file " + file2 + "\n\n" +
                "This is NOT a bug of ASC, it is conflict between two data files.");

}

void addterraintype ( pterraintype bdt )
{
   if ( bdt ) {
      TerrainMap::iterator i = terrainmap.find ( bdt->id );
      if ( i != terrainmap.end() )
         duplicateIDError ( "terraintype", bdt->id, bdt->location, bdt->name, i->second->location, i->second->name );

      terrain[ terraintypenum++] = bdt;
      terrainmap[bdt->id] = bdt;
   }
}



void addobjecttype ( pobjecttype obj )
{
   if ( obj ) {

      ObjectMap::iterator i = objectmap.find ( obj->id );
      if ( i != objectmap.end() && i->second )
         duplicateIDError ( "objecttype", obj->id, obj->location, obj->name, i->second->location, i->second->name );

      objecttypes[ objecttypenum++] = obj;
      objectmap[obj->id] = obj;

      if ( obj->id == 9 )
         pathobject = obj;

      if ( obj->id == 1 )
         streetobjectcontainer = obj;

      if ( obj->id == 2 )
         railroadobject = obj;

      if ( obj->id == 6 )
         eisbrecherobject = obj;

      if ( obj->id == 7 )
         fahrspurobject = obj;
   }
}
void addvehicletype ( pvehicletype vhcl )
{
   if ( vhcl ) {

      VehicleMap::iterator i = vehiclemap.find ( vhcl->id );
      if ( i != vehiclemap.end() )
         duplicateIDError ( "vehicletype", vhcl->id, vhcl->location, vhcl->getName(), i->second->location, i->second->getName() );

      vehicletypes[ vehicletypenum++] = vhcl;
      vehiclemap[vhcl->id] = vhcl;
   }
}
void addbuildingtype ( pbuildingtype bld )
{
   if ( bld ) {

      BuildingMap::iterator i = buildingmap.find ( bld->id );
      if ( i != buildingmap.end() )
         duplicateIDError ( "buildingtype", bld->id, bld->location, bld->name, i->second->location, i->second->name );

      buildingtypes[ buildingtypenum++] = bld;
      buildingmap[bld->id] = bld;
   }
}

void addtechnology ( ptechnology tech )
{
   if ( tech ) {
      technology[ technologynum++] = tech;
      technologymap[tech->id] = tech;
   }
}



TextFileRepository textFileRepository;

void  loadalltextfiles ( )
{
   tfindfile ff ( "*.asctxt" );
   ASCString c = ff.getnextname();

   while( !c.empty() ) {
      if ( actprogressbar )
         actprogressbar->point();

      tnfilestream s ( c, tnstream::reading );

      displayLogMessage ( 6, "loadalltextfiles :: loading " + s.getLocation() + ", " );

      TextFormatParser tfp ( &s );

      displayLogMessage ( 5, "TFP running... " );

      TextPropertyGroup* tpg = tfp.run();

      textFileRepository[tpg->typeName].push_back ( tpg );

      displayLogMessage ( 5, "done\n" );

      c = ff.getnextname();
   }
   displayLogMessage ( 4, "loadalltextfiles completed\n");

   displayLogMessage ( 4, "Building inheritance...");

   for ( TextFileRepository::iterator i = textFileRepository.begin(); i != textFileRepository.end(); i++ ) {
      i->second.buildIDs();
      for ( TextPropertyList::iterator j = i->second.begin(); j != i->second.end(); j++ ) {
          (*j)->buildInheritance( i->second );
      }
   }
   displayLogMessage ( 4, "done\n");
}


void  freetextdata()
{
   textFileRepository.clear();
}



void         loadallvehicletypes(void)
{
   tfindfile ff ( "*.veh" );
   string c = ff.getnextname();

   while ( !c.empty() ) {
       if ( actprogressbar )
          actprogressbar->point();

       addvehicletype ( loadvehicletype( c.c_str() ) );

       c = ff.getnextname();
    }

    TextPropertyList& tpl = textFileRepository["vehicletype"];
    for ( TextPropertyList::iterator i = tpl.begin(); i != tpl.end(); i++ ) {
       if ( actprogressbar )
         actprogressbar->point();

       if ( !(*i)->isAbstract() ) {

        PropertyReadingContainer pc ( "vehicletype", *i );

        Vehicletype* vt = new Vehicletype;
        vt->runTextIO ( pc );
        pc.run();

        vt->filename = (*i)->fileName;
        vt->location = (*i)->location;
        addvehicletype ( vt );
     }
   }
   displayLogMessage ( 4, "loadallVehicleTypes completed\n");
}


void         loadallobjecttypes (void)
{
   tfindfile ff ( "*.obl" );

   string c = ff.getnextname();

   while ( !c.empty() ) {
       if ( actprogressbar )
          actprogressbar->point();

          addobjecttype ( loadobjecttype( c.c_str() ));

       c = ff.getnextname();
    }


    TextPropertyList& tpl = textFileRepository["objecttype"];
    for ( TextPropertyList::iterator i = tpl.begin(); i != tpl.end(); i++ ) {
       if ( actprogressbar )
         actprogressbar->point();

       if ( !(*i)->isAbstract() ) {
          displayLogMessage ( 8, "parsing object type %s ", (*i)->location.c_str());

          PropertyReadingContainer pc ( "objecttype", *i );

          ObjectType* ot = new ObjectType;
          ot->runTextIO ( pc );
          pc.run();

          ot->filename = (*i)->fileName;
          ot->location = (*i)->location;
          addobjecttype ( ot );
          displayLogMessage ( 8, "done\n");
       }
   }

   displayLogMessage ( 4, "loadallObjectTypes completed\n");
}



void         loadalltechnologies(void)
{
  int i;

  tfindfile ff ( "*.tec" );
  string c = ff.getnextname();

  while ( !c.empty() ) {
      if ( actprogressbar )
         actprogressbar->point();

      addtechnology ( loadtechnology( c.c_str() ));

      c = ff.getnextname();
   }

   for (i = 0; i < technologynum; i++)
      for (int l = 0; l < 6; l++) {
         ptechnology tech = gettechnology_forpos ( i );
         int j = tech->requiretechnologyid[l];
         if ( j > 0 )
            tech->requiretechnology[l] = gettechnology_forid ( j );
      }

   for (i = 0; i < technologynum; i++)
      gettechnology_forpos ( i ) -> getlvl();

   displayLogMessage ( 4, "loadallTechnologies completed\n");
}


void         loadallterraintypes(void)
{
   tfindfile ff ( "*.trr" );

   string c = ff.getnextname();

   while( !c.empty() ) {
      if ( actprogressbar )
         actprogressbar->point();

      addterraintype ( loadterraintype( c.c_str() ));

      c = ff.getnextname();
   }

   TextPropertyList& tpl = textFileRepository["terraintype"];
   for ( TextPropertyList::iterator i = tpl.begin(); i != tpl.end(); i++ ) {
      if ( actprogressbar )
        actprogressbar->point();

      if ( !(*i)->isAbstract() ) {
        PropertyReadingContainer pc ( "terraintype", *i );

        TerrainType* tt = new TerrainType;
        tt->runTextIO ( pc );
        pc.run();

        tt->filename = (*i)->fileName;
        tt->location = (*i)->location;
        addterraintype ( tt );
      }
   }

   displayLogMessage ( 4, "loadallTerrainTypes completed\n");
}




void         loadallbuildingtypes(void)
{
   tfindfile ff ( "*.bld" );

   string c = ff.getnextname();

   while( !c.empty() ) {
      if ( actprogressbar )
         actprogressbar->point();

      addbuildingtype ( loadbuildingtype( c.c_str() ));

      c = ff.getnextname();
   }

   TextPropertyList& tpl = textFileRepository["buildingtype"];
   for ( TextPropertyList::iterator i = tpl.begin(); i != tpl.end(); i++ ) {
      if ( actprogressbar )
        actprogressbar->point();

      if ( !(*i)->isAbstract() ) {
        PropertyReadingContainer pc ( "buildingtype", *i );

        BuildingType* bt = new BuildingType;
        bt->runTextIO ( pc );
        pc.run();

        bt->filename = (*i)->fileName;
        bt->location = (*i)->location;
        addbuildingtype ( bt );
      }
   }

   displayLogMessage ( 4, "loadallBuildingTypes completed\n");

}



vector<ItemFiltrationSystem::ItemFilter*> ItemFiltrationSystem::itemFilters;


ItemFiltrationSystem::ItemFilter::ItemFilter( const ASCString& _name, const IntRangeArray& unitsetIDs, bool _active )
{
   name = "UnitSet: " + _name;
   units = unitsetIDs;
   active = _active;
}


void ItemFiltrationSystem::ItemFilter::runTextIO ( PropertyContainer& pc )
{
    pc.addIntRangeArray ( "Buildings", buildings );
    pc.addIntRangeArray ( "Vehicles", units );
    pc.addIntRangeArray ( "Objects", objects );
    pc.addIntRangeArray ( "Terrain", terrain );
    pc.addBool ( "activated", active, false );
    pc.addString ( "name", name );
}



bool ItemFiltrationSystem::ItemFilter::isContained ( IntRangeArray& arr, int id )
{
   for ( IntRangeArray::iterator i = arr.begin(); i != arr.end(); i++ )
      if ( id >= i->from && id <= i->to )
         return true;
   return false;
}



bool ItemFiltrationSystem::ItemFilter::isContained ( ItemFiltrationSystem::Category cat, int id )
{
   switch ( cat ) {
      case Building: return isContained ( buildings, id );
      case Vehicle: return isContained ( units, id );
      case Object: return isContained ( objects, id );
      case Terrain: return isContained ( terrain, id );
   };
   return false;
}

bool ItemFiltrationSystem::isFiltered ( ItemFiltrationSystem::Category cat, int id )
{
   for ( vector<ItemFilter*>::iterator i = itemFilters.begin(); i != itemFilters.end(); i++ )
      if ( (*i)->isContained ( cat, id ) )
         if ( (*i)->isActive() )
            return true;

   return false;
}


void ItemFiltrationSystem::read ( )
{
    TextPropertyList& tpl = textFileRepository["itemfilter"];
    for ( TextPropertyList::iterator i = tpl.begin(); i != tpl.end(); i++ ) {

      PropertyReadingContainer pc ( "itemfilter", *i );

      ItemFilter* itf = new ItemFilter;
      itf->runTextIO ( pc );
      pc.run();

      // bmtt->filename = (*i)->fileName;
      // bmtt->location = (*i)->location;
      itemFilters.push_back ( itf );
   }
}


