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

#ifndef SDLMM_COLOR_H
#define SDLMM_COLOR_H

namespace SDLmm {
  typedef Uint32 Color;

  /*! \brief
    A small usefull class representing an RGB color.
    
    \author Adam Gates
  */
  class ColorRGB {
  public:
    //! Default constructor which creates an empty ColorRGB (rgb variables set
    //! to zero).
    ColorRGB() : r(0), g(0), b(0) {
    }

    //! This is the copy constructor. Simply initializes the value of an ColorRGB
    //! to that of another ColorRGB.
    /*! \param color an existing ColorRGB object. */
    ColorRGB(const ColorRGB& color) : r(color.r), g(color.g), b(color.b) {
    }

    //! Constructor which initializes the class from integer values.
    /*!
      \param nr, red component
      \param ng, green component
      \param nb, blue component
    */
    ColorRGB(Uint8 nr, Uint8 ng, Uint8 nb) : r(nr), g(ng), b(nb) {
    }

    //! Set the value of an ColorRGB to that of an existing ColorRGB
    ColorRGB& operator=(const ColorRGB& color) {
      if(this != &color)
      {
        r = color.r;
        g = color.g;
        b = color.b;
      }
      return *this;
    }

    //! Compare two colors for equality.
    /*!
      \return true if coordinates are identical in both colors.
    */  
    bool operator==(const ColorRGB& color) const {
      return ((r == color.r) && (g == color.g) && (b == color.b));
    }
    
    Uint8 r;
    Uint8 g;
    Uint8 b;
  };

  /*! \brief
    A small usefull class representing an RGBA color.
    
    \author Adam Gates
  */
  class ColorRGBA : public ColorRGB {
  public:
    //! Default constructor which creates an empty ColorRGBA (rgb variables set
    //! to zero).
    ColorRGBA() : ColorRGB(), a(0) {
    }

    //! This is the copy constructor. Simply initializes the value of an ColorRGBA
    //! to that of another ColorRGBA.
    /*! \param color an existing ColorRGBA object. */
    ColorRGBA(const ColorRGBA& color) : ColorRGB(color), a(color.a) {
    }

    //! Constructor which initializes the class from integer values.
    /*!
      \param nr, red component
      \param ng, green component
      \param nb, blue component
      \param na, alpha component
    */
    ColorRGBA(Uint8 nr, Uint8 ng, Uint8 nb, Uint8 na) : ColorRGB(nr, ng, nb), a(na) {
    }

    //! Set the value of an ColorRGBA to that of an existing ColorRGBA
    ColorRGBA& operator=(const ColorRGBA& color) {
      if(this != &color)
      {
        ColorRGB::operator=(color);
        a = color.a;
      }
      return *this;
    }

    //! Compare two colors for equality.
    /*!
      \return true if coordinates are identical in both colors.
    */  
    bool operator==(const ColorRGBA& color) const {
      return (ColorRGB::operator==(color) && (a == color.a));
    }
    
    Uint8 a;
  };
}

#endif // SDLMM_COLOR_H
