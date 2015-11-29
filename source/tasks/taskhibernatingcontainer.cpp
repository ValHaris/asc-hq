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

#include "taskhibernatingcontainer.h"

#include "../gamemap.h"
            

void TaskHibernatingContainer::registerHooks()
{
   GameMap::sigMapCreation.connect( sigc::ptr_fun( &TaskHibernatingContainer::hook ));
}

void TaskHibernatingContainer::hook( GameMap& gamemap )
{
   gamemap.tasks = new TaskHibernatingContainer();
}


void TaskHibernatingContainer::write ( tnstream& stream ) const
{
   writeStorage( stream );
   stream.writeInt( 0 );
   stream.writeInt( taskMagic );
}


void TaskHibernatingContainer::read ( tnstream& stream )
{
   readStorage( stream );
   int tasks = stream.readInt();
   if ( tasks != 0 )
      throw tfileerror( "pending tasks in game");
   
   int magic = stream.readInt();
   if ( magic != taskMagic ) 
      throw new tinvalidversion( stream.getLocation(), taskMagic, magic );

}

TaskHibernatingContainer::~TaskHibernatingContainer()
{
}
