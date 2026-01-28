/*
    This file is part of Advanced Strategic Command; http://www.asc-hq.de
    Copyright (C) 1994-2010  Martin Bickel  and  Marc Schellenberger
 
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


#ifndef ColorTransform_PlayerColorH
#define ColorTransform_PlayerColorH


#include "../playercolor.h"
#include "colorizer.h"

template<int pixelsize>
class ColorTransform_PlayerCol
{
      typedef typename PixelSize2Type<pixelsize>::PixelType PixelType;
   protected:
      ColorTransform_PlayerCol()
      {}
      ;

      PixelType transform( PixelType col)
      {
         return col;
      };

      void init( const Surface& src )
      {}
      ;

   public:
      ColorTransform_PlayerCol( const PlayerColor& player )
      {}
      ;
      ColorTransform_PlayerCol ( NullParamType npt )
      {}
      ;

      void setPlayer( int player )
      {}
      ;
};

template<>
class ColorTransform_PlayerCol<1>
{
      int shift;
      typedef PixelSize2Type<1>::PixelType PixelType;

   protected:
      ColorTransform_PlayerCol() : shift(0)
      {}
      ;

      PixelType transform( PixelType col)
      {
         if ( col >= 16 && col < 24 )
            return col + shift;
         else
            return col;
      };

      void init( const Surface& src )
   {}
      ;

   public:
      ColorTransform_PlayerCol( const PlayerColor& player )
      {
         setPlayer( player.getNum() );
      };

      ColorTransform_PlayerCol ( NullParamType npt ) : shift(0)
      {}
      ;

      void setPlayer( int player )
      {
         shift = player*8;
      };
};


template<int pixelsize>
class ColorTransform_PlayerTrueCol
{
      typedef typename PixelSize2Type<pixelsize>::PixelType PixelType;
      bool lateConversion;
      DI_Color sourceColor;

      PixelType refColor;
      int refr, refg, refb;  // this is NOT RED, GREEN, BLUE, but device dependant. Should be renamed
      int rshift, gshift, bshift;
   protected:
      ColorTransform_PlayerTrueCol() : refColor(0),refr(0),refg(0),refb(0)
      {}
      ;

      PixelType transform( PixelType col)
      {
         int r = (col >> rshift) & 0xff;
         int g = (col >> gshift) & 0xff;
         int b = (col >> bshift) & 0xff;

         if ( g==0 && b==0) {
            return ((refr * r / 256) << 16) + ((refg * r / 256) << 8) + (refb * r / 256) + (col & 0xff000000);
         } else
            if ( r==255 && g==b ) {
               return ((refr + ( 255-refr) * g / 255) << 16) + ((refg + ( 255-refg) * g / 255) << 8) + (refb + ( 255-refb) * g / 255) + (col & 0xff000000);
            } else
               return col;
      };

      void init( const Surface& src )
      {
         rshift = src.GetPixelFormat().Rshift();
         gshift = src.GetPixelFormat().Gshift();
         bshift = src.GetPixelFormat().Bshift();
         if ( lateConversion ) {
            setColor( src.GetPixelFormat().MapRGB( sourceColor ));
            lateConversion = false;
         }
      }

   public:
      ColorTransform_PlayerTrueCol( PixelType color ) : lateConversion( false )
      {
         setColor(color);
      };
      ColorTransform_PlayerTrueCol( DI_Color color ) : lateConversion( false )
      {
         setColor(color);
      };
      ColorTransform_PlayerTrueCol ( NullParamType npt ) : lateConversion( false ) , refColor(0),refr(0),refg(0),refb(0)
      {}
      ;

      void setColor( PixelType color )
      {
         refColor = color;
         refr = (color >> 16) & 0xff;
         refg = (color >> 8) & 0xff;
         refb = (color ) & 0xff;
      };

      void setColor( DI_Color color )
      {
         lateConversion = true;
         sourceColor = color;
      };
};

template<int pixelsize>
class ColorTransform_PlayerTrueColHSV
{
      typedef typename PixelSize2Type<pixelsize>::PixelType PixelType;
      int player;
      
      int rshift;
      int gshift;
      int bshift;
      int ashift;
   protected:
      ColorTransform_PlayerTrueColHSV() : player(0)
      {}
      ;

      PixelType transform( PixelType col)
      {
         if ( (col >> ashift) != Surface::transparent ) {
            int r = (col >> rshift) & 0xff;
            int g = (col >> gshift) & 0xff;
            int b = (col >> bshift) & 0xff;
   
            DI_Color d = colorSwitch.switchC( player,r,g,b);
            return (d.r << rshift) + (d.g << gshift) + (d.b << bshift)  + (col & (0xff << ashift));
         } else
            return col;

      };


   public:
      ColorTransform_PlayerTrueColHSV( int playerNum )  : player ( playerNum )
      {
      };
      ColorTransform_PlayerTrueColHSV ( NullParamType npt ) : player(0)
      {};

      void setPlayer( int playerNum )
      {
         player = playerNum;
      }
      
      void init( const Surface& src )
      {
         rshift = src.GetPixelFormat().Rshift();
         gshift = src.GetPixelFormat().Gshift();
         bshift = src.GetPixelFormat().Bshift();
         ashift = src.GetPixelFormat().Ashift();
      }

};

  template<>
  class ColorTransform_PlayerCol<4> : public ColorTransform_PlayerTrueCol<4> {
     
     public:
        ColorTransform_PlayerCol( const PlayerColor& player ) : ColorTransform_PlayerTrueCol<4>( player.getColor() )
        {
        };
   };



#endif
