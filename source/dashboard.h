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
#include "mapdisplay2.h"

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



#endif
