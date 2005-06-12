/*! \file asc-mainscreen.h
    \brief Everything that happens on the main screen of ASC
*/

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


#ifndef ascmainscreenH
 #define ascmainscreenH 

#include "paradialog.h"

class Menu;
class NewGuiHost;
class MapDisplayPG;

class MainScreenWidget : public PG_Widget {
    PG_Application& app;
    Surface backgroundImage;
    SDL_Rect blitRects[4];
    PG_Label* messageLine;
    int lastMessageTime;
public:
    MainScreenWidget( PG_Application& application );
    enum Panels { ButtonPanel, WindInfo, UnitInfo, OverviewMap };
    void spawnPanel ( Panels panel );

    void displayMessage( const ASCString& message );
    
protected:
    MapDisplayPG* mapDisplay;
    NewGuiHost* guiHost;
    Menu* menu;
    
    bool idleHandler( );

    void buildBackgroundImage();
    void eventBlit (SDL_Surface *surface, const PG_Rect &src, const PG_Rect &dst) ;
//    void eventDraw (SDL_Surface* surface, const PG_Rect& rect);
//    void Blit ( bool recursive = true, bool restore = true );
    ~MainScreenWidget() { };
};

//! displays a message in the status line of ASC
extern void displaymessage2( const char* formatstring, ... );

extern MainScreenWidget*  mainScreenWidget ;

#endif

