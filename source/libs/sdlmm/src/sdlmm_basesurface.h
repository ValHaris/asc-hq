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


#ifndef SDLMM_BASESURFACE_H
#define SDLMM_BASESURFACE_H
#include "sdlmm_config.h"
#include "sdlmm_spoint.h"
#include "sdlmm_srect.h"
#include "sdlmm_color.h"
#include "sdlmm_pixelformat.h"
#include <string>

namespace SDLmm {
  //! An abstract base class for graphical surfaces
  /*!

    Surfaces represent areas of "graphical" memory, memory that can be
    drawn to. The video framebuffer is returned as a VideoSurface by
    SetVideoMode() and GetVideoSurface(). Ordinary (non-framebuffer
    surfaces) are represented by a Surface. The clipping rectangle
    returned by clip_rect() can be set with SetClipRect() method.

    \author David Hedbor <david@hedbor.org>
  */
  class DECLSPEC BaseSurface {

  protected:
    //! The actual SDL_Surface allocated for this BaseSurface.
    SDL_Surface *me;
    virtual void SetSurface(SDL_Surface *surface) {
      if(me) {
        SDL_FreeSurface(me);
      }
      me = surface;
    }

    //! Constructor from an SDL_Surface*
    /*!
      This creates a new BaseSurface object from an existing
      SDL_Surface. Note that ownership of the SDL_Surface is passed
      on to the BaseSurface. It's very important not to free the original
      surface since that will cause a problem when the object is
      destructed (SDL_Surface storage freed twice). Use with caution.
    */
    explicit BaseSurface(SDL_Surface *surface)
      : me(surface) {
    }
    
    BaseSurface(const BaseSurface& other)
      : me(other.me) {
    }
    
    BaseSurface &operator=(const BaseSurface& other) {
      if(this != &other)
        SetSurface(other.me);
      return *this;
    }
    
  public:

    //! The destructor.
    virtual ~BaseSurface() {
      if(me) {
        SDL_FreeSurface(me);
      }
    }
    
    SDL_Surface *GetSurface() {
      ASSERT(me);
      return me;
    }

    const SDL_Surface *GetSurface() const {
      ASSERT(me);
      return me;
    }

    //! Lock the surface to allow direct access to the surface pixels.
    //! Returns true if lock succeeded, false otherwise.
    bool Lock();

    //! Unlock the surface.
    void Unlock();
    
    //! \name Informational methods
    //@{
    //! Returns true if this surface is initialized, false otherwise.
    /*! \warning Using an uninitialzied surface can cause many problems. */
    bool valid() const { return me != 0; }

    //! Returns the surface flags.
    Uint32 flags() const { return GetSurface()->flags; } 		   

    //! Returns the pixel format.
    const PixelFormat GetPixelFormat() const { return PixelFormat(GetSurface()->format); }
    PixelFormat GetPixelFormat() { return PixelFormat(GetSurface()->format); }

    //! Returns the width of the surface.
    int w() const { return GetSurface()->w; }			   

    //! Returns the height of the surface.
    int h() const { return GetSurface()->h; }			   

    //! Returns the scanline length in bytes
    Uint16 pitch() const { return GetSurface()->pitch; }		   

    //! Returns the surface clip rectangle
    const SRect clip_rect() const { return SRect(GetSurface()->clip_rect); } 

    //! Returns the pixel data, which can be used for low-level manipulation.
    /*!
      \warning You can only modify this surface when the surface is locked.
    */
    void *pixels() { return GetSurface()->pixels; }

    //! Returns the pixel data, which can be used for low-level manipulation.
    /*!
      \warning You can only modify this surface when the surface is locked.
    */
    const void *pixels() const { return GetSurface()->pixels; }

    //! Returns the hardware-specific surface info.
    struct private_hwdata *hwdata() const { return GetSurface()->hwdata; }
    //@}

    //! Set the pixel to the color
    /*!
      \warning You can only use this function when the surface is locked.
    */
    void SetPixel(int x, int y, Color color);

    //! Set the pixel to the color
    /*!
      \warning You can only use this function when the surface is locked
      and when the bytes per pixel is 1.
    */
    void SetPixel1(int x, int y, Color color);

    //! Set the pixel to the color
    /*!
      \warning You can only use this function when the surface is locked
      and when the bytes per pixel is 2.
    */
    void SetPixel2(int x, int y, Color color);

    //! Set the pixel to the color
    /*!
      \warning You can only use this function when the surface is locked
      and when the bytes per pixel is 3.
    */
    void SetPixel3(int x, int y, Color color);

    //! Set the pixel to the color
    /*!
      \warning You can only use this function when the surface is locked
      and when the bytes per pixel is 4.
    */
    void SetPixel4(int x, int y, Color color);

    //! Set the pixel to the color
    /*!
      \warning You can only use this function when the surface is locked.
    */
    void SetPixel(const SRect& point, Color color) { SetPixel(point.x, point.y, color); }

    //! Get the color of the pixel
    /*!
      \warning You can only use this function when the surface is locked.
    */
    Color GetPixel(int x, int y) const;

    //! Get the color of the pixel
    /*!
      \warning You can only use this function when the surface is locked.
    */
    Color GetPixel(const SRect& point) const { return GetPixel(point.x, point.y); }

    //! Sets the color key (transparent pixel) in a blittable surface
    //! and enables or disables RLE blit acceleration.
    /*!
      RLE acceleration can substantially speed up blitting of images
      with large horizontal runs of transparent pixels (i.e., pixels
      that match the \a key value). The key must be of the same pixel
      format as the surface, MapRGB() is often useful for obtaining an
      acceptable value.

      \returns Returns true for success, false if there was an error.
      \param flag If */
    bool SetColorKey(Uint32 flag, Color key);
      
    //! Adjust the alpha properties of this surface
    /*!

    SetAlpha is used for setting the surface alpha value and / or
    enabling and disabling alpha blending.

    \param flag used to specify whether alpha blending should be
    used (SDL_SRCALPHA) and whether the surface should use RLE
    acceleration for blitting (SDL_RLEACCEL). \a flag can be an OR'd
    combination of these two options, one of these options or 0. If
    SDL_SRCALPHA is not passed as a flag then all alpha information
    is ignored when blitting the surface.
      
    \param alpha the per-surface alpha value; a surface need not
    have an alpha channel to use per-surface alpha and blitting can
    still be accelerated with SDL_RLEACCEL.

    \note The per-surface alpha value of 128 is considered a special
    case and is optimised, so it's much faster than other
    per-surface values.

    \returns true for success or false if there was an error.
    */
    bool SetAlpha(Uint32 flag, Uint8 alpha);

    //! \name Clipping Methods
    //@{
    //! Resets the clipping rectangle for the surface.
    /*!
      This functions resets the clipping to the full size of the surface. 
      \sa GetClipRect, SetClipRect, Blit
    */
    void ResetClipRect();

    //! Sets the clipping rectangle for the surface.
    /*!
      Sets the clipping rectangle for a surface. When this surface is
      the destination of a blit, only the area within the clip
      rectangle will be drawn into.

      \param rect The rectangle pointed to by rect will be clipped to
      the edges of the surface so that the clip rectangle for a
      surface can never fall outside the edges of the surface.

      \sa GetClipRect, ResetClipRect, Blit
    */
    void SetClipRect(const SDL_Rect& rect);
    
    //! Gets the clipping rectangle for the surface.
    /*!
      Gets the clipping rectangle for a surface. When this surface is
      the destination of a blit, only the area within the clip
      rectangle is drawn into.

      \param rect reference to a rectangle which will be filled with
      the clipping rectangle of this surface.
      \sa SetClipRect, ResetClipRect, Blit
    */
    void GetClipRect(SDL_Rect& rect) const;
    //@}

    //! \name Blitting / Filling
    //@{
    //! Fast blit the entire source surface onto this surface.
    /*!
      The source surface will be blitted to this surface. If the
      source area is larger than the destination, clipping will
      occur. No scaling will be performed. Blitting should not be used
      on a locked surface. The entire surface will be copied to this
      surface using this function.

      \return If the blit is successful, it returns 0, otherwise it
      returns -1. If either of the surfaces were in video memory,
      and the blit returns -2, the video memory was lost. It should
      be reloaded with artwork and re-blitted.

      \param src the surface to blit from.
    */    
    int Blit(const BaseSurface& src);

    //! Fast blit the entire source surface onto this surface at the
    //! specified coordinates.
    /*!
      The source surface will be blitted to this surface. If the
      source area is larger than the destination, clipping will
      occur. No scaling will be performed. Blitting should not be used
      on a locked surface. The entire surface will be copied to this
      surface using this function. The final blit rectangle is saved
      in \a dstrect after all clipping is performed.

      \return If the blit is successful, it returns 0, otherwise it
      returns -1. If either of the surfaces were in video memory,
      and the blit returns -2, the video memory was lost. It should
      be reloaded with artwork and re-blitted.

      \param src the surface to blit from.
      \param dstrect the destination coordinates. Only the position 
      is used (i.e width and height are ignored). The width and height are
      set upon returning.
    */
    int Blit(const BaseSurface& src, SDL_Rect& dstrect);

    //! Fast blit the entire source surface onto this surface at the
    //! specified coordinates.
    /*!
      The source surface will be blitted to this surface. If the
      source area is larger than the destination, clipping will
      occur. No scaling will be performed. Blitting should not be used
      on a locked surface. The entire surface will be copied to this
      surface using this function. The final blit rectangle is saved
      in \a dstrect after all clipping is performed.

      \return If the blit is successful, it returns 0, otherwise it
      returns -1. If either of the surfaces were in video memory,
      and the blit returns -2, the video memory was lost. It should
      be reloaded with artwork and re-blitted.

      \param src the surface to blit from.
      \param dstpoint the destination coordinates.
      \param dstrect the final position and size is set upon returning.
    */
    int Blit(const BaseSurface& src, const SPoint& dstpoint, SDL_Rect& dstrect) { dstrect.x = dstpoint.x; dstrect.y = dstpoint.y; return Blit(src, dstrect); }

    //! Fast blit the entire source surface onto this surface at the
    //! specified coordinates.
    /*!
      The source surface will be blitted to this surface. If the
      source area is larger than the destination, clipping will
      occur. No scaling will be performed. Blitting should not be used
      on a locked surface. The entire surface will be copied to this
      surface using this function.

      \return If the blit is successful, it returns 0, otherwise it
      returns -1. If either of the surfaces were in video memory,
      and the blit returns -2, the video memory was lost. It should
      be reloaded with artwork and re-blitted.

      \param src the surface to blit from.
      \param dstpoint the destination coordinates.
    */
    int Blit(const BaseSurface& src, const SPoint& dstpoint);

    //! Fast blit the specified area part of the source surface onto
    //! this surface at the specified coordinates.
    /*!
      The choosen rectangle of the source surface will be blitted to
      this surface. If the source area is larger than the destination,
      clipping will occur. No scaling will be performed. Blitting
      should not be used on a locked surface. The entire surface will
      be copied to this surface using this function. The final blit
      rectangle is saved in \a dstrect after all clipping is performed
      (\a srcrect is not modified).

      \return If the blit is successful, it returns 0, otherwise it
      returns -1. If either of the surfaces were in video memory,
      and the blit returns -2, the video memory was lost. It should
      be reloaded with artwork and re-blitted.

      \param src the surface to blit from.
      \param srcrect the rectangular area to copy from the source
      surface.
      \param dstrect the destination coordinates. Only the position 
      is used (i.e width and height are ignored). The width and height are
      set upon returning.
    */
    int Blit(const BaseSurface& src, const SDL_Rect& srcrect, SDL_Rect& dstrect);

    //! Fast blit the specified area part of the source surface onto
    //! this surface at the specified coordinates.
    /*!
      The choosen rectangle of the source surface will be blitted to
      this surface. If the source area is larger than the destination,
      clipping will occur. No scaling will be performed. Blitting
      should not be used on a locked surface. The entire surface will
      be copied to this surface using this function. The final blit
      rectangle is saved in \a dstrect after all clipping is performed
      (\a srcrect is not modified).

      \return If the blit is successful, it returns 0, otherwise it
      returns -1. If either of the surfaces were in video memory,
      and the blit returns -2, the video memory was lost. It should
      be reloaded with artwork and re-blitted.

      \param src the surface to blit from.
      \param srcrect the rectangular area to copy from the source
      surface.
      \param dstpoint the destination coordinates.
      \param dstrect the final position and size is set upon returning.
    */
    int Blit(const BaseSurface& src, const SDL_Rect& srcrect, const SPoint& dstpoint, SDL_Rect& dstrect) { dstrect.x = dstpoint.x; dstrect.y = dstpoint.y; return Blit(src, srcrect, dstrect); }

    //! Fast blit the specified area part of the source surface onto
    //! this surface at the specified coordinates.
    /*!
      The choosen rectangle of the source surface will be blitted to
      this surface. If the source area is larger than the destination,
      clipping will occur. No scaling will be performed. Blitting
      should not be used on a locked surface. The entire surface will
      be copied to this surface using this function.

      \return If the blit is successful, it returns 0, otherwise it
      returns -1. If either of the surfaces were in video memory,
      and the blit returns -2, the video memory was lost. It should
      be reloaded with artwork and re-blitted.

      \param src the surface to blit from.
      \param srcrect the rectangular area to copy from the source
      surface.
      \param dstpoint the destination coordinates.
    */
    int Blit(const BaseSurface& src, const SDL_Rect& srcrect, const SPoint& dstpoint);

    //! Fast fill the surface with the specified color
    /*!
      If a clipping rectangle has been set using SetClipRect(), only the
      area within that rectangle will be filled.
      \returns Returns true for success, false if there was an error.
      \param color the color to fill with, generated by MapRGB() or MapRGBA().
    */
    bool Fill(Color color);

    //! Fast fill the surface with the specified color
    /*!
      If a clipping rectangle has been set using SetClipRect(), only the
      area within that rectangle will be filled.
      \returns Returns true for success, false if there was an error.
      \param r, g, b the red, green and blue color values.  
    */
    bool Fill(Uint8 r, Uint8 g, Uint8 b) { return Fill(GetPixelFormat().MapRGB(r, g, b)); }

    //! Fast fill the surface with the specified color
    /*!
      If a clipping rectangle has been set using SetClipRect(), only the
      area within that rectangle will be filled.
      \returns Returns true for success, false if there was an error.
      \param r, g, b the red, green and blue color values.
      \param a the alpha value
    */
    bool Fill(Uint8 r, Uint8 g, Uint8 b, Uint8 a) { return Fill(GetPixelFormat().MapRGBA(r, g, b, a)); }

    //! Performs a fast fill of the given rectangle with the specified color
    /*!
      If a clipping rectangle has been set using SetClipRect(), the area
      filled will be the intersection of the clipping rectangle and
      \a dstrect.
      \returns Returns true for success, false if there was an error.
      \param dstrect the rectangle to fill, upon returning it contains the 
        clipped rectangle that was actually filled.
      \param color the color to fill with, generated by MapRGB() or MapRGBA().
    */
    bool FillRect(SDL_Rect& dstrect, Color color);

    //! Performs a fast fill of the given rectangle with the specified color
    /*!
      If a clipping rectangle has been set using SetClipRect(), the area
      filled will be the intersection of the clipping rectangle and
      \a dstrect.
      \returns Returns true for success, false if there was an error.
      \param dstrect the rectangle to fill, upon returning it contains the 
        clipped rectangle that was actually filled.
      \param r, g, b the red, green and blue color values.  
    */
    bool FillRect(SDL_Rect& dstrect, Uint8 r, Uint8 g, Uint8 b) { return FillRect(dstrect, GetPixelFormat().MapRGB(r, g, b)); }

    //! Performs a fast fill of the given rectangle with the specified color
    /*!
      If a clipping rectangle has been set using SetClipRect(), the area
      filled will be the intersection of the clipping rectangle and
      \a dstrect.
      \returns Returns true for success, false if there was an error.
      \param dstrect the rectangle to fill, upon returning it contains the 
        clipped rectangle that was actually filled.
      \param r, g, b the red, green and blue color values.
      \param a the alpha value
    */
    bool FillRect(SDL_Rect& dstrect, Uint8 r, Uint8 g, Uint8 b, Uint8 a) { return FillRect(dstrect, GetPixelFormat().MapRGBA(r, g, b, a)); }

    //@}

    //! Convert the surface to the display format.
    /*!
      This function converts the surface to the pixel format and
      colors of the video framebuffer, making it suitable for fast
      blitting onto the display surface. 

      If you want to take advantage of hardware colorkey or alpha blit
      acceleration, you should set the colorkey and alpha value before
      calling this function.
      
      \returns The functions returns true if the conversion
      succeeded or false otherwise. If the conversion failed, the
      BaseSurface object will not have changed.

      \note Please note that this function doesn't return a new,
      modified object like the SDL_DisplayFormat() function does. Thus
      there is no need to manually free the old surface.

      \sa SetDisplayFormatAlpha(), SetAlpha(), SetColorKey()
    */
    virtual bool SetDisplayFormat() = 0;

    //! Convert the surface to the display format.
    /*!
      This function converts the surface to the pixel format and
      colors of the video framebuffer plus an alpha channel, making it
      suitable for fast blitting onto the display surface.

      If you want to take advantage of hardware colorkey or alpha blit
      acceleration, you should set the colorkey and alpha value before
      calling this function.
      
      \returns The functions returns true if the conversion
      succeeded or false otherwise. If the conversion failed, the
      BaseSurface object will not have changed.

      \note Please note that this function doesn't return a new,
      modified object like the SDL_DisplayFormatAlpha() function
      does. Thus there is no need to manually free the old surface.

      \sa SetDisplayFormat(), SetAlpha(), SetColorKey()
    */
    virtual bool SetDisplayFormatAlpha() = 0;

    //! Save a BaseSurface object as a Windows bitmap file
    /*!
      \param file the file name to save to.
      \returns True if the loading succeeded, false otherwise.
    */
    bool SaveBMP(const char *file) const;
    
    //! Save a BaseSurface object as a Windows bitmap file
    /*!
      \param file the file name to save to.
      \returns True if the loading succeeded, false otherwise.
    */
    bool SaveBMP(const std::string& file) const { return SaveBMP(file.c_str()); }
   
  };
}

#endif // SDLMM_BASESURFACE_H
