//     $Id: spfldutl.h,v 1.2 2009-04-18 13:48:38 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.1  2000/05/30 18:39:29  mbickel
//      Added support for multiple directories
//      Moved DOS specific files to a separate directory
//
//     Revision 1.3  1999/11/22 18:27:56  mbickel
//      Restructured graphics engine:
//        VESA now only for DOS
//        BASEGFX should be platform independant
//        new interface for initialization
//      Rewrote all ASM code in C++, but it is still available for the Watcom
//        versions
//      Fixed bugs in RLE decompression, BI map importer and the view calculation
//
//     Revision 1.2  1999/11/16 03:42:31  tmwilson
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

#ifndef _NOASM_

extern "C" int  luftlinie8 ( int x1, int y1, int x2, int y2 );
#pragma aux luftlinie8 parm [ eax ] [ ebx ] [ ecx ] [ edx ]

extern "C" int  rol ( int valuetorol, int rolwidth );
#pragma aux rol parm [ eax ] [ ecx ]

extern "C" void setvisibility ( word* visi, int valtoset, int actplayer );
#pragma aux setvisibility parm [ eax ] [ ebx ] [ ecx ] modify [ edx ]

extern "C" void dispspielfeld ( int* a, int* b );
#pragma aux dispspielfeld parm [ eax ] [ ebx ] modify [ edi esi ecx edx ]

extern "C" void copyvfb2displaymemory ( int* buf );
#pragma aux copyvfb2displaymemory parm [ ebx ] modify [ eax ecx edx esi edi ]

extern "C" void copyvfb2displaymemory_zoom ( void* parmbuf );
#pragma aux copyvfb2displaymemory_zoom parm [ ebx ] modify [ eax ecx edx esi edi ]

#endif
