/***************************************************************************
                          property.cpp  -  description
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

#include <stdio.h>
#include "Property.h"

TextProperty::TextProperty()
{
	this->setName(NULL);
	this->setDefault(NULL);
	this->setValue(this->getDefault());
}

TextProperty::TextProperty	(	const char* pszName,
								Named* pszValue		/*	=	NULL*/,
								const char* pszDefault		/*	=	NULL*/
							)
{
	_name.setName(pszName);
	this->setDefault(pszDefault);
	
	this->_ptrValue	=	pszValue;
}
		
TextProperty::~TextProperty()
{}

const char*	TextProperty::getName() const
{
	return _name.getName();
}

void		TextProperty::setName(const char* pszName)
{
	_name.setName(pszName);
}

void TextProperty::setValue(const char* pszValue)
{	
	_ptrValue->setName(pszValue);
}

const char*	TextProperty::getValue()		const
{
	return 		_ptrValue->getName()
			?	_ptrValue->getName()
			:	"";

}

const char*	TextProperty::getValueString() const
{
	return getValue();
}

void TextProperty::setValueString(const char* pszValue)
{
	setValue(pszValue);
}

void TextProperty::setDefault(const char* pszDefault)
{
		_default_value.setName(pszDefault);
}

const char* TextProperty::getDefault()	const
{	
	return	_default_value.getName();
}

void TextProperty::setValueToDefault()
{
	setValue(getDefault());
}


///////////////////////////////////////////////////////
//IntProperty

IntProperty::IntProperty	(	const char* pszName,
								int*		pInt	/*	=	NULL*/,
								int			Default	/*	=	0	*/
							)
{
	_name.setName(pszName);
	this->setDefault(Default);
	this->_ptrValue	=	pInt;
	*pInt=Default;
}

const char*	IntProperty::getName() const
{
	return _name.getName();
}

void		IntProperty::setName(const char* pszName)
{
	_name.setName(pszName);
}

void IntProperty::setValue(int Value)
{	
	*(this->_ptrValue)	=	Value;
}

int	IntProperty::getValue()		const
{
	return 	*_ptrValue;
}

void IntProperty::setDefault(int Default)
{
		_default_value	=	Default;
}

int IntProperty::getDefault()	const
{	
	return	_default_value;
}

const char*	IntProperty::getValueString()	const	
{	
	static char vstring[100];
	sprintf(vstring,"%d",getValue());
	return vstring;
}

void	IntProperty::setValueString(const char* pszValue)
{
	setValue(atoi(pszValue));
}

void IntProperty::setValueToDefault()
{
	setValue(getDefault());
}
