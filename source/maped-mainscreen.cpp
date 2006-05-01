
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
#include <algorithm>
#include <memory>
#include <SDL_image.h>
#include <SDL_mixer.h>

#include "paradialog.h"
#include "pgtooltiphelp.h"
#include "pgpopupmenu.h"

#include "basegfx.h"
#include "misc.h"
#include "iconrepository.h"
#include "maped-mainscreen.h"
#include "graphics/blitter.h"
#include "graphics/drawing.h"
#include "edglobal.h"
#include "mapdisplay.h"
#include "spfst.h"
#include "overviewmappanel.h"
#include "edselfnt.h"
#include "edmisc.h"
#include "gameoptions.h"

Maped_MainScreenWidget*  mainScreenWidget = NULL ;

MainScreenWidget* getMainScreenWidget()
{
   return mainScreenWidget;
}


class Menu : public PG_MenuBar {

    PG_PopupMenu* currentMenu;
    typedef list<PG_PopupMenu*> Categories;
    Categories categories;

   public:
      Menu ( PG_Widget *parent, const PG_Rect &rect=PG_Rect::null);
      
   protected:
      void setup();   
      bool execAction  (PG_PopupMenu::MenuItem* menuItem );

   private:
      void addbutton(const char* name, int id );
      void addfield ( const char* name );
     
};



bool Menu::execAction  (PG_PopupMenu::MenuItem* menuItem )
{
   execaction_ev( tuseractions( menuItem->getId() ) );

   return true;
}


void Menu::addfield( const char* name )
{
   currentMenu = new PG_PopupMenu( NULL, -1, -1, "" );
   categories.push_back ( currentMenu );
   Add ( name, currentMenu );
   currentMenu->sigSelectMenuItem.connect( SigC::slot( *this, &Menu::execAction ));

}

void Menu::addbutton( const char* name, int id )
{
   currentMenu->addMenuItem( name, id );
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
   addbutton ( "Increase Map Zoom\tKP+", act_increase_zoom );
   addbutton ( "Decrease Map Zoom\tKP-", act_decrease_zoom );
   currentMenu->addSeparator();
   addbutton( "Edit Preferences", act_editpreferences );
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
   currentMenu->addSeparator();
   addbutton ( "~S~etup Players",     act_setupplayers );
   addbutton ( "Setup ~A~lliances",     act_setupalliances );
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

namespace ContextMenu {

   class AutoTextContextAction : public ContextAction {
      public:
         ASCString getText( const MapCoordinate& pos )
         {
            return execactionnames[getActionID()];
         };
   };
   
class ContainerProperties: public ContextAction {
   public:
      bool available( const MapCoordinate& pos )
      {
         return actmap->getField(pos)->getContainer();
      };

      ASCString getText( const MapCoordinate& pos )
      {
         if ( actmap->getField(pos)->vehicle )
            return "edit unit properties";
         else
            return "edit building properties";
      };

      int getActionID()
      {
         return act_changeunitvals ;
      }
};

class ContainerCargo: public ContextAction {
   public:
      bool available( const MapCoordinate& pos )
      {
         return actmap->getField(pos)->getContainer() && actmap->getField(pos)->getContainer()->baseType->maxLoadableUnits > 0;
      };

      ASCString getText( const MapCoordinate& pos )
      {
         if ( actmap->getField(pos)->vehicle )
            return "edit unit cargo";
         else
            return "edit building cargo";
      };

      int getActionID()
      {
         return act_changecargo ;
      }
};

class DeleteVehicle: public ContextAction {
   public:
      bool available( const MapCoordinate& pos )
      {
         return actmap->getField(pos)->vehicle;
      };

      ASCString getText( const MapCoordinate& pos )
      {
         return "delete unit";
      };

      int getActionID()
      {
         return act_deleteunit ;
      }
};

class DeleteBuilding: public ContextAction {
   public:
      bool available( const MapCoordinate& pos )
      {
         return actmap->getField(pos)->building;
      };

      ASCString getText( const MapCoordinate& pos )
      {
        return "delete building";
      };

      int getActionID()
      {
         return act_deletebuilding;
      }
};

class ContainerProduction: public AutoTextContextAction {
   public:
      bool available( const MapCoordinate& pos )
      {
         return actmap->getField(pos)->getContainer() && actmap->getField(pos)->getContainer()->baseType->hasFunction( ContainerBaseType::InternalVehicleProduction  );
      };

      int getActionID()
      {
         return act_changeproduction ;
      }
};

class DeleteMine: public AutoTextContextAction {
   public:
      bool available( const MapCoordinate& pos )
      {
         return !actmap->getField(pos)->mines.empty() ;
      };

      int getActionID()
      {
         return act_deletemine ;
      }
};

class ChangeMineStrength: public AutoTextContextAction {
   public:
      bool available( const MapCoordinate& pos )
      {
         return !actmap->getField(pos)->mines.empty() ;
      };

      int getActionID()
      {
         return act_changeminestrength ;
      }
};

class FieldResources: public AutoTextContextAction {
   public:
      bool available( const MapCoordinate& pos )
      {
         return true;
      };

      int getActionID()
      {
         return act_changeresources;
      }
};

class DeleteTopObject: public AutoTextContextAction {
   public:
      bool available( const MapCoordinate& pos )
      {
         return !actmap->getField(pos)->objects.empty();
      };

      int getActionID()
      {
         return act_deletetopmostobject;
      }
};

class DeleteAllObjects: public AutoTextContextAction {
   public:
      bool available( const MapCoordinate& pos )
      {
         return !actmap->getField(pos)->objects.empty();
      };

      int getActionID()
      {
         return act_deleteallobjects;
      }
};


} // namespace ContextMenu

Maped_MainScreenWidget::Maped_MainScreenWidget( PG_Application& application )
              : MainScreenWidget( application ),
              vehicleSelector( NULL ), buildingSelector( NULL ), objectSelector(NULL), terrainSelector(NULL), mineSelector(NULL),
              weatherSelector( NULL ), selectionName(NULL), coordinateDisplay(NULL), currentSelectionWidget(NULL), contextMenu(NULL)
{

   setup( false );
   mapDisplay->mouseButtonOnField.connect( SigC::slot( mousePressedOnField ));
   mapDisplay->mouseButtonOnField.connect( SigC::slot( *this, &Maped_MainScreenWidget::clickOnMap ));
   mapDisplay->mouseDraggedToField.connect( SigC::slot( mouseDraggedToField ));

   

   
   setupStatusBar();
   
   menu = new Menu(this, PG_Rect(15,0,Width()-200,20));

   
   PG_Application::GetApp()->sigKeyDown.connect( SigC::slot( *this, &Maped_MainScreenWidget::eventKeyDown ));
   
   int xpos = Width() - 150;
   int w = 140;
   int ypos = 180;
   
   PG_Button* button = new PG_Button( this, PG_Rect( xpos, ypos, w, 20), "Select Vehicle" );
   button->sigClick.connect( SigC::slot( *this, &Maped_MainScreenWidget::selectVehicle ));
   ypos += 25;
   
   PG_Button* button2 = new PG_Button( this, PG_Rect( xpos, ypos, w, 20), "Select Building" );
   button2->sigClick.connect( SigC::slot( *this, &Maped_MainScreenWidget::selectBuilding ));
   ypos += 25;

   PG_Button* button3 = new PG_Button( this, PG_Rect( xpos, ypos, w, 20), "Select Object" );
   button3->sigClick.connect( SigC::slot( *this, &Maped_MainScreenWidget::selectObject ));
   ypos += 25;
   
   PG_Button* button4 = new PG_Button( this, PG_Rect( xpos, ypos, w, 20), "Select Terrain" );
   button4->sigClick.connect( SigC::slot( *this, &Maped_MainScreenWidget::selectTerrain ));
   ypos += 25;

   PG_Button* button5 = new PG_Button( this, PG_Rect( xpos, ypos, w, 20), "Select Mine" );
   button5->sigClick.connect( SigC::slot( *this, &Maped_MainScreenWidget::selectMine ));
   ypos += 25;
    
   
   new PG_Label( this, PG_Rect( xpos, ypos, w/2 - 5, 20), "Brush:");
   brushSelector = new DropDownSelector( this, PG_Rect( xpos+w/2+5, ypos, w/2-5, 20));
   brushSelector->AddItem("1");
   brushSelector->AddItem("3");
   brushSelector->AddItem("5");
   brushSelector->selectionSignal.connect( SigC::slot( *this, &Maped_MainScreenWidget::brushChanged ));
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
   selection.selectionChanged.connect( SigC::slot( *this, &Maped_MainScreenWidget::selectionChanged ));

   spawnOverviewMapPanel( "Mapeditor_OverviewMap" );

   addContextAction( new ContextMenu::ContainerProperties );
   addContextAction( new ContextMenu::ContainerCargo );
   addContextAction( new ContextMenu::ContainerProduction );
   addContextAction( new ContextMenu::DeleteVehicle );
   addContextAction( new ContextMenu::DeleteBuilding );
   addContextAction( new ContextMenu::ChangeMineStrength );
   addContextAction( new ContextMenu::DeleteMine );
   addContextAction( new ContextMenu::FieldResources );
   addContextAction( new ContextMenu::DeleteTopObject );
   addContextAction( new ContextMenu::DeleteAllObjects );
}

void Maped_MainScreenWidget::setupStatusBar()
{
   int x = mapDisplay->my_xpos;
   int y =  mapDisplay->my_ypos + mapDisplay->Height() + 25;
   int height = 20;

   messageLine = new PG_Label ( this, PG_Rect( x, y, 200, height ) );
   messageLine->SetFontSize(11);
   x += 210;
   

   coordinateDisplay = new PG_Label ( this, PG_Rect( x, y, 80, height ) );
   coordinateDisplay->SetFontSize(11);
   
   updateFieldInfo.connect( SigC::slot( *this, &Maped_MainScreenWidget::updateStatusBar ));
}

void Maped_MainScreenWidget::updateStatusBar()
{
   MapCoordinate pos = actmap->getCursor();
   coordinateDisplay->SetText( ASCString::toString( pos.x) + " / " + ASCString::toString( pos.y ));
}

void Maped_MainScreenWidget::selectionChanged( const MapComponent* item )
{
   selectionName->SetText( item->getItemType()->getName() + "(ID: " + ASCString::toString( item->getItemType()->getID()   ) + ")");
}


void Maped_MainScreenWidget::brushChanged( int i )
{
   selection.brushSize = i+1;
   if ( selection.brushSize < 1 )
      selection.brushSize = 1;
}

void Maped_MainScreenWidget:: addContextAction( ContextAction* contextAction )
{
   contextActions.push_back( contextAction );
}


bool Maped_MainScreenWidget::clickOnMap( const MapCoordinate& field, const SPoint& pos, bool changed, int button, int prio)
{
   if ( prio > 1 )
      return false;
   
   if( button == 3 ) {

      mapDisplay->cursor.goTo( field );
      
      int counter = 0;
      for ( deallocating_vector<ContextAction*>::iterator i = contextActions.begin(); i != contextActions.end(); ++i )
         if ( (*i)->available( field ))
            ++counter;

      if ( !counter )
         return false;

      if ( contextMenu )
         delete contextMenu;
      
      contextMenu = new PG_PopupMenu( this, pos.x, pos.y );

      for ( deallocating_vector<ContextAction*>::iterator i = contextActions.begin(); i != contextActions.end(); ++i )
         if ( (*i)->available( field )) {
            MapCoordinate mc = field;
            contextMenu->addMenuItem( (*i)->getText( field ), (*i)->getActionID() );
         }

      contextMenu->sigSelectMenuItem.connect( SigC::slot( *this, &Maped_MainScreenWidget::runContextAction   ));
      
      contextMenu->Show();
      return true;
   } else {
      if ( contextMenu ) {
         delete contextMenu;
         contextMenu = NULL;
      }
   }
   return false;
}

bool Maped_MainScreenWidget::runContextAction  (PG_PopupMenu::MenuItem* menuItem )
{
   execaction_ev( tuseractions( menuItem->getId() ) );

   return true;
}

#ifdef WIN32
# include "win32/win32-errormsg.h"
# include  "win32/msvc/mdump.h"
#endif


extern void testFieldSelector();

bool Maped_MainScreenWidget::eventKeyDown(const SDL_KeyboardEvent* key)
{
   int mod = SDL_GetModState() & ~(KMOD_NUM | KMOD_CAPS | KMOD_MODE);

   if ( !mod  ) {
      switch ( key->keysym.sym ) {
         case SDLK_RETURN:
         case SDLK_SPACE: execaction_ev( act_primaryAction );
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
         case SDLK_f:  execaction_ev(act_changeproduction);
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

         case SDLK_ESCAPE: execaction_ev(act_end);
                        return true;
         case SDLK_PLUS:
         case SDLK_KP_PLUS: execaction_ev( act_increase_zoom );
            return true;

         case SDLK_MINUS:
         case SDLK_KP_MINUS: execaction_ev( act_decrease_zoom );
            return true;
            
            default:;
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

         case SDLK_d:  testFieldSelector();
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
            case SDLK_F11:
               if ( mod & KMOD_SHIFT ) {
                  if (choice_dlg("Do you really want to crash ASC ?","~y~es","~n~o") == 1) {
                     char* c = NULL;
                     *c = 1;
                  }
               }  
            return true;
         default:;
   
       } 
   }

   if ( mod & KMOD_SHIFT ) {
      switch ( key->keysym.sym ) {
         case SDLK_d: execaction_ev(act_changeunitdir);
            return true;
         default:;
      }
   }

   if ( mod & KMOD_ALT ) {
      switch ( key->keysym.sym ) {
            case SDLK_RETURN:
               getPGApplication().toogleFullscreen();
               return true;

            default:;
      }
   }

   return false;
}



class MapItemSelectionWindow : public ItemSelectorWindow {
   public:
      MapItemSelectionWindow( PG_Widget *parent, const PG_Rect &r , const ASCString& title, SelectionItemFactory* itemFactory )
         : ItemSelectorWindow( parent, r, title, itemFactory ) {};
      
      void itemSelected( const SelectionWidget* )
      {
         if ( CGameOptions::Instance()->maped_modalSelectionWindow )
            Hide();
         QuitModal();
      }

};

typedef PG_Window* PG_WindowPointer;

template <class ItemType> 
void showSelectionWindow( PG_Widget* parent, PG_WindowPointer &selectionWindow, const ItemRepository<ItemType>& itemRepository  )
{
   if ( !selectionWindow ) {
      ItemSelectorWindow* sw = new MapItemSelectionWindow( parent, PG_Rect( parent->Width()-300, 100, 280, parent->Height()-150), "select item", new MapItemTypeWidgetFactory< MapItemTypeWidget<ItemType> >(itemRepository) );

      filtersChangedSignal.connect( SigC::slot( *sw, &ItemSelectorWindow::reLoad ));
      selectionWindow = sw;
   }
   
   selectionWindow->Show();
   selectionWindow->RunModal();
   if ( CGameOptions::Instance()->maped_modalSelectionWindow )
      selectionWindow->Hide();
      
}


bool Maped_MainScreenWidget :: selectVehicle()
{
   showSelectionWindow( this, vehicleSelector, vehicleTypeRepository );
   return true;
}

bool Maped_MainScreenWidget :: selectBuilding()
{
   showSelectionWindow( this, buildingSelector, buildingTypeRepository );
   return true;
}

bool Maped_MainScreenWidget :: selectObject()
{
   showSelectionWindow( this, objectSelector, objectTypeRepository );
   return true;
}

bool Maped_MainScreenWidget :: selectTerrain()
{
   showSelectionWindow( this, terrainSelector, terrainTypeRepository );
   return true;
}

bool Maped_MainScreenWidget :: selectMine()
{
   showSelectionWindow( this, mineSelector, mineTypeRepository );
   return true;
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




