/*! \file sg.h
    \brief Interface for various global functions and variables
*/


//     $Id: sg.h,v 1.9 2002-05-07 19:52:47 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.8  2002/02/21 17:06:52  mbickel
//      Completed Paragui integration
//      Moved mail functions to own file (messages)
//
//     Revision 1.7  2001/01/28 14:04:19  mbickel
//      Some restructuring, documentation and cleanup
//      The resource network functions are now it their own files, the dashboard
//       as well
//      Updated the TODO list
//
//     Revision 1.6  2001/01/25 23:45:03  mbickel
//      Moved map displaying routins to own file (mapdisplay.cpp)
//      Wrote program to create pcx images from map files (map2pcx.cpp)
//      Fixed bug in repair function: too much resource consumption
//      AI improvements and bug fixes
//      The BI3 map import function now evaluates the player status (human/
//       computer)
//
//     Revision 1.5  2001/01/21 12:48:36  mbickel
//      Some cleanup and documentation
//
//     Revision 1.4  2000/08/05 20:18:03  mbickel
//      Restructured Fullscreen Image loading
//
//     Revision 1.3  2000/05/22 15:40:37  mbickel
//      Included patches for Win32 version
//
//     Revision 1.2  1999/11/16 03:42:27  tmwilson
//     	Added CVS keywords to most of the files.
//     	Started porting the code to Linux (ifdef'ing the DOS specific stuff)
//     	Wrote replacement routines for kbhit/getch for Linux
//     	Cleaned up parts of the code that gcc barfed on (char vs unsigned char)
//     	Added autoconf/automake capabilities
//     	Added files used by 'automake --gnu'
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


#ifndef sgH
#define sgH

#include "dialog.h"

extern void mainloopgeneralmousecheck ( void );
extern void mainloopgeneralkeycheck ( tkey& ch );
extern void repaintdisplay();

#ifdef sgmain
 #include "controls.h"
 extern cmousecontrol* mousecontrol;
#endif

extern pprogressbar actprogressbar ;


#endif

