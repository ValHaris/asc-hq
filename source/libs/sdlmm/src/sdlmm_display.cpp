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
#include "sdlmm_global.h"
#include "sdlmm_display.h"

// The SDLmm::Display class.
// This is the main video display surface, based on SDLmm::BaseSurface

namespace SDLmm {
  // Update the screen

  Display& Display::GetDisplay()
  {
      static Display d;
      // In case we setup the display with another method, like using
      // ParaGUI or what not.
      if(!d.me)	d.me = SDL_GetVideoSurface();
      return d;
  }

  void Display::UpdateRect(Sint32 x, Sint32 y, Sint32 w, Sint32 h) {
    SDL_UpdateRect(me, x, y, w, h);
  }	
  
  void Display::UpdateRect(SDL_Rect& rect) {
    SDL_UpdateRect(me, rect.x, rect.y, rect.w, rect.h);
  }
  
  void Display::UpdateRects(int numrects, SDL_Rect *rects) {
    SDL_UpdateRects(me, numrects, rects);
  }

  void Display::SetCaption(const char *title, const char *icon) {
    SDL_WM_SetCaption(title, icon);
  }
  
  void Display::SetCaption(const std::string& title, 
				const std::string& icon) {
    SDL_WM_SetCaption(title.c_str(), icon.c_str());
  }
  
  void Display::GetCaption(char **title, char **icon) {
    SDL_WM_GetCaption(title, icon);
  }
  
  void Display::GetCaption(std::string &title, std::string &icon) {
    char *_title, *_icon;
    SDL_WM_GetCaption(&_title, &_icon);
    title = _title;
    icon  = _icon;
  }
  
  void Display::SetIcon(BaseSurface& icon, Uint8 *mask) {
    SDL_WM_SetIcon(icon.GetSurface(), mask);
  }
  
  bool Display::Iconify() {
    return SDL_WM_IconifyWindow() != 0;
  }
  
  bool Display::ToggleFullScreen() {
    return SDL_WM_ToggleFullScreen(me) != 0;
  }
  
  SDL_GrabMode Display::GrabInput(SDL_GrabMode mode) {
    return SDL_WM_GrabInput(mode);
  }

  bool Display::SetVideoMode(int w, int h, int bpp, Uint32 flags) {
    SDL_Surface *tmp = SDL_SetVideoMode(w, h, bpp, flags);
    if(!tmp) return false;
    me = tmp;
    return true;
  }
  
  // Video mode stuff
  int Display::VideoModeOK(int w, int h, int bpp, Uint32 flags) {
    return SDL_VideoModeOK(w, h, bpp, flags);
  }
  
  SDL_Rect **Display::ListModes(SDL_PixelFormat *format, Uint32 flags) {
    return SDL_ListModes(format, flags);
  }

  bool Display::Init() {
    Uint32 wasinit = SDL_WasInit(SDL_INIT_EVERYTHING);
    bool init_success(true);
    if(!wasinit) {
      init_success = SDL_Init(SDL_INIT_VIDEO) == 0;
    } else if(!(wasinit & SDL_INIT_VIDEO)) {
      init_success = SDL_InitSubSystem(SDL_INIT_VIDEO) == 0;
    }
    return init_success;
  }

  void Display::Quit() {
    if(SDL_WasInit(SDL_INIT_VIDEO)) {
      SDL_QuitSubSystem(SDL_INIT_VIDEO);
    }
  }
}

