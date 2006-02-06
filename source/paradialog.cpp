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


#include "paradialog.h"
#include "events.h"
#include "gameoptions.h"
#include "sg.h"
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





DropDownSelector::DropDownSelector( PG_Widget *parent, const PG_Rect &r, int id, const std::string &style) 
     : PG_DropDown( parent, r, id, style ), first(true)
{
   SetEditable(false);
   sigSelectItem.connect( SigC::slot( *this, &DropDownSelector::itemSelected ));
}

bool DropDownSelector::itemSelected(  ) // PG_ListBoxBaseItem* i, void* p
{
   selectionSignal( GetSelectedItemIndex ());
   return true;
}


void DropDownSelector::AddItem (const std::string &text, void *userdata, Uint16 height)
{
   PG_DropDown::AddItem( text, userdata, height );
   if ( first ) {
      first = false;
      SelectFirstItem();
   }   
}




ASC_PG_App* pgApp = NULL;


void signalQuit( int i )
{
   getPGApplication().Quit();
}


ASC_PG_App :: ASC_PG_App ( const ASCString& themeName )  : fullScreen(false)
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




bool ASC_PG_App:: InitScreen ( int w, int h, int depth, Uint32 flags )
{
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






ASC_PG_Dialog :: ASC_PG_Dialog ( PG_Widget *parent, const PG_Rect &r, const ASCString& windowtext, WindowFlags flags, const ASCString& style, int heightTitlebar )
      :PG_Window ( parent, r, windowtext, flags, style, heightTitlebar ),
      // quitModalLoopValue ( 0 ), 
      caller(0)
{
   //   mainScreenWidget->setDirty();
   //   SDL_mutexP ( eventHandlingMutex );
   // sigMouseButtonDown.connect(SigC::slot(*this, &ASC_PG_Dialog::eventMouseButtonDown));
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















const int widgetTypeNum = 16;
const char* widgetTypes[widgetTypeNum]
=
   { "image",
     "area",
     "statictext",
     "textoutput",
     "bargraph",
     "specialDisplay",
     "specialInput",
     "dummy",
     "multilinetext",
     "scrollarea",
     "button",
     "radiobutton",
     "checkbox",
     "lineedit",
     "slider",
     "plain"
   };

enum  WidgetTypes  { Image,
                     Area,
                     StaticLabel,
                     TextOutput,
                     BarGraph,
                     SpecialDisplay,
                     SpecialInput,
                     Dummy,
                     MultiLineText,
                     ScrollArea,
                     Button,
                     RadioButton,
                     CheckBox,
                     LineEdit,
                     Slider,
                     Plain };

const int imageModeNum = 5;
const char* imageModes[imageModeNum]
    = { "tile",
        "stretch",
        "tile3h",
        "tile3v",
        "tile9" };


const int textAlignNum = 3;
const char* textAlignment[textAlignNum]
=
   { "left",
     "center",
     "right"
   };


const int barDirectionNum = 4;
const char* barDirections[barDirectionNum]
=
   { "left2right",
     "right2left",
     "top2buttom",
     "buttom2top"
   };

const int sliderDirectionNum = 2;
const char* sliderDirections[sliderDirectionNum]
=
   { "vertical",
     "horizontal"
   };
   


ASCGUI_Window::WidgetParameters::WidgetParameters()
      : backgroundMode(PG_Draw::TILE),  
        textAlign( PG_Label::LEFT ), textAlign_defined(false),
        fontColor(0xffffff), fontColor_defined(false),
        fontAlpha(255), fontAlpha_defined(false),
        fontSize(8), fontSize_defined(false),
        backgroundColor_defined(false),
        transparency(0), transparency_defined(false),
        hidden(false)
{
}

void  ASCGUI_Window::WidgetParameters::runTextIO ( PropertyReadingContainer& pc )
{
   if ( pc.find( "BackgroundImage" )) {
      pc.addString( "BackgroundImage", backgroundImage, backgroundImage );
      int i = backgroundMode;
      pc.addNamedInteger( "BackgroundMode", i, imageModeNum, imageModes, i);
      backgroundMode = PG_Draw::BkMode( i );
   }

   if ( pc.find( "TextAlign" )) {
      int ta = textAlign;
      pc.addNamedInteger( "TextAlign", ta, textAlignNum, textAlignment, ta );
      textAlign = PG_Label::TextAlign( ta );
      textAlign_defined = true;
   }

   if ( pc.find( "FontColor")) {
      pc.addInteger("FontColor", fontColor, fontColor );
      fontColor_defined = true;
   }
   
   pc.addString("FontName", fontName, fontName );
   
   if ( pc.find( "FontAlpha")) {
      pc.addInteger("FontAlpha", fontAlpha, fontAlpha );
      fontAlpha_defined = true;
   }    
   
   if ( pc.find( "FontSize")) {
      pc.addInteger("FontSize", fontSize, fontSize );
      fontSize_defined = true;
   }
   
   if ( pc.find( "BackgroundColor" )) {
      pc.addInteger("BackgroundColor", backgroundColor,  backgroundColor );
      backgroundImage.clear();
      backgroundColor_defined = true;
   };
   
   if ( pc.find("Transparency")) {
      pc.addInteger("Transparency", transparency, transparency );
      transparency_defined = true;
   }
   
   pc.addBool( "hidden", hidden, hidden );
   pc.addString("Style", style, style );
}


void  ASCGUI_Window::WidgetParameters::assign( BarGraphWidget* widget )
{
   if ( !widget )
      return;

   if ( backgroundColor_defined )
      widget->setColor( backgroundColor );

   assign( (PG_ThemeWidget*)widget );

}


void  ASCGUI_Window::WidgetParameters::assign( PG_ThemeWidget* widget )
{
   if ( !widget )
      return;

   
   if ( !backgroundImage.empty() )
      widget->SetBackground( IconRepository::getIcon(backgroundImage).getBaseSurface(), backgroundMode );
   else {
      if ( backgroundColor_defined ) {
         widget->SetBackground( NULL );
         widget->SetSimpleBackground( true );
      }
   }

   if ( backgroundColor_defined )
      widget->SetBackgroundColor( backgroundColor );
      
   assign( (PG_Widget*)widget );
}

void  ASCGUI_Window::WidgetParameters::assign( PG_Label* widget )
{
   if ( !widget )
      return;

   if ( textAlign_defined )
      widget->SetAlignment( textAlign );

   assign( (PG_Widget*)widget );
}


void  ASCGUI_Window::WidgetParameters::assign( PG_Widget* widget )
{
   if ( !widget )
      return;

   if ( fontColor_defined ) 
      widget->SetFontColor( fontColor, true );
      
   if ( !fontName.empty() )
      widget->SetFontName( fontName, true );
      
   if ( fontAlpha_defined )
      widget->SetFontAlpha( fontAlpha, true );
   
   if ( fontSize_defined )
      widget->SetFontSize( fontSize, true );
   
   if ( transparency_defined )
      widget->SetTransparency( transparency, true );
      
   if ( hidden )
      widget->Hide(false);
}


ASCGUI_Window::ASCGUI_Window ( PG_Widget *parent, const PG_Rect &r, const ASCString& panelName_, const ASCString& baseStyle, bool loadTheme )
      : PG_Window ( parent, r, "", DEFAULT, baseStyle, 9 ), panelName( panelName_ ), textPropertyGroup(NULL)
{
      // FIXME Hide button does not delete Panel      
}


PG_Rect ASCGUI_Window::parseRect ( PropertyReadingContainer& pc, PG_Widget* parent )
{
   int x,y,w,h,x2,y2;
   // pc.openBracket( "position" );
   pc.addInteger( "x", x );
   pc.addInteger( "y", y );
   pc.addInteger( "width", w, 0 );
   pc.addInteger( "height", h, 0 );
   pc.addInteger( "x2", x2, 0 );
   pc.addInteger( "y2", y2, 0 );
   // pc.closeBracket();

   PG_Rect r ( x,y,w,h);

   if ( x < 0 )
      r.x = parent->Width() - w + x;

   if ( r.y < 0 )
      r.y = parent->Height() - h + y;

   if ( x2 != 0 ) {
      if ( x2 < 0 )
         x2 = parent->Width() + x2;

      w = x2 - r.x;
   }

   if ( y2 != 0 ) {
      if ( y2 < 0 )
         y2 = parent->Height() + y2;

      h = y2 - r.y;
   }


   if ( w <= 0 )
      r.w = parent->Width() - r.x;
   else
      r.w = w;

   if ( h <= 0 )
      r.h = parent->Height() - r.y;
   else
      r.h = h;

   return r;
}


void ASCGUI_Window::parsePanelASCTXT ( PropertyReadingContainer& pc, PG_Widget* parent, WidgetParameters widgetParams )
{
   ASCString name;
   pc.addString( "name", name, "" );
   parent->SetName( name );
   
   int transparency;
   pc.addInteger("localtransparency", transparency, -1 );
   if ( transparency != -1 ) {
      parent->SetTransparency( transparency );
   }
   
   if ( pc.find( "userHandler" )) {
      ASCString label;
      pc.addString( "userHandler", label);
      userHandler( label, pc, parent, widgetParams );
   }


   vector<ASCString> childNames;


   if ( pc.find( "ChildWidgets" )) 
      pc.addStringArray( "ChildWidgets", childNames );


   int widgetNum;
   pc.addInteger( "WidgetNum", widgetNum, 0 );

   for ( int i = 0; i < widgetNum; ++i) 
      childNames.push_back( ASCString("Widget") + ASCString::toString( i ));

      
   PG_RadioButton* radioButtonGroup = NULL;

   for ( int i = 0; i < childNames.size(); ++i) {
      pc.openBracket( childNames[i] );

      ASCString toolTipHelp;
      pc.addString( "ToolTipHelp", toolTipHelp, "" );
      

      PG_Rect r = parseRect( pc, parent );

      widgetParams.runTextIO( pc );
      
      bool hasStyle = pc.find( "style" );
      ASCString style;
      pc.addString( "style", style, "Panel" );


      int type;
      pc.addNamedInteger( "type", type, widgetTypeNum, widgetTypes );

      PG_Widget* newWidget = NULL;
      
      if ( type == Image ) {
         ASCString filename;
         pc.addString( "FileName", filename, "" );
         int imgMode;
         pc.addNamedInteger( "mode", imgMode, imageModeNum, imageModes, 0 );

         
         if ( !filename.empty() ) {
            try {
               bool dirtyUpdate = true;
               Surface& surf = IconRepository::getIcon(filename);
               if ( surf.GetPixelFormat().BytesPerPixel() == 4 ) {
                  Uint32* p = (Uint32*) surf.pixels();
                  int aMask = surf.GetPixelFormat().Amask();
                  int aShift = surf.GetPixelFormat().Ashift();
                  for ( int y = 0; y < surf.h() && dirtyUpdate; ++y ) {
                     for ( int x = 0; x < surf.w() && dirtyUpdate; ++x)
                        if ( ((p[x] & aMask) >> aShift ) != Surface::opaque )
                           dirtyUpdate = false;
                      p += surf.pitch() / 4;
                  }
               }
               
               PG_Image* img = new PG_Image( parent, PG_Point(r.x, r.y ), surf.getBaseSurface(), false, PG_Draw::BkMode(imgMode) );
               newWidget = img;
               img->SetDirtyUpdate(dirtyUpdate);

               widgetParams.assign ( img );
               parsePanelASCTXT( pc, img, widgetParams );
            } catch ( tfileerror ) {
               displaymessage( "unable to load " + filename, 1 );
            }
         } else {
            PG_Image* img = new PG_Image( parent, PG_Point(r.x, r.y ), NULL, false, PG_Draw::BkMode(imgMode) );
            newWidget = img;
            widgetParams.assign ( img );
            parsePanelASCTXT( pc, img, widgetParams );
         }

      }
      if ( type == Area ) {
         bool mode;
         pc.addBool( "in", mode, true );

         Emboss* tw = new Emboss ( parent, r );
         // PG_ThemeWidget* tw = new PG_ThemeWidget ( parent, r, style );
         widgetParams.assign ( tw );
         parsePanelASCTXT( pc, tw, widgetParams );
         newWidget = tw;
      }

      if ( type == StaticLabel ) {
         ASCString text;
         pc.addString( "text", text );

         PG_Label* lb = new PG_Label ( parent, r, text, style );
         if ( !hasStyle )
            widgetParams.assign ( lb );
         parsePanelASCTXT( pc, lb, widgetParams );
         newWidget = lb;
      }
      if ( type == TextOutput ) {
         PG_Label* lb = new PG_Label ( parent, r, PG_NULLSTR, style );

         if ( !hasStyle )
            widgetParams.assign ( lb );
         parsePanelASCTXT( pc, lb, widgetParams );
         newWidget = lb;
      }
      if ( type == MultiLineText ) {

         PG_MultiLineEdit* lb = new PG_MultiLineEdit ( parent, r, style );

         lb->SetEditable(false);
         if ( !hasStyle ) {
            lb->SetBorderSize(0);
            widgetParams.assign ( lb );
         }
         parsePanelASCTXT( pc, lb, widgetParams );
         newWidget = lb;
      }

      if ( type == BarGraph ) {
         int dir;
         pc.addNamedInteger( "direction", dir, barDirectionNum, barDirections, 0 );

         int cnum;
         pc.addInteger("Colors",cnum, 0 );
         BarGraphWidget::Colors colorRange;
         for ( int i = 0; i < cnum; ++i ) {
            pc.openBracket("Color" + ASCString::toString(i));
            int col;
            pc.addInteger("color", col);
            double f;
            pc.addDFloat("fraction", f );
            colorRange.push_back( make_pair(f,col) );
            pc.closeBracket();
         }

         BarGraphWidget* bg = new BarGraphWidget ( parent, r, BarGraphWidget::Direction(dir) );
         bg->setColor( colorRange );
         widgetParams.assign ( bg );
         parsePanelASCTXT( pc, bg, widgetParams );
         newWidget = bg;
      }


      if ( type == SpecialDisplay ) {
         SpecialDisplayWidget* sw = new SpecialDisplayWidget ( parent, r );
         widgetParams.assign ( sw );

         parsePanelASCTXT( pc, sw, widgetParams );
         newWidget = sw;
      }

      if ( type == SpecialInput ) {
         SpecialInputWidget* sw = new SpecialInputWidget ( parent, r );

         parsePanelASCTXT( pc, sw, widgetParams );
         newWidget = sw;
      }

      if ( type == Dummy ) {
         SpecialInputWidget* sw = new SpecialInputWidget ( parent, r );
         parsePanelASCTXT( pc, sw, widgetParams );
         newWidget = sw;
      }
      if ( type == ScrollArea ) {
         PG_ScrollWidget* sw = new PG_ScrollWidget( parent, r, style );
         ASCString scrollbar;
         pc.addString( "horizontal_scollbar", scrollbar, "true" );
         if ( scrollbar.compare_ci( "false" ) == 0)
            sw->EnableScrollBar(false, PG_ScrollBar::HORIZONTAL );

         pc.addString( "vertical_scollbar", scrollbar, "true" );
         if ( scrollbar.compare_ci( "false" ) == 0)
            sw->EnableScrollBar(false, PG_ScrollBar::VERTICAL );

         if ( !hasStyle )
            widgetParams.assign ( sw );
            
         parsePanelASCTXT( pc, sw, widgetParams );
         newWidget = sw;
      }
      
      if ( type == Button ) {
         PG_Button* sw = new PG_Button( parent, r, style );

         ASCString text;
         pc.addString( "text", text );
         
         if ( !text.empty() )
            sw->SetText( text );
         
         
         if ( !hasStyle )
            widgetParams.assign ( sw );
            
         parsePanelASCTXT( pc, sw, widgetParams );
         newWidget = sw;
      }

      if ( type == RadioButton ) {
         PG_RadioButton* sw = new PG_RadioButton( parent, r, style );
         if ( radioButtonGroup )
            radioButtonGroup->AddToGroup( sw );
            
         radioButtonGroup = sw;

         ASCString text;
         pc.addString( "text", text );
         
         if ( !text.empty() )
            sw->SetText( text );
         
         if ( !hasStyle )
            widgetParams.assign ( sw );
            
         parsePanelASCTXT( pc, sw, widgetParams );
         newWidget = sw;
         
      }
      if ( type == CheckBox ) {
         PG_CheckButton* sw = new PG_CheckButton( parent, r, style );

         ASCString text;
         pc.addString( "text", text );
         
         if ( !text.empty() )
            sw->SetText( text );
         
         if ( !hasStyle )
            widgetParams.assign ( sw );
            
         parsePanelASCTXT( pc, sw, widgetParams );
         newWidget = sw;
         
      }
      if ( type == LineEdit ) {
         PG_LineEdit* sw = new PG_LineEdit( parent, r, style );
         
         ASCString text;
         pc.addString( "text", text, "" );
         
         if ( !text.empty() )
            sw->SetText( text );
         
         if ( !hasStyle )
            widgetParams.assign ( sw );
            
         parsePanelASCTXT( pc, sw, widgetParams );
         newWidget = sw;
      }
      if ( type == Slider ) {
         
         int direction = 0;
         pc.addNamedInteger( "Orientation", direction, sliderDirectionNum, sliderDirections, direction);
        
         
         PG_ScrollBar* sb = new PG_Slider( parent, r, PG_ScrollBar::ScrollDirection(direction), -1, style );
         
         if ( !hasStyle )
            widgetParams.assign ( sb );
            
         parsePanelASCTXT( pc, sb, widgetParams );
         newWidget = sb;
      }
      if ( type == Plain ) {
         
         PG_ThemeWidget* w = new PG_ThemeWidget( parent, r );
         
         if ( !hasStyle )
            widgetParams.assign ( w );
            
         parsePanelASCTXT( pc, w, widgetParams );
         newWidget = w;
      }
            
      if ( newWidget && newWidget->GetName().empty() ) 
         newWidget->SetName( childNames[i] );
      
      if ( newWidget && newWidget->GetName() == "$pos" )
         newWidget->SetName( pc.getNameStack() );
      

      if ( newWidget && !toolTipHelp.empty() )
         new PG_ToolTipHelp( newWidget, toolTipHelp );

      pc.closeBracket();
   }
}

void ASCGUI_Window::rename( const ASCString& widgetName, const ASCString& newname, PG_Widget* parent )
{
   if ( !parent )
      parent = this;

   PG_Widget* w = parent->FindChild( widgetName, true );
   if ( w )
      w->SetName( newname );
}

void ASCGUI_Window::setLabelText ( const ASCString& widgetName, const ASCString& text, PG_Widget* parent )
{
   if ( !parent )
      parent = this;

   PG_Label* l = dynamic_cast<PG_Label*>( parent->FindChild( widgetName, true ) );
   if ( l )
      l->SetText( text );
   else {
      PG_MultiLineEdit* l = dynamic_cast<PG_MultiLineEdit*>( parent->FindChild( widgetName, true ) );
      if ( l )
         l->SetText( text );
   }
}

void ASCGUI_Window::setLabelColor ( const ASCString& widgetName, PG_Color color, PG_Widget* parent )
{
   if ( !parent )
      parent = this;

   PG_Label* l = dynamic_cast<PG_Label*>( parent->FindChild( widgetName, true ) );
   if ( l )
      l->SetFontColor ( color );
   else {
      PG_MultiLineEdit* l = dynamic_cast<PG_MultiLineEdit*>( parent->FindChild( widgetName, true ) );
      if ( l )
         l->SetFontColor ( color );
   }
}


void ASCGUI_Window::setLabelText ( const ASCString& widgetName, int i, PG_Widget* parent )
{
   ASCString s = ASCString::toString(i);
   setLabelText ( widgetName, s, parent );
}

void ASCGUI_Window::setImage ( const ASCString& widgetName, Surface& image, PG_Widget* parent )
{
   setImage( widgetName, image.getBaseSurface(), parent);
}

void ASCGUI_Window::setImage ( const ASCString& widgetName, SDL_Surface* image, PG_Widget* parent )
{
   if ( !parent )
      parent = this;

   PG_Image* i = dynamic_cast<PG_Image*>( parent->FindChild( widgetName, true ) );
   if ( i ) {
      i->SetImage( image, false );
      if ( image )
         i->SizeWidget( image->w, image->h);
   }
}

void ASCGUI_Window::setWidgetTransparency ( const ASCString& widgetName, int transparency, PG_Widget* parent  )
{
   if ( !parent )
      parent = this;

   PG_Widget* i = parent->FindChild( widgetName, true );
   if ( i )
      i->SetTransparency( transparency );
}


void ASCGUI_Window::hide ( const ASCString& widgetName, PG_Widget* parent )
{
   if ( !parent )
      parent = this;

   PG_Widget* i = parent->FindChild( widgetName, true );
   if ( i )
      i->Hide();
}

void ASCGUI_Window::show ( const ASCString& widgetName, PG_Widget* parent )
{
   if ( !parent )
      parent = this;

   PG_Widget* i = parent->FindChild( widgetName, true );
   if ( i )
      i->Show();
}



void ASCGUI_Window::setBargraphValue( const ASCString& widgetName, float fraction )
{
   BarGraphWidget* bgw = dynamic_cast<BarGraphWidget*>( FindChild( widgetName, true ) );
   if ( bgw )
      bgw->setFraction( fraction );
}


void ASCGUI_Window::setBarGraphColor( const ASCString& widgetName, PG_Color color )
{
   BarGraphWidget* bgw = dynamic_cast<BarGraphWidget*>( FindChild( widgetName, true ) );
   if ( bgw )
      bgw->setColor( color );
}




bool ASCGUI_Window::setup()
{
   try {
      WidgetParameters widgetParameters = getDefaultWidgetParams();
      
      {
         tnfilestream s ( panelName.toLower() + ".ascgui", tnstream::reading );

         TextFormatParser tfp ( &s );
         textPropertyGroup = tfp.run();
      }

      PropertyReadingContainer pc ( "panel", textPropertyGroup );

      int w, h;
      pc.addInteger( "width", w, 0 );
      pc.addInteger( "height", h, 0 );

      if ( w > 0 && h > 0 )
         SizeWidget( w, h, false );
      else
         if ( h > 0 )
            SizeWidget( Width(), h, false );
         else
            if ( w > 0 )
               SizeWidget( w, Height(), false );


      if ( pc.find("x") && pc.find("y" )) {
         int x1,y1;
         pc.addInteger( "x", x1 );
         pc.addInteger( "y", y1 );

         if ( x1 < 0 )
            x1 = GetParent()->Width() - Width() + x1;

         if ( y1 < 0 )
            y1 = GetParent()->Height() - Height() + y1;

         MoveWidget( x1, y1, false );
      }

      int titlebarHeight;
      pc.addInteger("Titlebarheight", titlebarHeight, -1 );
      if ( titlebarHeight != -1 )
         SetTitlebarHeight( titlebarHeight );

      ASCString title;
      pc.addString("Title", title, "" );
      if ( !title.empty() )
         SetTitle( title );
         
      widgetParameters.runTextIO( pc );
      widgetParameters.assign ( this );

      parsePanelASCTXT( pc, this, widgetParameters );

      return true;
   } catch ( ParsingError err ) {
      displaymessage( ASCString("parsing error: ") + err.getMessage(), 1 );
   } catch ( tfileerror err ) {
      displaymessage( ASCString("could not acces file: ") + err.getFileName(), 1 );
   }
   return false;

}

ASCGUI_Window::~ASCGUI_Window()
{
   if ( textPropertyGroup )
      delete textPropertyGroup;
}


Panel :: Panel ( PG_Widget *parent, const PG_Rect &r, const ASCString& panelName_, bool loadTheme )
       : ASCGUI_Window( parent, r, panelName_, "Panel", loadTheme )
{
   if ( loadTheme )
      setup();
      
   BringToFront();
}

ASCGUI_Window::WidgetParameters Panel::getDefaultWidgetParams()
{
    static WidgetParameters defaultWidgetParameters;
    static ASCString panelBackgroundImage;
    static bool defaultsLoaded = false;
    if ( !defaultsLoaded ) {
       tnfilestream s ( "default.ascgui", tnstream::reading );

       TextFormatParser tfp ( &s );
       auto_ptr<TextPropertyGroup> tpg ( tfp.run());

       PropertyReadingContainer pc ( "panel", tpg.get() );

       defaultWidgetParameters.runTextIO ( pc );
       pc.addString("PanelBackgroundImage", panelBackgroundImage );
       defaultsLoaded = true;
    }
    return defaultWidgetParameters;
}




ConfigurableWindow :: ConfigurableWindow ( PG_Widget *parent, const PG_Rect &r, const ASCString& panelName_, bool loadTheme )
       : ASCGUI_Window( parent, r, panelName_, "Window", loadTheme )
{
   if ( loadTheme )
      setup();
      
   BringToFront();
}

ASCGUI_Window::WidgetParameters ConfigurableWindow ::getDefaultWidgetParams()
{
   return  WidgetParameters();
}


BarGraphWidget:: BarGraphWidget (PG_Widget *parent, const PG_Rect &rect, Direction direction ) : PG_ThemeWidget( parent, rect, false ), fraction(1), dir(direction)
{
}

void BarGraphWidget::eventBlit (SDL_Surface *surface, const PG_Rect &src, const PG_Rect &dst)
{
   PG_Rect d = dst;
   if ( dir == l2r ) {
      d.w = min( max(0, int( float(dst.w) * fraction)), int(dst.w)) ;
   }
   if ( dir == r2l ) {
      int x2 = d.x + d.w;
      d.w = min( max(0, int( float(dst.w) * fraction)), int(dst.w)) ;
      d.x = x2 - d.w;
   }
   if ( dir == t2b ) {
      d.h = min( max(0, int( float(dst.h) * fraction)), int(dst.h)) ;
   }
   if ( dir == b2t ) {
      int y2 = d.y + d.h;
      d.h = min( max(0, int( float(dst.h) * fraction)), int(dst.h)) ;
      d.y = y2 - d.h;
   }

   if ( d.h <= 0 || d.w <= 0 )
      return;


/*   PG_Draw::DrawThemedSurface(
          surface,
          d,
          my_has_gradient ? &my_gradient : 0,
          my_background,
          my_backgroundMode,
          my_blendLevel );
          */

   Uint32 c = color.MapRGBA( PG_Application::GetScreen()->format, 255-GetTransparency());
   for ( Colors::iterator i = colors.begin(); i != colors.end(); ++i)
      if ( fraction < i->first ) {
         PG_Color col = i->second;
         c = col.MapRGBA( PG_Application::GetScreen()->format, 255-GetTransparency());
      }   


   SDL_FillRect(PG_Application::GetScreen(), &d, c);

}


void BarGraphWidget::setFraction( float f )
{
   fraction = f;
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



      PropertyEditorWidget :: PropertyEditorWidget ( PG_Widget *parent, const PG_Rect &r, const std::string &style, int labelWidthPercentage ) : PG_ScrollWidget( parent, r, style ), styleName( style ), ypos ( 0 ), lineHeight(25), lineSpacing(2), labelWidth(labelWidthPercentage)
      {
      };
      
      std::string PropertyEditorWidget :: GetStyleName() {
         return styleName;  
      };

      void PropertyEditorWidget :: Reload() {
         for ( PropertyFieldsType::iterator i = propertyFields.begin(); i != propertyFields.end(); ++i )
            (*i)->Reload();
      };
      
            
      bool PropertyEditorWidget :: Valid() {
         for ( PropertyFieldsType::iterator i = propertyFields.begin(); i != propertyFields.end(); ++i )
            if ( ! (*i)->Valid() )
               return false;
         return true;
      };
      
      bool PropertyEditorWidget :: Apply() {
         if ( !Valid() )
            return false;
                        
         for ( PropertyFieldsType::iterator i = propertyFields.begin(); i != propertyFields.end(); ++i )
            (*i)->Apply();
            
         return true;
      };
      
      PG_Rect PropertyEditorWidget :: RegisterProperty( const std::string& name, PropertyEditorField* propertyEditorField )
      {
         propertyFields.push_back( propertyEditorField );
         
         int w = Width() - my_widthScrollbar - 2 * lineSpacing;
      
         PG_Label* label = new PG_Label( this, PG_Rect( 0, ypos, w * labelWidth / 100 - 1, lineHeight), name );
         label->LoadThemeStyle( styleName, "Label" );
         PG_Rect r  ( w * labelWidth / 100 , ypos, w * ( 100 - labelWidth ) / 100  - 1, lineHeight );
         ypos += lineHeight + lineSpacing;
         return r;
      };
      
      PropertyEditorWidget :: ~PropertyEditorWidget() 
      {
         for ( PropertyFieldsType::iterator i = propertyFields.begin(); i != propertyFields.end(); ++i )
            delete *i;
      };
      
