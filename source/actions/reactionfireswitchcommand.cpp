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


#include "reactionfireswitchcommand.h"

#include "../vehicle.h"
#include "../mapfield.h"
#include "../gamemap.h"
#include "../viewcalculation.h"
#include "../spfst.h"
#include "../mapdisplayinterface.h"
#include "action-registry.h"
#include "changeunitproperty.h"
#include "changeunitmovement.h"


bool ReactionFireSwitchCommand :: avail ( const Vehicle* unit, bool newState )
{
   if ( !unit )
      return false;

   if ( newState == true ) {
      if ( unit->getMap()->getField( unit->getPosition() )->getContainer() == unit ) 
         if ( !unit->baseType->hasFunction(ContainerBaseType::NoReactionfire ))
            if ( unit->reactionfire.getStatus() == Vehicle::ReactionFire::off )
               if ( unit->weapexist() )
                  for ( int i = 0; i < unit->typ->weapons.count; ++i )
                     if ( unit->typ->weapons.weapon[i].offensive() && unit->typ->weapons.weapon[i].reactionFireShots )
                        return true;
   } else {
      if ( unit->reactionfire.getStatus() != Vehicle::ReactionFire::off )
         return true;         
   }
   
   return false;
}



ReactionFireSwitchCommand :: ReactionFireSwitchCommand ( Vehicle* container )
   : UnitCommand ( container )
{
}

void ReactionFireSwitchCommand :: setNewState( bool enabled )
{
   newRFstate = enabled;
   setState( SetUp );  
}


 

ActionResult ReactionFireSwitchCommand::go ( const Context& context )
{
   if ( getState() != SetUp )
      return ActionResult(21002);

   Vehicle* unit = getUnit();
   if ( !avail( unit, newRFstate ))
      return ActionResult( 22300 );
   
   
   if ( newRFstate == true ) {
         
      auto_ptr<ChangeUnitProperty> propChange ( new ChangeUnitProperty( getUnit(), ChangeUnitProperty::ReactionFire, (int) Vehicle::ReactionFire::init2 ));
      ActionResult res = propChange->execute( context );
      
      if ( res.successful() ) 
         propChange.release();
      
      return res;
   } else {
      if (     unit->reactionfire.getStatus() != Vehicle::ReactionFire::init1a 
            && unit->reactionfire.getStatus() != Vehicle::ReactionFire::init2 
            && !unit->typ->hasFunction(ContainerBaseType::MoveWithReactionFire)  ) {
         
         auto_ptr<ChangeUnitMovement> propChange ( new ChangeUnitMovement( getUnit(), 0 ));
         ActionResult res = propChange->execute( context );
      
         if ( res.successful() ) 
            propChange.release();
         
      }
      
      auto_ptr<ChangeUnitProperty> propChange ( new ChangeUnitProperty( getUnit(), ChangeUnitProperty::ReactionFire, (int) Vehicle::ReactionFire::off ));
      ActionResult res = propChange->execute( context );
      
      if ( res.successful() ) 
         propChange.release();
      
      return res;
   }
}



static const int ReactionFireSwitchCommandVersion = 1;

void ReactionFireSwitchCommand :: readData ( tnstream& stream )
{
   UnitCommand::readData( stream );
   int version = stream.readInt();
   if ( version > ReactionFireSwitchCommandVersion )
      throw tinvalidversion ( "ReactionFireSwitchCommand", ReactionFireSwitchCommandVersion, version );
   newRFstate = stream.readInt();
}

void ReactionFireSwitchCommand :: writeData ( tnstream& stream ) const
{
   UnitCommand::writeData( stream );
   stream.writeInt( ReactionFireSwitchCommandVersion );
   stream.writeInt( newRFstate );
}


ASCString ReactionFireSwitchCommand :: getCommandString() const
{
   ASCString c;
   c.format("SetReactionFire ( %d, %d )", getUnitID(), newRFstate );
   return c;

}

GameActionID ReactionFireSwitchCommand::getID() const
{
   return ActionRegistry::ReactionFireSwitchCommand;
}

ASCString ReactionFireSwitchCommand::getDescription() const
{
   ASCString s = newRFstate ? "Enable " : "Disable ";
   s += "reaction fire ";
   
   if ( getMap()->getUnit( getUnitID() ))
      s += " for " + getMap()->getUnit( getUnitID() )->getName();
   
   return s;
}

namespace
{
   const bool r1 = registerAction<ReactionFireSwitchCommand> ( ActionRegistry::ReactionFireSwitchCommand );
}

