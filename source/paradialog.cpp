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
#include "pgeventobject.h"
#include "pgpopupmenu.h"
#include "pgspinnerbox.h"
#include "pglog.h"
#include "pgmenubar.h"

#include "paradialog.h"
#include "events.h"
#include "gameoptions.h"
#include "sg.h"
#include "sdl/sound.h"

ASC_PG_App* pgApp = NULL;


ASC_PG_App :: ASC_PG_App ( const ASCString& themeName )
{
   this->themeName = themeName;
   EnableSymlinks(true);
   int i = 0;
   bool themeFound = false;
   ASCString path;
   do {
      path = getSearchPath ( i++ );
      if ( !path.empty() ) {
         AddArchive ( path.c_str() );
         if ( !themeFound )
             themeFound = AddArchive ( (path + themeName + ".zip").c_str() );
      }
   } while ( !path.empty() );
   PG_LogConsole::SetLogLevel ( PG_LOG_ERR );
   reloadTheme();
}

void ASC_PG_App :: reloadTheme()
{
   if ( !LoadTheme(themeName.c_str()))
      fatalError ( "Could not load Paragui theme for ASC");
}


/*
PG_Theme* ASC_PG_App::LoadTheme(const char* xmltheme, bool asDefault, const char* searchpath) {
	PG_Theme* theme = NULL;

	PG_LogDBG("Locating theme '%s' ...", xmltheme);

	// MacOS does not use file path separator '/', instead ':' is used
	// There could be clever solution for this, but for a while...
	// let's assume that "data" directory must exist in working directory on MacOS.
	// Masahiro Minami<elsur@aaa.letter.co.jp>
	// 01/05/06

	// add paths to the archive

	//#ifndef macintosh

	if(searchpath != NULL) {
		if(AddArchive(searchpath)) {
			PG_LogDBG("'%s' added to searchpath", searchpath);
		}
	}

	theme = PG_Theme::Load(xmltheme);

	if(theme && asDefault) {

		const char* c = theme->FindDefaultFontName();
		if(c == NULL) {
			PG_LogWRN("Unable to load default font ...");
			delete theme;
			return NULL;
		}

		DefaultFont = new PG_Font(c, theme->FindDefaultFontSize());
		DefaultFont->SetStyle(theme->FindDefaultFontStyle());

		PG_LogMSG("defaultfont: %s", c);
		PG_LogMSG("size: %i", DefaultFont->GetSize());

		my_background = theme->FindSurface("Background", "Background", "background");
		my_backmode = theme->FindProperty("Background", "Background", "backmode");
		SDL_Color* bc = theme->FindColor("Background", "Background", "backcolor");
		if(bc != NULL) {
			my_backcolor = *bc;
		}
		if(my_scaled_background) {
			// Destroyed scaled background if present
			SDL_FreeSurface(my_scaled_background);
			my_scaled_background = 0;
		}
	} else {

		PG_LogWRN("Failed to load !");
	}

	if((my_Theme != NULL) && asDefault) {
		delete my_Theme;
		my_Theme = NULL;
	}

	if(asDefault && theme) {
		my_Theme = theme;
	}

	return theme;
}
*/

//! A Paragui widget that fills the whole screen and redraws it whenever Paragui wants to it.
class MainScreenWidget : public PG_ThemeWidget, public PG_EventObject {
    bool gameInitialized;
    bool dirtyFlag;
public:
    MainScreenWidget( )
       : PG_ThemeWidget(NULL, PG_Rect ( 0, 0, ::getScreen()->w, ::getScreen()->h ), true),
         gameInitialized (false), dirtyFlag(true) {};

    //! to be called after ASC has completed loading and repaintdisplay() is available and working.
    void gameReady() { gameInitialized = true; Show(); };

    void setDirty() { dirtyFlag = true; };

protected:
//    void eventDraw (SDL_Surface* surface, const PG_Rect& rect);
    void eventBlit ( SDL_Surface* srf, const PG_Rect &src, const PG_Rect& dst );
};

MainScreenWidget* mainScreenWidget = NULL;
/*
void MainScreenWidget::eventDraw (SDL_Surface* surface, const PG_Rect& rect)
{
    PG_ThemeWidget::eventDraw(surface, rect);

    if ( gameInitialized ) {
       SDL_Surface* screen = ::getScreen();
       initASCGraphicSubsystem( surface, NULL );
       repaintdisplay();
       initASCGraphicSubsystem( screen, NULL );
    }
}
*/
void MainScreenWidget::eventBlit ( SDL_Surface* srf, const PG_Rect &src, const PG_Rect& dst )
{
    if ( gameInitialized && dirtyFlag ) {
       SDL_Surface* screen = ::getScreen();
       initASCGraphicSubsystem( srf, NULL );
       repaintdisplay();
       initASCGraphicSubsystem( screen, NULL );
       dirtyFlag = false;
    }
    PG_ThemeWidget::eventBlit( srf, src, dst );
}



void setupMainScreenWidget()
{
   mainScreenWidget = new MainScreenWidget();
   mainScreenWidget->gameReady();
}


//! Adapter class for using Paragui Dialogs in ASC. This class transfers the event control from ASC to Paragui and back. All new dialog classes should be derived from this class
class ASC_PG_Dialog : public PG_Window {
       SDL_Surface* background;
    protected:
       int quitModalLoop;
    public:
       ASC_PG_Dialog ( PG_Widget *parent, const PG_Rect &r, const char *windowtext, Uint32 flags=WF_DEFAULT, const char *style="Window", int heightTitlebar=25);
       int Run( );
       ~ASC_PG_Dialog();
};



ASC_PG_Dialog :: ASC_PG_Dialog ( PG_Widget *parent, const PG_Rect &r, const char *windowtext, Uint32 flags, const char *style, int heightTitlebar )
       :PG_Window ( parent, r, windowtext, flags, style, heightTitlebar ),
        quitModalLoop ( 0 )
{
   mainScreenWidget->setDirty();
//   SDL_mutexP ( eventHandlingMutex );
}

ASC_PG_Dialog::~ASC_PG_Dialog ()
{
//   SDL_mutexV ( eventHandlingMutex );
}

int ASC_PG_Dialog::Run ( )
{
   queueEvents ( true );
   while ( !quitModalLoop ) {
      SDL_Event event;
      if ( getQueuedEvent( event ))
         pgApp->PumpIntoEventQueue( &event );
      else
         SDL_Delay ( 2 );
   }
   queueEvents ( false );
   return quitModalLoop;
}




   // A testwindow class

 class SoundSettings : public ASC_PG_Dialog {
        CGameOptions::SoundSettings soundSettings;
        void updateSettings();
 public:
 	SoundSettings(PG_Widget* parent, const PG_Rect& r );
 protected:
 	bool eventButtonClick(int id, PG_Widget* widget);
 	bool eventScrollPos(int id, PG_Widget* widget, unsigned long data);
 	bool eventScrollTrack(int id, PG_Widget* widget, unsigned long data);
 };

SoundSettings::SoundSettings(PG_Widget* parent, const PG_Rect& r ) :
               ASC_PG_Dialog(parent, r, "Sound Settings", WF_SHOW_CLOSE )
{
        soundSettings = CGameOptions::Instance()->sound;

        PG_CheckButton* musb = new PG_CheckButton(this, 20, PG_Rect( 30, 50, 200, 20 ), "Enable Music" );
        new PG_Label ( this, PG_Rect(30, 80, 150, 20), "Music Volume" );
	PG_Slider* mus = new PG_Slider(this, 21, PG_Rect(180, 80, 200, 20), PG_SB_HORIZONTAL);
	mus->SetRange(0,100);
	mus->SetPosition(soundSettings.musicVolume);
        if ( soundSettings.muteMusic )
           musb->SetUnpressed();
        else
           musb->SetPressed();


        PG_CheckButton* sndb = new PG_CheckButton(this, 30, PG_Rect( 30, 150, 200, 20 ), "Enable Sound" );
        new PG_Label ( this, PG_Rect(30, 180, 150, 20), "Sound Volume" );
	PG_Slider* snd = new PG_Slider(this, 31, PG_Rect(180, 180, 200, 20), PG_SB_HORIZONTAL);
	snd->SetRange(0,100);
	snd->SetPosition(soundSettings.soundVolume);
        if ( soundSettings.muteEffects )
           sndb->SetUnpressed();
        else
           sndb->SetPressed();


	new PG_Button(this, 100, PG_Rect(30,r.h-40,(r.w-70)/2,30), "OK");
	new PG_Button(this, 101, PG_Rect(r.w/2+5,r.h-40,(r.w-70)/2,30), "Cancel");
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

bool SoundSettings::eventScrollPos(int id, PG_Widget* widget, unsigned long data){
	return false;
}

bool SoundSettings::eventScrollTrack(int id, PG_Widget* widget, unsigned long data) {
	if(id == 21){
                CGameOptions::Instance()->sound.musicVolume = data;
                CGameOptions::Instance()->setChanged();
                updateSettings();
		return true;
	}

	if(id == 31){
                CGameOptions::Instance()->sound.soundVolume = data;
                CGameOptions::Instance()->setChanged();
                updateSettings();
		return true;
	}
	return false;
}

bool SoundSettings::eventButtonClick(int id, PG_Widget* widget) {
	if (id==PG_WINDOW_CLOSE ) {
           quitModalLoop = 1;
           return true;
	}

	if(id == 100) {
           quitModalLoop = 1;
           return true;
	}

	if(id == 101) {
           quitModalLoop = 2;
           CGameOptions::Instance()->sound = soundSettings;
           updateSettings();
           return true;
	}

        //music
        if ( id == 20 ) {
           CGameOptions::Instance()->sound.muteMusic = !(static_cast<PG_CheckButton*>(widget))-> GetPressed();
           updateSettings();
           return true;
        }

        //sound effects
        if ( id == 30 ) {
           CGameOptions::Instance()->sound.muteEffects = !(static_cast<PG_CheckButton*>(widget))-> GetPressed();
           updateSettings();
           return true;
        }

	return PG_Window::eventButtonClick(id, widget);
}


void soundSettings( )
{
  SoundSettings wnd1(NULL, PG_Rect(50,50,500,300));
  wnd1.Show();
  wnd1.Run();
}
