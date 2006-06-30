/*! \file controls.cpp
   Controlling units (which is graudally moved to vehicletype.cpp and unitctrl.cpp );
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

// #include "basegfx.h"
#include "newfont.h"
#include "spfst.h"
#include "loaders.h"
#include "misc.h"
#include "controls.h"
#include "events.h"
#include "stack.h"
#include "dlg_box.h"
#include "dialog.h"
#include "attack.h"
#include "gamedlg.h"
#include "gameoptions.h"
#include "ai/ai.h"
#include "errors.h"
// #include "password_dialog.h"
#include "viewcalculation.h"
#include "replay.h"
#include "resourcenet.h"
#include "itemrepository.h"
#include "strtmesg.h"
#include "messagedlg.h"
#include "gameevent_dialogs.h"
#include "cannedmessages.h"
#include "network.h"
#include "mapdisplay.h"

#include "dialogs/choosetech.h"

tmoveparams moveparams;


class InitControls {
  public:
   InitControls ( void ) 
   {
      memset ( (void*) &moveparams, 0, sizeof ( moveparams ));
   };
} initControls;


/*
  class   tsearchexternaltransferfields : public SearchFields {
                      public:
                                Building*            bld;
                                char                    numberoffields;
                                void                    searchtransferfields( Building* building );
                                virtual void            testfield ( const MapCoordinate& mc );
                                tsearchexternaltransferfields ( GameMap* _gamemap ) : SearchFields ( _gamemap ) {};
                             };

void         tsearchexternaltransferfields :: testfield( const MapCoordinate& mc )
{ 
     tfield* fld  = gamemap->getField ( mc );
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
   if ( bld->typ->hasFunction( ContainerBaseType::ExternalEnergyTransfer  ) ||
        bld->typ->hasFunction( ContainerBaseType::ExternalMaterialTransfer  ) ||
        bld->typ->hasFunction( ContainerBaseType::ExternalFuelTransfer  ) ||
        bld->typ->hasFunction( ContainerBaseType::ExternalAmmoTransfer  ) ) {
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
*/



  class   tsearchdestructbuildingfields : public SearchFields {
                      public:
                                Vehicle*                vehicle;
                                char                    numberoffields;
                                tfield*                  startfield;
                                void                    initdestructbuilding( int x, int y );
                                virtual void            testfield ( const MapCoordinate& mc );
                                tsearchdestructbuildingfields ( GameMap* _gamemap ) : SearchFields ( _gamemap ) {};
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
   tfield* fld = getfield(xp,yp);
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
                       tputmine ( GameMap* _gamemap ) : SearchFields ( _gamemap ) {};
              };


void         tputmine::testfield(const MapCoordinate& mc)
{
   tfield* fld = gamemap->getField(mc);
   if ( !fld->vehicle  &&  !fld->building && fieldvisiblenow( fld, player )) {
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
         tfield* fld = actmap->getField(pos);
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
               tfield* fld = actmap->getField(pos);
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
               tfield* fld = getfield( dest.x, dest.y );
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

   static const  int         movemalus[6]  = { 0, 3, 5, 0, 5, 3 };
   
   if ( checkHemming )
      for (int c = 0; c < sidenum; c++) {
         int x = dest.x;
         int y = dest.y;
         getnextfield( x,  y, c );
         tfield* fld = getfield ( x, y );
         if ( fld ) {
           int d = (c - direc);

           if (d >= sidenum)
              d -= sidenum;

           if (d < 0)
              d += sidenum;

           tfield* fld = getfield(x,y);
           if ( fld->vehicle && dest.getNumericalHeight() >= 0 ) {
              if ( vehicle->getMap()->getPlayer(vehicle).diplomacy.isHostile( fld->vehicle->getOwner() ) )
                 if ( attackpossible28(fld->vehicle,vehicle, NULL, dest.getBitmappedHeight() ))
                    movecost += movemalus[d];
              
           }
         }
      }

    /*******************************/
    /*    Wind calculation        ÿ */
    /*******************************/
   if ( wm && checkWind )
      if (dest.getNumericalHeight() >= 4 && dest.getNumericalHeight() <= 6 &&
          start.getNumericalHeight() >= 4 && start.getNumericalHeight() <= 6 &&
          actmap->weather.windSpeed  ) {
         movecost -=  wm->getDist( direc ) * dist;
         fuelcost -=  wm->getDist ( direc ) * dist;

         if ( movecost < 1 )
           movecost = 1;

         if ( fuelcost <= 0 )
           fuelcost = 0;
      }
   return make_pair(movecost,fuelcost);
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
               actstorage.resource(i) += getResource ( getStorageCapacity().resource(i) -  actstorage.resource(i), i, 0 );
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
      int placed;
      {
         PutResource putresource ( getMap(), scope );
         placed = putresource.getresource ( entryPosition.x, entryPosition.y, resourcetype, need, queryonly, color/8, scope );
      }
      // if ( !queryonly && placed > 0 )
      //   resourceChanged();
      return placed;
   }
}


int  Building :: getResource ( int      need,    int resourcetype, bool queryonly, int scope )
{
   if ( need < 0 )
      return -putResource( -need, resourcetype, queryonly, scope );
   else {
      int got;
      {
         GetResource gr ( getMap(), scope );
         got = gr.getresource ( entryPosition.x, entryPosition.y, resourcetype, need, queryonly, color/8, scope );
      }
      // if ( !queryonly && got > 0 )
      //   resourceChanged();
      return got;
      
   }
}

int  Building :: getResource ( int      need,    int resourcetype ) const
{
   int scope = 1;
   int got;
   {
      GetResource gr ( getMap(), scope );
      got = gr.getresource ( entryPosition.x, entryPosition.y, resourcetype, need, true, color/8, scope );
   }
   return got;
}

void researchCheck( Player& player )
{
   Research& research = player.research;
   if (research.activetechnology == NULL && research.progress ) 
      chooseTechnology( player );
      
   if ( research.activetechnology )
      if( find ( research.developedTechnologies.begin(), research.developedTechnologies.end(), research.activetechnology->id ) != research.developedTechnologies.end()) {
         research.progress = 0;
         chooseTechnology( player );
      }
      
   while ( research.activetechnology  &&  (research.progress >= research.activetechnology->researchpoints)) {
      int mx = research.progress - research.activetechnology->researchpoints;

      showtechnology( research.activetechnology );
      
      if ( research.activetechnology )
         logtoreplayinfo ( rpl_techResearched, research.activetechnology->id, player.getPosition() );

      NewVehicleTypeDetection pfzt;

      research.addtechnology();

      pfzt.evalbuffer ();

      chooseTechnology( player );

      research.progress = mx;
   }
}

