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


#include "internalammotransfercommand.h"

#include "../vehicle.h"
#include "../mapfield.h"
#include "../gamemap.h"
#include "../viewcalculation.h"
#include "../spfst.h"
#include "changeunitmovement.h"
#include "../mapdisplayinterface.h"
#include "action-registry.h"
#include "../reactionfire.h"
#include "../soundList.h"
#include "consumeammo.h"
#include "servicecommand.h"

bool InternalAmmoTransferCommand :: avail ( const Vehicle* unit )
{
   if ( unit ) {
      for( int i=0; i<unit->typ->weapons.count; i++ )
      {
         const SingleWeapon* weapon = unit->getWeapon( i );
         if( weapon->canRefuel() ) 
            return true;
         
         for( int j=0; j<i; j++ )
         {
            const SingleWeapon* otherWeapon = unit->getWeapon( j );
            if( weapon->equals( otherWeapon ) ) 
               return true;
         }
      }
      return false;
   }

   return false;
}


InternalAmmoTransferCommand :: InternalAmmoTransferCommand ( Vehicle* unit)
   : UnitCommand ( unit )
{

}



bool InternalAmmoTransferCommand::checkConsistency()
{
   map<int,int> amount;
   
   Vehicle* v = getUnit();
   for ( int w = 0; w < v->typ->weapons.count; ++w ) {
      if ( ammoAmount.size() < w )
         return false;
            
      int  type = v->typ->weapons.weapon[w].getScalarWeaponType();
      amount[type] += v->ammo[w] - ammoAmount[w];  
   }
   
   for ( map<int,int>::iterator i = amount.begin(); i != amount.end(); ++i )
      if ( i->second != 0 )
         return false;
   
   return true;
      
}


bool InternalAmmoTransferCommand::setAmmounts( const vector<int>& ammo )
{
   ammoAmount = ammo;
   
   return checkConsistency();
}


ActionResult InternalAmmoTransferCommand::go ( const Context& context )
{
   if ( getState() != SetUp )
      return ActionResult(22000);

   Vehicle* unit = getUnit();
   
   if ( !avail( unit ))
      return ActionResult( 22600 );
   
   if ( !checkConsistency())
      return ActionResult( 23300 );
   
   for ( int w = 0; w < unit->typ->weapons.count; ++w ) {
      if ( unit->ammo[w] != ammoAmount[w] ) {
         auto_ptr<ConsumeAmmo> cr ( new ConsumeAmmo( unit, unit->typ->weapons.weapon[w].getScalarWeaponType(), w, unit->ammo[w] - ammoAmount[w] ));
         ActionResult res = cr->execute( context );
         if ( !res.successful() )
            return res;
         else
            cr.release();
      }
   }
         
   setState( Finished );
   
   return ActionResult(0);
}



static const int InternalAmmoTransferCommandVersion = 1;

void InternalAmmoTransferCommand :: readData ( tnstream& stream )
{
   UnitCommand::readData( stream );
   int version = stream.readInt();
   if ( version > InternalAmmoTransferCommandVersion )
      throw tinvalidversion ( "InternalAmmoTransferCommand", InternalAmmoTransferCommandVersion, version );
   
   readClassContainer( ammoAmount, stream );
}

void InternalAmmoTransferCommand :: writeData ( tnstream& stream ) const
{
   UnitCommand::writeData( stream );
   stream.writeInt( InternalAmmoTransferCommandVersion );
   writeClassContainer( ammoAmount, stream );
}


ASCString InternalAmmoTransferCommand :: getCommandString() const
{
   ASCString c;
   c.format("InternalAmmoTransfer ( %d )", getUnitID() );
   return c;

}

GameActionID InternalAmmoTransferCommand::getID() const
{
   return ActionRegistry::InternalAmmoTransferCommand;
}

ASCString InternalAmmoTransferCommand::getDescription() const
{
   ASCString s = "InternalAmmoTransfer ";
   
   if ( getUnit())
      s += " for unit " + getUnit()->getName();
   
   return s;
}

namespace
{
   const bool r1 = registerAction<InternalAmmoTransferCommand> ( ActionRegistry::InternalAmmoTransferCommand );
}

