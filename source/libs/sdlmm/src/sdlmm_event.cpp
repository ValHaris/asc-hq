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

// SDLmm event handling
#include <SDL.h>
#include "sdlmm_eventhandler.h"
#include "sdlmm_event.h"
#include <iostream>
namespace SDLmm {

  bool Event::Poll(bool fetch) {
    if(fetch) {
      return SDL_PollEvent(&me) != 0;
    } else {
      return SDL_PollEvent(0) != 0;
    }
  }
  
  bool Event::Wait(bool fetch) {
    if(fetch) {
      return SDL_WaitEvent(&me) != 0;
    } else {
      return SDL_WaitEvent(0) != 0;
    }
  }

  bool Event::Push() {
    return SDL_PushEvent(&me) == 0;
  }

  void Event::PumpEvents() {
    SDL_PumpEvents();
  }

  void Event::SetEventFilter(SDL_EventFilter filter) {
    SDL_SetEventFilter(filter);
  }    

  SDL_EventFilter Event::GetEventFilter() {
    return SDL_GetEventFilter();
  }

  Uint8 Event::EventState(Uint8 type, int state) {
    return SDL_EventState(type, state);
  }

  Uint8 *Event::GetKeyState(int &numkeys) {
    return SDL_GetKeyState(&numkeys);
  }

  Uint8 *Event::GetKeyState() {
    return SDL_GetKeyState(0);
  }
  
  SDLMod Event::GetModState() {
    return SDL_GetModState();
  }

  char *Event::GetKeyName(SDLKey key) {
    return SDL_GetKeyName(key);
  }
  
  void Event::SetModState(SDLMod modstate) {
    SDL_SetModState(modstate);
  }

  bool Event::EnableKeyRepeat(int delay, int interval) {
    return SDL_EnableKeyRepeat(delay, interval) == 0;
  }

  Uint8 Event::GetMouseState(int *x, int *y) {
    return SDL_GetMouseState(x, y);
  }
  
  Uint8 Event::GetRelativeMouseState(int *x, int *y) {
    return SDL_GetRelativeMouseState(x, y);
  }
  Uint8 Event::GetAppState() {
    return SDL_GetAppState();
  }
  int Event::JoystickEventState(int state) {
    return SDL_JoystickEventState(state);
  }

  void Event::HandleEvents(EventHandler &handler) {
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
      HandleEvent(handler, event);
    }
  }

  void Event::HandleEvent(EventHandler &handler, const SDL_Event& event) {
    bool ev_handled = false;
    switch(event.type) {
    case SDL_ACTIVEEVENT:
      ev_handled = handler.HandleActiveEvent(event.active.gain == 1,
                                             event.active.state);
      break;
    case SDL_KEYDOWN:
      ev_handled = handler.HandleKeyPressEvent(event.key.keysym);
      break;
    case SDL_KEYUP: 
      ev_handled = handler.HandleKeyReleaseEvent(event.key.keysym);
      break;
    case SDL_MOUSEMOTION: 
      ev_handled = handler.HandleMouseMotionEvent(event.motion.state,
                                                  event.motion.x,
                                                  event.motion.y,
                                                  event.motion.xrel,
                                                  event.motion.yrel);
      break;
    case SDL_MOUSEBUTTONDOWN:
      ev_handled = handler.HandleMouseButtonPressEvent(event.button.button,
                                                       event.button.x,
                                                       event.button.y);
      break;
    case SDL_MOUSEBUTTONUP: 
      ev_handled = handler.HandleMouseButtonReleaseEvent(event.button.button,
                                                         event.button.x,
                                                         event.button.y);
      break;
    case SDL_JOYAXISMOTION: 
      ev_handled = handler.HandleJoyAxisEvent(event.jaxis.which,
                                              event.jaxis.axis,
                                              event.jaxis.value);
      break;
    case SDL_JOYBALLMOTION: 
      ev_handled = handler.HandleJoyBallEvent(event.jball.which,
                                              event.jball.ball,
                                              event.jball.xrel,
                                              event.jball.yrel);
      break;
    case SDL_JOYHATMOTION:
      ev_handled = handler.HandleJoyHatEvent(event.jhat.which,
                                             event.jhat.hat,
                                             event.jhat.value);
      break;
    case SDL_JOYBUTTONDOWN:
      ev_handled = handler.HandleJoyButtonPressEvent(event.jbutton.which,
                                                     event.jbutton.button);
      break;
    case SDL_JOYBUTTONUP: 
      ev_handled = handler.HandleJoyButtonReleaseEvent(event.jbutton.which,
                                                       event.jbutton.button);
      break;
    case SDL_QUIT: 
      ev_handled = handler.HandleQuitEvent();
      break;
    case SDL_SYSWMEVENT: 
      ev_handled = handler.HandleSysWMEvent();
      break;
    case SDL_VIDEORESIZE:
      ev_handled = handler.HandleResizeEvent(event.resize.w, event.resize.h);
      break;
    case SDL_USEREVENT: 
      ev_handled = handler.HandleUserEvent(event.user.type,
                                           event.user.code,
                                           event.user.data1,
                                           event.user.data2);
      break;
    }
    if(!ev_handled) {
      handler.HandleEvent(event);
    }
  }
}
