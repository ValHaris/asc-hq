/*
 * SDLmm - a C++ wrapper for SDL and related libraries
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

// SDL Video Info class.

#ifndef SDLMM_VIDEOINFO_H
#define SDLMM_VIDEOINFO_H

#include "sdlmm_pixelformat.h"

namespace SDLmm {
  //! Video target information
  /*!
    This object contains read-only information about the video
    hardware. If this is called before Display::SetVideoMode(), the vfmt
    member of the returned structure will contain the pixel format of
    the "best" video mode.

    \todo Build a status function allowing for easier access to the
    flags. This can be done using a bitfield which would allow for
    checking for multiple features at once.
    \warning Before using information from VideoInfo, the video
    subsystem must already be initialized.
    \sa Display::Init(), Display::SetVideoMode()
  */    
  class DECLSPEC VideoInfo {
  public:
    //! The video info struct.
    /*!
      This struct contains the information about the display. You
      can access it directly or use the VideoInfo functions like
      VideoInfo::hw_available(). The data stored in this pointer
      can be updated (if needed) using VideoInfo::GetVideoInfo().
    */
    const SDL_VideoInfo *me;

    //! Update the pointer to the video information.
    /*!
      \return If the update fails, VideoInfo::me is set to zero and
      false is returned. true is returned for success.
      
      \note The pointer stored in this object should be updated
      automatically so there should be no need for the programmer to
      call this function manually.
    */      
    bool GetVideoInfo();
    
    //! Returns true if hardware acceleration is available.
    bool hw_available();

    //! Returns true if a window managed is available.
    bool wm_available();

    //! Returns true if hardware to hardware blits are accelerated.
    bool blit_hw();

    //! Returns true if hardware to hardware colorkey blits are accelerated.
    bool blit_hw_CC();

    //! Returns true if hardware to hardware alpha blits are accelerated.
    bool blit_hw_A();

    //! Returns true if software to hardware blits are accelerated.
    bool blit_sw();

    //! Returns true if software to hardware colorkey blits are accelerated.
    bool blit_sw_CC();

    //! Returns true if software to hardware alpha blits are accelerated.
    bool blit_sw_A();

    //! Returns true if color fills are accelerated.
    bool blit_fill();

    //! Returns the total amount of video memory in kilobytes.
    Uint32 video_mem();

    //! Returns the SDL_PixelFormat struct for the video device.
    const PixelFormat GetPixelFormat() const { return PixelFormat(me->vfmt); }
    PixelFormat GetPixelFormat() { return PixelFormat(me->vfmt); }
  };
}
#endif // SDLMM_VIDEOINFO_H
