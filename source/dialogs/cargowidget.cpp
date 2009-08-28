/***************************************************************************
                          cargowidget.cpp  -  description
                             -------------------
    begin                : Tue Oct 24 2000
    copyright            : (C) 2000 by Martin Bickel
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

#include <pgimage.h>
#include <pgtooltiphelp.h>
#include <pgapplication.h>
#include <pgeventsupplier.h>
#include <pgmultilineedit.h>

#include "cargowidget.h"
#include "../containerbase.h"
#include "../iconrepository.h"
#include "../vehicle.h"
#include "../gamemap.h"
#include "../graphics/blitter.h"
#include "../graphics/drawing.h"
#include "../widgets/bargraphwidget.h"
#include "../paradialog.h"
#include "../gameoptions.h"
#include "../events.h"

HighLightingManager::HighLightingManager() : marked(0) {};

int HighLightingManager::getMark()
{
   return marked;
};

void HighLightingManager::setNew(int pos )
{
   int old = marked;
   marked = pos;
   markChanged(old,pos);
};



void StoringPosition :: markChanged(int old, int mark)
{
   if ( num == old || num == mark )
      Redraw();
}




int StoringPosition :: spWidth = -1;
int StoringPosition :: spHeight = -1;

PG_Rect StoringPosition :: CalcSize( const PG_Point& pos  )
{
   if ( spWidth < 0 ) {
      Surface& icon = IconRepository::getIcon( "hexfield-bld-1.png" );
      spWidth = icon.w();
      spHeight = icon.h();
   }

   return PG_Rect( pos.x, pos.y, spWidth, spHeight );
}


StoringPosition :: StoringPosition( PG_Widget *parent, const PG_Point &pos, const PG_Point& unitPos, HighLightingManager& highLightingManager, const ContainerBase::Cargo& storageVector, int number, bool regularPosition, CargoWidget* cargoWidget  )
      : PG_Widget ( parent, CalcSize(pos), true ), highlight( highLightingManager ), storage( storageVector), num(number), regular(regularPosition), unitPosition( unitPos ), dragState( Off ), dragTarget( NoDragging )
{
   highlight.markChanged.connect( SigC::slot( *this, &StoringPosition::markChanged ));
   highlight.redrawAll.connect( SigC::bind( SigC::slot( *this, &StoringPosition::Redraw), true));

   if ( unitPosition.x < 0 ) {
      unitPosition.x = (Width() - fieldsizex)/2;
      unitPosition.y = (Height() - fieldsizey)/2;
   }

   clippingSurface = Surface::createSurface( spWidth + 10, spHeight + 10, 32, 0 );
   
   
   this->cargoWidget = cargoWidget;
}

void StoringPosition::setBargraphValue( const ASCString& widgetName, float fraction )
{
   BarGraphWidget* bgw = dynamic_cast<BarGraphWidget*>( FindChild( widgetName, true ) );
   if ( bgw )
      bgw->setFraction( fraction );
}

void StoringPosition::setLabelText ( const ASCString& widgetName, const ASCString& text, PG_Widget* parent )
{
   if ( !parent )
      parent = this;

   PG_Label* l = dynamic_cast<PG_Label*>( parent->FindChild( widgetName, true ) );
   if ( l )
      l->SetText( text );
   else {
      PG_LineEdit* l = dynamic_cast<PG_LineEdit*>( parent->FindChild( widgetName, true ) );
      if ( l )
         l->SetText( text );
      else {
         PG_MultiLineEdit* l = dynamic_cast<PG_MultiLineEdit*>( parent->FindChild( widgetName, true ) );
         if ( l )
            l->SetText( text );
      }
   }
}


void StoringPosition :: eventBlit (SDL_Surface *surface, const PG_Rect &src, const PG_Rect &dst){
   
   PG_Draw::BlitSurface( clippingSurface.getBaseSurface(), src, PG_Application::GetScreen(), dst);
   
}


void StoringPosition :: eventDraw (SDL_Surface *surface, const PG_Rect &src)
{
   clippingSurface.FillTransparent();
   
   ASCString background = "hexfield-bld-";
   if ( dragTarget == NoDragging ) {
      background += regular ? "1" : "2";
      if (  num == highlight.getMark() )
         background += "h";
   } else {
      background += dragTarget==TargetAvail ? "1" : "2";
   }
   background += ".png";

   Surface& icon = IconRepository::getIcon( background );
  
   MegaBlitter<4,4,ColorTransform_None, ColorMerger_AlphaMerge> blitter;
   blitter.blit( icon, clippingSurface, SPoint(0,0));

   if ( num < storage.size() && storage[num] ) {
      int xpos = unitPosition.x;
      int ypos = unitPosition.y;
      if ( num != highlight.getMark() )
         ypos += 1;


      storage[num]->direction = 0;

      if ( storage[num]->getMovement() > 0  )
         storage[num]->paint( clippingSurface, SPoint(xpos,ypos), 0 ); // storage[num]->getOwner() );
      else
         storage[num]->paint( clippingSurface, SPoint(xpos,ypos), true, 0 ); // storage[num]->getMap()->getNeutralPlayerNum() );

      setBargraphValue( "DamageBar", float(100-storage[num]->damage)/100 );

      if ( storage[num]->getStorageCapacity().fuel ) {
         float f = float(storage[num]->getResource( maxint, 2, true, 0 )) / storage[num]->getStorageCapacity().fuel;
         setBargraphValue( "FuelBar", f );
      } else
         setBargraphValue( "FuelBar", 0 );

      if ( storage[num]->getStorageCapacity().material ) {
         float f = float(storage[num]->getResource( maxint, 1, true, 0 )) / storage[num]->getStorageCapacity().material;
         setBargraphValue( "MaterialBar", f );
      } else
         setBargraphValue( "MaterialBar", 0 );


      if ( storage[num]->typ->maxLoadableWeight ) {
         float f = float(storage[num]->cargoWeight()) / storage[num]->typ->maxLoadableWeight;
         setBargraphValue( "CargoBar", f );
      } else
         setBargraphValue( "CargoBar", 0 );

      setLabelText( "SlotUnitName", storage[num]->name );
         
      ContainerBase* container = storage[num];
      
      int slots = 0;
      for ( ContainerBase::Cargo::const_iterator i = container->getCargo().begin(); i != container->getCargo().end(); ++i )
         if( *i )
            ++slots;
               
      if ( container->baseType->maxLoadableUnits ) {
         float f = float( slots ) / float(container->baseType->maxLoadableUnits);
         setBargraphValue ( "SlotBar", f );
      } else
         setBargraphValue ( "SlotBar", 0 );
      
      
      setLabelText( "Weight",ASCString::toString( storage[num]->weight() ));
      
      
      ASCString s;
      if ( container->baseType->maxLoadableUnits )
         s.format( "%d / %d", container->cargoWeight(), container->baseType->maxLoadableWeight );
      else
         s = "-";
      setLabelText( "CargoUsage", s );
   
      s.format( "%d / %d", slots, container->baseType->maxLoadableUnits );
      setLabelText( "SlotSummary", s );
      
   } else {
      setBargraphValue( "DamageBar", 0 );
      setBargraphValue( "FuelBar", 0 );
      setBargraphValue( "MaterialBar", 0 );
      setBargraphValue( "CargoBar", 0 );
      setLabelText( "SlotUnitName", "" );
      setBargraphValue ( "SlotBar", 0 );
      setLabelText( "CargoUsage", "" );
      setLabelText( "SlotSummary", "" );
      setLabelText( "Weight", "" );
   }

   // PG_Draw::BlitSurface( clippingSurface.getBaseSurface(), src, PG_Application::GetScreen(), dst);
//    PG_Draw::DrawSurface( src, dst);
}


bool StoringPosition::eventMouseButtonDown(const SDL_MouseButtonEvent* button)
{
   if ( button->type != SDL_MOUSEBUTTONDOWN  )
      return false;

   if ( button->button == CGameOptions::Instance()->mouse.fieldmarkbutton ) {

      int oldPos = highlight.getMark();
      highlight.setNew( num );

      highlight.clickOnMarkedUnit( num, SPoint(button->x, button->y), num != oldPos );
      return true;
   }


/*
   if ( num >= storage.size() || !storage[num] )
      return false;
*/

   int x,y;
   PG_Application::GetEventSupplier()->GetMouseState(x, y);


   if ( button->button == CGameOptions::Instance()->mouse.dragndropbutton  && cargoWidget && cargoWidget->dragNdropEnabled() && getUnit() ) {

      SetCapture();
      dragState = Pressed;

      dragPointStart.x = x;
      dragPointStart.y = y;

      /*

      dragPointOld.x = x;
      dragPointOld.y = y;


      Draging = true;
      eventDragStart();
      dragimage = eventQueryDragImage();

      if(dragimage != NULL) {
      dragimagecache = PG_Draw::CreateRGBSurface(dragimage->w, dragimage->h);
      }

      cacheDragArea(dragPointOld); */
      return true;
   }

   return false;
}

bool StoringPosition ::eventMouseButtonUp(const SDL_MouseButtonEvent* button)
{
   if ( dragState != Off && (button->button != 4 && button->button != 5 )) {
      ReleaseCapture();
      dragState = Off;

      if ( !cargoWidget )
         return false;

      {
         EventHandlingMutex ehm();
         
         PG_Application::ShowCursor( PG_Application::NONE );
         PG_Application::SetCursor( NULL );
         PG_Application::ShowCursor( PG_Application::HARDWARE );
      }

      int x,y;
      PG_Application::GetEventSupplier()->GetMouseState(x, y);

      // x += mouseCursorOffset.x;
      // y += mouseCursorOffset.y;

      StoringPosition* s = dynamic_cast<StoringPosition*>( FindWidgetFromPos (x, y));
      if ( s )
         cargoWidget->releaseDrag( s->getUnit() );
      else {
         cargoWidget->releaseDrag( x, y);
      }



      return true;
   } else
      return false;
}

#define square(x) ((x)*(x))

bool StoringPosition::eventMouseMotion (const SDL_MouseMotionEvent *motion)
{
   if ( dragState != Off ) {
      if ( cargoWidget )
         cargoWidget->sigDragInProcess();

      if ( dragState == Pressed ) {
         if ( square(motion->x - dragPointStart.x) + square(motion->y - dragPointStart.y) > 9 ) {
            cargoWidget->startDrag( storage[num] );
            dragState = Dragging;

            static Surface surf;
            if ( !surf.valid() ) {
               surf = Surface::createSurface( fieldsizex+20, fieldsizey+20, 32 ); // somewhat larger because of potential shadow
               mouseCursorOffset = PG_Point( fieldsizex/2, fieldsizey/2);
            }
            surf.Fill( 0 );

            MegaBlitter<4,4,ColorTransform_None, ColorMerger_AlphaOverwrite> blitter;
            blitter.blit( IconRepository::getIcon("mouse.png"), surf, SPoint(0,0));

            EventHandlingMutex ehm();
            
            getUnit()->paint( surf, SPoint(0,0), getUnit()->getOwner() );
            PG_Application::ShowCursor( PG_Application::NONE );
            PG_Application::SetCursor( const_cast<SDL_Surface*>( surf.getBaseSurface() ));
            PG_Application::ShowCursor( PG_Application::SOFTWARE );
            
         }
      }
   }

   return false;
}


Vehicle* StoringPosition :: getUnit()
{
   if ( num >= storage.size() || !storage[num] )
      return NULL;
   else
      return storage[num];
}


vector<StoringPosition*> StoringPosition :: setup( PG_Widget* parent, ContainerBase* container, HighLightingManager& highLightingManager, int& unitColumnCount )
{
   vector<StoringPosition*> storingPositionVector;
   if ( parent) {
      int x = 0;
      int y = 0;
      int posNum = container->baseType->maxLoadableUnits;
      if ( container->getCargo().size() > posNum )
         posNum = container->getCargo().size();

      for ( int i = 0; i < posNum; ++i ) {
         StoringPosition* sp = new StoringPosition( parent, PG_Point( x, y), PG_Point(-1,-1), highLightingManager, container->getCargo(), i, container->baseType->maxLoadableUnits >= container->getCargo().size() );
         storingPositionVector.push_back( sp );
         x += StoringPosition::spWidth;
         if ( x + StoringPosition::spWidth >= parent->Width() - 20 ) {
            if ( !unitColumnCount )
               unitColumnCount = i + 1;
            x = 0;
            y += StoringPosition::spHeight;
         }
      }
   }
   return storingPositionVector;
}


 

CargoWidget :: CargoWidget( PG_Widget* parent, const PG_Rect& pos, ContainerBase* container, bool setup ) : PG_ScrollWidget( parent, pos ), dragNdrop(true), unitColumnCount(0), draggedUnit(NULL)
{
   this->container = container;
   SetTransparency( 255 );

   if ( setup )
      registerStoringPositions( StoringPosition::setup( this, container, unitHighLight, unitColumnCount ), unitColumnCount );

   if ( my_objVerticalScrollbar )
      my_objVerticalScrollbar->sigScrollTrack.connect ( SigC::slot( *this, &CargoWidget::handleScrollTrack ));

   if ( my_objHorizontalScrollbar )
      my_objHorizontalScrollbar->sigScrollTrack.connect ( SigC::slot( *this, &CargoWidget::handleScrollTrack ));
};

void CargoWidget::registerStoringPositions( vector<StoringPosition*> sp, const int& colCount )
{
   unitColumnCount = colCount;
   storingPositionVector  = sp;
   unitHighLight.markChanged.connect( SigC::slot( *this, &CargoWidget::checkStoringPosition ));
   unitHighLight.clickOnMarkedUnit.connect( SigC::slot( *this, &CargoWidget::click ));
}

bool  CargoWidget::handleScrollTrack (PG_ScrollBar *widget, long data)
{
   sigScrollTrack();
   return true;
}



void CargoWidget::click( int num, SPoint mousePos, bool first )
{
   if ( container->getCargo().size() > num )
      unitClicked( container->getCargo()[num], mousePos, first );
   else
      unitClicked( NULL, mousePos, first );
}


void CargoWidget :: redrawAll()
{
   BulkGraphicUpdates bgu( this );
   unitHighLight.redrawAll();
}


void CargoWidget :: moveSelection( int delta )
{
   int newpos = unitHighLight.getMark() + delta;

   if ( newpos < 0 )
      newpos = 0;

   if ( newpos >= storingPositionVector.size() )
      newpos = storingPositionVector.size() -1 ;

   if ( newpos != unitHighLight.getMark() )
      unitHighLight.setNew( newpos );
}

bool CargoWidget :: eventKeyDown(const SDL_KeyboardEvent* key)
{
   if ( key->keysym.sym == SDLK_RIGHT )  {
      moveSelection(1);
      return true;
   }
   if ( key->keysym.sym == SDLK_LEFT )  {
      moveSelection(-1);
      return true;
   }
   if ( key->keysym.sym == SDLK_UP )  {
      moveSelection(-unitColumnCount);
      return true;
   }
   if ( key->keysym.sym == SDLK_DOWN )  {
      moveSelection(unitColumnCount);
      return true;
   }
   return false;
};


void CargoWidget :: checkStoringPosition( int oldpos, int newpos )
{
//  PG_Widget* unitScrollAreaw = dynamic_cast<PG_Widget*>(FindChild( "UnitScrollArea", true ));
//  PG_ScrollWidget* unitScrollArea = dynamic_cast<PG_ScrollWidget*>(FindChild( "UnitScrollArea", true ));
//  if ( unitScrollArea )
   if ( newpos < storingPositionVector.size() && newpos >= 0 )
      ScrollToWidget( storingPositionVector[newpos] );

   unitMarked( getMarkedUnit() );
}

Vehicle* CargoWidget :: getMarkedUnit()
{
   int pos = unitHighLight.getMark();
   if ( !container || pos < 0 || pos >= container->getCargo().size() )
      return NULL;
   else
      return container->getCargo()[pos];
}


void CargoWidget :: startDrag( Vehicle* v )
{
   for ( StoringPositionVector::iterator i = storingPositionVector.begin(); i != storingPositionVector.end(); ++i ) {
      Vehicle* target = (*i)->getUnit();
      if ( target && target != v && sigDragAvail( v, target ) ) {
         (*i)->setDragTarget( StoringPosition::TargetAvail );
      } else {
         (*i)->setDragTarget( StoringPosition::TargetNotAvail );
      }
      draggedUnit = v;
   }
   Redraw();
}

void CargoWidget :: releaseDrag( Vehicle* v)
{
   for ( StoringPositionVector::iterator i = storingPositionVector.begin(); i != storingPositionVector.end(); ++i )
      (*i)->setDragTarget( StoringPosition::NoDragging );

   if ( v )
      sigDragDone( draggedUnit, v );
   else
      sigDragAborted();

   draggedUnit = NULL;
}

void CargoWidget :: releaseDrag( int x, int y )
{
   for ( StoringPositionVector::iterator i = storingPositionVector.begin(); i != storingPositionVector.end(); ++i )
      (*i)->setDragTarget( StoringPosition::NoDragging );

   if ( IsMouseInside() )
      sigDragAborted();
   else
      sigDragDone( draggedUnit, NULL );

   draggedUnit = NULL;
}
