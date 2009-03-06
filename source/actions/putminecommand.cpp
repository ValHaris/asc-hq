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


#include "putminecommand.h"

#include "../vehicle.h"
#include "../mapfield.h"
#include "../gamemap.h"
#include "../viewcalculation.h"
#include "../spfst.h"
#include "../mapdisplayinterface.h"
#include "action-registry.h"
#include "../itemrepository.h"
#include "../viewcalculation.h"

#include "changeunitmovement.h"
#include "spawnmine.h"
#include "consumeammo.h"
#include "removemine.h"

bool PutMineCommand :: avail ( Vehicle* unit )
{
   if ( unit )
      if (unit->getOwner() == unit->getMap()->actplayer)
         if (unit->typ->hasFunction( ContainerBaseType::PlaceMines ) )
            if ( !unit->attacked )
               return true;
   return false;
}


PutMineCommand :: PutMineCommand ( Vehicle* unit )
      : UnitCommand ( unit ), mode(Build)
{
   object = cmantitankmine;
   putMines = false;
   removeMines = false;

   weaponNum = -1;

   for ( int i = 0; i < unit->typ->weapons.count; i++)
      if ((unit->typ->weapons.weapon[i].getScalarWeaponType() == cwminen) && unit->typ->weapons.weapon[i].shootable() && (unit->typ->weapons.weapon[i].sourceheight & unit->height) ) {
         if ( !unit->typ->hasFunction( ContainerBaseType::NoMineRemoval ))
            removeMines = true;
         
         if (unit->ammo[i] > 0)
            putMines = true;
         
         weaponNum = i;
      }
}


vector<MapCoordinate> PutMineCommand::getFields()
{
   vector<MapCoordinate> fields = objectsRemovable;
   
   for ( map<MapCoordinate,vector<MineTypes> > ::iterator i = objectsCreatable.begin(); i != objectsCreatable.end(); ++i )
      if ( find(fields.begin(), fields.end(), i->first) == fields.end() )
         fields.push_back ( i->first );
   return fields;
}

const vector<MineTypes>& PutMineCommand::getCreatableMines( const MapCoordinate& pos )
{
   return objectsCreatable[pos];   
}

bool PutMineCommand::getRemovableMines( const MapCoordinate& pos )
{
   return find( objectsRemovable.begin(), objectsRemovable.end(), pos) != objectsRemovable.end();
}

ActionResult PutMineCommand::searchFields()
{
   if ( !getUnit() )
      return ActionResult(201);

   if (getUnit()->getMovement() < mineputmovedecrease) 
      return ActionResult( 119 );
   

   objectsCreatable.clear();
   objectsRemovable.clear();

   if ( weaponNum >= 0 ) 
      circularFieldIterator( getMap(), getUnit()->getPosition(), getUnit()->typ->weapons.weapon[weaponNum].maxdistance/10, getUnit()->typ->weapons.weapon[weaponNum].mindistance/10, FieldIterationFunctor( this, &PutMineCommand::fieldChecker ));

   if ( objectsRemovable.size() + objectsCreatable.size() ) {
      setState(Evaluated);
      return ActionResult(0);
   } else
      return ActionResult(21504);
}

void PutMineCommand :: fieldChecker( const MapCoordinate& pos )
{
   tfield* fld = getMap()->getField(pos);
   if ( !fld )
      return;
   
   if ( fld->vehicle || fld->building )
      return;
   
   if ( fieldvisiblenow( getMap()->getField(pos)) ) {
      
      if ( removeMines && !fld->mines.empty() && fld->getVisibility( getUnit()->getOwner() ) == visible_all)  {
         objectsRemovable.push_back( pos );
      }
      
      if (putMines && (fld->mines.empty() || fld->mineowner() == getUnit()->getOwner()) && fld->mines.size() < getMap()->getgameparameter ( cgp_maxminesonfield )) {
         if ( (fld->bdt & (getTerrainBitType( cbwater ).flip())).any() ) {
            objectsCreatable[pos].push_back( cmantipersonnelmine );
            objectsCreatable[pos].push_back( cmantitankmine );
         }
         if ( (fld->typ->art & getTerrainBitType(cbwater2)).any()
              || (fld->typ->art & getTerrainBitType(cbwater3)).any() )
            objectsCreatable[pos].push_back( cmmooredmine );
         
         if ( (fld->bdt & getTerrainBitType(cbwater0)).any()
                 || (fld->bdt & getTerrainBitType(cbwater1)).any()
                 || (fld->bdt & getTerrainBitType(cbwater2)).any()
                 || (fld->bdt & getTerrainBitType(cbwater3)).any() 
                 || (fld->bdt & getTerrainBitType(cbriver)).any() )
            objectsCreatable[pos].push_back( cmfloatmine );
         
      }
   }
}


void PutMineCommand :: setCreationTarget( const MapCoordinate& target, MineTypes mineType )
{
   tfield* fld = getMap()->getField(target);
   
   if( !fld )
      throw ActionResult(21002);
   
   this->target = target;
   this->object = mineType;

   mode = Build;
   
  
   setState( SetUp );
}

void PutMineCommand :: setRemovalTarget( const MapCoordinate& target )
{
   tfield* fld = getMap()->getField(target);
   
   if( !fld )
      throw ActionResult(21002);
   
   this->target = target;
   mode = Remove;
  
   setState( SetUp );
}


ActionResult PutMineCommand::go ( const Context& context )
{
   MapCoordinate targetPosition;

   if ( getState() != SetUp )
      return ActionResult(21002);

   searchFields();

   int strength = getUnit()->typ->weapons.weapon[weaponNum].maxstrength;
   
   ActionResult res(0);
   if ( mode == Build ) { 
      res = (new SpawnMine(getMap(), target, object, getUnit()->getOwner(), MineBasePunch[object-1] * strength / 64 ))->execute( context );
      
      computeview( getMap(), 0, false, &context );
            
      if ( res.successful() ) 
         res = (new ConsumeAmmo( getUnit(), getUnit()->typ->weapons.weapon[weaponNum].getScalarWeaponType(), weaponNum, 1 ))->execute(context);
   } else
      res = (new RemoveMine(getMap(), target, 0 ))->execute( context );
   
   
   if ( res.successful() )
      res = (new ChangeUnitMovement( getUnit(), mineputmovedecrease, true ))->execute(context);
   
   if ( res.successful() ) {
      setState( Completed );
   } else
      setState( Failed );
      
   if( context.display && fieldvisiblenow( getMap()->getField(target), context.viewingPlayer, getMap() ))
      context.display->displayMap();
   return res;

}

static const int putObjectCommandVersion = 1;

void PutMineCommand :: readData ( tnstream& stream )
{
   UnitCommand::readData( stream );
   int version = stream.readInt();
   if ( version > putObjectCommandVersion )
      throw tinvalidversion ( "PutMineCommand", putObjectCommandVersion, version );
   target.read( stream );
   object = (MineTypes) stream.readInt();
   mode = (Mode) stream.readInt();
   putMines = stream.readInt();
   removeMines = stream.readInt();;
   weaponNum = stream.readInt(); 
   
}

void PutMineCommand :: writeData ( tnstream& stream ) const
{
   UnitCommand::writeData( stream );
   stream.writeInt( putObjectCommandVersion );
   target.write( stream );
   stream.writeInt( object );
   stream.writeInt( mode );
   stream.writeInt( putMines );
   stream.writeInt( removeMines );
   stream.writeInt( weaponNum );
   
}

ASCString PutMineCommand :: getCommandString() const
{
   ASCString c;
   if ( mode == Build )
      c.format("putmine ( %d, %d, %d, %d )", getUnitID(), target.x, target.y, object );
   else
      c.format("removemine ( %d, %d, %d )", getUnitID(), target.x, target.y );
   return c;

}

GameActionID PutMineCommand::getID() const
{
   return ActionRegistry::PutMineCommand;
}

ASCString PutMineCommand::getDescription() const
{
   ASCString s = "Put Mine"; 
   if ( getUnit() ) {
      s += " by " + getUnit()->getName();
   }
   s += " on " + target.toString();
   return s;
}

namespace
{
const bool r1 = registerAction<PutMineCommand> ( ActionRegistry::PutMineCommand );
}

