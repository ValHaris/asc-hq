/***************************************************************************
                          valuation.cpp  -  description
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

#include <iostream>
#include "ai_common.h"
#include "../actions/attackcommand.h"
#include "../actions/moveunitcommand.h"

const int value_armorfactor = 100;
const int value_weaponfactor = 3000;

const int ccbt_repairfacility = 200;    //  basic threatvalues for buildings
const int ccbt_hq = 10000;
const int ccbt_recycling = 50;
const int ccbt_training = 150;





  class CalculateThreat_VehicleType {
                         protected:
                              AI*               ai;

                              const VehicleType*      fzt;
                              int               weapthreat[8];
                              int               value;

                              virtual int       getdamage ( void )      { return 0;   };
                              virtual int       getexpirience ( void )  { return 0;   };
                              virtual int       getammunition( int i )  { return 1;   };
                              virtual int       getheight ( void )      { return 255; };
                          public:
                              void              calc_threat_vehicletype ( const VehicleType* _fzt );
                              CalculateThreat_VehicleType ( AI* _ai ) { ai = _ai; };
                              virtual ~CalculateThreat_VehicleType() {};
                       };

  class CalculateThreat_Vehicle : public CalculateThreat_VehicleType {
                           protected:
                                Vehicle*          eht;
                                virtual int       getdamage ( void );
                                virtual int       getexpirience ( void );
                                virtual int       getammunition( int i );
                                virtual int       getheight ( void );
                           public:
                              void              calc_threat_vehicle ( Vehicle* _eht );
                              CalculateThreat_Vehicle ( AI* _ai ) : CalculateThreat_VehicleType ( _ai ) {};
                       };


void         CalculateThreat_VehicleType :: calc_threat_vehicletype ( const VehicleType* _fzt )
{
   fzt = _fzt;

   for ( int j = 0; j < 8; j++ )
      weapthreat[j] = 0;

   for ( int i = 0; i < fzt->weapons.count; i++)
      if ( fzt->weapons.weapon[i].shootable() )
         if ( fzt->weapons.weapon[i].offensive() )
            for ( int j = 0; j < 8; j++)
               if ( fzt->weapons.weapon[i].targ & (1 << j) ) {
                  int d = 0;
                  int m = 0;
                  AttackFormula af( ai->getMap() );
                  for ( int e = (fzt->weapons.weapon[i].mindistance + maxmalq - 1)/ maxmalq; e <= fzt->weapons.weapon[i].maxdistance / maxmalq; e++ ) {    // the distance between two fields is maxmalq
                     d++;
                     int n = int( WeapDist::getWeaponStrength( &fzt->weapons.weapon[i], 0, e*maxmalq ) * fzt->weapons.weapon[i].maxstrength * af.strength_damage(getdamage()) * ( 1 + af.strength_experience(getexpirience())) );
                     m += int( n / log10(double(10*d)));
                  }
                  if (getammunition(i) == 0)
                     m /= 2;

                  if ( (fzt->weapons.weapon[i].sourceheight & getheight()) == 0)
                     m /= 2;

                  /*
                  if ( !(getheight() & ( 1 << j )))
                     m /= 2;
                  */

                  if (m > weapthreat[j])
                     weapthreat[j] = m;
               }


   if ( !fzt->aiparam[ai->getPlayerNum()] )
      fzt->aiparam[ ai->getPlayerNum() ] = new AiValue ( getFirstBit ( fzt->height ));

   for ( int l = 0; l < 8; l++ )
      fzt->aiparam[ ai->getPlayerNum() ]->threat.threat[l] = weapthreat[l];

   value = fzt->armor * value_armorfactor * (100 - getdamage()) / 100;

   for ( int s = 0; s < 7; )
      if ( weapthreat[s] < weapthreat[s+1] ) {
         int temp = weapthreat[s];
         weapthreat[s] = weapthreat[s+1];
         weapthreat[s+1] = temp;
         if ( s > 0 )
            s--;
      } else
         s++;

   for ( int k = 0; k < 8; k++ )
      value += weapthreat[k] * value_weaponfactor / (k+1);

   fzt->aiparam[ ai->getPlayerNum() ]->setValue ( value );
   fzt->aiparam[ ai->getPlayerNum() ]->valueType = 0;
}


int          CalculateThreat_Vehicle :: getammunition( int i )
{
   return eht->ammo[i];
}

int          CalculateThreat_Vehicle :: getheight(void)
{
   return eht->height;
}

int          CalculateThreat_Vehicle :: getdamage(void)
{
   return eht->damage;
}

int          CalculateThreat_Vehicle :: getexpirience(void)
{
   return eht->experience;
}


void         CalculateThreat_Vehicle :: calc_threat_vehicle ( Vehicle* _eht )
{

   eht = _eht;
   calc_threat_vehicletype ( vehicleTypeRepository.getObject_byID ( eht->typ->id ) );

   if ( !eht->aiparam[ai->getPlayerNum()] )
      eht->aiparam[ai->getPlayerNum()] = new AiParameter ( eht );

   AiParameter* aip = eht->aiparam[ai->getPlayerNum()];
   for ( int l = 0; l < 8; l++ )
      aip->threat.threat[l] = eht->typ->aiparam[ ai->getPlayerNum() ]->threat.threat[l];

   int value = eht->typ->aiparam[ ai->getPlayerNum() ]->getValue();
   for ( ContainerBase::Cargo::const_iterator i = eht->getCargo().begin(); i != eht->getCargo().end(); ++i )
      if ( *i ) {
         if ( !(*i)->aiparam[ai->getPlayerNum()] ) {
            CalculateThreat_Vehicle ctv ( ai );
            ctv.calc_threat_vehicle( *i );
         }
         value += (*i)->aiparam[ai->getPlayerNum()]->getValue();
      }
      

   aip->setValue ( value );


   if ( aip->getJob() == AiParameter::job_undefined )
      if ( eht->canMove() )
         aip->setJob( AI::chooseJob ( eht->typ ));
/*
   generatethreatvalue();
   int l = 0;
   for ( int b = 0; b <= 7; b++) {
      eht->threatvalue[b] = weapthreatvalue[b];
      if (weapthreatvalue[b] > l)
         l = weapthreatvalue[b];
   }                         <Zwischenablage leer>
   eht->completethreatvalue = threatvalue2 + l;
   eht->completethreatvaluesurr = threatvalue2 + l;
   eht->threats = 0;
*/
}

AiParameter::JobList AI::chooseJob ( const VehicleType* typ )
{
   AiParameter::JobList jobList;

   if ( typ->recommendedAIJob != AiParameter::job_undefined ) {
      jobList.push_back ( typ->recommendedAIJob );
      return jobList;
   }

   int maxmove = minint;
   for ( int i = 0; i< 8; i++ )
      if ( typ->height & ( 1 << i ))
         maxmove = max ( typ->movement[i] , maxmove );

   int maxstrength = minint;
   for ( int w = 0; w < typ->weapons.count; w++ )
      if ( typ->weapons.weapon[w].offensive() )
         maxstrength= max (  typ->weapons.weapon[w].maxstrength, maxstrength );

   bool service = false;
   for ( int w = 0; w < typ->weapons.count; w++ )
      if ( typ->weapons.weapon[w].service() )
         service = true;

   if ( ( typ->hasFunction( ContainerBaseType::ExternalRepair ) || service) && maxmove >= minmalq && maxstrength < 45)
      jobList.push_back ( AiParameter::job_supply );


   if ( typ->hasFunction( ContainerBaseType::ConquerBuildings ) ) {
      /* if ( functions & cf_trooper )  {
         if ( typ->height & chfahrend )
            jobList.push_back ( AiParameter::job_conquer );
      } else { */
         if ( maxstrength < maxmove )
            jobList.push_back ( AiParameter::job_conquer );
      // }
   }

   if ( ( maxstrength*1.5 < typ->view
           || (maxstrength*1.5 < typ->jamming && !typ->hasFunction( ContainerBaseType::JamsOnlyOwnField )))
       && maxmove > minmalq  )
      jobList.push_back (  AiParameter::job_recon );

   if ( maxstrength > 0 )
      jobList.push_back ( AiParameter::job_fight );

   jobList.push_back ( AiParameter::job_undefined );
   return jobList;
}



void  AI :: calculateThreat ( const VehicleType* vt)
{
   CalculateThreat_VehicleType ctvt ( this );
   ctvt.calc_threat_vehicletype( vt );
}


void  AI :: calculateThreat ( Vehicle* eht )
{
   CalculateThreat_Vehicle ctv ( this );
   ctv.calc_threat_vehicle( eht );
}


void  AI :: calculateThreat ( Building* bld )
{
   calculateThreat ( bld, getPlayerNum());
//   calculateThreat ( bld, 8 );
}

void  AI :: calculateThreat ( Building* bld, int player )
{
   if ( !bld->aiparam[ player ] )
      bld->aiparam[ player ] = new AiValue ( getFirstBit ( bld->typ->height ) );

   int b;


   // Since we have two different resource modes now, this calculation should be rewritten....
   int value = (bld->plus.energy / 10) + (bld->plus.fuel / 10) + (bld->plus.material / 10) + (bld->actstorage.energy / 20) + (bld->actstorage.fuel / 20) + (bld->actstorage.material / 20);
   
   for ( ContainerBase::Cargo::const_iterator i = bld->getCargo().begin(); i != bld->getCargo().end(); ++i )
      if ( *i ) {
         if ( !(*i)->aiparam[ player ] )
            calculateThreat ( *i );
         value += (*i)->aiparam[ player ]->getValue();
      }

   for (b = 0; b < bld->getProduction().size(); b++)
      if ( bld->getProduction()[b] )  {
         if ( !bld->getProduction()[b]->aiparam[ player ] )
            calculateThreat ( bld->getProduction()[b] );
         value += bld->getProduction()[b]->aiparam[ player ]->getValue() / 10;
      }

   if (bld->typ->hasFunction( ContainerBaseType::InternalUnitRepair  ))
      value += ccbt_repairfacility;
   
   if (bld->typ->hasFunction( ContainerBaseType::TrainingCenter  ) )
      value += ccbt_training;
   if (bld->typ->hasFunction( ContainerBaseType::RecycleUnits  )  )
      value += ccbt_recycling;

   bld->aiparam[ player ]->setValue ( value );
}



void AI :: WeaponThreatRange :: run ( Vehicle* _veh, int x, int y, AiThreat* _threat )
{
   threat = _threat;
   veh = _veh;
   for ( height = 0; height < 8; height++ )
      for ( weap = 0; weap < veh->typ->weapons.count; weap++ )
         if ( veh->height & veh->typ->weapons.weapon[weap].sourceheight )
            if ( (1 << height) & veh->typ->weapons.weapon[weap].targ )
                if ( veh->typ->weapons.weapon[weap].shootable()  && veh->typ->weapons.weapon[weap].offensive() ) {
                   initsearch ( MapCoordinate(x, y), veh->typ->weapons.weapon[weap].maxdistance/maxmalq, veh->typ->weapons.weapon[weap].mindistance/maxmalq );
                   startsearch();
                }
}

void AI :: WeaponThreatRange :: testfield ( const MapCoordinate& mc )
{
   if ( dist*maxmalq <= veh->typ->weapons.weapon[weap].maxdistance )
      if ( dist*maxmalq >= veh->typ->weapons.weapon[weap].mindistance ) {
         AttackFormula af ( ai->getMap() );
         int strength = int ( WeapDist::getWeaponStrength( &veh->typ->weapons.weapon[weap], ai->getMap()->getField(mc)->getWeather(), dist*maxmalq, veh->height, 1 << height )
                              * veh->typ->weapons.weapon[weap].maxstrength
                              * (1 + af.strength_experience ( veh->experience ) + af.strength_attackbonus ( gamemap->getField(startPos)->getattackbonus() ))
                              * af.strength_damage ( veh->damage )
                             );

         if ( strength ) {
            int pos = mc.x + mc.y * ai->getMap()->xsize;
            if ( strength > threat[pos].threat[height] )
               threat[pos].threat[height] = strength;
         }
      }
}

void AI :: calculateFieldInformation ( void )
{
   if ( fieldNum && fieldNum != activemap->xsize * activemap->ysize ) {
      delete[] fieldInformation;
      fieldInformation = NULL;
      fieldNum = 0;
   }
   if ( !fieldInformation ) {
      fieldNum = activemap->xsize * activemap->ysize;
      fieldInformation = new FieldInformation[ fieldNum ];
   } else
      for ( int a = 0; a < fieldNum; a++ )
         fieldInformation[ a ].reset();

   AiThreat*  singleUnitThreat = new AiThreat[fieldNum];

   // we now check the whole map
   for ( int y = 0; y < activemap->ysize; y++ ) {
      checkKeys();
      for ( int x = 0; x < activemap->xsize; x++ ) {
         MapField* fld = activemap->getField ( x, y );
         if ( config.wholeMapVisible || fieldvisiblenow ( fld, getPlayerNum() ) )
            if ( fld->vehicle && getPlayer().diplomacy.isHostile( fld->vehicle->getOwner() )) {
               WeaponThreatRange wr ( this );
               if ( !fld->vehicle->typ->wait ) {

                  // The unit may have already moved this turn.
                  // So we give it the maximum movementrange

                  TemporaryContainerStorage tus ( fld->vehicle );

                  fld->vehicle->setMovement ( fld->vehicle->maxMovement(), 0);

                  if ( MoveUnitCommand::avail ( fld->vehicle )) {
                     MoveUnitCommand muc ( fld->vehicle );
                     muc.searchFields();

                     const set<MapCoordinate3D>& fields =  muc.getReachableFields();
                     for ( set<MapCoordinate3D>::const_iterator i = fields.begin(); i != fields.end(); ++i )
                        wr.run ( fld->vehicle, i->x, i->y, singleUnitThreat );
                  
                     const set<MapCoordinate3D>& ifields  =  muc.getReachableFieldsIndirect();
                     for ( set<MapCoordinate3D>::const_iterator i = ifields.begin(); i != ifields.end(); ++i )
                        wr.run ( fld->vehicle, i->x, i->y, singleUnitThreat );
                     
                  }
                  tus.restore();
               } else
                  wr.run ( fld->vehicle, x, y, singleUnitThreat );


               for ( int a = 0; a < fieldNum; a++ ) {
                  for ( int b = 0; b < 8; b++ )
                     fieldInformation[a].threat.threat[b] += singleUnitThreat[a].threat[b];

                  singleUnitThreat[ a ].reset();
               }
            }

         FieldInformation& fi = fieldInformation[y*getMap()->xsize+x];
         for ( int i = 0; i< sidenum; i++ ) {
            MapField* f = getMap()->getField ( getNeighbouringFieldCoordinate ( MapCoordinate(x,y), i ));
            if ( f && f->vehicle && f->vehicle->weapexist() && f->vehicle->color < 8*8 )
               fi.units[f->vehicle->color/8] += 1;
         }
         int n = 0;
         int c = -1;
         for ( int i = 0; i < 8; i++ ) {
            if ( fi.units[i] > n ) {
               n = fi.units[i];
               c = i;
            }
         }
         fi.control = c;
      }
   }
   delete[] singleUnitThreat;
}


void     AI :: calculateAllThreats( void )
{
   // Calculates the basethreats for all vehicle types
   if ( !baseThreatsCalculated ) {
      for ( int w = 0; w < vehicleTypeRepository.getNum(); w++) {
         VehicleType* fzt = vehicleTypeRepository.getObject_byPos(w);
         if ( fzt )
            calculateThreat( fzt );

      }
      baseThreatsCalculated = 1;
   }

   // Some further calculations that only need to be done once.
   if ( maxTrooperMove == 0) {
      for ( int v = 0; v < vehicleTypeRepository.getNum(); v++) {
         VehicleType* fzt = vehicleTypeRepository.getObject_byPos( v );
         if ( fzt )
            if ( fzt->hasFunction( ContainerBaseType::ConquerBuildings  ) )
               if ( fzt->movement[getFirstBit(chfahrend)] > maxTrooperMove )   // buildings can only be conquered on ground level, or by moving to adjecent field which is less
                  maxTrooperMove = fzt->movement[getFirstBit(chfahrend)];
      }
   }
   if ( maxTransportMove == 0 ) {
      for (int v = 0; v < vehicleTypeRepository.getNum(); v++) {
         VehicleType* fzt = vehicleTypeRepository.getObject_byPos( v );
         if ( fzt )
            for ( int w = 0; w <= 7; w++) // cycle through all levels of height
               if (fzt->movement[w] > maxTransportMove)
                  maxTransportMove = fzt->movement[w];
      }
      maxUnitMove = maxTransportMove;
   }
   for ( int height = 0; height < 8; height++ )
      if ( maxWeapDist[height] < 0 ) {

         maxWeapDist[height] = 0; // It may be possible that there is no weapon to shoot to a specific height

         for ( int v = 0; v < vehicleTypeRepository.getNum(); v++) {
            VehicleType* fzt = vehicleTypeRepository.getObject_byPos( v );
            if ( fzt )
               for ( int w = 0; w < fzt->weapons.count ; w++)
                  if ( fzt->weapons.weapon[w].maxdistance > maxWeapDist[height] )
                     if ( fzt->weapons.weapon[w].targ & ( 1 << height ))   // targ is a bitmap, each bit standing for a level of height
                         maxWeapDist[height] = fzt->weapons.weapon[w].maxdistance;
         }
      }




   // There are only 8 players in ASC, but there may be neutral units (player == 8)
   for ( int v = 0; v < 9; v++)
      if (activemap->player[v].exist() || v == 8) {

         // Now we cycle through all units of this player
         for ( Player::VehicleList::iterator vi = getPlayer(v).vehicleList.begin(); vi != getPlayer(v).vehicleList.end(); vi++ ) {
            Vehicle* veh = *vi;
            // if ( !veh->aiparam[ getPlayerNum() ] )
               calculateThreat ( veh );
         }

         // Now we cycle through all buildings
         for ( Player::BuildingList::iterator bi = getPlayer(v).buildingList.begin(); bi != getPlayer(v).buildingList.end(); bi++ )
            calculateThreat ( *bi );
      }

}

void AI :: FieldInformation :: reset ( )
{
   threat.reset();
   for ( int i = 0; i< 8; i++ )
      units[i] = 0;
   control = -1;
}


AiThreat& AI :: getFieldThreat ( int x, int y )
{
   if ( !fieldInformation )
      calculateFieldInformation ();
   return fieldInformation[y * activemap->xsize + x ].threat;
}

AI::FieldInformation& AI :: getFieldInformation ( int x, int y )
{
   if ( !fieldInformation )
      calculateFieldInformation ();
   return fieldInformation[y * activemap->xsize + x ];
}


void AI :: Section :: init ( AI* _ai, int _x, int _y, int xsize, int ysize, int _xp, int _yp )
{
   ai = _ai;
   init ( _x, _y, xsize, ysize, _xp, _yp );
}

void AI :: Section :: init ( int _x, int _y, int xsize, int ysize, int _xp, int _yp )
{
   x1 = _x;
   y1 = _y;
   x2 = _x + xsize;
   y2 = _y + ysize;

   xp = _xp;
   yp = _yp;

   if ( x1 < 0 ) x1 = 0;
   if ( y1 < 0 ) y1 = 0;
   if ( x2 >= ai->activemap->xsize ) x2 = ai->activemap->xsize-1;
   if ( y2 >= ai->activemap->ysize ) y2 = ai->activemap->ysize-1;


   centerx = (x1 + x2) / 2;
   centery = (y1 + y2) / 2;
   numberOfFields = (x2-x1+1) * ( y2-y1+1);

   absUnitThreat.reset();
   absFieldThreat.reset();

   for ( int j = 0; j < aiValueTypeNum; j++ )
     value[j] = 0;

   for ( int y = y1; y <= y2; y++ )
      for ( int x = x1; x <= x2; x++ ) {
         absFieldThreat += ai->getFieldThreat ( x, y );
         MapField* fld = ai->activemap->getField ( x, y );
         if ( fld->vehicle && ai->getPlayer().diplomacy.isHostile( fld->vehicle->getOwner() ) ) {
            if ( !fld->vehicle->aiparam[ ai->getPlayerNum() ] )
               ai->calculateThreat ( fld->vehicle );
            AiParameter& aip = * fld->vehicle->aiparam[ ai->getPlayerNum() ];
            absUnitThreat += aip.threat;
            value[ aip.valueType ] += aip.getValue();
         }
      }

   for ( int i = 0; i <  absUnitThreat.threatTypes; i++ ) {
      avgUnitThreat.threat[i] = absUnitThreat.threat[i] / numberOfFields;
      avgFieldThreat.threat[i] = absFieldThreat.threat[i] / numberOfFields;
   }

}

int AI :: Section :: numberOfAccessibleFields ( const Vehicle* veh )
{
   int num = 0;
   for ( int y = y1; y <= y2; y++ )
      for ( int x = x1; x <= x2; x++ )
         if ( fieldAccessible ( ai->activemap->getField ( x, y ), veh ) == 2)
            num++;

   return num;
}

AI :: Sections :: Sections ( AI* _ai ) : ai ( _ai ) , section ( NULL )
{
   sizeX = 8;
   sizeY = 16;
   numX = ai->activemap->xsize * 2 / sizeX + 1;
   numY = ai->activemap->ysize * 2 / sizeY + 1;
}

void AI :: Sections :: reset ()
{
   if ( section ) {
      delete[] section;
      section = NULL;
   }
}

AI :: Sections :: ~Sections()
{
   reset();  
}


void AI :: Sections :: calculate ( void )
{
   if ( !section ) {
      section = new Section[ numX*numY ]; //  ( ai );
      for ( int x = 0; x < numX; x++ )
         for ( int y = 0; y < numY; y++ )
            section[ x + numX * y ].init ( ai, x * ai->activemap->xsize / numX, y * ai->activemap->ysize / numY, sizeX, sizeY, x, y );

    }
}

AI::Section& AI :: Sections :: getForCoordinate ( int xc, int yc )
{
   if ( !section )
      calculate();

   int dist = maxint;
   Section* sec = NULL;
   for ( int x = 0; x < numX; x++ )
      for ( int y = 0; y < numY; y++ ) {
         Section& s2 = getForPos ( x, y );
         int d = beeline ( xc, yc, s2.centerx, s2.centery);
         if ( d < dist ) {
            dist = d;
            sec = &getForPos ( x, y );
         }
      }

   return *sec;
}

AI::Section& AI :: Sections :: getForPos ( int xn, int yn )
{
   if ( xn >= numX || yn >= numY || xn < 0 || yn < 0 )
      displaymessage( "AI :: Sections :: getForPos - invalid parameters: %d %d", 2, xn, yn );

   return section[xn+yn*numX];
}

/*
  This is like some ball rolling down from some high potential spike
*/
MapCoordinate AI :: Sections :: getAlternativeField( const MapCoordinate& pos, map<MapCoordinate,int>* destinationCounter, int height, AStar3D* ast )
{
   MapCoordinate result  = pos;
   int potential = destinationCounter->operator[](pos);
   for ( int d = 0; d< 6; ++d ) {
      MapCoordinate m = getNeighbouringFieldCoordinate( pos, d );
      if ( ast->getFieldAccess(m) & height) {
         if ( destinationCounter->find( m ) == destinationCounter->end() )
            return m;
         else {
            if ( (destinationCounter->find(m)->second * 11 / 10) < potential ) {
               result = m;
               potential = destinationCounter->find(m)->second;
            }
         }
      }
   }
   if ( result == pos )
      return result;
   else
      return getAlternativeField( result , destinationCounter, height, ast );
}


AI::Section* AI :: Sections :: getBest ( int pass, Vehicle* veh, MapCoordinate3D* dest, bool allowRefuellOrder, bool secondRun, map<MapCoordinate,int>* destinationCounter )
{
   /*
      In the first pass wwe check were all the units would go if there wouldn't be
      a threat anywhere.
      In the second pass the threat of a section is devided by the number of units that
      are going there
   */

   AStar3D* ast = 0;
   bool deleteAst = false;
   RefuelConstraint* rfc = NULL;
   if ( RefuelConstraint::necessary ( veh, *ai )) {
      if ( secondRun )
         rfc = new RefuelConstraint ( *ai, veh, veh->maxMovement()*5 );
      else
         rfc = new RefuelConstraint ( *ai, veh );

      rfc->findPath( false );
      ast = rfc->getAst();
   } else {
      ast = new AStar3D ( ai->getMap(), veh, false );
      ast->findAllAccessibleFields (  );
      deleteAst = true;
   }

   AiParameter& aip = *veh->aiparam[ ai->getPlayerNum() ];

   float d = minfloat;
   // float nd = minfloat;
   AI::Section* frst = NULL;

   float maxSectionThread = 0;
   for ( int y = 0; y < numY; y++ )
      for ( int x = 0; x < numX; x++ ) {
          AI::Section& sec = getForPos( x, y );
          int threat = sec.avgUnitThreat.threat[aip.valueType];
          if ( threat > maxSectionThread )
              maxSectionThread = threat;
      }


   TemporaryContainerStorage tus ( veh );
   veh->resetMovement(); // to make sure the wait-for-attack flag doesn't hinder the attack
   veh->attacked = 0;

   int sectionsPossibleWithMaxFuell = 0;

   for ( int h = 1; h < 0xff; h<<= 1 )
      if ( veh->typ->height & h )
         for ( int y = 0; y < numY; y++ )
            for ( int x = 0; x < numX; x++ ) {
                int xtogoSec = -1;
                int ytogoSec = -1;

                AI::Section& sec = getForPos( x, y );
                float t = 0;
                for ( int i = 0; i < aiValueTypeNum; i++ )
                   t += aip.threat.threat[i] * sec.value[i];

                float f = t;

                if ( sec.avgUnitThreat.threat[ veh->getValueType(h) ] ) {
                   int relThreat = int( 4*maxSectionThread / sec.avgUnitThreat.threat[veh->getValueType(h)] + 1);
                   f /= relThreat;
                }

                /*
                if ( sec.avgUnitThreat.threat[aip.valueType] >= 0 )
                   f = t / log( sec.avgUnitThreat.threat[aip.valueType] );
                else
                   f = t;
                */

                int dist = beeline ( veh->xpos, veh->ypos, sec.centerx, sec.centery ) + 3 * veh->maxMovement();
                if ( dist )
                   f /= log(double(dist));

                if ( f > d ) {
                   int ac  = 0;
                   int nac = 0;
                   int mindist = maxint;
                   int targets = 0;


                   for ( int yp = sec.y1; yp <= sec.y2; yp++ )
                      for ( int xp = sec.x1; xp <= sec.x2; xp++ ) {
                         if ( ast->getFieldAccess(xp, yp) & h ) {
                            int mandist = abs( sec.centerx - xp ) + 2*abs ( sec.centery - yp );
                            if ( mandist < mindist ) {
                               mindist = mandist;
                               
                               if ( destinationCounter && destinationCounter->find( MapCoordinate(xp,yp)) != destinationCounter->end() ) {
                                  /* we are checking if there are too many units heading for this field already 
                                     check how many are for neighbouringfields */
                                  
                                  
                                  MapCoordinate alt = getAlternativeField( MapCoordinate(xp,yp), destinationCounter, h, ast );
                                  xtogoSec = alt.x;
                                  ytogoSec = alt.y;
                               } else {
                                 xtogoSec = xp;
                                 ytogoSec = yp;
                               }
                            }

                            ai->setVision(visible_all);

                            ac++;

                            veh->xpos = xp;
                            veh->ypos = yp;
                            veh->height = h;

                            if ( AttackCommand::avail( veh )) {
                               AttackCommand attack ( veh ); 
                               attack.searchTargets();
                               targets += attack.getAttackableUnits().size();
                            }
                            ai->setVision(visible_ago);
                         } else
                            nac++;
                      }

                   if ( xtogoSec >= 0 && ytogoSec >= 0 ) {
                      if ( !rfc || rfc->returnFromPositionPossible ( MapCoordinate3D( xtogoSec, ytogoSec, h ))) {
                         int notAccessible = 100 * nac / (nac+ac);
                         if ( notAccessible < 85  && targets ) {   // less than 85% of fields not accessible
                            float nf = f * ( 100-notAccessible) / 100; // *  ( 100 - notAccessible );
                            if ( nf > d ) {
                               d = nf;
                               // nd = nf;
                               frst = &getForPos ( x, y );
                               if ( dest )
                                  *dest = MapCoordinate3D ( xtogoSec, ytogoSec, h );
                            }
                         }
                      } else
                         if ( allowRefuellOrder ) {
                            if ( rfc && rfc->returnFromPositionPossible ( MapCoordinate3D( xtogoSec, ytogoSec, h ), veh->getStorageCapacity().fuel ))
                               sectionsPossibleWithMaxFuell++;
                         }
                   }
                }
            }

   tus.restore();
   if ( deleteAst ) {
      delete ast;
   } else {
      ast = NULL;
      delete rfc;
   }

   if ( !frst ) {
      if ( sectionsPossibleWithMaxFuell && allowRefuellOrder )
         ai->issueRefuelOrder ( veh, false );
      else
        if ( RefuelConstraint::necessary ( veh, *ai ) && !secondRun )
           return getBest ( pass, veh, dest, allowRefuellOrder, true );

   }
   return frst;

}

