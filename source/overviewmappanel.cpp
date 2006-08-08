/***************************************************************************
                          paradialog.cpp  -  description
                             -------------------
    begin                : Thu Feb 21 2002
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



#include "overviewmappanel.h"
#include "spfst.h"
#include "graphics/blitter.h"
#include "graphics/drawing.h"
#include "mapdisplay.h"


OverviewMapPanel::OverviewMapPanel( PG_Widget *parent, const PG_Rect &r, MapDisplayPG* mapDisplay, const ASCString& widgetName )
   : LayoutablePanel ( parent, r, widgetName, true ), mapDisplayWidget( mapDisplay), currentZoom( 1 ), locked(false)
{
   SpecialDisplayWidget* sdw = dynamic_cast<SpecialDisplayWidget*>( FindChild( "overviewmap", true ) );
   if ( sdw ) {
      sdw->display.connect( SigC::slot( *this, &OverviewMapPanel::painter ));
      sdw->sigMouseMotion.connect( SigC::slot( *this, &OverviewMapPanel::mouseMotion ));
      sdw->sigMouseButtonDown.connect( SigC::slot( *this, &OverviewMapPanel::mouseButtonDown ));
   }
   
   ovmap = sdw;
   assert( ovmap );
      
   viewChanged.connect ( SigC::slot( *this, &OverviewMapPanel::redraw ));
   OverviewMapHolder::generationComplete.connect ( SigC::slot( *this, &OverviewMapPanel::redraw ));

   lockMapdisplay.connect( SigC::slot( *this, &OverviewMapPanel::lockPanel ));
   unlockMapdisplay.connect( SigC::slot( *this, &OverviewMapPanel::unlockPanel ));
   
}


template<int pixelsize>
class ColorMerger_Invert
{
      typedef typename PixelSize2Type<pixelsize>::PixelType PixelType;
      SDLmm::Color col;
   public:

      void assign ( PixelType src, PixelType* dest ) const
      {
         *dest = (col & 0xff000000) + (0xffffff - (*dest & 0xffffff));
         // *dest = col;
      };

     
      ColorMerger_Invert( SDLmm::Color color )
      {
         col = color;
      };
};


void OverviewMapPanel::painter ( const PG_Rect &src, const ASCString& name, const PG_Rect &dst)
{
   Surface screen = Surface::Wrap( PG_Application::GetScreen() );
   if ( name == "overviewmap" && actmap && !locked ) {
      Surface s = actmap->overviewMapHolder.getOverviewMap( false );

      MegaBlitter< gamemapPixelSize, gamemapPixelSize,ColorTransform_None,ColorMerger_AlphaOverwrite,SourcePixelSelector_DirectZoom> blitter;
      blitter.setSize( s.w(), s.h(), dst.w, dst.h );

      currentZoom  = blitter.getZoomX();
      blitter.blit( s, screen, SPoint(dst.x, dst.y) );

      SPoint ul = OverviewMapImage::map2surface( mapDisplayWidget->upperLeftCorner());
      SPoint lr = OverviewMapImage::map2surface( mapDisplayWidget->lowerRightCorner());
      ul.x = int( float( ul.x) * currentZoom );
      ul.y = int( float( ul.y) * currentZoom );
      lr.x = int( float( lr.x) * currentZoom );
      lr.y = int( float( lr.y) * currentZoom );

      if ( ul.x < 0 )
         ul.x = 0;
      if ( ul.y < 0 )
         ul.y = 0;
      if ( lr.x >= src.Width() )
         lr.x = src.Width() -1;
      if ( lr.y >= src.Height() )
         lr.y = src.Height() -1;

      rectangle<4>(screen, SPoint(dst.x + ul.x, dst.y + ul.y), lr.x-ul.x, lr.y-ul.y, ColorMerger_Invert<4>(0xff), ColorMerger_Invert<4>(0xff) );
   }
}


void OverviewMapPanel::lockPanel()
{
   locked = true;
   Update();
}

void OverviewMapPanel::unlockPanel()
{
   locked = false;
   Update();
}

      
bool OverviewMapPanel::mouseClick ( SPoint pos )
{
   SPoint unscaledPos = SPoint(int( float(pos.x) / currentZoom), int(float(pos.y) / currentZoom ));
   MapCoordinate mc = OverviewMapImage::surface2map( unscaledPos );
   if ( !(mc.valid() && mc.x < actmap->xsize && mc.y < actmap->ysize ))
      return false;

   mapDisplayWidget->centerOnField( mc );
   return true;
}


bool OverviewMapPanel::mouseButtonDown ( const SDL_MouseButtonEvent *button)
{
   if ( ovmap->IsMouseInside() )
      if ( button->type == SDL_MOUSEBUTTONDOWN && button->button == 1 ) {
         PG_Point p = ovmap->ScreenToClient( button->x, button->y );
         return mouseClick( SPoint( p.x, p.y ));
      }

   return false;
}

bool OverviewMapPanel::mouseMotion  (  const SDL_MouseMotionEvent *motion)
{
   if ( ovmap->IsMouseInside() )
      if ( motion->type == SDL_MOUSEMOTION && (motion->state & 1 ) ) {
         PG_Point p = ovmap->ScreenToClient( motion->x, motion->y );
         return mouseClick( SPoint( p.x, p.y ));
      }

   return false;
}

