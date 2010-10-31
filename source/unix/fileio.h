//     $Id: fileio.h,v 1.11 2009-04-18 13:48:40 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.10  2007/04/13 16:16:15  mbickel
//      Merged ASC2 branch
//
//     Revision 1.8.2.1  2006/07/08 17:00:14  mbickel
//      Merged updates from trunk
//      Fixed replay problems
//      Restructuring
//
//     Revision 1.9  2006/04/25 18:04:11  mbickel
//      Fixed compilation problems
//
//     Revision 1.8  2003/11/18 18:29:23  mbickel
//      Fixed unix compilation problems
//
//     Revision 1.7  2002/10/19 16:42:05  mbickel
//      Fixes to the build system
//
//     Revision 1.6  2002/01/29 20:42:16  mbickel
//      Improved finding of files with relative path
//      Added wildcards to music playlist files
//
//     Revision 1.5  2001/10/11 10:41:06  mbickel
//      Restructured platform fileio handling
//      Added map archival information to mapeditor
//
//     Revision 1.4  2001/05/18 22:30:30  mbickel
//      The data file is now installed in the correct directory
//      If the installation directory is changed with configure, the new path
//       will now be compiled directly into ASC
//
//     Revision 1.3  2001/02/26 21:14:32  mbickel
//      Added two small editors to the linux makefiles
//      Added some more truecolor hacks to the graphics engine
//
//     Revision 1.2  2000/10/12 21:37:57  mbickel
//      Further restructured platform dependant routines
//
//     Revision 1.1  2000/10/12 20:21:43  mbickel
//      Restructuring operating system dependant files
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

// this file takes care that the necessary header files for opendir / readdir
// are being included in basestrm.cpp

#ifndef fileio_h_included
 #define fileio_h_included

  #ifdef HAVE_SYS_DIRENT_H
   #include <sys/dirent.h>
  #endif
 
  #if HAVE_DIRENT_H
  # include <dirent.h>
  # define NAMLEN(dirent) strlen((dirent)->d_name)
  #else
  # define dirent direct
  # define NAMLEN(dirent) (dirent)->d_namlen
  # if HAVE_SYS_NDIR_H
  #  include <sys/ndir.h>
  # endif
  # if HAVE_SYS_DIR_H
  #  include <sys/dir.h>
  # endif
  # if HAVE_NDIR_H
  #  include <ndir.h>
  # endif
  #endif
  #define ASC_direct dirent
 
  #include <unistd.h>

extern const char* filereadmode;
extern const char* filewritemode;
extern const char* filereadmodetext;
extern const char* filewritemodetext;
extern const char pathdelimitter;
extern const char* pathdelimitterstring;
extern const char  foreignPathDelimitter;
extern const char* foreignPathDelimitterString;
extern const bool has_drive_letters;

extern const int maxfilenamelength;
extern const char* asc_configurationfile;

#endif
