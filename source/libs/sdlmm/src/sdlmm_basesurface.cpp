/*
 * SDLmm - a C++ wrapper for SDL and related libraries
 * Copyright � 2001 David Hedbor <david@hedbor.org>
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include "sdlmm_config.h"

#include <SDL.h>
#include "sdlmm_basesurface.h"

// The SDLmm::BaseSurface class.

namespace SDLmm {

  bool BaseSurface::Lock() {
    if(SDL_MUSTLOCK(me)) {
      return SDL_LockSurface(me) == 0;
    }
    else return true;
  }

  void BaseSurface::Unlock() {
    if(SDL_MUSTLOCK(me)) {
      SDL_UnlockSurface(me);
    }
  }

  void BaseSurface::SetPixel1(int x, int y, Color color) {
    // ASSERT(color < GetPixelFormat()->???);
    ASSERT(x >= 0);
    ASSERT(x < w());
    ASSERT(y >= 0);
    ASSERT(y < h());
    ASSERT(GetPixelFormat().BytesPerPixel() == 1);
    *((Uint8 *)pixels() + y * pitch() + x) = color;
  }

  void BaseSurface::SetPixel2(int x, int y, Color color) {
    // ASSERT(color < GetPixelFormat()->???);
    ASSERT(x >= 0);
    ASSERT(x < w());
    ASSERT(y >= 0);
    ASSERT(y < h());
    ASSERT(GetPixelFormat().BytesPerPixel() == 2);
    *((Uint16 *)pixels() + y * pitch()/2 + x) = color;
  }

  void BaseSurface::SetPixel3(int x, int y, Color color) {
    // ASSERT(color < GetPixelFormat()->???);
    ASSERT(x >= 0);
    ASSERT(x < w());
    ASSERT(y >= 0);
    ASSERT(y < h());
    ASSERT(GetPixelFormat().BytesPerPixel() == 3);
    Uint8 *pix = (Uint8 *)pixels() + y * pitch() + x * 3;

    if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
      pix[2] = color;
      pix[1] = color >> 8;
      pix[0] = color >> 16;
    } else {
      pix[0] = color;
      pix[1] = color >> 8;
      pix[2] = color >> 16;
    }
  }

  void BaseSurface::SetPixel4(int x, int y, Color color) {
    // ASSERT(color < GetPixelFormat()->???);
    ASSERT(x >= 0);
    ASSERT(x < w());
    ASSERT(y >= 0);
    ASSERT(y < h());
    ASSERT(GetPixelFormat().BytesPerPixel() == 4);
    *((Uint32 *)pixels() + y * pitch()/4 + x) = color;
  }

  void BaseSurface::SetPixel(int x, int y, Color color) {
    // ASSERT(color < GetPixelFormat()->???);
    ASSERT(x >= 0);
    ASSERT(x < w());
    ASSERT(y >= 0);
    ASSERT(y < h());

    switch(GetPixelFormat().BytesPerPixel())
    {
    case 1:
      SetPixel1(x, y, color);
      break;

    case 2:
      SetPixel2(x, y, color);
      break;

    case 3:
      SetPixel3(x, y, color);
      break;

    case 4:
      SetPixel4(x, y, color);
      break;

    default:
      ASSERT(false);
      break;
    }
  }

  Color BaseSurface::GetPixel(int x, int y) const {
    ASSERT(x >= 0);
    ASSERT(x < w());
    ASSERT(y >= 0);
    ASSERT(y < h());

    PixelFormat	pf(GetPixelFormat());

    const Uint8* the_pixel = (const Uint8*)pixels() + y * pitch() + x * pf.BytesPerPixel();

    switch(pf.BytesPerPixel())
    {
    case 1:
      return *((const Uint8 *)the_pixel);
      break;

    case 2:
      return *((const Uint16 *)the_pixel);
      break;

    case 3:
      { /* Format/endian independent */
        Uint32 r, g, b;
        Color color;
        r = *(the_pixel + pf.Rshift()/8);
        g = *(the_pixel + pf.Gshift()/8);
        b = *(the_pixel + pf.Bshift()/8);
        color = r << pf.Rshift();
        color |= g << pf.Gshift();
        color |= b << pf.Bshift();
        return color;
      }
      break;

    case 4:
      return *((const Uint32 *)(the_pixel));
      break;

    default:
      ASSERT(false);
      return 0;
      break;
    }  
  }

  int BaseSurface::Blit(const BaseSurface& src, const SDL_Rect& srcrect, SDL_Rect& dstrect) {
    return SDL_BlitSurface(src.me, const_cast<SDL_Rect*>(&srcrect), me, &dstrect);
  }

  int BaseSurface::Blit(const BaseSurface& src, const SDL_Rect& srcrect, const SPoint& dstpoint) {
    SDL_Rect  rect;
    rect.x = dstpoint.x; rect.y = dstpoint.y;
    return Blit(src, srcrect, rect);
  }

  int BaseSurface::Blit(const BaseSurface& src, SDL_Rect& dstrect) {
    return SDL_BlitSurface(src.me, 0, me, &dstrect);
  }

  int BaseSurface::Blit(const BaseSurface& src, const SPoint& dstpoint) {
    SDL_Rect  rect;
    rect.x = dstpoint.x; rect.y = dstpoint.y;
    return Blit(src, rect);
  }
  
  int BaseSurface::Blit(const BaseSurface& src) {
    return SDL_BlitSurface(src.me, 0, me, 0);
  }

  bool BaseSurface::Fill(Color color) {
    return SDL_FillRect(me, 0, color) == 0;
  }

  bool BaseSurface::FillRect(SDL_Rect& dstrect, Color color) {
    return SDL_FillRect(me, &dstrect, color) == 0;
  }

  // Set various things
  bool BaseSurface::SetColorKey(Uint32 flag, Color key) {
    return SDL_SetColorKey(me, flag, key) == 0;
  }
  
  bool BaseSurface::SetAlpha(Uint32 flag, Uint8 alpha) {
    return SDL_SetSurfaceAlphaMod(me, alpha) == 0;
  }
  
  void BaseSurface::SetClipRect(const SDL_Rect& rect) {
    SDL_SetClipRect(me, const_cast<SDL_Rect*>(&rect));
  }
  
  void BaseSurface::ResetClipRect() {
    SDL_SetClipRect(me, 0);
  }
  
  void BaseSurface::GetClipRect(SDL_Rect& rect) const {
    SDL_GetClipRect(me, &rect);
  }

  bool BaseSurface::SaveBMP(const char *file) const {
    if(me) {
      return SDL_SaveBMP(me, file) == 0;
    }
    return false;
  } 
}
