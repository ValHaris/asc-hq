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

// Wrappers for global SDL functions

#include <SDL.h>
#include "sdlmm_global.h"
#include "sdlmm_videoinfo.h"

namespace SDLmm {
  // Init and cleanup functions
  bool Init(Uint32 flags) {
    bool res = SDL_Init(flags) == 0;
    return res;
  }
  void Quit() { SDL_Quit(); }
  Uint32 WasInit(Uint32 flags) { return SDL_WasInit(flags); }

  // Error...
  const char *GetError() { return SDL_GetError(); }

//  const char *version() { return VERSION; }

}

