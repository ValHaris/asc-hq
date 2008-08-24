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


#include "inflictdamage.h"

#include "../containerbase.h"
#include "../gamemap.h"

#include "destructcontainer.h"
     
InflictDamage::InflictDamage( ContainerBase* container, int damage )
   : ContainerAction( container )
{
   this->damage = damage;
   this->originalDamage = -1;
   this->resultingDamage = -1;
}
      
      
ASCString InflictDamage::getDescription() const
{
   ASCString res = "Inflict " + ASCString::toString(damage) + " damage";
   return  res;
}
      
      
void InflictDamage::readData ( tnstream& stream ) 
{
   int version = stream.readInt();
   if ( version != 1 )
      throw tinvalidversion ( "InflictUnitDamage", 1, version );
   
   ContainerAction::readData( stream );
   
   damage = stream.readInt();
   originalDamage = stream.readInt();
   resultingDamage = stream.readInt();
};
      
      
void InflictDamage::writeData ( tnstream& stream ) const
{
   stream.writeInt( 1 );
   
   ContainerAction::writeData( stream );
   
   stream.writeInt( damage );
   stream.writeInt( originalDamage );
   stream.writeInt( resultingDamage );
};


GameActionID InflictDamage::getID() const
{
   return ActionRegistry::InflictDamage;
}

ActionResult InflictDamage::runAction( const Context& context )
{
   ContainerBase* c = getContainer();
   
   originalDamage = c->damage;
   c->damage += damage;
   if ( c->damage >= 100 ) 
      c->damage = 100;
   
   resultingDamage = c->damage;
   if ( c->damage >= 100 ) {
      GameAction* a = new DestructContainer( c );
      ActionResult r = a->execute( context );
      if ( !r.successful() )
         return r;
   }
   return ActionResult(0);
}


ActionResult InflictDamage::undoAction( const Context& context )
{
   ContainerBase* c = getContainer();
   
   if ( c->damage < damage )
      return ActionResult( 21201, c);
   
   c->damage -= damage;
   return ActionResult(0);
}

ActionResult InflictDamage::postCheck()
{
   ContainerBase* c = getContainer();
   
   if ( c->damage != resultingDamage )
      return ActionResult( 21201, c );  
   
   return ActionResult(0);
}



namespace {
   const bool r1 = registerAction<InflictDamage> ( ActionRegistry::InflictDamage );
}
