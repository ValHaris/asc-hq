/***************************************************************************
                          buildingtype.cpp  -  description
                             -------------------
    begin                : Fri Sep 29 2000
    copyright            : (C) 1994-2010 by Martin Bickel
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
#include "stringtokenizer.h"
#include "textfile_evaluation.h"
#include "graphics/surface.h"

#include "errors.h"
#include "sgstream.h"
#include "graphics/blitter.h"
#include "graphics/ColorTransform_PlayerColor.h"


/*
const char*  cbuildingfunctions[cbuildingfunctionnum+1]  =
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
                "view satellites",
                NULL };
*/

BuildingType :: BuildingType ( void )
{
   for ( int x = 0; x < 4; x++ )
      for ( int y = 0; y < 6; y++ ) {
         field_Exists[x][y] = false;
         for ( int w = 0; w < cwettertypennum; ++w )
            for ( int c = 0; c < maxbuildingpicnum; ++c )
                bi_picture[w][c][x][y] = -1;
      }

   buildingNotRemovable = false;
   
   static const float matrix[] = { 1, 0, 0,
                                   0, 1, 0,
                                   0, 0, 1 };
   
   productionEfficiency = ResourceMatrix( matrix );
   
}


int BuildingType :: getMemoryFootprint() const
{
   int size = sizeof(*this);
   
   for ( int i = 0; i < cwettertypennum; ++i )
      for ( int j = 0; j < maxbuildingpicnum; ++j )
         for ( int k = 0; k < 4; ++k )
            for ( int l = 0; l < 6; ++l )
               size += w_picture[i][j][k][l].getMemoryFootprint() - sizeof( Surface ); 
   return size;
}


int   BuildingType :: getBIPicture( const LocalCoordinate& localCoordinate, int weather , int constructionStep ) const
{
   return bi_picture [weather][constructionStep][localCoordinate.x][localCoordinate.y];
}
        

const Surface&   BuildingType :: getPicture ( const LocalCoordinate& localCoordinate, int weather, int constructionStep ) const
{
   if( constructionStep >= construction_steps )
     constructionStep = construction_steps-1;
     
   if ( !weatherBits.test(weather))
      weather = 0;
     
   if ( bi_picture [weather][constructionStep][localCoordinate.x][localCoordinate.y] <= 0 ) 
      return w_picture[weather][constructionStep][localCoordinate.x][localCoordinate.y];
   else 
      return GraphicSetManager::Instance().getPic(bi_picture [weather][constructionStep][localCoordinate.x][localCoordinate.y]);
   
}

void  BuildingType::paint ( Surface& s, SPoint pos, const PlayerColor& player, int weather, int constructionStep ) const
{
   for ( int x = 0; x < 4; x++ )
      for ( int y = 0; y < 6; y++ ) 
         if ( fieldExists(LocalCoordinate(x,y) ))
            paintSingleField(s,pos,LocalCoordinate(x,y),player,weather,constructionStep);
     
}

void  BuildingType::paint ( Surface& s, SPoint pos  ) const
{
   for ( int x = 0; x < 4; x++ )
      for ( int y = 0; y < 6; y++ ) 
         if ( fieldExists(LocalCoordinate(x,y) ))
            paintSingleField(s,pos,LocalCoordinate(x,y));
     
}


void  BuildingType:: paintSingleField ( Surface& s, SPoint pos, const LocalCoordinate& localCoordinate, const PlayerColor& player, int weather, int constructionStep ) const
{
   megaBlitter<ColorTransform_PlayerCol,
               ColorMerger_AlphaOverwrite,
               SourcePixelSelector_Plain,
               TargetPixelSelector_All>
            ( getPicture(localCoordinate,weather,constructionStep), 
               s, 
               SPoint( pos.x + localCoordinate.x * fielddistx + ( localCoordinate.y & 1 ) * fielddisthalfx, pos.y + localCoordinate.y * fielddisty), 
               player, 
               nullParam, nullParam, nullParam );
   
   // s.Blit( getPicture(localCoordinate,weather,constructionStep),SPoint( pos.x + localCoordinate.x * fielddistx + ( localCoordinate.y & 1 ) * fielddisthalfx, pos.y + localCoordinate.y * fielddisty));
}

void  BuildingType:: paintSingleField ( Surface& s, SPoint pos, const LocalCoordinate& localCoordinate, int weather, int constructionStep ) const
{
   megaBlitter<ColorTransform_None,
               ColorMerger_AlphaOverwrite,
               SourcePixelSelector_Plain,
               TargetPixelSelector_All>
            ( getPicture(localCoordinate,weather,constructionStep), 
               s, 
               SPoint( pos.x + localCoordinate.x * fielddistx + ( localCoordinate.y & 1 ) * fielddisthalfx, pos.y + localCoordinate.y * fielddisty), 
               nullParam, nullParam, nullParam, nullParam );
   
   // s.Blit( getPicture(localCoordinate,weather,constructionStep),SPoint( pos.x + localCoordinate.x * fielddistx + ( localCoordinate.y & 1 ) * fielddisthalfx, pos.y + localCoordinate.y * fielddisty));
}



MapCoordinate  BuildingType :: getFieldCoordinate ( const MapCoordinate& entryOnMap, const LocalCoordinate& localCoordinates ) const
{
   int orgx = entryOnMap.x - entry.x - (entry.y & ~entryOnMap.y & 1 );
   int orgy = entryOnMap.y - entry.y;

   int dx = orgy & 1;

   int yy = orgy + localCoordinates.y;
   int xx = orgx + localCoordinates.x + (dx & ~yy);
   MapCoordinate mc ( xx, yy );
   return mc;
}

BuildingType::LocalCoordinate BuildingType::getLocalCoordinate( const MapCoordinate& entryOnMap, const MapCoordinate& field ) const
{
   int homex = entryOnMap.x - entry.x;
   int homey = entryOnMap.y - entry.y;

   if ( (entry.y & 1) && !(entryOnMap.y & 1))
      homex -= 1;
      

   int ly = field.y - homey;
   int lx = field.x - homex;
   if ( (ly & 1) && (homey & 1 ))
      lx -= 1;
   
   if ( lx >= 0 && lx < 4 && ly >= 0 && ly < 6 && fieldExists(LocalCoordinate(lx,ly)))
      return LocalCoordinate(lx,ly);
   else
      return LocalCoordinate();  
}



const int building_version = 15;
const int guardInt = 0xA5C0A5C0;

void BuildingType :: read ( tnstream& stream )
{
   int version = stream.readInt();
   if ( version <= building_version && version >= 1) {

      bool picsAvail[ cwettertypennum ][ maxbuildingpicnum ][4][6];
   
      for ( int v = 0; v < cwettertypennum; v++ )
         for ( int w = 0; w < maxbuildingpicnum; w++ )
            for ( int x = 0; x < 4; x++ )
               for ( int y = 0; y < 6 ; y++ ) {
                   picsAvail[v][w][x][y] = stream.readInt( );
                   if ( picsAvail[v][w][x][y] ) {
                      field_Exists[x][y] = true;
                      weatherBits.set(v);
                   }   
               }    
                   

      for ( int v = 0; v < cwettertypennum; v++ )
         for ( int w = 0; w < maxbuildingpicnum; w++ )
            for ( int x = 0; x < 4; x++ )
               for ( int y = 0; y < 6 ; y++ ) {
                   int i = stream.readInt( );
                   bi_picture[v][w][x][y] = i;
                   if ( i > 0 ) {
                      field_Exists[x][y] = true;
                      weatherBits.set(v);
                   }   
               }  

                   
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
      stream.readUint8( ); // was: loadcapability =
      stream.readUint8( ); // was: unitheightreq =
      productionCost.material = stream.readInt( );
      productionCost.fuel = stream.readInt( );
      if ( version <= 9 ) {
         int special = stream.readInt( );
         convertOldFunctions( special, stream.getLocation() );
      }

      technologylevel = stream.readUint8( );
      stream.readUint8( ); // was: researchid

      terrainaccess.read ( stream );

      construction_steps = stream.readInt( );
      maxresearchpoints = stream.readInt( );
      asc_mode_tank.energy = stream.readInt( );
      asc_mode_tank.material = stream.readInt( );
      asc_mode_tank.fuel = stream.readInt( );
      maxplus.energy = stream.readInt( );
      maxplus.material = stream.readInt( );
      maxplus.fuel = stream.readInt( );
      efficiencyfuel = stream.readInt( );
      efficiencymaterial = stream.readInt( );
      stream.readInt( ); // guibuildicon
      stream.readInt( ); // terrain_access = (TerrainAccess*)
 
      bi_mode_tank.energy = stream.readInt( );
      bi_mode_tank.material = stream.readInt( );
      bi_mode_tank.fuel = stream.readInt( );

      if ( version >= 5 ) {
         defaultProduction.energy = stream.readInt( );
         defaultProduction.material = stream.readInt( );
         defaultProduction.fuel = stream.readInt( );
      }


      height = 1 << getFirstBit ( stream.readInt() );
      stream.readInt( ); // was: unitheight_forbidden =
      externalloadheight = stream.readInt( );

      if ( version >= 15 )
          if ( stream.readInt() != guardInt )
              throw tinvalidversion  ( stream.getLocation(), 1000, 0 );

      if ( version >= 3)
         stream.readInt(); // was: vehicleCategoriesLoadable =


      if ( version >= 2 ) {
         if ( version <= 11 ) {
            for ( int x = 0; x < 4; x++ )
               for ( int y = 0; y < 6; y++ ) {
                  int id = stream.readInt();
                  if ( id > 0 )
                     destructionObjects.insert( make_pair(LocalCoordinate(x,y), id));
               }
         }
      } else {
         for ( int w = 0; w < 9; w++ )
             stream.readInt( ); 
         destructionObjects.clear();
      }

      if ( __loadName )
         name = stream.readString();

      for ( int k = 0; k < maxbuildingpicnum ; k++)
         for ( int j = 0; j <= 5; j++)
            for ( int i = 0; i <= 3; i++)
               for ( int w = 0; w < cwettertypennum; w++ )
                 if ( picsAvail[w][k][i][j] ) 
                    if ( bi_picture[w][k][i][j] == -1 ) 
                       w_picture[w][k][i][j].read(stream );
                 
      if ( version >= 15 )
          if ( stream.readInt() != guardInt+1 )
              throw tinvalidversion  ( stream.getLocation(), 1001, 0 );


      if ( version >= 4 )
         ContainerBaseType::read ( stream );

      if ( version >= 6 )
         nominalresearchpoints = stream.readInt();

      if ( version >= 15 )
          if ( stream.readInt() != guardInt+2 )
              throw tinvalidversion  ( stream.getLocation(), 1002, 0 );

      if ( version >= 7 ) {
         techDependency.read( stream );
         defaultMaxResearchpoints = stream.readInt();
      }

      if ( version >= 8 )
         infotext = stream.readString(true);

      if ( version >= 11 )
         buildingNotRemovable = stream.readInt();
      
      if ( version >= 15 )
          if ( stream.readInt() != guardInt+3 )
              throw tinvalidversion  ( stream.getLocation(), 1003, 0 );

      if ( version >= 12 ) {
         int num = stream.readInt();
         for ( int i = 0; i < num; ++i ) {
            int x = stream.readInt();
            int y = stream.readInt();
            int id = stream.readInt();
            destructionObjects.insert( make_pair( LocalCoordinate(x,y),id));
         }
      }
      
      if ( version >= 13 )
         for ( int w = 0; w < cwettertypennum; ++w )
            originalImageFilename[w] = stream.readString();
      
      if ( version >= 14 )
         description = stream.readString();

      if ( version >= 15 )
          if ( stream.readInt() != guardInt+4 )
              throw tinvalidversion  ( stream.getLocation(), 1004, 0 );

   } else
      throw tinvalidversion  ( stream.getLocation(), building_version, version );
}

void BuildingType :: write ( tnstream& stream ) const
{
   stream.writeInt ( building_version );

   for ( int v = 0; v < cwettertypennum; v++ )
      for ( int w = 0; w < maxbuildingpicnum; w++ )
         for ( int x = 0; x < 4; x++ )
            for ( int y = 0; y < 6 ; y++ ) {
                stream.writeInt ( w_picture[v][w][x][y].valid() );
            }

   for ( int v = 0; v < cwettertypennum; v++ )
      for ( int w = 0; w < maxbuildingpicnum; w++ )
         for ( int x = 0; x < 4; x++ )
            for ( int y = 0; y < 6 ; y++ )
                stream.writeInt ( bi_picture[v][w][x][y] );

           /*     
   for ( int x = 0; x < 4; x++ )
      for ( int y = 0; y < 6 ; y++ )
         stream.writeInt( field_Exists[x][y] );
         */
                
                
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
   stream.writeUint8 ( 0);
   stream.writeUint8 ( 0 );
   stream.writeInt ( productionCost.material );
   stream.writeInt ( productionCost.fuel );
   stream.writeUint8 ( technologylevel );
   stream.writeUint8 ( 0 );

   terrainaccess.write ( stream );

   stream.writeInt ( construction_steps );
   stream.writeInt ( maxresearchpoints );
   stream.writeInt ( asc_mode_tank.energy );
   stream.writeInt ( asc_mode_tank.material );
   stream.writeInt ( asc_mode_tank.fuel );
   
   stream.writeInt ( maxplus.energy );
   stream.writeInt ( maxplus.material );
   stream.writeInt ( maxplus.fuel );
   stream.writeInt ( efficiencyfuel );
   stream.writeInt ( efficiencymaterial );
   stream.writeInt ( 1 ); // guibuildicon
   stream.writeInt ( 1 );

   stream.writeInt ( bi_mode_tank.energy );
   stream.writeInt ( bi_mode_tank.material );
   stream.writeInt ( bi_mode_tank.fuel );

   stream.writeInt ( defaultProduction.energy );
   stream.writeInt ( defaultProduction.material );
   stream.writeInt ( defaultProduction.fuel );

   stream.writeInt ( height );
   stream.writeInt ( 0 );
   stream.writeInt ( externalloadheight );

   stream.writeInt( guardInt );

   stream.writeInt ( 0 );

   if ( !name.empty() )
      stream.writeString ( name );

    for (int k = 0; k < maxbuildingpicnum; k++)
       for (int j = 0; j <= 5; j++)
          for (int i = 0; i <= 3; i++)
             for ( int w = 0; w < cwettertypennum; w++ )
                if ( w_picture[w][k][i][j].valid() )
                   if ( bi_picture[w][k][i][j] == -1 )
                      w_picture[w][k][i][j].write(stream);

    stream.writeInt( guardInt +1 );
    ContainerBaseType::write ( stream );

    stream.writeInt( nominalresearchpoints );

    stream.writeInt( guardInt +2 );

    techDependency.write ( stream );
    stream.writeInt( defaultMaxResearchpoints );

    stream.writeString ( infotext );
    stream.writeInt( buildingNotRemovable );
    
    stream.writeInt( guardInt +3 );

    stream.writeInt( destructionObjects.size());
    for ( DestructionObjects::const_iterator i = destructionObjects.begin(); i != destructionObjects.end(); ++i) {
       stream.writeInt( i->first.x);
       stream.writeInt( i->first.y);
       stream.writeInt( i->second);
    }
    
    for ( int w = 0; w < cwettertypennum; ++w )
       stream.writeString( originalImageFilename[w] );
    
    stream.writeString( description );
    stream.writeInt( guardInt +4 );
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

      pc.addBreakpoint();
   
      ContainerBaseType::runTextIO ( pc );
   
      pc.addInteger ( "ConstructionStages", construction_steps );

      for ( int i = 0; i < cwettertypennum; i++ )
         for ( int x = 0; x < 4; x++ )
            for ( int y = 0; y < 6; y++ )
               if ( w_picture[i][0][x][y].valid() )
                  weatherBits.set(i);

      pc.addTagArray( "Weather", weatherBits, cwettertypennum, weatherTags );


      ASCString fieldNames;
      for ( int a = 0; a < 4; a++ )
         for ( int b = 0; b < 6; b++ )
            if ( w_picture[0][0][a][b].valid() || bi_picture[0][0][a][b] > 0 ) {
               fieldNames += LocalCoordinate( a, b).toString();
               fieldNames += " ";
            }

      pc.addString( "Fields", fieldNames );

      typedef vector<LocalCoordinate> Fields;
      Fields fields;
      StringTokenizer st ( fieldNames );
      ASCString t = st.getNextToken();
      while ( !t.empty() ) {
         LocalCoordinate lc ( t );
         fields.push_back ( lc );
         t = st.getNextToken();
         field_Exists[lc.x][lc.y] = true;
      }



            
      bool bi3pics = false;

      for ( int i = 0; i < 4; i++ )
         for ( int j = 0; j < 6; j++ )
            if ( bi_picture[0][0][i][j] >= 0 )
               bi3pics = true;

      pc.addBool  ( "UseGFXpics", bi3pics );

      if ( bi3pics ) {
         pc.openBracket ( "GFXpictures");
         for ( int w = 0; w < cwettertypennum; w++ )
            if ( weatherBits.test(w) ) {
               pc.openBracket (weatherTags[w] );

               for ( int c = 0; c < construction_steps; c++ ) {
                  pc.openBracket ( ASCString("Stage")+strrr(c+1) );

                  for ( Fields::iterator i = fields.begin(); i != fields.end(); i++ ) 
                     pc.addInteger ( i->toString(), bi_picture[w][c][i->x][i->y] );
                  

                  pc.closeBracket();
               }
               pc.closeBracket();
               
            }
         pc.closeBracket();
      } else {
         pc.openBracket ( "Pictures");
         if ( !pc.isReading() ) {
            for ( int w = 0; w < cwettertypennum; w++ )
               if ( weatherBits.test(w) ) {
                  Surface s = Surface::createSurface( construction_steps*500, 250, 32, 0 );
                  for ( int c = 0; c < construction_steps; c++ )
                     for ( int x = 0; x < 4; x++ )
                        for ( int y = 0; y < 6; y++ )
                           if ( w_picture[w][c][x][y].valid() )
                              megaBlitter<ColorTransform_None,
                                          ColorMerger_AlphaOverwrite,
                                          SourcePixelSelector_Plain,
                                          TargetPixelSelector_All>
                                       ( w_picture[w][c][x][y], 
                                          s,
                                          SPoint( 500*c + x * fielddistx + (y&1)*fielddisthalfx, y * fielddisty), 
                                          nullParam, nullParam, nullParam, nullParam );
                  ASCString file = extractFileName_withoutSuffix ( filename )+weatherAbbrev[w]+".png";
                  pc.addImage ( weatherTags[w], s, file, false  );
                  pc.addString( ASCString(weatherTags[w]) + "_OriginalImageFilename", originalImageFilename[w] );
               }
         } else {
            for ( int w = 0; w < cwettertypennum; w++ )
               if ( weatherBits.test(w) ) {
                  ASCString fileName = extractFileName_withoutSuffix ( filename )+weatherAbbrev[w]+".png";
                  Surface s;
                  pc.addImage ( weatherTags[w], s, fileName, false );
                  originalImageFilename[w] = fileName;

//                  if ( s.GetPixelFormat().BitsPerPixel() != 8 )
                     //fatalError("Building image " + filename + " does not have 8 Bit color depth!");
            
                  int depth = s.GetPixelFormat().BitsPerPixel();
                  for ( int c = 0; c < construction_steps; c++ )
                     for ( Fields::iterator i = fields.begin(); i != fields.end(); i++ ) {
                        Surface& img = w_picture[w][c][i->x][i->y];
                        img = Surface::createSurface(fieldsizex,fieldsizey,depth);
                        int xx = 500*c + i->x * fielddistx + (i->y&1)*fielddisthalfx;
                        int yy = i->y * fielddisty;
                        if ( depth == 8 ) {
                           img.Blit( s, SDLmm::SRect(SPoint(xx,yy),fieldsizex,fieldsizey), SPoint(0,0));
                           applyFieldMask(img);
                        } else {
                           MegaBlitter<4,4,ColorTransform_None,ColorMerger_PlainOverwrite,SourcePixelSelector_Rectangle> blitter;
                           blitter.setSrcRectangle( SDLmm::SRect(SPoint(xx,yy),fieldsizex,fieldsizey) );
                           blitter.blit( s, img, SPoint(0,0) );
                           applyFieldMask(img,0,0,false);
                        }
                     }

               }

         }
         pc.closeBracket();
      }

      bool rubble = destructionObjects.size() > 0;
      pc.addBool ( "RubbleObjects", rubble );
      if ( rubble ) {
         if ( pc.isReading() ) 
            destructionObjects.clear();
         
         pc.openBracket ( "Rubble");
         for ( Fields::iterator i = fields.begin(); i != fields.end(); i++ ) {
            vector<int> ids;
            typedef DestructionObjects::const_iterator J;
            pair<J,J> b = destructionObjects.equal_range(*i);
            for ( J j = b.first; j != b.second; ++j)
               ids.push_back(j->second);
            pc.addIntegerArray ( i->toString(), ids );
            
            if ( pc.isReading() ) {
               for ( vector<int>::iterator j = ids.begin(); j != ids.end(); ++j )
                  destructionObjects.insert(make_pair(*i, *j));
            }
         }
         pc.closeBracket();
      }


      ASCString entryString = entry.toString();
      pc.addString ( "Entry", entryString );
      if ( pc.isReading() ) {
         StringTokenizer st ( entryString );
         entry = LocalCoordinate ( st.getNextToken() );
      }
      if ( !fieldExists( entry ))
         pc.error( "Building " + name + " has invalid entrance position!" );

      pc.addInteger( "Armor", _armor );

      if ( pc.find( "Features" ) || !pc.isReading())
         pc.addTagArray ( "Features", features, functionNum, containerFunctionTags );
      else {
         int special = 0;
         pc.addTagInteger ( "Functions", special, cbuildingfunctionnum, buildingFunctionTags );
         convertOldFunctions( special, pc.getFileName() );
      } 
      pc.addInteger ( "Techlevel", technologylevel );

      pc.openBracket("TerrainAccess" );
       terrainaccess.runTextIO ( pc );
      pc.closeBracket();


      pc.openBracket ( "ConstructionCost" );
       productionCost.runTextIO ( pc );
      pc.closeBracket ();

      pc.addTagInteger( "Height", height, choehenstufennum, heightTags );

      pc.addTagInteger( "ExternalLoading", externalloadheight, choehenstufennum, heightTags );

      pc.addBool ( "NotRemovable", buildingNotRemovable, false );

      techDependency.runTextIO( pc, ASCString("b")+strrr(id) );

   }
   catch ( InvalidString ) {
      pc.error ( "Could not parse building field coordinate");
   }
}

void BuildingType::convertOldFunctions( int abilities, const ASCString& location  )
{
   features.reset();
   if ( abilities & 1 ) warningMessage ( location + ": The HQ function for buildings is not supported any more");
   if ( abilities & 2 ) features.set( TrainingCenter );
   if ( abilities & (1 << 3) ) features.set( InternalVehicleProduction );
   if ( abilities & (1 << 4) ) features.set( AmmoProduction );
   if ( abilities & (1 << 8) ) features.set( InternalUnitRepair );
   if ( abilities & (1 << 9) ) features.set( RecycleUnits );
   if ( abilities & (1 << 10) ) features.set( Research );
   if ( abilities & (1 << 11) ) features.set( Sonar );
   if ( abilities & (1 << 12) ) features.set( WindPowerPlant );
   if ( abilities & (1 << 13) ) features.set( SolarPowerPlant );
   if ( abilities & (1 << 14) ) features.set( MatterConverter );
   if ( abilities & (1 << 15) ) features.set( MiningStation );
   if ( abilities & (1 << 16) ) {
      features.set( ExternalMaterialTransfer );
      features.set( ExternalFuelTransfer );
      features.set( ExternalAmmoTransfer );
   }
   if ( abilities & (1 << 17) ) features.set( ProduceNonLeavableUnits );
   if ( abilities & (1 << 18) ) features.set( ResourceSink );
   if ( abilities & (1 << 19) ) {
      features.set( ExternalMaterialTransfer );
      features.set( ExternalFuelTransfer );
   }
   if ( abilities & (1 << 20) ) features.set( ExternalAmmoTransfer );
   if ( abilities & (1 << 21) ) features.set( NoObjectChaining );
   if ( abilities & (1 << 22) ) features.set( SelfDestructOnConquer );
   if ( abilities & (1 << 23) ) features.set( SatelliteView );
}

