//     $Id: building.cpp,v 1.7 1999-11-25 21:59:59 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.6  1999/11/23 21:07:21  mbickel
//      Many small bugfixes
//
//     Revision 1.5  1999/11/22 18:26:53  mbickel
//      Restructured graphics engine:
//        VESA now only for DOS
//        BASEGFX should be platform independant
//        new interface for initialization
//      Rewrote all ASM code in C++, but it is still available for the Watcom
//        versions
//      Fixed bugs in RLE decompression, BI map importer and the view calculation
//
//     Revision 1.4  1999/11/18 17:31:01  mbickel
//      Improved BI-map import translation tables
//      Moved macros to substitute Watcom specific routines into global.h
//
//     Revision 1.3  1999/11/16 17:03:56  mbickel
//     Made ASC compilable for DOS again :-)
//     Merged all the bug fixes in that I did last week
//
//     Revision 1.2  1999/11/16 03:41:10  tmwilson
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

/*  Unit Buildings     */          

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "tpascal.inc"
#include "typen.h"
#include "misc.h"
#include "basegfx.h"
#include "newfont.h"
#include "keybp.h"
#include "timer.h"
#include "dlg_box.h"
#include "spfst.h"
#include "stdio.h"
#include "string.h"
#include "dialog.h"
#include "mousehnd.h"
#include "building.h"
#include "controls.h"
#include "loadpcx.h"
#include "sg.h"
#include "stack.h"
#include "gamedlg.h"
#include "basestrm.h"
#ifdef _DOS_
#include <conio.h>
#endif


int                        autofill_prodtnk = 1;
pcontainercontrols         cc;
pbuildingcontrols          cc_b;
ptransportcontrols         cc_t;

#define maxrecursiondepth 10


int recursiondepth = -1;
tbuildingparamstack buildingparamstack[maxrecursiondepth];


#ifdef HEXAGON
 #define containerxpos 100
 #define containerypos 70
#else
 #define containerxpos 0
 #define containerypos 0
#endif


const int repaint=0;
const int repaintall=0;
const int up=1;
const int down=2;
const int left=3;
const int right=4;

#ifdef HEXAGON
 #define unitsshownx 6
#else
 #define unitsshownx 9
#endif

 #define unitsshowny 3

#ifdef HEXAGON
 int unitposx[ unitsshownx + 1 ] = { containerxpos + 20, 
                                     containerxpos + 76, 
                                     containerxpos + 135, 
                                     containerxpos + 190, 
                                     containerxpos + 250, 
                                     containerxpos + 304,
                                     containerxpos + 304 + fieldxsize };

 int unitposy[ unitsshowny + 1 ] = { containerypos + 105, 
                                 containerypos + 159, 
                                 containerypos + 213,
                                 containerypos + 213 + fieldysize };
#else
 int unitposx[ unitsshownx + 1] = { 57 + 0 * 38, 
                                 57 + 1 * 38, 
                                 57 + 2 * 38, 
                                 57 + 3 * 38,
                                 57 + 4 * 38,
                                 57 + 5 * 38,
                                 57 + 6 * 38,
                                 57 + 7 * 38,
                                 57 + 8 * 38,
                                 57 + 9 * 38 };

 int unitposy[ unitsshowny + 1 ] = { 148, 148+35, 148 + 2 * 35, 148 + 3 * 35  };

#endif



#ifdef HEXAGON
 #define subwinx1 (containerxpos + 13)
 #define subwiny1 (containerypos + 276 )
 #define subwinx2 ( subwinx1 + 345 )
 #define subwiny2 ( subwiny1 + 110 )
 #define laschxpos subwinx1
 #define laschypos (subwiny2+1)
 #define laschdist 50
 #define nameposx containerxpos + 149
 #define nameposy containerypos + 17

#else

 #define subwinx1 52
 #define subwiny1 264
 #define subwinx2 397
 #define subwiny2 374
 #define laschxpos 52
 #define laschypos 375
 #define laschdist 50
 #define nameposx 188
 #define nameposy 59
#endif

#define laschheight 12
#define laschstepwidth 3
#define bkgrcol 171
#define bkgrdarkcol 172


#ifdef HEXAGON
 int tabmarkpos[3] = { containerypos + 154-42, containerypos + 273-42+54, containerypos + 388-42+54 };
 int tabmarkposx = containerxpos+ 4;
#else
 int tabmarkpos[3] = { 154, 273, 388 };
 int tabmarkposx = 43;
#endif


const char* resourceusagestring = "; need: ~%d~ energy, ~%d~ material, ~%d~ fuel";




int getstepwidth ( int max )
{
   double ep = log10 ( max );
   if ( ep > 2.0 )
      ep -= 2.0;
   else
      ep = 0.0;

   return (int)pow ( 10, ep );
}



void  container ( pvehicle eht, pbuilding bld )
{
    recursiondepth++;
    memset ( &buildingparamstack[recursiondepth], 0, sizeof ( buildingparamstack[recursiondepth] ));
    npush ( cc );
    npush ( cc_b );
    npush ( cc_t );

    if ( eht && eht->color == actmap->actplayer * 8  ) {
        buildingparamstack[recursiondepth].eht = eht;
        ccontainer_t  containert;
        containert.init ( eht );
        containert.run ();
        containert.done ();
    } else
       if (bld && ( bld->color == actmap->actplayer * 8  || bld->color == 8*8 )) {
           buildingparamstack[recursiondepth].bld = bld;
           ccontainer_b  containerb;
           containerb.init ( bld );
           containerb.run ();
           containerb.done ();
       };

    npop ( cc_t );
    npop ( cc_b );
    npop ( cc );
    recursiondepth--;
}




/***********************************************************************************/
/*                                                                                 */
/*   class    ccontainercontrols                                                   */
/*                                                                                 */
/***********************************************************************************/
ccontainercontrols :: ccontainercontrols (void)
{
   cc = this;
}



int   ccontainercontrols :: crepairanything :: checkto ( int olddamage, int newdamage, int energycost, int materialcost, int fuelcost, int effizienz )
{
   if ( newdamage > olddamage ) 
       newdamage = olddamage;

   int   needede = energycost    / 100 * (olddamage-newdamage) / effizienz;
   int   neededm = materialcost  / 100 * (olddamage-newdamage) / effizienz;
   int   neededf = fuelcost      / 100 * (olddamage-newdamage) / effizienz;

   int   availae = cc->getenergy  ( needede, 0 );
   int   availam = cc->getmaterial( neededm, 0 );
   int   availaf = cc->getfuel    ( neededf, 0 );

   int   eperc;
   if ( needede )
      eperc = 100 * availae / needede;
   else
      eperc = 100;

   int   mperc;
   if ( neededm )
      mperc = 100 * availam / neededm;
   else
      mperc = 100;

   int   fperc;
   if ( neededf )
      fperc = 100 * availaf / neededf;
   else
      fperc = 100;


   int perc;
   if ( mperc < eperc )
      perc = mperc;
   else
      perc = eperc;

   if ( fperc < perc )
      perc = fperc;


   int repairabledamage = ( olddamage-newdamage) * perc / 100 ;

   energycosts   = energycost   / 100 * repairabledamage  / effizienz;
   materialcosts = materialcost / 100 * repairabledamage  / effizienz;
   fuelcosts     = fuelcost     / 100 * repairabledamage  / effizienz;
   return repairabledamage;
};


int   ccontainercontrols :: crepairunit :: available ( pvehicle eht )
{
   if ( eht->damage )
      return ( ( cc->getspecfunc ( mbuilding ) & cgrepairfacilityb )  ||  ( cc->getspecfunc ( mtransport ) & cfrepair ) );
   else
      return 0;
}



int   ccontainercontrols :: crepairunit :: repairto (pvehicle eht, char newdamage)
{
   int ndamage = eht->damage - checkto ( eht, newdamage );
   int e = cc->getenergy   ( energycosts, 1 );
   int m = cc->getmaterial ( materialcosts, 1 );
   int f = cc->getfuel     ( fuelcosts, 1 );
   if ( e < energycosts   ||   m < materialcosts   ||  f < fuelcosts ) {
      displaymessage( " ccontainercontrols :: crepairunit :: repairto    \n Not enough resources ! \n repair canceled ! ", 1 );
      int ep = cc->putenergy   ( e );
      int mp = cc->putmaterial ( m );
      int fp = cc->putfuel     ( f );

      /*
      if ( ep < e  &&  mp < m )
         displaymessage2 ( " %d energy and %d material lost ! ", e - ep, m - mp );
      else
         if ( ep < e )
            displaymessage2 ( " %d energy lost ! ", e - ep );
         else
            if ( mp < m )
               displaymessage2 ( " %d material lost ! ", m - mp );
       */
   } else
      eht->damage = ndamage;

   return ndamage;
}




void  ccontainercontrols :: crefill :: fuel (pvehicle eht, int newfuel)
{
   if ( newfuel > eht->typ->tank )
      newfuel = eht->typ->tank;

   if ( newfuel > eht->getmaxfuelforweight() )
      newfuel = eht->getmaxfuelforweight();

   if ( newfuel > eht->fuel ) {
      eht->fuel += cc->getfuel ( newfuel - eht->fuel, 1 );
   } else {
      int delta = eht->fuel - newfuel;
      delta = cc->putfuel ( delta );
      eht->fuel -= delta;
   }
};


void  ccontainercontrols :: crefill :: material (pvehicle eht, int newmaterial)
{
   if ( newmaterial > eht->typ->material )
      newmaterial = eht->typ->material;

   if ( newmaterial > eht->getmaxmaterialforweight() )
      newmaterial = eht->getmaxmaterialforweight();

   if ( newmaterial > eht->material ) {
      eht->material += cc->getmaterial ( newmaterial - eht->material, 1 );
   } else {
      int delta = eht->material - newmaterial;
      delta = cc->putmaterial ( delta );
      eht->material -= delta;
   }
};




void  ccontainercontrols :: crefill :: ammunition (pvehicle eht, char weapon, int newa )
{
   if ( eht->typ->weapons->weapon[ weapon ].typ & ( cwweapon | cwmineb ) ) {
      if ( newa > eht->typ->weapons->weapon[ weapon ].count )
         newa = eht->typ->weapons->weapon[ weapon ].count;

      if ( newa > eht->ammo[weapon] ) 
         eht->ammo[weapon]  +=  cc->getammunition ( log2 ( eht->typ->weapons->weapon[ weapon ].typ & ( cwweapon | cwmineb ) ) , newa - eht->ammo[weapon], 1, gameoptions.container.autoproduceammunition );
      else {
         cc->putammunition ( log2 ( eht->typ->weapons->weapon[ weapon ].typ & ( cwweapon | cwmineb ) ) , eht->ammo[weapon]  - newa, 1 );
         eht->ammo[weapon] = newa;
      }
   }

};



void  ccontainercontrols :: crefill :: filleverything ( pvehicle eht )
{
   fuel     ( eht, maxint );
   material ( eht, maxint );
   for (int i = 0; i < eht->typ->weapons->count; i++) 
     if ( eht->typ->weapons->weapon[ i ].typ & ( cwweapon | cwmineb ) ) 
        ammunition ( eht, i, maxint );

}

void  ccontainercontrols :: crefill :: emptyeverything ( pvehicle eht )
{
   fuel     ( eht, 0 );
   material ( eht, 0 );
   for (int i = 0; i < eht->typ->weapons->count; i++) 
     if ( eht->typ->weapons->weapon[ i ].typ & ( cwweapon | cwmineb ) ) 
        ammunition ( eht, i, 0 );

}


int ccontainercontrols :: cmove_unit_in_container :: moveupavail ( pvehicle eht )
{
   if ( eht )
      if ( recursiondepth > 0 ) {
         if ( buildingparamstack[recursiondepth-1].bld )
            return buildingparamstack[recursiondepth-1].bld->vehicleloadable ( eht );
         else
         if ( buildingparamstack[recursiondepth-1].eht )
            return buildingparamstack[recursiondepth-1].eht->vehicleloadable ( eht );
      }
   return 0;
}

int ccontainercontrols :: cmove_unit_in_container :: movedownavail ( pvehicle eht, pvehicle into )
{
   if ( eht )
      if ( into )
         if ( into->vehicleloadable ( eht ))
           return 1;
   return 0;
}


void ccontainercontrols :: cmove_unit_in_container :: moveup ( pvehicle eht )
{
   if ( moveupavail( eht ) )
      if ( eht ) {
        pvehicle targe = buildingparamstack[recursiondepth-1].eht;
        pbuilding targb = buildingparamstack[recursiondepth-1].bld;

        if ( targe ) {
           int i = 0;
           while ( i < maxloadableunits   &&  targe->loading[i] )
              i++;

           if ( !targe->loading[i] ) {
              targe->loading[i] = eht;
              i = 0;
              while ( cc_t->vehicle->loading[i] != eht) 
                 i++;

              cc_t->vehicle->loading[i] = 0;

           }
        } else
        if ( targb ) {
           int i = 0;
           while ( i < maxloadableunits   &&  targb->loading[i] )
              i++;

           if ( !targb->loading[i] ) {
              targb->loading[i] = eht;
              i = 0;
              while ( cc_t->vehicle->loading[i] != eht) 
                 i++;

              cc_t->vehicle->loading[i] = 0;
           }
        } 
      }
}


void ccontainercontrols :: cmove_unit_in_container :: movedown ( pvehicle eht, pvehicle into )
{
   if ( movedownavail(eht, into) ) {
  
       int i = 0;
       while ( i < maxloadableunits   &&  into->loading[i] )
          i++;

       if ( !into->loading[i] ) {
          into->loading[i] = eht;

          i = 0;
          if ( cc_t ) {
             while ( cc_t->vehicle->loading[i] != eht) 
               i++;
 
             cc_t->vehicle->loading[i] = 0;
          } else
          if ( cc_b ) {
             while ( cc_b->building->loading[i] != eht) 
               i++;
 
             cc_b->building->loading[i] = 0;
          } 
       }
   }   
}




/***********************************************************************************/
/*                                                                                 */
/*   class    cbuildingcontrols : Geb„ude-Innereien                                */
/*                                                                                 */
/***********************************************************************************/

cbuildingcontrols :: cbuildingcontrols (void)
{
   cc_b = this;
   cc_t = NULL;
}

void  cbuildingcontrols :: init (pbuilding bldng)
{
   building = bldng;
};



char  cbuildingcontrols :: getactplayer (void)
{
   return   building->color / 8;
};


int   cbuildingcontrols :: putenergy (int e, int abbuchen )
{
   return building->put_energy( e, 0, !abbuchen );
};


int   cbuildingcontrols :: putmaterial (int m, int abbuchen )
{
   return building->put_energy( m, 1 , !abbuchen );
};



int   cbuildingcontrols :: putfuel (int f, int abbuchen )
{
   return building->put_energy( f, 2, !abbuchen );
};







int   cbuildingcontrols :: putammunition ( int weapontype, int ammunition, int abbuchen)
{
   if ( abbuchen )
      building->munition[weapontype] += ammunition;
   return ammunition;
};



int   cbuildingcontrols :: getenergy ( int need, int abbuchen )
{
   return building->get_energy ( need, 0,  !abbuchen );
};



int   cbuildingcontrols :: getmaterial ( int need, int abbuchen )
{
   return building->get_energy ( need, 1, !abbuchen );
};



int   cbuildingcontrols :: getfuel ( int need, int abbuchen )
{
   return building->get_energy ( need, 2, !abbuchen );
};



int    cbuildingcontrols :: getammunition ( int weapontype, int num, int abbuchen, int produceifrequired )
{
   if ( building->munition[ weapontype ] > num  ) {

      if ( abbuchen )
         building->munition[ weapontype ] -= num;
      return num;

   } else {
      int toprod = num - building->munition[ weapontype ];
      int prd;
      if ( produceifrequired )
         prd = weaponpackagesize * cc_b->produceammunition.checkavail ( weapontype, (toprod + weaponpackagesize - 1) / weaponpackagesize );
      else
         prd = 0;

      if ( abbuchen ) {
         if ( produceifrequired )
            cc_b->produceammunition.produce ( weapontype, (toprod + weaponpackagesize - 1) / weaponpackagesize );

         if ( building->munition[ weapontype ] > num ) {
            building->munition[ weapontype ] -= num;
            return num;
         } else {
            int i = building->munition[ weapontype ];
            building->munition[ weapontype ] = 0;
            return i;
         }
      } else {
         if ( building->munition[ weapontype ] + prd > num )
            return num;
         else
            return building->munition[ weapontype ] + prd;
      }

   }
}

int    cbuildingcontrols :: ammotypeavail ( int type )
{
   return 1;
}


int   cbuildingcontrols :: getxpos (void)
{
   return   building->xpos;
};



int   cbuildingcontrols :: getypos (void)
{
   return   building->ypos;
};



int   cbuildingcontrols :: getspecfunc ( tcontainermode mode )
{
   if ( mode == mbuilding )
      return building->typ->special;
   else
      return 0;
};


int   cbuildingcontrols :: movement (  pvehicle eht, int mode )
{
   if ( eht->movement < minmalq )
      return 0;
   movementparams.height   = eht->height;
   movementparams.movement = eht->movement;
   movementparams.attacked = eht->attacked;

   int unitheight = -1;
   if ( building->typ->buildingheight <= chgetaucht )
      unitheight = building->typ->buildingheight;

   moveparams.movestatus = 0;
   int ma = moveavail( eht );
   if ( ma == 3 ) 
      eht->attacked = 1;

   if ( ma  >= 2 )
     ::movement( eht, unitheight );


   int check = 0;
   int orgheight = eht->height;
   int orgmove = eht->movement;

   int perc = eht->movement * 1024 / eht->typ->movement[log2 ( eht->height ) ];
   while ( moveparams.movestatus == 0  && check < 2) {
      if ( check == 0 ) {
         int h = eht->height << 1;
         if ( eht->typ->height & h ) {
            eht->height = h;
            eht->movement = eht->typ->movement[log2 ( eht->height ) ] * perc / 1024;
            ma = moveavail ( eht );
            if ( ma == 3 ) 
               eht->attacked = 1;
            if ( ma >= 2 ) 
               ::movement( eht, unitheight );
            
         } else
            check = 1;

      } else {
         int h = eht->height >> 1;
         if ( eht->typ->height & h ) {
            eht->height = h;
            eht->movement = eht->typ->movement[log2 ( eht->height ) ] * perc / 1024;

            ma = moveavail ( eht );
            if ( ma == 3 ) 
               eht->attacked = 1;
            if ( ma >= 2 ) 
               ::movement( eht, unitheight );
            
         } else
            check = 2;

      }

   }
   if ( moveparams.movestatus == 0 ) {
     eht->height = orgheight;
     eht->movement = orgmove;
   }

   return moveparams.movestatus != 0;
}

int   cbuildingcontrols :: moveavail ( pvehicle eht )
{
  if ( recursiondepth > 0 )
     return 0;
  if ( eht->movement < minmalq )
     return 0;

  if ( eht->typ->height & building->typ->unitheightreq )
    if ( eht->height & building->typ->loadcapability )
       return 2;
    else
       return 1;
  else
     if ( eht->functions & cf_trooper )
        return 2;
     else
        return 0;
}



void  cbuildingcontrols :: removevehicle ( pvehicle *peht )
{
   for (int i=0; i<=31; i++) {
       if ( *peht == building->loading[i] ) 
          building->loading[i]=NULL;
   };
   logtoreplayinfo ( rpl_removeunit, building->xpos, building->ypos, (*peht)->networkid );
   ::removevehicle ( peht );
}


void  cbuildingcontrols :: crecycling :: resourceuse (pvehicle eht) 
{
   int   output;
   if ( cc->getspecfunc( mbuilding ) & cgrecyclingplantb) 
      output = recyclingoutput;
   else 
      output = destructoutput;
                                                                                           
   material = eht->typ->production.material * (100 - eht->damage/2 ) / 100 / output;
   energy = 0;
}


void  cbuildingcontrols :: crecycling :: recycle (pvehicle eht)
{
   if (choice_dlg("do you really want to recycle this unit ?","~y~es","~n~o") == 1) { 
      resourceuse ( eht );
   
      if ( gameoptions.container.emptyeverything )
          cc->refill.emptyeverything ( eht );
   
   
      cc->putmaterial ( material );
   
      cc_b->removevehicle (&eht);
  }
};



void   cbuildingcontrols :: cnetcontrol :: setnetmode ( int category, int stat )
{
   if ( stat )
      cc_b->building->netcontrol |= category;
   else
      cc_b->building->netcontrol &= ~category;


};

int    cbuildingcontrols :: cnetcontrol :: getnetmode ( int mode )
{
    return  cc_b->building->netcontrol & mode;
};


void   cbuildingcontrols :: cnetcontrol :: emptyeverything ( void )
{
   setnetmode ( cnet_moveenergyout   + cnet_movematerialout   + cnet_movefuelout +
                cnet_stopenergyinput + cnet_stopmaterialinput + cnet_stopfuelinput       , 1 );

   setnetmode ( cnet_stopenergyoutput + cnet_stopmaterialoutput + cnet_stopfueloutput + 
                cnet_storeenergy      + cnet_storematerial      + cnet_storefuel         , 0 );
};

void    cbuildingcontrols :: cnetcontrol :: reset ( void )
{
   setnetmode ( 0, -1 );
};




int   cbuildingcontrols :: crepairbuilding :: available ( void )
{
   if ( cc_b->building->damage )
      return 1;
   else
      return 0;
}


int   cbuildingcontrols :: crepairbuilding :: checkto ( char newdamage )
{
   return cbuildingcontrols :: crepairanything :: checkto ( cc_b->building->damage, newdamage, cc_b->building->typ->produktionskosten.sprit, cc_b->building->typ->produktionskosten.material, 0, repairefficiency_building );
};



int   cbuildingcontrols :: crepairbuilding :: repairto ( char newdamage)
{
   int ndamage = cc_b->building->damage - checkto ( newdamage );
   int e = cc->getenergy   ( energycosts, 1 );
   int m = cc->getmaterial ( materialcosts, 1 );
   if ( e < energycosts   ||   m < materialcosts ) {
      displaymessage( " cbuildingcontrols :: crepairbuilding :: repairto    \n Not enough resources ! \n repair canceled ! ", 1 );
      int ep = cc->putenergy   ( e );
      int mp = cc->putmaterial ( m );

      if ( ep < e  &&  mp < m )
         displaymessage2 ( " %d energy and %d material lost ! ", e - ep, m - mp );
      else
         if ( ep < e )
            displaymessage2 ( " %d energy lost ! ", e - ep );
         else
            if ( mp < m )
               displaymessage2 ( " %d material lost ! ", m - mp );

   } else
      cc_b->building->damage = ndamage;

   return ndamage;
}


int   cbuildingcontrols :: crepairunitinbuilding :: checkto (pvehicle eht, char newdamage)
{
   return ccontainercontrols :: crepairanything :: checkto ( eht->damage, newdamage, eht->typ->production.energy, eht->typ->production.material, 0, repairefficiency_building );
};



cbuildingcontrols :: cproduceammunition :: cproduceammunition ( void )
{
   baseenergyusage    = 0; 
   basematerialusage  = 0; 
   basefuelusage      = 0; 
}




int  cbuildingcontrols :: cproduceammunition :: checkavail ( int weaptype, int num )
{
   if ( cc_b->building->typ->special & cgammunitionproductionb ) {
      int needede = cwaffenproduktionskosten[weaptype][0] * num;
      int neededm = cwaffenproduktionskosten[weaptype][1] * num;
      int neededf = cwaffenproduktionskosten[weaptype][2] * num;
   
      int   availae = cc->getenergy  ( baseenergyusage   + needede, 0 ) - baseenergyusage;
      int   availam = cc->getmaterial( basematerialusage + neededm, 0 ) - basematerialusage;
      int   availaf = cc->getfuel    ( basefuelusage     + neededf, 0 ) - basefuelusage;
   
      int   eperc;
      if ( needede )
         eperc = 100 * availae / needede;
      else
         eperc = 100;
   
      int   mperc;
      if ( neededm )
         mperc = 100 * availam / neededm;
      else
         mperc = 100;
   
      int   fperc;
      if ( neededf )
         fperc = 100 * availaf / neededf;
      else
         fperc = 100;
   
   
      int perc;
      if ( mperc < eperc )
         perc = mperc;
      else
         perc = eperc;
   
      if ( fperc < perc )
         perc = fperc;
   
      if ( perc < 0 )
         perc = 0;
      if ( perc > 100 )
         perc = 100;
   
      int prod = num * perc / 100;
   
      energyneeded   = cwaffenproduktionskosten[weaptype][0] * prod;
      materialneeded = cwaffenproduktionskosten[weaptype][1] * prod;
      fuelneeded     = cwaffenproduktionskosten[weaptype][2] * prod;
   
      return prod;
   } else {
      energyneeded   = 0;
      materialneeded = 0;
      fuelneeded     = 0;
      return 0;
   }
}


void cbuildingcontrols :: cproduceammunition :: produce ( int weaptype, int num )
{
    int n = checkavail( weaptype, num );
    cc->getenergy   ( energyneeded,   1 );
    cc->getmaterial ( materialneeded, 1 );
    cc->getfuel     ( fuelneeded,     1 );
    cc->putammunition ( weaptype, n*weaponpackagesize, 1 );

}






int   cbuildingcontrols :: cproduceunit :: available (pvehicletype fzt)
{
   if ( actmap->player[ cc->getactplayer() ].research.vehicletypeavailable ( fzt, actmap ) )
      if (( cc->getenergy( fzt->production.energy, 0 )     >= fzt->production.energy)  &&  
          ( cc->getmaterial( fzt->production.material, 0 ) >= fzt->production.material)) 
            return 1;
   
  return 0;
}



pvehicle cbuildingcontrols :: cproduceunit :: produce (pvehicletype fzt)
{
   pvehicle    eht;
   generatevehicle_cl ( fzt, cc->getactplayer() , eht, cc->getxpos(), cc->getypos() );
   int engot = cc->getenergy   ( fzt->production.energy,   1 );
   int magot = cc->getmaterial ( fzt->production.material, 1 );


   int i = 0;
   int n = 1;
   while ( n ) {
      if ( cc_b->building->loading[ i ] == NULL ) {
         cc_b->building->loading[ i ] = eht;
         n = 0;
      } else 
         i++;
      if ( i >= 32 ) {
         dispmessage2 ( 400, NULL );
         cc->putenergy ( engot );
         cc->putenergy ( magot );
         ::removevehicle ( &eht ); 
      }

   } /* endwhile */

   if ( gameoptions.container.filleverything ) 
      cc->refill.filleverything ( eht );

   return eht;
};

pvehicle cbuildingcontrols :: cproduceunit :: produce_hypothetically (pvehicletype fzt)
{
   pvehicle    eht;
   generatevehicle_cl ( fzt, cc->getactplayer() , eht, cc->getxpos (), cc->getypos () );


   return eht;
};




int   cbuildingcontrols :: ctrainunit :: available ( pvehicle eht )
{
   if ( actmap->gameparameter[cgp_bi3_training] )
      return 0;

   if ( eht->experience <= maxunitexperience - trainingexperienceincrease )  
     if ( !eht->attacked ) {
      if (  cc->getspecfunc ( mbuilding ) & cgtrainingb ) {
         int num = 0;
         int numsh = 0;
         for (int i = 0; i < eht->typ->weapons->count; i++ ) 
            if ( eht->typ->weapons->weapon[i].typ & cwshootableb )
               if ( eht->ammo[i] )
                  numsh++;
               else
                  num++;

         if ( num == 0  &&  numsh > 0 )
            return 1;
      }
   }

   return 0;
}



void  cbuildingcontrols :: ctrainunit :: trainunit ( pvehicle eht )
{
   if ( available ( eht ) ) {
      eht->experience+= trainingexperienceincrease;
      for (int i = 0; i < eht->typ->weapons->count; i++ ) 
         if ( eht->typ->weapons->weapon[i].typ & cwshootableb )
            eht->ammo[i]--;

      eht->attacked = 1;
      eht->movement = 0;
      logtoreplayinfo ( rpl_trainunit, cc->getxpos (), cc->getypos (), eht->experience, eht->networkid );

   }                                   
};



int   cbuildingcontrols :: cdissectunit :: available ( pvehicle eht )
{
   if ( eht )
      if (  cc->getspecfunc ( mbuilding ) & cgresearchb ) 
         if ( actmap->player[ cc->getactplayer() ].research.vehicletypeavailable ( eht->typ, actmap ) ) {
            if ( !actmap->player[ cc->getactplayer() ].research.vehicleclassavailable( eht->typ, eht->klasse, actmap ) )
               return 1;

         } else
            return 1;

   return 0;
}

void   cbuildingcontrols :: cdissectunit :: dissectunit ( pvehicle eht )
{
   if ( available ( eht ) ){
      if ( gameoptions.container.emptyeverything )
         cc->refill.emptyeverything ( eht );
      dissectvehicle ( eht );
      cc_b->removevehicle ( &eht );
   }
}


/***********************************************************************************/
/*                                                                                 */
/*   class    ctransportcontrols : Geb„ude-Innereien                                */
/*                                                                                 */
/***********************************************************************************/

ctransportcontrols :: ctransportcontrols (void)
{
   cc_t = this;
   cc_b = NULL;
}

void  ctransportcontrols :: init (pvehicle eht)
{
   vehicle = eht;
};



char  ctransportcontrols :: getactplayer (void)
{
   return   vehicle->color / 8;
};



int   ctransportcontrols :: putmaterial (int m, int abbuchen )
{
   if ( vehicle->typ->material < vehicle->material + m ) {
      int dif = vehicle->typ->material - vehicle->material;
      if ( abbuchen )
         vehicle->material += dif;
      return dif;
   } else {
      if ( abbuchen )
         vehicle->material += m;
      return m;
   }
};



int   ctransportcontrols :: putfuel (int f, int abbuchen)
{
   if ( vehicle->typ->functions & cffuelref) {
      if ( vehicle->typ->tank < vehicle->fuel + f ) {
         int dif = vehicle->typ->tank - vehicle->fuel;
         if ( abbuchen )
            vehicle->fuel += dif;
         return dif;
      } else {
         if ( abbuchen )
            vehicle->fuel += f;
         return f;
      }
   } else
      return 0;

};



int   ctransportcontrols :: putenergy (int e, int abbuchen )
{
   return 0;
};




int   ctransportcontrols :: putammunition ( int weapontype, int ammunition, int abbuchen)
{
   int ammo = ammunition;
   for ( int i = 0; i < vehicle->typ->weapons->count; i++ )
      if ( ammo )
        if ( vehicle->typ->weapons->weapon[i].typ & ( 1 << weapontype ) ) {
          int dif = vehicle->typ->weapons->weapon[i].count - vehicle->ammo[i];
          if ( dif > ammo )
             dif = ammo;
          if ( abbuchen )
             vehicle->ammo[i] += dif;
          ammo -= dif;
        }
   return ammo;      
};



int   ctransportcontrols :: getenergy ( int need, int abbuchen )
{
   if ( need < vehicle->energy ) {
      if ( abbuchen )
         vehicle->energy-=need;
      return need;
   } else {
      int e = vehicle->energy;
      if ( abbuchen )
         vehicle->energy = 0;
      return e;
   }
};



int   ctransportcontrols :: getmaterial ( int need, int abbuchen )
{
   if ( need < vehicle->material ) {
      if ( abbuchen )
         vehicle->material-=need;
      return need;
   } else {
      int m = vehicle->material;
      if ( abbuchen )
         vehicle->material = 0;
      return m;
   }
};



int   ctransportcontrols :: getfuel ( int need, int abbuchen )
{
   if ( vehicle->typ->functions & cffuelref) {
      if ( need < vehicle->fuel ) {
         if ( abbuchen )
            vehicle->fuel-=need;
         return need;
      } else {
         int f = vehicle->fuel;
         if ( abbuchen )
            vehicle->fuel = 0;
         return f;
      }
   } else 
      return 0;
};



int    ctransportcontrols :: getammunition ( int weapontype, int num, int abbuchen, int produceifrequired  )
{
   int ammo = 0;
   for ( int i = 0; i < vehicle->typ->weapons->count ; i++)
      if ( ammo < num )
        if ( vehicle->typ->weapons->weapon[i].typ & ( 1 << weapontype ) ) {
          int dif = num - ammo;
          if ( dif > vehicle->ammo[i] )
             dif = vehicle->ammo[i];

          if ( abbuchen )
             vehicle->ammo[i] -= dif;
          ammo += dif;
        }
   return ammo;      
};


int    ctransportcontrols :: ammotypeavail ( int type )
{
   for ( int i = 0; i < vehicle->typ->weapons->count ; i++)
      if ( vehicle->typ->weapons->weapon[i].typ & ( 1 << type ) ) 
         return 1;
   return 0;
}



int   ctransportcontrols :: getxpos (void)
{
   return   vehicle->xpos;
};



int   ctransportcontrols :: getypos (void)
{
   return   vehicle->ypos;
};



int   ctransportcontrols :: getspecfunc ( tcontainermode mode )
{
   if ( mode == mtransport )
      return vehicle->functions;
   else
      return 0;
};


int   ctransportcontrols :: moveavail ( pvehicle eht )
{
   if ( recursiondepth > 0 )
      return 0;
   if ( eht->movement < minmalq )
      return 0;

   if ( vehicle->height <= chgetaucht )
      if ( eht->functions & cf_trooper )
         return 2;
      else
         return 0;

   if ( (vehicle->height < chtieffliegend) ) {

      if ((eht->typ->height & vehicle->typ->loadcapabilityreq ) &&
         ((eht->typ->height & vehicle->typ->loadcapabilitynot ) == 0 ) &&
         ((eht->typ->steigung <= flugzeugtraegerrunwayverkuerzung ) || eht->height <= chfahrend ))

         if ( eht->height  & vehicle->typ->loadcapability )
            return 2;
         else
            return 1;
      else
         if ( eht->functions & cf_trooper )
            return 2;
         else
            return 0;
   } else      
   if ( (vehicle->height <= chfliegend) && ( eht->functions & cfparatrooper)) {
      if ((eht->typ->height & vehicle->typ->loadcapabilityreq ) &&
         ((eht->typ->height & vehicle->typ->loadcapabilitynot ) == 0 ))
         return 3;
      else
         return 0;
   } else
     if ( vehicle->height == eht->height  && ( eht->height & eht->typ->height) )  // to be sure...
        return 2;
     else
        return 0;
}


int   ctransportcontrols :: movement (  pvehicle eht, int mode )
{
   if ( eht->movement < minmalq )
      return 0;

   movementparams.height   = eht->height;
   movementparams.movement = eht->movement;
   movementparams.attacked = eht->attacked;

   int unitheight = -1;
   if ( vehicle->height <= chgetaucht )
      unitheight = vehicle->height;

   moveparams.movestatus = 0;
   int ma = moveavail( eht );
   if ( ma == 3 ) 
      eht->attacked = 1;
   if (  ma >= 2 )
      ::movement( eht, unitheight );
   else {
      if ( ma == 1 ) {
         if ( ( eht->height << 1 ) & vehicle->typ->loadcapability ) {
             eht->height <<=1;
             if ( eht->typ->steigung )
                eht->movement = eht->typ->steigung * maxmalq;
              else
                eht->movement = maxmalq * 3 / 2;

             ::movement( eht, unitheight );
             if ( moveparams.movestatus == 0 ) {
                eht->height   = movementparams.height;
                eht->movement = movementparams.movement;
                eht->attacked = movementparams.attacked;
             }
         }

      } else 
          return 0;

   }

   return moveparams.movestatus !=0;
}


void  ctransportcontrols :: removevehicle ( pvehicle *peht )
{
   for (int i=0; i<=31; i++) {
       if ( *peht == vehicle->loading[i] ) 
          vehicle->loading[i]=NULL;
   };
   logtoreplayinfo ( rpl_removeunit, vehicle->xpos, vehicle->ypos, (*peht)->networkid );
   ::removevehicle ( peht );
}




int   ctransportcontrols :: crepairunitintransport :: checkto (pvehicle eht, char newdamage)
{
   return ccontainercontrols :: crepairanything :: checkto ( eht->damage, newdamage, 0, eht->typ->production.material, eht->typ->production.energy, repairefficiency_unit );
};










/***********************************************************************************/
/*                                                                                 */
/*   class    ccontainer : Geb„ude- / Transporter-Innereien                        */
/*                                                                                 */
/***********************************************************************************/

#define  eoh   55

ccontainer :: ccontainer (void)
{
    mousestat = 1;
    keymode = 0;

    pushallmouseprocs ( );

    // removemouseproc ( (void*) mousescrollproc );
    // npush ( mouseproc );
    // mouseproc = NULL;

    if ( mouseparams.pictpointer != icons.mousepointer ) 
       setnewmousepointer ( icons.mousepointer, 0,0 );


    allsubwindownum = 0;
    mark.x=0;
    mark.y=0;
    repaintresources = 1;
    repaintammo = 0;
    memset ( &picture[0], 0, sizeof ( picture ));
    memset ( &pictgray[0], 0, sizeof ( pictgray ));
    actsubwindow = NULL;
    inactivefield = icons.container.mark.inactive;
    activefield   = icons.container.mark.active;

    containeronlinemousehelp = new tcontaineronlinemousehelp ( this );

};





void   ccontainer :: registersubwindow ( psubwindow subwin )
{
   allsubwindows[ allsubwindownum ] = subwin;
   allsubwindownum ++;
}


ccontainer :: ~ccontainer (void)
{
    delete containeronlinemousehelp;

    allsubwindownum = 0;
    // addmouseproc ( (void*) mousescrollproc );

    popallmouseprocs ( );



   //     npop ( mouseproc );
};

void  ccontainer :: buildgraphics( void )
{
    int mss = getmousestatus();

   #ifdef HEXAGON
    putspriteimage ( containerxpos, containerypos, icons.container.container_window );
   #else
    try {
       tnfilestream stream ( "bkgr_con.pcx", 1 );
       loadpcxxy ( &stream, 0, 0);
    }
    catch ( tfileerror ) {
    } /* endcatch */
   #endif

    
    activefontsettings.color = 15; 
    activefontsettings.background = 201;
    activefontsettings.length = 80;
    activefontsettings.justify = centertext;
    activefontsettings.font = schriften.guifont;

    if ( name1 )
       showtext2c (name1, nameposx, nameposy );

    if ( name2 )
       showtext2c (name2, nameposx+112, nameposy );

    repaintresources = 1;
    showresources ();
    showammo();

    for (int i = 0; i < 3; i++) 
        putrotspriteimage( tabmarkposx, tabmarkpos[i], icons.container.tabmark[i == keymode], actmap->actplayer*8 );

    if ( actsubwindow )  {
       actsubwindow->display();
       actsubwindow->paintalllaschen( 1 );
    }

}


void  ccontainer :: init (void *pict, int col, char *name, char *descr)
{
   containerpicture = pict;
   name1 = name;
   name2 = descr;
   buildgraphics();
}

int ammoorderxlat[8] = { 2, 3, 4, 5, 6, 7, 0, 1 };

void  ccontainer :: showammo ( void )
{
   activefontsettings.background = 201;
   activefontsettings.length = 19;
   activefontsettings.justify = righttext;
   activefontsettings.font = schriften.guifont;
   for (int i = 0; i < 8; i++) 
      showtext2c ( strrr ( getammunition ( ammoorderxlat[i], maxint, 0 )), nameposx - 12 + i * 28, nameposy + 59 );

}

void  ccontainer :: showresources ( void )
{
   if ( repaintresources ) {
    activefontsettings.color = 15; 
    activefontsettings.background = 201;
    activefontsettings.length = 29;
    activefontsettings.justify = righttext;
    activefontsettings.font = schriften.guifont;

    showtext2c ( strrr ( getenergy   ( maxint, 0 ) ), nameposx + 18, nameposy + 27 );
    showtext2c ( strrr ( getmaterial ( maxint, 0 ) ), nameposx + 91, nameposy + 27 );
    showtext2c ( strrr ( getfuel     ( maxint, 0 ) ), nameposx + 164, nameposy + 27 );

    repaintresources = 0;
   }
}

int    ccontainer :: getfieldundermouse ( int* x, int* y )
{
      for ( int i = 0; i < unitsshowny; i++ )
         for ( int j = 0; j < unitsshownx; j++ ) {
            int xp = unitposx[j];
            int yp = unitposy[i];
            if ( mouseparams.x >= xp && mouseparams.x < xp+fieldxsize && mouseparams.y >= yp && mouseparams.y < yp + fieldysize) {
               int k=0;
   
               #ifdef HEXAGON
                int xd = mouseparams.x - xp;
                int yd = mouseparams.y - yp;
                short unsigned int* pw = (word*) icons.fieldshape;
                unsigned char* pc = (unsigned char*) icons.fieldshape;
                pc+=4;
               
                if ( pw[0] >= xd  && pw[1] >= yd )
                   if ( pc[ yd * ( pw[0] + 1) + xd] != 255 )
                      k++;
               
               #else
                 int y;
                 for (y=0; y<10 ;y++ ) 
                    if (mouseparams.x >  xp + 10 - y   &&   mouseparams.x <= xp + 20 + y   &&   mouseparams.y == yp + y)
                       k++;
     
                 for (y=10;y<20 ;y++ ) 
                    if (mouseparams.x >= xp  &&   mouseparams.x <= xp + 30   &&   mouseparams.y == yp + y)
                       k++;
     
                 for (y=10; y>0 ;y-- ) 
                    if (mouseparams.x >  xp + 10 - y   &&   mouseparams.x <= xp + 20 + y   &&   mouseparams.y == yp + 30 - y)
                       k++;
               #endif
                 if ( k ) {
                    *x = j;
                    *y = i;
                    return 1;
                 }
            }
         }
   return 0;
}

void   ccontainer :: checkformouse( void )
{
   if ( gameoptions.mouse.fieldmarkbutton )
      if ( mouseparams.taste == gameoptions.mouse.fieldmarkbutton ) {
         int i,j;
         if ( getfieldundermouse ( &i, &j ))
            if ( i != mark.y  ||  j != mark.x ) {
               setinvisiblemouserectanglestk ( unitposx[mark.x], unitposy[mark.y], unitposx[mark.x+1], unitposy[mark.y+1] );
               putspriteimage ( unitposx[mark.x], unitposy[mark.y], inactivefield);
               displayloading ( mark.x, mark.y );
               getinvisiblemouserectanglestk ();
               mark.x = j;
               mark.y = i;
               setinvisiblemouserectanglestk ( unitposx[mark.x], unitposy[mark.y], unitposx[mark.x+1], unitposy[mark.y+1] );
               putspriteimage ( unitposx[mark.x], unitposy[mark.y], activefield);
               displayloading ( mark.x, mark.y, 0, 1 );
               getinvisiblemouserectanglestk ();
               mousestat = 1;
            }
   
        dashboard.checkformouse();
      }

   if ( gameoptions.mouse.smallguibutton )
      if ( mouseparams.taste == gameoptions.mouse.smallguibutton ) {
         int x; 
         int y;
         int r = getfieldundermouse ( &x, &y );
   
         if ( r ) 
            if ( (mark.x != x || mark.y != y) ) { // && ( moveparams.movestatus == 0   ||  getfield(actmap->xpos + x , actmap->ypos + y)->temp == 0) ) {
               setinvisiblemouserectanglestk ( unitposx[mark.x], unitposy[mark.y], unitposx[mark.x+1], unitposy[mark.y+1] );
               putspriteimage ( unitposx[mark.x], unitposy[mark.y], inactivefield);
               displayloading ( mark.x, mark.y );
               getinvisiblemouserectanglestk ();
               mark.x = x;
               mark.y = y;
               setinvisiblemouserectanglestk ( unitposx[mark.x], unitposy[mark.y], unitposx[mark.x+1], unitposy[mark.y+1] );
               putspriteimage ( unitposx[mark.x], unitposy[mark.y], activefield);
               displayloading ( mark.x, mark.y, 0, 1 );
               getinvisiblemouserectanglestk ();
               mousestat = 1;
            } else 
              if ( mousestat == 2 ) { //  ||  mousestat == 0 ||  (moveparams.movestatus && getfield( actmap->xpos + x, actmap->ypos + y)->temp )  ) {
                 if ( mark.x == x && mark.y == y ) {
                    int num = actgui->painticons();
                    actgui->paintsmallicons( gameoptions.mouse.smallguibutton, num <= 1 );
                 }
   
                 mousestat = 1;
              }
      } else 
        if ( mousestat == 1 )
           mousestat = 2;


}

void ccontainer :: unitchanged( void )
{
}

void  ccontainer :: paintvehicleinfo ( void )
{
   dashboard.paintvehicleinfo ( getmarkedunit(), NULL, NULL );
}

void  ccontainer :: run ()
{     
   end = 0;
   dashboard.x = 0xffff;
   mousevisible(true);
   do {
      if (keypress () ) {
         tkey input=r_key();
         containeronlinemousehelp->removehelp ();

         if ( input == ct_tab ) {
            setinvisiblemouserectanglestk ( tabmarkposx, tabmarkpos[keymode], tabmarkposx + 10, tabmarkpos[keymode] + 10 );
            putrotspriteimage( tabmarkposx, tabmarkpos[keymode], icons.container.tabmark[0], actmap->actplayer*8 );
            getinvisiblemouserectanglestk ();

            keymode++;
            if ( keymode >= 3 )
               keymode = 0;

            setinvisiblemouserectanglestk ( tabmarkposx, tabmarkpos[keymode], tabmarkposx + 10, tabmarkpos[keymode] + 10 );
            putrotspriteimage( tabmarkposx, tabmarkpos[keymode], icons.container.tabmark[1], actmap->actplayer*8 );
            getinvisiblemouserectanglestk ();
         }

         if ( keymode == 2 )
            if ( actsubwindow ) {
               actsubwindow->checklaschkey( input );
            }

         if ( actsubwindow && keymode == 1)
            if ( actsubwindow )
               actsubwindow-> checkforkey ( input );

 /*        if ( input == ct_esc ) 
            if ( unitmode == mnormal )
               end=1; */

         if ( keymode == 0 ) {
            actgui->checkforkey (input);
         
            if ( input==ct_up || input==ct_8k)    movemark(up);
            if ( input==ct_down || input==ct_2k)  movemark(down);
            if ( input==ct_left || input==ct_4k)  movemark(left);
            if ( input==ct_right || input==ct_6k) movemark(right);

         }
         if ( input == ct_0 )  {
            char* nm = getnextfilenumname ( "screen", "pcx", 0 );
            writepcx ( nm, 0, 0, agmp->resolutionx-1, agmp->resolutiony-1, pal );
            displaymessage2( "screen saved to %s", nm );
         }

      }

      if ( repaintresources ) {
         showresources();
         if ( actsubwindow )
            actsubwindow-> resetresources( 0 );

         repaintresources = 0;
      }

      if ( repaintammo ) {
         showammo ();
         if ( actsubwindow )
            actsubwindow-> resetammo ( 0 );

         repaintammo = 0;
      }


      actgui->checkformouse ();
      if ( actsubwindow )
         actsubwindow-> mousecheck ( 0 );

      checkformouse();

      if ((dashboard.x != mark.x ) || ( dashboard.y != mark.y )) {
         paintvehicleinfo ( );
         dashboard.x = mark.x ;
         dashboard.y = mark.y ;
         actgui->painticons();
         if ( actsubwindow )
            actsubwindow->unitchanged();
         unitchanged();
      } 

      containeronlinemousehelp->checkforhelp();

   } while (end == 0); 
}

                                                                 
void  ccontainer :: done ()
{
   if ( recursiondepth == 0 ) 
      displaymap ();

   actgui->restorebackground();
}




void  ccontainer :: movemark (int direction)
{
    if (direction != repaint  &&  direction != repaintall ) {
        setinvisiblemouserectanglestk ( unitposx[mark.x], unitposy[mark.y], unitposx[mark.x+1], unitposy[mark.y+1] );

        putspriteimage ( unitposx[mark.x], unitposy[mark.y], inactivefield);
        displayloading ( mark.x, mark.y );

        getinvisiblemouserectanglestk ();
    } else {
        if ( direction == repaintall ) {
           setpictures();

           setinvisiblemouserectanglestk ( unitposx[0], unitposy[0], unitposx[unitsshownx-1], unitposy[unitsshowny-1] );
           for ( int y = 0; y < unitsshowny; y++ )
              for ( int x = 0; x < unitsshownx; x++ )
                putspriteimage (unitposx [x], unitposy[y], inactivefield);
           displayloading();
           getinvisiblemouserectanglestk ( );
        }
    }
       

    switch (direction) {
        case up :
            mark.y--;
            if (mark.y < 0) mark.y=unitsshowny-1;
        break;    
        case down :
            mark.y++;
            if (mark.y >= unitsshowny ) mark.y=0;
        break;    
        case left :
            mark.x--;
            if (mark.x < 0) mark.x=unitsshownx-1;
        break;    
        case right :
            mark.x++;
            if (mark.x >= unitsshownx ) mark.x=0;
        break;
    };

    setinvisiblemouserectanglestk ( unitposx[mark.x], unitposy[mark.y], unitposx[mark.x+1], unitposy[mark.y+1] );

    putspriteimage (unitposx[mark.x], unitposy[mark.y], activefield);
    displayloading ( mark.x, mark.y, 0, 1 );

    getinvisiblemouserectanglestk ( );

    actgui->painticons();
            
};
        
pvehicletype ccontainer :: getmarkedunittype ( void )
{
   return NULL;
}

void  ccontainer :: setpictures ( void )
{
   if ( unitmode == mnormal ) {
      inactivefield = icons.container.mark.inactive;
      activefield   = icons.container.mark.active;
   } else 
   if ( unitmode == mloadintocontainer ) {
      inactivefield = icons.container.mark.movein_inactive;
      activefield   = icons.container.mark.movein_active;
   }

   for ( int i = 0; i < 32; i++ ) {
      pvehicle unit = getloadedunit ( i );
      if ( unit ) {
         picture[i] = unit->typ->picture[0] ;
         if ( unit->movement >= minmalq )
            pictgray[i] = 0;
         else
            pictgray[i] = 1;
      } else
         picture[i] = NULL;
   }
}


void  ccontainer :: displayloading ( int x, int y, int dx, int dy )
{
    void* pict = picture [ x+y*unitsshownx ];
    if ( pict ) {
        if ( pictgray[ x+y*unitsshownx ] )
           putspriteimage (unitposx[x] - dx, unitposy[y] - dy, xlatpict(xlatpictgraytable, pict )  );
        else
           putrotspriteimage (unitposx[x] - dx, unitposy[y] - dy, pict, getactplayer()*8 );
    }
}

void  ccontainer :: displayloading (void)
{
    for (int x=0; x < unitsshownx; x++) 
        for (int y=0; y < unitsshowny ; y++) 
           displayloading ( x, y );
}



void    ccontainer ::  setactunittogray ( void )
{
   pictgray[mark.x + mark.y * unitsshownx] = 1;
   movemark ( repaint );
}




// -------------------------------- ----------------------------------------------------------



ccontainer :: cammunitiontransfer_subwindow :: cammunitiontransfer_subwindow ( void )
{
   strcpy ( name, "ammunition transfer" );
   laschpic1 = icons.container.lasche.a.ammotransfer[0];
   laschpic2 = icons.container.lasche.a.ammotransfer[1];
   eht = NULL;
   txtptr = "external";
   num = 0;

   for (int i = 0; i < waffenanzahl; i++) {
      objcoordinates[i].x1 = subwinx1 + 48  + 40 * i ;
      objcoordinates[i].x2 = subwinx1 + 58  + 40 * i ;
      objcoordinates[i].y1 = subwiny1 + 41;
      objcoordinates[i].y2 = subwiny1 + 90;
      objcoordinates[i].type = 3;
      objcoordinates[i].t1 = 0;
      objcoordinates[i].t2 = 8;
   } /* endfor */

   objcoordinates[9].x1 = subwinx1 + 217;
   objcoordinates[9].x2 = subwinx1 + 303;
   objcoordinates[9].y1 = subwiny1 + 4;
   objcoordinates[9].y2 = subwiny1 + 14;
   objcoordinates[9].type = 5;
   
   memset ( &actdisp, 0, sizeof ( actdisp ));
   actschieber = 0 ;
   externalloadingactive = 0;
}

int  ccontainer :: cammunitiontransfer_subwindow :: subwin_available ( void )
{
   csubwindow :: subwin_available ( );
   if ( next )
     next->subwin_available ();

   return 0;
}

int  ccontainer :: cammunitiontransfer_subwindow :: externalloadavailable ( void )
{
   return 0;
}

void  ccontainer :: cammunitiontransfer_subwindow :: paintobj ( int numm, int stat )
{
   csubwindow :: paintobj ( numm, stat );

   if ( objcoordinates[numm].type == 3 ) {
      setinvisiblemouserectanglestk ( objcoordinates[numm].x1,   objcoordinates[numm].y1,   objcoordinates[numm].x2+10,   objcoordinates[numm].y2 );
      if ( numm < num ) {
         putimage ( objcoordinates[numm].x1-1,   objcoordinates[numm].y1-1,  icons.container.subwin.ammotransfer.schiene );
         actdisp[numm] = 2;
   
         int offs = 0;
         if ( numm == actschieber )
            offs = 1;
   
         if ( objcoordinates[numm].y2 - objcoordinates[numm].t1 - objcoordinates[numm].t2 == objcoordinates[numm].y1 )
            putimage ( objcoordinates[numm].x1,   objcoordinates[numm].y2 - objcoordinates[numm].t1 - objcoordinates[numm].t2,  icons.container.subwin.ammotransfer.schieber[offs + 2] );
         else
            putimage ( objcoordinates[numm].x1,   objcoordinates[numm].y2 - objcoordinates[numm].t1 - objcoordinates[numm].t2,  icons.container.subwin.ammotransfer.schieber[offs] );
   
         activefontsettings.color = white;
         activefontsettings.font = schriften.guifont;
         activefontsettings.length = 29;
         activefontsettings.justify = centertext;
         activefontsettings.background = bkgrcol;
         showtext2c ( strrr ( weaps[numm].actnum  ), subwinx1 + 31 + numm * 40, subwiny1 + 25 );
         showtext2c ( strrr ( weaps[numm].buildnum + weaps[numm].orgnum - weaps[numm].actnum  ), subwinx1 + 31 + numm * 40, subwiny1 + 96 );
      } else {
         putimage ( objcoordinates[numm].x1-1,   objcoordinates[numm].y1-1,  icons.container.subwin.ammotransfer.schieneinactive );
         actdisp[numm] = 1;
         bar ( subwinx1 + 31 + numm * 40, subwiny1 + 25, subwinx1 + 61 + numm * 40, subwiny1 + 35, bkgrcol );
         bar ( subwinx1 + 31 + numm * 40, subwiny1 + 96, subwinx1 + 61 + numm * 40, subwiny1 +106, bkgrcol );
         bar ( subwinx1 + 31 + numm * 40, subwiny1 + 38, subwinx1 + 45 + numm * 40, subwiny1 + 93, bkgrcol );
      }
      getinvisiblemouserectanglestk ( ); 
   }
   if ( objcoordinates[numm].type == 5  && externalloadavailable() ) {
      setinvisiblemouserectanglestk ( objcoordinates[numm].x1,   objcoordinates[numm].y1,   objcoordinates[numm].x2+10,   objcoordinates[numm].y2 );
      if ( externalloadingactive ) 
         activefontsettings.font = schriften.guicolfont;
      else
         activefontsettings.font = schriften.guifont;
      activefontsettings.background = 255;
      activefontsettings.length = 65;
      activefontsettings.height = 0;
      activefontsettings.justify = centertext;
      if ( stat == 0 ) {
         putimage ( subwinx1 + 217, subwiny1 + 4, icons.container.subwin.ammotransfer.button );
         showtext2c ( txtptr, subwinx1 + 228, subwiny1 +5 );
      } else {
         putimage ( subwinx1 + 217, subwiny1 + 4, icons.container.subwin.ammotransfer.buttonpressed );
         showtext2c ( txtptr, subwinx1 + 229, subwiny1 + 6 );
      }
      getinvisiblemouserectanglestk ( ); 

   }
}

void  ccontainer :: cammunitiontransfer_subwindow :: reset ( pvehicle veh )
{
  int i;

   num = 0;
   if ( veh )
      eht = veh;
   else {
      if ( !externalloadingactive )
         eht = hostcontainer->getmarkedunit();
   }
   if ( eht ) {
      for (i = 0; i < eht->typ->weapons->count; i++) {
         if ( eht->typ->weapons->weapon[i].typ & ( cwweapon | cwmineb ) ) {
            int typ = log2( eht->typ->weapons->weapon[i].typ & ( cwweapon | cwmineb ));
            if ( cc->ammotypeavail ( typ )) {
               weaps[num].name = cwaffentypen [ typ ];
               weaps[num].maxnum = eht->typ->weapons->weapon[i].count;
               weaps[num].orgnum = eht->ammo[i];
               weaps[num].actnum = weaps[num].orgnum;
               weaps[num].buildnum = cc->getammunition ( typ, maxint, 0 );
               weaps[num].pos = i;
               weaps[num].type = typ;
               num++;
            }
         }
      } /* endfor */
      if ( eht->typ->material ) {
            weaps[num].name = cdnames [ 1 ];
            weaps[num].maxnum = eht->typ->material;
            weaps[num].orgnum = eht->material;
            weaps[num].actnum = weaps[num].orgnum;
            weaps[num].buildnum = cc->getmaterial ( maxint, 0 );
            weaps[num].pos  = 101;
            weaps[num].type = 101;
            num++;
      }
      if ( eht->typ->tank ) {
            weaps[num].name = cdnames [ 2 ];
            weaps[num].maxnum = eht->typ->tank;
            weaps[num].orgnum = eht->fuel;
            weaps[num].actnum = weaps[num].orgnum;
            weaps[num].buildnum = cc->getfuel ( maxint, 0 );
            weaps[num].pos  = 102;
            weaps[num].type = 102;
            num++;
      }
      for ( i = 0; i < num; i++ ) {
         check(i);
         objcoordinates[i].type = 3;
      }

   }
}                                                          

int   ccontainer :: cammunitiontransfer_subwindow :: gpres ( int i )
{
   int n;
   int diff = weaps[i].actnum - weaps[i].orgnum;
   if ( weaps[i].pos < 100 ) {
      if ( diff > 0 ) 
         n = cc->getammunition ( weaps[i].type, diff, 0 );
      else 
         n = -cc->putammunition ( weaps[i].type, -diff, 0 );
   } else
      if ( weaps[i].pos == 101 ) {
         if ( diff > 0 )
            n = cc->getmaterial ( diff, 0 );
         else
            n = -cc->putmaterial ( -diff, 0 );
      } else
         if ( weaps[i].pos == 102 ) {
            if ( diff > 0 ) 
               n = cc->getfuel ( diff, 0 );
            else 
               n = -cc->putfuel ( -diff, 0 );
      
         };

   return n;
}

void  ccontainer :: cammunitiontransfer_subwindow :: check ( int i )
{
    weaps[i].actnum = weaps[i].orgnum + gpres(i);
    int length = objcoordinates[i].y2 - objcoordinates[i].y1 - objcoordinates[i].t2;
    objcoordinates[i].t1 = length * weaps[i].actnum / weaps[i].maxnum;
}


void  ccontainer :: cammunitiontransfer_subwindow :: display ( void )
{
   if ( hostcontainer->getmarkedunit() != eht )
      reset();

   setinvisiblemouserectanglestk ( subwinx1, subwiny1, subwinx2, subwiny2 );
   npush ( activefontsettings );
   putimage ( subwinx1, subwiny1, icons.container.subwin.ammotransfer.main );
   for ( int i = 0; i < 8; i++ )
      actdisp[i] = 0;

   csubwindow :: display ();

   displayvariables();

   paintobj ( 9, 0 );


   npop ( activefontsettings );
   getinvisiblemouserectanglestk (  );

}

void  ccontainer :: cammunitiontransfer_subwindow :: displayvariables ( void )
{
   activefontsettings.color = white;
   activefontsettings.font = schriften.guifont;
   activefontsettings.justify = lefttext;
   for (int i = 0; i < 8; i++) {

      if ( i < num ) {
         paintobj ( i, 0 );
         npush ( *agmp );
      
         agmp-> linearaddress = (int) new char [ 10000 ];
      
         agmp-> scanlinelength = 100;
         agmp-> windowstatus = 100;
      
         int x1 = 0;
         int y1 = 0;
         int x2 = 55;
         int y2 = 14;
         char* buf = new char [ imagesize ( x1, y1, x2, y2 ) ];
      
         memset ( (void*) agmp-> linearaddress, bkgrcol, 10000 );
         activefontsettings.length = 0;
         activefontsettings.justify = lefttext;
         activefontsettings.background = 255;
         showtext2c ( weaps[i].name, 10, 10 );
         getimage ( 10, 10, 10 + x2, 10 + y2, buf );
         npush ( *agmp );
         *agmp = *hgmp;
         putrotspriteimage90 ( subwinx1 + 31 + 40 * i , subwiny1 + 38, buf, 0 );
         npop ( *agmp );
      
         delete buf;
         delete  ( (void*)agmp-> linearaddress );
         npop  ( *agmp );
      } else
        if ( actdisp[i] != 1 )
           paintobj ( i, 0 );

        
   }

}

void  ccontainer :: cammunitiontransfer_subwindow :: transfer ( void )
{
   for ( int i = 0; i < num; i++ ) {
      if ( weaps[i].pos < 100 )
         ammunition ( eht, weaps[i].pos, weaps[i].actnum );

      if ( weaps[i].pos == 101 )
         material ( eht, weaps[i].actnum );

      if ( weaps[i].pos == 102 )
         fuel ( eht, weaps[i].actnum );
   }
   dashboard.x = 0xffff;
}

void  ccontainer :: cammunitiontransfer_subwindow :: unitchanged ( void )
{
   if ( !externalloadingactive ) {
      if ( hostcontainer->getmarkedunit() != eht )
         reset();
      displayvariables();
   }
}


void  ccontainer :: cammunitiontransfer_subwindow :: execexternalload ( void )
{
}

void  ccontainer :: cammunitiontransfer_subwindow :: checkformouse ( void )
{
   if ( mouseparams.taste == 1 ) {
      for (int i = 0; i < num; i++) {
         if ( mouseparams.x >= objcoordinates[i].x1    && mouseparams.x <= objcoordinates[i].x2  &&
              mouseparams.y >= objcoordinates[i].y1    && mouseparams.y <= objcoordinates[i].y2 ) {

            int repnt = 0;
            int tp = weaps[i].actnum;
            if ( i != actschieber ) {
               int old = actschieber;
               actschieber = i;
               paintobj ( old, 0 );
               repnt = 1;
            }

            int relpos = objcoordinates[i].y2 -  mouseparams.y  /* - objcoordinates[i].t2 / 2 */ ;
            if ( relpos < 0 )
               relpos = 0;

            int maxlen = ( objcoordinates[i].y2 - objcoordinates[i].y1 - objcoordinates[i].t2 + 1 );
            if ( relpos > maxlen )
               relpos = maxlen;

            if ( relpos != objcoordinates[i].t1 ) {
               int n = relpos * weaps[i].maxnum / maxlen;
               int oldnum = weaps[i].actnum;      
               if ( n != oldnum ) {
                  weaps[i].actnum = n;
                  check ( i );
                  if ( weaps[i].actnum != oldnum ) {
                     paintobj ( i, 0 );
                     transfer();
                  }
               }

            }

            if (  tp == weaps[i].actnum && repnt )
               paintobj ( i, 0 );


         }
      } /* endfor */

      if ( externalloadavailable () && objpressedbymouse ( 9 ) ) 
         execexternalload();
      

   }
}

void  ccontainer :: cammunitiontransfer_subwindow :: checkforkey ( tkey taste )
{
  if ( num ) {
     if ( taste == ct_right  || taste==ct_6k ) {
        int olds = actschieber;
        if ( actschieber+1 < num )
           actschieber++;
        else
           actschieber = 0;
   
        paintobj ( olds, 0 );
        paintobj ( actschieber, 0 );
     }
     if ( taste == ct_left || taste == ct_4k ) {
        int olds = actschieber;
        if ( actschieber > 0 )
           actschieber--;
        else
           actschieber = num-1;
   
        paintobj ( olds, 0 );
        paintobj ( actschieber, 0 );
     }
   
     if ( taste == ct_up  ||  taste == ct_down || taste==ct_8k  || taste==ct_2k ) {
         int st = getstepwidth ( weaps[actschieber].maxnum );
         int n ;
         if ( taste == ct_up  || taste==ct_8k ) {
            if ( weaps[actschieber].actnum + st <= weaps[actschieber].maxnum  ) 
               n = weaps[actschieber].actnum + st;
            else 
               n = weaps[actschieber].maxnum;
         }
         if ( taste == ct_down  || taste==ct_2k ) {
            if ( weaps[actschieber].actnum > st   ) 
               n = weaps[actschieber].actnum - st;
            else 
               n = 0;
         }
         int oldnum = weaps[actschieber].actnum;
         weaps[actschieber].actnum = n;
         check ( actschieber );
         if ( weaps[actschieber].actnum != oldnum ) {
            paintobj ( actschieber, 0 );
            transfer();
         }
   
     }
/*   
     if ( taste == ct_enter || taste == ct_enterk ) { 
          transfer();
          reset();
          for ( int i = 0; i < num; i++ ) 
             paintobj ( i, 0 );
     }
*/
  }
}

void ccontainer :: cammunitiontransfer_subwindow :: resetammo ( int mode )
{
   if ( mode == 0 )
      (*firstavailable)->resetammo(1);
   else {
      reset ();
      if ( hostcontainer->actsubwindow == this )
         displayvariables();

      if ( nextavailable )
         nextavailable->resetammo ( mode );
   }
}

void ccontainer :: cammunitiontransfer_subwindow :: resetresources ( int mode )
{
   if ( mode == 0 )
      (*firstavailable)->resetresources(1);
   else {
      reset ();
      if ( hostcontainer->actsubwindow == this )
         displayvariables();

      if ( nextavailable )
         nextavailable->resetresources ( mode );
   }
}




int ccontainer_b :: cammunitiontransferb_subwindow :: externalloadavailable ( void )
{
   return cc_b->building->typ->special & cgexternalloadingb;
}


void ccontainer_b :: cammunitiontransferb_subwindow :: execexternalload ( void )
{
  if ( !externalloadingactive ) {
     int f = searchexternaltransferfields ( cc_b->building );
     if ( f ) {
        int ms = getmousestatus();
        if (ms == 2) 
           mousevisible(false); 
 
        actgui->restorebackground();
        npush (actgui);
        actgui = hostcontainer->oldguihost;
 
        int cursorx = cursor.posx;
        int cursory = cursor.posy;
 
        displaymap ();
        dashboard.x = 0xffff;
        mousevisible( true );
        addmouseproc ( &mousescrollproc );
 
        do {
    
            tkey input;
            if (keypress ()) {
                mainloopgeneralkeycheck (input);
            }
    
            mainloopgeneralmousecheck ();
    
        } while ( moveparams.movestatus==130 ) ;
        removemouseproc ( &mousescrollproc );
        if ( mouseparams.pictpointer != icons.mousepointer ) 
           setnewmousepointer ( icons.mousepointer, 0,0 );
 
        mousevisible ( false );
        pvehicle markedvehicle = NULL;
        if ( moveparams.movestatus == 131 ) {
           markedvehicle = getactfield()->vehicle ;
           externalloadingactive = 1;
        }
        moveparams.movestatus = 0;

        cursor.hide ();
        cursor.posx = cursorx;
        cursor.posy = cursory;
        cursor.show ();
        cleartemps(7);
        hostcontainer->buildgraphics();
        hostcontainer->displayloading ();
        hostcontainer->movemark (repaint);
        actgui->restorebackground();
        npop (actgui);
        actgui->restorebackground();
        dashboard.x = 0xffff;
 
        if (ms == 2) 
           mousevisible(true); 

        reset ( markedvehicle );
        displayvariables();
        cleartemps ( 7 );
     } else
        dispmessage2 ( 401, NULL );
   } else {
      externalloadingactive = 0;
      paintobj ( 9, 0 );
      unitchanged();
   }
}



void  ccontainer :: hosticons_c :: seticonmains ( pcontainer maintemp )
{
    pgeneralicon_c t = (pgeneralicon_c) getfirsticon ();
    t->setmain ( maintemp );
}    








ccontainer :: moveicon_c :: moveicon_c ( void )
{
   strcpy ( filename, "movement" );
};

int   ccontainer :: moveicon_c :: available    ( void ) 
{
   if ( main->unitmode != mnormal )
      return 0;

    pvehicle eht = main->getmarkedunit(); 

    if ( eht && eht->color == actmap->actplayer * 8 ) 
       return main->moveavail ( eht );

    return 0;
}

void  ccontainer :: moveicon_c :: exec         ( void ) 
{
    int stat = main->movement ( main->getmarkedunit() ); 

    if ( stat ) {
       int ms = getmousestatus();
       if (ms == 2) 
          mousevisible(false); 

       actgui->restorebackground();
       npush (actgui);
       actgui = main->oldguihost;

       int cursorx = cursor.posx;
       int cursory = cursor.posy;

       displaymap ();
       dashboard.x = 0xffff;
       mousevisible( true );
       addmouseproc ( &mousescrollproc );

       do {
   
           tkey input;
           if (keypress ()) {
               mainloopgeneralkeycheck (input);
           }
   
           mainloopgeneralmousecheck ();
   
       } while ( moveparams.movestatus) ;
       removemouseproc ( &mousescrollproc );
       if ( mouseparams.pictpointer != icons.mousepointer ) 
          setnewmousepointer ( icons.mousepointer, 0,0 );

       mousevisible ( false );
       cursor.hide ();
       cursor.posx = cursorx;
       cursor.posy = cursory;
       cursor.show ();
       cleartemps(7);
       main->buildgraphics();
       main->displayloading ();
       main->movemark (repaint);
       npop (actgui);
       actgui->restorebackground();
       dashboard.x = 0xffff;
       pvehicle eht = main->getmarkedunit();
       if ( eht ) {
           eht->height   = main->movementparams.height;
           eht->movement = main->movementparams.movement;
           eht->attacked = main->movementparams.attacked;
       }

       if (ms == 2) 
          mousevisible(true); 
    }
}



ccontainer :: repairicon_c :: repairicon_c ( void )
{
   strcpy ( filename, "repair" );
};

int   ccontainer :: repairicon_c :: available    ( void ) 
{
   if ( main->unitmode != mnormal )
      return 0;

    pvehicle eht = main->getmarkedunit(); 
    if ( eht && eht->color == actmap->actplayer * 8) 
        if ( eht->damage > 0 ) 
           if ( ccontainercontrols :: crepairunit :: available ( eht ) )
              return 1; 

    return 0;
}

void  ccontainer :: repairicon_c :: exec         ( void ) 
{
  repairto ( main->getmarkedunit() , 0 );
  dashboard.x = 0xffff;
}

char* ccontainer :: repairicon_c :: getinfotext  ( void )
{
   checkto ( main->getmarkedunit() , 0 );
   strcpy ( &infotextbuf[100], infotext );
   sprintf ( &infotextbuf[100+strlen( &infotextbuf[100])], resourceusagestring, energycosts, materialcosts, fuelcosts );
   return &infotextbuf[100];
}



ccontainer :: fill_dialog_icon_c :: fill_dialog_icon_c ( void )
{
   strcpy ( filename, "refueld" );
};

int   ccontainer :: fill_dialog_icon_c :: available    ( void ) 
{
   if ( main->unitmode != mnormal )
      return 0;

   pvehicle eht = main->getmarkedunit(); 
   if ( eht && eht->color == actmap->actplayer * 8) 
      return 1;

   return 0;
}




ccontainer :: fill_icon_c :: fill_icon_c ( void )
{
   strcpy ( filename, "refuel" );
};

void  ccontainer :: fill_icon_c :: exec         ( void ) 
{
   filleverything ( main->getmarkedunit() );
   main->repaintresources = 1;
   dashboard.x = 0xffff;
}




ccontainer :: exit_icon_c :: exit_icon_c ( void )
{
   strcpy ( filename, "exit" );
};

void  ccontainer :: exit_icon_c :: exec         ( void ) 
{
   main->end = 1;
}


int   ccontainer :: exit_icon_c :: available    ( void ) 
{
   return   main->unitmode == mnormal ;
}



ccontainer :: container_icon_c :: container_icon_c ( void )
{
   strcpy ( filename, "loadinga" );
};

void  ccontainer :: container_icon_c :: exec         ( void ) 
{
   pvehicle eht = main->getmarkedunit(); 
   container ( eht, NULL );
   main->buildgraphics();
   main->displayloading ();
   main->movemark (repaint);
}


int   ccontainer :: container_icon_c :: available    ( void ) 
{
   if ( main->unitmode != mnormal )
      return 0;

   pvehicle eht = main->getmarkedunit(); 
   if ( eht && eht->color == actmap->actplayer * 8) 
      if ( eht->typ->loadcapacity > 0 )
         if ( recursiondepth +1 < maxrecursiondepth )
            return 1;

   return 0;
}




int ccontainer :: cmoveup_icon_c :: available ( void )
{
   if ( main->unitmode != mnormal )
      return 0;

   return moveupavail ( main->getmarkedunit () );
}

void  ccontainer :: cmoveup_icon_c :: exec ( void ) 
{
   moveup( main->getmarkedunit () );
              main->displayloading ();
              main->movemark (repaint);
}

ccontainer :: cmoveup_icon_c :: cmoveup_icon_c ( void )
{
   strcpy ( filename, "contnup" );
}



int ccontainer :: cmovedown_icon_c :: available ( void )
{
   if ( main->unitmode == mnormal ) {
      for ( int i = 0; i < maxloadableunits; i++ ) {
         pvehicle eht = main->getloadedunit ( i );
         if ( eht ) 
            if ( eht != main->getmarkedunit ())
               if ( movedownavail ( main->getmarkedunit () , eht ))
                  return 1;
      }
   } else if ( main->unitmode == mloadintocontainer ) {
      if ( movedownavail ( unittomove, main->getmarkedunit()))
         return 1;
   }
   return 0;
}

void  ccontainer :: cmovedown_icon_c :: exec ( void ) 
{
   if ( main->unitmode == mnormal ) {
      unittomove = main->getmarkedunit();
      dashboard.x = 0xffff;
      main->unitmode = mloadintocontainer;
      main->setpictures();
      main->movemark ( repaintall );
   } else if ( main->unitmode == mloadintocontainer ) {
      movedown( unittomove, main->getmarkedunit());
      dashboard.x = 0xffff;
      main->unitmode = mnormal;
      main->setpictures();
      main->movemark ( repaintall );
   }
}

ccontainer :: cmovedown_icon_c :: cmovedown_icon_c ( void )
{
   strcpy ( filename, "contndwn" );
}













csubwindow :: csubwindow ( void )
{
    first = &buildingparamstack[recursiondepth].csubwindow_first;
    firstavailable = &buildingparamstack[recursiondepth].csubwindow_firstavailable;

    *firstavailable = NULL;
    lastclickpos = 0;

    next = *first;
    *first = this;

    hostcontainer = NULL;
    laschcol = 22 + actmap->actplayer * 8;
    objnum = 0;
}


csubwindow :: ~csubwindow ( )
{                                                
    *first = NULL;
    *firstavailable = NULL;

}


void  csubwindow :: sethostcontainer ( pcontainer cntn )
{
   (*first)->sethostcontainerchain ( cntn );
}

void csubwindow :: sethostcontainerchain ( pcontainer cntn )
{
   hostcontainer = cntn;
   if ( next )
      next->sethostcontainerchain ( cntn );
}
   


int csubwindow :: subwin_available ( void )
{
   if ( cc->getactplayer() == actmap->actplayer ) {
      hostcontainer->registersubwindow ( this );
      nextavailable = (*firstavailable);
      (*firstavailable) = this;
   }
   return 0;
}


void csubwindow :: display ( void )
{
   activefontsettings.font = schriften.guifont;
   activefontsettings.length = 147;
   activefontsettings.justify = lefttext;
   activefontsettings.background = 201;
   showtext2c ( name, subwinx1 + 15, subwiny1 + 4 );
}


void csubwindow :: unitchanged ( void )
{
}




void csubwindow :: paintlasche ( void )
{
   /*
   setinvisiblemouserectanglestk ( laschx1, subwiny2, laschx2, subwiny2 + laschheight );

   int color = laschcol;
   if ( hostcontainer->actsubwindow == this )
      color -= 3;


   int steps = laschheight / laschstepwidth;
   if ( laschheight % laschstepwidth )
      steps++;

   for ( int i = 0; i < steps ; i++ ) {
      int laschy2 = (i+1) * laschstepwidth;
      if ( laschy2 > laschheight )
         laschy2 = laschheight;

      line ( laschx1+i, subwiny2, laschx1+i, subwiny2 + laschy2 , color );
   }

   bar ( laschx1 + steps, subwiny2, laschx2 - steps, subwiny2 + laschheight, color );
               
   for ( i = 0; i < steps ; i++ ) { 
      int laschy2 = ( steps - i ) * laschstepwidth;
      if ( laschy2 > laschheight )
         laschy2 = laschheight;

      line ( laschx2-steps+i+1, subwiny2, laschx2-steps+i+1, subwiny2 + laschy2 , color );
   }
   npush ( activefontsettings );
   activefontsettings.color = white;
   activefontsettings.length = laschx2 - laschx1 - 2*steps;
   activefontsettings.justify = centertext;
   activefontsettings.background = 255;
   activefontsettings.height = 0;
   activefontsettings.font = schriften.guifont;
   showtext2 ( name, laschx1 + steps, subwiny2 + ( laschheight - activefontsettings.font->height ) / 2 );
   npop ( activefontsettings );

   getinvisiblemouserectanglestk (  );

   */
   setinvisiblemouserectanglestk ( laschxpos + laschx1 * laschdist, laschypos, laschxpos + laschx1 * laschdist + 45, laschypos + 22 );
   if ( hostcontainer->actsubwindow == this )
      putimage ( laschxpos + laschx1 * laschdist, laschypos, laschpic1 );
   else
      putimage ( laschxpos + laschx1 * laschdist, laschypos, laschpic2 );
   getinvisiblemouserectanglestk (  );
}


void csubwindow :: paintalllaschen ( int i )
{
   if ( i == 1 )
      (*firstavailable)->paintalllaschen();
   else {
      paintlasche( );
      if ( nextavailable )
         nextavailable->paintalllaschen();
   }
}


void csubwindow :: setlaschenpos ( int x1, int maxwidth )
{
   /*
   int steps = laschheight / laschstepwidth;
   if ( laschheight % laschstepwidth )
      steps++;

   int textwidth = gettextwdth ( name, schriften.guifont ) + 2 * steps;
   if ( textwidth > maxwidth )
      textwidth = maxwidth;
   laschx1 = x1 + subwinx1;
   laschx2 = x1 + subwinx1 + textwidth;
   if ( nextavailable )
      nextavailable->setlaschenpos ( x1 + textwidth + 2, maxwidth );
      */

   laschx1 = x1;
   if ( nextavailable )
      nextavailable->setlaschenpos ( x1 + 1, maxwidth );
}


void csubwindow :: init ( void )
{
   (*first) -> subwin_available ();

   if ( hostcontainer->allsubwindownum ) {
     if ( (*firstavailable) ) {
        (*firstavailable)->setlaschenpos ( 0, (subwinx2 - subwinx1) / hostcontainer->allsubwindownum - 2 );
        (*firstavailable)->setactive();
     }
   } else {
      npush ( activefontsettings );
      activefontsettings.color = white;
      activefontsettings.font = schriften.arial8;
      activefontsettings.length = 0;
      activefontsettings.justify = lefttext;
      showtext2 ( " no subwindows available ", subwinx1 + 20, subwiny1 + 20 );
      npop ( activefontsettings );
   }   
}

void csubwindow :: checklaschkey ( tkey taste )
{
      if ( taste == ct_right  || taste==ct_6k )
         if ( nextavailable )
            nextavailable->setactive();
         else
            if ( (*firstavailable) != this )
               (*firstavailable)->setactive();

      if ( taste == ct_left || taste==ct_4k ) {
         psubwindow sw = (*firstavailable);
         if ( this != (*firstavailable) ) {
            while ( sw->nextavailable && sw->nextavailable != this )
               sw = sw->nextavailable;
   
            if ( sw->nextavailable == this ) 
              sw->setactive();

         } else {
            while ( sw->nextavailable )
               sw = sw->nextavailable;

            if ( sw != this )
              sw->setactive();
            
         }
      }
}


void  csubwindow :: mousecheck ( int checkall )
{
   if ( mouseparams.taste == 0 )
      lastclickpos = 0;

   if ( checkall ) {
      if ( mouseparams.x >= subwinx1  &&  mouseparams.x <= subwinx2  && 
           mouseparams.y >= subwiny2  &&  mouseparams.x <= subwiny2+laschheight &&
           mouseparams.taste == 1  &&  (lastclickpos == 0  ||  lastclickpos == 1)) {
               lastclickpos = 1;
               if ( mouseparams.x >= laschxpos + laschx1*laschdist  &&  mouseparams.x <= laschxpos + (laschx1+1)*laschdist ) {
                  setactive(  );
               } else
                  if ( nextavailable )
                     nextavailable->mousecheck ( checkall );
           }

   } else {
      if ( mouseparams.x >= subwinx1  &&  mouseparams.x <= subwinx2  && 
           mouseparams.y >= subwiny1  &&  mouseparams.y <= subwiny2 &&
           mouseparams.taste == 1  &&  (lastclickpos == 0  ||  lastclickpos == 2)) {
             checkformouse();
             lastclickpos = 2;
           
       }
       if ( mouseparams.y >= subwiny2  &&  mouseparams.x <= subwiny2+laschheight &&
            mouseparams.taste == 1  &&  (lastclickpos == 0  ||  lastclickpos == 1)) 
            if ( (*firstavailable) )
               (*firstavailable)->mousecheck ( 1 );
   }
}


void csubwindow :: setactive ( void )
{
   if ( hostcontainer->actsubwindow != this ) {
      //hostcontainer->actsubwindow->setinactive();
      hostcontainer->actsubwindow = this;
      display();
      if ( (*firstavailable) ) 
         (*firstavailable)->paintalllaschen();
     
   }
}


void csubwindow :: paintobj ( int num, int stat )
{
   setinvisiblemouserectanglestk ( objcoordinates[num].x1,   objcoordinates[num].y1,   objcoordinates[num].x2,   objcoordinates[num].y2 );
   if ( objcoordinates[num].type == 1 ) {
       if ( stat != 2 ) {
          rectangle ( objcoordinates[num].x1,   objcoordinates[num].y1,   objcoordinates[num].x2,   objcoordinates[num].y2,   black );
          rectangle ( objcoordinates[num].x1+1, objcoordinates[num].y1+1, objcoordinates[num].x2-1, objcoordinates[num].y2-1, black );
       } else {
          rahmen ( true, objcoordinates[num].x1,   objcoordinates[num].y1,   objcoordinates[num].x2,   objcoordinates[num].y2  );
          rahmen ( true, objcoordinates[num].x1+1, objcoordinates[num].y1+1, objcoordinates[num].x2-1, objcoordinates[num].y2-1 );
       }
       if ( stat == 0 )
          bar ( objcoordinates[num].x1+2, objcoordinates[num].y1+2, objcoordinates[num].x2-2, objcoordinates[num].y2-2, bkgrcol );
       if ( stat == 1 )
          bar ( objcoordinates[num].x1+2, objcoordinates[num].y1+2, objcoordinates[num].x2-2, objcoordinates[num].y2-2, laschcol-1 );
       if ( stat == 2 )
          bar ( objcoordinates[num].x1+2, objcoordinates[num].y1+2, objcoordinates[num].x2-2, objcoordinates[num].y2-2, laschcol-4 );
   }
   if ( objcoordinates[num].type == 2 ) {
       if ( stat == 0 )
          rectangle ( objcoordinates[num].x1,   objcoordinates[num].y1,   objcoordinates[num].x2,   objcoordinates[num].y2,   black );
       else
          rahmen ( true, objcoordinates[num].x1,   objcoordinates[num].y1,   objcoordinates[num].x2,   objcoordinates[num].y2  );
   }       
   getinvisiblemouserectanglestk ( );
}




int  csubwindow :: objpressedbymouse ( int num )
{
   if ( mouseparams.x >= objcoordinates[num].x1  &&  mouseparams.y >= objcoordinates[num].y1  &&  mouseparams.x <= objcoordinates[num].x2  &&  mouseparams.y <= objcoordinates[num].y2  &&  mouseparams.taste == 1 ) {
       paintobj ( num, 1 );
       while ( mouseparams.x >= objcoordinates[num].x1  &&
           mouseparams.y >= objcoordinates[num].y1  &&
           mouseparams.x <= objcoordinates[num].x2  &&
           mouseparams.y <= objcoordinates[num].y2  &&
           mouseparams.taste == 1 );
       paintobj ( num, 0 );
       if ( mouseparams.x >= objcoordinates[num].x1  &&  mouseparams.y >= objcoordinates[num].y1  &&  mouseparams.x <= objcoordinates[num].x2  &&  mouseparams.y <= objcoordinates[num].y2 ) 
          return 1;
   }
   return 0;
}


void csubwindow :: resetammo ( int mode )
{
   if ( mode == 0 )
      (*firstavailable)->resetammo(1);
   else
      if ( nextavailable )
         nextavailable->resetammo ( mode );
}

void csubwindow :: resetresources ( int mode )
{
   if ( mode == 0 )
      (*firstavailable)->resetresources(1);
   else
      if ( nextavailable )
         nextavailable->resetresources ( mode );
}














/***********************************************************************************/
/*                                                                                 */
/*   class    ccontainer_b : Geb„ude-Innereien                                     */
/*                                                                                 */
/***********************************************************************************/




cbuildingsubwindow :: cbuildingsubwindow ( void )
{                                                
   firstb = &buildingparamstack[recursiondepth].cbuildingsubwindow_firstb;
            
    nextb = *firstb;
    *firstb = this;

    hostcontainerb = NULL;
}

cbuildingsubwindow :: ~cbuildingsubwindow (  )
{                                                
    *firstb = NULL;
}

void cbuildingsubwindow :: sethostcontainerchain ( pcontainer_b cntn )
{
   hostcontainerb = cntn;
   if ( nextb )
      nextb->sethostcontainerchain ( cntn );
}

void  cbuildingsubwindow :: sethostcontainer ( pcontainer_b cntn )
{
   (*first)  ->sethostcontainerchain ( cntn );
   (*firstb) ->sethostcontainerchain ( cntn );
}






ccontainer_b :: ccontainer_b ( void )
{
   oldguihost = actgui;
   actgui = &hosticons_cb;
   actgui->restorebackground();
   unitmode = mnormal;
   memset ( &produceableunits, 0, sizeof ( produceableunits ));
}

void  ccontainer_b :: init ( pbuilding bld )
{
    hosticons_cb.init ( hgmp->resolutionx, hgmp->resolutiony );
    hosticons_cb.seticonmains ( this );
    hosticons_cb.starticonload();
    
   if ( bld ) {
      int mss = getmousestatus();
      if ( mss == 2 )
         mousevisible ( false );

      cbuildingcontrols :: init ( bld );

      setpictures();

      int x,y;                                           // setzen des mapcursors auf den Geb„udeeingang
      getbuildingfieldcoordinates (building, building->typ->entry.x, building->typ->entry.y, x, y);
      cursor.gotoxy ( x , y );

      ccontainer :: init ( building->getpicture ( building->typ->entry.x , building->typ->entry.y ),
                          building->color, building->name, building->typ->name);
      ccontainer :: displayloading ();
      ccontainer :: movemark (repaint);

      subwindows.repairbuilding_subwindow.sethostcontainer ( this );
      subwindows.repairbuilding_subwindow.init();
      if ( mss == 2 )
         mousevisible ( true );

   }; 
}

void ccontainer_b :: unitchanged( void )
{
   if ( unitmode == mproduction ) {
      pvehicletype fzt = getmarkedunittype();
      if ( fzt  && actmap->player[ cc->getactplayer() ].research.vehicletypeavailable ( fzt, actmap ) ) {
         int en = fzt->production.energy;
         int ma = fzt->production.material;
         int fu = 0;

         if ( gameoptions.container.filleverything ) {
            int en1 = en;
            int ma1 = ma;
            int fu1 = fu;
   
            fu += fzt->tank;
            ma += fzt->material;
   
            displaymessage2(" production costs ~%d~ energy, ~%d~ material and ~%d~ fuel (empty: %d energy, %d material, %d fuel)", en, ma, fu, en1, ma1, fu1 );
         } else 
            displaymessage2(" production costs ~%d~ energy, ~%d~ material and ~%d~ fuel ", en, ma, fu );
         

      } else
         displaymessage2(" ");
   }  
}

void  ccontainer_b :: setpictures ( void )
{
  int i;
   if ( unitmode == mnormal  || unitmode == mloadintocontainer ) {
      ccontainer::setpictures();
   } else 

   if ( unitmode == mproduction ) {
      int num = 0;
      for (i = 0; i < 32; i++ )
         if ( building->production[i]  &&  
              actmap->player[ cc->getactplayer() ].research.vehicletypeavailable ( building->production[i], actmap )  &&  
              building->typ->vehicleloadable( building->production[i] ) ) {
            produceableunits[num] = building->production[i];
            picture[num] = building->production[i]->picture[0] ;
            int en = building->production[i]->production.energy;
            int ma = building->production[i]->production.material;
            if ( getenergy ( en, 0 ) < en  ||  getmaterial ( ma, 0 ) < ma )
               pictgray[num] = 1;
            else
               pictgray[num] = 0;
            num++;
         };
      for ( i = num; i < 32; i++ ) {
         picture[i] = NULL;
         pictgray[i] = 0;
      }

      inactivefield = icons.container.mark.repairinactive;
      activefield   = icons.container.mark.repairactive;
   }
}



pvehicle    ccontainer_b :: getmarkedunit (void)
{
   if ( unitmode == mnormal || unitmode == mloadintocontainer)
      return building->loading[mark.y*unitsshownx + mark.x];
   else
      return NULL;
}

pvehicletype ccontainer_b :: getmarkedunittype ( void )
{
   if ( unitmode == mproduction )
      return produceableunits[mark.y*unitsshownx + mark.x];
   else
      return NULL;
}



pvehicle cbuildingcontrols :: getloadedunit (int num)
{
  return building->loading[num];
};

pvehicle ccontainer_b :: getloadedunit (int num)
{
   if ( unitmode == mnormal || unitmode == mloadintocontainer )
      return building->loading[num];
   else
      return NULL;
};

void  ccontainer_b :: paintvehicleinfo ( void )
{
   if ( unitmode == mnormal )
      dashboard.paintvehicleinfo ( getmarkedunit(), NULL, NULL );
   else {
      pvehicletype fzt = getmarkedunittype ();
      if ( fzt ) {
         pvehicle tempunit = produceunit.produce_hypothetically( fzt );
         :: dashboard.paintvehicleinfo ( tempunit, NULL, NULL );
         :: removevehicle ( &tempunit );
      }

   }
}



ccontainer_b :: ~ccontainer_b ( )
{
   actgui = oldguihost ;
}



int    ccontainer_b :: putammunition (int  weapontype, int  ammunition, int abbuchen)
{
   if ( abbuchen )
      repaintammo = 1;

   return cbuildingcontrols :: putammunition ( weapontype, ammunition, abbuchen );
}


int    ccontainer_b :: getammunition ( int weapontype, int num, int abbuchen, int produceifrequired  )
{
   if ( abbuchen )
      repaintammo = 1;

   return cbuildingcontrols :: getammunition ( weapontype, num, abbuchen, produceifrequired  );
}


int    ccontainer_b :: putenergy (int e, int abbuchen  )
{
   if ( abbuchen )
      repaintresources = 1;
   return cbuildingcontrols :: putenergy ( e, abbuchen );
}

int    ccontainer_b :: putmaterial (int m, int abbuchen  )
{
   if ( abbuchen )
      repaintresources = 1;
   return cbuildingcontrols :: putmaterial ( m, abbuchen );
}

int    ccontainer_b :: putfuel (int f, int abbuchen  )
{
   if ( abbuchen )
      repaintresources = 1;
   return cbuildingcontrols :: putfuel ( f, abbuchen );
}

int    ccontainer_b :: getenergy ( int need, int abbuchen )
{
   if ( abbuchen )
      repaintresources = 1;
   return cbuildingcontrols :: getenergy ( need, abbuchen );
}

int    ccontainer_b :: getmaterial ( int need, int abbuchen )
{
   if ( abbuchen )
      repaintresources = 1;
   return cbuildingcontrols :: getmaterial ( need, abbuchen );
}

int    ccontainer_b :: getfuel ( int need, int abbuchen )
{
   if ( abbuchen )
      repaintresources = 1;
   return cbuildingcontrols :: getfuel ( need, abbuchen );
}



// Gui




void  ccontainer_b :: chosticons_cb :: init ( int resolutionx, int resolutiony )
{
   chainiconstohost ( &icons.movement );     //   muá erst eingesetzt werden !
   tguihost::init ( resolutionx, resolutiony );
}








// --------------- Subwindows ---------------------------------------------------------------------------------- 







ccontainer_b :: crepairbuilding_subwindow :: crepairbuilding_subwindow ( void )
{
   strcpy ( name, "building info / repair" );
   laschpic1 = icons.container.lasche.a.buildinginfo[0];
   laschpic2 = icons.container.lasche.a.buildinginfo[1];

   objcoordinates[0].x1 = subwinx1 + 164;
   objcoordinates[0].y1 = subwiny1 +  34;
   objcoordinates[0].x2 = subwinx1 + 200;
   objcoordinates[0].y2 = subwiny1 + 50;
   objcoordinates[0].type = 5;

   ndamag = 0;

   helplist.num = 10;

   static tonlinehelpitem repairbuildinghelpitems[10]  = {{ 53 + subwinx1 , 26 + subwiny1 , 153 + subwinx1, 35 + subwiny1, 20100 },
                                                          { 52 + subwinx1 , 49 + subwiny1 ,  92 + subwinx1, 60 + subwiny1, 20101 },
                                                          {113 + subwinx1 , 49 + subwiny1 , 153 + subwinx1, 60 + subwiny1, 20102 },
                                                          {164 + subwinx1 , 34 + subwiny1 , 200 + subwinx1, 50 + subwiny1, 20103 },
                                                          { 26 + subwinx1 , 71 + subwiny1 ,  92 + subwinx1, 81 + subwiny1, 20104 },
                                                          { 26 + subwinx1 , 83 + subwiny1 ,  92 + subwinx1, 93 + subwiny1, 20105 },
                                                          { 26 + subwinx1 , 95 + subwiny1 ,  92 + subwinx1,105 + subwiny1, 20106 },
                                                          {113 + subwinx1 , 71 + subwiny1 , 153 + subwinx1, 81 + subwiny1, 20107 },
                                                          {277 + subwinx1 , 22 + subwiny1 , 297 + subwinx1,108 + subwiny1, 20108 },
                                                          {308 + subwinx1 , 22 + subwiny1 , 328 + subwinx1,108 + subwiny1, 20109 }};

   helplist.item = repairbuildinghelpitems;

}

int  ccontainer_b :: crepairbuilding_subwindow :: subwin_available ( void )
{
   cbuildingsubwindow :: subwin_available ( );
   if ( next )
     next->subwin_available ();

   return 0;
}

void  ccontainer_b :: crepairbuilding_subwindow :: display ( void )
{
   setinvisiblemouserectanglestk ( subwinx1, subwiny1, subwinx2, subwiny2 );
   npush ( activefontsettings );
   putimage ( subwinx1, subwiny1, icons.container.subwin.buildinginfo.main );

   csubwindow :: display();

   for ( int i = 0; i < 8; i++ ) {
      if ( cc_b->building->typ->loadcapability & ( 1 << ( 7 - i) ))
         putimage ( subwinx1 + 277, subwiny1 + 22 + i * 11, icons.container.subwin.buildinginfo.height1[i] );
      else
         putimage ( subwinx1 + 277, subwiny1 + 22 + i * 11, icons.container.subwin.buildinginfo.height2[i] );

      if ( cc_b->building->typ->unitheightreq & ( 1 << ( 7 - i ) ))
         putimage ( subwinx1 + 308, subwiny1 + 22 + i * 11, icons.container.subwin.buildinginfo.height1[i] );
      else
         putimage ( subwinx1 + 308, subwiny1 + 22 + i * 11, icons.container.subwin.buildinginfo.height2[i] );
   }
   activefontsettings.font = schriften.guifont;
   activefontsettings.length = 36;
   activefontsettings.justify = righttext;
   activefontsettings.background = 255;
   showtext2c ( strrr ( cc_b->building->typ->armor ), subwinx1 + 53,  subwiny1 + 72 );
   showtext2c ( strrr ( cc_b->building->typ->jamming ),  subwinx1 + 53,  subwiny1 + 84 );
   showtext2c ( strrr ( cc_b->building->typ->view ),     subwinx1 + 53,  subwiny1 + 96 );
   showtext2c ( strrr ( cc_b->building->typ->loadcapacity ),  subwinx1 + 140, subwiny1 + 72 );

   paintvariables();

/*   activefontsettings.color = white;
   activefontsettings.font = schriften.guifont;
   activefontsettings.length = 60;
   activefontsettings.justify = lefttext;
   activefontsettings.background = bkgrdarkcol;


   showtext2c ( "damage:",       subwinx1 + 20, subwiny1 + 10 );
   showtext2c ( "repairable:",   subwinx1 + 20, subwiny1 + 27 );
   showtext2c ( "repair cost: ", subwinx1 + 20, subwiny1 + 44 );


   objcoordinates[0].x1 = subwinx1 + 182;
   objcoordinates[0].x2 = subwinx2 -  20;
   objcoordinates[0].y1 = subwiny1 +  44;
   objcoordinates[0].y2 = subwiny1 +  58 + activefontsettings.font->height;
   objcoordinates[0].type = 2;
   objnum = 1;
   paintobj ( 0, 0 );

   activefontsettings.length = objcoordinates[0].x2 - objcoordinates[0].x1 - 2;
   activefontsettings.height = 0;
   activefontsettings.justify = centertext;
   showtext2c ( "repair !", objcoordinates[0].x1 + 1 , ( objcoordinates[0].y1 + objcoordinates[0].y2 - activefontsettings.font->height ) / 2 );
*/
   npop ( activefontsettings );
   getinvisiblemouserectanglestk (  );

}

void ccontainer_b :: crepairbuilding_subwindow :: paintvariables ( void )
{
   npush ( activefontsettings );
   int newdamage = checkto ( ndamag );

   int w = ( 100 - cc_b->building->damage);
   int c;
   if ( w > 23 )       
      c = green; 
   else
      if ( w > 15 )
         c = yellow; 
      else
         if ( w > 7 )
            c = lightred; 
         else
            c = red; 

   bar ( subwinx1 + 53, subwiny1 + 26, subwinx1 + 53 + ( 100 - cc_b->building->damage), subwiny1 + 34, c );

   if( cc_b->building->damage )
      bar ( subwinx1 + 53 + ( 100 - cc_b->building->damage)+1, subwiny1 + 26, subwinx1 + 152, subwiny1 + 34, 244 );

   putimage ( subwinx1 + 68, subwiny1 + 26, icons.container.subwin.buildinginfo.block );
   putimage ( subwinx1 + 86, subwiny1 + 26, icons.container.subwin.buildinginfo.block );
   putimage ( subwinx1 +104, subwiny1 + 26, icons.container.subwin.buildinginfo.block );

   if ( cc_b->building->damage ) {
      activefontsettings.font = schriften.guifont;
      activefontsettings.length = 37;
      activefontsettings.justify = righttext;
      activefontsettings.background = 244;
   
      showtext2c ( strrr ( energycosts ), subwinx1 + 53, subwiny1 + 51 );
      showtext2c ( strrr ( materialcosts ), subwinx1 + 114, subwiny1 + 51 );
   } else {
      bar ( subwinx1 + 53, subwiny1 + 50 , subwinx1 + 53 + 38, subwiny1 + 59 , 244 );
      bar ( subwinx1 + 114, subwiny1 + 50, subwinx1 + 114 + 38, subwiny1 + 59 , 244 );
   }
   npop ( activefontsettings );
}

void  ccontainer_b :: crepairbuilding_subwindow :: paintobj ( int num, int stat )
{
   csubwindow::paintobj( num, stat );
   if ( objcoordinates[num].type == 5 ) {
      setinvisiblemouserectanglestk ( objcoordinates[num].x1,   objcoordinates[num].y1,   objcoordinates[num].x2,   objcoordinates[num].y2 );
      if ( stat == 0 )
         putrotspriteimage ( objcoordinates[num].x1, objcoordinates[num].y1, icons.container.subwin.buildinginfo.repair, actmap->actplayer * 8  );
      else
         putrotspriteimage ( objcoordinates[num].x1, objcoordinates[num].y1, icons.container.subwin.buildinginfo.repairpressed, actmap->actplayer * 8  );
      getinvisiblemouserectanglestk (  );
   }
}

void  ccontainer_b :: crepairbuilding_subwindow :: checkformouse ( void )
{
   if ( available () )
      if ( objpressedbymouse ( 0 ) ) {
         repairto ( ndamag );
         paintvariables();
      }
}

void  ccontainer_b :: crepairbuilding_subwindow :: checkforkey ( tkey taste )
{

}


//............................................................................................


ccontainer_b :: cnetcontrol_subwindow :: cnetcontrol_subwindow ( void )
{
   strcpy ( name, "net control" );
   laschpic1 = icons.container.lasche.a.netcontrol[0];
   laschpic2 = icons.container.lasche.a.netcontrol[1];
}

int  ccontainer_b :: cnetcontrol_subwindow :: subwin_available ( void )
{
   if ( actmap->resourcemode != 1 )
      cbuildingsubwindow :: subwin_available ( );

   if ( next )
     next->subwin_available ();

   return 0;
}

void  ccontainer_b :: cnetcontrol_subwindow :: paintobj ( int num, int stat )
{
   csubwindow::paintobj( num, stat );
   if ( objcoordinates[num].type == 10 ) {
      setinvisiblemouserectanglestk ( objcoordinates[num].x1,   objcoordinates[num].y1,   objcoordinates[num].x2+10,   objcoordinates[num].y2 );
      if ( stat == 1 )
         putrotspriteimage ( objcoordinates[num].x1, objcoordinates[num].y1, icons.container.subwin.netcontrol.active, actmap->actplayer * 8  );
      else
         putrotspriteimage ( objcoordinates[num].x1, objcoordinates[num].y1, icons.container.subwin.netcontrol.inactive, actmap->actplayer * 8  );
      getinvisiblemouserectanglestk (  );
   }
}


void  ccontainer_b :: cnetcontrol_subwindow :: display ( void )
{
   setinvisiblemouserectanglestk ( subwinx1, subwiny1, subwinx2, subwiny2 );
   putimage ( subwinx1, subwiny1, icons.container.subwin.netcontrol.main );

   npush ( activefontsettings );
   csubwindow :: display();
   activefontsettings.font = schriften.guifont;
   activefontsettings.length = 0;
   activefontsettings.justify = lefttext;
   activefontsettings.background = 255;
   objnum = 0;
   for ( int y = 0; y < 4; y++ ) {
      showtext2c ( cgeneralnetcontrol[y], subwinx1 + 12, subwiny1 + 32 + 20 * y );
      for ( int x = 0; x < 3 ; x++) {
         objcoordinates[objnum].x1 = subwinx1 + 210 + 44 * x;
         objcoordinates[objnum].y1 = subwiny1 +  29 + 20 * y;
         objcoordinates[objnum].x2 = objcoordinates[objnum].x1 + 20;
         objcoordinates[objnum].y2 = objcoordinates[objnum].y1 + 14;
         objcoordinates[objnum].type = 10;

         if ( getstatus ( objnum ) )
            paintobj( objnum ,1 ) ;
         else
            paintobj( objnum ,0 ) ;

         objnum++;
      } /* endfor */
   }

   npop ( activefontsettings );
   getinvisiblemouserectanglestk ( );

}

int ccontainer_b :: cnetcontrol_subwindow :: getstatus ( int num )
{
   switch ( num ) {
      case 0:
      case 1:
      case 2:  return getnetmode ( cnet_storeenergy << num );
      case 3:
      case 4:
      case 5:  return getnetmode ( cnet_moveenergyout << (num-3) );
      case 6:
      case 7:
      case 8:  return getnetmode ( cnet_stopenergyinput << (num-6) );
      case 9:
      case 10:
      case 11: return getnetmode ( cnet_stopenergyoutput << (num-9) );
   } /* endswitch */
   return 0;
}

void ccontainer_b :: cnetcontrol_subwindow :: objpressed ( int num )
{
   switch ( num ) {
      case 0:
      case 1:
      case 2:  setnetmode ( cnet_storeenergy << (num % 3), !getnetmode ( cnet_storeenergy << (num % 3) ) );
               break;
      case 3:
      case 4:
      case 5:  setnetmode ( cnet_moveenergyout << (num % 3), !getnetmode ( cnet_moveenergyout << (num % 3) ) );
               break;
      case 6:
      case 7:
      case 8:  setnetmode ( cnet_stopenergyinput << (num % 3), !getnetmode ( cnet_stopenergyinput << (num % 3) ) );
               break;
      case 9:
      case 10:
      case 11: setnetmode ( cnet_stopenergyoutput << (num % 3), !getnetmode ( cnet_stopenergyoutput << (num % 3) ) );
               break;
   } /* endswitch */


   if ( getstatus ( num ) )
      paintobj ( num, 1 );
   else
      paintobj ( num, 0 );

   if ( getnetmode ( cnet_storeenergy << (num % 3) ) && getnetmode ( cnet_moveenergyout << (num % 3) ) ) {
      if ( num < 3 ) {
         setnetmode ( cnet_moveenergyout << (num % 3), 0 );
         paintobj ( num + 3, 0 );
      } else {
         setnetmode ( cnet_storeenergy << (num % 3), 0 );
         paintobj ( (num % 3), 0 );
      }
   }
   hostcontainerb->repaintresources = 1;
   hostcontainerb->building->execnetcontrol();
}



void  ccontainer_b :: cnetcontrol_subwindow :: checkformouse ( void )
{
   for (int i = 0; i < 12; i++ ) {
      if ( mouseparams.x >= objcoordinates[i].x1  &&  mouseparams.y >= objcoordinates[i].y1  &&  mouseparams.x <= objcoordinates[i].x2  &&  mouseparams.y <= objcoordinates[i].y2  &&  mouseparams.taste == 1 ) {
         objpressed ( i );
         while ( mouseparams.x >= objcoordinates[i].x1  &&
             mouseparams.y >= objcoordinates[i].y1  &&
             mouseparams.x <= objcoordinates[i].x2  &&
             mouseparams.y <= objcoordinates[i].y2  &&
             mouseparams.taste == 1 );
//         objpressed ( i );
      }
   } /* endfor */
}

void  ccontainer_b :: cnetcontrol_subwindow :: checkforkey ( tkey taste )
{
  switch ( taste ) {
     case ct_1: objpressed ( 0 );
        break;
     case ct_2: objpressed ( 1 );
        break;
     case ct_3: objpressed ( 2 );
        break;
     case ct_q: objpressed ( 3 );
        break;
     case ct_w: objpressed ( 4 );
        break;
     case ct_e: objpressed ( 5 );
        break;
     case ct_a: objpressed ( 6 );
        break;
     case ct_s: objpressed ( 7 );
        break;
     case ct_d: objpressed ( 8 );
        break;
     case ct_y: objpressed ( 9 );
        break;
     case ct_x: objpressed ( 10 );
        break;
     case ct_c: objpressed ( 11 );
        break;
  } /* endswitch */
}


//............................................................................................


ccontainer_b :: cconventionelpowerplant_subwindow :: cconventionelpowerplant_subwindow ( void )
{
   strcpy ( name, "power plant" );
   laschpic1 = icons.container.lasche.a.powerplant[0];
   laschpic2 = icons.container.lasche.a.powerplant[1];

   materialcolor = 232;
   fuelcolor = 125;
   objcoordinates[0].x1 = subwinx1 + 316;
   objcoordinates[0].y1 = subwiny1 +   3;
   objcoordinates[0].x2 = subwinx1 + 343;
   objcoordinates[0].y2 = subwiny1 +  15;
   objcoordinates[0].type = 17;
}

int  ccontainer_b :: cconventionelpowerplant_subwindow :: subwin_available ( void )
{
   if ( ( hostcontainer->getspecfunc ( mbuilding ) & cgconventionelpowerplantb ) && ( cc_b->building->maxplus.a.energy ))
      cbuildingsubwindow :: subwin_available ( );

   if ( next )
     next->subwin_available ();

   return 0;
}

void  ccontainer_b :: cconventionelpowerplant_subwindow :: display ( void )
{

   if ( cc_b->building->maxplus.a.energy )
      power = 1024 * cc_b->building->plus.a.energy / cc_b->building->maxplus.a.energy;
   else
      power = 0;


   setinvisiblemouserectanglestk ( subwinx1, subwiny1, subwinx2, subwiny2 );
   
   npush ( activefontsettings );

   putimage ( subwinx1, subwiny1, icons.container.subwin.conventionelpowerplant.main );

   csubwindow :: display();
   activefontsettings.color = white;
   activefontsettings.font = schriften.guifont;
   activefontsettings.length = 0;
   activefontsettings.justify = lefttext;
   activefontsettings.height = 0;
/*   showtext2c ( "energy plus:", subwinx1 + 8, subwiny1 + 25 );

   showtext2c ( "fuel     cost:",     subwinx1 + 8, subwiny1 + 43 );
   showtext2c ( "material cost:",     subwinx1 + 8, subwiny1 + 61 );*/

//   showtext2c ( "avail in:",        subwinx1 + 8, subwiny1 + 79 );

//   showtext2 ( "act. technology:", subwinx1 + 195, subwiny1 + 4 );
   showtext2c ( "change all buildings:",subwinx1+179,subwiny1 + 5 );



   gx1 = subwinx1 + 181;
   gy1 = subwiny1 + 105 - 70;
   gx2 = subwinx1 + 181 + 150;
   gy2 = subwiny1 + 105;


   npop ( activefontsettings );

   displayvariables();
   getinvisiblemouserectanglestk();

}

int ccontainer_b :: cconventionelpowerplant_subwindow :: allbuildings = 0;

void ccontainer_b :: cconventionelpowerplant_subwindow :: setnewpower ( int pwr )
{
   int x = gx1 + ( gx2 - gx1 ) * power / 1024;

   int cl ;
   if ( x == gx1 )
      cl = 247;
   else
      if ( x == gx1+1 )
         cl = 244;
      else
         cl = 201;

   line( x, gy1, x, gy2-1, cl );

   power = pwr;

   if ( allbuildings ) {
      pbuilding bld = actmap->player[actmap->actplayer].firstbuilding;
      while ( bld ) {
         if ( bld->typ->special & cgconventionelpowerplantb ) {
            bld->plus.a.energy = bld->maxplus.a.energy * power/1024;
            if ( bld->plus.a.energy  > bld->maxplus.a.energy )
               bld->plus.a.energy = bld->maxplus.a.energy;
          }
          bld=bld->next;
      }
   } else {
      pbuilding bld = cc_b->building;
      bld->plus.a.energy = bld->maxplus.a.energy * power/1024;
      if ( bld->plus.a.energy  > bld->maxplus.a.energy )
         bld->plus.a.energy = bld->maxplus.a.energy;
   }

}

void  ccontainer_b :: cconventionelpowerplant_subwindow :: displayvariables ( void )
{
  int x;

   npush ( activefontsettings );
   activefontsettings.color = white;
   activefontsettings.font = schriften.guifont;
   activefontsettings.length = 51;
   activefontsettings.justify = righttext;
   activefontsettings.height = 0;
   activefontsettings.background = 201;

   int fuel;
   int material;
   returnresourcenuseforpowerplant ( cc_b->building, cc_b->building->plus.a.energy, &material, &fuel );

   showtext2c ( strrr( cc_b->building->plus.a.energy ), subwinx1 + 63, subwiny1 + 25 );

   showtext2c ( strrr( material ), subwinx1 +  63, subwiny1 + 43 );
   showtext2c ( strrr( fuel ), subwinx1 +  63, subwiny1 + 61 );

   activefontsettings.length = 25;
   int em,ef;
   getpowerplantefficiency ( cc_b->building, &em, &ef );

   showtext2c ( strrr( em ), subwinx1 + 120, subwiny1 + 81 );
   showtext2c ( strrr( ef ), subwinx1 + 120, subwiny1 + 99 );


   activefontsettings.justify = centertext;
   activefontsettings.length = 22;
   activefontsettings.background = 255;

   paintobj ( 0,0 );


   returnresourcenuseforpowerplant ( cc_b->building, cc_b->building->maxplus.a.energy, &material, &fuel );
   int max;
   if ( fuel > material )
      max = fuel * 17/16;
   else
      max = material * 17/16;

   int dist = gx2-gx1;
   for (x = dist; x >0 ; x--) {
       int res = cc_b->building->maxplus.a.energy * x / dist;
       returnresourcenuseforpowerplant ( cc_b->building, res, &material, &fuel );

       if ( max ) {
          putpixel ( gx1 + x, gy2 - ( gy2-gy1 ) * fuel / max, fuelcolor );
          putpixel ( gx1 + x, gy2 - ( gy2-gy1 ) * material / max, materialcolor );
       }

   } /* endfor */
   x = gx1 + ( gx2 - gx1 ) * power / 1024;

   line( x, gy1, x, gy2-1, white );

   npop ( activefontsettings );
}


void  ccontainer_b :: cconventionelpowerplant_subwindow :: checkformouse ( void )
{
   if ( mouseparams.taste == 1 ) {
      if ( mouseinrect ( gx1, gy1, gx2, gy2 ) ) {
         int newpower = 1024 * (mouseparams.x-gx1) / (gx2-gx1);

         if ( newpower < 0 )
            newpower = 0;
         if ( newpower > 1024 )
            newpower = 1024;

         if ( newpower != power ) {
            setinvisiblemouserectanglestk ( subwinx1, subwiny1, subwinx2, subwiny2 );

            setnewpower( newpower );

            displayvariables();

            getinvisiblemouserectanglestk ( );

         }
      }
      if ( objpressedbymouse(0) ) {
         allbuildings = !allbuildings;
         setinvisiblemouserectanglestk ( subwinx1, subwiny1, subwinx2, subwiny2 );
         displayvariables();
         getinvisiblemouserectanglestk ( );
      }
   }
}


void  ccontainer_b :: cconventionelpowerplant_subwindow :: paintobj ( int num, int stat )
{
  if ( objcoordinates[0].type == 17 ) {

     setinvisiblemouserectanglestk ( objcoordinates[num].x1,   objcoordinates[num].y1,   objcoordinates[num].x2+10,   objcoordinates[num].y2 );

     activefontsettings.font = schriften.guifont;
     activefontsettings.height = 0;
     activefontsettings.justify = centertext;
     activefontsettings.length = 22;
     activefontsettings.background = 255;

     if ( stat == 0 ) {
        putimage ( objcoordinates[0].x1, objcoordinates[0].y1, icons.container.subwin.conventionelpowerplant.button[0] );
        if ( allbuildings ) 
           showtext2c ( "yes", subwinx1+318, subwiny1 +  5 );
        else 
           showtext2c ( "no",  subwinx1+318, subwiny1 +  5 );
     } else {
        putimage ( objcoordinates[0].x1, objcoordinates[0].y1, icons.container.subwin.conventionelpowerplant.button[1] );
        if ( allbuildings ) 
           showtext2c ( "yes", subwinx1+319, subwiny1 +  6 );
        else 
           showtext2c ( "no",  subwinx1+319, subwiny1 +  6 );
     }

     getinvisiblemouserectanglestk ( );
  }
}


void  ccontainer_b :: cconventionelpowerplant_subwindow :: checkforkey ( tkey taste )
{
  if ( taste == ct_space  ||  taste == ct_a ) {
     allbuildings = !allbuildings;
     setinvisiblemouserectanglestk ( subwinx1, subwiny1, subwinx2, subwiny2 );
     displayvariables();
     getinvisiblemouserectanglestk ( );
  }
  int keyspeed = 50;
  if ( (taste == ct_left || taste==ct_4k) && power > 0 ) {
     setinvisiblemouserectanglestk ( subwinx1, subwiny1, subwinx2, subwiny2 );

     if ( power > keyspeed )
        setnewpower ( power - keyspeed );
     else
        setnewpower ( 0 );

     displayvariables();
     getinvisiblemouserectanglestk ( );
  }
  if ( (taste == ct_right  || taste==ct_6k) && power < 1024 ) {
     setinvisiblemouserectanglestk ( subwinx1, subwiny1, subwinx2, subwiny2 );

     if ( power+keyspeed < 1024 )
        setnewpower ( power + keyspeed );
     else
        setnewpower ( 1024 );

     displayvariables();
     getinvisiblemouserectanglestk ( );
  }
}

//............................................................................................


ccontainer_b :: cwindpowerplant_subwindow :: cwindpowerplant_subwindow ( void )
{
   strcpy ( name, "wind power" );
   laschpic1 = icons.container.lasche.a.wind[0];
   laschpic2 = icons.container.lasche.a.wind[1];
}

int  ccontainer_b :: cwindpowerplant_subwindow :: subwin_available ( void )
{
   if ( hostcontainer->getspecfunc ( mbuilding ) & cgwindkraftwerkb )
      cbuildingsubwindow :: subwin_available ( );
   if ( next )
     next->subwin_available ();

   return 0;
}

void  ccontainer_b :: cwindpowerplant_subwindow :: display ( void )
{
   npush ( activefontsettings );
   setinvisiblemouserectanglestk ( subwinx1, subwiny1, subwinx2, subwiny2 );
   putimage ( subwinx1, subwiny1, icons.container.subwin.windpower.main );

   csubwindow :: display();
   activefontsettings.length = 95;
   activefontsettings.justify = lefttext;
   activefontsettings.background = 201;
   activefontsettings.font = schriften.guifont;
   showtext2c ( " act power : ", subwinx1 + 9, subwiny1 + 38 );
   showtext2c ( " power storable: ", subwinx1 + 9, subwiny1 + 62 );
   showtext2c ( " max power : ", subwinx1 + 9, subwiny1 + 86 );

   activefontsettings.length = 53;
   activefontsettings.justify = righttext;
   int prod = cc_b->building->getenergyplus( -2 );
   int storable = cc_b->building->getenergyplus( -1 );
   showtext2c ( strrr ( prod ), subwinx1 + 117, subwiny1 + 38 );

   char buf[100];
   if ( prod ) {
      strcpy ( buf, strrr ( storable / prod * 100));
      strcat ( buf, "%" );
   } else 
      strcpy ( buf, "-" );

   showtext2c ( buf, subwinx1 + 117, subwiny1 + 62 );
   showtext2c ( strrr ( cc_b->building->maxplus.a.energy ), subwinx1 + 117, subwiny1 + 86 );

   getinvisiblemouserectanglestk (  );
   npop ( activefontsettings );

}



void  ccontainer_b :: cwindpowerplant_subwindow :: checkformouse ( void )
{
}

void  ccontainer_b :: cwindpowerplant_subwindow :: checkforkey ( tkey taste )
{

}

void ccontainer_b :: cwindpowerplant_subwindow :: resetresources ( int mode )
{
   if ( mode == 0 )
      (*firstavailable)->resetresources(1);
   else {
      if ( hostcontainer->actsubwindow == this )
         display();

      if ( nextavailable )
         nextavailable->resetresources ( mode );
   }
}

//............................................................................................


ccontainer_b :: csolarpowerplant_subwindow :: csolarpowerplant_subwindow ( void )
{
   strcpy ( name, "solar power" );
   laschpic1 = icons.container.lasche.a.solar[0];
   laschpic2 = icons.container.lasche.a.solar[1];
}

int  ccontainer_b :: csolarpowerplant_subwindow :: subwin_available ( void )
{
   if ( hostcontainer->getspecfunc ( mbuilding ) & cgsolarkraftwerkb )
      cbuildingsubwindow :: subwin_available ( );
   if ( next )
     next->subwin_available ();

   return 0;
}

void  ccontainer_b :: csolarpowerplant_subwindow :: display ( void )
{
   npush ( activefontsettings );
   setinvisiblemouserectanglestk ( subwinx1, subwiny1, subwinx2, subwiny2 );
   putimage ( subwinx1, subwiny1, icons.container.subwin.solarpower.main );
   csubwindow :: display();

   activefontsettings.length = 95;
   activefontsettings.justify = lefttext;
   activefontsettings.background = 201;
   activefontsettings.font = schriften.guifont;
   showtext2c ( " act power : ", subwinx1 + 9, subwiny1 + 38 );
   showtext2c ( " power storable: ", subwinx1 + 9, subwiny1 + 62 );
   showtext2c ( " max power : ", subwinx1 + 9, subwiny1 + 86 );

   activefontsettings.length = 53;
   activefontsettings.justify = righttext;
   int prod = cc_b->building->getenergyplus( -2 );
   int storable = cc_b->building->getenergyplus( -1 );
   showtext2c ( strrr ( prod ), subwinx1 + 117, subwiny1 + 38 );

   char buf[100];
   if ( prod ) {
      strcpy ( buf, strrr ( storable / prod * 100));
      strcat ( buf, "%" );
   } else 
      strcpy ( buf, "-" );

   showtext2c ( buf, subwinx1 + 117, subwiny1 + 62 );

   if ( actmap->resourcemode == 1 )
      showtext2c ( strrr ( cc_b->building->bi_resourceplus.a.energy ), subwinx1 + 117, subwiny1 + 86 );
   else
      showtext2c ( strrr ( cc_b->building->maxplus.a.energy ), subwinx1 + 117, subwiny1 + 86 );


   getinvisiblemouserectanglestk (  );
   npop ( activefontsettings );
}



void  ccontainer_b :: csolarpowerplant_subwindow :: checkformouse ( void )
{
}

void  ccontainer_b :: csolarpowerplant_subwindow :: checkforkey ( tkey taste )
{

}


void ccontainer_b :: csolarpowerplant_subwindow :: resetresources ( int mode )
{
   if ( mode == 0 )
      (*firstavailable)->resetresources(1);
   else {
      if ( hostcontainer->actsubwindow == this )
         display();

      if ( nextavailable )
         nextavailable->resetresources ( mode );
   }
}


//............................................................................................


ccontainer_b :: cammunitionproduction_subwindow :: cammunitionproduction_subwindow ( void )
{
  int i;

   strcpy ( name, "ammunition production" );

   for (i = 0; i < waffenanzahl; i++) {
      objcoordinates[i].x1 = subwinx1 + 23  + 37 * i ;
      objcoordinates[i].x2 = objcoordinates[i].x1 + 10;
      objcoordinates[i].y1 = subwiny1 + 28;
      objcoordinates[i].y2 = subwiny1 + 90;
      objcoordinates[i].type = 3;
      objcoordinates[i].t1 = 0;
      objcoordinates[i].t2 = 8;
   } /* endfor */

   txtptr = "produce";

   laschpic1 = icons.container.lasche.a.ammoproduction[0];
   laschpic2 = icons.container.lasche.a.ammoproduction[1];
   maxproduceablenum = 20;
   grad = 50;
   materialneeded = 0;
   energyneeded = 0;
   fuelneeded = 0;

   for ( i = 0; i < waffenanzahl; i++) 
      toproduce[i] = 0;
                                           
   objcoordinates[9].x1 = subwinx1 + 217;
   objcoordinates[9].x2 = subwinx1 + 303;
   objcoordinates[9].y1 = subwiny1 + 4;
   objcoordinates[9].y2 = subwiny1 + 14;
   objcoordinates[9].type = 5;

   actschieber = 0 ;

}


int  ccontainer_b :: cammunitionproduction_subwindow :: subwin_available ( void )
{
   if ( hostcontainer->getspecfunc ( mbuilding ) & cgammunitionproductionb )
      cbuildingsubwindow :: subwin_available ( );
   if ( next )
     next->subwin_available ();

   return 0;
}

void  ccontainer_b :: cammunitionproduction_subwindow :: displayusage ( void )
{
   int e = 0;
   int m = 0;
   int f = 0;

   for ( int i = 0; i < waffenanzahl; i++ ) {
       e += cwaffenproduktionskosten[i][0] * toproduce[i];
       m += cwaffenproduktionskosten[i][1] * toproduce[i];
       f += cwaffenproduktionskosten[i][2] * toproduce[i];
   }

   activefontsettings.color = white;
   activefontsettings.font = schriften.guifont;
   activefontsettings.length = 35;
   activefontsettings.justify = righttext;
   activefontsettings.background = 201;
   showtext2c ( strrr ( e ), subwinx1 + 305, subwiny1 + 37 );
   showtext2c ( strrr ( m ), subwinx1 + 305, subwiny1 + 67 );
   showtext2c ( strrr ( f ), subwinx1 + 305, subwiny1 + 97 );

}

void  ccontainer_b :: cammunitionproduction_subwindow :: display ( void )
{
  int i;

   npush ( activefontsettings );
   setinvisiblemouserectanglestk ( subwinx1, subwiny1, subwinx2, subwiny2 );
   putimage ( subwinx1, subwiny1, icons.container.subwin.ammoproduction.main );
   csubwindow :: display();
   activefontsettings.color = white;
   activefontsettings.font = schriften.guifont;
   activefontsettings.justify = lefttext;


   npush ( *agmp );

   agmp-> linearaddress = (int) new char [ 10000 ];

   agmp-> scanlinelength = 100;
   agmp-> windowstatus = 100;

   int x1 = 0;
   int y1 = 0;
   int x2 = 66;
   int y2 = 14;
   char* buf = new char [ imagesize ( x1, y1, x2, y2 ) ];

   for (i = 0; i < waffenanzahl; i++) {
      memset ( (void*) agmp-> linearaddress, 255, 10000 );
      activefontsettings.length = 0;
      activefontsettings.justify = lefttext;
      activefontsettings.background = 255;
      showtext2c ( cwaffentypen[i], 10, 10 );
      getimage ( 10, 10, 10 + x2, 10 + y2, buf );
      npush ( *agmp );
      *agmp = *hgmp;
      putrotspriteimage90 ( subwinx1 + 5 + 37 * i , subwiny1 + 27, buf, 0 );
      npop ( *agmp );

      objcoordinates[i].t1 = num2pos ( toproduce[i] );
   }

   delete buf;
   delete  ( (void*) agmp-> linearaddress );
   npop  ( *agmp );


   for (i = 0; i < waffenanzahl; i++) 
      paintobj ( i, 0 );

   activefontsettings.font = schriften.guifont;
   activefontsettings.background = 255;
   activefontsettings.length = 65;
   activefontsettings.height = 0;
   activefontsettings.justify = centertext;
   showtext2c ( txtptr, subwinx1 + 228, subwiny1 + 5 );

   displayusage(); 

   getinvisiblemouserectanglestk (  );
   npop ( activefontsettings );

}

void ccontainer_b :: cammunitionproduction_subwindow :: produce( void )
{
  for (int i = 0; i < waffenanzahl; i++) {
     cbuildingcontrols :: cproduceammunition :: produce ( i, toproduce[i] );
     toproduce[i] = 0;
  }
}


int  ccontainer_b :: cammunitionproduction_subwindow :: pos2num ( int pos )
{                                                                          
  int ydiff = objcoordinates[0].y2 - objcoordinates[0].y1 - objcoordinates[0].t2;
//  return ( (double) maxproduceablenum * pow ( pos, grad )  /  pow ( ydiff , grad ) );

  int result = (int)( maxproduceablenum * ( exp ( pos / grad ) - 1 ) / ( exp ( ydiff / grad ) - 1 ));
  if ( result < 0 )
     result = 0;
  if ( result > maxproduceablenum )
     result = (int)maxproduceablenum;
  return result;
}

int  ccontainer_b :: cammunitionproduction_subwindow :: num2pos ( int num )
{
  int ydiff = objcoordinates[0].y2 - objcoordinates[0].y1 - objcoordinates[0].t2;
//  return sqrt ( (double) num * pow ( ydiff, grad ) / maxproduceablenum );

  int result = (int)( grad * log ( num / maxproduceablenum  * ( exp ( ydiff / grad ) - 1 ) + 1 ));
  if ( result < 0 )
     result = 0;
  if ( result > ydiff )
     result = ydiff;

  return result;
}


void ccontainer_b :: cammunitionproduction_subwindow :: checknewval ( int weaptype, int num ) 
{
   baseenergyusage    = 0; 
   basematerialusage  = 0; 
   basefuelusage      = 0; 

   for (int i = 0; i < waffenanzahl; i++) {
       if ( i != weaptype ) {
          int n = checkavail ( i, toproduce[i] );
          if ( n != toproduce[i] ) 
             setnewamount( i, n );

          baseenergyusage    += energyneeded;
          basematerialusage  += materialneeded;
          basefuelusage      += fuelneeded;
       }
   } /* endfor */

   if ( weaptype != -1 ) {
      int n = checkavail ( weaptype, num );
      if ( n != toproduce[weaptype] ) 
         setnewamount( weaptype, n );
   }
   displayusage( );
}

void ccontainer_b :: cammunitionproduction_subwindow :: setnewamount ( int weaptype, int num )
{
   if ( weaptype > 8 )
      displaymessage(" 1 !!!!! ", 2);
   toproduce[weaptype] = num;
   objcoordinates[weaptype].t1 = num2pos ( num );
   paintobj ( weaptype, 0 );
}

void ccontainer_b :: cammunitionproduction_subwindow :: paintobj ( int num, int stat )
{
   csubwindow :: paintobj ( num, stat );
   if ( objcoordinates[num].type == 3 ) {
      setinvisiblemouserectanglestk ( objcoordinates[num].x1,   objcoordinates[num].y1,   objcoordinates[num].x2+10,   objcoordinates[num].y2 );
      putimage ( objcoordinates[num].x1,   objcoordinates[num].y1,  icons.container.subwin.ammoproduction.schiene );

      int offs = 0;
      if ( num == actschieber )
         offs = 1;

      if ( objcoordinates[num].y2 - objcoordinates[num].t1 - objcoordinates[num].t2 == objcoordinates[num].y1 )
         putimage ( objcoordinates[num].x1,   objcoordinates[num].y2 - objcoordinates[num].t1 - objcoordinates[num].t2,  icons.container.subwin.ammoproduction.schieber[offs + 2] );
      else
         putimage ( objcoordinates[num].x1,   objcoordinates[num].y2 - objcoordinates[num].t1 - objcoordinates[num].t2,  icons.container.subwin.ammoproduction.schieber[offs] );

      activefontsettings.color = white;
      activefontsettings.font = schriften.guifont;
      activefontsettings.length = 29;
      activefontsettings.justify = centertext;
      activefontsettings.background = bkgrcol;
      showtext2c ( strrr ( toproduce[num] * weaponpackagesize ), subwinx1 + 6 + num * 37, subwiny1 + 96 );
      getinvisiblemouserectanglestk ( ); 
   }
   if ( objcoordinates[num].type == 5 ) {
      setinvisiblemouserectanglestk ( objcoordinates[num].x1,   objcoordinates[num].y1,   objcoordinates[num].x2+10,   objcoordinates[num].y2 );
      activefontsettings.font = schriften.guifont;
      activefontsettings.background = 255;
      activefontsettings.length = 65;
      activefontsettings.height = 0;
      activefontsettings.justify = centertext;
      if ( stat == 0 ) {
         putimage ( subwinx1 + 217, subwiny1 + 4, icons.container.subwin.ammoproduction.button );
         showtext2c ( txtptr, subwinx1 + 228, subwiny1 +5 );
      } else {
         putimage ( subwinx1 + 217, subwiny1 + 4, icons.container.subwin.ammoproduction.buttonpressed );
         showtext2c ( txtptr, subwinx1 + 229, subwiny1 + 6 );
      }
      getinvisiblemouserectanglestk ( ); 

   }
}

void  ccontainer_b :: cammunitionproduction_subwindow :: checkformouse ( void )
{
  int i;

   if ( mouseparams.taste == 1 ) {
      for (i = 0; i < waffenanzahl; i++) {
         if ( mouseparams.x >= objcoordinates[i].x1    && mouseparams.x <= objcoordinates[i].x2  &&
              mouseparams.y >= objcoordinates[i].y1    && mouseparams.y <= objcoordinates[i].y2 ) {

            int repnt = 0;
            int tp = toproduce[i];
            if ( i != actschieber ) {
               int old = actschieber;
               actschieber = i;
               paintobj ( old, 0 );
               repnt = 1;
            }

            int relpos = objcoordinates[i].y2 -  mouseparams.y - objcoordinates[i].t2 / 2 ;
            if ( relpos < 0 )
               relpos = 0;
            if ( relpos > ( objcoordinates[i].y2 - objcoordinates[i].y1 - objcoordinates[i].t2 + 1 ) )
               relpos = objcoordinates[i].y2 - objcoordinates[i].y1 - objcoordinates[i].t2 + 1;

            if ( relpos != objcoordinates[i].t1  ) {
               int num = pos2num ( relpos );
               if ( num != toproduce[i]  ) 
                  checknewval ( i, num );
            }

            if (  tp == toproduce[i] && repnt )
               paintobj ( i, 0 );


         }
       } /* endfor */

       if ( objpressedbymouse ( 9 ) ) {
          produce();
          for ( i = 0; i < waffenanzahl; i++ ) {
             objcoordinates[i].t1 = 0;
             paintobj ( i, 0 );
          }
          displayusage();

       }

   }
}

void  ccontainer_b :: cammunitionproduction_subwindow :: checkforkey ( tkey taste )
{
  if ( taste == ct_right  || taste==ct_6k ) {
     int olds = actschieber;
     if ( actschieber < 7 )
        actschieber++;
     else
        actschieber = 0;

     paintobj ( olds, 0 );
     paintobj ( actschieber, 0 );

  }
  if ( taste == ct_left || taste==ct_4k) {
     int olds = actschieber;
     if ( actschieber > 0 )
        actschieber--;
     else
        actschieber = 7;

     paintobj ( olds, 0 );
     paintobj ( actschieber, 0 );
  }
  if ( taste == ct_up  || taste==ct_8k) 
      if ( toproduce[actschieber] < maxproduceablenum ) 
         checknewval ( actschieber, toproduce[actschieber] + 1 );
  
  if ( taste == ct_down  || taste==ct_2k )
      if ( toproduce[actschieber] > 0 ) 
         checknewval ( actschieber, toproduce[actschieber] - 1 );

  if ( taste == ct_enter || taste == ct_enterk ) { 
     produce();
     for ( int i = 0; i < waffenanzahl; i++ ) {
        objcoordinates[i].t1 = 0;
        paintobj ( i, 0 );
     }
     displayusage();
  }

}



//............................................................................................


ccontainer_b :: cresourceinfo_subwindow :: cresourceinfo_subwindow ( void )
{                                     
   strcpy ( name, "resource info" );
   laschpic1 = icons.container.lasche.a.resourceinfo[0];
   laschpic2 = icons.container.lasche.a.resourceinfo[1];
   recalc = 1;
}

int  ccontainer_b :: cresourceinfo_subwindow :: subwin_available ( void )
{
   cbuildingsubwindow :: subwin_available ( );
   if ( next )
     next->subwin_available ();

   return 0;
}

const char* resourceinfotxt[4]  = {  "avail", "capacity", "plus", "usage" };
const char* resourceinfotxt2[3] = {  "local", "net",      "global" };

void  ccontainer_b :: cresourceinfo_subwindow :: display ( void )
{
  int y;

   npush ( activefontsettings );
   setinvisiblemouserectanglestk ( subwinx1, subwiny1, subwinx2, subwiny2 );
   putimage ( subwinx1, subwiny1, icons.container.subwin.resourceinfo.main );
   csubwindow :: display();
   activefontsettings.color = white;
   activefontsettings.font = schriften.guifont;
   activefontsettings.length = 0;
   activefontsettings.justify = lefttext;
   activefontsettings.background = 255;

   for (y = 0; y < 4; y++ )
      showtext2c ( resourceinfotxt[y], subwinx1 + 5, subwiny1 + 58 + y * 13 );

   activefontsettings.length = 40;
   activefontsettings.justify = centertext;
   for ( y = 0; y < 3; y++ ) 
      showtext2c ( resourceinfotxt2[y], subwinx1 + 77 + y * 99, subwiny1 + 27 );


   displayvariables ();

   getinvisiblemouserectanglestk (  );
   npop ( activefontsettings );

}
                                                                // frher X         frher: mode
int  ccontainer_b :: cresourceinfo_subwindow :: getvalue ( int resourcetype, int y, int scope )
{
   /*
   if ( mode == 0 ) {
      switch ( y ) {
      case 0: if ( x == 0 )
                 return cc_b->building->actstorage.energy;
              if ( x == 1 )
                 return cc_b->building->actstorage.material;
              if ( x == 2 )
                 return cc_b->building->actstorage.fuel;
         break;
      case 1: return cc_b->building->typ->gettank(x);
      case 2: if ( x == 0 )
                 return cc_b->building->getenergyplus ( -2 );
              if ( x == 1 )
                 return cc_b->building->getmaterialplus ( -2 );
              if ( x == 2 )
                 return cc_b->building->getfuelplus ( -2 );
         break;
      case 3: {
                 tresources res;
                 cc_b->building->getresourceusage ( &res );
                 return res.resource[x];
              }
      } 
   }
   if ( mode == 1 ) {
      switch ( y ) {
         case 0: return cc_b->building->get_energy ( maxint, x );
         case 1: {  // tank
                    GetResourceCapacity grc;
                    return grc.getresource ( 
                   return cc_b->building->getenergy ( maxint, x+64 );      
         case 2: {  // plus
                    return cc_b->building->getenergy ( maxint, x+16 );      
         case 3: {  // usage
                     return cc_b->building->getenergy ( maxint, x+32 );      
      } 
   }
   if ( mode == 2 ) {
      if ( recalc ) {
         memset ( &resource, 0, sizeof( resource ));
         pbuilding bld = actmap->player[actmap->actplayer].firstbuilding;
         while ( bld ) {
           comment on
            resource.avail.energy += bld->energy;
            resource.avail.material += bld->material;
            resource.avail.fuel += bld->sprit;

            resource.tank.energy += bld->typ->energytank;
            resource.tank.material += bld->typ->materialtank;
            resource.tank.fuel += bld->typ->fueltank;

            resource.plus.a.energy   += bld->plus.a.energy;
            resource.plus.a.material += bld->plus.a.material;
            resource.plus.a.fuel     += bld->plus.a.fuel;
           comment off 

            resource.r[0][0] += bld->actstorage.energy;
            resource.r[0][1] += bld->actstorage.material;
            resource.r[0][2] += bld->actstorage.fuel;

            resource.r[1][0] += bld->typ->gettank(0);
            resource.r[1][1] += bld->typ->gettank(1);
            resource.r[1][2] += bld->typ->gettank(2);

            resource.r[2][0] += bld->getenergyplus ( -2 );
            resource.r[2][1] += bld->getmaterialplus ( -2 );
            resource.r[2][2] += bld->getfuelplus ( -2 );

            tresources res;
            bld->getresourceusage ( &res );
            resource.r[3][0] += res.resource[0];
            resource.r[3][1] += res.resource[1];
            resource.r[3][2] += res.resource[2];

            for ( int i = 0; i < 4; i++ )
               for ( int j = 0; j < 3; j++ )
                  if ( resource.r[i][j] < 0 )
                     resource.r[i][j] = maxint;

            bld = bld->next;
         }
         recalc = 0;
      }
      return resource.r[y][x];
   }
   return 0;
   */

  switch ( y ) {
         case 0: {  // avail 
                        GetResource gr;
                        return gr.getresource ( cc_b->building->xpos, cc_b->building->ypos, resourcetype, maxint, 1, cc_b->building->color/8, scope );
                 }
         case 1: {  // tank
                        GetResourceCapacity grc;
                        return grc.getresource ( cc_b->building->xpos, cc_b->building->ypos, resourcetype, maxint, 1, cc_b->building->color/8, scope );
                 }
         case 2: {  // plus
                        GetResourcePlus grp;
                        return grp.getresource ( cc_b->building->xpos, cc_b->building->ypos, resourcetype, cc_b->building->color/8, scope );
                 }
         case 3: {  // usage
                        GetResourceUsage gru;
                        return gru.getresource ( cc_b->building->xpos, cc_b->building->ypos, resourcetype, cc_b->building->color/8, scope );
                 }
  } /* endswitch */
  return -1;
}


void ccontainer_b :: cresourceinfo_subwindow :: displayvariables( void )
{
   activefontsettings.color = white;
   activefontsettings.font = schriften.guifont;
   activefontsettings.length = 29;
   activefontsettings.justify = righttext;
   activefontsettings.height = 0;
   activefontsettings.background = bkgrdarkcol;

   char buf[50];

   for ( int c = 0; c < 3; c++ )
      for ( int x = 0; x < 3; x++ )
         for ( int y = 0; y < 4; y++ ) 
            showtext2c ( int2string ( getvalue ( x, y, c ), buf ), subwinx1 + 49 + ( c * 3 + x ) * 33, subwiny1 + 57 + y * 13 );

   activefontsettings.length = 0;
   activefontsettings.length = 100;
   recalc = 1;
}



void  ccontainer_b :: cresourceinfo_subwindow :: checkformouse ( void )
{
}

void  ccontainer_b :: cresourceinfo_subwindow :: checkforkey ( tkey taste )
{

}


void ccontainer_b :: cresourceinfo_subwindow :: resetammo ( int mode )
{
   if ( mode == 0 )
      (*firstavailable)->resetammo(1);
   else {
      if ( hostcontainer->actsubwindow == this )
         displayvariables();

      if ( nextavailable )
         nextavailable->resetammo ( mode );
   }
}

void ccontainer_b :: cresourceinfo_subwindow :: resetresources ( int mode )
{
   if ( mode == 0 )
      (*firstavailable)->resetresources(1);
   else {
      if ( hostcontainer->actsubwindow == this )
         displayvariables();

      if ( nextavailable )
         nextavailable->resetresources ( mode );
   }
}

//............................................................................................

int ccontainer_b :: cresearch_subwindow :: allbuildings = 0;

ccontainer_b :: cresearch_subwindow :: cresearch_subwindow ( void )
{
   strcpy ( name, "research" );
   laschpic1 = icons.container.lasche.a.research[0];
   laschpic2 = icons.container.lasche.a.research[1];
   materialcolor = 125;
   energycolor = 232;
   objcoordinates[0].x1 = subwinx1 + 117;
   objcoordinates[0].y1 = subwiny1 +  95;
   objcoordinates[0].x2 = subwinx1 + 146;
   objcoordinates[0].y2 = subwiny1 + 108;
   objcoordinates[0].type = 17;
}

int  ccontainer_b :: cresearch_subwindow :: subwin_available ( void )
{
   if ( (hostcontainer->getspecfunc ( mbuilding ) & cgresearchb) && ( cc_b->building->maxresearchpoints ))
      cbuildingsubwindow :: subwin_available ( );
   if ( next )
      next->subwin_available ();

   return 0;
}

void  ccontainer_b :: cresearch_subwindow :: display ( void )
{
   if ( cc_b->building->maxresearchpoints )
      research = 1024 * cc_b->building->researchpoints / cc_b->building->maxresearchpoints;
   else
      research = 0;

   setinvisiblemouserectanglestk ( subwinx1, subwiny1, subwinx2, subwiny2 );

   npush ( activefontsettings );

   putimage ( subwinx1, subwiny1, icons.container.subwin.research.main );

   csubwindow :: display();
   activefontsettings.color = white;
   activefontsettings.font = schriften.guifont;
   activefontsettings.length = 0;
   activefontsettings.justify = lefttext;
   activefontsettings.height = 0;
   showtext2c ( "research points:", subwinx1 + 8, subwiny1 + 25 );

   showtext2c ( "energy cost:",     subwinx1 + 8, subwiny1 + 43 );
   showtext2c ( "material cost:",   subwinx1 + 8, subwiny1 + 61 );

   showtext2c ( "avail in:",        subwinx1 + 8, subwiny1 + 79 );

//   showtext2 ( "act. technology:", subwinx1 + 195, subwiny1 + 4 );
   showtext2c ( "change all buildings:",subwinx1+8,subwiny1 + 98 );



   gx1 = subwinx1 + 181;
   gy1 = subwiny1 + 104 - 70;
   gx2 = subwinx1 + 181 + 150;
   gy2 = subwiny1 + 104;


   activefontsettings.justify = lefttext;
   activefontsettings.height = 0;
   activefontsettings.background = 255;
   if ( actmap->player[actmap->actplayer].research.activetechnology ) {
      showtext2c ( actmap->player[actmap->actplayer].research.activetechnology->name, subwinx1 + 195, subwiny1 + 4 );
      // showtext2 ( "avail in:", subwinx1 + 130, subwiny1 +        1 + abstand2 + abstand1 + abstand2 + abstand1 );
   } else
      showtext2c ( "none", subwinx1 + 195, subwiny1 + 4  );

   npop ( activefontsettings );

   displayvariables();
   getinvisiblemouserectanglestk();
}


void ccontainer_b :: cresearch_subwindow :: setnewresearch ( int res )
{
   int x = gx1 + ( gx2 - gx1 ) * research / 1024;

   int cl ;
   if ( x == gx1 )
      cl = 247;
   else
      if ( x == gx1+1 )
         cl = 244;
      else
         cl = 201;

   line( x, gy1, x, gy2-1, cl );

   research = res;

   if ( allbuildings ) {
      pbuilding bld = actmap->player[actmap->actplayer].firstbuilding;
      while ( bld ) {
         if ( bld->typ->special & cgresearchb ) {
            bld->researchpoints = bld->maxresearchpoints * research/1024;
            if ( bld->researchpoints > bld->maxresearchpoints )
               bld->researchpoints = bld->maxresearchpoints;
          }
          bld=bld->next;
      }
   } else {
      pbuilding bld = cc_b->building;
      bld->researchpoints = bld->maxresearchpoints * research/1024;
      if ( bld->researchpoints > bld->maxresearchpoints )
         bld->researchpoints = bld->maxresearchpoints;
   }
}


void  ccontainer_b :: cresearch_subwindow :: displayvariables ( void )
{
  int x;

   npush ( activefontsettings );
   activefontsettings.color = white;
   activefontsettings.font = schriften.guifont;
   activefontsettings.length = 28;
   activefontsettings.justify = righttext;
   activefontsettings.height = 0;
   activefontsettings.background = 201;

   int energy;
   int material;
   returnresourcenuseforresearch ( cc_b->building, cc_b->building->researchpoints, &energy, &material );

   showtext2c ( strrr( cc_b->building->researchpoints ), subwinx1 + 115, subwiny1 + 25 );

   showtext2c ( strrr( energy ), subwinx1 + 115, subwiny1 + 43 );
   showtext2c ( strrr( material ), subwinx1 + 115, subwiny1 + 61 );


   showtext2c ( "avail in:",        subwinx1 + 8, subwiny1 + 79 );

   int rppt = 0;
   pbuilding bld = actmap->player[actmap->actplayer].firstbuilding;
   while ( bld ) {
      rppt += bld->researchpoints;
      bld=bld->next;
   } /* endwhile */

   if ( rppt  && actmap->player[actmap->actplayer].research.activetechnology ) {
      showtext2c ( strrr( (actmap->player[actmap->actplayer].research.activetechnology->researchpoints - actmap->player[actmap->actplayer].research.progress + rppt-1) / rppt ),  subwinx1 + 115, subwiny1 + 79 );
   } else
      bar ( subwinx1 + 115, subwiny1 + 79, subwinx1 + 115 + activefontsettings.length, subwiny1 + 79 + activefontsettings.font->height, activefontsettings.background );

   activefontsettings.justify = centertext;
   activefontsettings.length = 22;
   activefontsettings.background = 255;


   putimage ( subwinx1 + 117, subwiny1 + 95, icons.container.subwin.research.button[0] );
   if ( allbuildings ) 
      showtext2c ( "yes", subwinx1+120, subwiny1 + 98 );
   else 
      showtext2c ( "no",  subwinx1+120, subwiny1 + 98 );
   

                                                              // ->typ ??
   returnresourcenuseforresearch ( cc_b->building, cc_b->building->maxresearchpoints, &energy, &material );
   int max;
   if ( energy > material )
      max = energy * 17/16;
   else
      max = material * 17/16;

   int dist = gx2-gx1;
   for (x = dist; x >0 ; x--) {
       int res = cc_b->building->maxresearchpoints * x / dist;
       returnresourcenuseforresearch ( cc_b->building, res, &energy, &material );


       if ( max ) {
          putpixel ( gx1 + x, gy2 - ( gy2-gy1 ) * energy / max, energycolor );
          putpixel ( gx1 + x, gy2 - ( gy2-gy1 ) * material / max, materialcolor );
       }

   } /* endfor */
   x = gx1 + ( gx2 - gx1 ) * research / 1024;

   line( x, gy1, x, gy2-1, yellow );

   npop ( activefontsettings );
}


void  ccontainer_b :: cresearch_subwindow :: checkformouse ( void )
{
   if ( mouseparams.taste == 1 ) {
      if ( mouseinrect ( gx1, gy1, gx2, gy2 ) ) {
         int newresearch = 1024 * (mouseparams.x-gx1) / (gx2-gx1);
         if ( newresearch < 0 )
            newresearch = 0;
         if ( newresearch > 1024 )
            newresearch = 1024;
         if ( newresearch != research ) {
            setinvisiblemouserectanglestk ( subwinx1, subwiny1, subwinx2, subwiny2 );

            int x = gx1 + ( gx2 - gx1 ) * research/ 1024;

            int cl ;
            if ( x == gx1 )
               cl = 247;
            else
               if ( x == gx1+1 )
                  cl = 244;
               else
                  cl = 201;
            line( x, gy1, x, gy2-1, cl );

            setnewresearch( newresearch );

            displayvariables();

            getinvisiblemouserectanglestk ( );

         }
      }
      if ( objpressedbymouse(0) ) {
         allbuildings = !allbuildings;
         setinvisiblemouserectanglestk ( subwinx1, subwiny1, subwinx2, subwiny2 );
         displayvariables();
         getinvisiblemouserectanglestk ( );
      }
   }
}

void  ccontainer_b :: cresearch_subwindow :: paintobj ( int num, int stat )
{
  if ( objcoordinates[0].type == 17 ) {

     setinvisiblemouserectanglestk ( objcoordinates[num].x1,   objcoordinates[num].y1,   objcoordinates[num].x2+10,   objcoordinates[num].y2 );

     activefontsettings.font = schriften.guifont;
     activefontsettings.height = 0;
     activefontsettings.justify = centertext;
     activefontsettings.length = 22;
     activefontsettings.background = 255;

     if ( stat == 0 ) {
        putimage ( objcoordinates[0].x1, objcoordinates[0].y1, icons.container.subwin.research.button[0] );
        if ( allbuildings ) 
           showtext2c ( "yes", subwinx1+120, subwiny1 + 98 );
        else 
           showtext2c ( "no",  subwinx1+120, subwiny1 + 98 );
     } else {
        putimage ( objcoordinates[0].x1, objcoordinates[0].y1, icons.container.subwin.research.button[1] );
        if ( allbuildings ) 
           showtext2c ( "yes", subwinx1+121, subwiny1 + 99 );
        else 
           showtext2c ( "no",  subwinx1+121, subwiny1 + 99 );
     }

     getinvisiblemouserectanglestk ( );
  }
}


void  ccontainer_b :: cresearch_subwindow :: checkforkey ( tkey taste )
{
  if ( taste == ct_space  ||  taste == ct_a ) {
     allbuildings = !allbuildings;
     setinvisiblemouserectanglestk ( subwinx1, subwiny1, subwinx2, subwiny2 );
     displayvariables();
     getinvisiblemouserectanglestk ( );
  }
  int keyspeed = 50;
  if ( (taste == ct_left || taste==ct_4k ) && research > 0 ) {
     setinvisiblemouserectanglestk ( subwinx1, subwiny1, subwinx2, subwiny2 );
     if ( research > keyspeed )
        setnewresearch ( research - keyspeed );
     else
        setnewresearch ( 0 );

     displayvariables();
     getinvisiblemouserectanglestk ( );
  }
  if ( (taste == ct_right  || taste==ct_6k) && research < 1024 ) {
     setinvisiblemouserectanglestk ( subwinx1, subwiny1, subwinx2, subwiny2 );
     if ( research+keyspeed < 1024 )
        setnewresearch ( research + keyspeed );
     else
        setnewresearch ( 1024 );

     displayvariables();
     getinvisiblemouserectanglestk ( );
  }
}


//............................................................................................

int ccontainer_b :: cminingstation_subwindow :: allbuildings = 0;

ccontainer_b :: cminingstation_subwindow :: cminingstation_subwindow ( void )
{
   strcpy ( name, "mining" );
   laschpic1 = icons.container.lasche.a.miningstation[0];
   laschpic2 = icons.container.lasche.a.miningstation[1];
   materialcolor = 125;
   energycolor = 232;
   objcoordinates[0].x1 = subwinx1 + 316;
   objcoordinates[0].y1 = subwiny1 +   2;
   objcoordinates[0].x2 = subwinx1 + 344;
   objcoordinates[0].y2 = subwiny1 +  17;
   objcoordinates[0].type = 17;
   mininginfo = NULL;



   helplist.num =  5;

   static tonlinehelpitem miningstationhelpitems[ 5]   = {{ 37 + subwinx1 , 23 + subwiny1 , 119 + subwinx1, 35 + subwiny1, 20120 },
                                                          { 37 + subwinx1 , 41 + subwiny1 , 119 + subwinx1, 53 + subwiny1, 20121 },
                                                          { 37 + subwinx1 , 59 + subwiny1 , 119 + subwinx1, 71 + subwiny1, 20122 },
                                                          { 37 + subwinx1 , 77 + subwiny1 , 119 + subwinx1, 89 + subwiny1, 20123 },
                                                          {178 + subwinx1 , 23 + subwiny1 , 344 + subwinx1,108 + subwiny1, 20124 }};

   helplist.item = miningstationhelpitems;



}

int  ccontainer_b :: cminingstation_subwindow :: subwin_available ( void )
{

   mode = 0;
   if ( hostcontainer->getspecfunc ( mbuilding ) & cgminingstationb )  {
      if ( hostcontainer->getspecfunc ( mbuilding ) & cgmaterialproductionb )
         mode = 1;
      if ( hostcontainer->getspecfunc ( mbuilding ) & cgfuelproductionb )
         mode = 2;
      if ( mode )
         cbuildingsubwindow :: subwin_available ( );
   }

   if ( next )
      next->subwin_available ();

   return 0;
}

void  ccontainer_b :: cminingstation_subwindow :: display ( void )
{

   if ( mode == 1 ) {
      if ( cc_b->building->maxplus.a.material )
         extraction = 1024 * cc_b->building->plus.a.material / cc_b->building->maxplus.a.material;
      else
         extraction = 0;
   } else {
      if ( cc_b->building->maxplus.a.fuel )
         extraction = 1024 * cc_b->building->plus.a.fuel / cc_b->building->maxplus.a.fuel;
      else
         extraction = 0;
   }

   setinvisiblemouserectanglestk ( subwinx1, subwiny1, subwinx2, subwiny2 );

   npush ( activefontsettings );

   putimage ( subwinx1, subwiny1, icons.container.subwin.miningstation.main );

   csubwindow :: display();

   putimage( subwinx1 + 39, subwiny1 + 25, icons.container.subwin.miningstation.resource[2 - mode ] );

/*   activefontsettings.color = white;
   activefontsettings.font = schriften.guifont;
   activefontsettings.length = 0;
   activefontsettings.justify = lefttext;
   activefontsettings.height = 0;
   showtext2c ( "extraction:",      subwinx1 + 63, subwiny1 + 25 );

//   showtext2c ( "energy cost:",     subwinx1 + 8, subwiny1 + 43 );
//   showtext2c ( "material cost:",   subwinx1 + 8, subwiny1 + 61 );

   showtext2c ( "distance:",        subwinx1 + 63, subwiny1 + 81 );

//   showtext2 ( "act. technology:", subwinx1 + 195, subwiny1 + 4 );

   showtext2c ( "change buildings:", subwinx1 + 180, subwiny1 + 5 );
*/


   gx1 = subwinx1 + 181;
   gy1 = subwiny1 + 104 - 70;
   gx2 = subwinx1 + 181 + 150;
   gy2 = subwiny1 + 104;


   activefontsettings.justify = lefttext;
   activefontsettings.height = 0;
   activefontsettings.background = 255;

   npop ( activefontsettings );



   if ( !mininginfo ) {

      tgetmininginfo gmi;

      gmi.run ( cc_b->building );

      mininginfo = gmi.mininginfo;
   }


   int max = mininginfo->efficiency[0] * 11 / 10;
   for ( int i = 0; i < maxminingrange; i++ ) {
      int y = gy2 - ( gy2 - gy1 ) * mininginfo->efficiency[i] / max;
      int xd = (gx2-gx1) / maxminingrange ;
      bar ( gx1 + i * xd , y, gx1 + (i+1) * xd, gy2, 160 + 15 * mininginfo->avail[ i ].resource[ mode ] / mininginfo->max[ i ].resource[ mode ] );
   }
   
   displayvariables();
   getinvisiblemouserectanglestk();

}


void ccontainer_b :: cminingstation_subwindow :: setnewextraction ( int res )
{
   int x = gx1 + ( gx2 - gx1 ) * extraction / 1024;

   int cl ;
   if ( x == gx1 )
      cl = 247;
   else
      if ( x == gx1+1 )
         cl = 244;
      else
         cl = 201;

   line( x, gy1, x, gy2-1, cl );

   extraction = res;

   if ( allbuildings ) {
      pbuilding bld = actmap->player[actmap->actplayer].firstbuilding;
      while ( bld ) {
         if ( (bld->typ->special & cgminingstationb) && (( bld->typ->special & cgmaterialproductionb && mode == 1) || ( bld->typ->special & cgfuelproductionb && mode == 2))) {
            bld->plus.resource[mode] = bld->maxplus.resource[mode] * extraction/1024;
            if ( bld->plus.resource[mode] > bld->maxplus.resource[mode] )
               bld->plus.resource[mode] = bld->maxplus.resource[mode];
          }
          bld=bld->next;
      }
   } else {
      pbuilding bld = cc_b->building;
      bld->plus.resource[mode] = bld->maxplus.resource[mode] * extraction/1024;
      if ( bld->plus.resource[mode] > bld->maxplus.resource[mode] )
         bld->plus.resource[mode] = bld->maxplus.resource[mode];
   }
}


void  ccontainer_b :: cminingstation_subwindow :: displayvariables ( void )
{
  int i;

   npush ( activefontsettings );
   activefontsettings.color = white;
   activefontsettings.font = schriften.guifont;
   activefontsettings.length = 52;
   activefontsettings.justify = righttext;
   activefontsettings.height = 0;
   activefontsettings.background = 201;

   char c[100];

   if ( mode == 1 )
      strcpy ( c, strrr ( cc_b->building->getmaterialplus( 0 ) ));
   else
      strcpy ( c, strrr ( cc_b->building->getfuelplus( 0 ) ));         // aktualle F”rderung

   strcat ( c , " / ");
   strcat ( c, strrr ( cc_b->building->plus.resource[mode] ));         // maximale F”rderung

   showtext2c ( c,               subwinx1 + 63, subwiny1 + 25 );



   int dist = 100;
   for (i = maxminingrange; i >= 0; i-- )
      if ( mininginfo->avail[ i ].resource[ mode ] )
         dist = i;

   strcpy ( c, strrr ( cc_b->building->lastmineddist ));
   strcat ( c , " / ");
   strcat ( c, strrr ( dist ));
   showtext2c ( c ,                                                 subwinx1 + 63, subwiny1 + 43 );

   if ( mode == 1 ) {
      if ( !cc_b->building->typ->efficiencymaterial )
         displaymessage(" the %s has a material efficiency of 0 !", 2, cc_b->building->typ->name );
      i = cc_b->building->getmininginfo ( mode ) * 1024 / cc_b->building->typ->efficiencymaterial;
   } else {
      if ( !cc_b->building->typ->efficiencyfuel )
         displaymessage(" the %s has a fuel efficiency of 0 !", 2, cc_b->building->typ->name );
      i = cc_b->building->getmininginfo ( mode ) * 1024 / cc_b->building->typ->efficiencyfuel;
   }
 
   int t = 0;
   if ( i ) 
       t = mininginfo->avail [ dist ].resource[ mode ] / i;         // in wieviel Runden wird n„chste Entfernung erreicht 

   showtext2c ( strrr ( t ),                                        subwinx1 + 63, subwiny1 + 61 );


   if ( mode == 1 )
      i =  cc_b->building->typ->efficiencymaterial;
   else
      i =  cc_b->building->typ->efficiencyfuel;

   showtext2c ( strrr ( i ),                                        subwinx1 + 63, subwiny1 + 79 );

//   showtext2c ( "energy cost:",     subwinx1 + 8, subwiny1 + 43 );
//   showtext2c ( "material cost:",   subwinx1 + 8, subwiny1 + 61 );


/*   int energy;
   int material;
   returnresourcenuseforresearch ( cc_b->building, cc_b->building->researchpoints, &energy, &material );

   showtext2c ( strrr( cc_b->building->researchpoints ), subwinx1 + 115, subwiny1 + 25 );

   showtext2c ( strrr( energy ), subwinx1 + 115, subwiny1 + 43 );
   showtext2c ( strrr( material ), subwinx1 + 115, subwiny1 + 61 );


   showtext2c ( "avail in:",        subwinx1 + 8, subwiny1 + 79 );

   int rppt = 0;
   pbuilding bld = actmap->player[actmap->actplayer].firstbuilding;
   while ( bld ) {
      rppt += bld->researchpoints;
      bld=bld->next;
   } 

   if ( rppt  && actmap->player[actmap->actplayer].research.activetechnology ) {
      showtext2c ( strrr( (actmap->player[actmap->actplayer].research.activetechnology->researchpoints - actmap->player[actmap->actplayer].research.progress + rppt-1) / rppt ),  subwinx1 + 115, subwiny1 + 79 );
   } else
      bar ( subwinx1 + 115, subwiny1 + 79, subwinx1 + 115 + activefontsettings.length, subwiny1 + 79 + activefontsettings.font->height, activefontsettings.background );

   activefontsettings.justify = centertext;
   activefontsettings.length = 22;
   activefontsettings.background = 255;



   paintobj ( 0, 0 );


   returnresourcenuseforresearch ( cc_b->building, cc_b->building->typ->maxresearchpoints, &energy, &material );
   int max;
   if ( energy > material )
      max = energy * 17/16;
   else
      max = material * 17/16;

   int dist = gx2-gx1;
   for (int x = dist; x >0 ; x--) {
       int res = cc_b->building->maxresearchpoints * x / dist;
       returnresourcenuseforresearch ( cc_b->building, res, &energy, &material );


       if ( max ) {
          putpixel ( gx1 + x, gy2 - ( gy2-gy1 ) * energy / max, energycolor );
          putpixel ( gx1 + x, gy2 - ( gy2-gy1 ) * material / max, materialcolor );
       }

   } 
   x = gx1 + ( gx2 - gx1 ) * research / 1024;

   line( x, gy1, x, gy2-1, yellow );

   */
   npop ( activefontsettings );
}


void  ccontainer_b :: cminingstation_subwindow :: checkformouse ( void )
{
   if ( mouseparams.taste == 1 ) {
      /*
      if ( mouseinrect ( gx1, gy1, gx2, gy2 ) ) {
         int newresearch = 1024 * (mouseparams.x-gx1) / (gx2-gx1);
         if ( newresearch < 0 )
            newresearch = 0;
         if ( newresearch > 1024 )
            newresearch = 1024;
         if ( newresearch != research ) {
            setinvisiblemouserectanglestk ( subwinx1, subwiny1, subwinx2, subwiny2 );

            int x = gx1 + ( gx2 - gx1 ) * research/ 1024;

            int cl ;
            if ( x == gx1 )
               cl = 247;
            else
               if ( x == gx1+1 )
                  cl = 244;
               else
                  cl = 201;
            line( x, gy1, x, gy2-1, cl );

            setnewresearch( newresearch );

            displayvariables();

            getinvisiblemouserectanglestk ( );

         }
      }
      */
      if ( objpressedbymouse(0) ) {
         if ( allbuildings < 2 )
            allbuildings++;
         else
            allbuildings = 0;

         paintobj( 0, 0 );
      }
   }
}

void  ccontainer_b :: cminingstation_subwindow :: paintobj ( int num, int stat )
{
   /*
  if ( objcoordinates[0].type == 17 ) {

     setinvisiblemouserectanglestk ( objcoordinates[num].x1,   objcoordinates[num].y1,   objcoordinates[num].x2+10,   objcoordinates[num].y2 );

     activefontsettings.font = schriften.guifont;
     activefontsettings.height = 0;
     activefontsettings.justify = centertext;
     activefontsettings.length = 22;
     activefontsettings.background = 255;

     int x;
     int y;
     if ( stat == 0 ) {
        putimage ( objcoordinates[0].x1, objcoordinates[0].y1, icons.container.subwin.miningstation.button[0] );
        x = 319;
        y =   5;
     } else {
        putimage ( objcoordinates[0].x1, objcoordinates[0].y1, icons.container.subwin.miningstation.button[1] );
        x = 320;
        y =   6;
     }

     if ( allbuildings == 0 ) 
        showtext2c ( "all", subwinx1+x, subwiny1 + y );
     else 
        if ( allbuildings == 1 )
           if ( mode == 1 )
              showtext2c ( "mat.", subwinx1+x, subwiny1 + y );
           else
              showtext2c ( "fuel", subwinx1+x, subwiny1 + y );
        else
           showtext2c ( "this", subwinx1+x, subwiny1 + y );

     getinvisiblemouserectanglestk ( );
  }
  */
}


void  ccontainer_b :: cminingstation_subwindow :: checkforkey ( tkey taste )
{
   /*
  if ( taste == ct_space  ||  taste == ct_a ) {
     if ( allbuildings < 2 )
        allbuildings++;
     else
        allbuildings = 0;

     paintobj( 0, 0 );
  }
  
  int keyspeed = 50;
  if ( (taste == ct_left || taste==ct_4k)  && research > 0 ) {
     setinvisiblemouserectanglestk ( subwinx1, subwiny1, subwinx2, subwiny2 );
     if ( research > keyspeed )
        setnewresearch ( research - keyspeed );
     else
        setnewresearch ( 0 );

     displayvariables();
     getinvisiblemouserectanglestk ( );
  }
  if ( (taste == ct_right || taste==ct_6k) && research < 1024 ) {
     setinvisiblemouserectanglestk ( subwinx1, subwiny1, subwinx2, subwiny2 );
     if ( research+keyspeed < 1024 )
        setnewresearch ( research + keyspeed );
     else
        setnewresearch ( 1024 );

     displayvariables();
     getinvisiblemouserectanglestk ( );
  }
  */
}

ccontainer_b :: cminingstation_subwindow :: ~cminingstation_subwindow ()
{
  if ( mininginfo ) {
     delete mininginfo;
     mininginfo = NULL;
  }
}



// GUI

                






int      ccontainer_b :: repairicon_cb :: checkto  (pvehicle eht, char newdamage)
{
   return cbuildingcontrols :: crepairunitinbuilding :: checkto ( eht, newdamage );
}


ccontainer_b :: trainuniticon_cb :: trainuniticon_cb ( void )
{
   strcpy ( filename, "training" );
};

int   ccontainer_b :: trainuniticon_cb :: available    ( void ) 
{
   if ( main->unitmode != mnormal )
      return 0;

   pvehicle eht = main->getmarkedunit(); 
   if ( eht && eht->color == actmap->actplayer * 8 ) 
      return cbuildingcontrols :: ctrainunit :: available ( eht );

   return 0;
}

void  ccontainer_b :: trainuniticon_cb :: exec         ( void ) 
{
  trainunit ( main->getmarkedunit() );
  dashboard.x = 0xffff;
  main->setactunittogray();
  main->repaintammo = 1;
}



ccontainer_b :: dissectuniticon_cb :: dissectuniticon_cb ( void )
{
   strcpy ( filename, "sezieren" );
};

int   ccontainer_b :: dissectuniticon_cb :: available    ( void ) 
{
   pvehicle eht = main->getmarkedunit(); 
   if ( eht && eht->color == actmap->actplayer * 8) 
      return cbuildingcontrols :: cdissectunit :: available ( eht );

   return 0;
}

void  ccontainer_b :: dissectuniticon_cb :: exec         ( void ) 
{
  pvehicle eht = main->getmarkedunit();
  dissectunit ( eht );
  main->movemark (repaint);
  dashboard.x = 0xffff;
}



void  ccontainer_b :: fill_dialog_icon_cb :: exec         ( void ) 
{
   verlademunition ( main->getmarkedunit(), NULL, cc_b->building, 0 ); 
   dashboard.x = 0xffff;
   main->repaintresources = 1;
}



int   ccontainer_b :: fill_icon_cb :: available    ( void ) 
{
   pvehicle eht = main->getmarkedunit(); 
   if ( eht && eht->color == actmap->actplayer * 8) {
      if ( eht->material < eht->typ->material )
        return 1;
      if ( eht->fuel < eht->typ->tank )
        return 1;
      for (int i = 0; i < eht->typ->weapons->count; i++) 
         if ( eht->typ->weapons->weapon[ i ].typ & ( cwweapon | cwmineb ) ) 
            if ( eht->ammo[i] < eht->typ->weapons->weapon[ i ].count )
               return 1;
   }


   return 0;
}




ccontainer_b :: produceuniticon_cb :: produceuniticon_cb ( void )
{
   strcpy ( filename, "produnit" );
};

int   ccontainer_b :: produceuniticon_cb :: available    ( void ) 
{
   if ( cc_b->building->color == actmap->actplayer * 8 ) {
      if ( main->unitmode == mnormal ) {
         if ( main->getspecfunc ( mbuilding ) & cgvehicleproductionb ) {
            pvehicle eht = main->getmarkedunit(); 
            if ( eht ) 
               return 0;
            else
               return 1;
     
         } 
      } else 
         if ( main->getmarkedunittype() )
            if (  cbuildingcontrols :: cproduceunit :: available ( main->getmarkedunittype() ) )
               return 1;
   }
   return 0;
}

void  ccontainer_b :: produceuniticon_cb :: exec         ( void ) 
{
   if ( main->unitmode == mnormal ) {

      main->unitmode = mproduction;
      main->unitchanged();

   } else {

      produce  ( main->getmarkedunittype() );
      main->unitmode = mnormal;

   }
   main->setpictures();
   main->movemark ( repaintall );
   dashboard.x = 0xffff;
}


char* ccontainer_b :: produceuniticon_cb :: getinfotext  ( void )
{
   pvehicletype fzt = main->getmarkedunittype();
   if ( fzt ) {
      int en = fzt->production.energy;
      int ma = fzt->production.material;
      int fu = 0;

      if ( gameoptions.container.filleverything ) {
         int en1 = en;
         int ma1 = ma;
         int fu1 = fu;

         fu += fzt->tank;
         ma += fzt->material;

         strcpy ( &infotextbuf[100], infotext );
         sprintf ( &infotextbuf[100+strlen( &infotextbuf[100])], resourceusagestring, en, ma, fu );
         sprintf ( &infotextbuf[100+strlen( &infotextbuf[100])], "(empty: %d energy, %d material, %d fuel)", en1, ma1, fu1 );
      } else {
         strcpy ( &infotextbuf[100], infotext );
         sprintf ( &infotextbuf[100+strlen( &infotextbuf[100])], resourceusagestring, en, ma, fu );
      }
      return &infotextbuf[100];
   } else {
      return infotext;
   }
}





ccontainer :: productioncancelicon_cb :: productioncancelicon_cb ( void )
{
   strcpy ( filename, "cancel" );
};

int   ccontainer :: productioncancelicon_cb :: available    ( void ) 
{
    if ( main->unitmode != mnormal ) 
       return 1;
    return 0;
}

void  ccontainer :: productioncancelicon_cb :: exec         ( void ) 
{
   main->unitmode = mnormal;
   main->setpictures();
   main->movemark ( repaintall );
   dashboard.x = 0xffff;
}




ccontainer_b :: recyclingicon_cb :: recyclingicon_cb ( void )
{
    strcpy ( filename, "c_recycl" );
};

int  ccontainer_b :: recyclingicon_cb :: available ( void )
{
   pvehicle eht = main->getmarkedunit ();
   if ( eht && eht->color == actmap->actplayer * 8 )
        return 1;
   else
        return 0;
};   

void    ccontainer_b :: recyclingicon_cb :: exec ( void )
{
    recycle ( main->getmarkedunit() );
    main->movemark (repaint);
    dashboard.x = 0xffff;
};



char* ccontainer_b :: recyclingicon_cb :: getinfotext  ( void )
{
   resourceuse ( main->getmarkedunit() );
   strcpy ( &infotextbuf[100], infotext );
   int n = 0;
   sprintf ( &infotextbuf[100+strlen( &infotextbuf[100])], resourceusagestring, energy, -material, n );
   return &infotextbuf[100];
}






ccontainer_b :: takeofficon_cb :: takeofficon_cb             ( void ) 
{
    strcpy ( filename, "takeoff" );
}


int   ccontainer_b :: takeofficon_cb :: available    ( void ) 
{
   return true;
}

void  ccontainer_b :: takeofficon_cb :: exec         ( void ) 
{
   return;
}











/***********************************************************************************/
/*                                                                                 */
/*   class    ccontainer_t : Geb„ude-Innereien                                     */
/*                                                                                 */
/***********************************************************************************/








ccontainer_t :: ctransportinfo_subwindow :: ctransportinfo_subwindow ( void )
{
   strcpy ( name, "transport info" );
   laschpic1 = icons.container.lasche.a.transportinfo[0];
   laschpic2 = icons.container.lasche.a.transportinfo[1];

   helplist.num = 3;

   static tonlinehelpitem transportinfohelpitems[3]    = {{246 + subwinx1 , 22 + subwiny1 , 266 + subwinx1,108 + subwiny1, 20130 },
                                                          {277 + subwinx1 , 22 + subwiny1 , 297 + subwinx1,108 + subwiny1, 20131 },
                                                          {308 + subwinx1 , 22 + subwiny1 , 328 + subwinx1,108 + subwiny1, 20132 }};


   helplist.item = transportinfohelpitems;

}

int  ccontainer_t :: ctransportinfo_subwindow :: subwin_available ( void )
{
   csubwindow :: subwin_available ( );
   if ( next )
     next->subwin_available ();

   return 0;
}

void  ccontainer_t :: ctransportinfo_subwindow :: display ( void )
{
   setinvisiblemouserectanglestk ( subwinx1, subwiny1, subwinx2, subwiny2 );

   npush ( activefontsettings );
   putimage ( subwinx1, subwiny1, icons.container.subwin.transportinfo.main );

   csubwindow :: display();

   for ( int i = 0; i < 8; i++ ) {
      if ( cc_t->vehicle->typ->loadcapability & ( 1 << ( 7 - i) ))
         putimage ( subwinx1 + 246, subwiny1 + 22 + i * 11, icons.container.subwin.transportinfo.height1[i] );
      else
         putimage ( subwinx1 + 246, subwiny1 + 22 + i * 11, icons.container.subwin.transportinfo.height2[i] );

      if ( cc_t->vehicle->typ->loadcapabilityreq & ( 1 << ( 7 - i ) ))
         putimage ( subwinx1 + 277, subwiny1 + 22 + i * 11, icons.container.subwin.transportinfo.height1[i] );
      else
         putimage ( subwinx1 + 277, subwiny1 + 22 + i * 11, icons.container.subwin.transportinfo.height2[i] );

      if ( cc_t->vehicle->typ->loadcapabilitynot & ( 1 << ( 7 - i ) ))
         putimage ( subwinx1 + 308, subwiny1 + 22 + i * 11, icons.container.subwin.transportinfo.height1[i] );
      else
         putimage ( subwinx1 + 308, subwiny1 + 22 + i * 11, icons.container.subwin.transportinfo.height2[i] );
   }
   activefontsettings.font = schriften.guifont;
   activefontsettings.justify = lefttext;
   activefontsettings.length = 0;
   showtext3c ( "~Transport:~",  subwinx1 + 8,  subwiny1 + 25 );
   showtext2c ( "act load:",        subwinx1 + 70,  subwiny1 + 25 );
   showtext2c ( "max load:",        subwinx1 + 70,  subwiny1 + 33 );
   showtext2c ( "free:",            subwinx1 + 70,  subwiny1 + 41 );


   showtext3c ( "~Loaded Unit:~",   subwinx1 + 8,   subwiny1 + 62 );

   showtext2c ( "unladen weight:",  subwinx1 + 70,  subwiny1 + 62 );
   showtext2c ( "material:",        subwinx1 + 70,  subwiny1 + 70 );
   showtext2c ( "fuel:",            subwinx1 + 70,  subwiny1 + 78 );
   showtext2c ( "loaded units:",    subwinx1 + 70,  subwiny1 + 86 );
   putimage ( subwinx1 + 70,  subwiny1 + 97, icons.container.subwin.transportinfo.sum );

   paintvariables();

   npop ( activefontsettings );
   getinvisiblemouserectanglestk (  );

}

void ccontainer_t :: ctransportinfo_subwindow :: paintvariables ( void )
{
   npush ( activefontsettings );

   activefontsettings.font = schriften.guifont;
   activefontsettings.length = 30;
   activefontsettings.justify = righttext;
   activefontsettings.background = 201;

   pvehicle eht = cc_t->vehicle;
   int mass = eht->cargo();
   int free = eht->typ->loadcapacity - mass;

   activefontsettings.length = 40;

   showtext2c ( strrr ( mass ),                          subwinx1 + 170,  subwiny1 + 25 );
   showtext2c ( strrr ( cc_t->vehicle->typ->loadcapacity ),  subwinx1 + 170,  subwiny1 + 33 );
   showtext2c ( strrr ( free ),                          subwinx1 + 170,  subwiny1 + 41 );

   eht = hostcontainer->getmarkedunit();
   if ( eht ) {
      showtext2c ( strrr ( eht->typ->weight ),                       subwinx1 + 170,  subwiny1 +  62 );
      showtext2c ( strrr ( eht->material * materialweight / 1024 ),  subwinx1 + 170,  subwiny1 +  70 );
      showtext2c ( strrr ( eht->fuel     * fuelweight     / 1024 ),  subwinx1 + 170,  subwiny1 +  78 );
      showtext2c ( strrr ( eht->cargo() ),                           subwinx1 + 170,  subwiny1 +  86 );
      showtext2c ( strrr ( eht->weight() ),                          subwinx1 + 170,  subwiny1 +  96 );
  } else {
      showtext2c ( "-",                       subwinx1 + 170,  subwiny1 +  62 );
      showtext2c ( "-",                       subwinx1 + 170,  subwiny1 +  70 );
      showtext2c ( "-",                       subwinx1 + 170,  subwiny1 +  78 );
      showtext2c ( "-",                       subwinx1 + 170,  subwiny1 +  86 );
      showtext2c ( "-",                       subwinx1 + 170,  subwiny1 +  96 );
  }
   line ( subwinx1 + 70,  subwiny1 +  95 , subwinx1 + 210,  subwiny1 +  95 , lightgray );


   npop ( activefontsettings );
}

void ccontainer_t :: ctransportinfo_subwindow :: unitchanged ( void )
{
   setinvisiblemouserectanglestk ( subwinx1 + 199, subwiny1 + 72, subwinx1 + 230, subwiny1 + 110 );
   paintvariables();
   getinvisiblemouserectanglestk (  );
}

void ccontainer_t :: ctransportinfo_subwindow :: checkformouse ( void )
{
}

void ccontainer_t :: ctransportinfo_subwindow :: checkforkey ( tkey taste )
{
}

//............................................................................................








ccontainer_t :: ccontainer_t ( void )
{
   oldguihost = actgui;
   actgui = &hosticons_ct;
   actgui->restorebackground();
   unitmode = mnormal;
}

void  ccontainer_t :: init (pvehicle eht)
{
    hosticons_ct.init ( hgmp->resolutionx, hgmp->resolutiony );
    hosticons_ct.seticonmains ( this );
    hosticons_ct.starticonload();
    

   if ( eht ) {
      int mss = getmousestatus();
      if ( mss == 2 )
         mousevisible ( false );

      ctransportcontrols :: init ( eht );

      setpictures();


      int x = vehicle->xpos;
      int y = vehicle->ypos;
/*      
      cursor.hide ();
      cursor.posx = x - actmap->xpos;
      cursor.posy = y - actmap->ypos;
      cursor.show ();
*/ 

      ccontainer :: init ( vehicle->typ->picture[0], vehicle->color, vehicle->name, vehicle->typ->description );
      ccontainer :: displayloading ();
      ccontainer :: movemark (repaint);

      subwindows.ammunitiontransfer.sethostcontainer ( this );
      subwindows.ammunitiontransfer.init();

      if ( mss == 2 )
         mousevisible ( true );
   }; 
};



pvehicle    ccontainer_t :: getmarkedunit (void)
{
   if ( unitmode == mnormal || unitmode == mloadintocontainer)
      return vehicle->loading[mark.y*unitsshownx + mark.x];
   else
      return NULL;
}



pvehicle ctransportcontrols :: getloadedunit (int num)
{
   return vehicle->loading[num];
};

pvehicle ccontainer_t :: getloadedunit (int num)
{
   if ( unitmode == mnormal || unitmode == mloadintocontainer )
      return vehicle->loading[num];
   else
      return NULL;
};



ccontainer_t :: ~ccontainer_t ( )
{
   actgui = oldguihost ;
}


int    ccontainer_t :: putammunition (int  weapontype, int  ammunition, int abbuchen)
{
   if ( abbuchen )
      repaintammo = 1;

   return ctransportcontrols :: putammunition ( weapontype, ammunition, abbuchen );
}


int    ccontainer_t :: getammunition ( int weapontype, int num, int abbuchen, int produceifrequired  )
{
   if ( abbuchen )
      repaintammo = 1;

   return ctransportcontrols :: getammunition ( weapontype, num, abbuchen, produceifrequired  );
}

int    ccontainer_t :: putmaterial (int m, int abbuchen  )
{
   if ( abbuchen )
      repaintresources = 1;
   return ctransportcontrols :: putmaterial ( m, abbuchen );
}

int    ccontainer_t :: putfuel (int f, int abbuchen  )
{
   if ( abbuchen )
      repaintresources = 1;
   return ctransportcontrols :: putfuel ( f, abbuchen );
}

int    ccontainer_t :: getenergy ( int need, int abbuchen )
{
   if ( abbuchen )
      repaintresources = 1;
   return ctransportcontrols :: getenergy ( need, abbuchen );
}

int    ccontainer_t :: getmaterial ( int need, int abbuchen )
{
   if ( abbuchen )
      repaintresources = 1;
   return ctransportcontrols :: getmaterial ( need, abbuchen );
}

int    ccontainer_t :: getfuel ( int need, int abbuchen )
{
   if ( abbuchen )
      repaintresources = 1;
   return ctransportcontrols :: getfuel ( need, abbuchen );
}



// Gui


void  ccontainer_t :: chosticons_ct :: init ( int resolutionx, int resolutiony )
{
   chainiconstohost ( &icons.movement );     //   muá erst eingesetzt werden !
   tguihost::init ( resolutionx, resolutiony );
}








// --------------- GUI ---------------------------------------------------------------------------------- 





                

void  ccontainer_t :: fill_dialog_icon_ct :: exec         ( void ) 
{
   verlademunition ( main->getmarkedunit(), cc_t->vehicle, NULL, 0 ); 
   dashboard.x = 0xffff;
   main->repaintresources = 1;
}




int   ccontainer_t :: fill_icon_ct :: available    ( void ) 
{
   if ( main->unitmode != mnormal )
      return 0;


   pvehicle eht = main->getmarkedunit(); 
   if ( eht && eht->color == actmap->actplayer * 8) {
      if ( eht->material < eht->typ->material )
        return 1;
      if ( eht->fuel < eht->typ->tank )
        return 1;
      for (int i = 0; i < eht->typ->weapons->count; i++) 
         if ( eht->typ->weapons->weapon[ i ].typ & ( cwweapon | cwmineb ) ) 
            if ( eht->ammo[i] < eht->typ->weapons->weapon[ i ].count )
               for (int j = 0; j < cc_t->vehicle->typ->weapons->count; j++) 
                  if ( cc_t->vehicle->typ->weapons->weapon[ j ].typ & ( cwweapon | cwmineb ) ) 
                     if ( cc_t->vehicle->ammo[j] )
                        if ( eht->typ->weapons->weapon[ i ].typ & ( cwweapon | cwmineb ) & cc_t->vehicle->typ->weapons->weapon[ j ].typ ) 
                           return 1;
   }


   return 0;
}




int      ccontainer_t :: repairicon_ct :: checkto  (pvehicle eht, char newdamage)
{
   return ctransportcontrols :: crepairunitintransport :: checkto ( eht, newdamage );
}














































//------------------------------------------------------------------------------- GENERAL_ICON

// static pgeneralicon_c   generalicon_c :: first = NULL;
   
generalicon_c::generalicon_c ( void ) 
{
   first = &buildingparamstack[recursiondepth].generalicon_c__first;

   next = *first;
   *first = this;
}


pnguiicon   generalicon_c::nxt( void )
{
   return next;
}
void      generalicon_c::setnxt   ( pnguiicon ts )
{
   next = (pgeneralicon_c) ts ;
}

pnguiicon   generalicon_c::frst( void )
{
   return *first;
}
void        generalicon_c::setfrst  ( pnguiicon ts )
{
   *first = (pgeneralicon_c) ts;
}


void        generalicon_c::setmain ( pcontainer maintemp )
{
    main = maintemp;
    if ( next )
       next->setmain ( maintemp );
}

generalicon_c:: ~generalicon_c ( )
{
    first = NULL;
}

void tcontaineronlinemousehelp :: checkforhelp ( void )
{
   if ( gameoptions.onlinehelptime )
      if ( ticker > lastmousemove+gameoptions.onlinehelptime )
         if ( active == 1 )
            if ( mouseparams.taste == 0 ) {
               if ( hostcontainer->actsubwindow )
                  if ( hostcontainer->actsubwindow->helplist.num )
                      checklist ( &hostcontainer->actsubwindow->helplist );
            }
   
}

tcontaineronlinemousehelp :: tcontaineronlinemousehelp ( pcontainer host )
{
   hostcontainer = host;
}
