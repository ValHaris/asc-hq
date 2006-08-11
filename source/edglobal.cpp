/** \file edglobal.cpp
    \brief various functions for the mapeditor
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

#include "dialogs/unitinfodialog.h"
#include "dialogs/editmapparam.h"   
#include "dialogs/alliancesetup.h"
#include "dialogs/playersetup.h"
#include "dialogs/editgameoptions.h"
#include "dialogs/admingame.h"
#include "dialogs/eventeditor.h"
#include "dialogs/newmap.h"
#include "dialogs/terraininfo.h"

   
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
        "Clear Mineral Resources"};



SelectionHolder selection;

void SelectionHolder::setSelection( const MapComponent& component ) 
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

void SelectionHolder::setWeather( int weather )
{
   currentWeather = weather;
   if ( currentItem )
      selectionChanged( currentItem );
}


const MapComponent* SelectionHolder::getSelection()
{
   return currentItem;
}

void SelectionHolder::pickup ( tfield* fld )
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


      
        
// õS Infomessage

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


double unitStrengthValue( Vehicle* veh )
{
   double s = veh->typ->productionCost.energy + veh->typ->productionCost.material;
   AttackFormula af;
   s *= (af.strength_experience( veh->experience) + af.defense_experience( veh->experience))/2 + 1.0 ;
   s *= af.strength_damage( veh->damage );
   return s;
}

void showPlayerStrength()
{
   ASCString message;
   for ( int i = 0; i< 8; ++i ) {
      double strength = 0;
      Resources r;
      for ( Player::VehicleList::iterator j = actmap->player[i].vehicleList.begin(); j != actmap->player[i].vehicleList.end(); ++j ) {
         strength += unitStrengthValue( *j );
         r += (*j)->typ->productionCost;
      }

      message += "\nPlayer " + ASCString::toString(i) + " " + actmap->player[i].getName() + "\n";
      message += "strength: ";
      ASCString s;
      s.format("%9.0f", ceil(strength/10000) );
      message += s + "\n";
      message += "Unit production cost: \n";
      for ( int k = 0; k < 3; ++k ) {
         message += resourceNames[k];
         message += ": " + ASCString::toString(r.resource(k)/1000 ) + "\n";
      }
      message += "Unit count: " + ASCString::toString( int( actmap->player[i].vehicleList.size()));
      message += "\n\n";

   }
   tviewanytext vat ;
   vat.init ( "Player strength summary", message.c_str(), 20, -1 , 450, 480 );
   vat.run();
   vat.done();
}





// õS ExecAction


//! this executes all functions that use legacy Eventhandling
void execaction( int code)
{
   switch(code) {
    case act_help : help(1000);
       break;
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
                      tfield* pf2 = getactfield();
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
                         tfield* pf2 = getactfield();
                         mapsaved = false;
                         if (pf2 != NULL) {
                            if (pf2->vehicle != NULL)
                               delete pf2->vehicle;
                            else
                               if (pf2->building != NULL)
                                  delete pf2->building;
                               else
                                  if ( !pf2->mines.empty() )
                                     pf2->removemine( -1 );
                                  else
                                     pf2->removeobject( NULL );

                            mapsaved = false;
                            mapChanged( actmap );
                         }
                      }
        break;
    case act_deleteunit : {
                         tfield* pf2 = getactfield();
                         if (pf2 != NULL)
                            if (pf2->vehicle != NULL) {
                               delete pf2->vehicle;
                               mapsaved = false;
                               mapChanged( actmap );
                            }
                         }
        break;
     case act_deletebuilding : {
                         tfield* pf2 = getactfield();
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
                         tfield* pf2 = getactfield();
                         if ( pf2 ) {
                            mapsaved = false;
                            pf2->removeobject( NULL );
                            mapChanged( actmap );
                         }
                      }
        break;
     case act_deleteallobjects : {
                         tfield* pf2 = getactfield();
                         if ( pf2 ) {
                            mapsaved = false;
                            pf2->objects.clear( );
                            calculateallobjects();
                            mapChanged( actmap );
                         }
                      }
        break;
     case act_deletemine : {
                         tfield* pf2 = getactfield();
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
                 tfield* pf2 = getactfield();
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
              /*
    case act_mirrorcursorx :   {
                    cursor.gotoxy ( actmap->xsize-getxpos(), getypos() );
                    int tmp = farbwahl;
                    farbwahl = altefarbwahl;
                    altefarbwahl = tmp;
                    showallchoices();
                    showStatusBar();
                 }

       break;
    case act_mirrorcursory :   {
                    cursor.gotoxy ( getxpos(), actmap->ysize-getypos() );
                    int tmp = farbwahl;
                    farbwahl = altefarbwahl;
                    altefarbwahl = tmp;
                    showallchoices();
                    showStatusBar();
                 }
       break;
       */
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
         help(1020);
         tviewanytext vat;
         ASCString msg = kgetstartupmessage();
         vat.init ( "about", msg.c_str() );
         vat.run();
         vat.done();
      }
      break;
   case act_smoothcoasts : {
            ForestCalculation::smooth ( 6, actmap, NULL );
            displaymap();
      }
      break;
   case act_import_bi_map : {
         ASCString path = getbipath();
         if ( path.empty() )
            break;

         ASCString wildcard = path + "mis" + pathdelimitterstring + "*.dat" ;

         ASCString filename = selectFile( wildcard, true );
         if ( !filename.empty() ) {
            importbattleislemap ( path.c_str(), filename.c_str(), terrainTypeRepository.getObject_byID(9999)->weather[0] );
            displaymap();
         }
      }
      break;
   case act_insert_bi_map : {
         ASCString path = getbipath();
         if ( path.empty() )
            break;

         ASCString wildcard = path + "mis" + pathdelimitterstring + "*.dat" ;

         ASCString filename = selectFile( wildcard, true );
         if ( !filename.empty() ) {
            insertbattleislemap ( actmap->getCursor().x, actmap->getCursor().y, path.c_str(), filename.c_str() );
            displaymap();
         }
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
   case act_setzoom : choosezoomlevel();
      break;
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
   case act_copyToClipboard: if ( getactfield() )
                                 if ( getactfield()->vehicle ) {
                                    ClipBoard::Instance().clear();
                                    ClipBoard::Instance().addUnit( getactfield()->vehicle );
                                 } else
                                    if ( getactfield()->building ) {
                                       ClipBoard::Instance().clear();
                                       ClipBoard::Instance().addBuilding( getactfield()->building );
                                    }
      break;
   case act_cutToClipboard: if ( getactfield() )
                              if ( getactfield()->vehicle ) {
                                 ClipBoard::Instance().clear();
                                 ClipBoard::Instance().addUnit( getactfield()->vehicle );
                                 execaction ( act_deleteunit );
                                 mapsaved = false;
                              } else
                                 if ( getactfield()->building ) {
                                    ClipBoard::Instance().clear();
                                    ClipBoard::Instance().addBuilding( getactfield()->building );
                                    execaction ( act_deletebuilding );
                                    mapsaved = false;
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
   case act_playerStrengthSummary: showPlayerStrength();
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
    case act_selbodentypAll : mainScreenWidget->selectTerrain();
       break;
    case act_selunit : mainScreenWidget->selectVehicle();
       break;
    case act_selbuilding : mainScreenWidget->selectBuilding(); 
       break;
    case act_selobject : mainScreenWidget->selectObject();
       break;
    case act_selobjectAll: if ( mapSwitcher.getDefaultAction() == MapSwitcher::select ) 
                              execaction ( act_setactivefieldvals );
                           execaction( act_switchmaps);
       break;                    
    case act_selmine : mainScreenWidget->selectMine();
       break;
    case act_unitinfo :  unitInfoDialog();
       break;
    case act_setmapparameters: setmapparameters( actmap );
       break;
    case act_setupalliances :  setupalliances( actmap, true );
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
    case act_maptopcx : writemaptopcx ( actmap );
       break;
    case act_end : {
          if ( mapSwitcher.getDefaultAction() == MapSwitcher::select ) 
             execaction(act_switchmaps);
          else
             if (choice_dlg("Do you really want to quit ?","~y~es","~n~o") == 1) {
                if (mapsaved == false )
                   if (choice_dlg("Map not saved ! Save now ?","~y~es","~n~o") == 1)
                      k_savemap(false);
                   
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
                                          warning("this unit/building has no production capabilities");
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

      case act_events :   eventEditor();
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
         
   };
}

void         execaction_ev(int code)
{
   execaction_pg(code);
   // getPGApplication().enableLegacyEventHandling ( true );
   execaction(code);
   // getPGApplication().enableLegacyEventHandling ( false );
}
