/*
    This file is part of Advanced Strategic Command; http://www.asc-hq.de
    Copyright (C) 1994-2003  Martin Bickel  and  Marc Schellenberger

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

#ifndef surface2pngH
 #define surface2pngH
 
 #include "surface.h"
 #include "../ascstring.h"
 

 // these functions are really slow

 void writePNG( const ASCString& filename, const Surface& s ); 
 void writePNGtrim( const ASCString& filename, const Surface& s ); 
 void writePNG( const ASCString& filename, const Surface& s, int x1, int y1, int x2, int y2  ); 
 void writePNG( const ASCString& filename, const Surface& s, const SDLmm::SRect& rect ); 
 
 
 
#endif

