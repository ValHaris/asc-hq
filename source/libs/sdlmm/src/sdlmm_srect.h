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

/*!
  \struct SDL_Rect
  \brief A SDL_Rect defines a rectangular area of pixels. It is documented in
         detail in the SDL documentation.
  \author Sam Lantinga <slouken@libsdl.org>
*/

#ifndef SDLMM_SRECT_H
#define SDLMM_SRECT_H
#include  "sdlmm_spoint.h"

namespace SDLmm {
  /*! \brief
    This enhanced version of \a SDL_Rect includes a number of handy
    short-hand constructors. Since the class is derived from the
    SDL_Rect struct, it can be used in any context where an \a SDL_Rect
    is valid.
    
    \todo Extend the class with useful utility functions in regards to
    rectangles like intersections.
    \author David Hedbor <david@hedbor.org>
  */
  class DECLSPEC SRect : public SDL_Rect {
  public:
    //! Default constructor which creates an empty SRect (xywh variables set
    //! to sero).
    SRect();

    //! This is the copy constructor. Simply initializes the value of an SRect
    //! to that of another SRect.
    /*! \param rect an existing SRect object. */
    SRect(const SRect& rect);

    //! Constructor which initializes the class from an SDL_Rect struct.
    /*! \param rect an existing SDL_Rect struct. */
    SRect(const SDL_Rect& rect);

    //! Constructor which initializes the class from an SPoint object.
    /*!
      The x and y values will be initialized with the x/y values of the SPoint.
      h and w will be set to zero.
      \param point reference to an existing SPoint object. */
    SRect(const SPoint& point);

    //! Construct an object using the given upper left and bottom right 
    //! corners.
    /*!
      \param upper_left_point SPoint for the upper right corner of the rectangle
      \param bottom_right_point SPoint for the bottom right corner of the rectangle
    */
    SRect(const SPoint &upper_left_point,
          const SPoint &bottom_right_point);

    //! Constructor which initializes the class from an SPoint object
    //! and integer values.
    /*!
      The x and y values will be initialized with the x/y values of
      the SPoint and the dimensions from nw and nh.
      \param nw, nh the width and height of the rectangle
      \param point reference to an existing SPoint object for the upper right
      corner.
    */
    SRect(const SPoint &point, Uint16 nw, Uint16 nh);
    
    //! Constructor which initializes the class from integer values.
    /*!
      \param nx, ny position of the upper left corner of the rectangle
      \param nw, nh the width and height of the rectangle
    */
    SRect(Sint16 nx, Sint16 ny, Uint16 nw, Uint16 nh);

    //! Constructor which initializes the width and height from integer values.
    /*!
      The x/y coordinates for the upper left corner will be set to zero.
      \param nw, nh the width and height of the rectangle
    */
    SRect(Uint16 nw, Uint16 nh);

    //! Set the value of an SRect to that of an existing SDL_Rect or SRect
    SRect& operator=(const SDL_Rect& rect) {
      if(this == &rect) return *this;
      x = rect.x; y = rect.y;
      w = rect.w; h = rect.h;
      return *this;
    }

    //! Compare two rectangles for equality.
    /*!
      \return true if x / y coordinates and h / w are identical in both rectangles.
    */  
    bool operator==(const SDL_Rect& rect) const {
      return ((x == rect.x) && (y == rect.y) &&
              (w == rect.w) && (h == rect.h));
    }

    //! Move the position of the rectangle.
    /*!
      \param point difference to move by.
    */
    void Move(const SPoint& point) {
        x += point.x; y += point.y;
    }
    
    //! Is the point in the rectangle?
    bool Contains(const SPoint& point) const {
        return (x <= point.x) && (y <= point.y) &&
               ((x+w) > point.x) && ((y+h) > point.y);
    }
    
    //! Get the coordinates for the upper left corner of the SRect
    /*! \return SPoint object */
    SPoint GetUpperLeft() const { return SPoint(x, y); }    
    //! Get the coordinates for the upper right corner of the SRect
    /*! \return SPoint object */
    SPoint GetUpperRight() const { return SPoint(x+w, y); }    
    //! Get the coordinates for the bottom left corner of the SRect
    /*! \return SPoint object */
    SPoint GetBottomLeft() const { return SPoint(x, y+h); }    
    //! Get the coordinates for the bottom right corner of the SRect
    /*! \return SPoint object */
    SPoint GetBottomRight() const { return SPoint(x+w, y+h); }    
    
  };
  
  //! Get intersection rectangle of two rectangles
  /*! The intersection rectangle is the largest rectangle that is
      contained by both r1 and r2. */
  SRect Intersect(const SRect& r1, const SRect& r2);

  //! Get union rectangle of two rectangles
  /*! The union rectangle is the smallest rectangle that is
      contains both r1 and r2. */
  SRect Union(const SRect& r1, const SRect& r2);
}

#endif // SDLMM_SRECT_H
