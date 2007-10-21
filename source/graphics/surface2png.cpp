
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

#ifdef NO_INTERNAL_PNG
#include <SDL_image.h>
#else
#include <png.h>
#endif

#include "surface2png.h"
#include "../messaginghub.h"

void writePNG( const ASCString& filename, const Surface& s )
{
   writePNG( filename, s, 0,0, s.w(), s.h());
}
 
void writePNGtrim( const ASCString& filename, const Surface& s )
{
   int x2 = 0;
   int y2 = 0;
   int x1 = s.w();
   int y1 = s.h();
   
   for ( int y = 0; y < s.h(); ++y )
      for ( int x = 0; x < s.w(); ++x )
         if ( s.GetPixelFormat().GetRGBA( s.GetPixel(x,y)).a != Surface::transparent ) {
            x1 = min(x1, x);
            y1 = min(y1, y);
            x2 = max(x2, x);
            y2 = max(y2, y); 
         }
         
   writePNG( filename, s, x1,y1, x2-x1+1, y2-y1+1);
}

void writePNG( const ASCString& filename, const Surface& s, const SDLmm::SRect& rect )
{
   writePNG( filename, s, rect.x, rect.y, rect.w, rect.h);
}


#ifdef NO_INTERNAL_PNG

void writePNG( const ASCString& filename, const Surface& s, int x1, int y1, int w, int h  )
{
   if ( s.GetPixelFormat().BytesPerPixel () != 4 ) {
      errorMessage("Only 32 Bit images are supported for PNG writing");
      return;
   } 

   SDL_RWops *rw=SDL_RWFromFile( filename.c_str(),"wb");
   if(rw==NULL) {
      errorMessage("Could not open " + filename + " for writing"); 
      return;
   }
   
   IMG_WritePNG_RW( rw, s.GetSurface(), x1,y1,w,h);

   SDL_RWclose(rw);

}

#else

void writePNG( const ASCString& filename, const Surface& s, int x1, int y1, int w, int h  )
{

   if ( x1 < 0 )
      x1 = 0;
   if ( y1 < 0 )
      y1 = 0;
   if ( w >= s.w()-x1 )
      w = s.w() -x1 ;
   if ( h >= s.h()-y1 )
      h = s.h() -y1 ;
    
   if ( s.GetPixelFormat().BytesPerPixel() != 4 && s.GetPixelFormat().BytesPerPixel() != 1 ) {
      errorMessage("Only 8 Bit and 32 Bit images are supported for PNG writing");
      return;
   } 

   
   FILE* fp = fopen(filename.c_str(), "wb");
   if( fp == NULL)
   {
      errorMessage("Could not open " + filename + " for writing"); 
      return;
   }

   png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
   png_infop info_ptr = png_create_info_struct(png_ptr);
   png_init_io(png_ptr, fp);

   png_color my_palette[256];

   if ( s.GetPixelFormat().BytesPerPixel() == 4 ){
      png_set_IHDR(png_ptr, info_ptr, w, h, 8, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
   } else {
      png_set_IHDR(png_ptr, info_ptr, w, h, 8, PNG_COLOR_TYPE_PALETTE, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

      SDL_Palette* sdlpal = s.GetPixelFormat().palette();
      if ( sdlpal->ncolors > 256 ) {
         errorMessage("A palettes with more than 256 colors is not supported"); 
         return;
      }

      for ( int c = 0; c < sdlpal->ncolors; ++c ) {
         my_palette[c].red = sdlpal->colors[c].r;
         my_palette[c].green = sdlpal->colors[c].g;
         my_palette[c].blue = sdlpal->colors[c].b;
      }

      png_set_PLTE(png_ptr, info_ptr, my_palette, sdlpal->ncolors);
   }

   time_t          gmt;
   png_time        mod_time;
   time(&gmt);
   png_convert_from_time_t(&mod_time, gmt);
   png_set_tIME(png_ptr, info_ptr, &mod_time);
   

   png_text        text_ptr[1];
   text_ptr[0].key = const_cast<char*>("Title");
   text_ptr[0].text = const_cast<char*>("Advanced Strategic Command");
   text_ptr[0].compression = PNG_TEXT_COMPRESSION_NONE;
   png_set_text(png_ptr, info_ptr, text_ptr, 1);
   

   Uint8* linebuf = new Uint8[w*4];
   
   png_write_info(png_ptr, info_ptr);
   

   int iii = 0;
   char* pix = (char*)s.pixels();
   for ( int y = y1; y < y1 + h; ++y ) {
      Uint32* srcpix32 = (Uint32*)(pix + y * s.pitch());
      srcpix32 += x1;
      Uint8* srcpix8 = (Uint8*)(pix + y * s.pitch());
      srcpix8 += x1;
      if ( s.GetPixelFormat().BytesPerPixel() == 4) {
         for ( int x = 0; x < w; ++x ) 
            s.GetPixelFormat().GetRGBA( srcpix32[x], linebuf[x*4],linebuf[x*4+1],linebuf[x*4+2],linebuf[x*4+3] ); 
      } else {
         if ( s.GetPixelFormat().BytesPerPixel() == 1)
            for ( int x = 0; x < w; ++x )  {
               linebuf[x] = srcpix8[x];
               if ( linebuf[x] )
                  ++iii; 
            }
      }
      png_write_row(png_ptr, (png_bytep) linebuf);
   }
   delete[] linebuf;
   
   png_write_end(png_ptr, info_ptr);
   png_destroy_write_struct(&png_ptr, &info_ptr);
   fclose(fp);
}
#endif


