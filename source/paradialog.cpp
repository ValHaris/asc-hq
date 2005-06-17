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


#include "paradialog.h"
#include "events.h"
#include "gameoptions.h"
#include "sg.h"
#include "spfst.h"


#include "resourceplacement.h"

#include "iconrepository.h"
#include "graphics/drawing.h"



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



ASC_PG_App :: ASC_PG_App ( const ASCString& themeName )
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
         }
      }
   } while ( !path.empty() );
   PG_LogConsole::SetLogLevel ( PG_LOG_ERR );
   reloadTheme();

   pgApp = this;
   SetEventSupplier ( &eventSupplier );
}

ASC_PG_App& getPGApplication()
{
   return *pgApp;
}



bool ASC_PG_App:: InitScreen ( int w, int h, int depth, Uint32 flags )
{
   bool result = PG_Application::InitScreen ( w, h, depth, flags  );
   if ( result ) {
      initASCGraphicSubsystem ( GetScreen(), NULL );
      Surface::SetScreen( GetScreen() );
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
/*
bool ASC_PG_App::eventKeyUp (const SDL_KeyboardEvent *key){
 if(key->keysym.sym == SDLK_ESCAPE){
 GameDialog::gameDialog();
 }

return true;
}
*/

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


bool ASC_PG_Dialog::eventKeyUp(const SDL_KeyboardEvent *key){
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











class Emboss : public PG_Widget {
   public:

      Emboss (PG_Widget *parent, const PG_Rect &rect ) : PG_Widget( parent, rect, false )
      {
      }


      void eventBlit (SDL_Surface *surface, const PG_Rect &src, const PG_Rect &dst) {
         Surface s = Surface::Wrap( PG_Application::GetScreen() );
         rectangle<4> ( s, SPoint(dst.x, dst.y), dst.w, dst.h, ColorMerger_Brightness<4>( 1.4 ), ColorMerger_Brightness<4>( 0.7 ));
      };
};





const int widgetTypeNum = 9;
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
     "multilinetext"
   };

enum  WidgetTypes  { Image,
                     Area,
                     StaticLabel,
                     TextOutput,
                     BarGraph,
                     SpecialDisplay,
                     SpecialInput,
                     Dummy,
                     MultiLineText };

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



Panel::WidgetParameters::WidgetParameters()
      : backgroundMode(PG_Draw::TILE),  textAlign( PG_Label::LEFT ), fontAlpha(255), fontSize(8), transparency(0), hidden(false)
{
}

void  Panel::WidgetParameters::runTextIO ( PropertyReadingContainer& pc )
{
   pc.addString( "BackgroundImage", backgroundImage, backgroundImage );
   int i = backgroundMode;
   pc.addNamedInteger( "BackgroundMode", i, imageModeNum, imageModes, i);
   backgroundMode = PG_Draw::BkMode( i );

   int ta = textAlign;
   pc.addNamedInteger( "TextAlign", ta, textAlignNum, textAlignment, ta );
   textAlign = PG_Label::TextAlign( ta );

   pc.addInteger("FontColor", fontColor, fontColor );
   pc.addString("FontName", fontName, fontName );
   pc.addInteger("FontAlpha", fontAlpha, fontAlpha );
   pc.addInteger("FontSize", fontSize, fontSize );
   pc.addInteger("BackgroundColor", backgroundColor,  backgroundColor );
   pc.addInteger("Transparency", transparency, transparency );
   pc.addBool( "hidden", hidden, hidden );

}


void  Panel::WidgetParameters::assign( BarGraphWidget* widget )
{
   if ( !widget )
      return;

   widget->setColor( backgroundColor );

   assign( (PG_ThemeWidget*)widget );

}


void  Panel::WidgetParameters::assign( PG_ThemeWidget* widget )
{
   if ( !widget )
      return;

   if ( !backgroundImage.empty() )
      widget->SetBackground( IconRepository::getIcon(backgroundImage).getBaseSurface(), backgroundMode );
   else
      widget->SetBackground( NULL );

   widget->SetBackgroundColor( backgroundColor );

   assign( (PG_Widget*)widget );

}

void  Panel::WidgetParameters::assign( PG_Label* widget )
{
   if ( !widget )
      return;


   widget->SetAlignment( textAlign );

   assign( (PG_Widget*)widget );
}


void  Panel::WidgetParameters::assign( PG_Widget* widget )
{
   if ( !widget )
      return;

   widget->SetFontColor( fontColor );
   if ( !fontName.empty() )
      widget->SetFontName( fontName );
   widget->SetFontAlpha( fontAlpha );
   widget->SetFontSize( fontSize );
   widget->SetTransparency( transparency );
   if ( hidden )
      widget->Hide(false);
}


Panel::Panel ( PG_Widget *parent, const PG_Rect &r, const ASCString& panelName_, bool loadTheme )
      : PG_Window ( parent, r, "", DEFAULT, "Panel", 9 ), panelName( panelName_ ), textPropertyGroup(NULL)
{
   if ( loadTheme )
      setup();
      
      // FIXME Hide button does not delete Panel
   BringToFront();
      
}


PG_Rect Panel::parseRect ( PropertyReadingContainer& pc, PG_Widget* parent )
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
         x2 = parent->Height() + y2;

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


void Panel::parsePanelASCTXT ( PropertyReadingContainer& pc, PG_Widget* parent, WidgetParameters widgetParams )
{
   ASCString name;
   pc.addString( "name", name, "" );
   parent->SetName( name );

   if ( pc.find( "userHandler" )) {
      ASCString label;
      pc.addString( "userHandler", label);
      userHandler( label, pc, parent, widgetParams );
   }

   int widgetNum;
   pc.addInteger( "WidgetNum", widgetNum, 0 );

   for ( int i = 0; i < widgetNum; ++i) {
      pc.openBracket( ASCString("Widget") + strrr(i));


      PG_Rect r = parseRect( pc, parent );

      widgetParams.runTextIO( pc );


      int type;
      pc.addNamedInteger( "type", type, widgetTypeNum, widgetTypes );

      if ( type == Image ) {
         ASCString filename;
         pc.addString( "FileName", filename, "" );
         int imgMode;
         pc.addNamedInteger( "mode", imgMode, imageModeNum, imageModes, 0 );

         if ( !filename.empty() ) {
            try {
               Surface& surf = IconRepository::getIcon(filename);

               PG_Image* img = new PG_Image( parent, PG_Point(r.x, r.y ), surf.getBaseSurface(), false, PG_Draw::BkMode(imgMode) );

               widgetParams.assign ( img );
               parsePanelASCTXT( pc, img, widgetParams );
            } catch ( tfileerror ) {
               displaymessage( "unable to load " + filename, 1 );
            }
         } else {
            PG_Image* img = new PG_Image( parent, PG_Point(r.x, r.y ), NULL, false, PG_Draw::BkMode(imgMode) );
            widgetParams.assign ( img );
            parsePanelASCTXT( pc, img, widgetParams );
         }

      }
      if ( type == Area ) {
         bool mode;
         pc.addBool( "in", mode, true );

         ASCString style;
         pc.addString( "style", style, "Emboss" );

         Emboss* tw = new Emboss ( parent, r );
         // PG_ThemeWidget* tw = new PG_ThemeWidget ( parent, r, style );
         widgetParams.assign ( tw );
         parsePanelASCTXT( pc, tw, widgetParams );
      }

      if ( type == StaticLabel ) {
         ASCString text;
         pc.addString( "text", text );

         PG_Label* lb = new PG_Label ( parent, r, text );
         widgetParams.assign ( lb );
         parsePanelASCTXT( pc, lb, widgetParams );
      }
      if ( type == TextOutput ) {
         PG_Label* lb = new PG_Label ( parent, r );
         widgetParams.assign ( lb );
         parsePanelASCTXT( pc, lb, widgetParams );
      }
      if ( type == MultiLineText ) {
         PG_MultiLineEdit* lb = new PG_MultiLineEdit ( parent, r );
         lb->SetBorderSize(0);
         lb->SetEditable(false);
         widgetParams.assign ( lb );
         parsePanelASCTXT( pc, lb, widgetParams );
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
      }


      if ( type == SpecialDisplay ) {
         SpecialDisplayWidget* sw = new SpecialDisplayWidget ( parent, r );
         widgetParams.assign ( sw );

         parsePanelASCTXT( pc, sw, widgetParams );
      }

      if ( type == SpecialInput ) {
         SpecialInputWidget* sw = new SpecialInputWidget ( parent, r );

         parsePanelASCTXT( pc, sw, widgetParams );
      }

      if ( type == Dummy ) {
         SpecialInputWidget* sw = new SpecialInputWidget ( parent, r );
         parsePanelASCTXT( pc, sw, widgetParams );
      }


      pc.closeBracket();
   }
}



void Panel::setLabelText ( const ASCString& widgetName, const ASCString& text, PG_Widget* parent )
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

void Panel::setLabelColor ( const ASCString& widgetName, PG_Color color, PG_Widget* parent )
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


void Panel::setLabelText ( const ASCString& widgetName, int i, PG_Widget* parent )
{
   ASCString s = ASCString::toString(i);
   setLabelText ( widgetName, s, parent );
}

void Panel::setImage ( const ASCString& widgetName, Surface& image, PG_Widget* parent )
{
   setImage( widgetName, image.getBaseSurface(), parent);
}

void Panel::setImage ( const ASCString& widgetName, SDL_Surface* image, PG_Widget* parent )
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

void Panel::hide ( const ASCString& widgetName, PG_Widget* parent )
{
   if ( !parent )
      parent = this;

   PG_Widget* i = parent->FindChild( widgetName, true );
   if ( i )
      i->Hide();
}

void Panel::show ( const ASCString& widgetName, PG_Widget* parent )
{
   if ( !parent )
      parent = this;

   PG_Widget* i = parent->FindChild( widgetName, true );
   if ( i )
      i->Show();
}



void Panel::setBargraphValue( const ASCString& widgetName, float fraction )
{
   BarGraphWidget* bgw = dynamic_cast<BarGraphWidget*>( FindChild( widgetName, true ) );
   if ( bgw )
      bgw->setFraction( fraction );
}


void Panel::setBarGraphColor( const ASCString& widgetName, PG_Color color )
{
   BarGraphWidget* bgw = dynamic_cast<BarGraphWidget*>( FindChild( widgetName, true ) );
   if ( bgw )
      bgw->setColor( color );
}


Panel::WidgetParameters Panel::getDefaultWidgetParams()
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


bool Panel::setup()
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

Panel::~Panel()
{
   if ( textPropertyGroup )
      delete textPropertyGroup;
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


   SDL_FillRect(PG_Application::GetScreen(), (SDL_Rect*)&d, c);

}


void BarGraphWidget::setFraction( float f )
{
   fraction = f;
}


PG_Rect calcMessageBoxSize( const ASCString& message )
{
  int counter = 0;
  for ( int i = 0; i< message.length(); ++i)
     if ( message[i] == '\n' )
        counter++;

  return PG_Rect( 100, 100, 500, 150 + counter * 10 );
}



void errorMessageDialog( const ASCString& message )
{
   PG_Rect size = calcMessageBoxSize(message);
   PG_MessageBox msg( NULL, calcMessageBoxSize(message), "Error", message,PG_Rect(200,100,100,40), "OK" );
   msg.Show();
   msg.RunModal();
}

void warningMessageDialog( const ASCString& message )
{
   PG_Rect size = calcMessageBoxSize(message);
   PG_MessageBox msg( NULL, size, "Warning", message,PG_Rect(size.w/2 - 50,size.h - 40, 100, 30), "OK" );
   msg.Show();
   msg.RunModal();
}



MessageDialog::MessageDialog(PG_Widget* parent, const PG_Rect& r, const std::string& windowtitle, const std::string& windowtext, const PG_Rect& btn1, const std::string& btn1text, const PG_Rect& btn2, const std::string& btn2text, PG_Label::TextAlign textalign, const std::string& style) :
ASC_PG_Dialog(parent, r, windowtitle, MODAL) {

	my_btnok = new PG_Button(this, btn1, btn1text);
	my_btnok->SetID(1);
	my_btnok->sigClick.connect(slot(*this, &MessageDialog::handleButton));
	
	my_btncancel = new PG_Button(this, btn2, btn2text);
	my_btncancel->SetID(2);
	my_btncancel->sigClick.connect(slot(*this, &MessageDialog::handleButton));

	Init(windowtext, textalign, style);
}

MessageDialog::MessageDialog(PG_Widget* parent, const PG_Rect& r, const std::string& windowtitle, const std::string& windowtext, const PG_Rect& btn1, const std::string& btn1text, PG_Label::TextAlign textalign, const std::string& style) :
ASC_PG_Dialog(parent, r, windowtitle, MODAL) {

	my_btnok = new PG_Button(this, btn1, btn1text);
	my_btnok->SetID(1);
	my_btnok->sigClick.connect(slot(*this, &MessageDialog::handleButton));
	my_btncancel = NULL;

	Init(windowtext, textalign, style);
}

//Delete the Buttons
MessageDialog::~MessageDialog() {
	delete my_btnok;
	delete my_btncancel;
}

void MessageDialog::Init(const std::string& windowtext, int textalign, const std::string& style) {

	my_textbox = new PG_RichEdit(this, PG_Rect(10, 40, my_width-20, my_height-50));
	my_textbox->SendToBack();
	my_textbox->SetTransparency(255);
	my_textbox->SetText(windowtext);

	my_msgalign = textalign;

	LoadThemeStyle(style);
}

void MessageDialog::LoadThemeStyle(const std::string& widgettype) {
	PG_Window::LoadThemeStyle(widgettype);

	my_btnok->LoadThemeStyle(widgettype, "Button1");
	if(my_btncancel) {
		my_btncancel->LoadThemeStyle(widgettype, "Button2");
	}
}

//Event?
bool MessageDialog::handleButton(PG_Button* button) {
	//Set Buttonflag to ButtonID
        quitModalLoop( button->GetID() );
	return true;
}

