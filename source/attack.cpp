/*! \file attack.cpp
    \brief All the routines for fighting in ASC
*/


//     $Id: attack.cpp,v 1.53 2001-10-22 18:22:47 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.52  2001/10/16 19:58:19  mbickel
//      Added title screen for mapeditor
//      Updated source documentation
//
//     Revision 1.51  2001/10/03 20:56:06  mbickel
//      Updated data files
//      Updated online help
//      Clean up of Pulldown menues
//      Weapons can now have different efficiencies against different unit classes
//
//     Revision 1.50  2001/10/02 14:06:27  mbickel
//      Some cleanup and documentation
//      Bi3 import tables now stored in .asctxt files
//      Added ability to choose amoung different BI3 import tables
//      Added map transformation tables
//
//     Revision 1.49  2001/09/25 15:13:07  mbickel
//      New version number
//      Fixed crash when reaction fire during ascend
//
//     Revision 1.48  2001/09/13 17:43:11  mbickel
//      Many, many bug fixes
//
//     Revision 1.47  2001/08/07 15:58:09  mbickel
//      Fixed crash in mail list
//      Fixed crash in weapon info with mines
//      Fixed cancel Button in object construction
//
//     Revision 1.46  2001/07/25 19:56:56  mbickel
//      Fixed: kamikaze not working
//
//     Revision 1.45  2001/07/15 21:00:25  mbickel
//      Some cleanup in the vehicletype class
//
//     Revision 1.44  2001/07/14 13:15:17  mbickel
//      Rewrote sound handling
//
//     Revision 1.43  2001/02/26 12:34:59  mbickel
//      Some major restructuing:
//       new message containers
//       events don't store pointers to units any more
//       tfield class overhauled
//
//     Revision 1.42  2001/02/18 17:52:35  mbickel
//      Fixed some compilation problems on Linux
//
//     Revision 1.41  2001/02/18 15:36:59  mbickel
//      Some cleanup and documentation
//      Restructured: vehicle and building classes into separate files
//         tmap, tfield and helper classes into separate file (gamemap.h)
//      basestrm : stream mode now specified by enum instead of int
//
//     Revision 1.40  2001/02/08 21:21:02  mbickel
//      AI attacks and services more sensibly
//
//     Revision 1.39  2001/02/04 21:26:53  mbickel
//      The AI status is written to savegames -> new savegame revision
//      Lots of bug fixes
//
//     Revision 1.38  2001/02/01 22:48:27  mbickel
//      rewrote the storing of units and buildings
//      Fixed bugs in bi3 map importing routines
//      Fixed bugs in AI
//      Fixed bugs in mapeditor
//
//     Revision 1.37  2001/01/28 14:04:01  mbickel
//      Some restructuring, documentation and cleanup
//      The resource network functions are now it their own files, the dashboard
//       as well
//      Updated the TODO list
//
//     Revision 1.36  2001/01/21 12:48:35  mbickel
//      Some cleanup and documentation
//
//     Revision 1.35  2001/01/19 13:33:46  mbickel
//      The AI now uses hemming
//      Several bugfixes in Vehicle Actions
//      Moved all view calculation to viewcalculation.cpp
//      Mapeditor: improved keyboard support for item selection
//
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

#include "mapalgorithms.h"

#ifdef sgmain
 #include "soundList.h"
#endif





bool  AttackFormula :: checkHemming ( pvehicle     d_eht,  int     direc )
{ 
   pvehicle     s_eht;

   int x = d_eht->xpos;
   int y = d_eht->ypos; 
   getnextfield(x, y, direc);
   pfield fld = getfield(x,y);

   if ( fld ) 
      s_eht = fld->vehicle;
   else 
      s_eht = NULL; 

   return attackpossible2n ( s_eht, d_eht );
} 


float AttackFormula :: getHemmingFactor ( int relDir )
{
   const float  maxHemmingFactor = 1.4;  // = +140% !
   #ifdef HEXAGON
   const float hemming[sidenum-1]  = { 4, 11, 16, 11, 4 };
   const float maxHemmingSum = 46;
   #else
   const float hemming[sidenum-1]  = {3, 7, 11, 16, 11, 7, 3};
   const float maxHemmingSum = 58;
   #endif
   relDir  %= 6;
   if ( relDir < 0 )
      relDir += sidenum;
   return hemming[relDir]*maxHemmingFactor/maxHemmingSum;
}


float AttackFormula :: strength_hemming ( int  ax,  int ay,  pvehicle d_eht )
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
   float a = (300 - 2 * damage) / 3;
   return a / 100;
}

float AttackFormula :: strength_experience ( int experience )
{
	float e =		(experience < 0)
				?	0	
				:	experience ;
   return e/maxunitexperience * 2.875;
}

float AttackFormula :: defense_experience ( int experience )
{
   float e =		(experience < 0)
				?	0	
				:	experience ;
   
   return e/maxunitexperience * 1.15;
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
   synchronedisplay = 0;
}


void tfight :: calc ( void )
{
   const float damagefactor = 4;
   const float armordivisor = 5;


   if ( av.strength ) { 
      float absstrength = float(av.strength )
                          * ( 1 + strength_experience ( av.experience ) + strength_attackbonus ( av.attackbonus ) )
                          * strength_damage ( av.damage ) 
                          * dv.hemming;

      float absdefense = float(dv.armor / armordivisor )
                          * ( 1 + defense_defensebonus ( dv.defensebonus ) + defense_experience ( dv.experience ) );

      int w = int(dv.damage + absstrength / absdefense * 100 / damagefactor );

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

      int w = int(av.damage + absstrength / absdefense * 100 / damagefactor );

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


int positions[][4] = {{ 459,    312,    468,      412  },
                      { 471,    312,    480,      412  },
                      { 483,    312,    492,      412  },
                      { 494,    312,    504,      412  },
                      { 510,    312,    531,      412  },
                      { 542,    312,    563,      412  },
                      { 569,    312,    578,      412  },
                      { 581,    312,    590,      412  },
                      { 593,    312,    602,      412  },
                      { 605,    312,    614,      412  }};

void tfight :: paintbar ( int num, int val, int col )
{
   if ( val ) 
      bar ( agmp->resolutionx - ( 640 - positions[num][0] ) , positions[num][3] - ( val - 1), agmp->resolutionx - ( 640 - positions[num][2] ), positions[num][3], col );

}

void tfight :: paintline ( int num, int val, int col )
{
   if ( val ) 
      line ( agmp->resolutionx - ( 640 - positions[num][0] ) , positions[num][3] - ( val - 1), agmp->resolutionx - ( 640 - positions[num][2] ), positions[num][3] - ( val - 1), col );

}



#define maxdefenseshown 24
#define maxattackshown 24

void tunitattacksunit::calcdisplay( int ad, int dd ) {
  #ifdef sgmain
  SoundList::getInstance().play ( SoundList::shooting, _attackingunit->getWeapon(av.weapnum)->getScalarWeaponType() );
  #endif
  tfight::calcdisplay(ad,dd);
  #ifdef sgmain
   if ( av.damage >= 100 || dv.damage >= 100 )
      SoundList::getInstance().play( SoundList::unitBlowsUp , 0 );
  #endif
}
  
void tfight :: calcdisplay ( int ad, int dd )
{

   collategraphicoperations cgo ( agmp->resolutionx - ( 640 - 450), 211, agmp->resolutionx - ( 640 - 623 ), 426 );

   setinvisiblemouserectanglestk ( agmp->resolutionx - ( 640 - 450), 211, agmp->resolutionx - ( 640 - 623 ), 426 );
   if ( !icons.attack.orgbkgr ) {
      icons.attack.orgbkgr = new char [ imagesize ( agmp->resolutionx - ( 640 - 450), 211, agmp->resolutionx - ( 640 - 623 ), 426 ) ];
   }
   getimage ( agmp->resolutionx - ( 640 - 451 ), 211, agmp->resolutionx - ( 640 - 624 ), 426, icons.attack.orgbkgr );

   int bk = 156;

   putimage ( agmp->resolutionx - ( 640 - 450 ), 211, icons.attack.bkgr );
   paintimages ( agmp->resolutionx - ( 640 - 481 ), 226, agmp->resolutionx - ( 640 - 563 ), 226 );

   int ac = 20 + av.color * 8;
   int dc = 20 + dv.color * 8;

   // ursprungs-heilheit zeichnen 
   paintbar ( 4, 100 - av.damage, ac );
   int avd = av.damage;

   paintbar ( 5, 100 - dv.damage, dc );
   int dvd = dv.damage;


   // einkeilung;

   paintbar ( 6, 100 *  (dv.hemming - 1 ) / 4, dc );

   if ( av.attackbonus > 0 )
      if ( av.attackbonus > maxattackshown )
         paintbar ( 0, 100, ac - 2 );
      else
         paintbar ( 0, 100 * av.attackbonus / maxattackshown, ac );
   else
      if ( av.attackbonus < -maxattackshown )
         paintbar ( 0, 100 , yellow );
      else
         paintbar ( 0, -100 * av.attackbonus / maxattackshown, yellow );


   if ( av.defensebonus > 0 )
      if ( av.defensebonus > maxdefenseshown )
         paintbar ( 1, 100, ac - 2 );
      else
         paintbar ( 1, 100 * av.defensebonus / maxdefenseshown, ac );
   else
      if ( av.defensebonus < -maxdefenseshown )
         paintbar ( 1, 100, yellow );
      else
         paintbar ( 1, -100 * av.defensebonus / maxdefenseshown, yellow );




   if ( dv.attackbonus > 0 )
      if ( dv.attackbonus > maxattackshown )
         paintbar ( 9, 100, dc - 2 );
      else
         paintbar ( 9, 100 * dv.attackbonus / maxattackshown, dc );
   else
      if ( dv.attackbonus < -maxattackshown )
         paintbar ( 9, 100 , yellow );
      else
         paintbar ( 9, -100 * dv.attackbonus / maxattackshown, yellow );


   if ( dv.defensebonus > 0 )
      if ( dv.defensebonus > maxdefenseshown )
         paintbar ( 8, 100, dc - 2 );
      else
         paintbar ( 8, 100 * dv.defensebonus / maxdefenseshown, dc );
   else
      if ( dv.defensebonus < -maxdefenseshown )
         paintbar ( 8, 100, yellow );
      else
         paintbar ( 8, -100 * dv.defensebonus / maxdefenseshown, yellow );

   cgo.off();

   int t = ticker;
   calc();


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



   int steps;
   if ( av.damage - avd > dv.damage - dvd )
      steps = av.damage - avd ;
   else
      steps = dv.damage - dvd;

   int d1 = avd;
   int d2 = dvd;

   int starttime = ticker;
   while ( ticker < starttime + time2 ) {
      int finished = 1000 * (ticker - starttime) / time2;
      int newpos1 = avd + steps * finished / 1000;
      int newpos2 = dvd + steps * finished / 1000;
      cgo.on();

      int i;
      if ( avd != av.damage )
         for ( i = d1; i <= newpos1 && i <= av.damage; i++ )
            paintline ( 4, 100 - i, bk );

      d1 = i;

      int j;
      if ( dvd != dv.damage )
         for ( j = d2; j <= newpos2 && j <= dv.damage; j++ )
            paintline ( 5, 100 - j, bk );

      d2 = j;

      cgo.off();
   }
   for ( int i = 0; i < av.damage; i++ )
      paintline ( 4, 100 - i, bk );

   for ( int i = 0; i < dv.damage; i++ )
      paintline ( 5, 100 - i, bk );

   t = ticker;
   do {
      releasetimeslice();
   } while ( t + time3 > ticker ); /* enddo */
   putimage ( agmp->resolutionx - ( 640 - 451 ), 211, icons.attack.orgbkgr );
   getinvisiblemouserectanglestk ( );
}




tunitattacksunit :: tunitattacksunit ( pvehicle &attackingunit, pvehicle &attackedunit, bool respond, int weapon )
{
   setup ( attackingunit, attackedunit, respond, weapon );
}

void tunitattacksunit :: setup ( pvehicle &attackingunit, pvehicle &attackedunit, bool respond, int weapon )
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
   av.color      = attackingunit->color >> 3;
   av.initiative = attackingunit->typ->initiative;
   av.kamikaze   = attackingunit->functions & cfkamikaze;

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


   if ( dist <= maxmalq  &&  respond  && !av.kamikaze ) {
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

   if ( respond ) {
      weap = attackedunit->getWeapon( dv.weapnum );

      dv.strength  = int( attackedunit->weapstrength[ dv.weapnum ]
                          * weapDist.getWeapStrength(weap, dist, attackedunit->height, attackingunit->height )
                          * attackedunit->typ->weapons.weapon[ dv.weapnum ].targetingAccuracy[attackingunit->typ->movemalustyp] / 100 );
      field = getfield ( attackedunit->xpos, attackedunit->ypos );
      dv.attackbonus  = field->getattackbonus();
      _respond = 1;

      dv.weapcount   = attackedunit->ammo [ dv.weapnum ];

   } else {
      dv.strength = 0;
      dv.attackbonus = 0;
      _respond = 0;
   }


   dv.armor = attackedunit->armor;
   dv.damage    = attackedunit->damage;
   dv.experience = attackedunit->experience;
   if ( dist <= maxmalq )
      dv.hemming = strength_hemming ( attackingunit->xpos, attackingunit->ypos, attackedunit );
   else
      dv.hemming = 1;


   if ( attackedunit->height <= chfahrend )
      dv.defensebonus = getfield ( attackedunit->xpos, attackedunit->ypos ) -> getdefensebonus();
   else
      dv.defensebonus = 0;

   dv.color      = attackedunit->color >> 3;
   dv.initiative = attackedunit->typ->initiative;
   dv.kamikaze = 0;
}



void tunitattacksunit :: setresult ( void )
{
   _attackingunit->experience = av.experience;
   _attackingunit->ammo[ av.weapnum ] = av.weapcount;

   if ( _attackingunit->reactionfire.getStatus() >= Vehicle::ReactionFire::ready )
      _attackingunit->reactionfire.enemiesAttackable &= 0xff ^ ( 1 <<  dv.color );

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

void tunitattacksunit :: paintimages ( int xa, int ya, int xd, int yd )
{
   putrotspriteimage ( xa, ya, _attackingunit->typ->picture[0], _attackingunit->color );
   putrotspriteimage ( xd, yd, _attackedunit ->typ->picture[0], _attackedunit->color  );
};








tunitattacksbuilding :: tunitattacksbuilding ( pvehicle attackingunit, int x, int y, int weapon )
{
   setup ( attackingunit, x, y, weapon );
}


void tunitattacksbuilding :: setup ( pvehicle attackingunit, int x, int y, int weapon )
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
   av.color      = attackingunit->color >> 3;
   av.initiative = attackingunit->typ->initiative;
   av.kamikaze   = attackingunit->functions & cfkamikaze;

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

   dv.defensebonus = 0; // getfield ( x, y ) -> getdefensebonus();
   dv.color      = _attackedbuilding->color >> 3;
   dv.initiative = 0;
   dv.kamikaze = 0;


}

void tunitattacksbuilding::calcdisplay( int ad, int dd ) {
  #ifdef sgmain
        SoundList::getInstance().play( SoundList::shooting  , _attackingunit->getWeapon(av.weapnum)->getScalarWeaponType() );
  #endif
  tfight::calcdisplay(ad,dd);
  #ifdef sgmain
   if ( av.damage >= 100 || dv.damage >= 100 )
      SoundList::getInstance().play( SoundList::unitBlowsUp );
  #endif
}


void tunitattacksbuilding :: setresult ( void )
{
   // _attackingunit->experience = av.experience;
   _attackingunit->ammo[ av.weapnum ] = av.weapcount;

   _attackingunit->postAttack();

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

void tunitattacksbuilding :: paintimages ( int xa, int ya, int xd, int yd )
{
   putrotspriteimage ( xa, ya, _attackingunit->typ->picture[0], _attackingunit->color );
   putrotspriteimage ( xd, yd, getfield ( _x, _y ) -> picture , _attackedbuilding->color  );
};




tmineattacksunit :: tmineattacksunit ( pfield mineposition, int minenum, pvehicle &attackedunit )
{
   setup ( mineposition, minenum, attackedunit );
}

void tmineattacksunit :: setup ( pfield mineposition, int minenum, pvehicle &attackedunit )
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
            if ( m->type == cmantipersonnelmine   &&  (attackedunit->functions & cf_trooper ) )
               strength *= 2;

            for ( int j = 1; j < cnt; j++ )
               strength = strength * 2 / 3;

            av.strength += strength;
            cnt++;
         }
   } else {
      Mine& m = mineposition->getMine ( minenum );
      av.strength = m.strength;
      if ( m.type  == cmantipersonnelmine   &&  (attackedunit->functions & cf_trooper ) )
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

   dv.strength = 0;
   dv.armor = attackedunit->armor;
   dv.damage    = attackedunit->damage;
   dv.experience = attackedunit->experience;
   dv.defensebonus = 0;
   dv.hemming    = 1;
   dv.weapnum = 0;
   dv.weapcount = 0;
   dv.color = attackedunit->color >> 3;
   dv.initiative = attackedunit->typ->initiative;
   dv.attackbonus = 0;
   dv.kamikaze = 0;
}

void tmineattacksunit::calcdisplay( int ad, int dd ) {
  #ifdef sgmain
  SoundList::getInstance().play( SoundList::shooting  , 1 );
  #endif
  tfight::calcdisplay(ad,dd);
  #ifdef sgmain
   if ( av.damage >= 100 || dv.damage >= 100 )
      SoundList::getInstance().play( SoundList::unitBlowsUp );
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

void tmineattacksunit :: paintimages ( int xa, int ya, int xd, int yd )
{
   if ( _minenum == -1 ) {
      tfield::MineContainer::iterator m = _mineposition->mines.begin();
      while ( ! m->attacksunit  ( _attackedunit ))
         m++;

      putspriteimage    ( xa, ya, getmineadress ( m->type ));
   } else
      putspriteimage    ( xa, ya, getmineadress ( _mineposition->getMine(_minenum).type ));

   putrotspriteimage ( xd, yd, _attackedunit->typ->picture[0], _attackedunit->color  );
}




tunitattacksobject :: tunitattacksobject ( pvehicle attackingunit, int obj_x, int obj_y, int weapon )
{
   setup ( attackingunit, obj_x, obj_y, weapon );
}

void tunitattacksobject :: setup ( pvehicle attackingunit, int obj_x, int obj_y, int weapon )
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
                       * weapDist.getWeapStrength(weap, dist, -1, -1  )
                       * attackingunit->typ->weapons.weapon[_weapon].targetingAccuracy[cmm_building] / 100 );

   av.armor = attackingunit->armor;
   av.damage    = attackingunit->damage;
   av.experience = attackingunit->experience;
   av.weapnum    = _weapon;
   av.weapcount   = attackingunit->ammo [ _weapon ];
   av.kamikaze   = attackingunit->functions & cfkamikaze;

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

}

void tunitattacksobject::calcdisplay( int ad, int dd ) {
  #ifdef sgmain
  SoundList::getInstance().play( SoundList::shooting  , _attackingunit->getWeapon(av.weapnum)->getScalarWeaponType() );
  #endif
  tfight::calcdisplay(ad,dd);
}


void tunitattacksobject :: setresult ( void )
{
   // _attackingunit->experience = av.experience;
   _attackingunit->ammo[ av.weapnum ] = av.weapcount;

   _attackingunit->postAttack();

   _obji->damage    = dv.damage;

   /* Remove the object if it was destroyed */
   if ( _obji->damage >= 100 ) {
     getfield ( _x, _y )-> removeobject ( _obji->typ );
   }

   actmap->time.set ( actmap->time.turn(), actmap->time.move()+1);

}


void tunitattacksobject :: paintimages ( int xa, int ya, int xd, int yd )
{
   putrotspriteimage ( xa, ya, _attackingunit->typ->picture[0], _attackingunit->color );
   _obji->typ->display ( xd - 5, yd - 5 );
}









pattackweap  attackpossible( const pvehicle     angreifer, int x, int y)
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
         if (getdiplomaticstatus2(efield->building->color, angreifer->color) == cawar)
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
            for ( int i = 0; i <= angreifer->typ->weapons.count - 1; i++)
               if (angreifer->typ->weapons.weapon[i].shootable() )
                  if ( angreifer->typ->weapons.weapon[i].getScalarWeaponType() == cwcannonn ||
                       angreifer->typ->weapons.weapon[i].getScalarWeaponType() == cwbombn ) {
                     if ( angreifer->typ->weapons.weapon[i].targetingAccuracy[cmm_building] )
                        if (chfahrend & angreifer->typ->weapons.weapon[i].targ ) {
                           if (fieldvisiblenow(efield, angreifer->color/8)) {
                              int d = beeline(angreifer->xpos,angreifer->ypos,x,y);
                              if (d <= angreifer->typ->weapons.weapon[i].maxdistance)
                                 if (d >= angreifer->typ->weapons.weapon[i].mindistance) {
                                    if (angreifer->height & angreifer->typ->weapons.weapon[i].sourceheight )
                                       if (angreifer->ammo[i] > 0) {
                                          atw->strength[atw->count ] = angreifer->weapstrength[i];
                                          atw->num[atw->count ] = i;
                                          atw->typ[atw->count ] = 1 << angreifer->typ->weapons.weapon[i].getScalarWeaponType();
                                          atw->target = AttackWeap::object;
                                          atw->count++;
                                       }

                                 }
                           }
                        }
                  }
         }
   }

   return atw;
}


bool attackpossible2u( const pvehicle attacker, const pvehicle target, pattackweap atw )
{
   pvehicle angreifer = attacker;
   pvehicle verteidiger = target;
   int result = false;
   if ( atw )
      atw->count = 0;

   if ( !angreifer )
     return false ;

   if ( !verteidiger )
     return false ;

   if (angreifer->typ->weapons.count == 0)
     return false ;

   if ( getdiplomaticstatus2 ( angreifer->color, verteidiger->color ) == cawar )
      for ( int i = 0; i < angreifer->typ->weapons.count ; i++)
         if (angreifer->typ->weapons.weapon[i].shootable() )
            if (angreifer->typ->weapons.weapon[i].offensive() )
               if (verteidiger->height & angreifer->typ->weapons.weapon[i].targ )
                  if (angreifer->height & angreifer->typ->weapons.weapon[i].sourceheight )
                     if ( angreifer->typ->weapons.weapon[i].targetingAccuracy[ verteidiger->typ->movemalustyp] > 0 )
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



bool attackpossible28( const pvehicle attacker, const pvehicle target, pattackweap atw )
{
   pvehicle angreifer = attacker;
   pvehicle verteidiger = target;

   int result = false;
   if ( atw )
      atw->count = 0;

   if (angreifer == NULL)
     return false ;

   if (verteidiger == NULL)
     return false ;

   if (angreifer->typ->weapons.count == 0)
     return false ;

   if ( getdiplomaticstatus2 ( angreifer->color, verteidiger->color ) == cawar )
      for ( int i = 0; i < angreifer->typ->weapons.count ; i++)
         if (angreifer->typ->weapons.weapon[i].shootable() )
            if (angreifer->typ->weapons.weapon[i].offensive() )
               if (verteidiger->height & angreifer->typ->weapons.weapon[i].targ )
                  if (minmalq <= angreifer->typ->weapons.weapon[i].maxdistance)
                     if (minmalq >= angreifer->typ->weapons.weapon[i].mindistance)
                        if (angreifer->height & angreifer->typ->weapons.weapon[i].sourceheight )
                           if ( angreifer->typ->weapons.weapon[i].targetingAccuracy[ verteidiger->typ->movemalustyp ] > 0)
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


bool attackpossible2n( const pvehicle attacker, const pvehicle target, pattackweap atw )
{
   pvehicle angreifer = attacker;
   pvehicle verteidiger = target;

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
   if ( getdiplomaticstatus2 ( angreifer->color, verteidiger->color ) == cawar )
      if ( !angreifer->attacked )
         if ( !angreifer->typ->wait || !angreifer->hasMoved() )
            for ( int i = 0; i < angreifer->typ->weapons.count ; i++)
               if (angreifer->typ->weapons.weapon[i].shootable() )
                  if (angreifer->typ->weapons.weapon[i].offensive() )
                     if (verteidiger->height & angreifer->typ->weapons.weapon[i].targ )
                        if (dist <= angreifer->typ->weapons.weapon[i].maxdistance)
                           if (dist >= angreifer->typ->weapons.weapon[i].mindistance)
                              if (angreifer->height & angreifer->typ->weapons.weapon[i].sourceheight )
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

bool vehicleplattfahrbar( const pvehicle     vehicle,
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

void WeapDist::loaddata(void)
{ 
   tnfilestream stream ( "weapons.dat", tnstream::reading );
   stream.readdata ( &data, sizeof( data ));
} 


float WeapDist::getWeapStrength ( const SingleWeapon* weap, int dist, int attacker_height, int defender_height, int reldiff  )
{
  int         translat[31]  = { 6, 255, 1, 3, 2, 4, 0, 5, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
                                 255, 255, 255, 255, 255, 255}; 

   if ( !weap )
      return 0;

   int scalar = weap->getScalarWeaponType();
   if ( scalar >= 31 || scalar < 0 )
      return 0;

   if ( scalar == 1 )     // mine
      return 1;

   int typ = translat[ scalar ];
   if ( typ == 255 ) {
      displaymessage("tweapdist::getweapstrength: invalid type ", 1 );
      return 1;
   }


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

   int minstrength = 255 - 255 * weap->minstrength / weap->maxstrength;

   int relstrength = 255 - ( 255 - data[typ][reldiff] ) * minstrength / ( 255 - data[typ][255] );

   float rel = relstrength;

   if ( attacker_height != -1 && defender_height!= -1 ) {
      int hd = getheightdelta ( log2 ( attacker_height ), log2 ( defender_height ));
      return rel * weap->efficiency[6+hd] / 100 / 255;
   } else
      return rel / 255 ;
}

WeapDist weapDist;

