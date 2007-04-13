/*
    ParaGUI - crossplatform widgetset
    Copyright (C) 2000-2004  Alexander Pipelka
 
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
    Update Date:      $Date: 2007-04-13 16:15:57 $
    Source File:      $Source: /home/martin/asc/v2/svntest/games/asc/source/libs/paragui/include/pgsingleton.h,v $
    CVS/RCS Revision: $Revision: 1.2 $
    Status:           $State: Exp $
*/

/** \file pgsingleton.h
	Header file for the PG_Singleton class template
*/

#ifndef PG_SINGLETON_H
#define PG_SINGLETON_H

template< class T >
class PG_Singleton {
public:
	static T& GetInstance() {
		static T obj;
		return obj;
	}

protected:
	PG_Singleton() {}

	~PG_Singleton() {}

private:
	PG_Singleton(const T&);

	T& operator=(const T&);
};

#endif
