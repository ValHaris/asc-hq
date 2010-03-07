/*
     This file is part of Advanced Strategic Command; http://www.asc-hq.org
     Copyright (C) 1994-2009  Martin Bickel
 
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


#include "moveunittask.h"

#include "../spfst.h"
#include "../astar2.h"
#include "../actions/moveunitcommand.h"

#include "taskids.h"

TaskIdentifier MoveUnitTask::getID() const
{
   return TaskID::MoveUnit;
}
            

bool MoveUnitTask::available( const Vehicle* unit, const MapCoordinate& destination )
{
   if ( !unit )
      return false;
   
   MapField* fld = unit->getMap()->getField(destination);
   if ( !fld )
      return false;
   
   return fieldAccessible( fld, unit ) == 2;
}


MoveUnitTask::MoveUnitTask( Vehicle* unit, const MapCoordinate3D& destination )
   : UnitTask( unit ), enterContainer(false), allowInterrupts(true)
{
   this->destination = destination;
   setState( SetUp );
}

void MoveUnitTask::readData ( tnstream& stream )
{
   UnitTask::read( stream );
   destination.read( stream );
}

void MoveUnitTask::writeData ( tnstream& stream )
{
   UnitTask::write( stream );
   destination.write( stream );
}

ActionResult MoveUnitTask::run (const Context& context ) 
{
   
   if ( !available( getUnit(), destination )) 
      return ActionResult(30001);  
   
   
   AStar3D::Path path;
   AStar3D astar ( getMap(), getUnit(), false );
   
   astar.findPath ( path, destination );

   auto_ptr<MoveUnitCommand> mum ( new MoveUnitCommand( getUnit() ));
   mum->searchFields();
   
   AStar3D::Path::const_iterator pi = path.begin();
   if ( pi == path.end() )
      return ActionResult( 30003 );

   AStar3D::Path::const_iterator lastmatch = pi;
   while ( pi != path.end() ) {
      MapField* fld = getMap()->getField ( pi->x, pi->y );
      bool ok = true;
      if ( fld->getContainer() ) {
         if ( pi+1 !=path.end() )
            ok = false;
         else {
            if ( fld->building && !enterContainer )
               ok = false;
            if ( fld->vehicle && !enterContainer )
               ok = false;
         }
      }

      if ( ok )
         if ( mum->isFieldReachable3D(*pi, true) )
            lastmatch = pi;

      ++pi;
   }

   if ( lastmatch == path.begin() )
      return ActionResult( 30004 );
  
   mum->setDestination( *lastmatch );
   if ( !allowInterrupts )
      mum->setFlags( MoveUnitCommand::NoInterrupt );
   
   ActionResult res = mum->execute( context );
   if ( res.successful() ) {
      mum.release();
      
      Vehicle* v = getUnit();
      if ( !v )
         return ActionResult( 30005 );
      else {
         if ( v->getPosition() == destination )
            setState( Completed );
      }
   }  
   
   return res;
};
