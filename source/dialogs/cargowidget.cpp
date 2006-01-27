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

#include "cargowidget.h"
#include "../containerbase.h"
#include "../iconrepository.h"
#include "../vehicle.h"
#include "../gamemap.h"
#include "../graphics/blitter.h"



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
      Update();
}

bool StoringPosition :: eventMouseButtonDown (const SDL_MouseButtonEvent *button)
{
   if ( button->type == SDL_MOUSEBUTTONDOWN && button->button == SDL_BUTTON_LEFT ) {
      highlight.setNew( num );
      return true;
   }
   return false;
}


StoringPosition :: StoringPosition( PG_Widget *parent, const PG_Point &pos, HighLightingManager& highLightingManager, StorageVector& storageVector, int number, bool regularPosition  )
      : PG_Widget ( parent, PG_Rect( pos.x, pos.y, spWidth, spHeight)), highlight( highLightingManager ), storage( storageVector), num(number), regular(regularPosition)
{
   if ( !clippingSurface.valid() )
      clippingSurface = Surface::createSurface( spWidth + 10, spHeight + 10, 32, 0 );

   highlight.markChanged.connect( SigC::slot( *this, &StoringPosition::markChanged ));
   highlight.redrawAll.connect( SigC::bind( SigC::slot( *this, &StoringPosition::Update), true));
}




void StoringPosition :: eventBlit (SDL_Surface *surface, const PG_Rect &src, const PG_Rect &dst)
{
   clippingSurface.Fill(0);

   ASCString background = "hexfield-bld-";
   background += regular ? "1" : "2";
   if (  num == highlight.getMark() )
      background += "h";
   background += ".png";

   Surface& icon = IconRepository::getIcon( background );

   MegaBlitter<4,4,ColorTransform_None, ColorMerger_AlphaOverwrite> blitter;
   blitter.blit( icon, clippingSurface, SPoint(0,0));

   if ( num < storage.size() && storage[num] ) {
      int ypos;
      if ( num == highlight.getMark() )
         ypos = 0;
      else
         ypos = 1;

      if( storage[num]->getMovement() > 0  )
         storage[num]->typ->paint( clippingSurface, SPoint(0,ypos), storage[num]->getOwner() );
      else
         storage[num]->typ->paint( clippingSurface, SPoint(0,ypos), storage[num]->getMap()->getNeutralPlayerNum() );
   }

   PG_Draw::BlitSurface( clippingSurface.getBaseSurface(), src, PG_Application::GetScreen(), dst);
}


vector<StoringPosition*> StoringPosition :: setup( PG_Widget* parent, ContainerBase* container, HighLightingManager& highLightingManager, int& unitColumnCount )
{
   vector<StoringPosition*> storingPositionVector;
   if ( parent) {
      int x = 0;
      int y = 0;
      int posNum = container->baseType->maxLoadableUnits;
      if ( container->cargo.size() > posNum )
         posNum = container->cargo.size();

      for ( int i = 0; i < posNum; ++i ) {
         storingPositionVector.push_back( new StoringPosition( parent, PG_Point( x, y), highLightingManager, container->cargo, i, container->baseType->maxLoadableUnits >= container->cargo.size() ));
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


Surface StoringPosition::clippingSurface;




CargoWidget :: CargoWidget( PG_Widget* parent, const PG_Rect& pos, ContainerBase* container ) : PG_ScrollWidget( parent, pos ), unitColumnCount(0)
{
   this->container = container;
   SetTransparency( 255 );
   storingPositionVector = StoringPosition::setup( this, container, unitHighLight, unitColumnCount );

   unitHighLight.markChanged.connect( SigC::slot( *this, &CargoWidget::checkStoringPosition ));
};

void CargoWidget :: redrawAll()
{
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
   PG_ScrollWidget* unitScrollArea = dynamic_cast<PG_ScrollWidget*>(FindChild( "UnitScrollArea", true ));
   if ( unitScrollArea )
      if ( newpos < storingPositionVector.size() && newpos >= 0 )
         unitScrollArea->ScrollToWidget( storingPositionVector[newpos] );

   unitMarked( getMarkedUnit() );
}

Vehicle* CargoWidget :: getMarkedUnit()
{
   int pos = unitHighLight.getMark();
   if ( !container || pos < 0 || pos >= container->cargo.size() )
      return NULL;
   else
      return container->cargo[pos];
}

