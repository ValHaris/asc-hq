/*
    ParaGUI - crossplatform widgetset
    drawtile - draw tiles onto surfaces
 
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
    Update Date:      $Date: 2006-02-15 21:30:16 $
    Source File:      $Source: /home/martin/asc/v2/svntest/games/asc/source/libs/paragui/src/draw/drawtile.cpp,v $
    CVS/RCS Revision: $Revision: 1.1.2.1 $
    Status:           $State: Exp $
*/

#include "pgdraw.h"
#include <cmath>

void PG_Draw::DrawTile(SDL_Surface* surface, const PG_Rect& ref, const PG_Rect& drawrect, SDL_Surface* tilemap) {
	PG_Point index1;
	PG_Point index2;
	PG_Rect oldclip;

	if (!surface || !tilemap) {
		return;
	}

	if (!tilemap->w || !tilemap->h || !surface->w || !surface->h) {
		return;
	}

	int dx = (int)std::abs((double)(drawrect.x - ref.x));
	int dy = (int)std::abs((double)(drawrect.y - ref.y));

	index1.x = dx / tilemap->w;
	index1.y = dy / tilemap->h;

	index2.x = (dx + drawrect.w + tilemap->w - 1) / tilemap->w;
	index2.y = (dy + drawrect.h + tilemap->h - 1) / tilemap->h;

	SDL_GetClipRect(surface, const_cast<PG_Rect*>(&oldclip));
	SDL_SetClipRect(surface, const_cast<PG_Rect*>(&drawrect));

	PG_Rect src(0,0, tilemap->w, tilemap->h);
	PG_Rect dst = src;

	for(int y = index1.y; y < index2.y; y++) {
		for(int x = index1.x; x < index2.x; x++) {

			dst.x = ref.x + x * tilemap->w;
			dst.y = ref.y + y * tilemap->h;

			PG_Draw::BlitSurface(tilemap, src, surface, dst);
		}
	}

	SDL_SetClipRect(surface, const_cast<PG_Rect*>(&oldclip));
}
