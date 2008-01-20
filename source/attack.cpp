/*! \file attack.cpp
    \brief All the routines for fighting in ASC
*/


/*
    This file is part of Advanced Strategic Command; http://www.asc-hq.de
    Copyright (C) 1994-2005  Martin Bickel  and  Marc Schellenberger

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
#include <math.h>

#include "typen.h"
#include "buildingtype.h"
#include "vehicletype.h"
#include "attack.h"
#include "spfst.h"



bool  AttackFormula :: checkHemming ( Vehicle*     d_eht,  int     direc )
{ 
   Vehicle*     s_eht;

   int x = d_eht->xpos;
   int y = d_eht->ypos; 
   getnextfield(x, y, direc);
   tfield* fld = getfield(x,y);

   if ( fld )
      s_eht = fld->vehicle;
   else 
      s_eht = NULL;

   if ( !s_eht )
      return false;

   if ( s_eht->height >= chtieffliegend || d_eht->height >= chtieffliegend )
      return false;

   return attackpossible2n ( s_eht, d_eht );
} 


float AttackFormula :: getHemmingFactor ( int relDir )
{
   const float  maxHemmingFactor = 1.4;  // = +140% !
   const float hemming[sidenum-1]  = { 4, 11, 16, 11, 4 };
   const float maxHemmingSum = 46;
   relDir  %= 6;
   if ( relDir < 0 )
      relDir += sidenum;

   if ( relDir == 5 )
      warning("float AttackFormula :: getHemmingFactor - invalid direction" );
      
   return hemming[relDir]*maxHemmingFactor/maxHemmingSum;
}


float AttackFormula :: strength_hemming ( int  ax,  int ay,  Vehicle* d_eht )
{
   float hemm = 0;
   int attackDir = getdirection(ax,ay,d_eht->xpos,d_eht->ypos);
   for ( int i = 0; i < sidenum-1; i++) {

      int direc = i+1 + (attackDir-sidenum/2);
      if (direc < 0)
         direc += sidenum;

      if (direc >= sidenum)
         direc -= sidenum;

      if ( checkHemming (d_eht,direc ))
         hemm += getHemmingFactor(i);
   }

   return  hemm + 1;
}


float AttackFormula :: strength_damage ( int damage )
{
   return 1 - (2.0 * float(damage) / 300.0);
}

float AttackFormula :: strength_experience ( int experience )
{
	float e =		(experience < 0)
				?	0	
				:	experience ;
   return e/maxunitexperience * 2.875 / actmap->getgameparameter( cgp_experienceDivisorAttack );
}

float AttackFormula :: defense_experience ( int experience )
{
   float e =		(experience < 0)
				?	0
				:	experience ;

   return e/maxunitexperience * 1.15 / actmap->getgameparameter( cgp_experienceDivisorDefense );
}

float AttackFormula :: strength_attackbonus ( int abonus )
{
   float a = abonus;
   return a/8;
}


float AttackFormula :: defense_defensebonus ( int defensebonus )
{
   float d = defensebonus;
   return d/8;
}



tfight :: tfight ( void )
{
}


void tfight :: calc ( void )
{
   int damagefactor = actmap->getgameparameter ( cgp_attackPower );
   const float armordivisor = 5;


   if ( av.strength ) { 
      float absstrength = float(av.strength )
                          * ( 1 + strength_experience ( av.experience ) + strength_attackbonus ( av.attackbonus ) )
                          * strength_damage ( av.damage ) 
                          * dv.hemming;

      float absdefense = float(dv.armor / armordivisor )
                          * ( 1 + defense_defensebonus ( dv.defensebonus ) + defense_experience ( dv.experience ) );

      int w = int( ceil(dv.damage + absstrength / absdefense * 1000 / damagefactor ));

      if (dv.damage > w ) 
         warning("fatal error at attack: \ndecrease of damage d!");

      if (dv.damage == w )
         w = dv.damage+1;

      if (w > 100) 
         dv.damage = 100; 
      else 
         dv.damage = w; 

      if ( av.weapcount > 0 )
         av.weapcount--;
      else
         av.weapcount = 0;


      av.experience++;

      if ( dist <= 10 )
         av.experience += 1;

      if ( dv.damage >= 100 ) 
         av.experience += 1;



      if ( av.experience > maxunitexperience )
         av.experience = maxunitexperience;

   } 

   if ( dv.strength ) { 
      float absstrength = float(dv.strength )
                          * ( 1 + strength_experience ( dv.experience ) + strength_attackbonus ( dv.attackbonus ) )
                          * strength_damage ( dv.damage ) ;

      float absdefense = float(av.armor / armordivisor)
                          * ( 1 + defense_defensebonus ( av.defensebonus )+ defense_experience ( av.experience ));

      int w = int( ceil(av.damage + absstrength / absdefense * 1000 / damagefactor ));

      if (av.damage > w ) 
         warning("fatal error at attack: \ndecrease of damage a!");

      if (w > 100) 
         av.damage = 100; 
      else 
         av.damage = w; 


      if ( dv.weapcount > 0 )
         dv.weapcount--;
      else
         dv.weapcount = 0;

      if ( av.damage >= 100 ) {
         dv.experience += 2;
         if ( dv.experience > maxunitexperience )
            dv.experience = maxunitexperience;

      } else
        if ( dv.experience < maxunitexperience )
           dv.experience++;
   } 

   if ( av.kamikaze ) 
      av.damage = 100;

}



tunitattacksunit :: tunitattacksunit ( Vehicle* &attackingunit, Vehicle* &attackedunit, bool respond, int weapon, bool reactionfire )
{
   this->reactionfire = reactionfire;
   setup ( attackingunit, attackedunit, respond, weapon );
}

void tunitattacksunit :: setup ( Vehicle* &attackingunit, Vehicle* &attackedunit, bool respond, int weapon )
{
   _attackingunit = attackingunit;
   _attackedunit  = attackedunit;

   _pattackingunit = &attackingunit;
   _pattackedunit  = &attackedunit;

   dist = beeline ( attackingunit->xpos, attackingunit->ypos, attackedunit->xpos, attackedunit->ypos );
   int _weapon;

   if ( weapon == -1 ) {
      pattackweap atw = attackpossible( attackingunit, attackedunit->xpos, attackedunit->ypos );
      int n = -1;
      int s = 0;
      for ( int i = 0; i < atw->count; i++ )
         if ( atw->strength[i] > s ) {
            s = atw->strength[i];
            n = i;
         }

      _weapon = atw->num[n];

      delete atw;

   } else
      _weapon  = weapon;

   const SingleWeapon* weap = attackingunit->getWeapon(_weapon);


   int targetWeather = attackedunit->getMap()->getField( attackedunit->getPosition() )->getweather();

   av.strength = int ( ceil( attackingunit->weapstrength[_weapon]
                        * WeapDist::getWeaponStrength(weap, targetWeather, dist, attackingunit->height, attackedunit->height )
                        * attackingunit->typ->weapons.weapon[_weapon].targetingAccuracy[attackedunit->typ->movemalustyp] / 100 ));
   av.armor  = attackingunit->getArmor();
   av.damage     = attackingunit->damage;
   av.experience  = attackingunit->experience;
   av.hemming    = 1;
   av.weapnum    = _weapon;
   av.weapcount  = attackingunit->ammo [ _weapon ];
   av.color      = attackingunit->getOwner();
   av.initiative = attackingunit->typ->initiative;
   av.kamikaze   = attackingunit->typ->hasFunction( ContainerBaseType::KamikazeOnly  );
   av.height = attackingunit->height;
   av.weapontype = attackingunit->typ->weapons.weapon[ _weapon ].getScalarWeaponType();

   tfield* field = getfield ( attackingunit->xpos, attackingunit->ypos );

   if ( attackingunit->height <= chfahrend ) {
      // if ( dist <= maxmalq )
         av.attackbonus  = field->getattackbonus();
      //else
      //   av.attackbonus = 0;
      av.defensebonus = field->getdefensebonus();
   } else {
      av.attackbonus = 0;
      av.defensebonus = 0;
   }


   dv.weapnum = -1;
   if ( dist <= maxmalq  &&  respond  && !av.kamikaze  && fieldvisiblenow( field, attackedunit->getOwner()) ) {
      AttackWeap atw;
      attackpossible2n ( attackedunit, attackingunit, &atw );
      int n = -1;
      int s = 0;
      for ( int i = 0; i < atw.count; i++ )
         if ( atw.strength[i] > s ) {
            s = atw.strength[i];
            n = i;
         }

      if ( n < 0 )
         respond = 0;
      else
         dv.weapnum = atw.num [ n ];

   } else
      respond = 0;

   if ( attackedunit->typ->hasFunction( ContainerBaseType::KamikazeOnly  ))
      respond = 0;

   if ( respond ) {
      weap = attackedunit->getWeapon( dv.weapnum );


      int attackerWeather = attackingunit->getMap()->getField( attackingunit->getPosition() )->getweather();

      dv.strength  = int ( ceil( attackedunit->weapstrength[ dv.weapnum ]
                           * WeapDist::getWeaponStrength(weap, attackerWeather, dist, attackedunit->height, attackingunit->height )
                           * attackedunit->typ->weapons.weapon[ dv.weapnum ].targetingAccuracy[attackingunit->typ->movemalustyp] / 100 ));
      field = getfield ( attackedunit->xpos, attackedunit->ypos );
      dv.attackbonus  = field->getattackbonus();
      _respond = 1;

      dv.weapcount   = attackedunit->ammo [ dv.weapnum ];
      dv.weapontype = attackedunit->typ->weapons.weapon[ dv.weapnum ].getScalarWeaponType();

   } else {
      dv.strength = 0;
      dv.attackbonus = 0;
      _respond = 0;
   }


   dv.armor = attackedunit->getArmor();
   dv.damage    = attackedunit->damage;
   dv.experience = attackedunit->experience;
   if ( dist <= maxmalq && attackingunit->height < chtieffliegend )
      dv.hemming = strength_hemming ( attackingunit->xpos, attackingunit->ypos, attackedunit );
   else
      dv.hemming = 1;


   if ( attackedunit->height <= chfahrend )
      dv.defensebonus = getfield ( attackedunit->xpos, attackedunit->ypos ) -> getdefensebonus();
   else
      dv.defensebonus = 0;

   dv.color      = attackedunit->getOwner();
   dv.initiative = attackedunit->typ->initiative;
   dv.kamikaze = 0;
   dv.height = attackedunit->height;
}



void tunitattacksunit :: setresult ( void )
{
   _attackingunit->experience = av.experience;
   _attackingunit->ammo[ av.weapnum ] = av.weapcount;

   _attackingunit->postAttack( reactionfire );

   _attackingunit->reactionfire.weaponShots[ av.weapnum]--;
   _attackingunit->reactionfire.nonattackableUnits.push_back ( _attackedunit->networkid );

   _attackedunit->damage    = dv.damage;
   _attackingunit->damage    = av.damage;

   if ( _respond ) {
      _attackedunit->experience = dv.experience;
      _attackedunit->ammo[ dv.weapnum ] = dv.weapcount;
   }

   /* If the attacking vehicle was destroyed, remove it */
   if ( _attackingunit->damage >= 100 ) {
     delete *_pattackingunit;
     *_pattackingunit = NULL;
   }

   /* If the attacked vehicle was destroyed, remove it */
   if ( _attackedunit->damage >= 100 ) {
     delete *_pattackedunit;
     *_pattackedunit = NULL;
   }
   actmap->time.set ( actmap->time.turn(), actmap->time.move()+1);
}








tunitattacksbuilding :: tunitattacksbuilding ( Vehicle* attackingunit, int x, int y, int weapon )
{
   setup ( attackingunit, x, y, weapon );
}


void tunitattacksbuilding :: setup ( Vehicle* attackingunit, int x, int y, int weapon )
{
   _attackingunit = attackingunit;
   _x = x;
   _y = y;
   _attackedbuilding  = getfield ( x, y ) -> building;

   dist = beeline ( attackingunit->xpos, attackingunit->ypos, x, y );
   int _weapon;

   if ( weapon == -1 ) {
      pattackweap atw = attackpossible( attackingunit, x, y );
      int n = -1;
      int s = 0;
      for ( int i = 0; i < atw->count; i++ )
         if ( atw->strength[i] > s ) {
            s = atw->strength[i];
            n = i;
         }

      _weapon = atw->num[n];

      delete atw;
   } else
      _weapon  = weapon;

   const SingleWeapon *weap = &attackingunit->typ->weapons.weapon[_weapon];

   int targetWeather = getfield(x,y)->getweather();

   av.strength  = int (ceil( attackingunit->weapstrength[_weapon]
                        * WeapDist::getWeaponStrength(weap, targetWeather, dist, attackingunit->height, _attackedbuilding->typ->buildingheight )
                        * attackingunit->typ->weapons.weapon[_weapon].targetingAccuracy[cmm_building] / 100 ));

   av.armor = attackingunit->getArmor();
   av.damage    = attackingunit->damage;
   av.experience = attackingunit->experience;
   av.hemming    = 1;
   av.weapnum    = _weapon;
   av.weapcount  = attackingunit->ammo [ _weapon ];
   av.weapontype = attackingunit->typ->weapons.weapon[ _weapon ].getScalarWeaponType();
   av.color      = attackingunit->getOwner();
   av.initiative = attackingunit->typ->initiative;
   av.kamikaze   = attackingunit->typ->hasFunction( ContainerBaseType::KamikazeOnly  );
   av.height = attackingunit->height;

   tfield* field = getfield ( attackingunit->xpos, attackingunit->ypos );

   if ( attackingunit->height <= chfahrend ) {
      av.defensebonus = field->getdefensebonus();
      // if ( dist <= maxmalq )
         av.attackbonus  = field->getattackbonus();
      //else
      //   av.attackbonus = 0;
   } else {
      av.defensebonus = 0;
      av.attackbonus  = 0;
   }


   dv.strength = 0;
   dv.attackbonus = 0;


   dv.armor = _attackedbuilding->getArmor();
   dv.damage    = _attackedbuilding->damage;
   dv.experience = 0;
   dv.hemming    = 1;
   dv.weapnum = -1;

   dv.defensebonus = 0; // getfield ( x, y ) -> getdefensebonus();
   dv.color      = _attackedbuilding->getOwner();
   dv.initiative = 0;
   dv.kamikaze = 0;
   dv.height = _attackedbuilding->typ->buildingheight;
}


void tunitattacksbuilding :: setresult ( void )
{
   // _attackingunit->experience = av.experience;
   _attackingunit->ammo[ av.weapnum ] = av.weapcount;

   _attackingunit->postAttack( false );

   _attackingunit->damage    = av.damage;
   _attackedbuilding->damage    = dv.damage;

   /* Remove the attacking unit if it was destroyed */
   if ( _attackingunit->damage >= 100 ) {
      delete _attackingunit;
      _attackingunit = NULL;
   }


   /* Remove attacked building if it was destroyed */
   if ( _attackedbuilding->damage >= 100 ) {
     delete _attackedbuilding ;
     _attackedbuilding = NULL;
   }

   actmap->time.set ( actmap->time.turn(), actmap->time.move()+1);
}



tmineattacksunit :: tmineattacksunit ( tfield* mineposition, int minenum, Vehicle* &attackedunit )
{
   setup ( mineposition, minenum, attackedunit );
}

void tmineattacksunit :: setup ( tfield* mineposition, int minenum, Vehicle* &attackedunit )
{
   if ( mineposition->mines.empty() )
      errorMessage(" tmineattacksunit :: setup \n no mine to attack !\n" );

   if ( attackedunit->height >= chtieffliegend )
      errorMessage(" tmineattacksunit :: setup \n mine attacks flying unit!\n" );

   dist = 10;

   _mineposition = mineposition;
   _attackedunit = attackedunit;
   _pattackedunit = &attackedunit;


   _minenum = minenum;

   if ( minenum == -1 ) {
      int cnt = 1;
      av.strength = 0;
      for ( tfield::MineContainer::iterator m = mineposition->mines.begin(); m != mineposition->mines.end(); m++ )
         if ( m->attacksunit ( attackedunit )) {
            int strength = m->strength;
            if ( m->type == cmantipersonnelmine   &&  (attackedunit->typ->movemalustyp ==  cmm_trooper ) )
               strength *= 2;

            for ( int j = 1; j < cnt; j++ )
               strength = strength * 2 / 3;

            av.strength += strength;
            cnt++;
         }
   } else {
      Mine& m = mineposition->getMine ( minenum );
      av.strength = m.strength;
      if ( m.type  == cmantipersonnelmine   &&  (attackedunit->typ->movemalustyp ==  cmm_trooper ) )
         av.strength *= 2;
   }

   av.armor = 1;
   av.damage = 0;
   av.experience = 0;
   av.defensebonus = 0;
   av.hemming    = 1;
   av.weapnum = 0;
   av.weapcount = 1;
   av.color = 8;
   av.initiative = 256;
   av.attackbonus = 8;
   av.kamikaze = 0;
   av.height = 0;
   av.weapontype = cwminen;

   dv.strength = 0;
   dv.armor = attackedunit->getArmor();
   dv.damage    = attackedunit->damage;
   dv.experience = attackedunit->experience;
   dv.defensebonus = 0;
   dv.hemming    = 1;
   dv.weapnum = 0;
   dv.weapcount = 0;
   dv.color = attackedunit->getOwner();
   dv.initiative = attackedunit->typ->initiative;
   dv.attackbonus = 0;
   dv.kamikaze = 0;
   dv.height = attackedunit->height;
}


void tmineattacksunit :: setresult ( void )
{
   if ( _minenum == -1 ) {
      for ( tfield::MineContainer::iterator m = _mineposition->mines.begin(); m != _mineposition->mines.end(); )
         if ( m->attacksunit ( _attackedunit ))
            m = _mineposition->mines.erase ( m );
         else
            m++;
   } else
      _mineposition->removemine ( _minenum );

   _attackedunit->damage = dv.damage;

   /* Remove the mined vehicle if it was destroyed */
   if ( _attackedunit->damage >= 100 ) {
     delete *_pattackedunit;
     *_pattackedunit = NULL;
   }

}


Mine* tmineattacksunit :: getFirstMine()
{
   if ( _mineposition && _mineposition->mines.size() )
      return &( * _mineposition->mines.begin() );
   else
      return NULL;
}



tunitattacksobject :: tunitattacksobject ( Vehicle* attackingunit, int obj_x, int obj_y, int weapon )
{
   setup ( attackingunit, obj_x, obj_y, weapon );
}

void tunitattacksobject :: setup ( Vehicle* attackingunit, int obj_x, int obj_y, int weapon )
{

   _x = obj_x;
   _y = obj_y;

   targetField = getfield ( obj_x, obj_y );

   _attackingunit = attackingunit;

   dist = beeline ( attackingunit->xpos, attackingunit->ypos, obj_x, obj_y );

   for ( tfield::ObjectContainer::reverse_iterator o = targetField->objects.rbegin(); o != targetField->objects.rend(); o++ )
      if ( o->typ->armor > 0 ) {
         _obji = &(*o);
         break;
      }

   int _weapon;

   if ( weapon == -1 ) {

      pattackweap atw = attackpossible( attackingunit, obj_x, obj_y );
      int n = -1;
      int s = 0;
      for ( int i = 0; i < atw->count; i++ )
         if ( atw->strength[i] > s ) {
            s = atw->strength[i];
            n = i;
         }

      _weapon = atw->num[n];

      delete atw;

   } else
      _weapon  = weapon;

   const SingleWeapon *weap = &attackingunit->typ->weapons.weapon[weapon];
   av.strength  = int ( ceil( attackingunit->weapstrength[weapon]
                        * WeapDist::getWeaponStrength(weap, targetField->getweather(), dist, attackingunit->height, _obji->typ->getEffectiveHeight() )
                        * attackingunit->typ->weapons.weapon[_weapon].targetingAccuracy[cmm_building] / 100 ));

   av.armor = attackingunit->getArmor();
   av.damage    = attackingunit->damage;
   av.experience = attackingunit->experience;
   av.weapnum    = _weapon;
   av.weapcount   = attackingunit->ammo [ _weapon ];
   av.kamikaze   = attackingunit->typ->hasFunction( ContainerBaseType::KamikazeOnly  );
   av.height = attackingunit->height;
   av.weapontype = attackingunit->typ->weapons.weapon[ _weapon ].getScalarWeaponType();

   tfield* field2 = getfield ( attackingunit->xpos, attackingunit->ypos );

   if ( attackingunit->height <= chfahrend ) {
      av.defensebonus = field2->getdefensebonus();
      // if ( dist <= maxmalq )
         av.attackbonus  = field2->getattackbonus();
      //else
      //   av.attackbonus = 0;
   } else {
      av.defensebonus = 0;
      av.attackbonus  = 0;
   }


   dv.strength = 0;
   dv.attackbonus = 0;

   dv.armor = _obji->typ->armor;
   dv.damage    = _obji->damage;
   dv.experience = 0;

   dv.defensebonus = 0; // field  -> getdefensebonus();
   dv.hemming    = 1;
   dv.color = 8 ;
   dv.kamikaze = 0;
   dv.height = 0;
}


void tunitattacksobject :: setresult ( void )
{
   // _attackingunit->experience = av.experience;
   _attackingunit->ammo[ av.weapnum ] = av.weapcount;

   _attackingunit->postAttack( false );

   _obji->damage    = dv.damage;
   _attackingunit->damage    = av.damage;

   /* Remove the object if it was destroyed */
   if ( _obji->damage >= 100 ) {
     getfield ( _x, _y )-> removeobject ( _obji->typ );
   }

   /* Remove the attacking unit if it was destroyed */
   if ( _attackingunit->damage >= 100 ) {
      delete _attackingunit;
      _attackingunit = NULL;
   }


   actmap->time.set ( actmap->time.turn(), actmap->time.move()+1);

}






pattackweap  attackpossible( const Vehicle*     angreifer, int x, int y)
{
  pattackweap atw = new AttackWeap;

  memset(atw, 0, sizeof(*atw));


   if ((x < 0) || (y < 0) || (x >= actmap->xsize) || (y >= actmap->ysize))
      return atw;
   if (angreifer == NULL)
      return atw;
   if (angreifer->typ->weapons.count == 0)
      return atw;

   tfield* efield = getfield(x,y);

   if ( efield->vehicle ) {
      if (fieldvisiblenow(efield, angreifer->color/8))
         attackpossible2n ( angreifer, efield->vehicle, atw );
   }
   else
      if (efield->building != NULL) {
         if ( actmap->getPlayer(angreifer).diplomacy.isHostile( efield->building->getOwner() ) || efield->building->color == 8*8 )
            for (int i = 0; i < angreifer->typ->weapons.count ; i++)
               if (angreifer->typ->weapons.weapon[i].shootable() )
                  if (angreifer->typ->weapons.weapon[i].offensive() )
                     if ( angreifer->typ->weapons.weapon[i].targetingAccuracy[cmm_building] > 0 ) {
                        int tm = efield->building->typ->buildingheight;
                        if (tm & angreifer->typ->weapons.weapon[i].targ) {
                           if (fieldvisiblenow(efield, angreifer->color/8)) {
                              int d = beeline(angreifer->xpos,angreifer->ypos,x,y);
                              if (d <= angreifer->typ->weapons.weapon[i].maxdistance)
                                 if (d >= angreifer->typ->weapons.weapon[i].mindistance) {
                                    if (angreifer->height & angreifer->typ->weapons.weapon[i].sourceheight)
                                       if ( angreifer->typ->weapons.weapon[i].efficiency[6 + getheightdelta ( log2( angreifer->height), log2(tm))] )
                                          if (angreifer->ammo[i] > 0) {
                                             atw->strength[atw->count ] = angreifer->weapstrength[i];
                                             atw->typ[atw->count ] = 1 << angreifer->typ->weapons.weapon[i].getScalarWeaponType() ;
                                             atw->num[atw->count ] = i;
                                             atw->target = AttackWeap::building;
                                             atw->count++;
                                          }

                                 }
                           }
                        }
                     }
      }

   if ( efield->objects.size() ) {
      int n = 0;
      for ( tfield::ObjectContainer::iterator j = efield->objects.begin(); j != efield->objects.end(); j++ )
         if ( j->typ->armor > 0 )
            n++;

      if ( n > 0 )
         if ((efield->vehicle == NULL) && ( efield->building == NULL)) {
            bool found = false;
            for ( tfield::ObjectContainer::reverse_iterator j = efield->objects.rbegin(); j != efield->objects.rend(); ++j ) {
               for ( int i = 0; i <= angreifer->typ->weapons.count - 1; i++)
                  if (angreifer->typ->weapons.weapon[i].shootable() )
                     if ( angreifer->typ->weapons.weapon[i].getScalarWeaponType() == cwcannonn ||
                          angreifer->typ->weapons.weapon[i].getScalarWeaponType() == cwlasern ||
                          angreifer->typ->weapons.weapon[i].getScalarWeaponType() == cwcruisemissile ||
                          angreifer->typ->weapons.weapon[i].getScalarWeaponType() == cwlargemissilen ||
                          angreifer->typ->weapons.weapon[i].getScalarWeaponType() == cwsmallmissilen ||
                          angreifer->typ->weapons.weapon[i].getScalarWeaponType() == cwbombn ) {
                        if ( angreifer->typ->weapons.weapon[i].targetingAccuracy[cmm_building] )
                              if (fieldvisiblenow(efield, angreifer->color/8)) {
                                 int d = beeline(angreifer->xpos,angreifer->ypos,x,y);
                                 if (d <= angreifer->typ->weapons.weapon[i].maxdistance)
                                    if (d >= angreifer->typ->weapons.weapon[i].mindistance) {
                                       if (angreifer->height & angreifer->typ->weapons.weapon[i].sourceheight )
                                          if ( angreifer->typ->weapons.weapon[i].targ & j->typ->getEffectiveHeight() )
                                             if ( angreifer->typ->weapons.weapon[i].efficiency[6 + getheightdelta ( log2( angreifer->height), log2(j->typ->getEffectiveHeight()))] )
                                                if (angreifer->ammo[i] > 0) {
                                                   atw->strength[atw->count ] = angreifer->weapstrength[i];
                                                   atw->num[atw->count ] = i;
                                                   atw->typ[atw->count ] = 1 << angreifer->typ->weapons.weapon[i].getScalarWeaponType();
                                                   atw->target = AttackWeap::object;
                                                   atw->count++;
                                                   found = true;
                                                }

                                    }
                              }

                     }
               if ( found )
                  return atw;
            }

         }
   }

   return atw;
}


bool attackpossible2u( const Vehicle* attacker, const Vehicle* target, pattackweap atw, int targetheight )
{
   if ( targetheight == -1 )
      targetheight = target->height;

   const Vehicle* angreifer = attacker;
   const Vehicle* verteidiger = target;
   int result = false;
   if ( atw )
      atw->count = 0;

   if ( !angreifer )
     return false ;

   if ( !verteidiger )
     return false ;

   if (angreifer->typ->weapons.count == 0)
     return false ;

   if ( actmap->player[angreifer->getOwner()].diplomacy.isHostile( verteidiger->getOwner() )  )
      for ( int i = 0; i < angreifer->typ->weapons.count ; i++)
         for ( int h = 0; h < 8; h++ )
            if ( targetheight & (1<<h))
               if (angreifer->typ->weapons.weapon[i].shootable() )
                  if (angreifer->typ->weapons.weapon[i].offensive() )
                     if ( (1<<h) & angreifer->typ->weapons.weapon[i].targ )
                        if (angreifer->height & angreifer->typ->weapons.weapon[i].sourceheight )
                           if ( angreifer->typ->weapons.weapon[i].targetingAccuracy[ verteidiger->typ->movemalustyp] > 0 )
                              if ( angreifer->typ->weapons.weapon[i].efficiency[6 + getheightdelta ( log2( angreifer->height), h)] )
                                 if (angreifer->ammo[i] > 0) {
                                    result = true;
                                    if ( atw ) {
                                       atw->strength[atw->count] = angreifer->weapstrength[i] * angreifer->typ->weapons.weapon[i].targetingAccuracy[ verteidiger->typ->movemalustyp] / 100;
                                       atw->num[atw->count ] = i;
                                       atw->typ[atw->count ] = 1 << angreifer->typ->weapons.weapon[i].getScalarWeaponType();
                                       atw->target = AttackWeap::vehicle;
                                       atw->count++;
                                    }
                                 }

   return result;
}



bool attackpossible28( const Vehicle* attacker, const Vehicle* target, pattackweap atw, int targetHeight )
{
   const Vehicle* angreifer = attacker;
   const Vehicle* verteidiger = target;

   if ( targetHeight < 0 )
      targetHeight = target->height;
   
   int result = false;
   if ( atw )
      atw->count = 0;

   if (angreifer == NULL)
     return false ;

   if (verteidiger == NULL)
     return false ;

   if (angreifer->typ->weapons.count == 0)
     return false ;

//   if ( actmap->player[angreifer->getOwner()].diplomacy.isHostile( verteidiger->getOwner() )  )
      for ( int i = 0; i < angreifer->typ->weapons.count ; i++)
         if (angreifer->typ->weapons.weapon[i].shootable() )
            if (angreifer->typ->weapons.weapon[i].offensive() )
               if (targetHeight & angreifer->typ->weapons.weapon[i].targ )
                  if (minmalq <= angreifer->typ->weapons.weapon[i].maxdistance)
                     if (minmalq >= angreifer->typ->weapons.weapon[i].mindistance)
                        if (angreifer->height & angreifer->typ->weapons.weapon[i].sourceheight )
                           if ( angreifer->typ->weapons.weapon[i].targetingAccuracy[ verteidiger->typ->movemalustyp ] > 0)
                              if ( angreifer->typ->weapons.weapon[i].efficiency[6 + getheightdelta ( log2( angreifer->height), log2(targetHeight))] )
                                 if (angreifer->ammo[i] > 0) {
                                    result =  true;
                                    if ( atw ) {
                                       atw->strength[atw->count] = angreifer->weapstrength[i] * angreifer->typ->weapons.weapon[i].targetingAccuracy[ verteidiger->typ->movemalustyp] / 100;
                                       atw->num[atw->count ] = i;
                                       atw->typ[atw->count ] = 1 << angreifer->typ->weapons.weapon[i].getScalarWeaponType();
                                       atw->target = AttackWeap::vehicle;
                                       atw->count++;
                                    }
                                 }

   return result;
}


bool attackpossible2n( const Vehicle* attacker, const Vehicle* target, pattackweap atw )
{
   const Vehicle* angreifer = attacker;
   const Vehicle* verteidiger = target;

   int result = false;
   if ( atw )
      atw->count = 0;

   if (angreifer == NULL)
     return false ;

   if (verteidiger == NULL)
     return false ;

   if (angreifer->typ->weapons.count == 0)
     return false ;

   int dist = beeline ( angreifer, verteidiger );
   if ( actmap->player[angreifer->getOwner()].diplomacy.isHostile( verteidiger->getOwner() ) )
      if ( !angreifer->attacked )
         if ( !angreifer->typ->wait || !angreifer->hasMoved() || angreifer->reactionfire.getStatus() == Vehicle::ReactionFire::ready)
            for ( int i = 0; i < angreifer->typ->weapons.count ; i++)
               if (angreifer->typ->weapons.weapon[i].shootable() )
                  if (angreifer->typ->weapons.weapon[i].offensive() )
                     if (verteidiger->height & angreifer->typ->weapons.weapon[i].targ )
                        if (dist <= angreifer->typ->weapons.weapon[i].maxdistance)
                           if (dist >= angreifer->typ->weapons.weapon[i].mindistance)
                              if (angreifer->height & angreifer->typ->weapons.weapon[i].sourceheight )
                                 if ( angreifer->typ->weapons.weapon[i].efficiency[6 + getheightdelta ( log2( angreifer->height), log2(verteidiger->height))] )
                                    if ( angreifer->typ->weapons.weapon[i].targetingAccuracy[ verteidiger->typ->movemalustyp ] > 0)
                                       if (angreifer->ammo[i] > 0) {
                                          result = true;
                                          if ( atw ) {
                                             atw->strength[atw->count] = angreifer->weapstrength[i] * angreifer->typ->weapons.weapon[i].targetingAccuracy[ verteidiger->typ->movemalustyp ] / 100;
                                             atw->num[atw->count ] = i;
                                             atw->typ[atw->count ] = 1 << angreifer->typ->weapons.weapon[i].getScalarWeaponType();
                                             atw->target = AttackWeap::vehicle;
                                             atw->count++;
                                          }
                                       }


   return result;
}

bool vehicleplattfahrbar( const Vehicle*     vehicle,
                           const tfield*        field)
{
   return false;
/*
   if (vehicle == NULL)
      return ( false );
   if (field == NULL)
      return ( false );
   if (field->vehicle == NULL)
      return ( false );

   if ((vehicle->color != field->vehicle->color) &&
      (vehicle->height == chfahrend) &&
      (field->vehicle->height == chfahrend) && 
      (field->vehicle->functions & cftrooper) && 
      (vehicle->weight() >= fusstruppenplattfahrgewichtsfaktor * field->vehicle->weight()))
      return ( true ); 
   return ( false );
*/
} 


float WeapDist::getWeaponStrength ( const SingleWeapon* weap, int weather, int dist, int attacker_height, int defender_height, int reldiff  )
{
/*
  int         translat[31]  = { 6, 255, 1, 3, 2, 4, 0, 5, 255, 255, 6, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
                                 255, 255, 255, 255, 255, 255};
*/

   if ( !weap )
      return 0;

   int scalar = weap->getScalarWeaponType();
   if ( scalar >= 31 || scalar < 0 )
      return 0;

   if ( scalar == 1 )     // mine
      return 1;

      /*
   int typ = translat[ scalar ];
   if ( typ == 255 ) {
      displaymessage("tweapdist::getweapstrength: invalid type ", 1 );
      return 1;
   }
   */


   if ( weap->maxdistance == 0 )
      return 0;

   if ( weap->minstrength == 0 )
      return 0;

   if ( reldiff == -1) {
      if ( dist < weap->mindistance || dist > weap->maxdistance ) {
         warning("tweapdist::getweapstrength: invalid range: \n min = " + ASCString::toString(weap->mindistance ) + " ; max = " + ASCString::toString( weap->maxdistance ) + " ; req = " + ASCString::toString( dist));
         return 0;
      }

      if ( weap->maxdistance - weap->mindistance != 0 )
         reldiff = 255 * (dist - weap->mindistance) / ( weap->maxdistance - weap->mindistance) ;
      else
         reldiff = 0;
   }

//   int minstrength = 255 - 255 * weap->minstrength / weap->maxstrength;

//   int relstrength = 255 - ( 255 - data[typ][reldiff] ) * minstrength / ( 255 - data[typ][255] );

   float relpos = float(reldiff) / 255.0;

/*   if ( weap->maxstrength && weap->minstrength )
      relpos /= float(weap->maxstrength) / float(weap->minstrength);

   return 1.0 - relpos;*/


   float relstrength = weap->maxstrength - relpos * ( weap->maxstrength - weap->minstrength );

   float weatherFactor = 1;
   int heightEff = 100;
   if ( attacker_height != -1 && defender_height != -1 ) {
      int hd = getheightdelta ( log2 ( attacker_height ), log2 ( defender_height ));
      heightEff = weap->efficiency[6+hd];

      if ( attacker_height >= chtieffliegend && weather != 0 && defender_height != -1) {
         int weatherRelevantHeightDelta = min( abs( getheightdelta ( log2 ( attacker_height ), log2 ( defender_height ))), 3);
         if ( weather == 1 || weather == 3 || weather == 5 )
            weatherFactor = 1 - 0.07*weatherRelevantHeightDelta;
         else
            if ( weather == 2 || weather == 4 )
               weatherFactor = 1 - 0.2 * weatherRelevantHeightDelta;
      }

      if ( attacker_height == chsatellit )
         weatherFactor = 1 - (1-weatherFactor)/2;
   }


   return relstrength * heightEff * weatherFactor / float(weap->maxstrength * 100) ;

/*   if ( attacker_height != -1 && defender_height!= -1 ) {
      int hd = getheightdelta ( log2 ( attacker_height ), log2 ( defender_height ));
      return relstrength * weap->efficiency[6+hd] / 100 ;
   } else
      return relstrength ;
      */
}


