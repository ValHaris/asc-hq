//     $Id: guiiconhandler.h,v 1.1.2.3 2004-12-12 19:58:45 mbickel Exp $
//
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

#if defined(karteneditor) && !defined(pbpeditor)
#error the mapeditor is not supposed the gui icons  !
#endif


#ifndef guiiconhandlerH
#define guiiconhandlerH

#include "libs/loki/Functor.h"
#include "graphics/surface.h"

#include "paradialog.h"

#include "typen.h"



class GuiFunction {

     public:
       virtual bool available( const MapCoordinate& pos ) = 0;
       virtual void execute( const MapCoordinate& pos ) = 0;
       virtual Surface& getImage( const MapCoordinate& pos ) = 0;
       virtual ASCString getName( const MapCoordinate& pos ) = 0;
       virtual ~GuiFunction() {};
};

class GenericGuiFunction : public GuiFunction {
        ASCString name;
        Surface icon;
     public:
        typedef Loki::Functor<bool, TYPELIST_1(const MapCoordinate&)> Availability;
        typedef Loki::Functor<void, TYPELIST_1(const MapCoordinate&)> Execution;
     protected:
        Availability avail;
        Execution exec;   
     public:   
     
        GenericGuiFunction( Surface icon_, Availability availability, Execution execution, const ASCString& iconName ) 
                         : name(iconName), icon(icon_), avail(availability), exec(execution) {};

        bool available( const MapCoordinate& pos ) { return avail(pos); };
        void execute( const MapCoordinate& pos )   { exec(pos); };
        Surface& getImage( const MapCoordinate& pos ) { return icon; };
        ASCString getName( const MapCoordinate& pos ) { return name; };
};


class GuiButton : public PG_Button {
          GuiFunction* func;
          MapCoordinate pos;
       public:
          GuiButton( PG_Widget *parent, const PG_Rect &r );
          void registerFunc( GuiFunction* f, const MapCoordinate& position );
          void unregisterFunc();
          bool exec();
};

class NewGuiHost;

class GuiIconHandler {

     typedef list<GuiFunction*> Functions;
     Functions functions;    
     
     NewGuiHost* host;

     
     public:
       GuiIconHandler() : host(NULL) {};
     
       /** registers a user function. Icons are displayed in the order that they were registered. 
          By passing an object here, the GuiIconHandler wil obtain ownership of the object and delete it on his destruction */
       void registerUserFunction( GuiFunction* function );
       void registerHost( NewGuiHost* guiIconHost ) { host = guiIconHost; };

       void eval();
       ~GuiIconHandler();
};


class NewGuiHost : public PG_Window {
        GuiIconHandler* handler;
     public:
        NewGuiHost (PG_Widget *parent, const PG_Rect &r ) ;
        void pushIconHandler( GuiIconHandler* iconHandler );
        void eval();

        typedef vector<GuiButton*> Buttons;
        Buttons buttons;
     
        GuiButton* getButton( int i );
        
        //! disables all button from i onward
        void disableButtons( int i );
        
        ~NewGuiHost();
            
};



#endif

