
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "bargraphwidget.h"

#include <pgapplication.h>

BarGraphWidget:: BarGraphWidget (PG_Widget *parent, const PG_Rect &rect, Direction direction ) : PG_ThemeWidget( parent, rect, false ), fraction(1), dir(direction)
{
}

void BarGraphWidget::eventBlit (SDL_Surface *surface, const PG_Rect &src, const PG_Rect &dst)
{
   PG_Rect d = dst;
   if ( dir == l2r ) {
      d.w = min( max(0, int( float(dst.w) * fraction)), int(dst.w)) ;
   }
   if ( dir == r2l ) {
      int x2 = d.x + d.w;
      d.w = min( max(0, int( float(dst.w) * fraction)), int(dst.w)) ;
      d.x = x2 - d.w;
   }
   if ( dir == t2b ) {
      d.h = min( max(0, int( float(dst.h) * fraction)), int(dst.h)) ;
   }
   if ( dir == b2t ) {
      int y2 = d.y + d.h;
      d.h = min( max(0, int( float(dst.h) * fraction)), int(dst.h)) ;
      d.y = y2 - d.h;
   }

   if ( d.h <= 0 || d.w <= 0 )
      return;


/*   PG_Draw::DrawThemedSurface(
   surface,
   d,
   my_has_gradient ? &my_gradient : 0,
   my_background,
   my_backgroundMode,
   my_blendLevel );
          */

   Uint32 c = color.MapRGBA( PG_Application::GetScreen()->format, 255-GetTransparency());
   for ( Colors::iterator i = colors.begin(); i != colors.end(); ++i)
      if ( fraction < i->first ) {
      PG_Color col = i->second;
      c = col.MapRGBA( PG_Application::GetScreen()->format, 255-GetTransparency());
      }


      SDL_FillRect(PG_Application::GetScreen(), &d, c);

}


void BarGraphWidget::setFraction( float f )
{
   fraction = f;
}


