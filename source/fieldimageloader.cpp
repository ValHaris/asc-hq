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
            avg += c[0] + c[1] + c[2];
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
         if ( c[3] != Surface::transparent ) {
            int v = (c[0] + c[1] + c[2]) / 3;

            int nv = targetWhite + ( v - int(avg)) * (255-targetWhite) / 64;
            if ( nv > 255)
               nv = 255;
            if ( nv < 0 )
               nv = 0;
            c[0] = c[1] = c[2] = nv;
         }
         c += 4;

      }
   }
}

bool imageEmpty( const Surface&  s ) 
{
   bool allWhite = true;
   bool allTransparent = true;
   for ( int y = 0; y < s.h(); ++y ) {
      const char* c = (char*) s.pixels();
      c += y * s.pitch();
      for ( int x = 0; x < s.w(); ++x ) {
         if(  c[3] != Surface::transparent )
            allTransparent  = false;
         else {
            int* i = (int*) c;
            if ( (*i & 0xffffff) != 0xffffff )
               allWhite = false;
         }
      }
      if ( !allWhite && !!allTransparent )
         return false;
   }
   return allWhite || allTransparent;
}

vector<Surface> loadASCFieldImageArray ( const ASCString& file, int num )
{
   vector<Surface> images;

   int xsize;
   if ( num <= 10)
      xsize = (num+1)* 100;
   else
      xsize = 1100;

   
   tnfilestream fs ( file, tnstream::reading );
   
   Surface s ( IMG_Load_RW ( SDL_RWFromStream( &fs ), 1));
   if ( !s.valid() )
      fatalError("could not read image " + file );
   
   if ( s.GetPixelFormat().BitsPerPixel() == 8 )
      s.assignDefaultPalette();

      
   int depth = s.GetPixelFormat().BitsPerPixel();
   
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
            // warning("Truecolor PCX image detected: " + file);
            colorKeyAllowed = true;
         }
         
         if ( s.GetPixelFormat().BitsPerPixel() == 32 ) {
            MegaBlitter<4,4,ColorTransform_None,ColorMerger_PlainOverwrite,SourcePixelSelector_Rectangle > blitter;
            blitter.setSrcRectangle(SDLmm::SRect(SPoint(x1,y1),fieldsizex,fieldsizey));
            blitter.blit( s, s2, SPoint(0,0)  );
         } else {
            s2.Blit( s, SDLmm::SRect(SPoint(x1,y1),fieldsizex,fieldsizey), SPoint(0,0));
         }
         applyLegacyFieldMask(s2,0,0, colorKeyAllowed);
         if ( colorKeyAllowed ) 
            s2.ColorKey2AlphaChannel();
         
         s2.detectColorKey();
         if ( imageEmpty(s2))
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
   return images;
}

Surface loadASCFieldImage ( const ASCString& file, bool applyFieldMaskToImage )
{
   StringTokenizer st ( file, fileNameDelimitter );
   FileName fn = st.getNextToken();
   fn.toLower();
   if ( fn.suffix() == "png" ) {
      SDLmm::Surface* s = NULL;
      do {
         tnfilestream fs ( fn, tnstream::reading );
         RWOPS_Handler rwo( SDL_RWFromStream( &fs ) );
         SDLmm::Surface s2 ( IMG_LoadPNG_RW ( rwo.Get() ));
         rwo.Close();
         // s2.SetAlpha ( SDL_SRCALPHA, SDL_ALPHA_OPAQUE );
         if ( !s )
            s = new SDLmm::Surface ( s2 );
         else {
            int res = s->Blit ( s2 );
            if ( res < 0 )
               warning ( "ImageProperty::operation_eq - couldn't blit surface "+fn);
         }

         fn = st.getNextToken();
      } while ( !fn.empty() );
      if ( s )  {
         Surface s3( *s );
         if ( applyFieldMaskToImage )
            applyFieldMask(s3,0,0,false);

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
            s.SetColorKey( SDL_SRCCOLORKEY, 255 );
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
               warning ( "ImageProperty::operation_eq - couldn't blit surface "+fn);

            fn = st.getNextToken();
         }


         if ( applyFieldMaskToImage )
            if ( s.w() >= fieldsizex && s.h() >= fieldsizey )
               applyFieldMask(s,0,0,false);

         return s;
      }
   throw ASCexception();
}


