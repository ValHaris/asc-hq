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

      int runAI;
      int forceWindowedMode;
      int mapeditForceFullscreenMode;
      int xresolution;
      int yresolution;
      int mapeditor_xresolution;
      int mapeditor_yresolution;
      int specialForestChaining;


      struct Mouse
      {
         int scrollbutton;
         int fieldmarkbutton;
         int smallguibutton;
         int largeguibutton;
         int smalliconundermouse;  // 0: nie;  1: immer; 2: nur wenn vehicle, geb„ude, oder temp unter MAUS
         int centerbutton;         
         int unitweaponinfo;
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
