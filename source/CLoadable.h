/***************************************************************************
                          CLoadable.h  -  description
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
#ifndef CLOADABLE_H
#define CLOADABLE_H

#include "Property.h"
#include "PropertyGroup.h"

class CGameOptions;

/**
* 
*/
class	CLoadableGameOptions	:	public PropertyGroup	{
	public:
		CLoadableGameOptions (CGameOptions* pOptions);
		virtual	~CLoadableGameOptions();
	private:
		CLoadableGameOptions();
		CGameOptions* _pOptions;	
};
		
#endif	//#ifndef CLOADABLE_H
