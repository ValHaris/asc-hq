/***************************************************************************
                          PropertyGroup.h  -  description
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
#ifndef PROPERTYGROUP_H
#define PROPERTYGROUP_H

#include "PropertyIF.h"
#include "Named.h"

#include <list>
#include <istream>
/**
* A Class thats holds a Group of Propertys
*
* @author Frank Landgraf
*/
class PropertyGroup	:	public	Named	{
	public:
	
		class PtrList	:	public	std::list<PropertyIF*>	{
			public:
				~PtrList();	
		};
		
		PropertyGroup();
		
		void		add(PropertyIF*);
		PropertyIF*	find(const char* pszName);
		
		const PtrList&	getPropertyList();

		virtual void		setValueToDefault();

		virtual	bool		Load(std::istream& is);                  
		virtual	bool		Save(std::ostream& os);                       

	private:
		PtrList	_propertys;
};
#endif //#ifndef PROPERTYGROUP_H
