/*! \file palette.cpp
    \brief The color palette and various color translation tables
*/

//     $Id: palette.cpp,v 1.7 2001-10-31 18:34:33 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.6  2001/09/20 15:36:09  mbickel
//      New object displaying mode
//
//     Revision 1.5  2001/01/28 20:42:14  mbickel
//      Introduced a new string class, ASCString, which should replace all
//        char* and std::string in the long term
//      Split loadbi3.cpp into 3 different files (graphicselector, graphicset)
//
//     Revision 1.4  2001/01/28 14:04:15  mbickel
//      Some restructuring, documentation and cleanup
//      The resource network functions are now it their own files, the dashboard
//       as well
//      Updated the TODO list
//
//     Revision 1.3  2000/12/23 13:19:47  mbickel
//      Made ASC compileable with Borland C++ Builder
//
//     Revision 1.2  1999/11/16 03:42:19  tmwilson
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
tpixelxlattable bi2asc_color_translation_table;

bool asc_paletteloaded = 0;

char* borland_c_sucks ( void )
{
   return "just some code";
}
