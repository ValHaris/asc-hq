/***************************************************************************
                          mapdisplay.h  -  description
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

#ifndef mapdisplay2H
 #define mapdisplay2H

 #include <sigc++/sigc++.h>

 
#include "typen.h"
#include "basegfx.h"
#include "events.h"
#include "soundList.h"


#include "paradialog.h"


class MapDisplayPG: public PG_Widget {
      float zoom;
      MapCoordinate offset;
      Surface* surface;
      int surfaceBorder;
      
      struct Icons {
         Surface mapBackground;
         Surface notVisible;
         Surface cursor;
         Surface markField;
      };
      static Icons icons;
      
      void readData();
      
      void setupFastBlitterCommands();
      struct {
         struct {
            int x1,y1,x2,y2;
         } displayed;   
         int numx, numy;
      } field;   
      
      struct Cursor {
         Cursor() : visible(false) {};
         bool visible;
      } cursor;   
      
      enum Dirty { Nothing, Curs, Map } dirty;
      
   protected:
   
      // Paragui stuff
      void eventBlit(SDL_Surface* srf, const PG_Rect& src, const PG_Rect& dst);
      void eventDraw(SDL_Surface* surface, const PG_Rect& rect);


      bool eventMouseButtonDown (const SDL_MouseButtonEvent *button);
      bool eventMouseButtonUp (const SDL_MouseButtonEvent *button);
      
      //
   
   
      void checkViewPosition();
      
      void setNewZoom( float zoom );   
      void paintTerrain( int playerView );
      void fillSurface( int playerView );

      // return the position of a field on the internal surface      
      int getFieldPosX( int x, int y ) { 
          if (y & 1 )   // even lines are shifted to the right
             return surfaceBorder + fielddisthalfx + x * fielddistx;
          else
             return surfaceBorder + x * fielddistx;
      };             
      
      // return the position of a field on the internal surface      
      int getFieldPosY( int x, int y ) {
          return surfaceBorder + y * fielddisty;
      };    
      
      SPoint getFieldPos( int x, int y ) {
          return SPoint( getFieldPosX(x,y), getFieldPosY(x,y));
      };



      MapCoordinate screenPos2mapPos( const SPoint& pos );
      MapCoordinate widgetPos2mapPos( const SPoint& pos );

      SPoint mapPos2internalPos ( const MapCoordinate& pos );
      SPoint internal2widget( const SPoint& pos );
      SPoint widget2screen( const SPoint& pos );
      
      void blitInternalSurface( SDL_Surface* dest, const SPoint& pnt );
            
   public:
      MapDisplayPG ( PG_Widget *parent, const PG_Rect r );

      //! repaints to the internal surface, but does not blit this surface the screen
      void updateMap( bool force = false );
      
      //! update the internal surface and blits it to the screen
      void updateWidget();

};

extern void benchMapDisplay();


#endif
