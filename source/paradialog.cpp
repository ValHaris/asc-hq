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

#include "paradialog.h"
#include "events.h"
#include "gameoptions.h"
#include "sg.h"
#include "sdl/sound.h"

#include "resourceplacement.h"



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
         AddArchive ( path.c_str() );
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
   if ( !LoadTheme(themeName.c_str()))
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
            
       }else
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
            
       }else
         SDL_Delay ( 2 );
   }
#ifndef sgmain
   setEventRouting ( eventQueue, !eventQueue );
#endif   
   return quitModalLoopValue;
}




   // A testwindow class

 class SoundSettings : public ASC_PG_Dialog {
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


bool SoundSettings::eventScrollTrack(PG_Slider* slider, long data) {
	if(slider->GetID() == 21){
                CGameOptions::Instance()->sound.musicVolume = data;
                CGameOptions::Instance()->setChanged();
                updateSettings();
		return true;
	}

	if(slider->GetID() == 31){
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




 


