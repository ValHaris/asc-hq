//     $Id: typen.h,v 1.100 2001-09-25 18:03:35 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.99  2001/09/24 17:22:12  mbickel
//      Fixed crash in end of turn
//      Improved documentation
//
//     Revision 1.98  2001/09/13 17:43:12  mbickel
//      Many, many bug fixes
//
//     Revision 1.97  2001/08/27 21:03:55  mbickel
//      Terraintype graphics can now be mounted from any number of PNG files
//      Several AI improvements
//
//     Revision 1.96  2001/08/26 20:55:04  mbickel
//      bin2text can now load text files too
//      LoadableItemType interface class added
//
//     Revision 1.95  2001/07/28 21:09:08  mbickel
//      Prepared vehicletype structure for textIO
//
//     Revision 1.94  2001/07/27 21:13:35  mbickel
//      Added text based file formats
//      Terraintype and Objecttype restructured
//
//     Revision 1.93  2001/07/18 18:15:52  mbickel
//      Fixed: invalid sender of mails
//      Fixed: unmoveable units are moved by AI
//      Some reformatting of source files
//
//     Revision 1.92  2001/07/14 19:13:16  mbickel
//      Rewrote sound system
//      Moveing units make sounds
//      Added sound files to data
//
//     Revision 1.91  2001/07/13 12:53:01  mbickel
//      Fixed duplicate icons in replay
//      Fixed crash in tooltip help
//
//     Revision 1.90  2001/07/11 20:44:37  mbickel
//      Removed some vehicles from the data file.
//      Put all legacy units in into the data/legacy directory
//
//     Revision 1.89  2001/07/08 22:54:22  mbickel
//      Added version checking for coordinate loading
//      Fixed crash when loading AI-savegames
//
//     Revision 1.88  2001/04/01 12:59:35  mbickel
//      Updated win32 project files to new ai file structure
//      Added viewid win32-project
//      Improved AI : production and service path finding
//
//     Revision 1.87  2001/03/30 12:43:16  mbickel
//      Added 3D pathfinding
//      some cleanup and documentation
//      splitted the ai into several files, now located in the ai subdirectory
//      AI cares about airplane servicing and range constraints
//
//     Revision 1.86  2001/02/26 12:35:34  mbickel
//      Some major restructuing:
//       new message containers
//       events don't store pointers to units any more
//       tfield class overhauled
//
//     Revision 1.85  2001/02/18 15:37:21  mbickel
//      Some cleanup and documentation
//      Restructured: vehicle and building classes into separate files
//         tmap, tfield and helper classes into separate file (gamemap.h)
//      basestrm : stream mode now specified by enum instead of int
//
//     Revision 1.84  2001/02/15 21:57:08  mbickel
//      The AI doesn't try to attack with recon units any more
//
//     Revision 1.83  2001/02/11 11:39:45  mbickel
//      Some cleanup and documentation
//
//     Revision 1.82  2001/02/06 16:27:42  mbickel
//      bugfixes, bugfixes and bugfixes
//
//     Revision 1.81  2001/02/04 21:27:00  mbickel
//      The AI status is written to savegames -> new savegame revision
//      Lots of bug fixes
//
//     Revision 1.80  2001/02/01 22:48:52  mbickel
//      rewrote the storing of units and buildings
//      Fixed bugs in bi3 map importing routines
//      Fixed bugs in AI
//      Fixed bugs in mapeditor
//
//     Revision 1.79  2001/01/28 14:04:20  mbickel
//      Some restructuring, documentation and cleanup
//      The resource network functions are now it their own files, the dashboard
//       as well
//      Updated the TODO list
//
//     Revision 1.78  2001/01/23 21:05:23  mbickel
//      Speed up of AI
//      Lot of bugfixes in AI
//      Moved Research to own files (research.*)
//      Rewrote storing of developed technologies
//      Some cleanup and documentation
//
//     Revision 1.77  2001/01/22 20:00:11  mbickel
//      Fixed bug that made savegamefrom campaign games unloadable
//      Optimized the terrainAccess-checking
//
//     Revision 1.76  2001/01/21 16:37:22  mbickel
//      Moved replay code to own file ( replay.cpp )
//      Fixed compile problems done by cleanup
//
//     Revision 1.75  2001/01/21 12:48:37  mbickel
//      Some cleanup and documentation
//
//     Revision 1.74  2001/01/04 15:14:09  mbickel
//      configure now checks for libSDL_image
//      AI only conquers building that cannot be conquered back immediately
//      tfindfile now returns strings instead of char*
//
//     Revision 1.73  2000/12/31 15:25:26  mbickel
//      The AI now conqueres neutral buildings
//      Removed "reset password" buttons when starting a game
//
//     Revision 1.72  2000/12/28 16:58:39  mbickel
//      Fixed bugs in AI
//      Some cleanup
//      Fixed crash in building construction
//
//     Revision 1.71  2000/12/21 11:00:50  mbickel
//      Added some code documentation
//
//     Revision 1.70  2000/11/29 11:05:31  mbickel
//      Improved userinterface of the mapeditor
//      map::preferredfilenames uses now strings (instead of char*)
//
//     Revision 1.69  2000/11/29 09:40:25  mbickel
//      The mapeditor has now two maps simultaneously active
//      Moved memorychecking functions to its own file: memorycheck.cpp
//      Rewrote password handling in ASC
//
//     Revision 1.68  2000/11/15 19:28:36  mbickel
//      AI improvements
//
//     Revision 1.67  2000/11/14 20:36:45  mbickel
//      The AI can now use supply vehicles
//      Rewrote objecttype IO routines to make the structure independant of
//       the memory layout
//
//     Revision 1.66  2000/11/11 11:05:20  mbickel
//      started AI service functions
//
//     Revision 1.65  2000/11/08 19:37:40  mbickel
//      Changed the terrain types (again): "lava" now replaces "small trench"
//
//     Revision 1.64  2000/11/08 19:31:16  mbickel
//      Rewrote IO for the tmap structure
//      Fixed crash when entering damaged building
//      Fixed crash in AI
//      Removed item CRCs
//
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

#ifndef typen_h                                                                            
#define typen_h

#include <time.h>
#include <list>
#include <bitset>

#include "global.h"

#ifdef HAVE_LIMITS
 #include <limits>
#else
 #include <limits.h>
#endif


#include "pointers.h"
#include "tpascal.inc"
#include "misc.h"
#include "basestrm.h"
#include "errors.h"

#include "password.h"
#include "research.h"



#pragma pack(1)


//! A Ellipse that is used for highlighting elements of the screen during the tutorial
class  EllipseOnScreen {
   public:
      int x1, y1, x2, y2;
      int color;
      float precision;
      int active;
      EllipseOnScreen ( void ) { active = 0; };
      void paint ( void );
   };


//! The number of different weapon types
const int waffenanzahl = 8;

//! The number of different weather; Be careful: 'fog' is included here, but it is not used and has a different concept
const int cwettertypennum = 6;

//! The number of vehicle categories; Each category has its own move malus
const int cmovemalitypenum  = 18;

//! The movemalus type for a building. It is used for #SingleWeapon.targets_not_hittable .
const int cmm_building = 11;

//! The number of levels of height
const int choehenstufennum =  8;

//! The maximum number of number of different images for a building and a weather. For example, A wind power plant might have 6 different images, depending on the direction of the wind.
const int maxbuildingpicnum  = 8;

//! The number of game paramters that can be specified for each map.
const int gameparameternum = 19;

//! The maximum experience value of a #Vehicle
const int maxunitexperience = 23;

//! the number of bits that specify the terrain of a field
const int cbodenartennum = 33;

class IntRange {
     public:
           int from;
           int to;
           IntRange(): from(-1), to(-1) {};
           IntRange( int from_, int to_ ): from(from_), to(to_) {};
};


typedef bitset<64> BitSet;





//! The number of different resources that ASC uses
const int resourceTypeNum = 3;
//! The number of different resources that ASC uses
const int resourceNum = resourceTypeNum;

//! The Container for the three different Resources that ASC uses.
class Resources {
  public:
     int energy;
     int material;
     int fuel;

     int& resource ( int type ) {
        switch ( type ) {
           case 0: return energy;
           case 1: return material;
           case 2: return fuel;
           default: throw OutOfRange();
        }
     };

     const int& resource ( int type ) const {
        switch ( type ) {
           case 0: return energy;
           case 1: return material;
           case 2: return fuel;
           default: throw OutOfRange();
        }
     };

     Resources ( void ) : energy ( 0 ), material ( 0 ), fuel ( 0 ) {};
     Resources ( int e, int m, int f ) : energy ( e ), material ( m ), fuel ( f ) {};
     Resources& operator-= ( const Resources& res ) { energy-=res.energy; material-=res.material; fuel-=res.fuel; return *this;};
     bool operator>= ( const Resources& res ) { return energy >= res.energy && material>=res.material && fuel>=res.fuel; };
     bool operator< ( const Resources& res ) { return !(*this >= res); };
     Resources operator* ( double d );
     Resources& operator+= ( const Resources& res ) { energy+=res.energy; material+=res.material; fuel+=res.fuel; return *this;};
     enum { Energy, Material, Fuel };
     void read ( tnstream& stream );
     void write ( tnstream& stream ) const;
     void runTextIO ( PropertyContainer& pc );
};

extern Resources operator- ( const Resources& res1, const Resources& res2 );
extern Resources operator+ ( const Resources& res1, const Resources& res2 );


/** A mathematical matrix that can be multiplied with a #Resources instance (which is mathematically a vector) 
    to form a new #Resources vector. */
class ResourceMatrix {
           float e[resourceTypeNum][resourceTypeNum];
        public:
           ResourceMatrix ( const float* f );
           Resources operator* ( const Resources& r ) const;
};





//! the image for a terraintype ( #tterraintype ) that is shown on the small map
struct tquickview {
   struct {
      char p1;
      char p3[3][3];
      char p5[5][5];
   } dir[8];
};


/*
  //! an image, which may either be independant or part of a graphic set
  struct thexpic {
    void* picture;
    //! the position of the image in a graphic set. < 0 if it is seperate pictureand not from a graphic set
    int   bi3pic;

    //! is the image a flipped version of an image from the graphic set. Bit 0 : flipped horizontally, bit 1: flipped vertically
    int   flip;
  };
*/

typedef struct teventstore* peventstore;
struct teventstore {
    int          num;
    peventstore  next;
    int      eventid[256];
    int      mapid[256];
};




class  Message {
   public:
     int from;      // BM ; Bit 9 ist system
     int to;        // BM
     time_t time;
     ASCString text;
     int id;
     int runde;  //  Zeitpunkt des abschickens
     int move;   //  "
     Message ( pmap spfld );

     /** Constructor.
         \param msg      The message text
         \param gamemap  The map that this message is currently being played
         \param rec      The receipient. Bitmapped: each bit one player
         \param from     The sender. Bitmapped too! 512 = system
     */
     Message ( const ASCString& msg, pmap gamemap,int rec, int from = 512 );  // fuer Meldungen vom System
};

//! A list that stores pointers, but deletes the objects (and not only the pointers) on destruction
template <class T> class PointerList : public list<T> {
   public:
     ~PointerList() {
        for ( iterator it=begin(); it!=end(); it++ )
            delete *it;
     };
};


typedef PointerList<Message*> MessageContainer;
typedef list<Message*> MessagePntrContainer;

//! the time in ASC, measured in turns and moves
union tgametime {
  struct { signed short move, turn; }a ;
  int abstime;
};


//! Coordinate on the twodimensional map
class MapCoordinate {
         public:
            int x;
            int y;
            MapCoordinate ( ) : x(-1), y(-1 ) {};
            MapCoordinate ( int _x, int _y) : x(_x), y(_y) {};
            bool operator< ( const MapCoordinate& mc ) const { return y < mc.y || ( y == mc.y && x < mc.x );};
            bool operator== ( const MapCoordinate& mc ) const { return y == mc.y && x == mc.x;};
            void write( tnstream& stream ) const { stream.writeInt ( 3000 ); stream.writeInt ( x ); stream.writeInt ( y); };
            void read( tnstream& stream ) {
               int vers = stream.readInt ( );
               if ( vers == 3500 )
                  fatalError ( " MapCoordinate::read - data was written by MapCoordinate3D ");

               x = stream.readInt ( );
               y = stream.readInt ( );
            };
            bool valid() { return x >= 0 && y >= 0 ; };
      };


//! Coordinate on the map including height
class MapCoordinate3D : public MapCoordinate {
         public:
            int z;
            int getBitmappedHeight ( ) { return 1<<z; };
            int getNumericalHeight ( ) { return z; };
            // MapCoordinate3D& operator= ( const MapCoordinate& mc ) { x = mc.x; y = mc.y; z = -1 );
            MapCoordinate3D ( ) : MapCoordinate(), z(-1) {};
            MapCoordinate3D ( int _x, int _y, int _z) : MapCoordinate ( _x, _y ), z ( _z ) {};
            MapCoordinate3D ( const MapCoordinate& mc ) : MapCoordinate ( mc ), z ( -1 ) {};
            MapCoordinate3D ( const MapCoordinate& mc, int height ) : MapCoordinate ( mc ), z ( height ) {};
            bool operator== ( const MapCoordinate3D& mc ) const { return y == mc.y && x == mc.x && (z == mc.z || z == -1 || mc.z == -1);};
            void write( tnstream& stream ) const { stream.writeInt ( 3500 ); stream.writeInt ( z ); MapCoordinate::write( stream ); };
            void read( tnstream& stream ) {
               stream.readInt ( );
               z = stream.readInt ( );
               MapCoordinate::read ( stream );
            };
      };



//! An abstract base class that provides the interface for all loadable items
class LoadableItemType {
    public:
       //! The filename of the item
       ASCString filename;

       //! The filename and location on disk (including containerfiles) of the object. Can only be used for informational purposes
       ASCString location;
       
       virtual void read ( tnstream& stream ) = 0;
       virtual void write ( tnstream& stream ) const = 0;
       virtual void runTextIO ( PropertyContainer& pc ) = 0;
       virtual ~LoadableItemType() {};
};




class tevent {
  public:
    union {
      struct {  word         saveas; char action, num;  }a;  /*  CEventActions  */
      int      id;               /* Id-Nr      ==> Technology.Requireevent; Tevent.trigger; etc.  */
    } ;

    pascal_byte         player;   // 0..7  fuer die normalen Spieler
    // 8 wenn das Event unabh„ngig vom Spieler sofort auftreten soll

    char         description[20];

    union {
      void*    rawdata;
      char*    chardata;
      int*     intdata;
    };
    int          datasize;
    pevent       next;
    int          conn;   // wird nur im Spiel gebraucht, BIt 0 gibt an, das andere events abh„nging sind von diesem
    word         trigger[4];   /*  CEventReason  */

    class  LargeTriggerData {
      public:

       class  PolygonEntered {
         public:
           int size;
           int dummy;
           int vehiclenetworkid;
           int* data;
           int tempnwid;
           int tempxpos;
           int tempypos;
           int color;                // bitmapped
           int reserved[7];
           PolygonEntered ( void );
           PolygonEntered ( const PolygonEntered& poly );
           ~PolygonEntered ( );
        };

        tgametime time;
        int xpos, ypos;
        int networkid;
        pbuilding    building;
        int         dummy;
        int          mapid;
        int          id;
        tevent::LargeTriggerData::PolygonEntered* unitpolygon;
        int reserved[32];
        LargeTriggerData ( void );
        LargeTriggerData ( const LargeTriggerData& data );
        ~LargeTriggerData();
    };

    LargeTriggerData* trigger_data[4];

    pascal_byte         triggerconnect[4];   /*  CEventTriggerConn */
    pascal_byte         triggerstatus[4];   /*  Nur im Spiel: 0: noch nicht erf?llt
                                         1: erf?llt, kann sich aber noch „ndern
                                         2: unwiederruflich erf?llt
                                         3: unerf?llbar */
    tgametime     triggertime;     // Im Karteneditor auf  -1 setzen !!
    // Werte ungleich -1 bedeuten automatisch, dass das event bereits erf?llt ist und evt. nur noch die Zeit abzuwait ist

    struct {
      int turn;
      int move;   // negative Zahlen SIND hier zul„ssig !!! 
    } delayedexecution;

    /* Funktionsweise der verzoegerten Events: 
       Sobald die Trigger erf?llt sind, wird triggertime[0] ausgef?llt. Dadurch wird das event ausgeloest,
       sobald das Spiel diese Zeit erreicht ist, unabh„ngig vom Zustand des mapes 
       ( Trigger werden nicht erneut ausgewertet !)
    */
    tevent ( void );
    tevent ( const tevent& event );
    ~tevent ( void );
};

/*
  struct teventact { 
    union { 
      struct {  word         saveas, action;  }a;  // Id-Nr   ==> Technology.Requireevent; Tevent.trigger; etc.
      int      ID;    //   CEventActions  
    };
  };
*/


  /*  Datenaufbau des triggerData fieldes: [ hi 16 Bit ] [ low 16 Bit ] [ 32 bit Integer ] [ Pointer ]      [ low 24 Bit       ]  [ high 8 Bit ]
     'turn/move',                            move           turn
     'building/unit     ',Kartened/Spiel                                                   PBuilding/Pvehicle
                          disk               ypos           xpos
     'technology researched',                                             Tech. ID
     'event',                                                             Event ID
     'tribut required'                                                                                         Hoehe des Tributes      Spieler, von dem Tribut gefordert wird 
     'all enemy *.*'                                                      Bit 0: alle nicht allierten
                                                                          Bit 1: alle, die ?ber die folgenden Bits festgelegt werden, ob alliiert oder nicht
                                                                            Bit 2 : Spieler 0
                                                                            ...
                                                                            Bit 9 : Spieler 7

     'unit enters polygon'  pointer auf teventtrigger_polygonentered

     der Rest benoetigt keine weiteren Angaben
    */ 



     /*  DatenAufbau des Event-Data-Blocks:

      TLosecampaign, TEndCampaign, TWeatherchangeCompleted
                 benoetigen keine weiteren Daten


      TNewTechnologyEvent :
                 data = NULL;
                 SaveAs = TechnologyID;
              Gilt fuer researched wie auch available

      TMessageEvent
             Data = NULL;
             SaveAs: MessageID des Abschnittes in TextDatei

      TNextMapEvent:
             Data = NULL;
             saveas: ID der n„chsten Karte;

      TRunScript+NextMapEvent:
             Data = pointer auf Dateinamen des Scriptes ( *.scr );
             saveas: ID der n„chsten Karte;

      TeraseEvent:
             data[0] = ^int
                       ID des zu loeschenden Events
             data[1] = mapid

      Tweatherchange            ( je ein int , alles unter Data )
              wetter            ( -> cwettertypen , Wind ist eigene eventaction )
              fieldadressierung      ( 1: gesamtes map     )
                      ³              ( 0: polygone               )
                      ³              
                      ÃÄÄÄÄÄ 0 ÄÄÄ>  polygonanzahl
                      ³                   ÃÄÄ   eckenanzahl
                      ³                             ÃÄÄ x position
                      ³                                 y position
                      ³
                      ³
                      ÀÄÄÄÄÄ 1 ÄÄÄ|  

      Twindchange
              intensit„t[3]         ( fuer tieffliegend, normalfliegend und hochfliegend ; -1 steht fuer keine Aenderung )
              Richtung[3]           ( dito )


      Tmapchange               ( je ein int , alles unter Data )        { wetter wird beibehalten ! }
            numberoffields ( nicht die Anzahl fielder insgesamt, 
               ÃÄÄ>  bodentypid
                     drehrichtung
                     fieldadressierung   ( wie bei tweatherchange )


      Treinforcements        ( alles unter DATA )
             int num      // ein int , der die Anzahl der vehicle angibt. Die vehicle, die ein Transporter geladen hat, werden NICHT mitgez„hlt.
                  ÃÄÄÄÄ > die vehicle, mit tspfldloaders::writeunit in einen memory-stream geschrieben. 


      TnewVehicleDeveloped
            saveas  = ID des nun zur Verf?gung stehenden vehicletypes


      Tpalettechange
           Data =  Pointer auf String, der den Dateinamen der Palettendatei enth„lt.

      Talliancechange
           Data : Array[8][8] of int                      // status der Allianzen. Sollte vorerst symetrisch bleiben, also nur jeweils 7 Werte abfragen.
                                                             Vorerst einfach Zahlwerte eingeben.
                                                             256 steht fuer unver„ndert,
                                                             257 fuer umkehrung

      TGameParameterchange    
           int nummer_des_parameters ( -> gameparametername[] )
           int neuer_wert_des_parameters

      Ellipse
           int x1 , y1, x2, y2, x orientation , y orientation


    Wenn Data != NULL ist, MUss datasize die Groesse des Speicherbereichs, auf den Data zeigt, beinhalten.

 */





/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/// Even more miscellaneous structures...
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////


typedef struct tguiicon* pguiicon ;
struct tguiicon { 
  void*      picture[2]; 
  char         txt[31]; 
  unsigned char         id; 
  char         key[4];
  word         realkey[4]; 
  unsigned char         order; 
};


struct ticons { 
   struct { 
     void      *pfeil1, *pfeil2; 
   } weapinfo; 
   void*      statarmy[3]; 
   void*      height[8];      // fuer vehicleinfo - DLG-Box
   void*        height2[3][8];  // fuer vehicleinfo am map
   void*        player[8];      // aktueller Spieler in der dashboard: FARBE.RAW 
   void*        allianz[8][3];  // Allianzen in der dashboard: ALLIANC.RAW 
   void*        diplomaticstatus[8]; 
   void*        selectweapongui[12];
   void*        selectweaponguicancel;
   void*        unitinfoguiweapons[13];
   void*        experience[maxunitexperience+1];
   void*        wind[9];
   void*        windarrow;
   void*        stellplatz;
   void*        guiknopf;   // reingedr?ckter knopf
   void*        computer;
   void*        windbackground;
   void*        smallmapbackground;
   void*        weaponinfo[5];
   void*        X;
   struct {
     struct       {
         void* active;
         void* inactive;
         void* repairactive;
         void* repairinactive;
         void* movein_active;
         void* movein_inactive;
     } mark;
     struct       {
         struct {
           void* start;
           void* active;
           void* inactive;
         } netcontrol;
         struct {
           void* start;
           void* button;
           void* buttonpressed;
           void* schieber[4];
           void* schiene;
         } ammoproduction;
         struct {
           void* start;
         } resourceinfo;
         struct {
           void* start;
         } windpower;
         struct {
           void* start;
         } solarpower;
         struct {
           void* start;
           void* button;
           void* buttonpressed;
           void* schieber[4];
           void* schiene;
           void* schieneinactive;
           void* singlepage[2];
           void* plus[2];
           void* minus[2];
         } ammotransfer;
         struct {
           void* start;
           void* button[2];
           void* schieber;
         } research;
         struct {
           void* start;
           // void* button[2];
           void* schieber;
         } conventionelpowerplant;
         struct {
           void* start;
           void* height1[8];
           void* height2[8];
           void* repair;
           void* repairpressed;
           void* block;
         } buildinginfo;
         struct {
           void* start;
                  void* zeiger;
           void* button[2];
           void* resource[2];
           void* graph;
           void* axis[3];
           void* pageturn[2];
         
           void* schieber;
         } miningstation;
         struct {
           void* start;
                  void* zeiger;
           void* schieber;
         } mineralresources;
         struct {
           void* start;
           void* height1[8];
           void* height2[8];
           void* sum;
         } transportinfo;
     } subwin;
     union {
        void* sym[11][2];
        struct {
          void*  ammotransfer[2];
          void*  research[2];
          void*  resourceinfo[2];
          void*  netcontrol[2];
          void*  solar[2];
          void*  ammoproduction[2];
          void*  wind[2];
          void*  powerplant[2];
          void*  buildinginfo[2];
          void*  miningstation[2];
          void*  transportinfo[2];
          void*  mineralresources[2];
        } a;
     } lasche;
   
     void* tabmark[2];
     void* container_window;
   } container;
   struct {
     void* bkgr;
     void* orgbkgr;
   } attack;
   void*        pfeil2[8];     // beispielsweise fuer das Mouse-Scrolling 
   void*        mousepointer;
   void*        fieldshape;
   void*        hex2octmask;
   void*        mapbackground;
   void*        mine[8]; // explosive mines
   struct {                  
               void*     nv8;
               void*     va8;
               void*     fog8;
               #ifndef HEXAGON
               void*     nv4[4];
               void*     va4[4];
               void*     fog4[4];
               void*     viereck[256];
               #endif
    } view;  
}; 





/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/// Structure field naming constants 
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////


#define cminenum 4
extern const char*  cminentypen[cminenum] ;
extern const int cminestrength[cminenum]  ;
 #define cmantipersonnelmine 1  
 #define cmantitankmine 2  
 #define cmmooredmine 3  
 #define cmfloatmine 4  



enum { capeace, cawar, cawarannounce, capeaceproposal, canewsetwar1, canewsetwar2, canewpeaceproposal, capeace_with_shareview };
enum { visible_not, visible_ago, visible_now, visible_all };


  extern const char*  cwettertypen[];

extern const char* gameparametername[ gameparameternum ];
extern const int gameparameterdefault [ gameparameternum ];
enum { cgp_fahrspur, cgp_eis, cgp_movefrominvalidfields, cgp_building_material_factor, cgp_building_fuel_factor,
       cgp_forbid_building_construction, cgp_forbid_unitunit_construction, cgp_bi3_training, cgp_maxminesonfield,
       cgp_antipersonnelmine_lifetime, cgp_antitankmine_lifetime, cgp_mooredmine_lifetime, cgp_floatingmine_lifetime,
       cgp_buildingarmor, cgp_maxbuildingrepair, cgp_buildingrepairfactor, cgp_globalfuel, cgp_maxtrainingexperience,
       cgp_initialMapVisibility };



extern  const char*  choehenstufen[8] ;
 #define chtiefgetaucht 1  
 #define chgetaucht 2  
 #define chschwimmend 4  
 #define chfahrend 8  
 #define chtieffliegend 16  
 #define chfliegend 32  
 #define chhochfliegend 64  
 #define chsatellit 128  


#define cwaffentypennum 12
 extern const char*  cwaffentypen[cwaffentypennum] ; 
 #define cwcruisemissile 0
 #define cwcruisemissileb ( 1 << cwcruisemissile )
 #define cwminen 1  
 #define cwmineb ( 1 << cwminen   )
 #define cwbombn 2  
 #define cwbombb ( 1 << cwbombn  )
 #define cwairmissilen 3  
 #define cwairmissileb ( 1 << cwairmissilen  )
 #define cwgroundmissilen 4  
 #define cwgroundmissileb ( 1 << cwgroundmissilen  )
 #define cwtorpedon 5  
 #define cwtorpedob ( 1 << cwtorpedon  )
 #define cwmachinegunn 6  
 #define cwmachinegunb ( 1 << cwmachinegunn )
 #define cwcannonn 7  
 #define cwcannonb ( 1 << cwcannonn )
 #define cwweapon ( cwcruisemissileb | cwbombb | cwairmissileb | cwgroundmissileb | cwtorpedob | cwmachinegunb | cwcannonb )
 #define cwshootablen 11  
 #define cwshootableb ( 1 << cwshootablen  )
 #define cwammunitionn 9  
 #define cwammunitionb ( 1 << cwammunitionn )
 #define cwservicen 8  
 #define cwserviceb ( 1 << cwservicen )
 extern const int cwaffenproduktionskosten[cwaffentypennum][3];  /*  Angabe: Waffentyp; energy - Material - Sprit ; jeweils fuer 5er Pack */








extern const char*  resourceNames[3];



extern  const char* cconnections[6];
 #define cconnection_destroy 1  
 #define cconnection_conquer 2  
 #define cconnection_lose 4  
 #define cconnection_seen 8
 #define cconnection_areaentered_anyunit 16
 #define cconnection_areaentered_specificunit 32
 //   conquered = You conquered sth.      
 //   lost      = an enemy conquered sth. from you


const int ceventtriggernum = 21;
extern const char* ceventtriggerconn[]; 
 #define ceventtrigger_and 1  
 #define ceventtrigger_or 2  
 #define ceventtrigger_not 4  
 #define ceventtrigger_klammerauf 8  
 #define ceventtrigger_2klammerauf 16  
 #define ceventtrigger_2klammerzu 32  
 #define ceventtrigger_klammerzu 64  
  /*  reihenfolgenpriorit„t: in der Reihenfolge von oben nach unten wird der TriggerCon ausgewertet
               AND   OR
               NOT
               (
               eigentliches event
               )
    */ 


#define ceventactionnum 21
extern const char* ceventactions[ceventactionnum]; // not bitmapped 
 enum { cemessage,   ceweatherchange, cenewtechnology, celosecampaign, cerunscript,     cenewtechnologyresearchable, 
        cemapchange, ceeraseevent,    cecampaignend,   cenextmap,      cereinforcement, ceweatherchangecomplete, 
        cenewvehicledeveloped, cepalettechange, cealliancechange,      cewindchange,    cenothing, 
        cegameparamchange, ceellipse, ceremoveellipse, cechangebuildingdamage };


extern const char*  ceventtrigger[]; 
 enum { ceventt_turn = 1 ,               ceventt_buildingconquered, ceventt_buildinglost,  ceventt_buildingdestroyed, ceventt_unitlost, 
        ceventt_technologyresearched,    ceventt_event,             ceventt_unitconquered, ceventt_unitdestroyed,     
        ceventt_allenemyunitsdestroyed,  ceventt_allunitslost,      ceventt_allenemybuildingsdestroyed, 
        ceventt_allbuildingslost,        ceventt_energytribute,     ceventt_materialtribute, ceventt_fueltribute, 
        ceventt_any_unit_enters_polygon, ceventt_specific_unit_enters_polygon, ceventt_building_seen, ceventt_irrelevant };


extern const char*  cmovemalitypes[cmovemalitypenum];


const int experienceDecreaseDamageBoundaryNum = 4;
extern const int experienceDecreaseDamageBoundaries[experienceDecreaseDamageBoundaryNum];


/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/// Constants that specify the layout of ASC
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

#define maxloadableunits 27  // Mehr vehicle d?rfen nicht in einen Transporter rein

#define guiiconsizex 49  
#define guiiconsizey 35  


#ifdef HEXAGON
 #define maxmalq 10  
 #define minmalq 10
 #define fieldxsize 48    /*  Breite eines terrainbildes  */ 
 #define fieldysize 48  
 #define fielddistx 64
 #define fielddisty 24
 #define fielddisthalfx 32

 //! the number of sides that a field has; is now fixed at 6;
 const int sidenum = 6;
#else
 #define maxmalq 12  
 #define minmalq 8
 #define fieldxsize 40    /*  Breite eines terrainbildes  */ 
 #define fieldysize 39  
 #define fielddirecpictsize 800
 #define fielddistx 40
 #define fielddisty 20
 #define fielddisthalfx 20
 #define sidenum 8
#endif

#define fieldsizex fieldxsize
#define fieldsizey fieldysize

  extern const int directionangle [ sidenum ];


#define fieldsize (fieldxsize * fieldysize + 4 )
#define unitsizex 30
#define unitsizey 30
#define tanksize (( unitsizex+1 ) * ( unitsizey+1 ) + 4 )
#define unitsize tanksize


#ifdef HAVE_LIMITS

 #ifdef max
  #undef max
 #endif
 #ifdef min
  #undef min
 #endif

 #define maxint numeric_limits<int>::max()
 #define minint numeric_limits<int>::min()
 
 #define maxfloat numeric_limits<float>::max()
 #define minfloat numeric_limits<float>::min()
#else

 #define maxint INT_MAX
 #define minint INT_MIN
 
 #define maxfloat FLT_MAX
 #define minfloat FLT_MIN
#endif



/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/// Constants that define the behaviour of units and buildings
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

//! The movement decrease in percent of the total movemnt that is used for attacking; only used for units that can move after attacking
const int attackmovecost = 0;  

#define movement_cost_for_repaired_unit 24
#define movement_cost_for_repairing_unit 12
#define attack_after_repair 1       // Can the unit that is beeing repaired attack afterwards? 

#define mineputmovedecrease 8  
#define streetmovemalus 8  
#define railroadmovemalus 8  
#define searchforresorcesmovedecrease 8


#define fusstruppenplattfahrgewichtsfaktor 2  
#define mingebaeudeeroberungsbeschaedigung 80  
#define flugzeugtraegerrunwayverkuerzung 2  

#define autorepairdamagedecrease 10    // only for old units ; new one use autorepairrate

#define air_heightincmovedecrease 18  
#define air_heightdecmovedecrease 0
#define sub_heightincmovedecrease 12
#define sub_heightdecmovedecrease 12
#define helicopter_attack_after_ascent 1  // nach abheben angriff moeglich
#define helicopter_attack_after_descent 1  // nach landen angriff moeglich
#define helicopter_landing_move_cost 16   // zus„tzlich zu den Kosten fuer das Wechseln der Hoehenstufe 
#define weaponpackagesize 5

#define trainingexperienceincrease 2

#define brigde1buildcostincrease 12       // jeweils Basis 8; flaches Wasser
#define brigde2buildcostincrease 16       // jeweils Basis 8; mitteltiefes Wasser
#define brigde3buildcostincrease 36       // jeweils Basis 8; tiefes Wasser


#define lookintoenemytransports false  
#define lookintoenemybuildings false  

#define recyclingoutput 2    /*  Material div RecyclingOutput  */ 
#define destructoutput 5
#define nowindplanefuelusage 1      // herrscht kein Wind, braucht ein Flugzeug pro Runde soviel Sprit wie das fliegend dieser Anzahl fielder
  //   #define maxwindplainfuelusage 32   // beim nextturn: tank -= fuelconsumption * (maxwindplainfuelusage*nowindplainfuelusage + windspeed) / maxwindplainfuelusage     
#define maxwindspeed 128          // Wind der St„rke 256 legt pro Runde diese Strecke zur?ck: 128 entspricht 16 fieldern diagonal !


#define generatortruckefficiency 2  // fuer jede vehicle Power wird soviel Sprit gebraucht !

#define researchenergycost 512      // fuer 1000 researchpoints wird soviel energie benoetigt.
#define researchmaterialcost 200    //                                     material
#define researchcostdouble 10000    // bei soviel researchpoints verdoppeln sich die Kosten
#define minresearchcost 0.5
#define maxresearchcost 4
#define airplanemoveafterstart 12
#define airplanemoveafterlanding (2*minmalq - 1 )

#define mine_movemalus_increase 50   // percent

#define tfieldtemp2max 255
#define tfieldtemp2min 0


#define cnet_storeenergy        0x001           // es wird garantiert,  dass material immer das 2 und fuel das 4 fache von energy ist
#define cnet_storematerial      0x002
#define cnet_storefuel          0x004

#define cnet_moveenergyout      0x008
#define cnet_movematerialout    0x010
#define cnet_movefuelout        0x020

#define cnet_stopenergyinput    0x040
#define cnet_stopmaterialinput  0x080
#define cnet_stopfuelinput      0x100

#define cnet_stopenergyoutput   0x200
#define cnet_stopmaterialoutput 0x400
#define cnet_stopfueloutput     0x800


#define resource_fuel_factor 100         // die im boden liegenden Bodensch„tzen ergeben effektiv soviel mal mehr ( bei Bergwerkseffizienz 1024 )
#define resource_material_factor 100     // "

#define destruct_building_material_get 3 // beim Abreissen erh„lt man 1/3 des eingesetzten Materials zur?ck
#define destruct_building_fuel_usage 10  // beim Abreissen wird 10 * fuelconsumption Fuel fuelconsumptiont


#define dissectunitresearchpointsplus  2    // Beim dissectn einer vehicle wird der sovielte Teil der Researchpoints jeder unbekannten Technologie gutgeschrieben

#define dissectunitresearchpointsplus2 3    // Beim dissectn einer vehicle wird der sovielte Teil der Researchpoints jeder unbekannten Technologie gutgeschrieben.
  // fuer die Technologie existieren aber bereits von einem anderen sezierten vehicletype gutschriften.

//! The maximum number of fields a mining station can extract mineral resources from; Counted from its entry
const int maxminingrange = 10;

//! The weight of 1024 units of aa Resource 
extern const int resourceWeight[ resourceTypeNum ];

#define objectbuildmovecost 16  // vehicle->movement -= (8 + ( fld->movemalus[0] - 8 ) / ( objectbuildmovecost / 8 ) ) * kosten des obj


extern const int csolarkraftwerkleistung[];

//! The number of netcontrol actions ; see #cnetcontrol 
const int cnetcontrolnum  = 12;

//! The actions that can be used to control a ResourceNet
extern const char* cnetcontrol[cnetcontrolnum];

extern const char* cgeneralnetcontrol[];

#define unspecified_error 9999

#define greenbackgroundcol 156

#pragma pack()

#endif
