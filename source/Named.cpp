/***************************************************************************
                          Named.cpp  -  description
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

#include "Named.h"

#include <cstring>
#include <cstdlib>

Named::Named()
      :	_name(NULL)
{}


Named::Named(const char* pszName)
{
   this->setName(pszName);
}


Named::Named(const Named& n)
{
   setName(n.getName());
}

Named::Named(const ASCString& pszName)
{
   this->setName(pszName.c_str());
}

Named& Named::operator= (const Named& n)
{
   setName ( n.getName());
   return *this;
}


Named::~Named()
{
   if (_name!=NULL)
      free(_name);
}

const char*	Named::getName() const
{
   return _name;
}

void	Named::setName(const char* pszName)
{
   if (_name!=NULL)
      free(_name);
   _name	=	(pszName!=NULL && pszName[0])	?	strdup(pszName)	:	NULL;
}
