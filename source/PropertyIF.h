/***************************************************************************
                          propertyIF.h  -  description
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

#ifndef PROPERTYIF_H
#define PROPERTYIF_H

 /**
* An Interface (abstract class) thats holds a named (String)Value.
* @author Frank Landgraf
*/
class PropertyIF	{
	public:
		virtual	const char*	getName() const				=	0;
		virtual void		setName(const char*)		=	0;

		virtual	const char*	getValueString() const		=	0;
		virtual void		setValueString(const char*)	=	0;

		virtual void		setValueToDefault()			=	0;
};

#endif //#ifndef PROPERTYIF_H
