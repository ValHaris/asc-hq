/*! \file loadimage.h
    Displaying fullscreen JPEG images; implementation is platform dependent.
*/

/***************************************************************************
                          loadimage.h  -  description
                             -------------------
    begin                : Sat Aug 5 2000
    copyright            : (C) 2000 by Martin Bickel
    email                : bickel@asc-hq.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef loadimage_h_included
 #define loadimage_h_included



enum FullscreenImageSettings { FIS_noTrueColor, FIS_oldModeNum };

extern int loadFullscreenImage ( const char* filename );
extern void closeFullscreenImage ( void );
extern void setFullscreenSetting ( FullscreenImageSettings setting, int value );

#endif