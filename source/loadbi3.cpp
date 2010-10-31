/*! \file loadbi3.cpp
    \brief Importing maps from Battle Isle
*/
 
/*
    This file is part of Advanced Strategic Command; http://www.asc-hq.de
    Copyright (C) 1994-2010 Martin Bickel  and  Marc Schellenberger

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
#include <stdlib.h>
#include <cstring>

#include "loadbi3.h"
#include "typen.h"
#include "buildingtype.h"
#include "vehicletype.h"
#include "misc.h"
#include "gameoptions.h"
#include "graphicset.h"
#include "spfst.h"
#include "spfst-legacy.h"
#include "loaders.h"
#include "dialog.h"
#include "itemrepository.h"
#include "textfile_evaluation.h"

/*#ifdef convert/er
  #error The small editors should not need to use LoadBi3
  #endif*/


#pragma pack(1)



class timporterror : public ASCexception {};


int battleisleversion = -1;

const int LIBFilesAnz  = 11;

struct  TLIBFiles {
      const char*  Name;
      int FirstRecO;
      int RecSize;
      int DataInRecOfs;
      int DataSize;
      int Anzahl;
  };
 TLIBFiles LIBFiles [ LIBFilesAnz ] = 
      {{"part000.lib",  0xC, 0x240,   0, 0x240, 1335},
      {"unit000.lib",  0x4, 0x240,   0, 0x240, 128},   // 128 f?r BI3   // 118 f?r BI2 Scenery
      {"layr000.lib",  0x4, 0x240,   0, 0x240, 24},
      {"layr001.lib",  0x4, 0x240,   0, 0x240, 24},
      {"layr002.lib",  0x4, 0x240,   0, 0x240, 24},
      {"layr003.lib",  0x4, 0x240,   0, 0x240, 24},
      {"layr004.lib",  0x4, 0x240,   0, 0x240, 24},
      {"layr005.lib",  0x4, 0x240,   0, 0x240, 24},
      {"layr006.lib",  0x4, 0x240,   0, 0x240, 24},
      {"curs000.lib",  0xC, 0x25E,0x1A, 0x240, 12},
      {"palt000.lib",  0xC, 0x300,   0, 0x300, 4 } ,
      } ;
      

int libs_to_load = 9;


void checkbi3dir ( void )
{
   if ( CGameOptions::Instance()->BI3directory.empty() ) {
      readgameoptions();
      /*
      if ( !gameoptions.bi3.dir.getName() ) {
         gameoptions.bi3.dir = new char [300];
         gameoptions.bi3.dir[0] = 0;
      }
      */
   }

   bool notfound;
   do {
      notfound = false;
      
      ASCString filename;

      for ( int i = 0; i < libs_to_load ; i++ ) {
         filename =  CGameOptions::Instance()->BI3directory + LIBFiles[i].Name ;
         
         if ( !exist ( filename ) ) {
            filename = CGameOptions::Instance()->BI3directory + "LIB" + pathdelimitterstring + LIBFiles[i].Name;
            if ( !exist ( filename ) ) {
               printf("Battle Isle file %s not found !\n", filename.c_str() );
               notfound = true;
            }
         }
      }
     if ( notfound ) {
            char bi3path[10000];
            printf("Enter Battle Isle directory:\n" );
            if ( !scanf ( "%s", bi3path ) )
               bi3path[0] = 0;

            CGameOptions::Instance()->BI3directory = bi3path;
            appendbackslash ( CGameOptions::Instance()->BI3directory );
            CGameOptions::Instance()->setChanged();
     }
   } while ( notfound ); /* enddo */
   battleisleversion = 3;
}


ASCString getbi3path ( void )
{
   return CGameOptions::Instance()->BI3directory;
}


//! a table to translate a Battle Isle map into an ASC map
class Bi3MapTranslationTable {
    public:
       //! The first entry is the picture number that is going to be replaced by the picture number of the second entry.
       vector< pair<int,int> > terraintranslation;

       struct Terrain2id {
           int BIpic;
           int terrainid;
           int weather;
           void read ( tnstream& stream ) {
               int version = stream.readInt();
               if ( version != 1 )
                  throw tinvalidversion( stream.getDeviceName(), 1, version );
               BIpic = stream.readInt();
               terrainid = stream.readInt();
               weather = stream.readInt();
           };

           void write ( tnstream& stream ) const {
               stream.writeInt( 1 );
               stream.writeInt( BIpic );
               stream.writeInt( terrainid );
               stream.writeInt( weather );
           };
       };
       /** The first entry is the picture number that is going to be replaced by the
           terrain with the ID of the second number and the weather of the third
           number */
       vector< Terrain2id > terrain2idTranslation;


       struct Terraincombixlat {
                   int bigraph;
                   int terrainid;
                   int terrainweather;
                   int objectid;
                   void read ( tnstream& stream ) {
                      stream.readInt();
                      bigraph = stream.readInt();
                      terrainid = stream.readInt();
                      terrainweather = stream.readInt();
                      objectid = stream.readInt();
                   };

                   void write ( tnstream& stream ) const {
                      stream.writeInt( 1 );
                      stream.writeInt( bigraph );
                      stream.writeInt( terrainid );
                      stream.writeInt( terrainweather );
                      stream.writeInt( objectid );
                   };

            };
        //! This is a special translation for the fields that must be translated to a terrain AND an additional object
        vector<Terraincombixlat> terraincombixlat;


        struct Objecttranslataion {
           int BIpic;
           int objects[4];
           int terrainID;
           int terrainWeather;
           void read ( tnstream& stream ) {
               stream.readInt();
               BIpic = stream.readInt();
               for ( int i = 0; i< 4; ++i )
                  objects[i] = stream.readInt();
               terrainID = stream.readInt();
               terrainWeather = stream.readInt();
           };

           void write ( tnstream& stream ) const {
               stream.writeInt( 1 );
               stream.writeInt( BIpic );
               for ( int i = 0; i< 4; ++i )
                   stream.writeInt( objects[i] );
               stream.writeInt( terrainID );
               stream.writeInt( terrainWeather );
           };
        };

        /** These BI object pictures can be translated to up to four ASC objects. The first entry is again the BI picture number,
            the following four the pictures for the ASC objects. A -1 is used if the entry is not used. It does not matter if you
            use " -1, 1500, -1  , -1 " or " 1500, -1  , -1,  -1" or any other order. */
        vector<Objecttranslataion> objecttranslate ;

       /** These BI object pictures can be translated to ASC objects.
           The first entry is again the BI picture number, the second is the
           ID of the ASC object. */
        vector< pair<int,int> > object2IDtranslate;

        void runTextIO ( PropertyContainer& pc );

        void read ( tnstream& stream );
        void write ( tnstream& stream ) const;
        ASCString filename, location;
};


void Bi3MapTranslationTable :: read ( tnstream& stream )
{
   int version = stream.readInt();
   readClassContainer( terraintranslation, stream );
   readClassContainer( terrain2idTranslation, stream );
   readClassContainer( terraincombixlat, stream );
   readClassContainer( objecttranslate, stream );
   readClassContainer( object2IDtranslate, stream );
   if ( version >= 2 ) {
      filename = stream.readString();
      location = stream.readString();
   }

}



void Bi3MapTranslationTable :: write ( tnstream& stream ) const
{
   stream.writeInt ( 2 );
   writeClassContainer ( terraintranslation, stream );
   writeClassContainer ( terrain2idTranslation, stream );
   writeClassContainer ( terraincombixlat, stream );
   writeClassContainer ( objecttranslate, stream );
   writeClassContainer ( object2IDtranslate, stream );
   stream.writeString( filename );
   stream.writeString( location );
}


void Bi3MapTranslationTable :: runTextIO ( PropertyContainer& pc )
{
   vector<int> _terraintranslation;
   pc.addIntegerArray ( "TerrainTranslation", _terraintranslation );

   vector<int> _terrain2idtrans;
   pc.addIntegerArray ( "Terrain2IDTranslation", _terrain2idtrans );

   vector<int> _terrainobjtranslation;
   pc.addIntegerArray ( "TerrainObjTranslation", _terrainobjtranslation );

   vector<int> _objecttranslation;
   if ( pc.find( "ObjectTranslation" ))
      pc.addIntegerArray ( "ObjectTranslation", _objecttranslation );

   vector<int> _object2IDtranslation;
   pc.addIntegerArray ( "Object2IDTranslation", _object2IDtranslation );

   vector<int> object3translation;
   if ( pc.find( "Object3Translation" ))
      pc.addIntegerArray ( "Object3Translation", object3translation );


   if ( _terraintranslation.size() % 2 )
      fatalError ( "Bi3 map translation : terraintranslation - Invalid number of entries ");
   terraintranslation.clear();
   for ( int i = 0; i < _terraintranslation.size()/2; i++ )
      terraintranslation.push_back ( make_pair( _terraintranslation[i*2], _terraintranslation[i*2+1] ));


   if ( _terrain2idtrans.size() % 3 )
      fatalError ( "Bi3 map translation : terrain2idTranslation - Invalid number of entries ");
   terrain2idTranslation.clear();
   for ( int i = 0; i < _terrain2idtrans.size()/3; i++ ) {
      Terrain2id t2i;
      t2i.BIpic = _terrain2idtrans[i*3];
      t2i.terrainid = _terrain2idtrans[i*3+1];
      t2i.weather = _terrain2idtrans[i*3+2];
      terrain2idTranslation.push_back ( t2i );
   }

   if ( _terrainobjtranslation.size() % 4 )
      fatalError ( "Bi3 map translation : terrainobjtranslation - Invalid number of entries ");
   terraincombixlat.clear();
   for ( int i = 0; i < _terrainobjtranslation.size()/4; i++ ) {
       Terraincombixlat tcx;
       tcx.bigraph = _terrainobjtranslation[i*4];
       tcx.terrainid = _terrainobjtranslation[i*4+1];
       tcx.terrainweather = _terrainobjtranslation[i*4+2];
       tcx.objectid = _terrainobjtranslation[i*4+3];
       terraincombixlat.push_back ( tcx );
   }

   if ( _objecttranslation.size() % 5 )
      fatalError ( "Bi3 map translation : objecttranslation - Invalid number of entries ");
   objecttranslate.clear();
   for ( int i = 0; i < _objecttranslation.size()/5; i++ ) {
      Objecttranslataion ot;
      ot.BIpic = _objecttranslation[i*5];
      for ( int j = 0; j < 4; j++ )
         ot.objects[j] = _objecttranslation[i*5 + 1 + j];
      ot.terrainID = -1;
      ot.terrainWeather = 0;

      objecttranslate.push_back ( ot );
   }


   if ( object3translation.size() % 7 )
      fatalError ( "Bi3 map translation : object3translation - Invalid number of entries ");
   for ( int i = 0; i < object3translation.size()/7; i++ ) {
      Objecttranslataion ot;
      ot.BIpic = object3translation[i*7];
      for ( int j = 0; j < 4; j++ )
         ot.objects[j] = object3translation[i*7 + 1 + j];
      ot.terrainID = object3translation[i*7 + 5];
      ot.terrainWeather = object3translation[i*7 + 6];

      objecttranslate.push_back ( ot );
   }


   if ( _object2IDtranslation.size() % 2 )
      fatalError ( "Bi3 map translation : object2IDtranslation - Invalid number of entries ");
   object2IDtranslate.clear();
   for ( int i = 0; i < _object2IDtranslation.size()/2; i++ )
      object2IDtranslate.push_back ( make_pair ( _object2IDtranslation[i*2], _object2IDtranslation[i*2+1] ));
}




PointerVector<Bi3MapTranslationTable*> bi3ImportTables;

Bi3MapTranslationTable* findTable( const ASCString& filename ) {
   for ( PointerVector<Bi3MapTranslationTable*>::iterator i = bi3ImportTables.begin(); i != bi3ImportTables.end(); ++i )
      if ( (*i)->filename == filename )
         return *i;
   return NULL;
}

vector<ASCString> getBI3ImportTables()
{
   vector<ASCString> list;
   for ( PointerVector<Bi3MapTranslationTable*>::iterator i = bi3ImportTables.begin(); i != bi3ImportTables.end(); ++i )
      list.push_back ( (*i)->filename );
   return list;
}


void BI3TranslationTableLoader::read ( tnstream& stream )
{
   stream.readInt();
   readPointerContainer ( bi3ImportTables, stream );
}

void BI3TranslationTableLoader::write ( tnstream& stream )
{
   stream.writeInt( 1 );
   writePointerContainer ( bi3ImportTables, stream );
}


ASCString BI3TranslationTableLoader::getTypeName()
{
   return "bi3maptranslation";
}

void BI3TranslationTableLoader::readTextFiles( PropertyReadingContainer& prc, const ASCString& fileName, const ASCString& location )
{
   Bi3MapTranslationTable* bmtt = new Bi3MapTranslationTable;
   bmtt->runTextIO ( prc );
   bmtt->filename = fileName;
   bmtt->location = location;
   bi3ImportTables.push_back ( bmtt );
}


const int fuelfactor = 120;
const int materialfactor = 390;
const int energyfactor = 390;




      const char* HeadID = "MSSN";
      const char* ACTNID = "ACTN";
      const char* SHOPID = "SHOP";
      const char* MAPID  = "MAP\0";
      const char* MISSID = "MISS";



class tloadBImap {
       Bi3MapTranslationTable* translationTable;
     
       struct THeader {
         char ID[4];                            // { 'MSSN' }
         int MissPos;
         int MapPos ;
         int ShopPos;
         int ACTNPos;
         Uint16 DFNum ;
         int DFPos[32] ;                     // { Pos nach DFXX }
         int DFLength[32];                   // { +4B von DFXX }
       };

       struct TACTN {
         char ID[4];
         Uint16 XSize, YSize;
       };

       typedef Uint16 TStdProdRec[64];
       typedef char TAllianzen[6];


       struct TMISSPart {
           char ID[4];
           Uint16 Stuff1 [3];
           Uint16 NextMiss;
           char StartWeather;
           char WhoPlays;           // { jeder Mensch/Computer ein Bit }
           char PlayType;           // { jeder Computer ein Bit }
           char Recs2Win;           // { Wieviel Gewinnrecords aus map erf llt werden m ssen, damit man gewinnt (f r jeden player gleich) }
           char Stuff2[3];
           char Landscape;
           TAllianzen Allianz;      // { Wer mit wem }
           Uint16 Stuff3[65];
           TStdProdRec StdProd;
           Uint16 Stuff4[24];
       };
          
       struct TMAPHead {
              char ID[4];
       };
          
       struct TFileMap { 
              Uint16 What1;
              Uint16 Round, Move;
              char Player, What2;
              Uint16 Stuff2;
              Uint16 Zero1;
              Uint16 Nr1;
              Uint16 Nr2;
              Uint16 Nr3;
              Uint16 Zero2[11];
       };
          
       struct TSHOPHead {
              char ID[4];
              Uint16 Num;
       };
          
       typedef int TVehContent[8];
       typedef TVehContent *PVehContent;
          
          union TShopContent {
             Uint16 XY[4][4];
             Uint16 All[16];
             TVehContent Veh;
          };
       typedef TShopContent *PShopContent;
         
       typedef Uint16 TProduceRec[4];

       struct TFileShop {
         Uint16 ID;
         Uint16 Pos1;
         union  {
            struct {
               Uint16 ShopType;
               Uint16 Stuff06;  // { Zero }
               Uint16 Name;
               Uint16 Pos2;
               Uint16 E, M;
               char EP, MP ;
               TShopContent Content;
               Uint16 ShopType2;
               TProduceRec Produce;
               Uint16 Stuff60 ; // { Zero }
               Uint16 Prior;
               Uint16 Owner;
               Uint16 Stuff66[7];  // { Zero }
            } a;
            struct {
                char Zero2[6];
                Uint16 W0A;
                char Zero3[6];
                Uint16 W48;
                Uint16 W4A;
                char Zero4[2];
                Uint16 W4E;
            } b;
            char raw[76];
         };
       };

       dynamic_array<char*> names;
       int namenum;
       bool fakemap;

     protected:
        int xoffset, yoffset;
        TerrainType::Weather* defaultterraintype;
     public:
       void LoadFromFile( const char* path, const char* AFileName, TerrainType::Weather* trrn, string* errorOutput );
       tloadBImap ( Bi3MapTranslationTable* _translationTable ) : translationTable ( _translationTable ) {
          xoffset = 0;
          yoffset = 0;
          fakemap = false;
       };

       void ____fakeMap____ ( void ) { fakemap = true; };
       virtual ~tloadBImap() {};
     private:

       tn_file_buf_stream* MissFile;
       THeader Header;

       struct TTextItem {
           int a;
           int b;
           char name[1000];
       };

       dynamic_array<TTextItem> ShopNameList;
       void LoadTXTFile ( char* filename );
//       int TPWMtextfile;

       void ReadMISSPart(void);
       void ReadACTNPart(void);
       void ReadSHOPPart(void);
       void ReadShopNames( char *txtdata, unsigned long txtsize );
       Vehicle* getunit ( int typ, int col );
       VehicleType* getvehicletype ( int typ );
       char* GetStr ( int a, int b );
       int convcol ( int c );

       TMISSPart OrgMissRec;
       void GetTXTName ( const char* path, const char* filename, char* buf );

      protected:
        char* missing;
        virtual void preparemap ( int x, int y  ) = 0;
        virtual MapField* getfield ( int x, int y );

};

class ImportBiMap : public tloadBImap {
         protected:
           virtual void preparemap ( int x, int y  );
         public:
           ImportBiMap ( Bi3MapTranslationTable* _translationTable ) : tloadBImap (  _translationTable ) {};
};

class InsertBiMap : public tloadBImap {
         protected:
           virtual void preparemap ( int x, int y  );
           virtual MapField* getfield ( int x, int y );
         public:
           InsertBiMap ( int x, int y, Bi3MapTranslationTable* _translationTable ) : tloadBImap (  _translationTable ) {
             xoffset = x; 
             yoffset = y;
           };
};


MapField* tloadBImap :: getfield ( int x, int y )
{
   return ::getfield ( x, y );
}

MapField* InsertBiMap :: getfield ( int x, int y )
{
   return ::getfield ( xoffset + x, yoffset + y );
}


void  tloadBImap ::  ReadMISSPart(void)
{ 

  MissFile->seek ( Header.MissPos );

  MissFile->readdata2 ( OrgMissRec ); 
  if ( strncmp ( OrgMissRec.ID, MISSID, 4)  ) {
     strcat ( missing, "\nFatal error: No Battle Isle mission; invalid MissID\n"  );
     throw timporterror (); 
  }

/*
  int I; 
  StdProd = OrgMissRec.StdProd; 
  for (I = 0; I <= 63; I++) 
      StdProd[I] = StdProd[I] & 0x03; 

  PlayerType = OrgMissRec.PlayType & 0x3F; 
  Allianz = OrgMissRec.Allianz; 
  for (I = 0; I <= 5; I++) 
     Allianz[I] = Allianz[I] & 0x3F; 
  Recs2Win = OrgMissRec.Recs2Win;
  StartWeather = OrgMissRec.StartWeather; 
  if ( StartWeather < 0  ||  StartWeather > 4 )
     StartWeather = 0; 
  NextMiss = OrgMissRec.NextMiss; 
  Landscape = OrgMissRec.Landscape; 
*/
} 

int tloadBImap :: convcol ( int c )
{
   int cl = getFirstBit ( c );
   if ( cl == 6 )
      return 8;
   else
     if ( cl < 2 )
        cl = 1 - cl;
   return cl;
}

  
const int bi3unitnum = 74;
int translateunits[ bi3unitnum ][2] = { {1201,26}, {1270,26}, {1202,13}, {1200,6}, {1203,-1},
                                        {1204,16}, {1205,17}, {1206,4}, {1207,65}, {1208,-1},
                                        {1209,7},  {1210,66}, {1211,2}, {1212,3},  {1213,17},
                                        {1214,51}, {1215,17}, {1216,2}, {1217,10}, {1218,11},
                                        {1219,16}, {1220,61}, {1221,49},{1222,56}, {1276,-1},
                                        {1223,25}, {1224,72}, {1225,57},{1226,58}, {1227,7},
                                        {1228,59}, {1241,9},  {1242,22},{1244,19}, {1243,29},
                                        {1245,55}, {1246,19}, {1271,21},{1247,52}, {1248,31},
                                        {1251,15}, {1252,15}, {1256,39},{1257,42}, {1258,47},
                                        {1259,14}, {1260,36}, {1261,34},{1262,47}, {1263,37},
                                        {1264,38}, {1265,5},  {1229,18},{1277,-1}, {1249,-1},
                                        {1230,1},  {1231,66}, {1266, 5},{1232,64}, {1233,-1},
                                        {1234,4},  {1235,4},  {1267,35},{1250,29}};


VehicleType*  tloadBImap :: getvehicletype ( int tp )
{
   for ( int j = 0; j < vehicleTypeRepository.getNum(); j++ ) {
      VehicleType* tnk = vehicleTypeRepository.getObject_byPos ( j );
      if ( tnk )
         if ( tnk->bipicture > 0 )
            if ( tnk->bipicture == 1340 + tp * 2 )
                  return tnk;
   }


   if ( tp < bi3unitnum )
      for ( int i = 0; i < 2; i++ )
         if ( translateunits[tp][i] > 0 ) {
            VehicleType* tnk = vehicleTypeRepository.getObject_byID ( translateunits[tp][i] );
            if ( tnk )
               return tnk;
         }
   return NULL;
}

Vehicle* tloadBImap :: getunit ( int tp, int col )
{
   if ( tp != 0xffff && tp != 0xff && col != 0xff ) {
      VehicleType* vt = getvehicletype ( tp );
      if ( vt ) {
         Vehicle* eht = new Vehicle ( vt, actmap, col );
         eht->fillMagically();
         return eht;
      }
   }

   return NULL;
}

void         generatemap( TerrainType::Weather*   bt,
                          int                xsize,
                          int                ysize)
{
   delete actmap;
   actmap = new GameMap;
   for (int k = 1; k < 8; k++)
      actmap->player[k].stat = Player::computer;

   actmap->maptitle = "new map";

   actmap->allocateFields(xsize, ysize);

   if ( actmap->field== NULL)
      displaymessage ( "Could not generate map !! \nProbably out of enough memory !",2);

   for ( int l = 0; l < xsize*ysize; l++ ) {
      actmap->field[l].typ = bt;
      actmap->field[l].setparams();
      actmap->field[l].setMap( actmap );
   }

   actmap->_resourcemode = 1;
}


void ImportBiMap :: preparemap ( int x, int y  )
{
    generatemap ( defaultterraintype, x, y ); 
    actmap->setgameparameter( cgp_movefrominvalidfields, 1);
    actmap->setgameparameter( cgp_forbid_building_construction, 1);
}

void InsertBiMap :: preparemap ( int x, int y  )
{
   if ( yoffset & 1 ) {
      yoffset--;
      strcat ( missing, "y position for map insertion must be even; current position decreased\n");
   }
   int xp = xoffset;
   int yp = yoffset;
   if ( xp + x > actmap->xsize ) {
      int dx = xp + x - actmap->xsize;
      strcat ( missing, "map had to be resized in X direction\n");
      int r = actmap->resize ( 0, 0, 0, dx );
      if ( r ) {
         strcat ( missing, "Resizing failed !!\n");
         throw timporterror();
      }
   }
   if ( yp + y > actmap->ysize ) {
      int dy = yp + y - actmap->ysize;
      strcat ( missing, "map had to be resized in Y direction\n");
      int r = actmap->resize ( 0, dy, 0, 0 );
      if ( r ) {
         strcat ( missing, "Resizing failed !!\n");
         throw timporterror();
      }
   }

   for ( int b = 0; b < y; b++ )
      for ( int a = 0; a < x; a++ )
         getfield ( a, b ) -> deleteeverything();
}

void  stu_height ( Vehicle* vehicle )
{
   char l;
   MapField* fld = getfield ( vehicle->xpos, vehicle->ypos );

   vehicle->height = chfahrend;

   for (l=chsatellit; l> chfahrend ;  ) {
      if (vehicle->typ->height & l )
         vehicle->height = l;
      l>>=1;
   } /* endfor */

   for (l=chtiefgetaucht; l<= chschwimmend ;  ) {
      if (vehicle->typ->height & l )
        if (vehicle->typ->terrainaccess.accessible (  fld->bdt ) > 0 )
           vehicle->height = l;
      l<<=1;
   }

   if (vehicle->typ->height & chfahrend)
      if (vehicle->typ->terrainaccess.accessible ( fld->bdt ) > 0 )
         vehicle->height = chfahrend;

   vehicle->setMovement ( vehicle->typ->movement[getFirstBit( vehicle->height )] );
}


void        tloadBImap ::   ReadACTNPart(void)
{ 
    struct {
       int X, Y;
    } Size;

    TACTN        ACTNHead; 
  
    MissFile->seek ( Header.ACTNPos );
    MissFile->readdata2( ACTNHead ); 
    if ( strncmp ( ACTNHead.ID ,ACTNID,4 )) {
       strcat ( missing, "\nFatal error: No Battle Isle mission; invalid ACTNID\n"  );
       throw timporterror (); 
    }

    Size.X = ACTNHead.XSize;
    Size.Y = ACTNHead.YSize;
    if ( Size.Y & 1 )
       Size.Y++;

    preparemap ( Size.X / 2, Size.Y * 2 );

    for ( int i = 0; i < 6; i++ )
       if ( OrgMissRec.WhoPlays & (1<< i))
          actmap->player[ convcol ( 1 << i) ].stat = Player::PlayerStatus( (OrgMissRec.PlayType>>i) & 1);
       else
          actmap->player[convcol ( 1 << i) ].stat = Player::off;

    /*  Terrain  */ 
    int Y, X;

    int missnum = 0;
    dynamic_array<int> miss;

    Uint16*         Line = new Uint16[Size.X]; 

    for (Y = 0; Y < Size.Y ; Y++) { 
      MissFile->readdata( Line, Size.X * 2); 
      for (X = 0; X < Size.X ; X++) {
         for ( int tr = 0; tr < translationTable->terraintranslation.size(); tr++ )
            if ( Line[X] == translationTable->terraintranslation[tr].first )
               Line[X] = translationTable->terraintranslation[tr].second;
         int found = 0;
         MapField* fld = getfield ( X / 2, Y * 2 + (X & 1) );
         fld->tempw = Line[X];
         fld->temp3 = 0;


         for ( int i = 0; i < translationTable->terrain2idTranslation.size(); i++ ) {
            if ( Line[X] == translationTable->terrain2idTranslation[i].BIpic ) {
                pterraintype trrn = terrainTypeRepository.getObject_byID ( translationTable->terrain2idTranslation[i].terrainid );
                int w = translationTable->terrain2idTranslation[i].weather;
                if ( trrn )
                  if ( trrn->weather[w] ) {
                     fld->typ = trrn->weather[w];
                     fld->setparams();
                     found = 1;
                  }
            }
         }

         if ( !found )
            for ( int j = 0; j < translationTable->terraincombixlat.size(); j++ )
               if ( Line[X] == translationTable->terraincombixlat[j].bigraph ) {
                  pterraintype trrn = terrainTypeRepository.getObject_byID ( translationTable->terraincombixlat[j].terrainid );
                  if ( trrn ) {
                     fld->typ = trrn->weather[translationTable->terraincombixlat[j].terrainweather];
                     ObjectType* obj = NULL;
                     if ( translationTable->terraincombixlat[j].objectid > 0 )
                        obj = objectTypeRepository.getObject_byID ( translationTable->terraincombixlat[j].objectid );
                     if ( obj )
                        fld->addobject ( obj, -1, 1 );
                     fld->setparams();
                     found = 1;
                  }
               }

         if ( !found )
            for ( int i = 0; i < terrainTypeRepository.getNum(); i++ ) {
               pterraintype trrn = terrainTypeRepository.getObject_byPos ( i );
               if ( trrn )
                  for ( int j = 0; j < cwettertypennum; j++ )
                     if ( trrn->weather[j] )
                        if ( trrn->weather[j]->bi_pict == Line[X] ) {
                           fld->typ = trrn->weather[j];
                           fld->setparams();
                           found = 1;
                        }
            }


         if ( !found ) {
            int fnd = 0;
            for ( int k = 0; k < missnum; k++ )
               if ( miss[k] == Line[X] )
                  fnd = 1;

            if ( !fnd )
               miss[missnum++] = Line[X];

         }
      }
    }

    if ( missnum ) {
       strcat ( missing, "The following terrain fields could not be found: " );
       for ( int k = 0; k < missnum; k++ ) {
          strcat ( missing, strrr ( miss[k] ));
          strcat ( missing, ", ");
       }
       strcat ( missing, "\n\n");
    }



    missnum = 0;

    //  Objekte  
    for (Y = 0; Y < Size.Y ; Y++) { 
      MissFile->readdata( Line, Size.X * 2 ); 
      
      for (X = 0; X < Size.X ; X++) {
         int found = 0;
         int newx = X / 2;
         int newy = Y * 2 + (X & 1);

         if ( Line[X] != 0xffff )
            getfield ( newx, newy )->tempw = Line[X];


         int xl = 0;
         int xlt[5];
         int trrID = -1;
         int trrWeather = 0;
         xlt[0] = Line[X];
         for ( int b = 0; b < translationTable->objecttranslate.size(); b++ )
             if ( translationTable->objecttranslate[b].BIpic == Line[X] ) {
                for ( int c = 1; c < 5; c++ )
                   if ( translationTable->objecttranslate[b].objects[c] != -1 )
                      xlt[xl++] = translationTable->objecttranslate[b].objects[c];
                trrID = translationTable->objecttranslate[b].terrainID;
                trrWeather = translationTable->objecttranslate[b].terrainWeather;
             }
         if ( xl == 0 ) {
            for ( int c = 0; c < translationTable->terraintranslation.size(); c++ )
                if ( translationTable->terraintranslation[c].first == Line[X] )
                   xlt[xl++] = translationTable->terraintranslation[c].second;

            if ( xl == 0 )
               xl = 1;
         }

         for ( int m = 0; m < xl; m++ ) {
            int found_without_force = 0;
            for ( int pass = 0; pass < 2 && !found_without_force; pass++ ) {
               for ( int i = 0; i < translationTable->object2IDtranslate.size(); i++ )
                  if ( xlt[m] == translationTable->object2IDtranslate[i].first )  {
                     ObjectType* obj = objectTypeRepository.getObject_byID ( translationTable->object2IDtranslate[i].second );
                     if ( obj ) {
                        MapField* fld = getfield ( newx, newy );
                        if ( pass == 1 || obj->getFieldModification(fld->getWeather()).terrainaccess.accessible ( fld->bdt )) {
                           fld -> addobject ( obj, 0, true );
                           found |= 1;
                           if ( pass == 0 )
                             found_without_force = 1;
                        }
                     }
               }

               if ( !(found & 1) )
                  for ( int i = 0; i < objectTypeRepository.getNum(); i++ ) {
                     ObjectType* obj = objectTypeRepository.getObject_byPos ( i );
                     if ( obj )
                        for ( int ww = 0; ww < cwettertypennum; ww++ )
                           if ( obj->weather.test(ww) )
                              for ( unsigned int j = 0; j < obj->weatherPicture[ww].images.size(); j++ )
                                 if ( obj->weatherPicture[ww].bi3pic[j] == xlt[m]  && !(found & 2)  && !( GraphicSetManager::Instance().getMode(xlt[m]) & 256) ) {
                                    MapField* fld = getfield ( newx, newy );
                                    if ( pass == 1 || obj->getFieldModification(fld->getWeather()).terrainaccess.accessible ( fld->bdt )) {
                                       fld -> addobject ( obj, 0, true );
                                       found |= 1;
                                       if ( pass == 0 )
                                         found_without_force = 1;
                                    }
                                 }
                  }
            }
         }

         if ( trrID >= 0 ) {
            pterraintype trrn = terrainTypeRepository.getObject_byID ( trrID );
            if ( trrn ) {
               if ( !trrn->weather[trrWeather] )
                  trrWeather = 0;

               if ( trrn->weather[trrWeather] ) {
                  MapField* fld = getfield ( newx, newy );
                  fld->typ = trrn->weather[trrWeather];
                  fld->setparams();
               }
            }
         }


         if ( !found  && Line[X] != 0xffff ) {
            if ( fakemap ) {
               ObjectType* o = new ObjectType;
               *o = *objectTypeRepository.getObject_byID ( 44 );
               int id = 1000000;
               while ( objectTypeRepository.getObject_byID ( id ))
                 id++;

               o->id = id;
               o->weather = 1;

               o->weatherPicture[0].resize(1);

               // loadbi3pict_double ( Line[X], &o->weatherPicture[0].images[0] );
               o->weatherPicture[0].bi3pic[0] = Line[X];

               // addobjecttype ( o );

               // getfield ( newx, newy )->addobject ( o, 0, true );

            } else {

               getfield ( newx, newy )->temp3 = 1;

               int fnd = 0;
               for ( int k = 0; k < missnum; k++ )
                  if ( miss[k] == Line[X] )
                     fnd = 1;

               if ( !fnd )
                  if ( Line[X] < 44  || Line[X] > 88 )
                     miss[missnum++] = Line[X];
            }
            
         }
      }
      
    } 
/*
    if ( missnum ) {
       strcat ( missing, "The following objects could not be found: " );
       for ( int k = 0; k < missnum; k++ ) {
          strcat ( missing, strrr ( miss[k] ));
          strcat ( missing, ", ");
       }
       strcat ( missing, "\n\n");
    }
*/

    for (Y = 0; Y < Size.Y ; Y++) { 
      MissFile->readdata ( Line, Size.X * 2 ); 
      for (X = 0; X < Size.X ; X++) {
         int tp = Line[X] & 255;
         int cl = convcol(Line[X] >> 8);
         Vehicle* eht = getunit ( tp, cl );
         if ( eht ) {
            MapField* fld = getfield ( X / 2, Y * 2 + (X & 1) );
            fld->vehicle = eht;
            fld->vehicle->xpos = xoffset + X / 2;
            fld->vehicle->ypos = yoffset + Y * 2 + (X & 1);
            stu_height ( fld->vehicle );
         }
      }
    } 
    delete[] Line;
} 
 
struct blds {
  BuildingType* bld;
  int pictnum;
  int terrainmatch;
  int objectmatch;
} ;


void       tloadBImap :: ReadSHOPPart( void )
{ 
   TSHOPHead    SHOPHead;
   TFileShop    FileShop;
//   PVehContent  PVC; 
 
   int firstmissingbuilding = 1;
 
   MissFile->seek ( Header.ShopPos );
   MissFile->readdata2 ( SHOPHead ); 
   if ( strncmp ( SHOPHead.ID, SHOPID, 4 )) {
      strcat ( missing, "\nFatal error: No Battle Isle mission; invalid ShopID\n"  );
      throw timporterror (); 
   }

   int ShopNum = 0; 
 //  int VehContNum = 0;
 //   int AINum = 0;
   for ( int I = 0; I < SHOPHead.Num ; I++ ) {
      MissFile->readdata2 ( FileShop );
      if ( FileShop.ID == 1) {   //  wenn kein ki punkt  
        if (FileShop.a.ShopType == 32) {   //  Vehicle  
           int IsVehCont = false; 
           for ( int J = 0; J < 8; J++ ) 
              if ( FileShop.a.Content.Veh[J] != 0xFFFF ) 
                 IsVehCont = true; 
           if ( IsVehCont ) {
              int Y = FileShop.Pos1 / 64;
              int X = FileShop.Pos1 % 64;
              MapField* fld = getfield ( X / 2, Y * 2 + (X & 1) );
              if ( fld->vehicle ) 
                 for ( int j = 0; j < 8; j++ ) 
                    if ( FileShop.a.Content.Veh[j] >= 0 ) {
                       Vehicle* eht = getunit ( FileShop.a.Content.Veh[j], fld->vehicle->color/8 );
                       if ( eht ) {
                          eht->xpos = xoffset + X / 2;
                          eht->ypos = yoffset + Y * 2 + (X & 1);
                          fld->vehicle->addToCargo( eht );
                       }
                    }

           } 
        }
       
        else {
           int Y = FileShop.Pos1 / 64;
           int X = FileShop.Pos1 % 64;
           int newx = X / 2;
           int newy = Y * 2 + (X & 1);
            
           MapField* fld = getfield ( newx, newy );

           dynamic_array<blds> bldlist;
           int bldlistnum = 0;

           for ( int i = 0; i < buildingTypeRepository.getNum(); i++ ) {
               BuildingType* bld  = buildingTypeRepository.getObject_byPos ( i );
               if ( bld )
                  for ( int w = 0; w < cwettertypennum; w++ ) 
                     for ( int p = 0; p < maxbuildingpicnum; p++ ) 
                           if ( bld->getBIPicture(bld->entry, w, p) == fld->tempw ) {
                              bldlist[ bldlistnum ].bld = bld;
                              int cnt = 0;
                              int terrainmatch = 0;
                              int objmatch = 0;
                              bool fail = false;
                              for ( int m = 0; m < 4; m++ )
                                 for ( int n = 0; n < 6; n++ )
                                    if ( bld->fieldExists( BuildingType::LocalCoordinate(m , n) ) ) {
                                       MapCoordinate pos = bld->getFieldCoordinate ( MapCoordinate(newx, newy), BuildingType::LocalCoordinate(m, n) );
                                       MapField* fld2 = getfield ( pos.x, pos.y );
                                       if ( !fld2 )
                                          fail = true;
                                       else {
                                          if ( bld->terrainaccess.accessible ( fld2->bdt ) > 0 )
                                             terrainmatch++;
                                          if ( bld->getBIPicture( BuildingType::LocalCoordinate(m,n), w, p) == fld2->tempw )
                                             objmatch++;
                                       }
                                       cnt++;
                                    }
                              if ( !fail ) {
                                 bldlist[ bldlistnum ].pictnum = cnt;
                                 bldlist[ bldlistnum ].terrainmatch = terrainmatch;
                                 bldlist[ bldlistnum ].objectmatch = objmatch;

                                 bldlistnum++;
                              }

                           }
           }

           int found = 0;
           if ( bldlistnum ) {
              for ( int j = 0; j < bldlistnum-1;  )
                 if ( ( bldlist[ j ].objectmatch < bldlist[ j+1 ].objectmatch ) ||
                      ( bldlist[ j ].objectmatch == bldlist[ j+1 ].objectmatch && bldlist[ j ].terrainmatch < bldlist[ j+1 ].terrainmatch) ) {
                    blds tmp = bldlist[ j ];
                    bldlist[ j ] = bldlist[ j+1 ];
                    bldlist[ j+1 ] = tmp;
                    if ( j > 0 )
                       j--;
                 } else
                   j++;

              int actpos = 0;
              while ( !found && actpos < bldlistnum ) {
                  BuildingType* bld = bldlist[actpos].bld;
                  for ( int w = 0; w < cwettertypennum; w++ ) 
                     for ( int p = 0; p < maxbuildingpicnum; p++ )  
                         if ( !found ) {
                            int match = 1;
                            for ( int m = 0; m < 4; m++ )
                               for ( int n = 0; n < 6; n++ )
                                  if ( bld->fieldExists( BuildingType::LocalCoordinate(m , n) ) ) 
                                     if ( bld->getPicture( BuildingType::LocalCoordinate(m , n), w, p ).valid() ) {
                                        MapCoordinate pos = bld->getFieldCoordinate ( MapCoordinate(newx, newy), BuildingType::LocalCoordinate(m, n) );
                                        MapField* fld2 = getfield ( pos.x, pos.y );
                                        if ( fld2->tempw != bld->getBIPicture( BuildingType::LocalCoordinate(m , n), w, p ))
                                           match = 0;
                                     }
                            if ( match )
                               found = 1;
                         }
                  actpos++;
              }
              actpos--;

              if ( found ) {
                 putbuilding ( actmap, MapCoordinate(xoffset + newx, yoffset + newy), 0, bldlist[actpos].bld, bldlist[actpos].bld->construction_steps - 1, 1 );
                 if ( fld->building ) {
                    
                    for ( int m = 0; m < 4; m++ )
                       for ( int n = 0; n < 6; n++ )
                          if ( fld->building->getPicture( BuildingType::LocalCoordinate(m , n) ).valid() ) {
                             fld->building->getField ( BuildingType::LocalCoordinate(m, n) )->temp3 = 0;
                          }

                       /*
                             if ( field->object )
                                for ( int o = 0; o < field->object->objnum; o++ )
                                   field->removeobject ( field->object->object[o]->typ );
                          }
                     */
                     found = 255;
                 } else
                     found = 254;
              } 
           } 

           if ( fld->building ) {

//              if ( battleisleversion == 3 )
              const char* shopStr = GetStr( ShopNum + 2,16);
              if ( shopStr )
                 fld->building->name = shopStr;   /*  bi3 macht das so  */ 
              /*
              else
                 ArrShop.NameStr = GetStr(FileShop.Name, 16);    //  bi2 wahrscheinlich so
                 */
              int newcol = convcol ( FileShop.a.Owner );
              if ( newcol != fld->building->color/8 )
                 fld->building->convert ( newcol );

              for ( int j = 0; j < 16; j++ ) {
                 Vehicle* eht = getunit ( FileShop.a.Content.All[j], fld->building->color/8 );
                 if ( eht ) {
                    fld->building->addToCargo( eht );
                    eht->xpos = newx;
                    eht->ypos = newy;
                 }
              }

              int prodnum = 0;
              for ( int k= 0; k < 4; k++ ) {
                 VehicleType* vt = getvehicletype ( FileShop.a.Produce[k] );
                 if ( vt ) {
                    int fnd = 0;
                    for ( int l = 0; l < prodnum; l++ )
                       if ( fld->building->getProduction()[l] == vt )
                          fnd++;

                    if ( !fnd )
                       if ( fld->building->typ->vehicleFit( vt ))
                          fld->building->addProductionLine( vt );
                 }
              }
              for ( int l = 0; l < 64; l++ )
                 if ( OrgMissRec.StdProd[l] & 3 ) {
                    VehicleType* vt = getvehicletype ( l );
                    if ( vt ) {
                       int fnd = 0;
                       for ( int l = 0; l < prodnum; l++ )
                          if ( fld->building->getProduction()[l] == vt )
                             fnd++;
                       if ( !fnd )
                           if ( fld->building->typ->vehicleFit( vt ))
                              fld->building->addProductionLine( vt );
                    }
                 }
              fld->building->bi_resourceplus.energy = energyfactor * FileShop.a.EP;
              fld->building->bi_resourceplus.material = materialfactor * FileShop.a.MP;
              fld->building->bi_resourceplus.fuel = fuelfactor * FileShop.a.EP;

              fld->building->actstorage.energy = energyfactor * FileShop.a.E;
              fld->building->actstorage.material = materialfactor * FileShop.a.M;
              fld->building->actstorage.fuel = fuelfactor * FileShop.a.E;

           } else {
                 if ( found == 254 ) {
                    char tmp[100];
                    int obj = fld->tempw;
                    sprintf( tmp, "The building at position %d / %d using pic #%d could not be set\n", newx, newy, obj );
                    strcat ( missing, tmp );
                 } else {
                    if ( firstmissingbuilding ) {
                       strcat ( missing, "The buildings at the following coordinates could not be found:\n " );
                       firstmissingbuilding = 0;
                    }
                    char tmp[100];
                    int obj = fld->tempw;
                    sprintf( tmp, "%d / %d using pic #%d\n", newx, newy, obj );
                    strcat ( missing, tmp );
                 }

           }
           ShopNum++;

        }
      } 
      
   } 
//   if ( TPWMtextfile ) 
//      strcat ( missing, "\nThe names of the buildings could not be read because the text file is TPWM encoded. Please decode it first if you want to keep the names of the shops !\n");
   

   int missnum = 0;
   dynamic_array<int> miss;

   for ( int y = 0; y < actmap->ysize; y++ )
      for ( int x = 0; x < actmap->xsize; x++ ) 
         if ( ::getfield(x,y)->temp3 ) {
            int m = ::getfield(x,y)->tempw;
            if ( m > 0 && m != 0xffff ) {
               int fnd = 0;
               for ( int k = 0; k < missnum; k++ )
                  if ( miss[k] == m )
                     fnd = 1;

               if ( !fnd )
                  miss[missnum++] = m;
               
            }
         }
      
     
   if ( missnum ) {
      strcat ( missing, "The following objects could not be found: " );
      for ( int k = 0; k < missnum; k++ ) {
         strcat ( missing, strrr ( miss[k] ));
         strcat ( missing, ", ");
      }
      strcat ( missing, "\n\n");
   }


} 


char* tloadBImap :: GetStr( int a, int b )
{
//   if ( TPWMtextfile )
//      return NULL;
//   else
      for ( int i = 0; i <= ShopNameList.getlength(); i++ )
         if ( ShopNameList[i].a == a && ShopNameList[i].b == b )
            return strdup ( ShopNameList[i].name );

   return NULL;
}

/* TPTC: TOCONV.PAS(303): Warning: Nested function, tok=SplitName 
    
Boolean      SplitName(char *       S,
                       char *       Name,
                       Uint16 *       R1,
                       Uint16 *       R2)
      { 
        Integer      Code; 
      
        Result = false; 
        strcpy(S,Trim(S)); 
        if (strcmp(S,"") != 0) { 
          if (S[0] != '#') return;
          val(copy(S,2,4),R1,&Code); 
          if (Code != 0) return;
          if (S[5] != ':') return;
          val(copy(S,7,4),R2,&Code); 
          if (Code != 0) return;
          if (S[10] != '{') return;
          if (S[strlen(S)-1] != '}') return;
          strcpy(S,copy(S,12,strlen(S) - 12)); 
  //           Delete(S, 1, 11);
  //        Delete(S, Length(S), 1); 
          strcpy(Name,S); 
          sbld(Result,"%b",true); 
        } 
      } 

*/    


void    tloadBImap :: ReadShopNames( char* databuf, unsigned long datasize )
{ 
   unsigned long dptr=0;
   char buf[ 1000];
   int c;      
   do {
      do {
         c = databuf[dptr++];
      } while ( c != '#' && dptr<datasize );
      if ( c == '#' ) {
         int i;
         for ( i = 0; i < 4; i++ ) {
            c = databuf[dptr++];
            if ( c < '0' || c > '9' )
               return;

            buf[i] = c;
         }
         buf[4] = 0;
         int a1 = atoi ( buf );
         if ( databuf[dptr++] != ':' )
            return;

         for ( i = 0; i < 4; i++ ) {
            c = databuf[dptr++];
            if ( c < '0' || c > '9' )
               return;

            buf[i] = c;
         }
         buf[4] = 0;
         int a2 = atoi ( buf );

         if ( databuf[dptr++] != '{' )
            return;

         i = 0;
         do {
            c = databuf[dptr++];
            buf[i++] = c;
         } while ( c != '}' && dptr<datasize ); /* enddo */

         if ( c == '}' ) {
            buf[i-1] = 0;
            int pos = ShopNameList.getlength()+1;
            ShopNameList[pos].a = a1;
            ShopNameList[pos].b = a2;
            strcpy ( ShopNameList[pos].name , buf );
         }
      }
   } while ( dptr<datasize );
} 


void tloadBImap :: GetTXTName ( const char* path, const char* filename, char* buf )
{
    strcpy ( buf, path );
    strcat ( buf, "ger" );
    strcat ( buf, pathdelimitterstring );
    strcat ( buf, filename );
    strcpy ( &buf[ strlen ( buf ) - 3], "txt" );
    if ( exist ( buf )) 
       return;

    strcpy ( buf, path );
    strcat ( buf, "eng" );
    strcat ( buf, pathdelimitterstring );
    strcat ( buf, filename );
    strcpy ( &buf[ strlen ( buf ) - 3], "txt" );
    if ( exist ( buf ))
       return;

    buf[0] = 0;
    return;
}
  
void tloadBImap :: LoadTXTFile ( char* filename )
{

   FILE* fp = fopen ( filename, "rb" );
   if ( !fp )
      return;

   // get file size
   fseek(fp, 0, SEEK_END);
   unsigned long txtsize=ftell(fp);
   fseek(fp, 0, SEEK_SET);

   // databuffer which holds the textdata 
   char *txtbuffer=(char *)malloc(txtsize+10000);
   memset(txtbuffer, 0, txtsize+10000);

   char buf[1000];
   if ( fread ( buf, 1, 4, fp ) != 4)
      throw treadafterend( filename );
   
   if ( strncmp ( buf, "TPWM",4) == 0  ) {
      // The file is compressed.
//      unsigned long tpwmsize=txtsize; // store the size of the compressed file.
      if ( fread(&txtsize, 4, 1, fp) != 1 ) // this is the uncompressed size.
         throw treadafterend( filename );
      
      unsigned long outptr=0;
      txtbuffer=(char *)realloc(txtbuffer, txtsize+10000);

      // the tpwm algorithm is quite simple:
      // you have datablocks containing 1 codebyte and 8 values.
      // the codebyte determines whether to copy a single char or
      // to repeat a previous block. Each bit in the codebyte marks
      // the meaning (and size) of one value. The MSB is linked to the 
      // first value and the LSB to the last one.
      // A cleared bit means: the value is 1 byte sized and gets copied directly.
      // A set bit means: the value is a 2 byte pair and is interpreted as follows:
      // The lower nibble (bits 0-3) in the 1st byte is the length of the block +3 
      // (thus giving a min. block length of 3 and a max. of 18) and the second byte
      // plus the high nibble of the first byte shifted left by 4 bits is an offset
      // in reverse direction into the already decompressed data (thus giving a range
      // from offset to offset-4095.
      // or simply look at the code. could be easier to understand ;-)
      
      while (outptr<txtsize)
      {
         char code=0;
         char inbuf[16]; // worst case buffer usage is 8*2
         memset(inbuf,0,16);
         int inptr=0;
         if ( fread(&code, 1, 1, fp) != 1 )
            throw treadafterend( filename );
         char bitsset=((code>>7)&1)+((code>>6)&1)+((code>>5)&1)+((code>>4)&1)+((code>>3)&1)+((code>>2)&1)+((code>>1)&1)+((code>>0)&1);
         
         int toread = min(8+bitsset, int(txtsize-outptr));
         fread(&inbuf, toread, 1, fp);
   
         for (int i=0; (i<8)&&(outptr<txtsize); i++)
         {
            if ((code&(0x80>>i))==0)
            {
               // straight copy
               txtbuffer[outptr]=inbuf[inptr];
               outptr++; inptr++;
            } else {
               // repeat
               char d0,d1;
	       d0=inbuf[inptr]; inptr++;
	       d1=inbuf[inptr]; inptr++;
               int blklen=(d0&0xf)+3;
               int blkofs=((d0&0xf0)<<4)+d1;
               for (int j=0; (j<blklen)&&(outptr<txtsize); j++)
               {
                  txtbuffer[outptr]=txtbuffer[outptr-blkofs];
                  outptr++;
               }
            }
         }         
      }
   } else {
      // The file is uncompressed.
      if ( fread(txtbuffer, txtsize, 1, fp) != 1 )
         throw treadafterend( filename );
   }

    txtbuffer[txtsize]=0;

   fclose ( fp );

   ReadShopNames( txtbuffer, txtsize );

   free(txtbuffer);

   /*
   LevelDescription:= GetStr(0, 0);
      LevelName:= GetStr(1, 24);

      P:= Pos('|', LevelDescription);
      if (Length(LevelDescription)> 0) and (P<> 0) then begin
        SavePlayerNames:= true;
        S:= Copy(LevelDescription, P, Length(LevelDescription)-P+1);
        Delete(LevelDescription, P, Length(LevelDescription)-P+1);
        Delete(S, 1, 1);
        for I:= 0 to 6 do begin
          P:= Pos('|', S);
          if P= 0 then P:= Length(S)+ 1;
          FPlayers[I].Name:= Copy(S, 1, P-1);
          Delete(S, 1, P);
          P:= Pos('|', S);
          if P= 0 then P:= Length(S)+ 1;
          try
            FPlayers[I].Color:= StrToInt(Copy(S, 1, P-1));
          except
            FPlayers[I].Color:= 0;
          end;
          Delete(S, 1, P);
        end;
      end else
        SavePlayerNames:= false;
   */
}

void tloadBImap :: LoadFromFile( const char* path, const char* AFileName, TerrainType::Weather* trrn, string* errorOutput )
{
   defaultterraintype = trrn;
   char temp[4000];
   missing = temp;
   missing[0] = 0;
   namenum = 0;
//   PShopNameItem PSNI;

    char TXTName[1000];
    bool error = false;
    try {
       GetTXTName( path, AFileName, TXTName );
   
       LoadTXTFile(TXTName);
   
       char completefilename[1000];
       strcpy ( completefilename, path );
       strcat ( completefilename, "mis" );
       strcat ( completefilename, pathdelimitterstring );
       strcat ( completefilename, AFileName );
       tn_file_buf_stream stream ( completefilename, tnstream::reading );
       MissFile = &stream;
       MissFile->readdata2( Header );
       if ( strncmp ( Header.ID, HeadID, 4 )) {
          strcat ( missing, "\nFatal error: No Battle Isle mission; invalid HeadID\n"  );
          throw timporterror (); 
       }
       ReadMISSPart();
       ReadACTNPart();
       ReadSHOPPart();
       /*
       ReadMAPPart();
       LoadDFs();
       UnPackDF(AktDFNum);
       */

       if ( GetStr ( 1, 24 ) )
          actmap->maptitle = GetStr ( 1, 24 );
       else
          actmap->maptitle.erase();

       if ( actmap->maptitle.empty() )
          actmap->maptitle = "imported BI map";

    } /* endtry */
    catch ( treadafterend ) {
       throw;
    }
    catch ( tfileerror err ) {
       strcat ( missing, "\nA fatal error occured while accessing the file " );
       strcat ( missing, err.getFileName().c_str() );
       strcat ( missing, "\n" );
       error = true;
    } /* endcatch */
    catch ( ASCexception ) {
       strcat ( missing, "\nA fatal error occured" );
       error = true;
    } /* endcatch */

    if ( !error )
       calculateallobjects( actmap );
    if ( missing[0] ) {
      if ( errorOutput )
         (*errorOutput)=missing;
      else {
         tviewanytext vat;
         vat.init ( "warning", missing );
         vat.run();
         vat.done();
      }
    }

}


ASCString defaultImportTranslationTable;

void setDefaultBI3ImportTranslationTable( const ASCString& filename )
{
   defaultImportTranslationTable = filename;
}



void importbattleislemap ( const ASCString& path, const ASCString& mapfilename, TerrainType::Weather* trrn, const ASCString& importTable, ASCString* errorOutput, bool fakemap  )
{
    GameMap* oldmap = actmap;
    actmap = NULL;
    GraphicSetManager::Instance().setActive(1);
    try {
       ImportBiMap lbim ( findTable(importTable) );
       if ( fakemap )
          lbim.____fakeMap____();
       lbim.LoadFromFile ( path.c_str(), mapfilename.c_str(), trrn, errorOutput );
       actmap->_resourcemode = 1;
       actmap->cleartemps ( 7 );
    }
    catch ( ASCexception ) {
       delete actmap;
       actmap = oldmap;
       throw;
    }
    delete oldmap;
}


void insertbattleislemap ( int x, int y, const ASCString& path, const ASCString& mapfilename, const ASCString& importTable  )
{
    GraphicSetManager::Instance().setActive ( 1 );
    InsertBiMap lbim ( x, y, findTable(importTable) );
    lbim.LoadFromFile ( path.c_str(), mapfilename.c_str(), NULL, NULL );
    actmap->_resourcemode = 1;
    actmap->cleartemps ( 7 );
}


#pragma pack()

