/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <algorithm>
#include <cmath>
#include "containerbase-functions.h"
#include "typen.h"
#include "containercontrols.h"
#include "resourcenet.h"
#include <iostream>



template <class T, ContainerBaseType::ContainerFunctions f>
class GenericWorkerFactory : public ContainerBase::WorkClassFactory
{
      bool available( const ContainerBase* cnt )
      {
         return cnt->baseType->hasFunction( f );
      };

      ContainerBase::Work* produce( ContainerBase* cnt, bool queryOnly )
      {
         return new T(cnt);
      };
};


class BiResourceGenerationFactory : public ContainerBase::WorkClassFactory
{
      bool available( const ContainerBase* cnt )
      {
         return true;
      };

      ContainerBase::Work* produce( ContainerBase* cnt, bool queryOnly )
      {
         return new BiResourceGeneration(cnt);
      };
};

class MiningStationFactory : public ContainerBase::WorkClassFactory
{
      bool available( const ContainerBase* cnt )
      {
         return cnt->baseType->hasFunction( ContainerBaseType::MiningStation );
      };

      ContainerBase::Work* produce( ContainerBase* cnt, bool queryOnly )
      {
         return new MiningStation(cnt, queryOnly);
      };
};

class AutoHarvestObjectsFactory : public ContainerBase::WorkClassFactory
{
      bool available( const ContainerBase* cnt )
      {
         return cnt->baseType->hasFunction( ContainerBaseType::AutoHarvestObjects );
      };

      ContainerBase::Work* produce( ContainerBase* cnt, bool queryOnly )
      {
         return new AutoHarvestObjects(cnt, queryOnly);
      };
};

namespace
{
   const bool r5 = ContainerBase::registerWorkClassFactory( new BiResourceGenerationFactory, false );
   const bool r1 = ContainerBase::registerWorkClassFactory( new GenericWorkerFactory<MatterConverter, ContainerBaseType::MatterConverter>() );
   const bool r2 = ContainerBase::registerWorkClassFactory( new GenericWorkerFactory<ResourceSink, ContainerBaseType::ResourceSink> );
   const bool r3 = ContainerBase::registerWorkClassFactory( new GenericWorkerFactory<WindPowerplant, ContainerBaseType::WindPowerPlant> );
   const bool r4 = ContainerBase::registerWorkClassFactory( new GenericWorkerFactory<SolarPowerplant, ContainerBaseType::SolarPowerPlant> );
   const bool r6 = ContainerBase::registerWorkClassFactory( new MiningStationFactory );
   const bool r7 = ContainerBase::registerWorkClassFactory( new AutoHarvestObjectsFactory );
}

AutoHarvestObjects::AutoHarvestObjects( ContainerBase* _bld, bool justQuery_ )
{
   base = _bld;
   justQuery = justQuery_;
   hasRun = false;
   fieldCounter = 0;
}

bool AutoHarvestObjects::finished()
{
   return hasRun;
}


void AutoHarvestObjects::harvestObject( const MapCoordinate& pos, const ObjectType* obj )
{
   tfield* currentField = base->getMap()->getField(pos);
   if ( !currentField )
      return;

   // we are only harvesting on fields who have non-harvestable neighbouring field, 
   // to prevent clearing of areas where the 'island' fields that are supposed to remain
   // are not covered by the oject
   int regrowFields = 0;
   int spreadingFields = 0;
    for ( int i = 0; i < 6; ++i ) {
      MapCoordinate nextField = getNeighbouringFieldCoordinate( pos, i );
      if ( !harvestOnPosition(nextField)) {
         tfield* fld = base->getMap()->getField( nextField);
         if ( fld ) {
            if ( fld->checkforobject(obj))
               ++regrowFields ;
            else {
               if ( obj->buildable(fld))
                  ++spreadingFields;
            }
         }
      }
   }
   if ( regrowFields <= 0 || spreadingFields > 0 )
      return;


   Object* object = currentField->checkforobject( obj );

   if( object != NULL ) {
      Resources removeValue = object->typ->removecost;
      Resources removeCost;
      Resources removeBenefit;

      if( removeValue.energy < 0 )
         removeBenefit.energy = -removeValue.energy;
      else
         removeCost.energy = removeValue.energy;

      if( removeValue.material < 0 )
         removeBenefit.material = -removeValue.material;
      else
         removeCost.material = removeValue.material;

      if( removeValue.fuel < 0 )
         removeBenefit.fuel = -removeValue.fuel;
      else
         removeCost.fuel = removeValue.fuel;

      if( base->getResource( removeCost, true, 1, base->getOwner() ) == removeCost ) {
         cost += removeCost;
         harvested += removeBenefit;
         if( !justQuery ) {
            base->getResource( removeCost, false,  1, base->getOwner());
            base->putResource( removeBenefit, false,  1, base->getOwner() );
            currentField->removeobject( obj, true );
         }
         ++fieldCounter;
      }
   }
}

void AutoHarvestObjects::processField( const MapCoordinate& pos )
{
   tfield* currentField = base->getMap()->getField(pos);
   if ( !currentField )
      return;
   
   if ( currentField->building )
      return;

   if ( fieldCounter >= base->baseType->autoHarvest.maxFieldsPerTurn )
      return;
   
   for ( vector<IntRange>::const_iterator i = base->baseType->autoHarvest.objectsHarvestable.begin(); i != base->baseType->autoHarvest.objectsHarvestable.end();  ++i )
      for( int id=i->from; id <= i->to; ++id ) {
         const ObjectType *obj = base->getMap()->getobjecttype_byid( id );
         if ( obj )
            harvestObject( pos, obj );
      }

   for ( vector<IntRange>::const_iterator i = base->baseType->autoHarvest.objectGroupsHarvestable.begin(); i != base->baseType->autoHarvest.objectGroupsHarvestable.end(); ++i )
      for( int j = 0; j < base->getMap()->getObjectTypeNum(); ++j ) {
         const ObjectType *obj = base->getMap()->getobjecttype_bypos( j );
         if ( obj->groupID >= i->from && obj->groupID <= i->to )
            harvestObject( pos, obj );
      }

}



bool AutoHarvestObjects::harvestOnPosition( const MapCoordinate& pos )
{
   if ((pos.y+1)%3) 
      return true;
   else
      return false;
}

void AutoHarvestObjects::iterateField( const MapCoordinate& pos )
{
   /*
   if ( pos.y & 1 ) {
      processField(pos);
      return ;
   } else {
      if ( (((pos.y >> 1) & 1) && (pos.x & 1 )) || (((pos.y >> 1 ) & 1 )==0 && (pos.x & 1 )==0 )) {
         processField(pos);
         return;
      }
   }
   */
   if ( harvestOnPosition(pos ) ) 
      // y+1 is to match the harvesting pattern of player Xyphagoroszh on PBP planet Lussx :)
      processField(pos);
   
}




bool AutoHarvestObjects::run()
{
   hasRun = true;
   if( base->getCarrier() != NULL )
      return false;

   circularFieldIterator( base->getMap(), base->getPosition(), 0, base->baseType->autoHarvest.range, FieldIterationFunctor( this, &AutoHarvestObjects::iterateField ));

   return true;
}

Resources AutoHarvestObjects::getPlus()
{
   return harvested;
}

Resources AutoHarvestObjects::getUsage()
{
   return Resources();
}


float  getminingstationeficency ( int dist )
{
   // f ( x ) = a / ( b * ( x + d ) ) - c

   double a,b,c,d;

   a          =     10426.400 ;
   b          =     1.0710969 ;
   c          =     568.88887 ;
   d          =     6.1111109 ;

   return (a / ( b * (dist + d)) - c) / 1024;
}


GetMiningInfo::MiningInfo::MiningInfo()
{
   for ( int i = 0; i < maxminingrange+2; i++ )
      efficiency[i] = 0;
   nextMiningDistance = -1;
}

GetMiningInfo :: GetMiningInfo ( const ContainerBase* container ) : SearchFields ( container->getMap() ), miningStation( container )
{
   run();
}

void GetMiningInfo :: testfield ( const MapCoordinate& mc )
{
   tfield* fld = gamemap->getField ( mc );
   if ( miningInfo.efficiency[ dist ] == 0 )
      miningInfo.efficiency[ dist ] = int(getminingstationeficency ( dist ) * 1024);

   miningInfo.avail[dist].material += fld->material * resource_material_factor;
   miningInfo.avail[dist].fuel     += fld->fuel     * resource_fuel_factor;
   miningInfo.max[dist].material   += 255 * resource_material_factor;
   miningInfo.max[dist].fuel       += 255 * resource_fuel_factor;
   if ( miningInfo.nextMiningDistance == -1 ) {
      if ( miningStation->maxplus.fuel > 0 && fld->fuel > 0 )
         miningInfo.nextMiningDistance = dist;

      if ( miningStation->maxplus.material > 0 && fld->material > 0 )
         miningInfo.nextMiningDistance = dist;
   }
}


void GetMiningInfo :: run ()
{
   initsearch ( miningStation->getPosition(), 0, maxminingrange );
   startsearch();
}




MatterConverter :: MatterConverter( ContainerBase* _bld ) : bld ( _bld ), percentage ( 100 )
{}

bool MatterConverter :: run()
{
   int perc = percentage;

   int usageNum = 0;
   for ( int r = 0; r < 3; r++ )
      if ( bld->plus.resource(r) < 0 )
         ++usageNum;

   if ( usageNum > 0 ) {
      // if the resource generation requires other resources, don't waste anything
      // by producing more than storage capacity available

      for ( int r = 0; r < 3; r++ )
         if ( bld->plus.resource(r) > 0 ) {
            int p = bld->putResource ( bld->plus.resource(r), r, true, 1, bld->getOwner() );

            if ( perc > 100 * p / bld->plus.resource(r) )
               perc = 100 * p / bld->plus.resource(r) ;
         }
   }

   Resources toGet = bld->plus * perc / 100  ;
   for ( int r = 0; r < 3; r++ )
      if ( toGet.resource(r) < 0 )
         toGet.resource(r)  = - toGet.resource(r) ;
      else
         toGet.resource(r) = 0;


   Resources avail = bld->getResource ( toGet, true, 1, bld->getOwner() );

   for ( int r = 0; r < 3; r++ ) {
      if ( bld->plus.resource(r) < 0 ) {
         int p = 100 * avail.resource(r) / -bld->plus.resource(r);
         if ( p < perc )
            perc = p;
      }
   }


   bool didSomething = false;

   for ( int r = 0; r < 3; r++ )
      if ( bld->plus.resource(r) > 0 ) {
         bld->putResource( bld->plus.resource(r) * perc  / 100, r , false, 1, bld->getOwner() );
         if ( bld->plus.resource(r) * perc / 100  > 0)
            didSomething = true;

      } else {
         if ( bld->plus.resource(r) < 0 )
            bld->getResource( -bld->plus.resource(r) * perc  / 100, r , false, 1, bld->getOwner());
      }

   percentage -= perc;
   return didSomething;
}


bool MatterConverter :: finished()
{
   return percentage == 0;
}

Resources MatterConverter :: getPlus()
{
   Resources r;
   for ( int i = 0; i < 3; i++ )
      if ( bld->plus.resource(i) > 0 )
         r.resource(i) = bld->plus.resource(i);
   return r;
}

Resources MatterConverter :: getUsage()
{
   Resources r;
   for ( int i = 0; i < 3; i++ )
      if ( bld->plus.resource(i) < 0 )
         r.resource(i) = -bld->plus.resource(i);
   return r;
}


ResourceSink :: ResourceSink( ContainerBase* _bld ) : bld ( _bld )
{
   toGet = bld->plus;
   for ( int r = 0; r < 3; r++ )
      if ( toGet.resource(r) < 0 )
         toGet.resource(r)  = - toGet.resource(r) ;
      else
         toGet.resource(r) = 0;
}

bool ResourceSink :: run()
{
   Resources got  = bld->getResource( toGet, false, 1, bld->getOwner() );
   toGet -= got;
   for ( int r = 0; r < 3; r++ )
      if ( got.resource(r) > 0 )
         return true;

   return false;
}


bool ResourceSink :: finished()
{
   for ( int r = 0; r < 3; r++ )
      if ( toGet.resource(r) > 0 )
         return false;
   return true;
}

Resources ResourceSink :: getPlus()
{
   return Resources();
}

Resources ResourceSink :: getUsage()
{
   Resources r;
   for ( int i = 0; i < 3; i++ )
      if ( bld->plus.resource(i) < 0 )
         r.resource(i) = -bld->plus.resource(i);
   return r;
}


/*
Research :: Research( Building* _bld ) : bld ( _bld ), percentage ( 100 )
{
}
 
bool Research :: run()
{
   int perc = percentage;
 
   int usageNum = 0;
   for ( int r = 0; r < 3; r++ )
      if ( plus.resource(r) < 0 )
         ++usageNum;
 
   if ( usageNum > 0 ) {
      // if the resource generation requires other resources, don't waste anything
      // by producing more than storage capacity available
 
      for ( int r = 0; r < 3; r++ )
         if ( bld->plus.resource(r) > 0 ) {
            int p = putResource ( bld->plus.resource(r), 0, 1 );
 
            if ( perc > 100 * p / bld->plus.resource(r) )
               perc = 100 * p / bld->plus.resource(r) ;
}
}
 
   Resources toGet = bld->plus * perc / 100  ;
   for ( int r = 0; r < 3; r++ )
      if ( toGet.resource(r) < 0 )
         toGet.resource(r)  = - toGet.resource(r) ;
      else
         toGet.resource(r) = 0;
 
 
   Resources avail = bld->getResource ( toGet, 1 );
 
   for ( int r = 0; r < 3; r++ ) {
      if ( bld->plus.resource(r) ) {
         int p = 100 * avail.resource(r) / bld->plus.resource(r);
         if ( p < perc )
            perc = P;
}
}
 
 
   bool didSomething = false;
 
   for ( int r = 0; r < 3; r++ )
      if ( bld->plus.resource(r) > 0 ) {
         bld->putResource( bld->plus.resource(r) * perc  / 100, r , 0);
         if ( bld->plus.resource(r) * perc / 100  > 0)
            didSomething = true;
 
} else {
         bld->getResource( -bld->plus.resource(r) * perc  / 100, r , 0);
}
 
   percentage = perc;
   return didSomething;
}
 
 
bool Research :: finished()
{
   return percentage == 0;
}
 
Resources Research :: getUsage()
{
  Resources r;
  for ( int i = 0; i < 3; i++ 9
     if ( bld->plus.resource(r) < 0 )
        r.resource(r) = -bld->plus.resource(r)
  return r;
}
   **/

RegenerativePowerPlant :: RegenerativePowerPlant( ContainerBase* _bld ) : bld ( _bld )
{}

bool RegenerativePowerPlant :: finished()
{
   for( int r = 0; r < 3; r++ )
      if ( toProduce.resource(r) > 0 )
         return false;
   return true;
}

bool RegenerativePowerPlant :: run()
{
   Resources tp = bld->putResource( toProduce , false, 1, bld->getOwner() );
   bool didSomething = false;
   for  ( int r = 0; r < 3; r++ )
      if ( tp.resource(r) ) {
         didSomething = true;
         toProduce.resource(r) -= tp.resource(r);
      }
   return didSomething;
}

Resources RegenerativePowerPlant :: getUsage()
{
   return Resources();
}

Resources WindPowerplant :: getPlus()
{
   Resources p;
   for ( int r = 0; r < 3; r++ )
      p.resource(r) =  bld->maxplus.resource(r) * bld->getMap()->weather.windSpeed / 255;
   return p;
}

Resources SolarPowerplant :: getPlus()
{
   int sum = 0;
   int num = 0;
   vector<MapCoordinate> fields = bld->getCoveredFields();
   for ( vector<MapCoordinate>::iterator i = fields.begin(); i != fields.end(); ++i ) {
      tfield* fld = bld->getMap()->getField ( *i );
      int weather = 0;
      while ( fld->typ != fld->typ->terraintype->weather[weather] )
         weather++;

      sum += csolarkraftwerkleistung[weather];
      num ++;
   }

   Resources rplus;
   for ( int r = 0; r < 3; r++ )
      rplus.resource(r) = bld->maxplus.resource(r) * sum / ( num * 1024 );
   return rplus;
}


Resources BiResourceGeneration::getPlus()
{
   return bld->bi_resourceplus;
}




MiningStation :: MiningStation( ContainerBase* bld_  , bool justQuery_) : SearchFields ( bld_->getMap() ), bld ( bld_ ), justQuery( justQuery_ )
{
   int counter = 0;
   for ( int r = 1; r < 3; r++ ) {
      if ( bld->plus.resource(r) > 0 ) {
         ++counter;
         if ( counter == 2 )
            fatalError( ASCString("A mining station can only produce ONE kind of resource; building ID" ) + strrr(bld->baseType->id) + " is violating this" );

         toExtract_thisTurn.resource(r) = bld->plus.resource(r);

         for ( int rr = 0; rr < 3; rr++ )
            if ( bld->plus.resource(rr) < 0 )
               usageRatio[rr] = -double(bld->plus.resource(rr)) / double(bld->plus.resource(r));
            else
               usageRatio[rr] = 0;
      }
   }

   if( justQuery ) {
      hasRun = false;
      run();
      hasRun = true;
   } else
      hasRun = false;
}

bool MiningStation :: run()
{
   if ( justQuery && hasRun )
      return false;

   actuallyExtracted = Resources();

   for ( int r = 0; r < 3;++r)
      consumed[r] = 0;

   if ( !justQuery ) {
      spaceAvail = bld->putResource( toExtract_thisTurn, true, 1, bld->getOwner() );
      for ( int r = 0; r <3; ++r )
         if ( spaceAvail.resource(r) < 0 ) {
            warning( ASCString("map corruption detected; building space availability is negative! ") + resourceNames[r] );
            spaceAvail.resource( r ) = 0;
         }
   } else
      spaceAvail = toExtract_thisTurn;

   Resources toConsume;
   for ( int r = 0; r < 3; r++ )
      if ( bld->plus.resource(r) < 0 )
         toConsume.resource(r) = -bld->plus.resource(r);

   if ( !justQuery ) {
      powerAvail = bld->getResource( toConsume, true, 1, bld->getOwner() );
      for ( int r = 0; r <3; ++r )
         if ( powerAvail.resource(r) < 0 ) {
            warning( ASCString("map corruption detected; available power for mining station is negative! ") + resourceNames[r] );
            powerAvail.resource( r ) = 0;
         }
   } else
      powerAvail = toConsume;


   initsearch( bld->getPosition(), 0, maxminingrange );
   startsearch();

   if ( !justQuery) {
      for ( int r = 0; r < 3; ++r )
         bld->getResource( int(consumed[r]), r, false, 1, bld->getOwner() );
      bld->putResource(actuallyExtracted, false, 1, bld->getOwner() );
   }

   for ( int r = 0; r < 3; r++ )
      if ( actuallyExtracted.resource(r) > 0 )
         return true;

   return false;
}

void MiningStation :: testfield ( const MapCoordinate& mc )
{
   cancelSearch = true;
   for ( int r = 0; r < 3; r++ )
      if ( toExtract_thisTurn.resource(r) > 0 )
         cancelSearch = false;

   if ( cancelSearch == false ) {
      tfield* fld = gamemap->getField ( mc );
      float distEfficiency = getminingstationeficency ( dist );

      for ( int r = 1; r < 3; r++ ) {
         if ( toExtract_thisTurn.resource(r) > 0 ) {

            float resourceFactor;
            char *fieldResource;

            if ( r==1) {
               // resourceFactor = resource_material_factor * double(bld->baseType->efficiencymaterial) / 1024;
               resourceFactor = resource_material_factor;
               fieldResource = &fld->material;
            } else {
               // resourceFactor = resource_fuel_factor * double(bld->baseType->efficiencyfuel) / 1024;
               resourceFactor = resource_fuel_factor;
               fieldResource = &fld->fuel;
            }

            float perc = 1;

            // is enough resource available on the field
            perc = min  ( perc, (*fieldResource * resourceFactor) / (toExtract_thisTurn.resource(r) * distEfficiency));

            perc = min ( perc, float( double(spaceAvail.resource(r)) / (toExtract_thisTurn.resource(r) * distEfficiency )));

            for ( int i = 0; i < 3; ++i )
               if ( usageRatio[i] * toExtract_thisTurn.resource(r) > 0 )
                  perc = min  ( perc, float( double(powerAvail.resource(i)) / usageRatio[i] * toExtract_thisTurn.resource(r)));

            if ( perc < 0 ) {
               warning("Warning: mining station inconsistency\n");
               perc = 0;
            }

            if ( !justQuery )
               *fieldResource -= int( toExtract_thisTurn.resource(r) * perc * distEfficiency / resourceFactor );

            int ex = int( ceil(toExtract_thisTurn.resource(r) * perc * distEfficiency));
            actuallyExtracted.resource(r) += ex;
            spaceAvail.resource(r) -= ex;
            for ( int i = 0; i < 3; ++i) {
               if ( spaceAvail.resource(i) < -2 )  // we allow for small deviations due to rounding errors
                  warning("Warning: mining station inconsistency 2!\n");

               if ( spaceAvail.resource(i) < 0 )
                  spaceAvail.resource(i) = 0;
            }


            for ( int i = 0; i < 3; i++ ) {
               float c = usageRatio[i] * toExtract_thisTurn.resource(r) * perc;
               consumed[i] += c;
               powerAvail.resource(i) -= int( ceil(c) );
               if ( powerAvail.resource(i) < 0 )
                  powerAvail.resource(i) = 0;

            }

            toExtract_thisTurn.resource(r) -= int( toExtract_thisTurn.resource(r) * perc);


            if ( !justQuery ) {
               if ( !fld->resourceview )
                  fld->resourceview = new tfield::Resourceview;
               fld->resourceview->visible |= 1 << bld->getOwner();
               fld->resourceview->fuelvisible[bld->getOwner()] = fld->fuel;
               fld->resourceview->materialvisible[bld->getOwner()] = fld->material;
            }
         }
      }
   }
}

bool MiningStation :: finished()
{
   for ( int r = 0; r < 3; r++ )
      if ( toExtract_thisTurn.resource(r) )
         return false;
   return true;
}

Resources MiningStation :: getPlus()
{
   return actuallyExtracted;
}

Resources MiningStation :: getUsage()
{
   Resources res;
   for ( int r = 0; r < 3; ++r)
      res.resource(r) = int( ceil(consumed[r]));
   return res;
}

