//     $Id: loadbi3.h,v 1.6 2000-09-16 11:47:29 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
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

#ifndef loadbi3_h
#define loadbi3_h

/*! \file loadbi3.h
   Loading Battle Isle 3 maps and graphics
*/

#include "sgstream.h"
#include "typen.h"
#include "palette.h"

extern void loadbi3graphics( void );
extern void getbi3pict ( int* num, void** picture );
extern void getbi3pict_double ( int* num, void** picture );
extern int  loadbi3pict_double ( int num, void** pict, int interpolate = 0, int reference = 1 );
  // returns: 1 if picture is a reference
  //          0 if picture is a copy
             


extern void loadbi3pict ( int num, void** pict );
extern int bi3graphnum;
extern void check_bi3_dir ( void );
extern tpixelxlattable bi2asc_color_translation_table;
extern void importbattleislemap ( char* path, char* filename, pwterraintype trrn );
extern void insertbattleislemap ( int x, int y, char* path, char* filename  );

extern const char* getbi3path ( void );

extern int getterraintranslatenum ( void );
extern int getobjectcontainertranslatenum ( void );

extern const int* getterraintranslate ( int pos );
extern const int* getobjectcontainertranslate ( int pos );

extern int keeporiginalpalette;

extern int getGraphicSetIdFromFilename ( const char* filename );
extern int activateGraphicSet ( int id  );

#endif
