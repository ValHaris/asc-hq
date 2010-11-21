/*! \file attack.cpp
    \brief All the routines for fighting in ASC
*/


/*
    This file is part of Advanced Strategic Command; http://www.asc-hq.de
    Copyright (C) 1994-2010  Martin Bickel  and  Marc Schellenberger

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
#include <iostream>

#include "typen.h"
#include "buildingtype.h"
#include "vehicletype.h"
#include "attack.h"
#include "spfst.h"
#include "gameoptions.h"

#include "actions/changeunitproperty.h"
#include "actions/consumeammo.h"
#include "actions/registerunitrftarget.h"
#include "actions/inflictdamage.h"
#include "actions/removemine.h"
#include "actions/removeobject.h"
#include "actions/changeobjectproperty.h"


AttackFormula::AttackFormula( GameMap* gamemap ) 
{
   this->gamemap = gamemap;
}

bool  AttackFormula :: checkHemming ( Vehicle*     d_eht,  int     direc )
{ 
   Vehicle*     s_eht;

   int x = d_eht->xpos;
   int y = d_eht->ypos; 
   getnextfield(x, y, direc);
   MapField* fld = d_eht->getMap()->getField(x,y);

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
      warningMessage("float AttackFormula :: getHemmingFactor - invalid direction" );
      
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
   if ( experience < 0 )
      return 0.0;
   // =1+(($B$33-1)*(1-(0.1^(1/$B$34))^B48))
   float ninety = gamemap->getgameparameter( cgp_experienceAt90percentbonus );
   float maxExpBonus = ((float)gamemap->getgameparameter( cgp_maxAttackExperienceBonus ))/100.0;
   float e = maxExpBonus * ( 1.0 - pow( pow( 0.1, 1.0/ninety), experience ));
   return e;
}

float AttackFormula :: defense_experience ( int experience )
{
   if ( experience < 0 )
      return 0.0;
   
   float ninety = gamemap->getgameparameter( cgp_experienceAt90percentbonus );
   float maxExpBonus = ((float)gamemap->getgameparameter( cgp_maxDefenseExperienceBonus ))/100.0;
   float e = maxExpBonus * (1.0 - pow( pow( 0.1, 1.0/ninety), experience ));
   return e;
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



void tfight :: calc ( void )
{
   int damagefactor = gamemap->getgameparameter ( cgp_attackPower );
   const float armordivisor = 5;


   if ( av.strength ) { 
      float absstrength = float(av.strength )
                          * ( 1 + strength_experience ( av.experience_offensive ) + strength_attackbonus ( av.attackbonus ) )
                          * strength_damage ( av.damage ) 
                          * dv.hemming;

      float absdefense = float(dv.armor / armordivisor )
                          * ( 1 + defense_defensebonus ( dv.defensebonus ) + defense_experience ( dv.experience_defensive ) );

      int w = int( ceil(dv.damage + absstrength / absdefense * 1000 / damagefactor ));

      if (dv.damage > w ) 
         warningMessage("fatal error at attack: \ndecrease of damage d!");

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


      av.experience_offensive++;

      if ( dist <= 10 && dv.strength > 0 )
         av.experience_offensive += 1;

      if ( dv.damage >= 100 ) 
         av.experience_offensive += 1;

      if ( av.experience_offensive > maxunitexperience )
         av.experience_offensive = maxunitexperience;

      dv.experience_defensive += 1;
      if ( dv.experience_defensive > maxunitexperience )
         dv.experience_defensive = maxunitexperience;
      
   } 

   if ( dv.strength ) { 
      float absstrength = float(dv.strength )
                          * ( 1 + strength_experience ( dv.experience_offensive ) + strength_attackbonus ( dv.attackbonus ) )
                          * strength_damage ( dv.damage ) ;

      float absdefense = float(av.armor / armordivisor)
                          * ( 1 + defense_defensebonus ( av.defensebonus )+ defense_experience ( av.experience_defensive ));

      int w = int( ceil(av.damage + absstrength / absdefense * 1000 / damagefactor ));

      if (av.damage > w ) 
         warningMessage("fatal error at attack: \ndecrease of damage a!");

      if (w > 100) 
         av.damage = 100; 
      else 
         av.damage = w; 


      if ( dv.weapcount > 0 )
         dv.weapcount--;
      else
         dv.weapcount = 0;

      if ( av.damage >= 100 ) {
         dv.experience_offensive += 2;
         if ( dv.experience_offensive > maxunitexperience )
            dv.experience_offensive = maxunitexperience;

      } else
        if ( dv.experience_offensive < maxunitexperience )
           dv.experience_offensive++;
        
      av.experience_defensive += 1;
      if ( av.experience_defensive > maxunitexperience )
         av.experience_defensive = maxunitexperience;
        
   } 

   if ( av.kamikaze ) 
      av.damage = 100;

}



tunitattacksunit :: tunitattacksunit ( Vehicle* &attackingunit, Vehicle* &attackedunit, bool respond, int weapon, bool reactionfire )
   : UnitAttacksSomething( attackingunit->getMap() )
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
      AttackWeap* atw = attackpossible( attackingunit, attackedunit->xpos, attackedunit->ypos );
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


   int targetWeather = attackedunit->getMap()->getField( attackedunit->getPosition() )->getWeather();

   av.strength = int ( ceil( attackingunit->weapstrength[_weapon]
                        * WeapDist::getWeaponStrength(weap, targetWeather, dist, attackingunit->height, attackedunit->height )
                        * attackingunit->typ->weapons.weapon[_weapon].targetingAccuracy[attackedunit->typ->movemalustyp] / 100 ));
   av.armor  = attackingunit->getArmor();
   av.damage     = attackingunit->damage;
   av.experience_offensive  = attackingunit->experience_offensive;
   av.experience_defensive  = attackingunit->experience_defensive;
   av.hemming    = 1;
   av.weapnum    = _weapon;
   av.weapcount  = attackingunit->ammo [ _weapon ];
   av.color      = attackingunit->getOwner();
   av.initiative = attackingunit->typ->initiative;
   av.kamikaze   = attackingunit->typ->hasFunction( ContainerBaseType::KamikazeOnly  );
   av.height = attackingunit->height;
   av.weapontype = attackingunit->typ->weapons.weapon[ _weapon ].getScalarWeaponType();

   MapField* field = attackingunit->getMap()->getField ( attackingunit->xpos, attackingunit->ypos );

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


      int attackerWeather = attackingunit->getMap()->getField( attackingunit->getPosition() )->getWeather();

      dv.strength  = int ( ceil( attackedunit->weapstrength[ dv.weapnum ]
                           * WeapDist::getWeaponStrength(weap, attackerWeather, dist, attackedunit->height, attackingunit->height )
                           * attackedunit->typ->weapons.weapon[ dv.weapnum ].targetingAccuracy[attackingunit->typ->movemalustyp] / 100 ));
      field = attackingunit->getMap()->getField ( attackedunit->xpos, attackedunit->ypos );
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
   dv.experience_offensive = attackedunit->experience_offensive;
   dv.experience_defensive = attackedunit->experience_defensive;
   if ( dist <= maxmalq && attackingunit->height < chtieffliegend )
      dv.hemming = strength_hemming ( attackingunit->xpos, attackingunit->ypos, attackedunit );
   else
      dv.hemming = 1;


   if ( attackedunit->height <= chfahrend )
      dv.defensebonus = attackedunit->getMap()->getField ( attackedunit->xpos, attackedunit->ypos ) -> getdefensebonus();
   else
      dv.defensebonus = 0;

   dv.color      = attackedunit->getOwner();
   dv.initiative = attackedunit->typ->initiative;
   dv.kamikaze = 0;
   dv.height = attackedunit->height;
}


void log( const Vehicle* attacker, const Vehicle* attackee )
{
   if( CGameOptions::Instance()->logKillsToConsole ) 
      std::cout << attackee->getOwner() << ";" << attacker->getOwner() << ";" << attackee->typ->id  << ";"
            << attackee->typ->name  << ";" << attackee->experience_offensive << ";" << attacker->getMap()->time.turn() <<  "\n" ;
}

void tunitattacksunit :: setresult( const Context& context )
{
   int nwid = _attackingunit->networkid;
   GameMap* map = _attackingunit->getMap();
   
   GameAction* a = new ChangeUnitProperty( _attackingunit, ChangeUnitProperty::ExperienceOffensive, av.experience_offensive );
   a->execute ( context );
   
   GameAction* a2 = new ChangeUnitProperty( _attackingunit, ChangeUnitProperty::ExperienceDefensive, av.experience_defensive );
   a2->execute ( context );
   
   GameAction* b = new ConsumeAmmo( _attackingunit, _attackingunit->typ->weapons.weapon[av.weapnum].getScalarWeaponType(), av.weapnum, _attackingunit->ammo[ av.weapnum ] - av.weapcount );
   b->execute ( context );
   
   _attackingunit->postAttack( reactionfire, context );
   
   GameAction* c = new RegisterUnitRFTarget( map, nwid, av.weapnum, _attackedunit->networkid  );
   c->execute ( context );
   
   if ( _respond ) {
      GameAction* d = new ChangeUnitProperty( _attackedunit, ChangeUnitProperty::ExperienceOffensive, dv.experience_offensive );
      d->execute ( context );
      
      GameAction* e = new ConsumeAmmo( _attackedunit, _attackedunit->typ->weapons.weapon[dv.weapnum].getScalarWeaponType(), dv.weapnum, _attackedunit->ammo[ dv.weapnum ] - dv.weapcount );
      e->execute ( context );
   }
   
   GameAction* d2 = new ChangeUnitProperty( _attackedunit, ChangeUnitProperty::ExperienceDefensive, dv.experience_defensive );
   d2->execute ( context );
   
   
   GameAction* f = new InflictDamage( _attackingunit, av.damage - _attackingunit->damage );
   f->execute ( context );
   
   if( av.damage >= 100 )
      log ( _attackedunit, _attackingunit );

   if( dv.damage >= 100 )
      log ( _attackingunit, _attackedunit );
   
   GameAction* g = new InflictDamage( _attackedunit, dv.damage - _attackedunit->damage  );
   g->execute ( context );
   

   /* If the attacking vehicle was destroyed, remove it */
   if ( av.damage >= 100 ) {
     *_pattackingunit = NULL;
   }

   /* If the attacked vehicle was destroyed, remove it */
   if ( dv.damage >= 100 ) {
     *_pattackedunit = NULL;
   }
}







tunitattacksbuilding :: tunitattacksbuilding ( Vehicle* attackingunit, int x, int y, int weapon )
   : UnitAttacksSomething( attackingunit->getMap() )
{
   setup ( attackingunit, x, y, weapon );
}


void tunitattacksbuilding :: setup ( Vehicle* attackingunit, int x, int y, int weapon )
{
   _attackingunit = attackingunit;
   _x = x;
   _y = y;
   _attackedbuilding  = attackingunit->getMap()->getField ( x, y ) -> building;

   dist = beeline ( attackingunit->xpos, attackingunit->ypos, x, y );
   int _weapon;

   if ( weapon == -1 ) {
      AttackWeap* atw = attackpossible( attackingunit, x, y );
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

   int targetWeather = attackingunit->getMap()->getField(x,y)->getWeather();

   av.strength  = int (ceil( attackingunit->weapstrength[_weapon]
                        * WeapDist::getWeaponStrength(weap, targetWeather, dist, attackingunit->height, _attackedbuilding->typ->height )
                        * attackingunit->typ->weapons.weapon[_weapon].targetingAccuracy[cmm_building] / 100 ));

   av.armor = attackingunit->getArmor();
   av.damage    = attackingunit->damage;
   av.experience_offensive = attackingunit->experience_offensive;
   av.experience_defensive = attackingunit->experience_defensive;
   av.hemming    = 1;
   av.weapnum    = _weapon;
   av.weapcount  = attackingunit->ammo [ _weapon ];
   av.weapontype = attackingunit->typ->weapons.weapon[ _weapon ].getScalarWeaponType();
   av.color      = attackingunit->getOwner();
   av.initiative = attackingunit->typ->initiative;
   av.kamikaze   = attackingunit->typ->hasFunction( ContainerBaseType::KamikazeOnly  );
   av.height = attackingunit->height;

   MapField* field = attackingunit->getMap()->getField ( attackingunit->xpos, attackingunit->ypos );

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
   dv.experience_offensive = 0;
   dv.experience_defensive = 0;
   dv.hemming    = 1;
   dv.weapnum = -1;

   dv.defensebonus = 0; // getfield ( x, y ) -> getdefensebonus();
   dv.color      = _attackedbuilding->getOwner();
   dv.initiative = 0;
   dv.kamikaze = 0;
   dv.height = _attackedbuilding->typ->height;
}


void tunitattacksbuilding :: setresult( const Context& context )
{
   MapCoordinate target = _attackedbuilding->getPosition();
   
   GameAction* b = new ConsumeAmmo( _attackingunit, _attackingunit->typ->weapons.weapon[av.weapnum].getScalarWeaponType(), av.weapnum, _attackingunit->ammo[ av.weapnum ] - av.weapcount );
   b->execute ( context );
   
   _attackingunit->postAttack( false, context );
   
   GameAction* f = new InflictDamage( _attackingunit, av.damage - _attackingunit->damage );
   f->execute ( context );
   
   GameAction* g = new InflictDamage( _attackedbuilding, dv.damage - _attackedbuilding->damage  );
   g->execute ( context );
}



tmineattacksunit :: tmineattacksunit ( const MapCoordinate& mineposition, int minenum, Vehicle* &attackedunit )
   : tfight( attackedunit->getMap() )
{
   setup ( mineposition, minenum, attackedunit );
}

void tmineattacksunit :: setup ( const MapCoordinate& position, int minenum, Vehicle* &attackedunit )
{
   this->position = position;
   
   MapField* mineposition = attackedunit->getMap()->getField( position );
   
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
      for ( MapField::MineContainer::iterator m = mineposition->mines.begin(); m != mineposition->mines.end(); m++ )
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
   av.experience_offensive = 0;
   av.experience_defensive = 0;
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
   dv.experience_offensive = attackedunit->experience_offensive;
   dv.experience_defensive = attackedunit->experience_defensive;
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


void tmineattacksunit :: setresult( const Context& context )
{
   vector<GameAction*> actions;
   if ( _minenum == -1 ) {
      for ( MapField::MineContainer::iterator m = _mineposition->mines.begin(); m != _mineposition->mines.end(); ++m)
         if ( m->attacksunit ( _attackedunit ))
            actions.push_back ( new RemoveMine(_attackedunit->getMap(), position, m->identifier));
   } else {
      int counter = 0;
      for ( MapField::MineContainer::iterator m = _mineposition->mines.begin(); m != _mineposition->mines.end(); ++m, ++counter)
         if ( counter == _minenum )
            actions.push_back ( new RemoveMine(_attackedunit->getMap(), position, m->identifier));
   }

   for ( vector<GameAction*>::iterator i = actions.begin(); i != actions.end(); ++i )
      (*i)->execute( context );
      
   
   (new InflictDamage( _attackedunit, dv.damage - _attackedunit->damage  ))->execute ( context );
 
}


Mine* tmineattacksunit :: getFirstMine()
{
   if ( _mineposition && _mineposition->mines.size() )
      return &( * _mineposition->mines.begin() );
   else
      return NULL;
}



tunitattacksobject :: tunitattacksobject ( Vehicle* attackingunit, int obj_x, int obj_y, int weapon )
   : UnitAttacksSomething( attackingunit->getMap() )
{
   setup ( attackingunit, obj_x, obj_y, weapon );
}

void tunitattacksobject :: setup ( Vehicle* attackingunit, int obj_x, int obj_y, int weapon )
{

   _x = obj_x;
   _y = obj_y;

   targetField = attackingunit->getMap()->getField ( obj_x, obj_y );

   _attackingunit = attackingunit;

   dist = beeline ( attackingunit->xpos, attackingunit->ypos, obj_x, obj_y );

   for ( MapField::ObjectContainer::reverse_iterator o = targetField->objects.rbegin(); o != targetField->objects.rend(); o++ )
      if ( o->typ->armor > 0 ) {
         _obji = &(*o);
         break;
      }

   int _weapon;

   if ( weapon == -1 ) {

      AttackWeap* atw = attackpossible( attackingunit, obj_x, obj_y );
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
                        * WeapDist::getWeaponStrength(weap, targetField->getWeather(), dist, attackingunit->height, _obji->typ->getEffectiveHeight() )
                        * attackingunit->typ->weapons.weapon[_weapon].targetingAccuracy[cmm_building] / 100 ));

   av.armor = attackingunit->getArmor();
   av.damage    = attackingunit->damage;
   av.experience_offensive = attackingunit->experience_offensive;
   av.experience_defensive = attackingunit->experience_defensive;
   av.weapnum    = _weapon;
   av.weapcount   = attackingunit->ammo [ _weapon ];
   av.kamikaze   = attackingunit->typ->hasFunction( ContainerBaseType::KamikazeOnly  );
   av.height = attackingunit->height;
   av.weapontype = attackingunit->typ->weapons.weapon[ _weapon ].getScalarWeaponType();

   MapField* field2 = attackingunit->getMap()->getField ( attackingunit->xpos, attackingunit->ypos );

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
   dv.experience_offensive = 0;
   dv.experience_defensive = 0;

   dv.defensebonus = 0; // field  -> getdefensebonus();
   dv.hemming    = 1;
   dv.color = 8 ;
   dv.kamikaze = 0;
   dv.height = 0;
}


void tunitattacksobject :: setresult( const Context& context )
{
   GameMap* map = _attackingunit->getMap();
   
   GameAction* b = new ConsumeAmmo( _attackingunit, _attackingunit->typ->weapons.weapon[av.weapnum].getScalarWeaponType(), av.weapnum, _attackingunit->ammo[ av.weapnum ] - av.weapcount );
   b->execute ( context );
   
   _attackingunit->postAttack( false, context );
   
   GameAction* f = new InflictDamage( _attackingunit, av.damage - _attackingunit->damage );
   f->execute ( context );
   
   
   MapCoordinate position( _x, _y );
   
   if ( dv.damage >= 100 ) {
      (new RemoveObject(map, position, _obji->typ->id))->execute(context);
   } else {
      GameAction* g = new ChangeObjectProperty( map, position, _obji, ChangeObjectProperty::Damage, dv.damage );
      g->execute ( context );
   }
}





AttackWeap*  attackpossible( const Vehicle*     attacker, int x, int y)
{
  AttackWeap* atw = new AttackWeap;

  memset(atw, 0, sizeof(*atw));


   if ((x < 0) || (y < 0) || (x >= attacker->getMap()->xsize) || (y >= attacker->getMap()->ysize))
      return atw;
   if (attacker == NULL)
      return atw;
   if (attacker->typ->weapons.count == 0)
      return atw;

   MapField* efield = attacker->getMap()->getField(x,y);

   if ( efield->getVehicle() ) {
      if (fieldvisiblenow(efield, attacker->color/8))
         attackpossible2n ( attacker, efield->getVehicle(), atw );
   }
   else if (efield->building != NULL) {
         if ( attacker->getMap()->getPlayer(attacker).diplomacy.isHostile( efield->building->getOwner() ) || efield->building->color == 8*8 )
            for (int i = 0; i < attacker->typ->weapons.count ; i++)
               if (attacker->typ->weapons.weapon[i].shootable() )
                  if (attacker->typ->weapons.weapon[i].offensive() )
                     if ( attacker->typ->weapons.weapon[i].targetingAccuracy[cmm_building] > 0 ) {
                        int tm = efield->building->typ->height;
                        if (tm & attacker->typ->weapons.weapon[i].targ) {
                           if (fieldvisiblenow(efield, attacker->color/8)) {
                              int d = beeline(attacker->xpos,attacker->ypos,x,y);
                              if (d <= attacker->typ->weapons.weapon[i].maxdistance)
                                 if (d >= attacker->typ->weapons.weapon[i].mindistance) {
                                    if (attacker->height & attacker->typ->weapons.weapon[i].sourceheight)
                                       if ( attacker->typ->weapons.weapon[i].efficiency[6 + getheightdelta ( getFirstBit( attacker->height), getFirstBit(tm))] )
                                          if (attacker->ammo[i] > 0) {
                                             atw->strength[atw->count ] = attacker->weapstrength[i];
                                             atw->typ[atw->count ] = 1 << attacker->typ->weapons.weapon[i].getScalarWeaponType() ;
                                             atw->num[atw->count ] = i;
                                             atw->target = AttackWeap::building;
                                             atw->count++;
                                          }

                                 }
                           }
                        }
                     }
   } else if ( efield->objects.size() ) {
      int n = 0;
      for ( MapField::ObjectContainer::iterator j = efield->objects.begin(); j != efield->objects.end(); j++ )
         if ( j->typ->armor > 0 )
            n++;

      if ( n > 0 )
         if ((efield->vehicle == NULL) && ( efield->building == NULL)) {
            bool found = false;
            for ( MapField::ObjectContainer::reverse_iterator j = efield->objects.rbegin(); j != efield->objects.rend(); ++j ) {
               for ( int i = 0; i <= attacker->typ->weapons.count - 1; i++)
                  if (attacker->typ->weapons.weapon[i].shootable() )
                     if ( attacker->typ->weapons.weapon[i].getScalarWeaponType() == cwcannonn ||
                          attacker->typ->weapons.weapon[i].getScalarWeaponType() == cwlasern ||
                          attacker->typ->weapons.weapon[i].getScalarWeaponType() == cwcruisemissile ||
                          attacker->typ->weapons.weapon[i].getScalarWeaponType() == cwlargemissilen ||
                          attacker->typ->weapons.weapon[i].getScalarWeaponType() == cwsmallmissilen ||
                          attacker->typ->weapons.weapon[i].getScalarWeaponType() == cwtorpedon ||
                          attacker->typ->weapons.weapon[i].getScalarWeaponType() == cwbombn ) {
                        if ( attacker->typ->weapons.weapon[i].targetingAccuracy[cmm_building] )
                              if (fieldvisiblenow(efield, attacker->color/8)) {
                                 int d = beeline(attacker->xpos,attacker->ypos,x,y);
                                 if (d <= attacker->typ->weapons.weapon[i].maxdistance)
                                    if (d >= attacker->typ->weapons.weapon[i].mindistance) {
                                       if (attacker->height & attacker->typ->weapons.weapon[i].sourceheight )
                                          if ( attacker->typ->weapons.weapon[i].targ & j->typ->getEffectiveHeight() )
                                             if ( attacker->typ->weapons.weapon[i].efficiency[6 + getheightdelta ( getFirstBit( attacker->height), getFirstBit(j->typ->getEffectiveHeight()))] )
                                                if (attacker->ammo[i] > 0) {
                                                   atw->strength[atw->count ] = attacker->weapstrength[i];
                                                   atw->num[atw->count ] = i;
                                                   atw->typ[atw->count ] = 1 << attacker->typ->weapons.weapon[i].getScalarWeaponType();
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


bool attackpossible2u( const Vehicle* attacker, const Vehicle* target, AttackWeap* atw, int targetheight )
{
   if ( targetheight == -1 )
      targetheight = target->height;

   int result = false;
   if ( atw )
      atw->count = 0;

   if ( !attacker )
     return false ;

   if ( !target )
     return false ;

   if (attacker->typ->weapons.count == 0)
     return false ;

   if ( attacker->getMap()->player[attacker->getOwner()].diplomacy.isHostile( target->getOwner() )  )
      for ( int i = 0; i < attacker->typ->weapons.count ; i++)
         for ( int h = 0; h < 8; h++ )
            if ( targetheight & (1<<h))
               if (attacker->typ->weapons.weapon[i].shootable() )
                  if (attacker->typ->weapons.weapon[i].offensive() )
                     if ( (1<<h) & attacker->typ->weapons.weapon[i].targ )
                        if (attacker->height & attacker->typ->weapons.weapon[i].sourceheight )
                           if ( attacker->typ->weapons.weapon[i].targetingAccuracy[ target->typ->movemalustyp] > 0 )
                              if ( attacker->typ->weapons.weapon[i].efficiency[6 + getheightdelta ( getFirstBit( attacker->height), h)] )
                                 if (attacker->ammo[i] > 0) {
                                    result = true;
                                    if ( atw ) {
                                       atw->strength[atw->count] = attacker->weapstrength[i] * attacker->typ->weapons.weapon[i].targetingAccuracy[ target->typ->movemalustyp] / 100;
                                       atw->num[atw->count ] = i;
                                       atw->typ[atw->count ] = 1 << attacker->typ->weapons.weapon[i].getScalarWeaponType();
                                       atw->target = AttackWeap::vehicle;
                                       atw->count++;
                                    }
                                 }

   return result;
}



bool attackpossible28( const Vehicle* attacker, const Vehicle* target, AttackWeap* atw, int targetHeight )
{
   if ( targetHeight < 0 )
      targetHeight = target->height;
   
   bool result = false;
   if ( atw )
      atw->count = 0;

   if (attacker == NULL)
     return false ;

   if (target == NULL)
     return false ;

   if (attacker->typ->weapons.count == 0)
     return false ;

//   if ( attacker->getMap()->player[attacker->getOwner()].diplomacy.isHostile( target->getOwner() )  )
      for ( int i = 0; i < attacker->typ->weapons.count ; i++)
         if (attacker->typ->weapons.weapon[i].shootable() )
            if (attacker->typ->weapons.weapon[i].offensive() )
               if (targetHeight & attacker->typ->weapons.weapon[i].targ )
                  if (minmalq <= attacker->typ->weapons.weapon[i].maxdistance)
                     if (minmalq >= attacker->typ->weapons.weapon[i].mindistance)
                        if (attacker->height & attacker->typ->weapons.weapon[i].sourceheight )
                           if ( attacker->typ->weapons.weapon[i].targetingAccuracy[ target->typ->movemalustyp ] > 0)
                              if ( attacker->typ->weapons.weapon[i].efficiency[6 + getheightdelta ( getFirstBit( attacker->height), getFirstBit(targetHeight))] )
                                 if (attacker->ammo[i] > 0) {
                                    result =  true;
                                    if ( atw ) {
                                       atw->strength[atw->count] = attacker->weapstrength[i] * attacker->typ->weapons.weapon[i].targetingAccuracy[ target->typ->movemalustyp] / 100;
                                       atw->num[atw->count ] = i;
                                       atw->typ[atw->count ] = 1 << attacker->typ->weapons.weapon[i].getScalarWeaponType();
                                       atw->target = AttackWeap::vehicle;
                                       atw->count++;
                                    }
                                 }

   return result;
}


bool attackpossible2n( const Vehicle* attacker, const Vehicle* target, AttackWeap* atw )
{
   int result = false;
   if ( atw )
      atw->count = 0;

   if (attacker == NULL)
     return false ;

   if (target == NULL)
     return false ;

   if (attacker->typ->weapons.count == 0)
     return false ;

   int dist = beeline ( attacker, target );
   if ( attacker->getMap()->player[attacker->getOwner()].diplomacy.isHostile( target->getOwner() ) )
      if ( !attacker->attacked )
         if ( !attacker->typ->wait || !attacker->hasMoved() || attacker->reactionfire.getStatus() == Vehicle::ReactionFire::ready)
            for ( int i = 0; i < attacker->typ->weapons.count ; i++)
               if (attacker->typ->weapons.weapon[i].shootable() )
                  if (attacker->typ->weapons.weapon[i].offensive() )
                     if (target->height & attacker->typ->weapons.weapon[i].targ )
                        if (dist <= attacker->typ->weapons.weapon[i].maxdistance)
                           if (dist >= attacker->typ->weapons.weapon[i].mindistance)
                              if (attacker->height & attacker->typ->weapons.weapon[i].sourceheight )
                                 if ( attacker->typ->weapons.weapon[i].efficiency[6 + getheightdelta ( getFirstBit( attacker->height), getFirstBit(target->height))] )
                                    if ( attacker->typ->weapons.weapon[i].targetingAccuracy[ target->typ->movemalustyp ] > 0)
                                       if (attacker->ammo[i] > 0) {
                                          result = true;
                                          if ( atw ) {
                                             atw->strength[atw->count] = attacker->weapstrength[i] * attacker->typ->weapons.weapon[i].targetingAccuracy[ target->typ->movemalustyp ] / 100;
                                             atw->num[atw->count ] = i;
                                             atw->typ[atw->count ] = 1 << attacker->typ->weapons.weapon[i].getScalarWeaponType();
                                             atw->target = AttackWeap::vehicle;
                                             atw->count++;
                                          }
                                       }


   return result;
}

bool vehicleplattfahrbar( const Vehicle*     vehicle,
                           const MapField*        field)
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
         warningMessage("tweapdist::getweapstrength: invalid range: \n min = " + ASCString::toString(weap->mindistance ) + " ; max = " + ASCString::toString( weap->maxdistance ) + " ; req = " + ASCString::toString( dist));
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
      int hd = getheightdelta ( getFirstBit ( attacker_height ), getFirstBit ( defender_height ));
      heightEff = weap->efficiency[6+hd];

      if ( attacker_height >= chtieffliegend && weather != 0 && defender_height != -1) {
         int weatherRelevantHeightDelta = min( abs( getheightdelta ( getFirstBit ( attacker_height ), getFirstBit ( defender_height ))), 3);
         if ( weather == 1 || weather == 3  )
            weatherFactor = 1 - 0.07*weatherRelevantHeightDelta;
         else
            if ( weather == 2 || weather == 4 || weather == 5 )
               weatherFactor = 1 - 0.2 * weatherRelevantHeightDelta;
      }

      if ( attacker_height == chsatellit )
         weatherFactor = 1 - (1-weatherFactor)/2;
   }


   return relstrength * heightEff * weatherFactor / float(weap->maxstrength * 100) ;

/*   if ( attacker_height != -1 && defender_height!= -1 ) {
      int hd = getheightdelta ( getFirstBit ( attacker_height ), getFirstBit ( defender_height ));
      return relstrength * weap->efficiency[6+hd] / 100 ;
   } else
      return relstrength ;
      */
}


