/***************************************************************************
                          vehicletype.cpp  -  description
                             -------------------
    begin                : Fri Sep 29 2000
    copyright            : (C) 2000 by Martin Bickel
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

#include <algorithm>
#include "vehicletype.h"
#include "errors.h"
#include "graphicset.h"
#include "errors.h"
#include "basegfx.h"


const char*  cvehiclefunctions[cvehiclefunctionsnum]  = {
                   "sonar",
                   "paratrooper",
                   "mine-layer",
                   "trooper",
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
                   "sailing",
                   "auto repair",
                   "generator",
                   "search for mineral resources automatically",
                   "Kamikaze only",
                   "immune to mines",
                   "refuels energy" };


Vehicletype :: Vehicletype ( void )
{
   int i;

   name = NULL;
   description = NULL;
   infotext = NULL;

   memset ( &oldattack, 0, sizeof ( oldattack ));

   armor = 0;

   for ( i = 0; i < 8; i++ )
      picture[i] = NULL;
   height     = 0;
   researchid = 0;
   _terrain   = 0;
   _terrainreq = 0;
   _terrainkill = 0;
   steigung = 0;
   jamming = 0;
   view = 0;
   wait = 0;
   loadcapacity = 0;
   maxunitweight = 0;
   loadcapability = 0;
   loadcapabilityreq = 0;
   loadcapabilitynot = 0;
   id = 0;
   fuelConsumption = 0;
   functions = 0;
   for ( i = 0; i < 8; i++ )
      movement[i] = 0;
   movemalustyp = 0;
   classnum = 0;
   for ( i = 0; i < 8; i++ ) {
      classnames[i] = NULL;
      for ( int j = 0; j< 8; j++)
         classbound[i].weapstrength[j] = 0;

      classbound[i].armor = 0;
      classbound[i].techlevel = 0;
      classbound[i].techrequired[0] = 0;
      classbound[i].techrequired[1] = 0;
      classbound[i].techrequired[2] = 0;
      classbound[i].techrequired[3] = 0;
      classbound[i].eventrequired = 0;
      classbound[i].vehiclefunctions = 0;
   }

   maxwindspeedonwater = 0;
   digrange = 0;
   initiative = 0;
   _terrainnot = 0;
   _terrainreq1 = 0;
   objectsbuildablenum = 0;
   objectsbuildableid = NULL;

   weight = 0;
   bipicture = -1;
   vehiclesbuildablenum = 0;
   vehiclesbuildableid = NULL;

   buildicon = NULL;
   buildingsbuildablenum = 0;
   buildingsbuildable = NULL;
   autorepairrate = 0;

   for ( i = 0; i < 8; i++ )
      aiparam[i] = NULL;

   weapons = new UnitWeapon;
   terrainaccess = new tterrainaccess;
   filename = NULL;
   vehicleCategoriesLoadable = -1;
}

int Vehicletype :: vehicleloadable ( pvehicletype fzt ) const
{
   if (( ( (loadcapabilityreq & fzt->height) || !loadcapabilityreq ) &&
         ((loadcapabilitynot & fzt->height) == 0))
        || (fzt->functions & cf_trooper))

      if ( maxunitweight >= fzt->weight )
        return 1;
      return 0;
}


int Vehicletype::maxweight ( void ) const
{
   return weight + tank.fuel * resourceWeight[Resources::Fuel] / 1024 + tank.material * resourceWeight[Resources::Material] / 1024;
}

int Vehicletype::maxsize ( void ) const
{
   return weight;
}

#ifndef converter
extern void* generate_vehicle_gui_build_icon ( pvehicletype tnk );
#endif

const int vehicle_version = 4;


void Vehicletype :: read ( tnstream& stream )
{
   int version = stream.readInt();
   if ( version > vehicle_version || version < 1) {
      string s = "invalid version for reading vehicle: ";
      s += strrr ( version );
      throw ASCmsgException ( s );
   }


   int   j;

   name = (char*) stream.readInt();
   description = (char*) stream.readInt();
   infotext = (char*) stream.readInt();

   if ( version <= 2 ) {
      oldattack.weaponcount = stream.readChar();
      for ( j = 0; j< 8; j++ ) {
         oldattack.waffe[j].typ = stream.readWord();
         oldattack.waffe[j].targ = stream.readChar();
         oldattack.waffe[j].sourceheight = stream.readChar();
         oldattack.waffe[j].maxdistance = stream.readWord();
         oldattack.waffe[j].mindistance = stream.readWord();
         oldattack.waffe[j].count = stream.readChar();
         oldattack.waffe[j].maxstrength = stream.readChar();
         oldattack.waffe[j].minstrength = stream.readChar();
      }
   }

   productionCost.energy   = stream.readWord();
   productionCost.material = stream.readWord();
   armor = stream.readWord();
   for ( j = 0; j < 8; j++ )
       picture[j] = (void*)  stream.readInt();

   height = stream.readChar();
   researchid = stream.readWord();
   if ( version <= 2 ) {
      _terrain = stream.readInt();
      _terrainreq = stream.readInt();
      _terrainkill = stream.readInt();
   }
   steigung = stream.readChar();
   jamming = stream.readChar();
   view = stream.readWord();
   wait = stream.readChar();

   if ( version <= 2 )
      stream.readChar (); // dummy

   loadcapacity = stream.readWord();
   maxunitweight = stream.readWord();
   loadcapability = stream.readChar();
   loadcapabilityreq = stream.readChar();
   loadcapabilitynot = stream.readChar();
   id = stream.readWord();
   tank.fuel = stream.readInt();
   fuelConsumption = stream.readWord();
   tank.energy = stream.readInt();
   tank.material = stream.readInt();
   functions = stream.readInt();

   for ( j = 0; j < 8; j++ )
      movement[j] = stream.readChar();


   movemalustyp = stream.readChar();

   if ( version <= 2 )
      for ( j = 0; j < 9; j++ )
          stream.readWord( ); // dummy1

   classnum = stream.readChar();
   for ( j = 0; j < 8; j++ )
       classnames[j] = (char*) stream.readInt();

   for ( j = 0; j < 8; j++ ) {
      int k;
      for ( k = 0; k < 8; k++ )
         classbound[j].weapstrength[k] = stream.readWord();

      if ( version <= 2 )
         stream.readWord (  ); // dummy2

      classbound[j].armor = stream.readWord();
      classbound[j].techlevel = stream.readWord();
      for ( k = 0; k < 4; k++ )
         classbound[j].techrequired[k] = stream.readWord();

      classbound[j].eventrequired = stream.readChar();
      classbound[j].vehiclefunctions = stream.readInt();
      if ( version <= 2 )
         stream.readChar( ); // dummy
   }

   maxwindspeedonwater = stream.readChar();
   digrange = stream.readChar();
   initiative = stream.readInt();
   _terrainnot = stream.readInt();
   _terrainreq1 = stream.readInt();
   objectsbuildablenum = stream.readInt();
   objectsbuildableid = (int*) stream.readInt();
   weight = stream.readInt();
   terrainaccess = (pterrainaccess) stream.readInt();
   bipicture = stream.readInt();
   vehiclesbuildablenum = stream.readInt();
   vehiclesbuildableid = (int*) stream.readInt();
   buildicon = (void*) stream.readInt();
   buildingsbuildablenum = stream.readInt();
   buildingsbuildable = (Vehicletype::tbuildrange*) stream.readInt();
   weapons = (UnitWeapon*) stream.readInt();
   autorepairrate = stream.readInt();
   if ( version <= 2 )
      stream.readInt( ); // dummy

   if ( version >= 4 )
      vehicleCategoriesLoadable = stream.readInt();
   else
      vehicleCategoriesLoadable = -1;

   if (name)
      stream.readpchar( &name );

   if (description)
      stream.readpchar( &description );

   if (infotext)
      stream.readpchar( &infotext );

   int i;
   for ( i=0;i<8  ;i++ )
      if ( classnames[i] )
         stream.readpchar( &(classnames[i]) );

   if ( functions & cfautorepair )
      if ( !autorepairrate )
         autorepairrate = autorepairdamagedecrease; // which is 10

   int size;
   for (i=0;i<8  ;i++ )
      if ( picture[i] )
         if ( bipicture <= 0 )
            stream.readrlepict ( &picture[i], false, &size);
         else
            loadbi3pict_double ( bipicture, &picture[i], 1); // CGameOptions::Instance()->bi3.interpolate.units );


   if ( objectsbuildablenum ) {
      objectsbuildableid = new int [ objectsbuildablenum ];
      for ( i = 0; i < objectsbuildablenum; i++ )
         objectsbuildableid[i] = stream.readInt ( );
   }

   if ( vehiclesbuildablenum ) {
      vehiclesbuildableid = new int [ vehiclesbuildablenum ];
      for ( i = 0; i < vehiclesbuildablenum; i++ )
         vehiclesbuildableid[i] = stream.readInt() ;
   }

   weapons = new UnitWeapon;
   if ( weapons && version > 1) {
      weapons->count = stream.readInt();
      for ( j = 0; j< 16; j++ ) {
         weapons->weapon[j].set ( stream.readInt() );
         weapons->weapon[j].targ = stream.readInt();
         weapons->weapon[j].sourceheight = stream.readInt();
         weapons->weapon[j].maxdistance = stream.readInt();
         weapons->weapon[j].mindistance = stream.readInt();
         weapons->weapon[j].count = stream.readInt();
         weapons->weapon[j].maxstrength = stream.readInt();
         weapons->weapon[j].minstrength = stream.readInt();

         for ( int k = 0; k < 13; k++ )
            weapons->weapon[j].efficiency[k] = stream.readInt();

         weapons->weapon[j].targets_not_hittable = stream.readInt();

         if ( version <= 2 )
            for ( int l = 0; l < 9; l++ )
               stream.readInt(); // dummy

      }

      if ( version <= 2 )
         for ( int m = 0; m< 10; m++ )
            stream.readInt(); // dummy

   } else {
      weapons->count = oldattack.weaponcount;
      for ( i = 0; i < oldattack.weaponcount; i++ ) {
         weapons->weapon[i].set ( oldattack.waffe[i].typ );
         weapons->weapon[i].targ  = oldattack.waffe[i].targ;
         weapons->weapon[i].sourceheight  = oldattack.waffe[i].sourceheight;
         weapons->weapon[i].maxdistance  = oldattack.waffe[i].maxdistance;
         weapons->weapon[i].mindistance  = oldattack.waffe[i].mindistance;
         weapons->weapon[i].count  = oldattack.waffe[i].count;
         weapons->weapon[i].maxstrength  = oldattack.waffe[i].maxstrength;
         weapons->weapon[i].minstrength  = oldattack.waffe[i].minstrength;
         for ( int j = 0; j < 13; j++ )
            weapons->weapon[i].efficiency[j] = 100;

      }
   }

   #ifndef converter
      if ( bipicture <= 0 ) {
         TrueColorImage* zimg = zoomimage ( picture[0], fieldxsize, fieldysize, pal, 0 );
         void* pic = convertimage ( zimg, pal ) ;
         for (  i = 0; i < 6; i++ )
             picture[i] = rotatepict ( pic, directionangle[i] ); // -2 * pi / 6 * i
         delete[] pic;
         delete zimg;
      } else {
         for (  i = 1; i < 6; i++ )
            picture[i] = rotatepict ( picture[0], directionangle[i] ); // -2 * pi / 6 * i
      }
   #endif

   if ( terrainaccess ) {
      terrainaccess = new tterrainaccess;
      terrainaccess->read ( stream );
   } else {
      terrainaccess = new tterrainaccess;
      terrainaccess->terrain.set ( _terrain, 0 );
      terrainaccess->terrainreq.set ( _terrainreq, 0 );
      terrainaccess->terrainnot.set ( _terrainnot, 0 );
      terrainaccess->terrainkill.set ( _terrainkill, 0 );
   }

   if ( buildingsbuildablenum ) {
      buildingsbuildable = new tbuildrange[buildingsbuildablenum];
      for ( i = 0; i < buildingsbuildablenum; i++ ) {
         buildingsbuildable[i].from = stream.readInt();
         buildingsbuildable[i].to = stream.readInt();
      }
   }


   #ifndef converter
    buildicon = generate_vehicle_gui_build_icon ( this );
   #else
    buildicon = NULL;
   #endif

   filename = strdup ( stream.getDeviceName() );

}




void Vehicletype:: write ( tnstream& stream ) const
{
   int i,j;

   stream.writeInt ( vehicle_version );

   if ( name )
      stream.writeInt( 1 );
   else
      stream.writeInt( 0 );

   if ( description )
      stream.writeInt( 1 );
   else
      stream.writeInt( 0 );

   if ( infotext )
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
   stream.writeWord(researchid);
   stream.writeChar(steigung);
   stream.writeChar(jamming);
   stream.writeWord(view);
   stream.writeChar(wait);
   stream.writeWord(loadcapacity);
   stream.writeWord(maxunitweight);
   stream.writeChar(loadcapability);
   stream.writeChar(loadcapabilityreq);
   stream.writeChar(loadcapabilitynot);
   stream.writeWord(id );
   stream.writeInt(tank.fuel );
   stream.writeWord(fuelConsumption );
   stream.writeInt(tank.energy );
   stream.writeInt(tank.material );
   stream.writeInt(functions );
   for ( j = 0; j < 8; j++ )
       stream.writeChar( movement[j] );


   stream.writeChar(movemalustyp );
   stream.writeChar(classnum );
   for ( j = 0; j < 8; j++ )
      if ( classnames[j] )
          stream.writeInt( 1 );
      else
          stream.writeInt( 0 );


   for ( j = 0; j < 8; j++ ) {
      int k;
      for ( k = 0; k < 8; k++ )
         stream.writeWord(classbound[j].weapstrength[k]);

      stream.writeWord(classbound[j].armor);
      stream.writeWord(classbound[j].techlevel );
      for ( k = 0; k < 4; k++ )
         stream.writeWord(classbound[j].techrequired[k]);

      stream.writeChar(classbound[j].eventrequired );
      stream.writeInt(classbound[j].vehiclefunctions );
   }

   stream.writeChar(maxwindspeedonwater );
   stream.writeChar(digrange );
   stream.writeInt(initiative );
   stream.writeInt(_terrainnot );
   stream.writeInt(_terrainreq1 );
   stream.writeInt(objectsbuildablenum );

   if ( objectsbuildableid )
      stream.writeInt( 1 );
   else
      stream.writeInt( 0 );

   stream.writeInt(weight );
   if ( terrainaccess )
      stream.writeInt( 1 );
   else
      stream.writeInt( 0 );

   stream.writeInt(bipicture );
   stream.writeInt(vehiclesbuildablenum );
   if ( vehiclesbuildableid )
      stream.writeInt( 1 );
   else
      stream.writeInt( 0 );

   if ( buildicon )
      stream.writeInt( 1 );
   else
      stream.writeInt( 0 );

   stream.writeInt(buildingsbuildablenum );
   if ( buildingsbuildable )
      stream.writeInt( 1 );
   else
      stream.writeInt( 0 );

   if ( weapons )
      stream.writeInt( 1 );
   else
      stream.writeInt( 0 );

   stream.writeInt( autorepairrate );

   stream.writeInt( vehicleCategoriesLoadable );


   if (name)
      stream.writepchar( name );

   if (description)
      stream.writepchar( description );

   if (infotext)
      stream.writepchar( infotext );

   for (i=0; i<8; i++)
      if ( classnames[i] )
         stream.writepchar( classnames[i] );

   if ( bipicture <= 0 )
      for (i=0;i<8  ;i++ )
         if ( picture[i] )
            stream.writedata ( picture[i], getpicsize2 ( picture[i] ) );

   for ( i = 0; i < objectsbuildablenum; i++ )
      stream.writeInt ( objectsbuildableid[i] );

   for ( i = 0; i < vehiclesbuildablenum; i++ )
      stream.writeInt ( vehiclesbuildableid[i] );

   stream.writeInt(weapons->count );
   for ( j = 0; j< 16; j++ ) {
      stream.writeInt(weapons->weapon[j].gettype( ));
      stream.writeInt(weapons->weapon[j].targ);
      stream.writeInt(weapons->weapon[j].sourceheight );
      stream.writeInt(weapons->weapon[j].maxdistance );
      stream.writeInt(weapons->weapon[j].mindistance );
      stream.writeInt(weapons->weapon[j].count );
      stream.writeInt(weapons->weapon[j].maxstrength );
      stream.writeInt(weapons->weapon[j].minstrength );

      for ( int k = 0; k < 13; k++ )
         stream.writeInt(weapons->weapon[j].efficiency[k] );

      stream.writeInt(weapons->weapon[j].targets_not_hittable );
   }

   if ( terrainaccess )
      terrainaccess->write ( stream );

   for ( i = 0; i < buildingsbuildablenum; i++ ) {
      stream.writeInt( buildingsbuildable[i].from );
      stream.writeInt( buildingsbuildable[i].to );
   }

}



Vehicletype :: ~Vehicletype ( )
{
   int i;

   if ( name ) {
      delete[] name;
      name = NULL;
   }

   if ( description ) {
      delete[] description;
      description = NULL;
   }

   if ( infotext ) {
      delete[] infotext;
      infotext = NULL;
   }

   for ( i = 0; i < 8; i++ )
      if ( picture[i] ) {
         delete picture[i];
         picture[i] = NULL;
      }

   for ( i = 0; i < 8; i++ )
      if ( classnames[i] ) {
         delete[] classnames[i];
         classnames[i] = NULL;
      }

   if ( objectsbuildableid ) {
      delete[] objectsbuildableid;
      objectsbuildableid = NULL;
   }

   if ( terrainaccess ) {
      delete[] terrainaccess;
      terrainaccess = NULL;
   }

   if ( buildicon ) {
      delete buildicon;
      buildicon = NULL;
   }

   if ( buildingsbuildable ) {
      delete[] buildingsbuildable;
      buildingsbuildable = NULL;
   }

   if ( weapons ) {
      delete weapons;
      weapons = NULL;
   }

   for ( i = 0; i < 8; i++ )
      if ( aiparam[i] ) {
         delete aiparam[i];
         aiparam[i] = NULL;
      }
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

ASCString SingleWeapon::getName ( void )
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
   memset ( weapon, 0, sizeof ( weapon ));
}



