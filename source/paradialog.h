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

#include <vector>
#include "global.h"

#include <sigc++/sigc++.h>

#include <paragui.h>
#include <pgapplication.h>
#include <pgmessagebox.h>
#include <pgdropdown.h>
#include <pgcolor.h>

#include "pgbutton.h"
#include "pgwidgetlist.h"
#include "pglabel.h"
#include "pgwindow.h"
#include "pgscrollbar.h"
#include "pgradiobutton.h"
#include "pgthemewidget.h"
#include "pgcheckbutton.h"
#include "pgslider.h"
#include "pglistbox.h"

#include "pgpropertyeditor.h"

#include "sdl/graphics.h"
#include "ascstring.h"
#include "textfile_evaluation.h"
#include "graphics/drawing.h"
#include "util/messaginghub.h"

class AutoProgressBar;

class StartupScreen: public sigc::trackable {
       vector<PG_Label*> infoLabels;
       PG_Label* versionLabel;
       PG_ThemeWidget* background;
       AutoProgressBar* progressBar; 
       Surface fullscreenImage;
       void disp( const ASCString& s );
       void dispLine( const ASCString& s, int line );
     public:
         StartupScreen( const ASCString& filename, sigc::signal<void>& ticker );        
         ~StartupScreen();
};

 class ASC_PG_App : public PG_Application {
       ASCString themeName;
       bool fullScreen;
       
       void messageDialog( const ASCString& message, MessagingHubBase::MessageType mt );

       typedef list<PG_MessageObject*> DeletionQueue;
       DeletionQueue deletionQueue;

       bool removeFromDeletionQueue( const PG_MessageObject* obj );

    protected:
       bool eventQuit(int id, PG_MessageObject* widget, unsigned long data);
	    void eventIdle();
       
    public:
       static const int mapDisplayID = 2;
       static const int mainScreenID = 1;
    
       ASC_PG_App ( const ASCString& themeName );
       bool InitScreen ( int w, int h, bool fullscreen );
       
       void reloadTheme();
       int Run ();
       void Quit ();

       bool isFullscreen() { return fullScreen; };

       void setIcon( const ASCString& filename );
       
       void processEvent();

       bool queueWidgetForDeletion( PG_Widget* widget );


       static sigc::signal<void,const SDL_Surface*> postScreenUpdate;

       ~ASC_PG_App();
 };

 extern ASC_PG_App& getPGApplication();

 class WindowCounter {
       static int windowNum;
    public:
       WindowCounter() { ++windowNum; };
       static int num() { return windowNum; };
       ~WindowCounter() { --windowNum; };
 };

 
 //! Adapter class for using Paragui Dialogs in ASC. This class transfers the event control from ASC to Paragui and back. All new dialog classes should be derived from this class
class ASC_PG_Dialog : public PG_Window {
    private:
       int stdButtonNum;
   protected:
      PG_MessageObject* caller;
      virtual bool closeWindow();
      bool quitModalLoopW(PG_Button* button, int value );
      bool quitModalLoop(int value );
      virtual bool eventKeyDown(const SDL_KeyboardEvent *key);
    public:
       ASC_PG_Dialog ( PG_Widget *parent, const PG_Rect &r, const ASCString& windowtext, WindowFlags flags=DEFAULT, const ASCString& style="Window", int heightTitlebar=25);
       PG_Button* AddStandardButton( const ASCString& name );
       enum StandardButtonDirectonType { Vertical, Horizontal };
       void StandardButtonDirection ( StandardButtonDirectonType dir );
       int RunModal();

       static PG_Rect centerRectangle( const PG_Rect& rect );

   private:
      StandardButtonDirectonType standardButtonDir;
};

class ASC_PropertyEditor : public PG_PropertyEditor {
   public:
      ASC_PropertyEditor(  PG_Widget *parent, const PG_Rect &r, const std::string &style="PropertyEditor", int labelWidthPercentage = 50 ) : PG_PropertyEditor( parent, r, style, labelWidthPercentage ) {};
      std::string GetStyleName( const std::string& widgetName ) {
         if ( widgetName == "DropDownSelectorProperty" )
            return "DropDown";
         else
            if ( widgetName == "BoolProperty" )
               return "CheckButton";
         else
            return PG_PropertyEditor::GetStyleName( widgetName );
      };
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



class SpecialDisplayWidget : public PG_Widget {
   public:

      typedef sigc::signal<void,const PG_Rect&, const ASCString&, const PG_Rect&> DisplayHook;
      
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

      void eventBlit (SDL_Surface *surface, const PG_Rect &src, const PG_Rect &dst);
};





class MessageDialog;

class PG_StatusWindowData : public StatusMessageWindowHolder::UserData {
      MessageDialog* md;
   public:
      PG_StatusWindowData( const ASCString& msg );
      void SetText( const ASCString& text );
      ~PG_StatusWindowData() ;
};

/** \return ButtonPressed,SelectedItem
*/
extern pair<int,int> new_chooseString ( const ASCString& title, const vector<ASCString>& entries, const vector<ASCString>& buttons, int defaultEntry = -1 );

extern bool MultiLineEditor( const ASCString& title, ASCString& textToEdit );

//! uses the new dialog engine
extern ASCString editString2( const ASCString& title, const ASCString& defaultValue = "" );

extern int choiceDialog(const ASCString& text, const ASCString& button1, const ASCString& button2, const ASCString& shortLabel );


class BulkGraphicUpdates {
      PG_Widget* parent;
      bool bulk;
      bool active;
   public:
      BulkGraphicUpdates( PG_Widget* parent = NULL );
      void release();
      ~BulkGraphicUpdates();
};


#endif
