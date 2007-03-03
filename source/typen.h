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
#include <map>
#include <SDL_types.h>

#include "global.h"


#ifdef HAVE_LIMITS
 #include <limits>
#else
 #include <limits.h>
#endif

#include "basictypes.h"

#include "basestrm.h"
#include "messaginghub.h"


//! the color depth of the gamemap
const int gamemapPixelSize = 4;

//! the number of players that ASC can manage. This includes the neutral semi-played, which can't do anything
const int playerNum = 9;


//! The number of different weapon types
const int waffenanzahl = 8;

//! The number of different weather
const int cwettertypennum = 6;

//! The number of vehicle categories; Each category has its own move malus
const int cmovemalitypenum  = 18;

//! The movemalus type for a building. It is used for #SingleWeapon::targetingAccuracy .
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

class PropertyContainer;

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
     bool operator!= ( const Resources& res ) { return energy!=res.energy || material!=res.material || fuel!=res.fuel; };
     Resources& operator+= ( const Resources& res ) { energy+=res.energy; material+=res.material; fuel+=res.fuel; return *this;};
     enum { Energy, Material, Fuel };
     void read ( tnstream& stream );
     void write ( tnstream& stream ) const;
     void runTextIO ( PropertyContainer& pc );
     void runTextIO ( PropertyContainer& pc, const Resources& defaultValue );
     static const char* name( int r );
     ASCString toString();
     
     static const int materialColor = 0x8b3d1e;
     static const int fuelColor = 0x8b7800;
     static const int energyColor = 0x3b2dd7;
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
  int move() const { return abstime % 0x10000; };
  int turn() const { return abstime / 0x10000; };
  static bool comp ( const GameTime& a, const GameTime& b ) { return a.abstime > b.abstime; };
  void set ( int turn, int move ) { abstime = (turn * 0x10000) + move ; };
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
            bool operator!= ( const MapCoordinate& mc ) const { return y != mc.y || x != mc.x;};
            void write( tnstream& stream ) const { stream.writeInt ( 3000 ); stream.writeInt ( x ); stream.writeInt ( y); };
            void read( tnstream& stream ) {
               int vers = stream.readInt ( );
               if ( vers == 3500 )
                  fatalError ( " MapCoordinate::read - data was written by MapCoordinate3D ");

               x = stream.readInt ( );
               y = stream.readInt ( );
            };
            bool valid() const { return x >= 0 && y >= 0 ; } ;
	         void move(int width, int height);
            ASCString toString() const;
      };

inline MapCoordinate operator-( const MapCoordinate& a, const MapCoordinate& b )
{
  return MapCoordinate(a.x - b.x, a.y - b.y );
}  
      
inline MapCoordinate operator+( const MapCoordinate& a, const MapCoordinate& b )
{
   return MapCoordinate(a.x + b.x, a.y + b.y );
}
      

//! Coordinate on the map including height
class MapCoordinate3D : public MapCoordinate {
            int z;
         public:
            int getBitmappedHeight ( ) const { if ( z >= 0 ) return 1<<z; else return 0;};
            int getNumericalHeight ( ) const { return z; };
            void setNumericalHeight ( int nh ) { z = nh; };
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


template< typename T> 
class deallocating_vector : public vector<T> {
   public:
      ~deallocating_vector() {
         for ( typename vector<T>::iterator i = vector<T>::begin(); i != vector<T>::end(); ++i )
            delete *i;
      };      
};

template< typename T, typename U>
class deallocating_map : public std::map<T,U> {
   public:
      ~deallocating_map() {
         for ( typename std::map<T,U>::iterator i = std::map<T,U>::begin(); i != std::map<T,U>::end(); ++i )
            delete i->second;
      };
};


class IntRange {
     public:
           int from;
           int to;
           IntRange(): from(-1), to(-1) {};
           IntRange( int oneValue ): from(oneValue), to(oneValue) {};
           IntRange( int from_, int to_ ): from(from_), to(to_) {};
           void read ( tnstream& stream );
           void write ( tnstream& stream ) const;
};

extern vector<IntRange> String2IntRangeVector( const ASCString& t );




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








extern const char*  resourceNames[3];


extern const char*  cmovemalitypes[cmovemalitypenum];
extern const char*  moveMaliTypeIcons[cmovemalitypenum];

const int experienceDecreaseDamageBoundaryNum = 4;
extern const int experienceDecreaseDamageBoundaries[experienceDecreaseDamageBoundaryNum];


/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/// Constants that specify the layout of ASC
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////


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
const int submarineMovement = 10;

#define movement_cost_for_repaired_unit 24
#define movement_cost_for_repairing_unit 12
#define attack_after_repair 1       // Can the unit that is beeing repaired attack afterwards?

#define mineputmovedecrease 10
#define mineremovemovedecrease 10

#define fusstruppenplattfahrgewichtsfaktor 2  
#define mingebaeudeeroberungsbeschaedigung 80  

#define lookintoenemytransports false
#define lookintoenemybuildings false

#define recyclingoutput 2    /*  Material div RecyclingOutput  */
#define destructoutput 5
#define nowindplanefuelusage 1      // herrscht kein Wind, braucht ein Flugzeug pro Runde soviel Sprit wie das fliegend dieser Anzahl fielder
  //   #define maxwindplainfuelusage 32   // beim nextturn: tank -= fuelconsumption * (maxwindplainfuelusage*nowindplainfuelusage + windspeed) / maxwindplainfuelusage     
const int maxwindspeed = 60;          // Wind with a strength of 255 means that the air moves 6 fields / turn


#define generatortruckefficiency 2  // fuer jede vehicle Power wird soviel Sprit gebraucht !

#define mine_movemalus_increase 50   // percent

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


#define resource_fuel_factor 80         // die im boden liegenden Bodenschtzen ergeben effektiv soviel mal mehr ( bei Bergwerkseffizienz 1024 )
#define resource_material_factor 80     // "

#define destruct_building_material_get 2 // beim Abreissen erhlt man 1/2 des eingesetzten Materials zur?ck
#define destruct_building_fuel_usage 10  // beim Abreissen wird 10 * fuelconsumption Fuel fuelconsumptiont


#define dissectunitresearchpointsplus  2    // Beim dissectn einer vehicle wird der sovielte Teil der Researchpoints jeder unbekannten Technologie gutgeschrieben

#define dissectunitresearchpointsplus2 3    // Beim dissectn einer vehicle wird der sovielte Teil der Researchpoints jeder unbekannten Technologie gutgeschrieben.
  // fuer die Technologie existieren aber bereits von einem anderen sezierten vehicletype gutschriften.

//! The maximum number of fields a mining station can extract mineral resources from; Counted from its entry
const int maxminingrange = 10;

const double productionLineConstructionCostFactor = 0.5;
const double productionLineRemovalCostFactor = 0.2;



extern const int csolarkraftwerkleistung[];

//! The number of netcontrol actions ; see #cnetcontrol 
const int cnetcontrolnum  = 12;

//! The actions that can be used to control a ResourceNet
extern const char* cnetcontrol[cnetcontrolnum];

extern const char* cgeneralnetcontrol[];

const int maxViewRange = 255;

#endif
