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
#include "tpascal.inc"
#include "typen.h"
#include "buildingtype.h"
#include "vehicletype.h"
#include "attack.h"
#include "newfont.h"
#include "basegfx.h"
#include "spfst.h"
#include "dlg_box.h"
#include "sgstream.h"
#include "events.h"
#include "loaders.h"
#include "gameoptions.h"
#include "viewcalculation.h"
#include "graphics/drawing.h"

#include "mapalgorithms.h"
#include "paradialog.h"
#define USE_COLOR_CONSTANTS
#include "pgcolors.h"
#include "iconrepository.h"
#ifdef sgmain
 #include "soundList.h"
#endif


bool  AttackFormula :: checkHemming ( Vehicle*     d_eht,  int     direc )
{ 
   Vehicle*     s_eht;

   int x = d_eht->xpos;
   int y = d_eht->ypos; 
   getnextfield(x, y, direc);
   pfield fld = getfield(x,y);

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
      displaymessage("float AttackFormula :: getHemmingFactor - invalid direction", 1 );
      
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

      int w = int(dv.damage + absstrength / absdefense * 1000 / damagefactor );

      if (dv.damage > w ) 
         displaymessage("fatal error at attack: \ndecrease of damage d!",1);

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

      if ( dv.damage >= 100 ) {
         av.experience += 2;
         if ( av.experience > maxunitexperience )
            av.experience = maxunitexperience;

      } else
        if ( av.experience < maxunitexperience )
           av.experience++;
   } 

   if ( dv.strength ) { 
      float absstrength = float(dv.strength )
                          * ( 1 + strength_experience ( dv.experience ) + strength_attackbonus ( dv.attackbonus ) )
                          * strength_damage ( dv.damage ) ;

      float absdefense = float(av.armor / armordivisor)
                          * ( 1 + defense_defensebonus ( av.defensebonus )+ defense_experience ( av.experience ));

      int w = int(av.damage + absstrength / absdefense * 1000 / damagefactor );

      if (av.damage > w ) 
         displaymessage("fatal error at attack: \ndecrease of damage a!",1);

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




#define maxdefenseshown 2
#define maxattackshown 2

void tunitattacksunit::calcdisplay( int ad, int dd ) {
  #ifdef sgmain
  SoundList::getInstance().playSound ( SoundList::shooting, _attackingunit->getWeapon(av.weapnum)->getScalarWeaponType(), false, _attackingunit->getWeapon(av.weapnum)->soundLabel );
  #endif
  tfight::calcdisplay(ad,dd);
  #ifdef sgmain
   if ( av.damage >= 100  )
      SoundList::getInstance().playSound( SoundList::unitExplodes , 0, false, _attackingunit->typ->killSoundLabel );
   if ( dv.damage >= 100 )
      SoundList::getInstance().playSound( SoundList::unitExplodes , 0, false, _attackedunit->typ->killSoundLabel );


  #endif
}


class AttackPanel : public Panel {
       tfight& engine;
       int attacker_exp;
       int defender_exp;
     public:
        AttackPanel ( tfight& engine_ ) ;
        void setBarGraphValue( const ASCString& widgetName, float fraction ) { Panel::setBargraphValue( widgetName, fraction ); };
      void setLabelText ( const ASCString& widgetName, int i ) { Panel::setLabelText ( widgetName, i ); };
      void setLabelText ( const ASCString& widgetName, const ASCString& i ) { Panel::setLabelText ( widgetName, i ); };
      void dispValue ( const ASCString& name, float value, float maxvalue, PG_Color color );
      void setBarGraphColor( const ASCString& widgetName, PG_Color color ) { Panel::setBarGraphColor( widgetName, color ); };
      void painter ( const PG_Rect &src, const ASCString& name, const PG_Rect &dst);
   private:
      void registerSpecialDisplay( const ASCString& name );
};



AttackPanel::AttackPanel ( tfight& engine_ ) : Panel( PG_Application::GetWidgetById(ASC_PG_App::mainScreenID), PG_Rect(0,0,170,200), "Attack" ), engine( engine_ )
{
   registerSpecialDisplay( "attacker_unit_pic" );
   registerSpecialDisplay( "defender_unit_pic" );
   registerSpecialDisplay( "attacker_unitexp" );
   registerSpecialDisplay( "defender_unitexp" );
   registerSpecialDisplay( "attacker_level" );
   registerSpecialDisplay( "defender_level" );
   registerSpecialDisplay( "attacker_weaponsymbol" );
   registerSpecialDisplay( "defender_weaponsymbol" );
   attacker_exp = engine.av.experience;
   defender_exp = engine.dv.experience;
}


void AttackPanel::registerSpecialDisplay( const ASCString& name )
{
   SpecialDisplayWidget* sdw = dynamic_cast<SpecialDisplayWidget*>( FindChild( name, true ) );
   if ( sdw )
     sdw->display.connect( SigC::slot( *this, &AttackPanel::painter ));
}


void AttackPanel::painter ( const PG_Rect &src, const ASCString& name, const PG_Rect &dst)
{
   Surface s = Surface::Wrap( PG_Application::GetScreen() );

   if ( name  == "attacker_unit_pic" ) {
      engine.paintAttacker( s, dst );
      return;
   }
   if ( name  == "defender_unit_pic" ) {
      engine.paintTarget( s, dst );
      return;
   }
   if ( name  == "attacker_unitexp" ) {
      s.Blit( IconRepository::getIcon("experience" + ASCString::toString(attacker_exp) + ".png"), SPoint(dst.x, dst.y) );
      return;
   }
   if ( name  == "defender_unitexp" ) {
      s.Blit( IconRepository::getIcon("experience" + ASCString::toString(defender_exp) + ".png"), SPoint(dst.x, dst.y) );
      return;
   }
   if ( name  == "attacker_level" && engine.av.height ) {
      s.Blit( IconRepository::getIcon("height-a" + ASCString::toString(log2(engine.av.height)) + ".png"), SPoint(dst.x, dst.y) );
      return;
   }
   if ( name  == "defender_level" && engine.dv.height ) {
      s.Blit( IconRepository::getIcon("height-a" + ASCString::toString(log2(engine.dv.height)) + ".png"), SPoint(dst.x, dst.y) );
      return;
   }

   if ( name  == "attacker_weaponsymbol" && engine.av.weapontype >= 0  ) {
      s.Blit( IconRepository::getIcon(SingleWeapon::getIconFileName( engine.av.weapontype ) + "-small.png"), SPoint(dst.x, dst.y) );
      return;
   }
   if ( name  == "defender_weaponsymbol" && engine.dv.weapontype >= 0  ) {
      s.Blit( IconRepository::getIcon(SingleWeapon::getIconFileName( engine.dv.weapontype ) + "-small.png"), SPoint(dst.x, dst.y) );
      return;
   }

}


void AttackPanel::dispValue ( const ASCString& name, float value, float maxvalue, PG_Color color )
{
   if ( value > 0 ) {
      setBarGraphColor( name + "bar", color );
      if ( value > maxvalue )
         setBarGraphValue( name + "bar", 1 );
      else
         setBarGraphValue( name + "bar", value / maxvalue );
   } else {
      setBarGraphColor( name + "bar", PG_Colormap::yellow );
      if ( value < -maxvalue )
         setBarGraphValue( name + "bar", 1 );
      else
         setBarGraphValue( name + "bar", -value / maxvalue );
   }
   ASCString s;
   s.format ( "%d", int(value * 100) );
   setLabelText( name, s );
}


void tfight :: calcdisplay ( int ad, int dd )
{


   auto_ptr<AttackPanel> at ( new AttackPanel(*this));

   float avd = float( 100 - av.damage )/100;
   float dvd = float( 100 - dv.damage )/100;

   PG_Color attackingColor = lightenColor( actmap->player[getAttackingPlayer()].getColor(), 1.4 );
   PG_Color defendingColor = lightenColor( actmap->player[getDefendingPlayer()].getColor(), 1.4 );

   at->setBarGraphValue( "attacker_unitstatusbar", avd );
   at->setBarGraphColor( "attacker_unitstatusbar", attackingColor );

   at->setBarGraphValue( "defender_unitstatusbar", dvd );
   at->setBarGraphColor( "defender_unitstatusbar", defendingColor );

   at->setLabelText( "attacker_unitstatus", 100 - av.damage );
   at->setLabelText( "defender_unitstatus", 100 - dv.damage );



   at->setBarGraphValue( "attacker_hemmingbar", (dv.hemming -1) / 1.4 );
   at->setBarGraphColor( "attacker_hemmingbar", attackingColor );
   at->setBarGraphValue( "defender_hemmingbar", (av.hemming -1) / 1.4 );
   at->setBarGraphColor( "defender_hemmingbar", defendingColor );

   at->setLabelText( "defender_hemming", "-" );
   at->setLabelText( "attacker_hemming", int((dv.hemming-1) * 100 ));

   at->dispValue( "attacker_attackbonus", strength_attackbonus(av.attackbonus), maxattackshown, attackingColor );
   at->dispValue( "defender_attackbonus", strength_attackbonus(dv.attackbonus), maxattackshown, defendingColor );


   at->dispValue( "attacker_defencebonus", defense_defensebonus(av.defensebonus), maxattackshown, attackingColor );
   at->dispValue( "defender_defencebonus", defense_defensebonus(dv.defensebonus), maxattackshown, defendingColor );


   int t = ticker;
   calc();
   at->Show();


   int time1 = CGameOptions::Instance()->attackspeed1;
   if ( time1 <= 0 )
      time1 = 30;

   int time2 = CGameOptions::Instance()->attackspeed2;
   if ( time2 <= 0 )
      time2 = 50;

   int time3 = CGameOptions::Instance()->attackspeed3;
   if ( time3 <= 0 )
      time3 = 30;

   do {
      releasetimeslice();
   } while ( t + time1 > ticker ); /* enddo */



   if ( ad != -1 )
      av.damage = ad;

   if ( dd != -1 )
      dv.damage = dd;


   float avd2 = float( 100 - av.damage )/100;
   float dvd2 = float( 100 - dv.damage )/100;

   int starttime = ticker;
   while ( ticker < starttime + time2 ) {
      float p = float(ticker - starttime ) / time2;

      at->setBarGraphValue( "attacker_unitstatusbar", avd + (avd2-avd) * p );
      at->setBarGraphValue( "defender_unitstatusbar", dvd + (dvd2-dvd) * p );

      at->setLabelText( "attacker_unitstatus", int( 100.0 * (avd + (avd2-avd) * p )) );
      at->setLabelText( "defender_unitstatus", int( 100.0 * (dvd + (dvd2-dvd) * p )) );
      at->Update();
   }

   t = ticker;
   do {
      releasetimeslice();
   } while ( t + time3 > ticker ); /* enddo */


}


void UnitAttacksSomething::paintAttacker( Surface& surface, const SDL_Rect &dst )
{
   _attackingunit->typ->paint( surface, SPoint( dst.x, dst.y ), _attackingunit->getOwner() );
}



tunitattacksunit :: tunitattacksunit ( Vehicle* &attackingunit, Vehicle* &attackedunit, bool respond, int weapon )
{
   setup ( attackingunit, attackedunit, respond, weapon );
}

void tunitattacksunit :: setup ( Vehicle* &attackingunit, Vehicle* &attackedunit, bool respond, int weapon )
{
   _attackingunit = attackingunit;
   _attackedunit  = attackedunit;

   _pattackingunit = &attackingunit;
   _pattackedunit  = &attackedunit;

   int dist = beeline ( attackingunit->xpos, attackingunit->ypos, attackedunit->xpos, attackedunit->ypos );
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

   av.strength = int( attackingunit->weapstrength[_weapon]
                      * weapDist.getWeapStrength(weap, dist, attackingunit->height, attackedunit->height )
                      * attackingunit->typ->weapons.weapon[_weapon].targetingAccuracy[attackedunit->typ->movemalustyp] / 100 );
   av.armor  = attackingunit->armor;
   av.damage     = attackingunit->damage;
   av.experience  = attackingunit->experience;
   av.hemming    = 1;
   av.weapnum    = _weapon;
   av.weapcount  = attackingunit->ammo [ _weapon ];
   av.color      = attackingunit->getOwner();
   av.initiative = attackingunit->typ->initiative;
   av.kamikaze   = attackingunit->typ->functions & cfkamikaze;
   av.height = attackingunit->height;
   av.weapontype = attackingunit->typ->weapons.weapon[ _weapon ].getScalarWeaponType();

   pfield field = getfield ( attackingunit->xpos, attackingunit->ypos );

   if ( attackingunit->height <= chfahrend ) {
      if ( dist <= maxmalq )
         av.attackbonus  = field->getattackbonus();
      else
         av.attackbonus = 0;
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

   if ( attackedunit->typ->functions & cfkamikaze )
      respond = 0;

   if ( respond ) {
      weap = attackedunit->getWeapon( dv.weapnum );

      dv.strength  = int( attackedunit->weapstrength[ dv.weapnum ]
                          * weapDist.getWeapStrength(weap, dist, attackedunit->height, attackingunit->height )
                          * attackedunit->typ->weapons.weapon[ dv.weapnum ].targetingAccuracy[attackingunit->typ->movemalustyp] / 100 );
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


   dv.armor = attackedunit->armor;
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

//   if ( _attackingunit->reactionfire.getStatus() >= Vehicle::ReactionFire::ready )
//      _attackingunit->reactionfire.enemiesAttackable &= 0xff ^ ( 1 <<  dv.color );

   _attackingunit->postAttack();

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


void tunitattacksunit::paintTarget( Surface& surface, const SDL_Rect &dst )
{
   _attackedunit->paint( surface, SPoint( dst.x, dst.y ));
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

   int dist = beeline ( attackingunit->xpos, attackingunit->ypos, x, y );
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
   av.strength  = int( attackingunit->weapstrength[_weapon]
                       * weapDist.getWeapStrength(weap, dist, attackingunit->height, _attackedbuilding->typ->buildingheight )
                       * attackingunit->typ->weapons.weapon[_weapon].targetingAccuracy[cmm_building] / 100 );

   av.armor = attackingunit->armor;
   av.damage    = attackingunit->damage;
   av.experience = attackingunit->experience;
   av.hemming    = 1;
   av.weapnum    = _weapon;
   av.weapcount  = attackingunit->ammo [ _weapon ];
   av.weapontype = attackingunit->typ->weapons.weapon[ _weapon ].getScalarWeaponType();
   av.color      = attackingunit->getOwner();
   av.initiative = attackingunit->typ->initiative;
   av.kamikaze   = attackingunit->typ->functions & cfkamikaze;
   av.height = attackingunit->height;

   pfield field = getfield ( attackingunit->xpos, attackingunit->ypos );

   if ( attackingunit->height <= chfahrend ) {
      av.defensebonus = field->getdefensebonus();
      if ( dist <= maxmalq )
         av.attackbonus  = field->getattackbonus();
      else
         av.attackbonus = 0;
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


void tunitattacksbuilding::calcdisplay( int ad, int dd ) {
  #ifdef sgmain
   SoundList::getInstance().playSound( SoundList::shooting  , _attackingunit->getWeapon(av.weapnum)->getScalarWeaponType(), false, _attackingunit->getWeapon(av.weapnum)->soundLabel );
  #endif
  tfight::calcdisplay(ad,dd);
  #ifdef sgmain
   if ( dv.damage >= 100 )
      SoundList::getInstance().playSound( SoundList::buildingCollapses );
  #endif
}


void tunitattacksbuilding :: setresult ( void )
{
   // _attackingunit->experience = av.experience;
   _attackingunit->ammo[ av.weapnum ] = av.weapcount;

   _attackingunit->postAttack();

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

void tunitattacksbuilding :: paintTarget( Surface& surface, const SDL_Rect &dst )
{
   _attackedbuilding->paintSingleField( surface, SPoint(dst.x, dst.y), _attackedbuilding->getLocalCoordinate(MapCoordinate(_x,_y)));
}




tmineattacksunit :: tmineattacksunit ( pfield mineposition, int minenum, Vehicle* &attackedunit )
{
   setup ( mineposition, minenum, attackedunit );
}

void tmineattacksunit :: setup ( pfield mineposition, int minenum, Vehicle* &attackedunit )
{
   if ( mineposition->mines.empty() )
      displaymessage(" tmineattacksunit :: setup \n no mine to attack !\n",2 );

   if ( attackedunit->height >= chtieffliegend )
      displaymessage(" tmineattacksunit :: setup \n mine attacks flying unit!\n",2 );


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
   dv.armor = attackedunit->armor;
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

void tmineattacksunit::calcdisplay( int ad, int dd ) {
  #ifdef sgmain
  SoundList::getInstance().playSound( SoundList::shooting , 1 );
  #endif
  tfight::calcdisplay(ad,dd);
  #ifdef sgmain
   if ( dv.damage >= 100 )
      SoundList::getInstance().playSound( SoundList::unitExplodes , 0, false, _attackedunit->typ->killSoundLabel );
  #endif
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


void tmineattacksunit :: paintAttacker( Surface& surface, const SDL_Rect &dst )
{
  #if 0
   if ( _minenum == -1 ) {
      tfield::MineContainer::iterator m = _mineposition->mines.begin();
      while ( ! m->attacksunit  ( _attackedunit ))
         m++;

      putspriteimage    ( xa, ya, getmineadress ( m->type ));
   } else
      putspriteimage    ( xa, ya, getmineadress ( _mineposition->getMine(_minenum).type ));

  #endif
}

void tmineattacksunit :: paintTarget( Surface& surface, const SDL_Rect &dst )
{
   _attackedunit->typ->paint( surface, SPoint( dst.x, dst.y ), _attackedunit->getOwner() );
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

   int dist = beeline ( attackingunit->xpos, attackingunit->ypos, obj_x, obj_y );

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
   av.strength  = int( attackingunit->weapstrength[weapon]
                       * weapDist.getWeapStrength(weap, dist, attackingunit->height, _obji->typ->getEffectiveHeight() )
                       * attackingunit->typ->weapons.weapon[_weapon].targetingAccuracy[cmm_building] / 100 );

   av.armor = attackingunit->armor;
   av.damage    = attackingunit->damage;
   av.experience = attackingunit->experience;
   av.weapnum    = _weapon;
   av.weapcount   = attackingunit->ammo [ _weapon ];
   av.kamikaze   = attackingunit->typ->functions & cfkamikaze;
   av.height = attackingunit->height;
   av.weapontype = attackingunit->typ->weapons.weapon[ _weapon ].getScalarWeaponType();

   pfield field2 = getfield ( attackingunit->xpos, attackingunit->ypos );

   if ( attackingunit->height <= chfahrend ) {
      av.defensebonus = field2->getdefensebonus();
      if ( dist <= maxmalq )
         av.attackbonus  = field2->getattackbonus();
      else
         av.attackbonus = 0;
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

void tunitattacksobject::calcdisplay( int ad, int dd ) {
  #ifdef sgmain
  SoundList::getInstance().playSound ( SoundList::shooting, _attackingunit->getWeapon(av.weapnum)->getScalarWeaponType(), false, _attackingunit->getWeapon(av.weapnum)->soundLabel );
  #endif
  tfight::calcdisplay(ad,dd);
}


void tunitattacksobject :: setresult ( void )
{
   // _attackingunit->experience = av.experience;
   _attackingunit->ammo[ av.weapnum ] = av.weapcount;

   _attackingunit->postAttack();

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



void tunitattacksobject :: paintTarget( Surface& surface, const SDL_Rect &dst )
{
   _obji->typ->display ( surface, SPoint( dst.x, dst.y ) );
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

   pfield efield = getfield(x,y);

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
                          angreifer->typ->weapons.weapon[i].getScalarWeaponType() == cwairmissilen ||
                          angreifer->typ->weapons.weapon[i].getScalarWeaponType() == cwgroundmissilen ||
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


bool attackpossible2u( const Vehicle* attacker, const Vehicle* target, pattackweap atw, int uheight )
{
   if ( uheight == -1 )
      uheight = target->height;

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
            if ( uheight & (1<<h))
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

   if ( actmap->player[angreifer->getOwner()].diplomacy.isHostile( verteidiger->getOwner() )  )
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
                           const pfield        field)
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


float WeapDist::getWeapStrength ( const SingleWeapon* weap, int dist, int attacker_height, int defender_height, int reldiff  )
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
         displaymessage("tweapdist::getweapstrength: invalid range: \n min = %d ; max = %d ; req = %d ",1, weap->mindistance, weap->maxdistance, dist);
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

   int heightEff = 100;
   if ( attacker_height != -1 && defender_height != -1 ) {
      int hd = getheightdelta ( log2 ( attacker_height ), log2 ( defender_height ));
      heightEff = weap->efficiency[6+hd];
   }

   return relstrength * heightEff / float(weap->maxstrength * 100) ;

/*   if ( attacker_height != -1 && defender_height!= -1 ) {
      int hd = getheightdelta ( log2 ( attacker_height ), log2 ( defender_height ));
      return relstrength * weap->efficiency[6+hd] / 100 ;
   } else
      return relstrength ;
      */
}

WeapDist weapDist;

