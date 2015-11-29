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
      sigc::signal<void,int,int> markChanged;
      void setNew(int pos );
      sigc::signal<bool> redrawAll;
      //! the bool param is set to true if this is the first click on a unit 
      sigc::signal<void,int, SPoint, bool> clickOnMarkedUnit;
};

class CargoWidget;

class StoringPosition : public PG_Widget
{
      HighLightingManager& highlight;
      const ContainerBase::Cargo& storage;
      int num;
      bool regular;
      CargoWidget* cargoWidget;

      PG_Point unitPosition;
      PG_Point dragPointStart;
      PG_Point mouseCursorOffset;
            
      static int spWidth;
      static int spHeight;

      static PG_Rect CalcSize( const PG_Point& pos  );
      
      enum DragState { Off, Pressed, Dragging } dragState;
      
      Surface clippingSurface;
      
   protected:
      void markChanged(int old, int mark);

      void eventDraw (SDL_Surface *surface, const PG_Rect &src);
      void eventBlit (SDL_Surface *surface, const PG_Rect &src, const PG_Rect &dst);
      
      void setBargraphValue( const ASCString& widgetName, float fraction);
      void setLabelText ( const ASCString& widgetName, const ASCString& text, PG_Widget* parent = NULL );
      bool eventMouseButtonDown(const SDL_MouseButtonEvent* button);
      bool eventMouseButtonUp(const SDL_MouseButtonEvent* button);
      bool eventMouseMotion (const SDL_MouseMotionEvent *motion);
   public:
      enum DragTarget { NoDragging, TargetAvail, TargetNotAvail } ;
   protected:
      DragTarget dragTarget;
      
   public:
      void setDragTarget( DragTarget dragTarget ) { this->dragTarget = dragTarget; };
      
      static vector<StoringPosition*> setup( PG_Widget* parent, ContainerBase* container, HighLightingManager& highLightingManager, int& unitColumnCount );
      Vehicle* getUnit();
      StoringPosition( PG_Widget *parent, const PG_Point &pos, const PG_Point& unitPos, HighLightingManager& highLightingManager, const ContainerBase::Cargo& storageVector, int number, bool regularPosition, CargoWidget* cargoWidget = NULL  );
};


class CargoWidget : public PG_ScrollWidget {
      ContainerBase* container;
      bool dragNdrop;
      int unitColumnCount;
      Vehicle* draggedUnit;
      void moveSelection( int delta );
      
      typedef vector<StoringPosition*> StoringPositionVector;
      StoringPositionVector storingPositionVector;
      
      void checkStoringPosition( int oldpos, int newpos );
      HighLightingManager unitHighLight;

      void click( int num, SPoint mousePos, bool first );
   protected:
      bool 	handleScrollTrack (long data);
      
   public:
      CargoWidget( PG_Widget* parent, const PG_Rect& pos, ContainerBase* container, bool setup );
      bool eventKeyDown(const SDL_KeyboardEvent* key);
      Vehicle* getMarkedUnit();
      sigc::signal<void,Vehicle*> unitMarked;

      //! the bool param is set to true if this is the first click on a unit 
      sigc::signal<void,Vehicle*,SPoint,bool> unitClicked;
      void redrawAll();
      
      void startDrag( Vehicle* v );
      void releaseDrag( Vehicle* v = NULL );
      void releaseDrag( int x, int y );
      
      //! First param: dragged unit, Second Param: target unit
      sigc::signal<void, Vehicle*, Vehicle*> sigDragDone;
      
      //! First param: dragged unit, Second Param: target unit
      sigc::signal<bool, Vehicle*, Vehicle*> sigDragAvail;
      
      sigc::signal<void> sigDragInProcess;
      sigc::signal<void> sigDragAborted;

      sigc::signal<void> sigScrollTrack;

      void enableDragNDrop( bool enable ) { dragNdrop = enable; };
      bool dragNdropEnabled() const { return dragNdrop; }; 
      void registerStoringPositions( vector<StoringPosition*> sp, const int& colcount );
      HighLightingManager& getHighLightingManager() { return unitHighLight; };
};


#endif

