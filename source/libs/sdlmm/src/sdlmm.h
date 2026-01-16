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

/*! \mainpage SDLmm - A C++ Wrapper for the Simple DirectMedia Layer
  \section intro Introduction

  SDLmm is a C++ glue for SDL, or the Simple DirectMedia Layer, which
  is a generic API that provides low level access to audio, keyboard,
  mouse, joystick, 3D hardware via OpenGL, and 2D framebuffer across
  multiple platforms. 

  SDLmm aims to stay as close as possible to the C API while taking
  advantage of native C++ features like object orientation. We will
  also aim at being platform independent as much as possible. I.e
  we'll try to support ever platform supported by SDL.

  \section misc Project Page

  This website and the SDLmm project pages are hosted at <a
  href="http://sourceforge.net/">SourceForge</a>. For CVS information,
  downloads, bug tracking, feature requests, mailing list information
  and more, visit <a href="http://sourceforge.net/projects/sdlmm/">our
  SourceForge project page</a>.
  
  \section license Licensing 

  This library is, like SDL, distributed under GNU LGPL version 2
  license, which can be found in the file \e COPYING. This license
  allows you to use SDLmm freely in commercial programs as long as you
  link with the dynamic library.

  \section develplan Development Plan

  This is a brief idea of the planned order of implementation, in
  order of priority (highest first).
  
  - Implement a complete wrapper around the SDL library, using a
    logical and simple object oriented design. Use as few global
    functions as possible.

   - Add support for commonly used third party libraries. This
     includes popular libs like SDL_image, SDL_ttf and SDL_mixer.

   - Build new features on the SDLmm framework, adding useful features
     not available in SDL. This might include utility functions like
     scaled blitting, drawing primitives (lines, polygons etc) and
     sprites. Since there are other libraries providing many of these
     features, it is to be determined what actually will be added.

   \section goals Development Goals

   - Build a logical C++ glue, allowing for easy-to-use object
     oriented SDL programming, keeping everything within it's own
     namespace, SDLmm.

   - Stay as close as possible to the syntax as SDL. This includes
     using a similar naming scheme. For example SDL_BlitSurface()
     becomes SDLmm::Surface::Blit().

  - When it makes sense, convert functions to returning true of false
    to indiciate success/failure as opposed to 0 or -1. Sometimes this
    is not viable, since a function might return 0, -1 or -2 (for
    example).

  - Avoid the use of unnecessary global functions. If it makes sense
    to move a function to a class, then it should be done. Also, avoid
    duplication. I.e if there is a SDLmm::Surface::LoadBMP() method, there
    won't be a global SDLmm::LoadBMP() method.

  - Use of exceptions is yet to be decided. Right now, none are
    used. However they should be considered for use in at least
    constructors where there is no other way to indicate a failure
    than to thrown an exception. In any case, exceptions would only be
    used for FATAL errors.

  \section docgoals Documentation Goals

  - Write and maintain class documentation using the doxygen
    toolkit. All actual documentation is to be done in the include
    files (and not the .cpp files). The reason being that the .h files
    always will include all functions, while the .cpp files might not
    (i.e inlined functions).

  - Write annotated code examples. Doxygen has a very nice feature for
    doing this. It just takes time to write the examples and to
    document them.
    
*/

/*! \file sdlmm.h 
  This include file is to be used in all SDLmm applications. All files
  requires are included here. You can also include just the files you
  need (for performance reasons), but beware of cross-file
  dependencies. 
 */

#ifndef SDLMM_H
#define SDLMM_H
#include <SDL.h>
/* The undef kludge is to prevent conflicts with other automake
 * packages..
 */
#include "sdlmm_global.h" 
#include "sdlmm_misc.h" 
#include "sdlmm_spoint.h"
#include "sdlmm_srect.h"
#include "sdlmm_color.h"
#include "sdlmm_basesurface.h"
#include "sdlmm_surface.h"
#include "sdlmm_pixelformat.h"
#include "sdlmm_display.h"
#include "sdlmm_timer.h"
#endif // SDLMM_H
