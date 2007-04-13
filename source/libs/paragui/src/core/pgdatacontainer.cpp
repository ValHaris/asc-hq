/*
    ParaGUI - crossplatform widgetset
    Copyright (C) 2000,2001  Alexander Pipelka
 
    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.
 
    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.
 
    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 
    Alexander Pipelka
    pipelka@teleweb.at
 
    Last Update:      $Author: mbickel $
    Update Date:      $Date: 2007-04-13 16:16:00 $
    Source File:      $Source: /home/martin/asc/v2/svntest/games/asc/source/libs/paragui/src/core/pgdatacontainer.cpp,v $
    CVS/RCS Revision: $Revision: 1.2 $
    Status:           $State: Exp $
*/

#include "pgdatacontainer.h"

PG_DataContainer::PG_DataContainer(Uint32 size) {
	if(size == 0) {
		my_data = NULL;
		my_size = 0;
	}

	my_data = new char[size];
	my_size = size;
}

PG_DataContainer::~PG_DataContainer() {
	if(my_data != NULL) {
		delete[] my_data;
	}
}


Uint32 PG_DataContainer::size() {
	return my_size;
}

char* PG_DataContainer::data() {
	return my_data;
}
