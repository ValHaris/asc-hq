/*
     This file is part of Advanced Strategic Command; http://www.asc-hq.de
     Copyright (C) 1994-2008  Martin Bickel  and  Marc Schellenberger

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


#include "constructbuildingcommand.h"

#include "../vehicle.h"
#include "../mapfield.h"
#include "../gamemap.h"
#include "../viewcalculation.h"
#include "../spfst.h"
#include "../mapdisplayinterface.h"
#include "action-registry.h"
#include "../itemrepository.h"
#include "../containercontrols.h"
#include "changeunitproperty.h"
#include "spawnbuilding.h"
#include "changeunitmovement.h"
#include "consumeresource.h"




bool ConstructBuildingCommand :: avail ( const Vehicle* eht )
{
   if ( !eht )
      return false;

   if ( eht->getMap()->getgameparameter(cgp_forbid_building_construction) )
      return false;

   if ( eht->attacked == false && !eht->hasMoved() )
      if ( eht->getOwner() == eht->getMap()->actplayer )
         if ( eht->typ->hasFunction( ContainerBaseType::ConstructBuildings  ))
            return true;

   return false;
}


ConstructBuildingCommand :: ConstructBuildingCommand ( Vehicle* container )
      : UnitCommand ( container ), buildingTypeID(-1)
{

}


Resources ConstructBuildingCommand::getProductionCost( const BuildingType* bld ) const
{
   int mf = getMap()->getgameparameter ( cgp_building_material_factor );
   int ff = getMap()->getgameparameter ( cgp_building_fuel_factor );

   if ( !mf )
      mf = 100;
   if ( !ff )
      ff = 100;

   return Resources( 0, bld->productionCost.material * mf / 100, bld->productionCost.fuel * ff / 100 );
}


ConstructBuildingCommand::Lack ConstructBuildingCommand::buildingProductionPrerequisites( const BuildingType* type ) const
{
   int l = 0;

   if ( !type->techDependency.available ( getMap()->getPlayer(getUnit()).research))
      l |= Lack::Research;


   int hd = getheightdelta ( log2 ( getUnit()->height ), log2 ( type->height ));

   if ( hd != 0 )
      l |= Lack::Level;


   Resources cost = getProductionCost( type );
   Resources avail = getUnit()->getResource( cost );
   for ( int r = 0; r < Resources::count ; ++r )
      if ( cost.resource(r) > avail.resource(r) )
         l |= 1 << r;

   return Lack(l);
}

ConstructBuildingCommand::Producables ConstructBuildingCommand :: getProduceableBuildings( )
{
   Producables entries;
   if ( !avail( getUnit() ) )
      return entries;

   for ( int i = 0; i < buildingTypeRepository.getNum(); i++) {
      const BuildingType* bld =  buildingTypeRepository.getObject_byPos( i );

      for ( int j = 0; j < getUnit()->typ->buildingsBuildable.size(); j++ )
         if ( getUnit()->typ->buildingsBuildable[j].from <= bld->id &&
               getUnit()->typ->buildingsBuildable[j].to   >= bld->id ) {

            entries.push_back ( ProductionEntry ( bld, getProductionCost( bld ), buildingProductionPrerequisites( bld  ) ));

         }
   }
   return entries;
}

void ConstructBuildingCommand :: fieldChecker( const MapCoordinate& pos )
{
   tfield* fld = getMap()->getField(pos);
   if ( !fld )
      return;

   if ( fld->vehicle || fld->building )
      return;
}

bool ConstructBuildingCommand::buildingFits( const MapCoordinate& entry )
{

   tfield* entryfield =  getMap()->getField(entry);
   if ( !entryfield )
      return false;

   BuildingType* bld = buildingTypeRepository.getObject_byID( buildingTypeID );

   if ( bld ) {
      bool b = true;
      for ( int y1 = 0; y1 <= 5; y1++)
         for ( int x1 = 0; x1 <= 3; x1++)
            if ( bld->fieldExists ( BuildingType::LocalCoordinate(x1, y1)) ) {
               tfield* fld = actmap->getField ( bld->getFieldCoordinate( entry, BuildingType::LocalCoordinate(x1,y1) ));
               if ( fld ) {
                  if ( fld->vehicle != NULL && fld->vehicle->height <= chfahrend )
                     b = false;
                  
                  if ( bld->height <= chfahrend )
                     if ( bld->terrainaccess.accessible ( fld->bdt ) <= 0 )
                        b = false;
                  
                  if (fld->building != NULL) {
                     if (fld->building->typ != bld)
                        b = false;
                     if (fld->building->getCompletion() == fld->building->typ->construction_steps - 1)
                        b = false;
                     if ( (entryfield->bdt & getTerrainBitType(cbbuildingentry) ).none() )
                        b = false;
                  }
                  if (entryfield->building != fld->building)
                     b = false;
               } else
                  b = false;
            }
      return b;
   }
   
   return false;
}


vector<MapCoordinate> ConstructBuildingCommand::getFields()
{
   vector<MapCoordinate> fields;
   Vehicle* veh = getUnit();
   BuildingType* bld = buildingTypeRepository.getObject_byID( buildingTypeID );
   if ( bld ) {
      for ( int d = 0; d < 6; ++d ) {
         MapCoordinate pos = getNeighbouringFieldCoordinate( veh->getPosition(), d );
         if ( buildingFits( pos ))
            fields.push_back( pos );
      }
   }

   return fields;
}

bool ConstructBuildingCommand :: isFieldUsable( const MapCoordinate& pos )
{
   vector<MapCoordinate> fields = getFields();
   return find( fields.begin(), fields.end(), pos ) != fields.end() ;
}


void ConstructBuildingCommand :: setTargetPosition( const MapCoordinate& pos )
{
   this->target = pos;
   tfield* fld = getMap()->getField(target);

   if ( !fld )
      throw ActionResult(21002);

   if ( buildingTypeID > 0  )
      setState( SetUp );

}



ActionResult ConstructBuildingCommand::go ( const Context& context )
{
   if ( getState() != SetUp )
      return ActionResult(21002);
   
   if ( !avail( getUnit() ))
      return ActionResult(22506);
   
   BuildingType* bld = buildingTypeRepository.getObject_byID( buildingTypeID );

   if ( !bld )
      return ActionResult( 22504 );
   
   Lack l = buildingProductionPrerequisites( bld );
   if ( !l.ok() )
      return ActionResult(22505);
   
   if ( !isFieldUsable( target ))
      return ActionResult(22503);
   
   
   
   auto_ptr<SpawnBuilding> sb ( new SpawnBuilding( getMap(), target, buildingTypeID, getUnit()->getOwner() ));
   ActionResult res = sb->execute( context );
   if ( res.successful() )
      sb.release();
   else
      return res;

   Resources cost = getProductionCost( bld );
   
   auto_ptr<ConsumeResource> cr ( new ConsumeResource( getUnit(), cost ));
   res = cr->execute( context );
   if ( res.successful() )
      cr.release();
   else
      return res;
   
   auto_ptr<ChangeUnitMovement> cum ( new ChangeUnitMovement( getUnit(), 0 ));
   res = cum->execute( context );
   if ( res.successful() )
      cum.release();
   else
      return res;

   auto_ptr<ChangeUnitProperty> cup ( new ChangeUnitProperty( getUnit(), ChangeUnitProperty::AttackedFlag, 1 ));
   res = cup->execute( context );
   if ( res.successful() )
      cup.release();
   else
      return res;
   
   if ( context.display )
      context.display->repaintDisplay();

   return ActionResult(0);
}

Building* ConstructBuildingCommand :: getProducedBuilding()
{
   return NULL;
}


static const int ConstructBuildingCommandVersion = 1;

      void ConstructBuildingCommand :: readData ( tnstream& stream )
{
   UnitCommand::readData( stream );
   int version = stream.readInt();
   if ( version > ConstructBuildingCommandVersion )
      throw tinvalidversion ( "ConstructBuildingCommand", ConstructBuildingCommandVersion, version );
   target.read( stream );
   buildingTypeID = stream.readInt();
}

void ConstructBuildingCommand :: writeData ( tnstream& stream ) const
{
   UnitCommand::writeData( stream );
   stream.writeInt( ConstructBuildingCommandVersion );
   target.write( stream );
   stream.writeInt( buildingTypeID );
}

void ConstructBuildingCommand :: setBuildingType( const BuildingType* type )
{
   buildingTypeID = type->id;
}


ASCString ConstructBuildingCommand :: getCommandString() const
{
   ASCString c;
   c.format("ConstructBuilding ( %d, %d, %d, %d )", getUnitID(), target.x, target.y, buildingTypeID );
   return c;
}

GameActionID ConstructBuildingCommand::getID() const
{
   return ActionRegistry::ConstructBuildingCommand;
}

ASCString ConstructBuildingCommand::getDescription() const
{
   ASCString s = "Construct ";

   const BuildingType* bt = getMap()->getbuildingtype_byid( buildingTypeID );
   if ( bt )
      s += bt->name;
   else
      s += "building";

   if ( getUnit() ) {
      s += " by " + getUnit()->getName();
   }
   s += " at " + target.toString();
   return s;
}

namespace
{
const bool r1 = registerAction<ConstructBuildingCommand> ( ActionRegistry::ConstructBuildingCommand );
}

