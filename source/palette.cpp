/*! \file palette.cpp
    \brief The color palette and various color translation tables
*/

/*
    This file is part of Advanced Strategic Command; http://www.asc-hq.de
    Copyright (C) 1994-2005  Martin Bickel  and  Marc Schellenberger

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

#include "palette.h"
/*
tpixelxlattable nochange;
tpixelxlattable dark1;
tpixelxlattable dark2;
tpixelxlattable light;
*/

dacpalette256 pal;

ppixelxlattable xlatpictgraytable;  

txlattables xlattables;
tmixbuf *colormixbuf;
char* colormixbufchar;
// tpixelxlattable bi2asc_color_translation_table;

bool asc_paletteloaded = 0;

const char* borland_c_sucks ( void )
{
   return "just some code";
}



