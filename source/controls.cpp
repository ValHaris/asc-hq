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
#include "events.h"
#include "stack.h"
#include "dlg_box.h"
#include "dialog.h"
#include "attack.h"
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
#include "cannedmessages.h"
#include "network.h"
#include "mapdisplay.h"

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


bool authenticateUser ( tmap* actmap, int forcepasswordchecking = 0, bool allowCancel = true )
{
   for ( int p = 0; p < 8; p++ )
      actmap->player[p].existanceAtBeginOfTurn = actmap->player[p].exist() && actmap->player[p].stat != Player::off;

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
                  stat = enterpassword ( actmap->player[actmap->actplayer].passwordcrc, specifyPassword, allowCancel );
                  if ( !stat )
                     return false;
               } while ( actmap->player[actmap->actplayer].passwordcrc.empty() && stat && viewtextquery ( 910, "warning", "~e~nter password", "~c~ontinue without password" ) == 0 ); /* enddo */
            }
      } else
         infoMessage("next player is " + actmap->player[actmap->actplayer].getName() );
   }

   moveparams.movestatus = 0;

   actmap->playerView = actmap->actplayer;

   computeview( actmap );
   actmap->beginTurn(); 

   updateFieldInfo();
   return true;
   
}



void researchCheck( Player& player )
{
   Research& research = player.research;
   if (research.activetechnology == NULL && research.progress ) 
      choosetechnology();
      
   if ( research.activetechnology )
      if( find ( research.developedTechnologies.begin(), research.developedTechnologies.end(), research.activetechnology->id ) != research.developedTechnologies.end()) {
         research.progress = 0;
         choosetechnology();
      }
      
   while ( research.activetechnology  &&  (research.progress >= research.activetechnology->researchpoints)) {
      int mx = research.progress - research.activetechnology->researchpoints;

      showtechnology( research.activetechnology );
      
      if ( research.activetechnology )
         logtoreplayinfo ( rpl_techResearched, research.activetechnology->id, player.getPosition() );

      NewVehicleTypeDetection pfzt;

      research.addtechnology();

      pfzt.evalbuffer ();

      choosetechnology();

      research.progress = mx;
   }
}






void runai( int playerView )
{
   actmap->playerView = playerView;

   if ( !actmap->player[ actmap->actplayer ].ai )
      actmap->player[ actmap->actplayer ].ai = new AI ( actmap, actmap->actplayer );

   actmap->player[ actmap->actplayer ].ai->run();
   updateFieldInfo();
}


int findNextPlayer( tmap* actmap )
{
   int p = actmap->actplayer;
   bool found = false;
   do {
      ++p;
      if ( p >= actmap->getPlayerCount()) 
         p = 0;
      
      if ( actmap->player[p].exist() )
         if ( actmap->player[p].stat != Player::off )
            found = true;
      
   } while ( !found );
   return p;
}

void next_turn ( int playerView )
{
   int lastPlayer = actmap->actplayer;
   int lastTurn = actmap->time.turn();

   if  ( lastPlayer >= 0 )
      actmap->endTurn();
   
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

      
   if ( findNextPlayer( actmap ) == lastPlayer ) {
      if ( !actmap->continueplaying ) {
         viewtext2(904);
         if (choice_dlg("Do you want to continue playing ?","~y~es","~n~o") == 2) {
            delete actmap;
            actmap = NULL;
            throw NoMapLoaded();
         } else {
            actmap->continueplaying = true;
            if ( actmap->replayinfo ) {
               delete actmap->replayinfo;
               actmap->replayinfo = NULL;
            }
         }
      }   
   }
      
 
   int loop = 0;
   bool closeLoop = false;
        
   do {
      
      int nextPlayer = findNextPlayer( actmap );
      
      if ( nextPlayer <= lastPlayer ) {
         actmap->endRound();
         ++loop;
      }   
      
      if ( loop > 2 ) {
         displaymessage("no human players found !", 1 );
         delete actmap;
         actmap = NULL;
         throw NoMapLoaded();
      }

               
      actmap->actplayer = nextPlayer;   
      
      if ( actmap->player[nextPlayer].stat == Player::computer ) {
         actmap->beginTurn();
         runai( lastPlayer );
         actmap->endTurn();
      }
      
      if ( actmap->player[nextPlayer].stat == Player::suspended ) {
         actmap->beginTurn();
         actmap->endTurn();
      }   
         
      if ( actmap->player[nextPlayer].stat == Player::human || actmap->player[nextPlayer].stat == Player::supervisor ) {
         if ( actmap->network && lastPlayer >= 0 ) {
            actmap->network->send( actmap, lastPlayer, lastTurn );
            delete actmap;
            actmap = NULL;
            throw NoMapLoaded();
         } else
            closeLoop = true;
      }
   } while ( !closeLoop ); /* enddo */

   authenticateUser( actmap, 0, false );
}

void checkUsedASCVersions ( Player& currentPlayer )
{
   for ( int i = 0; i < 8; i++ )
      if  ( actmap->player[i].exist() )
         if ( actmap->actplayer != i )
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




void continuenetworkgame ()
{
   ASCString filename = selectFile( ASCString("*") + tournamentextension + ";*.asc", true );
   if ( filename.empty() )
      return;

   StatusMessageWindowHolder smw = MessagingHub::Instance().infoMessageWindow( "loading " + filename );
   FileTransfer ft;
   auto_ptr<tmap> newMap ( mapLoadingExceptionChecker( filename, MapLoadingFunction( &ft, &FileTransfer::loadPBEMFile )));
   if ( !newMap.get() )
      return;
   
   if ( authenticateUser( newMap.get() )) {
      delete actmap;
      actmap = newMap.release();
   }
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

