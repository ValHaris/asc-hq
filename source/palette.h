//     $Id: palette.h,v 1.3 1999-12-28 21:03:15 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.2  1999/11/16 03:42:20  tmwilson
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

#ifndef palette_h_included
 #define palette_h_included

#pragma pack(1)

typedef char tmixbuf[256][256];
typedef tmixbuf* pmixbuf;
typedef char tpixelxlattable[256];
typedef tpixelxlattable* ppixelxlattable; 
typedef char dacpalette256[256][3];
typedef char dacpalettefst[256][4];


   struct txlattables {
             tpixelxlattable nochange;
             union {
                struct {
                   tpixelxlattable dark05;
                   tpixelxlattable dark1;
                   tpixelxlattable dark2;
                   tpixelxlattable dark3;
                }a;
                tpixelxlattable dark[4];
             };
             tpixelxlattable light;
          };
   
   extern txlattables xlattables;


extern tpixelxlattable nochange;
extern tpixelxlattable dark1;
extern tpixelxlattable dark2;
extern tpixelxlattable light;
extern tmixbuf *colormixbuf;
extern dacpalette256 pal;

extern int asc_paletteloaded;
extern ppixelxlattable xlatpictgraytable;  

#pragma pack()

#endif
