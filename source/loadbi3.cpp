/*! \file loadbi3.cpp
    \brief Importing maps from Battle Isle
*/


//     $Id: loadbi3.cpp,v 1.71 2002-11-23 18:36:24 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.70  2002/10/29 19:00:27  mbickel
//      Fixed crashes when inserting a BI3 map
//
//     Revision 1.69  2002/10/06 15:44:40  mbickel
//      Completed inheritance of .asctxt files
//      Speed up of replays
//
//     Revision 1.68  2002/03/19 19:29:31  mbickel
//      Changed BI3 map import to work in map2pcx without user intervention
//
//     Revision 1.67  2001/12/19 17:16:29  mbickel
//      Some include file cleanups
//
//     Revision 1.66  2001/12/19 11:46:35  mbickel
//      Applied patches from Michael Moerz:
//       - 64bit cleanup of demount.cpp, mount.cpp
//       - removal of #ifdef converter and moved conveter specific functions
//         to independant lib
//
//     Revision 1.65  2001/12/14 10:20:05  mbickel
//      Cleanup and enhancements to configure.in
//      Removed last remains of octagonal version from source files
//
//     Revision 1.64  2001/11/15 20:16:01  mbickel
//      Added a new BI3 import table
//      Fixed movement reduction changing height of a nearly empty unit and
//         refuelling it afterwards
//      Better error message for "ID not found"
//
//     Revision 1.63  2001/10/22 18:53:39  mbickel
//      Fixed BI3 map translation errors: wrong order of table evaluation
//
//     Revision 1.62  2001/10/21 20:18:39  mbickel
//      Fixed non-empty table problem with BI3 map import
//      Added ini parameter to specify wood behaviour
//
//     Revision 1.61  2001/10/21 13:16:59  mbickel
//      Cleanup and documentation
//
//     Revision 1.60  2001/10/16 15:33:03  mbickel
//      Added icons to data
//      ASC displays icons
//      Fixed crash when building icons
//      Removed assembled datafiles
//
//     Revision 1.59  2001/10/12 00:32:21  mbickel
//      Fixed crash in loadbi3
//
//     Revision 1.58  2001/10/11 10:41:06  mbickel
//      Restructured platform fileio handling
//      Added map archival information to mapeditor
//
//     Revision 1.57  2001/10/08 14:12:20  mbickel
//      Fixed crash in AI
//      Speedup of AI
//      Map2PCX improvements
//      Mapeditor usability improvements
//
//     Revision 1.56  2001/10/06 11:30:42  mbickel
//      Fixed bug in gametime calculation
//
//     Revision 1.55  2001/10/02 14:06:28  mbickel
//      Some cleanup and documentation
//      Bi3 import tables now stored in .asctxt files
//      Added ability to choose amoung different BI3 import tables
//      Added map transformation tables
//
//     Revision 1.54  2001/09/13 17:43:12  mbickel
//      Many, many bug fixes
//
//     Revision 1.53  2001/08/26 20:55:04  mbickel
//      bin2text can now load text files too
//      LoadableItemType interface class added
//
//     Revision 1.52  2001/08/09 19:28:22  mbickel
//      Started adding buildingtype text file functions
//
//     Revision 1.51  2001/08/09 15:58:59  mbickel
//      Some usability improvements in the map editor
//      More flexible BI3 map import
//      Better textfile error messages
//
//     Revision 1.50  2001/08/09 14:50:37  mbickel
//      Added palette.map to data directory
//      Improved usability of terrain selection in mapeditor
//      New terrain translation in bi3 import function
//      Better error messages in text parser
//      Better error message: duplicate ID
//
//     Revision 1.49  2001/08/09 10:28:23  mbickel
//      Fixed AI problems
//      Mapeditor can edit a units AI parameter
//
//     Revision 1.48  2001/07/28 21:09:08  mbickel
//      Prepared vehicletype structure for textIO
//
//     Revision 1.47  2001/07/28 11:19:12  mbickel
//      Updated weaponguide
//      moved item repository from spfst to itemrepository
//
//     Revision 1.46  2001/07/27 21:13:35  mbickel
//      Added text based file formats
//      Terraintype and Objecttype restructured
//
//     Revision 1.45  2001/05/17 20:10:22  mbickel
//      Fixed: mapeditor was unable to load maps
//      Removed debugging output from bi3 map loader
//
//     Revision 1.44  2001/04/30 11:41:25  mbickel
//      Fixed crash when importing BI maps due to different structure packing
//
//     Revision 1.43  2001/02/18 15:37:13  mbickel
//      Some cleanup and documentation
//      Restructured: vehicle and building classes into separate files
//         tmap, tfield and helper classes into separate file (gamemap.h)
//      basestrm : stream mode now specified by enum instead of int
//
//     Revision 1.42  2001/02/11 20:40:29  mbickel
//      Fixed compilation problems with gcc
//
//     Revision 1.41  2001/02/11 11:39:37  mbickel
//      Some cleanup and documentation
//
//     Revision 1.40  2001/02/01 22:48:42  mbickel
//      rewrote the storing of units and buildings
//      Fixed bugs in bi3 map importing routines
//      Fixed bugs in AI
//      Fixed bugs in mapeditor
//
//     Revision 1.39  2001/01/31 14:52:39  mbickel
//      Fixed crashes in BI3 map importing routines
//      Rewrote memory consistency checking
//      Fileselect dialog now uses ASCStrings
//
//     Revision 1.38  2001/01/28 23:00:40  mbickel
//      Made the small editors compilable with Watcom again
//
//     Revision 1.37  2001/01/28 20:42:13  mbickel
//      Introduced a new string class, ASCString, which should replace all
//        char* and std::string in the long term
//      Split loadbi3.cpp into 3 different files (graphicselector, graphicset)
//
//     Revision 1.36  2001/01/28 17:19:12  mbickel
//      The recent cleanup broke some source files; this is fixed now
//
//     Revision 1.35  2001/01/25 23:45:00  mbickel
//      Moved map displaying routins to own file (mapdisplay.cpp)
//      Wrote program to create pcx images from map files (map2pcx.cpp)
//      Fixed bug in repair function: too much resource consumption
//      AI improvements and bug fixes
//      The BI3 map import function now evaluates the player status (human/
//       computer)
//
//     Revision 1.34  2001/01/23 21:05:17  mbickel
//      Speed up of AI
//      Lot of bugfixes in AI
//      Moved Research to own files (research.*)
//      Rewrote storing of developed technologies
//      Some cleanup and documentation
//
//     Revision 1.33  2001/01/21 16:37:18  mbickel
//      Moved replay code to own file ( replay.cpp )
//      Fixed compile problems done by cleanup
//
//     Revision 1.32  2001/01/04 15:13:57  mbickel
//      configure now checks for libSDL_image
//      AI only conquers building that cannot be conquered back immediately
//      tfindfile now returns strings instead of char*
//
//     Revision 1.31  2000/12/23 13:19:46  mbickel
//      Made ASC compileable with Borland C++ Builder
//
//     Revision 1.30  2000/11/08 19:31:08  mbickel
//      Rewrote IO for the tmap structure
//      Fixed crash when entering damaged building
//      Fixed crash in AI
//      Removed item CRCs

/*
    This file is part of Advanced Strategic Command; http://www.asc-hq.de
    Copyright (C) 1994-1999  Martin Bickel  and  Marc Schellenberger

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
#include "newfont.h"
#include "basegfx.h"
#include "typen.h"
#include "buildingtype.h"
#include "vehicletype.h"
#include "misc.h"
#include "stack.h"
#include "palette.h"
#include "gameoptions.h"
#include "events.h"
#include "graphicset.h"
#include "spfst.h"
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
      char*  Name;
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

void check_bi3_dir ( void )
{
   loadbi3graphics();
}

void checkbi3dir ( void )
{
   char temp[1000];

   if ( !CGameOptions::Instance()->bi3.dir.getName() ) {
      readgameoptions();
      /*
      if ( !gameoptions.bi3.dir.getName() ) {
         gameoptions.bi3.dir = new char [300];
         gameoptions.bi3.dir[0] = 0;
      }
      */
   }

   int notfound;
   do {
      notfound = 0;

      for ( int i = 0; i < libs_to_load ; i++ ) {
		  if ( CGameOptions::Instance()->bi3.dir.getName() )
            strcpy ( temp, CGameOptions::Instance()->bi3.dir.getName() );
         else
            temp[0] = 0;

         strcat ( temp, LIBFiles[i].Name );
         
         if ( !exist ( temp ) ) {
            if ( CGameOptions::Instance()->bi3.dir.getName() )
               strcpy ( temp, CGameOptions::Instance()->bi3.dir.getName() );
            else
               temp[0] = 0;

            strcat ( temp, "LIB" );
            strcat ( temp, pathdelimitterstring );
            strcat ( temp, LIBFiles[i].Name );
            if ( !exist ( temp ) ) {
               printf("Battle Isle file %s not found !\n", temp );
               notfound++;
            }
         }
      }
     if ( notfound ) {
         if ( !graphicinitialized ) {
            char bi3path[10000];
            printf("Enter Battle Isle directory:\n" );
            scanf ( "%s", bi3path );
            if ( bi3path[ strlen ( bi3path )-1 ] != pathdelimitter )
               strcat ( bi3path, pathdelimitterstring );

            CGameOptions::Instance()->bi3.dir.setName ( bi3path );
            CGameOptions::Instance()->setChanged();
         } else {
            closegraphics();
            printf("\nplease run ASC first to create a config file !\n");
            exit(0);
         }
     }
   } while ( notfound ); /* enddo */
   battleisleversion = 3;
}


const char* getbi3path ( void )
{
	return CGameOptions::Instance()->bi3.dir.getName();
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
            };
        //! This is a special translation for the fields that must be translated to a terrain AND an additional object
        vector<Terraincombixlat> terraincombixlat;


        struct Objecttranslataion {
           int BIpic;
           int objects[4];
           int terrainID;
           int terrainWeather;
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

        ASCString filename, location;
};


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


   pc.run();

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

void readBI3translationTable ( )
{
    TextPropertyList& tpl = textFileRepository["bi3maptranslation"];
    for ( TextPropertyList::iterator i = tpl.begin(); i != tpl.end(); i++ ) {

      PropertyReadingContainer pc ( "bi3maptranslation", *i );

      Bi3MapTranslationTable* bmtt = new Bi3MapTranslationTable;
      bmtt->runTextIO ( pc );
      pc.run();

      bmtt->filename = (*i)->fileName;
      bmtt->location = (*i)->location;
      bi3ImportTables.push_back ( bmtt );
   }
}


const int fuelfactor = 120;
const int materialfactor = 390;
const int energyfactor = 390;




      char* HeadID = "MSSN";
      char* ACTNID = "ACTN";
      char* SHOPID = "SHOP";
      char* MAPID  = "MAP\0";
      char* MISSID = "MISS";



class tloadBImap {
       Bi3MapTranslationTable* translationTable;
     
       struct THeader {
         char ID[4];                            // { 'MSSN' }
         int MissPos;
         int MapPos ;
         int ShopPos;
         int ACTNPos;
         word DFNum ;
         int DFPos[32] ;                     // { Pos nach DFXX }
         int DFLength[32];                   // { +4B von DFXX }
       };

       struct TACTN {
         char ID[4];
         word XSize, YSize;
       };

       typedef word TStdProdRec[64];
       typedef char TAllianzen[6];


       struct TMISSPart {
           char ID[4];
           word Stuff1 [3];
           word NextMiss;
           char StartWeather;
           char WhoPlays;           // { jeder Mensch/Computer ein Bit }
           char PlayType;           // { jeder Computer ein Bit }
           char Recs2Win;           // { Wieviel Gewinnrecords aus map erf llt werden m ssen, damit man gewinnt (f r jeden player gleich) }
           char Stuff2[3];
           char Landscape;
           TAllianzen Allianz;      // { Wer mit wem }
           word Stuff3[65];
           TStdProdRec StdProd;
           word Stuff4[24];
       };
          
       struct TMAPHead {
              char ID[4];
       };
          
       struct TFileMap { 
              word What1;
              word Round, Move;
              char Player, What2;
              word Stuff2;
              word Zero1;
              word Nr1;
              word Nr2;
              word Nr3;
              word Zero2[11];
       };
          
       struct TSHOPHead {
              char ID[4];
              word Num;
       };
          
       typedef int TVehContent[8];
       typedef TVehContent *PVehContent;
          
          union TShopContent {
             word XY[4][4];
             word All[16];
             TVehContent Veh;
          };
       typedef TShopContent *PShopContent;
         
       typedef word TProduceRec[4];

       struct TFileShop {
         word ID;
         word Pos1;
         union  {
            struct {
               word ShopType;
               word Stuff06;  // { Zero }
               word Name;
               word Pos2;
               word E, M;
               char EP, MP ;
               TShopContent Content;
               word ShopType2;
               TProduceRec Produce;
               word Stuff60 ; // { Zero }
               word Prior;
               word Owner;
               word Stuff66[7];  // { Zero }
            } a;
            struct {
                char Zero2[6];
                word W0A;
                char Zero3[6];
                word W48;
                word W4A;
                char Zero4[2];
                word W4E;
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
       int TPWMtextfile;

       void ReadMISSPart(void);
       void ReadACTNPart(void);
       void ReadSHOPPart(void);
       void ReadShopNames( FILE* fp );
       pvehicle getunit ( int typ, int col );
       pvehicletype getvehicletype ( int typ );
       char* GetStr ( int a, int b );
       int convcol ( int c );

       TMISSPart OrgMissRec;
       void GetTXTName ( const char* path, const char* filename, char* buf );

      protected:
        char* missing;
        virtual void preparemap ( int x, int y  ) = 0;
        virtual pfield getfield ( int x, int y );

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
           virtual pfield getfield ( int x, int y );
         public:
           InsertBiMap ( int x, int y, Bi3MapTranslationTable* _translationTable ) : tloadBImap (  _translationTable ) {
             xoffset = x; 
             yoffset = y;
           };
};


pfield tloadBImap :: getfield ( int x, int y )
{
   return ::getfield ( x, y );
}

pfield InsertBiMap :: getfield ( int x, int y )
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
   int cl = log2 ( c );
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


pvehicletype  tloadBImap :: getvehicletype ( int tp )
{
   for ( int j = 0; j < vehicletypenum; j++ ) {
      pvehicletype tnk = getvehicletype_forpos ( j );
      if ( tnk )
         if ( tnk->bipicture > 0 ) 
            if ( tnk->bipicture == 1340 + tp * 2 ) 
                  return tnk;
   }


   if ( tp < bi3unitnum )
      for ( int i = 0; i < 2; i++ ) 
         if ( translateunits[tp][i] > 0 ) {
            pvehicletype tnk = getvehicletype_forid ( translateunits[tp][i] );
            if ( tnk )
               return tnk;
         }
   return NULL;
}

pvehicle tloadBImap :: getunit ( int tp, int col )
{
   if ( tp != 0xffff && tp != 0xff && col != 0xff ) {
      pvehicletype vt = getvehicletype ( tp );
      if ( vt ) {
         Vehicle* eht = new Vehicle ( vt, actmap, col );
         eht->fillMagically();
         return eht;
      }
   }

   return NULL;
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

void  stu_height ( pvehicle vehicle )
{
   char l;
   pfield fld = getfield ( vehicle->xpos, vehicle->ypos );

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

   vehicle->setMovement ( vehicle->typ->movement[log2( vehicle->height )] );
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
          actmap->player[ convcol ( 1 << i) ].stat = Player::tplayerstat( (OrgMissRec.PlayType>>i) & 1);
       else
          actmap->player[convcol ( 1 << i) ].stat = Player::off;

    /*  Terrain  */ 
    int Y, X;

    int missnum = 0;
    dynamic_array<int> miss;

    Word*         Line = new word[Size.X]; 

    for (Y = 0; Y < Size.Y ; Y++) { 
      MissFile->readdata( Line, Size.X * 2); 
      for (X = 0; X < Size.X ; X++) {
         for ( int tr = 0; tr < translationTable->terraintranslation.size(); tr++ )
            if ( Line[X] == translationTable->terraintranslation[tr].first )
               Line[X] = translationTable->terraintranslation[tr].second;
         int found = 0;
         pfield fld = getfield ( X / 2, Y * 2 + (X & 1) );
         fld->tempw = Line[X];
         fld->temp3 = 0;


         for ( int i = 0; i < translationTable->terrain2idTranslation.size(); i++ ) {
            if ( Line[X] == translationTable->terrain2idTranslation[i].BIpic ) {
                pterraintype trrn = getterraintype_forid ( translationTable->terrain2idTranslation[i].terrainid );
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
                  pterraintype trrn = getterraintype_forid ( translationTable->terraincombixlat[j].terrainid );
                  if ( trrn ) {
                     fld->typ = trrn->weather[translationTable->terraincombixlat[j].terrainweather];
                     pobjecttype obj = NULL;
                     if ( translationTable->terraincombixlat[j].objectid > 0 )
                        obj = getobjecttype_forid ( translationTable->terraincombixlat[j].objectid );
                     if ( obj )
                        fld->addobject ( obj, -1, 1 );
                     fld->setparams();
                     found = 1;
                  }
               }

         if ( !found )
            for ( int i = 0; i < terraintypenum; i++ ) {
               pterraintype trrn = getterraintype_forpos ( i );
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
                     pobjecttype obj = getobjecttype_forid ( translationTable->object2IDtranslate[i].second );
                     if ( obj ) {
                        pfield fld = getfield ( newx, newy );
                        if ( pass == 1 || obj->terrainaccess.accessible ( fld->bdt )) {
                           fld -> addobject ( obj, 0, true );
                           found |= 1;
                           if ( pass == 0 )
                             found_without_force = 1;
                        }
                     }
               }

               if ( !(found & 1) )
                  for ( int i = 0; i < objecttypenum; i++ ) {
                     pobjecttype obj = getobjecttype_forpos ( i );
                     if ( obj )
                        for ( int ww = 0; ww < cwettertypennum; ww++ )
                           if ( obj->weather.test(ww) )
                              for ( unsigned int j = 0; j < obj->weatherPicture[ww].images.size(); j++ )
                                 if ( obj->weatherPicture[ww].bi3pic[j] == xlt[m]  && !(found & 2)  && !( getActiveGraphicSet()->getMode(xlt[m]) & 256) ) {
                                    pfield fld = getfield ( newx, newy );
                                    if ( pass == 1 || obj->terrainaccess.accessible ( fld->bdt )) {
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
            pterraintype trrn = getterraintype_forid ( trrID );
            if ( trrn ) {
               if ( !trrn->weather[trrWeather] )
                  trrWeather = 0;

               if ( trrn->weather[trrWeather] ) {
                  pfield fld = getfield ( newx, newy );
                  fld->typ = trrn->weather[trrWeather];
                  fld->setparams();
               }
            }
         }


         if ( !found  && Line[X] != 0xffff ) {
            if ( fakemap ) {
               pobjecttype o = new ObjectType;
               *o = *getobjecttype_forid ( 44 );
               int id = 1000000;
               while ( getobjecttype_forid ( id ))
                 id++;

               o->id = id;
               o->weather = 1;

               o->weatherPicture[0].resize(1);

               loadbi3pict_double ( Line[X], &o->weatherPicture[0].images[0] );
               o->weatherPicture[0].bi3pic[0] = Line[X];

               addobjecttype ( o );

               getfield ( newx, newy )->addobject ( o, 0, true );

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
         pvehicle eht = getunit ( tp, cl );
         if ( eht ) {
            pfield fld = getfield ( X / 2, Y * 2 + (X & 1) );
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
  pbuildingtype bld;
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
              pfield fld = getfield ( X / 2, Y * 2 + (X & 1) );
              int nm = 0;
              if ( fld->vehicle ) 
                 for ( int j = 0; j < 8; j++ ) 
                    if ( FileShop.a.Content.Veh[j] >= 0 ) {
                       pvehicle eht = getunit ( FileShop.a.Content.Veh[j], fld->vehicle->color/8 );
                       if ( eht ) {
                          eht->xpos = xoffset + X / 2;
                          eht->ypos = yoffset + Y * 2 + (X & 1);
                          fld->vehicle->loading[nm++] = eht;
                       }
                    }

           } 
        }
       
        else {
           int Y = FileShop.Pos1 / 64;
           int X = FileShop.Pos1 % 64;
           int newx = X / 2;
           int newy = Y * 2 + (X & 1);
            
           pfield fld = getfield ( newx, newy );

           dynamic_array<blds> bldlist;
           int bldlistnum = 0;

           for ( int i = 0; i < buildingtypenum; i++ ) {
               pbuildingtype bld  = getbuildingtype_forpos ( i );
               if ( bld )
                  for ( int w = 0; w < cwettertypennum; w++ ) 
                     for ( int p = 0; p < maxbuildingpicnum; p++ ) 
                        if ( bld->w_picture[w][p][ bld->entry.x ] [ bld->entry.y ] )
                           if ( bld->bi_picture[w][p][ bld->entry.x ] [ bld->entry.y ] == fld->tempw ) {
                              bldlist[ bldlistnum ].bld = bld;
                              int cnt = 0;
                              int terrainmatch = 0;
                              int objmatch = 0;
                              bool fail = false;
                              for ( int m = 0; m < 4; m++ )
                                 for ( int n = 0; n < 6; n++ )
                                    if ( bld->getpicture( BuildingType::LocalCoordinate(m , n) ) ) {
                                       MapCoordinate pos = bld->getFieldCoordinate ( MapCoordinate(newx, newy), BuildingType::LocalCoordinate(m, n) );
                                       pfield fld2 = getfield ( pos.x, pos.y );
                                       if ( !fld2 )
                                          fail = true;
                                       else {
                                          if ( bld->terrainaccess.accessible ( fld2->bdt ) > 0 )
                                             terrainmatch++;
                                          if ( bld->bi_picture[w][p][m][n] == fld2->tempw )
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
                  pbuildingtype bld = bldlist[actpos].bld;
                  for ( int w = 0; w < cwettertypennum; w++ ) 
                     for ( int p = 0; p < maxbuildingpicnum; p++ )  
                         if ( !found ) {
                            int match = 1;
                            for ( int m = 0; m < 4; m++ )
                               for ( int n = 0; n < 6; n++ )
                                  if ( bld->w_picture[w][p][ m ] [ n ] )
                                     if ( bld->getpicture( BuildingType::LocalCoordinate(m , n) ) ) {
                                        MapCoordinate pos = bld->getFieldCoordinate ( MapCoordinate(newx, newy), BuildingType::LocalCoordinate(m, n) );
                                        pfield fld2 = getfield ( pos.x, pos.y );
                                        if ( fld2->tempw != bld->bi_picture[w][p][ m ] [ n ] )
                                           match = 0;
                                     }
                            if ( match )
                               found = 1;
                         }
                  actpos++;
              }
              actpos--;

              if ( found ) {
                 putbuilding ( MapCoordinate(xoffset + newx, yoffset + newy), 0, bldlist[actpos].bld, bldlist[actpos].bld->construction_steps - 1, 1 );
                 if ( fld->building ) {
                    
                    for ( int m = 0; m < 4; m++ )
                       for ( int n = 0; n < 6; n++ )
                          if ( fld->building->getpicture( BuildingType::LocalCoordinate(m , n) ) ) {
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

              if ( battleisleversion == 3 )
                 fld->building->name = GetStr( ShopNum + 2,16);   /*  bi3 macht das so  */ 
              /*
              else
                 ArrShop.NameStr = GetStr(FileShop.Name, 16);    //  bi2 wahrscheinlich so
                 */
              int newcol = convcol ( FileShop.a.Owner );
              if ( newcol != fld->building->color/8 )
                 fld->building->convert ( newcol );

              int unitnum = 0;
              for ( int j = 0; j < 16; j++ ) {
                 pvehicle eht = getunit ( FileShop.a.Content.All[j], fld->building->color/8 );
                 if ( eht ) {
                    fld->building->loading[unitnum++] = eht;
                    eht->xpos = newx;
                    eht->ypos = newy;
                 }
              }

              int prodnum = 0;
              for ( int k= 0; k < 4; k++ ) {
                 pvehicletype vt = getvehicletype ( FileShop.a.Produce[k] );
                 if ( vt ) {
                    int fnd = 0;
                    for ( int l = 0; l < prodnum; l++ )
                       if ( fld->building->production[l] == vt )
                          fnd++;

                    if ( !fnd )
                       if ( fld->building->typ->vehicleloadable( vt ))
                          fld->building->production [ prodnum++ ] = vt;
                 }
              }
              for ( int l = 0; l < 64; l++ )
                 if ( OrgMissRec.StdProd[l] & 3 ) {
                    pvehicletype vt = getvehicletype ( l );
                    if ( vt ) {
                       int fnd = 0;
                       for ( int l = 0; l < prodnum; l++ )
                          if ( fld->building->production[l] == vt )
                             fnd++;
                       if ( !fnd )
                          if ( prodnum < 32 )
                             if ( fld->building->typ->vehicleloadable( vt ))
                                 fld->building->production [ prodnum++ ] = vt;
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
   if ( TPWMtextfile ) 
      strcat ( missing, "\nThe names of the buildings could not be read because the text file is TPWM encoded. Please decode it first if you want to keep the names of the shops !\n");
   

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
   if ( TPWMtextfile )
      return NULL;
   else
      for ( int i = 0; i <= ShopNameList.getlength(); i++ )
         if ( ShopNameList[i].a == a && ShopNameList[i].b == b )
            return strdup ( ShopNameList[i].name );

   return NULL;
}

/* TPTC: TOCONV.PAS(303): Warning: Nested function, tok=SplitName 
    
Boolean      SplitName(char *       S,
                       char *       Name,
                       Word *       R1,
                       Word *       R2)
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


void    tloadBImap :: ReadShopNames( FILE* fp )
{ 
   char buf[ 1000];
   int c;      
   do {
      do {
         c = fgetc ( fp );
      } while ( c != '#' && c!= EOF );
      if ( c == '#' ) {
         int i;
         for ( i = 0; i < 4; i++ ) {
            c = fgetc ( fp );
            if ( c < '0' || c > '9' )
               return;

            buf[i] = c;
         }
         buf[4] = 0;
         int a1 = atoi ( buf );
         if ( fgetc ( fp ) != ':' )
            return;

         for ( i = 0; i < 4; i++ ) {
            c = fgetc ( fp );
            if ( c < '0' || c > '9' )
               return;

            buf[i] = c;
         }
         buf[4] = 0;
         int a2 = atoi ( buf );

         if ( fgetc ( fp ) != '{' )
            return;

         i = 0;
         do {
            c = fgetc ( fp );
            buf[i++] = c;
         } while ( c != '}' && c != EOF ); /* enddo */

         if ( c == '}' ) {
            buf[i-1] = 0;
            int pos = ShopNameList.getlength()+1;
            ShopNameList[pos].a = a1;
            ShopNameList[pos].b = a2;
            strcpy ( ShopNameList[pos].name , buf );
         }
      }
   } while ( c != EOF );
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
   FILE* fp = fopen ( filename, "r" );
   if ( !fp )
      return;


   char buf[1000];
   fread ( buf, 1, 4, fp );
   if ( strncmp ( buf, "TPWM",4) == 0  ) {
      TPWMtextfile = 1;
      fclose ( fp );
      return;
   }

   TPWMtextfile = 0;
   fseek ( fp, 0, SEEK_SET );

   ReadShopNames( fp );

   fclose ( fp );

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
       calculateallobjects();
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


Bi3MapTranslationTable* chooseImportTable ( )
{
  if ( bi3ImportTables.size() == 0)
     fatalError ( "No BI3 map translation tables present!");

  if ( bi3ImportTables.size() == 1)
     return bi3ImportTables[0];

  if ( defaultImportTranslationTable.empty() ) {
     vector<ASCString> strings;
     for ( int i = 0; i < bi3ImportTables.size(); i++ )
        strings.push_back ( bi3ImportTables[i]->filename );

     int t = chooseString ( "Select Import Table", strings );
     if ( t != 255 )
        return bi3ImportTables[ t ];
     else
        return bi3ImportTables[0];
  } else {
     for ( int i = 0; i < bi3ImportTables.size(); i++ )
         if ( bi3ImportTables[i]->filename.compare_ci ( defaultImportTranslationTable ) == 0 )
            return bi3ImportTables[i];

     fatalError ( "default BI3 import Table " +defaultImportTranslationTable + " not found !");

     // never executed...
     return bi3ImportTables[0];
  }
}



void importbattleislemap ( const char* path, const char* filename, TerrainType::Weather* trrn, string* errorOutput, bool fakemap  )
{
    pmap oldmap = actmap;
    actmap = NULL;
    activateGraphicSet ( 1 );
    try {
       ImportBiMap lbim ( chooseImportTable ( ) );
       if ( fakemap )
          lbim.____fakeMap____();
       lbim.LoadFromFile ( path, filename, trrn, errorOutput );
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


void insertbattleislemap ( int x, int y, const char* path, const char* filename  )
{
    activateGraphicSet ( 1 );
    InsertBiMap lbim ( x, y, chooseImportTable ( ) );
    lbim.LoadFromFile ( path, filename, NULL, NULL );
    actmap->_resourcemode = 1;
    actmap->cleartemps ( 7 );
}


#pragma pack()

