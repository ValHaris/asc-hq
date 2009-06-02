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


#include "jumpdrivecommand.h"

#include "../vehicle.h"
#include "../mapfield.h"
#include "../gamemap.h"
#include "../viewcalculation.h"
#include "../spfst.h"
#include "changeunitmovement.h"
#include "../mapdisplayinterface.h"
#include "action-registry.h"
#include "../reactionfire.h"
#include "../soundList.h"
#include "consumeresource.h"
#include "servicecommand.h"
#include "unitfieldregistration.h"

bool JumpDriveCommand :: avail ( const Vehicle* unit )
{
   if ( unit )
      if ( !unit->hasMoved() )
         if ( unit->height & unit->typ->jumpDrive.height )
            if ( unit->getResource( unit->typ->jumpDrive.consumption ) == unit->typ->jumpDrive.consumption ) 
               return true;

   return false;
}


JumpDriveCommand :: JumpDriveCommand ( Vehicle* unit)
   : UnitCommand ( unit )
{

}


bool JumpDriveCommand::fieldReachable( const MapCoordinate& dest )
{
   tfield* fld = getMap()->getField( dest );
   if ( beeline( dest, getUnit()->getPosition()) <= getUnit()->typ->jumpDrive.maxDistance )
      if ( !fld->vehicle && !fld->building ) 
         if ( fieldvisiblenow( fld, getUnit()->getOwner() ))
            if ( getUnit()->typ->jumpDrive.targetterrain.accessible( fld->bdt ) > 0 ) 
               return true;
   
   return false;
}


vector<MapCoordinate> JumpDriveCommand::getDestinations()
{
   vector<MapCoordinate> fields;
   
   if ( !avail( getUnit() ))
      return fields;
     
   GameMap* gamemap = getMap();
    
   for ( int y = 0; y < gamemap->ysize; ++y )
      for (int x = 0; x < gamemap->xsize; ++x ) {
         MapCoordinate dest (x,y); 
         if ( fieldReachable( dest)) 
            fields.push_back( dest );
         
      }
  return fields;
}

void JumpDriveCommand::setDestination( const MapCoordinate& position )
{
   if ( fieldReachable( position )) {
      destination = position;
      setState( SetUp );
   }
}


ActionResult JumpDriveCommand::go ( const Context& context )
{
   if ( getState() != SetUp )
      return ActionResult(22000);

   Vehicle* unit = getUnit();
   
   if ( !avail( unit ))
      return ActionResult( 22600 );
   
   if ( !fieldReachable( destination ))
      return ActionResult( 22601 );
   
   
   auto_ptr<ConsumeResource> cr ( new ConsumeResource( unit, unit->typ->jumpDrive.consumption ));
   ActionResult res = cr->execute( context );
   if ( !res.successful() )
      return res;
   else
      cr.release();
   
   auto_ptr<UnitFieldRegistration> ufr1 ( new UnitFieldRegistration( unit, unit->getPosition(), UnitFieldRegistration::RemoveView ));
   res = ufr1->execute( context );
   if ( !res.successful() )
      return res;
   else
      ufr1.release();
   
   auto_ptr<UnitFieldRegistration> ufr2 ( new UnitFieldRegistration( unit, unit->getPosition(), UnitFieldRegistration::UnregisterOnField ));
   res = ufr2->execute( context );
   if ( !res.successful() )
      return res;
   else
      ufr2.release();
   
   
   MapCoordinate3D dest3D (destination, unit->height );

   tsearchreactionfireingunits srfu( getMap() );
   srfu.init( unit , dest3D );

   if ( context.display )
      context.display->playPositionalSound( dest3D, SoundList::getInstance().getSound( SoundList::jumpdrive ));


   srfu.checkfield( dest3D, unit, context );
   srfu.finalCheck( unit->getOwner(), context );


   // the unit may have been shot down by RF, so we need to check if it still exists
   Vehicle* subject = getUnit();

   if ( subject ) {
      auto_ptr<UnitFieldRegistration> ufr3( new UnitFieldRegistration( unit, dest3D, UnitFieldRegistration::Position ));
      res = ufr3->execute( context );
      if ( !res.successful() )
         return res;
      else
         ufr3.release();
      
      auto_ptr<UnitFieldRegistration> ufr4 ( new UnitFieldRegistration( unit, dest3D, UnitFieldRegistration::RegisterOnField ));
      res = ufr4->execute( context );
      if ( !res.successful() )
         return res;
      else
         ufr4.release();
      
      
      auto_ptr<ChangeUnitMovement> cum ( new ChangeUnitMovement( unit, 0 ));
      res = cum->execute( context );
      if ( !res.successful() )
         return res;
      else
         cum.release();
      
      auto_ptr<UnitFieldRegistration> ufr5( new UnitFieldRegistration( unit, dest3D, UnitFieldRegistration::AddView ));
      res = ufr5->execute( context );
      if ( !res.successful() )
         return res;
      else
         ufr5.release();
   }
   evaluateviewcalculation( getMap(), 0, false, &context );
   
   if ( context.display )
      context.display->repaintDisplay();
   
   if ( res.successful() )
      setState( Completed );
   else
      setState( Failed );
   
   return res;
}



static const int JumpDriveCommandVersion = 1;

void JumpDriveCommand :: readData ( tnstream& stream )
{
   UnitCommand::readData( stream );
   int version = stream.readInt();
   if ( version > JumpDriveCommandVersion )
      throw tinvalidversion ( "JumpDriveCommand", JumpDriveCommandVersion, version );
   destination.read( stream );   
}

void JumpDriveCommand :: writeData ( tnstream& stream ) const
{
   UnitCommand::writeData( stream );
   stream.writeInt( JumpDriveCommandVersion );
   destination.write( stream );
}


ASCString JumpDriveCommand :: getCommandString() const
{
   ASCString c;
   c.format("unitJump ( map, %d, asc.MapCoordinate( %d, %d ) )", getUnitID(), destination.x, destination.y );
   return c;

}

GameActionID JumpDriveCommand::getID() const
{
   return ActionRegistry::JumpDriveCommand;
}

ASCString JumpDriveCommand::getDescription() const
{
   ASCString s = "Jump ";
   
   if ( getUnit())
      s += getUnit()->getName();
   
   s += " to " + destination.toString();
   return s;
}

namespace
{
   const bool r1 = registerAction<JumpDriveCommand> ( ActionRegistry::JumpDriveCommand );
}

