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
    Source File:      $Source: /home/martin/asc/v2/svntest/games/asc/source/libs/paragui/include/pgimage.h,v $ 
    CVS/RCS Revision: $Revision: 1.1.2.2 $ 
    Status:           $State: Exp $ 
*/

/** \file pgimage.h
	Header file for the PG_Image class.
*/

#ifndef PG_IMAGE_H
#define PG_IMAGE_H

#include "pgthemewidget.h"

/**
	@author Jaroslav Vozab 
 
	@short A static (non-resizeable by the end user) frame with image. 
*/

class DECLSPEC PG_Image : public PG_ThemeWidget  {
   static PG_Rect CalcWidgetSize( const PG_Point& p, const SDL_Surface* image );
public:
	/**
	@short Contructor of the PG_Image class (loading from file)

	Creates the widget and loads the image from a file

	@param parent pointer to the parent widget or NULL
	@param p position of the PG_Image widget
	@param filename	image-file to load
	@param drawmode Image drawmode (see PG_Draw::BkMode)
	@param style widgetstyle to use

	*/
	PG_Image(PG_Widget* parent, const PG_Point& p, const std::string& filename, PG_Draw::BkMode drawmode = PG_Draw::TILE, const std::string& style = "ThemeWidget");

	/**
	@short Contructor of the PG_Image class (image from surface)

	Creates the widget and loads the image from an SDL_Surface	

	@param parent pointer to the parent widget or NULL
	@param p position of the PG_Image widget
	@param image pointer to imagedata (SDL_Surface)
	@param freeimage if true the imagedata is handled by the widget
	@param drawmode Image drawmode (see PG_Draw::BkMode)
	@param style widgetstyle to use
	*/
	PG_Image(PG_Widget* parent, const PG_Point& p, SDL_Surface* image, bool freeimage = true, PG_Draw::BkMode drawmode = PG_Draw::TILE, const std::string& style = "ThemeWidget");

	/**
	@short Contructor of the PG_Image class (loading from file)

	Creates the widget and loads the image from a file using a colorkey

	@param parent pointer to the parent widget or NULL
	@param p position of the PG_Image widget
	@param filename	image-file to load
	@param	colorkey colorkey (0xRRGGBB)
	@param drawmode Image drawmode (see PG_Draw::BkMode)
	@param style widgetstyle to use

	*/
	PG_Image(PG_Widget* parent, const PG_Point& p, const std::string& filename, Uint32 colorkey, PG_Draw::BkMode drawmode = PG_Draw::TILE, const std::string& style = "ThemeWidget");

	/**
	@short Set the colorkey of the image

	@param key		colorkey (0xRRGGBB);
	*/
	void SetColorKey(const PG_Color& key);

	/**
	@ Set mode of drawing (BKMODE_TILE | BKMODE_STRETCH | BKMODE_3TILEH | BKMODE_3TILEV | BKMODE_9TILE)
	*/
	void SetDrawMode(PG_Draw::BkMode mode);

	Uint8 GetDrawMode() {
		return my_DrawMode;
	};

	bool LoadImage(const std::string& filename);

	bool LoadImage(const std::string& filename, const PG_Color& key);

	bool SetImage(SDL_Surface* image, bool bFreeImage = true);

	~PG_Image();

protected:

	SDL_Surface* my_cachedSrf;

	PG_Draw::BkMode my_DrawMode;

	/** */
	void eventDraw(SDL_Surface* surface, const PG_Rect& rect);

	/** */
	void eventBlit(SDL_Surface* surface, const PG_Rect& src, const PG_Rect& dst);
};

#endif // PG_IMAGE_H
