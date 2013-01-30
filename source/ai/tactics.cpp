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
#include "../actions/attackcommand.h"
#include "../actions/moveunitcommand.h"

const int attack_unitdestroyed_bonus = 90;


void AI :: searchTargets ( Vehicle* veh, const MapCoordinate3D& pos, TargetVector& tl, int moveDist, AStar3D& vm, int hemmingBonus )
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
   int fieldsWithChangedVisibility = evaluateviewcalculation ( getMap(), veh->getPosition(), veh->view, 0xff );


   if ( AttackCommand::avail ( veh )) {
      AttackCommand va ( veh );
      va.searchTargets();
      for ( AttackCommand::FieldList::const_iterator i = va.getAttackableUnits().begin(); i != va.getAttackableUnits().end(); ++i ) {
         MapCoordinate targ = i->first;
         const AttackWeap* aw = &(i->second);

         int bestweap = -1;
         int targdamage = -1;
         int weapstrength = -1;
         for ( int w = 0; w < aw->count; w++ ) {
            tunitattacksunit uau ( veh, getMap()->getField (targ)->vehicle, 1, aw->num[w] );
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

         tunitattacksunit uau ( veh, getMap()->getField(targ)->vehicle, 1, bestweap );
         mv->orgDamage = uau.av.damage;
         mv->damageAfterMove = uau.av.damage;
         mv->enemyOrgDamage = uau.dv.damage;
         uau.calc();


         mv->damageAfterAttack = uau.av.damage;
         mv->enemyDamage = uau.dv.damage;
         mv->enemy = getMap()->getField(targ)->vehicle;
         mv->movePos = pos;
         mv->attackx = targ.x;
         mv->attacky = targ.y;
         mv->weapNum = bestweap;
         mv->moveDist = moveDist;
         mv->attacker = veh;

         mv->positionThreat = getFieldThreat ( pos.x, pos.y ).threat[veh->getValueType()];


         for ( int nf = 0; nf < sidenum; nf++ ) {
            MapCoordinate mc = getNeighbouringFieldCoordinate ( MapCoordinate ( mv->attackx, mv->attacky), nf );
            MapField* fld = getMap()->getField(mc);
            if ( fld && !veh->typ->wait)
               mv->neighbouringFieldsReachable[nf] = (vm.visited.find( MapCoordinate3D(mc.x, mc.y, pos.getBitmappedHeight()) ) || ( veh->xpos == mc.x && veh->ypos == mc.y )) && !fld->building && (!fld->vehicle || fld->unitHere(veh));
            else
               mv->neighbouringFieldsReachable[nf] = false;

         }

         int attackerDirection = getdirection ( targ.x, targ.y, pos.x, pos.y );
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
            MapField* fld = getMap()->getField(mc);
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
      evaluateviewcalculation ( getMap(), veh->getPosition(), veh->view, 0xff );
}

bool AI::MoveVariant::operator> ( const AI::MoveVariant& mv2 ) const
{
   return (     result > mv2.result
            || (result == mv2.result && positionThreat < mv2.positionThreat  )
            || (result == mv2.result && positionThreat == mv2.positionThreat && moveDist < mv2.moveDist) );  // mv1.moveDist < mv2.moveDist
}


bool AI::MoveVariant::operator< ( const AI::MoveVariant& mv2 ) const
{
   return (     result < mv2.result
            || (result == mv2.result && positionThreat > mv2.positionThreat )
            || (result == mv2.result && positionThreat == mv2.positionThreat && moveDist > mv2.moveDist) );  // mv1.moveDist < mv2.moveDist
}

/*
bool operator> ( const AI::MoveVariant& mv1, const AI::MoveVariant& mv2 ) 
{
   return (     mv1.result > mv2.result
            || (mv1.result == mv2.result && mv1.positionThreat < mv2.positionThreat  )
            || (mv1.result == mv2.result && mv1.positionThreat == mv2.positionThreat && mv1.moveDist < mv2.moveDist) );  // mv1.moveDist < mv2.moveDist
}

bool operator< ( const AI::MoveVariant& mv1, const AI::MoveVariant& mv2 )
{
   return (     mv1.result < mv2.result
            || (mv1.result == mv2.result && mv1.positionThreat > mv2.positionThreat )
            || (mv1.result == mv2.result && mv1.positionThreat == mv2.positionThreat && mv1.moveDist > mv2.moveDist) );  // mv1.moveDist < mv2.moveDist
}

*/
bool AI::moveVariantComp ( const AI::MoveVariant* mv1, const AI::MoveVariant* mv2 )
{
   return *mv1 < *mv2;
   // return ( mv1->result < mv2->result || (mv1->result == mv2->result && mv1->moveDist > mv2->moveDist ));
}

void AI::getAttacks ( AStar3D& vm, Vehicle* veh, TargetVector& tv, int hemmingBonus, bool justOne, bool executeService )
{

   //! first check

   int x1 = veh->xpos;
   int y1 = veh->ypos;
   int x2 = veh->xpos;
   int y2 = veh->ypos;

   for ( AStar3D::VisitedContainer::iterator ff = vm.visited.begin(); ff != vm.visited.end(); ++ff ) {
      AStar3D::Node n = *ff;
      x1 = min ( x1, n.h.x );
      y1 = min ( y1, n.h.y );
      x2 = max ( x2, n.h.x );
      y2 = max ( y2, n.h.y );
   }

   int maxrange = 0;
   for ( int i = 0; i < veh->typ->weapons.count; ++i )
      maxrange = max ( maxrange, veh->typ->weapons.weapon[i].maxdistance );

   maxrange += 30;

   x1 = max ( 0, x1 - maxrange/10 );
   y1 = max ( 0, y1 - maxrange*2/10 );
   x2 = min ( getMap()->xsize, x2 + maxrange/10 );
   y2 = min ( getMap()->ysize, y2 + maxrange*2/10 );

   int enemycount = 0;
   for ( int y = y1; y <= y2 && !enemycount; ++y)
      for ( int x = x1; x <= x2; ++x) {
         MapField* fld = getMap()->getField(x,y);
         if ( fld && fld->vehicle)
            if ( getPlayer(fld->vehicle->getOwner()).diplomacy.isHostile( getPlayerNum() )  )
               enemycount++;
      }

   if( !enemycount )
      return;



   int orgxpos = veh->xpos ;
   int orgypos = veh->ypos ;

   if ( getMap()->getField ( veh->xpos, veh->ypos )->unitHere ( veh ) )  // unit not inside a building or transport
      searchTargets ( veh, veh->getPosition3D(), tv, 0, vm, hemmingBonus );

   if ( tv.size() && justOne )
      return;

   // Now we cycle through all fields that are reachable...
   if ( !veh->typ->wait ) {

      RefuelConstraint* apl = NULL;
      if ( RefuelConstraint::necessary ( veh, *this ))
        apl = new RefuelConstraint( *this, veh );

      int fuelLacking = 0;
      for ( AStar3D::VisitedContainer::iterator ff = vm.visited.begin(); ff != vm.visited.end(); ++ff ) {
         AStar3D::Node node = *ff;
         if ( !node.hasAttacked ) {
            MapField* fld = getMap()->getField (node.h);
            if ( !fld->vehicle && !fld->building ) {
                if ( !apl || apl->returnFromPositionPossible ( node.h )) {
                   searchTargets ( veh, node.h, tv, beeline ( node.h.x, node.h.y, orgxpos, orgypos ), vm, hemmingBonus );
                   if ( tv.size() && justOne )
                      return;
                } else
                   fuelLacking++;
             }
         }
      }

      if ( apl ) {
         delete apl;
         apl = NULL;
      }

      if ( !tv.size() && fuelLacking && executeService)
         issueRefuelOrder( veh, true );
   }
}

AI::AiResult AI::executeMoveAttack ( Vehicle* veh, TargetVector& tv )
{
   int unitNetworkID = veh->networkid;
   AiResult result;

   MoveVariant* mv = *max_element( tv.begin(), tv.end(), moveVariantComp );

   if ( mv->movePos != veh->getPosition3D() ) {
      VisibilityStates org_vision =  getVision() ;
      setVision(visible_now);
      
      auto_ptr<MoveUnitCommand> muc ( new MoveUnitCommand( veh ));
      muc->setFlags ( MoveUnitCommand::NoInterrupt );
      muc->setDestination( mv->movePos );
      ActionResult res = muc->execute( getContext () );
      if ( res.successful() )
         muc.release();
      else {
         displaymessage ( "AI :: executeMoveAttack \n error in movement with unit %d", 1, veh->networkid );
      }

      result.unitsMoved ++;
      setVision(org_vision);
   }

   // the unit may have been shot down due to reactionfire during movement
   if ( !getMap()->getUnit(unitNetworkID)) {
      result.unitsDestroyed++;
      return result;
   }

   if ( veh->attacked )
      return result;

   auto_ptr<AttackCommand> va (new AttackCommand( veh ));
   ActionResult res = va->searchTargets();
   if ( !res.successful() )
      displaymessage ( "AI :: executeMoveAttack \n error in attack step 2 with unit %d", 1, veh->networkid );

   VehicleTypeEfficiencyCalculator vtec( *this, veh, mv->enemy );

   va->setTarget( MapCoordinate( mv->attackx, mv->attacky), mv->weapNum );
   res = va->execute( getContext() );
   if ( !res.successful() )
      displaymessage ( "AI :: executeMoveAttack \n error in attack step 3 with unit %d", 1, veh->networkid );
   else
      va.release();

   vtec.calc();

   result.unitsMoved ++;

   // the unit may have been shot in the attack
   if ( !getMap()->getUnit(unitNetworkID)) {
      result.unitsDestroyed++;
      return result;
   }


   if ( MoveUnitCommand::avail( veh ))
      result += moveToSavePlace ( veh );

   return result;
}

bool AI::targetsNear( Vehicle* veh )
{
   AStar3D ast ( getMap(), veh, false, veh->getMovement() );
   ast.findAllAccessibleFields ();
   TargetVector tv;
   getAttacks ( ast, veh, tv, 0, true, false );
   if ( tv.size() )
      return true;
   else
      return false;
}


int AI::getDirForBestTacticsMove ( const Vehicle* veh, TargetVector& tv )
{
   if ( tv.size() <= 0 )
      return -1;

   MoveVariant* mv = *max_element( tv.begin(), tv.end(), moveVariantComp );
   return getdirection ( veh->xpos, veh->ypos, mv->movePos.x, mv->movePos.y );
}

MapCoordinate AI::getDestination ( Vehicle* veh )
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


AI::AiResult AI::moveToSavePlace ( Vehicle* veh, int preferredHeight )
{
   int unitNetworkID = veh->networkid;

   AiResult result;

   auto_ptr<MoveUnitCommand> muc( new MoveUnitCommand( veh ));

   ActionResult res = muc->searchFields( preferredHeight );
   if ( !res.successful() )
      return result;
   
   const set<MapCoordinate3D>& fields = muc->getReachableFields();
            
   if ( fields.empty() )
      return result;


   int xtogo = veh->xpos;
   int ytogo = veh->ypos;
   int threat = maxint;
   int dist = maxint;

   if ( getMap()->getField ( veh->xpos, veh->ypos)->unitHere ( veh ) ) {  // vehicle not in building / transport
      threat = int( getFieldThreat ( veh->xpos, veh->ypos).threat[ veh->aiparam[ getPlayerNum()]->valueType] * 1.5 + 1);
       // multiplying with 1.5 to make this field a bit unattractive, to allow other units (for example buggies) to attack from this field to, since it is probably quite a good position (else it would not have been chosen)
   }

   for ( set<MapCoordinate3D>::const_iterator i = fields.begin(); i != fields.end(); ++i ) {
      MapField* fld = getMap()->getField( *i );
      if ( !fld->vehicle && !fld->building ) {
         AiThreat& ait = getFieldThreat ( i->x, i->y );
         int _dist = beeline ( i->x, i->y, veh->xpos, veh->ypos);

            // make fields far away a bit unattractive; we don't want to move the whole distance back again next turn
         int t = int( ait.threat[ veh->aiparam[ getPlayerNum()]->valueType ] * log ( double(_dist) )/log(double(10)) );

         if ( t < threat || ( t == threat && (_dist < dist ))) {
            threat = t;
            xtogo = i->x;
            ytogo = i->y;
            dist = _dist;
         }
      }
   }

   if ( veh->xpos != xtogo || veh->ypos != ytogo ) {
      
      muc->setDestination( MapCoordinate(xtogo, ytogo ));
      
      res = muc->execute( getContext() );
      if ( res.successful() )
         muc.release();
      else
         displaymessage ( "AI :: moveToSavePlace \n error in movement3 step 3 with unit %d", 1, veh->networkid );

      result.unitsMoved++;
   }

   if ( !getMap()->getUnit( unitNetworkID ))
      result.unitsDestroyed++;

   return result;
}


int AI::changeVehicleHeight ( Vehicle* veh, int preferredDirection )
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

   typedef list<int> TactVehicles;
   TactVehicles tactVehicles;

   for ( Player::VehicleList::iterator vi = getPlayer().vehicleList.begin(); vi != getPlayer().vehicleList.end(); vi++ ) {
      Vehicle* veh = *vi;

      if ( !veh->aiparam[ getPlayerNum() ] )
          calculateThreat( veh );
      
      bool unitUsable = false;
      if ( veh->aiparam[ getPlayerNum() ]->getJob() == AiParameter::job_fight
        || veh->aiparam[ getPlayerNum() ]->getJob() == AiParameter::job_undefined
        || (veh->aiparam[ getPlayerNum() ]->getJob() == AiParameter::job_guard && veh->aiparam[ getPlayerNum() ]->dest_nwid == -1 && veh->aiparam[ getPlayerNum() ]->dest.x == -1 ))
         for ( int j = 0; j < tsk_num; j++ )
            if ( veh->aiparam[ getPlayerNum() ]->getTask() == tasks[j] )
               unitUsable = true;

      if ( getMap()->getField(veh->getPosition())->vehicle != veh ) {
         Vehicle* transport = getMap()->getField(veh->getPosition())->vehicle;
         if ( transport ) {
            const ContainerBaseType::TransportationIO* unloadSystem = transport->vehicleUnloadSystem( veh->typ, -1 );
            if ( unloadSystem && unloadSystem->disableAttack )
               continue;
         }
      }


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
               MapField* fld = getMap()->getField(x,y );
               if ( fld ) {
                  if ( fld->vehicle && getPlayer(veh->getOwner()).diplomacy.isHostile( fld->vehicle->getOwner() )  )
                     enemiesNear = true;
                  if ( fld->building && getPlayer(veh->getOwner()).diplomacy.isHostile( fld->building->getOwner() )  )
                     enemiesNear = true;
               }
            }

         if ( unitUsable && enemiesNear)
            tactVehicles.push_back ( veh->networkid );
      }
   }

   int hemmingBonus = 5;

   size_t lastTactVehiclesSize; // = tactVehicles.size();

   while ( !tactVehicles.empty() ) {
      lastTactVehiclesSize = tactVehicles.size();

      typedef map<int, MoveVariantContainer> Targets;
      Targets targets;

      int directAttackNum;
      do {
         directAttackNum = 0;
         for ( TactVehicles::iterator i = tactVehicles.begin(); i != tactVehicles.end(); ) {
            Vehicle* veh = getMap()->getUnit( *i );
            if ( veh ) {

               unitCounter++;
               displaymessage2("tact: processing operation %d", unitCounter );
               checkKeys();
   
               int stat = changeVehicleHeight ( veh, -1 );
               
               if ( !getMap()->getUnit(*i) )
                  continue;
               
               if ( stat == -1 ) { // couldn't change height due to blocked way or something similar
                  veh->aiparam[ getPlayerNum() ]->setTask( AiParameter::tsk_wait );
                  result.unitsWaiting++;
                  i++;
               } else {
   
                  AStar3D ast ( getMap(), veh, false, veh->getMovement() );
                  ast.findAllAccessibleFields ();
                  TargetVector tv;
                  getAttacks ( ast, veh, tv, hemmingBonus );
   
                  if ( !getMap()->getUnit(*i) )
                     continue;
                  
                  if ( tv.size() ) {
                     MoveVariant* mv = *max_element( tv.begin(), tv.end(), moveVariantComp );
   
                     // airplane landing constraints
   
                     bool directAttack = false;
                     if ( beeline ( mv->movePos.x, mv->movePos.y, mv->attackx, mv->attacky ) > maxmalq || veh->height >= chtieffliegend || (mv->enemy && mv->enemy->height >= chtieffliegend) )
                        directAttack = true;
   
                     int freeNeighbouringFields = 0;
                     for ( int j = 0; j < sidenum; j++ ) {
                        MapField* fld = getMap()->getField ( getNeighbouringFieldCoordinate ( MapCoordinate(mv->attackx, mv->attacky), j));
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
                        VisibilityStates org_vision = getVision();
                        setVision(visible_all);
   
                        AiResult res = executeMoveAttack ( veh, tv );
                        i = tactVehicles.erase ( i );
   
                        if ( res.unitsMoved )
                           unitsWorkedInTactics.insert(veh);
                        
                        if ( !res.unitsDestroyed )
                           veh->aiparam[ getPlayerNum() ]->setTask( AiParameter::tsk_tactics );
   
                        result += res;
                        
                        directAttackNum++;
   
                        setVision(org_vision);
   
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
            } else {
               // unit not available any more, get next one
               i = tactVehicles.erase ( i );

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
         VisibilityStates org_vision =  getVision();
         setVision(visible_all);

         /* we don't need to discard all the calculations made above after a single attack.
            Only the attacks that are near enough to be affected by the last movement and attack
            will be ignored and recalculated in the next pass
         */
         do {   // while currentTarget != targets.end()

            Vehicle* enemy = getMap()->getUnit( currentTarget->first);
            // the enemy may have been shot down by a direct attack earlier
            if ( enemy ) {
               affectedFields.push_back ( MapCoordinate(enemy->xpos, enemy->ypos) );

               MoveVariantContainer attacker = currentTarget->second;

               // use the most effective units first
               sort( attacker.begin(), attacker.end() );

               MoveVariantContainer::iterator mvci = attacker.begin();
               Vehicle* positions[sidenum];
               Vehicle* finalPositions[sidenum];
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
                        setVision(org_vision);
                        MapCoordinate affected =  MapCoordinate(finalPositions[i]->xpos, finalPositions[i]->ypos);
                        MapCoordinate3D dst = getNeighbouringFieldCoordinate( MapCoordinate3D( enemy->xpos, enemy->ypos, finalPositions[i]->height ), i);
                        dst.setnum ( dst.x, dst.y, -2 );
                        
                        moveUnit ( finalPositions[i], dst );
                        setVision(visible_all);

                        affectedFields.push_back ( affected );
                        // the unit may have been shot down due to reaction fire

                        if ( !getMap()->getUnit ( nwid ) ) {
                           TactVehicles::iterator att = find ( tactVehicles.begin(), tactVehicles.end(), nwid ) ;
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

                     vector<Vehicle*> unitsToMoveAwayAfterAttacking;

                     MapField* enemyField = getMap()->getField(enemy->xpos, enemy->ypos);
                     for ( int i = 0; i < finalAttackNum && enemyField->vehicle == enemy && finalAttackNum < maxint; i++ ) {
                        checkKeys();
                        if ( finalOrder[i] < 0 )
                           warningMessage("!!!");
                        // if ( i+1 < finalAttackNum ) {
                        if ( i < finalAttackNum && finalPositions[finalOrder[i]] ) {
                           Vehicle* a = finalPositions[finalOrder[i]];
                           if ( finalOrder[i] < 0 )
                              warningMessage("!!!");

                           unitsWorkedInTactics.insert(a);
                           
                           
                           int nwid = a->networkid;
                           auto_ptr<AttackCommand> va ( new AttackCommand ( a ));
                           va->searchTargets();
                           
                           VehicleTypeEfficiencyCalculator vtec (*this, finalPositions[finalOrder[i]], enemy );
                           va->setTarget( enemy->getPosition() );
                           ActionResult res = va->execute( getContext() );                           
                           if ( !res.successful() && strictChecks )
                             displaymessage("inconsistency #1 in AI::tactics attack", 1 );
                           
                           if ( res.successful() )
                              va.release();
                             

                           vtec.calc();

                           TactVehicles::iterator att = find ( tactVehicles.begin(), tactVehicles.end(), nwid ) ;
                           tactVehicles.erase ( att );
                           
                           if ( getMap()->getUnit( nwid ) && a->typ->hasFunction( ContainerBaseType::MoveAfterAttack ))
                              unitsToMoveAwayAfterAttacking.push_back( a );
                        }
                     }
                     
                     for ( vector<Vehicle*>::iterator i = unitsToMoveAwayAfterAttacking.begin(); i != unitsToMoveAwayAfterAttacking.end(); ++i )
                        moveToSavePlace( *i );
                     
                  } // unitcount > 0
                  
                  
               } // else { // if finalValue > 0

            } // if enemy

            bool processable = true;
            do {
               currentTarget++;
               if ( currentTarget != targets.end() ) {
                  if ( hemmingBonus == currentTarget->second.size()-1 ) {
                     for ( AffectedFields::iterator i = affectedFields.begin(); i != affectedFields.end(); i++ )
                        for ( MoveVariantContainer::iterator j = currentTarget->second.begin(); j != currentTarget->second.end(); j++ ) {
                            Vehicle* veh = j->attacker;

                            // here are only vehicles that attack neighbouring fields; ranged attacks are handled as "directAttacks" earlier
                            if ( beeline ( *i, veh->getPosition()) < veh->maxMovement()+20 )
                               processable = false;
                        }
                  } else
                     processable = false;
               }
            } while ( !processable && currentTarget != targets.end() );

         } while ( currentTarget != targets.end() );

         setVision(org_vision);

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

void AI :: tactics_findBestAttackOrder ( Vehicle** units, int* attackOrder, Vehicle* enemy, int depth, int damage, int& finalDamage, int* finalOrder, int& finalAttackNum )
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


int AI :: getValue( Vehicle* v ) 
{
   if ( v->aiparam[getPlayerNum()] == NULL ) {
      calculateThreat(v);  
   }
   return v->aiparam[getPlayerNum()]->getValue();
}

float AI :: getAttackValue ( const tfight& battle, Vehicle* attackingUnit, Vehicle* attackedUnit, float factor )
{
   calculateThreat( attackedUnit );
   float result =                     (battle.dv.damage - attackedUnit->damage) * getValue(attackedUnit) * factor 
          - 1/config.aggressiveness * (battle.av.damage - attackingUnit->damage) * getValue(attackingUnit) ;
   if ( battle.dv.damage >= 100 )
      result += attackedUnit->aiparam[getPlayerNum()]->getValue() * attack_unitdestroyed_bonus;
   return result;
}



class UnitAttacksUnit_FakeHemming : public tunitattacksunit {
           bool neighbours[sidenum];
           bool checkHemming ( Vehicle* d_eht, int direc )
           {
              return neighbours[direc];
           };
        public:
          UnitAttacksUnit_FakeHemming ( AI* ai, Vehicle* attacker, Vehicle* defender, Vehicle** _neighbours ) : tunitattacksunit ( attacker , defender )
          {
             for ( int i = 0; i < sidenum; i++ ) {
                MapField* fld = ai->getMap()->getField ( getNeighbouringFieldCoordinate ( attacker->getPosition(), i ));

                Vehicle* v = NULL;
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



void AI :: tactics_findBestAttackUnits ( const MoveVariantContainer& mvc, MoveVariantContainer::iterator& m, Vehicle** positions, float value, Vehicle** finalPosition, float& finalValue, int unitsPositioned, int recursionDepth, int startTime )
{
   if ( m == mvc.end() || unitsPositioned >= 6 || recursionDepth >= 8 || (startTime + config.maxTactTime < ticker && !benchMark)) {
      float value = 0;
      Vehicle* target = mvc.begin()->enemy;
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



bool AI :: vehicleValueComp ( const Vehicle* v1, const Vehicle* v2 )
{
   return v1->aiparam[ v1->color/8 ]->getValue() < v2->aiparam[ v1->color/8 ]->getValue();
}

bool AI :: buildingValueComp ( const Building* v1, const Building* v2 )
{
   return v1->aiparam[ v1->color/8 ]->getValue() < v2->aiparam[ v1->color/8 ]->getValue();
}
