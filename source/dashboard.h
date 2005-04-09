/***************************************************************************
                          dashboard.h  -  description
                             -------------------
    begin                : Sat Jan 27 2001
    copyright            : (C) 2001 by Martin Bickel
    email                : bickel@asc-hq.org
 ***************************************************************************/

/*! \file dashboard.h
    \brief The box displaying unit information
*/


/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/



#ifndef dashboardH
 #define dashboardH

#include "paradialog.h"

class Vehicletype;
class Vehicle;
class SingleWeapon;


class DashboardPanel : public Panel {
    protected:
      DashboardPanel ( PG_Widget *parent, const PG_Rect &r, const ASCString& panelName_, bool loadTheme );

      void painter ( const PG_Rect &src, const ASCString& name, const PG_Rect &dst);
      void registerSpecialDisplay( const ASCString& name );

    public:
      void eval();

};

class WindInfoPanel : public DashboardPanel {
        Surface windArrow;
        int dir;
     protected:
        void painter ( const PG_Rect &src, const ASCString& name, const PG_Rect &dst);
     public:
        WindInfoPanel (PG_Widget *parent, const PG_Rect &r ) ;
};

class UnitInfoPanel : public DashboardPanel {
     protected:
        bool onClick ( PG_MessageObject* obj, const SDL_MouseButtonEvent* event );
     public:
        UnitInfoPanel (PG_Widget *parent, const PG_Rect &r ) ;
};

class WeaponInfoPanel : public Panel {
        int weaponCount;
        static ASCString name;
     protected:
        bool onClick ( PG_MessageObject* obj, const SDL_MouseButtonEvent* event );
        void painter ( const PG_Rect &src, const ASCString& name, const PG_Rect &dst);

     public:
        WeaponInfoPanel (PG_Widget *parent, const Vehicle* veh, const Vehicletype* vt ) ;
        void showWeapon( const SingleWeapon* weap = NULL );

        static const ASCString& WIP_Name();
        // void eval();
};


#if 0


  //! The unit, weather and map information displayed on the right side of the screen
  class tdashboard {
                public:
                       pfont        font; 
                       int         x, y; 
                       Vehicle*     vehicle;
                       pvehicletype vehicletype;
                       const Building*    building;
                       pfield       objfield;

                       void         allocmem ( void );
                    protected:
                       int          largeWeaponsDisplayPos[16];
                       void*        fuelbkgr;
                       int          fuelbkgrread;
                       void*        imagebkgr;
                       int          imageshown;
                       int          movedisp;   // 0: Movement f?r Runde    1: movement mit Tank
                       int          windheightshown;
                       void*        windheightbackground;
                       void         putheight(integer      i,   integer      sel);
                       void         paintheight(void);
                       void         painttank(void);
                       void         paintweapon( int h, int num, int strength, const SingleWeapon  *weap );
                       void         paintweapons(void);
                       void         paintweaponammount( int h, int num, int max, bool dash = false );
                       void         paintdamage(void);
                       void         paintexperience(void);
                       void         paintmovement(void);
                       void         paintarmor(void);
                       void         paintwind( int repaint = 0 );
                       void         paintclasses ( void );
                       void         paintname ( void );
                       void         paintimage( void );
                       void         paintplayer( void );
                       void         paintalliances ( void ); 
                       void         paintsmallmap ( int repaint = 0  );
                       void         paintlargeweaponinfo ( void );
                       void         paintlargeweapon ( int pos, const char* name, int ammoact, int ammomax, int shoot, int refuel, int strengthmax, int strengthmin, int distmax, int distmin, int from, int to );
                       void         paintlargeweaponefficiency ( int pos, int* e, int alreadypainted, const int* hit );
                       void         paintzoom( void );
                     public:
                       struct {
                          void*        pic;
                          int x1, y1, x2, y2;
                          int picwidth;
                       } zoom;

                       void         checkformouse ( int func = 0 );
                       void paint ( const pfield fld, int playerview );
                       tdashboard ( void );
                       void paintvehicleinfo( Vehicle*     vehicle,
                                              const Building*    building,
                                              const pfield       _objfield,
                                              pvehicletype vehicletype );

                       void         paintlweaponinfo ( void );

                       int backgrndcol ;
                       int vgcol       ;
                       int ymx         ;
                       int ymn         ;
                       int ydl         ;
                       int munitnumberx ;

                       struct {
                           int mindist, maxdist, minstrength, maxstrength;
                           int displayed;
                       } weaps[8];

                       int repainthard;
                    protected:
                         char* str_2 ( int num );
                         int materialdisplayed;


                    }; 
  extern tdashboard dashboard; 

  #endif
#endif
