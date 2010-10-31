/*
     This file is part of Advanced Strategic Command; http://www.asc-hq.org
     Copyright (C) 1994-2010  Martin Bickel
 
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

#include "abstracttaskcontainer.h"
#include "../basestrm.h"
            
            
AbstractTaskContainer::AbstractTaskContainer() : newTasks(NULL), lastPlayer(-1)
{
   for ( int i = 0; i < GameMap::maxTotalPlayers; ++i )
      playerTasks[i] = NULL;
}            

AbstractTaskContainer::~AbstractTaskContainer() 
{
   for ( int i = 0; i < GameMap::maxTotalPlayers; ++i ) {
      delete playerTasks[i];
      playerTasks[i] = NULL;
   }
   
   delete newTasks;
   newTasks = NULL;
}            

void AbstractTaskContainer::writeStorage( tnstream& stream ) const
{
   stream.writeInt( taskMagic );
   for ( int i = 0; i < GameMap::maxTotalPlayers; ++i ) {
      if ( playerTasks[i] ) {
         stream.writeInt( 1 );
         playerTasks[i]->writetostream( &stream );
      } else
         stream.writeInt( 0 );
   }
   stream.writeInt( taskMagic );
   if ( newTasks ) {
      stream.writeInt( 1 );
      newTasks->writetostream( &stream );
   } else
      stream.writeInt( 0 );
      
   stream.writeInt( lastPlayer );
   stream.writeInt( taskMagic );
      
}

     
void AbstractTaskContainer::readStorage( tnstream& stream )
{
   int magic = stream.readInt();
   if ( magic != taskMagic ) 
      throw new tinvalidversion( stream.getLocation(), taskMagic, magic );
   
   for ( int i = 0; i < GameMap::maxTotalPlayers; ++i ) {
   
      delete playerTasks[i];
      playerTasks[i] = NULL;
      
      int data = stream.readInt();
      if ( data ) {
         playerTasks[i] = new MemoryStreamStorage();
         playerTasks[i]->readfromstream( &stream );
      }
   }
   
   magic = stream.readInt();
   if ( magic != taskMagic ) 
      throw new tinvalidversion( stream.getLocation(), taskMagic, magic );
   
   delete newTasks;
   newTasks = NULL;

   if ( stream.readInt() ) {   
      newTasks = new MemoryStreamStorage();
      newTasks->readfromstream( &stream );
   } 

   lastPlayer = stream.readInt();

   magic = stream.readInt();
   if ( magic != taskMagic ) 
      throw new tinvalidversion( stream.getLocation(), taskMagic, magic );
   
}
