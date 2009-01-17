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


#include "unitfieldregistration.h"

#include "../vehicle.h"
#include "../gamemap.h"
#include "../viewcalculation.h"
     
UnitFieldRegistration::UnitFieldRegistration( Vehicle* vehicle, const MapCoordinate3D& pos, Operation operation, ContainerBase* carrier )
   : UnitAction( vehicle->getMap(), vehicle->networkid)
{
   this->operation= operation;
   this->position = pos;
   if ( carrier )
      carrierID = carrier->getIdentification();
   else
      carrierID = 0; 
}
      
      
ASCString UnitFieldRegistration::getOpName() const 
{
   switch ( operation ) {
      case RegisterOnField: return "RegisterOnField";
      case UnregisterOnField: return "UnregisterOnField";
      case AddView: return "AddView";
      case RemoveView: return "RemoveView"; 
      case Position: return "Position";
      case Position3D: return "Position3D";
      case RegisterInCarrier: return "CarrierRegistration";
   }
   return "";
}
      
ASCString UnitFieldRegistration::getDescription() const
{
   ASCString res = getOpName();
   if ( getUnit(false) ) 
      res += " " + getUnit(false)->getName();

   if ( position.valid() )
      res += " " + position.toString();
   
   return  res;
}
      
static const int unitFieldRegistrationVersion = 2;
      
void UnitFieldRegistration::readData ( tnstream& stream ) 
{
   UnitAction::readData( stream );
   int version = stream.readInt();
   if ( version < 1 || version > unitFieldRegistrationVersion )
      throw tinvalidversion ( "UnitFieldRegistration", unitFieldRegistrationVersion, version );
   
   operation = (Operation) stream.readInt();
   position.read( stream );
   previousPosition.read( stream );
   stream.readInt();
   stream.readInt();
   
   if ( version >= 2 )
      carrierID = stream.readInt();
   else
      carrierID = 0;
};
      
      
void UnitFieldRegistration::writeData ( tnstream& stream ) const
{
   UnitAction::writeData( stream );
   stream.writeInt( unitFieldRegistrationVersion );
   stream.writeInt( (int) operation );
   position.write( stream );
   previousPosition.write( stream );
   stream.writeInt( 0 ); // dummy
   stream.writeInt( 0 ); // dummy
   stream.writeInt( carrierID );
};


GameActionID UnitFieldRegistration::getID() const
{
   return ActionRegistry::UnitFieldRegistration;
}

ActionResult UnitFieldRegistration::runAction( const Context& context )
{
   Vehicle* veh = getUnit();
   
   tfield* fld = getMap()->getField( position );
   
   switch ( operation ) {
      case RegisterOnField: 
         if ( fld->getContainer() )
            fld->getContainer()->addToCargo( veh );
         else 
            fld->vehicle = veh;
         break;
         
      case RegisterInCarrier:
      {
         ContainerBase* carrier = getMap()->getContainer( carrierID );
         if ( !carrier )
            throw ActionResult( 21303 );
         carrier->addToCargo( veh );
         break;
      }
      
      case UnRegisterFromCarrier:
      {
         ContainerBase* carrier = getMap()->getContainer( carrierID );
         if ( !carrier )
            throw ActionResult( 21303 );
         if ( !carrier->removeUnitFromCargo( veh ))
            throw ActionResult( 21302, veh );
         break;
      }
         
      case UnregisterOnField:
         if ( fld->getContainer() && fld->getContainer() != veh ) {
            if ( !fld->getContainer()->removeUnitFromCargo( veh ))
               throw ActionResult( 21302, veh );
         } else {
            if ( fld->vehicle != veh )
               throw ActionResult( 21301, veh );
            fld->vehicle = NULL;
         }
         break;
            
      case AddView:
         veh->addview();
         break;
         
      case RemoveView:
         veh->removeview();
         break;
      
      case Position3D:
         previousPosition = veh->getPosition();
         veh->xpos = position.x;
         veh->ypos = position.y;
         veh->height = position.getBitmappedHeight();
         break;
         
      case Position:
         previousPosition = veh->getPosition();
         veh->xpos = position.x;
         veh->ypos = position.y;
         break;
   }
   
   return ActionResult(0);
}


ActionResult UnitFieldRegistration::undoAction( const Context& context )
{
   Vehicle* veh = getUnit();
   
   tfield* fld = getMap()->getField( position );
   
   switch ( operation ) {
      case UnregisterOnField: 
         if ( fld->getContainer() )
            fld->getContainer()->addToCargo( veh );
         else 
            fld->vehicle = veh;
         break;
         
      case RegisterInCarrier:
      {
         ContainerBase* carrier = getMap()->getContainer( carrierID );
         if ( !carrier )
            throw ActionResult( 21303 );
         
         if ( !carrier->removeUnitFromCargo( veh ))
            throw ActionResult( 21302, veh );
         
         break;
      }
      
      case UnRegisterFromCarrier:
      {
         ContainerBase* carrier = getMap()->getContainer( carrierID );
         if ( !carrier )
            throw ActionResult( 21303 );
         
         carrier->addToCargo( veh );
         break;
      }
         
      case RegisterOnField:
         if ( fld->getContainer() && fld->getContainer() != veh ) {
            if ( !fld->getContainer()->removeUnitFromCargo( veh ))
               throw ActionResult( 21302, veh );
         } else {
            if ( fld->vehicle != veh )
               throw ActionResult( 21301, veh );
            fld->vehicle = NULL;
         }
         break;
            
      case RemoveView:
         veh->addview();
         break;
         
      case AddView:
         veh->removeview();
         break;
         
      case Position3D:
         veh->xpos = previousPosition.x;
         veh->ypos = previousPosition.y;
         veh->height = previousPosition.getBitmappedHeight();
         break;
         
      case Position:
         veh->xpos = previousPosition.x;
         veh->ypos = previousPosition.y;
         break;
         
   }
   
   return ActionResult(0);
}

ActionResult UnitFieldRegistration::preCheck()
{
   if ( operation == Position || operation == Position3D ) {
      if ( getUnit()->xpos != previousPosition.x || getUnit()->ypos != previousPosition.y )
         return ActionResult( 21206, getUnit() ); 
   }
   return ActionResult(0);
}

ActionResult UnitFieldRegistration::postCheck()
{
   return ActionResult(0);
}



namespace {
   const bool r1 = registerAction<UnitFieldRegistration> ( ActionRegistry::UnitFieldRegistration );
}

