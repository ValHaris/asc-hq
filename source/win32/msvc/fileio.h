//     $Id: fileio.h,v 1.3 2001-05-18 22:30:30 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.2  2000/10/16 14:34:14  mbickel
//      Win32 port is now running fine.
//      Removed MSVC project files and put them into a zip file in
//        asc/source/win32/msvc/
//
//     Revision 1.1  2000/10/14 10:52:57  mbickel
//      Some adjustments for a Win32 port
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

#error Visual C sucks ! Please read asc/doc/source.html

#ifndef fileio_h_included
 #define fileio_h_included

 #include <direct.h>
 #include <dirent.h>
 #define direct dirent

 const char* filereadmode = "rb";
 const char* filewritemode = "wb";
 const char pathdelimitter = '\\';
 const char* pathdelimitterstring = "\\";
 
 const int maxfilenamelength = 255;
 const char* asc_configurationfile = "asc.ini";

#endif