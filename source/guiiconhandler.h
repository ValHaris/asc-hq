//     $Id: guiiconhandler.h,v 1.10 2009-07-12 13:53:47 mbickel Exp $
//
/*
    This file is part of Advanced Strategic Command; http://www.asc-hq.de
    Copyright (C) 1994-2010  Martin Bickel  and  Marc Schellenberger

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

#if defined(karteneditor) && !defined(pbpeditor)
#error the mapeditor is not supposed to use the gui icons  !
#endif


#ifndef guiiconhandlerH
#define guiiconhandlerH

#include "loki/Functor.h"
#include "loki/Typelist.h"
#include "graphics/surface.h"

#include "paradialog.h"
#include "typen.h"
#include "containerbase.h"
#include "dashboard.h"

#include "actions/command.h"

extern const int smallGuiIconSizeX;
extern const int smallGuiIconSizeY;


class GuiFunction {

     public:
       virtual bool available( const MapCoordinate& pos, ContainerBase* subject, int num ) = 0;
       virtual bool checkForKey( const SDL_KeyboardEvent* key, int modifier, int num ) { return false; };
       virtual void execute( const MapCoordinate& pos, ContainerBase* subject, int num ) = 0;
       virtual Surface& getImage( const MapCoordinate& pos, ContainerBase* subject, int num ) = 0;
       virtual ASCString getName( const MapCoordinate& pos, ContainerBase* subject, int num ) = 0;
       virtual ~GuiFunction() {};
};

/*
class GenericGuiFunction : public GuiFunction {
        ASCString name;
        Surface icon;
     public:
        typedef Loki::Functor<bool, LOKI_TYPELIST_1(const MapCoordinate&)> Availability;
        typedef Loki::Functor<void, LOKI_TYPELIST_1(const MapCoordinate&)> Execution;
     protected:
        Availability avail;
        Execution exec;
     public:   
     
        GenericGuiFunction( Surface icon_, Availability availability, Execution execution, const ASCString& iconName ) 
                         : name(iconName), icon(icon_), avail(availability), exec(execution) {};

        bool available( const MapCoordinate& pos, int num ) { return avail(pos); };
        void execute( const MapCoordinate& pos, int num )   { exec(pos); };
        Surface& getImage( const MapCoordinate& pos, int num ) { return icon; };
        ASCString getName( const MapCoordinate& pos, int num ) { return name; };
};
*/

class GuiButton : public PG_Button {
          GuiFunction* func;
          MapCoordinate pos;
          ContainerBase* subject;
          int id;
          friend class SmallGuiButton;
       protected:
          void eventMouseEnter();
          void eventMouseLeave();
          void callFunc( const MapCoordinate& pos, ContainerBase* subject, int num );
       public:
          GuiButton( PG_Widget *parent, const PG_Rect &r );
          void registerFunc( GuiFunction* f, const MapCoordinate& position, ContainerBase* subject, int id );
          bool checkForKey( const SDL_KeyboardEvent* key, int modifier );
          void unregisterFunc();
          bool exec();
          bool ready() { return func; };
          void showInfoText();
};

class NewGuiHost;


class SmallGuiButton : public PG_Button {
          GuiButton* referenceButton;
          SDL_Surface* smallIcon;
       protected:
          void eventMouseEnter();
          void eventMouseLeave();
       public:
          SmallGuiButton( PG_Widget *parent, const PG_Rect &r, GuiButton* guiButton, NewGuiHost* host );
          void updateIcon();
          void press();
          void showInfoText();
          ~SmallGuiButton();
};



class GuiIconHandler {

       typedef list<GuiFunction*> Functions;
       Functions functions;



       friend class NewGuiHost;
       void registerHost( NewGuiHost* guiIconHost ) { host = guiIconHost; };
     protected:
        NewGuiHost* host;

     public:
       GuiIconHandler() : host(NULL) {};

       /** registers a user function. Icons are displayed in the order that they were registered.
          By passing an object here, the GuiIconHandler will obtain ownership of the object and delete it on his destruction */
       void registerUserFunction( GuiFunction* function );

       virtual bool checkForKey( const SDL_KeyboardEvent* key, int modifier );

       //! checks evaluates the field on which the cursor resides
       // virtual void eval();
       
       virtual void eval( const MapCoordinate& pos, ContainerBase* subject );
       virtual ~GuiIconHandler();
};

class MapDisplayPG;
class MainScreenWidget;
class SmallButtonHolder;

class NewGuiHost : public DashboardPanel {
        GuiIconHandler* handler;
        static NewGuiHost* theGuiHost;
        list<GuiIconHandler*> iconHandlerStack;
        MapDisplayPG* mapDisplay;
        bool enterKeyPressed;
        int keyPressedButton;
        
        void lockOptionsChanged( int options );

        void mapDeleted( GameMap& map );
        
        bool eventKeyDownSignal(const PG_MessageObject* o, const SDL_KeyboardEvent* key);
        bool eventKeyUpSignal(const PG_MessageObject* o, const SDL_KeyboardEvent* key);


     protected:
        bool mapIconProcessing( const MapCoordinate& pos, const SPoint& mousePos, bool cursorChanged, int button, int prio );

        typedef vector<GuiButton*> Buttons;
        Buttons buttons;

        typedef vector<SmallGuiButton*> SmallButtons;
        SmallButtons smallButtons;

        SmallGuiButton* getSmallButton( int i );
        
        
        static SmallButtonHolder* smallButtonHolder;


        bool eventKeyDown(const SDL_KeyboardEvent* key);
        bool eventKeyUp(const SDL_KeyboardEvent* key);

        //! when operated by keyboard, set a new button as the pressed one
        bool setNewButtonPressed( int i );

        void evalCursor();
        
     public:
        NewGuiHost (MainScreenWidget *parent, MapDisplayPG* mapDisplay, const PG_Rect &r ) ;
        
        static void pushIconHandler( GuiIconHandler* iconHandler );
        static GuiIconHandler* getIconHandler(  );
        static void popIconHandler();
        
        void eval( const MapCoordinate& pos, ContainerBase* subject );

        bool showSmallIcons( PG_Widget* parent, const SPoint& pos, bool cursorChanged );
        bool clearSmallIcons();

        GuiButton* getButton( int i );

        //! disables all button from i onward
        void disableButtons( int i );

        bool ProcessEvent (const SDL_Event *event, bool bModal);

        static Command* pendingCommand;
        
        ~NewGuiHost();

};

extern void resetActiveGuiAction( GameMap* map );


#endif

