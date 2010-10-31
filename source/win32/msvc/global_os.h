/*
    This file is part of Advanced Strategic Command; http://www.asc-hq.de
    Copyright (C) 1994-2010  Martin Bickel  and  Marc Schellenberger

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; see the file COPYING. If not, write to the 
    Free Software Foundation, Inc., 59 Temple Place, Suite 330, 
    Boston, MA  02111-1307  USA
*/


#ifndef global_os_h_included
 #define global_os_h_included

  // #define sdlheader "SDL.h"
  // #define sdlmixerheader "SDL_mixer.h"

  typedef unsigned int PointerSizedInt;

  #ifdef __cplusplus 
    #include <string>
    using namespace std;
  #endif
  #define HAVE_STRICMP 

  #ifndef HAVE_ITOA
  # define HAVE_ITOA 1
  #endif

  #define HAVE_DIRENT_H 
  #define HAVE_LIMITS
//  #define SIZE_T_not_identical_to_INT 1

  #define StaticClassVariable

  #define CASE_SENSITIVE_FILE_NAMES 0
  #define USE_HOME_DIRECTORY 0


#endif
