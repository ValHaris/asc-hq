//     $Id: typen.cpp,v 1.91 2002-11-15 20:54:11 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.90  2002/10/06 15:44:40  mbickel
//      Completed inheritance of .asctxt files
//      Speed up of replays
//
//     Revision 1.89  2002/03/27 00:18:21  mbickel
//      Changed the resource weight
//
//     Revision 1.88  2002/03/18 21:42:17  mbickel
//      Some cleanup and documentation in the Mine class
//      The number of mines is now displayed in the field information window
//
//     Revision 1.87  2002/02/21 17:06:52  mbickel
//      Completed Paragui integration
//      Moved mail functions to own file (messages)
//
//     Revision 1.86  2001/12/19 17:16:29  mbickel
//      Some include file cleanups
//
//     Revision 1.85  2001/12/14 10:20:05  mbickel
//      Cleanup and enhancements to configure.in
//      Removed last remains of octagonal version from source files
//
//     Revision 1.84  2001/11/22 13:49:32  mbickel
//      Fixed crash in Mapeditor when selection color 9
//      Fixed: turrets being displayed gray
//      Fixed: division by 0 by AI movement
//
//     Revision 1.83  2001/10/03 20:56:06  mbickel
//      Updated data files
//      Updated online help
//      Clean up of Pulldown menues
//      Weapons can now have different efficiencies against different unit classes
//
//     Revision 1.82  2001/10/02 14:06:29  mbickel
//      Some cleanup and documentation
//      Bi3 import tables now stored in .asctxt files
//      Added ability to choose amoung different BI3 import tables
//      Added map transformation tables
//
//     Revision 1.81  2001/09/13 17:43:12  mbickel
//      Many, many bug fixes
//
//     Revision 1.80  2001/08/19 10:48:49  mbickel
//      Fixed display problems in event dlg in mapeditor
//      Fixed error when starting campaign with AI as first player
//
//     Revision 1.79  2001/07/27 21:13:35  mbickel
//      Added text based file formats
//      Terraintype and Objecttype restructured
//
//     Revision 1.78  2001/07/14 19:13:16  mbickel
//      Rewrote sound system
//      Moveing units make sounds
//      Added sound files to data
//
//     Revision 1.77  2001/07/13 12:53:01  mbickel
//      Fixed duplicate icons in replay
//      Fixed crash in tooltip help
//
//     Revision 1.76  2001/07/11 20:44:37  mbickel
//      Removed some vehicles from the data file.
//      Put all legacy units in into the data/legacy directory
//
//     Revision 1.75  2001/04/03 11:54:16  mbickel
//      AI Improvements: production , servicing
//
//     Revision 1.74  2001/03/23 16:02:56  mbickel
//      Some restructuring;
//      started rewriting event system
//
//     Revision 1.73  2001/02/26 12:35:33  mbickel
//      Some major restructuing:
//       new message containers
//       events don't store pointers to units any more
//       tfield class overhauled
//
//     Revision 1.72  2001/02/18 15:37:21  mbickel
//      Some cleanup and documentation
//      Restructured: vehicle and building classes into separate files
//         tmap, tfield and helper classes into separate file (gamemap.h)
//      basestrm : stream mode now specified by enum instead of int
//
//     Revision 1.71  2001/02/11 11:39:44  mbickel
//      Some cleanup and documentation
//
//     Revision 1.70  2001/02/06 16:27:41  mbickel
//      bugfixes, bugfixes and bugfixes
//
//     Revision 1.69  2001/02/04 21:27:00  mbickel
//      The AI status is written to savegames -> new savegame revision
//      Lots of bug fixes
//
//     Revision 1.68  2001/02/01 22:48:51  mbickel
//      rewrote the storing of units and buildings
//      Fixed MapEventTriggerbugs in bi3 map importing routines
//      Fixed bugs in AI
//      Fixed bugs in mapeditor
//
//     Revision 1.67  2001/01/28 23:00:42  mbickel

//      Made the small editors compilable with Watcom again
//
//     Revision 1.66  2001/01/28 20:42:16  mbickel
//      Introduced a new string class, ASCString, which should replace all
//        char* and std::string in the long term
//      Split loadbi3.cpp into 3 different files (graphicselector, graphicset)
//
//     Revision 1.65  2001/01/23 21:05:22  mbickel
//      Speed up of AI
//      Lot of bugfixes in AI
//      Moved Research to own files (research.*)
//      Rewrote storing of developed technologies
//      Some cleanup and documentation
//
//     Revision 1.64  2001/01/22 20:00:10  mbickel
//      Fixed bug that made savegamefrom campaign games unloadable
//      Optimized the terrainAccess-checking
//
//     Revision 1.63  2001/01/21 16:37:22  mbickel
//      Moved replay code to own file ( replay.cpp )
//      Fixed compile problems done by cleanup
//
//     Revision 1.62  2000/12/26 14:46:02  mbickel
//      Made ASC compilable (and runnable) with Borland C++ Builder
//
//     Revision 1.61  2000/12/21 11:00:50  mbickel
//      Added some code documentation
//
//     Revision 1.60  2000/11/14 20:36:44  mbickel
//      The AI can now use supply vehicles
//      Rewrote objecttype IO routines to make the structure independant of
//       the memory layout
//
//     Revision 1.59  2000/11/08 19:37:39  mbickel
//      Changed the terrain types (again): "lava" now replaces "small trench"
//
//     Revision 1.58  2000/11/08 19:31:16  mbickel
//      Rewrote IO for the tmap structure
//      Fixed crash when entering damaged building
//      Fixed crash in AI
//      Removed item CRCs
//
//     Revision 1.57  2000/10/31 10:42:47  mbickel
//      Added building->vehicle service to vehicle controls
//      Moved tmap methods to gamemap.cpp
//
//     Revision 1.56  2000/10/26 18:15:03  mbickel
//      AI moves damaged units to repair
//      tmap is not memory layout sensitive any more
//
//     Revision 1.55  2000/10/18 17:09:42  mbickel
//      Fixed eventhandling for DOS
//
//     Revision 1.54  2000/10/12 22:24:02  mbickel
//      Made the DOS part of the new platform system work again
//
//     Revision 1.53  2000/10/11 15:33:46  mbickel
//      Adjusted small editors to the new ASC structure
//      Watcom compatibility
//
//     Revision 1.52  2000/10/11 14:26:51  mbickel
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
//     Revision 1.51  2000/09/27 16:08:30  mbickel
//      AI improvements
//
//     Revision 1.50  2000/09/25 20:04:41  mbickel
//      AI improvements
//
//     Revision 1.49  2000/09/25 15:06:00  mbickel
//      Some fixes for Watcom
//
//     Revision 1.48  2000/09/25 13:25:53  mbickel
//      The AI can now change the height of units
//      Heightchaning routines improved
//
//     Revision 1.47  2000/09/07 15:49:47  mbickel
//      some cleanup and documentation
//
//     Revision 1.46  2000/09/01 17:46:43  mbickel
//      Improved A* code
//      Renamed tvehicle class to Vehicle
//
//     Revision 1.45  2000/08/21 17:51:02  mbickel
//      Fixed: crash when unit reaching max experience
//      Fixed: crash when displaying research image
//      Fixed: crash when events referenced a unit that has been shot down
//      Fixed: screenshot being written to wrong directory
//
//     Revision 1.44  2000/08/13 10:24:09  mbickel
//      Fixed: movement decrease when cloning units
//      Fixed: refuel skipped next action in replay
//
//     Revision 1.43  2000/08/12 09:17:37  gulliver
//     *** empty log message ***
//
//     Revision 1.42  2000/08/11 12:24:07  mbickel
//      Fixed: no movement after refuelling unit
//      Restructured reading/writing of units
//
//     Revision 1.41  2000/08/10 10:20:18  mbickel
//      Added building function "produce all unit types"
//
//     Revision 1.40  2000/08/09 12:39:34  mbickel
//      fixed invalid height when constructing vehicle with other vehicles
//      fixed wrong descent icon being shown
//
//     Revision 1.39  2000/08/08 13:38:40  mbickel
//      Fixed: construction of buildings doesn't consume resources
//      Fixed: no unit information visible for satellites
//
//     Revision 1.38  2000/08/08 13:22:12  mbickel
//      Added unitCategoriesLoadable property to buildingtypes and vehicletypes
//      Added option: showUnitOwner
//
//     Revision 1.37  2000/08/08 09:48:30  mbickel
//
//      speed up of dialog boxes in linux
//      fixed graphical errors in attack
//      fixed graphical error in ammo transfer
//      fixed reaction fire not allowing manual attack
//
//     Revision 1.36  2000/08/07 16:29:23  mbickel
//      orbiting units don't consume fuel any more
//      Fixed bug in attack formula; improved attack formula
//      Rewrote reactionfire
//
//     Revision 1.35  2000/08/06 12:18:10  mbickel
//      Gameoptions: new default values
//      Maketank: negative buildable-object-IDs
//
//     Revision 1.34  2000/08/06 11:39:21  mbickel
//      New map paramter: fuel globally available
//      Mapeditor can now filter buildings too
//      Fixed unfreed memory in fullscreen image loading
//      Fixed: wasted cpu cycles in building
//      map parameters can be specified when starting a map
//      map parameters are reported to all players in multiplayer games
//
//     Revision 1.33  2000/08/05 15:30:31  mbickel
//      Fixed possible divisions by 0 in attack/defensebonus
//
//     Revision 1.32  2000/08/05 13:38:42  mbickel
//      Rewrote height checking for moving units in and out of
//        transports / building
//
//     Revision 1.31  2000/08/04 15:11:24  mbickel
//      Moving transports costs movement for units inside
//      refuelled vehicles now have full movement in the same turn
//      terrain: negative attack / defensebonus allowed
//      new mapparameters that affect damaging and repairing of building
//
//     Revision 1.30  2000/08/03 19:21:33  mbickel
//      Fixed: units had invalid height when produced in some buildings
//      Fixed: units could not enter building if unitheightreq==0
//      Started adding SDL_image support
//      Upgraded to SDL1.1.3 (for SDL_image)
//
//     Revision 1.29  2000/08/03 13:12:20  mbickel
//      Fixed: on/off switching of generator vehicle produced endless amounts of energy
//      Repairing units now reduces their experience
//      negative attack- and defenseboni possible
//      changed attackformula
//
//     Revision 1.28  2000/08/02 10:28:27  mbickel
//      Fixed: generator vehicle not working
//      Streams can now report their name
//      Field information shows units filename
//
//     Revision 1.27  2000/07/29 14:54:49  mbickel
//      plain text configuration file implemented
//
//     Revision 1.26  2000/07/05 10:49:37  mbickel
//      Fixed AI bugs
//      setbuildingdamage event now updates the screen
//
//     Revision 1.25  2000/07/05 09:24:01  mbickel
//      New event action: change building damage
//
//     Revision 1.24  2000/07/04 18:39:41  mbickel
//      Added weapon information in buildings
//      Made buried pipeline darker
//
//     Revision 1.23  2000/07/02 21:04:14  mbickel
//      Fixed crash in Replay
//      Fixed graphic errors in replay
//
//     Revision 1.22  2000/06/28 19:26:18  mbickel
//      fixed bug in object generation by building removal
//      Added artint.cpp to makefiles
//      Some cleanup
//
//     Revision 1.21  2000/06/28 18:31:03  mbickel
//      Started working on AI
//      Started making loaders independent of memory layout
//      Destroyed buildings can now leave objects behind.
//
//     Revision 1.20  2000/06/08 21:03:43  mbickel
//      New vehicle action: attack
//      wrote documentation for vehicle actions
//
//     Revision 1.19  2000/06/06 20:03:19  mbickel
//      Fixed graphical error when transfering ammo in buildings
//      Sound can now be disable by a command line parameter and the game options
//
//     Revision 1.18  2000/05/23 20:40:52  mbickel
//      Removed boolean type
//
//     Revision 1.17  2000/05/22 15:40:37  mbickel
//      Included patches for Win32 version
//
//     Revision 1.16  2000/05/07 12:12:18  mbickel
//      New mouse option dialog
//      weapon info can now be displayed by clicking on a unit
//
//     Revision 1.15  2000/05/06 20:25:25  mbickel
//      Fixed: -recognition of a second mouse click when selection a pd menu item
//             -movement: fields the unit can only pass, but not stand on them,
//                        are marked darker
//             -intedit/stredit: mouseclick outside is like hitting enter
//
//     Revision 1.14  2000/05/06 19:57:11  mbickel
//      Mapeditor/linux is now running
//
//     Revision 1.13  2000/04/27 16:25:30  mbickel
//      Attack functions cleanup
//      New vehicle categories
//      Rewrote resource production in ASC resource mode
//      Improved mine system: several mines on a single field allowed
//      Added unitctrl.* : Interface for vehicle functions
//        currently movement and height change included
//      Changed timer to SDL_GetTicks
//
//     Revision 1.12  2000/03/16 14:06:56  mbickel

//      Added unitset transformation to the mapeditor
//
//     Revision 1.11  2000/01/31 16:08:39  mbickel
//      Fixed crash in line
//      Improved error handling in replays
//
//     Revision 1.10  2000/01/25 19:28:16  mbickel
//      Fixed bugs:
//        invalid mouse buttons reported when moving the mouse
//        missing service weapon in weapon information
//        invalid text displayed in ammo production
//        undamaged units selected in repair vehicle function
//
//      speed up when playing weapon sound
//
//     Revision 1.9  2000/01/24 17:35:48  mbickel
//      Added dummy routines for sound under DOS
//      Cleaned up weapon specification
//
//     Revision 1.8  2000/01/24 08:16:52  steb
//     Changes to existing files to implement sound.  This is the first munge into
//     CVS.  It worked for me before the munge, but YMMV :)
//
//     Revision 1.7  2000/01/06 11:19:16  mbickel
//      Worked on the Linux-port again...
//
//     Revision 1.6  1999/12/27 13:00:14  mbickel
//      new vehicle function: each weapon can now be set to not attack certain
//                            vehicles
//
//     Revision 1.5  1999/12/07 22:02:08  mbickel
//      Added vehicle function "no air refuelling"
//
//     Revision 1.4  1999/11/23 21:07:38  mbickel
//      Many small bugfixes
//
//     Revision 1.3  1999/11/16 17:04:17  mbickel
//     Made ASC compilable for DOS again :-)
//     Merged all the bug fixes in that I did last week
//
//     Revision 1.2  1999/11/16 03:42:44  tmwilson
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

#include <stdio.h>
#include <cstring>

#include "global.h"
#include "misc.h"
#include "typen.h"
#include "graphicset.h"
#include "basegfx.h" 

#ifndef converter
#include "spfst.h"
#endif

#ifdef sgmain
 #include "network.h"
 #include "gameoptions.h"
#endif

#include "vehicletype.h"
#include "buildingtype.h"
#include "textfileparser.h"
#include "textfile_evaluation.h"

//! The different levels of height
const char*  choehenstufen[choehenstufennum] = {"deep submerged", "submerged", "floating", "ground level", "low-level flight", "flight", "high-level flight", "orbit"}; 

//! The different connections of something, be it unit, building, or field, to an event.
const char* cconnections[6]  = {"destroyed", "conquered", "lost", "seen", "area entererd by any unit", "area entered by specific unit"};

const char* ceventtriggerconn[8]  = {"AND ", "OR ", "NOT ", "( ", "(( ", ")) ", ") ", "Clear "}; 

//! All actions that can be performed by events
const char* ceventactions[ceventactionnum]  = {"message", "weather change", "new technology discovered", "lose campaign", "run script + next map",
                                               "new technology researchable", "map change", "discarded [ was erase event ]", "end campaign", "next map",
                                               "reinforcement","weather change completed", "new vehicle developed","palette change",
                                               "alliance change","wind change", "nothing", "change game parameter","paint ellipse","remove ellipse",
                                               "change building damage"};

const char* ceventtrigger[ceventtriggernum]  = {"*NONE*", "turn/move >=", "building conquered", "building lost",
                                                 "building destroyed", "unit lost", "technology researched",
                                                 "event", "unit conquered", "unit destroyed", "all enemy units destroyed",
                                                 "all units lost", "all enemy buildings destroyed/captured", "all buildings lost", 
                                                 "energy tribute <", "material tribute <", "fuel tribute <",
                                                 "any unit enters polygon", "specific unit enters polygon", "building is seen", "irrelevant (used internally)"};



const char*  cwaffentypen[cwaffentypennum]  = {"cruise missile", "mine",    "bomb",       "air - missile", "ground - missile", "torpedo", "machine gun",
                                               "cannon",         "service", "ammunition refuel", "laser (not implemented yet!)", "shootable"};

const char*  cmovemalitypes[cmovemalitypenum] = { "default",
                                                 "light tracked vehicle", "medium tracked vehicle", "heavy tracked vehicle",
                                                 "light wheeled vehicle", "medium wheeled vehicle", "heavy wheeled vehicle",
                                                 "trooper",               "rail vehicle",           "medium aircraft",
                                                 "medium ship",           "building / turret / object", "light aircraft",
                                                 "heavy aircraft",        "light ship",             "heavy ship",  "helicopter",
                                                 "hoovercraft"  };

const char* cnetcontrol[cnetcontrolnum] = { "store energy",           "store material",           "store fuel",           
                                            "move out all energy",           "move out all material",           "move out all fuel", 
                                            "stop storing energy", "stop storing material", "stop storing fuel", 
                                            "stop energy extraction", "stop material extraction", "stop fuelextraction" };
const char* cgeneralnetcontrol[4] = {       "store",  "move out", "stop storing", "stop extraction" };
                                          // Functionen in Geb„uden ....

const char*  cwettertypen[cwettertypennum] = {"dry (standard)","light rain", "heavy rain", "few snow", "lot of snow", "lot of snow + ice"};
const char*  resourceNames[3]  = {"energy", "material", "fuel"}; 
const int  cwaffenproduktionskosten[cwaffentypennum][3]    = {{20, 15, 10}, {2, 2, 0}, {3, 2, 0}, {3, 3, 2}, {3, 3, 2}, {4, 3, 2},
                                                              {1, 1, 0},    {1, 2, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}; // jeweils f?r weaponpackagesize Pack !

//! when repairing a unit, the experience of the unit is decreased by one when passing each of these damage levels
const int experienceDecreaseDamageBoundaries[experienceDecreaseDamageBoundaryNum] = { 80, 60, 40, 20 };

const int directionangle [ sidenum ] = 
 { 0, -53, -127, -180, -180 -53 , -180 -127 };

const int resourceWeight[ resourceTypeNum ] = { 0, 10, 4 };




const int csolarkraftwerkleistung[cwettertypennum] = { 1024, 512, 256, 756, 384 }; // 1024 ist Maximum







ResourceMatrix :: ResourceMatrix ( const float* f )
{
   int num = 0;
   for ( int i = 0; i < resourceTypeNum; i++ )
      for ( int j = 0; j < resourceTypeNum; j++ )
         e[i][j] = f[num++];
}


Resources ResourceMatrix :: operator* ( const Resources& r ) const
{
   Resources res;
   for ( int i = 0; i < resourceTypeNum; i++ )
      for ( int j = 0; j < resourceTypeNum; j++ )
         res.resource(i) += int( e[i][j] * r.resource(j));

   return res;
}


void Resources :: read ( tnstream& stream )
{
   for ( int i = 0; i< resourceTypeNum; i++ )
      resource(i) = stream.readInt();
}

void Resources :: write ( tnstream& stream ) const
{
   for ( int i = 0; i< resourceTypeNum; i++ )
      stream.writeInt( resource(i) );
}


Resources operator- ( const Resources& res1, const Resources& res2 )
{
   Resources res = res1;
   res -= res2;
   return res;
}

Resources operator+ ( const Resources& res1, const Resources& res2 )
{
   Resources res = res1;
   res += res2;
   return res;
}


Resources Resources::operator* ( double d )
{
   Resources rs;
   for ( int i = 0; i < resourceTypeNum; i++ )
      rs.resource(i) = int( resource(i)*d );
   return rs;
}


void Resources::runTextIO ( PropertyContainer& pc )
{
   pc.addInteger  ( "Energy", energy );
   pc.addInteger  ( "Material", material );
   pc.addInteger  ( "fuel", fuel );
}


////////////////////////////////////////////////////////////////////



tevent::LargeTriggerData::PolygonEntered :: PolygonEntered ( void )
{
   size = 0;
   vehiclenetworkid = 0;
   data = NULL;
   tempnwid = 0;
   tempxpos = 0;
   tempypos = 0;
   color = 0;
   memset ( reserved, 0, sizeof ( reserved ));
}

tevent::LargeTriggerData::PolygonEntered :: PolygonEntered ( const tevent::LargeTriggerData::PolygonEntered& poly )
{
   size = poly.size;
   vehiclenetworkid = poly.vehiclenetworkid;
   if ( poly.data ) {
      data = new int [size];
      for ( int i = 0; i < size; i++ )
         data[i] = poly.data[i];
   } else
      data = NULL;
   tempnwid = poly.tempnwid;
   tempxpos = poly.tempxpos;
   tempypos = poly.tempypos;
   color = poly.color;
   memcpy ( reserved, poly.reserved, sizeof ( reserved ));

}
tevent::LargeTriggerData::PolygonEntered :: ~PolygonEntered ( )
{
   if ( data ) {
      delete[] data;
      data = NULL;
   }
}


tevent::LargeTriggerData :: LargeTriggerData ( void )
{
   time.abstime = 0;
   xpos = -1;
   ypos = -1;
   building = NULL;
   mapid = 0;
   id = -1;  
   unitpolygon = NULL;  
   networkid = 0;
   memset ( reserved, 0, sizeof ( reserved ));
}

tevent::LargeTriggerData :: LargeTriggerData ( const LargeTriggerData& data )
{
   time = data.time;
   xpos = data.xpos;
   ypos = data.ypos;
   networkid = data.networkid;
   building = data.building;
   mapid = data.mapid;
   id = data.id;
   if ( data.unitpolygon ) {
      unitpolygon = new tevent::LargeTriggerData::PolygonEntered ( *data.unitpolygon );
   } else
      unitpolygon = NULL;
}

tevent::LargeTriggerData :: ~LargeTriggerData ( )
{
   if ( unitpolygon ) {
      delete unitpolygon;
      unitpolygon = NULL;
   }
}

tevent :: tevent ( void )
{
   a.action= 255;
   a.saveas = 0;
   a.num = 0;
   player = 0; 
   rawdata = NULL;
   next = NULL;
   datasize = 0;
   conn = 0;
   for ( int i = 0; i < 4; i++) { 
      trigger[i] = 0;
      triggerstatus[i] = 0;
      triggerconnect[i] = 0; 
      triggertime.set( -1, -1 );
      trigger_data[i] = new LargeTriggerData;
   }
   delayedexecution.turn = 0;
   delayedexecution.move = 0;
   description[0] = 0;
   triggertime.abstime = -1;
}

tevent :: tevent ( const tevent& event )
{
   id = event.id;
   player = event.player;
   strcpy ( description, event.description );
   rawdata = event.rawdata;
   datasize  = event.datasize;
   if ( datasize && event.rawdata ) {
      chardata = new char [ datasize ];
      memcpy ( rawdata, event.rawdata, datasize );
   } else
      datasize = 0;
   next = event.next;
   conn = event.conn;
   for ( int i = 0; i < 4; i++ ) {
      trigger[i] = event.trigger[i];
      if ( event.trigger_data[i] )
         trigger_data[i] = new LargeTriggerData ( *event.trigger_data[i] );
      else
         trigger_data[i] = NULL;

      triggerconnect[i] = event.triggerconnect[i];
      triggerstatus[i] = event.triggerstatus[i]; 
   }
   triggertime = event.triggertime;
   delayedexecution = event.delayedexecution;
}


tevent :: ~tevent ()
{
   for ( int i = 0; i < 4; i++ )
      if ( trigger_data[i] ) {
        delete trigger_data[i];
        trigger_data[i] = NULL;
      }

   if ( chardata ) {
      delete[]  chardata ;
      chardata = NULL;
   }
}




