/*
 * SDLmm - a C++ wrapper for SDL and related libraries
 * Copyright © 2001 David Hedbor <david@hedbor.org>
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
#include "sdlmm_pixelformat.h"

// The SDLmm::PixelFormat class.

namespace SDLmm {

  Color PixelFormat::MapRGB(Uint8 r, Uint8 g, Uint8 b) const {
    return SDL_MapRGB(me, r, g, b);
  }

  Color PixelFormat::MapRGBA(Uint8 r, Uint8 g, Uint8 b, Uint8 a) const {
    return SDL_MapRGBA(me, r, g, b, a);
  }
  
  void PixelFormat::GetRGB(Color pixel, Uint8 &r, Uint8 &g, Uint8 &b) const {
    SDL_GetRGB(pixel, me, &r, &g, &b);
  }

  void PixelFormat::GetRGBA(Color pixel, Uint8 &r, Uint8 &g, Uint8 &b, Uint8 &a) const {
    SDL_GetRGBA(pixel, me, &r, &g, &b, &a);
  }
}
