/***************************************************************************
                          buildingcapture.cpp  -  description
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






float AI :: getCaptureValue ( const pbuilding bld, const pvehicle veh  )
{
   HiddenAStar ast ( this, veh );
   HiddenAStar::Path path;
   ast.findPath ( path, bld->getEntry().x, bld->getEntry().y );
   if ( ast.getTravelTime() >= 0 )
      return getCaptureValue ( bld, ast.getTravelTime() );
   else
      return minfloat;
}


void AI :: BuildingCapture :: write ( tnstream& stream ) const
{
  stream.writeInt( 20 );
  stream.writeInt ( state );
  stream.writeInt ( unit );
  for ( vector<int>::const_iterator i = guards.begin(); i  != guards.end(); i++ ) {
     stream.writeInt ( 1 );
     stream.writeInt ( *i );
  }
  stream.writeInt ( 0 );
  stream.writeFloat ( captureValue );
  stream.writeInt ( nearestUnit );
}

void AI :: BuildingCapture :: read ( tnstream& stream )
{
  stream.readInt(); // version
  state = BuildingCaptureState( stream.readInt ( ));
  unit = stream.readInt ( );
  int i = stream.readInt ();
  while ( i ) {
     guards.push_back ( stream.readInt() );
     i = stream.readInt();
  }
  captureValue = stream.readFloat ();
  nearestUnit = stream.readInt ();
}



class  SearchReconquerBuilding : public SearchFields {
                                protected:
                                   AI& ai;
                                   pbuilding buildingToCapture;
                                   int mode;        // (1): nur fusstruppen; (2): 1 und transporter; (3): 2 und geb„ude
                                   vector<pvehicle> enemyUnits; // that can conquer the building
                                   float getThreatValueOfUnit ( pvehicle veh );
                                public:
                                   void testfield ( const MapCoordinate& mc );
                                   bool returnresult ( );
                                   void unitfound ( pvehicle eht );
                                   bool canUnitCapture ( pvehicle veh );
                                   SearchReconquerBuilding ( AI& _ai, pbuilding bld ) : SearchFields ( _ai.getMap() ), ai ( _ai ), buildingToCapture ( bld ), mode(3) {};
                                };

bool SearchReconquerBuilding :: returnresult( )
{
  return !enemyUnits.empty();
}


void         SearchReconquerBuilding :: unitfound(pvehicle     eht)
{
  enemyUnits.push_back ( eht );
  buildingToCapture->aiparam[ai.getPlayerNum()]->setAdditionalValue ( buildingToCapture->aiparam[ai.getPlayerNum()]->getValue() );
}

bool SearchReconquerBuilding :: canUnitCapture( pvehicle eht )
{
   return ((eht->functions & cf_conquer ))
           && fieldaccessible ( buildingToCapture->getEntryField(), eht) == 2 ;

}

void         SearchReconquerBuilding :: testfield(const MapCoordinate& mc)
{
      pvehicle eht = gamemap->getField(mc)->vehicle;
      // pbuilding bld = getfield(xp,yp)->building;
      if ( eht )
         if (getdiplomaticstatus(eht->color) != capeace)
            if ( canUnitCapture ( eht )) {
               VehicleMovement vm ( NULL );
               if ( vm.available ( eht )) {
                  vm.execute ( eht, -1, -1, 0 , -1, -1 );
                  if ( vm.reachableFields.isMember ( startPos ))
                     unitfound(eht);
               }
            }
            else
               if (mode >= 2)
                  if (eht->typ->maxLoadableUnits > 0)
                     for ( int w = 0; w <= 31; w++)
                        if ( eht->loading[w] )
                           if ( canUnitCapture ( eht->loading[w] ))
                              if (eht->maxMovement() + eht->loading[w]->maxMovement() >= beeline(mc, startPos))
                                 unitfound(eht);


/*
      if ( bld )
         if ( mode >= 3 )
            if (getdiplomaticstatus(bld->color) != capeace)
               for ( int w = 0; w <= 31; w++)
                  if ( bld->loading[w] )
                     if ( canUnitCapture ( bld->loading[w] ))
                        if ( bld->loading[w]->typ->movement[log2(bld->loading[w]->height)] <= beeline(xp,yp,startx,starty))
                           unitfound ( bld->loading[w] );

*/
}


float AI :: getCaptureValue ( const pbuilding bld, int traveltime  )
{
   if ( traveltime < 0 )
      traveltime = 0;
   return float(bld->aiparam[getPlayerNum()]->getValue()) / float(traveltime+1);
}


bool AI :: checkReConquer ( pbuilding bld, pvehicle veh )
{
   SearchReconquerBuilding srb ( *this, bld );
   srb.initsearch ( bld->getEntry(), (maxTransportMove + maxUnitMove/2) / maxmalq + 1, 1 );
   srb.startsearch();
   bool enemyNear = srb.returnresult();

   if ( enemyNear ) {
      float f = 0;
      for ( int i = 0; i < 32; i++ )
         if ( bld->loading[i] )
            if ( bld->loading[i]->getMovement() )
               f += bld->loading[i]->aiparam[ getPlayerNum()]->getValue();

      //! if the units inside the building are more worth than the own unit, capture the building regardless of whether it can be recaptured
      if ( f > veh->aiparam[getPlayerNum()]->getValue())
         return false;
      else
         return true;
   } else
      return false;

}

void AI :: checkConquer( )
{
   // remove all capture orders for buildings which are no longer controlled by the enemy

   for ( BuildingCaptureContainer::iterator bi = buildingCapture.begin(); bi != buildingCapture.end(); ) {
      BuildingCaptureContainer::iterator nxt = bi;
      ++nxt;
      pvehicle veh= getMap()->getUnit ( bi->second.unit );
      pbuilding bld = getMap()->getField( bi->first )->building;
      if ( getdiplomaticstatus2( bld->color, getPlayerNum()*8 ) != cawar
           || !( veh && fieldaccessible ( getMap()->getField( bi->first ), veh ) == 2 )) {

         if ( veh ) {
            veh->aiparam[getPlayerNum()]->resetTask ();
            veh->aiparam[getPlayerNum()]->setNextJob();
         }
         buildingCapture.erase ( bi );
      } else
         if ( veh && veh->color != getPlayerNum()*8 )
            buildingCapture.erase ( bi );

      bi = nxt;
   }

   displaymessage2("check for capturing enemy towns ... ");

   for ( Player::VehicleList::iterator vi = getPlayer().vehicleList.begin(); vi != getPlayer().vehicleList.end(); vi++ ) {
      pvehicle veh = *vi;
      if ( veh->aiparam[getPlayerNum()]->getJob() == AiParameter::job_conquer && veh->aiparam[getPlayerNum()]->getTask() == AiParameter::tsk_nothing && veh->canMove() ) {

         HiddenAStar ast ( this, veh );
         ast.findAllAccessibleFields( veh->maxMovement() * config.maxCaptureTime );

         pbuilding bestBuilding = NULL;
         float bestBuildingCaptureValue = minfloat;

         for ( int c = 0; c <= 8; c++ )
            if ( getPlayer(c).exist() && getdiplomaticstatus2 ( getPlayerNum()*8, c*8 ) == cawar ) {
               for ( Player::BuildingList::iterator bi = getPlayer(c).buildingList.begin(); bi != getPlayer(c).buildingList.end(); bi++ ) {
                  pbuilding bld = *bi;
                  if ( bld->getEntryField()->a.temp  && buildingCapture[bld->getEntry()].state == BuildingCapture::conq_noUnit ) {
                     float v = getCaptureValue ( bld, veh );
                     if ( v > bestBuildingCaptureValue ) {
                        bestBuildingCaptureValue = v;
                        bestBuilding = bld;
                     }
                  }
               }
            }

         if ( bestBuilding ) {
            BuildingCapture& bc = buildingCapture[ bestBuilding->getEntry() ];

            bc.captureValue = bestBuildingCaptureValue;
            bc.state = BuildingCapture::conq_conqUnit;
            bc.unit = veh->networkid;

            veh->aiparam[getPlayerNum()]->addJob( AiParameter::job_conquer, true );
            veh->aiparam[getPlayerNum()]->setTask( AiParameter::tsk_move );
            veh->aiparam[getPlayerNum()]->dest = bestBuilding->getEntry();
         } else
            veh->aiparam[getPlayerNum()]->setNextJob();
      }
   }


   // cycle through all neutral buildings which can be conquered by any unit
   displaymessage2("check for capturing neutral towns ... ");

   typedef vector<pbuilding> BuildingList;
   BuildingList buildingList;

   for ( Player::BuildingList::iterator bi = getPlayer(8).buildingList.begin(); bi != getPlayer(8).buildingList.end(); bi++ ) {
      pbuilding bld = *bi;
      if ( buildingCapture[ bld->getEntry() ].state == BuildingCapture::conq_noUnit ) {
         buildingList.push_back ( bld );

         int travelTime = maxint;
         pvehicle conquerer = NULL;

         for ( Player::VehicleList::iterator vi = getPlayer().vehicleList.begin(); vi != getPlayer().vehicleList.end(); vi++ ) {
            pvehicle veh = *vi;
            if ( veh->aiparam[getPlayerNum()]->getJob() != AiParameter::job_conquer || veh->aiparam[getPlayerNum()]->getTask() == AiParameter::tsk_nothing)
               if ( fieldaccessible ( bld->getEntryField(), veh ) == 2 ) {
                  HiddenAStar ast ( this, veh );
                  vector<MapCoordinate> path;
                  ast.findPath ( path, bld->getEntry().x, bld->getEntry().y );
                  int time = ast.getTravelTime();
                  if ( time >= 0 && time < travelTime ) {
                     conquerer = veh;
                     travelTime = time;
                  }
               }
         }
         if ( conquerer ) {
            BuildingCapture& bc = buildingCapture[ bld->getEntry() ];

            bc.nearestUnit  = conquerer->networkid;
            if ( travelTime >= 0 )
               bc.captureValue = getCaptureValue ( bld, travelTime );
            else
               bc.captureValue = 0;
         }

      }
   }

   sort ( buildingList.begin(), buildingList.end(), BuildingValueComp ( this ));

   for ( BuildingList::iterator i = buildingList.begin(); i != buildingList.end(); i++ ) {

      int travelTime = maxint;
      pvehicle conquerer = NULL;

      for ( Player::VehicleList::iterator vi = getPlayer().vehicleList.begin(); vi != getPlayer().vehicleList.end(); vi++ ) {
         pvehicle veh = *vi;
         if ( veh->aiparam[getPlayerNum()]->getJob() != AiParameter::job_conquer || veh->aiparam[getPlayerNum()]->getTask() == AiParameter::tsk_nothing )
            if ( fieldaccessible ( (*i)->getEntryField(), veh ) == 2 ) {
               HiddenAStar ast ( this, veh ) ;
               vector<MapCoordinate> path;
               ast.findPath ( path, (*i)->getEntry().x, (*i)->getEntry().y );
               int time = ast.getTravelTime();
               if ( time >= 0 && time < travelTime ) {
                  conquerer = veh;
                  travelTime = time;
               }
            }
      }

      if ( conquerer ) {
        BuildingCapture& bc = buildingCapture[ (*i)->getEntry() ];
        if ( conquerer->functions & cf_conquer )
           bc.state = BuildingCapture::conq_conqUnit;
        else
           bc.state = BuildingCapture::conq_unitNotConq;
        bc.unit = conquerer->networkid;
        conquerer->aiparam[getPlayerNum()]->addJob ( AiParameter::job_conquer, true );
        conquerer->aiparam[getPlayerNum()]->setTask ( AiParameter::tsk_move );
        conquerer->aiparam[getPlayerNum()]->dest = (*i)->getEntry();
      } else
        buildingCapture[ (*i)->getEntry() ].state = BuildingCapture::conq_unreachable;
   }

   for ( BuildingCaptureContainer::iterator bi = buildingCapture.begin(); bi != buildingCapture.end(); ) {
      BuildingCaptureContainer::iterator nxt = bi;
      ++nxt;
      pvehicle veh = getMap()->getUnit ( bi->second.unit );
      if ( veh ) {
         MapCoordinate dest = veh->aiparam[getPlayerNum()]->dest;
         int nwid = veh->networkid;
         moveUnit ( veh, dest, true );
         if ( getMap()->getUnit ( nwid ) && veh->getPosition() == dest ) {
            veh->aiparam[getPlayerNum()]->resetTask ();
            buildingCapture.erase ( bi );
         }
      } else
         buildingCapture.erase ( bi );
      checkKeys();
      bi = nxt;
   }

}


