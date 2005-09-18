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


float AI :: getCaptureValue ( const Building* bld, Vehicle* veh  )
{
   HiddenAStar ast ( this, veh );
   HiddenAStar::Path path;
   ast.findPath ( path, bld->getEntry().x, bld->getEntry().y );
   if ( ast.getTravelTime() >= 0 )
      // everything else being equal, prefer cheapest unit
      // TODO: factor 0.0001 should be made configurable
      return getCaptureValue ( bld, ast.getTravelTime() )-0.0001*veh->aiparam[getPlayerNum()]->getValue() ;
   else
      return -1;
      // return minfloat; // this makes no sense as minfloat>0!
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
                                   Building* buildingToCapture;
                                   int mode;        // (1): nur fusstruppen; (2): 1 und transporter; (3): 2 und geb„ude
                                   vector<Vehicle*> enemyUnits; // that can conquer the building
                                   float getThreatValueOfUnit ( Vehicle* veh );
                                public:
                                   void testfield ( const MapCoordinate& mc );
                                   bool returnresult ( );
                                   void unitfound ( Vehicle* eht );
                                   bool canUnitCapture ( Vehicle* veh );
                                   SearchReconquerBuilding ( AI& _ai, Building* bld ) : SearchFields ( _ai.getMap() ), ai ( _ai ), buildingToCapture ( bld ), mode(3) {};
                                };

bool SearchReconquerBuilding :: returnresult( )
{
  return !enemyUnits.empty();
}


void         SearchReconquerBuilding :: unitfound(Vehicle*     eht)
{
  enemyUnits.push_back ( eht );
  buildingToCapture->aiparam[ai.getPlayerNum()]->setAdditionalValue ( buildingToCapture->aiparam[ai.getPlayerNum()]->getValue() );
}

bool SearchReconquerBuilding :: canUnitCapture( Vehicle* eht )
{
   return (eht->typ->functions & cf_conquer )
           && fieldAccessible ( buildingToCapture->getEntryField(), eht) == 2 ;

}

void         SearchReconquerBuilding :: testfield(const MapCoordinate& mc)
{
      Vehicle* eht = gamemap->getField(mc)->vehicle;
      // Building* bld = getfield(xp,yp)->building;
      if ( eht )
         if ( ai.getPlayer().diplomacy.isHostile( eht->getOwner() ) )
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


float AI :: getCaptureValue ( const Building* bld, int traveltime  )
{
   if ( traveltime < 0 )
      traveltime = 0;
   return float(bld->aiparam[getPlayerNum()]->getValue()) / float(traveltime+1);
}


bool AI :: checkReConquer ( Building* bld, Vehicle* veh )
{
   SearchReconquerBuilding srb ( *this, bld );
   srb.initsearch ( bld->getEntry(), (maxTransportMove + maxUnitMove/2) / maxmalq + 1, 1 );
   srb.startsearch();
   bool enemyNear = srb.returnresult();

   if ( enemyNear && veh ) {
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


struct CaptureTriple {
  Building* bld;
  Vehicle* veh;
  float val;
};

typedef struct CaptureTriple* pCaptureTriple;

typedef vector<pCaptureTriple> CaptureList;

class CaptureTripleComp : public binary_function<pCaptureTriple,pCaptureTriple,bool> {
public:
  explicit CaptureTripleComp() {};
  bool operator() (const pCaptureTriple t1, const pCaptureTriple t2) const {
    return t1->val > t2->val;
  }
};

void AI :: checkConquer( )
{
   // remove all capture orders for buildings which are no longer controlled by the enemy

   for ( BuildingCaptureContainer::iterator bi = buildingCapture.begin(); bi != buildingCapture.end(); ) {
      BuildingCaptureContainer::iterator nxt = bi;
      ++nxt;
      Vehicle* veh= getMap()->getUnit ( bi->second.unit );
      Building* bld = getMap()->getField( bi->first )->building;
      if ( !bld ) {
         buildingCapture.erase ( bi );
         bi = nxt;
         continue;
      }

      if ( !getPlayer(bld->getOwner()).diplomacy.isHostile( getPlayerNum() ) 
           || !( veh && fieldAccessible ( getMap()->getField( bi->first ), veh ) == 2 )) {

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

   displaymessage2("check for capturing buildings ... ");

   // check for stagnation
   int num_reachable_buildings=0;
   for ( BuildingCaptureContainer::iterator bi = buildingCapture.begin(); bi != buildingCapture.end(); bi++ ) 
      if ( bi->second.state!=BuildingCapture::conq_unreachable ) num_reachable_buildings++;
   // if all blds are marked as unreachable, reevaluate in hope for a map-change
   if ( buildingCapture.size()>0 && num_reachable_buildings==0 ) {
      for ( BuildingCaptureContainer::iterator bi = buildingCapture.begin(); bi != buildingCapture.end(); bi++ ) 
        bi->second.state=BuildingCapture::conq_noUnit;
   }
   
   CaptureList captureList;

   int buildingCounter = 0;

   for ( int c = 0; c <= 8; c++ ) {
      if ( c<8 ) {
         if ( !getPlayer(c).exist() ) continue;
         if ( !getPlayer().diplomacy.isHostile( c)  ) continue;
      }
      for ( Player::BuildingList::iterator bi = getPlayer(c).buildingList.begin(); bi != getPlayer(c).buildingList.end(); bi++ ) {
         Building* bld = *bi;
         int reachable = 0;
         if ( buildingCapture[ bld->getEntry() ].state != BuildingCapture::conq_noUnit ) continue;
         bool enemyNear = checkReConquer ( bld, 0 );

         ++buildingCounter;
         displaymessage2("check for capturing building %d ", buildingCounter);

         for ( Player::VehicleList::iterator vi = getPlayer().vehicleList.begin(); vi != getPlayer().vehicleList.end(); vi++ ) {
            Vehicle* veh = *vi;
            if ( !veh->canMove() ) continue;
            if ( fieldAccessible ( bld->getEntryField(), veh ) != 2 ) continue;
            if ( c!=8 && !(veh->typ->functions & cf_conquer) ) continue;
            if ( veh->aiparam[getPlayerNum()]->hasJob( AiParameter::job_conquer)  &&
                 veh->aiparam[getPlayerNum()]->getTask() != AiParameter::tsk_nothing ) continue;

            // here, units can be excluded from capturing
            if ( c!=8 ) {
                if( !veh->aiparam[getPlayerNum()]->hasJob(AiParameter::job_conquer)  ) continue;
                if( veh->aiparam[getPlayerNum()]->getTask() != AiParameter::tsk_nothing ) continue;
            }

            // any further factors should be incorporated into getCaptureValue
            float val=getCaptureValue( bld, veh );

            // malus if enemy is near (relevant if we are short of capture-units
            // or building is practically worthless)
            // TODO: should be made an optional parameter to getCaptureValue
            if ( val>0 && enemyNear ) val -= 0.1*veh->aiparam[getPlayerNum()]->getValue();


            if ( val > 0 ) {
               pCaptureTriple triple = new CaptureTriple;
               triple->bld=bld;
               triple->veh=veh;
               triple->val=val;
               captureList.push_back( triple );
               reachable = true;
            }
         }
         if ( reachable==0 )
            buildingCapture[ bld->getEntry() ].state = BuildingCapture::conq_unreachable;
      }
   }

   sort ( captureList.begin(), captureList.end(), CaptureTripleComp() );

   for ( CaptureList::iterator i = captureList.begin(); i != captureList.end(); i++ ) {
      Building* bld = (*i)->bld;
      Vehicle* veh = (*i)->veh;
      // float val = (*i)->val;
      delete (*i);

      // check whether bld and veh are still available
      if ( buildingCapture[ bld->getEntry() ].state != BuildingCapture::conq_noUnit ) continue;
      if ( veh->aiparam[getPlayerNum()]->getJob() == AiParameter::job_conquer &&
           veh->aiparam[getPlayerNum()]->getTask() != AiParameter::tsk_nothing ) continue;

      // dispatch capture order
      BuildingCapture& bc = buildingCapture[ bld->getEntry() ];
      if ( veh->typ->functions & cf_conquer )
         bc.state = BuildingCapture::conq_conqUnit;
      else
         bc.state = BuildingCapture::conq_unitNotConq;
      bc.unit = veh->networkid;
      
      veh->aiparam[getPlayerNum()]->setJob ( AiParameter::job_conquer );
      veh->aiparam[getPlayerNum()]->setTask ( AiParameter::tsk_move );
      veh->aiparam[getPlayerNum()]->dest.setnum( bld->getEntry().x, bld->getEntry().y, -1 );
   }

   // execute capture orders
   for ( BuildingCaptureContainer::iterator bi = buildingCapture.begin(); bi != buildingCapture.end(); ) {
      BuildingCaptureContainer::iterator nxt = bi;
      ++nxt;
      Vehicle* veh = getMap()->getUnit ( bi->second.unit );
      if ( veh ) {
         MapCoordinate3D dest = veh->aiparam[getPlayerNum()]->dest;
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

   // do something useful with units that are not used for capturing buildings
   for ( Player::VehicleList::iterator vi = getPlayer().vehicleList.begin(); vi != getPlayer().vehicleList.end(); vi++ )
      if ( (*vi)->aiparam[getPlayerNum()]->getJob() == AiParameter::job_conquer &&
           (*vi)->aiparam[getPlayerNum()]->getTask() == AiParameter::tsk_nothing )
           (*vi)->aiparam[getPlayerNum()]->setNextJob();

}


