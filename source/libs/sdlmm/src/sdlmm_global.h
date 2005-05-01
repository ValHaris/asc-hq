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

// Wrappers for global SDL functions

#ifndef SDLMM_GLOBAL_H
#define SDLMM_GLOBAL_H


//! The namespace for the SDLmm library. 
namespace SDLmm {
  // Init and cleanup functions
  //! Initializes SDL
  /*!
    This method should be called before utilizing any other SDL or
    SDLmm functionality. The flags parameter specifies what part(s) of
    SDL to initialize (see the SDL documentation for details).
    \param flags the subsystems to initalize
    \return true on success, false on error

    \note In SDLmm the preferred way of initiating subsystem is to use
    the Init() function in the subsystem classes. I.e to initialize
    audio and video, run Display::Init() and Audio::Init(). Also note
    that instantiating an object might be enough (for example creating
    a VideoInfo instance will initialize the video subsystem).
   */
  bool  Init(Uint32 flags);
  
  //! Shut down SDL
  /*!
    Quit() shuts down all SDL subsystems and frees the resources
    allocated to them. This should always be called before you
    exit. For the sake of simplicity you can set Quit() as your atexit
    call, like this:

    \code
    SDLmm::Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO);
    atexit(SDLmm::Quit);
    \endcode
  */
  void  Quit();

  //! Check which subsystems are initialized
  /*!
    This allows you to see which SDL subsytems have been
    initialized.
    \param flags bitwise OR'd combination of the subsystems you wish
    to check
    \returns A bitwised OR'd combination of the initialized
    subsystems.
  */
  Uint32  WasInit(Uint32 flags);

  //! Get the latest error message.
  /*!
    \returns The error message for the last SDL function that failed.
   */
  const  char *GetError();

  //! Return the SDLmm library version.
  const  char *version();
}

#endif // SDLMM_GLOBAL_H
