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
#include "sdlmm_audio.h"
// Audio class

namespace SDLmm {
  bool Audio::Init() {
    Uint32 wasinit = SDL_WasInit(SDL_INIT_EVERYTHING);
    if(!wasinit) {
      return SDL_Init(SDL_INIT_AUDIO) == 0;
    } else if(!(wasinit & SDL_INIT_AUDIO)) {
      return SDL_InitSubSystem(SDL_INIT_AUDIO) == 0;
    }
    return true;
  }

  void Audio::Quit() {
    if(SDL_WasInit(SDL_INIT_AUDIO)) {
      SDL_QuitSubSystem(SDL_INIT_AUDIO);
    }
  }
}
  
