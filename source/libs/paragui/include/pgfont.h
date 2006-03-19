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
    Update Date:      $Date: 2006-03-19 19:56:01 $
    Source File:      $Source: /home/martin/asc/v2/svntest/games/asc/source/libs/paragui/include/pgfont.h,v $
    CVS/RCS Revision: $Revision: 1.1.2.2 $
    Status:           $State: Exp $
*/

/** \file pgfont.h
	Header file for the PG_Font and PG_FontEngine classes.
*/

#ifndef PG_FONT_H
#define PG_FONT_H

#include "paragui.h"
#include "pgdatacontainer.h"
#include "pgcolor.h"
#include "pgrect.h"
#include "pgstring.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_ERRORS_H
#include FT_CACHE_H
#include FT_CACHE_IMAGE_H
#include FT_CACHE_SMALL_BITMAPS_H

#ifdef HASH_MAP_INC
#include HASH_MAP_INC
#endif
#include <map>

class PG_FontFaceCacheItem;
class PG_GlyphCacheItem;
struct PG_FontDataInternal;

/**
	@short font description class
 
	This class defines the parameters of a font (size, style, ...)
*/
class DECLSPEC PG_Font {
public:

	//! Font styles
	typedef enum {
	    NORMAL = 0x00,
	    BOLD = 0x01,
	    ITALIC = 0x02,
	    UNDERLINE = 0x04
	} Style;

	/**
	Construct a PG_Font object
	@param fontfile name of the fontfile to load
	@param size size of the new font
	@param index font index of the loaded file
	*/
	PG_Font(const std::string& fontfile, int size=14, int index=0);

   PG_Font( const PG_Font& font );

   PG_Font& operator= ( const PG_Font& font );
   
	/**
	*/
	virtual ~PG_Font();

	/**
	*/
	int GetFontAscender();

	/**
	*/
	int GetFontDescender();

	/**
	Get the absolute height of the font (in pixels)
	@return height of the font
	*/
	int GetFontHeight();

	/**
	Set the color of the font
	@param c PG_Color class of the new color
	*/
	void SetColor(const PG_Color& c);
   void SetHighlightColor(const PG_Color& c);

	/**
	Get the current color of the font
	@return PG_Color class containing the current color
	*/
	PG_Color GetColor();
   PG_Color GetHighlightColor();

	/**
	Set the transparency of the font
	@param a font alpha value
	*/
	void SetAlpha(int a);

	/**
	Get the transparency of the font
	@return font alpha value
	*/
	int GetAlpha();

	/**
	Set the size of the font
	@param s new font size
	*/
	void SetSize(int s);

	/**
	Get the size of the font
	@return current size of the font
	*/
	int GetSize();

	/**
	*/
	void SetStyle(Style s);

	/**
	*/
	Style GetStyle();

	/**
	Set the name (filename) of the font
	@param fontfile name of the new fontfile.
	@return true - on success
	This function loads a new font from a file
	*/
	bool SetName(const std::string& fontfile);

	/**
	Get the current name of the font (filename)
	@return the filename of the font
	*/
	const std::string& GetName();

	void SetIndex(int index);

	int GetIndex();

private:

   void copy( const PG_Font& font );
   void unlink();
   
	DLLLOCAL PG_FontFaceCacheItem* GetFaceCache(int index=0);

	PG_FontDataInternal* my_internaldata;

	friend class PG_FontEngine;
};


/**
	@author Alexander Pipelka / Ales Teska
	@short C++ Encapsulation of the FreeType engine
*/

class DECLSPEC PG_FontEngine {
public:

	/** */
	PG_FontEngine();

	/** */
	~PG_FontEngine();

	/**
	render text onto a surface
	@param Surface		destination for rendered text
	@param ClipRect		clipping rectangle
	@param BaseLineX	x-startposition of the rendered text (inside the surface)
	@param BaseLineY	baseline of the rendered text (inside the surface)
	@param Text			text to render
	@param ParamIn		pointer to font
	@return true on success
	*/
	static bool RenderText(SDL_Surface *Surface, const PG_Rect& ClipRect, int BaseLineX, int BaseLineY, const PG_String& Text, PG_Font* ParamIn);

	static bool RenderText(SDL_Surface *Surface, const PG_Rect *ClipRect, int BaseLineX, int BaseLineY, const PG_String& Text, PG_Font* ParamIn);

	/**
	*/
	static bool GetTextSize(const PG_String& Text, PG_Font* ParamIn, Uint16 *Width = NULL, Uint16 *Height = NULL, int *BaselineY = NULL, int *FontLineSkip = NULL, Uint16 *FontHeight = NULL, int *Ascent = NULL, int *Descent = NULL);

protected:

	/**
	*/
	static bool BlitFTBitmap(SDL_Surface *Surface, FT_Bitmap *Bitmap, int PosX, int PosY, PG_Font* Param, const PG_Rect *ClipRect);

	/**
	*/
	static void FontEngineError(FT_Error error);

private:

	/**
	Load a font face from the archive
	@param filename fontfile to load (truetype)
	@param fontsize size of the face in pixels
	@return pointer to the loaded PG_FontFaceCacheItem
	*/
	static PG_FontFaceCacheItem* LoadFontFace(const std::string& filename, FT_F26Dot6 fontsize, int index = 0);

	/**
	*/
	static PG_GlyphCacheItem* GetGlyph(PG_Font *Param, int glyph_index);

	typedef std::map<FT_F26Dot6, PG_FontFaceCacheItem*> MAP_SUBITEMS;

	class FONT_ITEM {
	public:
		FONT_ITEM() : memdata(NULL) {}
		;

		virtual ~FONT_ITEM();

		std::string name;
		PG_DataContainer* memdata;
		MAP_SUBITEMS subitems;
	};

	typedef std::map<std::string, FONT_ITEM*> MAP_FONTS;

	static MAP_FONTS my_fontcache;
	static FT_Library my_library;

	friend class PG_Font;
};

#endif
