/*! \file mapdisplay.cpp
    \brief everything for displaying the map on the screen
*/


/***************************************************************************
                          mapdisplay.cpp  -  description
                             -------------------
    begin                : Wed Jan 24 2001
    copyright            : (C) 2001 by Martin Bickel
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


#include <cmath>

#include "global.h"

#include "mapdisplay.h"
#include "mapdisplay2.h"
#include "vehicletype.h"
#include "buildingtype.h"
#include "spfst.h"
#include "typen.h"
#include "loaders.h"
#include "gameoptions.h"
#include "dialog.h"
#include "stack.h"
#include "loadbi3.h"
#include "mapalgorithms.h"
#include "loadpcx.h"
#include "graphicset.h"
#include "graphics/blitter.h"

#ifdef sgmain
 #include "controls.h"
 #include "gui.h"
#endif

#include "iconrepository.h"


MapRenderer::Icons MapRenderer::icons;


void MapRenderer::readData()
{
   if ( !icons.mapBackground.valid() ) {
      icons.mapBackground = IconRepository::getIcon("mapbkgr.raw");
      icons.notVisible    = IconRepository::getIcon("hexinvis.raw");
      icons.markField     = IconRepository::getIcon("markedfield.pcx");
      icons.markField.detectColorKey();
      icons.markFieldDark = IconRepository::getIcon("markedfielddark.png");
   }
}


/*
  layer:
    0: terrain
    1: below everything objects
    2: deep submerged units and building
    3: deep submerged objects
    4: submerged units and buildings
    5: submerged objects
    6: floating  units and buildings
    7: floating  objects
    8: ground    units and buildings
    9: ground    objects
   10: low flying units and buildings
   11: low flying objects
   12: flying units and buildings
   13: flying objects
   14: high flying units and buildings
   15: high flying objects
   16: orbiting units
   17: orbiting objects
   18  view obstructions
*/

int MapRenderer::bitmappedHeight2pass( int height )
{
   return log2(height) * 2 + 2;
}


void MapRenderer::paintSingleField( Surface& surf, int playerView, pfield fld, int layer, const SPoint& pos, const MapCoordinate& mc )
{

   int binaryUnitHeight = 0;
   if ( layer > 1 ) 
      if ( !(layer & 1 ))
         binaryUnitHeight = 1 << (( layer-2)/2);
   

   VisibilityStates visibility = fieldVisibility ( fld, playerView );

   if ( layer == 0 && visibility >= visible_ago )
      fld->typ->paint ( surf, pos );


   if ( visibility > visible_ago ) {

      /* display buildings */
      if ( fld->building  &&  (fld->building->typ->buildingheight & binaryUnitHeight) )
         if ((visibility == visible_all) || (fld->building->typ->buildingheight >= chschwimmend) || ( fld->building->color == playerView*8 ))
            fld->building->paintSingleField( surf, pos, fld->building->getLocalCoordinate( mc ));


      /* display units */
      if ( fld->vehicle  &&  (fld->vehicle->height == binaryUnitHeight))
         if ( ( fld->vehicle->color == playerView * 8 ) || (visibility == visible_all) || ((fld->vehicle->height >= chschwimmend) && (fld->vehicle->height <= chhochfliegend)))
            fld->vehicle->paint( surf, pos );

   }

   // display objects
   if ( layer & 1 )
   for ( tfield::ObjectContainer::iterator o = fld->objects.begin(); o != fld->objects.end(); o++ ) {
      int h = o->typ->imageHeight;
      if ( visibility > visible_ago || (o->typ->visibleago && visibility >= visible_ago ))
         if (  h >= ((layer-1)/2)*30 && h < (layer-1)/2*30+30 )
            o->display ( surf, pos, fld->getweather() );
   }




   if ( visibility > visible_ago ) {
      /* display mines */
      /*
        if ( visibility == visible_all )
             if ( !fld->mines.empty() && layer == 4 ) {
                if ( fld->mines.begin()->type != cmmooredmine )
                   putspriteimage( r, yp, getmineadress(fld->mines.begin()->type) );
                else
                   putpicturemix ( r, yp, getmineadress(fld->mines.begin()->type, 1 ) ,  0, (char*) colormixbuf );
                #ifdef karteneditor
                bar ( r + 5 , yp +5, r + 15 , yp +10, 20 + fld->mineowner() * 8 );
                #endif
             }
       */


      /* display marked fields */

      if ( layer == 18 ) {
         if ( fld->a.temp && tempsvisible )
            surf.Blit( icons.markField, pos );
         else
            if ( fld->a.temp2 && tempsvisible )
               surf.Blit( icons.markFieldDark, pos );
      }


   } else {
      if (visibility == visible_ago) {
         if ( fld->building  &&  (fld->building->typ->buildingheight & binaryUnitHeight) )
            if ((visibility == visible_all) || (fld->building->typ->buildingheight >= chschwimmend) || ( fld->building->color == playerView*8 ))
               fld->building->paintSingleField( surf, pos, fld->building->getLocalCoordinate( mc ));

      }
   }

   /* display resources */
   /*
   if ( layer == 8 && b >= visible_ago) {
      #ifndef karteneditor
      if ( fld->resourceview && (fld->resourceview->visible & ( 1 << playerview) ) ){
         if ( showresources == 1 ) {
            showtext2( strrr ( fld->resourceview->materialvisible[playerview] ) , r + 10 , yp +10 );
            showtext2( strrr ( fld->resourceview->fuelvisible[playerview] )     , r + 10 , yp +20 );
         } else
            if ( showresources == 2 ) {
               if ( fld->resourceview->materialvisible[playerview] )
                  bar ( r + 10 , yp +2, r + 10 + fld->resourceview->materialvisible[playerview] / 10, yp +6, 23 );

               if ( fld->resourceview->fuelvisible[playerview] )
                  bar ( r + 10 , yp + 14 -2, r + 10 + fld->resourceview->fuelvisible[playerview] / 10, yp +14 +2 , 191 );
            }
      }
      #else
      if ( showresources == 1 ) {
         showtext2( strrr ( fld->material ) , r + 10 , yp );
         showtext2( strrr ( fld->fuel )     , r + 10 , yp + 10 );
      }
      else if ( showresources == 2 ) {
         if ( fld->material )
            bar ( r + 10 , yp -2, r + 10 + fld->material / 10, yp +2, 23 );
         if ( fld->fuel )
            bar ( r + 10 , yp +10 -2, r + 10 + fld->fuel / 10, yp +10 +2 , 191 );
      }
     #endif
   }
   */




   // display view obstructions
   if ( layer == 18 ) {
      if ( visibility == visible_ago) {
         MegaBlitter<1,colorDepth,ColorTransform_None,ColorMerger_AlphaShadow> blitter;
         // PG_Point pnt = ClientToScreen( 0,0 );
         blitter.blit( icons.notVisible, surf, pos);
         /*
                         // putspriteimage( r + unitrightshift , yp + unitdownshift , view.va8);
                         putshadow( r, yp, icons.view.nv8, &xlattables.a.dark2 );
                         if ( fld->a.temp && tempsvisible )
                            putspriteimage(  r, yp, cursor.markfield);
                         else
                            if ( fld->a.temp2 && tempsvisible )
                               putspriteimage(  r, yp, xlatpict ( &xlattables.a.dark2 , cursor.markfield));
          
         */
      } else
         if ( visibility == visible_not) {
            surf.Blit( icons.notVisible, pos );
            /*
                            if ( ( fld->a.temp || fld->a.temp2 ) && tempsvisible )
                                  putspriteimage(  r, yp, cursor.markfield);
            */

         }

   }

}



void MapRenderer::paintTerrain( Surface& surf, tmap* actmap, int playerView, const ViewPort& viewPort, const MapCoordinate& offset )
{
   GraphicSetManager::Instance().setActive ( actmap->graphicset );

   for (int pass = 0; pass <= 18 ;pass++ ) {
      for (int y= viewPort.y1; y < viewPort.y2; ++y )
         for ( int x=viewPort.x1; x < viewPort.x2; ++x ) {
            SPoint pos = getFieldPos(x,y);
            MapCoordinate mc = MapCoordinate( offset.x + x, offset.y + y );
            pfield fld = actmap->getField ( mc );
            if ( fld )
               paintSingleField( surf, playerView, fld, pass, pos, mc );
            else
               if ( pass == 0 )
                  surf.Blit( icons.mapBackground, pos );

         }
      additionalItemDisplayHook( surf, pass );
   }
}






MapDisplayPG* theMapDisplay = NULL;


void benchMapDisplay()
{
   int t = ticker;
   for ( int i = 0; i < 20; ++i )
      repaintMap();

   int t2 = ticker;
   
   for ( int i = 0; i< 20; ++i)
      theMapDisplay->Redraw();
   
   int t3 = ticker;

   ASCString s;
   s.format("update map: %d \nupdate widget: %d \n%f fps", t2-t,t3-t2, 20.0 / float(t3-t) * 100 );
   displaymessage(s, 1 );
}



MapDisplayPG::MapDisplayPG ( PG_Widget *parent, const PG_Rect r )
      : PG_Widget ( parent, r, false ) ,
      zoom( 0.75 ),
      surface(NULL),
      offset(0,0),
      dirty(Map),
      additionalUnit(NULL)
{
   readData();
   setNewZoom( zoom );

   repaintMap.connect( SigC::slot( *this, &MapDisplayPG::updateWidget ));


   SDL_Surface* ws = GetWidgetSurface ();
   if ( ws ) {
      Surface s = Surface::Wrap( ws );
      s.assignDefaultPalette();
   }
   
   MapRenderer::additionalItemDisplayHook.connect( SigC::slot( *this, &MapDisplayPG::displayAddons ));
   
   theMapDisplay = this;
}


MapDisplayPG::Icons MapDisplayPG::icons;


void MapDisplayPG::readData()
{
   if ( !icons.cursor.valid() ) {
      icons.cursor = IconRepository::getIcon( "curshex.raw" );
      icons.fieldShape = IconRepository::getIcon("hexinvis.raw");
   }
}


void MapDisplayPG::setNewZoom( float zoom )
{

   this->zoom = zoom;

   field.numx = int( ceil(float(Width())  / zoom / fielddistx) );
   field.numy = int( ceil(float(Height()) / zoom / fielddisty) );

   field.viewPort.x1 = -1;
   field.viewPort.y1 = -1;
   field.viewPort.x2 = field.numx + 1;
   field.viewPort.y2 = field.numy + 1;

   if ( field.numy & 1 )
      field.numy += 1;

   delete surface;
   surface = new Surface( Surface::createSurface ( field.numx * fielddistx + 2 * surfaceBorder, (field.numy - 1) * fielddisty + fieldysize +  2 * surfaceBorder, colorDepth*8 ));

   dirty = Map;
}


void MapDisplayPG::fillSurface( int playerView )
{
   checkViewPosition();
   paintTerrain( *surface, actmap, playerView, field.viewPort, offset );
   dirty = Curs;
}


void MapDisplayPG::checkViewPosition()
{
   if ( offset.x + field.numx >= actmap->xsize +1 )
      offset.x = max(0,actmap->xsize - field.numx +1 );

   if ( offset.y + field.numy >= actmap->ysize +2)
      offset.y = max(0,actmap->ysize - field.numy +2);

   if ( offset.y & 1 )
      offset.y -= 1;
}




template<int pixelSize>
class PixSel : public SourcePixelSelector_CacheZoom<pixelSize, SourcePixelSelector_DirectRectangle<pixelSize> >
{}
;


void MapDisplayPG::updateMap(bool force )
{
   if ( dirty > Curs || force )
      fillSurface( actmap->playerView );

}

void MapDisplayPG::updateWidget()
{
   updateMap(true);
   Update(true);
}


void MapDisplayPG::blitInternalSurface( SDL_Surface* dest, const SPoint& pnt )
{
   MegaBlitter<colorDepth,colorDepth,ColorTransform_None,ColorMerger_PlainOverwrite,PixSel> blitter;
   blitter.setZoom( zoom );
   blitter.initSource( *surface );
   blitter.setRectangle( SPoint( getFieldPosX(0,0), getFieldPosY(0,0)), int(float(Width()) / zoom), int(float(Height()) / zoom));
   Surface s = Surface::Wrap( dest );
   blitter.blit( *surface, s, SPoint(pnt.x, pnt.y ));
}


void MapDisplayPG::eventDraw ( SDL_Surface* srf, const PG_Rect& rect)
{
   if ( dirty > Nothing )
      updateMap();

   blitInternalSurface( srf, SPoint(0,0));
}

 class TargetPixelSelector_Rect {
        PG_Rect rect; 
        int x2,y2;
        int w,h;
        SPoint pos;
     protected:
        int skipTarget( int x, int y ) 
        { 
           x += pos.x;
           y += pos.y;
           if ( x >= rect.x && y >= rect.y && x < x2 && y < y2 )
              return 0; 
           else {
              if ( y < rect.y )
                 return 1;
              else
                 if ( x < rect.x )
                    return rect.x - x;
                 else   
                    if (y >= y2 )
                       return -1;
                    else   
                       return 1;     
           }      
        };
        void init( const Surface& srv, const SPoint& position ) 
        {
           w = srv.w();
           h = srv.h();
           pos = position;
        };
        
     public:
        void setTargetRect( const PG_Rect& r  ) 
        { 
           rect = r; 
           x2 = r.x + r.w; 
           y2 = r.y + r.h;
        };
        TargetPixelSelector_Rect ( NullParamType npt = nullParam ) :w(0xffffff),h(0xffffff) {};   
  };


void MapDisplayPG::displayCursor()
{
   if ( !actmap )
      return;
      
   int x = actmap->player[actmap->playerView].cursorPos.x - offset.x;
   int y = actmap->player[actmap->playerView].cursorPos.y - offset.y;
   if( x >= field.viewPort.x1 && x < field.viewPort.x2 && y >= field.viewPort.y1 && y < field.viewPort.y2 ) {
      // surface->Blit( icons.cursor, getFieldPos(x,y));
      MegaBlitter<1,colorDepth,ColorTransform_None,ColorMerger_AlphaOverwrite,SourcePixelSelector_Zoom,TargetPixelSelector_Valid> blitter;
      blitter.setZoom( zoom );

      Surface s = Surface::Wrap( PG_Application::GetScreen() );
      // PG_Point pnt = ClientToScreen( 0,0 );
      blitter.blit( icons.cursor, s, widget2screen ( internal2widget( mapPos2internalPos( MapCoordinate(x,y)))) );
   }
}


void MapDisplayPG::eventBlit(SDL_Surface* srf, const PG_Rect& src, const PG_Rect& dst)
{
   if ( !GetWidgetSurface ()) {
      if ( dirty > Nothing )
         updateMap();

      PG_Point pnt = ClientToScreen( 0,0 );
      blitInternalSurface( PG_Application::GetScreen(), SPoint(pnt.x,pnt.y));
   } else {
      PG_Widget::eventBlit(srf,src,dst);
   }

   if ( !cursor.invisible )
      displayCursor();


}


SPoint MapDisplayPG::mapPos2internalPos ( const MapCoordinate& pos )
{
   return getFieldPos( pos.x, pos.y );
}

SPoint MapDisplayPG::internal2widget( const SPoint& pos )
{
   return SPoint( int(float(pos.x - surfaceBorder) * zoom), int(float(pos.y - surfaceBorder) * zoom));
}

SPoint MapDisplayPG::widget2screen( const SPoint& pos )
{
   PG_Point p = ClientToScreen ( pos.x, pos.y );
   return SPoint ( p.x, p.y );
}


MapCoordinate MapDisplayPG::screenPos2mapPos( const SPoint& pos )
{
   PG_Point pnt = ScreenToClient( pos.x, pos.y );
   if ( pnt.x >= 0 && pnt.y >= 0 && pnt.x < Width() && pnt.y < Height() )
      return widgetPos2mapPos ( SPoint( pnt.x, pnt.y ));
   else
      return MapCoordinate();
}

MapCoordinate MapDisplayPG::widgetPos2mapPos( const SPoint& pos )
{
   int x = int( float(pos.x) / zoom ) + surfaceBorder;
   int y = int( float(pos.y) / zoom ) + surfaceBorder;

   for (int yy= field.viewPort.y1; yy < field.viewPort.y2; ++yy )
      for ( int xx=field.viewPort.x1; xx < field.viewPort.x2; ++xx ) {
         int x1 = getFieldPosX(xx,yy);
         int y1 = getFieldPosY(xx,yy);
         if ( x >= x1 && x < x1+ fieldsizex && y >= y1 && y < y1+fieldsizey )
            if ( icons.fieldShape.GetPixel(x-x1,y-y1) != 255 )
               return MapCoordinate(xx+offset.x,yy+offset.y);
      }

   return MapCoordinate();
}


bool MapDisplayPG::eventMouseButtonDown (const SDL_MouseButtonEvent *button)
{
   MapCoordinate mc = screenPos2mapPos( SPoint(button->x, button->y));
   if ( !(mc.valid() && mc.x < actmap->xsize && mc.y < actmap->ysize ))
      return false;

   if ( button->type == SDL_MOUSEBUTTONDOWN && button->button == CGameOptions::Instance()->mouse.fieldmarkbutton ) {
      bool changed = actmap->player[actmap->playerView].cursorPos != mc;
      actmap->player[actmap->playerView].cursorPos = mc;
      cursor.invisible = 0;
      dirty = Curs;

      updateFieldInfo();

      bool exit = mouseButtonOnField( mc, button, changed );
      Update();

      return true;
   }

   if ( button->type == SDL_MOUSEBUTTONDOWN && button->button == CGameOptions::Instance()->mouse.centerbutton ) {
      int newx = mc.x - field.numx / 2;
      int newy = mc.y - field.numy / 2;

      if ( newx < 0 )
         newx = 0;
      if ( newy < 0 )
         newy = 0;
      if ( newx > actmap->xsize - field.numx +1 )
         newx = actmap->xsize - field.numx + 1;
      if ( newy > actmap->ysize - field.numy +2 )
         newy = actmap->ysize - field.numy +2;

      if ( newy & 1 )
         newy--;

      if ( newx != offset.x  || newy != offset.y ) {
         offset.x = newx;
         offset.y = newy;
         dirty = Map;
         Redraw();
      }
      return true;
   }

   return false;
}

bool MapDisplayPG::eventMouseButtonUp (const SDL_MouseButtonEvent *button)
{
   return false;
}


MapDisplayPG::CursorHiding::CursorHiding()
{
   if ( theMapDisplay )
      ++theMapDisplay->cursor.invisible;
}

MapDisplayPG::CursorHiding::~CursorHiding()
{
   if ( theMapDisplay ) {
      --theMapDisplay->cursor.invisible;
      if ( !theMapDisplay->cursor.invisible )
         theMapDisplay->displayCursor();
   }
}


bool MapDisplayPG::fieldInView(const MapCoordinate& mc )
{
   if ( mc.x < offset.x || mc.y < offset.y || mc.x > offset.x + field.numx || mc.y > offset.y +  field.numy )
      return false;
   else
      return true;
}



Surface MapDisplayPG::createMovementBufferSurface()
{
   Surface s = Surface::createSurface( 2*surfaceBorder + effectiveMovementSurfaceWidth, 2*surfaceBorder + effectiveMovementSurfaceHeight, 8*colorDepth, 0xffffffff ) ;
   s.SetColorKey( SDL_SRCCOLORKEY, 0xffffff);
   return s;
}


void MapDisplayPG::initMovementStructure()
{
   if ( !movementMask[0].mask.valid() )
      for ( int dir = 0; dir < sidenum; ++dir ) {
         movementMask[dir].mask = createMovementBufferSurface();
         MapCoordinate start;
         if ( dir >= 2 && dir <= 4 )
            start = MapCoordinate( 1, 2 );
         else
            start = MapCoordinate( 1, 4 );

         MapCoordinate dest = getNeighbouringFieldCoordinate( start, dir );

         for ( int i = 0; i < sidenum; ++i)
            movementMask[dir].mask.Blit( icons.fieldShape, getFieldPos2( getNeighbouringFieldCoordinate( start, i )));
         for ( int i = 0; i < sidenum; ++i)
            movementMask[dir].mask.Blit( icons.fieldShape, getFieldPos2( getNeighbouringFieldCoordinate( dest, i )));

         SPoint pix = getFieldPos2( start );
         movementMask[dir].startFieldPos = pix;

         pix.x += fieldsizex/2;
         pix.y += fieldsizey/2;
         movementMask[dir].mask.SetColorKey( SDL_SRCCOLORKEY, movementMask[dir].mask.GetPixel( pix ) & ~movementMask[dir].mask.GetPixelFormat().Amask());
      }
}


void MapDisplayPG::displayMovementStep( Movement& movement, int percentage  )
{
   for (int pass = 0; pass <= 18 ;pass++ )
      for ( int i = 0; i < touchedFieldNum; ++i ) {
         SPoint pos = movement.touchedFields[i].surfPos;
         const MapCoordinate& mc = movement.touchedFields[i].mapPos;

         pfield fld = movement.actmap->getField ( mc );
         if ( fld ) {
            paintSingleField( *movement.surf, movement.playerView, fld, pass, pos, mc );

            if ( pass >= 2 && pass < 18 )
              if ( !(pass & 1 ))
                 if ( movement.veh->height & (1 << (( pass-2)/2))) {
                    SPoint pos;
                    pos.x = movement.from.x + (movement.to.x - movement.from.x) * percentage/100;
                    pos.y = movement.from.y + (movement.to.y - movement.from.y) * percentage/100;
                    movement.veh->paint( *movement.surf, pos );
                 }
         }

      }
   
   
   MegaBlitter<colorDepth,colorDepth,ColorTransform_None,ColorMerger_AlphaOverwrite> alphaBlitter;
   alphaBlitter.blit( *movement.mask, *movement.surf, SPoint(0,0));
   // movement.surf->Blit( *movement.mask );
   
   
   MegaBlitter<colorDepth,colorDepth,ColorTransform_None,ColorMerger_AlphaOverwrite,PixSel,TargetPixelSelector_Rect> blitter;
   blitter.setZoom( zoom );
   blitter.initSource( *movement.surf );
   blitter.setRectangle( SPoint( getFieldPosX(0,0), getFieldPosY(0,0)), effectiveMovementSurfaceWidth, effectiveMovementSurfaceHeight );
   blitter.setTargetRect ( *this );
   Surface s = Surface::Wrap( PG_Application::GetScreen() );
   blitter.blit( *movement.surf, s, movement.screenPos );
   
   
   SDL_UpdateRect(PG_Application::GetScreen(), movement.screenUpdatePos.x, movement.screenUpdatePos.y, movement.screenWidth, movement.screenHeight); 
   
   
   /*
   SDL_BlitSurface( movement.surf->getBaseSurface(), NULL, PG_Application::GetScreen(), NULL );
   SDL_UpdateRect(PG_Application::GetScreen(), 0, 0,PG_Application::GetScreen()->w, PG_Application::GetScreen()->h ); 
   */
   
}



bool ccompare( const MapDisplayPG::TouchedField& a, const MapDisplayPG::TouchedField& b )
{
   return a.realMap.y < b.realMap.y || (a.realMap.y == b.realMap.y && a.realMap.x < b.realMap.x);
}   

bool MapDisplayPG::TouchedField::operator==( const MapDisplayPG::TouchedField& b )
{
  return b.realMap == realMap && b.tempMap == tempMap;
}


void MapDisplayPG::displayUnitMovement( pmap actmap, Vehicle* veh, const MapCoordinate3D& from, const MapCoordinate3D& to )
{
   if ( !fieldInView( from ) && !fieldInView( to ))
      return;

   int startTime = ticker;
   int duration = CGameOptions::Instance()->movespeed;
   int endTime = startTime + duration;
   
   
   initMovementStructure();

   int dir = getdirection( from, to );
   
   if ( from.x == to.x && from.y == to.y )
      dir = 0;  // changing height vertically
   else   
      veh->direction = dir;
   
   MapCoordinate tempStart;
   if ( dir >= 2 && dir <= 4 ) {
      tempStart = MapCoordinate( 1, 2 );
   } else {
      tempStart = MapCoordinate( 1, 4 );
   }   

   typedef vector< TouchedField > TouchedFields;
   TouchedFields touchedFields;
   
   touchedFields.push_back ( TouchedField( from, tempStart ));
   for ( int i = 0; i < sidenum; ++i ) 
      touchedFields.push_back ( TouchedField( getNeighbouringFieldCoordinate(from, i), getNeighbouringFieldCoordinate(tempStart, i) ));
      
   MapCoordinate tempEnd = getNeighbouringFieldCoordinate( tempStart, dir );
   touchedFields.push_back ( TouchedField( to, tempEnd ));
   for ( int i = 0; i < sidenum; ++i ) 
      touchedFields.push_back ( TouchedField( getNeighbouringFieldCoordinate(to, i), getNeighbouringFieldCoordinate(tempEnd, i) ));
   

   // now we have all fields that are touched during movement in a list
   // let's sort it

   sort(   touchedFields.begin(), touchedFields.end(), ccompare );
   /*
   TouchedFields::iterator u = unique( touchedFields.begin(), touchedFields.end() );
   touchedFields.erase( u, touchedFields.end() );   
   */
        
   
   Movement movement;
   int i = 0;
   for ( TouchedFields::iterator j = touchedFields.begin(); j != touchedFields.end(); ++j )  {
      if ( i == 0 || movement.touchedFields[i-1].mapPos != j->realMap ) {
         movement.touchedFields[i].mapPos = j->realMap;
         movement.touchedFields[i].surfPos = getFieldPos2( j->tempMap );
         ++i;
      }   
   }   
           
   movement.veh = veh;
   movement.from = getFieldPos2( tempStart );
   movement.to = getFieldPos2( tempEnd );
   
   movement.screenPos = widget2screen( internal2widget( mapPos2internalPos( MapCoordinate(0,0)) - mapPos2internalPos( tempStart ) + mapPos2internalPos( from - offset ) ));
   movement.screenUpdatePos = movement.screenPos;
   
   SPoint lowerRight = SPoint( int( float(effectiveMovementSurfaceWidth) * zoom) + movement.screenPos.x, int( float(effectiveMovementSurfaceHeight) * zoom)+ movement.screenPos.y);
   
   if ( movement.screenUpdatePos.x < 0 )
      movement.screenUpdatePos.x = 0;
   if ( movement.screenUpdatePos.y < 0 )
      movement.screenUpdatePos.y = 0;   
   if ( lowerRight.x >= PG_Application::GetScreen()->w )
      lowerRight.x = PG_Application::GetScreen()->w - 1;
   if ( lowerRight.y >= PG_Application::GetScreen()->h )  
      lowerRight.y = PG_Application::GetScreen()->h - 1;
      
   movement.screenWidth = lowerRight.x - movement.screenUpdatePos.x + 1;
   movement.screenHeight= lowerRight.y - movement.screenUpdatePos.y + 1;
   
     
   movement.actmap = actmap;
   
   static Surface surface;
   if ( !surface.valid() )
      surface = createMovementBufferSurface();
   else
      surface.Fill(0xffffffff );   
         
   movement.surf = &surface;
   movement.mask = &movementMask[dir].mask;
   
   movement.playerView = actmap->playerView;
   
   int loopCounter = 0;
   int loopStartTicker = ticker;
   while ( ticker < endTime ) {
      displayMovementStep( movement, (ticker - startTime) * 100 / duration );
      ++loopCounter;
   }
   cout << (float(loopCounter) / float(ticker - loopStartTicker) * 100) << " / " << (float(loopCounter) / float(ticker - startTime) * 100) << " fps \n";
}

void MapDisplayPG::displayAddons( Surface& surf, int pass)
{
   if( additionalUnit ) 
      if ( pass == bitmappedHeight2pass( additionalUnit->height ) )  {
         SPoint p = mapPos2internalPos( additionalUnit->getPosition() - offset );
         if ( p.x >= 0 && p.y >= 0 && p.x + fieldsizex < surf.w() && p.y + fieldsizey < surf.h() )
            additionalUnit->paint( surf, p );
      }
        
}


void MapDisplayPG::registerAdditionalUnit( Vehicle* veh )
{
   additionalUnit = veh;
}

class PG_MapDisplay : public MapDisplayInterface {
           MapDisplayPG* mapDisplayWidget;
         public:
           PG_MapDisplay( MapDisplayPG* mapDisplayWidget_ ) : mapDisplayWidget( mapDisplayWidget_ ) {};
           
           int displayMovingUnit ( const MapCoordinate3D& start, const MapCoordinate3D& dest, Vehicle* vehicle, int fieldnum, int totalmove, SoundStartCallback soundStart );
           void deleteVehicle ( Vehicle* vehicle ) {};
           void displayMap ( void );
           void displayMap ( Vehicle* vehicle );
           void displayPosition ( int x, int y );
           void resetMovement ( void ) {};
           void startAction ( void );
           void stopAction ( void );
           void displayActionCursor ( int x1, int y1, int x2, int y2 ) {};
           void removeActionCursor ( void ) {};
           void updateDashboard ();
           void repaintDisplay ();
    };



int  PG_MapDisplay :: displayMovingUnit ( const MapCoordinate3D& start, const MapCoordinate3D& dest, Vehicle* vehicle, int fieldnum, int totalmove, SoundStartCallback soundStart )
{
   if ( actmap->playerView < 0 )
      return 0;

   int height1 = start.getNumericalHeight();
   int height2 = dest.getNumericalHeight();
   if ( height2 == -1 )
      height2 = height1;
   else
      if ( height1== -1 )
         height1 = height2;

   pfield fld1 = actmap->getField ( start );
   int view1 = fieldVisibility ( fld1, actmap->playerView );

   pfield fld2 = actmap->getField ( dest );
   int view2 = fieldVisibility ( fld2, actmap->playerView );

   if (  (view1 >= visible_now  &&  view2 >= visible_now ) || ( vehicle->getOwner() == actmap->playerView ))
      if ( ((vehicle->height >= chschwimmend) && (vehicle->height <= chhochfliegend)) || (( view1 == visible_all) && ( view2 == visible_all )) || ( actmap->actplayer == actmap->playerView )) {
         soundStart( 1 );
         mapDisplayWidget->displayUnitMovement( actmap, vehicle, start, dest );
      }


   int result = 0;
   if (view1 >= visible_now )
      result +=1;

   if ( view2 >= visible_now )
      result +=2;

   return result;
}

void PG_MapDisplay :: displayMap ( void )
{
   ::repaintMap();
}

void PG_MapDisplay :: displayMap ( Vehicle* vehicle )
{
   mapDisplayWidget->registerAdditionalUnit( vehicle ); 
   ::repaintMap();
   mapDisplayWidget->registerAdditionalUnit( NULL ); 
}


void PG_MapDisplay :: displayPosition ( int x, int y )
{
   mapDisplayWidget->Update();
}


void PG_MapDisplay :: startAction ( void )
{
}

void PG_MapDisplay :: stopAction ( void )
{
}

void PG_MapDisplay :: updateDashboard ( void )
{
   updateFieldInfo();
}

void PG_MapDisplay :: repaintDisplay ()
{
   ::repaintDisplay();
}

    
    

MapDisplayInterface& getDefaultMapDisplay()
{
   static PG_MapDisplay* mapDisplay = NULL;
   if ( !mapDisplay )
      mapDisplay = new PG_MapDisplay( theMapDisplay );
   return *mapDisplay;
}













bool tempsvisible;

extern void repaintdisplay();

int showresources = 0;

int lockdisplaymap = 0;


tpaintmapborder* mapborderpainter = NULL;


int vfbscanlinelength;


int vfbstartdif;
int scrstartdif;
int scrstartpage;

#define cursordownshift 0

#define cursorrightshift -8


#define mapborderwidth 4

int scrleftspace = 21;

#define scrtopspace 22
#define vfbleftspace 80
#define vfbtopspace 80



void copyvfb2displaymemory_zoom ( void* parmbuf, int x1, int y1, int x2, int y2 )
{
   int tempdirectscreenaccess = agmp->directscreenaccess;
   if ( hgmp->directscreenaccess != agmp->directscreenaccess )
      agmp->directscreenaccess = hgmp->directscreenaccess;

   int* parmi = (int*) parmbuf;

   char* esi = (char*) parmi[0];
   char* edi = (char*) parmi[1];

   int edx = parmi[3];
   int ecx;
   int* ebp;
   do {
      ebp = (int*) parmi[4];
      ecx =  parmi[2];
      do {

         esi += *ebp;

         *(edi++) = *(esi++);
         ebp++;

         ecx--;

      } while ( ecx ); /* enddo */

      esi += parmi[5];
      edi += parmi[6];
      ebp = (int*) parmi[7];

      edx--;
      esi += ebp[edx];

   } while ( edx ); /* enddo */

   copySurface2screen( x1, y1, x2, y2 );
   agmp->directscreenaccess = tempdirectscreenaccess;
}

void copyvfb2displaymemory_zoom ( void* parmbuf )
{
   copyvfb2displaymemory_zoom ( parmbuf, idisplaymap.invmousewindow.x1, idisplaymap.invmousewindow.y1, idisplaymap.invmousewindow.x2, idisplaymap.invmousewindow.y2 );
}



void         initMapDisplay( )
{
   int x = -1;
   int y = -1;
   int game_x;
   int maped_x;
   if ( x == -1) {
      game_x = ( agmp->resolutionx - 225 ) / fielddistx;
      maped_x = ( agmp->resolutionx - 290 ) / fielddistx;
#ifdef sgmain

      x = game_x;
#else

      x = maped_x;
#endif

   }



   if ( y == -1)
      y = ( agmp->resolutiony - 75 ) / fielddisty;

   if ( y & 1 )
      y--;

   if ( actmap ) {
      actmap->actplayer = 0;
      actmap->xsize = 0;
      actmap->ysize = 0;
      actmap->field = NULL;
   }

   tempsvisible = true;
   cursor.color = 0;
#ifdef sgmain

   idisplaymap.init( scrleftspace, 21, agmp->resolutionx - 215, agmp->resolutiony - 55 );
#else

   idisplaymap.init( scrleftspace, 21, agmp->resolutionx - 295, agmp->resolutiony - 40 );
#endif

}


int getmapposx ( void )  // return the screencoordinates of the upper left position of the displayed map
{
   return scrleftspace + cursorrightshift;
}

int getmapposy ( void )
{
   return scrtopspace  + cursordownshift;
}


int   getfieldundermouse ( int* xf, int* yf )
{
   for ( int i=0;i<=idisplaymap.getscreenxsize();i++ ) {
      for ( int j=0;j<idisplaymap.getscreenysize() ;j++ ) {
         int k;

         int xp = idisplaymap.getfieldposx( i, j );
         int yp = idisplaymap.getfieldposy( i, j );

         if ( mouseparams.x >= xp && mouseparams.x < xp+idisplaymap.getfieldsizex() && mouseparams.y >= yp && mouseparams.y < yp + idisplaymap.getfieldsizey() ) {
            k = 0;


            // putspriteimage ( xp, yp, icons.fieldshape );

            int xd = mouseparams.x - xp;
            int yd = mouseparams.y - yp;


            static int actpictwidth = -1;
            static void* picture = NULL;

            int actwidth = idisplaymap.getfieldsizex();
            if ( actpictwidth != actwidth ) {
               if ( picture )
                  delete[] picture ;

               TrueColorImage* zimg = zoomimage ( icons.fieldshape, idisplaymap.getfieldsizex(), idisplaymap.getfieldsizey(), pal, 0 );
               picture = convertimage ( zimg, pal ) ;
               delete zimg;

               int h;
               getpicsize ( picture, actpictwidth, h );
            }

            int pix = getpixelfromimage ( picture, xd, yd );
            if ( pix >= 0 && pix < 255 )
               k++;


            /*
             if ( pw[0] >= xd  && pw[1] >= yd )
                for ( int xxx = 0; xxx <= pw[0]; xxx++ )
                   for ( int yyy = 0; yyy <= pw[1]; yyy++ )
                      if ( pc[ yyy * ( pw[0] + 1) + xxx] != 255 )
                         putpixel ( xp + xxx, yp + yyy, 255 );
            */

            if ( k ) {
               *xf = i;
               *yf = j;
               return 1;
            }
         } /* endif */


      } /* endfor */
   } /* endfor */
   /*
   if ( mouseinrect ( &idisplaymap.invmousewindow  )) {
      int stepx =   fielddistx * zoomlevel.getzoomlevel() / 100 ;
      int stepy = 2*fielddisty * zoomlevel.getzoomlevel() / 100 ;
      int move = 0;
      if ( mouseparams.x > idisplaymap.invmousewindow.x2 - stepx )
         if (actmap->xpos + idisplaymap.getscreenxsize() < actmap->xsize)
            move += 1;

      if ( mouseparams.y > idisplaymap.invmousewindow.y2 - stepy )
         if (actmap->ypos + idisplaymap.getscreenysize() < actmap->ysize)
            move += 2;

      if ( move ) {

         int bb = cursor.an;
         if (bb)
            cursor.hide();

         if ( move & 1 )
            actmap->xpos++;
         else
            stepx = 0;

         if ( move & 2 )
            actmap->ypos+=2;
         else
            stepy = 0;

         setmouseposition ( mouseparams.x - stepx, mouseparams.y - stepy );

         displaymap();

         if (bb)
            cursor.show();

         return getfieldundermouse ( xf, yf );
      }
   }
   */

   return 0;
}


tdisplaymap idisplaymap;


ZoomLevel zoomlevel;

ZoomLevel :: ZoomLevel ( void )
{
   zoom = 75;
   queried = 0;
}

int ZoomLevel :: getmaxzoom ( void )
{
   return 100;
}

int ZoomLevel :: getminzoom ( void )
{
   return 33;
}

int ZoomLevel :: getzoomlevel ( void )
{
   if ( !queried ) {
      int mz;
#ifdef sgmain

      mz = CGameOptions::Instance()->mapzoom;
#else

      mz = CGameOptions::Instance()->mapzoomeditor;
#endif

      if ( mz >= getminzoom()  &&
            mz <= getmaxzoom() )
         zoom = mz;
      queried = 1;
   }
   return zoom;
}

void ZoomLevel :: setzoomlevel ( int newzoom )
{
   zoom = newzoom;
   idisplaymap.setnewsize ( zoom );
#ifdef sgmain

   CGameOptions::Instance()->mapzoom = newzoom;
#else

   CGameOptions::Instance()->mapzoomeditor = newzoom;
#endif

   CGameOptions::Instance()->setChanged();
}


void tgeneraldisplaymapbase :: setmouseinvisible ( void )
{}

void tgeneraldisplaymapbase :: restoremouse ( void )
{}

tgeneraldisplaymapbase :: tgeneraldisplaymapbase ( void )
{
   invmousewindow.x1 = 0;
   invmousewindow.y1 = 0;
   invmousewindow.x2 = hgmp->resolutionx;
   invmousewindow.y2 = hgmp->resolutiony;
}






int tgeneraldisplaymap :: getfieldsizex ( void )
{
   return fieldsizex * zoom / 100;
}

int tgeneraldisplaymap :: getfieldsizey ( void )
{
   return fieldsizey * zoom / 100;
}


int tgeneraldisplaymap :: getfielddistx ( void )
{
   return fielddistx * zoom / 100;
}

int tgeneraldisplaymap :: getfielddisty ( void )
{
   return fielddisty * zoom / 100;
}



void tgeneraldisplaymap :: init ( int xs, int ys )
{
   if ( actmap )
      playerview = actmap->playerView;

   dispmapdata.numberoffieldsx = xs;
   dispmapdata.numberoffieldsy = ys;
}


void tgeneraldisplaymap :: _init ( int xs, int ys )
{
   if ( actmap )
      playerview = actmap->playerView;

   window.xsize = xs;
   window.ysize = ys;

   setnewsize ( zoomlevel.getzoomlevel() );
}


void tgeneraldisplaymap :: setnewsize ( int _zoom )
{
   zoom = _zoom;
   dispmapdata.numberoffieldsx = 1 + (window.xsize - (fieldsizex + fielddisthalfx) * zoom / 100 ) * 100 / zoom / fielddistx;
   dispmapdata.numberoffieldsy = 2 + 2*((window.ysize - (fieldsizey + fielddisty) * zoom / 100 ) * 100 / zoom / (fielddisty*2));

   dispmapdata.disp.x1 = -1;
   dispmapdata.disp.y1 = -2;
   dispmapdata.disp.x2 = dispmapdata.numberoffieldsx + 2;
   dispmapdata.disp.y2 = dispmapdata.numberoffieldsy + 4;

   if ( dispmapdata.numberoffieldsy & 1 )
      dispmapdata.numberoffieldsy--;
}


int tdisplaymap :: getfieldposx ( int x, int y )
{
   if ( x < 0 )
      x = 0;

   if ( y & 1 )   /*  gerade reihennummern  */
      return windowx1 + (fielddisthalfx + x * fielddistx ) * zoom / 100;
   else
      return windowx1 + ( x * fielddistx ) * zoom / 100;
}


int tdisplaymap :: getfieldposy ( int x, int y )
{
   if ( y < 0 )
      y = 0;
   return windowy1  + (y * fielddisty) * zoom / 100;
}


tdisplaymap :: tdisplaymap ( void )
{
   copybufsteps = NULL;
}

void tdisplaymap :: init ( int x1, int y1, int x2, int y2 )
{
   invmousewindow.x1 = x1;
   invmousewindow.y1 = y1;
   invmousewindow.x2 = x2;
   invmousewindow.y2 = y2;

   rgmp = *agmp;
   rgmp.directscreenaccess = 1;

   tgeneraldisplaymap :: _init ( x2-x1, y2-y1 );


   dispmapdata.vfbwidth = window.xsize * 100/zoomlevel.getminzoom() + 5 * fielddistx;
   vfbscanlinelength = dispmapdata.vfbwidth;

   dispmapdata.vfbheight = window.ysize * 100/zoomlevel.getminzoom() + 10 * fielddisty;

   windowx1 = x1;
   windowy1 = y1;
   calcdisplaycache();

   if ( mapborderpainter )
      mapborderpainter->setrectangleborderpos ( x1-1, y1-1, x2+1, y2+1 );

   vfb.address = new char [ dispmapdata.vfbwidth * dispmapdata.vfbheight ];

   memset( vfb.address , 0, dispmapdata.vfbwidth * dispmapdata.vfbheight  );
   memset ( &displaymovingunit, 0, sizeof ( displaymovingunit ));

   vfbstartdif = vfbtopspace * vfbscanlinelength + vfbleftspace;

   vfb.parameters.resolutionx = dispmapdata.vfbwidth;
   vfb.parameters.resolutiony = dispmapdata.vfbheight;
   vfb.parameters.windowstatus = 100;
   vfb.parameters.scanlinelength= dispmapdata.vfbwidth;
   vfb.parameters.scanlinenumber = dispmapdata.vfbheight;
   vfb.parameters.bytesperscanline = dispmapdata.vfbwidth;
   vfb.parameters.byteperpix = 1;
   vfb.parameters.linearaddress = (PointerSizedInt) vfb.address;
   vfb.parameters.directscreenaccess = 1;

   vfb.parameters.surface = new Surface ( Surface::CreateSurface( vfb.address, dispmapdata.vfbwidth, dispmapdata.vfbheight, 8, dispmapdata.vfbwidth ) );
   vfb.parameters.surface->assignDefaultPalette();

}



void  tdisplaymap :: deletevehicle ( void )
{
   displaymovingunit.eht = NULL;
}



int  tgeneraldisplaymap :: getscreenxsize( int target )
{
   if ( actmap && dispmapdata.numberoffieldsx >= actmap->xsize && actmap->xsize > 0 )
      return actmap->xsize ;
   else
      return dispmapdata.numberoffieldsx;
}

int  tgeneraldisplaymap :: getscreenysize( int target )
{
   if ( actmap && dispmapdata.numberoffieldsy >= actmap->ysize && actmap->ysize > 0 )
      return actmap->ysize;
   else
      return dispmapdata.numberoffieldsy;
}


void tgeneraldisplaymap :: pnt_terrain ( void )
{
   if ( playerview < 0 )
      displaymessage("tgeneraldisplaymap :: pnt_terrain ; playerview < 0", 2 );

   *agmp = vfb.parameters;

   for (int y=dispmapdata.disp.y1; y < dispmapdata.disp.y2; y++ ) {
      for ( int x=dispmapdata.disp.x1; x < dispmapdata.disp.x2; x++ ) {
         pfield fld = getfield ( actmap->xpos + x, actmap->ypos + y );
         if ( fld ) {
            int b = fieldVisibility ( fld, playerview );

            if ( b != visible_not ) {
               int yp;
               int r;
               if (y & 1 )   /*  ungerade reihennummern  */
                  r = vfbleftspace + fielddisthalfx + x * fielddistx;
               else
                  r = vfbleftspace + x * fielddistx;

               yp = vfbtopspace + y * fielddisty;
               fld->typ->paint ( SPoint(r, yp) );
            }
         } else {
            int yp;
            int r;
            if (y & 1 )   /*  ungerade reihennummern  */
               r = vfbleftspace + fielddisthalfx + x * fielddistx;
            else
               r = vfbleftspace + x * fielddistx;

            yp = vfbtopspace + y * fielddisty;
            putspriteimage ( r, yp, icons.mapbackground );
         }

      } /* endfor */
   } /* endfor */
}

void tdisplaymap :: setnewsize ( int _zoom )
{
   tgeneraldisplaymap :: setnewsize ( _zoom );
   calcdisplaycache();
}


void tdisplaymap :: calcdisplaycache( void )
{
   int sz;
   if ( window.xsize > window.ysize )
      sz = window.xsize ;
   else
      sz = window.ysize ;

   if ( !copybufsteps ) {
      copybufsteps = new int [ sz ];
      copybufstepwidth = new int [ sz ];
   }

   int last = 0;
   for ( int xp = 0; xp < sz; xp++ ) {
      int pos = xp * 100 / zoom ;
      int n = pos - last - 1;
      if ( n < 0 )
         n = 0;
      copybufsteps[ xp ] = n;
      copybufstepwidth[ xp ] = n * dispmapdata.vfbwidth;
      last = pos;
      if ( xp < window.xsize )
         vfbwidthused = last;
   }
}

void tdisplaymap :: cp_buf ( void )
{

   if ( hgmp->windowstatus != 100 ) {}
   else {


      struct {
         int src;
         int dst;
         int x;
         int y;
         int* steps;
         int srcdif;
         int dstdif;
         int* vfbsteps;
      }
      parm;

      parm.dst = hgmp->linearaddress + windowx1 + windowy1 * hgmp->bytesperscanline ;
      parm.src = agmp->linearaddress + vfbstartdif;
      parm.x = window.xsize;
      parm.y = window.ysize;
      parm.steps = copybufsteps;
      parm.srcdif = agmp->bytesperscanline - vfbwidthused - 1;
      parm.dstdif = hgmp->bytesperscanline - window.xsize;
      parm.vfbsteps = copybufstepwidth;

      copyvfb2displaymemory_zoom ( &parm.src );

   }
}


void tdisplaymap :: cp_buf ( int x1, int y1, int x2, int y2 )
{

   struct {
      int src;
      int dst;
      int x;
      int y;
      int* steps;
      int srcdif;
      int dstdif;
      int* vfbsteps;
   }
   parm;

   parm.dst = hgmp->linearaddress + windowx1 + windowy1 * hgmp->bytesperscanline ;
   parm.src = agmp->linearaddress + vfbstartdif;
   parm.x = window.xsize;
   parm.y = window.ysize;
   parm.steps = copybufsteps;
   parm.srcdif = agmp->bytesperscanline - vfbwidthused - 1;
   parm.dstdif = hgmp->bytesperscanline - window.xsize;
   parm.vfbsteps = copybufstepwidth;

   copyvfb2displaymemory_zoom ( &parm.src, getfieldposx ( x1-2, y1-2), getfieldposy( x1-2, y1-2 ), getfieldposx ( x2, y2) + getfieldsizex(), getfieldposy( x2, y2 ) + getfieldsizex());
}




void tgeneraldisplaymap :: pnt_main ( void )
{
#ifndef sgmain
   activefontsettings.font = schriften.monogui;
   activefontsettings.background = darkgray;
   activefontsettings.color = white;
   activefontsettings.length = 0;
   activefontsettings.justify = lefttext;
   activefontsettings.height = 0;


   playerview = actmap->playerView;

   if ( playerview < 0 )
      displaymessage("tgeneraldisplaymap :: pnt_main ; playerview < 0", 2 );

   int b;
   pfield fld;

   int yp;
   int r;

   int x, y;

   /*****************************************************************/
   /*****************************************************************/

   for (int hgt = 0; hgt < 9 ;hgt++ ) {
      int binaryheight = 0;
      if ( hgt > 0 )
         binaryheight = 1 << ( hgt-1);

      for ( y=dispmapdata.disp.y1; y < dispmapdata.disp.y2; y++ ) {
         for ( x=dispmapdata.disp.x1; x < dispmapdata.disp.x2; x++ ) {
            fld = getfield ( actmap->xpos + x, actmap->ypos + y );
            if ( fld ) {
               b = fieldVisibility ( fld, playerview );

               if (y & 1 )   /*  ungerade reihennummern  */
                  r = vfbleftspace + fielddisthalfx + x * fielddistx;
               else
                  r = vfbleftspace + x * fielddistx;

               yp = vfbtopspace + y * fielddisty;

               if (b > visible_ago ) {


                  /* display buildings */

                  if ( fld->building  &&  (log2(fld->building->typ->buildingheight)+1 == hgt ) )
                     if ((b == visible_all) || (fld->building->typ->buildingheight >= chschwimmend) || ( fld->building->color == playerview*8 ))
                        fld->building->paintSingleField( getActiveSurface(), SPoint( r, yp), fld->building->getLocalCoordinate( MapCoordinate(actmap->xpos + x, actmap->ypos + y)));


                  /* display units */
                  if ( fld->vehicle  &&  (fld->vehicle->height == binaryheight))
                     if ( ( fld->vehicle->color == playerview * 8 ) || (b == visible_all) || ((fld->vehicle->height >= chschwimmend) && (fld->vehicle->height <= chhochfliegend)))
                        fld->vehicle->paint( getActiveSurface(), SPoint(r, yp));

               }

               /* display streets, railroads and pipelines */
               for ( tfield::ObjectContainer::iterator o = fld->objects.begin(); o != fld->objects.end(); o++ ) {
                  int h = o->typ->imageHeight;
                  if (b > visible_ago || o->typ->visibleago )
                     if (  h >= hgt*30 && h < 30 + hgt*30 )
                        o->display ( getActiveSurface(), SPoint(r, yp), fld->getweather() );
               }




               if (b > visible_ago ) {
                  /* display mines */
                  if ( b == visible_all )
                     if ( !fld->mines.empty() && hgt == 3 ) {
                        if ( fld->mines.begin()->type != cmmooredmine )
                           putspriteimage( r, yp, getmineadress(fld->mines.begin()->type) );
                        else
                           putpicturemix ( r, yp, getmineadress(fld->mines.begin()->type, 1 ) ,  0, (char*) colormixbuf );
#ifdef karteneditor

                        bar ( r + 5 , yp +5, r + 15 , yp +10, 20 + fld->mineowner() * 8 );
#endif

                     }


                  /* display marked fields */
                  if ( hgt == 8 ) {

                     if ( fld->a.temp && tempsvisible )
                        putspriteimage(  r, yp, cursor.markfield);
                     else
                        if ( fld->a.temp2 && tempsvisible )
                           putspriteimage(  r, yp, xlatpict ( &xlattables.a.dark2 , cursor.markfield));


#ifdef showtempnumber

                     activefontsettings.color = white;
                     showtext2(strrr( fld->temp ), r + 5, yp + 5 );
                     activefontsettings.color = black;
                     showtext2(strrr( fld->temp2 ), r + 5, yp + 20 );
#endif

                  }

               } else {
                  if (b == visible_ago) {
                     if ( fld->building  &&  (log2(fld->building->typ->buildingheight)+1 == hgt ) )
                        if ((b == visible_all) || (fld->building->typ->buildingheight >= chschwimmend) || ( fld->building->color == playerview*8 ))
                           fld->building->paintSingleField( getActiveSurface(), SPoint( r, yp), fld->building->getLocalCoordinate( MapCoordinate(actmap->xpos + x, actmap->ypos + y)));

                  }
               }

               /* display resources */
               if ( hgt == 8 && b >= visible_ago) {
#ifndef karteneditor
                  if ( fld->resourceview && (fld->resourceview->visible & ( 1 << playerview) ) ) {
                     if ( showresources == 1 ) {
                        showtext2( strrr ( fld->resourceview->materialvisible[playerview] ) , r + 10 , yp +10 );
                        showtext2( strrr ( fld->resourceview->fuelvisible[playerview] )     , r + 10 , yp +20 );
                     } else
                        if ( showresources == 2 ) {
                           if ( fld->resourceview->materialvisible[playerview] )
                              bar ( r + 10 , yp +2, r + 10 + fld->resourceview->materialvisible[playerview] / 10, yp +6, 23 );

                           if ( fld->resourceview->fuelvisible[playerview] )
                              bar ( r + 10 , yp + 14 -2, r + 10 + fld->resourceview->fuelvisible[playerview] / 10, yp +14 +2 , 191 );
                        }
                  }
#else
                  if ( showresources == 1 ) {
                     showtext2( strrr ( fld->material ) , r + 10 , yp );
                     showtext2( strrr ( fld->fuel )     , r + 10 , yp + 10 );
                  } else if ( showresources == 2 ) {
                     if ( fld->material )
                        bar ( r + 10 , yp -2, r + 10 + fld->material / 10, yp +2, 23 );
                     if ( fld->fuel )
                        bar ( r + 10 , yp +10 -2, r + 10 + fld->fuel / 10, yp +10 +2 , 191 );
                  }
#endif

               }

            }
         }
      }
      displayadditionalunits ( hgt );
   }

   /****************************************************************************/
   /*viewbehinderungen: Visible_Ago,  Visible_NOT  zeichnen  ...            ÿ */
   /****************************************************************************/
   for ( y=dispmapdata.disp.y1; y < dispmapdata.disp.y2; y++ ) {
      for ( x=dispmapdata.disp.x1; x < dispmapdata.disp.x2; x++ ) {
         fld = getfield ( actmap->xpos + x, actmap->ypos + y );
         if ( fld ) {
            b = fieldVisibility (fld, playerview );

            if (y & 1 )   /*  ungerade reihennummern  */
               r = vfbleftspace + fielddisthalfx + x * fielddistx;
            else
               r = vfbleftspace + x * fielddistx;
            yp = vfbtopspace + y * fielddisty;

            if (b == visible_ago) {
#if 0
               for (int hgt = 0; hgt < 9 ;hgt++ ) {
                  /*
                  int binaryheight = 0;
                  if ( hgt > 0 )
                     binaryheight = 1 << ( hgt-1);
                  */

                  /* display objects */
                  // if ( !fld->building )
                  for ( tfield::ObjectContainer::iterator o = fld->objects.begin(); o != fld->objects.end(); o++ )
                     if ( o->typ->visibleago ) {
                        int h = o->typ->height;
                        if (  h >= hgt*30 && h < 30 + hgt*30 )
                           o->display ( r - streetleftshift , yp - streettopshift, fld->getweather() );
                     }
               }
#endif

               // putspriteimage( r + unitrightshift , yp + unitdownshift , view.va8);
               // putshadow( r, yp, icons.view.nv8, &xlattables.a.dark2 );
               if ( fld->a.temp && tempsvisible )
                  putspriteimage(  r, yp, cursor.markfield);
               else
                  if ( fld->a.temp2 && tempsvisible )
                     putspriteimage(  r, yp, xlatpict ( &xlattables.a.dark2 , cursor.markfield));

#ifdef showtempnumber

               activefontsettings.color = white;
               showtext2(strrr( fld->temp ), r + 5, yp + 5 );
               activefontsettings.color = black;
               showtext2(strrr( fld->temp2 ), r + 5, yp + 20 );
#endif

            } else
               if (b == visible_not) {
                  // putspriteimage( r, yp, icons.view.nv8 );
                  if ( ( fld->a.temp || fld->a.temp2 ) && tempsvisible )
                     putspriteimage(  r, yp, cursor.markfield);

#ifdef showtempnumber

                  activefontsettings.color = white;
                  showtext2(strrr( fld->temp ), r + unitrightshift + 5, yp + unitdownshift + 5 );
                  activefontsettings.color = black;
                  showtext2(strrr( fld->temp2 ), r + unitrightshift + 5, yp + unitdownshift + 20 );
#endif

               }
            /*
                    activefontsettings.color = white;
                    activefontsettings.font = schriften.guifont;
                    activefontsettings.length = 0;
                    activefontsettings.background = 255;
                    showtext2(strrr( fld->view[1].view ), r + 10 + 5, yp + 10 + 5 );
                    showtext2(strrr( fld->view[0].jamming ), r + 10 + 5, yp + 10 + 15 );
             */
         }
      }
   }
#endif

}


void tgeneraldisplaymap :: displayadditionalunits ( int height )
{}

void tdisplaymap :: displayadditionalunits ( int height )
{
   if( displaymovingunit.eht )
      if ( height == 4 || height == 5 ) {

         if ( (height == 4 && displaymovingunit.eht->height <= chfahrend) || (height == 5 && displaymovingunit.eht->height > chfahrend )) {

            int xp;
            if (displaymovingunit.ypos & 1 )   /*  ungerade reihennummern  */
               xp = vfbleftspace  + fielddisthalfx + (displaymovingunit.xpos - actmap->xpos) * fielddistx;
            else
               xp = vfbleftspace + (displaymovingunit.xpos - actmap->xpos) * fielddistx;
            int yp = vfbtopspace + (displaymovingunit.ypos - actmap->ypos) * fielddisty;

            pfield fld = getfield ( displaymovingunit.xpos, displaymovingunit.ypos);
            int b = fieldVisibility ( fld, playerview );

            int shadowdist = -1;
            if ( displaymovingunit.hgt > 0 && displaymovingunit.eht->height > chfahrend )
               shadowdist = 6 * displaymovingunit.hgt / 10 ;


            if ( b == visible_all ||
                  (displaymovingunit.eht->height >= chschwimmend && displaymovingunit.eht->height <= chhochfliegend ) ||
                  displaymovingunit.eht->getOwner() == playerview )
               displaymovingunit.eht->paint( getActiveSurface(), SPoint( xp + displaymovingunit.dx , yp + displaymovingunit.dy), shadowdist );

         }
      }

}


void         displaymap(  )
{
#ifndef sgmain
   if ( !actmap )
      return;

   if ( !actmap->xsize  ||  !actmap->ysize   || lockdisplaymap )
      return;

   GraphicSetManager::Instance().setActive ( actmap->graphicset );

   int         ms;   /*  mousestatus  */
   char      bb;   /*  cursorstatus   */

   ms = getmousestatus();
   bb = cursor.an;
   if (bb)
      cursor.hide();

   if (ms == 2)
      idisplaymap.setmouseinvisible();

   npush ( *agmp );

   if ( mapborderpainter )
      if ( mapborderpainter->getlastpaintmode () < 1)
         mapborderpainter->paint();

   if ( actmap )
      idisplaymap.playerview = actmap->playerView;

#ifdef logging

   logtofile("spfst / displaymap ; vor pnt_terrain");
#endif

   idisplaymap.pnt_terrain (  );

#ifdef logging

   logtofile("spfst / displaymap ; vor pnt_main");
#endif

   idisplaymap.pnt_main (  );

#ifdef logging

   logtofile("spfst / displaymap ; vor cp_buf");
#endif

   idisplaymap.cp_buf (  );

   npop ( *agmp );




   if (ms == 2)
      idisplaymap.restoremouse();

   if (bb)
      cursor.show();

#else


   repaintMap();

#endif

}



struct tfieldlist
{
   int num;
   int x[6];
   int y[6];
   int minx, maxx;
   int miny, maxy;
   // int minxp, maxxp;
   // int minyp, maxyp;
   int visible;
};

typedef tfieldlist* pfieldlist;


void adddirpts ( int x, int y, pfieldlist lst, int dir )
{
   int xt = x;
   int yt = y;
   getnextfield ( xt, yt, dir );
   lst->x [ lst->num ] = xt;
   lst->y [ lst->num ] = yt;
   lst->num++;
}

int pointvisible ( int x, int y )
{
   if ( (x >= 0) &&
         (x <  idisplaymap.getscreenxsize()) &&
         (y >= 0) &&
         (y < idisplaymap.getscreenysize()))
      return 1;
   else
      return 0;

}


pfieldlist generatelst ( int x1, int y1, int x2, int y2 )
{
   pfieldlist list = new tfieldlist ;
   list->num = 2;
   list->x[0] = x1;
   list->y[0] = y1;
   list->x[1] = x2;
   list->y[1] = y2;
   list->visible = 0;

   if ( x1 != x2 || y1 != y2 ) {

      int dir = getdirection(x1,y1,x2,y2);

      switch ( dir ) {

         case 0:
            adddirpts ( x1, y1, list, 5 );
            adddirpts ( x1, y1, list, 1 );
            adddirpts ( x1, y1, list, 2 );
            break;

         case 1:
            adddirpts ( x1, y1, list, 0 );
            adddirpts ( x1, y1, list, 2 );
            adddirpts ( x2, y2, list, 2 );
            break;

         case 2:
            adddirpts ( x1, y1, list, 1 );
            adddirpts ( x1, y1, list, 3 );
            adddirpts ( x2, y2, list, 2 );
            break;

         case 3:
            adddirpts ( x2, y2, list, 2 );
            adddirpts ( x1, y1, list, 4 );
            break;

         case 4:
            adddirpts ( x1, y1, list, 3 );
            adddirpts ( x1, y1, list, 5 );
            adddirpts ( x2, y2, list, 3 );
            break;

         case 5:
            adddirpts ( x1, y1, list, 0 );
            adddirpts ( x1, y1, list, 4 );
            adddirpts ( x1, y1, list, 2 );
            break;

      } /* endswitch */

   }

   list->minx = 0xffff;
   list->miny = 0xffff;
   list->maxx = 0;
   list->maxy = 0;

   for (int i = 0; i < list->num; i++) {

      if ( list->x[ i ] < list->minx )
         list->minx = list->x[ i ];

      if ( list->y[ i ] < list->miny )
         list->miny = list->y[ i ];

      if ( list->x[ i ] > list->maxx )
         list->maxx = list->x[ i ];

      if ( list->y[ i ] > list->maxy )
         list->maxy = list->y[ i ];

   } /* endfor */

   if ( pointvisible ( list->minx, list->miny ) )
      list->visible = 1;
   if ( pointvisible ( list->minx, list->maxy ) )
      list->visible = 1;
   if ( pointvisible ( list->maxx, list->miny ) )
      list->visible = 1;
   if ( pointvisible ( list->maxx, list->maxy ) )
      list->visible = 1;

   return list;
}


void  tdisplaymap :: resetmovement ( void )
{
   memset ( &displaymovingunit, 0, sizeof ( displaymovingunit ));
}


void  tdisplaymap :: movevehicle( int x1,int y1, int x2, int y2, Vehicle* eht, int height1, int height2, int fieldnum, int totalmove )
{
   int dir;
   if ( x1 != x2 || y1 != y2 ) {
      dir = getdirection(x1,y1,x2,y2);
      eht->direction = dir;
   } else
      dir = -1;

   displaymovingunit.eht = NULL;

   idisplaymap.pnt_terrain (  );
   idisplaymap.pnt_main (  );


   npush ( dispmapdata );
   x1 -= actmap->xpos;
   y1 -= actmap->ypos;
   x2 -= actmap->xpos;
   y2 -= actmap->ypos;

   int xp1,xp2, yp1, yp2;

   pfieldlist touchedfields = generatelst ( x1, y1, x2, y2 );

   if ( touchedfields->visible ) {

      xp1 = vfbleftspace + touchedfields->minx * fielddistx;
      if ( xp1 < 0 )
         xp1 = 0;

      xp2 = vfbleftspace + touchedfields->maxx * fielddistx + fielddisthalfx + fieldxsize;
      if ( xp2 > vfbscanlinelength )
         xp2 = vfbscanlinelength;


      yp1 = vfbtopspace + touchedfields->miny * fielddisty;
      if ( yp1 < 0 )
         yp1 = 0;

      yp2 = vfbtopspace + touchedfields->maxy * fielddisty + fieldysize;
      if ( yp2 > dispmapdata.vfbheight )
         yp2 = dispmapdata.vfbheight;

      char* picbuf = new char [ imagesize ( xp1, yp1, xp2, yp2 ) ];
      getimage ( xp1, yp1, xp2, yp2, picbuf );

      int dx;
      int dy;

      switch ( dir ) {
         case -1:
            dx = 0;
            dy = 0;
            break;
         case 0:
            dx = 0;
            dy = -fielddisty*2;
            break;
         case 1:
            dx = fielddisthalfx;
            dy = -fielddisty;
            break;
         case 2:
            dx = fielddisthalfx;
            dy = fielddisty;
            break;
         case 3:
            dx = 0;
            dy = 2*fielddisty;
            break;
         case 4:
            dx = -fielddisthalfx;
            dy = fielddisty;
            break;
         case 5:
            dx = -fielddisthalfx;
            dy = -fielddisty;
            break;
      } /* endswitch */

      displaymovingunit.eht = eht;

      int h1;
      int h2;

      /*
       int r1;
       if ( dir & 1 )
          r1 = 20;
       else
          r1 = 40; */

      int r2 = 0;


      /************************************/
      /*     Schattenposition computen   */
      /************************************/


      if ( height1 != height2  && ( height1 > 3  || height2 > 3 ) ) {

         int completion = totalmove;
         int went = fieldnum;
         // int togo = totalmove - fieldnum;

         int ht1 = 10 * ( height1 - log2 ( chfahrend ) );
         int ht2 = 10 * ( height2 - log2 ( chfahrend ) );
         int htd = ht2- ht1;

         if ( height1 < height2 ) {
            if ( height1 == 3 ) {
               int takeoff = completion * 2 / 3 ;
               int ascend = completion - takeoff;
               if ( went >=  takeoff ) {
                  h1 = ht1 + htd * ( went-takeoff ) / ascend;
                  h2 = ht1 + htd * ( went-takeoff + 1 ) / ascend;
               } else {
                  h1 = ht1;
                  h2 = ht1;
               }
            } else {
               h1 = ht1 + htd * went / completion;
               h2 = ht1 + htd * (went + 1) / completion;
            }
         } else {
            if ( height2 == 3 ) {
               int landpos = (completion+2) / 3;

               if ( went <  landpos ) {
                  h1 = ht1 + htd * went / landpos;
                  h2 = ht1 + htd * (went + 1 ) / landpos;
               } else {
                  h1 = ht2;
                  h2 = ht2;
               }
            } else {
               h1 = ht1 + htd * went / completion;
               h2 = ht1 + htd * (went + 1) / completion;
            }
         }

      } else
         if ( height1 > log2(chfahrend) ) {
            h1 = 10 * ( height1 - log2 ( chfahrend ) );
            h2 = h1;
         } else {
            h1 = 0;
            h2 = 0;
         }

      // int step = 1;


      /************************************/
      /*     eigentliche movement         */
      /************************************/

      int starttick = ticker;
      int sdx = dx;
      int sdy = dy;

      do {
         putimage ( xp1, yp1, picbuf );

         r2++;


         int tick =  CGameOptions::Instance()->movespeed - ( ticker - starttick );
         if ( tick < 0 )
            tick = 0;

         SDL_Delay(1);


         displaymovingunit.xpos = x2;
         displaymovingunit.ypos = y2;
         displaymovingunit.dx = 0 ;// -dx;
         displaymovingunit.dy = 0 ;// -dy;
         if ( CGameOptions::Instance()->movespeed )
            displaymovingunit.hgt = h2 - ( h2 - h1 ) * tick / CGameOptions::Instance()->movespeed;
         else
            displaymovingunit.hgt = h2;

         int r;
         if ( displaymovingunit.ypos & 1 )   /*  ungerade reihennummern  */
            r = vfbleftspace + fielddisthalfx + x2 * fielddistx - dx;
         else
            r = vfbleftspace + x2 * fielddistx - dx;


         int yp = vfbtopspace + y2 * fielddisty - dy;


         if ( r >= 0  &&  yp >= 0 &&  yp+unitsizey <= dispmapdata.vfbheight && r+unitsizex <= dispmapdata.vfbwidth ) {
            int d = -1;
            if ( displaymovingunit.hgt )
               d = 6 * displaymovingunit.hgt / 10 ;

            displaymovingunit.eht->paint( getActiveSurface(), SPoint(r,yp), d );

            idisplaymap.cp_buf ( touchedfields->minx, touchedfields->miny, touchedfields->maxx, touchedfields->maxy );
         }


         if ( CGameOptions::Instance()->movespeed ) {
            dx = sdx * tick / CGameOptions::Instance()->movespeed;
            dy = sdy * tick / CGameOptions::Instance()->movespeed;
         } else {
            dx = 0;
            dy = 0;
         }


         /*
         if ( dx > 0 )
            dx-=step;
         else
            if ( dx < 0 )
               dx+=step;
         if ( dy > 0 )
            dy-=step;
         else
            if ( dy < 0 )
               dy+=step;
         */
      } while ( dx!=0 || dy!=0 || displaymovingunit.hgt != h2 ); /* enddo */

      displaymovingunit.xpos = x2 + actmap->xpos;
      displaymovingunit.ypos = y2 + actmap->ypos;

      delete touchedfields;
      delete[] picbuf;

   }
   npop ( dispmapdata );
   *agmp = *hgmp;
}


tlockdispspfld :: tlockdispspfld ( void )
{
   lockdisplaymap ++;
}

tlockdispspfld :: ~tlockdispspfld ()
{
   lockdisplaymap --;
}





tdisplaywholemap :: ~tdisplaywholemap ( )
{
   if ( dsp ) {
      delete dsp;
      dsp = NULL;
   }
}

void tdisplaywholemap :: init ( int xs, int ys )
{
   tgeneraldisplaymap :: init ( xs, ys );

   dispmapdata.disp.x1 = 0;
   dispmapdata.disp.y1 = 0;
   dispmapdata.disp.x2 = xs;
   dispmapdata.disp.y2 = ys;

   int bufsizex = xs * fielddistx + 200 ;
   int bufsizey = ys * fielddisty + 200 ;

   dsp = new tvirtualdisplay ( bufsizex, bufsizey, 0 );

   vfb.parameters = *agmp;
   vfb.parameters.directscreenaccess = 1;
   dispmapdata.vfbadress = (void*) agmp->linearaddress;

}

int tdisplaywholemap :: getWidth( )
{
   return getscreenxsize() * fielddistx + 20 + 1;
}

int tdisplaywholemap :: getHeight( )
{
   return (getscreenysize() + 1 ) * fielddisty + 1;
}

void tdisplaywholemap :: cp_buf ( void )
{
   writepcx ( filename.c_str(), vfbleftspace, vfbtopspace, vfbleftspace + getWidth()-1, vfbtopspace + getHeight()-1, pal );

}


void writemaptopcx ( bool confirmation, string filename, int* width, int* height )
{

   if ( confirmation ) {
      int mb = 1024*1024;
      int bufsizex = actmap->xsize * fielddistx + 200 ;
      int bufsizey = actmap->ysize * fielddisty + 200 ;
      int bufsize = (bufsizex * bufsizey + mb - 1) / mb;
      if ( choice_dlg( "About %d MB of Buffer will be allocated !", "c~o~ntinue", "~c~ancel", bufsize) == 2)
         return;

      displaymessage ( "writing image\nplease wait",0 );
   }


   npush ( *agmp );
   npush ( actmap->xpos );
   npush ( actmap->ypos );

   actmap->xpos = 0;
   actmap->ypos = 0;

   try {
      if ( filename.empty() )
         filename = getnextfilenumname ( "map", "pcx" );

      tdisplaywholemap wm ( filename );
      wm.init ( actmap->xsize, actmap->ysize );
      wm.pnt_terrain ( );
      wm.pnt_main ( );
      wm.cp_buf (  );
      if ( width )
         *width = wm.getWidth();

      if ( height )
         *height = wm.getHeight();
   } /* endtry */
   catch ( fatalgraphicserror ) {
      displaymessage("unable to generate image", 1 );
   } /* endcatch */

   npop ( actmap->ypos );
   npop ( actmap->xpos );
   npop ( *agmp );

   if ( confirmation )
      removemessage();
}


int mousecurs = 8;
int mousescrollspeed = 6;
int lastmousemapscrolltick = 0;

const int mousehotspots[9][2] =
   { {
        8, 0
     }
     , { 15, 0 }, { 15, 8 }, { 15, 15 },
     { 8, 15 }, { 0, 15 }, { 0, 8 }, { 0, 0 },
     { 0, 0 }
   };


void checkformousescrolling ( void )
{
#if 0
   if ( isfullscreen() )
      if ( !mouseparams.x  ||  !mouseparams.y   ||  mouseparams.x >= hgmp->resolutionx-1  ||   mouseparams.y >= hgmp->resolutiony-1 ) {
         if ( mouseparams.taste == CGameOptions::Instance()->mouse.scrollbutton ) {
            if ( lastmousemapscrolltick + mousescrollspeed < ticker ) {

               int newx = actmap->xpos;
               int newy = actmap->ypos;
               switch ( mousecurs ) {
                  case 0:
                     newy-=2;
                     break;
                  case 1:
                     newy-=2;
                     newx+=1;
                     break;
                  case 2:
                     newx+=1;
                     break;
                  case 3:
                     newy+=2;
                     newx+=1;
                     break;
                  case 4:
                     newy+=2;
                     break;
                  case 5:
                     newy+=2;
                     newx-=1;
                     break;
                  case 6:
                     newx-=1;
                     break;
                  case 7:
                     newy-=2;
                     newx-=1;
                     break;
               } /* endswitch */

               if ( newx < 0 )
                  newx = 0;
               if ( newy < 0 )
                  newy = 0;
               if ( newx > actmap->xsize - idisplaymap.getscreenxsize() )
                  newx = actmap->xsize - idisplaymap.getscreenxsize();
               if ( newy > actmap->ysize - idisplaymap.getscreenysize() )
                  newy = actmap->ysize - idisplaymap.getscreenysize();

               if ( newx != actmap->xpos  || newy != actmap->ypos ) {
                  cursor.hide();
                  int cursx = cursor.posx + (actmap->xpos - newx );
                  int cursy = cursor.posy + (actmap->ypos - newy );
                  if ( cursx < 0 )
                     cursx = 0;
                  if ( cursx >= idisplaymap.getscreenxsize())
                     cursx = idisplaymap.getscreenxsize() -1 ;
                  if ( cursy < 0 )
                     cursy = 0;
                  if ( cursy >= idisplaymap.getscreenysize())
                     cursy = idisplaymap.getscreenysize() -1 ;

                  cursor.posx = cursx;
                  cursor.posy = cursy;

                  actmap->xpos = newx;
                  actmap->ypos = newy;
                  displaymap();
                  lastmousemapscrolltick = ticker;

                  cursor.show();
               }
            }
         }
      }
#endif      
}



tmousescrollproc mousescrollproc;


void tmousescrollproc :: mouseaction ( void )
{
   int pntnum = -1;

   void* newpnt = NULL;

   if ( mouseparams.x1 == 0 ) {
      if ( mouseparams.y1 == 0 )
         pntnum = 7;
      else
         if ( mouseparams.y1 + mouseparams.ysize >= hgmp->resolutiony  )
            pntnum = 5;
         else
            pntnum = 6;
   } else {
      if ( mouseparams.x1 + mouseparams.xsize >= hgmp->resolutionx ) {
         if ( mouseparams.y1 == 0 )
            pntnum = 1;
         else
            if ( mouseparams.y1 + mouseparams.ysize >= hgmp->resolutiony  )
               pntnum = 3;
            else
               pntnum = 2;
      } else {
         if ( mouseparams.y1 == 0 )
            pntnum = 0;
         else
            if ( mouseparams.y1 + mouseparams.ysize >= hgmp->resolutiony  )
               pntnum = 4;
            else
               pntnum = 8;
      }
   }
   if ( pntnum < 8 )
      newpnt = icons.pfeil2[pntnum];
   else
      newpnt = icons.mousepointer;

   mousecurs = pntnum;

   if ( mouseparams.pictpointer != newpnt )
      setnewmousepointer ( newpnt, mousehotspots[pntnum][0], mousehotspots[pntnum][1] );
}



int  MapDisplay :: displayMovingUnit ( const MapCoordinate3D& start, const MapCoordinate3D& dest, Vehicle* vehicle, int fieldnum, int totalmove, SoundStartCallback soundStart )
{
   if ( actmap->playerView < 0 )
      return 0;

   int height1 = start.getNumericalHeight();
   int height2 = dest.getNumericalHeight();
   if ( height2 == -1 )
      height2 = height1;
   else
      if ( height1== -1 )
         height1 = height2;

   pfield fld1 = actmap->getField ( start );
   int view1 = fieldVisibility ( fld1, actmap->playerView );

   pfield fld2 = actmap->getField ( dest );
   int view2 = fieldVisibility ( fld2, actmap->playerView );

   if (  (view1 >= visible_now  &&  view2 >= visible_now ) || ( vehicle->getOwner() == actmap->playerView ))
      if ( ((vehicle->height >= chschwimmend) && (vehicle->height <= chhochfliegend)) || (( view1 == visible_all) && ( view2 == visible_all )) || ( actmap->actplayer == actmap->playerView )) {
         soundStart( 1 );
         idisplaymap.movevehicle( start.x, start.y, dest.x, dest.y, vehicle, height1, height2, fieldnum, totalmove );
      }


   int result = 0;
   if (view1 >= visible_now )
      result +=1;

   if ( view2 >= visible_now )
      result +=2;

   return result;
}


void MapDisplay :: displayMap ( void )
{
   displaymap();
}

void MapDisplay :: displayPosition ( int x, int y )
{
   displaymap();
}


void MapDisplay :: resetMovement ( void )
{
   idisplaymap.resetmovement();
}

void MapDisplay :: startAction ( void )
{
   int bb = cursor.an;

   if ( bb )
      cursor.hide();
   cursorstat[cursorstatnum++] = bb;
}

void MapDisplay :: stopAction ( void )
{
   int b;
   b = cursorstat[--cursorstatnum];
   if(  cursorstatnum < 0 )
      displaymessage ( "void MapDisplay :: stopAction / underflow !", 2 );

   if ( b )
      cursor.show();

}

void MapDisplay :: updateDashboard ( void )
{
#ifdef sgmain
   updateFieldInfo();
#endif

}

void MapDisplay :: repaintDisplay ()
{
   ::repaintdisplay();
}


#ifdef sgmain
tbackgroundpict backgroundpict;

tbackgroundpict :: tbackgroundpict ( void )
{
   background = NULL;
   inited = 0;
   run = 0;
   lastpaintmode = -1;
}

void tbackgroundpict :: paintrectangleborder ( void )
{
   if ( rectangleborder.initialized ) {
      int width = 2;
      for ( int i = 0; i < 3; i++ )
         for ( int j = 0; j < width; j++ ) {
            int p = i * width + j;
            int x;
            int y;
            for ( x = rectangleborder.x1 - p; x < rectangleborder.x2 + p; x++ )
               putpixel ( x, rectangleborder.y1 - p, xlattables.a.dark2[ getpixel ( x, rectangleborder.y1-p )] );
            for ( y = rectangleborder.y1 - p; y < rectangleborder.y2 + p; y++ )
               putpixel ( rectangleborder.x2 + p, y, xlattables.a.light3[ getpixel ( rectangleborder.x2 + p, y )] );
            for ( x = rectangleborder.x2 + p; x > rectangleborder.x1 - p; x-- )
               putpixel ( x, rectangleborder.y2 + p, xlattables.a.light3[ getpixel ( x, rectangleborder.y2+p )] );
            for ( y = rectangleborder.y2 + p; y > rectangleborder.y1 - p; y-- )
               putpixel ( rectangleborder.x1 - p, y, xlattables.a.dark2[ getpixel ( rectangleborder.x1 - p, y )] );

         }
   }
}


void tbackgroundpict :: init ( int reinit )
{
   if ( !inited || reinit ) {
      /*
      int borderx1 = getmapposx ( );
      int bordery1 = getmapposy ( );
      */
      int borderx1 = 0;
      int bordery1 = 0;

      int bordery2 = bordery1 + (idisplaymap.getscreenysize() - 1) * fielddisty + fieldsizey - 1;
      int borderx2 = borderx1 + (idisplaymap.getscreenxsize() - 1 ) * fielddistx + fieldsizex + fielddisthalfx - 1;

      int height, width;

      getpicsize ( borderpicture[2], width, height );

      borderpos[0].x = borderx1 - mapborderwidth;
      borderpos[0].y = bordery1 - mapborderwidth;

      borderpos[2].x = borderx2 + mapborderwidth - ( width -1 );
      borderpos[2].y = bordery1 - mapborderwidth;


      getpicsize ( borderpicture[6], width, height );

      borderpos[1].x = borderx1 - mapborderwidth + 28;
      borderpos[1].y = bordery1 - mapborderwidth;

      borderpos[6].x = borderx1 - mapborderwidth + 28 + fielddisthalfx;
      borderpos[6].y = bordery2 + mapborderwidth - ( height-1);


      getpicsize ( borderpicture[4], width, height );

      borderpos[3].x = borderx1 - mapborderwidth;
      borderpos[3].y = bordery1 + fielddisty;

      borderpos[4].x = borderx2 + mapborderwidth - ( width - 1 );
      borderpos[4].y = bordery1 + 2 * fielddisty;


      getpicsize ( borderpicture[5], width, height );

      borderpos[5].x = borderx1 - mapborderwidth;
      borderpos[5].y = bordery2 + mapborderwidth - ( height - 1 );


      getpicsize ( borderpicture[7], width, height );

      borderpos[7].x = borderx2 + mapborderwidth - ( width- 1);
      borderpos[7].y = bordery2 + mapborderwidth - ( height - 1 );

      inited = 1;

   }
}



void tbackgroundpict :: load ( void )
{
   int w;
   
   {
      tnfilestream stream ("amatur.raw", tnstream::reading);
      for ( int i = 0; i< 7; i++ )
         stream.readrlepict ( &dashboard[i], false, &w );
#ifdef sgmain

//      ::dashboard.zoom.pic = dashboard[6];
#endif

   }
   {
      tnfilestream stream ("hxborder.raw", tnstream::reading);
      for ( int i = 0; i < 8; i++ )
         stream.readrlepict ( &borderpicture[i], false, &w );
   }
}


void tbackgroundpict :: paintborder ( int dx, int dy )
{
   paintborder ( dx, dy , 0 );
}

void tbackgroundpict :: paintborder ( int dx, int dy, int reinit )
{
   if ( lastpaintmode < 1 ) {
      paintrectangleborder (  );
      lastpaintmode = 1;
   }
}

void  tbackgroundpict :: paint ( int resavebackground )
{
   collategraphicoperations cgo;
   init();

   {
      if ( !background ) {

         char filename[100];
         sprintf( filename, "%d%d.pcx", hgmp->resolutionx, hgmp->resolutiony );
         if ( exist ( filename )) {
            tnfilestream stream ( filename, tnstream::reading );
            loadpcxxy( &stream ,0,0);

            background = new char[ imagesize ( 0, 0, agmp->resolutionx, agmp->resolutiony  )];
         } else {
            displaymessage2("generating background picture; please wait ..." );
            if ( !asc_paletteloaded )
               loadpalette();
            int x = hgmp->resolutionx;
            int y = hgmp->resolutiony;
            {
               tvirtualdisplay vdp ( agmp->resolutionx, agmp->resolutiony );
               tnfilestream stream ( "640480.pcx", tnstream::reading );
               loadpcxxy( &stream ,0,0);
               char* pic = new char[ imagesize ( 0, 0, 639, 479 )];
               getimage ( 0, 0, 639, 479, pic );

               TrueColorImage* img = zoomimage ( pic, x, y, pal, 1, 0  );
               delete pic;
               background = convertimage ( img, pal );
            }
            putimage ( 0, 0, background );
            writepcx ( filename, 0, 0, agmp->resolutionx-1, agmp->resolutiony-1, pal );

            /*
            bar ( 0, 0, agmp->resolutionx-1, agmp->resolutiony-1, greenbackgroundcol );
            found = 0;
            */
         }

         putimage ( agmp->resolutionx - ( 640 - 450), 15,  dashboard[0] );
         putimage ( agmp->resolutionx - ( 640 - 450), 211, dashboard[1] );
         putimage ( agmp->resolutionx - ( 640 - 450), agmp->resolutiony - ( 480 - 433),  dashboard[2] );
         int l_width, l_height;
         int m_width, m_height;
         int r_width, r_height;
         getpicsize ( dashboard[3], l_width, l_height );
         getpicsize ( dashboard[4], m_width, m_height );
         getpicsize ( dashboard[5], r_width, r_height );

         int lpos = 14;
         putimage ( lpos,  agmp->resolutiony - ( 480 - 442),  dashboard[3] );
         putimage ( lpos + l_width,  agmp->resolutiony - ( 480 - 442),  dashboard[4] );

         int rpos = agmp->resolutionx - ( 640 - 433) - r_width + 1;
         lpos += l_width + m_width;

         putimage ( rpos,  agmp->resolutiony - ( 480 - 442),  dashboard[5] );

         while ( rpos > lpos ) {
            rpos -= m_width;
            putimage ( rpos,  agmp->resolutiony - ( 480 - 442),  dashboard[4] );
         }

         getimage ( 0, 0, agmp->resolutionx-1, agmp->resolutiony-1, background );

      } else
         putimage ( 0, 0, background );

      lastpaintmode = 0;

      if ( actmap && actmap->xsize && !lockdisplaymap )
         paintborder( getmapposx ( ), getmapposy ( ) );
   }
   if ( resavebackground  ||  !run ) {
      gui.savebackground ( );
   }
   run++;
//   ::dashboard.repainthard = 1;

}

int tbackgroundpict :: getlastpaintmode ( void )
{
   return lastpaintmode;
}

#endif


