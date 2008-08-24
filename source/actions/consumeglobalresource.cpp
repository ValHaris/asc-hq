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


#include "consumeresource.h"

#include "../vehicle.h"
#include "../gamemap.h"
     
ConsumeResource::ConsumeResource( ContainerBase* container, const Resources& toGet )
   : ContainerAction( container )
{
   this->toGet = toGet;
   this->globalStorage = globalStorage;
}
      
      
ASCString ConsumeResource::getDescription() const
{
   ASCString res = "Consume " + toGet.toString() + " resources ";
   const ContainerBase* c = getContainer();
   if ( c ) 
      res += " of  " + c->getName();
   return  res;
}
      
      
const intn consumeResourceVersion = 1;
      
void ConsumeResource::readData ( tnstream& stream ) 
{
   ContainerAction::readData( stream );
   
   int version = stream.readInt();
   if ( version < 1  || version > consumeResourceVersion)
      throw tinvalidversion ( "ConsumeResource", consumeResourceVersion, version );
   
   toGet.read( stream );
   got.read( stream );
};
      
      
void ConsumeResource::writeData ( tnstream& stream ) const
{
   ContainerAction::writeData( stream );
   stream.writeInt( consumeResourceVersion );
   toGet.write( stream );
   got.write( stream );
};


GameActionID ConsumeResource::getID() const
{
   return ActionRegistry::ConsumeResource;
}

ActionResult ConsumeResource::runAction( const Context& context )
{
   got = getContainer()->getResource( toGet, false, 1, context.actingPlayer->getPosition() );
   return ActionResult(0);
}


ActionResult ConsumeResource::undoAction( const Context& context )
{
   getContainer()->putResource( got, false, 1, context.actingPlayer->getPosition() );
   return ActionResult(0);
}

ActionResult ConsumeResource::postCheck()
{
   return ActionResult(0);
}


namespace {
   const bool r1 = registerAction<ConsumeResource> ( ActionRegistry::ConsumeResource );
}
