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
    Update Date:      $Date: 2007-02-18 19:25:54 $ 
    Source File:      $Source: /home/martin/asc/v2/svntest/games/asc/source/libs/paragui/src/widgets/pgimage.cpp,v $ 
    CVS/RCS Revision: $Revision: 1.1.2.2 $ 
    Status:           $State: Exp $ 
*/

#include "pgimage.h"


PG_Rect PG_Image::CalcWidgetSize( const PG_Point& p, const SDL_Surface* image )
{
   if ( image )
      return PG_Rect (p.x, p.y, image->w, image->h );
   else
      return PG_Rect (p.x, p.y, 1,1 );
}


PG_Image::PG_Image(PG_Widget* parent, const PG_Point& p, const std::string& filename, Uint32 colorkey, PG_Draw::BkMode drawmode, const std::string& style) : PG_ThemeWidget(parent, CalcWidgetSize(p, NULL), style), my_cachedSrf(NULL), my_DrawMode(drawmode) {
	LoadImage(filename, colorkey);

	if(my_image != NULL && drawmode == PG_Draw::TILE) {
		SizeWidget(my_image->w, my_image->h);
	}
}

PG_Image::PG_Image(PG_Widget* parent, const PG_Point& p, const std::string& filename, PG_Draw::BkMode drawmode, const std::string& style) : PG_ThemeWidget(parent, CalcWidgetSize(p, NULL), style), my_cachedSrf(NULL), my_DrawMode(drawmode) {
	LoadImage(filename);

	if(my_image != NULL && drawmode == PG_Draw::TILE) {
		SizeWidget(my_image->w, my_image->h);
	}
}



PG_Image::PG_Image(PG_Widget* parent, const PG_Point& p, SDL_Surface* image, bool freeimage, PG_Draw::BkMode drawmode, const std::string& style) : PG_ThemeWidget(parent, CalcWidgetSize(p, image), style), my_cachedSrf(NULL), my_DrawMode(drawmode) {
	SetImage(image, freeimage);

	if(my_image != NULL && drawmode == PG_Draw::TILE) {
		SizeWidget(my_image->w, my_image->h);
	}
}

PG_Image::~PG_Image() {
	DeleteThemedSurface(my_cachedSrf);
}

void PG_Image::eventDraw(SDL_Surface* surface, const PG_Rect& rect) {}

void PG_Image::eventBlit(SDL_Surface* srf, const PG_Rect& src, const PG_Rect& dst) {

	if(my_cachedSrf != NULL) {
		PG_Rect my_src;
		PG_Rect my_dst;

		GetClipRects(my_src, my_dst, *this);
		PG_Widget::eventBlit(my_cachedSrf, my_src, my_dst);
		return;
	}

	if(my_image == NULL) {
		return;
	}

	if(my_image->w != 0 && my_image->h != 0) {
		PG_Rect my_src;
		PG_Rect my_dst;

		GetClipRects(my_src, my_dst, *this);
		if(my_DrawMode != PG_Draw::STRETCH) {
			PG_Widget::eventBlit(my_image, my_src, my_dst);
			return;
		}

		my_cachedSrf = CreateThemedSurface(PG_Rect(0, 0, my_width, my_height), 0, my_background, my_DrawMode, my_blendLevel);
		PG_Draw::DrawThemedSurface(my_cachedSrf, PG_Rect(0, 0, my_src.w, my_src.h), 0, my_image, my_DrawMode, my_blendLevel);

		//PG_Widget::eventBlit(my_cachedSrf, my_src, my_dst);
		eventBlit(srf, src, dst);
	}
}

void PG_Image::SetColorKey(const PG_Color& key) {
	SDL_SetColorKey(my_image, SDL_SRCCOLORKEY, key);
	DeleteThemedSurface(my_cachedSrf);
	my_cachedSrf = NULL;
}

void PG_Image::SetDrawMode(PG_Draw::BkMode mode) {
	if(mode != my_DrawMode) {
		my_DrawMode = mode;
		DeleteThemedSurface(my_cachedSrf);
		my_cachedSrf = NULL;
	}
}

bool PG_Image::LoadImage(const std::string& filename) {
	DeleteThemedSurface(my_cachedSrf);
	my_cachedSrf = NULL;

	if(PG_ThemeWidget::LoadImage(filename)) {
		return true;
	}

	return false;
}

bool PG_Image::LoadImage(const std::string& filename, const PG_Color& key) {
	DeleteThemedSurface(my_cachedSrf);
	my_cachedSrf = NULL;

	if(PG_ThemeWidget::LoadImage(filename, key)) {
		return true;
	}

	return false;
}

bool PG_Image::SetImage(SDL_Surface* image, bool bFreeImage) {
	DeleteThemedSurface(my_cachedSrf);
	my_cachedSrf = NULL;

	if(PG_ThemeWidget::SetImage(image, bFreeImage)) {
		return true;
	}

	return false;
}
