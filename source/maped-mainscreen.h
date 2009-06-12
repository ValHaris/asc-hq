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


#ifndef mapedmainscreenH
 #define mapedmainscreenH 

#include "paradialog.h"
#include "edselfnt.h"
#include "graphics/surface.h"
#include "widgets/dropdownselector.h"
#include "mainscreenwidget.h"

class Menu;
class NewGuiHost;
class MapDisplayPG;

class SelectionItemWidget : public PG_Widget {
      const Placeable* it;
      static const int labelHeight = 15;
   public:
      SelectionItemWidget( PG_Widget* parent, const PG_Rect& pos ) : PG_Widget( parent, pos, true ), it(NULL)
      {
      }
           
      void set( const Placeable* item) { it = item; Redraw(true); };
   protected:
      void eventDraw (SDL_Surface *surface, const PG_Rect &rect) 
      {
         Surface s = Surface::Wrap( surface );
         s.Fill( s.GetPixelFormat().MapRGB( DI_Color( 0x73b16a ) ));
         
         if ( it ) {
            const MapComponent* mc = dynamic_cast<const MapComponent*>(it);
            if ( mc ) {
               SPoint pos ( (Width() - mc->displayWidth()) / 2, (Height() - mc->displayHeight())/2 );
               mc->display( s, pos );
            }
        }    
      };

};


   class ContextAction {
      public:
         virtual bool available( const MapCoordinate& pos ) = 0;
         virtual ASCString getText( const MapCoordinate& pos ) = 0;
         virtual int getActionID() = 0;
         virtual ~ContextAction() {};
   };
      
class Maped_MainScreenWidget : public MainScreenWidget {
    PG_Window* vehicleSelector;
    PG_Window* buildingSelector;
    PG_Window* objectSelector;
    PG_Window* terrainSelector;
    PG_Window* mineSelector;
    DropDownSelector* weatherSelector;
    DropDownSelector* playerSelector;
    DropDownSelector* brushSelector;
    PG_Label* selectionName;
    PG_Label* selectionName2;
    PG_Label* coordinateDisplay;
    SelectionItemWidget* currentSelectionWidget;
    deallocating_vector<ContextAction*> contextActions;
    PG_PopupMenu* contextMenu;
public:
    Maped_MainScreenWidget( PG_Application& application );

    bool clearSelection();
    bool selectVehicle();
    bool selectBuilding();
    bool selectObject();
    bool selectObjectList();
    bool selectTerrain();
    bool selectTerrainList();
    bool selectMine();
    bool selectLuaBrush();
    void updateStatusBar();

    void addContextAction( ContextAction* contextAction );
   
protected:

    bool clickOnMap( const MapCoordinate& field, const SPoint& pos, bool changed, int button, int prio);
   
    NewGuiHost* guiHost;
    Menu* menu;
    
    void brushChanged( int i );
    void selectionChanged( const Placeable* item ); 
    bool eventKeyUp(const SDL_KeyboardEvent* key);
    bool eventKeyDown(const SDL_KeyboardEvent* key);
    void setupStatusBar();
    
    void playerChanged( int player );

    ASCString getBackgroundImageFilename() { return "mapeditor-background.png"; };
    
    bool eventMouseButtonDown (const SDL_MouseButtonEvent *button);

    ~Maped_MainScreenWidget() { };
   private:
      bool runContextAction  (PG_PopupMenu::MenuItem* menuItem );
};

//! displays a message in the status line of ASC
extern void displaymessage2( const char* formatstring, ... );

extern Maped_MainScreenWidget*  mainScreenWidget ;

#endif

