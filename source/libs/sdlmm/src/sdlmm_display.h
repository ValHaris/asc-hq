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


#ifndef SDLMM_DISPLAY_H
#define SDLMM_DISPLAY_H

#include "sdlmm_basesurface.h"

namespace SDLmm {
  //! The video frame buffer surface.
  /*!
    This class represents the actual video frame buffer memory. If you
    are using hardware video support, the actual memory is stored on
    the graphics card. Please note that if you create more than one
    Display instance, they will all reference the same actual surface
    (i.e the current display).

    \note Since the actual SDL_Surface representation is shared among all
    Display instances, they will always reference the correct surface.
  */
  class DECLSPEC Display : public BaseSurface {
  protected:
    //! Dummy implementation of operator=() to stop initialization.
    Display& operator= (Display&);
    Display(Display&);
    
    Display() : BaseSurface(NULL)  {  } //!< A NOOP constructor.

  public:    
    ~Display() {
      me = 0; // Or it will be destroyed by the BaseSurface destructor...
    } 

    //! The one and only display.
    static Display& GetDisplay();

    //! \name Methods updating the screen
    //@{
    //! Makes sure the given area is updated on the screen.
    /*!
      If '\a x', '\a y', '\a w' and '\a h' are all 0 (the default),
      UpdateRect() will update the entire display.

      This function should not be called if Display is locked!

      \param x, y the top left corner of the rectangle to update
      \param w, h the width and height of the rectangle.

      \sa UpdateRect(SDL_Rect& rect), UpdateRects, SRect
    */
    void UpdateRect(Sint32 x = 0, Sint32 y = 0, Sint32 w = 0, Sint32 h = 0);

    //! Makes sure the entire screen is updated
    /*!
      This call is identical to calling UpdateRect() without arguments.

      This function should not be called if Display is locked!

      \sa UpdateRect, UpdateRects, SRect
    */
    void Update() { UpdateRect(); }

    //! Makes sure the given area is updated on the display.
    /*!
      This function should not be called if Display is locked!

      \param rect the rectangle to update. 
      \sa UpdateRects, SRect
    */
    void UpdateRect(SDL_Rect& rect);

    //! Makes sure the given list of rectangles is updated on the display
    /*!
      This function should not be called if Display is locked!
      
      \note It is adviced to call this function only once per frame,
      since each call has some processing overhead. This is no
      restriction since you can pass any number of rectangles each
      time.

      The rectangles are not automatically merged or checked for
      overlap. In general, the programmer can use his knowledge about
      his particular rectangles to merge them in an efficient way, to
      avoid overdraw.

      \param numrects the number of rectangles in the array.
      \param rects the array of rectangles to update
      \sa UpdateRect, SRect, Update
    */
    void UpdateRects(int numrects, SDL_Rect *rects);

    //! Swaps screen buffers.
    /*!
      On hardware that supports double-buffering, this function sets
      up a flip and returns. The hardware will wait for vertical
      retrace, and then swap video buffers before the next video
      surface Blit or Lock will return. On hardware that doesn't
      support double-buffering, this is equivalent to calling
      UpdateRect() (i.e w/o parameters).

      The SDL_DOUBLEBUF flag must have been passed to SetVideoMode,
      when setting the video mode for this function to perform
      hardware flipping.
      \return true for success, false for failure.
    */

    bool Flip() { return SDL_Flip(me) == 0; }
    //@}
   
    //! Setup the video mode with the specified width, height and
    //! color depth.
    /*!
      If bpp is zero, the color depth uses is that of the current
      display.
      
      \returns Returns true for success and false for failure.
      \param w, h width and height
      \param bpp color depth in bits-per-pixel
      \param flags the video flags
    */
    bool SetVideoMode(int w, int h, int bpp = 0, Uint32 flags = 0);

    //! \name Video Modes / Setup
    //@{
    //! Initializes the video subsystem
    /*!
      Call this method to setup the video subsystem. 
      \return true on success, false on error */
    static bool Init();

    //! Shut down the video subsystem
    static void Quit();
    
    //! Check to see if a particular video mode is supported.
    /*!

      VideoModeOK returns 0 if the requested mode is not supported
      under any bit depth, or returns the bits-per-pixel of the
      closest available mode with the given width, height and
      requested surface flags (see SetVideoMode).

      The bits-per-pixel value returned is only a suggested mode. You
      can usually request any bpp you want when setting the video mode
      and SDL will emulate that color depth with a shadow video
      surface.

      The arguments to VideoModeOK are the same ones you would pass to
      SetVideoMode.

      \sa SetVideoMode, VideoInfo
    */
    static int VideoModeOK(int w, int h, int bpp, Uint32 flags);

    //! Returns a pointer to an array of available screen dimensions
    //! for the given format and video flags.
    /*!
      Return a pointer to an array of available screen dimensions for
      the given format and video flags, sorted largest to
      smallest. Returns 0 if there are no dimensions available for
      a particular format, or -1 if any dimension is okay for the
      given format.

      If \a format is 0, the mode list will be for the format returned
      by VideoInfo()->vfmt(). The \a flag parameter is an OR'd
      combination of surface flags. The flags are the same as those
      used SetVideoMode and they play a strong role in deciding what
      modes are valid. For instance, if you pass SDL_HWSURFACE as a
      flag only modes that support hardware video surfaces will be
      returned.
     
      \sa SetVideoMode, VideoInfo, SRect
    */      
    static SDL_Rect **ListModes(SDL_PixelFormat *format = 0, Uint32 flags = 0);
    //@}


    //! \name Window Management
    //@{
    //! Sets the window title and icon name of the application.
    /*!
      \param title the new title
      \param icon the new icon title
    */
    void SetCaption(const char *title, const char *icon);

    //! Sets the window title and icon name of the application.
    /*!
      \param title the new title
      \param icon the new icon title
    */
    void SetCaption(const std::string& title, const std::string& icon);
    //! Gets the window title and icon name.
    /*!
      \param title, icons pointers to char * which will be set to the
      title and icon titles.
    */
    void GetCaption(char **title, char **icon);

    //! Gets the window title and icon name.
    /*!
      \param title, icons references to strings which will be set to
      the title and icon titles.
    */
    void GetCaption(std::string &title, std::string &icon);

    //! Sets the icon for the display window.
    /*! 
      This function must be called before the first call to
      SetVideoMode().

      It takes an \a icon surface, and a \a mask in MSB format.

      If mask is zero (default), the entire icon surface will be used
      as the icon.
    */
    void SetIcon(BaseSurface& icon, Uint8 *mask = 0);
      
    //! Iconify / minimize the application.

    /*!
      If the application is running in a window managed environment
      SDL attempts to iconify / minimize it. If Iconify() is
      successful, the application will receive a SDL_APPACTIVE loss
      event.

      \sa EventHandler::HandleActiveEvent()
      
      \returns true on success or false if iconification isn't
      supported or was refused by the window manager.
    */
    bool Iconify();

    //! Toggles between fullscreen and windowed mode.
    /*!
      Toggles the application between windowed and fullscreen mode, if
      supported. (X11 is the only target currently supported, BeOS
      support is experimental).

      \returns true on success, false on failure.
    */
    bool ToggleFullScreen();

    //! Grabs mouse and keyboard input.
    /*!

      Grabbing means that the mouse is confined to the application
      window, and nearly all keyboard input is passed directly to the
      application, and not interpreted by a window manager, if any.

      When mode is SDL_GRAB_QUERY the grab mode is not changed, but
      the current grab mode is returned.

      \param mode one of the following:
      \code
      typedef enum {
        SDL_GRAB_QUERY,   // Query the current mode
        SDL_GRAB_OFF,     // Ungrab the mouse and keyboard
        SDL_GRAB_ON       // Grab the mouse and keyboard
      } SDL_GrabMode;
      \endcode
    */
    SDL_GrabMode GrabInput(SDL_GrabMode mode);
    //@}

    // Documented in BaseSurface. Just return true since the display is always
    // in the display format. Naturally. :-)
    bool SetDisplayFormat() { return true; }
    bool SetDisplayFormatAlpha() { return true; }    
  };
}

#endif // SDLMM_DISPLAY_H
