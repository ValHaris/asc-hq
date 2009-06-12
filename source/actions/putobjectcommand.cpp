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


#include "putobjectcommand.h"

#include "../vehicle.h"
#include "../mapfield.h"
#include "../gamemap.h"
#include "../viewcalculation.h"
#include "../spfst.h"
#include "../mapdisplayinterface.h"
#include "action-registry.h"
#include "../itemrepository.h"

#include "removeobject.h"
#include "spawnobject.h"
#include "consumeresource.h"
#include "changeunitmovement.h"

bool PutObjectCommand :: avail ( Vehicle* eht )
{

   if ( eht )
      if (eht->getOwner() == eht->getMap()->actplayer)
         if ( eht->typ->objectsBuildable.size() || eht->typ->objectsRemovable.size() || eht->typ->objectGroupsBuildable.size() || eht->typ->objectGroupsRemovable.size())
            if ( !eht->attacked )
               return true;
   return false;
}


PutObjectCommand :: PutObjectCommand ( Vehicle* unit )
      : UnitCommand ( unit ), object(-1),mode(Build)
{

}


vector<MapCoordinate> PutObjectCommand::getFields()
{
   vector<MapCoordinate> fields;
   for ( map<MapCoordinate,vector<int> > ::iterator i = objectsCreatable.begin(); i != objectsCreatable.end(); ++i )
      fields.push_back ( i->first );
   
   for ( map<MapCoordinate,vector<int> > ::iterator i = objectsRemovable.begin(); i != objectsRemovable.end(); ++i )
      if ( find(fields.begin(), fields.end(), i->first) == fields.end() )
         fields.push_back ( i->first );
   return fields;
}

const vector<int>& PutObjectCommand::getCreatableObjects( const MapCoordinate& pos )
{
   return objectsCreatable[pos];   
}

const vector<int>& PutObjectCommand::getRemovableObjects( const MapCoordinate& pos )
{
   return objectsRemovable[pos];
}

ActionResult PutObjectCommand::searchFields()
{
   if ( !getUnit() )
      return ActionResult(201);


   objectsCreatable.clear();
   objectsRemovable.clear();

   circularFieldIterator( getMap(), getUnit()->getPosition(), 1, 1, FieldIterationFunctor( this, &PutObjectCommand::fieldChecker ));

   if ( objectsRemovable.size() + objectsCreatable.size() ) {
      setState(Evaluated);
      return ActionResult(0);
   } else
      return ActionResult(21504);
}

bool PutObjectCommand::checkObject( tfield* fld, ObjectType* objtype, Mode mode )
{
    if ( !objtype || !fld )
       return false;

    Vehicle* veh = getUnit();
    
    if ( mode == Build ) {
       if ( objtype->getFieldModification(fld->getWeather()).terrainaccess.accessible( fld->bdt ) > 0
            &&  !fld->checkForObject ( objtype )
            && objtype->techDependency.available(getMap()->player[veh->getOwner()].research) ){
//            && !getheightdelta ( log2( actvehicle->height), log2(objtype->getEffectiveHeight())) ) {

          if ( veh->getResource(objtype->buildcost, true) == objtype->buildcost && veh->getMovement() >= objtype->build_movecost )
             return true;
       }
    } else {
       if ( fld->checkForObject ( objtype ) ) {
//          &&  !getheightdelta ( log2( actvehicle->height), log2(objtype->getEffectiveHeight())) ) {
          Resources r = objtype->removecost;
          for ( int i = 0; i <3; ++i )
             if ( r.resource(i) < 0 )
                r.resource(i) = 0;

          if ( veh->getResource(r, true) == r && veh->getMovement() >= objtype->remove_movecost )
             return true;
       }
    }
    return false;
}


void PutObjectCommand :: fieldChecker( const MapCoordinate& pos )
{
   tfield* fld = getMap()->getField(pos);
   if ( !fld )
      return;
   
   if ( fld->vehicle || fld->building )
      return;
   
   if ( fieldvisiblenow( getMap()->getField(pos)) ) {
      
      Vehicle* veh = getUnit();
      
      for ( int i = 0; i < veh->typ->objectsBuildable.size(); i++ )
         for ( int j = veh->typ->objectsBuildable[i].from; j <= veh->typ->objectsBuildable[i].to; j++ )
            if ( checkObject( fld, getMap()->getobjecttype_byid ( j ), Build ))
               objectsCreatable[pos].push_back( j );


      for ( int i = 0; i < veh->typ->objectGroupsBuildable.size(); i++ )
         for ( int j = veh->typ->objectGroupsBuildable[i].from; j <= veh->typ->objectGroupsBuildable[i].to; j++ )
            for ( int k = 0; k < objectTypeRepository.getNum(); k++ ) {
               ObjectType* objtype = objectTypeRepository.getObject_byPos ( k );
               if ( objtype->groupID == j )
                  if ( checkObject( fld, objtype, Build ))
                     if ( find ( objectsCreatable[pos].begin(), objectsCreatable[pos].end(), objtype->id) == objectsCreatable[pos].end() )
                        objectsCreatable[pos].push_back( objtype->id );
            }

      for ( int i = 0; i < veh->typ->objectsRemovable.size(); i++ )
         for ( int j = veh->typ->objectsRemovable[i].from; j <= veh->typ->objectsRemovable[i].to; j++ )
            if ( checkObject( fld, getMap()->getobjecttype_byid ( j ), Remove ))
               objectsRemovable[pos].push_back( j );


      for ( int i = 0; i < veh->typ->objectGroupsRemovable.size(); i++ )
         for ( int j = veh->typ->objectGroupsRemovable[i].from; j <= veh->typ->objectGroupsRemovable[i].to; j++ )
            for ( int k = 0; k < objectTypeRepository.getNum(); k++ ) {
               ObjectType* objtype = objectTypeRepository.getObject_byPos ( k );
               if ( objtype->groupID == j )
                  if ( checkObject( fld, objtype, Remove ))
                     if ( find ( objectsRemovable[pos].begin(), objectsRemovable[pos].end(), objtype->id) == objectsRemovable[pos].end() )
                        objectsRemovable[pos].push_back( objtype->id );
            }

   }
}


void PutObjectCommand :: setTarget( const MapCoordinate& target, int objectID )
{

//  if ( getState() == Evaluated ) {
   this->target = target;
   this->object = objectID;

   tfield* fld = getMap()->getField(target);
   
   if( !fld )
      throw ActionResult(21002);
   
   ObjectType* obj = getMap()->getobjecttype_byid( objectID);
   if( !obj )
      throw ActionResult(21501);
      
   if ( !fld->checkForObject( obj ))
      mode = Build;
   else
      mode = Remove;
   
   setState( SetUp );
//  }
}

ActionResult PutObjectCommand::go ( const Context& context )
{
   MapCoordinate targetPosition;

   if ( getState() != SetUp )
      return ActionResult(22000);

   if ( !avail( getUnit() ) )
      return ActionResult(21506);
   
   searchFields();

   vector<int>* objects;
   if ( mode == Build ) {
      objects = &objectsCreatable[target];
   } else {
      objects = &objectsRemovable[target];
   }
   
   if ( find( objects->begin(), objects->end(), object ) == objects->end() )
      return ActionResult( 21507 );
   
   
   ObjectType* obj = getMap()->getobjecttype_byid( object );
   
   RecalculateAreaView rav ( getMap(), target, maxViewRange / maxmalq + 1, &context );
      

   bool objectAffectsVisibility = obj->basicjamming_plus || obj->viewbonus_plus || obj->viewbonus_abs != -1 || obj->basicjamming_abs != -1;
   if ( objectAffectsVisibility )
      rav.removeView();
   
   ActionResult res(0);
   if ( mode == Build ) 
      res = (new SpawnObject(getMap(), target, object ))->execute( context );
   else
      res = (new RemoveObject(getMap(), target, object ))->execute( context );
   
   
   if ( objectAffectsVisibility )
      rav.addView();
   
   if ( res.successful() ) 
      res = (new ConsumeResource( getUnit(), mode==Build ? obj->buildcost : obj->removecost ))->execute(context);
   
   if ( res.successful() )
      res = (new ChangeUnitMovement( getUnit(), mode==Build ? obj->build_movecost : obj->remove_movecost, true ))->execute(context);
   
   if ( res.successful() ) {
      if ( context.display )
         context.display->repaintDisplay();
      setState( Completed );
   } else
      setState( Failed );
   return res;

}

static const int putObjectCommandVersion = 1;

void PutObjectCommand :: readData ( tnstream& stream )
{
   UnitCommand::readData( stream );
   int version = stream.readInt();
   if ( version > putObjectCommandVersion )
      throw tinvalidversion ( "PutObjectCommand", putObjectCommandVersion, version );
   target.read( stream );
   object = stream.readInt();
   mode = (Mode) stream.readInt();
}

void PutObjectCommand :: writeData ( tnstream& stream ) const
{
   UnitCommand::writeData( stream );
   stream.writeInt( putObjectCommandVersion );
   target.write( stream );
   stream.writeInt( object );
   stream.writeInt( mode );
}

ASCString PutObjectCommand :: getCommandString() const
{
   ASCString c;
   if ( mode == Build )
      c.format("unitPutObject ( map, %d, asc.MapCoordinate( %d, %d), %d )", getUnitID(), target.x, target.y, object );
   else
      c.format("unitRemoveObject ( map, %d, asc.MapCoordinate( %d, %d), %d )", getUnitID(), target.x, target.y, object );
   return c;

}

GameActionID PutObjectCommand::getID() const
{
   return ActionRegistry::PutObjectCommand;
}

ASCString PutObjectCommand::getDescription() const
{
   ASCString s;
   
   if ( mode == Build )
      s = "Put ";
   else
      s = "Remove ";
   
   const ObjectType* obj = getMap()->getobjecttype_byid( object );
   if ( obj )
      s += obj->name;
   else
      s += "object";
   
   if ( getUnit() ) {
      s += " with " + getUnit()->getName();
   }
   s += " at " + target.toString();
   return s;
}

namespace
{
   const bool r1 = registerAction<PutObjectCommand> ( ActionRegistry::PutObjectCommand );
}

