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

#include "util/messaginghub.h"


#include "sdl/graphicsqueue.h"

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


class ASC_PG_ScreenUpdater : public PG_ScreenUpdater {
   public:
      void UpdateRect(SDL_Surface *screen, Sint32 x, Sint32 y, Sint32 w, Sint32 h)
      {
      #ifdef WIN32
         queueOperation( new UpdateRectOp( screen, x, y, w, h ));
      #else
         SDL_UpdateRect( screen,x,y,w,h);
         postScreenUpdate(screen);
      #endif
      };

      void UpdateRects(SDL_Surface *screen, int numrects, SDL_Rect *rects)
      {
      #ifdef WIN32
         queueOperation( new UpdateRectsOp( screen, numrects, rects ));
      #else
         SDL_UpdateRects( screen, numrects, rects );
         postScreenUpdate(screen);
      #endif
      }
} ascScreenUpdater;


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
   SetScreenUpdater ( &ascScreenUpdater );
   
   signal ( SIGINT, &signalQuit );
   
   PG_LineEdit::SetBlinkingTime( 500 );

   SetHighlightingTag( '~' );

   setMouseUpdateFlag( &CGameOptions::Instance()->hideMouseOnScreenUpdates );

}

bool ASC_PG_App :: queueWidgetForDeletion( PG_Widget* widget )
{
   deletionQueue.push_back( widget );
   widget->sigDelete.connect( SigC::slot( *this, &ASC_PG_App::removeFromDeletionQueue ));
   return true;
}


bool ASC_PG_App :: removeFromDeletionQueue( const PG_MessageObject* obj )
{
   DeletionQueue::iterator i = find ( deletionQueue.begin(), deletionQueue.end(), obj );
   if ( i != deletionQueue.end() )
      deletionQueue.erase( i );
   return true;
}


void ASC_PG_App :: setIcon( const ASCString& filename )
{
   SDL_Surface *icn = NULL;
   try {
      tnfilestream iconl ( filename, tnstream::reading );
      icn = IMG_Load_RW ( SDL_RWFromStream( &iconl ), 1);
      // SDL_SetColorKey(icn, SDL_SRCCOLORKEY, *((Uint8 *)icn->pixels));
      if ( icn )
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

   if ( !deletionQueue.empty() )
      delete deletionQueue.front();

   PG_Application::eventIdle();
}

#include "sdl/graphicsqueue.h"

void ASC_PG_App::SetNewScreenSurface( SDL_Surface* surface )
{
   SetScreen(surface, false);
}


bool ASC_PG_App::toggleFullscreen()
{
   if ( !GetScreen() )
      return false;

   int w = GetScreen()->w;
   int h = GetScreen()->h;

   // queueOperation( new MouseVisibility( false ), true );

   int flags = SDL_SWSURFACE;
   if ( !fullScreen )
      flags |= SDL_FULLSCREEN;

   queueOperation( new InitScreenOp( w,h,bitsperpixel,flags, InitScreenOp::ScreenRegistrationFunctor( this, &ASC_PG_App::SetNewScreenSurface )), true );
   fullScreen = GetScreen()->flags & SDL_FULLSCREEN;


/*
   SDL_Surface* screen = SDL_SetVideoMode(w, h, bitsperpixel, flags);
   if (screen == NULL) {
      screen = SDL_SetVideoMode(w, h, bitsperpixel, 0);
      fullScreen = false;
   } else
      fullScreen = !fullScreen;
   SetScreen(screen);
*/

   PG_Widget::UpdateScreen();

   // queueOperation( new MouseVisibility( true ), true );

   return true;
}


ASC_PG_App& getPGApplication()
{
   return *pgApp;
}


     
StartupScreen::StartupScreen( const ASCString& filename, SigC::Signal0<void>& ticker ) : infoLabel(NULL), versionLabel(NULL), background(NULL), progressBar(NULL), fullscreenImage(NULL)
{
   MessagingHub::Instance().statusInformation.connect( SigC::slot( *this, &StartupScreen::disp ));
   
   tnfilestream s ( filename, tnstream::reading );


   int rt = 0;
   int gt = 0;
   int bt = 0;

   fullscreenImage = Surface( IMG_Load_RW( SDL_RWFromStream( &s ), true ));
   if ( fullscreenImage.valid() ) {
      for ( int y = 0; y < fullscreenImage.h(); ++y ) {
         for ( int x = 0; x < fullscreenImage.w(); ++x ) {
            Uint8 r,g,b;
            fullscreenImage.GetPixelFormat().GetRGB( fullscreenImage.GetPixel(x,y), r,g,b ); 
            rt += r;
            gt += g;
            bt += b;
         }
      }
      rt /= fullscreenImage.h() * fullscreenImage.w();
      gt /= fullscreenImage.h() * fullscreenImage.w();
      bt /= fullscreenImage.h() * fullscreenImage.w();
   }




   background = new PG_ThemeWidget(NULL, PG_Rect(0,0,PG_Application::GetScreenWidth(), PG_Application::GetScreenHeight()));
   background->SetSimpleBackground(true);
   background->SetBackgroundColor( PG_Color( rt, gt, bt ));

   if ( fullscreenImage.valid() ) {
      float enw = float(PG_Application::GetScreenWidth() )/float(fullscreenImage.w());
      float enh = float(PG_Application::GetScreenHeight())/float(fullscreenImage.h());

	  displayLogMessage(6, "Startupscreen : %f / %f", enw, enh ); 

      // we allow a asymetric stretch of 5% 
      if ( enw / enh < 0.95 || enw / enh > 1.05 ) 
         enh = enw = min( enw, enh );

      int w = int( ceil( enw * fullscreenImage.w()));
      int h = int( ceil( enh * fullscreenImage.h()));
      PG_Rect rect ( (PG_Application::GetScreenWidth()-w)/2, (PG_Application::GetScreenHeight()-h)/2, w,h);
      PG_ThemeWidget* image = new PG_ThemeWidget( background, rect );
      image->SetBackground ( fullscreenImage.getBaseSurface(), PG_Draw::STRETCH );
   }

   int progressHeight = 15;
   SDL_Surface* screen = PG_Application::GetApp()->GetScreen();
   progressBar = new AutoProgressBar( ticker, background, PG_Rect( 0, screen->h - progressHeight, screen->w, progressHeight ) );

   infoLabel = new PG_Label( background, PG_Rect( screen->w/2, screen->h - progressHeight - 25, screen->w/2 - 10, 20 ));
   infoLabel->SetAlignment( PG_Label::RIGHT );

   if ( MessagingHub::Instance().getVerbosity() > 0 ) {
      versionLabel = new PG_Label( background, PG_Rect( 10, screen->h - progressHeight - 25, screen->w/2, 20 ));
      versionLabel->SetAlignment( PG_Label::LEFT );
      versionLabel->SetText( getVersionString() );
   }

   background->Show();
}

void StartupScreen::disp( const ASCString& s )
{
   infoLabel->SetText( s );
}

         
StartupScreen::~StartupScreen()
{
   progressBar->close();
   delete background;
}



bool ASC_PG_App:: InitScreen ( int w, int h, int depth, Uint32 flags )
{
   bitsperpixel = depth;
   bool result = PG_Application::InitScreen ( w, h, depth, flags  );
   if ( result ) {
      initASCGraphicSubsystem ( GetScreen() );
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
  
   return 0;
}

ASC_PG_App :: ~ASC_PG_App()
{
   while ( !deletionQueue.empty() )
      delete deletionQueue.front();
   
   shutdownASCGraphicSubsystem();
}



ASC_PG_Dialog :: ASC_PG_Dialog ( PG_Widget *parent, const PG_Rect &r, const ASCString& windowtext, WindowFlags flags, const ASCString& style, int heightTitlebar )
   :PG_Window ( parent, centerRectangle(r), windowtext, flags, style, heightTitlebar ),stdButtonNum(0), caller(0), standardButtonDir( Vertical )
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


PG_Rect ASC_PG_Dialog::centerRectangle( const PG_Rect& rect )
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


   if ( r.x + r.w >  PG_Application::GetScreenWidth() )
      r.x = PG_Application::GetScreenWidth() - r.w;
         
   if ( r.y + r.h >  PG_Application::GetScreenHeight() )
      r.y = PG_Application::GetScreenHeight() - r.h;

   return r;
}

void ASC_PG_Dialog::StandardButtonDirection ( StandardButtonDirectonType dir )
{
   standardButtonDir = dir;
}

PG_Button* ASC_PG_Dialog::AddStandardButton( const ASCString& name )
{
   ++stdButtonNum;
   
   if ( name.length() == 0 )
      return NULL;
   
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




class AntiBulkHandler {
      bool bulk;   
public:
   AntiBulkHandler() {
      bulk = PG_Application::GetBulkMode();
      if ( bulk )
         PG_Application::SetBulkMode(false);
   }
   ~AntiBulkHandler() {
      if ( bulk )
         PG_Application::SetBulkMode(true);
   }
};


void ASC_PG_App:: messageDialog( const ASCString& message, MessagingHubBase::MessageType mt )
{
   AntiBulkHandler abh;
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

void PG_StatusWindowData::SetText( const ASCString& text )
{
   md->SetText( text );
}


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

BulkGraphicUpdates :: BulkGraphicUpdates( PG_Widget* parent )
{
   bulk = PG_Application::GetBulkMode();
   this->parent = parent;
   PG_Application::SetBulkMode( true );
   active = true;
};


void BulkGraphicUpdates::release()
{
   if ( !bulk && active ) {
      PG_Application::SetBulkMode( false );
      if ( parent )
         parent->Update();
   }
   active = false;
}

BulkGraphicUpdates::~BulkGraphicUpdates()
{
   release();
}

void Emboss::eventBlit (SDL_Surface *surface, const PG_Rect &src, const PG_Rect &dst) 
{
   Surface s = Surface::Wrap( PG_Application::GetScreen() );

   PG_Rect clip= dst.IntersectRect( PG_Application::GetScreen()->clip_rect );
   if ( inv )
      rectangle<4> ( s, SPoint(dst.x, dst.y), dst.w, dst.h, ColorMerger_Brightness<4>( 0.7 ), ColorMerger_Brightness<4>( 1.4 ), clip);
   else
      rectangle<4> ( s, SPoint(dst.x, dst.y), dst.w, dst.h, ColorMerger_Brightness<4>( 1.4 ), ColorMerger_Brightness<4>( 0.7 ), clip);
};


class StringEditor  : public ASC_PG_Dialog {
      PG_LineEdit* editor;

   public:
      StringEditor( const ASCString& title, const ASCString& textToEdit ) : ASC_PG_Dialog( NULL, PG_Rect( -1, -1, 400, 200 ), title), editor(NULL) 
      {
         editor = new PG_LineEdit( this, PG_Rect( 10, 40, Width() - 20, 25 ) );
         editor->SetText( textToEdit );
         AddStandardButton( "OK" )->sigClick.connect( SigC::bind( SigC::slot( *this, &StringEditor::quitModalLoop ), 1 ));
      }

      ASCString GetEditedText() { return editor->GetText(); };
};

ASCString editString2( const ASCString& title, const ASCString& defaultValue )
{
   StringEditor se ( title, defaultValue );
   se.Show();
   se.RunModal();
   return se.GetEditedText();
}

int choiceDialog(const ASCString& text, const ASCString& button1, const ASCString& button2, const ASCString& shortLabel )
{

   map<ASCString,int>& answers = CGameOptions::Instance()->dialogAnswers;

   map<ASCString,int>::iterator i = answers.find( shortLabel );
   if ( i != answers.end() )
      return i->second;

   bool saveResult = false;
   int result = new_choice_dlg(text, shortLabel, button1, button2, saveResult);

   if ( saveResult ) {
      answers[shortLabel] = result;
      CGameOptions::Instance()->setChanged();
   }

   return  result;

}

