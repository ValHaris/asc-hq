/*
 * SDmm - a C++ wrapper for SDL and related libraries
 * Copyright © 2001 David Hedbor <david@hedbor.org>
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */


#ifndef SDLMM_SURFACE_H
#define SDLMM_SURFACE_H

#include "sdlmm_basesurface.h"

namespace SDLmm {
  //! A graphical surface structure which is used to store graphical data.
  /*!
    Surfaces represent areas of "graphical" memory, memory that can be
    drawn to or blitted onto other Surfaces. 

    \todo Implement a better constructor scheme, where the first
    argument is a dummy class which specified what to do. This would
    allow a construct where a surface can be loaded from an image
    file without first having to create an unitialized object.

    \author David Hedbor <david@hedbor.org>
  */
  
  class DECLSPEC Surface : public BaseSurface
  {
  public:
    //! Constructor from an SDL_Surface*
    /*!
      This creates a new Surface object from an existing
      SDL_Surface. It's very important not to free the original
      surface since that will cause a problem when the object is
      destructed (SDL_Surface storage freed twice). Use with caution.
    */
    explicit Surface(SDL_Surface *surface)
      : BaseSurface(surface) {
    }

    Surface(const Surface& other)
      : BaseSurface(other) {
      if (me)
        ++(me->refcount);
    }
    
    //! Create an uninitialized surface.
    /*!
      
      \warning Trying to use an uninitialized surface will result in
      crashes. Most functions to not have any checks to see whether
      the surface is initialized or not.
    */
    Surface(): BaseSurface(0) {   }
    
    //! Implementation of operator=
    Surface &operator=(const Surface& other) {
      BaseSurface::operator=(other);
      if (me)
        ++(me->refcount);
      return *this;
    }

    Surface Duplicate() const {
      Surface new_surface(CreateSurface(*this));
      new_surface.Blit(*this); // copy pixels
      return new_surface;
    }

    static Surface CreateSurface(const BaseSurface& other) {
      return Surface(
          SDL_CreateRGBSurface(other.w(), other.h(),
                               other.GetPixelFormat().BitsPerPixel(),
                               other.pitch(),
                               other.GetPixelFormat().Rmask(),
                               other.GetPixelFormat().Gmask(),
                               other.GetPixelFormat().Bmask(),
                               other.GetPixelFormat().Amask()));
    }

    //! Allocate an empty RGB surface.
    /*!
      If depth is 8 bits an empty palette is allocated for the
      surface, otherwise a 'packed-pixel' SDL_PixelFormat is created
      using the [RGBA]mask's provided. The flags specifies 

      \param flags the type of surface that should be created which is
      OR'd combination of the values as described in the SDL
      documentation.
      \param w, h width and height of the surface to create.
      \param d color depth in bits per pixel to use for this
      surface. If zero, use the depth of the current display.

      \param RGBAmask optional binary masks used to retrieve individual color values.
    */     
    static Surface CreateSurface(Uint32 flags, int w, int h, int d,
        Uint32 Rmask = 0, Uint32 Gmask = 0,
        Uint32 Bmask = 0, Uint32 Amask = 0) {
      return Surface(
          SDL_CreateRGBSurface(flags, w, h, d,
            Rmask, Gmask, Bmask, Amask));
    }

    //! Create a new Surface from the provided pixel data.
    /*!
      The data stored in \a pixels is assumed to be of the depth
      specified in the parameter \a d. The pixel data is not copied
      into the SDL_Surface structure \a me so it should no be freed until
      the Surface object has been destructed

      \param flags the type of surface that should be created which is
      OR'd combination of the values as described in the SDL
      documentation.
      \param w, h width and height of the surface to create.
      \param d color depth in bits per pixel to use for this
      surface. If zero, use the depth of the current display.
      \param p pitch the length (pitch) of each scanline in bytes.
      \param RGBAmask optional binary masks used to retrieve individual color values.
    */
    static Surface CreateSurface(void *pixels, int w, int h, int d, int p,
        Uint32 Rmask = 0, Uint32 Gmask = 0,
        Uint32 Bmask = 0, Uint32 Amask = 0) {
      return Surface(
          SDL_CreateRGBSurfaceFrom(pixels, w, h, d, p,
            Rmask, Gmask, Bmask, Amask));
    }
    
    // Documented in BaseSurface
    virtual bool SetDisplayFormat();
    virtual bool SetDisplayFormatAlpha();    
        
    //! Loads a Windows BMP and returns it as a new Surface
    /*!
      If the image couldn't be loaded, the Surface will be invalid. You can
      check the success of the image loading using the Surface.valid() method
      like the example code below.
      \code
      SDLmm::Surface img(SDLmm::Surface::LoadBMP("myimage.bmp"));
      if(!img.valid()) {
        cerr << "Loading of image myimage.bmp failed!\n";
        exit(1);
      }
      \endcode
      \param file the file to attempt to load
      \returns True if the loading succeeded, false otherwise.
    */
    static Surface LoadBMP(const char *file) {
        return Surface(SDL_LoadBMP(file));
    }
    
    //! Loads a Windows BMP and returns a new Surface
    /*!
      \param file the file to attempt to load
      \returns True if the loading succeeded, false otherwise.
    */
    static Surface LoadBMP(const std::string& file) { return LoadBMP(file.c_str()); }
  };
}

#endif // SDLMM_SURFACE_H
