//     $Id: loadbi3.h,v 1.2 1999-11-16 03:41:55 tmwilson Exp $
//
//     $Log: not supported by cvs2svn $
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


#include "sgstream.h"
#include "typen.h"

extern void loadbi3graphics( void );
extern void getbi3pict ( int* num, void** picture );
extern void getbi3pict_double ( int* num, void** picture );
extern void loadbi3pict_double ( int num, void** pict, int interpolate = 0 );
extern void loadbi3pict ( int num, void** pict );
extern int bi3graphnum;
extern void check_bi3_dir ( void );
extern unsigned char bi2asc_color_translation_table[256];
extern void importbattleislemap ( char* path, char* filename, pwterraintype trrn );
extern void insertbattleislemap ( int x, int y, char* path, char* filename  );

extern char* getbi3path ( void );

extern int getterraintranslatenum ( void );
extern int getobjectcontainertranslatenum ( void );

extern const int* getterraintranslate ( int pos );
extern const int* getobjectcontainertranslate ( int pos );

extern int keeporiginalpalette;

#endif
