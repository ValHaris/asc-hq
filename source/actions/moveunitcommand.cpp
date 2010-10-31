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

#include <iostream>

#include "moveunitcommand.h"

#include "../vehicle.h"
#include "../mapfield.h"
#include "../gamemap.h"
#include "../viewcalculation.h"
#include "../spfst.h"
#include "../mapdisplayinterface.h"
#include "vehicleattack.h"
#include "action-registry.h"
#include "moveunit.h"


bool MoveUnitCommand :: avail ( Vehicle* eht )
{
   
   if ( eht ) 
      return eht->canMove();
   
   return false;
}


bool MoveUnitCommand :: ascendAvail ( Vehicle* veh )
{
   if ( veh )
      if ( veh->getHeightChange( +1 ))
         return true;
   return false;
}

bool MoveUnitCommand :: descendAvail ( Vehicle* veh )
{
   if ( veh )
      if ( veh->getHeightChange( -1 ))
         return true;
   
   return false;
}


bool MoveUnitCommand :: longDistAvailable( const MapCoordinate& pos )
{
   MapField* fld = getMap()->getField(pos);
   if ( !fld )
      return false;
   
   if ( !getUnit() )
      return false;
   
   if ( pos == getUnit()->getPosition() )
      return false;

   for ( int h = 0; h < 8; ++h )
      if ( getUnit()->typ->height & (1<<h))
         if ( fieldAccessible( fld, getUnit(), 1 << h ) == 2 )
            return true;
   
   return false;

}



MoveUnitCommand :: MoveUnitCommand ( Vehicle* unit )
   : UnitCommand ( unit ), flags(0), verticalDirection(0), multiTurnMovement(false)
{
   
}


class HeightChangeLimitation: public AStar3D::OperationLimiter {
         bool allow_Height_Change;
      public:
         HeightChangeLimitation ( bool allow_Height_Change_ ) : allow_Height_Change ( allow_Height_Change_ ) {};
         virtual bool allowHeightChange() { return allow_Height_Change; };
         virtual bool allowMovement() { return true; };
         virtual bool allowEnteringContainer() { return true; };
         virtual bool allowLeavingContainer() { return true; };
         virtual bool allowDocking() { return true; };
};

class MovementLimitation: public AStar3D::OperationLimiter {
         bool simpleMode;
         int hcNum;
      public:
         MovementLimitation ( bool simpleMode_ ) : simpleMode ( simpleMode_ ), hcNum(0) {};
         virtual bool allowHeightChange() { ++hcNum; if ( simpleMode) return hcNum <= 1 ; else return true; };
         virtual bool allowMovement() { return !simpleMode; };
         virtual bool allowEnteringContainer() { return true; };
         virtual bool allowLeavingContainer() { return true; };
         virtual bool allowDocking() { return true; };
};

      
class PathFinder : public AStar3D {
   public:
      PathFinder ( Vehicle* veh, int maxDistance ) : AStar3D(veh->getMap(), veh, false, maxDistance ) {};

      /** searches for all fields that are within the range of maxDist and marks them.
         On each field one bit for each level of height will be set.
         The Destructor removes all marks.
      */
      void getMovementFields ( set<MapCoordinate3D>& reachableFields, set<MapCoordinate3D>& reachableFieldsIndirect, int height );
};
      
      
void PathFinder :: getMovementFields ( set<MapCoordinate3D>& reachableFields, set<MapCoordinate3D>& reachableFieldsIndirect, int height )
{
   Path dummy;
   findPath ( dummy, MapCoordinate3D(-1, -1, veh->height) );  //this field does not exist...

   int unitHeight = veh->getPosition().getNumericalHeight();
   if ( !this->actmap->getField ( veh->getPosition())->unitHere ( veh ))
      unitHeight = -1;

   // there are different entries for the same x/y coordinate but different height.
   // Since the UI is only in xy, we need to find the height which is the easiest to reach
   typedef multimap<MapCoordinate,Container::iterator > Fields;
   Fields fields;
   int orgHeight=-1;
   int minMovement = maxint;
   for ( Container::iterator i = visited.begin(); i != visited.end(); ++i ) {
      if ( i->h.x != veh->getPosition().x || i->h.y != veh->getPosition().y || i->h.getNumericalHeight() != unitHeight ) {
         int h = i->h.getNumericalHeight();
         // if ( h == -1 )
         //   h = i->enterHeight;
         if ( h == -1 || height == -1 || h == height ) {
            if ( i->canStop )
               fields.insert(make_pair(MapCoordinate(i->h),  i));
            else
               reachableFieldsIndirect.insert( i->h );
         }
      }
      if ( i->h.getNumericalHeight() >= 0 )
         if ( i->gval < minMovement ) {
            orgHeight = i->h.getNumericalHeight();
            minMovement = int (i->gval);
         }
   }
   for ( Fields::iterator i = fields.begin(); i != fields.end();  ) {
      int height = i->second->h.getNumericalHeight();
      int move = int(i->second->gval);
      Fields::key_type key = i->first;
      ++i;
      while ( i != fields.end() && i->first == key ) {
         if ( i->second->gval  < move || ( i->second->gval == move && abs(i->second->h.getNumericalHeight()-orgHeight) < abs(height-orgHeight) ))
            height = i->second->h.getNumericalHeight();
         ++i;
      }
      MapCoordinate3D f;
      f.setnum( key.x, key.y, height );
      reachableFields.insert ( f );
   }
}
      

ActionResult MoveUnitCommand::searchFields(int height, int capabilities)
{
   Vehicle* veh = getUnit();
   if ( !veh ) 
      return ActionResult(101);

   int h;
   if ( getMap()->getField(veh->getPosition())->unitHere(veh) )
      h = getFirstBit(veh->height); // != height-change: true
   else 
      h = -1; // height-change = false
      
   if ( height == -2  )
      h = -1;

   if ( (capabilities | flags) & LimitVerticalDirection ) {
      if ( getVerticalDirection() == 0 ) {
         HeightChangeLimitation hcl ( !(capabilities & DisableHeightChange) );
         PathFinder pf ( veh, veh->getMovement() );
         pf.registerOperationLimiter( &hcl );
         pf.getMovementFields ( reachableFields, reachableFieldsIndirect, h );
      } else {
         const VehicleType::HeightChangeMethod* hcm = veh->getHeightChange( getVerticalDirection() );
         if ( !hcm )
            fatalError ( "Inconsistent call to changeheight ");
   
         h = veh->getPosition().getNumericalHeight() + hcm->heightDelta;
         
         MovementLimitation ml ( capabilities & ShortestHeightChange );
         PathFinder pf ( veh, veh->getMovement() );
         pf.registerOperationLimiter( &ml );
         pf.getMovementFields ( reachableFields, reachableFieldsIndirect, h );
      }
   } else {
      PathFinder pf ( veh, veh->getMovement() );
      pf.getMovementFields ( reachableFields, reachableFieldsIndirect, -1 );
   }

   if ( reachableFields.size() == 0 ) 
      return ActionResult(107);
   
   setState( Evaluated );
   
   return ActionResult(0);
}



void MoveUnitCommand :: setDestination( const MapCoordinate3D& destination )
{
   this->destination = destination;
   setState( SetUp );
}

void MoveUnitCommand :: setDestination( const MapCoordinate& destination )
{
   if ( getState() != Evaluated )
      searchFields();
   
   bool found = false;
   for ( set<MapCoordinate3D>::iterator i = reachableFields.begin(); i != reachableFields.end(); ++i )
      if ( destination.x == i->x && destination.y == i->y ) {
         this->destination = *i;
         found = true;
         multiTurnMovement = false;
         break;
      }
      
   if ( !found ) {
      // assuming long dist movement
      MapField* fld  = getMap()->getField( destination );
      if ( !fld )
         return;
      
      if ( fld->getContainer() ) 
         this->destination.setnum( destination.x, destination.y, -1 );
      else
          this->destination = MapCoordinate3D( destination, getUnit()->getHeight() );
      
      
      multiTurnMovement = true;
   }
   
   setState( SetUp );
}

bool MoveUnitCommand::isFieldReachable( const MapCoordinate& pos, bool direct )
{
   if ( direct ) {
      for ( set<MapCoordinate3D>::iterator i = reachableFields.begin(); i != reachableFields.end(); ++i )
         if ( i->x == pos.x && i->y == pos.y )
            return true;
   } else {
      for ( set<MapCoordinate3D>::iterator i = reachableFieldsIndirect.begin(); i != reachableFieldsIndirect.end(); ++i )
         if ( i->x == pos.x && i->y == pos.y )
            return true;
   }
   return false;  
}

bool MoveUnitCommand::isFieldReachable3D( const MapCoordinate3D& pos, bool direct )
{
   AStar3D ast ( getMap(), getUnit(), false, getUnit()->getMovement() );
   AStar3D::Path localPath;
   ast.findPath ( localPath, pos );
   return !localPath.empty();
}


void MoveUnitCommand::calcPath()
{
   path.clear();
   {
      AStar3D ast ( getMap(), getUnit(), false, getUnit()->getMovement() );
      ast.findPath ( path, destination );
   }
   
   
   if ( path.empty() && multiTurnMovement ) {
      
      bool enterContainer = true;
      
      AStar3D::Path totalPath;
      AStar3D astar ( getMap(), getUnit(), false );
   
      astar.findPath ( totalPath, destination );
      if ( totalPath.empty() )
         return;
   
      AStar3D::Path::const_iterator pi = totalPath.begin();
      AStar3D::Path::const_iterator lastmatch = totalPath.begin();
      
      while ( pi != totalPath.end() ) {
         MapField* fld = getMap()->getField ( pi->x, pi->y );
         bool ok = true;
         if ( fld->getContainer() ) {
            if ( pi+1 !=totalPath.end() )
               ok = false;
            else {
               if ( fld->building && !enterContainer )
                  ok = false;
               if ( fld->vehicle && !enterContainer )
                  ok = false;
            }
         }

         if ( ok )
            if ( isFieldReachable3D(*pi, true) )
               lastmatch = pi;

         ++pi;
      }

      if ( lastmatch == totalPath.begin() )
         return;
  
      AStar3D ast ( getMap(), getUnit(), false, getUnit()->getMovement() );
      ast.findPath ( path, *lastmatch );
   }
}

const AStar3D::Path& MoveUnitCommand::getPath()
{
   return path;  
}


ActionResult MoveUnitCommand::go ( const Context& context )
{
   if ( getState() != SetUp )
      return ActionResult(22000);
   
   /*
   searchFields();
   
   if ( reachableFields.find( destination ) == reachableFields.end() ) 
      return ActionResult(105);
*/
   if ( !getUnit() )
      return ActionResult(21001);
   
   if (( getUnit()->getPosition() == destination) || (getUnit()->getPosition().x == destination.x && getUnit()->getPosition().y == destination.y && destination.getNumericalHeight() == -1  ))
      return ActionResult(22003);
   
   calcPath();
   
   if ( path.empty() ) 
      return ActionResult( 105 );
   
   if ( !multiTurnMovement && ( path.rbegin()->x != destination.x || path.rbegin()->y != destination.y  )) 
      return ActionResult( 105 );
   
   int nwid = getUnit()->networkid;

   if ( context.display )
      context.display->startAction();
   
   ActionResult res = (new MoveUnit(getUnit(), path, flags&NoInterrupt))->execute(context);
   
   if ( context.display )
      context.display->stopAction();


   int destDamage;
   if ( getMap()->getUnit( nwid ))
      destDamage = getUnit()->damage;
   else
      destDamage = 100;

   if ( res.successful() ) {
      if ( destDamage < 100 ) {
         if ( multiTurnMovement && getUnit()->getPosition3D() != destination )
            setState( Run );
         else
            setState( Finished );
      } else 
         setState( Finished );
   } else
      setState( Failed );
   
   return res;

}

void MoveUnitCommand :: setVerticalDirection( int dir )
{
   verticalDirection = dir;
   flags |= LimitVerticalDirection;  
}


static const int moveCommandVersion = 2;

void MoveUnitCommand :: readData ( tnstream& stream )
{
   UnitCommand::readData( stream );
   int version = stream.readInt();
   if ( version > moveCommandVersion )
      throw tinvalidversion ( "MoveCommand", moveCommandVersion, version );
   destination.read( stream );
   
   flags = stream.readInt( );
   verticalDirection = stream.readInt();
   if ( version >= 2 )
      multiTurnMovement = stream.readInt();
   else
      multiTurnMovement = false;
}

void MoveUnitCommand :: writeData ( tnstream& stream ) const
{
   UnitCommand::writeData( stream );
   stream.writeInt( moveCommandVersion );
   destination.write( stream );
   stream.writeInt( flags );
   stream.writeInt( verticalDirection );
   stream.writeInt( multiTurnMovement );
}

ASCString MoveUnitCommand :: getCommandString() const {
   ASCString c;
   c.format("unitMovement ( map, %d, asc.MapCoordinate( %d, %d), %d )", getUnitID(), destination.x, destination.y, destination.getNumericalHeight() );
   return c;
}

GameActionID MoveUnitCommand::getID() const 
{
   return ActionRegistry::MoveUnitCommand;   
}

ASCString MoveUnitCommand::getDescription() const
{
   ASCString s = "Move unit"; 
   if ( getUnit() ) {
      s += " " + getUnit()->getName();
   }
   s += " to " + destination.toString();
   return s;
}


int MoveUnitCommand::getCompletion()
{
   return 50;
}

bool MoveUnitCommand::operatable()
{
   return getUnit();
}

void MoveUnitCommand::rearm()
{
   if ( getState() == Finished || getState() == Run )
      setState( SetUp );
   
   deleteChildren();
}

vector<MapCoordinate> MoveUnitCommand::getCoordinates() const
{
   vector<MapCoordinate> pos;
   pos.push_back( destination );
   return pos;
}


namespace {
   const bool r1 = registerAction<MoveUnitCommand> ( ActionRegistry::MoveUnitCommand );
}
