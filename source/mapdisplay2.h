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
#include "libs/loki/Functor.h"

 
#include "typen.h"
#include "basegfx.h"
#include "events.h"
#include "soundList.h"


#include "paradialog.h"


class MapRenderer {
    
      struct Icons {
         Surface mapBackground;
         Surface notVisible;
         Surface markField;
      };
      static Icons icons;

     
      void readData();

    protected:
      MapRenderer() { readData(); };

      static const int surfaceBorder = 60;
            
      typedef struct {
         int x1,y1,x2,y2;
      } ViewPort;

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
      SPoint getFieldPos2( const MapCoordinate& pos ) {
          return SPoint( getFieldPosX(pos.x,pos.y), getFieldPosY(pos.x,pos.y));
      };
      
      void paintSingleField( Surface& surf, int playerView, pfield fld, int layer, const SPoint& pos, const MapCoordinate& mc );
      void paintTerrain( Surface& surf, tmap* actmap, int playerView, const ViewPort& viewPort, const MapCoordinate& offset );

    public:  
      typedef Loki::Functor<SPoint, TYPELIST_2(int,int)> PositionCalculator;
      static SigC::Signal3<void,Surface&, int, PositionCalculator> additionalItemDisplayHook;
};


class MapDisplayPG: public PG_Widget, protected MapRenderer {
      
      struct Icons {
         Surface cursor;
         Surface fieldShape;
      };
      static Icons icons;

      void readData();

            
      float zoom;
      Surface* surface;
      
      MapCoordinate offset;
     
      void setupFastBlitterCommands();
      struct {
         ViewPort viewPort;
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
      void fillSurface( int playerView );


      MapCoordinate screenPos2mapPos( const SPoint& pos );
      MapCoordinate widgetPos2mapPos( const SPoint& pos );

      SPoint mapPos2internalPos ( const MapCoordinate& pos );
      SPoint internal2widget( const SPoint& pos );
      SPoint widget2screen( const SPoint& pos );
      
      void blitInternalSurface( SDL_Surface* dest, const SPoint& pnt );

      
      static const int effectiveMovementSurfaceWidth = 4 * fielddisthalfx + fieldsizex;
      static const int effectiveMovementSurfaceHeight = 4*fieldsizey;
      
      struct {
         Surface mask;
         SPoint  startFieldPos;
      } movementMask[sidenum];
      static const int touchedFieldNum = 10;

    public:        
    
      class TouchedField {
         public:
            TouchedField( const MapCoordinate& real, const MapCoordinate& temp) : realMap(real), tempMap(temp) {};
            MapCoordinate realMap;
            MapCoordinate tempMap;
            bool operator==(const TouchedField& b );
      };
      
      
    protected:  

      struct Movement {
         Vehicle* veh;
         SPoint from;
         SPoint to;
         pmap actmap;
         Surface* surf;
         Surface* mask;
         int playerView;
         
        
         //! the area of the screen that is overwritten
         SPoint screenPos;
         SPoint screenUpdatePos;
         
         
         int screenWidth,screenHeight;
         int maxScreenWidth,maxScreenHeight;
         
         struct { 
            MapCoordinate mapPos;
            SPoint surfPos;
         } touchedFields[touchedFieldNum];
      
      };   
            
      void initMovementStructure();
      Surface createMovementBufferSurface();
      void displayMovementStep( Movement& movement, int percentage );
                  
   public:
      MapDisplayPG ( PG_Widget *parent, const PG_Rect r );
      
      void displayUnitMovement( pmap actmap, Vehicle* veh, const MapCoordinate3D& from, const MapCoordinate3D& to );

      bool fieldInView(const MapCoordinate& mc );
      
      //! repaints to the internal surface, but does not blit this surface the screen
      void updateMap( bool force = false );
      
      //! update the internal surface and blits it to the screen
      void updateWidget();

};

extern void benchMapDisplay();


#endif
