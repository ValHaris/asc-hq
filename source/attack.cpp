//     $Id: attack.cpp,v 1.34 2000-10-18 14:13:48 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.33  2000/10/11 14:26:15  mbickel
//      Modernized the internal structure of ASC:
//       - vehicles and buildings now derived from a common base class
//       - new resource class
//       - reorganized exceptions (errors.h)
//      Split some files:
//        typen -> typen, vehicletype, buildingtype, basecontainer
//        controls -> controls, viewcalculation
//        spfst -> spfst, mapalgorithm
//      bzlib is now statically linked and sources integrated
//
//     Revision 1.32  2000/09/16 11:47:21  mbickel
//      Some cleanup and documentation again
//
//     Revision 1.31  2000/08/13 11:55:07  mbickel
//      Attacking now decreases a units movement by 20% if it has the
//        "move after attack" flag.
//
//     Revision 1.30  2000/08/12 15:03:18  mbickel
//      Fixed bug in unit movement
//      ASC compiles and runs under Linux again...
//
//     Revision 1.29  2000/08/12 12:52:41  mbickel
//      Made DOS-Version compile and run again.
//
//     Revision 1.28  2000/08/12 09:17:14  gulliver
//     *** empty log message ***
//
//     Revision 1.27  2000/08/08 09:47:52  mbickel
//
//      speed up of dialog boxes in linux
//      fixed graphical errors in attack
//      fixed graphical error in ammo transfer
//      fixed reaction fire not allowing manual attack
//
//     Revision 1.26  2000/08/07 21:10:18  mbickel
//      Fixed some syntax errors
//
//     Revision 1.25  2000/08/07 16:29:19  mbickel
//      orbiting units don't consume fuel any more
//      Fixed bug in attack formula; improved attack formula
//      Rewrote reactionfire
//
//     Revision 1.24  2000/08/05 13:38:19  mbickel
//      Rewrote height checking for moving units in and out of
//        transports / building
//
//     Revision 1.23  2000/08/04 15:10:47  mbickel
//      Moving transports costs movement for units inside
//      refuelled vehicles now have full movement in the same turn
//      terrain: negative attack / defensebonus allowed
//      new mapparameters that affect damaging and repairing of building
//
//     Revision 1.22  2000/08/03 13:11:48  mbickel
//      Fixed: on/off switching of generator vehicle produced endless amounts of energy
//      Repairing units now reduces their experience
//      negative attack- and defenseboni possible
//      changed attackformula
//
//     Revision 1.21  2000/07/29 14:54:10  mbickel
//      plain text configuration file implemented
//
//     Revision 1.20  2000/07/16 14:19:59  mbickel
//      AI has now some primitive tactics implemented
//      Some clean up
//        moved weapon functions to attack.cpp
//      Mount doesn't modify PCX files any more.
//
//     Revision 1.19  2000/07/06 11:07:25  mbickel
//      More AI work
//      Started modularizing the attack formula
//
//     Revision 1.18  2000/07/02 21:04:10  mbickel
//      Fixed crash in Replay
//      Fixed graphic errors in replay
//
//     Revision 1.17  2000/06/08 21:03:39  mbickel
//      New vehicle action: attack
//      wrote documentation for vehicle actions
//
//     Revision 1.16  2000/06/04 21:39:17  mbickel
//      Added OK button to ViewText dialog (used in "About ASC", for example)
//      Invalid command line parameters are now reported
//      new text for attack result prediction
//      Added constructors to attack functions
//
//     Revision 1.15  2000/05/30 18:39:19  mbickel
//      Added support for multiple directories
//      Moved DOS specific files to a separate directory
//
//     Revision 1.14  2000/05/07 18:21:21  mbickel
//      Speed of attack animation can now be specified
//
//     Revision 1.13  2000/04/27 17:59:19  mbickel
//      Updated Kdevelop project file
//      Fixed some graphical errors
//
//     Revision 1.12  2000/04/27 16:25:14  mbickel
//      Attack functions cleanup
//      New vehicle categories
//      Rewrote resource production in ASC resource mode
//      Improved mine system: several mines on a single field allowed
//      Added unitctrl.* : Interface for vehicle functions
//        currently movement and height change included
//      Changed timer to SDL_GetTicks
//
//     Revision 1.11  2000/03/11 19:51:12  mbickel
//      Removed file name length limitation under linux
//      No weapon sound for attacked units any more (only attacker)
//
//     Revision 1.10  2000/03/11 18:22:04  mbickel
//      Added support for multiple graphic sets
//
//     Revision 1.9  2000/02/24 10:54:06  mbickel
//      Some cleanup and bugfixes
//
//     Revision 1.8  2000/01/25 19:28:06  mbickel
//      Fixed bugs:
//        invalid mouse buttons reported when moving the mouse
//        missing service weapon in weapon information
//        invalid text displayed in ammo production
//        undamaged units selected in repair vehicle function
//
//      speed up when playing weapon sound
//
//     Revision 1.7  2000/01/24 09:08:55  steb
//     Tidied up configure.in to support SDL properly.  Added sounds to Makefile.am
//     Tested.  It built ok for me. Still YMMV however :)
//
//     Revision 1.6  2000/01/24 08:16:49  steb
//     Changes to existing files to implement sound.  This is the first munge into
//     CVS.  It worked for me before the munge, but YMMV :)
//
//     Revision 1.5  2000/01/20 16:52:09  mbickel
//      Added Kamikaze attack
//
//     Revision 1.4  2000/01/01 19:04:13  mbickel
//     /tmp/cvsVhJ4Z3
//
//     Revision 1.3  1999/11/22 18:26:48  mbickel
//      Restructured graphics engine:
//        VESA now only for DOS
//        BASEGFX should be platform independant
//        new interface for initialization
//      Rewrote all ASM code in C++, but it is still available for the Watcom
//        versions
//      Fixed bugs in RLE decompression, BI map importer and the view calculation
//
//     Revision 1.2  1999/11/16 03:41:02  tmwilson
//     	Added CVS keywords to most of the files.
//     	Started porting the code to Linux (ifdef'ing the DOS specific stuff)
//     	Wrote replacement routines for kbhit/getch for Linux
//     	Cleaned up parts of the code that gcc barfed on (char vs unsigned char)
//     	Added autoconf/automake capabilities
//     	Added files used by 'automake --gnu'
//
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





int  AttackFormula :: checkHemming ( pvehicle     d_eht,  int     direc )
{ 
   pvehicle     s_eht; 

   if (direc < 0) 
      direc += sidenum;

   if (direc >= sidenum) 
      direc -= sidenum;

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




float AttackFormula :: strength_hemming ( int  ax,  int ay,  pvehicle d_eht )
{
   const float  maxHemmingFactor = 1.4;  // 1 + factor !!!
   #ifdef HEXAGON
   const int   hemming[5]  = { 4, 11, 16, 11, 4 };
   const int   maxHemmingSum = 46;
   #else
   const int   hemming[7]  = {3, 7, 11, 16, 11, 7, 3};
   const int   maxHemmingSum = 58;
   #endif


   float hemm = 0; 
   int direction = getdirection(ax,ay,d_eht->xpos,d_eht->ypos); 
   for ( int i = 0; i < sidenum-1; i++)
      if ( checkHemming (d_eht,i+1 + (direction-sidenum/2) ))
         hemm += hemming[i];


   return  hemm * maxHemmingFactor / maxHemmingSum + 1;

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
  sound.weaponSound(_attackingunit->getWeapon(av.weapnum)->getScalarWeaponType())->play();
  #endif
  tfight::calcdisplay(ad,dd);
  #ifdef sgmain
   if ( av.damage >= 100 || dv.damage >= 100 )
      sound.boom->play();
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


   t = ticker;
   do {
      releasetimeslice();
   } while ( t + time3 > ticker ); /* enddo */
   putimage ( agmp->resolutionx - ( 640 - 451 ), 211, icons.attack.orgbkgr );
   getinvisiblemouserectanglestk ( );
}




tunitattacksunit :: tunitattacksunit ( pvehicle &attackingunit, pvehicle &attackedunit, int respond, int weapon )
{
   setup ( attackingunit, attackedunit, respond, weapon );
}

void tunitattacksunit :: setup ( pvehicle &attackingunit, pvehicle &attackedunit, int respond, int weapon )
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

   SingleWeapon* weap = attackingunit->getWeapon(_weapon);

   av.strength = int( attackingunit->weapstrength[_weapon] * weapDist.getWeapStrength(weap, dist, attackingunit->height, attackedunit->height ));
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


   if ( dist <= maxmalq  &&  respond ) {
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

      dv.strength  = int( attackedunit->weapstrength[ dv.weapnum ] * weapDist.getWeapStrength(weap, dist, attackedunit->height, attackingunit->height ));
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

   if ( _attackingunit->reactionfire.status >= tvehicle::ReactionFire::ready )
      _attackingunit->reactionfire.enemiesAttackable &= 0xff ^ ( 1 <<  dv.color );

   _attackingunit->attacked = true; 

   _attackedunit->damage    = dv.damage;

   if ( _respond ) {
      _attackingunit->damage    = av.damage;

      _attackedunit->experience = dv.experience;
      _attackedunit->ammo[ dv.weapnum ] = dv.weapcount;
   }

   /* If the attacking vehicle was destroyed, remove it */
   if ( _attackingunit->damage >= 100 ) {
     // DEBUG("Attacker Destroyed");
     removevehicle ( _pattackingunit );
   }

   /* If the attacked vehicle was destroyed, remove it */
   if ( _attackedunit->damage >= 100 ) {
     // DEBUG("Target Destroyed");
     removevehicle ( _pattackedunit );
   }


   actmap->time.a.move++;
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

   SingleWeapon *weap = &attackingunit->typ->weapons->weapon[_weapon];
   av.strength  = int( attackingunit->weapstrength[_weapon] * weapDist.getWeapStrength(weap, dist, attackingunit->height, _attackedbuilding->typ->buildingheight ));
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
  sound.weaponSound(_attackingunit->getWeapon(av.weapnum)->getScalarWeaponType())->play();
  #endif
  tfight::calcdisplay(ad,dd);
  #ifdef sgmain
   if ( av.damage >= 100 || dv.damage >= 100 )
      sound.boom->play();
  #endif
}


void tunitattacksbuilding :: setresult ( void )
{
   _attackingunit->experience = av.experience;
   _attackingunit->ammo[ av.weapnum ] = av.weapcount;

   _attackingunit->attacked = true; 
   if ( _attackingunit->functions & cf_moveafterattack )
      _attackingunit->setMovement ( _attackingunit->getMovement() - _attackingunit->typ->movement[log2(_attackingunit->height)]*attackmovecost / 100 );
   else   
      _attackingunit->setMovement ( 0 );


   _attackedbuilding->damage    = dv.damage;

   /* Remove the attacking unit if it was destroyed */
   if ( _attackingunit->damage >= 100 ) {
      removevehicle ( &_attackingunit );
   }

   /* Remove attacked building if it was destroyed */
   if ( _attackedbuilding->damage >= 100 ) {
     removebuilding ( &_attackedbuilding );
   }

   actmap->time.a.move++;
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
   if ( !mineposition->object || !mineposition->object->mine )
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
      for ( int i = 0; i < mineposition->minenum(); i++ )
         if ( mineposition->object->mine[i]->attacksunit ( attackedunit )) {
            int strength = mineposition->object->mine[i]->strength;
            if ( mineposition->object->mine[i]->type  == cmantipersonnelmine   &&  (attackedunit->functions & cf_trooper ) )
               strength *= 2;

            for ( int j = 1; j < cnt; j++ )
               strength = strength * 2 / 3;

            av.strength += strength;
            cnt++;
         }
   } else {
     av.strength = mineposition->object->mine[minenum]->strength;
     if ( mineposition->object->mine[minenum]->type  == cmantipersonnelmine   &&  (attackedunit->functions & cf_trooper ) )
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
  sound.boom->play();
  #endif
  tfight::calcdisplay(ad,dd);
  #ifdef sgmain
   if ( av.damage >= 100 || dv.damage >= 100 )
      sound.boom->play();
  #endif
}


void tmineattacksunit :: setresult ( void )
{
   if ( _minenum == -1 ) {
      for ( int i = 0; i < _mineposition->minenum(); ) {
         if ( _mineposition->object->mine[i]->attacksunit ( _attackedunit )) {
            _mineposition->removemine ( i );
            i = 0;
         } else
            i++;
      }
   } else
      _mineposition->removemine ( _minenum );

   _attackedunit->damage = dv.damage;

   /* Remove the mined vehicle if it was destroyed */
   if ( _attackedunit->damage >= 100 ) {
     removevehicle ( _pattackedunit );
   }

}

void tmineattacksunit :: paintimages ( int xa, int ya, int xd, int yd ) 
{
   if ( _minenum == -1 ) {
      int num = _mineposition->mineattacks ( _attackedunit )-1;
      putspriteimage    ( xa, ya, getmineadress ( _mineposition->object->mine[num]->type ));
   } else
      putspriteimage    ( xa, ya, getmineadress ( _mineposition->object->mine[0]->type ));

   putrotspriteimage ( xd, yd, _attackedunit ->typ->picture[0], _attackedunit->color  );
}




tunitattacksobject :: tunitattacksobject ( pvehicle attackingunit, int obj_x, int obj_y, int weapon )
{
   setup ( attackingunit, obj_x, obj_y, weapon );
}

void tunitattacksobject :: setup ( pvehicle attackingunit, int obj_x, int obj_y, int weapon )
{
   
   _x = obj_x;
   _y = obj_y;

   pfield field = getfield ( obj_x, obj_y );
   _object = field->object;

   _attackingunit = attackingunit;

   int dist = beeline ( attackingunit->xpos, attackingunit->ypos, obj_x, obj_y );


   for ( int i = _object->objnum-1; i >= 0; i-- )
     if ( _object->object[ i ] -> typ->armor > 0 ) {
        _obji = _object->object[ i ];
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

   SingleWeapon *weap = &attackingunit->typ->weapons->weapon[weapon];
   av.strength  = int( attackingunit->weapstrength[weapon] * weapDist.getWeapStrength(weap, dist, -1, -1  ));
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
  sound.weaponSound(_attackingunit->getWeapon(av.weapnum)->getScalarWeaponType())->play();
  #endif
  tfight::calcdisplay(ad,dd);
}


void tunitattacksobject :: setresult ( void )
{
   // _attackingunit->experience = av.experience;
   _attackingunit->ammo[ av.weapnum ] = av.weapcount;

   _attackingunit->attacked = true; 

   if ( _attackingunit->functions & cf_moveafterattack )
      _attackingunit->setMovement ( _attackingunit->getMovement() - _attackingunit->typ->movement[log2(_attackingunit->height)]*attackmovecost / 100 );
   else   
      _attackingunit->setMovement ( 0 );


   _obji->damage    = dv.damage;

   /* Remove the object if it was destroyed */
   if ( _obji->damage >= 100 ) {
     getfield ( _x, _y )-> removeobject ( _obji->typ );
   }

   actmap->time.a.move++;

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
   if (angreifer->typ->weapons->count == 0) 
      return atw;

   pfield efield = getfield(x,y);
                       
   if ( efield->vehicle ) {
      if (fieldvisiblenow(efield, angreifer->color/8)) 
         attackpossible2n ( angreifer, efield->vehicle, atw );
   } 
   else 
      if (efield->building != NULL) { 
         if (getdiplomaticstatus2(efield->building->color, angreifer->color) == cawar) 
            for (int i = 0; i < angreifer->typ->weapons->count ; i++) 
               if (angreifer->typ->weapons->weapon[i].shootable() ) 
                  if (angreifer->typ->weapons->weapon[i].offensive() ) 
                     if (!( angreifer->typ->weapons->weapon[i].targets_not_hittable & ( 1 << cmm_building ))) { 
                        int tm = efield->building->typ->buildingheight;
                        if (tm & angreifer->typ->weapons->weapon[i].targ) {
                           if (fieldvisiblenow(efield, angreifer->color/8)) { 
                              int d = beeline(angreifer->xpos,angreifer->ypos,x,y); 
                              if (d <= angreifer->typ->weapons->weapon[i].maxdistance) 
                                 if (d >= angreifer->typ->weapons->weapon[i].mindistance) { 
                                    if (angreifer->height & angreifer->typ->weapons->weapon[i].sourceheight) 
                                       if (angreifer->ammo[i] > 0) { 
                                          atw->strength[atw->count ] = angreifer->weapstrength[i]; 
                                          atw->typ[atw->count ] = 1 << angreifer->typ->weapons->weapon[i].getScalarWeaponType() ;
                                          atw->num[atw->count ] = i; 
                                          atw->target = AttackWeap::building;
                                          atw->count++;
                                       } 

                                 } 
                           } 
                        } 
                     } 
      } 

   if ( efield->object ) {
      int n = 0;
      for ( int j = 0; j < efield->object->objnum; j++ )
         if ( efield->object->object[j]->typ->armor > 0 )
            n++;

      if ( n > 0 ) 
         if ((efield->vehicle == NULL) && ( efield->building == NULL)) {
            for ( int i = 0; i <= angreifer->typ->weapons->count - 1; i++) 
               if (angreifer->typ->weapons->weapon[i].shootable() ) 
                  if ( angreifer->typ->weapons->weapon[i].getScalarWeaponType() == cwcannonn ||  
                       angreifer->typ->weapons->weapon[i].getScalarWeaponType() == cwbombn ) { 
                     if (!( angreifer->typ->weapons->weapon[i].targets_not_hittable & ( 1 << cmm_building ))) 
                        if (chfahrend & angreifer->typ->weapons->weapon[i].targ ) { 
                           if (fieldvisiblenow(efield, angreifer->color/8)) { 
                              int d = beeline(angreifer->xpos,angreifer->ypos,x,y); 
                              if (d <= angreifer->typ->weapons->weapon[i].maxdistance) 
                                 if (d >= angreifer->typ->weapons->weapon[i].mindistance) { 
                                    if (angreifer->height & angreifer->typ->weapons->weapon[i].sourceheight ) 
                                       if (angreifer->ammo[i] > 0) { 
                                          atw->strength[atw->count ] = angreifer->weapstrength[i];
                                          atw->num[atw->count ] = i;
                                          atw->typ[atw->count ] = 1 << angreifer->typ->weapons->weapon[i].getScalarWeaponType();
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

   if (angreifer->typ->weapons->count == 0) 
     return false ;

   if ( getdiplomaticstatus2 ( angreifer->color, verteidiger->color ) == cawar )
      for ( int i = 0; i < angreifer->typ->weapons->count ; i++) 
         if (angreifer->typ->weapons->weapon[i].shootable() ) 
            if (angreifer->typ->weapons->weapon[i].offensive() ) 
               if (verteidiger->height & angreifer->typ->weapons->weapon[i].targ ) 
                  if (angreifer->height & angreifer->typ->weapons->weapon[i].sourceheight ) 
                     if (!( angreifer->typ->weapons->weapon[i].targets_not_hittable & ( 1 << verteidiger->typ->movemalustyp )))
                        if (angreifer->ammo[i] > 0) {
                           result = true;
                           if ( atw ) {
                              atw->strength[atw->count] = angreifer->weapstrength[i];
                              atw->num[atw->count ] = i;
                              atw->typ[atw->count ] = 1 << angreifer->typ->weapons->weapon[i].getScalarWeaponType();
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

   if (angreifer->typ->weapons->count == 0) 
     return false ;

   if ( getdiplomaticstatus2 ( angreifer->color, verteidiger->color ) == cawar )
      for ( int i = 0; i < angreifer->typ->weapons->count ; i++) 
         if (angreifer->typ->weapons->weapon[i].shootable() ) 
            if (angreifer->typ->weapons->weapon[i].offensive() ) 
               if (verteidiger->height & angreifer->typ->weapons->weapon[i].targ ) 
                  if (minmalq <= angreifer->typ->weapons->weapon[i].maxdistance) 
                     if (minmalq >= angreifer->typ->weapons->weapon[i].mindistance) 
                        if (angreifer->height & angreifer->typ->weapons->weapon[i].sourceheight ) 
                           if (!( angreifer->typ->weapons->weapon[i].targets_not_hittable & ( 1 << verteidiger->typ->movemalustyp )))
                              if (angreifer->ammo[i] > 0) {
                                 result =  true;
                                 if ( atw ) {
                                    atw->strength[atw->count] = angreifer->weapstrength[i];
                                    atw->num[atw->count ] = i;
                                    atw->typ[atw->count ] = 1 << angreifer->typ->weapons->weapon[i].getScalarWeaponType();
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

   if (angreifer->typ->weapons->count == 0) 
     return false ;

   int dist = beeline ( angreifer, verteidiger );
   if ( getdiplomaticstatus2 ( angreifer->color, verteidiger->color ) == cawar )
      if ( !angreifer->attacked )
         if ( !angreifer->typ->wait || !angreifer->hasMoved() )
            for ( int i = 0; i < angreifer->typ->weapons->count ; i++) 
               if (angreifer->typ->weapons->weapon[i].shootable() ) 
                  if (angreifer->typ->weapons->weapon[i].offensive() ) 
                     if (verteidiger->height & angreifer->typ->weapons->weapon[i].targ ) 
                        if (dist <= angreifer->typ->weapons->weapon[i].maxdistance) 
                           if (dist >= angreifer->typ->weapons->weapon[i].mindistance) 
                              if (angreifer->height & angreifer->typ->weapons->weapon[i].sourceheight ) 
                                 if (!( angreifer->typ->weapons->weapon[i].targets_not_hittable & ( 1 << verteidiger->typ->movemalustyp )))
                                    if (angreifer->ammo[i] > 0) {
                                       result = true;
                                       if ( atw ) {
                                          atw->strength[atw->count] = angreifer->weapstrength[i];
                                          atw->num[atw->count ] = i;
                                          atw->typ[atw->count ] = 1 << angreifer->typ->weapons->weapon[i].getScalarWeaponType();
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
   tnfilestream stream ( "weapons.dat", 1 );
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
      return 255;

   int typ = translat[ scalar ];
   if ( typ == 255 ) {
      displaymessage("tweapdist::getweapstrength: invalid type ", 1 );
      return 255;
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

