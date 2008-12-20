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


#include "task.h"
#include "../util/messaginghub.h"
#include "../basestrm.h"

Task::Task( GameMap* gamemap ) 
{
   state = Planned;
   this->gamemap = gamemap;
}
   
GameMap* Task::getMap()
{
   return gamemap;
}

const int currentTaskVersion = 1;
const int gameTaskToken = 0x9812a0c3;

void Task::read ( tnstream& stream )
{
   displayLogMessage(0, "reading " + ASCString::toString(getID()) + "\n");
   
   int version = stream.readInt();
   if ( version > currentTaskVersion )
      throw tinvalidversion ( "GameAction", currentTaskVersion, version );

   readData( stream );
   
   int token = stream.readInt();
   if ( token != gameTaskToken )
      throw tinvalidversion ("GameActionToken", gameTaskToken, token );
   
}

void Task::write ( tnstream& stream )
{
   stream.writeInt( getID() );
   stream.writeInt( currentTaskVersion );
   writeData( stream );
   stream.writeInt( gameTaskToken );
  
}
