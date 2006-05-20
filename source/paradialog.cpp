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

#include <boost/regex.hpp>

#include <paragui.h>
#include <pgapplication.h>
#include <pgmessagebox.h>
#include <pgdropdown.h>
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
#include "pgimage.h"
#include "pgmessagebox.h"
#include "pgwindow.h"
#include "pgrichedit.h"
#include "pgsdleventsupplier.h"
#include "pgmultilineedit.h"
#include "pgtooltiphelp.h"

#include "widgets/multilistbox.h"
#include "widgets/textrenderer.h"

#include "paradialog.h"
#include "events.h"
#include "gameoptions.h"
#include "spfst.h"
#include "strtmesg.h"


#include "resourceplacement.h"

#include "iconrepository.h"
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



class AutoProgressBar: public PG_ProgressBar {

      int starttime;
      int time;
      int lastticktime;
      int lastdisplaytime;
      int counter;
      vector<int> newTickTimes;
      vector<int> prevTickTimes;
      

      void tick()
      {
         
         newTickTimes.push_back ( ticker - starttime );
         
         // limit to 25 Hz to reduce graphic updates
         if ( lastdisplaytime + 4 < ticker ) {
            double p;
            // double p = double(ticker - starttime) * 100  / time;
            if ( counter < prevTickTimes.size() && time ) {
               int a = prevTickTimes[counter];
               p = 100 * a / time;
            } else
               p = counter / 100;
               
            if ( p > 99 )
               p = 99;
            
            SetProgress( p );
            lastdisplaytime = ticker;
         }
         
         ++counter;
         
            
         lastticktime = ticker;
      };

   public:
      AutoProgressBar( SigC::Signal0<void>& tickSignal, PG_Widget *parent, const PG_Rect &r=PG_Rect::null, const std::string &style="Progressbar" ) : PG_ProgressBar( parent, r, style ), lastticktime(-1), counter(0)
      {
         lastdisplaytime = starttime = ticker;
         
         tickSignal.connect( SigC::slot( *this, &AutoProgressBar::tick ));
         
         try {
            tnfilestream stream ( "progress.dat", tnstream::reading  );
            stream.readInt(); // version
            time = stream.readInt( );
            readClassContainer( prevTickTimes, stream );
         }
         catch ( ... ) {
            time = 200;
         };   
      };
      
      void close( )
      {
         try {
            tnfilestream stream ( "progress.dat", tnstream::writing  );
            stream.writeInt( 1 );
            stream.writeInt( lastticktime - starttime );
            writeClassContainer( newTickTimes, stream );
         }
         catch ( ... ) {
         }
      }
};



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



bool ASC_PG_App::toogleFullscreen()
{
   if ( !GetScreen() )
      return false;

   int w = GetScreen()->w;
   int h = GetScreen()->h;

   SDL_ShowCursor(0);

   int flags = 0;
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

   SDL_ShowCursor(1);

   return true;
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
         XMoveWindow(info.info.x11.display, info.info.x11.wmwindow, x,
y);
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
   :PG_Window ( parent, centerWindow(r), windowtext, flags, style, heightTitlebar ),stdButtonNum(0), caller(0)
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
   if ( r.x < 0 )
      r.x = (PG_Application::GetScreenWidth() - r.w) / 2;

   if ( r.y < 0 )
      r.y = (PG_Application::GetScreenHeight() - r.h) / 2;
   return r;
}

PG_Button* ASC_PG_Dialog::AddStandardButton( const ASCString& name )
{
   ++stdButtonNum;
   return new PG_Button( this, PG_Rect( Width() - 110, Height() - stdButtonNum * 40, 100, 30 ), name );
}


#if 0
int ASC_PG_Dialog::Run ( )
{
#ifndef sgmain
   bool eventQueue = setEventRouting ( true, false );
#endif
/*
   while ( !quitModalLoopValue ) {
      SDL_Event event;
      int motionx = 0;
      int motiony = 0;
      if ( getQueuedEvent( event )) {
         bool skipEvent = false;
         if ( event.type == SDL_MOUSEMOTION ) {
            SDL_Event nextEvent;
            if ( peekEvent ( nextEvent ) )
               if ( nextEvent.type == SDL_MOUSEMOTION ) {
                  skipEvent = true;
                  motionx += event.motion.xrel;
                  motiony += event.motion.yrel;
               }
            if ( !skipEvent ) {
               event.motion.xrel += motionx;
               event.motion.yrel += motiony;
               motionx = 0;
               motiony = 0;
            }
         }

         if ( !skipEvent )
            pgApp->PumpIntoEventQueue( &event );

      } else
         SDL_Delay ( 2 );
   }
   */
   int quitModalLoopValue = PG_Window::RunModal();
#ifndef sgmain
   setEventRouting ( eventQueue, !eventQueue );
#endif

   return quitModalLoopValue;
}
#endif


bool ASC_PG_Dialog::eventKeyDown(const SDL_KeyboardEvent *key){
  if(key->keysym.sym == SDLK_ESCAPE) {
        closeWindow();
    }
  return true;
}


#if 0
int ASC_PG_Dialog::RunModal ( )
{
   bool eventQueue = setEventRouting ( true, false );
/*
   while ( !quitModalLoopValue ) {
      SDL_Event event;
      int motionx = 0;
      int motiony = 0;
      if ( getQueuedEvent( event )) {
         bool skipEvent = false;
         if ( event.type == SDL_MOUSEMOTION ) {
            SDL_Event nextEvent;
            if ( peekEvent ( nextEvent ) )
               if ( nextEvent.type == SDL_MOUSEMOTION ) {
                  skipEvent = true;
                  motionx += event.motion.xrel;
                  motiony += event.motion.yrel;
               }
            if ( !skipEvent ) {
               event.motion.xrel += motionx;
               event.motion.yrel += motiony;
               motionx = 0;
               motiony = 0;
            }
         }

         if ( !skipEvent )
            ProcessEvent(&event, true);
            // pgApp->PumpIntoEventQueue( &event );

      } else
         SDL_Delay ( 2 );
   }
   */
   int quitModalLoopValue = PG_Window::RunModal();
   setEventRouting ( eventQueue, !eventQueue );

   return quitModalLoopValue;
}
#endif

void ASC_PG_Dialog::quitModalLoop(int value )
{
   SetModalStatus( value );
   PG_Window::QuitModal();
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











class  MessageDialog : public ASC_PG_Dialog {
   int hotkey1, hotkey2;
public:
	/**
	Creates a PopUp with 2 Buttons
		
	@param parent Parent widget
	@param r rectangle of PopUp
	@param windowtitle Title of window
	@param windowtext Text to appear in window
	@param btn1text Text to appear in Button 1
	@param btn2text Text to appear in Button 2
	@param textalign Alignment for windowtext
	@param style widgetstyle to use (default "MessageBox")
	*/
	MessageDialog(PG_Widget* parent, const PG_Rect& r, const std::string& windowtitle, const std::string& windowtext, const std::string& btn1text, const std::string& btn2text, PG_Label::TextAlign textalign = PG_Label::CENTER, const std::string& style="MessageBox");

	/**
	Creates a PopUp with 1 Button

	@param parent Parent widget
	@param r rectangle of PopUp
	@param windowtitle Title of window
	@param windowtext Text to appear in window
	@param btn1text Text to appear in Button 1
	@param textalign Alignment for windowtext
	@param style widgetstyle to use (default "MessageBox")
	*/
	MessageDialog(PG_Widget* parent, const PG_Rect& r, const std::string& windowtitle, const std::string& windowtext, const std::string& btn1text, PG_Label::TextAlign textalign = PG_Label::CENTER, const std::string& style="MessageBox");

	/**
	Creates a PopUp without Buttons

	@param parent Parent widget
	@param r rectangle of PopUp
	@param windowtitle Title of window
	@param windowtext Text to appear in window
	@param textalign Alignment for windowtext
	@param style widgetstyle to use (default "MessageBox")
	*/
	MessageDialog(PG_Widget* parent, const PG_Rect& r, const std::string& windowtitle, const std::string& windowtext, PG_Label::TextAlign textalign = PG_Label::CENTER, const std::string& style="MessageBox");
        
	/**
	Destructor
	*/
	~MessageDialog();

	void LoadThemeStyle(const std::string& widgettype);

 PG_RichEdit* getTextBox() { return my_textbox; };

protected:

	/**
	Checks if button is pressed

	@param button pointer to PG_BUtton
	*/
	virtual bool handleButton(PG_Button* button);

	PG_Button* my_btnok;
	PG_Button* my_btncancel;

   bool eventKeyDown (const SDL_KeyboardEvent *key);
        
         
private:

	PG_RichEdit* my_textbox;
	int my_msgalign;

	void Init(const std::string& windowtext, int textalign, const std::string& style) ;
};

int extractHotkey( const ASCString& s )
{
   bool found = false;
   for ( int i = 0; i < s.length(); ++i ) {
      if ( found )
         return s[i];
      if ( s[i] == '~' )
         found = true;
   }
   return 0;
}


MessageDialog::MessageDialog(PG_Widget* parent, const PG_Rect& r, const std::string& windowtitle, const std::string& windowtext, const std::string& btn1text, const std::string& btn2text, PG_Label::TextAlign textalign, const std::string& style) :
      ASC_PG_Dialog(parent, r, windowtitle, MODAL, style), hotkey1(0), hotkey2(0) {


   int buttonWidth = min( 120, r.Width() / 2 - 20 );
   PG_Rect btn1 = PG_Rect( r.Width() / 2 - buttonWidth - 10, r.Height() - 35, buttonWidth, 30 ); 

	my_btnok = new PG_Button(this, btn1, btn1text);
	my_btnok->SetID(1);
	my_btnok->sigClick.connect(slot(*this, &MessageDialog::handleButton));
 hotkey1 = extractHotkey( btn1text );

   PG_Rect btn2 = btn1;
   btn2.x = r.Width() / 2 + 10;

	my_btncancel = new PG_Button(this, btn2, btn2text);
	my_btncancel->SetID(2);
	my_btncancel->sigClick.connect(slot(*this, &MessageDialog::handleButton));
 hotkey2 = extractHotkey( btn2text );

	Init(windowtext, textalign, style);
}

MessageDialog::MessageDialog(PG_Widget* parent, const PG_Rect& r, const std::string& windowtitle, const std::string& windowtext, const std::string& btn1text, PG_Label::TextAlign textalign, const std::string& style) :
      ASC_PG_Dialog(parent, r, windowtitle, MODAL, style ), hotkey1(0), hotkey2(0), my_btncancel(NULL)
{

   int buttonWidth = min( 120, r.Width() - 20 );
   PG_Rect btn1 = PG_Rect( r.Width() / 2 - buttonWidth/2, r.Height() - 40, buttonWidth, 30 ); 

	my_btnok = new PG_Button(this, btn1, btn1text);
	my_btnok->SetID(1);
	my_btnok->sigClick.connect(slot(*this, &MessageDialog::handleButton));

 hotkey1 = extractHotkey( btn1text );
	Init(windowtext, textalign, style);
}

MessageDialog::MessageDialog(PG_Widget* parent, const PG_Rect& r, const std::string& windowtitle, const std::string& windowtext, PG_Label::TextAlign textalign, const std::string& style) :
      ASC_PG_Dialog(parent, r, windowtitle, MODAL, style ), hotkey1(0), hotkey2(0), my_btnok(NULL), my_btncancel(NULL)
{

	Init(windowtext, textalign, style);
}

bool MessageDialog::eventKeyDown (const SDL_KeyboardEvent *key)
{
   if (  key->keysym.sym == SDLK_ESCAPE ) {
      quitModalLoop(10);
      return true;
   }   
   if (  key->keysym.sym == SDLK_RETURN || key->keysym.sym == SDLK_KP_ENTER ) {
      quitModalLoop(11);
      return true;
   }   
   if (  key->keysym.sym == SDLK_SPACE ) {
      quitModalLoop(12);
      return true;
   }
   
   if (  key->keysym.unicode == hotkey1 && hotkey1 ) {
      quitModalLoop(1);
      return true;
   }
   
   if (  key->keysym.unicode == hotkey2 && hotkey2 ) {
      quitModalLoop(2);
      return true;
   }
   
   return false;      
}


//Delete the Buttons
MessageDialog::~MessageDialog() {
	delete my_btnok;
	delete my_btncancel;
}

void MessageDialog::Init(const std::string& windowtext, int textalign, const std::string& style) {

	my_textbox = new PG_RichEdit(this, PG_Rect(10, 40, my_width-20, my_height-50 - 40));
	my_textbox->SendToBack();
	my_textbox->SetTransparency(255);
	my_textbox->SetText(windowtext);

	my_msgalign = textalign;

	LoadThemeStyle(style);
}

void MessageDialog::LoadThemeStyle(const std::string& widgettype) {
         PG_Window::LoadThemeStyle(widgettype);

   if ( my_btnok )
	my_btnok->LoadThemeStyle(widgettype, "Button1");
        
	if(my_btncancel) {
		my_btncancel->LoadThemeStyle(widgettype, "Button2");
	}
}

//Event?
bool MessageDialog::handleButton(PG_Button* button) {
	//Set Buttonflag to ButtonID
        quitModalLoop( button ? button->GetID() : 0 );
	return true;
}


PG_Rect calcMessageBoxSize( const ASCString& message )
{
  int counter = 0;
  for ( int i = 0; i< message.length(); ++i)
     if ( message[i] == '\n' )
        counter++;

  return PG_Rect( 100, 100, 500, 150 + counter * 20 );
}



void errorMessageDialog( const ASCString& message )
{
   PG_Rect size = calcMessageBoxSize(message);
   MessageDialog msg( NULL, size, "Error", message, "OK", PG_Label::CENTER, "ErrorMessage" );
   msg.Show();
   msg.RunModal();
}

void warningMessageDialog( const ASCString& message )
{
   PG_Rect size = calcMessageBoxSize(message);
   MessageDialog msg( NULL, size, "Warning", message, "OK", PG_Label::CENTER, "WarningMessage" );
   msg.Show();
   msg.RunModal();
}

void infoMessageDialog( const ASCString& message )
{
   PG_Rect size = calcMessageBoxSize(message);
   MessageDialog msg( NULL, size, "Information", message, "OK" );
   msg.Show();
   msg.RunModal();
}


int  new_choice_dlg(const ASCString& title, const ASCString& leftButton, const ASCString& rightButton )
{
   PG_Rect size = calcMessageBoxSize(title);
   MessageDialog msg( NULL, size,"", "", leftButton, rightButton, PG_Label::CENTER, "Window" );
   msg.getTextBox()->SetFontSize( msg.getTextBox()->GetFontSize() + 3 );
   msg.getTextBox()->SetText(title);
      
   msg.Show();
   // PG_Widget::UpdateScreen();
   return msg.RunModal();
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


#if 0

class PageRenderer : public PG_ScrollWidget {

   struct RenderingAttribute {
      RenderingAttribute() : spaceAfter(0), baseline(0),linebreak(0), firstLineIndent(-1), furtherLineIndent(-1) {};
      int spaceAfter;
      int baseline;
      int linebreak;
      int firstLineIndent;
      int furtherLineIndent;
   };
   /*
   class Attributes {
      public:
         Attributes();
         virtual ~Attributes();
         virtual void assign( PG_Widget* w );
      int size;
      int color 
};*/

   struct TextAttributes {
      TextAttributes() : fontsize(10), textcolor(-1), backgroundcolor(-1) {};
      int fontsize;
      int textcolor;
      int backgroundcolor;
      void assign ( PG_Widget* w )
      {

         static PG_ThemeWidget* theme = NULL;
         if ( !theme )
            theme = new PG_ThemeWidget( NULL );
         
         w->SetFontSize( fontsize );
         if ( textcolor > 0 )
            w->SetFontColor ( textcolor );
         else
            w->SetFontColor ( theme->GetFontColor() );
      }
   };

   TextAttributes textAttributes;
   
   typedef std::map<PG_Widget*,RenderingAttribute> Attributes;
   Attributes attributes;


   typedef list<PG_Widget*> Widgets;
   Widgets widgets;
   PG_Widget* lastWidget;
   
   public:
      PageRenderer (PG_Widget *parent, const PG_Rect &r=PG_Rect::null, const std::string &style="ScrollWidget") : PG_ScrollWidget( parent, r, style ), lastWidget(NULL)
      {
         SetTransparency(255);
      };

      void SetText( const ASCString& text )
      {
         // PG_ThemeWidget::SetText( text );
         // parse( text );
      };

      bool isSpace( ASCString::charT character )
      {
         return character == ' ' || character == '\n' || character=='\r' || character == '\t';
      }

      bool isBreaker( ASCString::charT character )
      {
         return character == ':' || character == ',' || character=='.' || character == ';' || character == '-';
      }


      void arrangeLine( int y, const Widgets& line, int lineHeight )
      {
         int x = 0;
         for ( Widgets::const_iterator i = line.begin(); i != line.end(); ++i ) {
            (*i)->MoveWidget( x, y + lineHeight- (*i)->Height(), false );
            x += (*i)->Width();
            if ( attributes.find(*i ) != attributes.end() )
               x += attributes[*i].spaceAfter;
         }
      }

      int AreaWidth()
      {
         return max( int(GetListWidth()), Width() - 15 );
         // return Width() - 15;
      }

      void layout()
      {
         int x = 0;
         int y = 0;
         int lineHeight = 0;
         Widgets currentLine;
         
         int breakNow = 0;

         int firstLineIndent = 0;
         int furtherLineIndent = 0;
               
         for ( Widgets::iterator i = widgets.begin(); i != widgets.end(); ++i ) {
            if ( (x + (*i)->Width() >= AreaWidth() && x > 0) || breakNow ) {
               arrangeLine( y, currentLine, lineHeight );
               y += lineHeight + breakNow;
               
               if ( breakNow )
                  x = firstLineIndent;
               else
                  x = furtherLineIndent;
               
               lineHeight = 0;
               currentLine.clear();
               breakNow = 0;
            }
            
            currentLine.push_back ( *i );
            
            if ( lineHeight < (*i)->Height() )
               lineHeight = (*i)->Height();
               
            x += (*i)->Width();
            Attributes::iterator at = attributes.find(*i );
            if ( at != attributes.end()) {
               x += at->second.spaceAfter;
               if ( at->second.linebreak )
                  breakNow = at->second.linebreak;
               
               if ( at->second.firstLineIndent >= 0 )
                  firstLineIndent = at->second.firstLineIndent;
               
               if ( at->second.furtherLineIndent >= 0 )
                  furtherLineIndent = at->second.furtherLineIndent;
            }
                  
         }
         arrangeLine( y, currentLine, lineHeight );
         
      }

      void addWidget( PG_Widget* w )
      {
         if ( w ) {
            widgets.push_back( w );
            lastWidget = w;
         }
      }

      void addSpace( int space )
      {
         if ( lastWidget ) 
            attributes[lastWidget].spaceAfter += space * 5;
      }

      void addLinebreak( int pixel, int lines )
      {
         if ( lastWidget )  
            attributes[lastWidget].linebreak += pixel + 1 + (lines-1) * (textAttributes.fontsize+3);
      }

      void addIndentation( int firstLine, int furtherLines )
      {
         if ( lastWidget ) {
            if ( firstLine >= 0 ) 
               attributes[lastWidget].firstLineIndent = firstLine;

            if ( furtherLines >= 0 )
               attributes[lastWidget].firstLineIndent = furtherLines;
         }
      }
      

      ASCString substr( const ASCString& text, ASCString::const_iterator begin, ASCString::const_iterator end )
      {
         return text.substr( begin-text.begin(), end-begin+1);
      }
      
      ASCString::const_iterator token ( const ASCString& text, ASCString::const_iterator start )
      {
         ASCString::const_iterator end = start;
         bool isTag = *start == '#';
         
         while( end+1 != text.end() && (!isBreaker(*end) || isTag) && !isSpace(*end))
            ++end;

         if ( isSpace( *end )) {
            addWidget( render ( substr( text, start, end-1)));

            int hspace = 0;
            int vspace = 0;
            
            while ( end != text.end() && isSpace( *end)) {
               if ( *end == ' ' )
                  hspace += 1;
               else
                  if ( *end == '\n' ) {
                     hspace = 0;
                     vspace += 1;
                  }
                  
               ++end;
            }
            if ( hspace )
               addSpace( hspace );
            
            if ( vspace )
               addLinebreak( 0, vspace );
            
            return end;
         } else {
            addWidget( render( substr( text, start, end)));
            ++end;
            return end;
         }
      }
      
      void parse( const ASCString& text )
      {
         ASCString::const_iterator pos = text.begin();

         // skip spaces at beginning to text
         while ( pos == text.end() && isSpace(*pos) )
            ++pos;

         if ( pos == text.end() )
            return;
        
         while ( pos != text.end() )
            pos = token ( text, pos );

      }

      PG_Widget* parsingError( const ASCString& errorMessage )
      {
         PG_Widget* w = new PG_Label( this );
         textAttributes.assign( w );
         w->SetText( errorMessage );
         w->SetSizeByText();
         w->SetFontColor( 0xff0000 );
         return w;
      }
      
      virtual PG_Widget* render( const ASCString& token )
      {
         if ( token[0] == '#' ) {
            boost::smatch what;
            
            static boost::regex size( "#fontsize=(\\d+)#");
            if( boost::regex_match( token, what, size)) {
               ASCString s;
               s.assign( what[1].first, what[1].second );
               textAttributes.fontsize = atoi(s.c_str() );
               return NULL;
            }
            
            static boost::regex image( "#image=(\\S+)#");
            if( boost::regex_match( token, what, image)) {
               ASCString s;
               s.assign( what[1].first, what[1].second );
               Surface& surf = IconRepository::getIcon(s);
               PG_Widget* w = new PG_Image( this, PG_Point(0,0), surf.getBaseSurface(), false );
               return w;
            }
            
            static boost::regex color( "#fontcolor=(\\d+)#");
            if( boost::regex_match( token, what, color)) {
               ASCString s;
               s.assign( what[1].first, what[1].second );
               textAttributes.textcolor = atoi(s.c_str() );
               return NULL;
            }
            
            static boost::regex defcolor( "#fontcolor=default#");
            if( boost::regex_match( token, what, defcolor)) {
               textAttributes.textcolor = -1;
               return NULL;
            }
            
            static boost::regex legacycolor( "#color(\\d{3})#");
            if( boost::regex_match( token, what, legacycolor)) {
               ASCString s;
               s.assign( what[1].first, what[1].second );
               int index = atoi(s.c_str() );
               textAttributes.textcolor = pal[index][0] << 16 + pal[index][1] << 8 + pal[index][2];
               return NULL;
            }
            
            static boost::regex crtp( "#crtp=?(\\d+)#");
            if( boost::regex_match( token, what, crtp)) {
               ASCString s;
               s.assign( what[1].first, what[1].second );
               addLinebreak( atoi ( s.c_str()), 0);
               return NULL;
            }

            static boost::regex crt( "#crt#");
            if( boost::regex_match( token, what, crt)) {
               addLinebreak( 0, 1 );
               return NULL;
            }

            static boost::regex firstindent( "#eeinzug(\\d+)#");
            if( boost::regex_match( token, what, firstindent)) {
               ASCString s;
               s.assign( what[1].first, what[1].second );
               addIndentation( atoi(s.c_str()), -1 );
               return NULL;
            }
            
            static boost::regex furtherindent( "#aeinzug(\\d+)#");
            if( boost::regex_match( token, what, furtherindent)) {
               ASCString s;
               s.assign( what[1].first, what[1].second );
               addIndentation( -1, atoi(s.c_str()) );
               return NULL;
            }
            
            
            
            return parsingError ( "unknown token: " + token );
         } else {
               PG_Widget* w = new PG_Label( this );
               textAttributes.assign( w );
               w->SetText( token );
               w->SetSizeByText();
               w->SizeWidget( w->Width(), textAttributes.fontsize*4/3, false );
               return w;
         }
         return NULL;
      };
};
#endif


void testText()
{
   // ViewFormattedText vft ( "Test", "Hallo #fontsize=12# Welt #image=program-icon.png#\nNext Line g o U J ", PG_Rect( -1, -1, 400, 400 ));
   ViewFormattedText vft ( "Test", "g o U J ", PG_Rect( -1, -1, 400, 400 ));
   vft.Show();
   vft.RunModal();
}
