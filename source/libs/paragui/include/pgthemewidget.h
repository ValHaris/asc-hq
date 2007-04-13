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
    Update Date:      $Date: 2007-04-13 16:15:57 $
    Source File:      $Source: /home/martin/asc/v2/svntest/games/asc/source/libs/paragui/include/pgthemewidget.h,v $
    CVS/RCS Revision: $Revision: 1.2 $
    Status:           $State: Exp $
*/

/** \file pgthemewidget.h
	Header file for the PG_ThemeWidget class.
*/

#ifndef PG_THEMEWIDGET_H
#define PG_THEMEWIDGET_H

#include "pgwidget.h"
#include "pgdraw.h"

class PG_ThemeWidgetDataInternal;

/**
	@author Alexander Pipelka
 
	@short Base class for themed widgets
 
	This class is the base class for all themed widgets.<br>
	It handles various combinations of gradients and background images.
 
	\anchor theme_PG_ThemeWidget
 
	<h2>Theme support</h2><p>
 
	<b>widget type:</b>		ThemeWidget (default)<br>
	<b>object name:</b>		ThemeWidget<br>
 
	<h3>Parent theme:</h3>
 
	\ref theme_PG_Widget "PG_Widget" theme<br>
 
	<h3>Theme sample:</h3>
 
	\verbatim
	<widget>
		<type value="ThemeWidget"/>
			<object>
			<name value="ThemeWidget"/>
			<filename name="background" value="default/back.bmp"/>
			<property name="backmode" value="TILE"/>
			<gradient name="gradient"
				color0="0x00B0B1B6"
				color1="0x0083838D"
				color2="0x007B7B83"
				color3="0x00585765"/>
			<property name="blend" value="200"/>
			<property name="transparency" value="0"/>
			<property name="bordersize" value="1"/>
		</object>
	</widget>
	\endverbatim
	<p>
	<h2>Code:</h2><p>
*/

class DECLSPEC PG_ThemeWidget : public PG_Widget  {
public:

	/**
	Create a new PG_ThemeWidget object
	@param parent	Pointer to parent widget
	@param r			Position of the widget (related to parent or screen)
	@param style		Widgetstyle to load. This style should be defined at your theme file (default = "ThemeWidget")
	This constructor creates a themed widget without any drawing surface.
	All drawing operations can be done via the eventBlit() callback handler.
	*/
	PG_ThemeWidget(PG_Widget* parent, const PG_Rect& r = PG_Rect::null, const std::string& style="ThemeWidget");

	/**
	Create a new PG_ThemeWidget object
	@param parent	Pointer to parent widget
	@param r			Position of the widget (related to parent or screen)
	@param bCreateSurface true - create a drawing surface for the widget.
	@param style		Widgetstyle to load. This style should be defined at your theme file (default = "ThemeWidget")
	This constructor creates a themed widget with an internal drawing surface.
	All drawing operations can be done via the eventDraw() callback handler.
	Additional blitting can be done via the eventBlit() callback. eventBlit() will NOT draw onto the
	internal drawing surface. Blitting will be done on the screen surface.
	*/
	PG_ThemeWidget(PG_Widget* parent, const PG_Rect& r, bool bCreateSurface, const std::string& style="ThemeWidget");

	/**
	*/
	~PG_ThemeWidget();

	/**
	Load a specific themestyle
	@param	widgettype		name of widget type to load
	*/
	void LoadThemeStyle(const std::string& widgettype);

	void LoadThemeStyle(const std::string& widgettype, const std::string& objectname);

	/**
	Load the background image from a file
	@param	filename		path to background image file
	@param	mode			BKMODE_TILE | BKMODE_STRETCH | BKMODE_3TILEH | BKMODE_3TILEV | BKMODE_9TILE
	*/
	/**  */
	bool SetBackground(const std::string& filename, PG_Draw::BkMode mode=PG_Draw::TILE);

	/**
	Load the background image from a file and set the colorkey
	@param	filename		path to background image file
	@param	colorkey		colorkey
	@param	mode			BKMODE_TILE | BKMODE_STRETCH | BKMODE_3TILEH | BKMODE_3TILEV | BKMODE_9TILE
	*/
	/**  */
	bool SetBackground(const std::string& filename, PG_Draw::BkMode mode, const PG_Color& colorkey);

	/**
	Define a surface as background image (will not be freed)
	@param	surface		pointer to background surface
	@param	mode			BKMODE_TILE | BKMODE_STRETCH | BKMODE_3TILEH | BKMODE_3TILEV | BKMODE_9TILE
	@return						true on success
	*/
	bool SetBackground(SDL_Surface* surface, PG_Draw::BkMode mode=PG_Draw::TILE);

	/**
	Defines the "blend-level" of gradient & background
	@param	backblend	0 - background image fully visible / 255 - gradient fully visible
	*/
	void SetBackgroundBlend(Uint8 backblend);

	/**
	Set the background gradient
	@param grad				a PG_Gradient structure describing the gradient
	*/
	void SetGradient(const PG_Gradient& grad);

	/**
	Get the background gradient
	@return the background gradient
	*/
	PG_Gradient GetGradient();

	/**
	Display an image in the widget
	@param filename		name of the image file to load
	@return				true on success
	With this function you can define an image to be displayed at the upper left corner
	of the widget. Image data will be erased when deleting the widget or loading / setting
	a new image.
	*/
	bool LoadImage(const std::string& filename);

	/**
	Display an image in the widget using a colorkey
	@param filename		name of the image file to load
	@param key			the colorkey (0xRRGGBB)
	@return				true on success
	With this function you can define an image with a colorkey to be displayed at the upper left corner
	of the widget. Image data will be erased when deleting the widget or loading / setting
	a new image.
	*/
	bool LoadImage(const std::string& filename, const PG_Color& key);

	/**
	Display an image in the widget
	@param image		pointer to surface to display
	@param bFreeImage    true if the image should be freed by the widget (default = true)
	@return				true on success
	With this function you can define an image to be displayed at the upper left corner
	of the widget.
	*/
	bool SetImage(SDL_Surface* image, bool bFreeImage = true);

	/**
	Set the fransparency of the widget
	@param t	transparency value (0 - opaque / 255 - fully transparent)
	@param bRecursive	if child widget's transparency should be set the same as widget
	*/
	void SetTransparency(Uint8 t, bool bRecursive = false);

	/**
	Create a 'themed' surface

	@param r					the dimensions of the surface to create
	@param gradient		pointer to a gradient structure (may be NULL)
	@param background	pointer to a background surface (may be NULL)
	@param bkmode		the mode how to fill in the background surface (BKMODE_TILE | BKMODE_STRETCH | BKMODE_3TILEH | BKMODE_3TILEV | BKMODE_9TILE)
	@param blend				the blend-level between gradient an background
	@return 						the newly created surface

	The generated surface is managed by the surfacecache. Don't delete these surface with SDL_FreeSurface. Please use DeleteThemedSurface.
	*/
	static SDL_Surface* CreateThemedSurface(const PG_Rect& r, PG_Gradient* gradient, SDL_Surface* background, PG_Draw::BkMode bkmode, Uint8 blend);

	/**
	Delete a themed surface
	@param	surface		pointer to surface
	*/
	static void DeleteThemedSurface(SDL_Surface* surface);

	void SetSimpleBackground(bool simple);

	void SetBackgroundColor(const PG_Color& c);

protected:

	/** */
	void eventSizeWidget(Uint16 w, Uint16 h);

	/** */
	void eventDraw(SDL_Surface* surface, const PG_Rect& rect);

	/** */
	void eventBlit(SDL_Surface* surface, const PG_Rect& src, const PG_Rect& dst);


	bool my_has_gradient;

	PG_Gradient my_gradient;

	SDL_Surface* my_background;

	SDL_Surface* my_image;

	Uint8 my_blendLevel;

	bool my_backgroundFree;

	PG_Draw::BkMode my_backgroundMode;

private:

	PG_ThemeWidget(const PG_ThemeWidget&);

	PG_ThemeWidget& operator=(const PG_ThemeWidget&);

	DLLLOCAL void Init(const std::string& style);

	DLLLOCAL void CreateSurface(Uint16 w = 0, Uint16 h = 0);

	/**
	This function frees background surface if possible
	*/
	DLLLOCAL void FreeSurface();

	DLLLOCAL void FreeImage();

	// this is a bit rude but neccessary for future binary compatibility
	// because adding non-static data members would break the ABI.
	// For this we put all private data into a dynamically created struct.
	PG_ThemeWidgetDataInternal* _mid;
};

#endif // PG_THEMEWIDGET_H
