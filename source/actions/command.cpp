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


#include "command.h"
#include "../util/messaginghub.h"
#include "../basestrm.h"

Command::Command( GameMap* gamemap ) 
   : GameAction( gamemap ), state ( Planned )
{
}
   
void Command::readData ( tnstream& stream )
{
   stream.readInt();
   state = (State)stream.readInt();   
}


void Command::writeData ( tnstream& stream ) const
{
   stream.writeInt(1);
   stream.writeInt( (int) state );
}

void Command::setState( State state )
{
   this->state = state;  
}


ActionResult Command::runAction( const Context& context ) 
{
   ActionResult res = checkExecutionPrecondition();
   if ( !res.successful() )
      return res;
   else
      return go ( context );  
}


ActionResult Command::undoAction( const Context& context )
{
   return ActionResult(0);
}

ActionResult Command::redo( const Context& context )
{
   if ( getState() == Finished || getState() == Run )
      setState( SetUp );
   
   deleteChildren();
   
   Context c ( context, this );
   
   return go(c);
}

vector<MapCoordinate> Command::getCoordinates() const
{
   return vector<MapCoordinate>(); 
}

ActionResult Command::checkExecutionPrecondition() const
{
   return ActionResult(0);
}

