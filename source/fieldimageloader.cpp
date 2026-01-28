/***************************************************************************
                          textfile_evaluation.cpp  -  description
                             -------------------
    begin                : Thu Oct 06 2002
    copyright            : (C) 2002 by Martin Bickel
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

#include <vector>
#include <algorithm>
#include <iostream>
#include <SDL_image.h>
#include <boost/regex.hpp>

#include "global.h"
#include "ascstring.h"
#include "fieldimageloader.h"
#include "typen.h"
#include "graphics/blitter.h"

#include "basestreaminterface.h"
#include "stringtokenizer.h"
#include "graphics/surface2png.h"


const char* fileNameDelimitter = " =*/+<>,";


void snowify( Surface& s, bool adaptive )
{
   if ( !s.valid() )
      return;
   
   if ( s.GetPixelFormat().BitsPerPixel() != 32 )
      return;


   const int targetWhite = 210;

   float avg = 0;
   if ( adaptive ) {
      for ( int y = 0; y < s.h(); ++y ) {
         const char* c = (char*) s.pixels();
         c += y * s.pitch();
         for ( int x = 0; x < s.w(); ++x ) {
            uint32_t* i = (uint32_t*) c;
            avg +=  ((*i >> s.GetPixelFormat().Rshift()) & 0xff ) 
                  + ((*i >> s.GetPixelFormat().Gshift()) & 0xff ) 
                  + ((*i >> s.GetPixelFormat().Bshift()) & 0xff ) ;
            c += 4;
         }
      }

      avg /= float(s.h() * s.w() * 3);
   } else
      avg = 150;

   for ( int y = 0; y < s.h(); ++y ) {
      char* c = (char*) s.pixels();
      c += y * s.pitch();
      for ( int x = 0; x < s.w(); ++x ) {
         uint32_t* i = (uint32_t*) c;
         if ( ((*i >> s.GetPixelFormat().Ashift()) & 0xff ) != Surface::transparent ) {
            int v =  ((*i >> s.GetPixelFormat().Rshift()) & 0xff ) 
                  + ((*i >> s.GetPixelFormat().Gshift()) & 0xff ) 
                  + ((*i >> s.GetPixelFormat().Bshift()) & 0xff ) ;
            v /= 3;
            
            int nv = targetWhite + ( v - int(avg)) * (255-targetWhite) / 64;
            if ( nv > 255)
               nv = 255;
            if ( nv < 0 )
               nv = 0;
            
            *i = (nv << s.GetPixelFormat().Rshift()) | (nv << s.GetPixelFormat().Gshift() ) | (nv << s.GetPixelFormat().Bshift()) | (*i & s.GetPixelFormat().Amask() );
         }
         c += 4;
      }
   }
}

bool imageEmpty( const Surface&  s ) 
{
   bool allWhite = true;
   bool allTransparent = true;
   int amask = s.GetPixelFormat().Amask();
   int areference = Surface::transparent << s.GetPixelFormat().Ashift();
   
   int colmask = s.GetPixelFormat().Rmask() | s.GetPixelFormat().Gmask() | s.GetPixelFormat().Bmask();
   int colref = (0xff << s.GetPixelFormat().Rshift()) | (0xff << s.GetPixelFormat().Gshift()) | (0xff << s.GetPixelFormat().Bshift());
   
   for ( int y = 0; y < s.h(); ++y ) {
      const char* c = (char*) s.pixels();
      c += y * s.pitch();
      for ( int x = 0; x < s.w(); ++x ) {
         const uint32_t* i = (uint32_t*) c;
         if( (*i & amask) != areference )
            allTransparent  = false;

         if ( (*i & colmask) != colref )
            allWhite = false;
         
         c += 4;
      }
      if ( !allWhite && !allTransparent )
         return false;
   }
   return allWhite || allTransparent;
}

template<
int BytesPerSourcePixel,
int BytesPerTargetPixel
>
class ColorMaskTransformer
{
   public:
      typedef typename PixelSize2Type<BytesPerTargetPixel>::PixelType SourcePixelType;
      typedef typename PixelSize2Type<BytesPerTargetPixel>::PixelType TargetPixelType;
   private:
      SourcePixelType srcColorKey;
      TargetPixelType destColorKey;
      pair<int,int> rshift, gshift, bshift, ashift;  // first: shift left, second: shift right
      int rmask, gmask, bmask, amask;

      pair<int,int> calcShift(int source, int target) {
         if ( source > target )
            return make_pair(0, source-target);
         else
            return make_pair(target-source, 0);
      }
   public:
      ColorMaskTransformer( const Surface& sourceSurface, Surface& targetSurface )
      {
         rshift = calcShift( sourceSurface.GetPixelFormat().Rshift(), targetSurface.GetPixelFormat().Rshift());
         gshift = calcShift( sourceSurface.GetPixelFormat().Gshift(), targetSurface.GetPixelFormat().Gshift());
         bshift = calcShift( sourceSurface.GetPixelFormat().Bshift(), targetSurface.GetPixelFormat().Bshift());
         ashift = calcShift( sourceSurface.GetPixelFormat().Ashift(), targetSurface.GetPixelFormat().Ashift());

         rmask = sourceSurface.GetPixelFormat().Rmask();
         gmask = sourceSurface.GetPixelFormat().Gmask();
         bmask = sourceSurface.GetPixelFormat().Bmask();
         amask = sourceSurface.GetPixelFormat().Amask();
      }
      ;
      TargetPixelType convert ( SourcePixelType sp )
      {
         return ((sp & rmask) << rshift.first >> rshift.second)
               | ((sp & gmask) << gshift.first >> gshift.second)
               | ((sp & bmask) << bshift.first >> bshift.second)
               | ((sp & amask) << ashift.first >> ashift.second);
      };
};


vector<Surface> loadASCFieldImageArray ( const ASCString& file, int num, ImagePreparation* imagePreparation )
{
   vector<Surface> images;

   tnfilestream fs ( file, tnstream::reading );
   
   Surface s ( IMG_Load_RW ( SDL_RWFromStream( &fs ), 1));
   if ( !s.valid() )
      fatalError("could not read image " + file );
   
   if ( s.GetPixelFormat().BitsPerPixel() == 8 )
      s.assignDefaultPalette();

      
   int depth = s.GetPixelFormat().BitsPerPixel();
   
   try {
      for ( int i = 0; i < num; i++ ) {
         int x1 = (i % 10) * 100;
         int y1 = (i / 10) * 100;
         if ( depth > 8 && depth < 32 )
            depth = 32;

         Surface s2 = Surface::createSurface(fieldsizex,fieldsizey, depth );
         
         if ( s2.GetPixelFormat().BitsPerPixel() != 8 || s.GetPixelFormat().BitsPerPixel() != 8 ) {

            bool colorKeyAllowed = false;
            static boost::regex pcx( ".*\\.pcx$");
            boost::smatch what;
            if( boost::regex_match( copytoLower(file) , what, pcx)) {
               // warningMessage("Truecolor PCX image detected: " + file);
               colorKeyAllowed = true;
            }

            if ( s.GetPixelFormat().BitsPerPixel() == 32 ) {
               MegaBlitter<4,4,ColorTransform_None,ColorMerger_PlainOverwrite,SourcePixelSelector_Rectangle,TargetPixelSelector_All, ColorMaskTransformer> blitter;
               blitter.setSrcRectangle(SDLmm::SRect(SPoint(x1,y1),fieldsizex,fieldsizey));
               blitter.blit( s, s2, SPoint(0,0)  );
            } else {
               s2.Blit( s, SDLmm::SRect(SPoint(x1,y1),fieldsizex,fieldsizey), SPoint(0,0));
            }
            applyLegacyFieldMask(s2,0,0, colorKeyAllowed);
            if ( colorKeyAllowed )
               s2.ColorKey2AlphaChannel();

            if ( s2.GetPixelFormat().BitsPerPixel() == 8 )
               s2.SetColorKey(SDL_TRUE, 0xff);

            if ( depth != 32 || imageEmpty(s2))
               images.push_back( Surface() );
            else
               images.push_back ( s2 );


          } else {
             // we don't want any transformations from one palette to another; we just assume that all 8-Bit images use the same colorspace
             MegaBlitter<1,1,ColorTransform_None,ColorMerger_AlphaOverwrite,SourcePixelSelector_Rectangle > blitter;
             blitter.setSrcRectangle(SDLmm::SRect(SPoint(x1,y1),fieldsizex,fieldsizey));
             blitter.blit( s, s2, SPoint(0,0)  );
             applyFieldMask(s2);
             s2.detectColorKey();
             images.push_back ( s2 );
          }
      }
   } catch ( const ASCmsgException& ex) {
      throw ASCmsgException("Error loading " + file + ": " + ex.getMessage());
   }
   return images;
}


void TerrainImagePreparator::prepareImage( Surface& surface )
{
   if ( surface.w() >= fieldsizex && surface.h() >= fieldsizey )
      applyFieldMask(surface,0,0,false);
}

void VehicleImagePreparator::prepareImage( Surface& surface )
{
   if ( surface.w() >= fieldsizex && surface.h() >= fieldsizey )
      applyFieldMask(surface,0,0,false);
}


Surface loadASCFieldImage ( const ASCString& file, ImagePreparation* imagePreparation )
{
   bool save = false;

   StringTokenizer st ( file, fileNameDelimitter );
   FileName fn = st.getNextToken();
   fn.toLower();

   displayLogMessage ( 6, "loading file " + file + "\n" );

   if ( fn.suffix() == "png" ) {
      Surface* s = NULL;
      do {
         tnfilestream fs ( fn, tnstream::reading );
         RWOPS_Handler rwo( SDL_RWFromStream( &fs ) );
         Surface s2 ( IMG_LoadPNG_RW ( rwo.Get() ));
         rwo.Close();

         if ( s2.GetPixelFormat().BitsPerPixel() == 8)
            s2.SetColorKey( SDL_TRUE, 255 );

         if ( !s ) {
            if ( s2.hasDefaultPixelFormat() ) {
               s = new Surface ( s2 );
            } else {
               s = new Surface( Surface::createSurface(s2.w(),s2.h(), 32 ));
               if ( SDL_SetSurfaceBlendMode(s2.getBaseSurface(), SDL_BLENDMODE_NONE) < 0 )
                  throw ASCmsgException(ASCString("loadASCFieldImage :") + SDL_GetError());
               s->Blit(s2);
            }
         } else {
            megaBlitter<ColorTransform_None,ColorMerger_AlphaMerge,SourcePixelSelector_Plain,TargetPixelSelector_All>( s2, *s, SPoint(0,0), nullParam, nullParam, nullParam, nullParam );
         }

         fn = st.getNextToken();
      } while ( !fn.empty() );
      if ( s )  {
         Surface s3( *s );
         if ( imagePreparation )
            imagePreparation->prepareImage(s3);

         delete s;
         return s3;
      } else
         return Surface();

   } else
      if ( fn.suffix() == "pcx" ) {
         SDL_Surface* surface;
         {
            tnfilestream fs ( fn, tnstream::reading );

            RWOPS_Handler rwo ( SDL_RWFromStream( &fs ));
            surface = IMG_LoadPCX_RW ( rwo.Get() );
            rwo.Close();
         }

         if ( !surface )
            errorMessage( "error loading file " + fn );
            
         Surface s ( surface );

         if ( s.GetPixelFormat().BitsPerPixel() == 8)
            s.SetColorKey( SDL_TRUE, 255 );
         /*
         else 
            s.SetColorKey( SDL_SRCCOLORKEY, 0xfefefe );
         s.SetAlpha(SDL_SRCALPHA,SDL_ALPHA_OPAQUE);
         */

         fn = st.getNextToken();
         while ( !fn.empty() ) {
            tnfilestream fs ( fn, tnstream::reading );
            RWOPS_Handler rwo( SDL_RWFromStream( &fs ) );
            SDLmm::Surface s2 ( IMG_LoadPNG_RW ( rwo.Get() ));
            rwo.Close();
            int res = s.Blit ( s2 );
            if ( res < 0 )
               warningMessage ( "ImageProperty::operation_eq - couldn't blit surface "+fn);

            fn = st.getNextToken();
         }


         if ( imagePreparation )
            imagePreparation->prepareImage(s);

         return s;
      }
   throw ASCexception();
}


