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

#include "taskcontainer.h"

#include "task.h"
#include "../gamemap.h"
            
void TaskContainer::submit( Task* task )
{
   tasks.push_back( task );
}

TaskContainer::~TaskContainer()
{
   for ( Tasks::iterator i = tasks.begin(); i != tasks.end(); ++i )
      delete *i;
}

void TaskContainer::hook( GameMap& gamemap )
{
   gamemap.tasks = new TaskContainer();
}


static const int taskMagic = 0xda5cda5c;

void TaskContainer::write ( tnstream& stream )
{            
   stream.writeInt( tasks.size() );
   for ( Tasks::iterator i = tasks.begin(); i != tasks.end(); ++i ) {
      stream.writeInt( taskMagic );
      tmemorystreambuf buffer;
      {
         tmemorystream memstream( &buffer, tnstream::writing);
         (*i)->write( memstream );
      }
      
      buffer.writetostream( &stream );
      stream.writeInt( taskMagic );
   }
}


void read ( tnstream& stream )
{
   
}
