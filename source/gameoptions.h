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

#include "Named.h"

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
      int fastmove;
      int visibility_calc_algo;      // 0 sauber, 1 schnell;
      int movespeed;
      int endturnquestion;
      int smallmapactive;
      int units_gray_after_move;
      int mapzoom;
      int mapzoomeditor;
      int startupcount;
      int dontMarkFieldsNotAccessible_movement;
      int attackspeed1;
      int attackspeed2;
      int attackspeed3;
      int disablesound;
      int runAI;

      struct Mouse
      {
         Mouse(){};
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
            Container(){};
            int autoproduceammunition;
            int filleverything;
            int emptyeverything;
      } container;

      int onlinehelptime;
      int smallguiiconopenaftermove;
      int defaultpassword;
      int replayspeed;
      int showUnitOwner;

      struct Bi3
      {
         Bi3(){};
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
