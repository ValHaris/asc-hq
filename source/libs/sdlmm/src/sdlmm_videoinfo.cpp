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
#include "sdlmm_videoinfo.h"

// file desc here
namespace SDLmm {

  bool VideoInfo::GetVideoInfo() {
    me =  SDL_GetVideoInfo();
    return me != 0;
  }

  bool VideoInfo::hw_available() {
    return VideoInfo::me->hw_available;
  }

  bool VideoInfo::wm_available() {
    return VideoInfo::me->wm_available;
  }

  bool VideoInfo::blit_hw() {
    return VideoInfo::me->blit_hw;
  }

  bool VideoInfo::blit_hw_CC() {
    return VideoInfo::me->blit_hw_CC;
  }

  bool VideoInfo::blit_hw_A() {
    return VideoInfo::me->blit_hw_A;
  }

  bool VideoInfo::blit_sw() {
    return VideoInfo::me->blit_sw;
  }

  bool VideoInfo::blit_sw_CC() {
    return VideoInfo::me->blit_sw_CC;
  }

  bool VideoInfo::blit_sw_A() {
    return VideoInfo::me->blit_sw_A;
  }

  bool VideoInfo::blit_fill() {
    return VideoInfo::me->blit_fill;
  }

  Uint32 VideoInfo::video_mem() {
    return VideoInfo::me->video_mem;
  }
}
