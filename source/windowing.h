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

#ifndef windowingH
#define windowingH

#include "global.h"

#include <sigc++/sigc++.h>

#include "loki/SmartPtr.h"

#include "paradialog.h"
#include "widgets/bargraphwidget.h"

class ASCGUI_Window : public  PG_Window {
      ASCString panelName;
      void destruct();
   protected:
      bool setup();

      class WidgetParameters
      {
         public:
            WidgetParameters();
            ASCString backgroundImage;
            PG_Draw::BkMode backgroundMode;
            
            PG_Label::TextAlign textAlign;
            bool textAlign_defined;
            
            int fontColor;
            bool fontColor_defined;
            
            ASCString fontName;
            ASCString style;
            int fontAlpha;
            bool fontAlpha_defined;
            
            int fontSize;
            bool fontSize_defined;
            
            int backgroundColor;
            bool backgroundColor_defined;
            
            int transparency;
            bool transparency_defined;

            int bordersize;
            bool bordersize_defined;
            bool hidden;

            void assign( PG_Widget* widget );
            void assign( BarGraphWidget* widget );
            void assign( PG_ThemeWidget* widget );
            void assign( PG_Label* widget );
            void runTextIO ( PropertyReadingContainer& pc );
      };

      virtual WidgetParameters getDefaultWidgetParams() = 0;

      typedef deallocating_map<ASCString, TextPropertyGroup*> GuiCache;
      static GuiCache guiCache;
      
      TextPropertyGroup* textPropertyGroup;
   public:
      void setLabelText ( const ASCString& widgetName, const ASCString& text, PG_Widget* parent = NULL );
      void setLabelText ( const ASCString& widgetName, int i, PG_Widget* parent = NULL );
      void setLabelColor ( const ASCString& widgetName, PG_Color color, PG_Widget* parent = NULL );
      void setImage ( const ASCString& widgetName, Surface& image, PG_Widget* parent = NULL );
      void setImage ( const ASCString& widgetName, SDL_Surface* image = NULL, PG_Widget* parent = NULL );
      void setImage ( const ASCString& widgetName, const ASCString& filename, PG_Widget* parent = NULL );
      void hide( const ASCString& widgetName, PG_Widget* parent = NULL );
      void show( const ASCString& widgetName, PG_Widget* parent = NULL );
      void setBargraphValue( const ASCString& widgetName, float fraction, PG_Widget* parent = NULL );
      void setBarGraphColor( const ASCString& widgetName, PG_Color color, PG_Widget* parent = NULL );
      void rename( const ASCString& widgetName, const ASCString& newname, PG_Widget* parent = NULL );
      void setWidgetTransparency ( const ASCString& widgetName, int transparency, PG_Widget* parent = NULL );
      
      int RunModal();
   protected:

      virtual void userHandler( const ASCString& label, PropertyReadingContainer& pc, PG_Widget* parent, WidgetParameters widgetParams ) {}; 

      static PG_Rect parseRect ( PropertyReadingContainer& pc, PG_Widget* parent );
      void parsePanelASCTXT ( PropertyReadingContainer& pc, PG_Widget* parent, WidgetParameters widgetParams );

      ASCGUI_Window ( PG_Widget *parent, const PG_Rect &r, const ASCString& panelName_, const ASCString& baseStyle = "Panel", bool loadTheme = true );
      // FIXME Close button does not delete Panel
      ~ASCGUI_Window();
};


class Panel : public ASCGUI_Window {
   protected:
      WidgetParameters getDefaultWidgetParams();
   public:
      Panel ( PG_Widget *parent, const PG_Rect &r, const ASCString& panelName_, bool loadTheme = true );
};


//! this Panel saves its position in the gameoptions and loads it on construction
class LayoutablePanel: public Panel {
   private:
      ASCString name;
      static PG_Rect PositionCalculator( const PG_Rect& r, const ASCString& panelName );
      static const int DashBoardWidth = 170;
   protected:
      void eventMoveWidget (int x, int y);
      void eventHide();
      void eventShow();
   public:
      LayoutablePanel ( PG_Widget *parent, const PG_Rect &r, const ASCString& panelName_, bool loadTheme = true );
   
};

class ConfigurableWindow : public ASCGUI_Window {
   protected:
      WidgetParameters getDefaultWidgetParams();
   public:
      ConfigurableWindow ( PG_Widget *parent, const PG_Rect &r, const ASCString& panelName_, bool loadTheme = true );
};


#endif
