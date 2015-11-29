/***************************************************************************
                          paradialog.h  -  description
                             -------------------
    begin                : Thu Feb 21 2002
    copyright            : (C) 2002 by Martin Bickel
    email                : bickel@asc-hq.org
 ***************************************************************************/

/*! \file paradialog.h
    \brief Dialog classes based on the Paragui library
*/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef overviewmappanelH
 #define overviewmappanelH


#include "global.h"
#include "windowing.h"

class MapDisplayPG;


class OverviewMapPanel : public LayoutablePanel {
        MapDisplayPG* mapDisplayWidget;
        PG_Widget* ovmap;
        float currentZoom;
        bool locked;
     public:
        OverviewMapPanel( PG_Widget *parent, const PG_Rect &r, MapDisplayPG* mapDisplay, const ASCString& widgetName ) ;
     protected:
        void painter ( const PG_Rect &src, const ASCString& name, const PG_Rect &dst);
        void redraw() { Redraw(true); };
        bool mouseButtonDown ( PG_MessageObject* o, const SDL_MouseButtonEvent *button);
        bool mouseMotion ( PG_MessageObject* o, const SDL_MouseMotionEvent *motion);
        bool mouseClick ( SPoint pos );

   private:
      void lockPanel();
      void unlockPanel();

      
};


#endif
