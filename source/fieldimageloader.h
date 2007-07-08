/***************************************************************************
                          textfile_evaluation.h  -  description
                             -------------------
    begin                : Thu Oct 06 2002
    copyright            : (C) 2002 by Martin Bickel
    email                : bickel@asc-hq.org
 ***************************************************************************/


#ifndef fieldimageloaderH
#define fieldimageloaderH

#include "graphics/surface.h"

extern vector<Surface> loadASCFieldImageArray ( const ASCString& file, int num );
extern Surface loadASCFieldImage ( const ASCString& file, bool applyFieldMask = true );
extern void snowify( Surface& s, bool adaptive  = true);


#endif
