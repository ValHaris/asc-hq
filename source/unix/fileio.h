//     $Id: fileio.h,v 1.1 2000-10-12 20:21:43 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
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

  #ifdef HAVE_SYS_DIRENT_H
   #include <sys/dirent.h>
  #endif
 
  #if HAVE_DIRENT_H
   #include <dirent.h>
   #define NAMLEN(dirent) strlen((dirent)->d_name)
  #else
   #define dirent direct
   #define NAMLEN(dirent) (dirent)->d_namlen
   #if HAVE_SYS_NDIR_H
    #include <sys/ndir.h>
   #endif
   #if HAVE_SYS_DIR_H
    #include <sys/dir.h>
   #endif
   #if HAVE_NDIR_H
    #include <ndir.h>
   #endif
  #endif
  #define direct dirent

 const char* filereadmode = "r";
 const char* filewritemode = "w";
 const char pathdelimitter = '/';
 const char* pathdelimitterstring = "/";

 #define CASE_SENSITIVE_FILE_NAMES 1

 const int maxfilenamelength = 255;

#endif