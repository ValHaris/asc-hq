/** \file edglobal.cpp
    \brief various functions for the mapeditor
*/

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

#include <stdarg.h>
#include "global.h"
#include "vehicletype.h"
#include "buildingtype.h"
#include "edmisc.h"
#include "loadbi3.h"
#include "edgen.h"
#include "edselfnt.h"
#include "edglobal.h"
#include "gameoptions.h"
#include "mapdisplay.h"
#include "itemrepository.h"
#include "clipboard.h"
#include "resourceplacementdialog.h"
#ifdef WEATHERGENERATOR
# include "weatherdialog.h"
#endif
#include "maped-mainscreen.h"
#include "attack.h"
#include "mapimageexport.h"
#include "viewcalculation.h"
#include "statistics.h"
#include "spfst-legacy.h"

#include "dialogs/unitinfodialog.h"
#include "dialogs/editmapparam.h"   
#include "dialogs/alliancesetup.h"
#include "dialogs/playersetup.h"
#include "dialogs/editgameoptions.h"
#include "dialogs/admingame.h"
#include "dialogs/eventeditor.h"
#include "dialogs/newmap.h"
#include "dialogs/terraininfo.h"
#include "widgets/textrenderer.h"
#include "dialogs/exchangegraphics.h"
#include "dialogs/fileselector.h"
#include "dialogs/importbi3map.h"
#include "dialogs/unitguidedialog.h"
#include "dialogs/fileselector.h"
#include "stack.h"

   
   const char* execactionnames[execactionscount] = {
        "End MapEdit",
        "Help",
        "Goto EditMode",
        "Select terrain",
        "Select terrainAll",
        "Select unit",
        "Select color",
        "Select building",
        "Select object",
        "Select objectAll",
        "Select mine",
        "Select weather",
        "Setup alliances",
        "Setup Players",
        "Toggle ResourceMode",
        "Change UnitDirection",
        "Asc-Resource Mode",
        "Write Map2PCX",
        "Load map",
        "Change players",
        "New map",
        "Goto PolygonMode",
        "Rebuild Display",
        "Open UnitInfo",
        "View map",
        "About",
        "Create resources",
        "View/Change cargo",
        "View/Change resources",
        "Change TerrainDirection",
        "View/Change Events",
        "Toggle Fillmode",
        "Mapgenerator",
        "Use active field values as selection",
        "Delete active thing",
        "Show palette",
        "View/Change minestrength",
        "View/Change mapvalues",
        "View/Change production",
        "Save map",
        "View/Change item Values",
        "Mirror CX-Pos",
        "Mirror CY-Pos",
        "Place mine",
        "Place active thing",
        "Delete Unit",
        "Delete building",
        "Delete selected object",
        "Delete topmost object",
        "Delete all objects",
        "Delete mine",
        "AboutBox",
        "Save map as ...",
        "End PolygonMode",
        "Smooth coasts",
        "Import BI-Map",
        "SEPERATOR",
        "BI-Resource Mode",
        "Resize map",
        "Insert BI map",
        "Set zoom level",
        "Move Building",
        "set weather of whole map",
        "set map parameters",
        "terrain info",
        "set unit filter",
        "select graphic set",
        "unitset transformation",
        "Unitset Information",
        "switch maps",
        "transform map",
        "Edit Map Archival Information",
        "Display Resource Comparison",
        "specify unit production",
        "Paste",
        "Copy",
        "Cut",
        "Save Clipboard",
        "Load Clipboard",
        "Set Turn Number",
        "Show Pipeline Net",
        "Edit Technologies",
        "Edit ResearchPoints",
        "Generate TechTree",
        "Edit TechAdapter",
        "Reset Player Data...",
        "Fill map with resources",
        "setup weather generation",
        "Primary action",
        "Reset Player Data...",
        "View Player Strength",
        "Increase Zoom",
        "Decrease Zoom",
        "Edit Preferences",
        "Clear Mineral Resources",
        "Dump Building definition",
        "Dump Vehicle definition",
        "Dump Object definition",
        "PBP statistics",
        "Exchange Graphics",
        "Open Ctrl-key panel",
        "Close Ctrl-key panel",
        "Dump all vehicle definitions",
        "Clear Selection",
        "Dump all building definitions",
        "Mirror map",
        "Copy Area",
        "Paste Area",
        "Crash mapeditor",
        "Test Debug Function",
        "Unit Guide Dialog",
        "Run Lua Script",
	"Run Translation Script",
	"Dump all Terrain",
	"Dump all Objects",
        "show weapon range"
	 };



SelectionHolder selection;

void SelectionHolder::setSelection( const Placeable& component ) 
{
   delete currentItem;
   currentItem = component.clone();
   selectionChanged( currentItem );
}

void SelectionHolder::setPlayer( int player )
{
   actplayer = player;
   playerChanged( player );
   if ( currentItem )
      selectionChanged( currentItem );
}

void SelectionHolder::clear()
{ 
   currentItem = NULL;       
   selectionChanged( NULL ); 
}; 

void SelectionHolder::setWeather( int weather )
{
   currentWeather = weather;
   if ( currentItem )
      selectionChanged( currentItem );
}


const Placeable* SelectionHolder::getSelection()
{
   return currentItem;
}

void SelectionHolder::pickup ( MapField* fld )
{
   if ( fld->vehicle ) {
      VehicleItem v ( fld->vehicle->typ );
      actplayer = fld->vehicle->getOwner();
      setSelection( v );
   } else
   if ( fld->building ) {
      BuildingItem b ( fld->building->typ );
      actplayer = fld->building->getOwner();
      setSelection( b );
   } else
   if ( !fld->objects.empty() ) {
      ObjectItem o ( fld->objects.begin()->typ );
      setSelection( o );
   } else {
      TerrainItem t ( fld->typ->terraintype );
      setSelection( t );
   }
}


      
        
// � Infomessage

int infomessage( char* formatstring, ... )
{
   char stringtooutput[200];
   char* b;
   char* c = new char[200];
   // int linenum = 0;

   memset (stringtooutput, 0, sizeof ( stringtooutput ));

   b = stringtooutput;

   va_list paramlist;
   va_start ( paramlist, formatstring );

   vsprintf ( stringtooutput, formatstring, paramlist );

   va_end ( paramlist );

   npush ( activefontsettings );
   activefontsettings.justify = lefttext;
   activefontsettings.font = schriften.smallarial;
   activefontsettings.color = lightgray;
   activefontsettings.markcolor = red;
   activefontsettings.background = 0;
   activefontsettings.length = agmp->resolutionx - ( 640 - 387);

   int yy = agmp->resolutiony - ( 480 - 450 );
   showtext3c( stringtooutput, 37, yy );

   npop( activefontsettings );

   if ( formatstring == NULL  ||  formatstring[0] == 0 )
      lastdisplayedmessageticker = 0xffffff;
   else
      lastdisplayedmessageticker = ticker;


   delete ( c );

   return ++actdisplayedmessage;
}


ASCString getbipath ( void )
{
   ASCString filename = getbi3path();
   appendbackslash( filename );
   filename += "mis";
   filename += pathdelimitterstring;
   filename += "*.dat";

   int cnt = 0;

   while ( !exist ( filename )) {
      filename = editString("enter Battle Isle path", filename );
      if ( filename.empty() )
         return "";

      appendbackslash(filename);
      
      CGameOptions::Instance()->BI3directory = filename;
      CGameOptions::Instance()->setChanged ( 1 );

      filename += "mis";
      filename += pathdelimitterstring;
      filename += "*.dat";
      
      cnt++;
      #if CASE_SENSITIVE_FILE_NAMES == 1
      if (!exist ( filename ) && cnt == 1 )
         displaymessage("The 'mis' and 'ger' / 'eng' directories must be lower case to import files from them !", 1 );
      #endif
   }

   return getbi3path();
}



class PlayerColorPanel : public PG_Widget {
      int openTime;
   public:
      PlayerColorPanel() : PG_Widget(NULL, PG_Rect(20,20,320,40))
      {
         int width = 30;
         int gap = 5;
         for ( int i = 0; i < max(actmap->getPlayerCount(),9); ++i ) {
            PG_Widget* bar = new ColoredBar( actmap->getPlayer(i).getColor(), this, PG_Rect( gap + i * (width+gap), gap, width, width ));
            PG_Label* lab = new PG_Label( bar, PG_Rect(5,5,width-10,width-10), ASCString::toString(i));
            lab->SetFontSize(15);
         }

         PG_Application::GetApp()->sigAppIdle.connect( SigC::slot( *this, &PlayerColorPanel::idler ));
      }

      void Show( bool fade = false )
      {
         openTime = ticker;
         PG_Widget::Show(fade);
      }

      void Hide( bool fade = false )
      {
         openTime = 0;
         PG_Widget::Hide(fade);
      }


      bool eventKeyDown(const SDL_KeyboardEvent* key)
      {
         if ( key->keysym.sym >= '0' && key->keysym.sym <= '9' ) {
            selection.setPlayer( key->keysym.sym - '0' );
            return true;
         }
         Hide();
         return false;
      }

      bool idler()
      {
         if ( !IsVisible() )
            return false;

         if ( !openTime )
            return false;

         if ( ticker > openTime + 200 ) {
            Hide();
            return true;
         } else
            return false;

      }

};


void showPlayerPanel( bool open )
{
   static PlayerColorPanel* pcp = NULL;
   if ( open && !pcp ) 
      pcp = new PlayerColorPanel();


   if ( !pcp )
      return;

   if( open )
      pcp->Show();
   else
      pcp->Hide();
}


int countPlayersBinary( const ContainerBase* cont ) 
{
   int mask = 1 << cont->getOwner();
   for ( ContainerBase::Cargo::const_iterator i = cont->getCargo().begin(); i != cont->getCargo().end(); ++i )
      if ( *i )
         mask |= countPlayersBinary( *i );
   return mask;
}

int countPlayers( const ContainerBase* cont )
{
   int res = countPlayersBinary( cont );
   int count = 0;
   for ( int i = 0; i < cont->getMap()->getPlayerCount(); ++i )
      if ( res & (1 << i ))
         ++count;
   return count;
}

void infoMessageClipboardPlayers(const ContainerBase* cont )
{
   if ( countPlayers( cont ) > 1 )
      infoMessage("Info: this unit/building contains cargo from other players");
}

// � ExecAction


//! this executes all functions that use legacy Eventhandling
void execaction( int code)
{
   switch(code) {
    case act_toggleresourcemode :  {
         if ( mainScreenWidget )
            mainScreenWidget->toggleMapLayer( "resources");
         displaymap();
       }
       break;
    case act_asc_resource :   {
                                  actmap->_resourcemode = false;
                                  displaymessage ( "ASC Resource mode enabled", 3 );
                                }
       break;
    case act_bi_resource :   {
                                  actmap->_resourcemode = true;
                                  displaymessage ( "Battle Isle Resource mode enabled", 3 );
                               }
       break;
    case act_changeplayers : playerchange();
       break;
       /*
    case act_polymode :   {
          getpolygon(&pfpoly);
          if (pfpoly != NULL ) {
             tfill = false;
             polyfieldmode = true;

             tchangepoly cp;

             cp.poly = pfpoly;
             cp.setpolytemps(1);
             cursor.gotoxy(1,1);

             displaymap();
             pdbaroff();
          }
       }
       break;
       */
    case act_viewmap :  
             {
             while (mouseparams.taste != 0)
                releasetimeslice();
             // showmap ();
             displaymap();
             }
       break;
    case act_changeunitdir : {
                      MapField* pf2 = getactfield();
                      if ( pf2 && pf2->vehicle  ) {
                         pf2->vehicle->direction++;
                         if (pf2->vehicle->direction >= sidenum )
                            pf2->vehicle->direction = 0;
                         
                         mapsaved = false;
                         displaymap();
                      } 
                   }
       break;
    case act_changeresources :   changeresource();
       break;
    case act_createresources : {
                           tputresourcesdlg prd;
                           prd.init();
                           prd.run();
                           prd.done();
                           if ( mainScreenWidget )
                              mainScreenWidget->activateMapLayer( "resources", true);
                        
                           repaintMap();

                         }
       break;
       /*
    case act_fillmode :   if ( polyfieldmode == false ) {   
                 if (tfill == true) tfill = false;
                 else tfill = true; 
                 fillx1 = cursor.posx + actmap->xpos;
                 filly1 = cursor.posy + actmap->ypos; 
                 pdbaroff(); 
              } 
       break;
       */
    case act_mapgenerator : mapgenerator();
       break;
    case act_setactivefieldvals : if ( getactfield() ) 
                                      selection.pickup( getactfield() ); 
       break;
    case act_deletething : {
                         MapField* pf2 = getactfield();
                         mapsaved = false;
                         if (pf2 != NULL) {
                            if ( !removeCurrentItem() ) {
                              if (pf2->vehicle != NULL)
                                 delete pf2->vehicle;
                              else
                                 if (pf2->building != NULL)
                                    delete pf2->building;
                                 else
                                    if ( !pf2->mines.empty() )
                                       pf2->removemine( -1 );
                                    else 
                                       pf2->removeObject( NULL );
                            }
                                  

                            mapsaved = false;
                            mapChanged( actmap );
                         }
                      }
        break;
    case act_deleteunit : {
                         MapField* pf2 = getactfield();
                         if (pf2 != NULL)
                            if (pf2->vehicle != NULL) {
                               delete pf2->vehicle;
                               mapsaved = false;
                               mapChanged( actmap );
                            }
                         }
        break;
     case act_deletebuilding : {
                         MapField* pf2 = getactfield();
                         if (pf2 != NULL)
                            if (pf2->building != NULL) {
                               delete pf2->building;
                               mapsaved = false;
                               mapChanged( actmap );
                            }
                      }
        break;
     case act_deleteobject :
     case act_deletetopmostobject : {
                         MapField* pf2 = getactfield();
                         if ( pf2 ) {
                            mapsaved = false;
                            pf2->removeObject( NULL );
                            mapChanged( actmap );
                         }
                      }
        break;
     case act_deleteallobjects : {
                         MapField* pf2 = getactfield();
                         if ( pf2 ) {
                            mapsaved = false;
                            pf2->objects.clear( );
                            calculateallobjects( actmap );
                            mapChanged( actmap );
                         }
                      }
        break;
     case act_deletemine : {
                         MapField* pf2 = getactfield();
                         if (pf2 != NULL) {
                            mapsaved = false;
                            pf2->removemine( -1 );
                            mapChanged( actmap );
                         }
                      }
        break;
    case act_changeminestrength : changeminestrength();
       break;
    case act_changeunitvals :   {
                 MapField* pf2 = getactfield();
                 if ( pf2  ) {
                    if ( pf2->vehicle ) {
                       changeunitvalues(pf2->vehicle);
                       displaymap();
                    }
                    else if ( pf2->building ) {
                       changebuildingvalues(*pf2->building);
                    } /* endif */
                 } /* endif */
              } 
              break;
              
    case act_mirrorcursorx :   {
                     MapDisplayPG* md = getMainScreenWidget()->getMapDisplay();
                     md->cursor.goTo( MapCoordinate(actmap->xsize - md->cursor.pos().x, md->cursor.pos().y ) );
                 }

       break;
    case act_mirrorcursory :   {
                     MapDisplayPG* md = getMainScreenWidget()->getMapDisplay();
                     md->cursor.goTo( MapCoordinate(md->cursor.pos().x, actmap->ysize - md->cursor.pos().y ) );
                 }
       break;
       
//    case act_placemine : placemine();
//       break;
    case act_placething : placeCurrentItem();
       break;
       /*
    case act_endpolyfieldmode : {
          if (polyfieldmode) {
             polyfieldmode = false;
             tchangepoly cp;

             cp.poly = pfpoly;
             cp.setpolytemps(0);

             displaymap();
             pdbaroff();
             ch = 255;
          }
       }
       break;
       */
    case act_about :
    case act_aboutbox : {
       
       ASCString s = "#fontsize=22#Advanced Strategic Command\nMap Editor#fontsize=14#\n";
       s += getVersionAndCompilation();

       // s += "\n#fontsize=18#Credits#fontsize=14#\n";

       s += readtextmessage( 1020 );
                     
       ViewFormattedText vft( "About", s, PG_Rect(-1,-1,450,550));
       vft.Show();
       vft.RunModal();
       
       /*
         help(1020);
         tviewanytext vat;
         ASCString msg = kgetstartupmessage();
         vat.init ( "about", msg.c_str() );
         vat.run();
         vat.done();
       */
      }
      break;
   case act_smoothcoasts : {
            ForestCalculation::smooth ( 6, actmap, NULL );
            displaymap();
      }
      break;
   case act_resizemap : resizemap();
      break;
   case act_movebuilding: movebuilding();
      break;
      case act_setactweatherglobal: setweatherall ( selection.getWeather() );
                                 displaymap();
      break;
   case act_terraininfo: viewterraininfo( actmap, actmap->getCursor(), true);
      break;
   case act_setunitfilter: selectunitsetfilter();
                           filtersChangedSignal(); 
      break;
   // case act_setzoom : choosezoomlevel();
   //    break;
   case act_unitsettransformation: unitsettransformation();
      break;
   case act_unitSetInformation: viewUnitSetinfo();
      break;
   case act_primaryAction: if ( mapSwitcher.getDefaultAction() == MapSwitcher::select ) {
                              execaction ( act_setactivefieldvals );
                              execaction(act_switchmaps);
                           } else 
                             execaction( act_placething );
                             
                           break;
                           
   case act_switchmaps: mapSwitcher.toggle();
                        displaymap();
                        updateFieldInfo();
      break;
   case act_editArchivalInformation: editArchivalInformation();
      break;
   case act_displayResourceComparison : resourceComparison();
      break;
      /*
   case act_specifyunitproduction: unitProductionLimitation();
      break;
      */
   case act_pasteFromClipboard: if ( getactfield() && !getactfield()->getContainer() ) {
                                   ClipBoard::Instance().place( actmap->getCursor() );
                                   mapsaved = false;
                                   displaymap();
                                }
      break;
   case act_copyToClipboard: if ( getactfield() ) {
                                 if ( getactfield()->vehicle ) {
                                    ClipBoard::Instance().clear();
                                    infoMessageClipboardPlayers( getactfield()->vehicle );
                                    ClipBoard::Instance().addUnit( getactfield()->vehicle );
                                 } else
                                    if ( getactfield()->building ) {
                                       ClipBoard::Instance().clear();
                                       infoMessageClipboardPlayers( getactfield()->building );
                                       ClipBoard::Instance().addBuilding( getactfield()->building );
                                    }
                             }
      break;
   case act_cutToClipboard: if ( getactfield() ) {
                              if ( getactfield()->vehicle ) {
                                 ClipBoard::Instance().clear();
                                 infoMessageClipboardPlayers( getactfield()->vehicle );
                                 ClipBoard::Instance().addUnit( getactfield()->vehicle );
                                 execaction ( act_deleteunit );
                                 mapsaved = false;
                              } else
                                 if ( getactfield()->building ) {
                                    ClipBoard::Instance().clear();
                                    infoMessageClipboardPlayers( getactfield()->building );
                                    ClipBoard::Instance().addBuilding( getactfield()->building );
                                    execaction ( act_deletebuilding );
                                    mapsaved = false;
                                 }
                            }
      break;
   case act_saveClipboard:  saveClipboard();
      break;

   case act_readClipBoard:  readClipboard();
      break;
   case act_setTurnNumber:  actmap->time.set ( getid("Turn",actmap->time.turn(),0,maxint), 0 );
      break;
   case act_showPipeNet:    mainScreenWidget->getMapDisplay()->toggleMapLayer("pipes");
      repaintMap();
      break;
   case act_editResearch:  editResearch();
      break;
   case act_editResearchPoints:  editResearchPoints();
      break;
   case act_generateTechTree: generateTechTree();
      break;
   case act_editTechAdapter: editTechAdapter();
      break;
   case act_playerStrengthSummary: pbpplayerstatistics( actmap );
      break;
   case act_unitGuideDialog: unitGuideWindow(2);
      break;
      
    }
}

//! this executes all functions that use Paragui Eventhandling
void execaction_pg(int code) 
{
   switch(code) {
#ifdef WEATHERGENERATOR
      case act_setactnewweather: weatherConfigurationDialog();
         break;
#endif
    case act_selbodentyp : if ( mapSwitcher.getDefaultAction() == MapSwitcher::select ) 
                              execaction ( act_setactivefieldvals );
                           execaction( act_switchmaps);
       break;
    case act_selbodentypAll : mainScreenWidget->selectTerrainList();
       break;
    case act_selunit : mainScreenWidget->selectVehicle();
       break;
    case act_selbuilding : mainScreenWidget->selectBuilding(); 
       break;
    case act_selobject : mainScreenWidget->selectObject();
       break;
    case act_selobjectAll: mainScreenWidget->selectObjectList();
       /* if ( mapSwitcher.getDefaultAction() == MapSwitcher::select ) 
                              execaction ( act_setactivefieldvals );
                           execaction( act_switchmaps);
                           */
       break;                    
    case act_selmine : mainScreenWidget->selectMine();
       break;
    case act_unitinfo :  unitInfoDialog();
       break;
    case act_setmapparameters: setmapparameters( actmap );
       break;
    case act_setupalliances :  {
            DirectAllianceSetupStrategy dass;
            setupalliances( actmap, &dass, true );
         }
       break;
    case act_setupplayers :  setupPlayers( actmap, true );
       break;
    case act_loadmap :   {
                            if (mapsaved == false )
                               if (choice_dlg("Map not saved ! Save now ?","~y~es","~n~o") == 1) 
                                  k_savemap(false);

                            GameMap* oldmap = actmap;
                            actmap = NULL;
                            try {
                                k_loadmap();
                            }
                            catch ( ... ) {
                                displaymessage ( "error loading file",1 );
                            }
                            if ( !actmap ) {
                                actmap = oldmap;
                                oldmap = NULL;
                            } else {
                               delete oldmap;
                               oldmap = NULL;
                            }
                            displaymap();
                          } 
       break;
    case act_savemap :  k_savemap(false);
       break;
    case act_savemapas :  k_savemap(true);
       break;
    case act_maptopcx : {
       bool view = choice_dlg("Include view ?","~n~o", "~y~es") == 2;
       if ( view )
          computeview( actmap );
       writemaptopcx ( actmap, view );
                        }
       break;
    case act_end : {
          if ( mapSwitcher.getDefaultAction() == MapSwitcher::select ) 
             execaction(act_switchmaps);
          else
             if (choice_dlg("Do you really want to quit ?","~y~es","~n~o") == 1) {
                getPGApplication().Quit();
             }
       }
       break;
       case act_changecargo :
          if ( getactfield() && getactfield()->getContainer() )
             cargoEditor( getactfield()->getContainer() );
       
       break;
       case act_createresources2 : resourcePlacementDialog();
         displaymap();
       break;
       case act_changeproduction :   if ( getactfield() && getactfield()->getContainer() ) {
                                       if ( getactfield()->getContainer()->baseType->hasFunction( ContainerBaseType::InternalVehicleProduction  ))
                                          editProduction( getactfield()->getContainer() );
                                       else
                                          warningMessage("this unit/building has no production capabilities");
                                     }
       break;
       case act_selectgraphicset: selectgraphicset();
                              // showallchoices();
       break;
       case act_transformMap: transformMap();
       break;
      case act_increase_zoom:
         if ( mainScreenWidget && mainScreenWidget->getMapDisplay() ) {
            mainScreenWidget->getMapDisplay()->changeZoom( 10 );
            viewChanged();
            repaintMap();
         }
         break;
      case act_decrease_zoom:
         if ( mainScreenWidget && mainScreenWidget->getMapDisplay() ) {
            mainScreenWidget->getMapDisplay()->changeZoom( -10 );
            viewChanged();
            repaintMap();
         }
         break;
      case act_editpreferences:
         editGameOptions( false );
         break;

      case act_events :   eventEditor( actmap );
         break;
      case act_resetPlayerData: adminGame( actmap );
         break;
      case act_changemapvals :   editMap(actmap);
         break;
      case act_newmap :   newmap();
         break;
      case act_clearresources: {
         for ( int y = 0; y < actmap->ysize; ++y)
            for ( int x = 0; x < actmap->xsize; ++x ) {
               actmap->getField(x,y)->fuel = 0;
               actmap->getField(x,y)->material = 0;
            }
            repaintMap();
      }
      case act_dumpBuilding: 
         if ( getactfield() && getactfield()->building ) {
            ASCString filename = selectFile( "*.dump", false );
            if ( !filename.empty () ) {
               tn_file_buf_stream stream ( filename, tnstream::writing );
               PropertyWritingContainer pc ( "BuildingDump", stream );
               actmap->getbuildingtype_byid(getactfield()->building->typ->id)->runTextIO( pc );
            }
         } else
            errorMessage("no building selected");
         break;
      case act_dumpVehicle: 
         if ( getactfield() && getactfield()->vehicle ) {
            ASCString filename = selectFile( "*.dump", false );
            if ( !filename.empty () ) {
               tn_file_buf_stream stream ( filename, tnstream::writing );
               PropertyWritingContainer pc ( "VehicleDump", stream );
               actmap->getvehicletype_byid(getactfield()->vehicle->typ->id)->runTextIO( pc );
            }
         } else
            errorMessage("no vehicle selected");
         break;
      case act_dumpObject: 
         if ( getactfield() && !getactfield()->objects.empty() ) {
            ASCString filename = selectFile( "*.dump", false );
            if ( !filename.empty () ) {
               tn_file_buf_stream stream ( filename, tnstream::writing );
               PropertyWritingContainer pc ( "ObjectDump", stream );
               actmap->getobjecttype_byid( getactfield()->objects.front().typ->id)->runTextIO( pc );
            }
         } else
            errorMessage("no object selected");
         break;
    case act_help : help(1000);
       break;
    case act_pbpstatistics: pbpplayerstatistics( actmap );
       break;
    case act_exchangeGraphics: exchangeGraphics();
       break;
    case act_openControlPanel: showPlayerPanel(true);
       break;
    case act_releaseControlPanel: showPlayerPanel(false);
       break;
    case act_dumpAllVehicleDefinitions: {
                                          StatusMessageWindowHolder smw = MessagingHub::Instance().infoMessageWindow( "dumping all units" );
                                          for ( int i = 0; i < vehicleTypeRepository.getNum(); ++i ) {
                                             VehicleType* veh = vehicleTypeRepository.getObject_byPos( i );
                                             tn_file_buf_stream stream ( "Vehicle" + ASCString::toString( i ) + ".dump", tnstream::writing );
                                             PropertyWritingContainer pc ( "VehicleDump", stream );
                                             veh->runTextIO( pc );
                                          }
                                        };
       break;
    case act_dumpAllBuildings: {
                                          StatusMessageWindowHolder smw = MessagingHub::Instance().infoMessageWindow( "dumping all buildings" );
                                          for ( int i = 0; i < buildingTypeRepository.getNum(); ++i ) {
                                             BuildingType* bld = buildingTypeRepository.getObject_byPos( i );
                                             tn_file_buf_stream stream ( "Building" + ASCString::toString( i ) + ".dump", tnstream::writing );
                                             PropertyWritingContainer pc ( "BuildingDump", stream );
                                             bld->runTextIO( pc );
                                          }
                                        };
       break;
    case act_dumpAllTerrain: {
                                          StatusMessageWindowHolder smw = MessagingHub::Instance().infoMessageWindow( "dumping all terrain" );
                                          for ( int i = 0; i < terrainTypeRepository.getNum(); ++i ) {
                                             TerrainType* bld = terrainTypeRepository.getObject_byPos( i );
                                             tn_file_buf_stream stream ( "terrain" + ASCString::toString( i ) + ".dump", tnstream::writing );
                                             PropertyWritingContainer pc ( "terrainDump", stream );
                                             bld->runTextIO( pc );
                                          }
                                        };
       break;
    case act_dumpAllObjects: {
                                          ASCString text = "dumping all objects";
                                          StatusMessageWindowHolder smw = MessagingHub::Instance().infoMessageWindow( text );
                                          for ( int i = 0; i < objectTypeRepository.getNum(); ++i ) {
                                             ObjectType* bld = objectTypeRepository.getObject_byPos( i );
                                             tn_file_buf_stream stream ( "object" + ASCString::toString( i ) + ".dump", tnstream::writing );
                                             PropertyWritingContainer pc ( "objectDump", stream );
                                             bld->runTextIO( pc );
                                             smw.SetText( text + ": " + ASCString::toString( int(i * 100 / objectTypeRepository.getNum() )) + "%" );
                                          }
                                        };
       break;

      case act_clearSelection: selection.clear();
      break;
      case act_locateItemByID: locateItemByID();
         break;
      case act_mirrorMap: mirrorMap();
         break;
      case act_copyArea: copyArea();
         break;
      case act_pasteArea: pasteArea();
         break;
      case asc_nullPointerCrash: {
            char* p = NULL;
            *p = 1;
         }
         break;
      case asc_testFunction: testDebugFunction(); 
         break;
      case act_import_bi_map : 
         if (mapsaved == false )
            if (choice_dlg("Map not saved ! Save now ?","~y~es","~n~o") == 1) 
               k_savemap(false);
         importBI3Map( actmap );
         displaymap();
         break;
      case act_insert_bi_map : 
         importBI3Map( actmap, true );
         displaymap();
         break;
      case act_runLuaScript:
         selectAndRunLuaScript( "*.lua" );
         repaintMap();
         break;
      case act_runTranslationScript:
         selectAndRunLuaScript( "*.map.*" );
         repaintMap();
         break;
      case act_showweapnrange:
         mainScreenWidget->showWeaponRange( actmap, actmap->getCursor() );
         displaymap();
         break;
         
         
   };
}

void         execaction_ev(int code)
{
   execaction_pg(code);
   execaction(code);
}
