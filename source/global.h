/***************************************************************************
                          global.h  -  description
                             -------------------
    begin                : 
    copyright            : (C) 2001 by Martin Bickel
    email                : bickel@asc-hq.org
 ***************************************************************************/

/*! \file global.h
     \brief Global platform dependant definitions.
     This file just branches to the platform specific files in their 
     respective subdirectories.
*/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef global_h
 #define global_h

 #ifdef _WIN32_
  #include "win32/global_os.h"
 #else
  #ifdef _DOS_
   #include "dos/global_os.h"
  #else
   #ifdef _UNIX_
    #include "unix/global_os.h"
   #else
    #error "No system specified"
   #endif
  #endif
 #endif

#define ParserLoadImages 

#endif
