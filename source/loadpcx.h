/*! \file loadpcx.h
    \brief Interface for loading and writing of PCX images. 

    There are two independant implementations of these routines: #loadpcx.cpp and #dos/pcx.asm are written in assembly, and #loadpcxc.cpp is written in c++
*/

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

// extern char loadpcxxy ( const ASCString& name, bool setpal, int xpos, int ypos, int* xsize = NULL, int* ysize = NULL );
// extern char loadpcxxy( pnstream stream, int x, int y, bool setpalette = false, int* xsize = NULL, int* ysize = NULL );
extern void writepcx ( const ASCString& name, int x1, int y1, int x2, int y2, dacpalette256 pal );
extern void writepcx ( const ASCString& name, const Surface& s );
extern void writepcx ( const ASCString& name, const Surface& s, const SDLmm::SRect& rect );
// extern int pcxGetColorDepth ( const ASCString& name, int* width = NULL, int* height= NULL );

#endif
