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


#include "repairunitcommand.h"

#include "../vehicle.h"
#include "../mapfield.h"
#include "../gamemap.h"
#include "../viewcalculation.h"
#include "../spfst.h"
#include "../mapdisplayinterface.h"
#include "action-registry.h"
#include "changeunitproperty.h"
#include "changecontainerproperty.h"
#include "servicing.h"
#include "consumeresource.h"


bool RepairUnitCommand :: availInternally ( const ContainerBase* servicer )
{
   return servicer->baseType->hasFunction( ContainerBaseType::InternalUnitRepair );
}

      
bool RepairUnitCommand :: avail ( const ContainerBase* servicer )
{
   if ( !servicer )
      return false;
   
   if ( availInternally( servicer ))
      return true;
   
   const Vehicle* v = dynamic_cast<const Vehicle*>(servicer);
   if ( v ) 
      return availExternally( v );
   else
      return false;
}


bool RepairUnitCommand :: availExternally ( const Vehicle* veh )
{
   if ( !veh )
      return false;
   
   if ( veh && !veh->attacked && veh->reactionfire.getStatus() == Vehicle::ReactionFire::off ) 
      if ( veh->typ->hasFunction( ContainerBaseType::ExternalRepair  ))
         for ( int i = 0; i < veh->typ->weapons.count; i++ )
            if ( veh->typ->weapons.weapon[i].service() )
               return true;

   return false;
}



RepairUnitCommand :: RepairUnitCommand ( ContainerBase* container )
   : ContainerCommand ( container )
{
}



vector<Vehicle*> RepairUnitCommand :: getExternalTargets()
{
   ServiceTargetSearcher sts( getContainer(), ServiceTargetSearcher::checkRepair );
   sts.startSearch();
   externalTargets.clear();
   
   for ( ServiceTargetSearcher::Targets::const_iterator i = sts.getTargets().begin(); i != sts.getTargets().end(); ++i )
      if ( !(*i)->getCarrier() ) {
         Vehicle* v = dynamic_cast<Vehicle*>(*i);
         if ( v )
            externalTargets.push_back( v );
      }
   
   return externalTargets;
}

vector<Vehicle*> RepairUnitCommand :: getInternalTargets()
{
   ServiceTargetSearcher sts( getContainer(), ServiceTargetSearcher::checkRepair );
   sts.startSearch();
   
   internalTargets.clear();
   
   for ( ServiceTargetSearcher::Targets::const_iterator i = sts.getTargets().begin(); i != sts.getTargets().end(); ++i )
      if ( (*i)->getCarrier() == getContainer() ) {
         Vehicle* v = dynamic_cast<Vehicle*>(*i);
         if ( v )
            internalTargets.push_back( v );
      }
   
   return internalTargets;
}


bool RepairUnitCommand::validTarget( const Vehicle* target )
{
   ServiceTargetSearcher sts( getContainer(), ServiceTargetSearcher::checkRepair );
   sts.startSearch();
   for ( ServiceTargetSearcher::Targets::const_iterator i = sts.getTargets().begin(); i != sts.getTargets().end(); ++i )
      if ( target == *i ) 
         return true;
   
   return false;
}

void RepairUnitCommand::setTarget( Vehicle* target )
{
   if ( !target )
      return;
     
   if ( validTarget(target) ) {
      targetNWid = target->networkid;
      setState( SetUp );
   }
}



ActionResult RepairUnitCommand::go ( const Context& context )
{
   if ( getState() != SetUp )
      return ActionResult(22000);

   ContainerBase* servicer = getContainer();
   if ( !avail( servicer ))
      return ActionResult( 22401 );
   
   Vehicle* target = getMap()->getUnit( targetNWid );
   if ( !target )
      return ActionResult( 22400 );
   
   if ( !validTarget(target))
      return ActionResult( 22402 );
   
   
   Resources cost;
   int oldDamage = target->damage;
   int newDamage = servicer->getMaxRepair ( target, 0, cost );
   
   auto_ptr<ChangeContainerProperty> propChange ( new ChangeContainerProperty( target, ChangeContainerProperty::Damage, newDamage ));
   ActionResult res = propChange->execute( context );
   if ( res.successful() )
      propChange.release();
   else
      return res;
   
   int experience_o = max( 0, target->experience_offensive - Vehicle::getRepairExperienceDecrease( oldDamage, newDamage ));
   int experience_d = max( 0, target->experience_defensive - Vehicle::getRepairExperienceDecrease( oldDamage, newDamage ));
   
   if ( experience_o != target->experience_offensive ) {
      auto_ptr<ChangeUnitProperty> expChange ( new ChangeUnitProperty( target, ChangeUnitProperty::ExperienceOffensive, experience_o ));
      ActionResult res = expChange->execute( context );
      if ( res.successful() )
         expChange.release();
      else
         return res;
   }
   
   if ( experience_d != target->experience_defensive ) {
      auto_ptr<ChangeUnitProperty> expChange ( new ChangeUnitProperty( target, ChangeUnitProperty::ExperienceDefensive, experience_d ));
      ActionResult res = expChange->execute( context );
      if ( res.successful() )
         expChange.release();
      else
         return res;
   }
   
   
   auto_ptr<ConsumeResource> resource ( new ConsumeResource( getContainer(), cost ));
   res = resource->execute( context );
   if ( res.successful() )
      resource.release();
      
   
   return res;
}



static const int RepairUnitCommandVersion = 1;

void RepairUnitCommand :: readData ( tnstream& stream )
{
   ContainerCommand::readData( stream );
   int version = stream.readInt();
   if ( version > RepairUnitCommandVersion )
      throw tinvalidversion ( "RepairUnitCommand", RepairUnitCommandVersion, version );
   targetNWid = stream.readInt();
}

void RepairUnitCommand :: writeData ( tnstream& stream ) const
{
   ContainerCommand::writeData( stream );
   stream.writeInt( RepairUnitCommandVersion );
   stream.writeInt( targetNWid );
}


ASCString RepairUnitCommand :: getCommandString() const
{
   ASCString c;
   c.format("repairUnit ( map, %d, %d )", getContainerID(), targetNWid );
   return c;

}

GameActionID RepairUnitCommand::getID() const
{
   return ActionRegistry::RepairUnitCommand;
}

ASCString RepairUnitCommand::getDescription() const
{
   ASCString s = "Repair unit " + ASCString::toString( targetNWid );
   
   if ( getContainer( false ))
      s += " with " + getContainer()->getName();
   
   return s;
}

ActionResult RepairUnitCommand::checkExecutionPrecondition() const
{
   if ( getMap()->getCurrentPlayer().diplomacy.isAllied( getContainer() ))
      return ActionResult(0);
   else
      return ActionResult(101);
}


namespace
{
   const bool r1 = registerAction<RepairUnitCommand> ( ActionRegistry::RepairUnitCommand );
}

