/***************************************************************************
                          tactics.cpp  -  description
                             -------------------
    begin                : Fri Mar 30 2001
    copyright            : (C) 2001 by Martin Bickel
    email                : bickel@asc-hq.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "ai_common.h"


const int attack_unitdestroyed_bonus = 90;


void AI :: searchTargets ( pvehicle veh, const MapCoordinate3D& pos, TargetVector& tl, int moveDist, AStar3D& vm, int hemmingBonus )
{

   npush ( veh->xpos );
   npush ( veh->ypos );
   npush ( veh->height );

   bool unitRadarActivated = getMap()->getField(veh->getPosition())->unitHere(veh);
   if ( unitRadarActivated )
      veh->removeview();
   // int fieldsWithChangedVisibility = evaluateviewcalculation ( getMap(), veh->xpos, veh->ypos, veh->typ->view, 0xff );
   veh->xpos = pos.x;
   veh->ypos = pos.y;
   veh->height = pos.getBitmappedHeight();
   veh->addview();
   int fieldsWithChangedVisibility = evaluateviewcalculation ( getMap(), veh->getPosition(), veh->typ->view, 0xff );


   VehicleAttack va ( NULL, NULL );
   if ( va.available ( veh )) {
      va.execute ( veh, -1, -1, 0 , 0, -1 );
      for ( int g = 0; g < va.attackableVehicles.getFieldNum(); g++ ) {
         int xp, yp;
         va.attackableVehicles.getFieldCoordinates ( g, &xp, &yp );
         pattackweap aw = &va.attackableVehicles.getData ( g );

         int bestweap = -1;
         int targdamage = -1;
         int weapstrength = -1;
         for ( int w = 0; w < aw->count; w++ ) {
            tunitattacksunit uau ( veh, getfield ( xp, yp)->vehicle, 1, aw->num[w] );
            uau.calc();
            if ( uau.dv.damage > targdamage ) {
               bestweap = aw->num[w];
               targdamage = uau.dv.damage;
               weapstrength = aw->strength[w];
            } else
               if ( uau.dv.damage == 100 )
                  if ( weapstrength == -1 || weapstrength > aw->strength[w] ) {
                     bestweap = aw->num[w];
                     targdamage = uau.dv.damage;
                     weapstrength = aw->strength[w];
                  }
         }

         if ( bestweap == -1 )
            displaymessage ( "inconsistency in AI :: searchTarget", 1 );


         MoveVariant* mv = new MoveVariant;

         tunitattacksunit uau ( veh, getfield ( xp, yp)->vehicle, 1, bestweap );
         mv->orgDamage = uau.av.damage;
         mv->damageAfterMove = uau.av.damage;
         mv->enemyOrgDamage = uau.dv.damage;
         uau.calc();


         mv->damageAfterAttack = uau.av.damage;
         mv->enemyDamage = uau.dv.damage;
         mv->enemy = getfield ( xp, yp )->vehicle;
         mv->movePos = pos;
         mv->attackx = xp;
         mv->attacky = yp;
         mv->weapNum = bestweap;
         mv->moveDist = moveDist;
         mv->attacker = veh;

         mv->positionThreat = getFieldThreat ( pos.x, pos.y ).threat[veh->getValueType()];


         for ( int nf = 0; nf < sidenum; nf++ ) {
            MapCoordinate mc = getNeighbouringFieldCoordinate ( MapCoordinate ( mv->attackx, mv->attacky), nf );
            pfield fld = getMap()->getField(mc);
            if ( fld && !veh->typ->wait)
               mv->neighbouringFieldsReachable[nf] = (vm.fieldVisited( MapCoordinate3D(mc.x, mc.y, pos.getBitmappedHeight()) ) || ( veh->xpos == mc.x && veh->ypos == mc.y )) && !fld->building && (!fld->vehicle || fld->unitHere(veh));
            else
               mv->neighbouringFieldsReachable[nf] = false;

         }

         int attackerDirection = getdirection ( xp, yp, pos.x, pos.y );
         float hemmingFactor = 1;
         for ( int nf = 0; nf < sidenum-1 && nf < hemmingBonus; nf++ ) {
            // we are starting opposite the attacker, since this is the highest hemming bonus
            int checkDir;
            if ( nf == 0 )
               checkDir = attackerDirection+sidenum/2;
            else {
               if ( nf & 1 )
                  checkDir = attackerDirection+sidenum/2 + (nf+1)/2;
               else
                  checkDir = attackerDirection+sidenum/2 - (nf+1)/2;
            }

            MapCoordinate mc = getNeighbouringFieldCoordinate ( MapCoordinate ( mv->attackx, mv->attacky), checkDir%sidenum );
            pfield fld = getMap()->getField(mc);
            if ( fld && !fld->building && !fld->vehicle )
               hemmingFactor += AttackFormula::getHemmingFactor ( nf );
         }

         mv->result = getAttackValue ( uau, mv->attacker, mv->enemy, hemmingFactor );

         if ( mv->result > 0 )
            tl.push_back ( mv );

      }
   }

   // if ( fieldsWithChangedVisibility )
   //   evaluateviewcalculation ( getMap(), veh->xpos, veh->ypos, veh->typ->view, 0xff );
   veh->removeview();
   npop ( veh->height );
   npop ( veh->ypos );
   npop ( veh->xpos );

   if ( unitRadarActivated )
      veh->addview();

   if ( fieldsWithChangedVisibility || 1 )  // viewbug.sav !!!!!
      evaluateviewcalculation ( getMap(), veh->getPosition(), veh->typ->view, 0xff );
}

bool operator > ( const AI::MoveVariant& mv1, const AI::MoveVariant& mv2 )
{
   return (     mv1.result > mv2.result
            || (mv1.result == mv2.result && mv1.positionThreat < mv2.positionThreat  )
            || (mv1.result == mv2.result && mv1.positionThreat == mv2.positionThreat && mv1.moveDist < mv2.moveDist) );  // mv1.moveDist < mv2.moveDist
}

bool operator < ( const AI::MoveVariant& mv1, const AI::MoveVariant& mv2 )
{
   return (     mv1.result < mv2.result
            || (mv1.result == mv2.result && mv1.positionThreat > mv2.positionThreat )
            || (mv1.result == mv2.result && mv1.positionThreat == mv2.positionThreat && mv1.moveDist > mv2.moveDist) );  // mv1.moveDist < mv2.moveDist
}


bool AI::moveVariantComp ( const AI::MoveVariant* mv1, const AI::MoveVariant* mv2 )
{
   return *mv1 < *mv2;
   // return ( mv1->result < mv2->result || (mv1->result == mv2->result && mv1->moveDist > mv2->moveDist ));
}

void AI::getAttacks ( AStar3D& vm, pvehicle veh, TargetVector& tv, int hemmingBonus, bool justOne )
{
   int orgxpos = veh->xpos ;
   int orgypos = veh->ypos ;

   if ( getfield ( veh->xpos, veh->ypos )->unitHere ( veh ) )  // unit not inside a building or transport
      searchTargets ( veh, veh->getPosition3D(), tv, 0, vm, hemmingBonus );

   if ( tv.size() && justOne )
      return;

   // Now we cycle through all fields that are reachable...
   if ( !veh->typ->wait ) {

      RefuelConstraint* apl = NULL;
      if ( RefuelConstraint::necessary ( veh, *this ))
        apl = new RefuelConstraint( *this, veh );

      int fuelLacking = 0;
      for ( AStar3D::Container::iterator ff = vm.visited.begin(); ff != vm.visited.end(); ++ff )
         if ( !ff->hasAttacked ) {
            pfield fld = getMap()->getField (ff->h);
            if ( !fld->vehicle && !fld->building ) {
                if ( !apl || apl->returnFromPositionPossible ( ff->h )) {
                   searchTargets ( veh, ff->h, tv, beeline ( ff->h.x, ff->h.y, orgxpos, orgypos ), vm, hemmingBonus );
                   if ( tv.size() && justOne )
                      return;
                } else
                   fuelLacking++;
             }
         }

      if ( apl ) {
         delete apl;
         apl = NULL;
      }

      if ( !tv.size() && fuelLacking )
         issueRefuelOrder( veh, true );
   }
}

AI::AiResult AI::executeMoveAttack ( pvehicle veh, TargetVector& tv )
{
   int unitNetworkID = veh->networkid;
   AiResult result;

   MoveVariant* mv = *max_element( tv.begin(), tv.end(), moveVariantComp );

   if ( mv->movePos != veh->getPosition3D() ) {
      BaseVehicleMovement vm2 ( mapDisplay );

      VisibilityStates org_vision =  _vision ;
      _vision = visible_now;

      vm2.execute ( veh, mv->movePos.x, mv->movePos.y, 0, mv->movePos.getNumericalHeight(), 1 );
      if ( vm2.getStatus() != 3 )
         displaymessage ( "AI :: executeMoveAttack \n error in movement step 0 with unit %d", 1, veh->networkid );

      vm2.execute ( NULL, mv->movePos.x, mv->movePos.y, 3, mv->movePos.getNumericalHeight(), 1 );
      if ( vm2.getStatus() != 1000 )
         displaymessage ( "AI :: executeMoveAttack \n error in movement step 2 with unit %d", 1, veh->networkid );

      result.unitsMoved ++;
      _vision = org_vision;
   }

   // the unit may have been shot down due to reactionfire during movement
   if ( !getMap()->getUnit(unitNetworkID)) {
      result.unitsDestroyed++;
      return result;
   }

   VehicleAttack va ( mapDisplay, NULL );
   va.execute ( veh, -1, -1, 0 , 0, -1 );
   if ( va.getStatus() != 2 )
      displaymessage ( "AI :: executeMoveAttack \n error in attack step 2 with unit %d", 1, veh->networkid );

   VehicleTypeEfficiencyCalculator vtec( *this, veh, mv->enemy );

   va.execute ( NULL, mv->attackx, mv->attacky, 2 , -1, mv->weapNum );
   if ( va.getStatus() != 1000 )
      displaymessage ( "AI :: executeMoveAttack \n error in attack step 3 with unit %d", 1, veh->networkid );

   vtec.calc();

   result.unitsMoved ++;

   // the unit may have been shot in the attack
   if ( !getMap()->getUnit(unitNetworkID)) {
      result.unitsDestroyed++;
      return result;
   }


   VehicleMovement vm3 ( mapDisplay, NULL );
   if ( vm3.available ( veh ))
      result += moveToSavePlace ( veh, vm3 );

   return result;
}

bool AI::targetsNear( pvehicle veh )
{
   AStar3D ast ( getMap(), veh, false, veh->getMovement() );
   ast.findAllAccessibleFields ();
   TargetVector tv;
   getAttacks ( ast, veh, tv, 0, true );
   if ( tv.size() )
      return true;
   else
      return false;
}


int AI::getDirForBestTacticsMove ( const pvehicle veh, TargetVector& tv )
{
   if ( tv.size() <= 0 )
      return -1;

   MoveVariant* mv = *max_element( tv.begin(), tv.end(), moveVariantComp );
   return getdirection ( veh->xpos, veh->ypos, mv->movePos.x, mv->movePos.y );
}

MapCoordinate AI::getDestination ( const pvehicle veh )
{
   AiParameter::Task task = veh->aiparam[ getPlayerNum() ]->getTask();
   if ( task == AiParameter::tsk_nothing || task == AiParameter::tsk_tactics ) {
      TargetVector tv;
      AStar3D ast ( getMap(), veh, false, veh->getMovement() );
      ast.findAllAccessibleFields ();
      getAttacks ( ast, veh, tv, 0 );

      if ( tv.size() > 0 ) {

         MoveVariant* mv = *max_element( tv.begin(), tv.end(), moveVariantComp );
         return MapCoordinate ( mv->movePos.x, mv->movePos.y );
      } else
         task = AiParameter::tsk_strategy;
   }

   if ( task == AiParameter::tsk_strategy ) {
      Section* sec = sections.getBest ( 0, veh );
      if ( sec )
         return MapCoordinate ( sec->centerx, sec->centery );
   }

   return MapCoordinate ( veh->xpos, veh->ypos );
}


AI::AiResult AI::moveToSavePlace ( pvehicle veh, VehicleMovement& vm3, int preferredHeight )
{
   int unitNetworkID = veh->networkid;

   AiResult result;

   if ( vm3.getStatus() == 0 )
      vm3.execute ( veh, -1, -1, 0, -1, -1 );

   // if there are no fields reachable
   if ( vm3.getStatus() == -107 )
      return result;

   if ( vm3.getStatus() != 2 )
      displaymessage ( "AI :: moveToSavePlace \n error in movement3 step 0 with unit %d", 1, veh->networkid );

   int xtogo = veh->xpos;
   int ytogo = veh->ypos;
   int threat = maxint;
   int dist = maxint;

   if ( getfield ( veh->xpos, veh->ypos)->unitHere ( veh ) ) {  // vehicle not in building / transport
      threat = int( getFieldThreat ( veh->xpos, veh->ypos).threat[ veh->aiparam[ getPlayerNum()]->valueType] * 1.5 + 1);
       // multiplying with 1.5 to make this field a bit unattractive, to allow other units (for example buggies) to attack from this field to, since it is probably quite a good position (else it would not have been chosen)
   }

   for ( int f = 0; f < vm3.reachableFields.getFieldNum(); f++ )
      if ( !vm3.reachableFields.getField( f )->vehicle && !vm3.reachableFields.getField( f )->building ) {
         int x,y;
         vm3.reachableFields.getFieldCoordinates ( f, &x, &y );
         AiThreat& ait = getFieldThreat ( x, y );
         int _dist = beeline ( x, y, veh->xpos, veh->ypos);

            // make fields far away a bit unattractive; we don't want to move the whole distance back again next turn
         int t = int( ait.threat[ veh->aiparam[ getPlayerNum()]->valueType ] * log ( double(_dist) )/log(double(10)) );

         if ( t < threat || ( t == threat && _dist < dist )) {
            threat = t;
            xtogo = x;
            ytogo = y;
            dist = _dist;
         }
      }

   if ( veh->xpos != xtogo || veh->ypos != ytogo ) {
      vm3.execute ( NULL, xtogo, ytogo, 2, -1, -1 );
      if ( vm3.getStatus() != 3 )
         displaymessage ( "AI :: moveToSavePlace \n error in movement3 step 2 with unit %d", 1, veh->networkid );

      vm3.execute ( NULL, xtogo, ytogo, 3, -1,  1 );
      if ( vm3.getStatus() != 1000 )
         displaymessage ( "AI :: moveToSavePlace \n error in movement3 step 3 with unit %d", 1, veh->networkid );

      result.unitsMoved++;
   }

   if ( !getMap()->getUnit( unitNetworkID ))
      result.unitsDestroyed++;

   return result;
}


int AI::changeVehicleHeight ( pvehicle veh, VehicleMovement* vm, int preferredDirection )
{
#if 0  // ####
   int bh = getBestHeight ( veh );
   if ( bh != veh->height && bh != -1 ) {
      ChangeVehicleHeight* cvh;
      int newheight;
      if ( bh > veh->height ) {
         cvh = new IncreaseVehicleHeight ( mapDisplay, NULL );
         newheight = veh->height << 1;
      } else {
         cvh = new DecreaseVehicleHeight ( mapDisplay, NULL );
         newheight = veh->height >> 1;
      }
      auto_ptr<ChangeVehicleHeight> acvh ( cvh );

      if ( newheight & veh->typ->height ) {
         if ( cvh->available ( veh ) ) {

            int stat = cvh->execute ( veh, -1, -1, 0, newheight, 1 );
            if ( stat == 2 ) {   // if the unit could change its height vertically, or the height change is not available, skip this block

               int bestx = -1;
               int besty = -1;
               int moveremain = minint;

               if ( preferredDirection == -1 ) {
                 /*
                  // making a backup
                  TemporaryContainerStorage tus ( veh );
                  veh->height = newheight;
                  veh->resetMovement ( );
                  MapCoordinate mc = getDestination ( veh );
                  preferredDirection = getdirection ( veh->xpos, veh->ypos, mc.x, mc.y );
                  tus.restore();
                  */
                  preferredDirection = getdirection ( veh->xpos, veh->ypos, getMap()->xsize/2, getMap()->ysize/2 );
               }

               for ( int i = 0; i < cvh->reachableFields.getFieldNum(); i++ ) {
                  int newMoveRemain = cvh->reachableFields.getData ( i ).dist;

                  // check if we move in the direction that should be moved to
                  if ( preferredDirection != -1 ) {
                     int xp, yp;
                     cvh->reachableFields.getFieldCoordinates ( i, &xp, &yp );

                     int dir = preferredDirection - getdirection ( veh->xpos, veh->ypos, xp, yp );
                     if ( dir > sidenum/2 )
                        dir -= sidenum;
                     if ( dir < -sidenum/2 )
                        dir += sidenum;

                     dir = abs ( dir );
                     // ok, now we have the directional difference in dir, with a range of 0 .. 3 ; 0 meaning moving in the preferred direction


                     newMoveRemain -= (dir*2-3)* veh->typ->steigung*maxmalq / 3;
                     // if dir==0, we subtract -steigung   => newMoveRemain is getting larger because we move in the right direction
                     // if dir==3, we subtract +steigung
                  }

                  if ( newMoveRemain > moveremain ) {
                     cvh->reachableFields.getFieldCoordinates ( i, &bestx, &besty );
                     moveremain = newMoveRemain;
                  }
               }

               if ( bestx != -1 && besty != -1 ) {
                  cvh->execute ( NULL, bestx, besty, 2, -1, -1 );
                  if ( cvh->getStatus() == 1000 )
                     return 1;

                  if ( cvh->getStatus() != 3 )
                     displaymessage ( "AI :: changeVehicleHeight \n error in changeHeight step 2 with unit %d", 1, veh->networkid );

                  cvh->execute ( NULL, bestx, besty, 3, -1, -1 );
                  if ( cvh->getStatus() != 1000 )
                     displaymessage ( "AI :: changeVehicleHeight \n error in changeHeight step 3 with unit %d", 1, veh->networkid );

                  return 1;
               } else
                  return -1;
            } else {
               if ( stat == 1000 )
                 return 1;

               if ( veh->typ->steigung == 0 )
                 return -2;

               return -1;
            }
         } else {  // cvh->available
            return -2;
         }
      }

   }
   #endif
   return 0;
}


AI::AiResult AI::tactics( void )
{
   const int tsk_num = 3;
   AiParameter::Task tasks[tsk_num] = { AiParameter::tsk_nothing,
                                        AiParameter::tsk_tactics,
                                        AiParameter::tsk_tactwait
                                      };
   AiResult result;

   displaymessage2("starting tactics ... ");

   typedef list<pvehicle> TactVehicles;
   TactVehicles tactVehicles;

   for ( Player::VehicleList::iterator vi = getPlayer().vehicleList.begin(); vi != getPlayer().vehicleList.end(); vi++ ) {
      pvehicle veh = *vi;

      bool unitUsable = false;
      if ( veh->aiparam[ getPlayerNum() ]->getJob() == AiParameter::job_fight
        || veh->aiparam[ getPlayerNum() ]->getJob() == AiParameter::job_undefined
        || (veh->aiparam[ getPlayerNum() ]->getJob() == AiParameter::job_guard && veh->aiparam[ getPlayerNum() ]->dest_nwid == -1 && veh->aiparam[ getPlayerNum() ]->dest.x == -1 ))
         for ( int j = 0; j < tsk_num; j++ )
            if ( veh->aiparam[ getPlayerNum() ]->getTask() == tasks[j] )
               unitUsable = true;

      int maxWeapDist = minint;
      for ( int w = 0; w < veh->typ->weapons.count; w++ )
         if ( veh->typ->weapons.weapon[w].shootable() )
            maxWeapDist = max ( veh->typ->weapons.weapon[w].maxdistance , maxWeapDist );

      int maxMove = minint;
      for ( int h = 0; h < 8; h++ )
         if ( veh->typ->height & ( 1 << h ))
            maxMove = max ( veh->typ->movement[h], maxMove );

      if ( maxWeapDist > 0 ) {
         bool enemiesNear = false;
         int ydist = (maxMove + maxWeapDist) / maxmalq * 2;
         int xdist = ydist / 4;
         for ( int x = veh->xpos - xdist; x <= veh->xpos + xdist; x++ )
            for ( int y = veh->ypos - ydist; y <= veh->ypos + ydist; y++ ) {
               pfield fld = getMap()->getField(x,y );
               if ( fld ) {
                  if ( fld->vehicle && getdiplomaticstatus2 ( veh->color, fld->vehicle->color ) == cawar )
                     enemiesNear = true;
                  if ( fld->building && getdiplomaticstatus2 ( veh->color, fld->building->color ) == cawar )
                     enemiesNear = true;
               }
            }

         if ( unitUsable && enemiesNear)
            tactVehicles.push_back ( veh );
      }
   }

   int hemmingBonus = 5;

   int lastTactVehiclesSize; // = tactVehicles.size();

   while ( !tactVehicles.empty() ) {
      lastTactVehiclesSize = tactVehicles.size();

      typedef map<int, MoveVariantContainer> Targets;
      Targets targets;

      int directAttackNum;
      do {
         directAttackNum = 0;
         for ( TactVehicles::iterator i = tactVehicles.begin(); i != tactVehicles.end(); ) {
            pvehicle veh = *i;

            unitCounter++;
            displaymessage2("tact: processing operation %d", unitCounter );
            checkKeys();

            int stat = changeVehicleHeight ( veh, NULL );
            if ( stat == -1 ) { // couldn't change height due to blocked way or something similar
               veh->aiparam[ getPlayerNum() ]->setTask( AiParameter::tsk_wait );
               result.unitsWaiting++;
               i++;
            } else {

               AStar3D ast ( getMap(), veh, false, veh->getMovement() );
               ast.findAllAccessibleFields ();
               TargetVector tv;
               getAttacks ( ast, veh, tv, hemmingBonus );

               if ( tv.size() ) {
                  MoveVariant* mv = *max_element( tv.begin(), tv.end(), moveVariantComp );

                  // airplane landing constraints

                  bool directAttack = false;
                  if ( beeline ( mv->movePos.x, mv->movePos.y, mv->attackx, mv->attacky ) > maxmalq )
                     directAttack = true;

                  int freeNeighbouringFields = 0;
                  for ( int j = 0; j < sidenum; j++ ) {
                     pfield fld = getMap()->getField ( getNeighbouringFieldCoordinate ( MapCoordinate(mv->attackx, mv->attacky), j));
                     if ( fld )
                        if ( !fld->building && !fld->vehicle )
                           freeNeighbouringFields++;
                  }

                  if ( freeNeighbouringFields <= 1 )
                     directAttack = true;

                  if ( mv->enemyDamage >= 100 )
                     directAttack = true;

                  if ( directAttack ) {
                     /* to avoid recalculating the vision with every variant we are going to make it
                        easy...
                        Since all units now only attack the neighbouring fields, which is generally
                        visible, the AI does not cheat here.
                     */
                     VisibilityStates org_vision =  _vision ;
                     _vision = visible_all;

                     AiResult res = executeMoveAttack ( veh, tv );
                     i = tactVehicles.erase ( i );

                     if ( !res.unitsDestroyed )
                        veh->aiparam[ getPlayerNum() ]->setTask( AiParameter::tsk_tactics );

                     result += res;
                     directAttackNum++;

                     _vision = org_vision;

                  } else {
                     targets[mv->enemy->networkid].push_back( *mv );
                     i++;
                  }
               } else {
                  // there is nothing the unit can do in tactics mode
                  if ( veh->aiparam[ getPlayerNum() ]->getTask() != AiParameter::tsk_serviceRetreat )
                     veh->aiparam[ getPlayerNum() ]->resetTask();
                  i = tactVehicles.erase ( i );
               }
            }
         }

         // if there were direct attacks, some fields may now be blocked by units that did a directAttack,
         // so all other units plans must be recalculated

         if ( directAttackNum )
            targets.clear();

      } while ( directAttackNum );

      if ( !targets.empty() ) {
         // we are beginning with the targets that the most own units want to attack
         Targets::iterator currentTarget = targets.begin();
         for ( Targets::iterator i = targets.begin(); i != targets.end(); i++ )
            if ( i->second.size() > currentTarget->second.size())
               currentTarget = i;

         if ( currentTarget->second.size()-1 < hemmingBonus )
            hemmingBonus = currentTarget->second.size()-1;

         typedef list<MapCoordinate> AffectedFields;
         AffectedFields affectedFields;

         /* to avoid recalculating the vision with every variant we are going to make it
            easy...
            Since all units now only attack the neighbouring fields, which is generally
            visible, the AI does not cheat here.
         */
         VisibilityStates org_vision =  _vision ;
         _vision = visible_all;

         /* we don't need to discard all the calculations made above after a single attack.
            Only the attacks that are near enough to be affected by the last movement and attack
            will be ignored and recalculated in the next pass
         */
         do {   // while currentTarget != targets.end()

            pvehicle enemy = getMap()->getUnit( currentTarget->first);
            // the enemy may have been shot down by a direct attack earlier
            if ( enemy ) {
               affectedFields.push_back ( MapCoordinate(enemy->xpos, enemy->ypos) );

               MoveVariantContainer attacker = currentTarget->second;

               // use the most effective units first
               sort( attacker.begin(), attacker.end() );

               MoveVariantContainer::iterator mvci = attacker.begin();
               pvehicle positions[sidenum];
               pvehicle finalPositions[sidenum];
               for ( int i = 0; i< sidenum; i++ ) {
                  positions[i] = NULL;
                  finalPositions[i] = NULL;
               }
               float finalValue = 0;

               tactics_findBestAttackUnits ( attacker, mvci, positions, 0, finalPositions, finalValue, 0, 0, ticker );

               if ( finalValue > 0 ) {
                  for ( int i = 0; i < sidenum; i++ )
                     if ( finalPositions[i] ) {
                        int nwid = finalPositions[i]->networkid;
                        _vision = org_vision;
                        MapCoordinate3D dst = getNeighbouringFieldCoordinate( MapCoordinate3D( enemy->xpos, enemy->ypos, finalPositions[i]->height ), i);
                        dst.setnum ( dst.x, dst.y, -2 );
                        moveUnit ( finalPositions[i], dst );
                        _vision = visible_all;

                        affectedFields.push_back ( MapCoordinate(finalPositions[i]->xpos, finalPositions[i]->ypos) );
                        // the unit may have been shot down due to reaction fire

                        if ( !getMap()->getUnit ( nwid ) ) {
                           TactVehicles::iterator att = find ( tactVehicles.begin(), tactVehicles.end(), finalPositions[i] ) ;
                           tactVehicles.erase ( att );
                           finalPositions[i] = NULL;
                        }
                     }


                  int unitcount = 0;
                  for ( int i = 0; i < sidenum; i++ )
                     if ( finalPositions[i] )
                        unitcount++;

                  if ( unitcount ) {
                     int attackOrder[sidenum];
                     int finalOrder[sidenum];
                     for ( int i = 0; i< sidenum; i++ )
                        attackOrder[i] = finalOrder[i] = -1;


                     int finalDamage = -1;
                     int finalAttackNum = maxint;
                     tactics_findBestAttackOrder ( finalPositions, attackOrder, enemy, 0, enemy->damage, finalDamage, finalOrder, finalAttackNum );


                     pfield enemyField = getMap()->getField(enemy->xpos, enemy->ypos);
                     for ( int i = 0; i < finalAttackNum && enemyField->vehicle == enemy && finalAttackNum < maxint; i++ ) {
                        checkKeys();
                        if ( finalOrder[i] < 0 )
                           warning("!!!");
                        // if ( i+1 < finalAttackNum ) {
                        if ( i < finalAttackNum && finalPositions[finalOrder[i]] ) {
                           pvehicle a = finalPositions[finalOrder[i]];
                           VehicleAttack va ( mapDisplay, NULL );
                           if ( finalOrder[i] < 0 )
                              warning("!!!");
                           int iiii = (int) finalPositions[finalOrder[i]];
                           if ( iiii < 20 )
                              warning("!!!");

                           va.execute ( finalPositions[finalOrder[i]], -1, -1, 0, 0, -1 );
                           if ( va.getStatus() != 2 && strictChecks )
                              displaymessage("inconsistency #1 in AI::tactics attack", 1 );

                           VehicleTypeEfficiencyCalculator vtec (*this, finalPositions[finalOrder[i]], enemy );
                           va.execute ( NULL, enemy->xpos, enemy->ypos, 2, 0, -1 );
                           if ( va.getStatus() != 1000 && strictChecks )
                             displaymessage("inconsistency #1 in AI::tactics attack", 1 );

                           vtec.calc();

                           TactVehicles::iterator att = find ( tactVehicles.begin(), tactVehicles.end(), a ) ;
                           tactVehicles.erase ( att );
                        }
                     }
                  } // unitcount > 0
               } // else { // if finalValue > 0

            } // if enemy

            bool processable = true;
            do {
               currentTarget++;
               if ( currentTarget != targets.end() )
                  if ( hemmingBonus == currentTarget->second.size()-1 ) {
                     for ( AffectedFields::iterator i = affectedFields.begin(); i != affectedFields.end(); i++ )
                        for ( MoveVariantContainer::iterator j = currentTarget->second.begin(); j != currentTarget->second.end(); j++ ) {
                            pvehicle veh = j->attacker;

                            // here are only vehicles that attack neighbouring fields; ranged attacks are handled as "directAttacks" earlier
                            if ( beeline ( *i, veh->getPosition()) < veh->maxMovement()+20 )
                               processable = false;
                        }
                  } else
                     processable = false;
            } while ( !processable && currentTarget != targets.end() );

         } while ( currentTarget != targets.end() );

         _vision = org_vision;

      } else {
         // no attacks are possible
         tactVehicles.clear();
      }
      if ( lastTactVehiclesSize == tactVehicles.size() ) {
         // displaymessage ("AI :: tactics ; escaping infinite loop; please report this error !",1 );
         return result;
      }

   }

   displaymessage2("tactics completed ... ");

   return result;
}

void AI :: tactics_findBestAttackOrder ( pvehicle* units, int* attackOrder, pvehicle enemy, int depth, int damage, int& finalDamage, int* finalOrder, int& finalAttackNum )
{
   if ( damage > enemy->damage )
      if ( (depth < finalAttackNum && damage==finalDamage) || damage > finalDamage ) {
         finalDamage = damage;
         finalAttackNum = depth;
         for ( int i = 0; i < sidenum; i++ )
            finalOrder[i] = attackOrder[i];
      }

   if ( damage >= 100 || depth >= 6  )
      return;

   for ( int i = 0; i< sidenum; i++ ) {
       bool found = false;
       for ( int j = 0; j < depth; j++ )
          if ( attackOrder[j] == i)
              found = true;
       if ( units[i] && !found ) {
         // attackOrder[3] = 1   means that the unit on position 1 should attack as third
         attackOrder[depth] = i;

         npush ( units[i]->xpos );
         npush ( units[i]->ypos );
         MapCoordinate mc = getNeighbouringFieldCoordinate ( MapCoordinate ( enemy->xpos, enemy->ypos), i );
         units[i]->xpos = mc.x;
         units[i]->ypos = mc.y;

         tunitattacksunit battle ( units[i], enemy );
         int newdamage = battle.dv.damage;
         battle.calc();
         newdamage = battle.dv.damage - newdamage;
         npop ( units[i]->ypos );
         npop ( units[i]->xpos );

         tactics_findBestAttackOrder ( units, attackOrder, enemy, depth+1, damage+newdamage, finalDamage, finalOrder, finalAttackNum );

         attackOrder[depth] = -1;
      }
   }

}

float AI :: getAttackValue ( const tfight& battle, const pvehicle attackingUnit, const pvehicle attackedUnit, float factor )
{
   float result = (battle.dv.damage - attackedUnit->damage) * attackedUnit->aiparam[getPlayerNum()]->getValue() * factor - 1/config.aggressiveness * (battle.av.damage - attackingUnit->damage) * attackedUnit->aiparam[getPlayerNum()]->getValue() ;
   if ( battle.dv.damage >= 100 )
      result += attackedUnit->aiparam[getPlayerNum()]->getValue() * attack_unitdestroyed_bonus;
   return result;
}



class UnitAttacksUnit_FakeHemming : public tunitattacksunit {
           bool neighbours[sidenum];
           bool checkHemming ( pvehicle d_eht, int direc )
           {
              return neighbours[direc];
           };
        public:
          UnitAttacksUnit_FakeHemming ( AI* ai, pvehicle attacker, pvehicle defender, pvehicle* _neighbours ) : tunitattacksunit ( attacker , defender )
          {
             for ( int i = 0; i < sidenum; i++ ) {
                pfield fld = ai->getMap()->getField ( getNeighbouringFieldCoordinate ( attacker->getPosition(), i ));

                pvehicle v = NULL;
                if ( fld )
                   v = fld->vehicle;


                if ( v && attackpossible2n ( v, defender ) )
                   neighbours[i] = true;
                else
                   neighbours[i] = false;
             }

             for ( int i = 0; i < sidenum; i++ ) {
                if ( _neighbours[i] == attacker )
                   break;

                if ( _neighbours[i] )
                   neighbours[i] = true;
             }
             setup ( attacker, defender, true, -1 );
          };
        };



void AI :: tactics_findBestAttackUnits ( const MoveVariantContainer& mvc, MoveVariantContainer::iterator& m, pvehicle* positions, float value, pvehicle* finalPosition, float& finalValue, int unitsPositioned, int recursionDepth, int startTime )
{
   if ( m == mvc.end() || unitsPositioned >= 6 || recursionDepth >= 8 || (startTime + config.maxTactTime < ticker && !benchMark)) {
      float value = 0;
      pvehicle target = mvc.begin()->enemy;
      npush ( target->damage );
      for ( int i = 0; i < sidenum && target->damage!=100; i++ )
         if ( positions[i] ) {
            npush ( positions[i]->ypos );
            npush ( positions[i]->xpos );

            MapCoordinate mc = getNeighbouringFieldCoordinate ( target->getPosition(), i );
            positions[i]->xpos = mc.x;
            positions[i]->ypos = mc.y;

            UnitAttacksUnit_FakeHemming uau ( this, positions[i], target, positions );
            uau.calc();
            value += getAttackValue ( uau, positions[i], target );

            npop ( positions[i]->xpos );
            npop ( positions[i]->ypos );
         }
      npop ( target->damage );
      if ( value > finalValue ) {
         for ( int i = 0; i < sidenum; i++ )
            finalPosition[i] = positions[i] ;

         finalValue = value;
      }
   } else {
      for ( int i = 0; i< sidenum; i++ ) {
         if ( m->neighbouringFieldsReachable[i] && !positions[i] ) {
            positions[i] = m->attacker;
            if ( m->result > 0 )
               value += m->result;
            else
               value += 1;
            m++;
            tactics_findBestAttackUnits ( mvc, m, positions, value, finalPosition, finalValue, unitsPositioned+1, recursionDepth+1, startTime );
            m--;
            value -= m->result;
            positions[i] = NULL;
         } else {
            m++;
            tactics_findBestAttackUnits ( mvc, m, positions, value, finalPosition, finalValue, unitsPositioned, recursionDepth+1, startTime );
            m--;
         }
      }
   }
}



bool AI :: vehicleValueComp ( const pvehicle v1, const pvehicle v2 )
{
   return v1->aiparam[ v1->color/8 ]->getValue() < v2->aiparam[ v1->color/8 ]->getValue();
}

bool AI :: buildingValueComp ( const pbuilding v1, const pbuilding v2 )
{
   return v1->aiparam[ v1->color/8 ]->getValue() < v2->aiparam[ v1->color/8 ]->getValue();
}
