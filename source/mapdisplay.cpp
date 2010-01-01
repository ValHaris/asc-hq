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

// #define debugmapdisplay


#include <cmath>
#include <limits>

#include "pgeventsupplier.h"

#include "global.h"
#include "typen.h"
#include "mapdisplay.h"
#include "vehicletype.h"
#include "buildingtype.h"
#include "spfst.h"
#include "dialog.h"
#include "loaders.h"
#include "gameoptions.h"
#include "loadbi3.h"
#include "mapalgorithms.h"
#include "graphicset.h"
#include "graphics/blitter.h"
#include "graphics/drawing.h"
#include "loadpcx.h"
#include "iconrepository.h"
#include "mainscreenwidget.h"
#include "sdl/sound.h"
#include "spfst-legacy.h"

#ifndef karteneditor
 #include "dialogs/attackpanel.h"
#endif


#ifdef debugmapdisplay
#include <iostream>
#endif


MapRenderer::Icons MapRenderer::icons;

bool tempsvisible = true;


SigC::Signal0<void> lockMapdisplay;
SigC::Signal0<void> unlockMapdisplay;


class ContainerInfoLayer : public MapLayer {
      Surface& marker;
      bool hasCargo( const ContainerBase* c ) {
         for ( ContainerBase::Cargo::const_iterator i = c->getCargo().begin(); i != c->getCargo().end(); ++i )
            if ( *i )
               return true;
         return false;
      };
      
      bool hasOwnCargo( const ContainerBase* c, int viewingPlayer ) {
         for ( ContainerBase::Cargo::const_iterator i = c->getCargo().begin(); i != c->getCargo().end(); ++i )
            if ( *i ) {
               if ( (*i)->getOwner() == viewingPlayer )
                  return true;
               else
                  if ( hasOwnCargo( *i, viewingPlayer ))
                     return true;
            }
         return false;
      };
    public: 
      ContainerInfoLayer() : marker( IconRepository::getIcon("fieldcontainermarker.png") ) {};
         
      bool onLayer( int layer ) { return layer == 17; };
      void paintSingleField( const MapRenderer::FieldRenderInfo& fieldInfo,  int layer, const SPoint& pos );
};

void ContainerInfoLayer::paintSingleField( const MapRenderer::FieldRenderInfo& fieldInfo,  int layer, const SPoint& pos )
{
   if ( fieldInfo.visibility >= visible_ago) {
      if ( fieldInfo.fld->vehicle || (fieldInfo.fld->building && fieldInfo.fld->bdt.test(cbbuildingentry) )) {
         ContainerBase* c = fieldInfo.fld->getContainer();
         if ( hasCargo(c) ) {
            if ( c->getOwner() == fieldInfo.playerView  ) 
               fieldInfo.surface.Blit( marker, pos );
            else
               if ( hasOwnCargo(c, fieldInfo.playerView  ))
                  fieldInfo.surface.Blit( marker, pos );
         }
      }
   }
}


 
class ResourceGraphLayer : public MapLayer {
      void paintBar( const MapRenderer::FieldRenderInfo& fieldInfo, const SPoint& pos, int row, int amount, int color ) {
         /*
         int length = amount / 10;
         int maxlength = 255/10;
         if ( amount ) 
            paintFilledRectangle<4>( fieldInfo.surface, SPoint( pos.x + 10, pos.y + 2 + row*12), length, 5, ColorMerger_ColoredOverwrite<4>( color ));
         if ( length < maxlength )
            paintFilledRectangle<4>( fieldInfo.surface, SPoint( pos.x + 10 + length, pos.y + 2 + row*12), maxlength-length, 5, ColorMerger_ColoredOverwrite<4>( 0x888888 ));
         */

         int length = amount * 28 / 255;
         int maxlength = 28;
         if ( amount ) 
            paintFilledRectangle<4>( fieldInfo.surface, SPoint( pos.x + 10, pos.y + 12 + row*15), length, 8, ColorMerger_ColoredOverwrite<4>( color ));
         if ( length < maxlength )
            paintFilledRectangle<4>( fieldInfo.surface, SPoint( pos.x + 10 + length, pos.y + 12 + row*15), maxlength-length, 8, ColorMerger_ColoredOverwrite<4>( 0 ));
           
      };
    public: 
      bool onLayer( int layer ) { return layer == 17; };
      void paintSingleField( const MapRenderer::FieldRenderInfo& fieldInfo,  int layer, const SPoint& pos );
};

void ResourceGraphLayer::paintSingleField( const MapRenderer::FieldRenderInfo& fieldInfo,  int layer, const SPoint& pos )
{
#ifndef karteneditor
   if ( fieldInfo.visibility >= visible_ago) {
      bool visible = false;

      if ( fieldInfo.playerView == -1 )
         visible = true;

      if ( fieldInfo.playerView >= 0 )
         if ( fieldInfo.fld->resourceview && (fieldInfo.fld->resourceview->visible & ( 1 << fieldInfo.playerView) ) )
            visible = true;

      if ( visible ) {
         if ( fieldInfo.playerView>=0 && fieldInfo.gamemap->getPlayer(fieldInfo.playerView).stat == Player::supervisor ) {
            paintBar( fieldInfo, pos, 0, fieldInfo.fld->material, Resources::materialColor );
            paintBar( fieldInfo, pos, 1, fieldInfo.fld->fuel,     Resources::fuelColor );
         } else {
            paintBar( fieldInfo, pos, 0, fieldInfo.fld->resourceview->materialvisible[max(fieldInfo.playerView,0)], Resources::materialColor );
            paintBar( fieldInfo, pos, 1, fieldInfo.fld->resourceview->fuelvisible[max(fieldInfo.playerView,0)], Resources::fuelColor );
         }
      }
   }
#else
   paintBar( fieldInfo, pos, 0, fieldInfo.fld->material, Resources::materialColor );
   paintBar( fieldInfo, pos, 1, fieldInfo.fld->fuel, Resources::fuelColor );
#endif
}




class PipeLayer : public MapLayer {
      ObjectType* buried_pipeline;
      ObjectType* pipeline;
      bool isPipe( const ContainerBase* c ) {
         for ( ContainerBase::Cargo::const_iterator i = c->getCargo().begin(); i != c->getCargo().end(); ++i )
            if ( *i )
               return true;
         return false;
      };

      bool isObjectPipeline( const ObjectType* obj )
      {
         if ( !obj )
            return false;
         return (obj->displayMethod <= 1 ) && obj->fieldModification[0].terrain_or.test( cbpipeline );
      }

    public: 
      PipeLayer() : buried_pipeline( NULL ), pipeline ( NULL )
      {
         pipeline = objectTypeRepository.getObject_byID( 3 );
         if ( !isObjectPipeline( pipeline )) 
            pipeline = NULL;

         for ( int i = 0; i < objectTypeRepository.getNum(); ++i ) {
            ObjectType* obj = objectTypeRepository.getObject_byPos( i );
            if (obj->displayMethod == 1 && obj->fieldModification[0].terrain_or.test( cbpipeline )) {
               buried_pipeline = obj;
            }
            if (obj->displayMethod == 0 && obj->fieldModification[0].terrain_or.test( cbpipeline ) && !pipeline) {
               pipeline = obj;
            }
         }
      }
    
      bool onLayer( int layer ) { return layer == 17; };
      void paintSingleField( const MapRenderer::FieldRenderInfo& fieldInfo,  int layer, const SPoint& pos );
};

void PipeLayer::paintSingleField( const MapRenderer::FieldRenderInfo& fieldInfo,  int layer, const SPoint& pos )
{
   if ( !pipeline )
      return;
      
   if ( fieldInfo.visibility > visible_ago) {
      if ( fieldInfo.fld->building ) {
         pipeline->display( fieldInfo.surface, pos, 63, 0 );
      } else {
         if ( fieldInfo.fld->bdt.test( cbpipeline )) {
            Object* o = fieldInfo.fld->checkForObject( buried_pipeline );
            if ( o )
               pipeline->display( fieldInfo.surface, pos, o->dir, 0 );
            else {
               bool objfound = false;
               for ( MapField::ObjectContainer::iterator i = fieldInfo.fld->objects.begin(); i != fieldInfo.fld->objects.end(); ++i )
                  if ( i->typ->fieldModification[0].terrain_or.test( cbpipeline ) ) {
                     pipeline->display( fieldInfo.surface, pos, i->dir, 0 );
                     objfound = true;
                     break;
                  }   

               if ( !objfound )
                  pipeline->display( fieldInfo.surface, pos, 63, 0 );
                     
            }
         }
      }   
   }
}



class ReactionFireLayer : public MapLayer {
      Surface& image;
   public: 
      ReactionFireLayer() : image ( IconRepository::getIcon("rf-icon.png")) {} ;
      bool onLayer( int layer ) { return layer == 17; };
      void paintSingleField( const MapRenderer::FieldRenderInfo& fieldInfo,  int layer, const SPoint& pos );
};

void ReactionFireLayer::paintSingleField( const MapRenderer::FieldRenderInfo& fieldInfo,  int layer, const SPoint& pos )
{
   if ( fieldInfo.visibility > visible_ago) {
      if ( fieldInfo.fld->vehicle && fieldInfo.fld->vehicle->reactionfire.getStatus() != Vehicle::ReactionFire::off && fieldInfo.fld->vehicle->getOwner() == fieldInfo.playerView ) {
         MegaBlitter<colorDepth,colorDepth,ColorTransform_None,ColorMerger_AlphaMerge> blitter;
         blitter.blit( image, fieldInfo.surface, pos);
      }   
   }
}



class WeaponRange : public SearchFields
{
   public:
      int run ( const Vehicle* veh );
      void testfield ( const MapCoordinate& mc )
      {
         gamemap->getField( mc )->tempw = 1;
      };
      WeaponRange ( GameMap* _gamemap ) : SearchFields ( _gamemap )
      {}
      ;
};

int  WeaponRange :: run ( const Vehicle* veh )
{
   int found = 0;
   if ( fieldvisiblenow ( getfield ( veh->xpos, veh->ypos )))
      for ( int i = 0; i < veh->typ->weapons.count; i++ ) {
         if ( veh->typ->weapons.weapon[i].shootable() ) {
            initsearch ( veh->getPosition(), veh->typ->weapons.weapon[i].maxdistance/minmalq, (veh->typ->weapons.weapon[i].mindistance+maxmalq-1)/maxmalq );
            startsearch();
            found++;
         }
      }
   return found;
}


 


MapRenderer::ViewPort::ViewPort( int x1, int y1, int x2, int y2 ) 
{ 
   this->x1 = x1; 
   this->y1 = y1; 
   this->x2 = x2; 
   this->y2 = y2; 
};

MapRenderer::ViewPort::ViewPort()
{
};

MapRenderer :: MapRenderer()  
{ 
   readData(); 
};


void MapRenderer::readData()
{
   if ( !icons.mapBackground.valid() ) {
      icons.mapBackground = IconRepository::getIcon("mapbkgr.raw");
      icons.notVisible    = IconRepository::getIcon("hexinvis.raw");
      icons.markField     = IconRepository::getIcon("markedfield.png");
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
   return getFirstBit(height) * 2 + 2;
}


void MapRenderer::paintSingleField( const MapRenderer::FieldRenderInfo& fieldInfo,  int layer, const SPoint& pos )
{

   int binaryUnitHeight = 0;
   if ( layer > 1 ) 
      if ( !(layer & 1 ))
         binaryUnitHeight = 1 << (( layer-2)/2);
   
   MapField* fld = fieldInfo.fld;

   if ( layer == 0 && fieldInfo.visibility >= visible_ago )
      fld->typ->paint ( fieldInfo.surface, pos );


   if ( fieldInfo.visibility > visible_ago ) {

      /* display buildings */
      if ( fld->building  &&  (fld->building->typ->height & binaryUnitHeight) && fld->building->visible )
         if ((fieldInfo.visibility == visible_all) || (fld->building->typ->height >= chschwimmend) || ( fld->building->getOwner() == fieldInfo.playerView ))
            fld->building->paintSingleField( fieldInfo.surface, pos, fld->building->getLocalCoordinate( fieldInfo.pos ));


      /* display units */
      if ( fld->vehicle  &&  (fld->vehicle->height == binaryUnitHeight))
         if ( ( fld->vehicle->getOwner() == fieldInfo.playerView ) || (fieldInfo.visibility == visible_all) || ((fld->vehicle->height >= chschwimmend) && (fld->vehicle->height <= chhochfliegend)))
            fld->vehicle->paint( fieldInfo.surface, pos );

   }

   // display objects
   if ( layer & 1 )
      for ( MapField::ObjectContainer::iterator o = fld->objects.begin(); o != fld->objects.end(); o++ ) {
         int h = o->typ->imageHeight;
         if ( fieldInfo.visibility > visible_ago || (o->typ->visibleago && fieldInfo.visibility >= visible_ago ))
            if (  h >= ((layer-1)/2)*30 && h < (layer-1)/2*30+30 )
               o->display ( fieldInfo.surface, pos, fld->getWeather() );
      }




   if ( fieldInfo.visibility > visible_ago ) {
      /* display mines */
      
      if ( fieldInfo.visibility == visible_all )
         if ( !fld->mines.empty() && layer == 7 ) {
            for ( MapField::MineContainer::const_iterator i = fld->mines.begin(); i != fld->mines.end(); ++i )
               i->paint( fieldInfo.surface, pos );
         }
     


      /* display marked fields */

      if ( layer == 18 ) {
         if ( fld->a.temp && tempsvisible )
            fieldInfo.surface.Blit( icons.markField, pos );
         else
            if ( fld->a.temp2 && tempsvisible )
               fieldInfo.surface.Blit( icons.markFieldDark, pos );
      }


   } else {
      if (fieldInfo.visibility == visible_ago ) {
         if ( fld->building  &&  (fld->building->typ->height & binaryUnitHeight) && fld->building->visible )
            if ((fieldInfo.visibility == visible_all) || (fld->building->typ->height >= chschwimmend) || ( fld->building->getOwner() == fieldInfo.playerView ))
               fld->building->paintSingleField( fieldInfo.surface, pos, fld->building->getLocalCoordinate( fieldInfo.pos ));

      }
   }


   // display view obstructions
   if ( layer == 18 ) {
      if ( fieldInfo.visibility == visible_ago) {
         MegaBlitter<1,colorDepth,ColorTransform_None,ColorMerger_AlphaShadow> blitter;
         // PG_Point pnt = ClientToScreen( 0,0 );
         blitter.blit( icons.notVisible, fieldInfo.surface, pos);
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
         if ( fieldInfo.visibility == visible_not) {
            fieldInfo.surface.Blit( icons.notVisible, pos );
            /*
                            if ( ( fld->a.temp || fld->a.temp2 ) && tempsvisible )
                                  putspriteimage(  r, yp, cursor.markfield);
            */

         }

   }
   
   for ( LayerRenderer::iterator i = layerRenderer.begin(); i != layerRenderer.end(); ++i )
      if ( (*i)->isActive() && (*i)->onLayer(layer))
         (*i)->paintSingleField( fieldInfo , layer, pos );

}


void MapRenderer::paintBackground( Surface& surf, const ViewPort& viewPort )
{
   for (int y= viewPort.y1; y < viewPort.y2; ++y )
      for ( int x=viewPort.x1; x < viewPort.x2; ++x ) 
         paintBackgroundField( surf, getFieldPos(x,y) );
}

void MapRenderer::paintBackgroundField( Surface& surf, SPoint pos )
{
   surf.Blit( icons.mapBackground, pos );
}



void MapRenderer::paintTerrain( Surface& surf, GameMap* actmap, int playerView, const ViewPort& viewPort, const MapCoordinate& offset )
{
   FieldRenderInfo fieldRenderInfo( surf, actmap );
   fieldRenderInfo.playerView = playerView;

   GraphicSetManager::Instance().setActive ( actmap->graphicset );

   for (int pass = 0; pass <= 18 ;pass++ ) {
      for (int y= viewPort.y1; y < viewPort.y2; ++y )
         for ( int x=viewPort.x1; x < viewPort.x2; ++x ) {
            fieldRenderInfo.pos = MapCoordinate( offset.x + x, offset.y + y ); 
            fieldRenderInfo.fld = actmap->getField ( fieldRenderInfo.pos );
            SPoint pos = getFieldPos(x,y);
            if ( fieldRenderInfo.fld ) {
               fieldRenderInfo.visibility = fieldVisibility ( fieldRenderInfo.fld, playerView );
               paintSingleField( fieldRenderInfo, pass, pos );
            } else
               if ( pass == 0 )
                  paintBackgroundField( surf, pos );

         }
      additionalItemDisplayHook( surf, pass );
   }
   
}



void benchMapDisplay()
{/*
   int t = ticker;
   for ( int i = 0; i < 20; ++i )
      repaintMap();

   int t2 = ticker;
   
   for ( int i = 0; i< 20; ++i)
      theGlobalMapDisplay->Redraw();
   
   int t3 = ticker;

   ASCString s;
   s.format("update map: %d \nupdate widget: %d \n%f fps", t2-t,t3-t2, 20.0 / float(t3-t) * 100 );
   displaymessage(s, 1 );
   */
}


MapDisplayPG* MapDisplayPG::theMapDisplay = NULL;
MapDisplayPG* theGlobalMapDisplay = NULL;

MapDisplayPG::MapDisplayPG ( MainScreenWidget *parent, const PG_Rect r )
      : PG_Widget ( parent, r, false ) ,
      zoom(-1),
      surface(NULL),
      lastDisplayedMap(NULL),
      offset(0,0),
      dirty(Map),
      additionalUnit(NULL),
      disableKeyboardCursorMovement(false),
      signalPrio(0),
      cursor(this),
      lock(0)
{
   SetDirtyUpdate(true);
   dataLoaderTicker();
   
   readData();
   
   dataLoaderTicker();
   
   setNewZoom( CGameOptions::Instance()->mapzoom );

   repaintMap.connect( SigC::slot( *this, &MapDisplayPG::updateWidget ));

   PG_Application::GetApp()->sigKeyDown.connect( SigC::slot( *this, &MapDisplayPG::keyboardHandler ));

   SetName( "THEMapDisplay");

   dataLoaderTicker();
   SDL_Surface* ws = GetWidgetSurface ();
   if ( ws ) {
      Surface s = Surface::Wrap( ws );
      s.assignDefaultPalette();
   }
   
   MapRenderer::additionalItemDisplayHook.connect( SigC::slot( *this, &MapDisplayPG::displayAddons ));
   
   upperLeftSourceBlitCorner = SPoint( getFieldPosX(0,0), getFieldPosY(0,0));

   theMapDisplay = this;
   theGlobalMapDisplay = this;
   dataLoaderTicker();
   
   addMapLayer( new ResourceGraphLayer(), "resources" );
   addMapLayer( new ContainerInfoLayer(), "container" );
   addMapLayer( new PipeLayer()         , "pipes" );
   addMapLayer( new ReactionFireLayer() , "reactionfire" );
   
   parent->lockOptionsChanged.connect( SigC::slot( *this, &MapDisplayPG::lockOptionsChanged ));
   GameMap::sigMapDeletion.connect( SigC::slot( *this, &MapDisplayPG::sigMapDeleted ));
}



void MapDisplayPG::lockOptionsChanged( int options )
{
   if ( options & MainScreenWidget::LockOptions::MapControl )  
      EnableReceiver(false);
   else  
      EnableReceiver(true);
}

void MapDisplayPG::sigMapDeleted( GameMap& deletedMap )
{
   if ( &deletedMap == lastDisplayedMap ) { 
      paintBackground();
      Update();
   }
}


MapDisplayPG::~MapDisplayPG ()
{
   if ( surface ) {
      delete surface;
      surface = NULL;
   }
}


MapDisplayPG::Icons MapDisplayPG::icons;


void MapDisplayPG::readData()
{
   if ( !icons.cursor.valid() ) {
      icons.cursor = IconRepository::getIcon( "curshex.png" );
      icons.fieldShape = IconRepository::getIcon("hexinvis.raw");
   }
}


void MapDisplayPG::setNewZoom( int zoom )
{
   if ( zoom > 100 )
      zoom = 100;
   if ( zoom < 20 )
      zoom = 20;

   if ( zoom == this->zoom )
      return;
   
   this->zoom = zoom;

   field.numx = int( ceil(float(Width()) * 100  / zoom / fielddistx) );
   field.numy = int( ceil(float(Height()) * 100 / zoom / fielddisty) );

   field.viewPort.x1 = -1;
   field.viewPort.y1 = -1;
   field.viewPort.x2 = field.numx + 1;
   field.viewPort.y2 = field.numy + 1;

   if ( field.numy & 1 )
      field.numy += 1;

   delete surface;
   surface = new Surface( Surface::createSurface ( field.numx * fielddistx + 2 * surfaceBorder, (field.numy - 1) * fielddisty + fieldysize +  2 * surfaceBorder, colorDepth*8 ));

   dirty = Map;
   if ( zoom != CGameOptions::Instance()->mapzoom ) {
      CGameOptions::Instance()->mapzoom = zoom;
      CGameOptions::Instance()->setChanged();
   }
   newZoom( zoom );
}


void MapDisplayPG::fillSurface( int playerView )
{
   checkViewPosition( offset );
   if ( !lock ) {
      paintTerrain( *surface, actmap, playerView, field.viewPort, offset );
      lastDisplayedMap = actmap;
   }
   else
      paintBackground();
   dirty = Curs;
}

void MapDisplayPG::paintBackground( )
{
   MapRenderer::paintBackground( *surface, field.viewPort );
   lastDisplayedMap = NULL;
}



void MapDisplayPG::checkViewPosition( MapCoordinate& offset )
{
   if ( offset.x + field.numx >= actmap->xsize +1 )
      offset.x = max(0,actmap->xsize - field.numx +1 );

   if ( offset.y + field.numy >= actmap->ysize +4)
      offset.y = max(0,actmap->ysize - field.numy +4);

   if ( offset.y & 1 )
      offset.y -= 1;

   if ( offset.x < 0 )
      offset.x = 0;

   if ( offset.y < 0 )
      offset.y = 0;
}




template<int pixelSize>
class PixSel : public SourcePixelSelector_CacheZoom<pixelSize, SourcePixelSelector_DirectRectangle<pixelSize> >
{}
;


void MapDisplayPG::updateMap(bool force )
{
   if ( !actmap )
      return;
      
   if ( dirty > Curs || force ) 
      fillSurface( actmap->getPlayerView() );
}

void MapDisplayPG::updateWidget()
{
   updateMap(true);
   Update(true);
}



void MapDisplayPG::blitInternalSurface( SDL_Surface* dest, const SPoint& pnt, const PG_Rect& dstClip )
{
   if ( zoom != 100 ) {
      float fzoom = float(zoom) / 100.0;
      MegaBlitter<colorDepth,colorDepth,ColorTransform_None,ColorMerger_AlphaOverwrite,PixSel,TargetPixelSelector_Rect> blitter;
      blitter.setZoom( fzoom );
      blitter.initSource( *surface );

      // SourcePixelSelector
      blitter.setSrcRectangle( upperLeftSourceBlitCorner, int(float(Width()) / fzoom), int(float(Height()) / fzoom));

      PG_Rect clip= dstClip.IntersectRect( dest->clip_rect );
      blitter.setTargetRect( clip );
      
      Surface s = Surface::Wrap( dest );
      blitter.blit( *surface, s, SPoint(pnt.x, pnt.y ));
   } else {
      MegaBlitter<colorDepth,colorDepth,ColorTransform_None,ColorMerger_AlphaOverwrite,SourcePixelSelector_DirectRectangle,TargetPixelSelector_Rect> blitter;
      blitter.initSource( *surface );
      
      // SourcePixelSelector
      blitter.setSrcRectangle( upperLeftSourceBlitCorner, Width(), Height() );
      
      PG_Rect clip= dstClip.IntersectRect( dest->clip_rect );
      blitter.setTargetRect( clip );
      
      Surface s = Surface::Wrap( dest );
      blitter.blit( *surface, s, SPoint(pnt.x, pnt.y ));
   }   
   
}


void MapDisplayPG::displayCursor()
{
   displayCursor( *this );
}

void MapDisplayPG::displayCursor( const PG_Rect& dst )
{
   if ( !actmap )
      return;

   if ( dst.w <= 0 || dst.h <= 0 )
      return;
      
   int x = cursor.pos().x - offset.x;
   int y = cursor.pos().y  - offset.y;
   if( x >= field.viewPort.x1 && x < field.viewPort.x2 && y >= field.viewPort.y1 && y < field.viewPort.y2 && x >= 0 && y >= 0 && x < actmap->xsize && y < actmap->ysize ) {
      // surface->Blit( icons.cursor, getFieldPos(x,y));
      MegaBlitter<colorDepth,colorDepth,ColorTransform_None,ColorMerger_AlphaOverwrite,SourcePixelSelector_DirectZoom,TargetPixelSelector_Rect> blitter;
      blitter.setZoom( float(zoom) / 100.0 );

      PG_Rect clip= dst.IntersectRect( PG_Application::GetScreen()->clip_rect );

      if ( clip.w && clip.h ) {
         blitter.setTargetRect ( clip );
   
         Surface s = Surface::Wrap( PG_Application::GetScreen() );
         SPoint pos = widget2screen ( internal2widget( mapViewPos2internalPos( MapCoordinate(x,y))));
         blitter.blit( icons.cursor, s, pos );
      }
   }
}


void MapDisplayPG::UpdateRect( const PG_Rect& rect )
{
   SPoint p = widget2screen (  SPoint( rect.x, rect.y));
   PG_Rect sc ( p.x, p.y, rect.w, rect.h );
   sc = sc.IntersectRect( *this );
   eventBlit( GetWidgetSurface (), rect, sc );
   if ( sc.w && sc.h )
      PG_Application::UpdateRect(PG_Application::GetScreen(), sc.x, sc.y, sc.w, sc.h );
}


void MapDisplayPG::eventBlit(SDL_Surface* srf, const PG_Rect& src, const PG_Rect& dst)
{
   if ( !GetWidgetSurface ()) {
      if ( dirty > Nothing )
         updateMap();

      PG_Point pnt = ClientToScreen( 0,0 );
      blitInternalSurface( PG_Application::GetScreen(), SPoint(pnt.x,pnt.y), dst );
   } else {
      PG_Widget::eventBlit(srf,src,dst);
   }

   if ( !cursor.invisible )
      displayCursor( dst );


}


SPoint MapDisplayPG::mapGlobalPos2internalPos ( const MapCoordinate& pos )
{
   return getFieldPos( pos.x - offset.x, pos.y - offset.y );
}

SPoint MapDisplayPG::mapViewPos2internalPos ( const MapCoordinate& pos )
{
   return getFieldPos( pos.x, pos.y );
}


SPoint MapDisplayPG::internal2widget( const SPoint& pos )
{
   return SPoint( int(ceil(float(pos.x - surfaceBorder) * zoom / 100.0)), int(ceil(float(pos.y - surfaceBorder) * zoom / 100.0)));
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
   int x = int( float(pos.x) * 100 / zoom ) + surfaceBorder;
   int y = int( float(pos.y) * 100 / zoom ) + surfaceBorder;

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

MapCoordinate MapDisplayPG::upperLeftCorner()
{
   return offset;	
}

MapCoordinate MapDisplayPG::lowerRightCorner()
{
   return MapCoordinate( offset.x + field.numx , offset.y + field.numy );
}


bool MapDisplayPG::centerOnField( const MapCoordinate& mc )
{
   if ( !(mc.valid() && mc.x < actmap->xsize && mc.y < actmap->ysize ))
      return false;
      
   MapCoordinate newpos ( mc.x - field.numx / 2,  mc.y - field.numy / 2 );

   checkViewPosition( newpos );

   if ( newpos != offset  ) {
      offset = newpos;
      dirty = Map;
      Redraw();
      viewChanged();
   }
   return true;

}

void MapDisplayPG::redrawMapAtCursor ( const MapCoordinate& oldpos )
{
   if ( oldpos.valid() ) {
      SPoint p = internal2widget( mapGlobalPos2internalPos( oldpos ));
      UpdateRect( PG_Rect( p.x, p.y, fieldsizex, fieldsizey ) );
   }
         
   SPoint p = internal2widget( mapGlobalPos2internalPos( cursor.pos() ));
   UpdateRect( PG_Rect( p.x, p.y, fieldsizex, fieldsizey ) );
}


int MapDisplayPG::setSignalPriority( int priority )
{
   int old = signalPrio;
   signalPrio = priority;
   return old;
}

void filterQueuedZoomEvents()
{
   SDL_Event event;
   while ( PG_Application::GetEventSupplier()->PeepEvent(&event) && (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP) && (event.button.button == CGameOptions::Instance()->mouse.zoomoutbutton || event.button.button == CGameOptions::Instance()->mouse.zoominbutton ))
      PG_Application::GetEventSupplier()->PollEvent ( &event );
}

bool MapDisplayPG::eventMouseButtonDown (const SDL_MouseButtonEvent *button)
{
   MapCoordinate mc = screenPos2mapPos( SPoint(button->x, button->y));

   if ( button->type == SDL_MOUSEBUTTONDOWN && button->button == CGameOptions::Instance()->mouse.zoomoutbutton ) {
      changeZoom( 10 );
   
      MapCoordinate newpos = screenPos2mapPos( SPoint(button->x, button->y));
      MapCoordinate newOffset ( offset.x - ( newpos.x - mc.x ), offset.y - ( newpos.y - mc.y ));
      checkViewPosition( newOffset );
      offset = newOffset;

      viewChanged();
      repaintMap();
      filterQueuedZoomEvents();
      return true;
   }

   if ( button->type == SDL_MOUSEBUTTONDOWN && button->button == CGameOptions::Instance()->mouse.zoominbutton ) {
      changeZoom( -10 );

      MapCoordinate newpos = screenPos2mapPos( SPoint(button->x, button->y));
      MapCoordinate newOffset ( offset.x - ( newpos.x - mc.x ), offset.y - ( newpos.y - mc.y ));
      checkViewPosition( newOffset );
      offset = newOffset;

      viewChanged();
      repaintMap();
      filterQueuedZoomEvents();
      return true;
   }


   if ( !(mc.valid() && mc.x < actmap->xsize && mc.y < actmap->ysize ))
      return false;

   if ( button->type == SDL_MOUSEBUTTONDOWN && button->button == CGameOptions::Instance()->mouse.fieldmarkbutton ) {
      MapCoordinate oldpos = cursor.pos();
      bool changed = cursor.pos() != mc;
      cursor.pos() = mc;
      cursor.invisible = 0;
      dirty = Curs;

      updateFieldInfo();

      if ( changed )
         redrawMapAtCursor( oldpos );

      mouseButtonOnField( mc, SPoint(button->x, button->y), changed, button->button, signalPrio );
      return true;
      
   }

   if ( button->type == SDL_MOUSEBUTTONDOWN && button->button == CGameOptions::Instance()->mouse.centerbutton ) {
      return centerOnField( mc );
   }

   mouseButtonOnField( mc, SPoint(button->x, button->y), false, button->button, signalPrio );
   
   return false;
}

bool MapDisplayPG::eventMouseMotion (const SDL_MouseMotionEvent *button)
{
   MapCoordinate mc = screenPos2mapPos( SPoint(button->x, button->y));
   if ( !(mc.valid() && mc.x < actmap->xsize && mc.y < actmap->ysize ))
      return false;
      
   if ( button->type == SDL_MOUSEMOTION && (button->state == SDL_BUTTON( CGameOptions::Instance()->mouse.fieldmarkbutton) )) {
      bool changed = cursor.pos() != mc;
      if ( changed ) {
         MapCoordinate oldpos = cursor.pos();
         cursor.pos() = mc;
         cursor.invisible = 0;
         dirty = Curs;

         redrawMapAtCursor( oldpos );
         cursorMoved();

         mouseDraggedToField( mc, SPoint(button->x, button->y), changed, signalPrio );
   
         return true;
     }
   }
   return false;
}


bool MapDisplayPG::eventMouseButtonUp (const SDL_MouseButtonEvent *button)
{
   return false;
}


bool MapDisplayPG::fieldCompletelyInViewX( const MapCoordinate& pos )
{
   SPoint internal = mapGlobalPos2internalPos( pos);
   SPoint s = widget2screen( internal2widget( internal ));
   SPoint s2 = widget2screen( internal2widget( internal + SPoint(fieldsizex,fieldsizey) ));
   return s.x >= my_xpos && s2.x < my_xpos + my_width;
}

bool MapDisplayPG::fieldCompletelyInViewY( const MapCoordinate& pos )
{
   SPoint internal = mapGlobalPos2internalPos( pos);
   SPoint s = widget2screen( internal2widget( internal ));
   SPoint s2 = widget2screen( internal2widget( internal + SPoint(fieldsizex,fieldsizey) ));
   return s.y >= my_ypos && s2.y < my_ypos + my_height;
}

bool MapDisplayPG::fieldCompletelyInView( const MapCoordinate& pos )
{
   // for performance-reasons we don't call the above two methods
   SPoint internal = mapGlobalPos2internalPos( pos);
   SPoint s = widget2screen( internal2widget( internal ));
   SPoint s2 = widget2screen( internal2widget( internal + SPoint(fieldsizex,fieldsizey) ));
   return (s.y >= my_ypos && s2.y < my_ypos + my_height) && (s.x >= my_xpos && s2.x < my_xpos + my_width);

}


bool MapDisplayPG::fieldInView(const MapCoordinate& mc )
{
   if ( mc.x < offset.x || mc.y < offset.y || mc.x >= offset.x + field.numx || mc.y >= offset.y +  field.numy || !fieldCompletelyInView(mc) )
      return false;
   else
      return true;
}



Surface MapDisplayPG::createMovementBufferSurface()
{
   Surface s = Surface::createSurface( 2*surfaceBorder + effectiveMovementSurfaceWidth, 2*surfaceBorder + effectiveMovementSurfaceHeight, 8*colorDepth, 0x00ffffff ) ;
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


template<int pixelsize>
class SourcePixelSelector_DirectSubRectangle
{
   typedef typename PixelSize2Type<pixelsize>::PixelType PixelType;
   int x,y,x1,y1;
   int w,h;
   PG_Rect innerRect;
   int outerwidth;

   const PixelType* pointer;
   const PixelType* startPointer;
   int pitch;
   int linelength;
   const Surface* surface;
   protected:
      SourcePixelSelector_DirectSubRectangle() : x(0),y(0),x1(0),y1(0),w(0),h(0),outerwidth(0),pointer(NULL), surface(NULL)
      {}
      ;

      int getWidth()
      {
         return min(w, surface->w() -x1 );
      };
      int getHeight()
      {
         return min(h, surface->h()-y1 );
      };


      void init ( const Surface& srv )
      {
         surface = &srv;
         startPointer = pointer = (const PixelType*)(srv.pixels());
         linelength = srv.pitch()/sizeof(PixelType);
         pitch = linelength - w -1  ;
         y = y1;
         x = x1;
         pointer += x1 + y1 * linelength;
         outerwidth = getWidth();
      };

      PixelType getPixel(int x, int y)
      {
         x += x1;
         y += y1;
         if ( x >= 0 && y >= 0 && x < surface->w() && y < surface->h() )
            return surface->GetPixel(SPoint(x,y));
         else
            return surface->GetPixelFormat().colorkey();
      };


      PixelType nextPixel()
      {
         ++x;
         return *(pointer++);
      };


      void skipWholeLine()
      {
         pointer += linelength;
         ++y;
      };

      void skipPixels( int pixNum )
      {
         pointer += pixNum;
         x += pixNum;
      };

      int getSourcePixelSkip()
      {
         if ( y < innerRect.y )
            return outerwidth - (x - x1);
         else
            if ( y >= innerRect.y + innerRect.h )
               return outerwidth - (x - x1);
            else
               if ( x < innerRect.x )
                  return innerRect.x - x;
               else
                  if ( x >= innerRect.x + innerRect.w )
                     return outerwidth - (x - x1);
                  else
                     return 0;
       
      };
      
      void nextLine()
      {
         pointer = startPointer + x1 + (y++) * linelength;
         x = x1;
      };

   public:
      void setSrcRectangle( SPoint pos, int width, int height )
      {
         x1 = pos.x;
         y1 = pos.y;
         w = width;
         h = height;
      };
      void setInnerSrcRectangle( const PG_Rect& rect )
      {
         innerRect = rect;
      };

};

template<int pixelSize>
class MovePixSel : public SourcePixelSelector_CacheZoom<pixelSize, SourcePixelSelector_DirectSubRectangle<pixelSize> >
{}
;


void MapDisplayPG::displayMovementStep( Movement& movement, int percentage  )
{
#ifdef debugmapdisplay
   surface->Fill( 0xff00ff00);
#endif

   FieldRenderInfo fieldRenderInfo( *surface, movement.veh->getMap() );
   fieldRenderInfo.playerView = movement.playerView;
   for (int pass = 0; pass <= 18 ;pass++ ) {
      for ( int i = 0; i < movement.actualFieldNum; ++i ) {
         SPoint pos = movement.touchedFields[i].surfPos;
         fieldRenderInfo.pos = movement.touchedFields[i].mapPos;
         fieldRenderInfo.fld = movement.actmap->getField ( fieldRenderInfo.pos );
         if ( fieldRenderInfo.fld ) {
            fieldRenderInfo.visibility = fieldVisibility ( fieldRenderInfo.fld, fieldRenderInfo.playerView );

            paintSingleField( fieldRenderInfo, pass, pos );
         }
      }
      
      if ( pass >= 2 && pass < 18 )
         if ( !(pass & 1 ))
            if ( movement.veh->height & (1 << (( pass-2)/2))) {
               SPoint pos;
               pos.x = movement.from.x + (movement.to.x - movement.from.x) * percentage/100;
               pos.y = movement.from.y + (movement.to.y - movement.from.y) * percentage/100;
               int shadow = 0;
               if ( movement.fromShadow >= 0 && movement.toShadow >= 0 )
                  shadow = movement.fromShadow + (movement.toShadow - movement.fromShadow) * percentage/100;
               movement.veh->paint( *surface, pos, false, shadow );
            }
   }

#ifdef debugmapdisplay
   MegaBlitter<colorDepth,colorDepth,ColorTransform_None,ColorMerger_AlphaOverwrite,SourcePixelSelector_Plain,TargetPixelSelector_Valid> alphaBlitter;
   alphaBlitter.blit( *movement.mask, *surface, movement.maskPosition);
#endif
   
   PG_Rect targetArea  ( widget2screen( SPoint( 0, 0 )).x, widget2screen( SPoint( 0, 0 )).y, Width(), Height() );
   Surface s = Surface::Wrap( PG_Application::GetScreen() );
   
#ifdef debugmapdisplay
   s.Fill( 0xff00ff );
#endif

   if ( zoom != 100 ) {
      float fzoom = float(zoom) / 100.0;
      MegaBlitter<colorDepth,colorDepth,ColorTransform_None,ColorMerger_AlphaOverwrite,MovePixSel,TargetPixelSelector_Rect> blitter;
      blitter.setZoom( fzoom );
      blitter.initSource( *surface );
      
      blitter.setSrcRectangle( SPoint( getFieldPosX(0,0), getFieldPosY(0,0)), int(float(Width()) / fzoom), int(float(Height()) / fzoom));
      blitter.setInnerSrcRectangle( movement.blitViewPortInternal  );
      
      blitter.setTargetRect( targetArea );
      blitter.blit( *surface, s, movement.targetBlitPos );
   } else {
      MegaBlitter<colorDepth,colorDepth,ColorTransform_None,ColorMerger_AlphaOverwrite,SourcePixelSelector_DirectSubRectangle,TargetPixelSelector_Rect> blitter;
      blitter.initSource( *surface );
      
      blitter.setSrcRectangle( SPoint( getFieldPosX(0,0), getFieldPosY(0,0)), Width(), Height() );
      blitter.setInnerSrcRectangle( movement.blitViewPortInternal  );
      
      blitter.setTargetRect( targetArea  );
      blitter.blit( *surface, s, movement.targetBlitPos );
   }

#ifdef debugmapdisplay
   rectangle<4>( s, SPoint(targetArea.x, targetArea.y), targetArea.w, targetArea.h, ColorMerger_ColoredOverwrite<4>( 0x00ffff ), ColorMerger_ColoredOverwrite<4>( 0x00ffff ) );
   rectangle<4>( s, SPoint(movement.blitViewPortScreen.x, movement.blitViewPortScreen.y), movement.blitViewPortScreen.w, movement.blitViewPortScreen.h, ColorMerger_ColoredOverwrite<4>( 0x0000ff ), ColorMerger_ColoredOverwrite<4>( 0x0000ff ) );
   PG_Application::UpdateRect(PG_Application::GetScreen(), 0, 0, PG_Application::GetScreen()->w, PG_Application::GetScreen()->h );
#else   
   PG_Application::UpdateRect(PG_Application::GetScreen(), movement.blitViewPortScreen.x, movement.blitViewPortScreen.y, movement.blitViewPortScreen.w, movement.blitViewPortScreen.h );
#endif
}





bool ccompare( const MapCoordinate& a, const MapCoordinate& b )
{
   return a.y < b.y || (a.y == b.y && a.x < b.x);
}   


void MapDisplayPG::displayUnitMovement( GameMap* actmap, Vehicle* veh, const MapCoordinate3D& from, const MapCoordinate3D& to, int duration )
{
   static int col = 0xff;
   
#ifdef debugmapdisplay
   col += 30;
   if ( col > 255 )
      col -= 255;
#endif

   surface->Fill( col << 8 );
   
   if ( !fieldInView( from ) && !fieldInView( to ))
      return;

   if ( from.x == to.x && from.y == to.y && from.getBitmappedHeight() <= chschwimmend && to.getBitmappedHeight() <= chschwimmend )
      // we don't animate a descending or ascending submarine without movement
      return;

   int startTime = ticker;
   int endTime = startTime + duration;
   
   // initialisation that is only executed the first time the code runs here
   initMovementStructure();

   int dir = getdirection( from, to );
   
   if ( from.x == to.x && from.y == to.y )
      dir = -1;  // changing height vertically
   else   
      veh->direction = dir;
   
   typedef vector< MapCoordinate > TouchedFields;
   TouchedFields touchedFields;

   touchedFields.push_back ( from );
   for ( int ii = 0; ii < sidenum; ++ii ) {
      MapCoordinate p = getNeighbouringFieldCoordinate(from, ii);
      if ( fieldInView( p ) )
         touchedFields.push_back ( p );
   }
      
   if ( dir != -1 ) {
      if ( fieldInView( to ))
         touchedFields.push_back ( to);

      for ( int ii = 0; ii < sidenum; ++ii ) {
         MapCoordinate p = getNeighbouringFieldCoordinate(to, ii);
         if ( fieldInView( p ) )
            touchedFields.push_back ( p );
      }
   }
   

   // now we have all fields that are touched during movement in a list
   // let's sort it

   sort(   touchedFields.begin(), touchedFields.end(), ccompare );
   
   Movement movement;
   int i = 0;
   for ( TouchedFields::iterator j = touchedFields.begin(); j != touchedFields.end(); ++j )  {
      if ( i == 0 || movement.touchedFields[i-1].mapPos != *j ) {
         movement.touchedFields[i].mapPos = *j;
         movement.touchedFields[i].surfPos = mapGlobalPos2internalPos( *j );
         ++i;
      }
   }
   movement.actualFieldNum = i;
           
   movement.veh = veh;
   movement.from = mapGlobalPos2internalPos( from );
   movement.to = mapGlobalPos2internalPos( to );

   const int maxShadowWidth = 32;
   SPoint p1 = SPoint( min( movement.from.x, movement.to.x), min( movement.from.y, movement.to.y) );
   movement.blitViewPortInternal  = PG_Rect( p1.x, p1.y, max( movement.from.x, movement.to.x) - p1.x + maxShadowWidth + fieldsizex,
                                                         max( movement.from.y, movement.to.y) - p1.y + maxShadowWidth + fieldsizey );

   movement.blitViewPortInternal = movement.blitViewPortInternal.IntersectRect( PG_Rect( 0, 0, surface->w(), surface->h() ) );
   movement.targetBlitPos = widget2screen ( SPoint( 0, 0));
   SPoint a = widget2screen( internal2widget( SPoint( movement.blitViewPortInternal.x, movement.blitViewPortInternal.y )));
   SPoint b = widget2screen( internal2widget( SPoint( movement.blitViewPortInternal.x + movement.blitViewPortInternal.w, movement.blitViewPortInternal.y + movement.blitViewPortInternal.h )));

   
   movement.blitViewPortScreen = PG_Rect( a.x, a.y, b.x - a.x + 1, b.y - a.y + 1 );
   movement.blitViewPortScreen = movement.blitViewPortScreen.IntersectRect( PG_Rect( 0, 0, PG_Application::GetScreenWidth(), PG_Application::GetScreenHeight() ));

   movement.toShadow  = ContainerBase::calcShadowDist( to.getNumericalHeight() );
   movement.fromShadow  = ContainerBase::calcShadowDist( from.getNumericalHeight() );
   
   movement.actmap = actmap;
   
   if ( dir < 0 ) {
      movement.mask = &movementMask[0].mask;
      movement.maskPosition = mapGlobalPos2internalPos( from ) -  movementMask[0].startFieldPos;
   } else {
      movement.mask = &movementMask[dir].mask;
      movement.maskPosition = mapGlobalPos2internalPos( from ) -  movementMask[dir].startFieldPos;
   }
   
   movement.playerView = actmap->getPlayerView();
   
   int loopCounter = 0;
#ifdef debugmapdisplay
   int loopStartTicker = ticker;
#endif

   while ( ticker < endTime ) {
      displayMovementStep( movement, (ticker - startTime) * 100 / duration );
      ++loopCounter;
   }

   if( duration )
      displayMovementStep( movement, 100 );
   
#ifdef debugmapdisplay
   cout << (float(loopCounter) / float(ticker - loopStartTicker) * 100) << " / " << (float(loopCounter) / float(ticker - startTime) * 100) << " fps \n";
#endif
}

void MapDisplayPG::displayAddons( Surface& surf, int pass)
{
   if( additionalUnit ) 
      if ( pass == bitmappedHeight2pass( additionalUnit->height ) )  {
         SPoint p = mapViewPos2internalPos( additionalUnit->getPosition() - offset );
         if ( p.x >= 0 && p.y >= 0 && p.x + fieldsizex + ContainerBase::calcShadowDist(getFirstBit(chsatellit)) < surf.w() && p.y + fieldsizey  + ContainerBase::calcShadowDist(getFirstBit(chsatellit)) < surf.h() )
            additionalUnit->paint( surf, p, false, -1 );
      }

}

MapCoordinate& MapDisplayPG::Cursor::pos()
{
   if ( actmap )
      return actmap->getCursor();
   else {
      static MapCoordinate mc;
      return mc;
   }
}

void MapDisplayPG::scrollMap( int dir )
{
   if ( !actmap )
      return;

   MapCoordinate oldOffset = offset;

   const int stepWidth = 2;


   if ( dir == 7 || dir == 0 || dir == 1 )
      offset.y -= 2 * stepWidth;

   if ( dir >= 1 && dir <= 3 )
      offset.x += stepWidth;

   if ( dir >= 3 && dir <= 5 )
      offset.y += 2 * stepWidth;

   if ( dir >= 5 && dir <= 7)
      offset.x -= stepWidth;
   
   checkViewPosition( offset );

   if ( offset != oldOffset ) {
      dirty = Map;
      Update();
      viewChanged();
   }
}


void MapDisplayPG::moveCursor( int dir, int step )
{
   MapCoordinate pos = cursor.pos();

   switch ( dir ) {
      case 0:  pos.y -= 2 * step;
               break;
      case 1:  pos.y -= 1 * step;
               if ( !(pos.y & 1) )
                  pos.x += 1 * step;
               break;         
      case 2:  if ( step & 1 ) {
                  if ( pos.y & 1 ) {
                     if ( pos.x < actmap->xsize-1 ) {
                        pos.x += 1 * step;
                        pos.y -= 1 * step;
                     }
                  } else
                     pos.y += 1 * step;
               } else
                  pos.x += step/2;
               break;
      case 3:  pos.y += 1 * step;
               if ( !(pos.y & 1) )
                  pos.x += 1 * step;
               break;
      case 4:  pos.y += 2 * step;
               break;
      case 5:  pos.y += 1 * step;
               if ( pos.y & 1 )
                  pos.x -= 1 * step;
               break;
      case 6:  if ( step & 1) {
                  if ( pos.y & 1 ) {
                     pos.y -= 1 * step;
                  } else {
                     if ( pos.x > 0 ) {
                        pos.x -= 1 * step;
                        pos.y += 1 * step;
                     }
                  }
               } else
                  pos.x -= step/2;
               break;
      case 7:  pos.y -= 1 * step;
               if ( pos.y & 1 )
                  pos.x -= 1 * step;
               break;
   }

   if ( pos.x >= actmap->xsize )
      pos.x = actmap->xsize -1;

   if ( pos.x < 0 )
      pos.x = 0;

   if ( pos.y >= actmap->ysize )
      pos.y = actmap->ysize -1;

   if ( pos.y < 0 )
      pos.y = 0;

   if ( pos != cursor.pos() ) {
      MapCoordinate oldpos = cursor.pos();
      cursor.invisible = 0;
      dirty = Curs;

      cursor.pos() = pos;

      MapCoordinate oldOffset = offset;

      if ( pos.x < offset.x )
         offset.x = max( 0, pos.x );

      if ( pos.y < offset.y )
         offset.y = min( 0, pos.y & ~1);

      if ( pos.x > offset.x + field.numx - 2  || !fieldCompletelyInViewX( pos ))
         offset.x = pos.x - field.numx + 6;

      if ( pos.y > offset.y + field.numy - 2 || !fieldCompletelyInViewY( pos ))
         offset.y = (pos.y - field.numy + 8) & ~1;

      if ( offset != oldOffset )
         dirty = Map;

      checkViewPosition( offset );
      
      cursorMoved();
      if ( offset != oldOffset ) {
         Update();
         viewChanged();
      } else {
         redrawMapAtCursor( oldpos );
         /*
         SPoint p = internal2widget( mapGlobalPos2internalPos( oldpos ));
         UpdateRect( PG_Rect( p.x, p.y, fieldsizex, fieldsizey ) );
         
         p = internal2widget( mapGlobalPos2internalPos( pos ));
         UpdateRect( PG_Rect( p.x, p.y, fieldsizex, fieldsizey ) );
         */
      }
   }
}


bool MapDisplayPG::keyboardHandler( const SDL_KeyboardEvent* keyEvent)
{
   if ( !keyEvent || !actmap )
      return false;

   int keyStateNum;
   Uint8* keyStates = SDL_GetKeyState ( &keyStateNum );

   if ( keyEvent->type == SDL_KEYDOWN ) {
      if ( !disableKeyboardCursorMovement ) {
         if ( keyEvent->keysym.sym == SDLK_RIGHT && keyStates[SDLK_RIGHT] ) {
            moveCursor(2, 1);
            return true;
         }
         if ( keyEvent->keysym.sym == SDLK_LEFT  && keyStates[SDLK_LEFT]  ) {
            moveCursor(6, 1);
            return true;
         }
         if ( (keyEvent->keysym.sym == SDLK_UP  && keyStates[SDLK_UP] ) || ( keyEvent->keysym.sym == SDLK_KP8  && keyStates[SDLK_KP8] )) {
            moveCursor(0, 1);
            return true;
         }
         if ( (keyEvent->keysym.sym == SDLK_DOWN  && keyStates[SDLK_DOWN]) || (keyEvent->keysym.sym == SDLK_KP2  && keyStates[SDLK_KP2] )) {
            moveCursor(4, 1);
            return true;
         }
         if ( keyEvent->keysym.sym == SDLK_KP6  && keyStates[SDLK_KP6] ) {
            moveCursor(2, 2);
            return true;
         }
         if ( keyEvent->keysym.sym == SDLK_KP4 && keyStates[SDLK_KP4] ) {
            moveCursor(6, 2);
            return true;
         }
         if ( keyEvent->keysym.sym == SDLK_KP7  && keyStates[SDLK_KP7] ) {
            moveCursor(7, 1);
            return true;
         }
         if ( keyEvent->keysym.sym == SDLK_KP9  && keyStates[SDLK_KP9]) {
            moveCursor(1, 1);
            return true;
         }
         if ( keyEvent->keysym.sym == SDLK_KP1  && keyStates[SDLK_KP1]) {
            moveCursor(5, 1);
            return true;
         }
         if ( keyEvent->keysym.sym == SDLK_KP3  && keyStates[SDLK_KP3]) {
            moveCursor(3, 1);
            return true;
         }
         
      }   
   }
   return false;
}


void MapDisplayPG::registerAdditionalUnit( Vehicle* veh )
{
   additionalUnit = veh;
}

void MapDisplayPG::addMapLayer( MapLayer* layer, const ASCString& name )
{
   MapRenderer::addMapLayer( layer );
   layerMap[name] = layer;
   layer->setActive(false);
}

void MapDisplayPG::activateMapLayer( const ASCString& name, bool active )
{
   LayerMap::iterator i = layerMap.find( name );
   if ( i != layerMap.end() ) {
      i->second->setActive( active );
      layerChanged( active, name );
   }   
}

void MapDisplayPG::toggleMapLayer( const ASCString& name )
{
   LayerMap::iterator i = layerMap.find( name );
   if ( i != layerMap.end() ) {
      i->second->setActive( !i->second->isActive() );
      layerChanged( i->second->isActive(), name );
   }
}

bool MapDisplayPG::layerActive( const ASCString& name )
{
   LayerMap::iterator i = layerMap.find( name );
   if ( i != layerMap.end() ) 
      return i->second->isActive();
   else
      return false;
}


void MapDisplayPG::getActiveLayers( vector<ASCString>& list )
{
   list.clear();
   for ( LayerMap::iterator i = layerMap.begin(); i != layerMap.end(); ++i )
      if ( i->second->isActive() )
         list.push_back( i->first );
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


void MapDisplayPG::Cursor::goTo( const MapCoordinate& cursorPosition, const MapCoordinate& upperLeftScreenCorner )
{
   bool redraw = false;
   
   if ( upperLeftScreenCorner.valid() )
      if ( upperLeftScreenCorner != mapDisplay->offset ) {
         redraw = true;
         mapDisplay->offset = upperLeftScreenCorner;
      }
   
   MapCoordinate oldpos = pos();
   if ( !mapDisplay->fieldInView( cursorPosition) )
      mapDisplay->centerOnField(cursorPosition);
   
   pos()=cursorPosition;
   
   if ( pos() != oldpos || redraw) {
      invisible = 0;
      mapDisplay->dirty = MapDisplayPG::Curs;
      mapDisplay->Update();
      // mapDisplay->updateWidget();
   }
}

void MapDisplayPG::Cursor::goTo( const MapCoordinate& position )
{
   MapCoordinate oldpos = pos();
   if ( !mapDisplay->fieldInView( position) )
      mapDisplay->centerOnField(position);
   pos()=position;
   
   if ( pos() != oldpos ) {
      invisible = 0;
      mapDisplay->dirty = MapDisplayPG::Curs;
      mapDisplay->Update();
      // mapDisplay->updateWidget();
   }
      
}





int lockdisplaymap = 0;



void MapDisplayPG::LockDisplay::raiseLock()
{
   if ( theMapDisplay ) {
      ++theMapDisplay->lock;
      theMapDisplay->dirty = Map;
      theMapDisplay->Update();
   }
   lockMapdisplay();

}


MapDisplayPG::LockDisplay::LockDisplay( bool dummy ) : isDummy( dummy )
{
   if ( !dummy )
      raiseLock();      
}

MapDisplayPG::LockDisplay::~LockDisplay()
{
   if ( !isDummy ) {
      if ( theMapDisplay ) {
         --theMapDisplay->lock;
         if ( !theMapDisplay->lock  ) {
            theMapDisplay->dirty = Map;
            theMapDisplay->Update();
         }
      }
      if ( !theMapDisplay->lock )
         unlockMapdisplay();
   }
}



