//     $Id: loadpcx.h,v 1.2 1999-11-16 03:42:03 tmwilson Exp $
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

#ifndef loadpcx_h
#define loadpcx_h

#include "basestrm.h"

extern char loadpcxxy (char *name, boolean setpal, word x, word y);
extern char loadpcxxy( pnstream stream, int x, int y, int setpalette = 0 );
extern void writepcx ( char* name, int x1, int y1, int x2, int y2, dacpalette256 pal );


#endif
