/***************************************************************************
                          vehicletype.cpp  -  description
                             -------------------
    begin                : Fri Sep 29 2000
    copyright            : (C) 2000 by Martin Bickel
    email                : bickel@asc-hq.org
 ***************************************************************************/

/*! \file vehicletype.cpp
    \brief The #Vehicletype class
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
#include "global.h"
#include "vehicletype.h"
#include "sgstream.h"
#include "graphicset.h"
#include "terraintype.h"
#include "objecttype.h"
#include "textfileparser.h"
#include "textfiletags.h"
#include "textfile_evaluation.h"
#include "graphics/blitter.h"
#include "graphics/ColorTransform_PlayerColor.h"

/*
const char*  cvehiclefunctions[cvehiclefunctionsnum+1]  = {
                   "sonar",
                   "paratrooper",
                   "mine-layer",
                   "cruiser landing",
                   "repair vehicle",
                   "conquer buildings",
                   "move after attack",
                   "view satellites",
                   "construct ALL buildings",
                   "view mines",
                   "construct vehicles",
                   "construct specific buildings",
                   "refuel units",
                   "icebreaker",
                   "cannot be refuelled in air",
                   "refuels material",
                   "! (unused) !",
                   "makes tracks",
                   "drill for mineral resources manually",
                   "no reactionfire",
                   "auto repair",
                   "generator",
                   "search for mineral resources automatically",
                   "Kamikaze only",
                   "immune to mines",
                   "refuels energy",
                   "jams only own field",
                   "move with reaction fire on",
                   "only move to and from transports",
                   NULL };
*/
const char*  cwaffentypen[weaponTypeNum]  = {"cruise missile", "mine",    "bomb",       "large missile", "small missile", "torpedo", "machine gun",
      "cannon",         "service", "ammunition refuel", "laser", "shootable", "object placement"
                                            };

const int  ammoProductionCost[weaponTypeNum][3]    = { { 1500,1500,1500 }, // cruise missile
   {10, 10, 10},       // mine
   {40, 40, 40},       // bomb
   {200, 200, 200},    // big missile
   {50, 50, 50},       // small missile
   {20, 30, 40},       // torpedo
   {1, 1, 1},          // machine gun
   {5, 5, 1},          // cannon
   {0, 0, 0},     // service
   {0, 0, 0},     // ammo refuel
   {0, 0, 0},     // laser
   {0, 0, 0},     // shootable
   {0, 0, 0}
}
;    // objectPlacement


const bool  weaponAmmo[weaponTypeNum] = {
   true, true, true, true, true, true, true, true, false, false, false, false, false
};

Vehicletype :: Vehicletype ( void )
{
   recommendedAIJob = AiParameter::job_undefined;

   int i;

   armor = 0;

   height     = 0;
   cargoMovementDivisor = 2;
   wait = 0;
   fuelConsumption = 0;

   movement.resize(8);
   for ( i = 0; i < 8; i++ )
      movement[i] = 0;
   movemalustyp = 0;

   maxwindspeedonwater = 0;
   digrange = 0;
   initiative = 0;

   weight = 0;
   bipicture = -1;

   autorepairrate = 0;

   heightChangeMethodNum = 0;

   for ( i = 0; i < 8; i++ )
      aiparam[i] = NULL;

   unitConstructionMoveCostPercentage = 50;

   unitConstructionMinDistance = 1;
   unitConstructionMaxDistance = 1;
}


int Vehicletype::maxsize ( void ) const
{
   return weight;
}


const int vehicle_version = 30;



void Vehicletype::setupRemovableObjectsFromOldFileLayout ( )
{
   for ( vector<IntRange>::iterator i = objectsBuildable.begin(); i != objectsBuildable.end();  )
      if ( i->to < 0 && i->from < 0 ) {
         int mi = min ( -i->from, -i->to );
         int ma = max ( -i->from, -i->to );
         objectsRemovable.push_back ( IntRange ( mi, ma ));
         i = objectsBuildable.erase ( i );
      } else {
         objectsRemovable.push_back ( *i );
         i++;
      }
}

void Vehicletype :: read ( tnstream& stream )
{
   int version = stream.readInt();
   if ( version > vehicle_version || version < 1)
      throw tinvalidversion ( stream.getLocation(), vehicle_version, version );

   int   j;

   bool ___load_name = stream.readInt();
   bool ___load_description = stream.readInt();
   bool ___load_infotext = stream.readInt();

   if ( version <= 2 ) {
      weapons.count = stream.readChar();
      for ( j = 0; j< 8; j++ ) {
         weapons.weapon[j].set ( stream.readWord() );
         weapons.weapon[j].targ = stream.readChar();
         weapons.weapon[j].sourceheight = stream.readChar();
         weapons.weapon[j].maxdistance = stream.readWord();
         weapons.weapon[j].mindistance = stream.readWord();
         weapons.weapon[j].count = stream.readChar();
         weapons.weapon[j].maxstrength = stream.readChar();
         weapons.weapon[j].minstrength = stream.readChar();
         for ( int k = 0; k < 13; k++ )
            weapons.weapon[j].efficiency[k] = 100;
      }
   }

   if ( version <= 27 ) {
      productionCost.energy   = stream.readWord();
      productionCost.material = stream.readWord();
   } else
      productionCost.read( stream );

   armor = stream.readWord();

   bool picture[8];
   for ( j = 0; j < 8; j++ )
      if ( version <= 18 )
         picture[j] = stream.readInt();
      else
         picture[j] = false;

   height = stream.readChar();
   stream.readWord(); // was: researchID
   int _terrain = 0;
   int _terrainreq = 0;
   int _terrainkill = 0;
   if ( version <= 2 ) {
      _terrain = stream.readInt();
      _terrainreq = stream.readInt();
      _terrainkill = stream.readInt();
   }
   stream.readChar(); // steigung
   jamming = stream.readChar();
   view = stream.readWord();
   wait = stream.readChar();

   if ( version <= 2 )
      stream.readChar (); // dummy

   stream.readWord(); // dummy
   stream.readWord(); // dummy
   stream.readChar(); // dummy
   stream.readChar(); // dummy
   stream.readChar(); // dummy
   if ( version <= 18 )
      id = stream.readWord();
   else
      id = stream.readInt();

   if ( version < 24 ) {
      bi_mode_tank.resource(2) = bi_mode_tank.resource(2) = stream.readInt();
      fuelConsumption = stream.readWord();
      bi_mode_tank.resource(0) = bi_mode_tank.resource(0) = stream.readInt();
      bi_mode_tank.resource(1) = bi_mode_tank.resource(1) = stream.readInt();
   } else
      fuelConsumption = stream.readInt();

   if ( version <= 22 ) {
      int functions = stream.readInt();
      features = convertOldFunctions ( functions, stream.getLocation() );
   }

   for ( j = 0; j < 8; j++ )
      if ( version <= 4 )
         movement[j] = stream.readChar();
      else
         movement[j] = stream.readInt();


   movemalustyp = stream.readChar();
   if ( movemalustyp >= cmovemalitypenum )
      movemalustyp = cmovemalitypenum -1;

   if ( version <= 2 )
      for ( j = 0; j < 9; j++ )
         stream.readWord( ); // dummy1

   bool ___load_classnames[8];
   if ( version <= 15 ) {
      stream.readChar(); // classnum

      for ( j = 0; j < 8; j++ )
         ___load_classnames[j] = stream.readInt();

      for ( j = 0; j < 8; j++ ) {
         int k;
         for ( k = 0; k < 8; k++ )
            stream.readWord();

         if ( version <= 2 )
            stream.readWord (  ); // dummy2

         stream.readWord();
         stream.readWord();
         for ( k = 0; k < 4; k++ )
            stream.readWord();

         stream.readChar();
         stream.readInt();
         if ( version <= 2 )
            stream.readChar( ); // dummy
      }
   }

   maxwindspeedonwater = stream.readChar();
   digrange = stream.readChar();
   initiative = stream.readInt();
   int _terrainnot = 0;
   if ( version <= 4 ) {
      _terrainnot = stream.readInt();
      stream.readInt(); // _terrainreq1
   }
   int objectsbuildablenum = stream.readInt();
   if ( version <= 4 )
      stream.readInt(); // objectsbuildableid = (int*)

   weight = stream.readInt();

   bool ___loadterrainaccess = false;
   if ( version <= 4 )
      ___loadterrainaccess = stream.readInt();

   bipicture = stream.readInt();
   int vehiclesbuildablenum = stream.readInt();
   if ( version <= 4 )
      stream.readInt(); // vehiclesbuildableid = (int*)

   if ( version <= 4 )
      stream.readInt(); // buildicon

   int buildingsbuildablenum = stream.readInt();
   if ( version <= 4 )
      stream.readInt(); // buildingsbuildable = (Vehicletype::tbuildrange*)

   bool load_weapons = stream.readInt();
   autorepairrate = stream.readInt();
   if ( version >= 15 )
      readClassContainer ( wreckageObject, stream );
   else
      if ( version >= 8 ) {
         wreckageObject.clear();
         wreckageObject.push_back ( stream.readInt() );
      }


   if ( version <= 2 )
      stream.readInt( ); // dummy

   if ( version >= 4 )
      stream.readInt(); // dummy

   if (___load_name)
      name = stream.readString( true );

   if (___load_description)
      description = stream.readString( true );

   if (___load_infotext)
      infotext = stream.readString ( true );

   int i;
   if ( version <= 15 )
      for ( i=0;i<8  ;i++ )
         if ( ___load_classnames[i] )
            stream.readString ( true );

   if ( hasFunction( AutoRepair ) )
      if ( !autorepairrate )
         autorepairrate =  10;

   if ( version <= 18 ) {
      if ( picture[0] )
         image.read ( stream );

      for ( int i=1;i<8  ;i++ ) {
         if ( picture[i] ) {
            Surface s;
            s.read ( stream );
            bipicture = 0;
         }
      }
   } else {
      image.read ( stream );
   }


   if ( objectsbuildablenum )
      for ( i = 0; i < objectsbuildablenum; i++ ) {
         int from = stream.readInt ( );
         int to;
         if ( version <= 4 )
            to = from;
         else
            to = stream.readInt();

         objectsBuildable.push_back ( IntRange ( from, to ));
      }

   if ( version >= 6 ) {
      int num = stream.readInt();
      for ( int i = 0; i < num; i++ ) {
         int from = stream.readInt();
         int to   = stream.readInt();

         objectsRemovable.push_back ( IntRange ( from, to ));
      }
   } else
      setupRemovableObjectsFromOldFileLayout();

   if ( version >= 8 ) {
      int num = stream.readInt();
      for ( int i = 0; i < num; i++ ) {
         int from = stream.readInt();
         int to   = stream.readInt();

         objectGroupsBuildable.push_back ( IntRange ( from, to ));
      }

      num = stream.readInt();
      for ( int i = 0; i < num; i++ ) {
         int from = stream.readInt();
         int to   = stream.readInt();

         objectGroupsRemovable.push_back ( IntRange ( from, to ));
      }
   }

   if ( vehiclesbuildablenum )
      for ( i = 0; i < vehiclesbuildablenum; i++ ) {
         int from = stream.readInt ( );
         int to;
         if ( version <= 4 )
            to = from;
         else
            to = stream.readInt();

         vehiclesBuildable.push_back ( IntRange ( from, to ));
      }

   if ( load_weapons && version > 1) {
      weapons.count = stream.readInt();
      for ( j = 0; j< 16; j++ ) {
         weapons.weapon[j].set ( stream.readInt() );
         weapons.weapon[j].targ = stream.readInt();
         weapons.weapon[j].sourceheight = stream.readInt();
         weapons.weapon[j].maxdistance = stream.readInt();
         weapons.weapon[j].mindistance = stream.readInt();
         weapons.weapon[j].count = stream.readInt();
         weapons.weapon[j].maxstrength = stream.readInt();
         weapons.weapon[j].minstrength = stream.readInt();

         if ( version >= 17 )
            weapons.weapon[j].reactionFireShots = stream.readInt();

         if ( version >= 22 )
            weapons.weapon[j].name = stream.readString();

         /*
         if ( weapons.weapon[j].getScalarWeaponType() == cwminen )
            if ( weapons.weapon[j].mindistance < 8 )
               warning ( ASCString("unit with id ") + strrr ( id ) + " has invalid mine weapon range ");
         */

         for ( int k = 0; k < 13; k++ )
            weapons.weapon[j].efficiency[k] = stream.readInt();

         if ( version <= 6 ) {
            int targets_not_hittable = stream.readInt();
            for ( int i = 0; i < cmovemalitypenum; i++ )
               if ( targets_not_hittable & ( 1 << i))
                  weapons.weapon[j].targetingAccuracy[i] = 0;
         } else {
            int num = stream.readInt();
            for ( int i = 0; i < num; i++ )
               weapons.weapon[j].targetingAccuracy[i] = stream.readInt();
         }


         if ( version <= 2 )
            for ( int l = 0; l < 9; l++ )
               stream.readInt(); // dummy

         if ( version >= 11 ) {
            weapons.weapon[j].laserRechargeRate = stream.readInt();
            weapons.weapon[j].laserRechargeCost.read( stream );
         }
         if ( version >= 20 )
            weapons.weapon[j].soundLabel = stream.readString();

      }

      if ( version <= 2 )
         for ( int m = 0; m< 10; m++ )
            stream.readInt(); // dummy

   }

   if ( ___loadterrainaccess || version >= 5 )
      terrainaccess.read ( stream );
   else {
      terrainaccess.terrain.setInt ( _terrain, 0 );
      terrainaccess.terrainreq.setInt ( _terrainreq, 0 );
      terrainaccess.terrainnot.setInt ( _terrainnot, 0 );
      terrainaccess.terrainkill.setInt ( _terrainkill, 0 );
   }

   if ( buildingsbuildablenum )
      for ( i = 0; i < buildingsbuildablenum; i++ ) {
         int from = stream.readInt();
         int to = stream.readInt();
         buildingsBuildable.push_back ( IntRange ( from, to ));
      }


   filename = stream.getDeviceName();
   location = stream.getLocation();

   if ( version >= 9 )
      ContainerBaseType::read ( stream );

   if ( version >= 10 ) {
      heightChangeMethodNum = stream.readInt();
      heightChangeMethod.resize(heightChangeMethodNum );
      for ( int i = 0; i < heightChangeMethodNum; i++ )
         heightChangeMethod[i].read( stream );
   } else
      heightChangeMethodNum = 0;

   if ( version >= 12 )
      techDependency.read( stream );

   if ( version >= 13 && version <= 24 ) {
      Surface s;
      s.read( stream );
   }

   if ( version >= 14 && version < 18)
      cargoMovementDivisor = stream.readInt();
   else
      if ( version >= 18 )
         cargoMovementDivisor = stream.readFloat();

   if ( version >= 20 ) {
      movementSoundLabel = stream.readString();
      killSoundLabel = stream.readString();
   }

   if ( version >= 22 )
      readClassContainer( guideSortHelp, stream );

   if ( version >= 24 ) {
      efficiencyfuel = stream.readInt( );
      efficiencymaterial = stream.readInt( );
      asc_mode_tank.read( stream );
      bi_mode_tank.read( stream );


      maxresearchpoints = stream.readInt();
      defaultMaxResearchpoints = stream.readInt();
      nominalresearchpoints = stream.readInt();
      maxplus.read( stream );
      defaultProduction.read( stream );
   }

   for ( int w = 0; w < weapons.count; ++w )
      if ( weapons.weapon[w].canRefuel() )
         setFunction( ExternalAmmoTransfer );

   if ( version >= 26 ) {
      jumpDrive.height = stream.readInt();
      jumpDrive.targetterrain.read( stream );
      jumpDrive.consumption.read( stream );
      jumpDrive.maxDistance = stream.readInt();
   }

   if ( version >= 27 ) {
      int num = stream.readInt();
      for ( int i = 0; i < num; i++ ) {
         int from = stream.readInt();
         int to   = stream.readInt();
         objectLayedByMovement.push_back ( IntRange ( from, to ));
      }
   }

   if ( version >= 29 )
      unitConstructionMoveCostPercentage = stream.readInt();

   if ( version >= 30 ) {
      unitConstructionMinDistance = stream.readInt();
      unitConstructionMaxDistance = stream.readInt();
   }
}




void Vehicletype:: write ( tnstream& stream ) const
{
   int i,j;

   stream.writeInt ( vehicle_version );

   if ( !name.empty() )
      stream.writeInt( 1 );
   else
      stream.writeInt( 0 );

   if ( !description.empty() )
      stream.writeInt( 1 );
   else
      stream.writeInt( 0 );

   if ( !infotext.empty() )
      stream.writeInt( 1 );
   else
      stream.writeInt( 0 );

   productionCost.write( stream );
   stream.writeWord( armor );
   stream.writeChar( height );
   stream.writeWord(0); // researchid
   stream.writeChar(0); // steigung
   stream.writeChar(jamming);
   stream.writeWord(view);
   stream.writeChar(wait);
   stream.writeWord(0);
   stream.writeWord(0);
   stream.writeChar(0);
   stream.writeChar(0);
   stream.writeChar(0);
   stream.writeInt(id );
   stream.writeInt(fuelConsumption );
   for ( j = 0; j < 8; j++ )
      stream.writeInt( movement[j] );


   stream.writeChar(movemalustyp );

   stream.writeChar(maxwindspeedonwater );
   stream.writeChar(digrange );
   stream.writeInt(initiative );
   stream.writeInt( objectsBuildable.size() );

   stream.writeInt(weight );

   stream.writeInt(bipicture );
   stream.writeInt(vehiclesBuildable.size() );
   stream.writeInt( buildingsBuildable.size() );
   stream.writeInt( 1 ); // weapons

   stream.writeInt( autorepairrate );
   writeClassContainer( wreckageObject, stream );

   stream.writeInt( 0 );

   if ( !name.empty() )
      stream.writeString( name );

   if ( !description.empty() )
      stream.writeString( description );

   if ( !infotext.empty() )
      stream.writeString( infotext );

   image.write( stream );

   for ( i = 0; i < objectsBuildable.size(); i++ ) {
      stream.writeInt ( objectsBuildable[i].from );
      stream.writeInt ( objectsBuildable[i].to );
   }

   stream.writeInt ( objectsRemovable.size() );
   for ( i = 0; i < objectsRemovable.size(); i++ ) {
      stream.writeInt ( objectsRemovable[i].from );
      stream.writeInt ( objectsRemovable[i].to );
   }

   stream.writeInt ( objectGroupsBuildable.size() );
   for ( i = 0; i < objectGroupsBuildable.size(); i++ ) {
      stream.writeInt ( objectGroupsBuildable[i].from );
      stream.writeInt ( objectGroupsBuildable[i].to );
   }

   stream.writeInt ( objectGroupsRemovable.size() );
   for ( i = 0; i < objectGroupsRemovable.size(); i++ ) {
      stream.writeInt ( objectGroupsRemovable[i].from );
      stream.writeInt ( objectGroupsRemovable[i].to );
   }


   for ( i = 0; i < vehiclesBuildable.size(); i++ ) {
      stream.writeInt ( vehiclesBuildable[i].from );
      stream.writeInt ( vehiclesBuildable[i].to );
   }

   stream.writeInt(weapons.count );
   for ( j = 0; j< 16; j++ ) {
      stream.writeInt(weapons.weapon[j].gettype( ));
      stream.writeInt(weapons.weapon[j].targ);
      stream.writeInt(weapons.weapon[j].sourceheight );
      stream.writeInt(weapons.weapon[j].maxdistance );
      stream.writeInt(weapons.weapon[j].mindistance );
      stream.writeInt(weapons.weapon[j].count );
      stream.writeInt(weapons.weapon[j].maxstrength );
      stream.writeInt(weapons.weapon[j].minstrength );
      stream.writeInt(weapons.weapon[j].reactionFireShots );
      stream.writeString( weapons.weapon[j].name );

      for ( int k = 0; k < 13; k++ )
         stream.writeInt(weapons.weapon[j].efficiency[k] );

      stream.writeInt ( cmovemalitypenum );
      for ( int i = 0; i < cmovemalitypenum; i++ )
         stream.writeInt(weapons.weapon[j].targetingAccuracy[i] );

      stream.writeInt( weapons.weapon[j].laserRechargeRate );
      weapons.weapon[j].laserRechargeCost.write( stream );
      stream.writeString( weapons.weapon[j].soundLabel );
   }

   terrainaccess.write ( stream );

   for ( i = 0; i < buildingsBuildable.size(); i++ ) {
      stream.writeInt( buildingsBuildable[i].from );
      stream.writeInt( buildingsBuildable[i].to );
   }

   ContainerBaseType::write ( stream );


   stream.writeInt( heightChangeMethodNum );
   for ( i = 0; i < heightChangeMethodNum; i++ )
      heightChangeMethod[i].write( stream );

   techDependency.write( stream );

   stream.writeFloat ( cargoMovementDivisor );

   stream.writeString( movementSoundLabel );
   stream.writeString( killSoundLabel );
   writeClassContainer( guideSortHelp, stream );

   stream.writeInt( efficiencyfuel );
   stream.writeInt( efficiencymaterial );

   asc_mode_tank.write( stream );
   bi_mode_tank.write( stream );

   stream.writeInt( maxresearchpoints );
   stream.writeInt( defaultMaxResearchpoints );
   stream.writeInt( nominalresearchpoints );
   maxplus.write ( stream );
   defaultProduction.write( stream );

   stream.writeInt( jumpDrive.height );
   jumpDrive.targetterrain.write( stream );
   jumpDrive.consumption.write( stream );
   stream.writeInt( jumpDrive.maxDistance );

   stream.writeInt ( objectLayedByMovement.size() );
   for ( i = 0; i < objectLayedByMovement.size(); i++ ) {
      stream.writeInt ( objectLayedByMovement[i].from );
      stream.writeInt ( objectLayedByMovement[i].to );
   }

   stream.writeInt( unitConstructionMoveCostPercentage );
   stream.writeInt( unitConstructionMinDistance );
   stream.writeInt( unitConstructionMaxDistance );
}


ASCString Vehicletype::getName( ) const
{
   if ( !name.empty() )
      return name;
   else
      return description;
}

int Vehicletype :: maxSpeed ( ) const
{
   int maxUnitMovement = 0;
   for ( int i = 0; i < 8; i++ )
      maxUnitMovement = max ( maxUnitMovement, movement[i] );
   return maxUnitMovement;
}

int Vehicletype::getMemoryFootprint() const
{
   return sizeof(*this) + image.getMemoryFootprint();
}


Vehicletype :: ~Vehicletype ( )
{
   for ( int i = 0; i < 8; i++ )
      if ( aiparam[i] ) {
         delete aiparam[i];
         aiparam[i] = NULL;
      }
}



SingleWeapon::SingleWeapon()
{
   typ = 0;
   targ = 0;
   sourceheight = 0;
   maxdistance = 0;
   mindistance = 0;
   count = 0;
   maxstrength= 0;
   minstrength = 0;
   laserRechargeRate = 0;
   for ( int i = 0; i < 13; i++ )
      efficiency[i] = 0;
   for ( int i = 0; i < cmovemalitypenum; i++ )
      targetingAccuracy[i] = 100;

   reactionFireShots = 1;
}


/* Translate the weapon/mine/service bit pattern into scalar
 * weapon number for use in fetching UI resources.
 */
int SingleWeapon::getScalarWeaponType(void) const
{
   if ( typ & (cwweapon | cwmineb) )
      return log2 ( typ & (cwweapon | cwmineb) );
   else
      return -1;
}


bool SingleWeapon::requiresAmmo(void) const
{
   if ( typ & cwlaserb )
      return false;
   else
      return typ & ( cwweapon | cwmineb );
}

bool SingleWeapon::shootable( void ) const
{
   return typ & cwshootableb;
}

bool SingleWeapon::offensive( void ) const
{
   return typ & cwweapon;
}

bool SingleWeapon::service( void ) const
{
   return typ & cwserviceb;
}

bool SingleWeapon::placeObjects(  ) const
{
   return typ & cwobjectplacementb;
}


bool SingleWeapon::canRefuel( void ) const
{
   return typ & cwammunitionb;
}

void SingleWeapon::set
( int type )
{
   typ = type;
}

ASCString SingleWeapon::getName ( void ) const
{
   if ( !name.empty() )
      return name;

   ASCString s;

   int k = getScalarWeaponType();
   if ( k < weaponTypeNum && k >= 0 )
      s = cwaffentypen[k];
   else
      if ( service() || placeObjects() )
         s = cwaffentypen[cwservicen];
      else
         s = "undefined";

   return s;
}

ASCString    SingleWeapon::getIconFileName( int numerical )
{
   switch ( numerical ) {
      case cwcruisemissile:
         return "weap-cruisemissile";
      case cwbombn:
         return "weap-bomb";
      case cwlargemissilen:
         return "weap-bigmissile";
      case cwsmallmissilen:
         return "weap-smallmissile";
      case cwtorpedon:
         return "weap-torpedo";
      case cwmachinegunn:
         return "weap-machinegun";
      case cwcannonn:
         return "weap-cannon";
      case cwminen:
         return "weap-mine";
      case cwservicen:
         return "weap-service";
      case cwlasern:
         return "weap-laser";
      default:
         return "weap-undefined";
   };
}


bool SingleWeapon::equals( const SingleWeapon* otherWeapon ) const
{
   if (
      otherWeapon->targ == this->targ &&
      otherWeapon->sourceheight == this->sourceheight &&
      otherWeapon->maxdistance == this->maxdistance &&
      otherWeapon->mindistance == this->mindistance &&
      otherWeapon->count == this->count &&
      otherWeapon->maxstrength == this->maxstrength &&
      otherWeapon->minstrength == this->minstrength &&
      otherWeapon->gettype() == this->gettype()
   ) {
      bool equal = true;
      for ( int i=0; i<13; i++ ) {
         if ( otherWeapon->efficiency[ i ] != this->efficiency[ i ] ) {
            equal = false;
         }
      }
      if ( equal ) return true;
   }

   return false;
}


UnitWeapon :: UnitWeapon ( void )
{
   count = 0;
}




void Vehicletype::runTextIO ( PropertyContainer& pc )
{
   ContainerBaseType::runTextIO ( pc );

   pc.addString( "Description", description);

   pc.addInteger( "Armor", armor );

   ASCString fn;
   if ( filename.empty() ) {
      fn += "vehicle";
      fn += strrr(id);
   } else
      fn = extractFileName_withoutSuffix( filename );

   pc.addImage( "Picture", image, fn, true );
   if ( image.w() < fieldsizex || image.h() < fieldsizey )
      image.strech( fieldsizex, fieldsizey );

   image.assignDefaultPalette();

   pc.addTagInteger( "Height", height, choehenstufennum, heightTags );
   pc.addBool ( "WaitForAttack", wait );

   if ( bi_mode_tank == Resources(0,0,0) && asc_mode_tank == Resources(0,0,0)) {
      pc.openBracket( "Tank" );
      Resources tank;
      tank.runTextIO ( pc );
      pc.closeBracket();
      bi_mode_tank = tank;
      asc_mode_tank = tank;
   }

   pc.addInteger( "FuelConsumption", fuelConsumption );
   if ( !pc.find("Features") && pc.isReading() ) {
      int abilities;
      pc.addTagInteger ( "Abilities", abilities, legacyVehicleFunctionNum, vehicleAbilities );

      features = convertOldFunctions ( abilities, pc.getFileName() );

   } else
      pc.addTagArray ( "Features", features, functionNum, containerFunctionTags );


   pc.addIntegerArray ( "Movement", movement );
   while ( movement.size() < 8 )
      movement.push_back(0);
   for ( vector<int>::iterator i = movement.begin(); i != movement.end(); i++ )
      if ( *i > 255 )
         *i = 255;

   pc.addNamedInteger ( "Category", movemalustyp, cmovemalitypenum, unitCategoryTags );
   pc.addInteger("MaxSurvivableStorm", maxwindspeedonwater, 255 );
   pc.addInteger("ResourceDrillingRange", digrange, 0 );
   pc.addInteger("SelfRepairRate", autorepairrate, 0 );
   if ( pc.find ( "WreckageObject" ) || !pc.isReading() )
      pc.addIntegerArray("WreckageObject", wreckageObject );


   if ( pc.find( "CargoMovementDivisor" ))
      pc.addDFloat("CargoMovementDivisor", cargoMovementDivisor);
   else {
      pc.openBracket( "Transportation" );
      pc.addDFloat("CargoMovementDivisor", cargoMovementDivisor, 2 );
      pc.closeBracket();
   }


   pc.addInteger("Weight",  weight);
   pc.openBracket("TerrainAccess" );
   terrainaccess.runTextIO ( pc );
   pc.closeBracket();

   pc.openBracket ( "Construction" );
   pc.addIntRangeArray ( "Buildings", buildingsBuildable );
   pc.addIntRangeArray ( "Vehicles", vehiclesBuildable );
   pc.addIntRangeArray ( "Objects", objectsBuildable );
   if ( pc.isReading() ) {
      if ( pc.find ( "ObjectsRemovable" ))
         pc.addIntRangeArray ( "ObjectsRemovable", objectsRemovable );
      else
         setupRemovableObjectsFromOldFileLayout();
   } else
      pc.addIntRangeArray ( "ObjectsRemovable", objectsRemovable );

   pc.addIntRangeArray ( "ObjectGroupsBuildable", objectGroupsBuildable, false );
   pc.addIntRangeArray ( "ObjectGroupsRemovable", objectGroupsRemovable, false );
   pc.addInteger("UnitConstructionMoveCostPercentage", unitConstructionMoveCostPercentage, 50);
   pc.addInteger("UnitConstructionMinDistance", unitConstructionMinDistance, 1 );
   pc.addInteger("UnitConstructionMaxDistance", unitConstructionMaxDistance, 1 );
   pc.closeBracket();

   pc.openBracket ( "Weapons");
   // pc.addInteger("Initiative", initiative );
   pc.addInteger("Number", weapons.count );
   for ( int i = 0; i < weapons.count; i++ ) {
      pc.openBracket ( ASCString("Weapon")+strrr(i) );
      weapons.weapon[i].runTextIO( pc );
      pc.closeBracket();
      if ( hasFunction( NoReactionfire ) )
         weapons.weapon[i].reactionFireShots = 0;

   }

   pc.closeBracket();

   int job = recommendedAIJob;
   pc.addNamedInteger ( "AIJobOverride", job, AiParameter::jobNum, AIjobs, AiParameter::job_undefined );
   recommendedAIJob = AiParameter::Job(job);

   pc.addString("MovementSound", movementSoundLabel, "" );
   pc.addString("KillSound", killSoundLabel, "" );

   pc.addInteger("HeightChangeMethodNum", heightChangeMethodNum, 0 );
   heightChangeMethod.resize( heightChangeMethodNum );
   for ( int i = 0; i < heightChangeMethodNum; i++ ) {
      pc.openBracket( ASCString("HeightChangeMethod")+strrr(i) );
      heightChangeMethod[i].runTextIO ( pc );
      pc.closeBracket();
   }

   techDependency.runTextIO( pc, strrr(id) );



   pc.openBracket ( "ConstructionCost" );
   productionCost.runTextIO ( pc );
   int costCalcMethod = 0;
   pc.addNamedInteger( "CalculationMethod", costCalcMethod, productionCostCalculationMethodNum, productionCostCalculationMethod,  0 );
   if ( pc.isReading() ) {
      if ( !pc.find ( "material" ) && costCalcMethod == 0)
         costCalcMethod = 1;
   }

   if ( costCalcMethod == 1 )
      productionCost = calcProductionsCost();
   if ( costCalcMethod == 2 )
      productionCost += calcProductionsCost();

   if ( costCalcMethod != 0 ) {
      displayLogMessage ( 4, "unit %s id %d has a production cost of %d E; %d M;  %d F \n", name.c_str(), id, productionCost.energy, productionCost.material, productionCost.fuel );
   }

   pc.closeBracket ();

   if ( pc.find( "guideSortHelp") || !pc.isReading() )
      pc.addIntegerArray("guideSortHelp", guideSortHelp );

   bool hasService = false;
   for ( int w = 0; w < weapons.count; ++w ) {
      if ( weapons.weapon[w].canRefuel() )
         setFunction( ExternalAmmoTransfer );
      if ( weapons.weapon[w].service() )
         hasService = true;
   }

   if ( !hasService ) {
      features.reset( ExternalRepair );
      features.reset( ExternalEnergyTransfer );
      features.reset( ExternalMaterialTransfer );
      features.reset( ExternalFuelTransfer );
      features.reset( ExternalAmmoTransfer );
   }



   pc.openBracket ( "JumpDrive" );
   pc.addTagInteger( "Height", jumpDrive.height, choehenstufennum, heightTags, 0 );
   pc.openBracket ( "consumption" );
   jumpDrive.consumption.runTextIO ( pc, Resources(0,0,0) );
   pc.closeBracket();
   if ( jumpDrive.height || !pc.isReading() )
      jumpDrive.targetterrain.runTextIO ( pc );
   pc.addInteger( "MaxDistance", jumpDrive.maxDistance, maxint );
   pc.closeBracket();

   if ( jumpDrive.height && view )
      pc.error( "only units without radar may have a jump drive." );

   if ( !pc.isReading() || pc.find ( "ObjectsLayedByMovement" ))
      pc.addIntRangeArray ( "ObjectsLayedByMovement", objectLayedByMovement );
   else
      objectLayedByMovement.clear();

   if ( hasFunction( ContainerBaseType::IceBreaker ))
      objectLayedByMovement.push_back ( 6 );

   if ( hasFunction( ContainerBaseType::MakesTracks ))
      objectLayedByMovement.push_back ( 7 );


}

BitSet Vehicletype::convertOldFunctions( int abilities, const ASCString& location )
{
   BitSet features;
   if ( abilities & 1 )
      features.set( Sonar );
   if ( abilities & 2 )
      features.set( Paratrooper );
   if ( abilities & 4 )
      features.set( PlaceMines );
   if ( abilities & 8 )
      features.set( CruiserLanding );
   if ( abilities & 16 ) {
      features.set( ExternalRepair );
      features.set( InternalUnitRepair );
   }
   if ( abilities & 32 )
      features.set( ConquerBuildings );
   if ( abilities & 64 )
      features.set( MoveAfterAttack );
   if ( abilities & 128 )
      features.set( SatelliteView );
   if ( abilities & 256 )
      errorMessage( location + ": The features construct_ALL_buildings is not supported any more");
   if ( abilities & 512 )
      features.set( MineView );
   if ( abilities & 1024 )
      features.set( ExternalVehicleProduction );
   if ( abilities & 2048 )
      features.set( ConstructBuildings );
   if ( abilities & 4096 )
      features.set( ExternalFuelTransfer );
   if ( abilities & 8192 )
      features.set( IceBreaker );
   if ( abilities & 16384 )
      features.set( NoInairRefuelling );
   if ( abilities & 32768 )
      features.set( ExternalMaterialTransfer );
   if ( abilities & (1 << 17) )
      features.set( MakesTracks );
   if ( abilities & (1 << 18) )
      features.set( DetectsMineralResources );
   if ( abilities & (1 << 19) )
      features.set( NoReactionfire );
   if ( abilities & (1 << 20) )
      features.set( AutoRepair );
   if ( abilities & (1 << 21) )
      features.set( MatterConverter );
   // we probably need to setup the conversion matrix at this point

   if ( abilities & (1 << 22) )
      features.set( DetectsMineralResources );
   if ( abilities & (1 << 23) )
      features.set( KamikazeOnly );
   if ( abilities & (1 << 24) )
      features.set( ImmuneToMines );
   if ( abilities & (1 << 25) )
      features.set( ExternalEnergyTransfer );
   if ( abilities & (1 << 26) )
      features.set( JamsOnlyOwnField );
   if ( abilities & (1 << 27) )
      features.set( MoveWithReactionFire );
   if ( abilities & (1 << 28) )
      features.set( OnlyMoveToAndFromTransports );
   return features;
}

void  Vehicletype::paint ( Surface& s, SPoint pos, const PlayerColor& player, int direction ) const
{
   megaBlitter<ColorTransform_PlayerCol,ColorMerger_AlphaOverwrite,SourcePixelSelector_Plain,TargetPixelSelector_All>( getImage(), s, pos, player, nullParam, nullParam, nullParam );
}

void  Vehicletype::paint ( Surface& s, SPoint pos ) const
{
   megaBlitter<ColorTransform_None,ColorMerger_AlphaOverwrite,SourcePixelSelector_Plain,TargetPixelSelector_All>( getImage(), s, pos, nullParam, nullParam, nullParam, nullParam );
}


void SingleWeapon::runTextIO ( PropertyContainer& pc )
{
   pc.addTagInteger( "Type", typ, weaponTypeNum, weaponTags );
   pc.addTagInteger( "targets", targ, choehenstufennum, heightTags );
   pc.addTagInteger( "shotFrom", sourceheight, choehenstufennum, heightTags );
   pc.addInteger("MaxRange", maxdistance );
   pc.addInteger("MinRange", mindistance );
   pc.addInteger("Ammo", count );
   pc.addInteger("Punch@MaxRange", minstrength );
   pc.addInteger("Punch@MinRange", maxstrength );
   pc.addString("Sound", soundLabel, "");
   pc.addInteger("LaserRechargeRate", laserRechargeRate, 0 );
   pc.openBracket( "laserRechargeCost" );
   laserRechargeCost.runTextIO ( pc, Resources(0,0,0) );
   pc.closeBracket();

   pc.addInteger("ReactionFireShots", reactionFireShots, 1 );

   pc.openBracket("HitAccuracy" );
   {
      for ( int j = 0; j < 13; j++ )
         if ( j < 6 )
            pc.addInteger( ASCString("d")+strrr(abs(j-6)), efficiency[j] );
         else
            if ( j == 6 )
               pc.addInteger( "0", efficiency[j] );
            else
               pc.addInteger( ASCString("u")+strrr(j-6), efficiency[j] );
   }
   pc.closeBracket();

   if ( pc.isReading() ) {
      if ( pc.find ( "cantHit" )) {
         int targets_not_hittable;
         pc.addTagInteger( "cantHit", targets_not_hittable, cmovemalitypenum, unitCategoryTags );
         for ( int i = 0; i < cmovemalitypenum; i++ )
            if ( targets_not_hittable & ( 1 << i ))
               targetingAccuracy[i] = 0;
      }
      pc.openBracket("WeaponEffectiveness" );
      for ( int i = 0; i < cmovemalitypenum; i++ ) {
         if ( pc.find ( unitCategoryTags[i] ))
            pc.addInteger( unitCategoryTags[i], targetingAccuracy[i] );
      }
      pc.closeBracket();
   } else {
      pc.openBracket("WeaponEffectiveness" );
      for ( int i = 0; i < cmovemalitypenum; i++ )
         if ( targetingAccuracy[i] != 100 )
            pc.addInteger( unitCategoryTags[i], targetingAccuracy[i] );
      pc.closeBracket();
   }
   pc.addString("name", name, "");
}


void Vehicletype :: HeightChangeMethod :: runTextIO ( PropertyContainer& pc )
{
   pc.addTagInteger( "StartHeight", startHeight, choehenstufennum, heightTags );
   pc.addInteger("HeightDelta", heightDelta );
   pc.addInteger("MoveCost", moveCost, 0 );
   pc.addBool ( "CanAttack", canAttack );
   pc.addInteger("Dist", dist );
}



const int vehicleHeightChangeMethodVersion = 1;


void Vehicletype :: HeightChangeMethod :: read ( tnstream& stream )
{
   int version = stream.readInt();
   if ( version > vehicleHeightChangeMethodVersion || version < 1 ) {
      ASCString s = "invalid version for reading VehicleType :: HeightChangeMethod : ";
      s += strrr ( version );
      throw ASCmsgException ( s );
   }
   startHeight = stream.readInt();
   heightDelta = stream.readInt();
   moveCost = stream.readInt();
   canAttack = stream.readInt();
   dist = stream.readInt();
}

void Vehicletype :: HeightChangeMethod :: write ( tnstream& stream ) const
{
   stream.writeInt ( vehicleHeightChangeMethodVersion );
   stream.writeInt ( startHeight );
   stream.writeInt ( heightDelta );
   stream.writeInt ( moveCost );
   stream.writeInt ( canAttack );
   stream.writeInt ( dist );
}


/***************************************************************************
                          Preiskalkulation  -  description
                             -------------------
    begin                : So Aug 15 2004
    copyright            : (C) 2001 by Martin Bickel & Steffen Froelich
    email                : bickel@asc-hq.org

 vehicletype {
   constructioncost {
    calculationMode = [auto](default)[add][manual]
    energy = 0
    material = 0
    fuel = 0
   } constructioncost
 } vehicletYpe

Part I   -description
Part II  -beginn calculation
Part III -typecost
part IV  -weaponcost
Part V   -specialcost
Part VI  -addition
Part VII -malus

 ***************************************************************************/

//Part II beginn calculation


Resources Vehicletype :: calcProductionsCost2()
{
   Resources res;
   // Anfang -> Abschluss
   res.energy = 0;
   res.material = 0;
   res.fuel = 0;
   int typecoste = 0;
   int typecostm = 0;
   int weaponcoste = 0;
   int weaponcostm = 0;
   int specialcoste = 0;
   int specialcostm = 0;
   int movecostsize = 0;
   int rangecostsize = 0;


   // Part III typecost

   if ( movemalustyp == MoveMalusType::trooper) {
      typecoste += armor*2;
      typecostm += armor*2;
   } else
      if ( movemalustyp == MoveMalusType::light_wheeled_vehicle ) {
         typecoste += armor*6;
         typecostm += armor*6;
      } else
         if ( movemalustyp == MoveMalusType::light_tracked_vehicle || movemalustyp == MoveMalusType::medium_wheeled_vehicle ) {
            typecoste += armor*7;
            typecostm += armor*7;
         } else
            if ( movemalustyp == MoveMalusType::medium_tracked_vehicle || movemalustyp == MoveMalusType::heavy_wheeled_vehicle || movemalustyp == MoveMalusType::rail_vehicle || movemalustyp == MoveMalusType::structure) {
               typecoste += armor*8;
               typecostm += armor*8;
            } else
               if ( movemalustyp == MoveMalusType::heavy_tracked_vehicle ) {
                  typecoste += armor*9;
                  typecostm += armor*9;

               } else
                  if ( movemalustyp == MoveMalusType::hoovercraft) {
                     typecoste += armor*9;
                     typecostm += armor*9;
                  } else
                     if ( movemalustyp == MoveMalusType::light_ship ) {
                        typecoste += armor*8;
                        typecostm += armor*8;
                     } else
                        if ( movemalustyp == MoveMalusType::medium_ship ) {
                           typecoste += armor*10;
                           typecostm += armor*10;
                        } else
                           if ( movemalustyp == MoveMalusType::heavy_ship ) {
                              typecoste += armor*12;
                              typecostm += armor*12;

                           } else
                              if ( movemalustyp == MoveMalusType::light_aircraft || movemalustyp == MoveMalusType::helicopter ) {
                                 typecoste += armor*18;
                                 typecostm += armor*18;
                              } else
                                 if ( movemalustyp == MoveMalusType::medium_aircraft || movemalustyp == MoveMalusType::heavy_aircraft ) {
                                    typecoste += armor*20;
                                    typecostm += armor*20;

                                 } else {
                                    typecoste += armor*6;
                                    typecostm += armor*6;
                                 }

   // Zuschlag fuer Eisbrecher
   if ( hasFunction( IceBreaker ) ) {
      typecoste += armor *2;
      typecostm += armor *2;
   }
   // Zuschlag fuer U-Boote / Druckhuelle
   if ( height & chgetaucht ) {
      typecoste += armor*2;
      typecostm += armor*2;
   }
   // Zuschlag fuer orbitalfaehige Einheiten / Druckhlle
   if ( height & chsatellit ) {
      typecoste += armor*3;
      typecostm += armor*2;
   }
   // Zuschlag fuer hochfliegende Einheiten / Extra starke Triebwerke
   if ( height & chhochfliegend ) {
      typecoste += armor*2;
      typecostm += armor*2;
   }
   // Zuschlag fuer Transportkapazitaet
   if ( entranceSystems.size() > 0 ) {
      typecoste += maxLoadableUnits*100;
      typecostm += maxLoadableUnits*100;

      bool carrierCharge = false;

      // Zuschlag fr Flugzeugtraeger / Start- und Landeeinrichtungen
      for ( int T=0; T < entranceSystems.size(); ++T )
         if ( entranceSystems[T].container_height < chtieffliegend
               && (entranceSystems[T].height_abs & (chtieffliegend | chfliegend | chhochfliegend | chsatellit))
               && maxLoadableUnits > 2 )
            carrierCharge = true;

      if ( carrierCharge ) {
         typecoste += maxLoadableUnits*1000;
         typecostm += maxLoadableUnits*1000;
      }
   }
   // Check maximale Reichweite
   for ( int M=0; M < movement.size(); ++M ) {
      if ( movecostsize < movement[M] ) {
         movecostsize = movement[M];
      }
   }
   // Zuschlag fuer Triebwerke
   if (movecostsize > 70 ) {
      typecoste += (movecostsize-70)*15;
      typecostm += (movecostsize-70)*5;
   }

   // Zuschlag fr Flugzeugtriebwerke
   if (movecostsize > 120 ) {
      typecoste += (movecostsize-120)*10;
      typecostm += (movecostsize-120)*10;
   }
   // Zuschlag fr Hochleistungsflugzeugtriebwerke
   if (movecostsize > 170 ) {
      typecoste += (movecostsize-170)*12;
      typecostm += (movecostsize-170)*12;
   }
   // Zuschlag fr Spezialflugzeugtriebwerke
   if (movecostsize > 200 ) {
      typecoste += (movecostsize-200)*14;
      typecostm += (movecostsize-200)*14;
   }



   // Part IV - weaponcost

   if ( weapons.count > 0 ) {
      for ( int W=0; W < weapons.count; ++W ) {
         if (weapons.weapon[W].getScalarWeaponType() == cwmachinegunn && weapons.weapon[W].shootable() ) {
            weaponcoste += weapons.weapon[W].maxstrength*5;
            weaponcostm += weapons.weapon[W].maxstrength*5;
         }
         if (weapons.weapon[W].getScalarWeaponType() == cwcannonn && weapons.weapon[W].shootable() ) {
            weaponcoste += weapons.weapon[W].maxstrength*10;
            weaponcostm += weapons.weapon[W].maxstrength*10;
         }
         if (weapons.weapon[W].getScalarWeaponType() == cwbombn && weapons.weapon[W].shootable() ) {
            weaponcoste += weapons.weapon[W].maxstrength*8;
            weaponcostm += weapons.weapon[W].maxstrength*8;
         }
         if (weapons.weapon[W].getScalarWeaponType() == cwsmallmissilen && weapons.weapon[W].shootable() ) {
            weaponcoste += weapons.weapon[W].maxstrength*10;
            weaponcostm += weapons.weapon[W].maxstrength*10;
         }
         if (weapons.weapon[W].getScalarWeaponType() == cwlargemissilen && weapons.weapon[W].shootable() ) {
            weaponcoste += weapons.weapon[W].maxstrength*12;
            weaponcostm += weapons.weapon[W].maxstrength*12;
         }
         if (weapons.weapon[W].getScalarWeaponType() == cwcruisemissile && weapons.weapon[W].shootable() ) {
            weaponcoste += weapons.weapon[W].maxstrength*30;
            weaponcostm += weapons.weapon[W].maxstrength*30;
         }
         if (weapons.weapon[W].getScalarWeaponType() == cwtorpedon && weapons.weapon[W].shootable() ) {
            weaponcoste += weapons.weapon[W].maxstrength*11;
            weaponcostm += weapons.weapon[W].maxstrength*11;
         }
         if (weapons.weapon[W].getScalarWeaponType() == cwminen && weapons.weapon[W].shootable() ) {
            weaponcoste += weapons.weapon[W].maxstrength*2;
            weaponcostm += weapons.weapon[W].maxstrength*2;
         }
         if (weapons.weapon[W].getScalarWeaponType() == cwlasern && weapons.weapon[W].shootable() ) {
            weaponcoste += weapons.weapon[W].maxstrength*14;
            weaponcostm += weapons.weapon[W].maxstrength*13;
         }
         if (weapons.weapon[W].service() ) {
            weaponcoste += 1000;
            weaponcostm += 500;
         }
         if (weapons.weapon[W].canRefuel() ) {
            weaponcoste += 100;
            weaponcostm += 50;
         }
         // Check maximal Waffenreichweite
         if ( rangecostsize < weapons.weapon[W].maxdistance ) {
            rangecostsize = weapons.weapon[W].maxdistance;
         }
      }
      // Waffenreichweitenzuschlag Kurzstrecke
      if (rangecostsize > 19 ) {
         weaponcoste += (rangecostsize-10)*80;
         weaponcostm += (rangecostsize-10)*70;
      }
      // Waffenreichweitenzuschlag Mittelstrecke
      if (rangecostsize > 69 ) {
         weaponcoste += (rangecostsize-60)*150;
         weaponcostm += (rangecostsize-60)*100;
      }
      // Waffenreichweitenzuschlag Langstrecke
      if (rangecostsize > 99 ) {
         weaponcoste += (rangecostsize-90)*200;
         weaponcostm += (rangecostsize-90)*200;
      }
   }

   // Part V Specialcost
   // stealth (typecost) oder jamming (specialcost)

   if ( jamming > 0 && hasFunction( JamsOnlyOwnField ) ) {
      if (jamming < 31 ) {
         typecoste += jamming*20;  //  fuer Trooper oder eigenschaftsbedingt (klein, schnell)
         typecostm += jamming*10;
      } else {
         typecoste += jamming*50;  //  fuer alle hoeherwirkenden Stealthverfahren, Anstrich, besondere Konstruktion, tarnfeld usw.
         typecostm += jamming*30;
      }
   } else { // JAMMING
      specialcoste += jamming*200;
      specialcostm += jamming*120;
      // Armorzuschlag
      specialcostm += jamming*armor/10;
      // Bewegungszuschlag
      if (movecostsize > 70 ) {
         specialcostm += jamming*(movecostsize-70);
      }
      if (movecostsize > 120 ) {
         specialcostm += jamming*(movecostsize-120);
      }
      if (movecostsize > 170 ) {
         specialcostm += jamming*(movecostsize-170);
      }
      if (movecostsize > 200 ) {
         specialcostm += jamming*(movecostsize-200);
      }
   }

   // Baufunktionen
   if ( hasFunction( ConstructBuildings )) {
      specialcoste += 1000;
      specialcostm += 500;
   }
   if ( hasFunction( InternalVehicleProduction ) || hasFunction( ExternalVehicleProduction )) {
      specialcoste += 1000;
      specialcostm += 500;
   }
   if ( objectsBuildable.size() > 0 || objectGroupsBuildable.size() > 0 ) {
      specialcoste += 1000;
      specialcostm += 500;
   }
   // Res Search
   if ( hasFunction( DetectsMineralResources )) {
      specialcoste += digrange*150;
      specialcostm += digrange*100;
   }
   // Generator
   if ( hasFunction( MatterConverter) ) {
      specialcoste += 1000;
      specialcostm += 500;
   }
   //ParaTrooper
   if ( hasFunction( Paratrooper ) ) {
      specialcoste += armor*2 ;
      specialcostm += armor ;
   }
   // Reparaturfahrzeug
   if ( hasFunction(InternalUnitRepair) || hasFunction(ExternalRepair) ) {
      specialcoste += 1000;
      specialcostm += 500;
   }
   // Selbstreparatur / Heilung
   if ( hasFunction( AutoRepair ) ) {
      specialcoste += autorepairrate*armor / 15;
      specialcostm += autorepairrate*armor / 15;
   }
   // Radar
   if ( view > 40 ) {
      specialcoste += (view-40)*50;
      specialcostm += (view-40)*20;
   }
   if (view > 90 ) {
      specialcoste += (view-90)*100;
      specialcostm += (view-90)*100;
   }
   // Satview
   if ( hasFunction( SatelliteView ) ) {
      specialcoste += view*20;
      specialcostm += view*8;
   }
   //Sonar
   if ( hasFunction(Sonar) && (movemalustyp == MoveMalusType::trooper) ) {
      specialcoste += view*10;
      specialcostm += view*4;
   } else {
      if ( hasFunction(Sonar)) {
         specialcoste += view*20;
         specialcostm += view*10;
      }
   }
   //Move during reaction fire
   if ( hasFunction( MoveWithReactionFire )) {
      int rfweapcount = 0;
      for ( int i = 0; i < weapons.count; ++i )
         if ( weapons.weapon[i].shootable() )
            rfweapcount += weapons.weapon[i].reactionFireShots;

      specialcoste += rfweapcount * 100;
      specialcostm += rfweapcount * 50;
   }

   //move after attack
   if ( hasFunction( MoveAfterAttack ) ) {
      specialcoste += movecostsize*10;
      specialcostm += movecostsize*5;
   }

   // Part VI - Addition
   res.energy += typecoste + weaponcoste + specialcoste;
   res.material += typecostm + weaponcostm + specialcostm;

   // Part VII Abschlaege
   // keine Luftbetankung
   if ( hasFunction( NoInairRefuelling )) {
      res.energy -= typecoste/6;
      res.material -= typecostm/6;
   }

   // Kamikazeeinheiten
   if ( hasFunction( KamikazeOnly )) {
      res.energy -= (typecoste+weaponcoste)/2;
      res.material -= (typecostm+weaponcostm)/2;
   }

   // low movement
   if (movecostsize < 20 ) {
      res.energy -= typecoste/4;
      res.material -= typecostm/4;
   }

   // low movement
   if (movecostsize < 10 ) {
      res.energy -= typecoste/4;
      res.material -= typecostm/4;
   }
   // Part VIII Abschluss

   return res;
}


Resources Vehicletype :: calcProductionsCost()
{
   Resources res;
   // Anfang -> Abschluss
   res.energy = 0;
   res.material = 0;
   res.fuel = 0;
   int typecoste = 0;
   int typecostm = 0;
   int weaponcoste = 0;
   int weaponcostm = 0;
   int specialcoste = 0;
   int specialcostm = 0;
   int maxmoverange = 0;
   int maxweaponrange = 0;

   // Check Flugzeugtr�ger
   bool carrierCharge = false;
   for ( int T=0; T < entranceSystems.size(); ++T )
      if ((( entranceSystems[T].container_height < chtieffliegend && (entranceSystems[T].height_abs & (chtieffliegend | chfliegend | chhochfliegend | chsatellit))) ||
            (( entranceSystems[T].container_height & ( chfliegend | chhochfliegend | chsatellit)) && (entranceSystems[T].height_abs & ( chfliegend | chhochfliegend | chsatellit))))
            && maxLoadableUnits > 3 )
         carrierCharge = true;

   // Check maximale Bewegungsreichweite
   for ( int M=0; M < movement.size(); ++M ) {
      if ( maxmoverange < movement[M] ) {
         maxmoverange = movement[M];
      }
   }
   // Check maximale Waffenreichweite
   if ( weapons.count > 0 ) {
      for ( int WR=0; WR < weapons.count; ++WR ) {
         if ( maxweaponrange < weapons.weapon[WR].maxdistance ) {
            maxweaponrange = weapons.weapon[WR].maxdistance;
         }
      }
   }

// Part III typecost

   if ( movemalustyp == MoveMalusType::trooper) {
      typecoste += armor*2;
      typecostm += armor*2;
   } else
      if ( movemalustyp == MoveMalusType::light_wheeled_vehicle ) {
         typecoste += armor*6;
         typecostm += armor*6;
      } else
         if ( movemalustyp == MoveMalusType::light_tracked_vehicle || movemalustyp == MoveMalusType::medium_wheeled_vehicle ) {
            typecoste += armor*7;
            typecostm += armor*7;
         } else
            if ( movemalustyp == MoveMalusType::medium_tracked_vehicle || movemalustyp == MoveMalusType::heavy_wheeled_vehicle || movemalustyp == MoveMalusType::rail_vehicle || movemalustyp == MoveMalusType::structure) {
               typecoste += armor*8;
               typecostm += armor*8;
            } else
               if ( movemalustyp == MoveMalusType::heavy_tracked_vehicle ) {
                  typecoste += armor*9;
                  typecostm += armor*9;
               } else
                  if ( movemalustyp == MoveMalusType::hoovercraft) {
                     typecoste += armor*9;
                     typecostm += armor*9;
                  } else
                     if ( movemalustyp == MoveMalusType::light_ship ) {
                        typecoste += armor*8;
                        typecostm += armor*8;
                     } else
                        if ( movemalustyp == MoveMalusType::medium_ship ) {
                           typecoste += armor*10;
                           typecostm += armor*10;
                        } else
                           if ( movemalustyp == MoveMalusType::heavy_ship ) {
                              typecoste += armor*12;
                              typecostm += armor*12;
                           } else
                              if ( movemalustyp == MoveMalusType::helicopter ) {
                                 typecoste += armor*14;
                                 typecostm += armor*14;
                              } else
                                 if ( movemalustyp == MoveMalusType::light_aircraft ) {
                                    typecoste += armor*16;
                                    typecostm += armor*16;
                                 } else
                                    if ( movemalustyp == MoveMalusType::medium_aircraft || movemalustyp == MoveMalusType::heavy_aircraft ) {
                                       typecoste += armor*18;
                                       typecostm += armor*18;

                                    } else {
                                       typecoste += armor*6;
                                       typecostm += armor*6;
                                    }

   // Zuschlag fuer Eisbrecher
   if ( hasFunction( IceBreaker ) ) {
      typecoste += armor *2;
      typecostm += armor *2;
   }
   // Zuschlag fuer U-Boote / Druckhuelle
   if ( height & chgetaucht ) {
      if ( movemalustyp == MoveMalusType::light_ship || movemalustyp == MoveMalusType::medium_ship || movemalustyp == MoveMalusType::heavy_ship ) {
         typecoste += armor*2;
         typecostm += armor*2;
      } else {
         if ( movemalustyp == MoveMalusType::trooper ) {
            typecoste += armor*10;
            typecostm += armor*10;
         } else {
            typecoste += armor*6;
            typecostm += armor*6;
         }
      }
   }
   // Zuschlag fuer orbitalfaehige Einheiten / Druckhuelle
   if ( height & chsatellit ) {
      typecoste += armor*3;
      typecostm += armor*2;
   }
   // Zuschlag fuer hochfliegende Einheiten / Extra starke Triebwerke
   if ( height & chhochfliegend ) {
      typecoste += armor*2;
      typecostm += armor*2;
   }
   // Zuschlag fuer hochfliegende Einheiten / Extra starke Triebwerke
   if ( height & chfliegend ) {
      typecoste += armor*2;
      typecostm += armor*2;
   }
   // Zuschlag fuer hochfliegende Einheiten / Extra starke Triebwerke
   if ( height & chtieffliegend ) {
      typecoste += armor*2;
      typecostm += armor*2;
   }
   // Zuschlag fuer Transportkapazitaet
   if ( entranceSystems.size() > 0 ) {
      typecoste += maxLoadableUnits*100;
      typecostm += maxLoadableUnits*100;
      // Zuschlag fr Flugzeugtraeger / Start- und Landeeinrichtungen
      if ( carrierCharge ) {
         typecoste += maxLoadableUnits*1000;
         typecostm += maxLoadableUnits*1000;
      }
   }
   // Zuschlag fuer Triebwerke
   if (maxmoverange > 60 ) {
      typecoste += (maxmoverange-60)*10;
      typecostm += (maxmoverange-60)*5;
   }
   // Zuschlag fr Flugzeugtriebwerke
   if (maxmoverange > 110 ) {
      typecoste += (maxmoverange-110)*10;
      typecostm += (maxmoverange-110)*5;
   }
   // Zuschlag fr Hochleistungsflugzeugtriebwerke
   if (maxmoverange > 160 ) {
      typecoste += (maxmoverange-160)*10;
      typecostm += (maxmoverange-160)*5;
   }
   // Zuschlag fr Spezialflugzeugtriebwerke
   if (maxmoverange > 190 ) {
      typecoste += (maxmoverange-190)*10;
      typecostm += (maxmoverange-190)*5;
   }



   // Part IV - weaponcost

   if ( weapons.count > 0 ) {
      for ( int W=0; W < weapons.count; ++W ) {
         int weaponsinglecoste = 0;
         int weaponsinglecostm = 0;
         if (weapons.weapon[W].getScalarWeaponType() == cwmachinegunn || weapons.weapon[W].getScalarWeaponType() == cwsmallmissilen || weapons.weapon[W].getScalarWeaponType() == cwbombn || weapons.weapon[W].getScalarWeaponType() == cwminen) {
            if (weapons.weapon[W].getScalarWeaponType() == weapons.weapon[W].shootable() ) {
               weaponsinglecoste += weapons.weapon[W].maxstrength*5;
               weaponsinglecostm += weapons.weapon[W].maxstrength*5;
            } else {
               weaponsinglecoste += 100;
               weaponsinglecostm += 50;
            }
         }
         if (weapons.weapon[W].getScalarWeaponType() == cwlargemissilen || weapons.weapon[W].getScalarWeaponType() == cwtorpedon) {
            if (weapons.weapon[W].getScalarWeaponType() == weapons.weapon[W].shootable() ) {
               weaponsinglecoste += weapons.weapon[W].maxstrength*10;
               weaponsinglecostm += weapons.weapon[W].maxstrength*10;
            } else {
               weaponsinglecoste += 100;
               weaponsinglecostm += 50;
            }
         }
         if (weapons.weapon[W].getScalarWeaponType() == cwcannonn || weapons.weapon[W].getScalarWeaponType() == cwcruisemissile) {
            if (weapons.weapon[W].getScalarWeaponType() == weapons.weapon[W].shootable() ) {
               weaponsinglecoste += weapons.weapon[W].maxstrength*15;
               weaponsinglecostm += weapons.weapon[W].maxstrength*15;
            } else {
               weaponsinglecoste += 100;
               weaponsinglecostm += 50;
            }
         }
         if (weapons.weapon[W].getScalarWeaponType() == cwlasern && weapons.weapon[W].shootable() ) {
            weaponsinglecoste += weapons.weapon[W].maxstrength*12;
            weaponsinglecostm += weapons.weapon[W].maxstrength*10;
         }
         if (weapons.weapon[W].service() ) {
            weaponsinglecoste += 1000;
            weaponsinglecostm += 500;
         }
         if (weapons.weapon[W].canRefuel() ) {
            weaponsinglecoste += 100;
            weaponsinglecostm += 50;
         }
         // Waffenreichweitenzuschlag Kurzstrecke
         if (maxweaponrange > 19 ) {
            weaponsinglecoste += (weapons.weapon[W].maxdistance)*80;
            weaponsinglecostm += (weapons.weapon[W].maxdistance)*80;
         }
         // Waffenreichweitenzuschlag Mittelstrecke
         if (maxweaponrange > 69 ) {
            weaponsinglecoste += (weapons.weapon[W].maxdistance-60)*150;
            weaponsinglecostm += (weapons.weapon[W].maxdistance-60)*140;
         }
         // Waffenreichweitenzuschlag Langstrecke
         if (maxweaponrange > 99 ) {
            weaponsinglecoste += (weapons.weapon[W].maxdistance-90)*220;
            weaponsinglecostm += (weapons.weapon[W].maxdistance-90)*200;
         }
         // Waffenreichweitenzuschlag Interkontinental
         if (maxweaponrange > 129 ) {
            weaponsinglecoste += (weapons.weapon[W].maxdistance-120)*250;
            weaponsinglecostm += (weapons.weapon[W].maxdistance-120)*250;
         }
         //Move during reaction fire(MDRF) - Move After Attack(MAM) - No Attack After Move(NAAM) - ReactionFire(RF)
         int weaponspecial = 0;
         int weaponRF = weapons.weapon[W].reactionFireShots*weaponsinglecostm/10;
         int weaponMAM = maxmoverange*weaponsinglecostm/500;
         int weaponNAAM = weaponsinglecostm/5;
         int weaponMDRF = weapons.weapon[W].reactionFireShots*weaponsinglecostm*maxmoverange/400;

         if ( hasFunction( MoveWithReactionFire ) && weapons.weapon[W].shootable()) {
            if ( wait ) {
               if ( hasFunction( MoveAfterAttack ) ) {  // MDRF+NAAM+MAM (Defkind,Spear,Stahlschwein)
                  weaponspecial += weaponMDRF+weaponMAM-weaponNAAM;
               } else {                                 // MDRF+NAAM     (Coma, CM-U-Boote, Def-Panzer,Turrets)
                  weaponspecial += weaponMDRF-weaponNAAM;
               }
            } else {
               if ( hasFunction( MoveAfterAttack ) ) {  // MDRF+MAM      (Druk, Innocence, Skjold, PHM, Jub-O)
                  weaponspecial += weaponMDRF+weaponMAM;
               } else {                                 // MDRF          (Schiffe, fahrende Bunker, Luftabwehrpanzer/Trooper)
                  weaponspecial += weaponMDRF;
               }
            }
         } else {
            if ( wait ) {
               if ( hasFunction( MoveAfterAttack ) ) {  // NAAM+MAM      (Coma2, K5, PzH2000, Pulsar)
                  weaponspecial += weaponRF+weaponMAM-weaponNAAM;
               } else {                                 // NAAM          (Coma3, BodenCMs, Schienengesch�tze)
                  weaponspecial += weaponRF-weaponNAAM;
               }
            } else {                                    // MAM           (FAV,Flugzeug,U-Boot,WIG)
               if ( hasFunction( MoveAfterAttack ) ) {
                  weaponspecial += weaponRF+weaponMAM;
               } else {                                 // ohne alles    (Panzer/Trooper)
                  weaponspecial += weaponRF;
               }
            }
         }
         // Aufrechnung
         weaponcoste += weaponsinglecoste + weaponspecial ;
         weaponcostm += weaponsinglecostm + weaponspecial ;
      }
   }

   // Part V Specialcost
   // stealth (typecost) oder jamming (specialcost)

   if ( jamming > 0 && hasFunction( JamsOnlyOwnField ) ) {
      if (jamming < 31 ) {
         typecoste += jamming*20;  //  fuer Trooper oder eigenschaftsbedingt (klein, schnell)
         typecostm += jamming*10;
      } else {
         typecoste += jamming*50;  //  fuer alle hoeherwirkenden Stealthverfahren, Anstrich, besondere Konstruktion, tarnfeld usw.
         typecostm += jamming*30;
      }
   } else { // JAMMING
      specialcoste += jamming*200;
      specialcostm += jamming*120;
      // Armorzuschlag
      specialcostm += jamming*armor/10;
      // Bewegungszuschlag
      if (maxmoverange > 70 ) {
         specialcostm += jamming*(maxmoverange-70);
      }
      if (maxmoverange > 120 ) {
         specialcostm += jamming*(maxmoverange-120);
      }
      if (maxmoverange > 170 ) {
         specialcostm += jamming*(maxmoverange-170);
      }
      if (maxmoverange > 200 ) {
         specialcostm += jamming*(maxmoverange-200);
      }
   }

   // Baufunktionen
   if ( hasFunction( ConstructBuildings )) {
      specialcoste += 1000;
      specialcostm += 500;
   }
   if ( hasFunction( InternalVehicleProduction ) || hasFunction( ExternalVehicleProduction )) {
      specialcoste += 1000;
      specialcostm += 500;
   }
   if ( objectsBuildable.size() > 0 || objectGroupsBuildable.size() > 0 ) {
      specialcoste += 1000;
      specialcostm += 500;
   }
   // Res Search
   if ( hasFunction( DetectsMineralResources )) {
      specialcoste += digrange*150;
      specialcostm += digrange*100;
   }
   // Generator
   if ( hasFunction( MatterConverter) ) {
      specialcoste += 1000;
      specialcostm += 500;
   }
   //ParaTrooper
   if ( hasFunction( Paratrooper ) ) {
      specialcoste += armor*2 ;
      specialcostm += armor ;
   }
   // Reparaturfahrzeug
   if ( hasFunction(InternalUnitRepair) || hasFunction(ExternalRepair) ) {
      specialcoste += 1000;
      specialcostm += 500;
   }
   // Selbstreparatur / Heilung
   if ( hasFunction( AutoRepair ) ) {
      specialcoste += autorepairrate*armor / 15;
      specialcostm += autorepairrate*armor / 15;
   }
   // Radar
   if ( view > 40 ) {
      specialcoste += (view-40)*50;
      specialcostm += (view-40)*20;
   }
   if (view > 90 ) {
      specialcoste += (view-90)*100;
      specialcostm += (view-90)*100;
   }
   // Satview
   if ( hasFunction( SatelliteView ) ) {
      specialcoste += view*20;
      specialcostm += view*8;
   }
   //Sonar
   if ( hasFunction(Sonar) && (movemalustyp == MoveMalusType::trooper) ) {
      specialcoste += view*10;
      specialcostm += view*4;
   } else {
      if ( hasFunction(Sonar)) {
         specialcoste += view*20;
         specialcostm += view*10;
      }
   }
   //Move during reaction fire
//   if ( hasFunction( MoveWithReactionFire )) {
//      int rfweapcount = 0;
//      for ( int i = 0; i < weapons.count; ++i )
//         if ( weapons.weapon[i].shootable() )
//            rfweapcount += weapons.weapon[i].reactionFireShots;

//      specialcoste += rfweapcount * 100;
//      specialcostm += rfweapcount * 50;
//   }

   //move after attack
//   if ( hasFunction( MoveAfterAttack ) ) {
//      specialcoste += maxmoverange*10;
//      specialcostm += maxmoverange*5;
//   }

   // Part VI - Addition
   res.energy += typecoste + weaponcoste + specialcoste;
   res.material += typecostm + weaponcostm + specialcostm;

   // Part VII Abschlaege
   // keine Luftbetankung
   if ( hasFunction( NoInairRefuelling )) {
      res.energy -= typecoste/6;
      res.material -= typecostm/6;
   }

   // Kamikazeeinheiten
   if ( hasFunction( KamikazeOnly )) {
      res.energy -= (typecoste+weaponcoste)/2;
      res.material -= (typecostm+weaponcostm)/2;
   }

   // low movement
   if (maxmoverange < 20 ) {
      res.energy -= typecoste/4;
      res.material -= typecostm/4;
   }

   // low movement
   if (maxmoverange < 10 ) {
      res.energy -= typecoste/4;
      res.material -= typecostm/4;
   }
   // Part VIII Abschluss

   return res;
}
