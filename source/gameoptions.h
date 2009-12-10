/***************************************************************************
                          gameoptions.h  -  description
                             -------------------
    begin                : Thu Jun 29 2000
    copyright            : (C) 2000 by frank landgraf
    email                : 
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef GAMEOPTIONS_H
#define GAMEOPTIONS_H

#include <string>
#include "password.h"

#include "textfileparser.h"
#include "textfile_evaluation.h"


class CGameOptions
{
   public:
      /** returns the only Instance */
      static CGameOptions* Instance();

      CGameOptions();
      void setDefaults();
      void setDefaultDirectories();
/*      CGameOptions( const CGameOptions& cgo );

      void copy ( const CGameOptions& cgo );
      */

      void runTextIO ( PropertyContainer& pc );
      
      
      bool isChanged();
      void setChanged(bool flag = true);
      
      void load( const ASCString& filename );
      void save( const ASCString& filename );
      

      int getSearchPathNum();

      void setSearchPath ( int i, const ASCString& path );
      void addSearchPath ( const ASCString& path );
      ASCString getSearchPath( int i);

      int version;

      //! the number of steps to perform a move: 0 -> 3 step; != 0 -> 2 steps
      bool fastmove;

      //! 1/100 sec for a unit to move from one field to the next
      int movespeed;
      
      //! how much faster are unit going to move in replays (percent)
      int replayMoveSpeedFactor;

      //! question "do you want to end your turn"
      bool endturnquestion;

      //! are units that cannot move but shoot displayed in gray ?
      bool  units_gray_after_move;

      //! the zoom of the map display in ASC
      int mapzoom;

      //! the zoom of the map display in the map editor
      int mapzoomeditor;

      //! are fields marked that a unit could move to if there was no other unit standing
      // int dontMarkFieldsNotAccessible_movement;

      //! first delay of attack display
      int attackspeed1;

      //! speed of moving bars in attack display
      int attackspeed2;

      //! second delay in attack display
      int attackspeed3;

      //! force ASC to run in windowed mode (and not fullscreen, which is default)
      bool forceWindowedMode;

      //! use screen buffer on the graphics card instead of main memory
      bool hardwareSurface;

      //! hides the mouse cursor when screen updates are performed
      bool hideMouseOnScreenUpdates;

      //! enables the option to review your own replay to find replay bugs
      bool debugReplay;

      //! force the mapeditor to run in fullscreen mode (and not windowed, which is default)
      bool mapeditWindowedMode;

      //! the horizontal resolution ASC uses
      int xresolution;

      //! the vertical resolution ASC uses
      int yresolution;

      //! the horizontal resolution the mapeditor uses
      int mapeditor_xresolution;

      //! the vertical resolution the mapeditor uses
      int mapeditor_yresolution;

      bool maped_modalSelectionWindow;
      
      //! are units automatically trained in training centers
      bool automaticTraining;

      //! the mouse configuration. Mouse button are: bit 0 = left button ; bit 1 = right button ; bit 2 = center button
      struct Mouse
      {
         // int scrollbutton;
         
         //! the button to select a field without opening the menu
         int fieldmarkbutton;
         
         //! the button to select a field and open the menu there
         // int smallguibutton;

         //! the button to use the big icons on the right side
         // int largeguibutton;

         /** specifies the occasions when the small icons are positioned directly under the mouse pointer (instead of some pixel upwards)
             0 = never
             1 = always
             2 = only if there is a building or a unit on the field or if the field is a destination for the current unit action (default)
         */
         // int smalliconundermouse;

         //! the button to center the map around the selected field
         int centerbutton;
         
         //! used in the container dialog
         int dragndropbutton;

         //! the button to display the vehicle information
         // int unitweaponinfo;

         //! if != 0, a single click will mark a field AND open the small gui menu
         // int singleClickAction;

         int zoomoutbutton;
         int zoominbutton;

         //! marks a field and opens the small gui buttons with a single click (default: first click mark, second click: execute)
         bool singleClickAction;

         static ASCString getButtonName( int button );
      }
      mouse;


      struct SoundSettings
      {
         //! muted soud can be reactivated during the game
         bool muteEffects;

         //! muted soud can be reactivated during the game
         bool muteMusic;

         //! if the sound is off, it can only be reactivated by restarting asc, but doesn't lock the sound device
         bool off;

         int soundVolume;

         int musicVolume;
      }
      sound;

      struct {
         bool fillResources;
         bool fillAmmo;
      } unitProduction;

      //! produce required ammo upon refuelling
      bool autoproduceammunition;

      // int onlinehelptime;
      // int smallguiiconopenaftermove;

      int replayspeed;
      // int showUnitOwner;

      int panelColumns;

      bool cacheASCGUI;

      bool replayMovieMode;
      bool recordCampaignMaps;

      vector<ASCString> visibleMapLayer;

      //! the number of turns after which a unit will crash until the player is warned
      int aircraftCrashWarningTime;
      
      struct PanelData {
         int x;
         int y;
         bool visible;
      };

      typedef map<ASCString, PanelData> PanelDataContainer;
      PanelDataContainer panelData;

      bool getPanelData( const ASCString& name, PanelData& data );
      void updatePanelData( const ASCString& name, PanelData data );
      
      ASCString BI3directory;

      struct {
         ASCString hostname;
         int port;
         ASCString username;
      } pbemServer;

      ASCString mailProgram;
      
      ASCString graphicsDriver;

      ASCString startupMap;
      ASCString defaultPassword;
      ASCString defaultSuperVisorPassword;
      Password getDefaultPassword ( );
      Password getDefaultSupervisorPassword ( );

      struct {
         int framerate;
         int quality;
      } video;

      static const int maxSearchPathNum;
      int searchPathNum;
      ASCString searchPath[30];

      typedef map<ASCString,int> DialogAnswers;
      DialogAnswers dialogAnswers;

      ASCString languageOverride;
      
      //! will save all Immediate Game Event messages into a seperate file for doing translations
      bool saveEventMessagesExternal;
      
   private:
      bool _changed;
};

inline
bool CGameOptions::isChanged()
{
   return _changed;
}

inline
void CGameOptions::setChanged(bool flag )
{
   _changed = flag;
}


#endif //#ifndef GAMEOPTIONS_H
