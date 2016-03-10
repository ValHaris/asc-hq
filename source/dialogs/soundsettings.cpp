/*
     This file is part of Advanced Strategic Command; http://www.asc-hq.de
     Copyright (C) 1994-2010  Martin Bickel  and  Marc Schellenberger
 
     This program is free software; you can redistribute it and/or modify
     it under the terms of the GNU General Public License as published by
     the Free Software Foundation; either version 2 of the License, or
     (at your option) any later version.
 
     This program is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU General Public License for more details.
 
     You should have received a copy of the GNU General Public License
     along with this program; see the file COPYING. If not, write to the 
     Free Software Foundation, Inc., 59 Temple Place, Suite 330, 
     Boston, MA  02111-1307  USA
*/


#include "../paradialog.h"
#include "../gameoptions.h"
#include "../soundList.h"
#include "../widgets/textrenderer.h"

class SoundSettings : public ASC_PG_Dialog
{
      CGameOptions::SoundSettings sSettings;
      void updateSettings();
   public:
      SoundSettings(PG_Widget* parent, const PG_Rect& r, PG_MessageObject* caller);
      static void soundSettings(PG_MessageObject* caller);      
   protected:

      bool toggleMusic( bool state);
      bool toggleSound( bool state);
      bool buttonEvent( );
      bool eventScrollTrack_sound(long data);
      bool eventScrollTrack_music(long data);

      bool diag();
};


void soundSettings( PG_MessageObject* caller)
{
   SoundSettings::soundSettings(caller );
}


SoundSettings::SoundSettings(PG_Widget* parent, const PG_Rect& r, PG_MessageObject* c ) :
      ASC_PG_Dialog(parent, r, "Sound Settings", SHOW_CLOSE )
{
   sSettings = CGameOptions::Instance()->sound;

   PG_CheckButton* musb = new PG_CheckButton(this, PG_Rect( 30, 50, 200, 20 ), "Enable Music", 1 );
   musb->sigClick.connect( sigc::mem_fun( *this, &SoundSettings::toggleMusic ));
   new PG_Label ( this, PG_Rect(30, 80, 150, 20), "Music Volume" );
   PG_Slider* mus = new PG_Slider(this, PG_Rect(180, 80, 200, 20), PG_Slider::HORIZONTAL, 21);
   mus->SetRange(0,100);
   mus->SetPosition(sSettings.musicVolume);
   mus->sigScrollTrack.connect( sigc::mem_fun( *this, &SoundSettings::eventScrollTrack_music ));

   if ( sSettings.muteMusic )
      musb->SetUnpressed();
   else
      musb->SetPressed();


   PG_CheckButton* sndb = new PG_CheckButton(this, PG_Rect( 30, 150, 200, 20 ), "Enable Sound", 2 );
   sndb->sigClick.connect( sigc::mem_fun( *this, &SoundSettings::toggleSound ));
   new PG_Label ( this, PG_Rect(30, 180, 150, 20), "Sound Volume" );
   PG_Slider* snd = new PG_Slider(this, PG_Rect(180, 180, 200, 20), PG_Slider::HORIZONTAL, 31);
   snd->SetRange(0,100);
   snd->SetPosition(sSettings.soundVolume);
   snd->sigScrollTrack.connect( sigc::mem_fun( *this, &SoundSettings::eventScrollTrack_sound ));
   if ( sSettings.muteEffects )
      sndb->SetUnpressed();
   else
      sndb->SetPressed();


   PG_Button* b1 = new PG_Button(this, PG_Rect(30,r.h-40,(r.w-70)/2,30), "OK", 100);
   b1->sigClick.connect( sigc::hide(sigc::mem_fun( *this, &SoundSettings::closeWindow )));

   PG_Button* b2 = new PG_Button(this, PG_Rect(r.w/2+5,r.h-40,(r.w-70)/2,30), "Cancel", 101);
   b2->sigClick.connect( sigc::hide(sigc::mem_fun( *this, &SoundSettings::buttonEvent )));

   sigClose.connect( sigc::mem_fun( *this, &SoundSettings::closeWindow ));
   

   PG_Button* b3 = new PG_Button(this, PG_Rect( Width() -100, 25, 90, 20 ), "Diagnostics" );
   b3->sigClick.connect( sigc::hide( sigc::mem_fun( *this, &SoundSettings::diag)));

   // caller = c;
   // SetInputFocus();
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

bool SoundSettings::toggleMusic( bool state)
{
    CGameOptions::Instance()->sound.muteMusic = !state;
    updateSettings();
    return true;
}

bool SoundSettings::toggleSound( bool state)
{
   CGameOptions::Instance()->sound.muteEffects = !state;
   updateSettings();
   return true;
}


bool SoundSettings::eventScrollTrack_music(long data)
{
  CGameOptions::Instance()->sound.musicVolume = data;
  CGameOptions::Instance()->setChanged();
  updateSettings();
  return true;
}

bool SoundSettings::eventScrollTrack_sound(long data)
{
  CGameOptions::Instance()->sound.soundVolume = data;
  CGameOptions::Instance()->setChanged();
  updateSettings();
  return true;
}


bool SoundSettings::diag()
{
   ASCString text = SoundSystem::getInstance()->getDiagnosticText();
   ViewFormattedText vft( "Sound Diagnostics", text, PG_Rect( -1, -1, 500, 500 ));
   vft.Show();
   vft.RunModal();
   return true;
}


bool SoundSettings::buttonEvent( )
{
   quitModalLoop(2);
   CGameOptions::Instance()->sound = sSettings;
   updateSettings();
   return true;
}

void SoundSettings::soundSettings(PG_MessageObject* caller)
{
   // printf("c1c %d \n", ticker );
   SoundSettings wnd1(NULL, PG_Rect(50,50,500,300), caller);
   // printf("c2c %d \n", ticker );
   wnd1.Show();
   // printf("c3c %d \n", ticker );
   wnd1.RunModal();
   // printf("c4c %d \n", ticker );
}

