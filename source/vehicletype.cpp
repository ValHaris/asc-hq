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
#include "vehicletype.h"
#include "errors.h"
#include "graphicset.h"
#include "errors.h"
#include "basegfx.h"
#include "terraintype.h"
#include "objecttype.h"
#include "textfileparser.h"
#include "textfiletags.h"
#include "textfile_evaluation.h"


const char*  cvehiclefunctions[cvehiclefunctionsnum]  = {
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
                   "move with reaction fire on" };



Vehicletype :: Vehicletype ( void )
{
   recommendedAIJob = AiParameter::job_undefined;

   int i;

   armor = 0;

   for ( i = 0; i < 8; i++ )
      picture[i] = NULL;
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

   buildicon = NULL;
   autorepairrate = 0;

   heightChangeMethodNum = 0;

   for ( i = 0; i < 8; i++ )
      aiparam[i] = NULL;

}


int Vehicletype::maxweight ( void ) const
{
   return weight + tank.fuel * resourceWeight[Resources::Fuel] / 1000 + tank.material * resourceWeight[Resources::Material] / 1000;
}

int Vehicletype::maxsize ( void ) const
{
   return weight;
}

#ifndef converter
extern void* generate_vehicle_gui_build_icon ( pvehicletype tnk );
#endif

const int vehicle_version = 16;



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
   for ( j = 0; j < 8; j++ )
       picture[j] = (void*)  stream.readInt();

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
   id = stream.readWord();
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
      buildicon = (void*) stream.readInt();

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

   int size;
   for (i=0;i<8  ;i++ )
      if ( picture[i] ) {
         // if ( bipicture <= 0 )
            stream.readrlepict ( &picture[i], false, &size);
            bipicture = 0;
         /* else
            loadbi3pict_double ( bipicture, &picture[i], 1); // CGameOptions::Instance()->bi3.interpolate.units );
            */
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

      }

      if ( version <= 2 )
         for ( int m = 0; m< 10; m++ )
            stream.readInt(); // dummy

   }

   #ifndef converter
   setupPictures();
   #endif

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

   if ( version >= 13 ) {
      int w;
      stream.readrlepict ( &buildicon,  false, &w);
   } else {
      #ifndef converter
       buildicon = generate_vehicle_gui_build_icon ( this );
      #else
       buildicon = NULL;
      #endif
   }

   if ( version >= 14 )
      cargoMovementDivisor = stream.readInt();
}

void Vehicletype::setupPictures()
{
   #ifndef converter
      if ( !picture[0] )
         fatalError ( "The vehicletype " + getName() + " (ID: " + strrr( id ) + ") has an invalid picture" );

      if ( bipicture <= 0 ) {
         int count = 0;
         for ( int i = 0; i < 6; i++ )
            if ( picture[i] )
               ++count;
               
         if ( count == 6 )
            return;

         TrueColorImage* zimg = zoomimage ( picture[0], fieldxsize, fieldysize, pal, 0 );
         void* pic = convertimage ( zimg, pal ) ;
         for ( int i = 1; i < 6; i++ )
             picture[i] = rotatepict ( pic, directionangle[i] );
         delete[] picture[0];
         picture[0] = pic;
         delete zimg;
      } else {
         for ( int i = 1; i < 6; i++ )
            if ( !picture[i] )
               picture[i] = rotatepict ( picture[0], directionangle[i] );
      }
   #endif
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

   for ( j = 0; j < 8; j++ )
      if ( picture[j] )
         stream.writeInt( 1 );
      else
         stream.writeInt( 0 );

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
   stream.writeWord(id );
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

   // if ( bipicture <= 0 )
      for (i=0;i<8  ;i++ )
         if ( picture[i] )
            stream.writeImage ( picture[i], false );

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

      for ( int k = 0; k < 13; k++ )
         stream.writeInt(weapons.weapon[j].efficiency[k] );

      stream.writeInt ( cmovemalitypenum );
      for ( int i = 0; i < cmovemalitypenum; i++ )
         stream.writeInt(weapons.weapon[j].targetingAccuracy[i] );

      stream.writeInt( weapons.weapon[j].laserRechargeRate );
      weapons.weapon[j].laserRechargeCost.write( stream );

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

   stream.writerlepict( buildicon );

   stream.writeInt ( cargoMovementDivisor );
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
   int i;

   for ( i = 0; i < 8; i++ )
      if ( picture[i] ) {
         delete picture[i];
         picture[i] = NULL;
      }

   if ( buildicon ) {
      delete buildicon;
      buildicon = NULL;
   }

   for ( i = 0; i < 8; i++ )
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
      if ( service() )
         s = cwaffentypen[cwservicen];
      else
         s = "undefined";

   return s;
}

UnitWeapon :: UnitWeapon ( void )
{
   count = 0;
}


void Vehicletype::runTextIO ( PropertyContainer& pc )
{
   pc.addString( "Description", description);

   pc.addInteger( "Armor", armor );

   ASCString fn;
   if ( filename.empty() ) {
      fn += "vehicle";
      fn += strrr(id);
   } else
      fn = extractFileName_withoutSuffix( filename );

   pc.addImage( "Picture", picture[0], fn );

   pc.openBracket ( "ConstructionCost" );
   productionCost.runTextIO ( pc );
   pc.closeBracket ();

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

   pc.addInteger("CargoMovementDivisor", cargoMovementDivisor, 2 );

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

   ContainerBaseType::runTextIO ( pc );

   techDependency.runTextIO( pc, strrr(id) );


   setupPictures();
   #ifndef converter
   buildicon = generate_vehicle_gui_build_icon ( this );
   #endif
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



