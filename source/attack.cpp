//     $Id: attack.cpp,v 1.19 2000-07-06 11:07:25 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
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
// #include <vector>
#include "tpascal.inc"
#include "typen.h"
#include "attack.h"
#include "newfont.h"
#include "basegfx.h"
#include "keybp.h"
#include "spfst.h"
#include "dlg_box.h"
#include "sgstream.h"
#include "mousehnd.h"
#include "timer.h"
#include "loaders.h"
#include "soundList.h"


#ifdef _DOS_
 #define DEBUG( msg ) 
#else
 #define DEBUG( msg ) fprintf( stderr, "DEBUG : %s\n", msg )
#endif

   #define damagefaktor 4
   #define  verteidigungsfaktor  14     /* wird durch 8 geteilt */
   #define  kf_ships   12               /* wird durch 8 geteilt */
   #define  kf_tanks    8               /* wird durch 8 geteilt */
   #define  kf_planes  18               /* wird durch 8 geteilt */


int          attackstrength(byte         damage)
{ 
   return (300 - 2 * damage) / 3; 
} 

/*
int          get_strongest_weapon(pvehicle     d_eht,
                                  int          targetheight,
                                  int          distance,
                                  int          targettype )
{ 
   int i = 0; 
   int str = 0; 
   int hw = 255;   
   if (distance <= maxmalq) 
         while (i + 1 <= d_eht->typ->weapons->count) { 
            if ((d_eht->ammo[i] > 0) && (d_eht->weapstrength[i] > str) && (d_eht->typ->weapons->weapon[i].mindistance <= distance) && (d_eht->typ->weapons->weapon[i].maxdistance >= distance) && (d_eht->typ->weapons->weapon[i].targ & aheight ) 
                 && (d_eht->typ->weapons->weapon[i].sourceheight & d_eht->height )) {
               hw = i;
               str = d_eht->weapstrength[i]; 
            } 
            i++; 
      } 
   return hw; 
} 
*/





const float  maxsandwichedfactor = 1.4;  // 1 + factor !!!
#ifdef HEXAGON
const int   sandwich[5]  = { 4, 11, 16, 11, 4 };
const int   maxsandwichedsum = 46;
#else
const int   sandwich[7]  = {3, 7, 11, 16, 11, 7, 3};
const int   maxsandwichedsum = 58;
#endif

int  check_sandwiched ( pvehicle     d_eht,  shortint     direc )
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



int  calc_sandwiched( int  ax,  int ay,  pvehicle d_eht )
{ 

   int sandwiched = 0; 
   int direction = getdirection(ax,ay,d_eht->xpos,d_eht->ypos); 
   for ( int i = 0; i < sidenum-1; i++)
      if (check_sandwiched (d_eht,i+1 + (direction-sidenum/2) ))
         sandwiched += sandwich[i];

   return sandwiched;
} 








float AttackFormula :: damage ( int damage )
{
   float a = attackstrength( damage);
   return a / 100;
}

float AttackFormula :: experience ( int experience )
{
   float e = experience;
   if ( e < 1 )
      e = 1;
   return 1 + e/4;
}

float AttackFormula :: attackbonus ( int abonus )
{
   float a = abonus;
   return 1 + a/8;
}




tfight :: tfight ( void )
{
   synchronedisplay = 0;
}


void tfight :: calc ( void )
{

   if ( av.strength ) { 
      int w = (int)(dv.damage + 
            (  1000 * av.strength 
               * experience ( av.experience )
               * damage ( av.damage ) 
               * attackbonus ( av.attackbonus ) 
               * ( 1 + dv.einkeilung * maxsandwichedfactor / maxsandwichedsum )
                 / (dv.armor / 4 
                    * (10 + dv.defensebonus*10 / 8) / 10 )
               * 100 / damagefaktor )
            / 1000);

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
      int w = av.damage + 
         ( 1000 * dv.strength 
           * experience ( dv.experience )
           * attackstrength(dv.damage) / 100
           * attackbonus ( dv.attackbonus )
             / (av.armor / 4 
                * (10 + av.defensebonus*10 / 8) / 10)
           * 100 / damagefaktor )
         / 1000;


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
  /*
  if ( dv.strength ) {
     sound.weaponSound(_attackingunit->getWeapon(av.weapnum)->getScalarWeaponType())->playWait();
     sound.weaponSound(_attackedunit->getWeapon(dv.weapnum)->getScalarWeaponType())->play();
  } else
     sound.weaponSound(_attackingunit->getWeapon(av.weapnum)->getScalarWeaponType())->play();
  */
  sound.weaponSound(_attackingunit->getWeapon(av.weapnum)->getScalarWeaponType())->play();
  tfight::calcdisplay(ad,dd);
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

   paintbar ( 6, 100 *  dv.einkeilung / 58, dc );

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


   int time1 = gameoptions.attackspeed1;
   if ( time1 <= 0 )
      time1 = 30;

   int time2 = gameoptions.attackspeed2;
   if ( time2 <= 0 )
      time2 = 50;

   int time3 = gameoptions.attackspeed3;
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
      for ( i = d1; i <= newpos1 && i <= av.damage; i++ )
         paintline ( 4, 100 - i, bk );

      d1 = i;

      int j;
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

   av.strength   = attackingunit->weapstrength[_weapon] * weapdist->getweapstrength(weap, dist, attackingunit->height, attackedunit->height ) / 255;
   av.armor  = attackingunit->armor;
   av.damage     = attackingunit->damage;
   av.experience  = attackingunit->experience;
   av.einkeilung = 0;
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

      dv.strength  = attackedunit->weapstrength[ dv.weapnum ] * weapdist->getweapstrength(weap, dist, attackedunit->height, attackingunit->height ) / 255;
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
      dv.einkeilung = calc_sandwiched ( attackingunit->xpos, attackingunit->ypos, attackedunit );
   else
      dv.einkeilung = 0;

   
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

   if ( _attackingunit->reactionfire_active >= 3 ) 
      _attackingunit->reactionfire &= 0xff ^ ( 1 <<  dv.color );

   _attackingunit->attacked = true; 
   if ( !(_attackingunit->functions & cf_moveafterattack) )
     _attackingunit->movement = 0;

   _attackedunit->damage    = dv.damage;

   if ( _respond ) {
      _attackingunit->damage    = av.damage;

      _attackedunit->experience = dv.experience;
      _attackedunit->ammo[ dv.weapnum ] = dv.weapcount;
   }

   /* If the attacking vehicle was destroyed, remove it */
   if ( _attackingunit->damage >= 100 ) {
     DEBUG("Attacker Destroyed");
     sound.boom->play();
     removevehicle ( _pattackingunit );
   }

   /* If the attacked vehicle was destroyed, remove it */
   if ( _attackedunit->damage >= 100 ) {
     DEBUG("Target Destroyed");
     sound.boom->play();
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
   av.strength  = attackingunit->weapstrength[_weapon] * weapdist->getweapstrength(weap, dist, attackingunit->height, _attackedbuilding->typ->buildingheight ) / 255;
   av.armor = attackingunit->armor;
   av.damage    = attackingunit->damage;
   av.experience = attackingunit->experience;
   av.einkeilung = 0;
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


   dv.armor = _attackedbuilding->typ->armor;
   dv.damage    = _attackedbuilding->damage;
   dv.experience = 0;
   dv.einkeilung = 0;

   dv.defensebonus = 0; // getfield ( x, y ) -> getdefensebonus();
   dv.color      = _attackedbuilding->color >> 3;
   dv.initiative = 0;
   dv.kamikaze = 0;


}



void tunitattacksbuilding :: setresult ( void )
{
   _attackingunit->experience = av.experience;
   _attackingunit->ammo[ av.weapnum ] = av.weapcount;

   _attackingunit->attacked = true; 
   if ( !(_attackingunit->functions & cf_moveafterattack) )
      _attackingunit->movement = 0;


   _attackedbuilding->damage    = dv.damage;

   /* Remove the attacking unit if it was destroyed */
   if ( _attackingunit->damage >= 100 ) {
      sound.boom->play();
      removevehicle ( &_attackingunit );
   }

   /* Remove attacked building if it was destroyed */
   if ( _attackedbuilding->damage >= 100 ) {
     sound.boom->play();
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
   av.einkeilung = 0;
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
   dv.einkeilung = 0;
   dv.weapnum = 0;
   dv.weapcount = 0;
   dv.color = attackedunit->color >> 3;
   dv.initiative = attackedunit->typ->initiative;
   dv.attackbonus = 0;
   dv.kamikaze = 0;
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
     sound.boom->play();
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
   av.strength  = attackingunit->weapstrength[weapon] * weapdist->getweapstrength(weap, dist, -1, -1  ) / 255;
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
   dv.einkeilung = 0;
   dv.color = 8 ;
   dv.kamikaze = 0;

}

void tunitattacksobject :: setresult ( void )
{
   // _attackingunit->experience = av.experience;
   _attackingunit->ammo[ av.weapnum ] = av.weapcount;

   _attackingunit->attacked = true; 
   if ( !(_attackingunit->functions & cf_moveafterattack) )
      _attackingunit->movement = 0;


   _obji->damage    = dv.damage;

   /* Remove the object if it was destroyed */
   if ( _obji->damage >= 100 ) {
     sound.boom->play();
     getfield ( _x, _y )-> removeobject ( _obji->typ );
   }

   actmap->time.a.move++;

}


void tunitattacksobject :: paintimages ( int xa, int ya, int xd, int yd )
{
   putrotspriteimage ( xa, ya, _attackingunit->typ->picture[0], _attackingunit->color );
   _obji->typ->display ( xd - 5, yd - 5 );
}
