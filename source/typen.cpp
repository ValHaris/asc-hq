//     $Id: typen.cpp,v 1.76 2001-07-11 20:44:37 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
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
#include <string.h>

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
const char*  cminentypen[cminenum]  = {"antipersonnel mine", "antitank mine", "moored mine", "floating mine"};
const int cminestrength[cminenum]  = { 60, 120, 180, 180 };


const char*  cbodenarten[cbodenartennum]  = {"shallow water , coast"       ,
                                             "normal lowland",
                                             "swamp thick",
                                             "forest",
                                             "high mountains",
                                             "road",
                                             "railroad",
                                             "entry of building (not to be used for terrain)" ,
                                             "harbour (safe for ships at storm)",
                                             "runway"  ,
                                             "pipeline",
                                             "buried pipeline",
                                             "water",
                                             "deep water",
                                             "hard sand",
                                             "soft sand",
                                             "track possible",
                                             "small rocks",
                                             "mud",
                                             "snow",
                                             "deep snow",
                                             "mountains",
                                             "very shallow water",
                                             "large rocks",
                                             "lava",
                                             "ditch",
                                             "hillside",
                                             "turret foundation",
                                             "swamp thin",
                                             "Installation",
                                             "pack ice",
                                             "river",
                                             "frozen water" };
                                                             

const char*  cwaffentypen[cwaffentypennum]  = {"cruise missile", "mine",    "bomb",       "air - missile", "ground - missile", "torpedo", "machine gun",
                                               "cannon",         "service", "ammunition refuel", "laser (not implemented yet!)", "shootable"};

const char*  cmovemalitypes[cmovemalitypenum] = { "default",
                                                 "light tracked vehicle", "medium tracked vehicle", "heavy tracked vehicle",
                                                 "light wheeled vehicle", "medium wheeled vehicle", "heavy wheeled vehicle",
                                                 "trooper",               "rail vehicle",           "medium aircraft",
                                                 "medium ship",           "building / turret / object", "light aircraft",
                                                 "heavy aircraft",        "light ship",             "heavy ship" };

const char* cnetcontrol[cnetcontrolnum] = { "store energy",           "store material",           "store fuel",           
                                            "move out all energy",           "move out all material",           "move out all fuel", 
                                            "stop storing energy", "stop storing material", "stop storing fuel", 
                                            "stop energy extraction", "stop material extraction", "stop fuelextraction" };
const char* cgeneralnetcontrol[4] = {       "store",  "move out", "stop storing", "stop extraction" };
                                          // Functionen in Geb„uden ....

const char*  cwettertypen[cwettertypennum] = {"dry (standard)","light rain", "heavy rain", "few snow", "lot of snow", "fog (don't use!!)"};
const char*  resourceNames[3]  = {"energy", "material", "fuel"}; 
const int  cwaffenproduktionskosten[cwaffentypennum][3]    = {{20, 15, 10}, {2, 2, 0}, {3, 2, 0}, {3, 3, 2}, {3, 3, 2}, {4, 3, 2},
                                                              {1, 1, 0},    {1, 2, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}; // jeweils f?r weaponpackagesize Pack !

//! when repairing a unit, the experience of the unit is decreased by one when passing each of these damage levels
const int experienceDecreaseDamageBoundaries[experienceDecreaseDamageBoundaryNum] = { 80, 60, 40, 20 };

const int directionangle [ sidenum ] = 
#ifdef HEXAGON
 { 0, -53, -127, -180, -180 -53 , -180 -127 };
#else
 { 0, -45, -90, -135, -180, - 225, - 270, -315 };
#endif

const int resourceWeight[ resourceTypeNum ] = { 0, 4, 12 };


const int gameparameterdefault [ gameparameternum ] = { 1, 2, 0, 100, 100, 1, 0, 0, 1, 0, 0, 0, 0, 100, 100, 100, 1, maxunitexperience, 0 };
const char* gameparametername[ gameparameternum ] = { "lifetime of tracks", 
                                                      "freezing time of broken ice cover ( icebreaker )",
                                                      "move vehicles from unaccessible fields",
                                                      "building construction material factor (percent)",
                                                      "building construction fuel factor (percent)",
                                                      "forbid construction of buildings",
                                                      "forbid units to build units",
                                                      "use BI3 style training factor ",
                                                      "maximum number of mines on a single field",
                                                      "lifetime of antipersonnel mine",
                                                      "lifetime of antitank mine",
                                                      "lifetime of moored mine",
                                                      "lifetime of floating mine",
                                                      "building armor factor (percent)", 
                                                      "max building damage repair / turn",
                                                      "building repair cost increase (percent)",
                                                      "fuel globally available (BI resource mode)",
                                                      "maximum experience that can be gained by training",
                                                      "initial map visibility"};


const int csolarkraftwerkleistung[cwettertypennum] = { 1024, 512, 256, 756, 384 }; // 1024 ist Maximum





int  tobjecttype :: buildable ( pfield fld )
{
   #ifndef converter
   if ( fld->building )
      return 0;

   #ifdef HEXAGON
   if ( terrainaccess.accessible ( fld->bdt ) <= 0 )
       return 0;
   #endif

   #endif
   return 1;
}


void* tobjecttype :: getpic ( int i, int w )
{
   #ifdef HEXAGON
   if ( !(weather & (1 << w )))
      w = 0;

   if ( pictnum <= i )
      i = 0;

   return picture [w][ i ].picture;

   #else
   return picture [ i  ];
   #endif
}




void tobjecttype :: display ( int x, int y, int dir, int weather )
{
   #ifndef converter
  if ( id == 1 || id == 2 ) {
     putspriteimage ( x, y,  getpic( dir, weather ) );
  } else
#ifdef HEXAGON
  if ( id == 4 ) {
#else
  if ( id == 3 ||  id == 4 ) {
#endif

     if ( dir == 68 ) 
        putspriteimage ( x, y,  getpic ( 9, weather ) );
     else
     if ( dir == 34 )
        putspriteimage ( x, y,  getpic ( 10, weather ) );
     else
     if ( dir == 17 ) 
        putspriteimage ( x, y,  getpic ( 11, weather ) );
     else
     if ( dir == 136) 
        putspriteimage ( x, y,  getpic ( 12, weather ) );
     else
     if ( dir == 0) 
        putspriteimage ( x, y,  getpic ( 0, weather ) );
     else
        for (int i = 0; i <= 7; i++) 
           if ( dir & (1 << i))
              putspriteimage( x, y,  getpic ( i + 1, weather ) ); 

  } else
  if (  id == 5 ) {
     putspriteimage  ( x, y,  getpic ( 0, weather ) );
  } else
  #ifndef HEXAGON
  if (  id == 6  ||  id == 7 || id == 8 ) {
     for (int i = 0; i <= 7; i++) 
        if ( dir & (1 << i))
           putspriteimage( x, y,  getpic ( i, weather ) ); 
  } else 
  #endif

      if ( dirlistnum ) {
         for ( int i = 0; i < dirlistnum; i++ )
            if ( dirlist [ i ] == dir ) {
               putspriteimage ( x, y, getpic ( i, weather ) );
               return;
            }

         for ( int j = 0; j < dirlistnum; j++ )
            if ( dirlist [ j ] == 0 ) {
               putspriteimage ( x, y, getpic ( j, weather ) );
               return;
            }

         putspriteimage ( x, y, getpic ( 0, weather ) );
   
      } else 
        if ( dir < pictnum )
           putspriteimage ( x, y, getpic ( dir, weather ) );
        else
           putspriteimage ( x, y, getpic ( 0, weather ) );

  #endif
}



void tobjecttype :: display ( int x, int y )
{
   display ( x, y, 34, 0 );
}



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


////////////////////////////////////////////////////////////////////





/*
Message :: Message ( void )
{
   from = 0;
   to = 0;
   time = 0;
   id = 0;
}
*/
#ifndef sgmain

Message :: Message ( pmap spfld )
{
   from = 0;
   to = 0;
   time = 0;
   id = 0;
   runde = 0;
   move = 0;
}


Message :: Message ( const ASCString& , pmap gamemap, int rec, int from )
{
   from = 0;
   to = 0;
   time = 0;
   id = 0;
   runde = 0;
   move = 0;
}

#else

Message :: Message ( pmap spfld  )
{
   from = 1 << spfld->actplayer;
   runde = spfld->time.a.turn;
   move = spfld->time.a.move;
   time = ::time( NULL );
   to = 0;
   spfld->messageid++;
   id = spfld->messageid;

   spfld->messages.push_back ( this );
}


Message :: Message ( const ASCString& msg, pmap gamemap, int rec, int _from )  // f?r Meldungen vom System
{
   from = _from;
   runde = actmap->time.a.turn;
   move = actmap->time.a.move;
   time = ::time( NULL );
   to = rec;
   text = msg;
   gamemap->messageid++;
   id = gamemap->messageid;

   gamemap->messages.push_back ( this );

   for ( int i = 0; i < 8; i++ )
      if ( to & ( 1 << i ))
         actmap->player[i].unreadmessage.push_back ( this );
}


#endif






void      TerrainType::Weather::paint ( int x1, int y1 )
{
 #ifndef converter
  #ifdef HEXAGON
   putspriteimage ( x1, y1, picture[0] );
  #else
   char* c = (char*) direcpict[0];

   if ( agmp->windowstatus == 100 ) {
      int i;
      char* buf = (char*) (agmp->scanlinelength * y1 + x1 + agmp->linearaddress) + 19;

      for (i=1; i<= 19 ;i++ ) {
        for ( int num = 0; num < i * 2; num ++ ) {
           *buf = *c;
           c++;
           buf++;
        }

        buf += agmp->scanlinelength - 2 * i - 1;
      } /* endfor */
                            
      for (i=20; i > 0 ;i-- ) {
        for ( int num = 0; num < i * 2; num ++ ) {
           *buf = *c;
           c++;
           buf++;
        }

        buf += agmp->scanlinelength - 2 * i + 1;
      } /* endfor */

   }
  #endif
 #endif
}


bool tterrainbits :: toand ( const tterrainbits& bts ) const
{
   return ( (bts.terrain1 & terrain1) || (bts.terrain2 & terrain2));
}


tterrainaccess :: tterrainaccess ( void )
                : terrain ( -1, -1 )
{
   /*
   terrain.terrain1 = -1;
   terrain.terrain2 = -1;
   terrainreq.terrain1 = 0;
   terrainreq.terrain2 = 0;
   terrainnot.terrain1 = 0;
   terrainnot.terrain2 = 0;
   terrainkill.terrain1 = 0;
   terrainkill.terrain2 = 0;
   */
}

int tterrainaccess :: accessible ( const tterrainbits& bts )
{
   if ( terrain.toand ( bts )
         &&
         bts.existall ( terrainreq )
         &&
        !terrainnot.toand ( bts )
      ) return 1;
   else
      if ( terrainkill.toand ( bts ) )
         return -1;
      else
         return 0;
}


tterrainbits cbwater0 ( 1<<22, 0 );
tterrainbits cbwater1 ( 1, 0 );
tterrainbits cbwater2 ( 4096, 0 );
tterrainbits cbwater3 ( 8192, 0 );
/*
tterrainbits cbwater ( cbwater0.terrain1 + cbwater1.terrain1 + cbwater2.terrain1 + cbwater3.terrain1, 
                       cbwater0.terrain2 + cbwater1.terrain2 + cbwater2.terrain2 + cbwater3.terrain2 );
*/


tterrainbits cbwater ( cbwater0 | cbwater1 | cbwater2 | cbwater3 );


tterrainbits cb_all ( -1, -1 );
tterrainbits cbstreet ( 32, 0 );
tterrainbits cbrailroad ( 64, 0 );
tterrainbits cbbuildingentry ( 128, 0 );
tterrainbits cbharbour ( 256, 0 );
tterrainbits cbrunway ( 512, 0 );
tterrainbits cbpipeline ( 1024, 0 );
tterrainbits cbpowerline ( 2048, 0 );
tterrainbits cbfahrspur ( 1<<16, 0 );
tterrainbits cbfestland ( cb_all  ^ cbwater );
tterrainbits cbsnow1 ( 1<<19, 0 );
tterrainbits cbsnow2 ( 1<<20, 0 );
tterrainbits cbhillside ( 1<<26, 0 );
tterrainbits cbsmallrocks ( 1<<17, 0 );
tterrainbits cblargerocks ( 1<<23, 0 );


               tterrainbits operator~ ( const tterrainbits &tb )
               { 
                  tterrainbits tbs  ( ~(tb.terrain1), ~(tb.terrain2) );
                  return tbs;
               };


               tterrainbits operator| ( const tterrainbits& tb2, const tterrainbits& tb3 )
               { 
                  tterrainbits tb = tb2;
                  return tb |=tb3;
               };
/*
               tterrainbits& operator& ( tterrainbits tb2, tterrainbits tb3 ) 
               {
                  tterrainbits tb = tb2;
                  return tb &=tb3;
               };
*/
               bool operator& ( const tterrainbits& tb2, const tterrainbits& tb3 )
               { 
                  return tb2.toand ( tb3 ) ;
               };

               tterrainbits operator^ ( const tterrainbits& tb2, const tterrainbits& tb3 )
               {
                  tterrainbits tb = tb2;
                  return tb ^=tb3;
               };




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
      triggertime.a.turn = -1;
      triggertime.a.move = -1;
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



const int object_version = 1;

void tobjecttype :: read ( tnstream& stream )
{
   int version = stream.readInt();

   if ( version == object_version ) {

       id = stream.readInt();
       weather = stream.readInt();
       visibleago = stream.readInt();
       objectslinkablenum = stream.readInt();
       bool _objectsLinkable = stream.readInt();
       objectslinkable = NULL;
       bool _oldpicture = stream.readInt();
       oldpicture = NULL;

       pictnum = stream.readInt();
       armor = stream.readInt();

       movemalus_plus_count = stream.readChar();
       movemalus_plus = NULL; stream.readInt();

       movemalus_abs_count = stream.readChar();
       movemalus_abs = NULL; stream.readInt();

       attackbonus_plus = stream.readInt();
       attackbonus_abs  = stream.readInt();

       defensebonus_plus = stream.readInt();
       defensebonus_abs =  stream.readInt();

       basicjamming_plus = stream.readInt();
       basicjamming_abs = stream.readInt();

       height = stream.readInt();

       buildcost.read( stream );
       removecost.read ( stream );
       build_movecost = stream.readInt();
       remove_movecost = stream.readInt();

       bool _name = stream.readInt();
       name = NULL;

       no_autonet = stream.readInt();

       terrainaccess.read( stream );
       terrain_and.read ( stream );
       terrain_or.read ( stream );

       bool _buildicon = stream.readInt();
       buildicon = NULL;
       bool _removeicon = stream.readInt();
       removeicon = NULL;

       bool _dirlist = stream.readInt();
       dirlist = NULL;

       dirlistnum = stream.readInt();
       bool _picture[cwettertypennum];
       for ( int aa = 0; aa < cwettertypennum; aa++ ) {
          _picture[aa] = stream.readInt();
          picture[aa] = NULL;
       }

      int copycount = 0;

      int w;
      for ( int ww = 0; ww < cwettertypennum; ww++ )
         if ( weather & ( 1 << ww ))
            if ( pictnum ) {
               picture[ww] = new thexpic[pictnum];
               for ( int n = 0; n < pictnum; n++ ) {
                  stream.readInt(); // dummy
                  picture[ww][n].bi3pic = stream.readInt();
                  picture[ww][n].flip = stream.readInt();

                  if ( picture[ww][n].bi3pic != -1 )
                     loadbi3pict_double ( picture[ww][n].bi3pic,
                                         &picture[ww][n].picture,
                                         1,  // CGameOptions::Instance()->bi3.interpolate.objects
                                         0 );
                  else
                     stream.readrlepict ( &picture[ww][n].picture, false, &w);

                 #ifndef converter
                  if ( picture[ww][n].flip == 1 ) {
                     void* buf = new char [ imagesize ( 0, 0, fieldxsize, fieldysize ) ];
                     flippict ( picture[ww][n].picture, buf , 1 );
                     asc_free ( picture[ww][n].picture );
                     picture[ww][n].picture = buf;
                     copycount++;
                  }

                  if ( picture[ww][n].flip == 2 ) {
                     void* buf = new char [ imagesize ( 0, 0, fieldxsize, fieldysize ) ];
                     flippict ( picture[ww][n].picture, buf , 2 );
                     asc_free ( picture[ww][n].picture );
                     picture[ww][n].picture = buf;
                     copycount++;
                  }

                  if ( picture[ww][n].flip == 3 ) {
                     void* buf = new char [ imagesize ( 0, 0, fieldxsize, fieldysize ) ];
                     flippict ( picture[ww][n].picture, buf , 2 );
                     flippict ( buf, picture[ww][n].picture, 1 );
                     delete[] buf;
                     copycount++;
                  }

                  if ( picture[ww][n].bi3pic == -1 )
                     picture[ww][n].flip = 0;
                 #endif 
               }
            }

      if ( copycount == 0 )
         for ( int ww = 0; ww < cwettertypennum; ww++ )
            if ( weather & ( 1 << ww ))
               if ( pictnum )
                  for ( int n = 0; n < pictnum; n++ )
                     if ( picture[ww][n].bi3pic != -1 ) {
                        asc_free ( picture[ww][n].picture );
                        loadbi3pict_double ( picture[ww][n].bi3pic,
                                             &picture[ww][n].picture,
                                             1 ); // CGameOptions::Instance()->bi3.interpolate.objects );
                     }

       #ifndef converter
       int mmcount = cmovemalitypenum;

       if (mmcount < movemalus_plus_count )
          mmcount = movemalus_plus_count;
       #else
       int mmcount = movemalus_plus_count ;
       #endif

       movemalus_plus = new char[ mmcount ] ;

       int j;
       for (j=0; j< mmcount ; j++ ) {
          if (j < movemalus_plus_count )
             movemalus_plus[j] = stream.readChar();
          else
             if( j > 0 )
               movemalus_plus[j] = movemalus_plus[0];
             else
               movemalus_plus[j] = 0;

       }
       movemalus_plus_count = mmcount;



       #ifndef converter
       mmcount = cmovemalitypenum;

       if (mmcount < movemalus_abs_count )
          mmcount = movemalus_abs_count;
       #else
       mmcount = movemalus_abs_count ;
       #endif

       movemalus_abs = new char[ mmcount ] ;

       for (j=0; j< mmcount ; j++ ) {
          if (j < movemalus_abs_count )
             movemalus_abs[j] = stream.readChar();
          else
             if( j > 0 )
                movemalus_abs[j] = movemalus_abs[0];
             else
                movemalus_abs[j] = 0;
       }

       movemalus_abs_count = mmcount;

       if ( _name )
          stream.readpchar ( &name );


      if ( dirlistnum ) {
         dirlist = new int[ dirlistnum ];
         for ( int i = 0; i < dirlistnum; i++ )
            dirlist[i] = stream.readInt();
      }


      if ( objectslinkablenum ) {
         objectslinkableid = new int[ objectslinkablenum ];
         for ( int i = 0; i < objectslinkablenum; i++ )
             objectslinkableid[i] = stream.readInt();
      }

       buildicon = NULL;
       removeicon = NULL;
   } else
       throw ASCmsgException ( "invalid object file format version");
}

void tobjecttype :: write ( tnstream& stream )
{

    stream.writeInt ( object_version );

    stream.writeInt ( id);
    stream.writeInt ( weather );
    stream.writeInt ( visibleago );
    stream.writeInt ( objectslinkablenum );
    stream.writeInt ( -1 ); // was: objectslinkable
    stream.writeInt ( -1 ); // was: oldpicture
    stream.writeInt ( pictnum );
    stream.writeInt ( armor );

    stream.writeChar ( movemalus_plus_count );
    stream.writeInt ( -1 ); // was movemalus_plus

    stream.writeChar ( movemalus_abs_count );
    stream.writeInt ( -1 ); // was movemalus_abs

    stream.writeInt ( attackbonus_plus );
    stream.writeInt ( attackbonus_abs );
    stream.writeInt ( defensebonus_plus );
    stream.writeInt ( defensebonus_abs );

    stream.writeInt ( basicjamming_plus );
    stream.writeInt ( basicjamming_abs );

    stream.writeInt ( height );

    buildcost.write( stream );
    removecost.write ( stream );
    stream.writeInt( build_movecost );
    stream.writeInt( remove_movecost );

    stream.writeInt ( -1 ); // was name
    stream.writeInt ( no_autonet );

    terrainaccess.write( stream );
    terrain_and.write ( stream );
    terrain_or.write ( stream );

    stream.writeInt( -1 ); // was buildicon
    stream.writeInt( -1 ); // was removeicon

    stream.writeInt( -1 ); // was dirlist

    stream.writeInt ( dirlistnum );
    for ( int aa = 0; aa < cwettertypennum; aa++ )
       stream.writeInt( -1 ); // was picture

    for ( int ww = 0; ww < cwettertypennum; ww++ )
       if ( weather & ( 1 << ww ))
          for ( int l = 0; l < pictnum; l++ ) {
             stream.writeInt ( -1 );
             stream.writeInt ( picture[ww][l].bi3pic );
             stream.writeInt ( picture[ww][l].flip );
             if ( picture[ww][l].bi3pic == -1 )
                stream.writedata( picture[ww][l].picture, getpicsize2 ( picture[ww][l].picture) );
          }

    if ( movemalus_plus_count )
       for ( int i = 0; i < movemalus_plus_count; i++ )
           stream.writeInt ( movemalus_plus[i] );

    if ( movemalus_abs_count )
       for ( int i = 0; i < movemalus_abs_count; i++ )
           stream.writeInt ( movemalus_abs[i] );

    if ( name )
       stream.writepchar ( name );

    if ( dirlistnum )
       for ( int i = 0; i < dirlistnum; i++ )
          stream.writeInt ( dirlist[i] );

    if ( objectslinkablenum )
       for ( int i = 0; i < objectslinkablenum; i++ )
           stream.writeInt( objectslinkableid[i] );

}



