

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


#include "buildings.h"
#include "vehicle.h"
#include "typen.h"
#include "gamemap.h"
#include "stack.h"
#include "viewcalculation.h"
#include "replay.h"
#include "attack.h"
#include "util/messaginghub.h"
#include "mapdisplayinterface.h"
#include "reactionfire.h"
#include "spfst.h"
#include "itemrepository.h"

                typedef struct tunitlist* punitlist;
                struct tunitlist {
                        Vehicle* eht;
                        punitlist next;
                     };

const int rfPlayerCount = playerNum-1;

                punitlist unitlist[rfPlayerCount];


int tsearchreactionfireingunits :: maxshootdist[rfPlayerCount] = { -1, -1, -1, -1, -1, -1, -1, -1 };

tsearchreactionfireingunits :: tsearchreactionfireingunits ( GameMap* gamemap )
{
   this->gamemap = gamemap;
   for ( int i = 0; i < rfPlayerCount; i++ )
      unitlist[i] = NULL;
}

void tsearchreactionfireingunits :: init ( Vehicle* eht, const MapCoordinate3D& pos )
{
   initLimits();
   findOffensiveUnits( eht, pos.getNumericalHeight(), pos.x, pos.y, pos.x, pos.y );
}


void tsearchreactionfireingunits :: initLimits()
{
   if ( maxshootdist[0] == -1 ) {
      for (int i = 0; i < rfPlayerCount; i++ )
         maxshootdist[i] = 0;

      for (int i = 0; i < vehicleTypeRepository.getNum(); i++ ) {
         VehicleType* fzt = vehicleTypeRepository.getObject_byPos ( i );
         if ( fzt )
            for (int j = 0; j < fzt->weapons.count; j++ )
               if ( fzt->weapons.weapon[j].shootable() )
                  for (int h = 0; h < choehenstufennum; h++ )
                     if ( fzt->weapons.weapon[j].targ & ( 1 << h ) )
                        if ( fzt->weapons.weapon[j].maxdistance > maxshootdist[h] )
                           maxshootdist[h] = fzt->weapons.weapon[j].maxdistance;
      }
   }
}

void tsearchreactionfireingunits :: findOffensiveUnits( Vehicle* vehicle, int height, int x1, int y1, int x2, int y2 )
{
   x1 -= maxshootdist[height];
   y1 -= maxshootdist[height];
   x2 += maxshootdist[height];
   y2 += maxshootdist[height];

   GameMap* gamemap = vehicle->getMap();
   
   if ( x1 < 0 )
      x1 = 0;
   if ( y1 < 0 )
      y1 = 0;
   if ( x2 >= gamemap->xsize )
      x2 = gamemap->xsize -1;
   if ( y2 >= gamemap->ysize )
      y2 = gamemap->ysize -1;

   for ( int y = y1; y <= y2; y++ )
      for ( int x = x1; x <= x2; x++ ) {
         Vehicle* eht = gamemap->getField ( x, y )->vehicle;
         if ( eht )
            if ( eht->color != vehicle->color )
               if ( eht->reactionfire.getStatus() >= Vehicle::ReactionFire::ready )
                  // if ( eht->reactionfire.enemiesAttackable & ( 1 << ( vehicle->color / 8 )))
                     if ( gamemap->player[eht->getOwner()].diplomacy.isHostile( gamemap->actplayer))
                        if ( attackpossible2u ( eht, vehicle, NULL, vehicle->typ->height ) )
                           addunit ( eht );

      }
      
   if ( gamemap->getField(vehicle->xpos, vehicle->ypos)->vehicle == vehicle )
      for ( int i = 0; i < rfPlayerCount; i++ )
         if ( fieldvisiblenow ( gamemap->getField ( vehicle->xpos, vehicle->ypos ), i )) {
            punitlist ul  = unitlist[i];
            while ( ul ) {
               punitlist next = ul->next;
               
               AttackWeap* atw = attackpossible ( ul->eht, vehicle->xpos, vehicle->ypos );
               for ( int j = 0; j < atw->count; ++j )
                  if ( ul->eht->reactionfire.weaponShots[atw->num[j]] ) {
                     removeunit ( ul->eht );
                     break;
                  }   

               delete atw;
               ul = next;
            } /* endwhile */
         }

   for ( int player = 0; player < gamemap->getPlayerCount(); ++player ) {
      if ( gamemap->getPlayer(vehicle).diplomacy.isHostile( player )) {
         for ( Player::VehicleList::iterator i = gamemap->getPlayer(vehicle).vehicleList.begin(); i != gamemap->getPlayer(vehicle).vehicleList.end(); ++i) {
            if ( fieldvisiblenow( gamemap->getField( (*i)->getPosition() ), player )) {
               if ( visibleUnits.find( *i ) == visibleUnits.end() )
                  visibleUnits[*i] = 1 << player;
               else
                  visibleUnits[*i] |= 1 << player;
            }
         }
      }
   }
     

}


void tsearchreactionfireingunits :: init ( Vehicle* vehicle, const AStar3D::Path& fieldlist )
{
   initLimits();

   int x1 = maxint;
   int y1 = maxint;
   int x2 = 0;
   int y2 = 0;

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
   findOffensiveUnits ( vehicle, getFirstBit ( vehicle->height ), x1,y1,x2,y2);
          
}

void  tsearchreactionfireingunits :: addunit ( Vehicle* eht )
{
   int c = eht->getOwner();
   if ( c < 0 || c >= rfPlayerCount )
       return;

   punitlist ul = new tunitlist;
   ul->eht = eht;
   ul->next= unitlist[c];
   unitlist[c] = ul;
}


void tsearchreactionfireingunits :: removeunit ( Vehicle* vehicle )
{
   int c = vehicle->getOwner();
   if ( c < 0 || c >= rfPlayerCount )
       return;

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


int tsearchreactionfireingunits :: attack( Vehicle* attacker, Vehicle* target, const Context& context )
{
   MapField* fld = target->getMap()->getField( target->getPosition() );
   
   
   int result = 0;
   if ( attacker->reactionfire.canPerformAttack( target )) { 
      AttackWeap* atw = attackpossible ( attacker, target->xpos, target->ypos );
      if ( atw->count ) { // && (attacker->reactionfire.enemiesAttackable & (1 << (target->color / 8)))) {

         int strength = 0;
         int num = -1;
         for ( int j = 0; j < atw->count; j++ )
            if ( attacker->reactionfire.weaponShots[atw->num[j]] > 0 )
               if ( atw->strength[j]  > strength ) {
                  strength = atw->strength[j];
                  num = j;
               }

         if ( num >= 0 ) {

            int visibility = 0;
            if ( context.display ) {
               MessagingHub::Instance().statusInformation( "attacking with weapon " + ASCString::toString( atw->num[num] ));

               if ( fieldvisiblenow ( gamemap->getField (attacker->xpos, attacker->ypos ), gamemap->getPlayerView())) {
                  ++visibility;
                  context.display->cursor_goto( attacker->getPosition() );
                  int t = ASC_GetTicks();
                  while ( t + 15 > ASC_GetTicks() )
                     releasetimeslice();
               }

               if ( fieldvisiblenow ( fld, target, gamemap->getPlayerView() )) {
                  ++visibility;
                  context.display->cursor_goto( target->getPosition() );
                  int t = ASC_GetTicks();
                  while ( t + 15 > ASC_GetTicks() )
                     releasetimeslice();
               }
            }

            tunitattacksunit battle ( attacker, target, false, atw->num[num], true );
            // int nwid = target->networkid;

            if ( context.display && visibility)
               context.display->showBattle( battle ); 
            else
               battle.calc();

            if ( battle.dv.damage >= 100 )
               result = 2;
            else
               result = 1;

            battle.setresult( context );

            updateFieldInfo();
         }
      }
      delete atw;
   }

   return result;
}


int  tsearchreactionfireingunits :: checkfield ( const MapCoordinate3D& pos, Vehicle* &vehicle, const Context& context )
{

   int attacks = 0;
   int result = 0;

   MapField* fld = gamemap->getField( pos.x, pos.y );

   for ( int i = 0; i < rfPlayerCount; i++ ) {
      if ( fieldvisiblenow ( fld, i )) {
         punitlist ul  = unitlist[i];
         while ( ul  &&  !result ) {
            punitlist next = ul->next;
            
            if ( ul->eht ) { // the unit
                int nwid = ul->eht->networkid;
                int r = attack( ul->eht, vehicle, context );
                ul->eht = gamemap->getUnit(nwid); // could have been destroyed and be null now
                if ( r > 0 )
                   ++attacks;

                if ( r > 1 )
                   result = 1;
            }
            
            ul = next;
         } /* endwhile */
      }
   }

   if ( result )
      vehicle = NULL;

   return attacks;
}


int  tsearchreactionfireingunits :: finalCheck ( int currentPlayer, const Context& context )
{
   int destroyedUnits = 0;
   for ( int player = 0; player < gamemap->getPlayerCount(); ++player ) {
      if ( gamemap->getPlayer(currentPlayer).diplomacy.isHostile( player )) {
         list<int> exposedTargets;
         for ( Player::VehicleList::iterator exposedTarget = gamemap->getPlayer(currentPlayer).vehicleList.begin(); exposedTarget != gamemap->getPlayer(currentPlayer).vehicleList.end(); ++exposedTarget) {
            if ( fieldvisiblenow( gamemap->getField( (*exposedTarget)->getPosition() ), player )) {
               if ( visibleUnits.find( *exposedTarget ) == visibleUnits.end() || !(visibleUnits[*exposedTarget] & (1 << player)) ) { 
                  if ( (*exposedTarget)->getMap()->getField( (*exposedTarget)->getPosition() )->unitHere( *exposedTarget ))
                     exposedTargets.push_back( (*exposedTarget )->networkid );
               }
            }
         }
          
         for ( list<int>::iterator exposedTargetID = exposedTargets.begin(); exposedTargetID != exposedTargets.end(); ++exposedTargetID ) {
            Vehicle* exposedTarget = gamemap->getUnit( *exposedTargetID );
            if ( exposedTarget )  {
               bool destroyed = false;
               
               for ( Player::VehicleList::iterator enemyUnit = gamemap->getPlayer(player).vehicleList.begin(); enemyUnit != gamemap->getPlayer(player).vehicleList.end() && !destroyed; ++enemyUnit) {
                  if ( (*enemyUnit)->reactionfire.canPerformAttack( exposedTarget )) {
                     if ( beeline( *enemyUnit, exposedTarget ) <= maxshootdist[ exposedTarget->getPosition().getNumericalHeight()]) {
                        int r = attack( *enemyUnit, exposedTarget, context );
                        if ( r > 1 ) { 
                           destroyed = true;
                           ++destroyedUnits;
                        }
                     }
                  } 
               }
            }
         }
      }
   }
   return destroyedUnits;
}



tsearchreactionfireingunits :: ~tsearchreactionfireingunits()
{
   for ( int i = 0; i < rfPlayerCount; i++ ) {
      punitlist ul = unitlist[i];
      while ( ul ) {
         punitlist ul2 = ul->next;
         delete ul;
         ul = ul2;
      }
   }
}



