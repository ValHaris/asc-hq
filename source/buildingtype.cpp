/***************************************************************************
                          buildingtype.cpp  -  description
                             -------------------
    begin                : Fri Sep 29 2000
    copyright            : (C) 1994-2003 by Martin Bickel
    email                : bickel@asc-hq.org
 ***************************************************************************/

/*! \file buildingtype.cpp
    \brief The implementation of the buildingtype class
*/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <algorithm>

#include "vehicletype.h"
#include "buildingtype.h"
#include "graphicset.h"
#include "gameoptions.h"
#include "textfiletags.h"
#include "basegfx.h"
#include "stringtokenizer.h"
#include "textfile_evaluation.h"

#include "errors.h"

#ifdef sgmain
 #include "spfst.h"
#endif

 #include "sgstream.h"



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
                "external resource transfer",
                "construct units that cannot move out",
                "resource sink",
                "external resource transfer",
                "external ammo transfer",
                "no object chaining",
                "self destruct on conquer",
                "view satellites" };


BuildingType :: BuildingType ( void )
{
   for ( int x = 0; x < 4; x++ )
      for ( int y = 0; y < 6; y++ ) {
         for ( int w = 0; w < cwettertypennum; w++ )
            for ( int i = 0; i < maxbuildingpicnum; i++ ) {
               w_picture [w][i][x][y] = NULL;
               bi_picture [w][i][x][y] = -1;
            }
         destruction_objects [x][y] = 0;
      }
   maxresearchpoints = 0;
   defaultMaxResearchpoints = 0;
   nominalresearchpoints = 0;
}



void*   BuildingType :: getpicture ( const LocalCoordinate& localCoordinate ) const
{
   return w_picture[0][0][localCoordinate.x][localCoordinate.y];
}


#define compensatebuildingcoordinateorgx (a) (dx & (~a))
#define compensatebuildingcoordinatex ( + (dx & ~b) )



MapCoordinate  BuildingType :: getFieldCoordinate ( const MapCoordinate& entryPosition, const LocalCoordinate& localCoordinates )
{
   int orgx = entryPosition.x - entry.x - (entry.y & ~entryPosition.y & 1 );
   int orgy = entryPosition.y - entry.y;

   int dx = orgy & 1;

   int yy = orgy + localCoordinates.y;
   int xx = orgx + localCoordinates.x + (dx & ~yy);
   MapCoordinate mc ( xx, yy );
   return mc;
}


const int building_version = 8;

#ifndef converter
extern void* generate_building_gui_build_icon ( pbuildingtype bld );
#endif

void BuildingType :: read ( tnstream& stream )
{
   int version = stream.readInt();
   if ( version <= building_version && version >= 1) {

      for ( int v = 0; v < cwettertypennum; v++ )
         for ( int w = 0; w < maxbuildingpicnum; w++ )
            for ( int x = 0; x < 4; x++ )
               for ( int y = 0; y < 6 ; y++ )
                   w_picture[v][w][x][y] = (void*)stream.readInt( );

      for ( int v = 0; v < cwettertypennum; v++ )
         for ( int w = 0; w < maxbuildingpicnum; w++ )
            for ( int x = 0; x < 4; x++ )
               for ( int y = 0; y < 6 ; y++ )
                   bi_picture[v][w][x][y] = stream.readInt( );

      entry.x = stream.readInt( );
      entry.y = stream.readInt( );

      stream.readInt( ); // was: powerlineconnect.x
      stream.readInt( ); // was: powerlineconnect.y
      stream.readInt( ); // was: pipelineconnect.x
      stream.readInt( ); // was: pipelineconnect.y

      id = stream.readInt( );
      bool __loadName = stream.readInt( );
      _armor = stream.readInt( );
      jamming = stream.readInt( );
      view = stream.readInt( );
      stream.readInt( ); // was: loadcapacity
      stream.readChar( ); // was: loadcapability =
      stream.readChar( ); // was: unitheightreq =
      productionCost.material = stream.readInt( );
      productionCost.fuel = stream.readInt( );
      special = stream.readInt( );
      technologylevel = stream.readChar( );
      researchid = stream.readChar( );

      terrainaccess.read ( stream );

      construction_steps = stream.readInt( );
      maxresearchpoints = stream.readInt( );
      _tank.energy = stream.readInt( );
      _tank.material = stream.readInt( );
      _tank.fuel = stream.readInt( );
      maxplus.energy = stream.readInt( );
      maxplus.material = stream.readInt( );
      maxplus.fuel = stream.readInt( );
      efficiencyfuel = stream.readInt( );
      efficiencymaterial = stream.readInt( );
      guibuildicon = (char*) stream.readInt( );
      stream.readInt( ); // terrain_access = (pterrainaccess)

      _bi_maxstorage.energy = stream.readInt( );
      _bi_maxstorage.material = stream.readInt( );
      _bi_maxstorage.fuel = stream.readInt( );

      if ( version >= 5 ) {
         defaultProduction.energy = stream.readInt( );
         defaultProduction.material = stream.readInt( );
         defaultProduction.fuel = stream.readInt( );
      }


      buildingheight = 1 << log2 ( stream.readInt() );
      stream.readInt( ); // was: unitheight_forbidden =
      externalloadheight = stream.readInt( );

      if ( version >= 3)
         stream.readInt(); // was: vehicleCategoriesLoadable =


      if ( version >= 2 ) {
         for ( int x = 0; x < 4; x++ )
            for ( int y = 0; y < 6; y++ )
                destruction_objects[x][y] = stream.readInt( );
      } else {
         for ( int w = 0; w < 9; w++ )
             stream.readInt( );     // dummy

         for ( int x = 0; x < 4; x++ )
            for ( int y = 0; y < 6; y++ )
                destruction_objects[x][y] = 0;
      }

      if ( __loadName )
         name = stream.readString();

      for ( int k = 0; k < maxbuildingpicnum ; k++)
         for ( int j = 0; j <= 5; j++)
            for ( int i = 0; i <= 3; i++)
               for ( int w = 0; w < cwettertypennum; w++ )
                 if ( w_picture[w][k][i][j] )
                    if ( bi_picture[w][k][i][j] == -1 ) {
                       int sz;
                       stream.readrlepict ( &w_picture[w][k][i][j], false, &sz );
                     } else
                        loadbi3pict_double ( bi_picture[w][k][i][j],
                                             &w_picture[w][k][i][j],
                                             CGameOptions::Instance()->bi3.interpolate.buildings );


      if ( version >= 4 )
         ContainerBaseType::read ( stream );

      if ( version >= 6 )
         nominalresearchpoints = stream.readInt();

      if ( version >= 7 ) {
         techDependency.read( stream );
         defaultMaxResearchpoints = stream.readInt();
      }

      if ( version >= 8 )
         infotext = stream.readString();



     #ifdef converter
      guibuildicon = NULL;
     #else
      guibuildicon = generate_building_gui_build_icon ( this );
     #endif

   } else
      throw tinvalidversion  ( stream.getLocation(), building_version, version );
}

void BuildingType :: write ( tnstream& stream ) const
{
   stream.writeInt ( building_version );

   for ( int v = 0; v < cwettertypennum; v++ )
      for ( int w = 0; w < maxbuildingpicnum; w++ )
         for ( int x = 0; x < 4; x++ )
            for ( int y = 0; y < 6 ; y++ )
                stream.writeInt ( w_picture[v][w][x][y] != NULL );

   for ( int v = 0; v < cwettertypennum; v++ )
      for ( int w = 0; w < maxbuildingpicnum; w++ )
         for ( int x = 0; x < 4; x++ )
            for ( int y = 0; y < 6 ; y++ )
                stream.writeInt ( bi_picture[v][w][x][y] );

   stream.writeInt ( entry.x );
   stream.writeInt ( entry.y );
   stream.writeInt ( -1 ); // was powerlineconnect.x
   stream.writeInt ( -1 ); // was powerlineconnect.y
   stream.writeInt ( -1 ); // was pipelineconnect.x
   stream.writeInt ( -1 ); // was pipelineconnect.y

   stream.writeInt ( id );
   stream.writeInt ( !name.empty() );
   stream.writeInt ( _armor );
   stream.writeInt ( jamming );
   stream.writeInt ( view );
   stream.writeInt ( 0 );
   stream.writeChar ( 0);
   stream.writeChar ( 0 );
   stream.writeInt ( productionCost.material );
   stream.writeInt ( productionCost.fuel );
   stream.writeInt ( special );
   stream.writeChar ( technologylevel );
   stream.writeChar ( researchid );

   terrainaccess.write ( stream );

   stream.writeInt ( construction_steps );
   stream.writeInt ( maxresearchpoints );
   stream.writeInt ( _tank.energy );
   stream.writeInt ( _tank.material );
   stream.writeInt ( _tank.fuel );
   stream.writeInt ( maxplus.energy );
   stream.writeInt ( maxplus.material );
   stream.writeInt ( maxplus.fuel );
   stream.writeInt ( efficiencyfuel );
   stream.writeInt ( efficiencymaterial );
   stream.writeInt ( guibuildicon != NULL );
   stream.writeInt ( 1 );

   stream.writeInt ( _bi_maxstorage.energy );
   stream.writeInt ( _bi_maxstorage.material );
   stream.writeInt ( _bi_maxstorage.fuel );

   stream.writeInt ( defaultProduction.energy );
   stream.writeInt ( defaultProduction.material );
   stream.writeInt ( defaultProduction.fuel );

   stream.writeInt ( buildingheight );
   stream.writeInt ( 0 );
   stream.writeInt ( externalloadheight );

   stream.writeInt ( 0 );

   for ( int x = 0; x < 4; x++ )
      for ( int y = 0; y < 6; y++ )
          stream.writeInt ( destruction_objects[x][y] );

   if ( !name.empty() )
      stream.writeString ( name );

    for (int k = 0; k < maxbuildingpicnum; k++)
       for (int j = 0; j <= 5; j++)
          for (int i = 0; i <= 3; i++)
             for ( int w = 0; w < cwettertypennum; w++ )
                if ( w_picture[w][k][i][j] )
                   if ( bi_picture[w][k][i][j] == -1 )
                       stream.writeImage( w_picture[w][k][i][j], false);

    ContainerBaseType::write ( stream );

    stream.writeInt( nominalresearchpoints );

    techDependency.write ( stream );
    stream.writeInt( defaultMaxResearchpoints );

    stream.writeString ( infotext );
}


ASCString BuildingType :: LocalCoordinate :: toString ( ) const
{
  ASCString s;
  s += 'A'+x;
  s += '1'+y;
  return s;
}

class InvalidString : public ASCexception {};

BuildingType :: LocalCoordinate :: LocalCoordinate ( const ASCString& s )
{
  ASCString s2 = s;
  s2.toUpper();
  if ( s2.length() < 2 ) {
     x = -1;
     y = -1;
     throw InvalidString();
  } else {
     x = s2[0] - 'A';
     y = s2[1] - '1';
     if ( x < 0 || x > 5 || y < 0 || y > 7 )
        throw InvalidString();
  }
}


void BuildingType :: runTextIO ( PropertyContainer& pc )
{
   try {
      pc.addInteger ( "ConstructionStages", construction_steps );

      BitSet weatherBits;

      for ( int i = 0; i < cwettertypennum; i++ )
         for ( int x = 0; x < 4; x++ )
            for ( int y = 0; y < 6; y++ )
               if ( w_picture[i][0][x][y] )
                  weatherBits.set(i);

      pc.addTagArray( "Weather", weatherBits, cwettertypennum, weatherTags );


      ASCString fieldNames;
      for ( int a = 0; a < 4; a++ )
         for ( int b = 0; b < 6; b++ )
            if ( w_picture[0][0][a][b] ) {
               fieldNames += LocalCoordinate( a, b).toString();
               fieldNames += " ";
            }

      pc.addString( "Fields", fieldNames );

      typedef vector<LocalCoordinate> Fields;
      Fields fields;
      StringTokenizer st ( fieldNames );
      ASCString t = st.getNextToken();
      while ( !t.empty() ) {
         fields.push_back ( LocalCoordinate( t ));
         t = st.getNextToken();
      }


      bool bi3pics = false;

      for ( int i = 0; i < 4; i++ )
         for ( int j = 0; j < 6; j++ )
            if ( w_picture[0][0][i][j] && bi_picture[0][0][i][j] >= 0 )
               bi3pics = true;

      pc.addBool  ( "UseGFXpics", bi3pics );

      if ( bi3pics ) {
         pc.openBracket ( "GFXpictures");
         for ( int w = 0; w < cwettertypennum; w++ )
            if ( weatherBits.test(w) ) {
               pc.openBracket (weatherTags[w] );

               for ( int c = 0; c < construction_steps; c++ ) {
                  pc.openBracket ( ASCString("Stage")+strrr(c+1) );

                  for ( Fields::iterator i = fields.begin(); i != fields.end(); i++ ) {
                     pc.addInteger ( i->toString(), bi_picture[w][c][i->x][i->y] );
                     if ( pc.isReading() )
                        loadbi3pict_double ( bi_picture[w][c][i->x][i->y],
                                             &w_picture[w][c][i->x][i->y],
                                             CGameOptions::Instance()->bi3.interpolate.buildings );
                  }

                  pc.closeBracket();
               }
               pc.closeBracket();
            }
         pc.closeBracket();
      } else {
         pc.openBracket ( "Pictures");
         if ( !pc.isReading() ) {
            tvirtualdisplay vdd( construction_steps*500, 250, 255 );
            for ( int w = 0; w < cwettertypennum; w++ )
               if ( weatherBits.test(w) ) {
                  for ( int c = 0; c < construction_steps; c++ )
                     for ( int x = 0; x < 4; x++ )
                        for ( int y = 0; y < 6; y++ )
                           if ( w_picture[w][c][x][y] )
                              putspriteimage ( 500*c + x * fielddistx + (y&1)*fielddisthalfx, y * fielddisty, w_picture[w][c][x][y] );

                  void* img = asc_malloc ( imagesize ( 0, 0, construction_steps*500-1, 250-1 ));
                  getimage ( 0, 0, construction_steps*500-1, 250-1, img );

                  pc.addImage ( weatherTags[w], img, extractFileName_withoutSuffix ( filename )+weatherAbbrev[w]+".pcx" );

                  asc_free ( img );
               }
         } else {
            for ( int w = 0; w < cwettertypennum; w++ )
               if ( weatherBits.test(w) ) {
                  void* img = NULL;
                  pc.addImage ( weatherTags[w], img, extractFileName_withoutSuffix ( filename )+weatherAbbrev[w]+".pcx" );
                  tvirtualdisplay vd ( construction_steps*500, 250 );
                  putimage ( 0, 0, img );
                  asc_free ( img );


                  for ( int c = 0; c < construction_steps; c++ )
                     for ( Fields::iterator i = fields.begin(); i != fields.end(); i++ ) {
                        int x = i->x;
                        int y = i->y;
                        void* img = asc_malloc ( imagesize ( 0, 0, fieldsizex, fieldsizey ));
                        int xx = 500*c + x * fielddistx + (y&1)*fielddisthalfx;
                        int yy = y * fielddisty;
                        getimage ( xx, yy, xx + fieldsizex-1, yy + fieldsizey-1, img );
                        tvirtualdisplay vd ( fieldsizex, fieldsizey );
                        putimage ( 0, 0, img );
                        putmask ( 0, 0, getFieldMask(), 0 );
                        getimage ( 0, 0, fieldsizex-1, fieldsizey-1, img );
                        w_picture[w][c][x][y] = img;
                     }

               }

         }
         pc.closeBracket();
      }

      #ifndef converter
      guibuildicon = generate_building_gui_build_icon ( this );
      #else
      guibuildicon = NULL;
      #endif


      bool rubble = false;
      for ( int i = 0; i < 4; i++ )
         for ( int j = 0; j < 6; j++ )
            if ( destruction_objects[i][j] > 0 )
               rubble = true;

      pc.addBool ( "RubbleObjects", rubble );
      if ( rubble ) {
         pc.openBracket ( "Rubble");
         for ( Fields::iterator i = fields.begin(); i != fields.end(); i++ )
            pc.addInteger ( i->toString(), destruction_objects[i->x][i->y] );
         pc.closeBracket();
      }


      ASCString entryString = entry.toString();
      pc.addString ( "Entry", entryString );
      if ( pc.isReading() ) {
         StringTokenizer st ( entryString );
         entry = LocalCoordinate ( st.getNextToken() );
      }

      pc.addInteger( "Armor", _armor );

      pc.addTagInteger ( "Functions", special, cbuildingfunctionnum, buildingFunctionTags );

      pc.addInteger ( "Techlevel", technologylevel );

      pc.openBracket("TerrainAccess" );
       terrainaccess.runTextIO ( pc );
      pc.closeBracket();


      pc.addInteger ( "MaxResearch", maxresearchpoints, 0 );
      pc.addInteger ( "NominalResearch", nominalresearchpoints, maxresearchpoints/2 );
      pc.addInteger ( "MaxResearchpointsDefault", defaultMaxResearchpoints, maxresearchpoints );

      pc.openBracket ( "ConstructionCost" );
       productionCost.runTextIO ( pc );
      pc.closeBracket ();

      pc.openBracket ( "MaxResourceProduction" );
       maxplus.runTextIO ( pc );
      pc.closeBracket ();

      pc.openBracket ( "ResourceExtractionEfficiency");
       pc.addInteger( "Material", efficiencymaterial, 1024 );
       pc.addInteger( "Fuel", efficiencyfuel, 1024 );
      pc.closeBracket ();

      pc.openBracket ( "StorageCapacity" );
       pc.openBracket( "BImode" );
        _bi_maxstorage.runTextIO ( pc );
       pc.closeBracket();
       pc.openBracket ( "ASCmode" );
        _tank.runTextIO ( pc );
       pc.closeBracket();
      pc.closeBracket ();

      pc.openBracket( "DefaultProduction" );
       defaultProduction.runTextIO ( pc, Resources(0,0,0) );
      pc.closeBracket();


      pc.addTagInteger( "Height", buildingheight, choehenstufennum, heightTags );

      ContainerBaseType::runTextIO ( pc );

      pc.addTagInteger( "ExternalLoading", externalloadheight, choehenstufennum, heightTags );

      techDependency.runTextIO( pc, ASCString("b")+strrr(id) );

   }
   catch ( InvalidString ) {
      pc.error ( "Could not parse building field coordinate");
   }
}

