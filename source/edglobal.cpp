/** \file edglobal.cpp
    \brief various functions for the mapeditor
*/

//     $Id: edglobal.cpp,v 1.50 2002-11-02 14:13:17 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.49  2002/11/01 20:44:53  mbickel
//      Added function to specify which units can be build by other units
//
//     Revision 1.48  2002/10/12 17:28:03  mbickel
//      Fixed "enemy unit loaded" bug.
//      Changed map format
//      Improved log messages
//
//     Revision 1.47  2002/04/14 17:21:17  mbickel
//      Renamed global variable pf to pf2 due to name clash with SDL_mixer library
//
//     Revision 1.46  2002/03/17 21:25:18  mbickel
//      Fixed: View unit movement revealed the reaction fire status of enemy units
//      Mapeditor: new function "resource comparison"
//
//     Revision 1.45  2002/03/02 23:04:01  mbickel
//      Some cleanup of source code
//      Improved Paragui Integration
//      Updated documentation
//      Improved Sound System
//
//     Revision 1.44  2002/01/29 20:42:16  mbickel
//      Improved finding of files with relative path
//      Added wildcards to music playlist files
//
//     Revision 1.43  2001/12/14 10:20:05  mbickel
//      Cleanup and enhancements to configure.in
//      Removed last remains of octagonal version from source files
//
//     Revision 1.42  2001/10/31 18:34:31  mbickel
//      Some adjustments and fixes for gcc 3.0.2
//
//     Revision 1.41  2001/10/11 10:41:06  mbickel
//      Restructured platform fileio handling
//      Added map archival information to mapeditor
//
//     Revision 1.40  2001/10/11 10:22:49  mbickel
//      Some cleanup and fixes for Visual C++
//
//     Revision 1.39  2001/10/08 14:12:20  mbickel
//      Fixed crash in AI
//      Speedup of AI
//      Map2PCX improvements
//      Mapeditor usability improvements
//
//     Revision 1.38  2001/10/02 14:06:28  mbickel
//      Some cleanup and documentation
//      Bi3 import tables now stored in .asctxt files
//      Added ability to choose amoung different BI3 import tables
//      Added map transformation tables
//
//     Revision 1.37  2001/09/24 12:05:18  mbickel
//      Fixed: airplanes landing in wrong building for repairs
//
//     Revision 1.36  2001/09/23 23:06:20  mbickel
//      Fixed:
//       - ascent/descent during reactionfire
//       - movement with nearly empty fuel tank
//       - production icon displayed although unit could not be produced
//       - invisible building becoming visible in fog of war
//
//     Revision 1.35  2001/08/09 15:58:59  mbickel
//      Some usability improvements in the map editor
//      More flexible BI3 map import
//      Better textfile error messages
//
//     Revision 1.34  2001/08/09 14:50:37  mbickel
//      Added palette.map to data directory
//      Improved usability of terrain selection in mapeditor
//      New terrain translation in bi3 import function
//      Better error messages in text parser
//      Better error message: duplicate ID
//
//     Revision 1.33  2001/08/02 15:33:01  mbickel
//      Completed text based file formats
//
//     Revision 1.32  2001/07/28 11:19:10  mbickel
//      Updated weaponguide
//      moved item repository from spfst to itemrepository
//
//     Revision 1.31  2001/05/24 15:37:51  mbickel
//      Fixed: reaction fire could not be disabled when unit out of ammo
//      Fixed several AI problems
//
//     Revision 1.30  2001/02/26 12:35:10  mbickel
//      Some major restructuing:
//       new message containers
//       events don't store pointers to units any more
//       tfield class overhauled
//
//     Revision 1.29  2001/02/11 11:39:32  mbickel
//      Some cleanup and documentation
//
//     Revision 1.28  2001/02/01 22:48:37  mbickel
//      rewrote the storing of units and buildings
//      Fixed bugs in bi3 map importing routines
//      Fixed bugs in AI
//      Fixed bugs in mapeditor
//
//     Revision 1.27  2001/01/31 14:52:35  mbickel
//      Fixed crashes in BI3 map importing routines
//      Rewrote memory consistency checking
//      Fileselect dialog now uses ASCStrings
//
//     Revision 1.26  2001/01/25 23:44:57  mbickel
//      Moved map displaying routins to own file (mapdisplay.cpp)
//      Wrote program to create pcx images from map files (map2pcx.cpp)
//      Fixed bug in repair function: too much resource consumption
//      AI improvements and bug fixes
//      The BI3 map import function now evaluates the player status (human/
//       computer)
//
//     Revision 1.25  2001/01/21 16:37:16  mbickel
//      Moved replay code to own file ( replay.cpp )
//      Fixed compile problems done by cleanup
//
//     Revision 1.24  2000/12/26 21:04:34  mbickel
//      Fixed: putimageprt not working (used for small map displaying)
//      Fixed: mapeditor crashed on generating large maps
//
//     Revision 1.23  2000/11/29 11:05:27  mbickel
//      Improved userinterface of the mapeditor
//      map::preferredfilenames uses now strings (instead of char*)
//
//     Revision 1.22  2000/11/29 09:40:19  mbickel
//      The mapeditor has now two maps simultaneously active
//      Moved memorychecking functions to its own file: memorycheck.cpp
//      Rewrote password handling in ASC
//
//     Revision 1.21  2000/10/18 14:14:06  mbickel
//      Rewrote Event handling; DOS and WIN32 may be currently broken, will be
//       fixed soon.
//
//     Revision 1.20  2000/10/14 14:16:04  mbickel
//      Cleaned up includes
//      Added mapeditor to win32 watcom project
//
//     Revision 1.19  2000/10/11 14:26:30  mbickel
//      Modernized the internal structure of ASC:
//       - vehicles and buildings now derived from a common base class
//       - new resource class
//       - reorganized exceptions (errors.h)
//      Split some files:
//        typen -> typen, vehicletype, buildingtype, basecontainer
//        controls -> controls, viewcalculation
//        spfst -> spfst, mapalgorithm
//      bzlib is now statically linked and sources integrated
//
//     Revision 1.18  2000/08/12 12:52:46  mbickel
//      Made DOS-Version compile and run again.
//
//     Revision 1.17  2000/08/06 11:39:03  mbickel
//      New map paramter: fuel globally available
//      Mapeditor can now filter buildings too
//      Fixed unfreed memory in fullscreen image loading
//      Fixed: wasted cpu cycles in building
//      map parameters can be specified when starting a map
//      map parameters are reported to all players in multiplayer games
//
//     Revision 1.16  2000/08/02 15:52:55  mbickel
//      New unit set definition files
//      demount accepts now more than one container file
//      Unitset information dialog added
//
//     Revision 1.15  2000/07/31 18:02:53  mbickel
//      New configuration file handling
//      ASC searches its data files in all directories specified in ascrc
//      Renamed all tools so they begin with asc
//
//     Revision 1.14  2000/07/29 14:54:25  mbickel
//      plain text configuration file implemented
//
//     Revision 1.13  2000/06/28 19:26:15  mbickel
//      fixed bug in object generation by building removal
//      Added artint.cpp to makefiles
//      Some cleanup
//
//     Revision 1.12  2000/05/11 20:12:05  mbickel
//      mapedit(lin) can now import BI3 maps
//
//     Revision 1.11  2000/05/10 19:55:49  mbickel
//      Fixed empty loops when waiting for mouse events
//
//     Revision 1.10  2000/04/27 16:25:21  mbickel
//      Attack functions cleanup
//      New vehicle categories
//      Rewrote resource production in ASC resource mode
//      Improved mine system: several mines on a single field allowed
//      Added unitctrl.* : Interface for vehicle functions
//        currently movement and height change included
//      Changed timer to SDL_GetTicks
//
//     Revision 1.9  2000/04/01 11:38:37  mbickel
//      Updated the small editors
//      Added version numbering
//
//     Revision 1.8  2000/03/16 14:06:54  mbickel
//      Added unitset transformation to the mapeditor
//
//     Revision 1.7  2000/03/11 18:22:04  mbickel
//      Added support for multiple graphic sets
//
//     Revision 1.6  2000/02/02 20:48:34  mbickel
//      Fixed bug in BI3 path verification
//
//     Revision 1.5  2000/01/04 19:43:51  mbickel
//      Continued Linux port
//
//     Revision 1.4  1999/12/29 12:50:43  mbickel
//      Removed a fatal error message in GUI.CPP
//      Made some modifications to allow platform dependant path delimitters
//
//     Revision 1.3  1999/12/27 12:59:52  mbickel
//      new vehicle function: each weapon can now be set to not attack certain
//                            vehicles
//
//     Revision 1.2  1999/11/16 03:41:33  tmwilson
//     	Added CVS keywords to most of the files.
//     	Started porting the code to Linux (ifdef'ing the DOS specific stuff)
//     	Wrote replacement routines for kbhit/getch for Linux
//     	Cleaned up parts of the code that gcc barfed on (char vs unsigned char)
//     	Added autoconf/automake capabilities
//     	Added files used by 'automake --gnu'
//
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

#include <stdarg.h>
#include "global.h"
#include "vehicletype.h"
#include "buildingtype.h"
#include "edmisc.h"
#include "loadbi3.h"
#include "edevents.h"
#include "edgen.h"
#include "edselfnt.h"
#include "edglobal.h"
#include "gameoptions.h"
#include "mapdisplay.h"
#include "itemrepository.h"

mc_check mc;


   const char* execactionnames[execactionscount] = {
        "End MapEdit",
        "Help",
        "Goto EditMode",
        "Select terrain",
        "Select terrainALL",
        "Select unit",
        "Select color",
        "Select building",
        "Select special object",
        "Select mine",
        "Select weather",
        "Setup alliances",
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
        "Change GlobalDirection",
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
        "Place terrain",
        "Place Unit",
        "Place building",
        "Place special object",
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
        "specify unit production" };


// ıS Infomessage

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

//ıS MC_CHeck

void mc_check::on(void)
{
   if (mycursor.an == false ) {
      int ms = getmousestatus(); 
      if (ms == 1) { 		//mouse off
         if (mstatus == 0) mousevisible(true);
         else mstatus++;
      } else { 			//mouse on
         mstatus++;
      }
      if (cursor.an == false) {  //cursor off
         if (cstatus == 0) cursor.show();
         else cstatus++;
      } else { 			//cursor on
         cstatus++;
      } /* endif */
   }
}

void mc_check::off(void)
{
   if (mycursor.an == false ) {
      int ms = getmousestatus(); 
      if (ms == 1) { 		//mouse off
         mstatus--;
      } else { 			//mouse on
         if (mstatus == 0) mousevisible(false);
         else mstatus--;
      }
      if (cursor.an == false) {  	//cursor off
         cstatus--;
      } else { 			//cursor on
         if (cstatus == 0) cursor.hide();
         else cstatus--;
      } /* endif */
   }
}



class  GetString : public tdialogbox {
          public :
              int action;
              char* buf;
              void init(char* _title);
              virtual void run(void);
              virtual void buttonpressed(int id);
           };

void         GetString::init(char* _title)
{ 
   tdialogbox::init(); 
   title = _title; 
   x1 = 120; 
   xsize = 400; 
   y1 = 150; 
   ysize = 140; 
   action = 0; 

   windowstyle = windowstyle ^ dlg_in3d; 

   addbutton("~D~one",20,ysize - 40,100,ysize - 20,0,1,1,true); 
   addkey(1,ct_enter); 
   addbutton("~C~ancel",120,ysize - 40,200,ysize - 20,0,1,2,true); 
   addkey(2, ct_esc );

   addbutton("",20,60,xsize - 20,80,1,1,3,true); 
   addeingabe(3,buf,0,1000);

   buildgraphics(); 

   mousevisible(true); 
} 


void         GetString::run(void)
{ 
   if ( pcgo ) {
      delete pcgo;
      pcgo = NULL;
   }
   pbutton pb = firstbutton;
   while ( pb &&  (pb->id != 3)) 
      pb = pb->next;

   if ( pb )
      if ( pb->id == 3 )
         execbutton( pb , false );

   do { 
      tdialogbox::run(); 
   }  while ( !action ); 
} 


void         GetString::buttonpressed(int         id)
{ 
   tdialogbox::buttonpressed(id); 
   switch (id) {
      
      case 1:   
      case 2:   action = id; 
   break; 
   } 
} 


char*    getstring( char*  title, char* orgval )

{ 
   GetString     gi;

   gi.buf = orgval;
   gi.init( title );
   gi.run();
   gi.done();
   if ( gi.action == 2 )
      return NULL;
   else
      return gi.buf;
} 

char* getbipath ( void )
{
   char filename[1000];
   if ( getbi3path() )
      strcpy ( filename, getbi3path() );
   else
      filename[0] = 0;

   char filename2[1000];
   strcpy ( filename2, filename );
   appendbackslash( filename2 );
   strcat ( filename2, "mis");
   strcat ( filename2, pathdelimitterstring );
   strcat ( filename2, "*.dat");

   int cnt = 0;

   while ( !exist ( filename2 )) {
      char* res = getstring("enter Battle Isle path", filename );
      if ( res == NULL )
         return NULL;

      CGameOptions::Instance()->setChanged ( 1 );

      strcpy ( filename2, filename );
      appendbackslash( filename2 );
      strcat ( filename2, "mis");
      strcat ( filename2, pathdelimitterstring );
      strcat ( filename2, "*.dat");
      cnt++;
      #if CASE_SENSITIVE_FILE_NAMES == 1
      if (!exist ( filename2 ) && cnt == 1 )
         displaymessage("The 'mis' and 'ger' / 'eng' directories must be lower case to import files from them !", 1 );
      #endif
   }
   appendbackslash( filename );
   char* buf = strdup ( filename );
   CGameOptions::Instance()->bi3.dir.setName( filename );

   return buf;
}

// ıS ExecAction

void execaction(int code)
{
   switch(code) {
    case act_help :   if ( polyfieldmode ) help ( 1040 );
                       else help(1000);
       break;
    case act_selbodentypAll : {
                        ch = 0;
                        cursor.hide();
                        selterraintype( ct_f3 );
                        cursor.show();
                     }
       break;
    case act_selunit : {
                       ch = 0;
                       cursor.hide();
                       selvehicletype( ct_f4 );
                       cursor.show();
                     }
       break;
    case act_selcolor : {
                       ch = 0;
                       cursor.hide();
                       selcolor( ct_f5 );
                       cursor.show();
                     }
       break;
    case act_selbuilding : {
                       ch = 0;
                       cursor.hide();
                       selbuilding( ct_f6);
                       cursor.show();
                     }
       break;
    case act_selobject : {
                       ch = 0;
                       cursor.hide();
                       selobject( ct_f7 );
                       cursor.show();
                     }
       break;
    case act_selmine : {
                       ch = 0;
                       cursor.hide();
                       selmine( ct_f8 );
                       cursor.show();
                     }
       break;
    case act_selweather : {
                       ch = 0;
                       cursor.hide();
                       selweather( ct_f9  );   // !!!!!         // Test (Boolean) Testet, ob das wetter auch verfÅgbar ist fÅr bodentyp
                       cursor.show();                           // True : WIRD getestet / false : kein Test
                     }
       break;
    case act_setupalliances :  setupalliances();
       break;
    case act_toggleresourcemode :  {
                      if (showresources < 2) showresources++;
                      else showresources = 0;
                      displaymap();
       }
       break;
    case act_changeglobaldir : {
                                  auswahld++;
                                  if (auswahld > sidenum-1) auswahld = 0;
                                 //// if ( selectnr > cselcolor) selectnr = cselbodentyp; ???????
                                  showallchoices();
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
    case act_maptopcx : writemaptopcx ();  
       break;
    case act_loadmap :   {
                            if (mapsaved == false )
                               if (choice_dlg("Map not saved ! Save now ?","~y~es","~n~o") == 1) 
                                  k_savemap(false);

                             k_loadmap();
 
                             // actmap->player[8].firstvehicle = NULL;
 
                             pdbaroff(); 
                          } 
       break;
    case act_changeplayers : playerchange();
       break;
    case act_newmap :   newmap();
       break;
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
    case act_repaintdisplay :   repaintdisplay();
       break;
    case act_unitinfo :  vehicle_information();
       break;
    case act_viewmap :  
             {
             while (mouseparams.taste != 0)
                releasetimeslice();
             cursor.hide();
             showmap ();
             displaymap();
             cursor.show();
             }
       break;
    case act_changeunitdir : {
                      pf2 = getactfield();
                      if ( (pf2 != NULL) && (pf2->vehicle != NULL ) ){
                         pf2->vehicle->direction++;
                         if (pf2->vehicle->direction>sidenum-1) pf2->vehicle->direction = 0;
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
                         }
       break;
    case act_changecargo :   {
                 cursor.hide(); 
                 if ( getactfield()->building )                    
                    building_cargo( getactfield()->building );
                 else 
                    if ( getactfield()->vehicle )
                       unit_cargo( getactfield()->vehicle );
                 cursor.show(); 
              }
       break;
    case act_changeterraindir : {
                      pf2 = getactfield();
                      if (pf2 != NULL) {
                         pf2->direction++;
                         if (pf2->direction>sidenum-1) pf2->direction = 0;
                         mapsaved = false;
                         displaymap();
                      } 
                   }
       break;
    case act_events :   event();
       break;
    case act_fillmode :   if ( polyfieldmode == false ) {   
                 if (tfill == true) tfill = false;
                 else tfill = true; 
                 fillx1 = cursor.posx + actmap->xpos; 
                 filly1 = cursor.posy + actmap->ypos; 
                 pdbaroff(); 
              } 
       break;
    case act_mapgenerator : mapgenerator();
       break;
    case act_setactivefieldvals : {
                  pfield fld = getactfield();

                  if ( fld->vehicle ) {
                     auswahlf = getvehicletype_forid ( fld->vehicle->typ->id );
                     altefarbwahl = farbwahl;
                     farbwahl = fld->vehicle->color/8;
                     lastselectiontype = cselunit;
                     setnewvehicleselection ( auswahlf );
                  } else
                  if ( fld->building ) {
                     auswahlb = fld->building->typ;
                     altefarbwahl = farbwahl;
                     farbwahl = fld->building->color/8;
                     lastselectiontype = cselbuilding;
                     setnewbuildingselection ( auswahlb );
                  } else
                  if ( !fld->objects.empty() ) {
                     actobject = fld->objects.begin()->typ ;
                     lastselectiontype = cselobject;
                     setnewobjectselection ( actobject );
                  } else {
                     auswahld = fld->direction;
                     auswahl = fld->typ->terraintype;
                     lastselectiontype = cselbodentyp;
                     setnewterrainselection ( auswahl );
                  }
                  showallchoices();
               }
       break;
       
    case act_deletething : {
                         pf2 = getactfield();
                         mapsaved = false;
                         if (pf2 != NULL) {
                            if (pf2->vehicle != NULL)
                               delete pf2->vehicle;
                            else
                               if (pf2->building != NULL)
                                  delete pf2->building;
                               else {
                                  pf2->removeobject( NULL );
                                  pf2->removemine( -1 );
                                }
                            mapsaved = false;
                            displaymap();
                         }
                      } 
        break;
    case act_deleteunit : {
                         pf2 = getactfield();
                         if (pf2 != NULL)
                            if (pf2->vehicle != NULL) {
                               delete pf2->vehicle;
                               mapsaved = false;
                               displaymap();
                            }
                         }
        break;
     case act_deletebuilding : {
                         pf2 = getactfield();
                         if (pf2 != NULL) 
                            if (pf2->building != NULL) { 
                               delete pf2->building;
                               mapsaved = false;
                               displaymap();
                            }
                      }
        break;
     case act_deleteobject : {
                         pf2 = getactfield();
                         if ( pf2 ) {
                            mapsaved = false;
                            pf2->removeobject( actobject );
                            displaymap();
                         }
                      }
        break;
     case act_deletetopmostobject : {
                         pf2 = getactfield();
                         if ( pf2 ) {
                            mapsaved = false;
                            pf2->removeobject( NULL );
                            displaymap();
                         }
                      }
        break;
     case act_deleteallobjects : {
                         pf2 = getactfield();
                         if ( pf2 ) {
                            mapsaved = false;
                            pf2->objects.clear( );
                            calculateallobjects();
                            displaymap();
                         }
                      }
        break;
     case act_deletemine : {
                         pf2 = getactfield();
                         if (pf2 != NULL) {
                            mapsaved = false;
                            pf2->removemine( -1 );
                            displaymap();
                         }
                      }
        break;
    case act_showpalette : showpalette();
       break;
    case act_changeminestrength : changeminestrength();
       break;
    case act_changemapvals :   changemapvalues();
       break;                                        
    case act_changeproduction :   if ( getactfield()->building ) building_production( getactfield()->building );
       break;
    case act_savemap :  k_savemap(false);
       break;
    case act_savemapas :  k_savemap(true);
       break;
    case act_changeunitvals :   {
                 pf2 = getactfield();
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
    case act_placebodentyp : placebodentyp();
       break;
    case act_placeunit : placeunit();
       break;
    case act_placebuilding : placebuilding(farbwahl,auswahlb,true);
       break;
    case act_placeobject : placeobject();
       break;
    case act_placemine : placemine();
       break;
    case act_placething : putactthing();
       break;
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
    case act_end : {
       if ( mapSwitcher.getDefaultAction() == MapSwitcher::select ) {
          execaction(act_switchmaps);
          ch = ct_invvalue;
       } else
          if (choice_dlg("Do you really want to quit ?","~y~es","~n~o") == 2) ch = ct_invvalue;
             else {
                ch = ct_esc; //Exit MapEdit
                if (mapsaved == false )
                   if (choice_dlg("Map not saved ! Save now ?","~y~es","~n~o") == 1) k_savemap(false);
               }
          }
       break;
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
         char filename2[260];
         char* path = getbipath();
         if ( !path )
            break;
            
         strcpy ( filename2, path );
         strcat ( filename2, "mis");
         strcat ( filename2, pathdelimitterstring );
         strcat ( filename2, "*.dat");

         ASCString filename;
         fileselectsvga ( filename2, filename, true );
         if ( !filename.empty() ) {
            strcpy ( filename2, path );
            strcat ( filename2, "mis");
            strcat ( filename2, pathdelimitterstring );
            strcat ( filename2, filename.c_str());
            TerrainType::Weather* t = auswahl->weather[auswahlw];
            if ( !t )
               t = auswahl->weather[0];
            importbattleislemap ( path, filename.c_str(), t );
            displaymap();
         }
      }
      break;
   case act_insert_bi_map : {
         char filename2[260];

         char* path = getbipath();
         if ( !path )
            break;
            
         strcpy ( filename2, path );
         strcat ( filename2, "mis");
         strcat ( filename2, pathdelimitterstring );
         strcat ( filename2, "*.dat");

         ASCString filename;
         fileselectsvga ( filename2, filename, true );
         if ( !filename.empty() ) {
            strcpy ( filename2, path );
            strcat ( filename2, "mis");
            strcat ( filename2, pathdelimitterstring );
            strcat ( filename2, filename.c_str());
            insertbattleislemap ( getxpos(), getypos(), path, filename.c_str() );
            displaymap();
         }
      }
      break;
   case act_resizemap : cursor.hide();
                        resizemap();
                        cursor.checkposition ( actmap->xpos + cursor.posx, actmap->ypos + cursor.posy );
                        cursor.show();
      break;
   case act_movebuilding: movebuilding();
      break;
   case act_setactweatherglobal: setweatherall ( auswahlw, 1 );
                                 displaymap();
      break;
   case act_setmapparameters: setmapparameters();
      break;
   case act_terraininfo: viewterraininfo();
      break;
   case act_setunitfilter: selectunitsetfilter();
      break;
   case act_selectgraphicset: selectgraphicset();
                              showallchoices();
      break;
  #ifdef FREEMAPZOOM    
   case act_setzoom : choosezoomlevel();
      break;
  #endif
   case act_unitsettransformation: unitsettransformation();
      break;
   case act_unitSetInformation: viewUnitSetinfo();
      break;
   case act_selbodentyp: if ( mapSwitcher.getDefaultAction() == MapSwitcher::select ) {
                            execaction ( act_setactivefieldvals );
                            /*
                            auswahl = getactfield()->typ->terraintype;
                            setnewterrainselection ( auswahl );
                            showallchoices();
                            */
                         } else
                            lastselectiontype = cselbodentyp;
                         execaction(act_switchmaps);
                         break;
   case act_switchmaps: mapSwitcher.toggle();
                        displaymap();
                        showStatusBar();
                        showallchoices();
      break;
   case act_transformMap: transformMap();
      break;
   case act_editArchivalInformation: editArchivalInformation();
      break;
   case act_displayResourceComparison : resourceComparison();
      break;
   case act_specifyunitproduction: unitProductionLimitation();
      break;
    }
}

