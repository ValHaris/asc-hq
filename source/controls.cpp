/*! \file controls.cpp
   Controlling units (which is graudally moved to #vehicletype.cpp and #unitctrl.cpp );
   Things that are run when starting and ending someones turn
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
#include <cstring>
#include <math.h>
#include <stdarg.h>
#include <ctime>

#include "buildingtype.h"
#include "vehicletype.h"
#include "typen.h"

#include "basegfx.h"
#include "newfont.h"
#include "spfst.h"
#include "loaders.h"
#include "misc.h"
#include "controls.h"
#include "network.h"
#include "events.h"
#include "stack.h"
#include "dlg_box.h"
#include "dialog.h"
#include "attack.h"
#include "building.h"
#include "gamedlg.h"
#include "sg.h"
#include "gameoptions.h"
#include "ai/ai.h"
#include "errors.h"
#include "password_dialog.h"
#include "viewcalculation.h"
#include "replay.h"
#include "resourcenet.h"
#include "itemrepository.h"
#include "strtmesg.h"
#include "messagedlg.h"
#include "gameevent_dialogs.h"

tmoveparams moveparams;


class InitControls {
  public:
   InitControls ( void ) 
   {
      memset ( (void*) &moveparams, 0, sizeof ( moveparams ));
   };
} initControls;



  class   tsearchexternaltransferfields : public SearchFields {
                      public:
                                Building*            bld;
                                char                    numberoffields;
                                void                    searchtransferfields( Building* building );
                                virtual void            testfield ( const MapCoordinate& mc );
                                tsearchexternaltransferfields ( pmap _gamemap ) : SearchFields ( _gamemap ) {};
                             };

void         tsearchexternaltransferfields :: testfield( const MapCoordinate& mc )
{ 
     pfield fld  = gamemap->getField ( mc );
     if ( fld && fld->vehicle )
        if ( fld->vehicle->height & bld->typ->externalloadheight ) {
           numberoffields++;
           fld->a.temp = 123;
        }
}


void tsearchexternaltransferfields :: searchtransferfields( Building* building )
{
   actmap->cleartemps( 7 );
   bld = building;
   numberoffields = 0;
   if ( bld->typ->special & cgexternalloadingb ) {
      initsearch( bld->getEntry(), 1, 1 );
      startsearch();
   }
   if ( numberoffields )
      moveparams.movestatus = 130;
}

int searchexternaltransferfields ( Building* bld )
{
   tsearchexternaltransferfields setf ( actmap );
   setf.searchtransferfields ( bld );
   return setf.numberoffields;
}


// #define netdebug


  class   tsearchdestructbuildingfields : public SearchFields {
                      public:
                                Vehicle*                vehicle;
                                char                    numberoffields;
                                pfield                  startfield;
                                void                    initdestructbuilding( int x, int y );
                                virtual void            testfield ( const MapCoordinate& mc );
                                tsearchdestructbuildingfields ( pmap _gamemap ) : SearchFields ( _gamemap ) {};
                             };


void         tsearchdestructbuildingfields::initdestructbuilding( int x, int y )
{
   Vehicle*     eht = getfield(x,y)->vehicle;
   vehicle = eht;
   if (eht->attacked || (eht->typ->wait && eht->hasMoved() )) {
      dispmessage2(305,NULL);
      return;
   }
   actmap->cleartemps(7);
   initsearch( MapCoordinate(x,y), 1, 1 );
   numberoffields = 0;
   startfield = getfield(x,y);
   startsearch();
   if (numberoffields > 0) {
      moveparams.movestatus = 115;
      moveparams.vehicletomove = eht;
   }
   else {
      dispmessage2(306,"");
   }
}


void         tsearchdestructbuildingfields::testfield(const MapCoordinate& mc)
{
   startfield = gamemap->getField(mc);
   if (startfield->building && getheightdelta(log2(vehicle->height), log2(startfield->building->typ->buildingheight)) == 0 ) {
      numberoffields++;
      startfield->a.temp = 20;
   }
}





void         destructbuildinglevel1(int xp, int yp)
{
   tsearchdestructbuildingfields   sdbf ( actmap );
   sdbf.initdestructbuilding( xp, yp  );
}

Resources getDestructionCost( Building* bld, Vehicle* veh )
{
   Resources r;
   r.material = - bld->typ->productionCost.material * (100 - bld->damage) / destruct_building_material_get / 100;
   r.fuel = destruct_building_fuel_usage * veh->typ->fuelConsumption;
   return r;
}

void         destructbuildinglevel2( int xp, int yp)
{
   pfield fld = getfield(xp,yp);
   if (fld->a.temp == 20)
      if (moveparams.movestatus == 115) {
         actmap->cleartemps(7);
         Vehicle* eht = moveparams.vehicletomove;

         Building* bb = fld->building;
         Resources res = eht->getResource( getDestructionCost( bb, eht ), false);

         eht->setMovement ( 0 );
         eht->attacked = 1;

         if ( bb->getCompletion() ) {
            bb->setCompletion ( bb->getCompletion()-1 );
         } else {
            delete bb;
         }
         logtoreplayinfo ( rpl_removebuilding3, xp, yp, eht->networkid, res.energy, res.material, res.fuel );
         computeview( actmap );
         displaymap();
         moveparams.movestatus = 0;
      }
}








   //! looks up the fields a unit can put or remove a mine from
   class tputmine : public SearchFields {
                       int player;
                public:
                       char             mienentyp;
                       char             mienenlegen, mienenraeumen;
                       char             numberoffields;
                       virtual void     testfield ( const MapCoordinate& mc );
                       int              initpm( char mt, const Vehicle* eht );
                       void             run ( void );
                       tputmine ( pmap _gamemap ) : SearchFields ( _gamemap ) {};
              };


void         tputmine::testfield(const MapCoordinate& mc)
{
   pfield fld = gamemap->getField(mc);
   if ( !fld->vehicle  &&  !fld->building ) {
      fld->a.temp = 0;
      if ( !fld->mines.empty() ) {
         fld->a.temp += 2;
         numberoffields++;
      }
      if (mienenlegen && (fld->mines.empty() || fld->mineowner() == player) && fld->mines.size() < gamemap->getgameparameter ( cgp_maxminesonfield )) {
         fld->a.temp += 1;
         numberoffields++;
      }
   }
}




int          tputmine::initpm(  char mt, const Vehicle* eht )
{
   numberoffields = 0;
   mienenlegen = false;
   mienenraeumen = false;

   const SingleWeapon* weapon = NULL;

   if (eht->typ->weapons.count > 0)
      for ( int i = 0; i <= eht->typ->weapons.count - 1; i++)
         if ((eht->typ->weapons.weapon[i].getScalarWeaponType() == cwminen) && eht->typ->weapons.weapon[i].shootable() && (eht->typ->weapons.weapon[i].sourceheight & eht->height) ) {
            mienenraeumen = true;
            if (eht->ammo[i] > 0)
               mienenlegen = true;
            weapon = &eht->typ->weapons.weapon[i];
         }
   player = eht->color / 8;
   mienentyp = mt;
   if (eht->getMovement() < mineputmovedecrease) {
      mienenlegen = false;
      mienenraeumen = false;
      return -119;
   }
   if (mienenlegen || mienenraeumen)
      initsearch( eht->getPosition(), (weapon->mindistance + maxmalq-1) / maxmalq, weapon->maxdistance / maxmalq );
   return 0;
}


void         tputmine::run(void)
{
   if ((mienenlegen || mienenraeumen)) {
      startsearch();
      if (numberoffields > 0) {
         moveparams.movestatus = 90;
      }
   }
}






void  putMine( const MapCoordinate& pos, int typ, int delta )
{
   if (moveparams.movestatus == 0) {
      Vehicle* veh = actmap->getField(pos)->vehicle; 
      if ( !veh || veh->color != (actmap->actplayer << 3))
         return;
         
      moveparams.vehicletomove = veh;
         
      tputmine ptm ( actmap );
      int res = ptm.initpm(typ,veh);
      ptm.run();
      if ( res < 0 )
         dispmessage2 ( -res );
   }
   else
      if (moveparams.movestatus == 90) {
         Vehicle* eht = moveparams.vehicletomove;
         pfield fld = actmap->getField(pos);
         if ( fld->a.temp ) {

            if ( (fld->a.temp & 1) && ( delta > 0 )) {
               const Vehicletype* fzt = eht->typ;
               int  strength = 64;
               for ( int i = 0; i < fzt->weapons.count ; i++)
                  if ((fzt->weapons.weapon[i].getScalarWeaponType() == cwminen) && fzt->weapons.weapon[i].shootable() )
                     if ( fld-> putmine( actmap->actplayer, typ, MineBasePunch[typ-1] * strength / 64 )) {
                        eht->ammo[i]--;
                        eht->setMovement ( eht->getMovement() - mineputmovedecrease );
                        strength = eht->weapstrength[i];
                        logtoreplayinfo ( rpl_putmine, pos.x, pos.y, (int) actmap->actplayer, (int) typ, (int) MineBasePunch[typ-1] * strength / 64 );
                        break;
                     }


            }

            if ( (fld->a.temp & 2) && ( delta < 0 )) {
               pfield fld = actmap->getField(pos);
               fld->removemine( -1 );
               eht->decreaseMovement ( mineremovemovedecrease );
               logtoreplayinfo ( rpl_removemine, pos.x, pos.y );
            }
            actmap->cleartemps(7);
            computeview( actmap );
            moveparams.movestatus = 0;
            displaymap();
         }
      }
}







/*
void         refuelvehicle(int         b)
{
   Vehicle*     actvehicle;

   if (moveparams.movestatus == 0) {

      trefuelvehicle rfe;
      rfe.initrefuelling(getxpos(),getypos(),b);
      rfe.startsearch();

   }
   else {
      if (moveparams.movestatus == 65) {
         if (getactfield()->a.temp > 0) {
            actvehicle = getfield(moveparams.movesx,moveparams.movesy)->vehicle;
            verlademunition(getactfield()->vehicle,actvehicle,NULL,3 - b);
            actmap->cleartemps(7);
            moveparams.movestatus = 0;
         }
      }
      else
         if (moveparams.movestatus == 66)
            if (getactfield()->a.temp > 0) {
               actvehicle = getfield(moveparams.movesx,moveparams.movesy)->vehicle;
               // actvehicle->repairunit( getactfield()->vehicle );
               actmap->cleartemps(7);
               moveparams.movestatus = 0;
            }
      updateFieldInfo();
   }

}
*/




int windbeeline ( const MapCoordinate& start, const MapCoordinate& dest, WindMovement* wm ) {
   int x1 = start.x;
   int y1 = start.y;
   int dist = 0;
   while ( x1 != dest.x  || y1 != dest.y ) {
      dist+= minmalq;
      int direc = getdirection ( x1, y1, dest.x, dest.y );
      dist -= wm->getDist(direc);
      getnextfield ( x1, y1, direc );
   }
   return dist;
}













treactionfirereplay :: treactionfirereplay ( void )
{
   num = 0;
   unit = NULL;
}

void treactionfirereplay :: init ( Vehicle* eht, const AStar3D::Path& fieldlist )
{
   if ( runreplay.status > 0 ) {
      preactionfire_replayinfo rpli;
      do {
         rpli = runreplay.getnextreplayinfo ( );
         if ( rpli )
            replay[num++] = rpli;

      } while ( rpli );

      unit = eht;
   }
}

int  treactionfirereplay :: checkfield ( const MapCoordinate3D& pos, Vehicle* &eht, MapDisplayInterface* md )
{
   int attacks = 0;

   if ( eht == unit ) {
      for ( int i = 0; i < num; i++ ) {
         preactionfire_replayinfo rpli = replay[ i ];
         if ( eht && rpli->x2 == pos.x  && rpli->y2 == pos.y ) {

             pfield fld = getfield ( rpli->x1, rpli->y1 );
             pfield targ = getfield ( rpli->x2, rpli->y2 );

             npush ( targ->vehicle );
             targ->vehicle = eht;
             eht->xpos = pos.x;
             eht->ypos = pos.y;
             eht->height = pos.getBitmappedHeight();


             int attackvisible = fieldvisiblenow ( fld ) || fieldvisiblenow ( targ );


             if ( md && attackvisible ) {
             /*
               // cursor.setcolor ( 8 );

               cursor.gotoxy ( rpli->x1, rpli->y1 );
               int t = ticker;
               while ( t + 15 > ticker )
                  releasetimeslice();

               cursor.gotoxy ( pos.x, pos.y );
               t = ticker;
               while ( t + 15 > ticker )
                  releasetimeslice();

               // cursor.setcolor ( 0 );
               // cursor.hide();
               */
             }
             attacks++;

             tunitattacksunit battle ( fld->vehicle, targ->vehicle, 1, rpli->wpnum );
             battle.av.damage = rpli->ad1;
             battle.dv.damage = rpli->dd1;
             if ( md && attackvisible  )
                battle.calcdisplay ( rpli->ad2, rpli->dd2 );
             else {
                battle.calc ();
                battle.av.damage = rpli->ad2;
                battle.dv.damage = rpli->dd2;
             }

             int killed = 0;
             if ( battle.dv.damage >= 100 )
                killed = 1;

             battle.setresult ();
             updateFieldInfo();

             npop ( targ->vehicle );

             if ( killed )
                eht = NULL;
         }
      }
   }
   return attacks;
}

treactionfirereplay :: ~treactionfirereplay ( )
{
   for ( int i = 0; i < num; i++ )
      delete replay[i];
}



                typedef struct tunitlist* punitlist;
                struct tunitlist {
                        Vehicle* eht;
                        punitlist next;
                     };

                punitlist unitlist[8];


int tsearchreactionfireingunits :: maxshootdist[8] = { -1, -1, -1, -1, -1, -1, -1, -1 };

tsearchreactionfireingunits :: tsearchreactionfireingunits ( void )
{
   for ( int i = 0; i < 8; i++ )
      unitlist[i] = NULL;
}

void tsearchreactionfireingunits :: init ( Vehicle* vehicle, const AStar3D::Path& fieldlist )
{
   int x1 = maxint;
   int y1 = maxint;
   int x2 = 0;
   int y2 = 0;
   int j, h;

   if ( maxshootdist[0] == -1 ) {
      for (int i = 0; i < 8; i++ )
         maxshootdist[i] = 0;

      for (int i = 0; i < vehicleTypeRepository.getNum(); i++ ) {
         Vehicletype* fzt = vehicleTypeRepository.getObject_byPos ( i );
         if ( fzt )
            for (j = 0; j < fzt->weapons.count; j++ )
               if ( fzt->weapons.weapon[j].shootable() )
                  for (h = 0; h < 8; h++ )
                     if ( fzt->weapons.weapon[j].targ & ( 1 << h ) )
                        if ( fzt->weapons.weapon[j].maxdistance > maxshootdist[h] )
                           maxshootdist[h] = fzt->weapons.weapon[j].maxdistance;
      }
   }

   for ( AStar3D::Path::const_iterator i = fieldlist.begin(); i != fieldlist.end(); i++) {
      if ( i->x > x2 )
         x2 = i->x ;
      if ( i->y > y2 )
         y2 = i->y ;

      if ( i->x < x1 )
         x1 = i->x ;
      if ( i->y < y1 )
         y1 = i->y ;
   }
   int height = log2 ( vehicle->height );

   x1 -= maxshootdist[height];
   y1 -= maxshootdist[height];
   x2 += maxshootdist[height];
   y2 += maxshootdist[height];

   if ( x1 < 0 )
      x1 = 0;
   if ( y1 < 0 )
      y1 = 0;
   if ( x2 >= actmap->xsize )
      x2 = actmap->xsize -1;
   if ( y2 >= actmap->ysize )
      y2 = actmap->ysize -1;

   for ( int y = y1; y <= y2; y++ )
      for ( int x = x1; x <= x2; x++ ) {
         Vehicle* eht = getfield ( x, y )->vehicle;
         if ( eht )
            if ( eht->color != vehicle->color )
               if ( eht->reactionfire.getStatus() >= Vehicle::ReactionFire::ready )
                  if ( eht->reactionfire.enemiesAttackable & ( 1 << ( vehicle->color / 8 )))
                     if ( actmap->player[eht->getOwner()].diplomacy.isHostile( actmap->actplayer))
                        if ( attackpossible2u ( eht, vehicle, NULL, vehicle->typ->height ) )
                           addunit ( eht );

      }
   if ( getfield(vehicle->xpos, vehicle->ypos)->vehicle == vehicle )
      for ( int i = 0; i < 8; i++ )
         if ( fieldvisiblenow ( getfield ( vehicle->xpos, vehicle->ypos ), i )) {
            punitlist ul  = unitlist[i];
            while ( ul ) {
               punitlist next = ul->next;
               pattackweap atw = attackpossible ( ul->eht, vehicle->xpos, vehicle->ypos );
               for ( int j = 0; j < atw->count; ++j )
                  if ( ul->eht->reactionfire.weaponShots[atw->num[j]] ) {
                     removeunit ( ul->eht );
                     break;
                  }   

               delete atw;
               ul = next;
            } /* endwhile */
         }

}

void  tsearchreactionfireingunits :: addunit ( Vehicle* eht )
{
   int c = eht->color / 8;
   punitlist ul = new tunitlist;
   ul->eht = eht;
   ul->next= unitlist[c];
   unitlist[c] = ul;
}


void tsearchreactionfireingunits :: removeunit ( Vehicle* vehicle )
{
   int c = vehicle->color / 8;
   punitlist ul = unitlist[c];
   punitlist last = NULL;
   while ( ul  &&  ul->eht != vehicle ) {
      last = ul;
      ul = ul->next;
  }
  if ( ul  &&  ul->eht == vehicle ) {
     if ( last )
        last->next = ul->next;
     else
        unitlist[c] = ul->next;

     delete ul;
  }
}


int  tsearchreactionfireingunits :: checkfield ( const MapCoordinate3D& pos, Vehicle* &vehicle, MapDisplayInterface* md )
{

   int attacks = 0;
   int result = 0;

   pfield fld = getfield( pos.x, pos.y );
   npush ( fld->vehicle );
   fld->vehicle = vehicle;
   vehicle->xpos = pos.x;
   vehicle->ypos = pos.y;
   int oldheight = vehicle->height;
   if ( pos.getNumericalHeight() >= 0 )
      vehicle->height = pos.getBitmappedHeight();

   for ( int i = 0; i < 8; i++ ) {
      evaluatevisibilityfield ( actmap, fld, i, -1, actmap->getgameparameter ( cgp_initialMapVisibility ) );
      if ( fieldvisiblenow ( fld, i )) {
         punitlist ul  = unitlist[i];
         while ( ul  &&  !result ) {
            punitlist next = ul->next;
            if ( find ( ul->eht->reactionfire.nonattackableUnits.begin(), ul->eht->reactionfire.nonattackableUnits.end(), vehicle->networkid) == ul->eht->reactionfire.nonattackableUnits.end() ) {
               pattackweap atw = attackpossible ( ul->eht, pos.x, pos.y );
               if ( atw->count && (ul->eht->reactionfire.enemiesAttackable & (1 << (vehicle->color / 8)))) {

                  int ad1, ad2, dd1, dd2;

                  int strength = 0;
                  int num = -1;
                  for ( int j = 0; j < atw->count; j++ )
                     if ( ul->eht->reactionfire.weaponShots[atw->num[j]] > 0 )
                        if ( atw->strength[j]  > strength ) {
                           strength = atw->strength[j];
                           num = j;
                        }

                  if ( num >= 0 ) {

                     int visibility = 0;
                     if ( md ) {
                        displaymessage2 ( "attacking with weapon %d ", atw->num[num] );
                        
                        // cursor.setcolor ( 8 );

                        if ( fieldvisiblenow ( getfield (ul->eht->xpos, ul->eht->ypos ), actmap->playerView)) {
                           ++visibility;
                           md->cursor_goto( ul->eht->getPosition() );
                           int t = ticker;
                           while ( t + 15 > ticker )
                              releasetimeslice();
                        }

                        if ( fieldvisiblenow ( fld, actmap->playerView)) {
                           ++visibility;
                           md->cursor_goto( pos );
                           int t = ticker;
                           while ( t + 15 > ticker )
                              releasetimeslice();
                        }
/*
                        cursor.setcolor ( 0 );
                        cursor.hide();
                        */
                     }

                     Vehicle* veh = ul->eht;
                     tunitattacksunit battle ( veh, fld->vehicle, 0, atw->num[num] );
                     int nwid = fld->vehicle->networkid;

                     ad1 = battle.av.damage;
                     dd1 = battle.dv.damage;

                     if ( md && visibility)
                        battle.calcdisplay ();
                     else
                        battle.calc();

                     ad2 = battle.av.damage;
                     dd2 = battle.dv.damage;

                     attacks++;

                     if ( battle.dv.damage >= 100 )
                        result = 1;

                     // ul->eht->reactionfire.enemiesAttackable &= 0xff ^ ( 1 <<  (vehicle->color / 8) );

                     ul->eht->reactionfire.weaponShots[atw->num[num]]--;
                     ul->eht->reactionfire.nonattackableUnits.push_back ( nwid );

                     removeunit ( ul->eht );

                     battle.setresult();

                     if ( ad2 < 100 )
                        veh->attacked = false;

      //               logtoreplayinfo ( rpl_reactionfire, ulex, uley, x, y, ad1, ad2, dd1, dd2, atw->num[num] );

                     updateFieldInfo();
                  }


               }
               delete atw;
            }
            ul = next;
         } /* endwhile */
      }
   }
   npop ( fld->vehicle );
   for ( int i = 0; i < 8; i++ )
      evaluatevisibilityfield ( actmap, fld, i, -1, actmap->getgameparameter ( cgp_initialMapVisibility ) );

   vehicle->height = oldheight;

   if ( result )
      vehicle = NULL;

   return attacks;
}

tsearchreactionfireingunits :: ~tsearchreactionfireingunits()
{
   for ( int i = 0; i < 8; i++ ) {
      punitlist ul = unitlist[i];
      while ( ul ) {
         punitlist ul2 = ul->next;
         delete ul;
         ul = ul2;
      }
   }
}




pair<int,int> calcMoveMalus( const MapCoordinate3D& start,
                            const MapCoordinate3D& dest,
                            const Vehicle*     vehicle,
                            WindMovement* wm,
                            bool*  inhibitAttack,
                            bool container2container )
{
   int direc = getdirection ( start.x, start.y, dest.x, dest.y );


   int fuelcost = 10;
   int movecost;
   bool checkHemming = true;
   bool checkWind = wm != NULL;
   int dist = 1;

   if ( start.getNumericalHeight() >= 0 && dest.getNumericalHeight() >= 0 ) {

      // changing height
      if ( (start.getNumericalHeight() != dest.getNumericalHeight()) && !container2container ) {
          const Vehicletype::HeightChangeMethod* hcm = vehicle->getHeightChange( start.getNumericalHeight() < dest.getNumericalHeight() ? 1 : -1, start.getBitmappedHeight());
          if ( !hcm || hcm->dist != beeline ( start, dest )/maxmalq )
             fatalError("Calcmovemalus called with invalid height change distance");
          dist = hcm->dist;
          movecost = hcm->moveCost;
          fuelcost = max(hcm->dist*10,10);
          if ( inhibitAttack && !hcm->canAttack )
            *inhibitAttack = !hcm->canAttack;
          checkHemming = false;
          if ( start.getNumericalHeight() < 4 || dest.getNumericalHeight() < 4 )
             checkWind = false;
      } else
         // flying
         if (start.getNumericalHeight() >= 4 )
            movecost = maxmalq;
         else
            if ( start.getNumericalHeight() <= 1 ) {
               movecost = submarineMovement;
               checkWind = false;
            } else {
               // not flying
               pfield fld = getfield( dest.x, dest.y );
               if ( fld->building )
                  movecost = maxmalq;
               else
                  movecost = fld->getmovemalus( vehicle );
               checkWind = false;
            }

   } else
      if ( dest.getNumericalHeight() >= 0 ) {
        // moving out of container
        int mm = getfield( start.x, start.y )->getContainer()->vehicleUnloadSystem( vehicle->typ, dest.getBitmappedHeight() )->movecost;
        if ( mm > 0 )
            movecost = mm;
        else {
            if ( dest.getNumericalHeight() >= 4 )
               // flying
               movecost = maxmalq;
            else {
               if ( start.getNumericalHeight() <= 1 && start.getNumericalHeight() != -1 ) {
                  movecost = submarineMovement;
                  checkWind = false;
               } else {
                  movecost = getfield( dest.x, dest.y )->getmovemalus( vehicle );
               }
            }
        }
      } else {
        // moving from one container to another
        movecost = maxmalq;
        checkHemming = false;
        checkWind = false;
      }

/*
   static const  int         movemalus[6]  = { 8, 6, 3, 0, 3, 6 };
   
   if ( checkHemming )
      for (int c = 0; c < sidenum; c++) {
         int x = dest.x;
         int y = dest.y;
         getnextfield( x,  y, c );
         pfield fld = getfield ( x, y );
         if ( fld ) {
           int d = (c - direc);

           if (d >= sidenum)
              d -= sidenum;

           if (d < 0)
              d += sidenum;

           pfield fld = getfield(x,y);
           if ( fld->vehicle && dest.getNumericalHeight() >= 0 ) {
              if ( getdiplomaticstatus(fld->vehicle->color) == cawar ) 
                 if ( attackpossible28(fld->vehicle,vehicle, NULL, dest.getBitmappedHeight() ))
                    movecost += movemalus[d];
              
           }
         }
      }
*/
    /*******************************/
    /*    Wind calculation        ÿ */
    /*******************************/
   if ( wm && checkWind )
      if (dest.getNumericalHeight() >= 4 && dest.getNumericalHeight() <= 6 &&
          start.getNumericalHeight() >= 4 && start.getNumericalHeight() <= 6 &&
          actmap->weatherSystem->getCurrentWindSpeed()  ) {
         movecost -=  wm->getDist( direc ) * dist;
         fuelcost -=  wm->getDist ( direc ) * dist;

         if ( movecost < 1 )
           movecost = 1;

         if ( fuelcost <= 0 )
           fuelcost = 0;
      }
   return make_pair(movecost,fuelcost);
}




void checkalliances_at_endofturn ( void )
{
   #if 0
   for ( int i = 0; i < 8; i++ ) {
      int act = actmap->actplayer;
      if ( i != act ) {

         if ( actmap->alliances[i] [act] == canewsetwar2 ) {
            // message "sneak attack" to all players except attacker

            int to = 0;
            for ( int j = 0; j < 8; j++ )
               if ( j != act )
                  to |= 1 << j;

            ASCString txt;
            txt.format ( getmessage( 10001 ), actmap->player[act].getName().c_str(), actmap->player[i].getName().c_str() );
            new Message ( txt, actmap, to );
         }

         if ( actmap->alliances[i] [act] == capeaceproposal  &&  actmap->alliances_at_beginofturn[i] != capeaceproposal ) {
            int to = 0;
            for ( int j = 0; j < 8; j++ )
               if ( j == i )
                  to |= 1 << j;

            char txt[200];
            const char* sp;
            if ( actmap->alliances[act] [i] == capeaceproposal )
               sp = getmessage( 10005 );  // accepts peace
            else
               sp = getmessage( 10003 );  // proposes peace

            sprintf ( txt, sp, actmap->player[act].getName().c_str() );
            new Message ( txt, actmap, to );
         }

         if ( actmap->alliances[i] [act] == cawarannounce ) {
            int to = 0;
            for ( int j = 0; j < 8; j++ )
               if ( j == i )
                  to |= 1 << j;

            char txt[200];
            const char* sp = getmessage( 10002 );
            sprintf ( txt, sp, actmap->player[act].getName().c_str() );
            new Message ( txt, actmap, to );
         }

      }
   }
   #endif
   #warning alliances
}



void checkalliances_at_beginofturn ( void )
{
   #if 0
   int i;

   int act = actmap->actplayer ;
   for (i = 0; i < 8; i++ ) {
      if ( actmap->alliances[i][act] == cawarannounce || actmap->alliances[i][act] == canewsetwar2 ) {
         actmap->alliances[i][act] = cawar;
         actmap->alliances[act][i] = cawar;
         if ( actmap->shareview ) {
            actmap->shareview->mode[act][i] = false;
            actmap->shareview->mode[i][act] = false;
         }
      }
      if ( actmap->alliances[i][act] == capeaceproposal  &&  actmap->alliances[act][i] == capeaceproposal) {
         actmap->alliances[i][act] = capeace;
         actmap->alliances[act][i] = capeace;
      }
   }


   for ( i = 0; i < 8; i++ )
      actmap->alliances_at_beginofturn[i] = actmap->alliances[i][act];

   #endif
   #warning alliances
}









void Building :: execnetcontrol ( void )
{
   for ( int i = 0; i < 3; i++ )
      if ( !actmap->isResourceGlobal(i) ) {
         if (  netcontrol & (cnet_moveenergyout << i )) {
            npush (  netcontrol );
            netcontrol |= (cnet_stopenergyinput << i );
            actstorage.resource(i) -= putResource ( actstorage.resource(i), i, 0 );
            npop (  netcontrol );
         } else
            if (  netcontrol & (cnet_storeenergy << i )) {
               npush (  netcontrol );
               netcontrol |= (cnet_stopenergyoutput << i );
               actstorage.resource(i) += getResource ( gettank(i) -  actstorage.resource(i), i, 0 );
               npop (  netcontrol );
            }
      }

}

                    /*   modes: 0 = energy   ohne abbuchen
                                1 = material ohne abbuchen
                                2 = fuel     ohne abbuchen

                                  +4         mit abbuchen                         /
                                  +8         nur Tributzahlungen kassieren       /
                                 +16         plus zurueckliefern                 <  diese Bits schliessen sich gegenseitig aus
                                 +32         usage zurueckliefern                 \
                                 +64         tank zurueckliefern                   \
                                 */


int  Building :: putResource ( int      need,    int resourcetype, bool queryonly, int scope  )
{
   if ( need < 0 )
      return -getResource( -need, resourcetype, queryonly, scope );
   else {
      PutResource putresource ( getMap(), scope );
      return putresource.getresource ( entryPosition.x, entryPosition.y, resourcetype, need, queryonly, color/8, scope );
   }
}


int  Building :: getResource ( int      need,    int resourcetype, bool queryonly, int scope )
{
   if ( need < 0 )
      return -putResource( -need, resourcetype, queryonly, scope );
   else {
      GetResource gr ( getMap(), scope );
      return gr.getresource ( entryPosition.x, entryPosition.y, resourcetype, need, queryonly, color/8, scope );
   }
}



void newTurnForHumanPlayer ( int forcepasswordchecking = 0 )
{
   checkalliances_at_beginofturn ();
   for ( int p = 0; p < 8; p++ )
      actmap->player[p].existanceAtBeginOfTurn = actmap->player[p].exist() && actmap->player[p].stat != Player::off;

   if ( actmap->player[actmap->actplayer].stat == Player::human ) {

      int humannum = 0;
      for ( int i = 0; i < 8; i++ )
         if ( actmap->player[i].exist() )
            if ( actmap->player[i].stat == Player::human )
               humannum++;

      if ( humannum > 1  ||  forcepasswordchecking > 0 ) {
         tlockdispspfld ldsf;

         bool firstRound = actmap->time.turn() == 1;
         bool specifyPassword = firstRound && actmap->player[actmap->actplayer].passwordcrc.empty();
         // bool askForPassword = false;

         if ( (!actmap->player[actmap->actplayer].passwordcrc.empty() && actmap->player[actmap->actplayer].passwordcrc != CGameOptions::Instance()->getDefaultPassword() )
            || firstRound  ) {
               if ( forcepasswordchecking < 0 ) {
                  delete actmap;
                  actmap = NULL;
                  throw NoMapLoaded();
               } else {
                  bool stat;
                  do {
                     stat = enterpassword ( actmap->player[actmap->actplayer].passwordcrc, specifyPassword );
                  } while ( actmap->player[actmap->actplayer].passwordcrc.empty() && stat && viewtextquery ( 910, "warning", "~e~nter password", "~c~ontinue without password" ) == 0 ); /* enddo */
               }
         } else
            displaymessage("next player is:\n%s",3,actmap->player[actmap->actplayer].getName().c_str() );
      }

      checkforreplay();

      if ( actmap->lastjournalchange.abstime )
         if ( (actmap->lastjournalchange.turn() == actmap->time.turn() ) ||
              (actmap->lastjournalchange.turn() == actmap->time.turn()-1  &&  actmap->lastjournalchange.move() > actmap->actplayer ) )
                 viewjournal();

      updateFieldInfo();

      moveparams.movestatus = 0;


   }
   computeview( actmap );

   actmap->playerView = actmap->actplayer;

   initReplayLogging();

   if ( actmap->player[actmap->actplayer].stat == Player::human ) {
      Research& research = actmap->player[actmap->actplayer].research;
      if (research.activetechnology == NULL )
         if ( research.progress ) {
            choosetechnology();
         }
      if (research.activetechnology  )
         if( find ( research.developedTechnologies.begin(), research.developedTechnologies.end(), research.activetechnology->id ) != research.developedTechnologies.end()) {
            research.progress = 0;
            choosetechnology();
         }
         
      while ((research.activetechnology != NULL ) &&
             (research.progress >= research.activetechnology->researchpoints)) {
              int mx = research.progress - research.activetechnology->researchpoints;

              showtechnology( research.activetechnology );
              if ( research.activetechnology )
                 logtoreplayinfo ( rpl_techResearched, research.activetechnology->id, actmap->actplayer );

              NewVehicleTypeDetection pfzt;

              research.addtechnology();

              pfzt.evalbuffer ();

              choosetechnology();

              research.progress = mx;
      }
   }

   actmap->xpos = actmap->cursorpos.position[ actmap->actplayer ].sx;
   actmap->ypos = actmap->cursorpos.position[ actmap->actplayer ].sy;

//   cursor.gotoxy ( actmap->cursorpos.position[ actmap->actplayer ].cx, actmap->cursorpos.position[ actmap->actplayer ].cy , 0);

   updateFieldInfo();
   transfer_all_outstanding_tribute();
}





void sendnetworkgametonextplayer ( int oldplayer, int newplayer )
{
/*
   int num = 0;
   int pl[8];

   for (int i = 0; i < 8; i++) {
      if ( actmap->player[i].existent )
         if ( actmap->player[i].firstvehicle || actmap->player[i].firstbuilding ) {
            pl[num] = i;
            num++;
          }
   }  endfor */
#if 0

   tnetworkcomputer* compi = &actmap->network->computer[ actmap->network->player[ oldplayer ].compposition ];
   while ( compi->send.transfermethod == NULL  ||  compi->send.transfermethodid == 0 )
        setupnetwork( actmap->network, 2, oldplayer );

   displaymessage ( " starting data transfer ",0);

   try {
      compi->send.transfermethod->initconnection ( TN_SEND );
      compi->send.transfermethod->inittransfer ( &compi->send.data );

      tnetworkloaders nwl;
      nwl.savenwgame ( compi->send.transfermethod->stream );

      compi->send.transfermethod->closetransfer();
      compi->send.transfermethod->closeconnection();
   } /* endtry */
   catch ( tfileerror ) {
      displaymessage ( "error saving file", 1 );
   } /* endcatch */

   delete actmap;
   actmap = NULL;
   displaymessage( " data transfer finished",1);


   throw NoMapLoaded ();
#endif
}



void endTurn ( void )
{
   mousevisible(false);
   if ( actmap->actplayer >= 0 ) 
      actmap->endTurn();

   closeReplayLogging();


     /* *********************  allianzen ********************  */

  checkalliances_at_endofturn ();

     /* *********************  messages ********************  */

  MessagePntrContainer::iterator mi = actmap->unsentmessage.begin();
  while ( mi != actmap->unsentmessage.end() ) {
     actmap->player[ actmap->actplayer ].sentmessage.push_back ( *mi );
     for ( int i = 0; i < 8; i++ )
        if ( (*mi)->to & ( 1 << i ))
           actmap->player[ i ].unreadmessage.push_back ( *mi );

     mi = actmap->unsentmessage.erase ( mi );
  }

  if ( !actmap->newJournal.empty() ) {
     ASCString add = actmap->gameJournal;

     char tempstring[100];
     char tempstring2[100];
     sprintf( tempstring, "#color0# %s ; turn %d #color0##crt##crt#", actmap->player[actmap->actplayer].getName().c_str(), actmap->time.turn() );
     sprintf( tempstring2, "#color%d#", getplayercolor ( actmap->actplayer ));

     int fnd;
     do {
        fnd = 0;
        if ( !add.empty() )
           if ( add.find ( '\n', add.length()-1 ) != add.npos ) {
              add.erase ( add.length()-1 );
              fnd++;
           } else
             if ( add.length() > 4 )
                if ( add.find ( "#crt#", add.length()-5 ) != add.npos ) {
                  add.erase ( add.length()-5 );
                  fnd++;
                }

     } while ( fnd ); /* enddo */

     add += tempstring2;
     add += actmap->newJournal;
     add += tempstring;

     actmap->gameJournal = add;
     actmap->newJournal.erase();

     actmap->lastjournalchange.set ( actmap->time.turn(), actmap->actplayer );
  }
}


void nextPlayer( void )
{
   int oldplayer = actmap->actplayer;


   if ( !actmap->nextPlayer() ) {
      displaymessage("There are no players left any more !",1);
      delete actmap;
      actmap = NULL;
      throw NoMapLoaded ();
   }

   if ( CGameOptions::Instance()->debugReplay && oldplayer >= 0 && actmap->player[oldplayer].stat == Player::human && actmap->replayinfo)
      if (choice_dlg("run replay of your turn ?","~y~es","~n~o") == 1) {
         // cursor.gotoxy( actmap->cursorpos.position[oldplayer].cx, actmap->cursorpos.position[oldplayer].cy );
         runSpecificReplay ( oldplayer, oldplayer );
      }



//   int newplayer = actmap->actplayer;
   actmap->playerView = actmap->actplayer;

/*   if ( oldplayer >= 0 && actmap->network &&  oldplayer != actmap->actplayer && actmap->network->player[ newplayer ].compposition != actmap->network->player[ oldplayer ].compposition )
      sendnetworkgametonextplayer ( oldplayer, newplayer );
   else {
   */
/*
      tlockdispspfld ldsf;

      int forcepwd;  // Wenn der aktuelle player gerade verloren hat, muss fuer den naechsten player die Passwortabfrage kommen, auch wenn er nur noch der einzige player ist !
      if ( oldplayer >= 0  &&  !actmap->player[oldplayer].existent )
         forcepwd = 1;
      else
         forcepwd = 0;

      newturnforplayer( forcepwd );
*/
//   }
}


void runai( int playerView )
{
   actmap->playerView = playerView;

   if ( !actmap->player[ actmap->actplayer ].ai )
      actmap->player[ actmap->actplayer ].ai = new AI ( actmap, actmap->actplayer );

   actmap->player[ actmap->actplayer ].ai->run();
   updateFieldInfo();
}

void next_turn ( int playerView )
{
   int startTurn = actmap->time.turn();

   int pv;
   if ( playerView == -2 ) {
      if ( actmap->time.turn() <= 0 || actmap->actplayer < 0 )
         pv = -1;
      else
         if ( actmap->player[actmap->actplayer].stat != Player::human )
            pv = -1;
         else
            pv = actmap->actplayer;
   } else
      pv = playerView;


   do {
     endTurn();
     nextPlayer();

     if ( actmap->player[actmap->actplayer].stat == Player::computer )
        runai( pv );

     if ( actmap->time.turn() >= startTurn+2 ) {
        displaymessage("no human players found !", 1 );
        delete actmap;
        actmap = NULL;
        throw NoMapLoaded();
     }

   } while ( actmap->player[actmap->actplayer].stat != Player::human ); /* enddo */

   newTurnForHumanPlayer();
}

void initNetworkGame ( void )
{
   while ( actmap->player[actmap->actplayer].stat != Player::human ) {

     if ( actmap->player[actmap->actplayer].stat == Player::computer ) {
        computeview( actmap );
        runai(-1);
     }
     endTurn();
     nextPlayer();
   }

   newTurnForHumanPlayer( 0 );

   for ( int i = 0; i < 8; i++ )
      if  ( actmap->player[i].exist() )
         if ( actmap->player[i].ASCversion > 0 )
            if ( (actmap->player[i].ASCversion & 0xffffff00) > getNumericVersion() ) {
               new Message ( ASCString("Player ") + actmap->player[i].getName()
                        + " is using a newer version of ASC. \n"
                          "Please check www.asc-hq.org for updates.\n\n"
                          "Please do NOT report any problems with this version of ASC until "
                          "you have confirmed that they are also present in the latest "
                          "version of ASC.", actmap, 1<<actmap->actplayer );
               return;
            }

}


void continuenetworkgame ( void )
{
#if SDL_BYTEORDER != SDL_LIL_ENDIAN
   displaymessage("Sorry, multiplayer through file transfer is currently not available on big endian machines\nThis will be available with ASC 2.0 , please check www.asc-hq.org", 1 );
   return;
#endif

   tlockdispspfld ldsf;
#if 0
   tnetwork network;
/*
   int stat;
   int go = 0;
   do {
      stat = setupnetwork( &network, 1+8 );
      if ( stat == 1 ) {
         return;
      }
      if ( network.computer[0].receive.transfermethod == 0 )
         displaymessage("please choose a transfer method !",1 );
      else
         if ( network.computer[0].receive.transfermethodid != network.computer[0].receive.transfermethod->getid() )
            displaymessage("please setup transfer method !", 1 );
         else
            if ( !network.computer[0].receive.transfermethod->validateparams( &network.computer[0].receive.data, TN_RECEIVE ))
               displaymessage("please setup transfer method !", 1 );
            else
               go = 1;
   } while ( !go );

*/
   int stat;
   int go = 0;
   do {
      stat = network.computer[0].receive.transfermethod->setupforreceiving ( &network.computer[0].receive.data );
      if ( stat == 0 )
         return;

      if ( network.computer[0].receive.transfermethod  &&
           network.computer[0].receive.transfermethodid == network.computer[0].receive.transfermethod->getid()  &&
           network.computer[0].receive.transfermethod->validateparams( &network.computer[0].receive.data, TN_RECEIVE ))
           go = 1;
   } while ( !go );


   try {
       displaymessage ( " starting data transfer ",0);

       network.computer[0].receive.transfermethod->initconnection ( TN_RECEIVE );
       network.computer[0].receive.transfermethod->inittransfer ( &network.computer[0].receive.data );

       tnetworkloaders nwl;
       nwl.loadnwgame ( network.computer[0].receive.transfermethod->stream );

       network.computer[0].receive.transfermethod->closetransfer();

       network.computer[0].receive.transfermethod->closeconnection();

       if ( actmap->network )
          setallnetworkpointers ( actmap->network );

       removemessage();
   } /* endtry */

   catch ( InvalidID err ) {
      displaymessage( err.getMessage().c_str(), 1 );
      throw NoMapLoaded();
   } /* endcatch */
   catch ( tinvalidversion err ) {
      if ( err.expected < err.found )
         displaymessage( "File/module %s has invalid version.\nExpected version %d\nFound version %d\nPlease install the latest version from www.asc-hq.org", 1, err.getFileName().c_str(), err.expected, err.found );
      else
         displaymessage( "File/module %s has invalid version.\nExpected version %d\nFound version %d\nThis is a bug, please report it!", 1, err.getFileName().c_str(), err.expected, err.found );

      throw NoMapLoaded();
   } /* endcatch */
   catch ( tcompressionerror err ) {
      displaymessage( "The file cannot be decompressed. \nIt has probably been damaged during transmission from the previous player to you.\nTry sending it zip compressed or otherwise encapsulated.\n", 1 );
      throw NoMapLoaded();
   } /* endcatch */
   catch ( tfileerror err) {
      displaymessage( "error reading game %s ", 1, err.getFileName().c_str() );
      throw NoMapLoaded();
   } /* endcatch */
   catch ( ASCexception ) {
      displaymessage( "error loading game", 1 );
      throw NoMapLoaded();
   } /* endcatch */
   if ( !actmap || actmap->xsize <= 0 || actmap->ysize <= 0 )
      throw NoMapLoaded();


   initNetworkGame( );
   #endif
}







void dissectvehicle ( Vehicle* eht )
{
/*
  int i,j,k;

   ptechnology techs[32];
   int technum = 0;
   memset ( techs, 0, sizeof ( techs ));
   for (i = 0; i <= eht->klasse; i++)
      for (j = 0; j < 4; j++ )
         if ( eht->typ->classbound[i].techrequired[j] )
           if ( !actmap->player[actmap->actplayer].research.technologyresearched ( eht->typ->classbound[i].techrequired[j] )) {
               int found =  0;
               for (k = 0; k < technum; k++ )
                  if ( techs[k]->id == eht->typ->classbound[i].techrequired[j] )
                     found = 1;
               if ( !found ) {
                  ptechnology tec = technologyRepository.getObject_byID ( eht->typ->classbound[i].techrequired[j] );
                  if ( tec )
                     techs[technum++] = tec;
               }
           }


   for ( i = 0; i < technum; i++ ) {

      Player& player = actmap->player[actmap->actplayer];
      if ( player.research.activetechnology != techs[i] ) {

         int found = 0;     // Bit 1: Technologie gefunden
                            //     2: vehicletype gefunden
                            //     3: Technologie+vehicletype gefunden

         Player::DissectionContainer::iterator di = player.dissections.begin();
         while ( di != player.dissections.end() ) {
            if ( di->fzt == eht->typ )
               if ( di->tech == techs[i] )
                  found |= 4;
               else
                  found |= 2;

            if ( di->tech == techs[i] )
               found |= 1;

            di++;
         }

         if ( found & 4 ) {
            di = player.dissections.begin();
            while ( di != player.dissections.end() ) {
               if ( di->fzt == eht->typ )
                  if ( di->tech == techs[i] ) {
                     di->points += di->orgpoints / ( 1 << di->num);
                     di->num++;
                  }
               di++;
            }
         } else {
            Player::Dissection du;

            du.tech = techs[i];
            du.fzt = vehicleTypeRepository.getObject_byID ( eht->typ->id );

            if ( found & 1 )
               du.orgpoints = du.tech->researchpoints / dissectunitresearchpointsplus2;
            else
               du.orgpoints = du.tech->researchpoints / dissectunitresearchpointsplus;

            du.points = du.orgpoints;
            du.num = 1;

            player.dissections.push_back ( du );
         }
      } else
         player.research.progress+= techs[i]->researchpoints / dissectunitresearchpointsplus;

   }
   */
}



void         generatevehicle_cl ( Vehicletype* fztyp,
                                  int         col,
                                  Vehicle* &   vehicle,
                                  int          x,
                                  int          y )
{
   if ( actmap->player[ actmap->actplayer ].research.vehicletypeavailable ( fztyp ) ) {

      vehicle = new Vehicle ( fztyp, actmap, col );
              
      logtoreplayinfo ( rpl_produceunit, (int) fztyp->id , (int) col * 8, x, y, int(0), (int) vehicle->networkid );

      vehicle->xpos = x;
      vehicle->ypos = y;

      int height = -1;
      int maxmove = -1;
      for ( int h = 0; h < 8; h++ )
         if ( fztyp->height & ( 1 << h ))
            if ( fztyp->movement[h] > maxmove ) {
               maxmove = fztyp->movement[h];
               height = h;
            }
      vehicle->height = 1 << height;
      vehicle->setMovement ( 0 );



      if ( actmap->getgameparameter(cgp_bi3_training) >= 1 ) {
         int cnt = 0;

         for ( tmap::Player::BuildingList::iterator bi = actmap->player[actmap->actplayer].buildingList.begin(); bi != actmap->player[actmap->actplayer].buildingList.end(); bi++ )
            if ( (*bi)->typ->special & cgtrainingb )
               cnt++;

         vehicle->experience += cnt * actmap->getgameparameter(cgp_bi3_training);
         if ( vehicle->experience > maxunitexperience )
            vehicle->experience = maxunitexperience;
      }
   } else
     vehicle = NULL;
} 




#if 0


void cmousecontrol :: reset ( void )
{
   mousestat = 0;
}


void cmousecontrol :: chkmouse ( void )
{
   if ( CGameOptions::Instance()->mouse.fieldmarkbutton )
      if ( mouseparams.taste == CGameOptions::Instance()->mouse.fieldmarkbutton ) {
         int x;
         int y;
         int r = getfieldundermouse ( &x, &y );

         if ( r )
            if ( (cursor.posx != x || cursor.posy != y) && ( moveparams.movestatus == 0) ) {
               // collategraphicoperations cgo;
               mousestat = 1;
               mousevisible(false);
               cursor.hide();
               cursor.posx = x;
               cursor.posy = y;
               cursor.show();

               mousevisible(true);
            }
      }

   if ( CGameOptions::Instance()->mouse.centerbutton )
      if ( mouseparams.taste == CGameOptions::Instance()->mouse.centerbutton ) {
         int x; 
         int y;
         int r = getfieldundermouse ( &x, &y );
         x += actmap->xpos;
         y += actmap->ypos;
   
         if ( r ) {
            int newx = x - idisplaymap.getscreenxsize() / 2;
            int newy = y - idisplaymap.getscreenysize() / 2;

            if ( newx < 0 )
               newx = 0;
            if ( newy < 0 )
               newy = 0;
            if ( newx > actmap->xsize - idisplaymap.getscreenxsize() )
               newx = actmap->xsize - idisplaymap.getscreenxsize();
            if ( newy > actmap->ysize - idisplaymap.getscreenysize() )
               newy = actmap->ysize - idisplaymap.getscreenysize();
   
            if ( newy & 1 )
               newy--;

            if ( newx != actmap->xpos  || newy != actmap->ypos ) {
               // collategraphicoperations cgo;
               cursor.hide();
               actmap->xpos = newx;
               actmap->ypos = newy;
               displaymap();
               cursor.posx = x - actmap->xpos;
               cursor.posy = y - actmap->ypos;

               // cursor.gotoxy ( x, y );
               cursor.show();
            }
            while ( mouseparams.taste == CGameOptions::Instance()->mouse.centerbutton )
               releasetimeslice();
         }
      }

   if ( CGameOptions::Instance()->mouse.smallguibutton )
      if ( mouseparams.taste == CGameOptions::Instance()->mouse.smallguibutton ) {
         int x;
         int y;
         int r = getfieldundermouse ( &x, &y );

         if ( r )
            if ( (cursor.posx != x || cursor.posy != y) && ( moveparams.movestatus == 0   ||  getfield(actmap->xpos + x , actmap->ypos + y)->a.temp == 0) ) {
               // collategraphicoperations cgo;
               mousestat = 1;
               mousevisible(false);
               cursor.hide();
               cursor.posx = x;
               cursor.posy = y;
               bool mapRespositioned = cursor.show();
               if ( mapRespositioned )
                  while ( mouseparams.taste == CGameOptions::Instance()->mouse.smallguibutton ) {
                     releasetimeslice();
                  }

               mousevisible(true);
            } else
              if (    mousestat == 2
                  ||  mousestat == 0
                  ||  ((moveparams.movestatus || pendingVehicleActions.action) && getfield( actmap->xpos + x, actmap->ypos + y)->a.temp )
                  ||  CGameOptions::Instance()->mouse.singleClickAction ) {
                 {
                    // collategraphicoperations cgo;
                    if ( cursor.posx != x || cursor.posy != y ) {
                       mousevisible(false);
                       cursor.hide();
                       cursor.posx = x;
                       cursor.posy = y;
                       bool mapRespositioned = cursor.show();
                       // dashboard.paint( getactfield(), actmap-> playerView );
                       if ( mapRespositioned )
                          while ( mouseparams.taste == CGameOptions::Instance()->mouse.smallguibutton ) {
                             releasetimeslice();
                          }


                       mousevisible(true);
                    }

                    actgui->painticons();
                 }
                 pfield fld = getactfield();
                 bool positionedUnderCursor = false;
                 if (( fld->vehicle || fld->building) && fieldvisiblenow(fld))
                    positionedUnderCursor = true;
                 if ( fld->a.temp )
                    positionedUnderCursor = true;
                 actgui->paintsmallicons( CGameOptions::Instance()->mouse.smallguibutton, !positionedUnderCursor );
                 mousestat = 0;
              }
      } else
        if ( mousestat == 1 )
           mousestat = 2;

   if ( CGameOptions::Instance()->mouse.largeguibutton )
      if ( mouseparams.taste == CGameOptions::Instance()->mouse.largeguibutton ) {
         int x;
         int y;
         int r = getfieldundermouse ( &x, &y );
         if ( r && ( cursor.posx != x || cursor.posy != y) ) {
            mousevisible(false);
            cursor.hide();
            cursor.posx = x;
            cursor.posy = y;
            bool mapRespositioned = cursor.show();
            if ( mapRespositioned )
               while ( mouseparams.taste == CGameOptions::Instance()->mouse.smallguibutton ) {
                  releasetimeslice();
               }
            mousevisible(true);
         }
         actgui->painticons();

         actgui->runpressedmouse ( mouseparams.taste );
      }

   if ( CGameOptions::Instance()->mouse.unitweaponinfo )
      if ( mouseparams.taste == CGameOptions::Instance()->mouse.unitweaponinfo ) {
         int x;
         int y;
         int r = getfieldundermouse ( &x, &y );
         if ( r && ( cursor.posx != x || cursor.posy != y) ) {
            mousevisible(false);
            cursor.hide();
            cursor.posx = x;
            cursor.posy = y;
            cursor.show();
            mousevisible(true);
         }
         actgui->painticons();
         if ( getactfield()->vehicle ) {
//            dashboard.paintvehicleinfo( getactfield()->vehicle, NULL, NULL, NULL );
  //          dashboard.paintlweaponinfo();
         }
      }

/*
   if ( mouseparams.taste == minmenuekey ) {
      int x;
      int y;
      if (  getfieldundermouse ( &x, &y ) ) {
         actgui->paintsmallicons( minmenuekey );
      }
   }
*/

}
#endif

void  checkforvictory ( )
{
   if ( !actmap->continueplaying ) {
      int plnum = 0;
      for ( int i = 0; i < 8; i++ )
         if ( !actmap->player[i].exist() && actmap->player[i].existanceAtBeginOfTurn ) {
            int to = 0;
            for ( int j = 0; j < 8; j++ )
               if ( j != i )
                  to |= 1 << j;


            char txt[1000];
            const char* sp = getmessage( 10010 ); // Message "player has been terminated"

            sprintf ( txt, sp, actmap->player[i].getName().c_str() );
            new Message ( txt, actmap, to  );

            actmap->player[i].existanceAtBeginOfTurn = false;

            if ( i == actmap->actplayer ) {
               displaymessage ( getmessage ( 10011 ),1 );

               int humannum=0;
               for ( int j = 0; j < 8; j++ )
                  if (actmap->player[j].exist() && actmap->player[j].stat == Player::human )
                     humannum++;
               if ( humannum )
                  next_turn();
               else {
                  delete actmap;
                  actmap = NULL;
                  throw NoMapLoaded();
               }
            }
         } else
            plnum++;

      if ( plnum <= 1 ) {
         if ( actmap->player[actmap->actplayer].ai &&  actmap->player[actmap->actplayer].ai->isRunning() ) {
            displaymessage("You lost!",1);
         } else {
            displaymessage("Congratulations!\nYou won!",1);
            if (choice_dlg("Do you want to continue playing ?","~y~es","~n~o") == 2) {
               delete actmap;
               actmap = NULL;
               throw NoMapLoaded();
            } else {
               actmap->continueplaying = 1;
               if ( actmap->replayinfo ) {
                  delete actmap->replayinfo;
                  actmap->replayinfo = 0;
               }
            }
         }
      }
   }
}

