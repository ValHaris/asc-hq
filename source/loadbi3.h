//     $Id: loadbi3.h,v 1.12 2001-01-31 14:52:39 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.11  2001/01/28 23:00:40  mbickel
//      Made the small editors compilable with Watcom again
//
//     Revision 1.10  2001/01/28 20:42:13  mbickel
//      Introduced a new string class, ASCString, which should replace all
//        char* and std::string in the long term
//      Split loadbi3.cpp into 3 different files (graphicselector, graphicset)
//
//     Revision 1.9  2001/01/25 23:45:00  mbickel
//      Moved map displaying routins to own file (mapdisplay.cpp)
//      Wrote program to create pcx images from map files (map2pcx.cpp)
//      Fixed bug in repair function: too much resource consumption
//      AI improvements and bug fixes
//      The BI3 map import function now evaluates the player status (human/
//       computer)
//
//     Revision 1.8  2001/01/21 16:37:18  mbickel
//      Moved replay code to own file ( replay.cpp )
//      Fixed compile problems done by cleanup
//
//     Revision 1.7  2000/10/14 13:07:00  mbickel
//      Moved DOS version into own subdirectories
//      Win32 version with Watcom compiles and links ! But doesn't run yet...
//
//     Revision 1.6  2000/09/16 11:47:29  mbickel
//      Some cleanup and documentation again
//
//     Revision 1.5  2000/07/29 14:54:37  mbickel
//      plain text configuration file implemented
//
//     Revision 1.4  2000/04/01 11:38:39  mbickel
//      Updated the small editors
//      Added version numbering
//
//     Revision 1.3  2000/03/11 18:22:07  mbickel
//      Added support for multiple graphic sets
//
//     Revision 1.2  1999/11/16 03:41:55  tmwilson
//      Added CVS keywords to most of the files.
//      Started porting the code to Linux (ifdef'ing the DOS specific stuff)
//      Wrote replacement routines for kbhit/getch for Linux
//      Cleaned up parts of the code that gcc barfed on (char vs unsigned char)
//      Added autoconf/automake capabilities
//      Added files used by 'automake --gnu'
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

#ifndef loadbi3_h
#define loadbi3_h

/*! \file loadbi3.h
   Loading Battle Isle 3 maps and graphics
*/

#include "typen.h"
#include "sgstream.h"
#include "palette.h"

#ifdef converter
 #error The small editors should not need to use LoadBi3
#endif


extern void check_bi3_dir ( void );

// fakemap must not be used by a program other than map2pcx
extern void importbattleislemap ( const char* path, const char* filename, TerrainType::Weather* trrn, string* errorOutput = NULL, bool __fakeMap = false );
extern void insertbattleislemap ( int x, int y, const char* path, const char* filename  );

extern const char* getbi3path ( void );

extern int getterraintranslatenum ( void );
extern int getobjectcontainertranslatenum ( void );

extern const int* getterraintranslate ( int pos );
extern const int* getobjectcontainertranslate ( int pos );


#endif
