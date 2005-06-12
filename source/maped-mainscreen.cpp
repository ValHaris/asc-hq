
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
   addbutton ( "Vehicle\tF2",  act_selunit );
   addbutton ( "Terrain from map\tF3",  act_selbodentyp );
   addbutton ( "Terrain from list\tctrl-F3",  act_selbodentypAll );
   addbutton ( "Object from map \tF4",  act_selbodentyp );
   addbutton ( "Object from list \tctrl-F4",  act_selobject );
   addbutton ( "Building Type\tF5", act_selbuilding );
   addbutton ( "Mine\tF6", act_selmine );
   
   
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
              weatherSelector( NULL ), selectionName(NULL), coordinateDisplay(NULL), currentSelectionWidget(NULL)
{

   displayLogMessage ( 5, "MainScreenWidget: initializing panels:\n");

   displayLogMessage ( 7, "  Mapdisplay ");
   mapDisplay = new MapDisplayPG( this, PG_Rect(15,30,Width() - 200, Height() - 73));
   mapDisplay->SetID( ASC_PG_App::mapDisplayID );

   mapDisplay->mouseButtonOnField.connect( SigC::slot( mousePressedOnField ));
   mapDisplay->mouseDraggedToField.connect( SigC::slot( mouseDraggedToField ));

   
   setupStatusBar();
  
      
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
   
   int xpos = Width() - 150;
   int w = 140;
   int ypos = 180;
   
   PG_Button* button = new PG_Button( this, PG_Rect( xpos, ypos, w, 20), "Select Vehicle" );
   button->sigClick.connect( SigC::slot( *this, &MainScreenWidget::selectVehicle ));
   ypos += 25;
   
   PG_Button* button2 = new PG_Button( this, PG_Rect( xpos, ypos, w, 20), "Select Building" );
   button2->sigClick.connect( SigC::slot( *this, &MainScreenWidget::selectBuilding ));
   ypos += 25;

   PG_Button* button3 = new PG_Button( this, PG_Rect( xpos, ypos, w, 20), "Select Object" );
   button3->sigClick.connect( SigC::slot( *this, &MainScreenWidget::selectObject ));
   ypos += 25;
   
   PG_Button* button4 = new PG_Button( this, PG_Rect( xpos, ypos, w, 20), "Select Terrain" );
   button4->sigClick.connect( SigC::slot( *this, &MainScreenWidget::selectTerrain ));
   ypos += 25;
 
   
   new PG_Label( this, PG_Rect( xpos, ypos, w/2 - 5, 20), "Brush:");
   brushSelector = new DropDownSelector( this, PG_Rect( xpos+w/2+5, ypos, w/2-5, 20));
   brushSelector->AddItem("1");
   brushSelector->AddItem("3");
   brushSelector->AddItem("5");
   brushSelector->selectionSignal.connect( SigC::slot( *this, &MainScreenWidget::brushChanged ));
   ypos += 25;
   
  
   weatherSelector = new DropDownSelector( this, PG_Rect( xpos, ypos, w, 20));
   for ( int i = 0; i < cwettertypennum; ++i )
      weatherSelector->AddItem( cwettertypen[i] );
    ypos += 25;  
            
   weatherSelector->selectionSignal.connect( SigC::slot( selection, &SelectionHolder::setWeather ) );
//   weatherSelector->selectionSignal.connect( SigC::slot( *currentSelectionWidget, &SelectionItemWidget::Update ));
   

   playerSelector = new DropDownSelector( this, PG_Rect( xpos, ypos, w, 20));
   for ( int i = 0; i < 9; ++i )
      playerSelector->AddItem( "Player " + ASCString::toString(i) );
   playerSelector->selectionSignal.connect( SigC::slot( selection, &SelectionHolder::setPlayer ) );
   ypos += 25;
   
   currentSelectionWidget = new SelectionItemWidget( this, PG_Rect( Width() - BuildingItem::Width() - 10, ypos, BuildingItem::Width(), BuildingItem::Height() ) );
   ypos += BuildingItem::Height() + 5;

   selectionName = new PG_Label( this, PG_Rect( xpos, currentSelectionWidget->my_ypos + currentSelectionWidget->Height() + 10, currentSelectionWidget->Width(), 20 ));
   ypos += 25;
   
   selection.selectionChanged.connect( SigC::slot( *currentSelectionWidget, &SelectionItemWidget::set ));
   selection.selectionChanged.connect( SigC::slot( *this, &MainScreenWidget::selectionChanged ));

}

void MainScreenWidget::setupStatusBar()
{
   int x = mapDisplay->my_xpos;
   int y =  mapDisplay->my_ypos + mapDisplay->Height() + 25;
   int height = 20;

   messageLine = new PG_Label ( this, PG_Rect( x, y, 200, height ) );
   messageLine->SetFontSize(11);
   x += 210;
   

   coordinateDisplay = new PG_Label ( this, PG_Rect( x, y, 80, height ) );
   coordinateDisplay->SetFontSize(11);
   
   updateFieldInfo.connect( SigC::slot( *this, &MainScreenWidget::updateStatusBar ));
}

void MainScreenWidget::updateStatusBar()
{
   MapCoordinate pos = actmap->getCursor();
   coordinateDisplay->SetText( ASCString::toString( pos.x) + " / " + ASCString::toString( pos.y ));
}

void MainScreenWidget::selectionChanged( const MapComponent* item )
{
   selectionName->SetText( item->getName() + "(ID: " + ASCString::toString( item->getID()   ) + ")");
}


void MainScreenWidget::brushChanged( int i )
{
   selection.brushSize = i+1;
   if ( selection.brushSize < 1 )
      selection.brushSize = 1;
}


bool MainScreenWidget::eventKeyDown(const SDL_KeyboardEvent* key)
{
   int mod = SDL_GetModState() & ~(KMOD_NUM | KMOD_CAPS | KMOD_MODE);

   if ( !mod  ) {
      switch ( key->keysym.sym ) {
         case SDLK_RETURN:
         case SDLK_SPACE: execaction_ev( act_placething );
                        return true;
                        
         case SDLK_F1:  execaction_ev(act_help);
                        return true;
         case SDLK_F2 : execaction_ev(act_selunit);
                        return true;
         case SDLK_F3:  execaction_ev(act_selbodentyp);
                        return true;
         case SDLK_F4 : execaction_ev(act_selobject);
                        return true;
         case SDLK_F5 : execaction_ev(act_selbuilding);
                        return true;
         case SDLK_F6 : execaction_ev(act_selmine);
                        return true;
         case SDLK_a:   execaction_ev(act_movebuilding);
                        return true;
         case SDLK_b:   execaction_ev(act_changeresources);
                        return true;
         case SDLK_c:   execaction_ev(act_changecargo);
                        return true;
         case SDLK_d : execaction_ev(act_changeterraindir);
                        return true;
         case SDLK_e:  execaction_ev(act_events);
                        return true;
         case SDLK_f:  execaction_ev(act_fillmode);
                        return true;
         case SDLK_g: execaction_ev(act_mapgenerator);
                        return true;
         case SDLK_h: execaction_ev(act_setactivefieldvals);
                        return true;
         case SDLK_DELETE: execaction_ev(act_deletething);
                        return true;
         case SDLK_l : execaction_ev(act_showpalette);
                        return true;
         case SDLK_m : execaction_ev(act_changeminestrength);
                        return true;
         case SDLK_o: execaction_ev(act_changeplayers);
                        return true;

         case SDLK_p: execaction_ev(act_changeunitvals);
                        return true;

         case SDLK_r: execaction_ev(act_resizemap);
                        return true;

         case SDLK_s : execaction_ev(act_savemap);
                        return true;

         case SDLK_v:   execaction_ev(act_viewmap);
                        return true;

         case SDLK_x:   execaction_ev(act_mirrorcursorx);
                        return true;

         case SDLK_y:   execaction_ev(act_mirrorcursory);
                        return true;

         case SDLK_z:   execaction_ev(act_setzoom );
                        return true;

         case SDLK_8 : execaction_ev(act_placemine);
                        return true;

         case SDLK_TAB: execaction_ev(act_switchmaps );
                        return true;

         case SDLK_ESCAPE :  if ( polyfieldmode )
                                execaction_ev(act_endpolyfieldmode);
                             else
                                execaction_ev(act_end);
                        return true;
       }
   }
      
   if ( mod & KMOD_CTRL ) {
      switch ( key->keysym.sym ) {
         case SDLK_F3 : execaction_ev(act_selbodentypAll);
                        return true;
   
         case SDLK_F4 : execaction_ev(act_selobjectAll);
                        return true;

         case SDLK_a:  execaction_ev(act_setupalliances);
                        return true;
   
         case SDLK_b:  execaction_ev(act_toggleresourcemode);
                        return true;
   
         case SDLK_c:  execaction_ev(act_copyToClipboard);
                        return true;
   
         case SDLK_f: execaction_ev(act_createresources);
                        return true;
   
         case SDLK_g: execaction_ev(act_maptopcx);
                        return true;
   
         case SDLK_h: execaction_ev(act_setunitfilter);
                        return true;
   
         case SDLK_i: execaction_ev (act_import_bi_map );
                        return true;
   
         case SDLK_l: execaction_ev(act_loadmap);
                        return true;
   
         case SDLK_m: execaction_ev(act_changemapvals);
                        return true;
   
         case SDLK_n: execaction_ev(act_newmap);
                        return true;
   
         case SDLK_o: execaction_ev(act_polymode);
                        return true;
   
         case SDLK_p: execaction_ev(act_changeproduction);
                        return true;
   
         case SDLK_r: execaction_ev(act_repaintdisplay);
                        return true;
   
         case SDLK_u: execaction_ev(act_unitinfo);
                        return true;
   
         case SDLK_v: execaction_ev(act_pasteFromClipboard);
                        return true;
   
         case SDLK_w: execaction_ev(act_setactweatherglobal);
                        return true;

         case SDLK_x: execaction_ev(act_end);
                        return true;
   
       } 
   }                 
   return false;
}


bool MainScreenWidget :: selectVehicle()
{
   if ( !vehicleSelector ) 
      vehicleSelector = new ItemSelector<Vehicletype>( this, PG_Rect( Width()-300, 100, 280, Height()-150), vehicleTypeRepository);
      
   vehicleSelector->Show();
   vehicleSelector->RunModal();
   return true;
}

bool MainScreenWidget :: selectBuilding()
{
   if ( !buildingSelector ) 
      buildingSelector = new ItemSelector<BuildingType>( this, PG_Rect( Width()-300, 100, 280, Height()-150), buildingTypeRepository);
   
   buildingSelector->Show();
   buildingSelector->RunModal();
   return true;
}

bool MainScreenWidget :: selectObject()
{
   if ( !objectSelector ) 
      objectSelector = new ItemSelector<ObjectType>( this, PG_Rect( Width()-300, 100, 280, Height()-150), objectTypeRepository);
      
   objectSelector->Show();
   objectSelector->RunModal();
   return true;
}

bool MainScreenWidget :: selectTerrain()
{
   if ( !terrainSelector ) 
      terrainSelector = new ItemSelector<TerrainType>( this, PG_Rect( Width()-300, 100, 280, Height()-150), terrainTypeRepository);
      
   terrainSelector->Show();
   terrainSelector->RunModal();
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
      mapChanged.connect( SigC::slot( OverviewMapHolder::clearmap ));
   }
}


