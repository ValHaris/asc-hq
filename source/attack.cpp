//     $Id: attack.cpp,v 1.2 1999-11-16 03:41:02 tmwilson Exp $
//
//     $Log: not supported by cvs2svn $
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
#include "vesa.h"
#include "keybp.h"
#include "spfst.h"
#include "dlg_box.h"
#include "spfldutl.h"
#include "sgstream.h"
#include "mousehnd.h"
#include "timer.h"
#include "loaders.h"



   #define damagefaktor 4
   #define  verteidigungsfaktor  14     /* wird durch 8 geteilt */
   #define  kf_ships   12               /* wird durch 8 geteilt */
   #define  kf_tanks    8               /* wird durch 8 geteilt */
   #define  kf_planes  18               /* wird durch 8 geteilt */


byte         attackstrength(byte         damage)
{ 
   return (300 - 2 * damage) / 3; 
} 


byte         get_heaviest_weapon(pvehicle     d_eht,
                                 byte         aheight,
                                 byte         distance)
{ 
   int i = 0; 
   int str = 0; 
   int hw = 255;   /*  error-wert  ( keine waffe gefunden )  */ 
   if (distance <= maxmalq) 
         while (i + 1 <= d_eht->typ->weapons->count) { 
            if ((d_eht->ammo[i] > 0) && (d_eht->weapstrength[i] > str) && (d_eht->typ->weapons->weapon[i].mindistance <= distance) && (d_eht->typ->weapons->weapon[i].maxdistance >= distance) && (d_eht->typ->weapons->weapon[i].targ & aheight ) && 
               (d_eht->typ->weapons->weapon[i].sourceheight & d_eht->height )) {
               hw = i;
               str = d_eht->weapstrength[i]; 
            } 
            i++; 
      } 
   return hw; 
} 






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















tfight :: tfight ( void )
{
   synchronedisplay = 0;
}


void tfight :: calc ( void )
{

   if ( av.strength ) { 
      int w = (int)(dv.damage + 
            (  1000 * av.strength 
               * (10 + av.experience*10 / 4) / 10
               * attackstrength(av.damage) / 100
               * ( 8 + av.attackbonus ) / 8
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
           * (10 + dv.experience*10 / 4) / 10
           * attackstrength(dv.damage) / 100
           * ( 8 + dv.attackbonus ) / 8
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

void tfight :: calcdisplay ( int ad, int dd )
{                                
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



   int t = ticker;
   calc();

   do {

   } while ( t + 50 > ticker ); /* enddo */




   if ( ad != -1 )
      av.damage = ad;
   if ( dd != -1 )
      dv.damage = dd;


                             
   int steps;
   if ( av.damage - avd > dv.damage - dvd )
      steps = av.damage - avd ;
   else
      steps = dv.damage - dvd;

   int tme = 1;
   if ( steps < 20 )
      tme = 2;
      

   for ( int i = 0; i < steps; i++ ) {
      t = ticker;

      if ( i < av.damage - avd )
        paintline ( 4, 100 - ( avd + i ), bk );

      if ( i < dv.damage - dvd )
        paintline ( 5, 100 - ( dvd + i ), bk );

      do {

      } while ( t + tme > ticker ); /* enddo */
   }





   t = ticker;
   do {

   } while ( t + 70 > ticker ); /* enddo */
   putimage ( agmp->resolutionx - ( 640 - 451 ), 211, icons.attack.orgbkgr );
   getinvisiblemouserectanglestk ( );
}



/*

class tunitattacksunit : public tfight {
           void setup ( pvehicle attackingunit; pvehicle attackedunit; int respond; int weapon );
           void setresult ( void );

      };

*/

void tunitattacksunit :: setup ( pvehicle &attackingunit, pvehicle &attackedunit, int respond, int weapon )
{
   _attackingunit = attackingunit;
   _attackedunit  = attackedunit;

   _pattackingunit = &attackingunit;
   _pattackedunit  = &attackedunit;

   int dist = beeline ( attackingunit->xpos, attackingunit->ypos, attackedunit->xpos, attackedunit->ypos );
   int _weapon;

   if ( weapon == -1 )
      _weapon = get_heaviest_weapon( attackingunit, attackedunit->height, dist );
   else
      _weapon  = weapon;

   SingleWeapon* weap = &attackingunit->typ->weapons->weapon[_weapon];
   av.strength   = attackingunit->weapstrength[_weapon] * weapdist->getweapstrength(weap, dist, attackingunit->height, attackedunit->height ) / 255;
   av.armor  = attackingunit->armor;
   av.damage     = attackingunit->damage;
   av.experience  = attackingunit->experience;
   av.einkeilung = 0;
   av.weapnum    = _weapon;
   av.weapcount  = attackingunit->ammo [ _weapon ];
   av.color      = attackingunit->color >> 3;
   av.initiative = attackingunit->typ->initiative;

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
      pattackweap atw = attackpossible3n ( attackedunit, attackingunit );
      int n = -1;
      int s = 0;
      for ( int i = 0; i < atw->count; i++ )
         if ( atw->strength[i] > s ) {
            s = atw->strength[i];
            n = i;
         }

      if ( n < 0 )
         respond = 0;
      else
         dv.weapnum = atw->num [ n ];

      delete atw;

   } else
      respond = 0;

   if ( respond ) {
      weap = &attackedunit->typ->weapons->weapon[ dv.weapnum ];
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


}



void tunitattacksunit :: setresult ( void )
{
   _attackingunit->experience = av.experience;
   _attackingunit->ammo[ av.weapnum ] = av.weapcount;

/*
   if ( _attackingunit->reactionfire_active >= 3 ) 
      _attackingunit->reactionfire ^= 1 <<  dv.color;
*/

   _attackingunit->attacked = true; 
   if ( !(_attackingunit->functions & cf_moveafterattack) )
     _attackingunit->movement = 0;

   _attackedunit->damage    = dv.damage;

   if ( _respond ) {
      _attackingunit->damage    = av.damage;

      _attackedunit->experience = dv.experience;
      _attackedunit->ammo[ dv.weapnum ] = dv.weapcount;
   }

   if ( _attackingunit->damage >= 100 )
      removevehicle ( _pattackingunit );

   if ( _attackedunit->damage >= 100 )
      removevehicle ( _pattackedunit );


   actmap->time.a.move++;
}

void tunitattacksunit :: paintimages ( int xa, int ya, int xd, int yd ) 
{
   putrotspriteimage ( xa, ya, _attackingunit->typ->picture[0], _attackingunit->color );
   putrotspriteimage ( xd, yd, _attackedunit ->typ->picture[0], _attackedunit->color  );
};










void tunitattacksbuilding :: setup ( pvehicle attackingunit, int x, int y, int weapon )
{
   _attackingunit = attackingunit;
   _x = x;
   _y = y;
   _attackedbuilding  = getfield ( x, y ) -> building;

   int dist = beeline ( attackingunit->xpos, attackingunit->ypos, x, y );
   int _weapon;

   if ( weapon == -1 )
      _weapon = get_heaviest_weapon( attackingunit, _attackedbuilding->typ->buildingheight, dist );
   else
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


}



void tunitattacksbuilding :: setresult ( void )
{
   _attackingunit->experience = av.experience;
   _attackingunit->ammo[ av.weapnum ] = av.weapcount;

   _attackingunit->attacked = true; 
   if ( !(_attackingunit->functions & cf_moveafterattack) )
      _attackingunit->movement = 0;


   _attackedbuilding->damage    = dv.damage;

   if ( _attackingunit->damage >= 100 )
      removevehicle ( &_attackingunit );

   if ( _attackedbuilding->damage >= 100 )
      removebuilding ( &_attackedbuilding );

   actmap->time.a.move++;
}

void tunitattacksbuilding :: paintimages ( int xa, int ya, int xd, int yd ) 
{
   putrotspriteimage ( xa, ya, _attackingunit->typ->picture[0], _attackingunit->color );
   putrotspriteimage ( xd, yd, getfield ( _x, _y ) -> picture , _attackedbuilding->color  );
};


/*
class tmineattacksunit : public tfight {
            pfield _mineposition;
            pvehicle _attackedunit;

           void setup ( pfield mineposition; pvehicle attackedunit );
           void setresult ( void );

      };
*/

void tmineattacksunit :: setup ( pfield mineposition, pvehicle &attackedunit )
{
   if ( !mineposition->object || !mineposition->object->mine )
      displaymessage(" tmineattacksunit :: setup \n no mine to attack !\n",2 );

   if ( attackedunit->height >= chtieffliegend )
      displaymessage(" tmineattacksunit :: setup \n mine attacks flying unit!\n",2 );


   _mineposition = mineposition;
   _attackedunit = attackedunit;
   _pattackedunit = &attackedunit;

   av.strength = mineposition->object->minestrength;
   if ( (mineposition->object->mine >> 4 ) == cmantipersonnelmine   &&  (attackedunit->functions & cf_trooper ) )
      av.strength *= 2;

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

}


void tmineattacksunit :: setresult ( void )
{
   _mineposition->object->mine = 0;
   _mineposition->object->minestrength = 0;

   _attackedunit->damage = dv.damage;

   if ( _attackedunit->damage >= 100 )
      removevehicle ( _pattackedunit );

}

void tmineattacksunit :: paintimages ( int xa, int ya, int xd, int yd ) 
{
   putspriteimage    ( xa, ya, getmineadress ( _mineposition->object->mine >> 4 ));
   putrotspriteimage ( xd, yd, _attackedunit ->typ->picture[0], _attackedunit->color  );
}


/*
class tunitattacksobject : public tfight {
           pvehicle     _attackingunit; 
           pobjectcontainer      _object; 
         public:
           void setup ( pvehicle attackingunit; pobjectcontainer      object; int weapon );
           void setresult ( void );
      };
*/


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

}

void tunitattacksobject :: setresult ( void )
{
   // _attackingunit->experience = av.experience;
   _attackingunit->ammo[ av.weapnum ] = av.weapcount;

   _attackingunit->attacked = true; 
   if ( !(_attackingunit->functions & cf_moveafterattack) )
      _attackingunit->movement = 0;


   _obji->damage    = dv.damage;

   if ( _obji->damage >= 100 )
      getfield ( _x, _y )-> removeobject ( _obji->typ );

   actmap->time.a.move++;

}


void tunitattacksobject :: paintimages ( int xa, int ya, int xd, int yd )
{
   putrotspriteimage ( xa, ya, _attackingunit->typ->picture[0], _attackingunit->color );
   _obji->typ->display ( xd - 5, yd - 5 );
}
