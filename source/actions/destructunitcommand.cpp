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


#include "destructunitcommand.h"

#include "../vehicle.h"
#include "../mapfield.h"
#include "../gamemap.h"
#include "../viewcalculation.h"
#include "../spfst.h"
#include "../mapdisplayinterface.h"
#include "action-registry.h"
#include "../itemrepository.h"
#include "../containercontrols.h"
#include "consumeresource.h"
#include "servicecommand.h"
#include "destructcontainer.h"
#include "../soundList.h"
#include "viewregistration.h"


bool DestructUnitCommand :: avail ( const ContainerBase* unit )
{
   if ( !unit )
      return false;

   if ( unit->getMap()->getField( unit->getPosition() )->getContainer() == unit ) 
      return unit->baseType->hasFunction( ContainerBaseType::ManualSelfDestruct ) ; 
   
   return false;
}



DestructUnitCommand :: DestructUnitCommand ( ContainerBase* container )
   : ContainerCommand ( container )
{
   if ( avail( container ))
      setState( SetUp );
}



 

ActionResult DestructUnitCommand::go ( const Context& context )
{
   if ( getState() != SetUp )
      return ActionResult(21002);

   ContainerBase* container = getContainer();
   if ( !avail( container ))
      return ActionResult( 22101 );
   
   if ( context.display ) {
      Vehicle* v = dynamic_cast<Vehicle*>(container);
      if ( v )
         SoundList::getInstance().playSound( SoundList::unitExplodes , 0, false, v->typ->killSoundLabel );
      else 
         SoundList::getInstance().playSound( SoundList::buildingCollapses );
   }
   
   /*
   ViewRegistration* vr = new ViewRegistration( container, ViewRegistration::RemoveView );
   vr->execute( context );
*/   
   
   MapCoordinate pos = container->getPosition();
   int viewdist = container->baseType->view;
   
   
   auto_ptr<DestructContainer> destructor ( new DestructContainer( container ));
   ActionResult res = destructor->execute( context );
   if ( res.successful() )
      destructor.release();
   
   evaluateviewcalculation( getMap(), pos, viewdist, 0, false, &context );
   
   return res;
}



static const int DestructUnitCommandVersion = 1;

void DestructUnitCommand :: readData ( tnstream& stream )
{
   ContainerCommand::readData( stream );
   int version = stream.readInt();
   if ( version > DestructUnitCommandVersion )
      throw tinvalidversion ( "DestructUnitCommand", DestructUnitCommandVersion, version );
}

void DestructUnitCommand :: writeData ( tnstream& stream ) const
{
   ContainerCommand::writeData( stream );
   stream.writeInt( DestructUnitCommandVersion );
}


ASCString DestructUnitCommand :: getCommandString() const
{
   ASCString c;
   c.format("DestructUnit ( %d  )", getContainerID() );
   return c;

}

GameActionID DestructUnitCommand::getID() const
{
   return ActionRegistry::DestructUnitCommand;
}

ASCString DestructUnitCommand::getDescription() const
{
   ASCString s = "Destruct ";
   
   if ( getContainer(true) ) {
      s += " by " + getContainer()->getName();
   } else
      s += ASCString::toString( getContainerID() );
      
   return s;
}

namespace
{
   const bool r1 = registerAction<DestructUnitCommand> ( ActionRegistry::DestructUnitCommand );
}

