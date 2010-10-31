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


#include "cargomovecommand.h"

#include "../vehicle.h"
#include "../gamemap.h"
#include "../mapdisplayinterface.h"
#include "action-registry.h"
#include "../containercontrols.h"
#include "unitfieldregistration.h"


bool CargoMoveCommand :: moveOutAvail( const Vehicle* movingUnit  )
{
   if ( !movingUnit )
      return false;
   
   ContainerBase* carr = movingUnit ->getCarrier();
   if ( carr ) {
      ContainerBase* carr2 = carr->getCarrier();
      if ( carr2 )
         return carr2->vehicleFit( movingUnit  );
   }
   return false;

}

bool CargoMoveCommand :: moveInAvail( const Vehicle* movingUnit , Vehicle* newCarrier )
{
   if ( movingUnit && newCarrier )
      return newCarrier->vehicleFit( movingUnit );  
   else
      return false;
}


vector<Vehicle*> CargoMoveCommand :: getTargetCarriers()
{
   vector<Vehicle*> targets;
   
   ContainerBase* container = getUnit()->getCarrier();
   if ( !container )
      return targets;
   
   for ( ContainerBase::Cargo::const_iterator i = container->getCargo().begin(); i != container->getCargo().end(); ++i )
      if ( *i != getUnit() && *i )
         if ( moveInAvail ( getUnit(), *i ))
            targets.push_back( *i );

   return targets;
}


CargoMoveCommand :: CargoMoveCommand ( Vehicle* unit )
   : UnitCommand ( unit ), mode( undefined ), targetCarrier(-1)
{

}



 

ActionResult CargoMoveCommand::go ( const Context& context )
{
   if ( getState() != SetUp )
      return ActionResult(22000);

   if( mode == undefined )
      return ActionResult(21900);
   
   if ( mode == moveOutwards ) {
      if ( !moveOutAvail( getUnit() ) )
         return ActionResult( 21901 );
           
      Vehicle* targetContainer1 = dynamic_cast<Vehicle*>(getUnit()->getCarrier());
      if ( !targetContainer1 )
         return ActionResult( 21902 );
      
      ContainerBase* targetContainer2 = targetContainer1->getCarrier();
      if ( !targetContainer2 )
         return ActionResult( 21902 );
      
      ActionResult res = (new UnitFieldRegistration( getUnit(), getUnit()->getPosition(), UnitFieldRegistration::UnRegisterFromCarrier, getUnit()->getCarrier() ))->execute ( context );
      if ( !res.successful() )
         return res;
      
      res = (new UnitFieldRegistration( getUnit(), getUnit()->getPosition(), UnitFieldRegistration::RegisterInCarrier, targetContainer2 ))->execute ( context );
      return res;
      
   } else {
      Vehicle* targetContainer = getMap()->getUnit( targetCarrier );
      if ( !targetContainer )
         return ActionResult( 21902 );
      
      if ( !moveInAvail( getUnit(), targetContainer ) )
         return ActionResult( 21901 );
      
      ActionResult res = (new UnitFieldRegistration( getUnit(), getUnit()->getPosition(), UnitFieldRegistration::UnRegisterFromCarrier, getUnit()->getCarrier() ))->execute ( context );
      if ( !res.successful() )
         return res;
      
      res = (new UnitFieldRegistration( getUnit(), getUnit()->getPosition(), UnitFieldRegistration::RegisterInCarrier, targetContainer ))->execute ( context );
      return res;
   }
   
}

void CargoMoveCommand :: setMode( Mode mode ) 
{ 
   this->mode = mode; 
   if ( mode == moveOutwards || targetCarrier > 0 )
      setState( SetUp );
};


static const int CargoMoveCommandVersion = 1;

void CargoMoveCommand :: readData ( tnstream& stream )
{
   UnitCommand::readData( stream );
   int version = stream.readInt();
   if ( version > CargoMoveCommandVersion )
      throw tinvalidversion ( "CargoMoveCommand", CargoMoveCommandVersion, version );
   targetCarrier = stream.readInt();
   mode = (Mode) stream.readInt();
}

void CargoMoveCommand :: writeData ( tnstream& stream ) const
{
   UnitCommand::writeData( stream );
   stream.writeInt( CargoMoveCommandVersion );
   stream.writeInt( targetCarrier );
   stream.writeInt( mode );
}


void CargoMoveCommand :: setTargetCarrier( Vehicle* targetCarrier )
{
   if ( targetCarrier ) {
      this->targetCarrier = targetCarrier->networkid;
      if ( mode == moveInwards )
         setState( SetUp );
   } else
      this->targetCarrier = -1;
}


ASCString CargoMoveCommand :: getCommandString() const
{
   if ( mode == moveOutwards ) {
      ASCString c;
      c.format("cargoUnitMove ( map, %d, -1 )", getUnitID() );
      return c;
   }
   if ( mode == moveInwards ) {
      ASCString c;
      c.format("cargoUnitMove ( map, %d, %d )", getUnitID(), targetCarrier );
      return c;
   }
   return "";
}

GameActionID CargoMoveCommand::getID() const
{
   return ActionRegistry::CargoMoveCommand;
}

ASCString CargoMoveCommand::getDescription() const
{
   ASCString s = "Move ";
   if ( getUnit() )
      s += getUnit()->getName() ;
   else 
      s += "unit nwid " + ASCString::toString(getUnitID() );
   
   if ( mode == moveOutwards )
      s += " into outer carrier ";
   else
      s += " into inner carrier with ID " + ASCString::toString( targetCarrier );
   
   if ( getUnit() )
      s += " at " + getUnit()->getPosition().toString();
   
   return s;
}

namespace
{
   const bool r1 = registerAction<CargoMoveCommand> ( ActionRegistry::CargoMoveCommand );
}

