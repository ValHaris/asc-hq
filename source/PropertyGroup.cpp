/***************************************************************************
                          PropertyGroup.cpp  -  description
                             -------------------
    begin                : Thu Jun 29 2000
    copyright            : (C) 2000 by frank landgraf
    email                : 
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

//#include <stdio.h>
#include <iostream>
#include <iomanip>

// using namespace std;

#include "PropertyGroup.h"

/////////////////////////////////////////////////////////////////
// PropertyGroup

const char*	SEPARATOR	=	"=";
	
PropertyGroup::PtrList::~PtrList()	
{
	//Die Pointer muessen geloescht werden, 
	//das macht stl-list nicht selbst
	for	(	iterator 
				it	=	begin();
				it	!=	end();
				it++
		)
		delete *it;
}	
	
PropertyGroup::PropertyGroup()
	:	Named()
{}

PropertyIF*	PropertyGroup::find(const char* pszName)
{
	PropertyIF* pProperty	=	NULL;
	
	for	(	PtrList::const_iterator 
				it	=	_propertys.begin();
				it	!=	_propertys.end();
				it++
		)
	{
		if (!strcmp((*it)->getName(),pszName))
		{
			pProperty	=	*it;
			break;
		};
	};
	return pProperty;
}
		
void	PropertyGroup::add(PropertyIF* pProperty)
{
	_propertys.push_back(pProperty);
}

const PropertyGroup::PtrList&	PropertyGroup::getPropertyList()
{
	return _propertys;
}

void PropertyGroup::setValueToDefault()
{
	for	(	PtrList::iterator 
				it	=	_propertys.begin();
				it	!=	_propertys.end();
				it++
		)
		(*it)->setValueToDefault();
}

bool	PropertyGroup::Load(std::istream& is)	
{
	char buffer[256];
	char*	pszName;
	char*	pszValue;

	//set all values to default
	//
	// setValueToDefault();
				
	while	(!is.eof() && is.good() )	{
	
		is.getline(buffer,256);
		
		pszName	=	strtok(buffer,SEPARATOR);
		
		if (!pszName)	
			continue;
		
		pszValue	=	strtok(NULL,SEPARATOR); 

		PropertyIF* p	=	find(pszName);
	
		if	(p)
			p->setValueString(pszValue);
		else
			cerr	<<	"PropertyGroup::Load  Property "
					<<	pszName	
					<<	" not found"	<<endl;
	};
	return false;
}

bool	PropertyGroup::Save(std::ostream& os)
{
	for	(	PtrList::const_iterator 
				it	=	getPropertyList().begin();
				it	!=	getPropertyList().end();
				it++
		)
	{
		os	<<	(*it)->getName()
			<<	SEPARATOR
			<<	(*it)->getValueString()		
			<<	endl;
	};
	return false;
}
