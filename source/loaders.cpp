/*! \file loaders.cpp
    \brief procedure for loading and writing savegames, maps etc.

    IO for basic types like vehicletype, buildingtype etc which are also used by the small editors are found in sgstream

*/

//     $Id: loaders.cpp,v 1.77 2003-01-12 19:37:18 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.76  2002/12/20 16:40:58  mbickel
//      Fixed problems with inheritance of asctxt files
//      Changed the displaying of resources on map
//
//     Revision 1.75  2002/11/24 10:54:19  mbickel
//      made field allocation more robust
//
//     Revision 1.74  2002/11/23 18:36:24  mbickel
//      Fixed crash when deleting map that contained rubble generating buildings
//
//     Revision 1.73  2002/11/07 18:42:57  mbickel
//      Fixed crash in reaction fire
//      Improved autoborder object netification
//      Fixed crash when mail game was loaded by command line parameters
//
//     Revision 1.72  2002/10/12 17:28:04  mbickel
//      Fixed "enemy unit loaded" bug.
//      Changed map format
//      Improved log messages
//
//     Revision 1.71  2002/10/09 16:58:46  mbickel
//      Fixed to GrafikSet loading
//      New item filter for mapeditor
//
//     Revision 1.70  2002/09/19 20:20:05  mbickel
//      Cleanup and various bug fixes
//
//     Revision 1.69  2002/05/07 21:32:49  mbickel
//      Fixed crash in mapeditor
//      Fixed: conquering of allied buildings
//      Fixed: ambiguity in pulldown menu keys
//
//     Revision 1.68  2002/03/18 21:42:17  mbickel
//      Some cleanup and documentation in the Mine class
//      The number of mines is now displayed in the field information window
//
//     Revision 1.67  2002/01/07 11:40:40  mbickel
//      Fixed some syntax errors
//
//     Revision 1.66  2001/12/19 17:16:29  mbickel
//      Some include file cleanups
//
//     Revision 1.65  2001/12/14 10:20:05  mbickel
//      Cleanup and enhancements to configure.in
//      Removed last remains of octagonal version from source files
//
//     Revision 1.64  2001/10/11 10:41:06  mbickel
//      Restructured platform fileio handling
//      Added map archival information to mapeditor
//
//     Revision 1.63  2001/10/02 14:06:28  mbickel
//      Some cleanup and documentation
//      Bi3 import tables now stored in .asctxt files
//      Added ability to choose amoung different BI3 import tables
//      Added map transformation tables
//
//     Revision 1.62  2001/09/13 17:43:12  mbickel
//      Many, many bug fixes
//
//     Revision 1.61  2001/08/19 12:50:03  mbickel
//      fixed event trigger allenemybuildings
//
//     Revision 1.60  2001/08/19 12:31:26  mbickel
//      Fixed several bugs in event and campaign handling
//
//     Revision 1.59  2001/08/15 13:47:51  mbickel
//      Fixed crash when loading old vehicletypes
//
//     Revision 1.58  2001/08/09 19:28:22  mbickel
//      Started adding buildingtype text file functions
//
//     Revision 1.57  2001/08/06 20:54:43  mbickel
//      Fixed lots of crashes related to the new text files
//      Fixed delayed events
//      Fixed crash in terrin change event
//      Fixed visibility of mines
//      Fixed crashes in event loader
//
//     Revision 1.56  2001/07/28 11:19:12  mbickel
//      Updated weaponguide
//      moved item repository from spfst to itemrepository
//
//     Revision 1.55  2001/07/27 21:13:35  mbickel
//      Added text based file formats
//      Terraintype and Objecttype restructured
//
//     Revision 1.54  2001/07/18 16:05:47  mbickel
//      Fixed: infinitive loop in displaying "player exterminated" msg
//      Fixed: construction of units by units: wrong player
//      Fixed: loading bug of maps with mines
//      Fixed: invalid map parameter
//      Fixed bug in game param edit dialog
//      Fixed: cannot attack after declaring of war
//      New: ffading of sounds
//
//     Revision 1.53  2001/07/11 20:13:27  mbickel
//      Fixed crash when exception occures during game loading
//      Fixed wrong sender of mails
//
//     Revision 1.52  2001/07/09 12:08:40  mbickel
//      Fixed: crash when saving game
//
//     Revision 1.51  2001/05/21 12:46:19  mbickel
//      Fixed infinite loop in AI::strategy
//      Fixed bugs in mapeditor - event editing
//      Fixed bugs in even loading / writing
//      Fixed wrong build order AI <-> main program
//
//     Revision 1.50  2001/05/16 23:21:01  mbickel
//      The data file is mounted using automake
//      Added sgml documentation
//      Added command line parsing functionality;
//        integrated it into autoconf/automake
//      Replaced command line parsing of ASC and ASCmapedit
//
//     Revision 1.49  2001/04/03 11:54:16  mbickel
//      AI Improvements: production , servicing
//
//     Revision 1.48  2001/03/30 12:43:16  mbickel
//      Added 3D pathfinding
//      some cleanup and documentation
//      splitted the ai into several files, now located in the ai subdirectory
//      AI cares about airplane servicing and range constraints
//
//     Revision 1.47  2001/03/23 16:02:56  mbickel
//      Some restructuring;
//      started rewriting event system
//
//     Revision 1.46  2001/02/26 13:49:36  mbickel
//      Fixed bug in message loading
//      readString can now read strings that container \n
//
//     Revision 1.45  2001/02/26 12:35:17  mbickel
//      Some major restructuing:
//       new message containers
//       events don't store pointers to units any more
//       tfield class overhauled
//
//     Revision 1.44  2001/02/18 15:37:14  mbickel
//      Some cleanup and documentation
//      Restructured: vehicle and building classes into separate files
//         tmap, tfield and helper classes into separate file (gamemap.h)
//      basestrm : stream mode now specified by enum instead of int
//
//     Revision 1.43  2001/02/11 11:39:37  mbickel
//      Some cleanup and documentation
//
//     Revision 1.42  2001/02/04 21:26:57  mbickel
//      The AI status is written to savegames -> new savegame revision
//      Lots of bug fixes
//
//     Revision 1.41  2001/02/01 22:48:43  mbickel
//      rewrote the storing of units and buildings
//      Fixed bugs in bi3 map importing routines
//      Fixed bugs in AI
//      Fixed bugs in mapeditor
//
//     Revision 1.40  2001/01/31 14:52:39  mbickel
//      Fixed crashes in BI3 map importing routines
//      Rewrote memory consistency checking
//      Fileselect dialog now uses ASCStrings
//
//     Revision 1.39  2001/01/23 21:05:18  mbickel
//      Speed up of AI
//      Lot of bugfixes in AI
//      Moved Research to own files (research.*)
//      Rewrote storing of developed technologies
//      Some cleanup and documentation
//
//     Revision 1.38  2001/01/22 20:00:09  mbickel
//      Fixed bug that made savegamefrom campaign games unloadable
//      Optimized the terrainAccess-checking
//
//     Revision 1.37  2001/01/04 15:13:59  mbickel
//      configure now checks for libSDL_image
//      AI only conquers building that cannot be conquered back immediately
//      tfindfile now returns strings instead of char*
//
//     Revision 1.36  2000/11/29 11:05:29  mbickel
//      Improved userinterface of the mapeditor
//      map::preferredfilenames uses now strings (instead of char*)
//
//     Revision 1.35  2000/11/26 22:18:53  mbickel
//      Added command line parameters for setting the verbosity
//      Increased verbose output
//
//     Revision 1.34  2000/11/11 11:05:18  mbickel
//      started AI service functions
//
//     Revision 1.33  2000/11/08 19:31:09  mbickel
//      Rewrote IO for the tmap structure
//      Fixed crash when entering damaged building
//      Fixed crash in AI
//      Removed item CRCs
//
//     Revision 1.32  2000/10/26 18:14:57  mbickel
//      AI moves damaged units to repair
//      tmap is not memory layout sensitive any more
//
//     Revision 1.31  2000/10/18 14:14:14  mbickel
//      Rewrote Event handling; DOS and WIN32 may be currently broken, will be
//       fixed soon.
//
//     Revision 1.30  2000/10/14 13:07:00  mbickel
//      Moved DOS version into own subdirectories
//      Win32 version with Watcom compiles and links ! But doesn't run yet...
//
//     Revision 1.29  2000/10/14 10:52:51  mbickel
//      Some adjustments for a Win32 port
//
//     Revision 1.reactionfire ( this )28  2000/10/11 14:26:42  mbickel
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
//     Revision 1.27  2000/09/07 15:49:43  mbickel
//      some cleanup and documentation
//
//     Revision 1.26  2000/09/02 15:36:49  mbickel
//      Some minor cleanup and documentation
//
//     Revision 1.25  2000/08/28 15:58:58  mbickel
//      Fixed short displaying of map before password dialog when loading email
//        games by command line parameter
//      Fixed reaction fire not working when loading an old ( < ASC1.2.0 ) file
//
//     Revision 1.24  2000/08/21 17:50:59  mbickel
//      Fixed: crash when unit reaching max experience
//      Fixed: crash when displaying research image
//      Fixed: crash when events referenced a unit that has been shot down
//      Fixed: screenshot being written to wrong directory
//
//     Revision 1.23  2000/08/12 12:52:48  mbickel
//      Made DOS-Version compile and run again.
//
//     Revision 1.22  2000/08/11 12:24:03  mbickel
//      Fixed: no movement after refuelling unit
//      Restructured reading/writing of units
//
//     Revision 1.21  2000/08/07 16:29:21  mbickel
//      orbiting units don't consume fuel any more
//      Fixed bug in attack formula; improved attack formula
//      Rewrote reactionfire
//
//     Revision 1.20  2000/08/05 13:38:26  mbickel
//      Rewrote height checking for moving units in and out of
//        transports / building
//
//     Revision 1.19  2000/08/04 15:11:12  mbickel
//      Moving transports costs movement for units inside
//      refuelled vehicles now have full movement in the same turn
//      terrain: negative attack / defensebonus allowed
//      new mapparameters that affect damaging and repairing of building
//
//     Revision 1.18  2000/08/03 19:45:15  mbickel
//      Fixed some bugs in DOS code
//      Removed submarine.ascent != 0 hack
//
//     Revision 1.17  2000/08/03 13:12:15  mbickel
//      Fixed: on/off switching of generator vehicle produced endless amounts of energy
//      Repairing units now reduces their experience
//      negative attack- and defenseboni possible
//      changed attackformula
//
//     Revision 1.16  2000/07/29 14:54:37  mbickel
//      plain text configuration file implemented
//
//     Revision 1.15  2000/07/26 15:58:10  mbickel
//      Fixed: infinite loop when landing with an aircraft which is low on fuel
//      Fixed a bug in loadgame
//
//     Revision 1.14  2000/07/16 14:20:03  mbickel
//      AI has now some primitive tactics implemented
//      Some clean up
//        moved weapon functions to attack.cpp
//      Mount doesn't modify PCX files any more.
//
//     Revision 1.13  2000/06/28 19:26:16  mbickel
//      fixed bug in object generation by building removal
//      Added artint.cpp to makefiles
//      Some cleanup
//
//     Revision 1.12  2000/06/28 18:31:00  mbickel
//      Started working on AI
//      Started making loaders independent of memory layout
//      Destroyed buildings can now leave objects behind.
//
//     Revision 1.11  2000/05/25 11:07:44  mbickel
//      Added functions to check files for valid mail / savegame files.
//
//     Revision 1.10  2000/05/06 19:57:09  mbickel
//      Mapeditor/linux is now running
//
//     Revision 1.9  2000/04/27 16:25:24  mbickel
//      Attack functions cleanup
//      New vehicle categories
//      Rewrote resource production in ASC resource mode
//      Improved mine system: several mines on a single field allowed
//      Added unitctrl.* : Interface for vehicle functions
//        currently movement and height change included
//      Changed timer to SDL_GetTicks
//
//     Revision 1.8  2000/01/25 19:28:14  mbickel
//      Fixed bugs:
//        invalid mouse buttons reported when moving the mouse
//        missing service weapon in weapon information
//        invalid text displayed in ammo production
//        undamaged units selected in repair vehicle function
//
//      speed up when playing weapon sound
//
//     Revision 1.7  2000/01/24 17:35:45  mbickel
//      Added dummy routines for sound under DOS
//      Cleaned up weapon specification
//
//     Revision 1.6  1999/12/28 21:03:03  mbickel
//      Continued Linux port
//      Added KDevelop project files
//
//     Revision 1.5  1999/12/14 20:23:56  mbickel
//      getfiletime now works on containerfiles too
//      improved BI3 map import tables
//      various bugfixes
//
//     Revision 1.4  1999/11/23 21:07:31  mbickel
//      Many small bugfixes
//
//     Revision 1.3  1999/11/22 18:27:34  mbickel
//      Restructured graphics engine:
//        VESA now only for DOS
//        BASEGFX should be platform independant
//        new interface for initialization
//      Rewrote all ASM code in C++, but it is still available for the Watcom
//        versions
//      Fixed bugs in RLE decompression, BI map importer and the view calculation
//
//     Revision 1.2  1999/11/16 03:41:57  tmwilson
//      Added CVS keywords to most of the files.
//      Started porting the code to Linux (ifdef'ing the DOS specific stuff)
//      Wrote replacement routines for kbhit/getch for Linux
//      Cleaned up parts of the code that gcc barfed on (char vs unsigned char)
//      Added autoconf/automake capabilities
//      Added files used by 'automake --gnu'
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

#include <algorithm>
#include <cstring>
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>

#include "typen.h"
#include "misc.h"
#include "basegfx.h"
#include "newfont.h"
#include "buildingtype.h"
#include "vehicletype.h"
#include "spfst.h"
#include "loaders.h"
#include "dlg_box.h"
#include "dlg_box.h"
#include "dialog.h"
#include "sgstream.h"
#include "sg.h"
#include "attack.h"
#include "errors.h"
#include "networkdata.h"
#include "strtmesg.h"
#include "textfileparser.h"
#include "itemrepository.h"

#ifdef sgmain
#include "missions.h"
#include "ai/ai.h"
#endif


const word fileterminator = 0xa01a;
ticons icons;


 const char* savegameextension = "*.sav";
 const char* mapextension = "*.map";
 const char* tournamentextension = "*.asc";



void         seteventtriggers( pmap actmap )
{ 
  pevent       event;

  for ( int l = 0; l < 2; l++ ) {
      if ( l == 0 )
         event = actmap->firsteventtocome; 
      else
         event = actmap->firsteventpassed; 

      while ( event ) { 
         for ( int j = 0; j <= 3; j++) { 
            if ((event->trigger[j] == ceventt_buildingconquered) || 
                (event->trigger[j] == ceventt_buildinglost) || 
                (event->trigger[j] == ceventt_buildingdestroyed) ||
                (event->trigger[j] == ceventt_building_seen )) {
                 
               int xpos = event->trigger_data[j]->xpos; 
               int ypos = event->trigger_data[j]->ypos; 
               if ( xpos != -1  &&  ypos != -1  &&  event->triggerstatus[j] != 2 && actmap->getField(xpos,ypos)->building ) {
                  pbuilding building = actmap->getField(xpos,ypos)->building;
                  event->trigger_data[j]->building = building;
                  if ((event->trigger[j] == ceventt_buildingconquered))
                     building->connection |= cconnection_conquer;
                  if ((event->trigger[j] == ceventt_buildinglost))
                     building->connection |= cconnection_lose;
                  if ((event->trigger[j] == ceventt_buildingdestroyed))
                     building->connection |= cconnection_destroy;
                  if ((event->trigger[j] == ceventt_building_seen ))
                     building->connection |= cconnection_seen;
                } else
                  event->trigger_data[j]->building = NULL;

               event->trigger_data[j]->xpos = -1;
               event->trigger_data[j]->ypos = -1;
            }
            if ((event->trigger[j] == ceventt_unitconquered) ||
                (event->trigger[j] == ceventt_unitlost) ||
                (event->trigger[j] == ceventt_unitdestroyed)) {
               int xpos = event->trigger_data[j]->xpos;
               int ypos = event->trigger_data[j]->ypos;
               if ( xpos != -1 && ypos != -1  && event->triggerstatus[j] != 2 ) {
                  pvehicle vehicle;
                  if ( event->trigger_data[j]->networkid != -1 )
                     vehicle = actmap->getUnit ( xpos, ypos, event->trigger_data[j]->networkid );
                  else
                     vehicle = actmap->getField(xpos,ypos)->vehicle;

                  event->trigger_data[j]->networkid = vehicle->networkid;
                  if ((event->trigger[j] == ceventt_unitconquered)) 
                     vehicle->connection |= cconnection_conquer;
                  if ((event->trigger[j] == ceventt_unitlost)) 
                     vehicle->connection |= cconnection_lose;
                  if ((event->trigger[j] == ceventt_unitdestroyed)) 
                     vehicle->connection |= cconnection_destroy;
               } else
                  event->trigger_data[j]->networkid = 0;

               event->trigger_data[j]->xpos = -1;
               event->trigger_data[j]->ypos = -1;
            }
	         displayLogMessage ( 10, "3 ");
            if ((event->trigger[j] == ceventt_event)) {
              // int id = event->trigger_data[j]->id;
               pevent event1 = actmap->firsteventtocome; 
               if ( !event->trigger_data[j]->mapid )
                  while (event1 != NULL) { 
                     if (event1->id == event->id) 
                        event1->conn = 1; 
                     event1 = event1->next; 
                  } 
              /* event1 = actmap->firsteventpassed; 
               while (event1 != NULL) { 
                  if (event1->id == id) 
                     event->triggerdata[j].event = event1; 
                  event1 = event1->next;
               }*/
            }
           displayLogMessage ( 10, "4 ");
           if (event->trigger[j] == ceventt_any_unit_enters_polygon ||
               event->trigger[j] == ceventt_specific_unit_enters_polygon) {

	            displayLogMessage ( 10, "6 ");
               if ( event->trigger_data[j]->unitpolygon->vehiclenetworkid ) {
                  Vehicle* v = actmap->getUnit ( event->trigger_data[j]->unitpolygon->vehiclenetworkid );
                  if ( v )
                     v->connection |= cconnection_areaentered_specificunit;
               }
	            displayLogMessage ( 10, "7 ");
              #ifndef karteneditor
               if ( event->trigger[j] == ceventt_any_unit_enters_polygon ) {
                  displayLogMessage ( 10, "7-1 ");
                  mark_polygon_fields_with_connection ( actmap, event->trigger_data[j]->unitpolygon->data, cconnection_areaentered_anyunit );
               } else {
                  displayLogMessage ( 10, "7-2 ");
                  mark_polygon_fields_with_connection ( actmap, event->trigger_data[j]->unitpolygon->data, cconnection_areaentered_specificunit );
               }
              #endif
               displayLogMessage ( 10, "8 ");
           }
         }
         event = event->next; 
      } 
   }
   for ( int i = 0; i < 8; i++ )
      actmap->player[i].queuedEvents = 1;

} 



  #define csm_typid32 1      /*  b1  */
  #define csm_direction 2    /*  b1  */ 
  #define csm_vehicle 4      /*  b1  */
  #define csm_building 8     /*  b1  */
  #define csm_height 32      /*  b1  */
  #define csm_cnt2 64        /*  b1  */
  #define csm_b3 128         /*  b1  */

  #define csm_material 1     /*  b3  */
  #define csm_fuel 2         /*  b3  */
  #define csm_visible 4      /*  b3  */
//  #define csm_mine 8         /*  b3  */
  #define csm_weather 16     /*  b3  */
//  #define csm_fahrspur 32    /*  b3  */
  #define csm_object 64      /*  b3  */
  #define csm_b4 128         /* b3 */ 

  #define csm_resources  1     /* b4 */
  #define csm_connection 2     // b4
  #define csm_newobject  4     // b4





/**************************************************************/
/*     einzelnes Event schreiben / lesen                    ÿ */
/**************************************************************/

int eventversion = 2;

void   tspfldloaders::writeevent ( pevent event )
{
   int magic = -1;
   stream->writeInt ( magic );
   stream->writeInt ( eventversion );

   stream->writedata2( *event );

/*
   stream->writeInt ( event->id );
   stream->writeChar ( event->player );
   stream->writeSting ( event->description );
   if ( event->intdata )
      stream->writeInt ( 1 );
   else
      stream->writeInt ( 0 );

   stream->writeInt ( event->datasize );
   stream->writeInt ( event->next ? 1 : 0 );
   stream->writeInt ( event->conn );
   for ( int i = 0; i < 4; i++ )
      stream->writeWord ( event->trigger[i] );

   for ( int i = 0; i < 4; i++ )
      stream->writeInt ( event->trigger_data ? 1 : 0 );

   for ( int i = 0; i < 4; i++ )
      stream->writeChar ( event->triggerconnect[i] );

   for ( int i = 0; i < 4; i++ )
      stream->writeChar ( event->triggerstatus[i] );

   stream->writeInt ( event->triggertime.abs );

   stream->writeInt ( event->delayedexecution.turn );
   stream->writeInt ( event->delayedexecution.move );
*/

   if ( event->datasize  &&  event->rawdata )
      stream->writedata( event->rawdata, event->datasize);
   
   for (char j = 0; j <= 3; j++) {
      if ((event->trigger[j] == ceventt_buildingconquered) ||
           (event->trigger[j] == ceventt_buildinglost) || 
           (event->trigger[j] == ceventt_buildingdestroyed) || 
           (event->trigger[j] == ceventt_building_seen )) { 
             if ( event->triggerstatus[j] != 2 ) {
                stream->writeWord( event->trigger_data[j]->building->getEntry().x );
                stream->writeWord( event->trigger_data[j]->building->getEntry().y );
             } else {
                integer w = -1;
                stream->writedata2( w );
                stream->writedata2( w );
             }
       } 
       if ((event->trigger[j] == ceventt_unitconquered) || 
           (event->trigger[j] == ceventt_unitlost) || 
           (event->trigger[j] == ceventt_unitdestroyed)) { 
           int xp;
           int yp;
           int nwid;
           if ( event->triggerstatus[j] == 2 ) {
              xp = -1;
              yp = -1;
              nwid = -1;
           } else {
              nwid = event->trigger_data[j]->networkid;
              pvehicle v = actmap->getUnit ( nwid );
              xp = v->xpos;
              yp = v->ypos;
           }
           stream->writedata2( xp );
           stream->writedata2( yp );
           stream->writedata2( nwid );
       } 
       if ((event->trigger[j] == ceventt_event) ||
           (event->trigger[j] == ceventt_technologyresearched) ||
           (event->trigger[j] == ceventt_allenemyunitsdestroyed ) ||
           (event->trigger[j] == ceventt_allenemybuildingsdestroyed )) {
          stream->writedata2( event->trigger_data[j]->id );
       } 
       if (event->trigger[j] == ceventt_turn ) {
          stream->writedata2( event->trigger_data[j]->time.abstime );
       }
       if (event->trigger[j] == ceventt_any_unit_enters_polygon || 
           event->trigger[j] == ceventt_specific_unit_enters_polygon) {
              stream->writedata2( *event->trigger_data[j]->unitpolygon );
              int sz = event->trigger_data[j]->unitpolygon->size - sizeof ( *event->trigger_data[j]->unitpolygon );
              stream->writedata( event->trigger_data[j]->unitpolygon->data, sz );
              if ( event->trigger_data[j]->unitpolygon->vehiclenetworkid ) {
                 int nwid = event->trigger_data[j]->unitpolygon->vehiclenetworkid;
                 pvehicle v = actmap->getUnit ( nwid );
                 if ( v ) {
                    stream->writeInt( v->xpos );
                    stream->writeInt( v->ypos );
                 } else {
                    stream->writeInt( -1 );
                    stream->writeInt( -1 );
                 }
                 stream->writedata2( nwid );
              }
              
       }
       
    } 

}



void    tspfldloaders::readevent ( pevent& event1 )
{
     int magic;
     int version;
     stream->readdata2( magic );
     if ( magic == -1 ) {
        stream->readdata2 ( version );
        stream->readdata2 ( *event1 );
     } else {
        memcpy ( event1, &magic, sizeof ( magic ));
        int* pi = (int*) event1;
        pi++;
        stream->readdata ( pi, sizeof ( *event1) - sizeof ( int ));
        version = 0;
     }

     if ( version > eventversion )
        throw tinvalidversion ( "event", eventversion, version );

     event1->next = NULL; 
     event1->conn = 0;
     if ( event1->datasize && event1->rawdata ) {
        event1->rawdata = asc_malloc ( event1->datasize );
        stream->readdata ( event1->rawdata, event1->datasize );
     }  else {
        event1->datasize = 0;
        event1->rawdata = NULL;
     }

     for (char m = 0; m <= 3; m++) {
        event1->trigger_data[m] = new tevent::LargeTriggerData;
        if ( event1->trigger[m] ) {
           if ((event1->trigger[m] == ceventt_buildingconquered) || 
              (event1->trigger[m] == ceventt_buildinglost) ||
              (event1->trigger[m] == ceventt_buildingdestroyed) || 
              (event1->trigger[m] == ceventt_building_seen )) {
   
              integer xpos, ypos;
              stream->readdata2 ( xpos );
              stream->readdata2 ( ypos );
              event1->trigger_data[m]->xpos = xpos;
              event1->trigger_data[m]->ypos = ypos;
           } 
   
           if ((event1->trigger[m] == ceventt_unitconquered) || 
              (event1->trigger[m] == ceventt_unitlost) ||
              (event1->trigger[m] == ceventt_unitdestroyed)) {
   
              if ( version == 0 ) {
                 integer xpos, ypos;
                 stream->readdata2 ( xpos );
                 stream->readdata2 ( ypos );
                 event1->trigger_data[m]->xpos = xpos;
                 event1->trigger_data[m]->ypos = ypos;
                 event1->trigger_data[m]->networkid = -1;
              } else {
                 stream->readdata2( event1->trigger_data[m]->xpos );
                 stream->readdata2( event1->trigger_data[m]->ypos );
                 stream->readdata2( event1->trigger_data[m]->networkid );
              }
           } 
   
           if ((event1->trigger[m] == ceventt_event) ||
               (event1->trigger[m] == ceventt_technologyresearched)) {
               stream->readdata2 ( event1->trigger_data[m]->id );
           }

           if ( version >=2 )
              if (event1->trigger[m] == ceventt_allenemyunitsdestroyed ||
                  event1->trigger[m] == ceventt_allenemybuildingsdestroyed )
                 stream->readdata2 ( event1->trigger_data[m]->id );

           if (event1->trigger[m] == ceventt_turn) {
               stream->readdata2 ( event1->trigger_data[m]->time.abstime );
           } 
           if (event1->trigger[m] == ceventt_any_unit_enters_polygon || 
               event1->trigger[m] == ceventt_specific_unit_enters_polygon) {
                  event1->trigger_data[m]->unitpolygon = new tevent::LargeTriggerData::PolygonEntered;
                  stream->readdata2( *event1->trigger_data[m]->unitpolygon );
                  int sz = event1->trigger_data[m]->unitpolygon->size - sizeof ( *event1->trigger_data[m]->unitpolygon );
                  event1->trigger_data[m]->unitpolygon->data = new int [ (sz + sizeof(int) -1 ) / sizeof ( int ) ];
                  stream->readdata( event1->trigger_data[m]->unitpolygon->data, sz );
                  if ( event1->trigger_data[m]->unitpolygon->vehiclenetworkid || event1->trigger_data[m]->unitpolygon->dummy ) {
                     stream->readdata2( event1->trigger_data[m]->unitpolygon->tempxpos );
                     stream->readdata2( event1->trigger_data[m]->unitpolygon->tempypos );
                     stream->readdata2( event1->trigger_data[m]->unitpolygon->tempnwid );
                     event1->trigger_data[m]->unitpolygon->vehiclenetworkid = event1->trigger_data[m]->unitpolygon->tempnwid;
                  } 
                  
           }
        }

     } 

}




/**************************************************************/
/*     sezierungen schreiben / lesen                        ÿ */
/**************************************************************/


void   tspfldloaders::readdissections ( void )
{
   for (int i = 0; i < 8; i ++ ) {
      if ( spfld->player[ i ].__dissectionsToLoad ) {
         int k;
         do {
            Player::Dissection du;
            stream->readInt(); // dummy;
            stream->readInt(); // dummy;

            du.orgpoints = stream->readInt();
            du.points    = stream->readInt();
            du.num       = stream->readInt();

            k = stream->readInt();

            int j = stream->readInt();
            du.fzt = getvehicletype_forid ( j );
            if ( !du.fzt )
               throw InvalidID ( "vehicle", j );

            j = stream->readInt();
            du.tech = gettechnology_forid  ( j );
            if ( !du.tech )
               throw InvalidID ( "technology", j );

            spfld->player[ i ].dissections.push_back ( du );
         } while ( k );
      }
   } /* endfor */
}

void   tspfldloaders::writedissections ( void )
{
   for (int i = 0; i < 8; i ++ ) {
      Player::DissectionContainer::iterator di = spfld->player[i].dissections.begin();
      while ( di != spfld->player[i].dissections.end() ) {
         stream->writeInt ( 1 ); // dummy
         stream->writeInt ( 1 ); // dummy
         stream->writeInt ( di->orgpoints );
         stream->writeInt ( di->points );
         stream->writeInt ( di->num );

         di++;
         if ( di != spfld->player[i].dissections.end() )
            stream->writeInt ( 1 );
         else
            stream->writeInt ( 0 );

         stream->writeInt ( di->fzt->id );
         stream->writeInt ( di->tech->id );
      }
   }
}



/**************************************************************/
/*        Messages  schreiben / lesen                         */
/**************************************************************/

const int messageVersion = 0xabcdef;

void      tspfldloaders:: writemessages ( void )
{
   stream->writeInt ( messageVersion );

   int id = 0;
   for ( MessageContainer::iterator mi = spfld->messages.begin(); mi != spfld->messages.end();  ) {
      id++;
      (*mi)->id = id;

      stream->writeInt ( (*mi)->from );
      stream->writeInt ( (*mi)->to );
      stream->writeInt ( (*mi)->time );
      stream->writeInt ( 1 );
      stream->writeInt ( (*mi)->id );
      stream->writeInt ( (*mi)->gametime.turn() );
      stream->writeInt ( (*mi)->gametime.move() );


      ASCString& t = (*mi)->text;
      mi++;
      stream->writeInt ( mi != spfld->messages.end() ? 1 : 0 );

      stream->writeString ( t );
   }

   for ( int i = 0; i < 8; i++ )
      writemessagelist ( spfld->player[ i ].oldmessage );

   for ( int i = 0; i < 8; i++ )
      writemessagelist ( spfld->player[ i ].unreadmessage );

   for ( int i = 0; i < 8; i++ )
      writemessagelist ( spfld->player[ i ].sentmessage );

   writemessagelist ( spfld->unsentmessage );

   stream->writeInt ( messageVersion );

   stream->writeString ( spfld->gameJournal );
   stream->writeString ( spfld->newJournal );

}


void      tspfldloaders:: writemessagelist( MessagePntrContainer& lst )
{
   for ( MessagePntrContainer::iterator i = lst.begin(); i != lst.end(); i++ )
      stream->writeInt ( (*i)->id );
   stream->writeInt ( 0 );
}


/*
class MessageIDequals : public unary_function<Message*,bool>{
      int id;
   public:
      MessageIDequals ( int _id ) { id == _id; };
      bool operator() ( const Message* m ) { return m->id == id; };
};
*/

void      tspfldloaders:: readmessagelist( MessagePntrContainer& lst )
{
   int i = stream->readInt();
   while ( i ) {
      // MessageContainer::iterator mi = find ( spfld->messages.begin(), spfld->messages.end(), MessageIDequals ( i ));
      MessageContainer::iterator mi = spfld->messages.end();
      for ( MessageContainer::iterator mi2 = spfld->messages.begin(); mi2 != spfld->messages.end(); mi2++ )
         if ( (*mi2)->id == i )
            mi = mi2;

      if ( mi == spfld->messages.end())
         displaymessage ( "message list corrupted !\nplease report this bug!\nthe game will continue, but some messages will probably be missing\nand other instabilities may occur.",1);
      lst.push_back ( *mi );
      i = stream->readInt();
   }
}


void      tspfldloaders:: readmessages ( void )
{
   stream->readInt(); // magic

   while ( spfld->__loadmessages ) {
      Message* msg = new Message ( spfld );

      msg->from    = stream->readInt();
      msg->to      = stream->readInt();
      msg->time    = stream->readInt();
      bool msgtext = stream->readInt();
      msg->id      = stream->readInt();
      int t = stream->readInt();
      int m = stream->readInt();
      msg->gametime.set ( t, m );

      spfld->__loadmessages = stream->readInt();

      if ( msgtext )
         msg->text = stream->readString( true );
   }

   for ( int i = 0; i < 8; i++ )
      if ( spfld->player[ i ].__loadoldmessage )
         readmessagelist ( spfld->player[ i ].oldmessage );

   for ( int i = 0; i < 8; i++ )
      if ( spfld->player[ i ].__loadunreadmessage )
         readmessagelist ( spfld->player[ i ].unreadmessage );

   for ( int i = 0; i < 8; i++ )
      if ( spfld->player[ i ].__loadsentmessage )
         readmessagelist ( spfld->player[ i ].sentmessage );

   if ( spfld->__loadunsentmessage )
      readmessagelist ( spfld->unsentmessage );

   stream->readInt(); // magic

   if ( spfld->___loadJournal )
      spfld->gameJournal = stream->readString();

   if (  spfld->___loadNewJournal )
      spfld->newJournal = stream->readString();
}



/**************************************************************/
/*     Events  schreiben / lesen                            ÿ */
/**************************************************************/

void   tspfldloaders::writeeventstocome ( void )
{
    int      j = 0;
    pevent   event = spfld->firsteventtocome;
    while ( event ) {
       j++;
       event = event->next; 
    } 
 
    stream->writeInt( j );
 
    event = spfld->firsteventtocome; 
    while ( event ) {
       writeevent ( event   );
       event = event->next; 
    } ;
}


void         tspfldloaders::readeventstocome ( void )
{
   int j;
   pevent event1, event2;

   stream->readdata2 ( j );

   int k;
   if ( j ) {
      for (k = 1; k <= j; k++) { 
         event1 = new ( tevent );

         readevent ( event1  );

         if (k == 1) 
            spfld->firsteventtocome = event1; 
         else 
            event2->next = event1; 


         event2 = event1; 
      } 

      event1 = spfld->firsteventtocome;
      while ( event1 ) {
         for ( k = 0; k < 4; k++ )
            if ( event1->trigger[k] == ceventt_event ) {
               event2 = spfld->firsteventtocome;
               while ( event2 ) {
                  if ( event2->id == event1->trigger_data[k]->id )
                     event2->conn |= 1;
                  event2 = event2->next;
               } /* endwhile */
            }


         event1 = event1->next;
      }

   } 
}


void   tspfldloaders::writeeventspassed ( void )
{
    pevent event = spfld->firsteventpassed;
    int j = 0;
    while ( event ) {
       j++;
       event = event->next; 
    } 
                        
    stream->writedata2( j );
 
    event = spfld->firsteventpassed;
    while ( event ) {
       writeevent ( event  );
       event = event->next; 
    } 
}


void   tspfldloaders::readeventspassed ( void )
{                      
   int j;
   pevent event1, event2;
   stream->readdata2 ( j );
   if ( j ) {   
      for (int k = 1; k <= j; k++) {
         event1 = new ( tevent );

         readevent ( event1  );

         if (k == 1) 
            spfld->firsteventpassed = event1;
         else 
            event2->next = event1; 

         event2 = event1; 
      } 
   } 
}

void   tspfldloaders::writeoldevents ( void )
{
     peventstore oldevent =  spfld->oldevents;
     while ( oldevent ) {
        stream->writedata2( oldevent->num );
        if (oldevent->num) {
           stream->writedata ( oldevent->eventid, oldevent->num * sizeof( oldevent->eventid[0] ));
           stream->writedata ( oldevent->mapid, oldevent->num * sizeof( oldevent->mapid[0] ));
        }

        oldevent = oldevent->next; 
     }
     if ( spfld->oldevents ) {
        int n = 0;
        stream->writedata2( n );
     }
}

void   tspfldloaders::readoldevents ( void )
{
   if ( spfld->loadOldEvents ) {
      int  num;
      stream->readdata2 ( num );
      spfld->oldevents = NULL;
      peventstore oldevt = NULL;

      while ( num ) {
         oldevt = new ( teventstore );
         oldevt->num = num;
         oldevt->next = spfld->oldevents;
         stream->readdata ( oldevt->eventid, num * sizeof ( oldevt->eventid[0] ));
         stream->readdata ( oldevt->mapid, num * sizeof ( oldevt->mapid[0] ));

         stream->readdata2 ( num );
      }
      spfld->oldevents = oldevt;
   }
}



/**************************************************************/
/*     map schreiben / lesen / initialisieren         ÿ */
/**************************************************************/

void    tspfldloaders::writemap ( void )
{
       if ( !spfld )
          displaymessage ( "tspfldloaders::writemap  ; no map to write ! ",2);

       spfld->write ( *stream );
}


void     tmaploaders::initmap ( void )
{
    spfld->oldevents = NULL;
    spfld->firsteventtocome = NULL; 
    spfld->firsteventpassed = NULL; 
    spfld->network          = NULL;
    spfld->game_parameter = NULL;
}


void     tgameloaders::initmap ( void )
{
    spfld->game_parameter = NULL;
}


void     tspfldloaders::readmap ( void )
{
    spfld = new tmap;

    spfld->read ( *stream );
}



void tgameloaders :: writeAI ( )
{
   int a = 0;
   for ( int i = 0; i< 8; i++ )
      if ( spfld->player[i].ai )
         a += 1 << i;

   stream->writeInt ( a );
   for ( int i = 0; i < 8; i++ )
      if ( spfld->player[i].ai )
         spfld->player[i].ai->write( *stream );
}

void tgameloaders :: readAI ( )
{
#ifdef sgmain
   int a = stream->readInt();
   for ( int i = 0; i< 8; i++ )
      if ( a & ( 1 << i ) ) {
         AI* ai = new AI ( spfld, i );
         ai->read ( *stream );
         spfld->player[i].ai = ai;
      } else {
         spfld->player[i].ai = NULL;
      }
#endif
}


/**************************************************************/
/*     Network schreiben / lesen                            ÿ */
/**************************************************************/

void        tspfldloaders::writenetwork ( void )
{
  if ( spfld->network ) {
     int i;
     stream->writedata2 ( *spfld->network );
     for ( i = 0; i < 8  ; i++ ) 
        if (spfld->network->computer[i].name != NULL)
           stream->writepchar ( spfld->network->computer[i].name );

  }
}


void        tspfldloaders::readnetwork ( void )
{
   if ( spfld->network ) {
      int i;
      spfld->network = new ( tnetwork );
      stream->readdata2 ( *spfld->network );
      for (i=0; i<8 ; i++ ) 
         if (spfld->network->computer[i].name != NULL )
            stream->readpchar ( &spfld->network->computer[i].name );
   }
}




/**************************************************************/
/*     fielder schreiben / lesen                             ÿ */
/**************************************************************/

const int objectstreamversion = 1;

void   tspfldloaders::writefields ( void )
{
   int l = 0;
   int cnt1 = spfld->xsize * spfld->ysize;
   int cnt2;

   do {
      cnt2 = 0; 
      pfield fld = &spfld->field[l];
      /*

      RLE encoding not supported any more, since tfield is becomming too complex

      if (l + 2 < cnt1) { 
         l2 = l + 1; 
         fld2 = &spfld->field[l2];
       asfasfdasfd
         while ((l2 + 2 < cnt1) && ( memcmp(fld2, fld, sizeof(*fld2)) == 0) ) {
            cnt2++;
            l2++;
            fld2 = &spfld->field[l2];
         } 
      }
      */


      char b1 = 0;
      char b3 = 0;
      char b4 = 0;

      if (fld->typ->terraintype->id > 255) 
         b1 |= csm_typid32; 
      if (fld->direction != 0) 
         b1 |= csm_direction; 
      if (fld->vehicle != NULL) 
         b1 |= csm_vehicle; 
      if ( (fld->bdt & getTerrainBitType( cbbuildingentry )).any() )
         b1 |= csm_building; 


      if (cnt2 > 0) 
         b1 |= csm_cnt2; 

      if (fld->material > 0) 
         b3 |= csm_material; 
      if (fld->fuel > 0) 
         b3 |= csm_fuel; 

      if (fld->typ != fld->typ->terraintype->weather[0])
         b3 |= csm_weather;
      if (fld->visible)
         b3 |= csm_visible;
      if ( !fld->objects.empty() || !fld->mines.empty() )
         b4 |= csm_newobject;

      if ( fld->resourceview )
         b4 |= csm_resources;

      if ( fld->connection )
         b4 |= csm_connection;

      if ( b4 )
         b3 |= csm_b4;

      if ( b3 ) 
         b1 |= csm_b3; 

      stream->writeChar( b1 );

      if (b1 & csm_b3 ) 
         stream->writeChar ( b3 );

      if (b3 & csm_b4 )
         stream->writeChar ( b4 );

      if (b1 & csm_cnt2 ) 
         stream->writeInt ( cnt2 );

      if (b3 & csm_weather ) {
         int k = 1;
         while ( fld->typ != fld->typ->terraintype->weather[k]    &&   k < cwettertypennum ) {
            k++;
         } /* endwhile */
         
         if ( k == cwettertypennum ) {
            k = 0;
            displaymessage ( "invalid terrain ( weather not found ) at position %d \n",1,l );
          }
         stream->writeInt ( k );
      }

      if (b1 & csm_typid32 )                  
         stream->writeInt ( fld->typ->terraintype->id );
      else
         stream->writeChar ( fld->typ->terraintype->id );

      if (b1 & csm_direction )                    
         stream->writeChar ( fld->direction );

      if (b1 & csm_vehicle ) 
         fld->vehicle->write ( *stream );


      if (b1 & csm_building ) 
         fld->building->write ( *stream );

      if (b3 & csm_material ) 
         stream->writeChar ( fld->material );

      if (b3 & csm_fuel ) 
         stream->writeChar ( fld->fuel );

      if (b3 & csm_visible )
         stream->writeWord ( fld->visible );

      if ( b4 & csm_newobject ) {
         stream->writeInt ( objectstreamversion );

         stream->writeInt ( fld->mines.size() );
         for ( tfield::MineContainer::iterator m = fld->mines.begin(); m != fld->mines.end(); m++  ) {
            stream->writeInt ( m->type );
            stream->writeInt ( m->strength );
            stream->writeInt ( m->time );
            stream->writeInt ( m->player );
         }

         stream->writeInt ( fld->objects.size() );

         for ( tfield::ObjectContainer::iterator o = fld->objects.begin(); o != fld->objects.end(); o++  ) {
            stream->writeInt ( 1 ); // was: pointer to type
            stream->writeInt ( o->damage );
            stream->writeInt ( o->dir );
            stream->writeInt ( o->time );
            for ( int i = 0; i < 4; i++ )
               stream->writeInt ( 0 );  // dummy
            stream->writeInt ( o->typ->id );
         }
      }

      if (b4 & csm_resources ) {
         stream->writeChar ( fld->resourceview->visible );
         for ( int i = 0; i < 8; i++ )
            stream->writeChar ( fld->resourceview->fuelvisible[i] );
         for ( int i = 0; i < 8; i++ )
            stream->writeChar ( fld->resourceview->materialvisible[i] );
      }

      if ( b4 & csm_connection )
         stream->writeInt ( fld->connection );

      l += 1 + cnt2;
   }  while (l < cnt1);
}


void tspfldloaders::readfields ( void )
{
   int cnt2 = 0;
   int cnt1 = spfld->xsize * spfld->ysize;

   spfld->allocateFields ( spfld->xsize , spfld->ysize );
   
   if (spfld->field == NULL)
      displaymessage ( "Could not allocate memory for map ",2);

   int l = 0;
   pfield lfld = NULL;

   do {
      pfield fld2;

      if (cnt2 == 0) { 

         fld2 = & spfld->field[l];

         fld2->bdt.setInt ( 0 , 0 );

         char b1, b3, b4;
         b1 = stream->readChar();

         if (b1 & csm_b3 ) 
            b3 = stream->readChar();
         else 
            b3 = 0; 

         if (b3 & csm_b4 )
            b4 = stream->readChar();
         else
            b4 = 0;

         if (b1 & csm_cnt2 ) 
            cnt2 = stream->readInt();
         else
            cnt2 = 0; 

         int weather;
         if (b3 & csm_weather )
            weather = stream->readInt();
         else 
            weather = 0;


         int k;

         if (b1 & csm_typid32 ) 
            k = stream->readInt();
         else
            k = stream->readChar();

         pterraintype trn = getterraintype_forid ( k );
         if ( !trn ) 
            throw InvalidID ( "terrain", k );

         fld2->typ = trn->weather[weather];
         if ( !fld2->typ ) 
            throw InvalidID ( "terrain", k );

         if (b1 & csm_direction )
            fld2->direction = stream->readChar();
         else                                              
            fld2->direction = 0; 


         if (b1 & csm_vehicle ) {
             fld2->vehicle = Vehicle::newFromStream ( spfld, *stream );
             fld2->vehicle->setnewposition ( l%spfld->xsize, l/spfld->xsize );
         }

         if (b1 & csm_building ) {
            fld2->building = Building::newFromStream ( spfld, *stream );
            fld2->bdt |= getTerrainBitType(cbbuildingentry);
         }

         if (b3 & csm_material) 
            fld2->material = stream->readChar();
         else 
            fld2->material = 0; 

         if (b3 & csm_fuel) 
            fld2->fuel = stream->readChar();
         else 
            fld2->fuel = 0;

         if (b3 & csm_visible)
            fld2->visible = stream->readWord();
         else
            fld2->visible = 0;

         bool tempObjects[16];
         int  tempObjectNum;

         if (b3 & csm_object ) {

            char minetype = stream->readChar();
            char minestrength = stream->readChar();
            if ( minetype >> 4 ) {
               Mine m;
               m.type = MineTypes((minetype >> 1) & 7);
               m.strength = minestrength;
               m.time = 0;
               m.player = minetype >> 4;
               fld2->mines.push_back ( m );
            }

            tempObjectNum = stream->readInt();

            for ( int i = 0; i < 16; i++ )
               tempObjects[i] = stream->readInt();
         }

         if ( b4 & csm_newobject ) {
            int objectversion = stream->readInt();

            if ( objectversion != objectstreamversion )
               throw tinvalidversion ( "object", objectstreamversion, objectversion );

            int minenum = stream->readInt();

            for ( int i = 0; i < minenum; i++ ) {
               Mine m;
               m.type = MineTypes(stream->readInt());
               m.strength = stream->readInt();
               m.time = stream->readInt();
               m.player = stream->readInt();
               fld2->mines.push_back ( m );
            }

            tempObjectNum = stream->readInt();
         }

         if ( (b3 & csm_object) || (b4 & csm_newobject )) {
            for ( int n = 0; n < tempObjectNum; n++ ) {
               Object o;
               stream->readInt(); // was: type
               o.damage = stream->readInt();
               o.dir = stream->readInt();
               o.time = stream->readInt();
               for ( int i = 0; i < 4; i++ )
                  stream->readInt(); // dummy

               int id = stream->readInt();
               o.typ = getobjecttype_forid ( id );

               if ( !o.typ )
                  throw InvalidID ( "object", id );

               fld2->objects.push_back ( o );
            }
            fld2->sortobjects();
         }

         if (b4 & csm_resources ) {
            fld2->resourceview = new tfield::Resourceview;
            fld2->resourceview->visible = stream->readChar();
            for ( int i = 0; i < 8; i++ )
               fld2->resourceview->fuelvisible[i] = stream->readChar();
            for ( int i = 0; i < 8; i++ )
               fld2->resourceview->materialvisible[i] = stream->readChar();
         }

         if ( b4 & csm_connection ) 
            fld2->connection = stream->readInt();
         
         if (b1 & csm_cnt2 )
            lfld = fld2;

      } 
      else {
         spfld->field[l].typ = lfld->typ;
         spfld->field[l].fuel = lfld->material;
         spfld->field[l].visible = lfld->visible;
         spfld->field[l].direction = lfld->direction;
         spfld->field[l].picture = NULL;
         spfld->field[l].tempw = 0;
         spfld->field[l].connection = lfld->connection;
         for ( int i = 0; i < 8; i++ )
            spfld->field[l].view[i] = lfld->view[i];
         cnt2--;
      } 
      l++ ;
   }  while (l < cnt1);

}



/**************************************************************/
/*     Chain Items                                          ÿ */
/**************************************************************/

void   tspfldloaders::chainitems ( pmap actmap )
{
   int i = 0;
   for (int y = 0; y < actmap->ysize; y++)
      for (int x = 0; x < actmap->xsize; x++) {
          pfield fld = &actmap->field[i];
          fld->setparams();
          if ( (fld->bdt & getTerrainBitType( cbbuildingentry )).any() )
             fld->building->resetPicturePointers();
          i++;
      }
}



/**************************************************************/
/*     Set Player Existencies                               ÿ */
/**************************************************************/


tspfldloaders::tspfldloaders ( void )
{
   spfld = NULL;
}


tspfldloaders::~tspfldloaders ( void )
{
  delete spfld;
  spfld = NULL;
}












































int          tmaploaders::savemap( const ASCString& name )
{ 
   #ifdef logging
   logtofile ( "loaders / tmaploaders::savemap / started " );
   #endif

//   ASCString backupname= name + "~";
//   ::rename(name.c_str(), backupname.c_str());

   tnfilestream filestream ( name, tnstream::writing );

   stream = &filestream;

   spfld = actmap;


   /********************************************************************************/
   /*   Stream initialisieren, Dateiinfo schreiben , map schreiben         ÿ */
   /********************************************************************************/
   {
       stream->writepchar ( getFullVersionString() );  // description is not used any more
       stream->writeWord ( fileterminator );
       stream->writeInt ( actmapversion  );

       writemap ( );
   }

   writeeventstocome ();
   writefields ();
   stream->writeInt ( actmapversion );

   spfld = NULL;

   return 0;
} 






tmaploaders :: ~tmaploaders()
{
   if ( oldmap ) {
      delete oldmap ;
      oldmap = NULL;
   }
}

int          tmaploaders::loadmap( const char *       name )
{ 
    oldmap = actmap;
    actmap = NULL;

    displayLogMessage ( 4, "loading map %s ... ", name );

    tnfilestream filestream ( name, tnstream::reading);

    stream = &filestream;


    char* description = NULL;
 
    stream->readpchar ( &description );
    delete[] description;
 
    word w = stream->readWord();
 
    if ( w != fileterminator ) 
       throw tinvalidversion ( name, fileterminator, (int) w );


    int version = stream->readInt();
 
    if ( version > actmapversion || version < minmapversion )
       throw tinvalidversion ( name, actmapversion, version );
   

   displayLogMessage ( 8, "map, ");
   readmap ();

   displayLogMessage ( 8, "eventsToCome, ");
   readeventstocome ();

   displayLogMessage ( 8, "fields, ");
   readfields ();


  /*****************************************************************************************************/
  /*   šberprfen,  Stream schlieáen                 ÿ                                                 */
  /*****************************************************************************************************/

   version = stream->readInt();
   if (version > actmapversion || version < minmapversion ) {
      delete spfld;
      spfld = NULL;
      throw tinvalidversion ( name, actmapversion, version );
   } 

   displayLogMessage ( 8, "chainItems, ");
   chainitems ( spfld );

   for ( int sp = 7; sp >= 0; sp--)
      if ( spfld->player[sp].exist() )
         spfld->actplayer = sp;

   displayLogMessage ( 8, "setEventTriggers, ");
   seteventtriggers( spfld );

   displayLogMessage ( 8, "calculateallobjects ");
   calculateallobjects( spfld );


   displayLogMessage ( 8, "~oldmap, ");
   delete oldmap;
   oldmap = NULL;
   
   actmap = spfld;
   spfld = NULL;


   cursor.posx = 0;
   cursor.posy = 0;

   actmap->time.set ( 1, 0 );
   actmap->levelfinished = false;

   displayLogMessage ( 4, "done\n");

   return 0;
} 



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  */ 








void   tsavegameloaders::savegame( pnstream strm, pmap gamemap, bool writeReplays )
{
   stream = strm;
   spfld = gamemap;

   stream->writepchar( getFullVersionString() );
   stream->writeWord( fileterminator );

   stream->writeInt( actsavegameversion );
   writemap ();

   writenetwork ( );

   for ( int i = 0; i < 8; i++ )
      spfld->player[i].research.write_techs ( *stream );

   writemessages();
   writeeventstocome ();
   writeeventspassed ();
   writeoldevents    ();

   writefields ( );

   writedissections();

   if ( writeReplays && spfld->replayinfo ) {
      stream->writeInt( 1 );
      spfld->replayinfo->write(*stream);
   } else
      stream->writeInt( 0 );

   writeAI();

   stream->writeInt( actsavegameversion );

   spfld = NULL;
}


void         tsavegameloaders::savegame( const char* name )
{ 
   tnfilestream filestream ( name, tnstream::writing );
   savegame ( &filestream, actmap, true );
}




int   tsavegameloaders::loadgame( const char* filename )
{
   tnfilestream filestream ( filename, tnstream::reading );

   pmap spfld = loadgame ( &filestream );

   delete actmap;
   actmap = spfld;
   actmap->levelfinished = false;

   cursor.posx = 0;
   cursor.posy = 0;

   if ( actmap->replayinfo ) {
      if ( actmap->replayinfo->actmemstream )
         displaymessage2( "actmemstream already open at begin of turn ",2 );

      if ( actmap->replayinfo->guidata[actmap->actplayer] )
         actmap->replayinfo->actmemstream = new tmemorystream ( actmap->replayinfo->guidata[actmap->actplayer], tnstream::appending );
      else {
         actmap->replayinfo->guidata[actmap->actplayer] = new tmemorystreambuf;
         actmap->replayinfo->actmemstream = new tmemorystream ( actmap->replayinfo->guidata[actmap->actplayer], tnstream::writing );
      }
   }

   return 0;
}

tmap*          tsavegameloaders::loadgame( pnstream strm )
{
   stream = strm;

   stream->readString(); // was: description

   int w = stream->readWord();

   if ( w != fileterminator ) 
      throw tinvalidversion ( strm->getDeviceName(), fileterminator, w );
   
   int version = stream->readInt();

   if (version > actsavegameversion || version < minsavegameversion ) 
      throw tinvalidversion ( strm->getDeviceName(), actsavegameversion, version );
   

   readmap ();

   readnetwork ();

   for ( int i = 0; i < 8; i++ )
      spfld->player[i].research.read_techs ( *stream );

   readmessages();

   readeventstocome ();
   readeventspassed ();
   readoldevents    ();

   readfields ( );

   readdissections();

   bool loadReplay = true;
   if ( version >= 0xff35 ) 
      if ( !stream->readInt() )
         loadReplay = false;

   if ( loadReplay ) {
      spfld->replayinfo = new tmap::ReplayInfo;
      spfld->replayinfo->read ( *stream );
   } else
      spfld->replayinfo = NULL;


   if ( version >= 0xff34 )
      readAI ();

   version = stream->readInt();
   if (version > actsavegameversion || version < minsavegameversion )
      throw tinvalidversion ( strm->getDeviceName(), actsavegameversion, version );

   chainitems ( spfld );

   seteventtriggers( spfld );

   calculateallobjects( spfld );

   tmap* s = spfld;
   spfld = NULL;  // to avoid that is is deleted by the destructor of tsavegameloaders
   return s;
}



   /*****************************************************************************************************/
   /*   Netzwerk                                      ÿ                                                 */
   /*****************************************************************************************************/



/*
void         tnetworkloaders::checkcrcs ( void )
{
   if ( actmap )
      if ( actmap->objectcrc ) {
         tspeedcrccheck* ck = actmap->objectcrc->speedcrccheck;
   
         int i;
         for ( i = 0; i < 9; i++ ) {
            pbuilding bld = actmap->player[i].firstbuilding;
            while ( bld ) {
               ck->checkbuilding2 ( bld->typ );
               for (int j = 0; j< 32 ; j++) {
                   ck->checkunit2 ( bld->production[j] );
                   ck->checkunit2 ( bld->loading[j]->typ );
               }
   
               bld = bld->next;
            }
         }
   
         for ( i = 0; i < 8; i++ ) {
            pvehicle eht = actmap->player[i].firstvehicle;
            while ( eht ) {
               ck->checkunit2 ( eht->typ );
               for (int j = 0; j< 32 ; j++) 
                   ck->checkunit2 ( eht->loading[j]->typ );
               
               eht = eht->next;
            }
         }
      }
}
*/


int          tnetworkloaders::savenwgame( pnstream strm )
{ 
   spfld = actmap;

   stream = strm;

   stream->writepchar ( getFullVersionString() );  // description is not used any more
   stream->writeWord ( fileterminator );
 
   stream->writeInt ( actnetworkversion );

   writemap ();

   for ( int i = 0; i < 8; i++ )
      spfld->player[i].research.write_techs ( *stream );

   writemessages();

   writenetwork ( );

   stream->writeInt ( actnetworkversion );

   writeeventstocome ();
   writeeventspassed ();
   writeoldevents    ();


   writefields ( );

   writedissections();

   if ( spfld->replayinfo )
      spfld->replayinfo->write ( *stream );

   stream->writeInt ( actnetworkversion );

   spfld = NULL;

   return 0;
} 





int          tnetworkloaders::loadnwgame( pnstream strm )
{ 
   char* name = "network game";

   stream = strm;


   char* description = NULL;
   stream->readpchar ( &description );
   delete[] description;

   word w = stream->readWord();

   if ( w != fileterminator ) 
      throw tinvalidversion ( name, fileterminator, (int) w );


   int version = stream->readInt();

   if (version > actnetworkversion || version < minnetworkversion ) 
      throw tinvalidversion ( name, actnetworkversion, version );
   
   readmap ();

   for ( int i = 0; i < 8; i++ )
      spfld->player[i].research.read_techs ( *stream );


   readmessages();
   readnetwork ();

   version = stream->readInt();

   if (version > actnetworkversion || version < minnetworkversion ) 
      throw tinvalidversion ( name, actnetworkversion, version );


   readeventstocome ();
   readeventspassed ();
   readoldevents    ();
                     
   readfields ( );
 
   readdissections();

   if ( spfld->__loadreplayinfo ) {
      spfld->replayinfo = new tmap::ReplayInfo;
      spfld->replayinfo->read ( *stream );
   }

   stream->readdata2( version );
   if (version > actnetworkversion || version < minnetworkversion )
      throw tinvalidversion ( name, actnetworkversion, version );

   chainitems ( spfld );

   seteventtriggers( spfld );

   delete actmap;
   actmap = spfld;
   spfld = NULL;

   calculateallobjects();

   actmap->levelfinished = false;

  #ifdef sgmain
   getnexteventtime();
  #endif

  return 0;
}














void  savemap( const char * name )
{

   #ifdef logging
   logtofile ( "loaders / savemap / started " );
   #endif

   try {
     tmaploaders gl;
     gl.savemap ( name );
   } /* endtry */

   catch ( tfileerror err ) {
      displaymessage( "file error writing map to filename %s ", 1, err.getFileName().c_str() );
   } /* endcatch */
   catch ( ASCexception err) {
      displaymessage( "error writing map ", 1 );
   } /* endcatch */

}

void  loadmap( const char *       name )
{
   try {
     tmaploaders gl;
     gl.loadmap ( name );

     actmap->preferredFileNames.mapname[0] = name ;
   }
   catch ( InvalidID err ) {
      displaymessage( err.getMessage().c_str(), 1 );
      if ( !actmap || actmap->xsize <= 0)
         throw NoMapLoaded();
   } /* endcatch */
   catch ( tinvalidversion err ) {
      if ( err.expected < err.found )
         displaymessage( "File/module %s has invalid version.\nExpected version %d\nFound version %d\nPlease install the latest version from www.asc-hq.org", 1, err.getFileName().c_str(), err.expected, err.found );
      else
         displaymessage( "File/module %s has invalid version.\nExpected version %d\nFound version %d\nThis is a bug, please report it!", 1, err.getFileName().c_str(), err.expected, err.found );

      if ( !actmap || actmap->xsize <= 0)
         throw NoMapLoaded();
   } /* endcatch */
   catch ( tfileerror err) {
      displaymessage( "error reading map filename %s ", 1, err.getFileName().c_str() );
      if ( !actmap || actmap->xsize <= 0)
         throw NoMapLoaded();
   } /* endcatch */
   catch ( ASCexception ) {
      displaymessage( "error loading map", 1 );
      if ( !actmap || actmap->xsize <= 0)
         throw NoMapLoaded();
   } /* endcatch */
}


void  savegame( const char *       name )
{
   try {
      tsavegameloaders gl;
      gl.savegame ( name );
   }
   catch ( tfileerror err) {
      displaymessage( "error writing map to filename %s ", 1, err.getFileName().c_str() );
   } /* endcatch */
   catch ( ASCexception err) {
      displaymessage( "error writing map ", 1 );
   } /* endcatch */
}


void  loadgame( const char *       name )
{
   try {
      tsavegameloaders gl;
      gl.loadgame ( name );
   }
   catch ( InvalidID err ) {
      displaymessage( err.getMessage().c_str(), 1 );
      if ( !actmap || actmap->xsize <= 0)
         throw NoMapLoaded();
   } /* endcatch */
   catch ( tinvalidversion err ) {
      if ( err.expected < err.found )
         displaymessage( "File/module %s has invalid version.\nExpected version %d\nFound version %d\nPlease install the latest version from www.asc-hq.org", 1, err.getFileName().c_str(), err.expected, err.found );
      else
         displaymessage( "File/module %s has invalid version.\nExpected version %d\nFound version %d\nThis is a bug, please report it!", 1, err.getFileName().c_str(), err.expected, err.found );
      if ( !actmap || actmap->xsize <= 0)
         throw NoMapLoaded();
   } /* endcatch */
   catch ( tfileerror err) {
      displaymessage( "error reading map filename %s ", 1, err.getFileName().c_str() );
      if ( !actmap || actmap->xsize <= 0)
         throw NoMapLoaded();
   } /* endcatch */
   catch ( ASCexception ) {
      displaymessage( "error loading game", 1 );
      if ( !actmap || actmap->xsize <= 0)
         throw NoMapLoaded();
   } /* endcatch */

  #ifdef sgmain
   getnexteventtime();
  #endif
}



void  savereplay( int num )
{
   try {
      if ( !actmap->replayinfo )
         displaymessage ( "treplayloaders :: savereplay   ;   No replay activated !",2);

      if ( actmap->replayinfo->map[num] ) {
         delete actmap->replayinfo->map[num];
         actmap->replayinfo->map[num] = NULL;
      }

      actmap->replayinfo->map[num] = new tmemorystreambuf;
      tmemorystream memstream ( actmap->replayinfo->map[num], tnstream::writing );

      memstream.writeInt( actreplayversion );

      tsavegameloaders sgl;
      sgl.savegame ( &memstream, actmap, false );

      memstream.writeInt ( actreplayversion );
   }
   catch ( ASCexception err) {
      displaymessage( "error saving replay information", 1 );
   } /* endcatch */
}

void  loadreplay( pmemorystreambuf streambuf )
{
   try {
      char* name = "memorystream actmap->replayinfo";
      tmemorystream memstream ( streambuf, tnstream::reading );

      int version = memstream.readInt();
      if (version > actreplayversion || version < minreplayversion )
         throw tinvalidversion ( name, actreplayversion, version );

      tsavegameloaders sgl;
      tmap* replaymap = sgl.loadgame ( &memstream );

      version = memstream.readInt();
      if (version > actreplayversion || version < minreplayversion ) {
         delete replaymap;
         throw tinvalidversion ( name, actreplayversion, version );
      }

      delete actmap;
      actmap = replaymap;

   }
   catch ( InvalidID err ) {
      displaymessage( err.getMessage().c_str(), 1 );
      if ( !actmap || actmap->xsize == 0)
         throw NoMapLoaded();
   } /* endcatch */
   catch ( tinvalidversion err ) {
      if ( err.expected < err.found )
         displaymessage( "File/module %s has invalid version.\nExpected version %d\nFound version %d\nPlease install the latest version from www.asc-hq.org", 1, err.getFileName().c_str(), err.expected, err.found );
      else
         displaymessage( "File/module %s has invalid version.\nExpected version %d\nFound version %d\nThis is a bug, please report it!", 1, err.getFileName().c_str(), err.expected, err.found );
      if ( !actmap || actmap->xsize == 0)
         throw NoMapLoaded();
   } /* endcatch */
   catch ( tfileerror err) {
      displaymessage( "error reading map filename %s ", 1, err.getFileName().c_str() );
      if ( !actmap || actmap->xsize == 0)
         throw NoMapLoaded();
   } /* endcatch */
   catch ( ASCexception ) {
      displaymessage( "error loading replay", 1 );
      if ( !actmap || actmap->xsize == 0)
         throw NoMapLoaded();
   } /* endcatch */
}














bool validatemapfile ( const char* filename )
{

   char* description = NULL;

   try {

      tnfilestream stream ( filename, tnstream::reading );
      stream.readpchar ( &description, 200 );
      if ( description ) {
         delete[]  description ;
         description = NULL;
      }

      int w = stream.readWord();
      if ( w != fileterminator )
         throw tinvalidversion ( filename, fileterminator, (int) w );

      int version = stream.readInt();

      if (version > actmapversion || version < minmapversion ) 
         throw tinvalidversion ( filename, actmapversion, version );

   } /* endtry */

   catch ( ASCexception ) {
       return 0;
   } /* endcatch */

   return 1;
} 





bool validateemlfile ( const char* filename )
{
   char* description = NULL;

   try {

      tnfilestream stream ( filename, tnstream::reading );
      stream.readpchar ( &description, 200 );
      if ( description ) {
         delete[]  description ;
         description = NULL;
      }

      int w = stream.readWord();
      if ( w != fileterminator )
         throw tinvalidversion ( filename, fileterminator, (int) w );

      int version = stream.readInt();
   
      if (version > actnetworkversion || version < minnetworkversion ) 
         throw tinvalidversion ( filename, actnetworkversion, version );

   } /* endtry */

   catch ( ASCexception ) {
       return 0;
   } /* endcatch */


   return 1;
} 


bool validatesavfile ( const char* filename )
{

   char* description = NULL;

   try {

      tnfilestream stream ( filename, tnstream::reading );
      stream.readpchar ( &description, 200 );
      if ( description ) {
         delete[]  description ;
         description = NULL;
      }

      int w = stream.readWord();
      if ( w != fileterminator )
         throw tinvalidversion ( filename, fileterminator, w );

      int version = stream.readInt();
      if (version > actsavegameversion || version < minsavegameversion )
         throw tinvalidversion ( filename, actsavegameversion, version );

   } /* endtry */

   catch ( ASCexception ) {
       return 0;
   } /* endcatch */

   return 1;
} 









void         savecampaignrecoveryinformation( const ASCString& filename,
                                             int id)
{ 
   displaymessage("This has not been implemented yet, sorry!", 2 );
}






void         loadstreets(void)
{ 
  int          w;

  tnfilestream stream ( "hexmines.raw", tnstream::reading );
  for ( int i = 0; i < 4; i++) 
      stream.readrlepict( &icons.mine[i], false, &w);

}







void         loadicons(void)
{ 
  int w2;
  int          *w = & w2, i;


  {
      int xl[5] = { cawar, cawarannounce, capeaceproposal, capeace, capeace_with_shareview };
      tnfilestream stream ("allianc2.raw",tnstream::reading);
      for ( i = 0; i < 5; i++ )
         stream.readrlepict( &icons.diplomaticstatus[xl[i]],false,w); 
      icons.diplomaticstatus[canewsetwar1] = icons.diplomaticstatus[cawar];
      icons.diplomaticstatus[canewsetwar2] = icons.diplomaticstatus[cawar];
      icons.diplomaticstatus[canewpeaceproposal] = icons.diplomaticstatus[capeaceproposal];

  }
   
  {
      tnfilestream stream ("iconship.raw",tnstream::reading);
      stream.readrlepict( &icons.statarmy[2],false,w);
  }

  {
      tnfilestream stream ("icontank.raw",tnstream::reading);
      stream.readrlepict( &icons.statarmy[1],false,w);
  }
   
  {
      tnfilestream stream ("iconplan.raw",tnstream::reading);
      stream.readrlepict( &icons.statarmy[0],false,w);
  }
   
  {
      tnfilestream stream ("pfeil2.raw",tnstream::reading);
      stream.readrlepict( &icons.weapinfo.pfeil1, false, w);
  }
   
  {
      tnfilestream stream ("pfeil3.raw",tnstream::reading);
      stream.readrlepict( &icons.weapinfo.pfeil2, false, w);
  }

  {
      tnfilestream stream ("height.raw",tnstream::reading); 
      for (i = 0; i <= 7; i++) 
         stream.readrlepict( &icons.height[i],false,w);
   }

   weapDist.loaddata(); 
} 


/*
tspeedcrccheck :: tspeedcrccheck ( pobjectcontainercrcs crclist )
{
   status = 0;

   strng = NULL;
   strnglen = 0;

   int i;
*/
/*
   for ( i = 0; i <= maxterrainanz; i++ )
      bdt[i] = 0;

   for ( i = 0; i <= maxvehicletypeenanz; i++ )
      fzt[i] = 0;

   for ( i = 0; i <= maxbuildingnumber; i++ )
      bld[i] = 0;

   for ( i = 0; i <= maxtechnologynumber; i++ )
      tec[i] = 0;

   for ( i = 0; i <= maxobjectnumber; i++ )
      obj[i] = 0;
*/
/*
   list = crclist;







   for ( i = 0; i < list->unit.crcnum; i++ ) 
      if ( getcrc ( getvehicletype_forid ( list->unit.crc[i].id, 0 )) == list->unit.crc[i].crc )
         fzt[ list->unit.crc[i].id ] = 1;
      else    
         fzt[ list->unit.crc[i].id ] = 2;

   for ( i = 0; i < list->building.crcnum; i++ ) 
      if ( getcrc ( getbuildingtype_forid ( list->building.crc[i].id, 0 )) == list->building.crc[i].crc )
         bld[ list->building.crc[i].id ] = 1;
      else
         bld[ list->building.crc[i].id ] = 2;

   for ( i = 0; i < list->object.crcnum; i++ ) 
      if ( getcrc ( getobjecttype_forid ( list->object.crc[i].id, 0 )) == list->object.crc[i].crc )
         obj[ list->object.crc[i].id ] = 1;
      else
         obj[ list->object.crc[i].id ] = 2;

   for ( i = 0; i < list->terrain.crcnum; i++ ) 
      if ( getcrc ( getterraintype_forid ( list->terrain.crc[i].id, 0 )) == list->terrain.crc[i].crc )
         bdt[ list->terrain.crc[i].id ] = 1;
      else
         bdt[ list->terrain.crc[i].id ] = 2;

   for ( i = 0; i < list->technology.crcnum; i++ ) 
      if ( getcrc ( gettechnology_forid ( list->technology.crc[i].id, 0 )) == list->technology.crc[i].crc )
         tec[ list->technology.crc[i].id ] = 1;
      else
         tec[ list->technology.crc[i].id ] = 2;
}




void tspeedcrccheck :: additemtolist ( pcrcblock lst, int id, int crc )
{
    pcrc pc = lst->crc;
    lst->crc = new tcrc[ lst->crcnum + 1 ];
    for ( int i = 0; i < lst->crcnum; i++ )
       lst->crc[i] = pc[i];

    lst->crc[ lst->crcnum ].crc = crc;
    lst->crc[ lst->crcnum ].id  = id;

    if ( pc )
       delete pc;

    lst->crcnum++;
}


int  tspeedcrccheck :: checkunit     ( pvehicletype f, int add )
{
   if ( !f )
      return 1;

   int stat = fzt[ f->id ];
   if ( stat == 1 )
      return 1;
   else
      if ( stat == 2 )
         return 0;
      else
         if ( stat == 0 ) {
            if ( list->unit.restricted == 0 ||  ( add == 0  && list->unit.restricted == 1 ))
               return 1;
            else
              if ( list->unit.restricted == 1 ) {

                 additemtolist ( &list->unit, f->id, getcrc ( f ) );
                 appendstring ( "unit", f->description, f->id, 0 );

                 fzt[ f->id ] = 1;

                 return 1;
              } else
                 if ( list->unit.restricted == 2 )
                    return 0;

               
         }
   return 0;
}

int  tspeedcrccheck :: checkbuilding     ( pbuildingtype b, int add )
{
   if ( !b )
      return 1;

   int stat = bld[ b->id ];
   if ( stat == 1 )
      return 1;
   else
      if ( stat == 2 )
         return 0;
      else
         if ( stat == 0 ) {
            if ( list->building.restricted == 0 ||  ( add == 0  && list->building.restricted == 1 ) )
               return 1;
            else
              if ( list->building.restricted == 1 ) {
                 additemtolist ( &list->building, b->id, getcrc ( b ) );
                 appendstring ( "building", b->name, b->id, 0 );

                 bld[ b->id ] = 1;

                 return 1;
              } else
                 if ( list->building.restricted == 2 )
                    return 0;


         }
   return 0;
}

int  tspeedcrccheck :: checktech     ( ptechnology t, int add )
{
   if ( !t )
      return 1;

   int stat = tec[ t->id ];
   if ( stat == 1 )
      return 1;
   else
      if ( stat == 2 )
         return 0;
      else
         if ( stat == 0 ) {
            if ( list->technology.restricted == 0 ||  ( add == 0  && list->technology.restricted == 1 ))
               return 1;
            else
              if ( list->technology.restricted == 1 ) {
                 additemtolist ( &list->technology, t->id, getcrc ( t ) );
                 appendstring ( "technology", t->name, t->id, 0 );
                 tec[ t->id ] = 1;

                 return 1;
              } else
                 if ( list->technology.restricted == 2 )
                    return 0;

               
         }
   return 0;
}




int  tspeedcrccheck :: checkobj   ( pobjecttype o, int add )
{
   if ( !o )
      return 1;

   int stat = obj[ o->id ];
   if ( stat == 1 )
      return 1;
   else
      if ( stat == 2 )
         return 0;
      else
         if ( stat == 0 ) {
            if ( list->object.restricted == 0 ||  ( add == 0  && list->object.restricted == 1 ))
               return 1;
            else
              if ( list->object.restricted == 1 ) {
                 additemtolist ( &list->object, o->id, getcrc ( o ) );
                 appendstring ( "object", o->name, o->id, 0 );

                 obj[ o->id ] = 1;

                 return 1;
              } else
                 if ( list->object.restricted == 2 )
                    return 0;

               
         }
   return 0;
}

int  tspeedcrccheck :: checkterrain    ( pterraintype b, int add )
{
   if ( !b )
      return 1;

   int stat = bdt[ b->id ];
   if ( stat == 1 )
      return 1;
   else
      if ( stat == 2 )
         return 0;
      else
         if ( stat == 0 ) {
            if ( list->terrain.restricted == 0  ||  ( add == 0  && list->terrain.restricted == 1 ))
               return 1;
            else
              if ( list->terrain.restricted == 1 ) {
                 additemtolist ( &list->terrain, b->id, getcrc ( b ) );
                 appendstring ( "terrain", b->name, b->id, 0 );

                 bdt[ b->id ] = 1;

                 return 1;
              } else
                 if ( list->terrain.restricted == 2 )
                    return 0;

               
         }
   return 0;
}


void tspeedcrccheck :: appendstring ( char* s, char* d, int id, int mode )
{
   char st[200];
   switch ( mode ) {
      case 0:sprintf ( st, "NOTE : the crc of the %s named %s , id %d was appended to the crc-list\n", s, d, id );
         break;
      case 1:sprintf ( st, "#color4#ERROR#color0#: the %s named %s , id %d , failed the crc-check\n", s, d, id );
         break;
      case 2:sprintf ( st, "#color4#ERROR#color0#: the crc of the %s named %s , id %d , is not in the crc-list\n", s, d, id );
         break;
      case 3:sprintf ( st, "#color4#ERROR#color0#: the %s named %s , id %d , has an invalid crc\n", s, d, id );
         break;
   }

   if ( !strng ) {
      strnglen = 500;
      strng = new char[strnglen];
      strng[0] = 0;
   }

#ifdef _DOS_
   if ( _heapchk() != _HEAPOK ) 
      beep();
#endif

   while ( strlen ( strng ) + strlen ( st ) > strnglen ) {
      char* tmp = strng;
      int newsize = strnglen + 500;
      strng = new char[ newsize ];
      strcpy ( strng, tmp );
      delete[] tmp;
      strnglen = newsize;
   }

   strcat ( strng, st );
}



int tspeedcrccheck :: checkunit2     ( pvehicletype f,     int add  )
{
   int s = checkunit ( f, add );
   if ( !s  &&  add == 1  ) {
      appendstring ( "unit", f->description, f->id, 1 );
      status = 1;
   }
   return s;
}

int tspeedcrccheck :: checkbuilding2 ( pbuildingtype b,    int add   )
{
   int s = checkbuilding ( b, add );
   if ( !s  &&  add == 1  ) {
      appendstring ( "building", b->name, b->id, 1 );
      status = 1;
   }
   return s;
}

int tspeedcrccheck :: checktech2     ( ptechnology t,    int add    )
{
   int s = checktech ( t, add );
   if ( !s  &&  add == 1  ) {
      appendstring ( "technology", t->name, t->id, 1 );
      status = 1;
   }
   return s;
}

int tspeedcrccheck :: checkobj2      ( pobjecttype o,    int add    )
{
   int s = checkobj ( o, add );
   if ( !s  &&  add == 1  ) {
      appendstring ( "object", o->name, o->id, 1 );
      status = 1;
   }
   return s;
}

int tspeedcrccheck :: checkterrain2  ( pterraintype b,    int add    )
{
   int s = checkterrain ( b, add );
   if ( !s  &&  add == 1 ) {
      appendstring ( "terrain", b->name, b->id, 1 );
      status = 1;
   }
   return s;
}



int  tspeedcrccheck :: getstatus ( void )
{
   if ( strng  && strng[0] ) {
      tviewanytext tvat;
      tvat.init ( "crc status", strng ,60, 90, 520, 300 );
      tvat.run();
      tvat.done();
      delete[] strng;
      strng = NULL;
   }
   return status;
}
*/
