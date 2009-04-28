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


#include "viewregistration.h"
#include "action-registry.h"

#include "../vehicle.h"
#include "../gamemap.h"
#include "../viewcalculation.h"
     
ViewRegistration::ViewRegistration( ContainerBase* container, Operation operation )
   : ContainerAction( container), resultingViewChanges(-1)
{
   evalView = false;
   this->operation= operation;
}
      
      
ASCString ViewRegistration::getOpName() const 
{
   switch ( operation ) {
      case AddView: return "AddView";
      case RemoveView: return "RemoveView"; 
   }
   return "";
}
      
ASCString ViewRegistration::getDescription() const
{
   ASCString res = getOpName();
   if ( getContainer(true) ) 
      res += " " + getContainer(true)->getName();

   return  res;
}
      
      
void ViewRegistration::readData ( tnstream& stream ) 
{
   ContainerAction::readData( stream );
   int version = stream.readInt();
   if ( version != 1 )
      throw tinvalidversion ( "ViewRegistration", 1, version );
   
   operation = (Operation) stream.readInt();
   evalView = stream.readInt();
   resultingViewChanges = stream.readInt();
};
      
      
void ViewRegistration::writeData ( tnstream& stream ) const
{
   ContainerAction::writeData( stream );
   stream.writeInt( 1 );
   stream.writeInt( (int) operation );
   stream.writeInt( evalView );
   stream.writeInt( resultingViewChanges );
};


GameActionID ViewRegistration::getID() const
{
   return ActionRegistry::ViewRegistration;
}

ActionResult ViewRegistration::runAction( const Context& context )
{
   ContainerBase* veh = getContainer();
   
   
   switch ( operation ) {
      case AddView:
         veh->addview();
         break;
         
      case RemoveView:
         veh->removeview();
         break;
   }
   
   return ActionResult(0);
}


ActionResult ViewRegistration::undoAction( const Context& context )
{
   ContainerBase* veh = getContainer();
   switch ( operation ) {
      case RemoveView:
         veh->addview();
         break;
         
      case AddView:
         veh->removeview();
         break;
         
   }
   
   return ActionResult(0);
}

ActionResult ViewRegistration::preCheck()
{
   return ActionResult(0);
}

ActionResult ViewRegistration::postCheck()
{
   return ActionResult(0);
}



namespace {
   const bool r1 = registerAction<ViewRegistration> ( ActionRegistry::ViewRegistration );
}

