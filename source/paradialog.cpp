/***************************************************************************
                          paradialog.cpp  -  description
                             -------------------
    begin                : Thu Feb 21 2002
    copyright            : (C) 2002 by Martin Bickel
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


#include "global.h"

#include <SDL_image.h>
#include <signal.h>

#include <paragui.h>
#include <pgapplication.h>
#include <pgmessagebox.h>
#include <pgdropdown.h>
#include "pgbutton.h"
#include "pglabel.h"
#include "pgwindow.h"
#include "pgscrollbar.h"
#include "pgprogressbar.h"
#include "pgradiobutton.h"
#include "pgthemewidget.h"
#include "pgcheckbutton.h"
#include "pgslider.h"
#include "pglistbox.h"
#include "pgpopupmenu.h"
#include "pgmenubar.h"
#include "pgimage.h"
#include "pgmessagebox.h"
#include "pgwindow.h"
#include "pgrichedit.h"
#include "pgsdleventsupplier.h"
#include "pgmultilineedit.h"
#include "pgtooltiphelp.h"
#include "pglog.h"

#include "widgets/multilistbox.h"
#include "widgets/textrenderer.h"
#include "widgets/autoprogressbar.h"

#include "dialogs/messagedialog.h"

#include "paradialog.h"
#include "events.h"
#include "gameoptions.h"
#include "spfst.h"
#include "strtmesg.h"

#include "graphics/drawing.h"

#include "messaginghub.h"


class EventSupplier: public PG_SDLEventSupplier {
   public:

	/**
	Polls for currently pending events, and returns true if there are any pending events, or false if there are none available. 
	If event is not NULL, the next event is removed from the queue and stored in that area.

	@param	event	pointer to an event structure
	@return		true - events are available
	*/
	bool PollEvent(SDL_Event* event) {
            bool result = getQueuedEvent( *event );
            if ( result ) 
               CombineMouseMotionEvents( event );
            return result;
        };

	/**
	Checks if an event is in the queue. If there is, it will be copied into the event structure, 
	WITHOUT being removed from the event queue. 

	@param event pointer to an event structure
	@return  true - events are available
	*/
	bool PeepEvent(SDL_Event* event) {
            return peekEvent( *event );
        }
        
	/**
	Waits indefinitely for the next available event.

	@param event 	pointer to an event structure
	@return		return 0 if there was an error while waiting for events        
	*/
	int WaitEvent(SDL_Event* event)
        {
            while ( !getQueuedEvent( *event ))
               releasetimeslice();
            CombineMouseMotionEvents( event );
            return 1;  
        };

} eventSupplier;






ASC_PG_App* pgApp = NULL;


void signalQuit( int i )
{
   getPGApplication().Quit();
}


ASC_PG_App :: ASC_PG_App ( const ASCString& themeName )  : fullScreen(false), bitsperpixel(0)
{
   this->themeName = themeName;
   EnableSymlinks(true);
   EnableAppIdleCalls();
   sigAppIdle.connect(  idleEvent ); // I don't get a direct connection to work
   // sigAppIdle.connect( SigC::slot( &idler ));
   int i = 0;
   bool themeFound = false;
   ASCString path;
   do {
      path = getSearchPath ( i++ );
      if ( !path.empty() ) {
         AddArchive ( path );
         if ( !themeFound ){
            ASCString arch = path + themeName + ".zip";
            themeFound = AddArchive ( arch.c_str() );
            if ( themeFound ) 
               displayLogMessage( 4, "found dialog theme at " + arch );
            
         }
      }
   } while ( !path.empty() );
   PG_LogConsole::SetLogLevel ( PG_LOG_ERR );

   if ( !themeFound ) 
      displayLogMessage( 2, "did not found dialog theme!!" );

   reloadTheme();

   pgApp = this;
   SetEventSupplier ( &eventSupplier );
   
   signal ( SIGINT, &signalQuit );
   
   PG_LineEdit::SetBlinkingTime( 500 );

   SetHighlightingTag( '~' );
   
}

void ASC_PG_App :: setIcon( const ASCString& filename )
{
   SDL_Surface *icn = NULL;
   try {
      tnfilestream iconl ( filename, tnstream::reading );
      icn = IMG_Load_RW ( SDL_RWFromStream( &iconl ), 1);
      // SDL_SetColorKey(icn, SDL_SRCCOLORKEY, *((Uint8 *)icn->pixels));
      SDL_WM_SetIcon( icn, NULL );
   } catch ( ... ) {}
}

void ASC_PG_App :: Quit()
{
   sigQuit(this);
   PG_Application::Quit();
}


bool ASC_PG_App::eventQuit(int id, PG_MessageObject* widget, unsigned long data)
{
   sigQuit(this);
   return PG_Application::eventQuit( id, widget, data );
}

void ASC_PG_App::eventIdle()
{
   if ( redrawScreen  ) {
      PG_Widget::UpdateScreen();
	   PG_Application::UpdateRect(PG_Application::GetScreen(), 0,0,0,0);
      redrawScreen = false;
   }

   PG_Application::eventIdle();
}

#ifdef WIN32
#include "sdl/graphicsqueue.h"
#endif

bool ASC_PG_App::toggleFullscreen()
{
   if ( !GetScreen() )
      return false;

   int w = GetScreen()->w;
   int h = GetScreen()->h;

   queueOperation( new MouseVisibility( false ), true );

   int flags = SDL_SWSURFACE;
   if ( !fullScreen )
      flags |= SDL_FULLSCREEN;

   SDL_Surface* screen = SDL_SetVideoMode(w, h, bitsperpixel, flags);
   if (screen == NULL) {
      screen = SDL_SetVideoMode(w, h, bitsperpixel, 0);
      fullScreen = false;
   } else
      fullScreen = !fullScreen;

   SetScreen(screen);
   PG_Widget::UpdateScreen();

   queueOperation( new MouseVisibility( true ), true );

   return true;
}


ASC_PG_App& getPGApplication()
{
   return *pgApp;
}


     
StartupScreen::StartupScreen( const ASCString& filename, SigC::Signal0<void>& ticker ) : infoLabel(NULL), versionLabel(NULL), progressBar(NULL), fullscreenImage(NULL)
{
   MessagingHub::Instance().statusInformation.connect( SigC::slot( *this, &StartupScreen::disp ));
   
   tnfilestream s ( filename, tnstream::reading );
   fullscreenImage = IMG_Load_RW( SDL_RWFromStream( &s ), true );
   if ( fullscreenImage ) {
      PG_Application::GetApp()->SetBackground( fullscreenImage, PG_Draw::STRETCH );
      PG_Application::GetApp()->EnableBackground(true);
   }
   int progressHeight = 15;
   SDL_Surface* screen = PG_Application::GetApp()->GetScreen();
   progressBar = new AutoProgressBar( ticker, NULL, PG_Rect( 0, screen->h - progressHeight, screen->w, progressHeight ) );
   progressBar->Show();
   infoLabel = new PG_Label( NULL, PG_Rect( screen->w/2, screen->h - progressHeight - 25, screen->w/2 - 10, 20 ));
   infoLabel->SetAlignment( PG_Label::RIGHT );
   infoLabel->Show();
   if ( MessagingHub::Instance().getVerbosity() > 0 ) {
      versionLabel = new PG_Label( NULL, PG_Rect( 10, screen->h - progressHeight - 25, screen->w/2, 20 ));
      versionLabel->SetAlignment( PG_Label::LEFT );
      versionLabel->SetText( getVersionString() );
      versionLabel->Show();
   }
}

void StartupScreen::disp( const ASCString& s )
{
   infoLabel->SetText( s );
}

         
StartupScreen::~StartupScreen()
{
   PG_Application::GetApp()->DeleteBackground(); 
   SDL_FreeSurface( fullscreenImage );   
   progressBar->close();
   delete progressBar;
   delete infoLabel;
   delete versionLabel;
}



#if 0

#include "SDL_syswm.h"

static void SDL_center_window(SDL_Surface *screen)
{
   SDL_SysWMinfo info;
   SDL_VERSION(&info.version);
	
   if ( SDL_GetWMInfo(&info) > 0 ) {
      int x, y;
      int w, h;
		
#ifdef unix
		
      if ( info.subsystem == SDL_SYSWM_X11 ) {
         info.info.x11.lock_func();
         w = DisplayWidth(info.info.x11.display,
                     DefaultScreen(info.info.x11.display));
         h = DisplayHeight(info.info.x11.display,
                     DefaultScreen(info.info.x11.display));
         x = (w - screen->w)/2;
         y = (h - screen->h)/2;
         XMoveWindow(info.info.x11.display, info.info.x11.wmwindow, x, y);
         info.info.x11.unlock_func();
      }
		
#elif defined(WIN32)
		
      RECT windowRect, desktopRect;
		
      HWND desktop = GetDesktopWindow();
      ::GetWindowRect(desktop, &desktopRect);
      ::GetWindowRect(info.window, &windowRect);
		
      int desktopWidth = desktopRect.right - desktopRect.left;
      int desktopHeight = desktopRect.bottom - desktopRect.top;
      w = windowRect.right - windowRect.left;
      h = windowRect.bottom - windowRect.top;
      x = (desktopWidth - w) / 2;
      y = (desktopHeight - h) / 2;
		
      ::MoveWindow(info.window, x, y, w, h, true);
#else
		
#warning Need to implement these functions for other systems
		
#endif
    }
}

#endif

bool ASC_PG_App:: InitScreen ( int w, int h, int depth, Uint32 flags )
{
   bitsperpixel = depth;
   bool result = PG_Application::InitScreen ( w, h, depth, flags  );
   if ( result ) {
      initASCGraphicSubsystem ( GetScreen(), NULL );
      Surface::SetScreen( GetScreen() );

      fullScreen = flags & SDL_FULLSCREEN;
      
      MessagingHub::Instance().error.connect( SigC::bind( SigC::slot( *this, &ASC_PG_App:: messageDialog ), MessagingHubBase::Error ));
      MessagingHub::Instance().fatalError.connect( SigC::bind( SigC::slot( *this, &ASC_PG_App:: messageDialog ), MessagingHubBase::FatalError ));
      MessagingHub::Instance().warning.connect(SigC::bind( SigC::slot( *this, &ASC_PG_App:: messageDialog ), MessagingHubBase::Warning ));
      MessagingHub::Instance().infoMessage.connect( SigC::bind( SigC::slot( *this, &ASC_PG_App:: messageDialog ), MessagingHubBase::InfoMessage ));
   }

   return result;
}


void ASC_PG_App :: reloadTheme()
{
   if ( !LoadTheme(themeName ))
      fatalError ( "Could not load Paragui theme for ASC: " + themeName );
}


bool ASC_PG_App :: enableLegacyEventHandling( bool use )
{
   return !setEventRouting ( !use, use );
}


void ASC_PG_App::processEvent( )
{
   SDL_Event event;
	if ( GetEventSupplier()->PollEvent(&event)) 
		PumpIntoEventQueue(&event);
}


int ASC_PG_App::Run ( )
{
   enableLegacyEventHandling ( false );

   PG_Application::Run();
   
   enableLegacyEventHandling ( true );
   return 0;
}

ASC_PG_App :: ~ASC_PG_App()
{
   shutdownASCGraphicSubsystem();
}



ASC_PG_Dialog :: ASC_PG_Dialog ( PG_Widget *parent, const PG_Rect &r, const ASCString& windowtext, WindowFlags flags, const ASCString& style, int heightTitlebar )
   :PG_Window ( parent, centerWindow(r), windowtext, flags, style, heightTitlebar ),stdButtonNum(0), caller(0), standardButtonDir( Vertical )
{

   // it looks nice if you can see the map behind the dialog, but seeing other dialogs stacked above each other is just confusing, so we reduce transparency
   int t = GetTransparency();
   if ( WindowCounter::num() >= 1 ) {
      SetTransparency ( t/2 );
   }
}

int WindowCounter::windowNum = 0;


int ASC_PG_Dialog::RunModal()
{
   WindowCounter wc;
   
   return PG_Window::RunModal();
}


PG_Rect ASC_PG_Dialog::centerWindow( const PG_Rect& rect )
{
   PG_Rect r = rect;

   if ( r.w >  PG_Application::GetScreenWidth() )
      r.w = PG_Application::GetScreenWidth();
         
   if ( r.h >  PG_Application::GetScreenHeight() )
      r.h = PG_Application::GetScreenHeight();

   
   if ( r.x < 0 )
      r.x = (PG_Application::GetScreenWidth() - r.w) / 2;

   if ( r.y < 0 )
      r.y = (PG_Application::GetScreenHeight() - r.h) / 2;
   return r;
}

void ASC_PG_Dialog::StandardButtonDirection ( StandardButtonDirectonType dir )
{
   standardButtonDir = dir;
}

PG_Button* ASC_PG_Dialog::AddStandardButton( const ASCString& name )
{
   ++stdButtonNum;
   if ( standardButtonDir == Vertical )
      return new PG_Button( this, PG_Rect( Width() - 110, Height() - stdButtonNum * 40, 100, 30 ), name );
   else
      return new PG_Button( this, PG_Rect( Width() - 110 * stdButtonNum, Height() -40 , 100, 30 ), name );
}



bool ASC_PG_Dialog::eventKeyDown(const SDL_KeyboardEvent *key){
  if(key->keysym.sym == SDLK_ESCAPE) {
        closeWindow();
    }
  return true;
}



bool ASC_PG_Dialog::quitModalLoop(int value )
{
   SetModalStatus( value );
   PG_Window::QuitModal();
   return true;
}


/*
bool ASC_PG_Dialog::eventKeyUp (const SDL_KeyboardEvent *key){
if(key->keysym.sym == SDLK_ESCAPE){
   closeWindow();
 }
 return true;

}


bool ASC_PG_Dialog::eventKeyUp (const SDL_KeyboardEvent *key){
if(key->keysym.sym == SDLK_ESCAPE){
   closeWindow();
 }else if(key->keysym.sym == SDLK_DOWN){
     
 }else if(key->keysym.sym == SDLK_UP){
     
 }else if(key->keysym.sym == SDLK_RIGHT){
     
 }else if(key->keysym.sym == SDLK_LEFT){
     
 }
 return true;

}*/


bool ASC_PG_Dialog::closeWindow(){
  PG_Window::QuitModal();
  if( caller != 0){     
    caller->SetInputFocus();
  }
  return true;
}







void ASC_PG_App:: messageDialog( const ASCString& message, MessagingHubBase::MessageType mt )
{
   ASCString title;
   ASCString style;
   switch ( mt ) {
      case MessagingHubBase::Error: 
         title = "Error"; 
         style = "ErrorMessage";
         break;
      case MessagingHubBase::Warning: 
         title = "Warning"; 
         style = "WarningMessage";
         break;
      case MessagingHubBase::InfoMessage: 
         title = "Information"; 
         style = "Window";
         break;
      case MessagingHubBase::FatalError: 
         title = "Fatal Error"; 
         style = "FatalErrorMessage";
         break;
      default: break;
   };
      
   PG_Rect size = calcMessageBoxSize(message);
   MessageDialog msg( NULL, size, title, message,"OK", PG_Label::CENTER, style );
   msg.Show();
   msg.RunModal();
}


PG_StatusWindowData::PG_StatusWindowData( const ASCString& msg ) 
{
   md = new MessageDialog( NULL, calcMessageBoxSize( msg ), "status", msg, PG_Label::CENTER, "Window" );
   md->Show();
};

PG_StatusWindowData::~PG_StatusWindowData() 
{
   delete md;
};   


class   NewStringChooser : public ASC_PG_Dialog {
   PG_ListBox* listbox;
   int button;
   int item;
   
   bool buttonpressed( int i )
   {
      button = i;
      QuitModal();
      return true;
   }

   bool itemSelected( PG_ListBoxBaseItem* l )
   {
      PG_ListBoxDataItem<int>* listitem = dynamic_cast<PG_ListBoxDataItem<int>*>( l );
      if ( listitem ) {
         item = listitem->getData();
         return true;
      } else
         return false;
   }
   
   public :
      NewStringChooser ( const ASCString& _title, const vector<ASCString>& _strings , const vector<ASCString>& _buttons, int defaultEntry ) : ASC_PG_Dialog( NULL, PG_Rect( -1, -1, 400, 300 ), _title ), button(-1), item(-1)
      {
         listbox = new PG_ListBox( this, PG_Rect( 10, 30, Width()-140, Height() - 40) );
         listbox->SetMultiSelect( false );
         listbox->sigSelectItem.connect( SigC::slot( *this, &NewStringChooser::itemSelected ));

         int counter = 0;
         for ( vector<ASCString>::const_iterator i = _strings.begin(); i != _strings.end(); ++i ) {
           PG_ListBoxDataItem<int>* listitem = new PG_ListBoxDataItem<int>(listbox, 20, *i, counter );
           if ( counter == defaultEntry )
              listitem->Select();
            ++counter;
         }

         counter = 0;
         for ( vector<ASCString>::const_iterator i = _buttons.begin(); i != _buttons.end(); ++i ) {
            AddStandardButton(*i)->sigClick.connect( SigC::bind( SigC::slot( *this, & NewStringChooser::buttonpressed ),counter ));
            ++counter;
         }
      }

      int getButton()
      {
         return button;
      }
      int getItem()
      {
         return item;
      }
         
};


pair<int,int> new_chooseString ( const ASCString& title, const vector<ASCString>& entries, const vector<ASCString>& buttons, int defaultEntry  )
{
   NewStringChooser nsc ( title, entries, buttons, defaultEntry );
   nsc.Show();
   nsc.RunModal();
   return make_pair(nsc.getButton(), nsc.getItem() );
}



class MultiLineEditorDialog  : public ASC_PG_Dialog {
      PG_MultiLineEdit* editor;

   public:
      MultiLineEditorDialog( const ASCString& title, const ASCString& textToEdit ) : ASC_PG_Dialog( NULL, PG_Rect( -1, -1, 400, 400 ), title), editor(NULL) 
      {
         editor = new PG_MultiLineEdit( this, PG_Rect( 10, 40, Width() - 20, Height() - 80 ) );
         editor->SetText( textToEdit );
         AddStandardButton( "OK" )->sigClick.connect( SigC::bind( SigC::slot( *this, &MultiLineEditorDialog::quitModalLoop ), 1 ));
      }

      ASCString GetEditedText() { return editor->GetText(); };
};

bool MultiLineEditor( const ASCString& title, ASCString& textToEdit )
{
   MultiLineEditorDialog mle ( title, textToEdit );
   mle.Show();
   if ( mle.RunModal() ) {
      textToEdit = mle.GetEditedText();
      return true;
   } else
      return false;

}

