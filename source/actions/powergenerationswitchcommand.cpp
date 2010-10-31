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


#include "powergenerationswitchcommand.h"

#include "../vehicle.h"
#include "../mapfield.h"
#include "../gamemap.h"
#include "../viewcalculation.h"
#include "../spfst.h"
#include "../mapdisplayinterface.h"
#include "action-registry.h"
#include "changeunitproperty.h"


bool PowerGenerationSwitchCommand :: avail ( const Vehicle* unit, bool newState )
{
   if ( !unit )
      return false;

   if ( unit->getOwner() == unit->getMap()->actplayer  &&
         ( unit->typ->hasFunction( ContainerBaseType::MatterConverter  )))
      if ( unit->getGeneratorStatus() != newState )
         return true;
   
   return false;
}



PowerGenerationSwitchCommand :: PowerGenerationSwitchCommand ( Vehicle* container )
   : UnitCommand ( container )
{
}

void PowerGenerationSwitchCommand :: setNewState( bool enabled )
{
   newState = enabled;
   setState( SetUp );  
}


 

ActionResult PowerGenerationSwitchCommand::go ( const Context& context )
{
   if ( getState() != SetUp )
      return ActionResult(22000);

   Vehicle* unit = getUnit();
   if ( !avail( unit, newState ))
      return ActionResult( 22300 );
   
   auto_ptr<ChangeUnitProperty> propChange ( new ChangeUnitProperty( getUnit(), ChangeUnitProperty::PowerGeneration, newState ));
   ActionResult res = propChange->execute( context );
   
   if ( res.successful() ) 
      propChange.release();
   
   return res;
}



static const int PowerGenerationSwitchCommandVersion = 1;

void PowerGenerationSwitchCommand :: readData ( tnstream& stream )
{
   UnitCommand::readData( stream );
   int version = stream.readInt();
   if ( version > PowerGenerationSwitchCommandVersion )
      throw tinvalidversion ( "PowerGenerationSwitchCommand", PowerGenerationSwitchCommandVersion, version );
   newState = stream.readInt();
}

void PowerGenerationSwitchCommand :: writeData ( tnstream& stream ) const
{
   UnitCommand::writeData( stream );
   stream.writeInt( PowerGenerationSwitchCommandVersion );
   stream.writeInt( newState );
}


ASCString PowerGenerationSwitchCommand :: getCommandString() const
{
   ASCString c;
   c.format("unitPowerGenerationEnable ( map, %d, %d )", getUnitID(), newState );
   return c;

}

GameActionID PowerGenerationSwitchCommand::getID() const
{
   return ActionRegistry::PowerGenerationSwitchCommand;
}

ASCString PowerGenerationSwitchCommand::getDescription() const
{
   ASCString s = newState ? "Enable " : "Disable ";
   s += "power generation ";
   
   if ( getMap()->getUnit( getUnitID() ))
      s += " for " + getMap()->getUnit( getUnitID() )->getName();
   
   return s;
}

namespace
{
   const bool r1 = registerAction<PowerGenerationSwitchCommand> ( ActionRegistry::PowerGenerationSwitchCommand );
}

