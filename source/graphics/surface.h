/*
    This file is part of Advanced Strategic Command; http://www.asc-hq.de
    Copyright (C) 1994-2003  Martin Bickel  and  Marc Schellenberger

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

#ifndef surfaceH
 #define surfaceH

 #include "../libs/sdlmm/src/sdlmm.h"
 #include "../basestreaminterface.h"

 typedef SDLmm::SPoint SPoint;

 class Surface: public SDLmm::Surface {
    public:
      static const int transparent = 0;
      static const int opaque = 0xff;
      explicit Surface( SDL_Surface *surface);
      Surface(const SDLmm::Surface& other);
      Surface() : SDLmm::Surface(NULL) {};

      static Surface createSurface( int width, int height, SDLmm::Color color = 255 );
      static Surface createSurface( int width, int height, int depth, SDLmm::Color color = 255 );
      
      /**
         Creates an image from an BGI image structure.
      */
      void  newFromBGI( void* img );
      void* toBGI() const;

      void read ( tnstream& stream ) ;
      void write ( tnstream& stream ) const;
      void strech ( int width, int height );

      void writeDefaultPixelFormat ( tnstream& stream ) ;
      static void readDefaultPixelFormat ( tnstream& stream );

      //! assigns the default ASC palette to the surface (only for 8 Bit surfaces)
      void assignDefaultPalette();

      void assignPalette(SDL_Color* colors, int startColor = 0, int colorNum = 256 );

      //! tries to automatically detect the color key of the surface
      void detectColorKey( bool RLE = false );
      
   protected:
      virtual int getDepthFormat() { return -1; };
      void convert();
           
   private:
      static SDLmm::PixelFormat* default8bit;
      static SDLmm::PixelFormat* default32bit;

 };

 class TypedSurfaceBase  : public Surface{
    protected:
      explicit TypedSurfaceBase( SDL_Surface *surface) : Surface(surface) {};
      TypedSurfaceBase(const SDLmm::Surface& other) : Surface( other ) {};
      TypedSurfaceBase() : Surface(NULL) {};
 };     
 
 template<int colorDepth> class TypedSurface : public TypedSurfaceBase {
    public:
      static const int depth = colorDepth;
      explicit TypedSurface( SDL_Surface *surface) : TypedSurfaceBase(surface) {};
      
      //! the parameter depthcheck is primarily there to prevent accidential usage of this constructor
      explicit TypedSurface( SDLmm::Surface& surface , int depthCheck ) : TypedSurfaceBase(surface) {
         assert ( surface.GetPixelFormat().BytesPerPixel() == depth );
         assert ( depthCheck == depth );
      };
      
      TypedSurface(const TypedSurface<colorDepth>& other) : TypedSurfaceBase( other ) {};
      TypedSurface() : TypedSurfaceBase(NULL) {};
   protected:
      virtual int getDepthFormat() { return depth; };
 };
 
 typedef TypedSurface<1> Surface8;
 typedef TypedSurface<4> Surface32;
 
 
 template<int depth> TypedSurface<depth>& castSurface( Surface& s ) {
    assert ( s.GetPixelFormat().BytesPerPixel() == depth );
    return static_cast<TypedSurface<depth>& >(s);
 };
 
 
 void applyFieldMask( Surface& s, int x = 0, int y = 0 );
 
 //! applies a field mask that uses FEFEFE Color as Colorkey to load old images
 void applyLegacyFieldMask( Surface& s, int x = 0, int y = 0 );

 Surface rotateSurface( Surface& s, int degrees );

class SurfaceLock {
      Surface& surf;
   public:
      SurfaceLock( Surface& s ) : surf(s) { s.Lock(); };
      ~SurfaceLock() { surf.Unlock(); };
};

 
 
#endif

