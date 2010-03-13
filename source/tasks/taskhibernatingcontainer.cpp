/*
     This file is part of Advanced Strategic Command; http://www.asc-hq.org
     Copyright (C) 1994-2009  Martin Bickel
 
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

#include "taskhibernatingcontainer.h"

#include "task.h"
#include "../gamemap.h"
            

void TaskHibernatingContainer::hook( GameMap& gamemap )
{
   gamemap.tasks = new TaskHibernatingContainer();
}


void TaskHibernatingContainer::write ( tnstream& stream )
{
   stream.writeInt( buffer.size() );
   for ( Buffer::iterator i = buffer.begin(); i != buffer.end(); ++i ) {
      stream.writeInt( taskMagic );
      (*i)->writetostream( &stream );
      stream.writeInt( taskMagic );
   }
}


void TaskHibernatingContainer::read ( tnstream& stream )
{
   int size = stream.readInt();
   assertOrThrow( size >= 0 );
   buffer.clear();
   
   for ( int i = 0; i < size; ++i ) {
      int magic = stream.readInt();
      if ( magic != taskMagic ) 
         throw new tinvalidversion( stream.getLocation(), taskMagic, magic );
      
      tmemorystreambuf* buf = new tmemorystreambuf();
      buf->readfromstream( &stream );
      buffer.push_back( buf );
      
      magic = stream.readInt();
      if ( magic != taskMagic ) 
         throw new tinvalidversion( stream.getLocation(), taskMagic, magic );
   }
}

