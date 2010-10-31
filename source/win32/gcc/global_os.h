//     $Id: global_os.h,v 1.4 2009-04-18 13:48:41 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.3  2007/04/13 16:16:19  mbickel
//      Merged ASC2 branch
//
//     Revision 1.1.2.1  2005/02/15 14:21:45  mbickel
//      Merged changes of 1.15.3
//
//     Revision 1.2  2004/12/27 15:54:55  mbickel
//      Fixed mapeditor crash and limitations when resizing maps
//
//     Revision 1.1  2004/07/12 18:15:10  mbickel
//      Lots of tweaks and fixed for more portability and compilation with mingw
//
//
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

  typedef unsigned int PointerSizedInt;

  #ifndef minimalIO
   // #define sdlheader "SDL.h"
   // #define sdlmixerheader "SDL_mixer.h"
   // #include sdlheader
  #endif
//   #include <string>  // some Standard library include file to define the namespace std
  using namespace std;
  #define HAVE_STRICMP
  #define HAVE_ITOA
  #define HAVE_DIRENT_H
  #define HAVE_STDIO_H
  #define StaticClassVariable
  #define NoStdio

 #define CASE_SENSITIVE_FILE_NAMES 0
 #define USE_HOME_DIRECTORY 0


#endif
