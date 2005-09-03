/*
     This file is part of Advanced Strategic Command; http://www.asc-hq.de
     Copyright (C) 1994-1999  Martin Bickel  and  Marc Schellenberger
 
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

class SoundSettings : public ASC_PG_Dialog
{
      CGameOptions::SoundSettings sSettings;
      void updateSettings();
   public:
      SoundSettings(PG_Widget* parent, const PG_Rect& r, PG_MessageObject* caller);
      static void soundSettings(PG_MessageObject* caller);      
   protected:

      bool radioButtonEvent( PG_RadioButton* button, bool state);
      bool buttonEvent( PG_Button* button );
      bool eventScrollTrack(PG_Slider* slider, long data);
      
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
   musb->sigClick.connect(SigC::slot( *this, &SoundSettings::radioButtonEvent ));
   new PG_Label ( this, PG_Rect(30, 80, 150, 20), "Music Volume" );
   PG_Slider* mus = new PG_Slider(this, PG_Rect(180, 80, 200, 20), PG_Slider::HORIZONTAL, 21);
   mus->SetRange(0,100);
   mus->SetPosition(sSettings.musicVolume);
   if ( sSettings.muteMusic )
      musb->SetUnpressed();
   else
      musb->SetPressed();


   PG_CheckButton* sndb = new PG_CheckButton(this, PG_Rect( 30, 150, 200, 20 ), "Enable Sound", 2 );
   sndb->sigClick.connect(SigC::slot( *this, &SoundSettings::radioButtonEvent ));
   new PG_Label ( this, PG_Rect(30, 180, 150, 20), "Sound Volume" );
   PG_Slider* snd = new PG_Slider(this, PG_Rect(180, 180, 200, 20), PG_Slider::HORIZONTAL, 31);
   snd->SetRange(0,100);
   snd->SetPosition(sSettings.soundVolume);
   if ( sSettings.muteEffects )
      sndb->SetUnpressed();
   else
      sndb->SetPressed();


   PG_Button* b1 = new PG_Button(this, PG_Rect(30,r.h-40,(r.w-70)/2,30), "OK", 100);
   b1->sigClick.connect(SigC::slot( *this, &SoundSettings::closeWindow ));

   PG_Button* b2 = new PG_Button(this, PG_Rect(r.w/2+5,r.h-40,(r.w-70)/2,30), "Cancel", 101);
   b2->sigClick.connect(SigC::slot( *this, &SoundSettings::buttonEvent ));

   sigClose.connect( SigC::slot( *this, &SoundSettings::closeWindow ));
   
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

