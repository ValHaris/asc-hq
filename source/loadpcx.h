/*! \file loadpcx.h
    \brief Interface for loading and writing of PCX images. 

    There are two independant implementations of these routines: #loadpcx.cpp and #dos/pcx.asm are written in assembly, and #loadpcxc.cpp is written in c++
*/

//     $Id: loadpcx.h,v 1.8 2001-07-27 21:13:35 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.7  2001/02/28 14:10:05  mbickel
//      Added some small editors to linux makefiles
//      Added even more dirty hacks to basegfx: some more truecolor functions
//
//     Revision 1.6  2001/02/11 11:39:39  mbickel
//      Some cleanup and documentation
//
//     Revision 1.5  2001/01/25 23:45:00  mbickel
//      Moved map displaying routins to own file (mapdisplay.cpp)
//      Wrote program to create pcx images from map files (map2pcx.cpp)
//      Fixed bug in repair function: too much resource consumption
//      AI improvements and bug fixes
//      The BI3 map import function now evaluates the player status (human/
//       computer)
//
//     Revision 1.4  2000/05/30 18:39:25  mbickel
//      Added support for multiple directories
//      Moved DOS specific files to a separate directory
//
//     Revision 1.3  2000/05/23 20:40:47  mbickel
//      Removed boolean type
//
//     Revision 1.2  1999/11/16 03:42:03  tmwilson
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

#ifndef loadpcx_h
#define loadpcx_h

#include "basestrm.h"
#include "basegfx.h"
#include "ascstring.h"

extern char loadpcxxy ( const ASCString& name, char setpal, word x, word y);
extern char loadpcxxy( pnstream stream, int x, int y, int setpalette = 0 );
extern void writepcx ( const ASCString& name, int x1, int y1, int x2, int y2, dacpalette256 pal );
extern int pcxGetColorDepth ( const ASCString& name );

#endif
