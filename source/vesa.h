//     $Id: vesa.h,v 1.3 1999-11-22 18:28:06 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.2  1999/11/16 03:42:48  tmwilson
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

#ifndef vesa_h
#define vesa_h

#include "global.h"
#include "tpascal.inc"
#include "palette.h"



//*********** Initialization ************

extern int                    vesaerrorrecovery;
extern int reinitgraphics(int modenum);
extern int initgraphics ( int x, int y, int depth );
             // returns > 0  modenum to reestablish this mode
             //         < 0 : error

extern void showavailablemodes( void );

extern void  closegraphics ( void );



//*********** Misc ************


extern int copy2screen( void );

#ifdef _NOASM_
extern int dpmscapabilities;
extern int actdpmsmode;

#else

extern "C" int dpmscapabilities;
extern "C" int actdpmsmode;

#endif


#ifdef _NOASM_

 extern void setdisplaystart( int x, int y);
/*
 extern int setscanlinelength( int length );
 extern int getscanlinelength();
*/
 extern void set_vgapalette256 ( dacpalette256 pal );

#else

 extern "C" void setdisplaystart( int x, int y);
 #pragma aux setdisplaystart parm [ ecx ][ edx ]  modify [eax ebx];
 
/*
 extern "C" int setscanlinelength( int length );
 #pragma aux setscanlinelength parm [ ecx ] modify [ ebx edx ];
 
 extern "C" int getscanlinelength();
 #pragma aux getscanlinelength modify [ ebx ecx edx ];
*/

 extern "C" void set_vgapalette256 ( dacpalette256 pal );
 #pragma aux set_vgapalette256 parm [ eax ] modify [ ebx ecx edx esi ]

 /*
 extern "C" void waitretrace ( void );
 #pragma aux waitretrace modify [ al dx ]
 
 extern "C" void getdpmscapabilities ( void );
 #pragma aux getdpmscapabilities modify [ eax ebx ecx edx edi es ]
 
 extern "C" void controldpms ( char mode );
 #pragma aux controldpms parm [ bh ] modify [ eax ebx ecx edx ]
 */
 
 extern "C" void setvirtualpagepos ( int page );
 #pragma aux setvirtualpagepos parm [ eax ]


#endif

extern int dont_use_linear_framebuffer;
extern int graphicinitialized;

#endif
