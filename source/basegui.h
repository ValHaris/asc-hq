//     $Id: basegui.h,v 1.4 2000-10-18 14:13:49 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.3  1999/12/28 21:02:36  mbickel
//      Continued Linux port
//      Added KDevelop project files
//
//     Revision 1.2  1999/11/16 03:41:05  tmwilson
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


#ifndef basegui_h
#define basegui_h

#include "events.h"

#pragma pack(1)
struct tnguiiconfiledata {
          void*     picture[8];
          void*     picturepressed[8];
          char*     infotext;
          tkey      keys[6][6];
          int       priority;
       };

#pragma pack()
#endif
