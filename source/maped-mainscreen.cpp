
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "global.h"

#include <stdio.h>
#include <cstring>
#include <stdlib.h>
#include <new>
#include <cstdlib>
#include <ctype.h>
#include <signal.h>
#include <algorithm>
#include <memory>
#include <SDL_image.h>
#include <SDL_mixer.h>

#include "paradialog.h"

#include "basegfx.h"
#include "misc.h"
#include "iconrepository.h"
#include "maped-mainscreen.h"
#include "graphics/blitter.h"
#include "graphics/drawing.h"
#include "edglobal.h"
#include "basicmessages.h"
#include "mapdisplay2.h"
#include "spfst.h"
#include "overviewmappanel.h"
#include "edselfnt.h"
#include "edmisc.h"

MainScreenWidget*  mainScreenWidget = NULL ;


class Menu : public PG_MenuBar {

    PG_PopupMenu* currentMenu;
    typedef list<PG_PopupMenu*> Categories;
    Categories categories;

   public:
      Menu ( PG_Widget *parent, const PG_Rect &rect=PG_Rect::null);
      ~Menu();
      
   protected:
      void setup();   
      bool execAction  (PG_PopupMenu::MenuItem* menuItem );

   private:
      void addbutton(const char* name, int id );
      void addfield ( const char* name );
     
};



Menu::~Menu()
{
/*
   for ( Categories::iterator i = categories.begin(); i != categories.end(); ++i )
      delete *i;
*/      

}

bool Menu::execAction  (PG_PopupMenu::MenuItem* menuItem )
{
   execaction_ev( tuseractions( menuItem->getId() ) );

   return true;
}


void Menu::addfield( const char* name )
{
   ASCString s = name;
   while ( s.find ( "~") != ASCString::npos )
      s.erase( s.find( "~"),1 );
      
   currentMenu = new PG_PopupMenu( NULL, -1, -1, "" );
   categories.push_back ( currentMenu );
   Add ( s, currentMenu );
   currentMenu->sigSelectMenuItem.connect( SigC::slot( *this, &Menu::execAction ));

}

void Menu::addbutton( const char* name, int id )
{
   ASCString s = name;
   while ( s.find ( "~") != ASCString::npos )
      s.erase( s.find( "~"),1 );
      
   currentMenu->addMenuItem( s, id );
}


void Menu::setup()
{
  addfield ( "~F~ile" );
   addbutton ( "~N~ew map\tctrl+N" , act_newmap    );
   addbutton ( "~L~oad map\tctrl+L", act_loadmap   );
   addbutton ( "~S~ave map\tS",      act_savemap   );
   addbutton ( "Save map ~a~s",     act_savemapas );
   addbutton ( "Edit Map ~A~rchival Information", act_editArchivalInformation );
   currentMenu->addSeparator();
   addbutton ( "Load Clipboard",     act_readClipBoard );
   addbutton ( "Save Clipboard",     act_saveClipboard );
   currentMenu->addSeparator();
   addbutton ( "~W~rite map to PCX-File\tctrl+G", act_maptopcx);
    addbutton ( "~I~mport BI map\tctrl-i", act_import_bi_map );
    addbutton ( "Insert ~B~I map", act_insert_bi_map );
   currentMenu->addSeparator();
    addbutton ( "set zoom level", act_setzoom );
   currentMenu->addSeparator();
   addbutton ( "E~x~it\tEsc", act_end);

  addfield ("~E~dit");
   addbutton ( "~C~opy \tctrl+C",          act_copyToClipboard );
   addbutton ( "Cu~t~",                   act_cutToClipboard );
   addbutton ( "~P~aste \tctrl+V",         act_pasteFromClipboard );
   currentMenu->addSeparator();
   addbutton ( "Resi~z~e map\tR",             act_resizemap );
   addbutton ( "set global uniform ~w~eather‹¨«trl-W", act_setactweatherglobal );
   addbutton ( "configure weather generator", act_setactnewweather );
   addbutton ( "~C~reate regional ressources", act_createresources );
   addbutton ( "~C~reate global ressources\tctrl+F", act_createresources2 );
   addbutton ( "~S~et turn number",        act_setTurnNumber );
   addbutton ( "~E~dit technologies",          act_editResearch );
   addbutton ( "edit ~R~search points",          act_editResearchPoints );
   addbutton ( "edit ~T~ech adapter",          act_editTechAdapter );
   addbutton ( "reset player data...",   act_resetPlayerData );

  addfield ("~S~elect");
   addbutton ( "Vehicle Type\tF2",  act_selunit );
   addbutton ( "Terrain Type\tF3",  act_selbodentyp );
   addbutton ( "Object Type \tF4",  act_selobject );
   addbutton ( "Building Type\tF5", act_selbuilding );
   
   
  addfield ("~T~ools");
   addbutton ( "~V~iew map\tV",            act_viewmap );
   // addbutton ( "~S~how palette",             act_showpalette );
   addbutton ( "~R~ebuild display\tctrl+R",   act_repaintdisplay );
   currentMenu->addSeparator();
   addbutton ( "~M~ap generator\tG",          act_mapgenerator );
   addbutton ( "Sm~o~oth coasts",          act_smoothcoasts );
   addbutton ( "~U~nitset transformation",    act_unitsettransformation );
   addbutton ( "map ~t~ransformation",        act_transformMap );
   addbutton ( "Com~p~are Resources ", act_displayResourceComparison );
   addbutton ( "Show Pipeline Net", act_showPipeNet );
   addbutton ( "Generate Tech Tree", act_generateTechTree );

   addfield ("~O~ptions");
    addbutton ( "~M~ap values\tctrl+M",          act_changemapvals );
    addbutton ( "~C~hange players\tO",           act_changeplayers);
    addbutton ( "~E~dit events\tE",              act_events );
    addbutton ( "~S~etup Player + Alliances\tctrl+A",     act_setupalliances );
//    addbutton ( "unit production ~L~imitation", act_specifyunitproduction );
   currentMenu->addSeparator();
    addbutton ( "~T~oggle ResourceView\tctrl+B", act_toggleresourcemode);
    addbutton ( "~B~I ResourceMode",            act_bi_resource );
    addbutton ( "~A~sc ResourceMode",           act_asc_resource );
    addbutton ( "edit map ~P~arameters",        act_setmapparameters );
    addbutton ( "setup item ~F~ilters\tctrl+h",  act_setunitfilter );
    addbutton ( "select ~G~raphic set",         act_selectgraphicset );

   addfield ("~H~elp");
    addbutton ( "~U~nit Information\tctrl+U",    act_unitinfo );
    addbutton ( "unit~S~et Information",        act_unitSetInformation );
    addbutton ( "~T~errain Information",        act_terraininfo );
   currentMenu->addSeparator();
    addbutton ( "~H~elp System\tF1",             act_help );
    addbutton ( "~A~bout",                      act_about );
}


Menu::Menu ( PG_Widget *parent, const PG_Rect &rect)
    : PG_MenuBar( parent, rect, "MenuBar"),
      currentMenu(NULL)
{
   setup();
   
}  






MainScreenWidget::MainScreenWidget( PG_Application& application )
              : PG_Widget(NULL, PG_Rect ( 0, 0, app.GetScreen()->w, app.GetScreen()->h ), false),
              app ( application ) , messageLine(NULL), lastMessageTime(0), 
              vehicleSelector( NULL ), buildingSelector( NULL ), objectSelector(NULL), terrainSelector(NULL),
              currentSelectionWidget(NULL)
{

   displayLogMessage ( 5, "MainScreenWidget: initializing panels:\n");

   displayLogMessage ( 7, "  Mapdisplay ");
   mapDisplay = new MapDisplayPG( this, PG_Rect(15,30,Width() - 200, Height() - 73));
   mapDisplay->SetID( ASC_PG_App::mapDisplayID );

   mapDisplay->mouseButtonOnField.connect( SigC::slot( mousePressedOnField ));
   mapDisplay->mouseDraggedToField.connect( SigC::slot( mouseDraggedToField ));
   
   displayLogMessage ( 7, "done\n  Menu ");
   menu = new Menu(this, PG_Rect(15,0,Width()-200,20));

   SetID( ASC_PG_App::mainScreenID );

   // displayLogMessage ( 7, "done\n  ButtonPanel ");
   // spawnPanel ( ButtonPanel );

   // displayLogMessage ( 7, "done\n  WindInfo ");
   // spawnPanel ( WindInfo );

   // displayLogMessage ( 7, "done\n  UnitInfo ");
   // spawnPanel ( UnitInfo );

   displayLogMessage ( 7, "done\n  OverviewMap ");
   spawnPanel ( OverviewMap );

   displayLogMessage ( 5, "done\nMainScreenWidget completed\n");


   repaintDisplay.connect ( SigC::bind( SigC::slot( *this, &MainScreenWidget::Update ), true ));
   
   buildBackgroundImage();

   PG_Application::GetApp()->sigAppIdle.connect( SigC::slot( *this, &MainScreenWidget::idleHandler ));
   
   PG_Application::GetApp()->sigKeyDown.connect( SigC::slot( *this, &MainScreenWidget::eventKeyDown ));
   
   
   PG_Button* button = new PG_Button( this, PG_Rect( Width() - 150, 200, 140, 20), "Select Vehicle" );
   button->sigClick.connect( SigC::slot( *this, &MainScreenWidget::selectVehicle ));
   
   PG_Button* button2 = new PG_Button( this, PG_Rect( Width() - 150, 230, 140, 20), "Select Building" );
   button2->sigClick.connect( SigC::slot( *this, &MainScreenWidget::selectBuilding ));

   PG_Button* button3 = new PG_Button( this, PG_Rect( Width() - 150, 260, 140, 20), "Select Object" );
   button3->sigClick.connect( SigC::slot( *this, &MainScreenWidget::selectObject ));
   
   PG_Button* button4 = new PG_Button( this, PG_Rect( Width() - 150, 290, 140, 20), "Select Terrain" );
   button4->sigClick.connect( SigC::slot( *this, &MainScreenWidget::selectTerrain ));
 
   currentSelectionWidget = new SelectionItemWidget( this, PG_Rect( Width() - BuildingItem::Width() - 10, 330, BuildingItem::Width(), BuildingItem::Height() + MapComponent::fontHeight ) );
   
   setNewSelection.connect( SigC::slot( *currentSelectionWidget, &SelectionItemWidget::set ));
}

bool MainScreenWidget::eventKeyDown(const SDL_KeyboardEvent* key)
{
   switch ( key->keysym.sym ) {
      case SDLK_SPACE: execaction( act_placething );
                       return true;
   }                 
   return false;
}


bool MainScreenWidget :: selectVehicle()
{
   if ( !vehicleSelector ) 
      vehicleSelector = new ItemSelector<Vehicletype>( this, PG_Rect( Width()-300, 100, 280, Height()-150), vehicleTypeRepository);
      
   vehicleSelector->Show();
   return true;
}

bool MainScreenWidget :: selectBuilding()
{
   if ( !buildingSelector ) 
      buildingSelector = new ItemSelector<BuildingType>( this, PG_Rect( Width()-300, 100, 280, Height()-150), buildingTypeRepository);
   
   buildingSelector->Show();
   return true;
}

bool MainScreenWidget :: selectObject()
{
   if ( !objectSelector ) 
      objectSelector = new ItemSelector<ObjectType>( this, PG_Rect( Width()-300, 100, 280, Height()-150), objectTypeRepository);
      
   objectSelector->Show();
   return true;
}

bool MainScreenWidget :: selectTerrain()
{
   if ( !terrainSelector ) 
      terrainSelector = new ItemSelector<TerrainType>( this, PG_Rect( Width()-300, 100, 280, Height()-150), terrainTypeRepository);
      
   terrainSelector->Show();
   return true;
}


bool MainScreenWidget :: idleHandler( )
{
   if ( ticker > lastMessageTime + 300 ) {
      displayMessage( "" );
      lastMessageTime = 0xfffffff;
   }   
   return true;
}


void MainScreenWidget::buildBackgroundImage()
{
   if ( !backgroundImage.valid() ) {
      backgroundImage = Surface::createSurface( Width(), Height(), 32 );
      
      Surface& source = IconRepository::getIcon("mapeditor-background.png");
      
      MegaBlitter< 4, gamemapPixelSize, ColorTransform_None,ColorMerger_PlainOverwrite,SourcePixelSelector_DirectZoom> blitter;
      blitter.setSize( source.w(), source.h(), Width(), Height(), false );
      
      blitter.blit( source, backgroundImage, SPoint(0,0) );
      
      assert( mapDisplay );
      PG_Rect r = *mapDisplay;
      
      const int borderWidth = 5;
      for ( int i = 0; i <= borderWidth; ++i ) 
         rectangle<4> ( backgroundImage, SPoint(r.x-i, r.y-i), r.w+2*i, r.h+2*i, ColorMerger_Brightness<4>( 0.6 ), ColorMerger_Brightness<4>( 1.5 ));

      int x1 = r.x + 1;
      int y1 = r.y + 1;
      int x2 = r.x + r.Width() - 1;
      int y2 = r.y + r.Height() -1;

      blitRects[0] = PG_Rect(  0, 0, Width(), y1 ); 
      blitRects[1] = PG_Rect(  0, y1, x1,     y2 ); 
      blitRects[2] = PG_Rect( x2, y1, Width() - x2, y2 ); 
      blitRects[3] = PG_Rect( 0, y2, Width(), Height() - y2 ); 
      
/*     
      int mx1 = x1 - borderWidth;
      int mx2 = x2 + borderWidth;
      int my1 = y2 + 10;
      
      int msglength = mx2 - mx1;

      messageLine = new PG_Label ( this, PG_Rect( mx1 + 20, my1 + 9, msglength - 30, msgend.h() - 18) );
      messageLine->SetFontSize(11);
      */
         
   }
}

void MainScreenWidget::displayMessage( const ASCString& message )
{
   if ( messageLine ) {
      messageLine->SetText( message );
      lastMessageTime = ticker;
   }   
}

void displaymessage2( const char* formatstring, ... )
{

   ASCString s;
   
    std::va_list arg_ptr;
    va_start ( arg_ptr, formatstring );

    s.vaformat( formatstring, arg_ptr );

    va_end ( arg_ptr );

    if ( mainScreenWidget )
       mainScreenWidget->displayMessage(s);
}



void MainScreenWidget::eventBlit (SDL_Surface *surface, const PG_Rect &src, const PG_Rect &dst) 
{
   SDL_Rect dstrect;
   Surface s = Surface::Wrap( PG_Application::GetScreen() );
   dstrect.x = blitRects[0].x;
   dstrect.y = blitRects[0].y;
   s.Blit( backgroundImage, blitRects[0], dstrect );
   
   dstrect.x = blitRects[1].x;
   dstrect.y = blitRects[1].y;
   s.Blit( backgroundImage, blitRects[1], dstrect );
   
   dstrect.x = blitRects[2].x;
   dstrect.y = blitRects[2].y;
   s.Blit( backgroundImage, blitRects[2], dstrect );
   
   dstrect.x = blitRects[3].x;
   dstrect.y = blitRects[3].y;
   s.Blit( backgroundImage, blitRects[3], dstrect );
}


void MainScreenWidget::spawnPanel ( Panels panel )
{
   if ( panel == OverviewMap ) {
      assert( mapDisplay);
      OverviewMapPanel* smp = new OverviewMapPanel( this, PG_Rect(Width()-170, 0, 170, 160), mapDisplay );
      smp->Show();
   }
}


