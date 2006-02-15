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
    Update Date:      $Date: 2006-02-15 21:30:16 $
    Source File:      $Source: /home/martin/asc/v2/svntest/games/asc/source/libs/paragui/include/pgfactory.h,v $
    CVS/RCS Revision: $Revision: 1.1.2.1 $
    Status:           $State: Exp $
*/

#ifndef PG_FACTORY_H
#define PG_FACTORY_H

#include <map>
#include <string>

#include "pgsingleton.h"

/** \file pgfactory.h
	Header file for the PG_FactoryHolder class template.
	This include file defines the template class PG_FactoryHolder and defines PG_Factory.
*/

class PG_Widget;

template< class T, class PT = PG_Widget >
class PG_FactoryObject {
public:
	static T* CreateObject(PT* parent) {
		return new T(parent);
	}

	static T* CreateObject0() {
		return new T;
	}
};

template<class H>
class PG_FactoryHolder : public PG_Singleton< PG_FactoryHolder<H> > {
public:

	typedef PG_Widget* (*CREATEFN)(PG_Widget* parent);

	template< class T, class PT >
	static void RegisterClass(const H& classname) {
		PG_Singleton< PG_FactoryHolder<H> >::GetInstance().RegisterCreateFn(classname, (CREATEFN)&PG_FactoryObject<T, PT>::CreateObject);
	}

	template< class T >
	static void RegisterClass(const H& classname) {
		PG_Singleton< PG_FactoryHolder<H> >::GetInstance().RegisterCreateFn(classname, (CREATEFN)&PG_FactoryObject<T>::CreateObject);
	}

	template< class T >
	static void RegisterClass0(const H& classname) {
		PG_Singleton< PG_FactoryHolder<H> >::GetInstance().RegisterCreateFn(classname, (CREATEFN)&PG_FactoryObject<T>::CreateObject0);
	}

	static PG_Widget* CreateObject(const H& classname, PG_Widget* parent = NULL) {
		CREATEFN create = PG_Singleton< PG_FactoryHolder<H> >::GetInstance().creator_map[classname];

		if(create == NULL) {
			return NULL;
		}

		return create(parent);
	}

	static PG_Widget* CreateObject0(const H& classname, PG_Widget* parent = NULL) {
		CREATEFN create = PG_Singleton< PG_FactoryHolder<H> >::GetInstance().creator_map[classname];

		if(create == NULL) {
			return NULL;
		}

		return create(parent);
	}

protected:

	inline void RegisterCreateFn(const H& classname, CREATEFN fn) {
		creator_map[classname] = fn;
	}

	std::map< H, CREATEFN > creator_map;

	friend class PG_Singleton< PG_FactoryHolder<H> >;

};

typedef PG_FactoryHolder<std::string> PG_Factory;

#endif // PG_FACTORY_H
