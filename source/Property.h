/***************************************************************************
                          property.h  -  description
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
#ifndef PROPERTY_H
#define PROPERTY_H

#include "PropertyIF.h"
#include "Named.h"

#include <list>

/**
* A Class thats holds a named TextValue.
*
* the textValue is a pointer to char*
*
* @author Frank Landgraf
*/
class TextProperty	:	public	PropertyIF	{
	public:
		TextProperty();
		TextProperty(	const	char* pszName,
						Named* pszValue		=	NULL,
						const	char* pszDefault	=	NULL
					);
		
		virtual	~TextProperty();

		const char*	getName() const;
		void		setName(const char* pszName);

		void		setValue(const char*);
		const char*	getValue()		const;

		virtual	const char*	getValueString() const;

		virtual void		setValueString(const char* pszValue);
		
		void		setDefault(const char*);
		const char*	getDefault()	const;
		
		virtual void		setValueToDefault();

	private:
		Named	_name;
	//	Named	_value;
		Named	_default_value;

		Named*	_ptrValue;
};

/**
* A Class thats holds a named int Value.
*
* @author Frank Landgraf
*/
class IntProperty	:	public	PropertyIF	{
	public:
		IntProperty();
		IntProperty(	const char* pszName,
						int*		pInt	=	NULL,
						int			Default	=	0
				);
		virtual	~IntProperty(){};

		const char*	getName() const;
		void		setName(const char* pszName);
	
		void		setValue(int);
		int			getValue()		const;

		virtual	const char*	getValueString()	const;
		virtual void		setValueString(const char* pszValue);
		virtual void		setValueToDefault();

		void	setDefault(int);
		int		getDefault()	const;
		
	private:
		Named	_name;
		int		_default_value;
		int*	_ptrValue;	
};

#endif //#ifndef PROPERTY_H
