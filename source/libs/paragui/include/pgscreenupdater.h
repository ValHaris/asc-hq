/*
    ParaGUI - crossplatform widgetset
    Copyright (C) 2000,2001,2002  Alexander Pipelka
 
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
    Update Date:      $Date: 2008-06-27 19:22:11 $
    Source File:      $Source: /home/martin/asc/v2/svntest/games/asc/source/libs/paragui/include/pgscreenupdater.h,v $
    CVS/RCS Revision: $Revision: 1.1 $
    Status:           $State: Exp $
*/

#ifndef PG_SCREENUPDATER_H
#define PG_SCREENUPDATER_H


#include "SDL.h"

/**
 @author Martin Bickel
 
 @short Interface for classes manage the updating of Paragui's screen
 
*/
class DECLSPEC PG_ScreenUpdater {
public:
	virtual ~PG_ScreenUpdater() {};
	virtual void UpdateRect(SDL_Surface *screen, Sint32 x, Sint32 y, Sint32 w, Sint32 h) = 0;
	virtual void UpdateRects(SDL_Surface *screen, int numrects, SDL_Rect *rects) = 0;
};

#endif // PG_SCREENUPDATER_H
