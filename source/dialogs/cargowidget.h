/***************************************************************************
                          cargowdiget.h  -  description
                             -------------------
    begin                : Tue Feb 17 2001
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


#ifndef cargowidgetH
 #define cargowidgetH

#include "selectionwindow.h"
#include "../graphics/surface.h"
#include "../containerbase.h"

class Vehicle;
class ContainerBase;

class HighLightingManager
{
      int marked;
   public:
      HighLightingManager();
      int getMark();
      SigC::Signal2<void,int,int> markChanged;
      void setNew(int pos );
      SigC::Signal0<void> redrawAll;
      SigC::Signal2<void,int, SPoint> clickOnMarkedUnit;
};


class StoringPosition : public PG_Widget
{
      static Surface clippingSurface;
      HighLightingManager& highlight;
      const ContainerBase::Cargo& storage;
      int num;
      bool regular;
   protected:
      void markChanged(int old, int mark);
      bool eventMouseButtonDown (const SDL_MouseButtonEvent *button);

   public:

      static int spWidth;
      static int spHeight;

      static vector<StoringPosition*> setup( PG_Widget* parent, ContainerBase* container, HighLightingManager& highLightingManager, int& unitColumnCount );
      StoringPosition( PG_Widget *parent, const PG_Point &pos, HighLightingManager& highLightingManager, const ContainerBase::Cargo& storageVector, int number, bool regularPosition  );
      void eventBlit (SDL_Surface *surface, const PG_Rect &src, const PG_Rect &dst);
};


class CargoWidget : public PG_ScrollWidget {
      ContainerBase* container;
      int unitColumnCount;
      void moveSelection( int delta );
      
      vector<StoringPosition*> storingPositionVector;
      void checkStoringPosition( int oldpos, int newpos );
      HighLightingManager unitHighLight;

      void click( int num, SPoint mousePos );
      
   public:
      CargoWidget( PG_Widget* parent, const PG_Rect& pos, ContainerBase* container );
      bool eventKeyDown(const SDL_KeyboardEvent* key);
      Vehicle* getMarkedUnit();
      SigC::Signal1<void,Vehicle*> unitMarked;
      SigC::Signal2<void,Vehicle*,SPoint> unitClicked;
      void redrawAll();
};


#endif

