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
#include "Named.h"
#include "password.h"

class CGameOptions
{
   public:
		friend class CLoadableGameOptions;	
	   static const int searchPathNum;
		/** returns the only Instance */
		static	CGameOptions* Instance();

		CGameOptions(void);
		CGameOptions( const CGameOptions& cgo );
	  
		void setDefaults( void );
		void copy ( const CGameOptions& cgo );

		bool	isChanged();
		void	setChanged(bool flag	=	true);		
      
		const char*	getSearchPath(int i)	{	return(searchPath+i)->getName();};
	    int getSearchPathNum ( void );
		
		int version;
		
		//! the number of steps to perform a move: 0 -> 3 step; != 0 -> 2 steps
      int fastmove;

      //! 1/100 sec for a unit to move from one field to the next
      int movespeed;

      //! question "do you want to end your turn"
      int endturnquestion;

      //! either the small map or the wind information is displayed
      int smallmapactive;

      //! are units that cannot move but shoot displayed in gray ?
      int units_gray_after_move;

      //! the zoom of the map display in ASC
      int mapzoom;

      //! the zoom of the map display in the map editor
      int mapzoomeditor;

      //! not used at the moment
      int startupcount;

      //! are fields marked that a unit could move to if there was no other unit standing
      int dontMarkFieldsNotAccessible_movement;

      //! first delay of attack display
      int attackspeed1;

      //! speed of moving bars in attack display
      int attackspeed2;

      //! second delay in attack display
      int attackspeed3;

      //! muted soud can be reactivated during the game
      int sound_mute;

      //! if the sound is off, it can only be reactivated by restarting asc, but doesn't lock the sound device
      int sound_off;

      //! when an airplanes changes its height, should all accessible fields be found from which the height change can be started? May take some time on slower computers
      int heightChangeMovement;

      //! not relevant any more
      int runAI;
 
      //! force ASC to run in windowed mode (and not fullscreen, which is default)
      int forceWindowedMode;
    

      //! force the mapeditor to run in fullscreen mode (and not windowed, which is default)
      int mapeditForceFullscreenMode;

      //! the horizontal resolution ASC uses
      int xresolution;

      //! the vertical resolution ASC uses
      int yresolution;

      //! the horizontal resolution the mapeditor uses
      int mapeditor_xresolution;

      //! the vertical resolution the mapeditor uses
      int mapeditor_yresolution;

      //! temporary variable
      int specialForestChaining;


      //! the mouse configuration. Mouse button are: bit 0 = left button ; bit 1 = right button ; bit 2 = center button 
      struct Mouse
      {
         int scrollbutton;
         //! the button to select a field without opening the menu 
         int fieldmarkbutton;
         //! the button to select a field and open the menu there
         int smallguibutton;

         //! the button to use the big icons on the right side 
         int largeguibutton;

         /** specifies the occasions when the small icons are positioned directly under the mouse pointer (instead of some pixel upwards)
             0 = never
             1 = always
             2 = only if there is a building or a unit on the field or if the field is a destination for the current unit action (default)
         */ 
         int smalliconundermouse;  

         //! the button to center the map around the selected field 
         int centerbutton;

         //! the button to display the vehicle information          
         int unitweaponinfo;

         //! not used
         int dragndropmovement;  
      } mouse;

      class Container
      {
         public:
            int autoproduceammunition;
            int filleverything;
            int emptyeverything;
      } container;

      int onlinehelptime;
      int smallguiiconopenaftermove;

      int replayspeed;
      int showUnitOwner;

      struct Bi3
      {
         Named dir;
         struct Interpolate
         {
            Interpolate(){};
            int terrain;
            int units;
            int objects;
            int buildings;
         }
         interpolate;
      } bi3;

      Named defaultPassword;
      Named defaultSuperVisorPassword;
      Password getDefaultPassword ( );
      Password getDefaultSupervisorPassword ( );

	private:     
	  bool	_changed;
      Named* searchPath;
};

inline
bool	CGameOptions::isChanged()
{
	return _changed;
}

inline
void	CGameOptions::setChanged(bool flag	)
{
	_changed	=	flag;
}		
      

#endif //#ifndef GAMEOPTIONS_H
