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
      const int searchPathNum;
   public:
      CGameOptions(void);
      CGameOptions( const CGameOptions& cgo );

      void setDefaults( void );
      void copy ( const CGameOptions& cgo );

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
      int changed;
      Named* searchPath;
      int getSearchPathNum ( void );
};

extern CGameOptions gameoptions;

#endif //#ifndef GAMEOPTIONS_H
