/***************************************************************************
                          itemrepository.cpp   -  description
                             -------------------
    begin                : Thu Jul 28 2001
    copyright            : (C) 2001 by Martin Bickel
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

#include <map>
#include <vector>
#include "typen.h"
#include "itemrepository.h"
#include "textfileparser.h"
#include "sgstream.h"

#ifndef converter
 #include "dialog.h"
 #include "sg.h"
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


   map< int, pterraintype>  terrainmap;
   map< int, pobjecttype>  objectmap;
   map< int, pvehicletype>  vehiclemap;
   map< int, pbuildingtype>  buildingmap;
   map< int, ptechnology>  technologymap;

pterraintype getterraintype_forid ( int id, int crccheck )
{
   return terrainmap[id];
}
pobjecttype getobjecttype_forid ( int id, int crccheck  )
{
   return objectmap[id];
}
pvehicletype getvehicletype_forid ( int id, int crccheck  )
{
   return vehiclemap[id];
}
pbuildingtype getbuildingtype_forid ( int id, int crccheck  )
{
   return buildingmap[id];
}
ptechnology gettechnology_forid ( int id, int crccheck  )
{
   return technologymap[id];
}


pterraintype getterraintype_forpos ( int pos, int crccheck  )
{
   if ( pos < terraintypenum )
      return terrain[pos];
   else
      return NULL;
}
pobjecttype getobjecttype_forpos ( int pos, int crccheck  )
{
   if ( pos < objecttypenum )
      return objecttypes[pos];
   else
      return NULL;
}
pvehicletype getvehicletype_forpos ( int pos, int crccheck  )
{
   if ( pos < vehicletypenum )
      return vehicletypes[pos];
   else
      return NULL;
}
pbuildingtype getbuildingtype_forpos ( int pos, int crccheck  )
{
   if ( pos < buildingtypenum )
      return buildingtypes[pos];
   else
      return NULL;
}
ptechnology gettechnology_forpos ( int pos, int crccheck  )
{
   if ( pos < technologynum )
      return technology[pos];
   else
      return NULL;
}

void addterraintype ( pterraintype bdt )
{
   if ( bdt ) {
      terrain[ terraintypenum++] = bdt;
      terrainmap[bdt->id] = bdt;
   }
}
void addobjecttype ( pobjecttype obj )
{
   if ( obj ) {
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
      vehicletypes[ vehicletypenum++] = vhcl;
      vehiclemap[vhcl->id] = vhcl;
   }
}
void addbuildingtype ( pbuildingtype bld )
{
   if ( bld ) {
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




typedef map<ASCString,TextPropertyList> TextFileRepository;
TextFileRepository textFileRepository;

void  loadalltextfiles ( )
{
      tfindfile ff ( "*.asctxt" );
      ASCString c = ff.getnextname();

      while( !c.empty() ) {
         if ( actprogressbar )
            actprogressbar->point();

         tnfilestream s ( c, tnstream::reading );

         displayLogMessage ( 5, "loadalltextfiles :: loading " + c + ", " );

         TextFormatParser tfp ( &s );

         displayLogMessage ( 5, "TFP running... " );

         TextPropertyGroup* tpg = tfp.run();

         textFileRepository[tpg->typeName].push_back ( tpg );

         displayLogMessage ( 5, "done\n" );

         c = ff.getnextname();
      }
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

         PropertyReadingContainer pc ( "vehicletype", *i );

         Vehicletype* vt = new Vehicletype;
         vt->runTextIO ( pc );
         pc.run();

         vt->filename = (*i)->fileName;
         vt->location = (*i)->location;
         addvehicletype ( vt );
      }
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

         PropertyReadingContainer pc ( "objecttype", *i );

         ObjectType* ot = new ObjectType;
         ot->runTextIO ( pc );
         pc.run();

         ot->fileName = (*i)->fileName;
         ot->location = (*i)->location;
         addobjecttype ( ot );
      }
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
         ptechnology tech = gettechnology_forpos ( i, 0 );
         int j = tech->requiretechnologyid[l]; 
         if ( j > 0 ) 
            tech->requiretechnology[l] = gettechnology_forid ( j ); 
      } 

   for (i = 0; i < technologynum; i++)
      gettechnology_forpos ( i, 0 ) -> getlvl();

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

         PropertyReadingContainer pc ( "terraintype", *i );

         TerrainType* tt = new TerrainType;
         tt->runTextIO ( pc );
         pc.run();

         tt->fileName = (*i)->fileName;
         tt->location = (*i)->location;
         addterraintype ( tt );
      }


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
} 


