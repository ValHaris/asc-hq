/***************************************************************************
                          vehicle.cpp  -  description
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
#include "research.h"
#include "vehicletype.h"
#include "vehicle.h"
#include "buildingtype.h"
#include "viewcalculation.h"
#include "errors.h"
#include "basegfx.h"
#include "graphicset.h"
#include "errors.h"
#include "gameoptions.h"
#include "basegfx.h"
#include "spfst.h"





const float repairEfficiencyVehicle[resourceTypeNum*resourceTypeNum] = { 0,  0,  0,
                                                                         0,  0.5, 0,
                                                                         0.5, 0,  1 };




Vehicle :: Vehicle (  )
          : ContainerBase ( NULL, NULL, 0 ), repairEfficiency ( repairEfficiencyVehicle ), typ ( NULL ), reactionfire ( this )
{
}

Vehicle :: Vehicle ( const Vehicle& v )
          : ContainerBase ( NULL, NULL, 0 ), repairEfficiency ( repairEfficiencyVehicle ), typ ( NULL ), reactionfire ( this )
{
}


Vehicle :: Vehicle ( const Vehicletype* t, pmap actmap, int player )
          : ContainerBase ( t, actmap, player ), repairEfficiency ( repairEfficiencyVehicle ), typ ( t ), reactionfire ( this )
{
   viewOnMap = false;

   if ( player > 8 )
      fatalError ( "Vehicle :: Vehicle ; invalid player ");

   init();

   gamemap->player[player].vehicleList.push_back ( this );
   gamemap->unitnetworkid++;
   networkid = gamemap->unitnetworkid;
}


Vehicle :: ~Vehicle (  )
{
   if ( viewOnMap && gamemap ) {
      removeview();
      viewOnMap = false;
   }

   delete[] weapstrength;
   weapstrength = NULL;

   delete[] ammo;
   ammo = NULL;

   if ( gamemap ) {
      int c = color/8;

      tmap::Player::VehicleList::iterator i = find ( gamemap->player[c].vehicleList.begin(), gamemap->player[c].vehicleList.end(), this );
      if ( i != gamemap->player[c].vehicleList.end() )
         gamemap->player[c].vehicleList.erase ( i );

      for ( int i = 0; i < 8; i++ )
         gamemap->player[i].queuedEvents++;
   }

   for ( int i = 0; i < 32; i++ )
      if ( loading[i] )
         delete loading[i];

   pfield fld = gamemap->getField( xpos, ypos);
   if ( fld && fld->vehicle  == this )
     fld->vehicle = NULL;

}




void Vehicle :: init ( void )
{
   for ( int j = 0; j < 32; j++ )
       loading[j] = NULL;

   xpos = -1;
   ypos = -1;

   weapstrength = new int[16];
   ammo = new int[16];
   for ( int i = 0; i < 16; i++ ) {
      weapstrength[i] = 0;
      ammo[i] = 0;
   }

   klasse = 0;
   damage = 0;

   experience = 0;
   attacked = 0;

   if ( typ ) {
      height = 1 << log2( typ->height );

      // These are the preferred levels of height
      if (typ->height & chfahrend )
         height = chfahrend;
      else
         if (typ->height & chschwimmend )
            height = chschwimmend;

      armor = typ->armor * typ->classbound[klasse].armor / 1024;
      functions = typ->functions & typ->classbound[klasse].vehiclefunctions;
      for ( int m = 0; m < typ->weapons.count ; m++)
         weapstrength[m] = typ->weapons.weapon[m].maxstrength * typ->classbound[klasse].weapstrength[typ->weapons.weapon[m].getScalarWeaponType()] / 1024;

      setMovement ( typ->movement[log2(height)] );
   } else {
      height = 0;
      armor = 0;
      functions = 0;

      setMovement ( 0 );
   }

   direction = 0;
   xpos = -1;
   ypos = -1;
   energyUsed = 0;
   connection = 0;
   networkid = -1;
   reactionfire.status = ReactionFire::off;
   reactionfire.enemiesAttackable = 0;
   generatoractive = 0;

   for ( int a = 0; a < 8 ; a++ )
      aiparam[a] = NULL;
}


bool Vehicle :: canRepair( void )
{
   return (functions & cfrepair) || typ->autorepairrate ;
}

int Vehicle :: putResource ( int amount, int resourcetype, int queryonly, int scope )
{
   if ( amount < 0 ) {
      return -getResource( -amount, resourcetype, queryonly, scope );
   } else {
      int tostore = min ( getMaxResourceStorageForWeight ( resourcetype ) - tank.resource(resourcetype), amount);
      if ( !queryonly )
         tank.resource(resourcetype) += tostore;

      return tostore;
   }
}

int Vehicle :: getResource ( int amount, int resourcetype, int queryonly, int scope )
{
   if ( amount < 0 ) {
      return -putResource( -amount, resourcetype, queryonly, scope );
   } else {
      int toget = min ( tank.resource(resourcetype), amount);
      if ( !queryonly )
         tank.resource(resourcetype) -= toget;

      return toget;
   }
}


/*
void Vehicle :: clone ( pvehicle src, pmap actmap )
{

   typ = src->typ;
   baseType = src->baseType;

   color = src->color;
   damage = src->damage;
   tank = src->tank;
   memcpy ( ammo , src->ammo, 16*sizeof ( int  ));
   memcpy ( weapstrength , src->weapstrength, 16*sizeof ( int ));
   experience = src->experience;
   attacked = src->attacked;
   height = src->height;
   _movement = src->_movement;
   direction = src->direction;
   xpos = src->xpos;
   ypos = src->ypos;
   energyUsed = src->energyUsed;
   connection = src->connection;
   klasse = src->klasse;
   armor = src->armor;
   networkid = src->networkid;
   name = src->name ;
   functions = src->functions;
   reactionfire.status = src->reactionfire.status;
   reactionfire.enemiesAttackable = src->reactionfire.enemiesAttackable;
   generatoractive = src->generatoractive;

   for ( int i = 0; i < 32; i++ )
     if ( src->loading[i] )
        loading[i] = new Vehicle ( src->loading[i], NULL );
     else
        loading[i] = NULL;

   for ( int j = 0; j < 8; j++ )
      if ( src->aiparam[j] )
         aiparam[j] = new AiParameter ( *src->aiparam[j] );
      else
         aiparam[j] = NULL;

   if ( actmap )
      actmap->chainunit ( this );
}
*/

void Vehicle :: setGeneratorStatus ( bool status )
{
   if ( functions & cfgenerator ) {
      generatoractive = status;
      if ( status )
         tank.energy = typ->tank.energy - energyUsed;
      else {
         int endiff = typ->tank.energy- tank.energy - energyUsed;
         if ( tank.fuel < endiff * generatortruckefficiency )
            endiff = tank.fuel / generatortruckefficiency;

         tank.fuel -= endiff * generatortruckefficiency ;
         energyUsed += endiff;
         tank.energy = 0;
      }
   } else
     generatoractive = 0;
}


void Vehicle :: setup_classparams_after_generation ( void )
{
      armor = typ->armor * typ->classbound[klasse].armor / 1024;
      if ( typ->weapons.count ) {
         for ( int m = 0; m < typ->weapons.count ; m++) {
            ammo[m] = 0;
            weapstrength[m] = typ->weapons.weapon[m].maxstrength *
	      typ->classbound[klasse].weapstrength[typ->weapons.weapon[m].getScalarWeaponType()] / 1024;
         }
      }

      if ( typ->classnum )
        functions = typ->functions & typ->classbound[klasse].vehiclefunctions;
      else
        functions = typ->functions;

      attacked = true;

}


int Vehicle::cargo ( void ) const
{
   int w = 0;
   if ( typ->loadcapacity > 0)
      for (int c = 0; c <= 31; c++)
         if ( loading[c] )
            w += loading[c]->weight();
   return w;
}

int Vehicle::weight( void ) const
{
   return typ->weight + tank.fuel * resourceWeight[Resources::Fuel] / 1024 + tank.material * resourceWeight[Resources::Material] / 1024 + cargo();
}

int Vehicle::size ( void )
{
   return typ->weight;
}

void Vehicle::transform ( const pvehicletype type )
{
   typ = type;

   tank.fuel = typ->tank.fuel;
   tank.material = typ->tank.material;
   tank.energy = 0;
   generatoractive = 0;

   for ( int m = 0; m < typ->weapons.count ; m++) {
      ammo[m] = typ->weapons.weapon[m].count;
      weapstrength[m] = typ->weapons.weapon[m].maxstrength;
   }
   armor = typ->armor;
   klasse = 255;
}

void Vehicle :: postRepair ( int oldDamage )
{
   for ( int i = 0; i < experienceDecreaseDamageBoundaryNum; i++)
      if ( oldDamage > experienceDecreaseDamageBoundaries[i] && damage < experienceDecreaseDamageBoundaries[i] )
         if ( experience > 0 )
            experience-=1;
}


/*
void Vehicle :: repairunit(pvehicle vehicle, int maxrepair )
{
   if ( vehicle->damage  &&  tank.fuel  &&  tank.material ) {

      int orgdam = vehicle->damage;

      int dam;
      if ( vehicle->damage > maxrepair )
         dam = maxrepair;
      else
         dam = vehicle->damage;

      int fkost = dam * vehicle->typ->productionCost.energy / (100 * repairefficiency_unit );
      int mkost = dam * vehicle->typ->productionCost.material / (100 * repairefficiency_unit );
      int w;

      if (mkost <= material)
         w = 10000;
      else
         w = 10000 * material / mkost;

      if (fkost > fuel)
         if (10000 * fuel / fkost < w)
            w = 10000 * fuel / fkost;


      vehicle->damage -= dam * w / 10000;

      for ( int i = 0; i < experienceDecreaseDamageBoundaryNum; i++)
         if ( orgdam > experienceDecreaseDamageBoundaries[i] && vehicle->damage < experienceDecreaseDamageBoundaries[i] )
            if ( vehicle->experience > 0 )
               vehicle->experience-=1;


      if ( vehicle != this ) {
         if ( vehicle->getMovement() > movement_cost_for_repaired_unit )
            vehicle->setMovement ( vehicle->getMovement() -  movement_cost_for_repaired_unit );
         else
            vehicle->setMovement ( 0 );

         if ( !attack_after_repair )
            vehicle->attacked = 0;

         int unitloaded = 0;
         for ( int i = 0; i < 32; i++ )
            if ( loading[i] == vehicle )
               unitloaded = 1;

         if ( !unitloaded )
            if ( getMovement() > movement_cost_for_repairing_unit )
               setMovement ( getMovement() - movement_cost_for_repairing_unit );
            else
               setMovement ( 0 );
      }

      material -= w * mkost / 10000;
      fuel -= w * fkost / 10000;

   }
}
*/

void Vehicle :: endRound ( void )
{
   if ( tank.energy < typ->tank.energy - energyUsed  && generatoractive )
      if ( functions & cfgenerator ) {
         int endiff = typ->tank.energy - tank.energy - energyUsed;
         if ( tank.fuel < endiff * generatortruckefficiency )
            endiff = tank.fuel / generatortruckefficiency;

         tank.energy += endiff;
         tank.fuel -= endiff * generatortruckefficiency ;
         energyUsed = 0;
      }
}

void Vehicle :: endTurn( void )
{
   if ( typ->autorepairrate > 0 )
      if ( damage )
         repairItem ( this, max ( damage - typ->autorepairrate, 0 ) );

   reactionfire.endTurn();

   resetMovement();
   attacked = false;

}

void Vehicle :: resetMovement ( void )
{
    int move = typ->movement[log2(height)];
    setMovement ( move, -1 );
    /*
    if (actvehicle->typ->fuelconsumption == 0)
       actvehicle->movement = 0;
    else {
       if ((actvehicle->fuel << 3) / actvehicle->typ->fuelconsumption < move)
          actvehicle->movement = (actvehicle->fuel << 3) / actvehicle->typ->fuelconsumption ;
       else
          actvehicle->movement = move;
    }
    */
}

void Vehicle :: setMovement ( int newmove, int cargoDivisor )
{
   if ( newmove < 0 )
      newmove = 0;

   if ( cargoDivisor > 0 && typ)
      if ( typ->movement[ log2 ( height ) ] ) {
         int diff = _movement - newmove;
         int perc = 1000 * diff / typ->movement[ log2 ( height ) ] ;
         for ( int i = 0; i < 32; i++ ) {
            if ( loading[i] ) {
               int lperc = perc;
               if ( cargoDivisor )
                  lperc /= cargoDivisor;

               loading[i]->setMovement ( loading[i]->getMovement() - lperc * loading[i]->typ->movement[ log2 ( loading[i]->height)] / 1000 , 1 );
            }
         } /* endfor */
   }
   _movement = newmove;
}

bool Vehicle::hasMoved ( void ) const
{
   return _movement != typ->movement[ log2 ( height )];
}


int Vehicle :: getMovement ( bool checkFuel )
{
   if ( reactionfire.getStatus() != ReactionFire::off )
      return 0;
      
   if ( typ->fuelConsumption && checkFuel ) {
      if ( tank.fuel * minmalq / typ->fuelConsumption < _movement )
         return tank.fuel * minmalq / typ->fuelConsumption;
      else
         return _movement;
   } else
      return _movement;
}

void Vehicle :: decreaseMovement ( int amount )
{
  _movement -= amount;
  if ( _movement < 0 )
    _movement = 0;
  if ( _movement > typ->movement[log2(height)] )
    _movement = typ->movement[log2(height)];
}


bool Vehicle :: canMove ( void )
{
   if ( gamemap->getField ( xpos, ypos )->unitHere ( this ) )
      if ( getMovement() >= minmalq && reactionfire.getStatus() == Vehicle::ReactionFire::off )
         if ( terrainaccessible ( gamemap->getField ( xpos, ypos ), this ) || actmap->getgameparameter( cgp_movefrominvalidfields) )
            return true;
   return false;
}



int Vehicle::ReactionFire::enable ( void )
{
   #ifdef karteneditor
   status = ready;
   #else
   if ( status == off ) {
      int weaponCount = 0;
      int shootableWeaponCount = 0;
      for ( int w = 0; w < unit->typ->weapons.count; w++ )
         if ( unit->typ->weapons.weapon[w].shootable() ) {
              weaponCount++;
              if ( unit->typ->weapons.weapon[w].sourceheight & unit->height )
                 shootableWeaponCount++;
         }

      if ( weaponCount == 0 )
         return -214;

      if ( shootableWeaponCount == 0 )
         return -213;

      if ( unit->typ->wait ) {
         if ( unit->hasMoved())
            status = init1;
         else
            status = init2;
      } else {
         status = init2;
      }
      // unit->setMovement ( 0, 0 );
   }
   #endif
   return 0;
}

void Vehicle::ReactionFire::disable ( void )
{
   if ( status != off ) {
       status = off;
       enemiesAttackable = 0;
       unit->setMovement ( 0, 0 );
   }
}



void Vehicle::ReactionFire::endTurn ( void )
{
   if ( status >= init1 ) {
      if ( status == init1 )
         status = init2;
      else
         if ( status == init2 )
            status = ready;

      if ( status == ready )
         enemiesAttackable = 0xff;
      else
         enemiesAttackable = 0;
   }
}


bool Vehicle :: weapexist( void )
{
   if ( typ->weapons.count > 0)
      for ( int b = 0; b < typ->weapons.count ; b++)
         if ( typ->weapons.weapon[b].shootable() )
            if ( typ->weapons.weapon[b].offensive() )
               if ( ammo[b] )
                  return 1;
    return 0;
}


void Vehicle :: putimage ( int x, int y )
{
  #ifdef sgmain
   int shaded = ( getMovement() < minmalq ) && ( color == gamemap->actplayer*8) && (attacked || !typ->weapons.count || CGameOptions::Instance()->units_gray_after_move );
  #else
   int shaded = 0;
  #endif
    if ( height <= chgetaucht ) {
       if ( shaded )
          putpicturemix ( x, y, xlatpict(xlatpictgraytable,  typ->picture[  direction ]),  color, (char*) colormixbuf );
       else
          putpicturemix ( x, y,  typ->picture[ direction],  color, (char*) colormixbuf );
    } else {
          if ( height >= chtieffliegend ) {
             int d = 6 * ( log2 ( height) - log2 ( chfahrend ));
             putshadow ( x + d, y + d, typ->picture[direction] , &xlattables.a.dark3);
          } else
             if ( height == chfahrend )
                putshadow ( x + 1, y + 1,  typ->picture[ direction] , &xlattables.a.dark3);

          if ( shaded )
             putrotspriteimage( x, y, xlatpict(xlatpictgraytable,  typ->picture[  direction ]),  color);
          else
             putrotspriteimage( x, y,  typ->picture[ direction],  color);
    }
}


void Vehicle :: setnewposition ( int x , int y )
{
  xpos = x;
  ypos = y;
  if ( typ->loadcapacity > 0)
     for ( int i = 0; i <= 31; i++)
        if ( loading[i] )
           loading[i]->setnewposition ( x , y );
}

void Vehicle :: setnewposition ( const MapCoordinate& mc )
{
   setnewposition ( mc.x, mc.y );
}

/*
int Vehicle :: getstrongestweapon( int aheight, int distance)
{
   int str = 0;
   int hw = 255;   //  error-wert  ( keine waffe gefunden )
   for ( int i = 0; i < typ->weapons.count; i++) {

      if (( ammo[i]) &&
          ( typ->weapons.weapon[i].mindistance <= distance) &&
          ( typ->weapons.weapon[i].maxdistance >= distance) &&
          ( typ->weapons.weapon[i].targ & aheight ) &&
          ( typ->weapons.weapon[i].sourceheight & height )) {
            int astr = int( weapstrength[i] * weapDist.getWeapStrength( &typ->weapons.weapon[i], distance, height, aheight));
            if ( astr > str ) {
               str = astr;
               hw  = i;
            }
       }
   }
   return hw;
}
*/

void Vehicle::convert ( int col )
{
  if ( col > 8)
     fatalError("convertvehicle: \n color muá im bereich 0..8 sein ",2);

   int oldcol = color >> 3;

   tmap::Player::VehicleList::iterator i = find ( gamemap->player[oldcol].vehicleList.begin(), gamemap->player[oldcol].vehicleList.end(), this );
   if ( i != gamemap->player[oldcol].vehicleList.end())
      gamemap->player[oldcol].vehicleList.erase ( i );

   gamemap->player[col].vehicleList.push_back( this );

   color = col << 3;

   for ( int i = 0; i < 32; i++)
      if ( loading[i] )
         loading[i]->convert( col );

   #ifndef karteneditor
      if ( connection & cconnection_conquer )
         gamemap->player[oldcol].queuedEvents++;
         // releaseevent( this, NULL,cconnection_conquer) ;

      if ( connection & cconnection_lose )
         gamemap->player[oldcol].queuedEvents++;
         // releaseevent( this, NULL,cconnection_lose);

   #endif
}

void Vehicle :: constructvehicle ( pvehicletype tnk, int x, int y )
{
   if ( gamemap && vehicleconstructable( tnk, x, y )) {
      pvehicle v = new Vehicle( tnk, gamemap, color/8 );
      v->xpos = x;
      v->ypos = y;

      for ( int j = 0; j < 8; j++ ) {
         int a = int(height) << j;
         int b = int(height) >> j;
         if ( v->typ->height & a ) {
            v->height = a;
            break;
         }
         if ( v->typ->height & b ) {
            v->height = b;
            break;
         }
      }
      v->setMovement ( 0 );


      gamemap->getField ( x, y )->vehicle = v;
      tank -= tnk->productionCost;

      int refuel = 0;
      for ( int i = 0; i < typ->weapons.count; i++ )
         if ( typ->weapons.weapon[i].service()  )
            for ( int j = 0; j < typ->weapons.count ; j++) {
               if ( typ->weapons.weapon[j].canRefuel() )
                  refuel = 1;
               if ( functions & (cffuelref | cfmaterialref) )
                  refuel = 1;
            }
#ifdef sgmain
//      if ( refuel )
  //       verlademunition( v, this ,NULL, 3 );
#endif

      v->attacked = 1;
   }
}

bool  Vehicle :: vehicleconstructable ( pvehicletype tnk, int x, int y )
{
   if ( tnk->terrainaccess.accessible ( gamemap->getField(x,y)->bdt ) > 0 )
      if ( tnk->productionCost.material <= tank.material &&
           tnk->productionCost.energy   <= tank.fuel  )
           if ( beeline (x, y, xpos, ypos) <= maxmalq )
              if ( tnk->height & height )
                 return 1;


   return 0;
}


bool Vehicle :: buildingconstructable ( pbuildingtype building )
{
   if ( !building )
      return 0;


   if ( gamemap->getgameparameter(cgp_forbid_building_construction) )
       return 0;

   int mf = gamemap->getgameparameter ( cgp_building_material_factor );
   int ff = gamemap->getgameparameter ( cgp_building_fuel_factor );

   if ( !mf )
      mf = 100;
   if ( !ff )
      ff = 100;

   int hd = getheightdelta ( log2 ( height ), log2 ( building->buildingheight ));

   if ( hd != 0 && !(hd ==-11 && (height == chschwimmend || height == chfahrend)))
      return 0;


   if ( building->productionCost.material * mf / 100 <= tank.material   &&   building->productionCost.fuel * ff / 100 <= tank.fuel ) {
      int found = 0;
      if ( functions & cfputbuilding )
         found = 1;
      if ( functions & cfspecificbuildingconstruction )
         for ( int i = 0; i < typ->buildingsBuildable.size(); i++ )
            if ( typ->buildingsBuildable[i].from <= building->id &&
                 typ->buildingsBuildable[i].to   >= building->id )
                 found = 1;

      return found;
   } else
      return false;
}


int Vehicle :: searchstackforfreeweight ( pvehicle eht, int what )
{
   if ( eht == this ) {
      if ( what == 1 ) // material or fuel
         return maxint;
      else
         return typ->loadcapacity - cargo();
        // return typ->maxweight() + typ->loadcapacity - weight();
   } else {
      int w1 = typ->maxweight() + typ->loadcapacity - weight();
      int w2 = -1;
      for ( int i = 0; i < 32; i++ )
         if ( loading[i] ) {
            int w3 = loading[i]->searchstackforfreeweight ( eht, what );
            if ( w3 >= 0 )
               w2 = w3;
         }

      if ( w2 != -1 )
         if ( w2 < w1 )
            return w2;
         else
            return w1;
      else
         return -1;
   }
}

int Vehicle :: freeweight ( int what )
{
   pfield fld = gamemap->getField ( xpos, ypos );
   if ( fld->vehicle )
        return fld->vehicle->searchstackforfreeweight ( this, what );
   else
      if ( fld->building ) {
         for ( int i = 0; i < 32; i++ )
            if ( fld->building->loading[i] ) {
               int w3 = fld->building->loading[i]->searchstackforfreeweight ( this, what );
               if ( w3 >= 0 )
                  return w3;
            }
      }

   return -2;
}

int Vehicle::getMaxResourceStorageForWeight ( int resourcetype )
{
   pfield fld = gamemap->getField ( xpos, ypos );
   if ( fld->vehicle  &&  fld->vehicle != this && resourceWeight[resourcetype] ) {
      int fw = freeweight( 1 );
      if ( fw >= 0 ) {
         int maxf = fw * 1024 / resourceWeight[resourcetype];
         if ( maxf > typ->tank.resource(resourcetype) || maxf < 0 )
            return typ->tank.resource(resourcetype);
         else
            return maxf;
      } else
         return typ->tank.resource(resourcetype);
   } else
      return typ->tank.resource(resourcetype);
}

/*
int Vehicle::getmaxfuelforweight ( void )
{
   pfield fld = gamemap->getField ( xpos, ypos );
   if ( fld->vehicle  &&  fld->vehicle != this ) {
      int fw = freeweight( 1 );
      if ( fw >= 0 ) {
         int maxf = fw * 1024 / fuelweight;
         if ( maxf > typ->tank.fuel || maxf < 0 )
            return typ->tank.fuel;
         else
            return maxf;
      } else
         return typ->tank.fuel;
   } else
      return typ->tank.fuel;
}


int Vehicle::getmaxmaterialforweight ( void )
{
   pfield fld = gamemap->getField ( xpos, ypos );
   if ( fld->vehicle  &&  fld->vehicle != this ) {
      int fw = freeweight( 1 );
      if ( fw >= 0 ) {
         int maxm = fw * 1024 / materialweight;
         if ( maxm > typ->tank.material )
            return typ->tank.material;
         else
            return maxm;
      } else
         return typ->tank.material;

  } else
      return typ->tank.material;
}
*/


bool  Vehicle :: vehicleloadable ( pvehicle vehicle, int uheight ) const
{
   #ifdef sgmain
   if ( gamemap->getField ( xpos, ypos )->vehicle == this ) // not standing in some other transport / building
      if ( height & (chtieffliegend | chfliegend | chhochfliegend ))
         return 0;
   #endif

   if ( uheight == -1 )
      uheight = vehicle->height;


   if ( vehicle->functions & cf_trooper )
      if ( uheight & (chschwimmend | chfahrend ))
         uheight |= (chschwimmend | chfahrend );  //these heights are effectively the same


   if ( !(uheight & vehicle->height)  &&  !(uheight & height) )
      return 0;

   if (( ( typ->loadcapability    & vehicle->height)   &&
         (( typ->loadcapabilityreq & vehicle->typ->height) || !typ->loadcapabilityreq ) &&
         ((typ->loadcapabilitynot & vehicle->typ->height) == 0))
        || (vehicle->functions & cf_trooper )) {

      if ( typ->maxunitweight >= vehicle->weight() )
         if ( (cargo() + vehicle->weight() <= typ->loadcapacity) &&
              ( vehiclesLoaded() + 1 < maxloadableunits)) {

                 if ( gamemap->getField ( xpos, ypos )->vehicle != this )
                    return 2;
               #ifdef karteneditor
                  return 2;
               #else
                 if ( uheight != vehicle->height &&
                      height == uheight)
                      return 2;
                 else
                    if (vehicle->height == chtieffliegend)
                       if (vehicle->typ->steigung <= flugzeugtraegerrunwayverkuerzung)
                          return 2;
                       else
                          return 0;
                    else
                       if (vehicle->height == chfahrend) {
                           if ((height >= chschwimmend) &&
                               (height <= chfahrend))
                               return 2;
                           else
                               return 0;
                       } else
                          if ( vehicle->height == height )
                             return 2;
               #endif
              }

   }
   return 0;
}

void Vehicle :: addview ( void )
{
   if ( viewOnMap )
      fatalError ("void Vehicle :: addview - the vehicle is already viewing the map");

   viewOnMap = true;
   tcomputevehicleview bes ( gamemap );
   bes.init( this, +1 );
   bes.startsearch();
}

void Vehicle :: removeview ( void )
{
   if ( !viewOnMap )
      fatalError ("void Vehicle :: removeview - the vehicle is not viewing the map");

   tcomputevehicleview bes ( gamemap );
   bes.init( this, -1 );
   bes.startsearch();

   viewOnMap = false;
}


class tsearchforminablefields: public SearchFields {
      int shareview;
    public:
      int numberoffields;
      int run ( pvehicle     eht );
      virtual void testfield ( const MapCoordinate& mc );
      tsearchforminablefields ( pmap _gamemap ) : SearchFields ( _gamemap ) {};
  };


void         tsearchforminablefields::testfield( const MapCoordinate& mc )
{
    pfield fld = gamemap->getField ( mc );
    if ( !fld->building  ||  fld->building->color == gamemap->actplayer*8  ||  fld->building->color == 8*8)
       if ( !fld->vehicle  ||  fld->vehicle->color == gamemap->actplayer*8 ||  fld->vehicle->color == 8*8) {
          if ( !fld->resourceview )
             fld->resourceview = new tfield::Resourceview;

          for ( int c = 0; c < 8; c++ )
             if ( shareview & (1 << c) ) {
                fld->resourceview->visible |= ( 1 << c );
                fld->resourceview->fuelvisible[c] = fld->fuel;
                fld->resourceview->materialvisible[c] = fld->material;
             }
       }
}



int  tsearchforminablefields::run( pvehicle eht )
{
   if ( (eht->functions & cfmanualdigger) && !(eht->functions & cfautodigger) )
      if ( eht->attacked ||
          (eht->typ->wait && eht->hasMoved() ) ||
          (eht->getMovement() < searchforresorcesmovedecrease ))
          return -311;


   shareview = 1 << ( eht->color / 8);
   if ( gamemap->shareview )
      for ( int i = 0; i < 8; i++ )
         if ( i*8 != eht->color )
            if ( gamemap->player[i].exist() )
               if ( gamemap->shareview->mode[eht->color/8][i] )
                  shareview += 1 << i;

   numberoffields = 0;
   initsearch( eht->getPosition(), eht->typ->digrange, 0 );
   if ( eht->typ->digrange )
      startsearch();

   if ( (eht->functions & cfmanualdigger) && !(eht->functions & cfautodigger) )
      eht->setMovement ( eht->getMovement() - searchforresorcesmovedecrease );

   if ( !gamemap->mineralResourcesDisplayed && (eht->functions & cfmanualdigger) && !(eht->functions & cfautodigger))
      gamemap->mineralResourcesDisplayed = 1;

   return 1;
}


int Vehicle::maxMovement ( void )
{
   return typ->movement[log2(height)];
}

int Vehicle::searchForMineralResources ( void )
{
    tsearchforminablefields sfmf ( gamemap );
    return sfmf.run( this );
}


void Vehicle :: fillMagically( void )
{
   #ifndef karteneditor
   fprintf(stderr, "ASC: Warning, Vehicle :: fillMagically called outside mapeditor");
   #endif
   tank = typ->tank;

   for ( int m = 0; m < typ->weapons.count ; m++) {
      ammo[m] = typ->weapons.weapon[m].count;
      weapstrength[m] = typ->weapons.weapon[m].maxstrength;
   }
   armor = typ->armor;
   klasse = 255;
}




#define cem_experience    0x1
#define cem_damage        0x2
#define cem_fuel          0x4
#define cem_ammunition    0x8
#define cem_weapstrength  0x10
#define cem_loading       0x20
#define cem_attacked      0x40
#define cem_height        0x80
#define cem_movement      0x100
#define cem_direction     0x200
#define cem_material      0x400
#define cem_energy        0x800
#define cem_class         0x1000
#define cem_networkid     0x2000
#define cem_name          0x4000
#define cem_armor         0x8000
#define cem_reactionfire  0x10000
#define cem_reactionfire2 0x20000
#define cem_poweron       0x40000
#define cem_weapstrength2 0x80000
#define cem_ammunition2   0x100000
#define cem_energyUsed    0x200000
#define cem_position      0x400000
#define cem_aiparam       0x800000






void   Vehicle::write ( tnstream& stream, bool includeLoadedUnits )
{
    stream.writeWord ( typ->id );
    stream.writeChar ( color );

    int bm = 0;

    if ( experience )
       bm |= cem_experience;
    if ( damage    )
       bm |= cem_damage;

    if ( tank.fuel < typ->tank.fuel )
       bm |= cem_fuel;

    if ( typ->weapons.count )
       for (char m = 0; m < typ->weapons.count ; m++) {
          if ( ammo[m] < typ->weapons.weapon[m].count )
             bm |= cem_ammunition2;
          if ( weapstrength[m] != typ->weapons.weapon[m].maxstrength )
             bm |= cem_weapstrength2;

       }
    if ( includeLoadedUnits )
       for ( int i = 0; i < 32; i++ )
          if ( loading[i] )
              bm |= cem_loading;

    if ( attacked  )
       bm |= cem_attacked;
    if ( height != chfahrend )
       bm |= cem_height;
    if ( _movement < typ->movement[log2(height)] )
       bm |= cem_movement;

    if ( direction )
       bm |= cem_direction;

    if ( tank.material < typ->tank.material )
       bm |= cem_material  ;

    if ( tank.energy   < typ->tank.energy   )
       bm |= cem_energy;

    if ( energyUsed )
       bm |= cem_energyUsed;

    if ( klasse    )
       bm |= cem_class;

    if ( armor != typ->armor )
       bm |= cem_armor;

    if ( networkid )
       bm |= cem_networkid;

    if ( !name.empty() )
       bm |= cem_name;

    if ( reactionfire.status )
       bm |= cem_reactionfire;

    if ( reactionfire.enemiesAttackable )
       bm |= cem_reactionfire2;

    if ( generatoractive )
       bm |= cem_poweron;

    if ( xpos != 0 || ypos != 0 )
       bm |= cem_position;

    for ( int i = 0; i < 8; i++ )
       if ( aiparam[i] )
          bm |= cem_aiparam;



    stream.writeInt( bm );

    if ( bm & cem_experience )
         stream.writeChar ( experience );

    if ( bm & cem_damage )
         stream.writeChar ( damage );

    if ( bm & cem_fuel )
         stream.writeInt ( tank.fuel );

    if ( bm & cem_ammunition2 )
       for ( int j= 0; j < 16; j++ )
         stream.writeInt ( ammo[j] );

    if ( bm & cem_weapstrength2 )
       for ( int j = 0; j < 16; j++ )
         stream.writeInt ( weapstrength[j] );

    if ( bm & cem_loading ) {
       char c=0;
       for ( int k = 0; k <= 31; k++)
          if ( loading[k] )
             c++;

       stream.writeChar ( c );

       if ( c )
          for ( int k = 0; k <= 31; k++)
             if ( loading[k] )
                loading[k]->write ( stream );
    }

    if ( bm & cem_height )
         stream.writeChar ( height );

    if ( bm & cem_movement )
         stream.writeChar ( _movement );

    if ( bm & cem_direction )
         stream.writeChar ( direction );

    if ( bm & cem_material )
         stream.writeInt ( tank.material );

    if ( bm & cem_energy )
         stream.writeInt ( tank.energy );

    if ( bm & cem_class )
         stream.writeChar ( klasse );

    if ( bm & cem_armor )
         stream.writeWord ( armor );

    if ( bm & cem_networkid )
         stream.writeInt ( networkid );

    if ( bm & cem_attacked )
         stream.writeChar ( attacked );

    if ( bm & cem_name     )
         stream.writeString ( name );

    if ( bm & cem_reactionfire )
       stream.writeChar ( reactionfire.status );

    if ( bm & cem_reactionfire2 )
       stream.writeChar ( reactionfire.enemiesAttackable );

    if ( bm & cem_poweron )
       stream.writeInt ( generatoractive );

    if ( bm & cem_energyUsed )
       stream.writeInt ( energyUsed );

    if ( bm & cem_position ) {
       stream.writeInt ( xpos );
       stream.writeInt ( ypos );
    }

    if ( bm & cem_aiparam ) {
       stream.writeInt( 0x23451234 );
       for ( int i = 0; i < 8; i++ )
          stream.writeInt ( aiparam[i] != NULL );

       for ( int i = 0; i < 8; i++ )
          if ( aiparam[i] )
             aiparam[i]->write ( stream );

       stream.writeInt( 0x23451234 );
    }
}

void   Vehicle::read ( tnstream& stream )
{
    int _id = stream.readWord ();
    stream.readChar (); // color
    if ( _id != typ->id )
       fatalError ( "Vehicle::read - trying to read a unit of different type" );

    readData ( stream );
}

void   Vehicle::readData ( tnstream& stream )
{

    int bm = stream.readInt();

    if ( bm & cem_experience )
       experience = stream.readChar();
    else
       experience = 0;

    if ( bm & cem_damage )
       damage = stream.readChar();
    else
       damage = 0;

    if ( bm & cem_fuel )
       tank.fuel = stream.readInt();
    else
       tank.fuel = typ->tank.fuel;

    if ( bm & cem_ammunition ) {
       for ( int i = 0; i < 8; i++ )
         ammo[i] = stream.readWord();
    } else
     if ( bm & cem_ammunition2 ) {
        for ( int i = 0; i < 16; i++ ) {
          ammo[i] = stream.readInt();
          if ( ammo[i] > typ->weapons.weapon[i].count )
             ammo[i] = typ->weapons.weapon[i].count;
          if ( ammo[i] < 0 )
             ammo[i] = 0;
        }

     } else
       for (int i=0; i < typ->weapons.count ;i++ )
          ammo[i] = typ->weapons.weapon[i].count;


    if ( bm & cem_weapstrength ) {
       for ( int i = 0; i < 8; i++ )
         weapstrength[i] = stream.readWord();

    } else
     if ( bm & cem_weapstrength2 ) {
        for ( int i = 0; i < 16; i++ )
           weapstrength[i] = stream.readInt();
     } else
       for (int i=0; i < typ->weapons.count ;i++ )
          weapstrength[i] = typ->weapons.weapon[i].maxstrength;

    if ( bm & cem_loading ) {
       char c = stream.readChar();

       if ( c ) {
          for (int k = 0; k < c; k++)
             loading[k] = Vehicle::newFromStream ( gamemap, stream );

          for ( int l = c; l < 32; l++ )
             loading[l] = NULL;
       }
    }

    if ( bm & cem_height )
       height = stream.readChar();
    else
       height = chfahrend;

    if ( ! (height & typ->height) )
       height = 1 << log2 ( typ->height );

    if ( bm & cem_movement )
       setMovement ( stream.readChar ( ), 0 );
    else
       setMovement ( typ->movement [ log2 ( height ) ], 0 );

    if ( bm & cem_direction )
       direction = stream.readChar();
    else
       direction = 0;

    if ( bm & cem_material )
       tank.material = stream.readInt();
    else
       tank.material = typ->tank.material;

    if ( bm & cem_energy )
       tank.energy = stream.readInt();
    else
       tank.energy = typ->tank.energy;

    if ( bm & cem_class )
       klasse = stream.readChar();
    else
       klasse = 0;

    if ( bm & cem_armor )
       armor = stream.readWord();
    else
       armor = typ->armor;

    if ( bm & cem_networkid )
       networkid = stream.readInt();
    else
       networkid = 0;

    if ( bm & cem_attacked )
       attacked = stream.readChar();

    if ( bm & cem_name )
       name = stream.readString ( );

    int reactionfirestatus = 0;
    if ( bm & cem_reactionfire )
       reactionfirestatus = stream.readChar();

    if ( bm & cem_reactionfire2 )
       reactionfire.enemiesAttackable = stream.readChar();
    else
       reactionfire.enemiesAttackable = 0;

    if ( reactionfirestatus >= 8 && reactionfire.enemiesAttackable <= 4 ) { // for transition from the old reactionfire system ( < ASC1.2.0 ) to the new one ( >= ASC1.2.0 )
       reactionfire.status = ReactionFire::Status ( reactionfire.enemiesAttackable );
       reactionfire.enemiesAttackable = reactionfirestatus;
       setMovement ( typ->movement [ log2 ( height ) ], -1 );
    } else
       reactionfire.status = ReactionFire::Status ( reactionfirestatus );

    if ( bm & cem_poweron )
       generatoractive = stream.readInt();
    else
       generatoractive = 0;

    if ( bm & cem_energyUsed )
       energyUsed =  stream.readInt ();
    else
       energyUsed = 0;

    if ( bm & cem_position ) {
       int x = stream.readInt ( );
       int y = stream.readInt ( );
       setnewposition ( x, y );
    } else
       setnewposition ( 0, 0 );

    if ( bm & cem_aiparam ) {
       int magic = stream.readInt();
       if ( magic != 0x23451234 )
          throw ASCmsgException ( "Vehicle::read() - inconsistent data stream" );
       for ( int i = 0; i < 8; i++ ) {
          bool b = stream.readInt ( );
          if ( b )
             aiparam[i] = new AiParameter ( this );
       }

       for ( int i = 0; i < 8; i++ )
          if ( aiparam[i] )
             aiparam[i]->read ( stream );

       magic = stream.readInt();
       if ( magic != 0x23451234 )
          throw ASCmsgException ( "Vehicle::read() - inconsistent data stream" );
    }



    #ifdef sgmain
    if (klasse == 255) {
       if ( typ->classnum ) {
          for (int i = 0; i < typ->classnum ; i++ )
             if ( gamemap->player[ color/8 ].research.vehicleclassavailable( typ, i ) )
                klasse = i;
             else
                break;


          functions = typ->functions & typ->classbound[klasse].vehiclefunctions;
       } else {
          functions = typ->functions ;
          klasse = 0;
       }

       armor = typ->armor * typ->classbound[klasse].armor / 1024;
       if (typ->weapons.count )
          for ( int m = 0; m < typ->weapons.count ; m++)
             if ( typ->weapons.weapon[m].getScalarWeaponType() >= 0 )
                weapstrength[m] = typ->weapons.weapon[m].maxstrength * typ->classbound[klasse].weapstrength[ typ->weapons.weapon[m].getScalarWeaponType()] / 1024;
             else
                weapstrength[m] = 0;

    } else {
      if ( typ->classnum )
         functions = typ->functions & typ->classbound[klasse].vehiclefunctions;
      else
         functions = typ->functions ;
    }
    #endif
}

MapCoordinate3D Vehicle :: getPosition ( )
{
   MapCoordinate3D mc;
   mc.x = xpos;
   mc.y = ypos;
   mc.z = height;
   return mc;
}

Vehicle* Vehicle::newFromStream ( pmap gamemap, tnstream& stream )
{
   int id = stream.readWord ();
   pvehicletype fzt = gamemap->getvehicletype_byid ( id );
   if ( !fzt )
      throw InvalidID ( "vehicle", id );

   int color = stream.readChar ();

   Vehicle* v = new Vehicle ( fzt, gamemap, color/8 );

   v->readData ( stream );
   return v;
}

/** Returns the SingleWeapon corresponding to the weaponNum for this
 *  vehicle.
 */
const SingleWeapon* Vehicle::getWeapon( unsigned weaponNum )
{
  if ( weaponNum <= typ->weapons.count )
     return &typ->weapons.weapon[weaponNum];
  else
     return NULL;
}

