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
   Source File:      $Source: /home/martin/asc/v2/svntest/games/asc/source/libs/paragui/include/pgpoint.h,v $
   CVS/RCS Revision: $Revision: 1.1.2.1 $
   Status:           $State: Exp $
*/

/** \file pgpoint.h
	Header file for the PG_Pointt class.
*/

#ifndef PG_POINT_H
#define PG_POINT_H

#include "SDL.h"

/**
	@author Alexander Pipelka
 
	@short A wrapper for a point on the screen.
 
	Useful when its necessary to specify a 2D using Cartesian coordinates (x, y).
 
*/


class DECLSPEC PG_Point {
public:

	PG_Point();

	PG_Point(Sint16 _x, Sint16 _y);

	Sint16 x;
	Sint16 y;

	static PG_Point null;
};

#endif // PG_POINT_H
