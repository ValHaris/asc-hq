//     $Id: typen.h,v 1.153 2004-08-19 19:19:52 mbickel Exp $

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

#ifndef typenH
#define typenH

#include <time.h>
#include <list>
#include <bitset>
#include <SDL_types.h>

#include "global.h"

#ifdef HAVE_LIMITS
 #include <limits>
#else
 #include <limits.h>
#endif

#include "basictypes.h"

// #include <values.h>

#include "pointers.h"
#include "tpascal.inc"
#include "misc.h"
#include "basestrm.h"
#include "errors.h"

// #include "password.h"





//! A Ellipse that is used for highlighting elements of the screen during the tutorial
class  EllipseOnScreen {
   public:
      int x1, y1, x2, y2;
      int color;
      float precision;
      int active;
      EllipseOnScreen ( void ) { active = 0; };
      void paint ( void );
      void read( tnstream& stream );
      void write ( tnstream& stream );
   };


//! The number of different weapon types
const int waffenanzahl = 8;

//! The number of different weather
const int cwettertypennum = 6;

//! The number of vehicle categories; Each category has its own move malus
const int cmovemalitypenum  = 18;

//! The movemalus type for a building. It is used for #SingleWeapon.targets_not_hittable .
const int cmm_building = 11;
const int cmm_trooper = 7;

//! The number of levels of height
const int choehenstufennum =  8;

//! The maximum number of number of different images for a building and a weather. For example, A wind power plant might have 6 different images, depending on the direction of the wind.
const int maxbuildingpicnum  = 8;

//! The maximum experience value of a #Vehicle
const int maxunitexperience = 23;


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
//     Resources operator* ( double d );
     bool operator== ( const Resources& res ) { return energy==res.energy && material==res.material && fuel==res.fuel; };
     Resources& operator+= ( const Resources& res ) { energy+=res.energy; material+=res.material; fuel+=res.fuel; return *this;};
     enum { Energy, Material, Fuel };
     void read ( tnstream& stream );
     void write ( tnstream& stream ) const;
     void runTextIO ( PropertyContainer& pc );
     void runTextIO ( PropertyContainer& pc, const Resources& defaultValue );
     static const char* name( int r );
     ASCString toString();
};

extern Resources operator- ( const Resources& res1, const Resources& res2 );
extern Resources operator+ ( const Resources& res1, const Resources& res2 );
extern Resources operator* ( const Resources& res1, float a );
extern Resources operator/ ( const Resources& res1, float a );


/** A mathematical matrix that can be multiplied with a #Resources instance (which is mathematically a vector) 
    to form a new #Resources vector. */
class ResourceMatrix {
           float e[resourceTypeNum][resourceTypeNum];
        public:
           ResourceMatrix ( const float* f );
           Resources operator* ( const Resources& r ) const;
};





//! the time in ASC, measured in turns and moves
struct GameTime {
  GameTime() { abstime = 0; };
  int move() { return abstime % 0x10000; };
  int turn() { return abstime / 0x10000; };
  static bool comp ( const GameTime& a, const GameTime& b ) { return a.abstime > b.abstime; };
  void set ( int turn, int move ) { abstime = (turn * 0x10000) + move ; };
  int abstime;
};


//! the image for a terraintype ( #tterraintype ) that is shown on the small map
struct FieldQuickView {
      char p1;
      char p3[3][3];
      char p5[5][5];
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
            bool valid() const { return x >= 0 && y >= 0 ; } ;
      };


//! Coordinate on the map including height
class MapCoordinate3D : public MapCoordinate {
            int z;
         public:
            int getBitmappedHeight ( ) const { if ( z >= 0 ) return 1<<z; else return 0;};
            int getNumericalHeight ( ) const { return z; };
            // MapCoordinate3D& operator= ( const MapCoordinate& mc ) { x = mc.x; y = mc.y; z = -1 );
            MapCoordinate3D ( ) : MapCoordinate(), z(-1) {};
            MapCoordinate3D ( int _x, int _y, int bitmappedz) : MapCoordinate ( _x, _y ), z ( log2(bitmappedz) ) {};
            // MapCoordinate3D ( const MapCoordinate& mc ) : MapCoordinate ( mc ), z ( -2 ) {};
            MapCoordinate3D ( const MapCoordinate& mc, int bitmappedHeight ) : MapCoordinate ( mc ), z ( log2(bitmappedHeight) ) {};
            void setnum ( int _x, int _y, int numericalz ) { x = _x; y = _y; z = numericalz; };
            bool operator== ( const MapCoordinate3D& mc ) const { return y == mc.y && x == mc.x && (z == mc.z || z == -2 || mc.z == -2);};
            bool operator!= ( const MapCoordinate3D& mc ) const { return (mc.x != x) || (mc.y != y) || (mc.z != z); };
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

class IntRange {
     public:
           int from;
           int to;
           IntRange(): from(-1), to(-1) {};
           IntRange( int from_, int to_ ): from(from_), to(to_) {};
           void read ( tnstream& stream );
           void write ( tnstream& stream ) const;
};

extern vector<IntRange> String2IntRangeVector( const ASCString& t );


template<typename C>
void writePointerContainer ( const C& c, tnstream& stream  )
{
   stream.writeInt ( 1 );
   stream.writeInt ( c.size() );
   typedef typename C::const_iterator IT;
   for ( IT i = c.begin(); i != c.end(); ++i )
      (*i)->write ( stream );
}

template<typename BaseType>
void readPointerContainer ( vector<BaseType*>& v, tnstream& stream  )
{
   stream.readInt(); // version
   int num = stream.readInt();
   for ( int i = 0; i < num; ++i ) {
      BaseType* bt = new BaseType;
      bt->read( stream );
      v.push_back( bt );
   }
}



template<typename C>
void writeClassContainer ( const C& c, tnstream& stream  )
{
   stream.writeInt ( 1 );
   stream.writeInt ( c.size() );
   typedef typename C::const_iterator IT;
   for ( IT i = c.begin(); i != c.end(); ++i )
      i->write ( stream );
}

template<typename C>
void readClassContainer ( C& c, tnstream& stream  )
{
   int version = stream.readInt();
   int num = stream.readInt();
   for ( int i = 0; i < num; ++i ) {
      typedef typename C::value_type VT;
      VT vt;
      vt.read( stream );
      c.push_back( vt );
   }
}

template<>
inline void writeClassContainer<> ( const vector<ASCString>& c, tnstream& stream  )
{
   stream.writeInt ( 1 );
   stream.writeInt ( c.size() );
   typedef vector<ASCString>::const_iterator IT;
   for ( IT i = c.begin(); i != c.end(); ++i )
      stream.writeString(*i);
}


template<>
inline void readClassContainer<> ( vector<ASCString>& c, tnstream& stream  )
{
   stream.readInt(); // version
   int num = stream.readInt();
   for ( int i = 0; i < num; ++i )
      c.push_back( stream.readString() );
}

template<>
inline void writeClassContainer<> ( const vector<int>& c, tnstream& stream  )
{
   stream.writeInt ( 1 );
   stream.writeInt ( c.size() );
   typedef vector<int>::const_iterator IT;
   for ( IT i = c.begin(); i != c.end(); ++i )
      stream.writeInt(*i);
}


template<>
inline void readClassContainer<> ( vector<int>& c, tnstream& stream  )
{
   stream.readInt(); // version
   int num = stream.readInt();
   for ( int i = 0; i < num; ++i )
      c.push_back( stream.readInt() );
}

template<>
inline void writeClassContainer<> ( const vector<pair<int,int> >& c, tnstream& stream  )
{
   stream.writeInt ( 1 );
   stream.writeInt ( c.size() );
   typedef vector<pair<int,int> >::const_iterator IT;
   for ( IT i = c.begin(); i != c.end(); ++i ) {
      stream.writeInt(i->first);
      stream.writeInt(i->second );
   }
}


template<>
inline void readClassContainer<> ( vector<pair<int,int> >& c, tnstream& stream  )
{
   stream.readInt(); // version
   int num = stream.readInt();
   for ( int i = 0; i < num; ++i ) {
       int first = stream.readInt();
       int second = stream.readInt();
       c.push_back( make_pair(first,second) );
   }
}



class MoveMalusType {
   public:
      enum {  deflt,
              light_tracked_vehicle,
              medium_tracked_vehicle,
              heavy_tracked_vehicle,
              light_wheeled_vehicle,
              medium_wheeled_vehicle,
              heavy_wheeled_vehicle,
              trooper,
              rail_vehicle,
              medium_aircraft,
              medium_ship,
              structure,
              light_aircraft,
              heavy_aircraft,
              light_ship,
              heavy_ship,
              helicopter,
              hoovercraft };
   };



/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/// Even more miscellaneous structures...
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

#pragma pack(1)

typedef struct tguiicon* pguiicon ;
struct tguiicon {
  void*      picture[2];
  char         txt[31];
  unsigned char         id;
  char         key[4];
  Uint16         realkey[4];
  unsigned char         order;
};


struct ticons {
   struct {
     void      *pfeil1, *pfeil2;
   } weapinfo;
   void*        statarmy[3];
   void*        height[8];      // fuer vehicleinfo - DLG-Box
   void*        height2[3][8];  // fuer vehicleinfo am map
   void*        player[8];      // aktueller Spieler in der dashboard: FARBE.RAW
   void*        allianz[8][3];  // Allianzen in der dashboard: ALLIANC.RAW 
   void*        diplomaticstatus[8]; 
   void*        selectweapongui[13];
   void*        selectweaponguicancel;
   void*        unitinfoguiweapons[14];
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
    } view;  
};




#pragma pack()

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/// Structure field naming constants 
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////



enum { capeace, cawar, cawarannounce, capeaceproposal, canewsetwar1, canewsetwar2, canewpeaceproposal, capeace_with_shareview };
enum VisibilityStates { visible_not, visible_ago, visible_now, visible_all };


  extern const char*  cwettertypen[];




extern  const char*  choehenstufen[8] ;
 #define chtiefgetaucht 1  
 #define chgetaucht 2
 #define chschwimmend 4  
 #define chfahrend 8  
 #define chtieffliegend 16  
 #define chfliegend 32  
 #define chhochfliegend 64  
 #define chsatellit 128  


#define cwaffentypennum 13
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
 #define cwweapon ( cwcruisemissileb | cwbombb | cwairmissileb | cwgroundmissileb | cwtorpedob | cwmachinegunb | cwcannonb | cwlaserb )
 #define cwshootablen 11
 #define cwshootableb ( 1 << cwshootablen  )
 #define cwlasern 10
 #define cwlaserb ( 1 << cwlasern  )
 #define cwammunitionn 9
 #define cwammunitionb ( 1 << cwammunitionn )
 #define cwservicen 8
 #define cwserviceb ( 1 << cwservicen )
 #define cwobjectplacementn 12
 #define cwobjectplacementb ( 1 << cwobjectplacementn )
 extern const int cwaffenproduktionskosten[cwaffentypennum][3];  /*  Angabe: Waffentyp; energy - Material - Sprit ; jeweils fuer 5er Pack */








extern const char*  resourceNames[3];


extern const char*  cmovemalitypes[cmovemalitypenum];


const int experienceDecreaseDamageBoundaryNum = 4;
extern const int experienceDecreaseDamageBoundaries[experienceDecreaseDamageBoundaryNum];


/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/// Constants that specify the layout of ASC
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

#define guiiconsizex 49  
#define guiiconsizey 35  


 #define maxmalq 10  
 #define minmalq 10
 #define fieldxsize 48    /*  Breite eines terrainbildes  */ 
 #define fieldysize 48  
 #define fielddistx 64
 #define fielddisty 24
 #define fielddisthalfx 32

 //! the number of sides that a field has; is now fixed at 6;
 const int sidenum = 6;

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

//! the movemalus for all submerged units
const int submarineMovement = 11;

#define movement_cost_for_repaired_unit 24
#define movement_cost_for_repairing_unit 12
#define attack_after_repair 1       // Can the unit that is beeing repaired attack afterwards?

#define mineputmovedecrease 10
#define mineremovemovedecrease 10
#define streetmovemalus 8
#define railroadmovemalus 8
#define searchforresorcesmovedecrease 8


#define fusstruppenplattfahrgewichtsfaktor 2  
#define mingebaeudeeroberungsbeschaedigung 80  

#define autorepairdamagedecrease 10    // only for old units ; new one use autorepairrate

#define weaponpackagesize 1

#define brigde1buildcostincrease 12       // jeweils Basis 8; flaches Wasser
#define brigde2buildcostincrease 16       // jeweils Basis 8; mitteltiefes Wasser
#define brigde3buildcostincrease 36       // jeweils Basis 8; tiefes Wasser


#define lookintoenemytransports false  
#define lookintoenemybuildings false

#define recyclingoutput 2    /*  Material div RecyclingOutput  */
#define destructoutput 5
#define nowindplanefuelusage 1      // herrscht kein Wind, braucht ein Flugzeug pro Runde soviel Sprit wie das fliegend dieser Anzahl fielder
  //   #define maxwindplainfuelusage 32   // beim nextturn: tank -= fuelconsumption * (maxwindplainfuelusage*nowindplainfuelusage + windspeed) / maxwindplainfuelusage     
const int maxwindspeed = 60;          // Wind with a strength of 255 means that the air moves 12 fields / turn


#define generatortruckefficiency 2  // fuer jede vehicle Power wird soviel Sprit gebraucht !

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

#define destruct_building_material_get 2 // beim Abreissen erh„lt man 1/2 des eingesetzten Materials zur?ck
#define destruct_building_fuel_usage 10  // beim Abreissen wird 10 * fuelconsumption Fuel fuelconsumptiont


#define dissectunitresearchpointsplus  2    // Beim dissectn einer vehicle wird der sovielte Teil der Researchpoints jeder unbekannten Technologie gutgeschrieben

#define dissectunitresearchpointsplus2 3    // Beim dissectn einer vehicle wird der sovielte Teil der Researchpoints jeder unbekannten Technologie gutgeschrieben.
  // fuer die Technologie existieren aber bereits von einem anderen sezierten vehicletype gutschriften.

//! The maximum number of fields a mining station can extract mineral resources from; Counted from its entry
const int maxminingrange = 10;

const float productionLineConstructionCostFactor = 0.5;
const float productionLineRemovalCostFactor = 0.2;

//! The weight of 1000 units of a Resource
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


#endif
