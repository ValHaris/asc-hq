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


#include "trainunitcommand.h"

#include "../vehicle.h"
#include "../mapfield.h"
#include "../gamemap.h"
#include "../viewcalculation.h"
#include "../spfst.h"
#include "../mapdisplayinterface.h"
#include "action-registry.h"
#include "../itemrepository.h"
#include "../containercontrols.h"
#include "consumeammo.h"
#include "changeunitproperty.h"

bool TrainUnitCommand :: avail ( const ContainerBase* carrier, const Vehicle* unit )
{
   if ( !carrier || !unit )
      return false;
   
   if ( carrier->getMap()->getgameparameter( cgp_bi3_training ) )
      return false;
   
   if ( !carrier->findUnit( unit->networkid, false ) )
      return false;
   
   if ( unit->getCarrier() != carrier )
      return false;
   
   int maxExp = min( unit->getMap()->getgameparameter( cgp_maxtrainingexperience ), maxunitexperience);
   if(    unit->experience_offensive >= maxExp
       && unit->experience_defensive >= maxExp )
      return false;
   
   if ( unit->attacked )
      return false;
   
   
   if ( !carrier->baseType->hasFunction(ContainerBaseType::TrainingCenter) )
      return false;
   
   int num = 0;
   int numsh = 0;
   for (int i = 0; i < unit->typ->weapons.count; i++ )
      if ( unit->typ->weapons.weapon[i].shootable()  && (unit->typ->weapons.weapon[i].count > 0 )) {
         if ( unit->ammo[i] )
            numsh++;
         else
            num++;
      }
      
   if ( num == 0  &&  numsh > 0 )
      return true;
   else
      return false;
}



TrainUnitCommand :: TrainUnitCommand ( ContainerBase* container )
   : ContainerCommand ( container ), unitID(-1)
{

}



 

ActionResult TrainUnitCommand::go ( const Context& context )
{
   if ( getState() != SetUp )
      return ActionResult(22000);

   Vehicle* unit = getMap()->getUnit( unitID );
   if ( !unit )
      return ActionResult( 22100 );
   
   if ( !avail( getContainer(), unit ))
      return ActionResult( 22101 );
   
   int newexp_o = unit->experience_offensive + getMap()->getgameparameter( cgp_trainingIncrement );
   int newexp_d = unit->experience_defensive + getMap()->getgameparameter( cgp_trainingIncrement );
   
   int maxexp = min( unit->getMap()->getgameparameter( cgp_maxtrainingexperience ), maxunitexperience);
   if ( newexp_o > maxexp )
      newexp_o = maxexp;
   
   if ( newexp_d > maxexp )
      newexp_d = maxexp;
   
   if ( newexp_o != unit->experience_offensive ) {
      auto_ptr<ChangeUnitProperty> train ( new ChangeUnitProperty( unit, ChangeUnitProperty::ExperienceOffensive, newexp_o ));
      ActionResult res = train->execute( context );
      if ( res.successful() )
         train.release();
   }
         
   if ( newexp_d != unit->experience_defensive ) {
      auto_ptr<ChangeUnitProperty> train ( new ChangeUnitProperty( unit, ChangeUnitProperty::ExperienceDefensive, newexp_d ));
      ActionResult res = train->execute( context );
      if ( res.successful() )
         train.release();
   }
   
   auto_ptr<ChangeUnitProperty> train2 ( new ChangeUnitProperty( unit, ChangeUnitProperty::AttackedFlag, 1 ));
   ActionResult res = train2->execute( context );
   if ( res.successful() )
      train2.release();
   
   auto_ptr<ChangeUnitProperty> move ( new ChangeUnitProperty( unit, ChangeUnitProperty::Movement, 0 ));
   res = move->execute( context );
   if ( res.successful() )
      move.release();
   
   
   for (int i = 0; i < unit->typ->weapons.count; i++ ) {
      if ( unit->typ->weapons.weapon[i].shootable() && (unit->typ->weapons.weapon[i].count > 0 )) {
         auto_ptr<ConsumeAmmo> consumer ( new ConsumeAmmo( unit, unit->typ->weapons.weapon[i].getScalarWeaponType(), i, 1 ));
         res = consumer->execute( context );
         if ( res.successful() )
            consumer.release();
         else 
            break;
      }
   }
   
   
   if ( res.successful() )
      setState( Finished );
   else
      setState( Failed );
   
   return res;
}



static const int TrainUnitCommandVersion = 1;

void TrainUnitCommand :: readData ( tnstream& stream )
{
   ContainerCommand::readData( stream );
   int version = stream.readInt();
   if ( version > TrainUnitCommandVersion )
      throw tinvalidversion ( "TrainUnitCommand", TrainUnitCommandVersion, version );
   unitID = stream.readInt();
}

void TrainUnitCommand :: writeData ( tnstream& stream ) const
{
   ContainerCommand::writeData( stream );
   stream.writeInt( TrainUnitCommandVersion );
   stream.writeInt( unitID );
}

void TrainUnitCommand :: setUnit( Vehicle* unit )
{
   unitID = unit->networkid;
   setState( SetUp );
}


ASCString TrainUnitCommand :: getCommandString() const
{
   ASCString c;
   c.format("trainUnit ( map, %d, %d )", getContainerID(), unitID );
   return c;

}

GameActionID TrainUnitCommand::getID() const
{
   return ActionRegistry::TrainUnitCommand;
}

ASCString TrainUnitCommand::getDescription() const
{
   ASCString s = "Train ";
   
   if ( getMap()->getUnit( unitID ))
      s += getMap()->getUnit( unitID )->getName();
   else
      s += "unit id " + ASCString::toString(unitID);
   
   return s;
}

namespace
{
   const bool r1 = registerAction<TrainUnitCommand> ( ActionRegistry::TrainUnitCommand );
}

