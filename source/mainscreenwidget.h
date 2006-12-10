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


#ifndef mainscreenwidgetH
 #define mainscreenwidgetH

#include "paradialog.h"
#include "messaginghub.h"
#include "overviewmappanel.h"

// class Menu;
class MapDisplayPG;
class OverviewMapPanel;

class MainScreenWidget : public PG_Widget {
    PG_Application& app;
    Surface backgroundImage;
    SDL_Rect blitRects[4];
    
    int lastMessageTime;
    int lastMouseScrollTime;

    StatusMessageWindowHolder createStatusWindow( const ASCString& msg );
    
    void buildBackgroundImage( bool messageLine );
        
    OverviewMapPanel* overviewMapPanel;

    friend class StandardActionLocker;
    
protected:
    int lockOptions;
    
public:
    MainScreenWidget( PG_Application& application );

    void displayMessage( const ASCString& message );
    
    void activateMapLayer( const ASCString& name, bool active );
    void toggleMapLayer( const ASCString& name );

    MapDisplayPG* getMapDisplay() { return mapDisplay; };

    OverviewMapPanel* getOverviewMapPanel()
    {
      return overviewMapPanel;
    };

    class StandardActionLocker {
         MainScreenWidget* widget;
         bool locked;
         void operator=( StandardActionLocker& locker ) {};
         int options;
       public:
          StandardActionLocker( MainScreenWidget* mainScreenWidget, int options );
          StandardActionLocker( const StandardActionLocker& locker );
          void lock();
          void unlock();
          ~StandardActionLocker();
    };
    
    struct LockOptions {
      enum Options{ Menu = 1, MapActions = 2, MapControl = 4 };
    }; 
    

    SigC::Signal1<void,int> lockOptionsChanged;
    
    
protected:
   virtual void lockStandardActions( int dir, int options = 0 ) {};
   
   MapDisplayPG* mapDisplay;
    // Menu* menu;
    PG_Label* messageLine;

    void spawnOverviewMapPanel ( const ASCString& panelName = "OverviewMap");
    
    void setup( bool messageLine );
    
    bool idleHandler( PG_MessageObject* msgObj );

    void mouseScrollChecker();

    virtual ASCString getBackgroundImageFilename() = 0;
    void eventBlit (SDL_Surface *surface, const PG_Rect &src, const PG_Rect &dst) ;
    ~MainScreenWidget() { };
};


extern MainScreenWidget* getMainScreenWidget();

#endif

