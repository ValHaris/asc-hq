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

#include "paradialog.h"
#include "events.h"
#include "gameoptions.h"
#include "sg.h"
#include "sdl/sound.h"

#include "resourceplacement.h"
#include "textfile_evaluation.h"

#include "iconrepository.h"


ASC_PG_App* pgApp = NULL;

ASC_PG_App :: ASC_PG_App ( const ASCString& themeName )
      : quitModalLoopValue ( 0 )
{
   this->themeName = themeName;
   EnableSymlinks(true);
   int i = 0;
   bool themeFound = false;
   ASCString path;
   do {
      path = getSearchPath ( i++ );
      if ( !path.empty() ) {
         AddArchive ( path );
         if ( !themeFound )
            themeFound = AddArchive ( (path + themeName + ".zip").c_str() );
      }
   } while ( !path.empty() );
   PG_LogConsole::SetLogLevel ( PG_LOG_ERR );
   reloadTheme();

   pgApp = this;
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
      fatalError ( "Could not load Paragui theme for ASC");
}


bool ASC_PG_App :: enableLegacyEventHandling( bool use )
{
   return !setEventRouting ( !use, use );
}


int ASC_PG_App::Run ( )
{
   enableLegacyEventHandling ( false );

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
   enableLegacyEventHandling ( true );
   return quitModalLoopValue;
}






ASC_PG_Dialog :: ASC_PG_Dialog ( PG_Widget *parent, const PG_Rect &r, const ASCString& windowtext, WindowFlags flags, const ASCString& style, int heightTitlebar )
      :PG_Window ( parent, r, windowtext, flags, style, heightTitlebar ),
      quitModalLoopValue ( 0 )
{
   //   mainScreenWidget->setDirty();
   //   SDL_mutexP ( eventHandlingMutex );
}

ASC_PG_Dialog::~ASC_PG_Dialog ()
{
   //   SDL_mutexV ( eventHandlingMutex );
}

int ASC_PG_Dialog::Run ( )
{
#ifndef sgmain
   bool eventQueue = setEventRouting ( true, false );
#endif

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
#ifndef sgmain
   setEventRouting ( eventQueue, !eventQueue );
#endif

   return quitModalLoopValue;
}




// A testwindow class

class SoundSettings : public ASC_PG_Dialog
{
      CGameOptions::SoundSettings soundSettings;
      void updateSettings();
   public:
      SoundSettings(PG_Widget* parent, const PG_Rect& r );
   protected:

      bool radioButtonEvent( PG_RadioButton* button, bool state);
      bool buttonEvent( PG_Button* button );
      bool eventScrollTrack(PG_Slider* slider, long data);
      bool closeWindow()
      {
         quitModalLoop(1);
         return true;
      };

};

SoundSettings::SoundSettings(PG_Widget* parent, const PG_Rect& r ) :
      ASC_PG_Dialog(parent, r, "Sound Settings", SHOW_CLOSE )
{
   soundSettings = CGameOptions::Instance()->sound;

   PG_CheckButton* musb = new PG_CheckButton(this, PG_Rect( 30, 50, 200, 20 ), "Enable Music", 1 );
   musb->sigClick.connect(SigC::slot( *this, &SoundSettings::radioButtonEvent ));
   new PG_Label ( this, PG_Rect(30, 80, 150, 20), "Music Volume" );
   PG_Slider* mus = new PG_Slider(this, PG_Rect(180, 80, 200, 20), PG_Slider::HORIZONTAL, 21);
   mus->SetRange(0,100);
   mus->SetPosition(soundSettings.musicVolume);
   if ( soundSettings.muteMusic )
      musb->SetUnpressed();
   else
      musb->SetPressed();


   PG_CheckButton* sndb = new PG_CheckButton(this, PG_Rect( 30, 150, 200, 20 ), "Enable Sound", 2 );
   sndb->sigClick.connect(SigC::slot( *this, &SoundSettings::radioButtonEvent ));
   new PG_Label ( this, PG_Rect(30, 180, 150, 20), "Sound Volume" );
   PG_Slider* snd = new PG_Slider(this, PG_Rect(180, 180, 200, 20), PG_Slider::HORIZONTAL, 31);
   snd->SetRange(0,100);
   snd->SetPosition(soundSettings.soundVolume);
   if ( soundSettings.muteEffects )
      sndb->SetUnpressed();
   else
      sndb->SetPressed();


   PG_Button* b1 = new PG_Button(this, PG_Rect(30,r.h-40,(r.w-70)/2,30), "OK", 100);
   b1->sigClick.connect(SigC::slot( *this, &SoundSettings::closeWindow ));

   PG_Button* b2 = new PG_Button(this, PG_Rect(r.w/2+5,r.h-40,(r.w-70)/2,30), "Cancel", 101);
   b2->sigClick.connect(SigC::slot( *this, &SoundSettings::buttonEvent ));

   sigClose.connect( SigC::slot( *this, &SoundSettings::closeWindow ));
}


void SoundSettings::updateSettings()
{
   SoundSystem::getInstance()->setMusicVolume( CGameOptions::Instance()->sound.musicVolume );
   SoundSystem::getInstance()->setEffectVolume( CGameOptions::Instance()->sound.soundVolume );
   SoundSystem::getInstance()->setEffectsMute( CGameOptions::Instance()->sound.muteEffects );
   if ( CGameOptions::Instance()->sound.muteMusic )
      SoundSystem::getInstance()->pauseMusic();
   else
      SoundSystem::getInstance()->resumeMusic();

}

bool SoundSettings::radioButtonEvent( PG_RadioButton* button, bool state)
{
   if ( button->GetID() == 1 )
      CGameOptions::Instance()->sound.muteMusic = !state;
   if ( button->GetID() == 2 )
      CGameOptions::Instance()->sound.muteEffects = !state;
   updateSettings();
   return true;
}


bool SoundSettings::eventScrollTrack(PG_Slider* slider, long data)
{
   if(slider->GetID() == 21) {
      CGameOptions::Instance()->sound.musicVolume = data;
      CGameOptions::Instance()->setChanged();
      updateSettings();
      return true;
   }

   if(slider->GetID() == 31) {
      CGameOptions::Instance()->sound.soundVolume = data;
      CGameOptions::Instance()->setChanged();
      updateSettings();
      return true;
   }
   return false;
}



bool SoundSettings::buttonEvent( PG_Button* button )
{
   quitModalLoop(2);
   CGameOptions::Instance()->sound = soundSettings;
   updateSettings();
   return true;
}


void soundSettings( )
{
   // printf("c1c %d \n", ticker );
   SoundSettings wnd1(NULL, PG_Rect(50,50,500,300));
   // printf("c2c %d \n", ticker );
   wnd1.Show();
   // printf("c3c %d \n", ticker );
   wnd1.Run();
   // printf("c4c %d \n", ticker );
}



const int widgetTypeNum = 7;
const char* widgetTypes[widgetTypeNum]
=
   { "image",
     "area",
     "statictext",
     "textoutput",
     "bargraph",
     "specialDisplay",
     "specialInput"
   };

enum  WidgetTypes  { Image,
                     Area,
                     StaticLabel,
                     TextOutput,
                     BarGraph,
                     SpecialDisplay,
                     SpecialInput };

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


class WidgetParameters
{
   public:
      WidgetParameters();
      ASCString backgroundImage;
      PG_Draw::BkMode backgroundMode;
      PG_Label::TextAlign textAlign;
      int fontColor;
      ASCString fontName;
      int fontAlpha;
      int fontSize;
      int backgroundColor;
      int transparency;
      void assign( PG_Widget* widget );
      void assign( BarGraphWidget* widget );
      void assign( PG_ThemeWidget* widget );
      void assign( PG_Label* widget );
      void runTextIO ( PropertyReadingContainer& pc );
};

WidgetParameters::WidgetParameters()
      : backgroundMode(PG_Draw::TILE),  textAlign( PG_Label::LEFT ), fontAlpha(255), fontSize(8), transparency(0)
{
}

void  WidgetParameters::runTextIO ( PropertyReadingContainer& pc )
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

}


void  WidgetParameters::assign( BarGraphWidget* widget )
{
   if ( !widget )
      return;

   widget->setColor( backgroundColor );

   assign( (PG_ThemeWidget*)widget );

}


void  WidgetParameters::assign( PG_ThemeWidget* widget )
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

void  WidgetParameters::assign( PG_Label* widget )
{
   if ( !widget )
      return;


   widget->SetAlignment( textAlign );

   assign( (PG_Widget*)widget );
}


void  WidgetParameters::assign( PG_Widget* widget )
{
   if ( !widget )
      return;

   widget->SetFontColor( fontColor );
   if ( !fontName.empty() )
      widget->SetFontName( fontName );
   widget->SetFontAlpha( fontAlpha );
   widget->SetFontSize( fontSize );
   widget->SetTransparency( transparency );
}


void parsePanelASCTXT ( PropertyReadingContainer& pc, PG_Widget* parent, WidgetParameters widgetParams )
{
   int widgetNum;
   pc.addInteger( "WidgetNum", widgetNum, 0 );


   for ( int i = 0; i < widgetNum; ++i) {
      pc.openBracket( ASCString("Widget") + strrr(i));

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


      widgetParams.runTextIO( pc );


      int type;
      pc.addNamedInteger( "type", type, widgetTypeNum, widgetTypes );

      if ( type == Image ) {
         ASCString filename;
         pc.addString( "FileName", filename );
         int imgMode;
         pc.addNamedInteger( "mode", imgMode, imageModeNum, imageModes, 0 );

         try {
            Surface& surf = IconRepository::getIcon(filename);

            PG_Image* img = new PG_Image( parent, PG_Point(r.x, r.y ), surf.getBaseSurface(), false, PG_Draw::BkMode(imgMode) );
            widgetParams.assign ( img );
            parsePanelASCTXT( pc, img, widgetParams );
         } catch ( tfileerror ) {
            displaymessage( "unable to load " + filename, 1 );
         }
      }
      if ( type == Area ) {
         bool mode;
         pc.addBool( "in", mode, true );

         ASCString style;
         pc.addString( "style", style, "Emboss" );


         PG_ThemeWidget* tw = new PG_ThemeWidget ( parent, r, style );
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

         ASCString name;
         pc.addString( "name", name );

         PG_Label* lb = new PG_Label ( parent, r );
         widgetParams.assign ( lb );
         lb->SetName( name );
         parsePanelASCTXT( pc, lb, widgetParams );
      }
      if ( type == BarGraph ) {
         ASCString name;
         pc.addString( "name", name );

         BarGraphWidget* bg = new BarGraphWidget ( parent, r );
         widgetParams.assign ( bg );
         bg->SetName( name );
         parsePanelASCTXT( pc, bg, widgetParams );
      }


      if ( type == SpecialDisplay ) {
         ASCString name;
         pc.addString("name", name );
         SpecialDisplayWidget* sw = new SpecialDisplayWidget ( parent, r );
         sw->SetName( name );
         widgetParams.assign ( sw );

         parsePanelASCTXT( pc, sw, widgetParams );
      }

      if ( type == SpecialInput ) {
         ASCString name;
         pc.addString("name", name );
         SpecialInputWidget* sw = new SpecialInputWidget ( parent, r );
         sw->SetName( name );

         parsePanelASCTXT( pc, sw, widgetParams );
      }


      pc.closeBracket();
   }
}

Panel::Panel ( PG_Widget *parent, const PG_Rect &r, const ASCString& panelName_, bool loadTheme )
      : PG_Window ( parent, r, "", DEFAULT, "Panel", 9 ), panelName( panelName_ )
{
   if ( loadTheme )
      setup();
}


void Panel::setLabelText ( const ASCString& widgetName, const ASCString& text )
{
   PG_Label* l = dynamic_cast<PG_Label*>( FindChild( widgetName, true ) );
   if ( l )
      l->SetText( text );
}


void Panel::setBargraphValue( const ASCString& widgetName, float fraction )
{
   BarGraphWidget* bgw = dynamic_cast<BarGraphWidget*>( FindChild( widgetName, true ) );
   if ( bgw )
      bgw->setFraction( fraction );
}


bool Panel::setup()
{
   try {

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

      tnfilestream s ( panelName.toLower() + ".ascgui", tnstream::reading );

      TextFormatParser tfp ( &s );
      auto_ptr<TextPropertyGroup> tpg ( tfp.run());

      PropertyReadingContainer pc ( "panel", tpg.get() );

      int w, h;
      pc.addInteger( "width", w, 0 );
      pc.addInteger( "height", h, 0 );

      if ( w > 0 && h > 0 )
         SizeWidget( w, h, false );

      WidgetParameters widgetParameters = defaultWidgetParameters;
      widgetParameters.assign ( this );
      SetBackground( IconRepository::getIcon(panelBackgroundImage).getBaseSurface() );
      
      widgetParameters.runTextIO( pc );
         
      parsePanelASCTXT( pc, this, widgetParameters );
      
      return true;
   } catch ( ParsingError err ) {
      displaymessage( ASCString("parsing error: ") + err.getMessage(), 1 );
   } catch ( tfileerror err ) {
      displaymessage( ASCString("could not acces file: ") + err.getFileName(), 1 );
   }
   return false;

}




BarGraphWidget:: BarGraphWidget (PG_Widget *parent, const PG_Rect &rect ) : PG_ThemeWidget( parent, rect, false ), fraction(1)
{
}

void BarGraphWidget::eventBlit (SDL_Surface *surface, const PG_Rect &src, const PG_Rect &dst)
{
   PG_Rect d = dst;
   d.w = min( max(0, int( float(dst.w) * fraction)), int(dst.w)) ;
   
/*   PG_Draw::DrawThemedSurface( 
          surface, 
          d, 
          my_has_gradient ? &my_gradient : 0,
          my_background,
          my_backgroundMode,
          my_blendLevel );
          */
          
   Uint32 c = color.MapRGBA( PG_Application::GetScreen()->format, 255-GetTransparency());
   
   SDL_FillRect(PG_Application::GetScreen(), (SDL_Rect*)&d, c);
          
}

void BarGraphWidget:: setColor( int c )
{
   color = c;
}

void BarGraphWidget::setFraction( float f )
{
   fraction = f;
}
