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

#ifndef SDLMM_SPOINT_H
#define SDLMM_SPOINT_H

namespace SDLmm {
  /*! \brief
    A small usefull class representing a point in 2 dimensional space.
    
    \author Adam Gates
  */
  class DECLSPEC SPoint {
  public:
    //! Default constructor which creates an empty SPoint (xy variables set
    //! to zero).
    SPoint() : x(0), y(0) {
    }

    //! This is the copy constructor. Simply initializes the value of an SPoint
    //! to that of another SPoint.
    /*! \param point an existing SPoint object. */
    SPoint(const SPoint& point) : x(point.x), y(point.y) {
    }

    //! Constructor which initializes the class from integer values.
    /*!
      \param nx, horizontal position
      \param ny, vertical position
    */
    SPoint(Sint16 nx, Sint16 ny) : x(nx), y(ny) {  }

    //! Set the value of an SPoint to that of an existing SPoint
    SPoint& operator=(const SPoint& point) {
      if(this != &point)
      {
        x = point.x;
        y = point.y;
      }
      return *this;
    }

    //! Compare two points for equality.
    /*!
      \return true if coordinates are identical in both points.
    */  
    bool operator==(const SPoint& point) const {
      return ((x == point.x) && (y == point.y));
    }

    /*!
      \return true if both point.x and point.y is larger than
      x and y in this point.
    */
    bool operator<(const SPoint &point) const {
      return x < point.x && y < point.y;
    }

    /*!
      \return true if both point.x and point.y is larger than or equal to
      x and y in this point.
    */
    bool operator<=(const SPoint &point) const {
      return x <= point.x && y <= point.y;
    }
    
  /*!
    \return true if both point.x and point.y is smaller than
      x and y in this point.
    */
    bool operator>(const SPoint &point) const {
      return !operator<(point);
    }
  /*!
    \return true if both point.x and point.y is smaller than or equal to
      x and y in this point.
    */
    bool operator>=(const SPoint &point) const {
      return !operator<=(point);
    }

    SPoint& operator+=(const SPoint& point) {
      x += point.x;
	  y += point.y;
	  return *this;
    }

    SPoint& operator-=(const SPoint& point) {
      x -= point.x;
	  y -= point.y;
	  return *this;
    }

    SPoint operator+(const SPoint& point) const {
      return SPoint(x + point.x, y + point.y);
    }

    SPoint operator-(const SPoint& point) const {
      return SPoint(x - point.x, y - point.y);
    }

    SPoint& operator*=(Sint16 scalar) {
      x *= scalar;
	  y *= scalar;
	  return *this;
    }

    SPoint& operator/=(Sint16 scalar) {
      x /= scalar;
	  y /= scalar;
	  return *this;
    }

    SPoint operator*(Sint16 scalar) const {
      return SPoint(x * scalar, y * scalar);
    }

    SPoint operator/(Sint16 scalar) const {
      return SPoint(x / scalar, y / scalar);
    }

    Sint16 x;
    Sint16 y;
  };
}

#endif // SDLMM_SPOINT_H
