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

#ifndef paradialogH
 #define paradialogH

#include "global.h"

#include <sigc++/sigc++.h>

#include <paragui.h>
#include <pgapplication.h>
#include <pgmessagebox.h>
#include <pgdropdown.h>
#include <pgcolor.h>
#include <pgrichedit.h>
#include <pgtimerobject.h>

#include "pgbutton.h"
#include "pgwidgetlist.h"
#include "pglabel.h"
#include "pgwindow.h"
#include "pgmaskedit.h"
#include "pgscrollbar.h"
#include "pgprogressbar.h"
#include "pgradiobutton.h"
#include "pgthemewidget.h"
#include "pgcheckbutton.h"
#include "pgslider.h"
#include "pglistbox.h"
#include "pgcolumnitem.h"
#include "pgpopupmenu.h"
#include "pgspinnerbox.h"
#include "pglog.h"
#include "pgmenubar.h"
#include "pgrichedit.h"
#include "pgdropdown.h"

#include "sdl/graphics.h"
#include "ascstring.h"
#include "textfile_evaluation.h"
#include "graphics/drawing.h"
#include "messaginghub.h"

class AutoProgressBar;

class StartupScreen: public SigC::Object {
       PG_Label* infoLabel;
       PG_Label* versionLabel;
       AutoProgressBar* progressBar; 
       SDL_Surface* fullscreenImage;
        void disp( const ASCString& s );
     public:
         StartupScreen( const ASCString& filename, SigC::Signal0<void>& ticker );        
         ~StartupScreen();
};

 class ASC_PG_App : public PG_Application {
       ASCString themeName;
       bool fullScreen;
       int bitsperpixel;
       
       void messageDialog( const ASCString& message, MessagingHubBase::MessageType mt );
       
    protected:
       bool eventQuit(int id, PG_MessageObject* widget, unsigned long data);
	    void eventIdle();
       
    public:
       static const int mapDisplayID = 2;
       static const int mainScreenID = 1;
    
       ASC_PG_App ( const ASCString& themeName );
       bool InitScreen ( int w, int h, int depth = 0, Uint32 flags = SDL_SWSURFACE|SDL_HWPALETTE );
       
       void reloadTheme();
       int Run ();
       void Quit ();

       bool isFullscreen() { return fullScreen; };
       bool toogleFullscreen();

       
       void processEvent();
       bool enableLegacyEventHandling( bool use );
       
       // SigC::Signal0<void> sigQuit;
      // PG_Theme* LoadTheme(const char* xmltheme, bool asDefault = true, const char* searchpath = NULL );
 };

 extern ASC_PG_App& getPGApplication();
 
 //! Adapter class for using Paragui Dialogs in ASC. This class transfers the event control from ASC to Paragui and back. All new dialog classes should be derived from this class
class ASC_PG_Dialog : public PG_Window {
       // SDL_Surface* background;
    private:
       // int quitModalLoopValue;       
    protected:
      PG_MessageObject* caller;
      bool closeWindow();
      void quitModalLoop(int value ); 
      virtual bool eventKeyDown(const SDL_KeyboardEvent *key);
    public:
       ASC_PG_Dialog ( PG_Widget *parent, const PG_Rect &r, const ASCString& windowtext, WindowFlags flags=DEFAULT, const ASCString& style="Window", int heightTitlebar=25);
};


class DropDownSelector: public PG_DropDown {
        bool first;
     protected:
        bool itemSelected( ); // PG_ListBoxBaseItem* i, void* p );   
     public:
        DropDownSelector( PG_Widget *parent, const PG_Rect &r=PG_Rect::null, int id=-1, const std::string &style="DropDown");
        void AddItem (const std::string &text, void *userdata=NULL, Uint16 height=0);
        SigC::Signal1<void, int> selectionSignal;
};


class ColoredBar : public PG_ThemeWidget {
   public:
      ColoredBar( PG_Color col,  PG_Widget *parent, const PG_Rect &r ) : PG_ThemeWidget( parent, r )
      {
         SetGradient ( PG_Gradient( col,col,col,col ));
         SetBorderSize(0);
         SetBackgroundBlend ( 255 );
      };
};


class BarGraphWidget;



class SpecialDisplayWidget : public PG_Widget {
   public:

      typedef SigC::Signal3<void,const PG_Rect&, const ASCString&, const PG_Rect&> DisplayHook;
      
      DisplayHook display;
      
      SpecialDisplayWidget (PG_Widget *parent, const PG_Rect &rect ) : PG_Widget( parent, rect, false )
      {
      }
      
      
      void eventBlit (SDL_Surface *surface, const PG_Rect &src, const PG_Rect &dst) {
         display( src, GetName(), dst );
      };
      /*
      void eventDraw (SDL_Surface *surface, const PG_Rect &rect) {
         Surface s = Surface::Wrap( surface );
         display( s, rect, GetID(), *this );
      };
      */

};

class BarGraphWidget : public PG_ThemeWidget {
      float fraction;
      PG_Color color;
   public:
      typedef vector<pair<double,int> > Colors;
      enum Direction { l2r, r2l, t2b, b2t };
   private:
      Direction dir;
      Colors colors;
   public:
      BarGraphWidget (PG_Widget *parent, const PG_Rect &rect, Direction direction );

      void setFraction( float f );
      void setColor( int c ) { color = c; };
      void setColor( PG_Color c ) { color = c; };
      void setColor( Colors colors ) { this->colors = colors; };

      void eventBlit (SDL_Surface *surface, const PG_Rect &src, const PG_Rect &dst);
};


class SpecialInputWidget : public PG_Widget {
   public:
     
      SpecialInputWidget (PG_Widget *parent, const PG_Rect &rect ) : PG_Widget( parent, rect, false ) { SetTransparency(255); };
      void eventBlit (SDL_Surface *surface, const PG_Rect &src, const PG_Rect &dst) { };
};

class Emboss : public PG_Widget {
      bool inv;
   public:

      Emboss (PG_Widget *parent, const PG_Rect &rect, bool inverted = false ) : PG_Widget( parent, rect, false ), inv(inverted)
      {
      }


      void eventBlit (SDL_Surface *surface, const PG_Rect &src, const PG_Rect &dst) {
         Surface s = Surface::Wrap( PG_Application::GetScreen() );
         if ( inv )
            rectangle<4> ( s, SPoint(dst.x, dst.y), dst.w, dst.h, ColorMerger_Brightness<4>( 0.7 ), ColorMerger_Brightness<4>( 1.4 ));
         else
            rectangle<4> ( s, SPoint(dst.x, dst.y), dst.w, dst.h, ColorMerger_Brightness<4>( 1.4 ), ColorMerger_Brightness<4>( 0.7 ));
      };
};



class MessageDialog;

class PG_StatusWindowData : public StatusMessageWindowHolder::UserData {
      MessageDialog* md;
   public:
      PG_StatusWindowData( const ASCString& msg );
      ~PG_StatusWindowData() ;
};


extern int  new_choice_dlg(const ASCString& title, const ASCString& leftButton, const ASCString& rightButton );

   


#endif
