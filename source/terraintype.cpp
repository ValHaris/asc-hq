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

#include "basegfx.h"
#include "terraintype.h"
#include "gameoptions.h"
#include "graphicset.h"
#include "sgstream.h"
#include "textfileparser.h"
#include "textfiletags.h"
#include "textfile_evaluation.h"

const char*  cbodenarten[cbodenartennum]  = {"shallow water"       ,
                                             "normal lowland",
                                             "swamp",
                                             "forest",
                                             "high mountains",
                                             "road",
                                             "railroad",
                                             "entry of building (not to be used for terrain)" ,
                                             "harbour",
                                             "runway"  ,
                                             "pipeline",
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
                                             "lava barrier" };




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
   pc.addTagArray ( "terrain_any", terrain, cbodenartennum, bodenarten );
   pc.addTagArray ( "terrain_all", terrainreq, cbodenartennum, bodenarten );
   pc.addTagArray ( "terrain_not", terrainnot, cbodenartennum, bodenarten );
   pc.addTagArray ( "terrain_kill", terrainkill, cbodenartennum, bodenarten );
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

void      TerrainType::Weather::paint ( int x1, int y1 )
{
 #ifndef converter
   putspriteimage ( x1, y1, pict );
 #endif
}

const FieldQuickView* TerrainType::Weather::getQuickView()
{
   if  ( bi_pict >= 0 ) {
      return getActiveGraphicSet()->getQuickView( bi_pict );
   } else {
      if (!quickView ) {
         quickView = generateAverageCol( pict );
      }
      return quickView;
   }
}


const int terrain_version = 2;


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


void TerrainType::read( tnstream& stream )
{
   int version = stream.readInt();
   if ( version == terrain_version || version == 1) {

      stream.readInt(); // name = (char*)
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
            Weather* pgbt = weather[i];

            int j;

            pgbt->pict = (void*) stream.readInt();

            for ( j = 1; j < 8; j++ )
               stream.readInt(); // pgbt->picture[j]

            for ( j = 0; j < 8; j++ )
               stream.readInt(); // pgbt->direcpict[j] = (void*)

            if ( version == 1 ) {
               stream.readInt(); //dummy1
               pgbt->defensebonus = stream.readWord();
               pgbt->attackbonus = stream.readWord();
               pgbt->basicjamming = stream.readChar();
            } else {
               pgbt->defensebonus = stream.readInt();
               pgbt->attackbonus = stream.readInt();
               pgbt->basicjamming = stream.readInt();
            }
            int move_maluscount = stream.readChar();
            stream.readInt(); // pgbt->movemalus = (char*)
            stream.readInt(); // pgbt->terraintype
            bool readQuickView = stream.readInt();

            pgbt->art.read ( stream );

            pgbt->bi_pict = stream.readInt();
            for ( j = 1; j < 6; j++ )
               stream.readInt(); //pgbt->bi_picture[j] =

            pgbt->move_malus.read( stream, minmalq, move_maluscount );

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
            if ( pgbt->pict )
               if ( pgbt->bi_pict == -1 ) {
                  int sze;
                  stream.readrlepict ( &pgbt->pict, false, &sze );
                  // stream.readdata ( pgbt->pict, fieldsize ); // endian ????
                } else
                   loadbi3pict_double ( pgbt->bi_pict,
                                        &pgbt->pict,
                                        CGameOptions::Instance()->bi3.interpolate.terrain );


            if ( readQuickView )
               pgbt->readQuickView( stream );

         } else
            weather[i] = NULL;


      } /* endfor */

   } else
      throw tinvalidversion ( stream.getDeviceName(), terrain_version, version );
}

void TerrainType::Weather::readQuickView ( tnstream& stream )
{
   quickView = new FieldQuickView;

   stream.readdata ( quickView, sizeof ( *quickView )); // endian ok !!!

   FieldQuickView temp;
   for ( int i = 1; i < 8; i++ )
      stream.readdata ( &temp, sizeof ( *quickView )); // endian ok !!!
}


void TerrainType::write ( tnstream& stream ) const
{
   int m;

   stream.writeInt ( terrain_version );
   stream.writeInt ( !name.empty() );
   stream.writeInt ( id );
   for ( m = 0; m < cwettertypennum; m++ )
      stream.writeInt ( weather[m] != NULL );

   for ( m = 0; m < 8; m++ )
      stream.writeInt ( 0 ); // bbt->neighbouringfield[nf]

   stream.writeString( name );
   for (int i=0;i<cwettertypennum ;i++ ) {
     if ( weather[i] ) {
        stream.writeInt ( weather[i]->pict == NULL ?  0 : 1 );

        for ( m = 1; m < 8; m++ )
           stream.writeInt ( 0 );

        for ( m = 0; m < 8; m++ )
           stream.writeInt ( 0 );

        stream.writeInt ( weather[i]->defensebonus );
        stream.writeInt ( weather[i]->attackbonus );
        stream.writeInt ( weather[i]->basicjamming );
        stream.writeChar ( 0 ); // was: movemalus count
        stream.writeInt ( 1 );
        stream.writeInt ( 1 );
        stream.writeInt ( 0); // was: quickview
        weather[i]->art.write ( stream );


        stream.writeInt ( weather[i]->bi_pict );
        for ( m = 1; m< 6; m++ )
           stream.writeInt ( -1 );

        weather[i]->move_malus.write ( stream );

        if ( weather[i]->pict && weather[i]->bi_pict == -1 )
           stream.writeImage ( weather[i]->pict, false );

     }
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
   BitSet weatherBits;
   for ( int i = 0; i < cwettertypennum; i++ )
      if ( weather[i] )
         weatherBits.set(i);

   pc.addString( "Name", name );
   pc.addInteger( "ID", id );

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

   pc.addBool  ( "UseGFXpics", bi3pics );
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
      pc.addImage ( "picture", pict, s + weatherAbbrev[w] );

   } else {
      pc.addInteger ( "GFX_Picture", bi_pict );
      loadbi3pict_double ( bi_pict,
                           &pict,
                           CGameOptions::Instance()->bi3.interpolate.terrain );
   }
   
   pc.addInteger ( "DefenseBonus", defensebonus );
   pc.addInteger ( "AttackBonus",  attackbonus );
   pc.addInteger ( "BasicJamming", basicjamming );
   pc.addDFloatArray ( "MoveMalus", move_malus );
   while ( move_malus.size() < cmovemalitypenum ) 
     if ( move_malus.size() == 0 )
        move_malus.push_back ( minmalq );
     else
        move_malus.push_back ( move_malus[0] );


   pc.addTagArray ( "TerrainProperties", art, cbodenartennum, bodenarten );
}




TerrainBits getTerrainBitType ( TerrainBitTypes tbt )
{
   TerrainBits tb;
   switch ( tbt ) {
      case cbwater0 : tb.setInt( 1<<22 ); break;
      case cbwater1 : tb.setInt( 1 );   break;
      case cbwater2 : tb.setInt( 4096 ); break;
      case cbwater3 : tb.setInt( 8192 ); break;
      case cbwater  : tb.setInt ( 1<<22 | 1 | 4096 | 8192 ); break;
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

   };
   return tb;
}


