/***************************************************************************
                          vehicletype.cpp  -  description
                             -------------------
    begin                : Fri Sep 29 2000
    copyright            : (C) 2000 by Martin Bickel
    email                : bickel@asc-hq.org
 ***************************************************************************/

/*! \file vehicletype.cpp
    \brief The #VehicleType class
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
#include "errors.h"
#include "graphicset.h"
#include "basicmessages.h" 
#include "errors.h"
#include "basegfx.h"
#include "terraintype.h"
#include "objecttype.h"
#include "textfileparser.h"
#include "textfiletags.h"
#include "textfile_evaluation.h"
#include "graphics/blitter.h"


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
                   NULL };



Vehicletype :: Vehicletype ( void )
{
   recommendedAIJob = AiParameter::job_undefined;

   int i;

   armor = 0;

   height     = 0;
   cargoMovementDivisor = 2;
   wait = 0;
   fuelConsumption = 0;
   functions = 0;

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

}


int Vehicletype::maxsize ( void ) const
{
   return weight;
}

#ifndef converter
extern void* generate_vehicle_gui_build_icon ( pvehicletype tnk );
#endif

const int vehicle_version = 21;



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

   productionCost.energy   = stream.readWord();
   productionCost.material = stream.readWord();
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
   tank.fuel = stream.readInt();
   fuelConsumption = stream.readWord();
   tank.energy = stream.readInt();
   tank.material = stream.readInt();
   functions = stream.readInt();

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

   if ( functions & cfautorepair )
      if ( !autorepairrate )
         autorepairrate = autorepairdamagedecrease; // which is 10

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

   if ( version >= 13 ) 
      buildicon.read( stream );
   else {
      #ifndef converter
       buildicon = generate_gui_build_icon ( this );
      #endif
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

   stream.writeWord( productionCost.energy );
   stream.writeWord( productionCost.material );
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
   stream.writeInt(tank.fuel );
   stream.writeWord(fuelConsumption );
   stream.writeInt(tank.energy );
   stream.writeInt(tank.material );
   stream.writeInt(functions );
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
   for ( int i = 0; i < heightChangeMethodNum; i++ )
      heightChangeMethod[i].write( stream );

   techDependency.write( stream );

   buildicon.write(stream);

   stream.writeFloat ( cargoMovementDivisor );

   stream.writeString( movementSoundLabel );
   stream.writeString( killSoundLabel );
}


const ASCString& Vehicletype::getName( ) const
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
int SingleWeapon::getScalarWeaponType(void) const {
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

void SingleWeapon::set ( int type )
{
   typ = type;
}

ASCString SingleWeapon::getName ( void ) const
{
   ASCString s;

   int k = getScalarWeaponType();
   if ( k < cwaffentypennum && k >= 0 )
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
      case cwcruisemissile: return "weap-cruisemissile";
      case cwbombn: return "weap-bomb";
      case cwairmissilen: return "weap-bigmissile";
      case cwgroundmissilen: return "weap-smallmissile";
      case cwtorpedon: return "weap-torpedo";
      case cwmachinegunn: return "weap-machinegun";
      case cwcannonn: return "weap-cannon";
      case cwminen: return "weap-mine";
      default: return "weap-undefined";
   };
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

   pc.addImage( "Picture", image, fn );
   if( image.w() < fieldsizex || image.h() < fieldsizey )
      image.strech( fieldsizex, fieldsizey );

   pc.addTagInteger( "Height", height, choehenstufennum, heightTags );
   pc.addBool ( "WaitFortack", wait );
   pc.openBracket( "Tank" );
     tank.runTextIO ( pc );
   pc.closeBracket();
   pc.addInteger( "FuelConsumption", fuelConsumption );
   pc.addTagInteger( "Abilities", functions, cvehiclefunctionsnum, vehicleAbilities );
   pc.addIntegerArray ( "Movement", movement );
   for ( vector<int>::iterator i = movement.begin(); i != movement.end(); i++ )
      if ( *i > 255 )
         *i = 255;

   pc.addNamedInteger ( "Category", movemalustyp, cmovemalitypenum, unitCategoryTags );
   pc.addInteger("MaxSurvivableStorm", maxwindspeedonwater, 255 );
   pc.addInteger("ResourceDrillingRange", digrange, 0 );
   pc.addInteger("SelfRepairRate", autorepairrate, 0 );
   if ( pc.find ( "WreckageObject" ) || !pc.isReading() )
      pc.addIntegerArray("WreckageObject", wreckageObject );

   pc.addDFloat("CargoMovementDivisor", cargoMovementDivisor, 2 );

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
   pc.closeBracket();

   pc.openBracket ( "Weapons");
    pc.addInteger("Initiative", initiative );
    pc.addInteger("Number", weapons.count );
    for ( int i = 0; i < weapons.count; i++ ) {
        pc.openBracket ( ASCString("Weapon")+strrr(i) );
        weapons.weapon[i].runTextIO( pc );
        pc.closeBracket();
        if ( functions & cfno_reactionfire )
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
   int costCalcMethod;
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


   #ifndef converter
   buildicon = generate_gui_build_icon ( this );
   #endif
}

void  Vehicletype::paint ( Surface& s, SPoint pos, int player, int direction ) const
{
   megaBlitter<ColorTransform_PlayerCol,ColorMerger_AlphaOverwrite,SourcePixelSelector_Plain,TargetPixelSelector_All>( getImage(), s, pos, player, nullParam, nullParam, nullParam );
}


void SingleWeapon::runTextIO ( PropertyContainer& pc )
{
   pc.addTagInteger( "Type", typ, cwaffentypennum, weaponTags );
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

   pc.openBracket("HitAccuracy" ); {
     for ( int j = 0; j < 13; j++ )
        if ( j < 6 )
           pc.addInteger( ASCString("d")+strrr(abs(j-6)), efficiency[j] );
        else
           if ( j == 6 )
              pc.addInteger( "0", efficiency[j] );
           else
              pc.addInteger( ASCString("u")+strrr(j-6), efficiency[j] );
   } pc.closeBracket();

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
    copyright            : (C) 2001 by Martin Bickel & Steffen Fröhlich
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
Part VI  -addierung
Part VII -Abschläge

 ***************************************************************************/

//Part II beginn calculation


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
		int movecostsize = 0;
		int rangecostsize = 0;
		

// Part III typecost

		if ( movemalustyp == MoveMalusType::trooper) {
			typecoste += armor*2;
			typecostm += armor*2;
		} else
		if ( movemalustyp == MoveMalusType::light_tracked_vehicle || movemalustyp == MoveMalusType::medium_tracked_vehicle || movemalustyp == MoveMalusType::heavy_tracked_vehicle || movemalustyp == MoveMalusType::light_wheeled_vehicle || movemalustyp == MoveMalusType::medium_wheeled_vehicle || movemalustyp == MoveMalusType::heavy_wheeled_vehicle || movemalustyp == MoveMalusType::rail_vehicle || movemalustyp == MoveMalusType::structure) {
			typecoste += armor*8;
			typecostm += armor*8;
		} else
		if ( movemalustyp == MoveMalusType::hoovercraft) {
			typecoste += armor*9;
			typecostm += armor*9;
		} else
		if ( movemalustyp == MoveMalusType::light_ship || movemalustyp == MoveMalusType::medium_ship || movemalustyp == MoveMalusType::heavy_ship ) {
			typecoste += armor*10;
			typecostm += armor*10;
		} else
		if ( movemalustyp == MoveMalusType::light_aircraft || movemalustyp == MoveMalusType::medium_aircraft || movemalustyp == MoveMalusType::heavy_aircraft || movemalustyp == MoveMalusType::helicopter ) {
			typecoste += armor*16;
			typecostm += armor*16;
		} else {
			typecoste += armor*5;
			typecostm += armor*5;
		}

		// Zuschlag für Eisbrecher
		if ( functions & cficebreaker ) {
			typecoste += armor *2;
			typecostm += armor *2;
		}
		// Zuschlag für U-Boote / Druckhülle
		if ( height & chgetaucht ) {
			typecoste += armor*2;
			typecostm += armor*2;
		}
		// Zuschlag für orbitalfähige Einheiten / Druckhülle
		if ( height & chsatellit ) {
			typecoste += armor*3;
			typecostm += armor*2;
		}
		// Zuschlag für hochfliegende Einheiten / Extra starke Triebwerke
		if ( height & chhochfliegend ) {
			typecoste += armor*2;
			typecostm += armor*2;
		}
		// Zuschlag für Transportkapazität
		if ( entranceSystems.size() > 0 ) {
			typecoste += maxLoadableUnits*100;
			typecostm += maxLoadableUnits*100;

			// Zuschlag für Flugzeugträger / Start- und Landeeinrichtungen
			for ( int T=0; T < entranceSystems.size(); ++T ) {
				if ( entranceSystems[T].container_height == chfahrend && entranceSystems[T].height_abs == chtieffliegend && maxLoadableUnits > 2 ) {
					typecoste += maxLoadableUnits*800;
					typecostm += maxLoadableUnits*800;
				}
			}
		}
		// Check maximale Reichweite
		for ( int M=0; M < movement.size(); ++M ) {
			if ( movecostsize < movement[M] ) {
				movecostsize = movement[M];
			}
		}
		// Zuschlag für Triebwerke
		if (movecostsize > 70 ) {
			typecoste += (movecostsize-70)*30;
			typecostm += (movecostsize-70)*15;
		}
		
		// Zuschlag für Flugzeugtriebwerke
		if (movecostsize > 120 ) {
			typecoste += (movecostsize-120)*10;
			typecostm += (movecostsize-120)*5;
		}
		// Zuschlag für Hochleistungsflugzeugtriebwerke
		if (movecostsize > 170 ) {
			typecoste += (movecostsize-170)*5;
			typecostm += (movecostsize-170)*5;
		}
		// Zuschlag für Spezialflugzeugtriebwerke
		if (movecostsize > 200 ) {
			typecoste += (movecostsize-200)*10;
			typecostm += (movecostsize-200)*10;
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
				if (weapons.weapon[W].getScalarWeaponType() == cwgroundmissilen && weapons.weapon[W].shootable() ) {
					weaponcoste += weapons.weapon[W].maxstrength*10;
					weaponcostm += weapons.weapon[W].maxstrength*10;
				}
				if (weapons.weapon[W].getScalarWeaponType() == cwairmissilen && weapons.weapon[W].shootable() ) {
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
				weaponcostm += (rangecostsize-10)*50;
			}
			// Waffenreichweitenzuschlag Mittelstrecke
			if (rangecostsize > 69 ) {
				weaponcoste += (rangecostsize-60)*100;
				weaponcostm += (rangecostsize-60)*80;
			}
			// Waffenreichweitenzuschlag Langstrecke
			if (rangecostsize > 99 ) {
				weaponcoste += (rangecostsize-90)*110;
				weaponcostm += (rangecostsize-90)*100;
			}
		}

// Part V Specialcost
		// stealth (typecost) oder jamming (specialcost)

		if ( jamming > 0 && (functions & cfownFieldJamming) ) {
			if (jamming < 31 ) {
				typecoste += jamming*20;  //  für Trooper oder eigenschaftsbedingt (klein, schnell)
				typecostm += jamming*10;
			} else {
				typecoste += jamming*50;  //  für alle höherwirkenden Stealthverfahren, Anstrich, besondere Konstruktion, tarnfeld usw.
				typecostm += jamming*30;
			}
		} else {
			specialcoste += jamming*170;
			specialcostm += jamming*150;
		}

		// Baufunktionen
		if ( (functions & cfputbuilding) || (functions & cfspecificbuildingconstruction ) ) {
			specialcoste += 1000;
			specialcostm += 500;
		}
		if ( functions & cfvehicleconstruction ) {
			specialcoste += 1000;
			specialcostm += 500;
		}
		if ( objectsBuildable.size() > 0 || objectGroupsBuildable.size() > 0 ) {
			specialcoste += 1000;
			specialcostm += 500;
		}
      // Res Search
		if ( (functions & cfmanualdigger) || (functions & cfautodigger) ) {
			specialcoste += digrange*150;
			specialcostm += digrange*100;
		}
      // Generator
		if ( functions & cfgenerator ) {
			specialcoste += 1000;
			specialcostm += 500;
		}
      //ParaTrooper
		if ( functions & cfparatrooper ) {
			specialcoste += armor*2 ;
			specialcostm += armor ;
		}
		// Reparaturfahrzeug
		if ( functions & cfrepair ) {
			specialcoste += 1000;
			specialcostm += 500;
		}
		// Selbstreparatur / Heilung
		if ( functions & cfautorepair ) {
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
		if ( functions & cfsatellitenview ) {
			specialcoste += view*20;
			specialcostm += view*8;
		}
		//Sonar
		if ( (functions & cfsonar) && (movemalustyp == MoveMalusType::trooper) ) {
			specialcoste += view*10;
			specialcostm += view*4;
		} else {
			if (functions & cfsonar) {
				specialcoste += view*20;
				specialcostm += view*10;
			}
		}
		//Move during reaction fire
		if ( functions & cfmovewithRF ) {
         int rfweapcount = 0;
         for ( int i = 0; i < weapons.count; ++i )
            if ( weapons.weapon[i].shootable() )
               rfweapcount += weapons.weapon[i].reactionFireShots;

			specialcoste += rfweapcount * 100;
			specialcostm += rfweapcount * 50;
		}

		//move after attack
		if ( functions & cf_moveafterattack ) {
			specialcoste += movecostsize*10;
			specialcostm += movecostsize*5;
		}

// Part VI - Addition
		res.energy += typecoste + weaponcoste + specialcoste;
		res.material += typecostm + weaponcostm + specialcostm;

// Part VII Abschläge
		// keine Luftbetankung
		if (functions & cfnoairrefuel) {
			res.energy -= typecoste/6;
			res.material -= typecostm/6;
		}

		// Kamikazeeinheiten
		if (functions & cfkamikaze) {
			res.energy -= (typecoste+weaponcoste)/2;
			res.material -= (typecostm+weaponcostm)/2;
		}

		// low movement
		if (movecostsize < 20 ) {
			res.energy -= typecoste/5;
			res.material -= typecostm/6;
		}

		// low movement
		if (movecostsize < 10 ) {
			res.energy -= typecoste/5;
			res.material -= typecostm/6;
		}
// Part VIII Abschluss

	return res;
}


