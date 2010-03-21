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


#include "transfercontrolcommand.h"

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
#include "convertcontainer.h"

bool TransferControlCommand :: avail ( const ContainerBase* item )
{
   if ( !item  )
      return false;
   
   if ( item->getMap()->getgameparameter( cgp_disableUnitTransfer ))
      return false;
   
   Receivers rec = getReceivers( item->getMap(), item->getOwner(), item->getCarrier() );
   
   return rec.size() > 0 ;
}


TransferControlCommand::Receivers TransferControlCommand::getReceivers( GameMap* map, int currentPlayer, bool isInCarrier )
{
   Receivers rec;
   if ( !map )
      return rec;
   
   for ( int p = 0; p < map->getPlayerCount(); ++p )
      if ( p != currentPlayer )
         if ( map->player[p].exist() ) {
            if ( isInCarrier ) {
               if ( map->player[p].diplomacy.getState( currentPlayer ) >= ALLIANCE )
                  rec.push_back( &map->player[p] );   
            } else {
               if ( map->player[p].diplomacy.getState( currentPlayer ) >= PEACE )
                  rec.push_back( &map->player[p] );   
            }
         }
   
   return rec;
}


TransferControlCommand::Receivers TransferControlCommand::getReceivers()
{
   return getReceivers( getContainer()->getMap(), getContainer()->getOwner(), getContainer()->getCarrier() );  
}


TransferControlCommand :: TransferControlCommand ( ContainerBase* container )
   : ContainerCommand ( container ), receivingPlayer(-1)
{

}





ActionResult TransferControlCommand::go ( const Context& context )
{
   if ( getState() != SetUp )
      return ActionResult(22000);

   if ( !avail( getContainer() ))
      return ActionResult(22800);
   
   bool found = false;
   Receivers rec = getReceivers();
   for ( Receivers::const_iterator i = rec.begin(); i != rec.end(); ++i )
      if ( (*i)->getPosition() == receivingPlayer )
         found = true;
   
   if ( !found )
      return ActionResult(22801);
   
   auto_ptr<ConvertContainer> cc ( new ConvertContainer( getContainer(), receivingPlayer ));
   ActionResult res = cc->execute(context);
   
   if ( res.successful() ) {
      cc.release();
      setState( Finished );
      
      computeview( getMap(), 0, false, &context );
      
      if ( context.display )
         context.display->repaintDisplay();
   } else
      setState( Failed );
   
   return res;
}



static const int TransferControlCommandVersion = 1;

void TransferControlCommand :: readData ( tnstream& stream )
{
   ContainerCommand::readData( stream );
   int version = stream.readInt();
   if ( version > TransferControlCommandVersion )
      throw tinvalidversion ( "TransferControlCommand", TransferControlCommandVersion, version );
   receivingPlayer = stream.readInt();
}

void TransferControlCommand :: writeData ( tnstream& stream ) const
{
   ContainerCommand::writeData( stream );
   stream.writeInt( TransferControlCommandVersion );
   stream.writeInt( receivingPlayer );
}

void TransferControlCommand :: setReceiver( const Player* receiver )
{
   if ( receiver ) {
      receivingPlayer = receiver->getPosition();    
      setState( SetUp );
   }
}


ASCString TransferControlCommand :: getCommandString() const
{
   ASCString c;
   c.format("transferControl ( map, %d, %d )", getContainerID(), receivingPlayer );
   return c;

}

GameActionID TransferControlCommand::getID() const
{
   return ActionRegistry::TransferControlCommand;
}

ASCString TransferControlCommand::getDescription() const
{
   ASCString s = "Transfer Control";
   
   if ( getContainer(true) ) {
      s += " of " + getContainer()->getName();
   }
   
   s += "to " + getMap()->getPlayer( receivingPlayer ).getName();
   return s;
}

namespace
{
   const bool r1 = registerAction<TransferControlCommand> ( ActionRegistry::TransferControlCommand );
}

