//     $Id: fileio.h,v 1.4 2001-10-11 10:41:06 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.3  2001/05/18 22:30:30  mbickel
//      The data file is now installed in the correct directory
//      If the installation directory is changed with configure, the new path
//       will now be compiled directly into ASC
//
//     Revision 1.2  2001/02/26 21:14:33  mbickel
//      Added two small editors to the linux makefiles
//      Added some more truecolor hacks to the graphics engine
//
//     Revision 1.1  2000/10/14 10:55:06  mbickel
//      Added ASC-Win32 project files for Watcom
//
//     Revision 1.2  2000/10/12 21:37:56  mbickel
//      Further restructured platform dependant routines
//
//     Revision 1.1  2000/10/12 20:21:42  mbickel
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

#ifndef fileio_h_included
 #define fileio_h_included

 #include <direct.h>
 #define direct dirent

 extern const char* filereadmode;
 extern const char* filewritemode;
 extern const char* filereadmodetext;
 extern const char* filewritemodetext;
 extern const char pathdelimitter;
 extern const char* pathdelimitterstring;
 
 extern const int maxfilenamelength;
 extern const char* asc_configurationfile;

#endif
