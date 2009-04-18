//     $Id: global_os.h,v 1.7 2009-04-18 13:48:41 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.6  2001/05/18 22:30:30  mbickel
//      The data file is now installed in the correct directory
//      If the installation directory is changed with configure, the new path
//       will now be compiled directly into ASC
//
//     Revision 1.5  2000/10/31 18:08:28  mbickel
//      Borland C now uses unsigned char ( tools/Makefile changed )
//      Added header file for size_t to global_os.h
//
//     Revision 1.4  2000/10/17 13:04:16  mbickel
//      New terrainaccess reading/writing
//      Added Win32 project files
//
//     Revision 1.3  2000/10/16 14:34:14  mbickel
//      Win32 port is now running fine.
//      Removed MSVC project files and put them into a zip file in
//        asc/source/win32/msvc/
//
//     Revision 1.2  2000/10/14 13:07:05  mbickel
//      Moved DOS version into own subdirectories
//      Win32 version with Watcom compiles and links ! But doesn't run yet...
//
//     Revision 1.1  2000/10/14 10:55:06  mbickel
//      Added ASC-Win32 project files for Watcom
//
//     Revision 1.1  2000/10/12 21:37:56  mbickel
//      Further restructured platform dependant routines
//
//     Revision 1.1  2000/10/12 20:21:43  mbickel
//      Restructuring operating system dependant files
//
//
/*
    This file is part of Advanced Strategic Command; http://www.asc-hq.de
    Copyright (C) 1994-1999  Martin Bickel  and  Marc Schellenberger

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

// this file takes care that the necessary header files for opendir / readdir
// are being included in basestrm.cpp

#ifndef global_os_h_included
 #define global_os_h_included

  #define HAVE_STRICMP
  #define HAVE_ITOA
  #define HAVE_EXCEPTION
  #define HAVE_LIMITS
  #define HAVE_LOG2
  #define StaticClassVariable static
  #ifdef __WATCOM_CPLUSPLUS__      // and not Watcom C
  namespace std {};
  #endif

  #define CASE_SENSITIVE_FILE_NAMES 0
  #define USE_HOME_DIRECTORY 0

  #define sdlheader "sdl.h"
  #include <wchar.h>
#endif
