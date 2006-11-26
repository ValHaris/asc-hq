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
    Update Date:      $Date: 2006-11-26 20:38:05 $
    Source File:      $Source: /home/martin/asc/v2/svntest/games/asc/source/libs/paragui/src/font/pgfont.cpp,v $
    CVS/RCS Revision: $Revision: 1.1.2.3 $
    Status:           $State: Exp $
*/

#include "pgfont.h"
#include "pglog.h"
#include "pgfilearchive.h"
#include "facecache.h"
#include "pgstring.h"

//#define OLD_TEXTRENDERING

//SDL_Surface* PG_FontEngine::my_charSurface = NULL;
PG_FontEngine::MAP_FONTS PG_FontEngine::my_fontcache;
FT_Library PG_FontEngine::my_library;

PG_FontEngine::PG_FontEngine() {
	FT_Init_FreeType(&my_library);
}

PG_FontEngine::~PG_FontEngine() {

	// clean fontcache
	for(MAP_FONTS::iterator i = my_fontcache.begin(); i != my_fontcache.end(); i++) {
		delete (*i).second;
	}
	my_fontcache.clear();

	FT_Done_FreeType(my_library);

	/*if (my_charSurface != NULL) {
		SDL_FreeSurface(my_charSurface);
	}*/
}

void PG_FontEngine::FontEngineError(FT_Error error) {
#undef __FTERRORS_H__
#define FT_ERRORDEF( e, v, s )  { e, s },
#define FT_ERROR_START_LIST  {
#define FT_ERROR_END_LIST    { -1, NULL } };
	const struct {
		int		err_code;
		const char	*err_msg;
	}
	ft_errors[] =
#include FT_ERRORS_H

	    int	i = 0;
	while (ft_errors[i].err_code != -1) {
		if (ft_errors[i].err_code == error) {
			PG_LogWRN("FreeType error %d : %s",error,ft_errors[i].err_msg);
			return;
		}
		i++;
	}

	PG_LogWRN("FreeType : Unknown error : %d", error);
	return;
}

/*bool PG_FontEngine::PG_GetFontFace(PG_Font* Param) {
	PG_FontFaceCacheItem* CacheItem = LoadFontFace(
	                                      Param->GetName(),
	                                      Param->GetSize());
 
	Param->SetFaceCache(CacheItem);
 
	return (Param->GetFaceCache() != NULL);
}*/

PG_GlyphCacheItem* PG_FontEngine::GetGlyph(PG_Font *Param, int glyph_index) {

	PG_FontFaceCacheItem* facecache = Param->GetFaceCache();
	PG_GlyphCacheItem *GlyphCacheItem = facecache->GlyphCache[glyph_index];

	if(GlyphCacheItem != NULL) {
		return GlyphCacheItem;
	}

	FT_Face face = facecache->Face;

	if (FT_Load_Glyph(face, glyph_index, FT_LOAD_RENDER ))
		return NULL;

	Uint32 bitmapsize = face->glyph->bitmap.pitch * face->glyph->bitmap.rows;
	GlyphCacheItem = new PG_GlyphCacheItem(bitmapsize);

	GlyphCacheItem->Glyph_Index = glyph_index;
	GlyphCacheItem->Bitmap = face->glyph->bitmap;
	GlyphCacheItem->Bitmap_left = face->glyph->bitmap_left;
	GlyphCacheItem->Bitmap_top = face->glyph->bitmap_top;
	GlyphCacheItem->Advance_x = FT_CEIL(face->glyph->metrics.horiAdvance);
	memcpy(GlyphCacheItem->data(), face->glyph->bitmap.buffer, bitmapsize);
	GlyphCacheItem->Bitmap.buffer = (unsigned char*)GlyphCacheItem->data();

	facecache->GlyphCache[glyph_index] = GlyphCacheItem;
	return GlyphCacheItem;
}

#ifndef OLD_TEXTRENDERING

template < class DT >
inline void BlitTemplate(DT pixels, SDL_Surface* Surface, FT_Bitmap *Bitmap, int PosX, int PosY, int x0, int x1, int y0, int y1, PG_Font *Param) {
	int xw = x1-x0;

	SDL_PixelFormat* format = Surface->format;
	Uint8 Rloss = format->Rloss;
	Uint8 Gloss = format->Gloss;
	Uint8 Bloss = format->Bloss;
	Uint8 Aloss = format->Aloss;
	Uint8 Rloss8 = 8-Rloss;
	Uint8 Gloss8 = 8-Gloss;
	Uint8 Bloss8 = 8-Bloss;
	Uint8 Aloss8 = 8-Aloss;
	Uint8 Rshift = format->Rshift;
	Uint8 Gshift = format->Gshift;
	Uint8 Bshift = format->Bshift;
	Uint8 Ashift = format->Ashift;
	Uint32 Rmask = format->Rmask;
	Uint32 Gmask = format->Gmask;
	Uint32 Bmask = format->Bmask;
	Uint32 Amask = format->Amask;

	Uint8 bpp = format->BytesPerPixel;
	Uint32 pitch = Surface->pitch;
	Uint32 src_pitch = Bitmap->pitch;
	register Uint8* src_pixels = Bitmap->buffer + x0 + y0*Bitmap->pitch;
	register Uint8* dst_pixels = (Uint8*)pixels + (PosX+x0)*bpp + (PosY+y0)*pitch /*+ Surface->offset*/;
	Uint8* line;

	Uint8 r,g,b,a;
	unsigned rv,gv,bv,av;
	Sint32 cr,cg,cb;
	Uint32 color = 0;
	Sint32 v;

	PG_Color fc = Param->GetColor();
	cr = fc.r;
	cg = fc.g;
	cb = fc.b;

	int alpha = Param->GetAlpha();

	line = dst_pixels;
	for (register int y = y0; y <y1; y++, src_pixels += src_pitch) {

		dst_pixels = line;

		for (register int x = x0; x < x1; x++, dst_pixels += bpp) {

			// get source pixel value
			v = *(Uint8 *)(src_pixels++);

			// don't do anything if the pixel is fully transparent
			if(v == 0) {
				continue;
			}

			if(alpha != 255) {
				v = (v * alpha) >> 8;
			}

			// Get the pixel
			color = *((DT) (dst_pixels));
			switch(Surface->format->BytesPerPixel) {
				default:
					// get the RGBA values
					rv = (color & Rmask) >> Rshift;
					r = (rv << Rloss) + (rv >> Rloss8);
					gv = (color & Gmask) >> Gshift;
					g = (gv << Gloss) + (gv >> Gloss8);
					bv = (color & Bmask) >> Bshift;
					b = (bv << Bloss) + (bv >> Bloss8);
					if(Amask) {
						av = (color & Amask) >> Ashift;
						a = (av << Aloss) + (av >> Aloss8);
					} else
						a = SDL_ALPHA_OPAQUE;

					//SDL_GetRGBA(color, format, &r, &g, &b, &a);

					// calculate new RGBA values
					if(v == 255) {
						r = cr;
						g = cg;
						b = cb;
					} else {
						//r += ((cr - r) * v) / 255;
						//g += ((cg - g) * v) / 255;
						//b += ((cb - b) * v) / 255;
						r += ((cr - r) * v) >> 8;
						g += ((cg - g) * v) >> 8;
						b += ((cb - b) * v) >> 8;
					}

					// if the destination pixel is full transparent
					// use the pixel shading as alpha
					if(a == 0) {
						a = v;
					}

					// get the destination color
					color = (r >> Rloss) << Rshift
					        | (g >> Gloss) << Gshift
					        | (b >> Bloss) << Bshift
					        | ((a >> Aloss) << Ashift & Amask);
					// Set the pixel
					*((DT) (dst_pixels)) = color;
					break;

				case 3:
					cr = (fc.r << format->Rshift) >> 16 & 0xff;
					cg = (fc.g << format->Gshift) >> 8 & 0xff;
					cb = fc.b << format->Bshift & 0xff;

					if (v == 255) {
						r = cr;
						g = cg;
						b = cb;
					}
					// calculate new RGB values
					else {
						b = *(dst_pixels);
						g = *(dst_pixels+1);
						r = *(dst_pixels+2);
						r += ((cr - r) * v) >> 8;
						g += ((cg - g) * v) >> 8;
						b += ((cb - b) * v) >> 8;
					}

					*dst_pixels = b;
					*(dst_pixels + 1) = g;
					*(dst_pixels + 2) = r;
					break;

				case 1:
					SDL_GetRGBA(color, format, &r, &g, &b, &a);

					// calculate new RGBA values
					if(v == 255) {
						r = cr;
						g = cg;
						b = cb;
					} else {
						//r += ((cr - r) * v) / 255;
						//g += ((cg - g) * v) / 255;
						//b += ((cb - b) * v) / 255;
						r += ((cr - r) * v) >> 8;
						g += ((cg - g) * v) >> 8;
						b += ((cb - b) * v) >> 8;
					}

					// if the destination pixel is full transparent
					// use the pixel shading as alpha
					if(a == 0) {
						a = v;
					}
					color = SDL_MapRGBA(format, r,g,b, a);
					*((DT) (dst_pixels)) = color;
					break;
			}

		}
		src_pixels -= xw;
		line += pitch;
	}
}

bool PG_FontEngine::BlitFTBitmap(SDL_Surface *Surface, FT_Bitmap *Bitmap, int PosX, int PosY, PG_Font *Param, const PG_Rect* ClipRect) {
	int BitmapRealWidth;

	// i think we can skip that test
	/*if(Bitmap->pixel_mode != ft_pixel_mode_grays) {
		return false;
	}*/

	// do nothing if we're fully transparent
	if (Param->GetAlpha() == 0)
		return true;

	BitmapRealWidth = Bitmap->width;

	// get the cliprectangle of the surface
	static PG_Rect srfclip;
	SDL_GetClipRect(Surface, &srfclip);

	// the real clipping rectangle = surfaceclip  / ClipRect
	static PG_Rect clip;
	clip = *ClipRect / srfclip;

	//Italic font is widther than normal
	//if (Param->Style & PG_FSTYLE_ITALIC) {
	//	BitmapRealWidth += (int)(Bitmap->rows * PG_FITALIC_ANGLE);
	//}

	int x0 = 0;
	int x1 = BitmapRealWidth;
	int y0 = 0;
	int y1 = Bitmap->rows;

	if(PosX < clip.x) {
		x0 = clip.x - PosX;
	}

	if(PosX+BitmapRealWidth > clip.x + clip.w) {
		x1 = (clip.x + clip.w) - PosX;
	}

	if(PosY < clip.y) {
		y0 = clip.y - PosY;
	}

	if(PosY+Bitmap->rows > clip.y + clip.h) {
		y1 = (clip.y + clip.h) - PosY;
	}

	if((x1 <= x0) || (y1 <= y0)) {
		return false;
	}

	switch(Surface->format->BytesPerPixel) {
		case 1:
		case 3:
			BlitTemplate((Uint8*)Surface->pixels, Surface, Bitmap, PosX, PosY, x0, x1, y0, y1, Param);
			break;
		case 2:
			BlitTemplate((Uint16*)Surface->pixels, Surface, Bitmap, PosX, PosY, x0, x1, y0, y1, Param);
			break;
		case 4:
			BlitTemplate((Uint32*)Surface->pixels, Surface, Bitmap, PosX, PosY, x0, x1, y0, y1, Param);
			break;
		default:
			PG_LogWRN("Unable to draw font: unsupported bit depth!");
			break;
	}

	return true;
}

#else

bool PG_FontEngine::BlitFTBitmap(SDL_Surface *Surface, FT_Bitmap *Bitmap, int PosX, int PosY, PG_Font *Param, PG_Rect *ClipRect) {
	int		x,y;
	Uint32		*raw_pixels;
	SDL_Rect	TargetPos;
	SDL_Rect	SourcePos;
	int		BitmapRealWidth;

	if (Param->Alpha == 0)
		return true;

	TargetPos.x = PosX;
	TargetPos.y = PosY;

	//Italic font is widther than normal
	BitmapRealWidth = Bitmap->width;
	if (Param->Style & PG_FSTYLE_ITALIC) {
		BitmapRealWidth += (int)(Bitmap->rows * PG_FITALIC_ANGLE);
	}

	//There is no need to draw
	if ((TargetPos.x > (ClipRect->x + ClipRect->w)) || (TargetPos.y > (ClipRect->y +ClipRect->h )) || ((TargetPos.y+Bitmap->rows) < ClipRect->y) || ((TargetPos.x+BitmapRealWidth) < ClipRect->x))
		return true;

	//Is my_charSurface big enough ??
	if (my_charSurface != NULL) {
		if ((my_charSurface->w < BitmapRealWidth)||(my_charSurface->h < Bitmap->rows)) {
			SDL_FreeSurface(my_charSurface);
			my_charSurface = NULL;
		}
	}

	if (my_charSurface == NULL) {
		my_charSurface = SDL_CreateRGBSurface(SDL_SRCALPHA , BitmapRealWidth, Bitmap->rows, 32, 0xff0000, 0xff00, 0xff, 0xff000000);
		if (my_charSurface == NULL) {
			PG_LogWRN("Can`t get char surface : %s",SDL_GetError());
			return false;
		}
	}

	if (Param->Style & PG_FSTYLE_ITALIC) {
		SDL_FillRect(my_charSurface, NULL, 0);
	}

	SourcePos.x = 0; // = my_charSurface->clip_rect;
	SourcePos.y = 0;
	SourcePos.w = BitmapRealWidth;
	SourcePos.h = Bitmap->rows;

	//Clipping
	if ((TargetPos.x + SourcePos.w) > (ClipRect->x + ClipRect->w))
		SourcePos.w = ClipRect->w - (TargetPos.x - ClipRect->x);
	if ((TargetPos.y + SourcePos.h) > (ClipRect->y + ClipRect->h))
		SourcePos.h = ClipRect->h - (TargetPos.y - ClipRect->y);
	if (TargetPos.x < ClipRect->x) {
		int	delta;

		delta = ClipRect->x - TargetPos.x;
		SourcePos.w -= delta;
		TargetPos.x += delta;
		SourcePos.x += delta;
	}

	if (TargetPos.y < ClipRect->y) {
		int	delta;

		delta = ClipRect->y - TargetPos.y;
		SourcePos.h -= delta;
		TargetPos.y += delta;
		SourcePos.y += delta;
	}

	raw_pixels = (Uint32 *) my_charSurface->pixels;
	Uint32 pitch_diff = 0;

	switch (Bitmap->pixel_mode) {
		case ft_pixel_mode_grays: {
				Uint8	*SrcPix = Bitmap->buffer;
				int	a;


				if (Param->Style & PG_FSTYLE_ITALIC) {
					double ioffset = Bitmap->rows * PG_FITALIC_ANGLE;

					for (y = 0; y < Bitmap->rows; y++, SrcPix += Bitmap->pitch, ioffset -= PG_FITALIC_ANGLE) {
						for (x = 0; x <  Bitmap->width; x++) {
							a = *(Uint8 *)(SrcPix++);

							// What the hell should do this ?
							//if (Param->Alpha != 255)
							//	a = (a * Param->Alpha) / 255;

							raw_pixels[int(ioffset) + x + (my_charSurface->pitch/4)*(y)] = Param->Color.MapRGBA(my_charSurface->format, a);
						}
						SrcPix -= x;
					}
				} else
					pitch_diff = (my_charSurface->pitch/4);

				for (y = 0; y < Bitmap->rows; y++, SrcPix += Bitmap->pitch) {
					for (x = 0; x <  Bitmap->width; x++) {
						a = *(Uint8 *)(SrcPix++);

						// Q: What the hell should do this ?
						// A: This allows for alpha
						// rendering of text. // Neo

						if (Param->Alpha != 255)
							a = (a * Param->Alpha) / 255;

						*raw_pixels = Param->Color.MapRGBA(my_charSurface->format, a);
						raw_pixels++;
					}
					SrcPix -= x;
					raw_pixels -= x;
					raw_pixels += pitch_diff;
				}
				break;
			}

		default:
			PG_LogWRN("Unknown pixel type in font !");
			return false;
	}

	// Final blit
	SDL_Rect BoldTarget = TargetPos;
	SDL_BlitSurface(my_charSurface, &SourcePos, Surface, &TargetPos);

	//BOLD
	if (Param->Style & PG_FSTYLE_BOLD) {
		BoldTarget.x += Param->FaceCache->Bold_Offset;
		SDL_BlitSurface(my_charSurface, &SourcePos, Surface, &BoldTarget);
	}

	return true;
}

#endif


bool PG_FontEngine::RenderText(SDL_Surface *Surface, const PG_Rect& ClipRect, int BaseLineX, int BaseLineY, const PG_String& Text, PG_Font *ParamIn) {
	return RenderText(Surface, (PG_Rect*)&ClipRect, BaseLineX, BaseLineY, Text, ParamIn);
}

bool PG_FontEngine::RenderText(SDL_Surface *Surface, const PG_Rect *ClipRect, int BaseLineX, int BaseLineY, const PG_String& Text, PG_Font* font) {
	static bool bRecursion = false;
	int OriBaseX = BaseLineX;
	FT_UInt previous = 0;

	PG_FontFaceCacheItem* FaceCache = font->GetFaceCache();

	// invalid font ?
	if (FaceCache == NULL) {
		return false;
	}

	FT_Face Face = FaceCache->Face;
	FT_Vector  delta;

	if(SDL_MUSTLOCK(Surface)) {
		SDL_LockSurface(Surface);
	}

	Uint32 c0;

	//Go thu text and draw characters
	int len = Text.size();
	for(int i = 0; i < len; i++) {
		int glyph_index;
		PG_GlyphCacheItem* Glyph;
		int OldBaseLineX = BaseLineX;

		c0 = PG_Char(Text[i]);

		//Skip drawing we go non-printable char
		if (c0 < 32) {
			continue;
		}

		//Get glyph index
		glyph_index = FT_Get_Char_Index(Face, c0);

		//Make space between characters == kerneling
		if ( FaceCache->Use_Kerning && previous && glyph_index ) {
			FT_Get_Kerning(Face, previous, glyph_index, ft_kerning_default, &delta );
			BaseLineX += delta.x >> 6;
		}
		previous = glyph_index;

		//Get glyph bitmap
		Glyph = GetGlyph(font, glyph_index);

		//Blit glyph bitmap into the surface
		if (c0 != ' ') {
			BlitFTBitmap(Surface, &Glyph->Bitmap, BaseLineX + Glyph->Bitmap_left, BaseLineY - Glyph->Bitmap_top, font, ClipRect);
		}

		BaseLineX += Glyph->Advance_x;
		if (font->GetStyle() & PG_Font::BOLD) {
			BaseLineX += FaceCache->Bold_Offset;
		}

		//UNDERLINE
		//TO-DO : Underline is not transparent !!!! (Fill must be replaced by Blit)
		if (font->GetStyle() & PG_Font::UNDERLINE) {
			SDL_Rect	und_rect;

			und_rect.x = OldBaseLineX;
			und_rect.y = BaseLineY;
			und_rect.h = FaceCache->Underline_Height;
			und_rect.w = BaseLineX - OldBaseLineX;

			SDL_FillRect(
			    Surface,
			    &und_rect,
			    font->GetColor().MapRGB(Surface->format)
			);
		}
	}

	//BOLD
	if (font->GetStyle() & PG_Font::BOLD && !bRecursion) {
		bRecursion = true;
		RenderText(Surface, ClipRect, OriBaseX+1, BaseLineY, Text, font);
		bRecursion = false;
	}

	if(SDL_MUSTLOCK(Surface)) {
		SDL_UnlockSurface(Surface);
	}

	return true;
}

bool PG_FontEngine::GetTextSize(const PG_String& Text, PG_Font* font, Uint16 *Width, Uint16 *Height, int *BaselineY, int *FontLineSkip, Uint16 *FontHeight, int *Ascent, int *Descent) {
	FT_UInt			previous = 0;
	int				BaseLineX = 0;
	int				preBaseLineY = 0;
	int				MaxY = 0;

	Uint16			preFontHeight = 0;
	int				preLineSkip = 0;
	int				preAscent = 0;
	int				preDescent = 0;

	PG_FontFaceCacheItem* FaceCache = font->GetFaceCache();

	// invalid font ?
	if (FaceCache == NULL) {
		return false;
	}

	FT_Face Face = FaceCache->Face;

	//Initial parametrs check
	if (FaceCache != NULL) {
		if (preFontHeight < FaceCache->Height)
			preFontHeight = FaceCache->Height;
		if (preLineSkip < FaceCache->LineSkip)
			preLineSkip = FaceCache->LineSkip;
		if (preAscent < FaceCache->Ascent)
			preAscent = FaceCache->Ascent;
		if (preDescent > FaceCache->Descent)
			preDescent = FaceCache->Descent;
	}

	//Go thu text and get sizes of the characters

	Uint32 c0;

	int len = Text.size();
	for(int i = 0; i < len; i++) {
		int glyph_index;
		PG_GlyphCacheItem	*Glyph;

		c0 = Text[i];

		//Skip non-printable char
		if (c0 < 32) {
			continue;
		}

		//Get glyph index
		glyph_index = FT_Get_Char_Index(Face, c0);

		//Make space between characters == kerneling
		if ( FaceCache->Use_Kerning && previous && glyph_index ) {
			FT_Vector  delta;

			FT_Get_Kerning(Face, previous, glyph_index, ft_kerning_default, &delta );
			BaseLineX += delta.x >> 6;
		}
		previous = glyph_index;

		//Get glyph bitmap
		Glyph = GetGlyph(font, glyph_index);

		if (preBaseLineY < Glyph->Bitmap_top) {
			preBaseLineY = Glyph->Bitmap_top;
		}

		if (MaxY < Glyph->Bitmap.rows) {
			MaxY = Glyph->Bitmap.rows;
		}

		BaseLineX += Glyph->Advance_x;
		if (font->GetStyle() & PG_Font::BOLD) {
			BaseLineX += FaceCache->Bold_Offset;
		}

	}

	if (Height != NULL)
		*Height = MaxY;
	if (Width != NULL)
		*Width = BaseLineX;
	if (BaselineY!= NULL)
		*BaselineY = preBaseLineY;
	if (FontLineSkip!= NULL)
		*FontLineSkip = preLineSkip;
	if (FontHeight != NULL)
		*FontHeight = preFontHeight;
	if (Ascent != NULL)
		*Ascent = preAscent;
	if (Descent != NULL)
		*Descent = preDescent;

	return true;
}

PG_FontFaceCacheItem* PG_FontEngine::LoadFontFace(const std::string&  filename, FT_F26Dot6 fontsize, int index) {

	// lets see if the file is already in the cache
	FONT_ITEM* item = my_fontcache[filename];

	// NO -> Load the face from the file
	if(item == NULL) {

		// open the fontfile
		PG_DataContainer* data = PG_FileArchive::ReadFile(filename);
		if(!data) {
			return NULL;
		}

		// create new font item
		item = new FONT_ITEM;
		item->name = filename;
		item->memdata = data;

		// add the face to the cache
		my_fontcache[filename] = item;
	}

	// let's see if we already have a face for the given size
	PG_FontFaceCacheItem* subitem = item->subitems[fontsize];

	// NO -> create new face for the size
	if(subitem == NULL) {
		subitem = new PG_FontFaceCacheItem;
		subitem->fontsize = fontsize;

		// create the freetype face
		FT_New_Memory_Face(my_library, (FT_Byte*)item->memdata->data(),
		                   item->memdata->size(), 0,
		                   &(subitem->Face));

		// check if the font is scaleable
		if (!FT_IS_SCALABLE(subitem->Face) ) {
			PG_LogWRN("Font %s is not scalable !", filename.c_str());
			delete subitem;
			return NULL;
		}

		// set font size
		FT_Set_Char_Size(subitem->Face, 0, fontsize*64, 0, 0);

		// set subitem params
		subitem->Bold_Offset = 1 + fontsize / 20;
		subitem->Underline_Height = FT_FLOOR(FT_MulFix(subitem->Face->underline_thickness, subitem->Face->size->metrics.y_scale));
		if ( subitem->Underline_Height < 1 ) {
			subitem->Underline_Height = 1;
		}

		subitem->Ascent = FT_CEIL(FT_MulFix(subitem->Face->bbox.yMax, subitem->Face->size->metrics.y_scale));
		subitem->Descent  = FT_CEIL(FT_MulFix(subitem->Face->bbox.yMin, subitem->Face->size->metrics.y_scale));
		subitem->Height = subitem->Ascent - subitem->Descent + 1;
		subitem->LineSkip = FT_CEIL(FT_MulFix(subitem->Face->height, subitem->Face->size->metrics.y_scale));
		subitem->Use_Kerning = FT_HAS_KERNING(subitem->Face);

		// add to subitem list
		item->subitems[fontsize] = subitem;
	}

	return subitem;
}

PG_FontEngine::FONT_ITEM::~FONT_ITEM() {
	for(MAP_SUBITEMS::iterator i = subitems.begin(); i != subitems.end(); i++) {
		delete (*i).second;
	}
	delete memdata;
}


/*
 * Local Variables:
 * c-basic-offset: 8
 * End:
 */


