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


#include "convertcontainer.h"
#include "destructcontainer.h"
#include "action-registry.h"

#include "../vehicle.h"

ConvertContainer::ConvertContainer( ContainerBase* container, int newPlayer )
   : ContainerAction( container ), newOwner(newPlayer), previousOwner(-1)
{
}
      
ASCString ConvertContainer::getDescription() const
{
   ASCString res = "Change owner of " + getContainer()->getName();
   res += " to " + ASCString::toString(newOwner);
   return  res;
}
      
      
void ConvertContainer::readData ( tnstream& stream ) 
{
   ContainerAction::readData( stream );
   int version = stream.readInt();
   if ( version != 1 )
      throw tinvalidversion ( "ConvertContainer", 1, version );
   
   newOwner = stream.readInt();
   previousOwner = stream.readInt();
};
      
      
void ConvertContainer::writeData ( tnstream& stream ) const
{
   ContainerAction::writeData( stream );
   stream.writeInt( 1 );
   stream.writeInt( newOwner );
   stream.writeInt( previousOwner );
};


GameActionID ConvertContainer::getID() const
{
   return ActionRegistry::ConvertContainer;
}


ActionResult ConvertContainer::runAction( const Context& context )
{
   ContainerBase* c = getContainer();
   previousOwner = c->getOwner();
   
   if ( c->baseType->hasFunction( ContainerBaseType::SelfDestructOnConquer  ) ) {
      (new DestructContainer(c))->execute(context);
      return ActionResult(0);
   }

   c->registerForNewOwner( newOwner );
   
   for ( ContainerBase::Cargo::iterator i = c->cargo.begin(); i != c->cargo.end(); ++i )
      if ( *i ) 
         (new ConvertContainer( *i, newOwner ))->execute(context);
   
   
   c->conquered();
   ContainerBase::anyContainerConquered(c);
   
   return ActionResult(0);
}


ActionResult ConvertContainer::undoAction( const Context& context )
{
   getContainer()->registerForNewOwner( previousOwner );
   return ActionResult(0);
}

ActionResult ConvertContainer::preCheck()
{
   if ( getContainer()->getOwner() != previousOwner  )
      throw ActionResult( 21204, getContainer(), "owner" );
   
   return ActionResult(0);
}

ActionResult ConvertContainer::postCheck()
{
   if ( getContainer()->getOwner() != newOwner  )
      throw ActionResult( 21204, getContainer(), "owner" );
   
   return ActionResult(0);
}


namespace {
   const bool r1 = registerAction<ConvertContainer> ( ActionRegistry::ConvertContainer );
}
