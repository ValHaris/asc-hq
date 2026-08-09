/***************************************************************************
                          textfile_evaluation.h  -  description
                             -------------------
    begin                : Thu Oct 06 2002
    copyright            : (C) 2002 by Martin Bickel
    email                : bickel@asc-hq.org
 ***************************************************************************/


#ifndef fieldimageloaderH
#define fieldimageloaderH

#include <vector>
#include "graphics/surface.h"

class ImagePreparation {
public:
   virtual void prepareImage( Surface& surface ) = 0;
   virtual ~ImagePreparation() {};
};

class TerrainImagePreparator : public ImagePreparation {
public:
   virtual void prepareImage( Surface& surface );
};

class VehicleImagePreparator : public ImagePreparation {
public:
   virtual void prepareImage( Surface& surface );
};

extern vector<Surface> loadASCFieldImageArray ( const ASCString& file, int num, ImagePreparation* imagePreparation = NULL );
extern Surface loadASCFieldImage ( const ASCString& file, ImagePreparation* imagePreparation = NULL );
extern void snowify( Surface& s, bool adaptive  = true);


#endif
