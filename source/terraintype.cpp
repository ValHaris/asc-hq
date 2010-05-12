/***************************************************************************
                          terraintype.cpp  -  description
                             -------------------
    begin                : Fri Jul 27 2001
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

// #include "basegfx.h"
#include "terraintype.h"
#include "gameoptions.h"
#include "graphicset.h"
#include "sgstream.h"
#include "textfileparser.h"
#include "textfiletags.h"
#include "textfile_evaluation.h"
#include "overviewmapimage.h"
#include "graphics/blitter.h"
#include "fieldimageloader.h"

const char*  terrainProperty[terrainPropertyNum+1]  = {"shallow water"       ,  // 0
                                             "normal lowland",
                                             "swamp",
                                             "forest",
                                             "high mountains",                  // 4
                                             "road",
                                             "railroad",
                                             "entry of building (not to be used for terrain)" ,
                                             "harbour",
                                             "runway"  ,
                                             "pipeline",                        // 10
                                             "buried pipeline",
                                             "water",
                                             "deep water",
                                             "hard sand",
                                             "soft sand",
                                             "track possible",
                                             "small rocks",
                                             "mud",
                                             "snow",
                                             "deep snow",
                                             "mountains",
                                             "very shallow water",
                                             "large rocks",
                                             "lava",
                                             "ditch",
                                             "hillside",
                                             "turret foundation",
                                             "morass",
                                             "Installation",
                                             "pack ice",
                                             "river",
                                             "frozen water",
                                             "bridge",
                                             "lava barrier",
                                             "spaceport",
                                             "beacon",
                                             "fire",
                                             NULL };




void TerrainAccess::read ( tnstream& stream ) {
   terrain.read ( stream );
   terrain.reset( 7 );
   terrainreq.read ( stream );
   terrainnot.read ( stream );
   terrainnot.reset( 7 );
   terrainkill.read ( stream );

   for ( int a = 0; a < 10; a++ )
       stream.readInt( ); //dummy
};

void TerrainAccess::write ( tnstream& stream ) const {
   terrain.write ( stream );
   terrainreq.write ( stream );
   terrainnot.write ( stream );
   terrainkill.write ( stream );

   for ( int a = 0; a < 10; a++ )
       stream.writeInt( 0 ); //dummy
};

void TerrainAccess::runTextIO ( PropertyContainer& pc )
{
   pc.addTagArray ( "terrain_any", terrain, terrainPropertyNum, terrainProperties );
   pc.addTagArray ( "terrain_all", terrainreq, terrainPropertyNum, terrainProperties );
   pc.addTagArray ( "terrain_not", terrainnot, terrainPropertyNum, terrainProperties );
   pc.addTagArray ( "terrain_kill", terrainkill, terrainPropertyNum, terrainProperties );
}

void TerrainBits::appendToString( ASCString& text ) const
{
   for (int i = 0; i < terrainPropertyNum ; i++) {
      TerrainBits bts;
      bts.set ( i );

      if ( (*this & bts).any() ) {
         text += "- " ;
         text += terrainProperty[i];
         text += "\n";
      }
   } /* endfor */
}




TerrainType::MoveMalus::MoveMalus()
{
   resize( cmovemalitypenum);
   for ( int i = 0; i <  cmovemalitypenum; i++ )
      at(i) = 100;
}

TerrainType::TerrainType()
{
   id = -1;
   for ( int i = 0; i < cwettertypennum; i++ )
      weather[i] = NULL;

}


void      TerrainType::Weather::paint ( Surface& s, SPoint pos )
{
   Surface* img;
   if ( bi_pict == -1 )
      img = &image;
   else 
      img = &GraphicSetManager::Instance().getPic( bi_pict);

   megaBlitter<ColorTransform_None, 
               ColorMerger_AlphaOverwrite, 
               SourcePixelSelector_Plain,
               TargetPixelSelector_All>
            (*img,s,pos,nullParam,nullParam,nullParam,nullParam);
}


const OverviewMapImage* TerrainType::Weather::getQuickView()
{
   if  ( bi_pict >= 0 ) {
      return GraphicSetManager::Instance().getQuickView( bi_pict );
   } else {
      if (!quickView ) {
         quickView = new OverviewMapImage( image );
      }
      return quickView;
   }
}

TerrainType::Weather::~Weather()
{ 
   delete quickView; 
}


const int terrain_version = 6;


void TerrainType::MoveMalus::read( tnstream& stream, int defaultValue, int moveMalusCount )
{
   clear();

   int version;
   if ( moveMalusCount <= 0 ) {
      version = stream.readInt();
      moveMalusCount = stream.readInt();
   } else
      version = 0;

   #ifndef converter
    int mmcount = moveMalusCount;
    if (mmcount < moveMalusCount )
       mmcount = moveMalusCount;
   #else
    int mmcount = moveMalusCount ;
   #endif

   for ( int j=0; j< mmcount ; j++ ) {
      if (j < moveMalusCount ) {
         int i;
         if ( version < 2 )
            i = stream.readChar();
         else
            i = stream.readInt();
         push_back ( i );
      } else
         if ( j == 0 )
            push_back ( defaultValue );
         else
            push_back ( at(0) );
   }
}

void TerrainType::MoveMalus::write ( tnstream& stream ) const
{
  stream.writeInt(2);
  stream.writeInt( size() );
  for ( int m = 0; m < size(); m++ )
     stream.writeInt ( at(m) );
}

void TerrainType::Weather::read ( tnstream& stream )
{
   read( stream, terrain_version );
}


void TerrainType::Weather::read ( tnstream& stream, int version )
{
   bool loadImage = true;
   if (version <=2 ) {
      loadImage = stream.readInt();

      for ( int j = 1; j < 8; j++ )
         stream.readInt(); // pgbt->picture[j]

      for ( int j = 0; j < 8; j++ )
         stream.readInt(); // pgbt->direcpict[j] = (void*)
   }

   if ( version == 1 ) {
      stream.readInt(); //dummy1
      defensebonus = stream.readWord();
      attackbonus = stream.readWord();
      basicjamming = stream.readChar();
   } else {
      defensebonus = stream.readInt();
      attackbonus = stream.readInt();
      basicjamming = stream.readInt();
   }
   int move_maluscount = stream.readChar();
   stream.readInt(); // pgbt->movemalus = (char*)
   stream.readInt(); // pgbt->terraintype
   stream.readInt(); // bool readQuickView

   art.read ( stream );

   bi_pict = stream.readInt();
   for ( int j = 1; j < 6; j++ )
      stream.readInt(); //pgbt->bi_picture[j] =

   move_malus.read( stream, minmalq, move_maluscount );

   if ( version >= 5 )
      originalImageFilename = stream.readString();
   
/*
   for ( j=0; j<8 ;j++ )
      if ( pgbt->picture[j] )
         if ( pgbt->bi_picture[j] == -1 ) {
            pgbt->picture[j] = asc_malloc ( fieldsize );
            stream.readdata ( pgbt->picture[j], fieldsize );
            } else
               loadbi3pict_double ( pgbt->bi_picture[j],
                                    &pgbt->picture[j],
                                    CGameOptions::Instance()->bi3.interpolate.terrain );

*/
   if ( loadImage )
      if ( bi_pict == -1 )
         image.read ( stream );
}

void TerrainType::Weather::write ( tnstream& stream ) const
{
   stream.writeInt ( defensebonus );
   stream.writeInt ( attackbonus );
   stream.writeInt ( basicjamming );
   stream.writeChar ( 0 ); // was: movemalus count
   stream.writeInt ( 1 );
   stream.writeInt ( 1 );
   stream.writeInt ( 0); // was: quickview
   art.write ( stream );


   stream.writeInt ( bi_pict );
   for ( int m = 1; m< 6; m++ )
      stream.writeInt ( -1 );

   move_malus.write ( stream );

   stream.writeString( originalImageFilename );
   
   if ( bi_pict == -1 )
      image.write ( stream );
}


void TerrainType::read( tnstream& stream )
{
   int version = stream.readInt();
   if ( version > terrain_version || (version < 5 && version !=1) ) 
      throw tinvalidversion ( stream.getDeviceName(), terrain_version, version );
      
   stream.readInt(); 
   id   = stream.readInt();

   bool ___loadWeather[cwettertypennum];
   for ( int ww = 0; ww < cwettertypennum; ww++ )
      ___loadWeather[ww] = stream.readInt();

   for ( int nf = 0; nf < 8; nf++ )
      stream.readInt(); // neighbouringfield[nf]

   name = stream.readString();

   for ( int i=0; i<cwettertypennum ;i++ ) {
      if ( ___loadWeather[i] ) {
         weather[i] = new TerrainType::Weather ( this );
         weather[i]->read( stream, version );
      } else
         weather[i] = NULL;

   } /* endfor */

   if ( version >= 6 )
      readClassContainer( secondaryIDs, stream );
}


void TerrainType::write ( tnstream& stream ) const
{
   stream.writeInt ( terrain_version );
   stream.writeInt ( !name.empty() );
   stream.writeInt ( id );
   for ( int m = 0; m < cwettertypennum; m++ )
      stream.writeInt ( weather[m] != NULL );

   for ( int m = 0; m < 8; m++ )
      stream.writeInt ( 0 ); // bbt->neighbouringfield[nf]

   stream.writeString( name );
   for ( int i = 0; i < cwettertypennum ; i++ ) 
      if ( weather[i] ) 
         weather[i]->write( stream );

   writeClassContainer( secondaryIDs, stream );

   /*
   stream.writeString(filename);
   stream.writeString(location);
   */
}


int TerrainType::getMemoryFootprint() const
{
   int size = sizeof(*this);
   
   for ( int w = 0; w < cwettertypennum; ++w )
      if ( weather[w] ) 
         size += weather[w]->getMemoryFootprint();
      
   return size;
}

int TerrainType::Weather::getMemoryFootprint() const
{
   int size = sizeof(*this);
   size += image.getMemoryFootprint();
   if ( quickView )
      size += quickView->getMemoryFootprint();
   return size;
}


TerrainType::~TerrainType()
{
   for ( int i = 0; i< cwettertypennum; ++i) {
      delete weather[i];
      weather[i] = NULL;
   }
}


TerrainAccess :: TerrainAccess ( void )
{
   terrain.flip();
}

int TerrainAccess :: accessible ( const TerrainBits& bts ) const
{
   if (     (terrain & bts).any()
         && (terrainreq & bts) == terrainreq
         && (terrainnot & bts ).none()
      ) return 1;
   else
      if ( (terrainkill & bts).any() )
         return -1;
      else
         return 0;
}


void TerrainBits::setInt ( int terrain1, int terrain2 )
{
   reset();
   for ( int i = 0; i < 32; i++ )
      if ( terrain1 & ( 1 << i ))
         set(i);

   for ( int i = 0; i < 32; i++ )
      if ( terrain2 & ( 1 << i ))
         set(32+i);
}

void TerrainBits::read ( tnstream& stream )
{
     int terrain1 = stream.readInt();
     int terrain2 = stream.readInt();
     setInt ( terrain1, terrain2 );
  };

void TerrainBits::write ( tnstream& stream ) const
{
     int terrain1 = 0;
     int terrain2 = 0;
     for ( int i = 0; i < 32; i++ )
        if ( test(i) )
           terrain1 |=   1 << i ;

     for ( int i = 0; i < 32; i++ )
        if ( test(32+i) )
           terrain2 |=   1 << i ;

     stream.writeInt( terrain1 );
     stream.writeInt( terrain2 );
}




void TerrainType :: runTextIO ( PropertyContainer& pc )
{
   pc.addBreakpoint(); 
  
   
   BitSet weatherBits;
   for ( int i = 0; i < cwettertypennum; i++ )
      if ( weather[i] )
         weatherBits.set(i);

   pc.addString( "Name", name );
   pc.addInteger( "ID", id );

   if ( pc.find( "SecondaryIDs") || !pc.isReading())
      pc.addIntegerArray("SecondaryIDs", secondaryIDs );


   pc.addTagArray( "Weather", weatherBits, cwettertypennum, weatherTags );

   for ( int i = 0; i < cwettertypennum; i++ )
      if ( weatherBits.test(i) ) {

         if ( pc.isReading() )
            weather[i] = new TerrainType::Weather ( this );

         pc.openBracket  ( weatherTags[i] );
         weather[i]->runTextIO ( pc );
         pc.closeBracket (  );
      }

}

void TerrainType::Weather::runTextIO ( PropertyContainer& pc )
{
   bool bi3pics = false;

   if ( !pc.isReading() )
      if ( bi_pict >= 0 )
         bi3pics = true;

   pc.addBool  ( "UseGFXpics", bi3pics, false );
   if ( !bi3pics ) {
      bi_pict = -1;
      int w = cwettertypennum-1;
      for ( int i = 0; i < cwettertypennum-1; i++ )
         if ( terraintype->weather[i] == this )
            w = i;

      ASCString s = extractFileName_withoutSuffix ( terraintype->filename );
      if ( s.empty() ) {
         s = "terrain";
         s += strrr(terraintype->id);
      }
      s += weatherAbbrev[w];
      pc.addImage ( "picture", image, s, true );
      
      if ( pc.isReading() ) {
         originalImageFilename = s;
      } else {
         pc.addString( "OriginalImageFilename", originalImageFilename );
      }
      
      // applyFieldMask( image );

      if ( pc.isReading() ) {
         int operations;
         pc.addNamedInteger("GraphicOperations", operations, graphicOperationNum, graphicOperations, 0 );
         if ( operations == 1 )  
            snowify( image );
         else
            if ( operations == 2 )
               snowify( image, false );
      }

   } else {
      pc.addInteger ( "GFX_Picture", bi_pict );
      if  ( bi_pict < 0 )
         fatalError ( "invalid BI-image " );
   }
   
   pc.addInteger ( "DefenseBonus", defensebonus, 0 );
   pc.addInteger ( "AttackBonus",  attackbonus, 0 );
   pc.addInteger ( "BasicJamming", basicjamming, 0 );
   pc.addDFloatArray ( "MoveMalus", move_malus );
   while ( move_malus.size() < cmovemalitypenum ) 
     if ( move_malus.size() == 0 )
        move_malus.push_back ( minmalq );
     else
        move_malus.push_back ( move_malus[0] );


   pc.addTagArray ( "TerrainProperties", art, terrainPropertyNum, terrainProperties );
}




TerrainBits getTerrainBitType ( TerrainBitTypes tbt )
{
   TerrainBits tb;
   switch ( tbt ) {
      case cbwater0 : tb.setInt( 1<<22 ); break;
      case cbwater1 : tb.setInt( 1 );   break;
      case cbwater2 : tb.setInt( 4096 ); break;
      case cbwater3 : tb.setInt( 8192 ); break;
      case cbwater  : tb.setInt (  (1<<22)  // very shallow water
                                 | (1<< 0)  // shallow water
                                 | (1<<12)  // water
                                 | (1<<13)  // deep water
                                 | (1<<31)  // river
                                 ); break;
      case cbstreet : tb.setInt ( 32, 0 ); break;
      case cbrailroad : tb.setInt ( 64, 0 ); break;
      case cbbuildingentry : tb.setInt ( 128, 0 ); break;
      case cbharbour     : tb.setInt ( 256, 0 ); break;
      case cbrunway      : tb.setInt ( 512, 0 ); break;
      case cbpipeline    : tb.setInt ( 1024, 0 ); break;
      case cbpowerline   : tb.setInt ( 2048, 0 ); break;
      case cbfahrspur    : tb.setInt ( 1<<16, 0 ); break;
      case cbfestland    : tb.flip(); tb ^= getTerrainBitType( cbwater ); break;
      case cbsnow1       : tb.setInt ( 1<<19, 0 ); break;
      case cbsnow2       : tb.setInt ( 1<<20, 0 ); break;
      case cbhillside    : tb.setInt ( 1<<26, 0 ); break;
      case cbsmallrocks  : tb.setInt ( 1<<17, 0 ); break;
      case cblargerocks  : tb.setInt ( 1<<23, 0 ); break;
      case cbfrozenwater : tb.setInt ( 0, 1 ); break;
      case cbicebreaking : tb |= getTerrainBitType( cbfrozenwater )
                              | getTerrainBitType( cbsnow1 )
                              | getTerrainBitType(cbsnow2 );
      case cbriver       : tb.setInt(  1 << 31, 0 ); break;

   };
   return tb;
}


ASCString TerrainAccess :: toString( bool brief ) const
{
   ASCString text;

   text += "Terrain properties of which one must be available:\n" ;
   terrain.appendToString( text );

   if ( !( brief && terrainreq.none() )) {
      text += "\n\nTerrain properties that must all be set:\n" ;
      terrainreq.appendToString( text );
   }

   if ( !( brief && terrainnot.none() )) {
      text += "\n\nTerrain properties that must not be set:\n" ;
      terrainnot.appendToString( text );
   }

   if ( !( brief && terrainkill.none() )) {
      text += "\n\nTerrain properties that are fatal:\n" ;
      terrainkill.appendToString( text );
   }

   return text;
}
