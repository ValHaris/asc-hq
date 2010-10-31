//     $Id: fileio.h,v 1.5 2009-04-18 13:48:41 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.4  2004/07/12 18:15:10  mbickel
//      Lots of tweaks and fixed for more portability and compilation with mingw
//
//     Revision 1.3  2003/11/21 19:25:44  mbickel
//      Fixed: invisible own units when directview disabled
//
//     Revision 1.2  2000/10/14 10:52:56  mbickel
//      Some adjustments for a Win32 port
//
//     Revision 1.1  2000/10/12 20:21:43  mbickel
//      Restructuring operating system dependant files
//
//
/*
    This file is part of Advanced Strategic Command; http://www.asc-hq.de
    Copyright (C) 1994-2010  Martin Bickel  and  Marc Schellenberger

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

// this file takes care that the necessary header files for opendir / readdir
// are being included in basestrm.cpp

#ifdef __BORLANDC__
 #include "borland/fileio.h"
#else
 #ifdef __GNUG__
  #include "gcc/fileio.h"
 #else
  #ifdef __WATCOM_CPLUSPLUS__
   #include "watcom/fileio.h"
  #else
   #ifdef _MSC_VER
    #include "msvc/fileio.h"
   #endif
  #endif
 #endif
#endif

 extern const bool has_drive_letters;
