//     $Id: typen.cpp,v 1.56 2000-10-26 18:15:03 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
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


const char*  cbodenarten[cbodenartennum]  = {"shallow water , coast"       ,    "normal lowland",   "swamp thick",       "forest",
                                             "high mountains",                  "road",             "railroad",    "entry of building (not to be used for terrain)" ,
                                             "harbour (safe for ships at storm)",                         "runway"  ,         "pipeline",    "buried pipeline",        
                                             "water",                           "deep water",       "hard sand",   "soft sand",  
                                             "track possible",                  "small rocks",      "mud",         "snow", 
                                             "deep snow",                       "mountains",        "very shallow water",
                                             "large rocks",                     "small trench",     "ditch",  "hillside",
                                             "turret foundation",               "swamp thin",       "Installation", "pack ice", "river", "frozen water" };
                                                             

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
const int  cwaffenproduktionskosten[cwaffentypennum][3]  = {{20, 15, 10}, {2, 2, 0}, {3, 2, 0}, {3, 3, 2}, {3, 3, 2}, {4, 3, 2},
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

tobjectcontainer :: tobjectcontainer ( void )
{
   minenum = 0;
   for ( int i = 0; i < maxminesonfield; i++ )
      mine[ i ] = NULL;

   objnum = 0;
}

int  tobjectcontainer :: checkforemptyness ( void )
{
   return 0;

/*
   if ( mine )
     return 0;   

    if ( special & ( cbstreet | cbrailroad | cbpowerline | cbpipeline ))
       return 0;

    return 1;
*/

}

bool tfield :: unitHere ( const pvehicle veh )
{
   if ( vehicle == veh )
      return true;
   if ( vehicle && veh && vehicle->networkid == veh->networkid )
      return true;
   return false;
}

int tfield :: getweather ( void )
{
   for ( int w = 0; w < cwettertypennum; w++ )
      if ( typ == typ->terraintype->weather[w] )
         return w;
   return -1;
}

void tfield :: sortobjects ( void )
{
   if ( object )
      for ( int a = 0; a < object->objnum-1; ) {
         if ( object->object[ a ]->typ->height > object->object[ a+1 ]->typ->height ) {
            pobject o = object->object[ a ];
            object->object[ a ] = object->object[ a+1 ];
            object->object[ a+1 ] = o;
            if ( a > 0 )
               a--;
         } else
            a++;
      }

}


int tfield :: minenum ( void )
{
  if ( object ) 
     return object->minenum;
  else
     return 0;
}

int  tfield :: putmine( int col, int typ, int strength )
{ 
  #ifndef converter
   if ( mineowner() >= 0  && mineowner() != col )
      return 0;

   if ( !object )
      object = new tobjectcontainer;

   if ( object->minenum >= maxminesonfield )
      return 0;

   /*
   if ( object->minenum >= actmap->getgameparameter ( cgp_maxminesonfield ))
      return 0;
   */

   object->mine[ object->minenum ] = new tmine; 
   object->mine[ object->minenum ]->strength = strength ;
   object->mine[ object->minenum ]->color = col;
   object->mine[ object->minenum ]->type = typ;
   if ( actmap && actmap->time.a.turn>= 0 )
      object->mine[ object->minenum ]->time = actmap->time.a.turn;
   else
      object->mine[ object->minenum ]->time = 0;

   object->minenum++;
   return 1;
  #else
   return 0;
  #endif
} 

int tfield :: mineowner( void )
{
   if ( !object )
      return -1;
   if ( !object->minenum )
      return -1;

   return object->mine[0]->color;
}


void tfield :: removemine( int num )
{ 
   if ( !object )
      return;

   if ( num == -1 )
      num = object->minenum - 1;

   if ( num >= object->minenum )
      return;

   delete object->mine[num];

   for ( int i = num+1; i < object->minenum; i++ )
      object->mine[i-1] = object->mine[i];

   object->minenum--;
   object->mine[object->minenum] = NULL;
} 



#ifdef converter

int tfield :: mineattacks ( const pvehicle veh ) { return 0; }
void tfield :: checkminetime ( int time ) { }
int tfield :: getx( void ) { return 0; }
int tfield :: gety( void ) { return 0; }

void  tfield :: addobject( pobjecttype obj, int dir, int force )
{ 
} 


void tfield :: removeobject( pobjecttype obj )
{ 
} 
#else
int tfield :: getx( void )
{
   int n = this - actmap->field;
   return n % actmap->xsize;
}

int tfield :: gety( void )
{
   int n = this - actmap->field;
   return n / actmap->xsize;
}
#endif




int tfield :: getattackbonus ( void )
{
   int a = typ->attackbonus;
   if ( object )
      for ( int i = 0; i < object->objnum; i++ ) {
         if ( object->object[i]->typ->attackbonus_abs != -1 )
            a = object->object[i]->typ->attackbonus_abs;
         else
            a += object->object[i]->typ->attackbonus_plus;
      }

   if ( a > -8 )
      return a;
   else
      return -7;
}

int tfield :: getdefensebonus ( void )
{
   int a = typ->defensebonus;
   if ( object )
      for ( int i = 0; i < object->objnum; i++ ) {
         if ( object->object[i]->typ->defensebonus_abs != -1 )
            a = object->object[i]->typ->defensebonus_abs;
         else
            a += object->object[i]->typ->defensebonus_plus;
      }

   if ( a > -8 )
      return a;
   else
      return -7;
}

int tfield :: getjamming ( void )
{
   int a = typ->basicjamming;
   if ( object )
      for ( int i = 0; i < object->objnum; i++ ) {
         if ( object->object[i]->typ->basicjamming_abs >= 0 )
            a = object->object[i]->typ->basicjamming_abs;
         else
            a += object->object[i]->typ->basicjamming_plus;
      }
   return a;
}

int tfield :: getmovemalus ( const pvehicle veh )
{       
   int mnum = minenum();
   if ( mnum ) {
      int movemalus = _movemalus[veh->typ->movemalustyp];
      int col = mineowner();
      if ( veh->color == col*8 ) 
         movemalus += movemalus * mine_movemalus_increase * mnum / 100;

      return movemalus;
   } else
      return _movemalus[veh->typ->movemalustyp];
}

int tfield :: getmovemalus ( int type )
{
  return _movemalus[type];
}

void tfield :: setparams ( void )
{
   int i;
   bdt = typ->art;

   for ( i = 0; i < cmovemalitypenum; i++ )
      _movemalus[i] = typ->movemalus[i];

   if ( object ) 
      for ( int j = 0; j < object->objnum; j++ ) {
         pobjecttype o = object->object[j]->typ;

         bdt  &=  o->terrain_and;
         bdt  |=  o->terrain_or;

         for ( i = 0; i < cmovemalitypenum; i++ ) {
            _movemalus[i] += o->movemalus_plus[i];
            if ( (o->movemalus_abs[i] != 0) && (o->movemalus_abs[i] != -1) )
               _movemalus[i] = o->movemalus_abs[i];
            if ( _movemalus[i] < minmalq )
               _movemalus[i] = minmalq;
         }
      } /* endfor */

   #ifndef converter
   if ( building ) {
      if ( this == building->getField( building->typ->entry.x, building->typ->entry.y))
         bdt |= cbbuildingentry; 

     #ifdef HEXAGON
      if ( building )
         for (int x = 0; x < 4; x++) 
            for ( int y = 0; y < 6; y++ ) 
               if ( building->getField ( x, y ) == this )
                  if ( building->getpicture ( x, y ) )
                     picture = building->getpicture ( x, y );
     #endif
   }
   #endif
}


tobject :: tobject ( void )
{
   typ = NULL;
   dir = 0;
   damage = 0;
}

tobject :: tobject ( pobjecttype t )
{
   typ = t;
   dir = 0;
   damage = 0;
}


void tobject :: setdir ( int direc )
{
   dir = direc;
}

int  tobject :: getdir ( void )
{
   return dir;
}

int  tobjecttype :: buildable ( pfield fld )
{
   if ( fld->building )
      return 0;

   #ifdef HEXAGON
   if ( terrainaccess.accessible ( fld->bdt ) <= 0 )
       return 0;
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


void tobject :: display ( int x, int y, int weather )
{
 #ifndef converter
  if ( typ->id == 7 ) {
    #ifndef HEXAGON
     for (int i = 0; i <= 7; i++) 
        if ( dir & (1 << i)) 
           putxlatfilter( x, y, typ->getpic ( i ), &xlattables.nochange[0] ); 
    #else
        if ( dir < typ->pictnum )
           putshadow  ( x, y,  typ->getpic ( dir, weather ) , &xlattables.a.dark1);
        else
           putshadow  ( x, y,  typ->getpic ( 0, weather ) , &xlattables.a.dark1);
    #endif
  } else
 #ifndef HEXAGON
  if ( typ->id == 8 ) {
     putspriteimage( x, y, typ->getpic ( 0 ) ); 
     for (int i = 0; i <= 7; i++) 
        if ( dir & (1 << i))
           putspriteimage( x, y, typ->getpic ( i + 1 ) ); 
  } else
 #endif
  if ( typ->id == 30 ) {   // pipeline
        if ( dir < typ->pictnum )
           putshadow  ( x, y,  typ->getpic ( dir, weather ) , &xlattables.a.dark1);
        else
           putshadow  ( x, y,  typ->getpic ( 0, weather ) , &xlattables.a.dark1);
  } else 
     typ->display ( x, y, dir, weather );

  #endif    //converter
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


pobject tfield :: checkforobject ( pobjecttype o )
{
   if ( object )
      return object->checkforobject( o );
   else
      return NULL;
}

pobject tobjectcontainer :: checkforobject ( pobjecttype o )
{
   for ( int i = 0; i < objnum; i++ )
      if ( object[i]->typ == o )
         return object[i];
   return NULL;
}



tresourceview :: tresourceview ( void )
{
   visible = 0;
   memset ( &fuelvisible, 0, sizeof ( fuelvisible ));
   memset ( &materialvisible, 0, sizeof ( materialvisible ));
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

void Resources :: write ( tnstream& stream )
{
   for ( int i = 0; i< resourceTypeNum; i++ )
      stream.writeInt( resource(i) );
}


////////////////////////////////////////////////////////////////////


void tresearch :: read( tnstream& stream ) {
   progress = stream.readInt();
   activetechnology = (ptechnology) stream.readInt();

   for ( int i = 0; i < waffenanzahl; i++ )
      unitimprovement.weapons[i] = stream.readWord();
   unitimprovement.armor = stream.readWord();
   for ( int j = 0; j < 44-waffenanzahl*2; j++ )
       stream.readChar(); // dummy

   techlevel = stream.readInt();
   developedtechnologies = (pdevelopedtechnologies) stream.readInt();

}

void tresearch :: write( tnstream& stream ) {
   stream.writeInt( progress );
   if ( activetechnology )
      stream.writeInt( 1 );
   else
      stream.writeInt ( 0 );

   for ( int i = 0; i < waffenanzahl; i++ )
      stream.writeWord ( unitimprovement.weapons[i] );

   stream.writeWord ( unitimprovement.armor );
   for ( int j = 0; j < 44-waffenanzahl*2; j++ )
       stream.writeChar( 0 ); // dummy

   stream.writeInt ( techlevel );
   if ( developedtechnologies )
      stream.writeInt ( 1 );
   else
      stream.writeInt ( 0 );
}



/*
void tbuilding :: repairunit(pvehicle vehicle, int maxrepair )
{
   if ( vehicle->damage ) {

      int dam;
      if ( vehicle->damage > maxrepair )
         dam = maxrepair;
      else
         dam = vehicle->damage;

      int fkost = dam * vehicle->typ->production.energy / (100 * repairefficiency_building );
      int mkost = dam * vehicle->typ->production.material / (100 * repairefficiency_building );
      int w;

      if (mkost <= getenergy ( mkost material)
         w = 10000;
      else
         w = 10000 * material / mkost;

      if (fkost > fuel)
         if (10000 * fuel / fkost < w)
            w = 10000 * fuel / fkost;


      vehicle->damage = vehicle->damage * (1 - w / 10000);
      material -= w * mkost / 10000;
      fuel -= w * fkost / 10000;
   }
}
*/






tmessage :: tmessage ( void )
{
   from = 0;
   to = 0;
   time = 0;
   text = NULL;
   id = 0;
   next = NULL;
}

#ifndef sgmain

tmessage :: tmessage ( pmap spfld )
{
   from = 0;
   to = 0;
   time = 0;
   text = NULL;
   id = 0;
   next = NULL;
   runde = 0;
   move = 0;
}

tmessage :: tmessage ( char* txt, int rec )  // f?r Meldungen vom System
{
   from = 0;
   to = 0;
   time = 0;
   text = NULL;
   id = 0;
   next = NULL;
   runde = 0;
   move = 0;
}

#else

tmessage :: tmessage ( pmap spfld  )
{
   from = 1 << spfld->actplayer;
   runde = spfld->time.a.turn;
   move = spfld->time.a.move;
   time = ::time( NULL );
   to = 0;
   text = NULL;
   spfld->messageid++;
   id = spfld->messageid;
   next = NULL;
   if ( spfld->message ) {
      pmessage mes = spfld->message;
      while ( mes->next )
         mes = mes->next;
      mes->next = this;
   } else 
     spfld->message = this;

}

extern tmap map;

tmessage :: tmessage ( char* txt, int rec )  // f?r Meldungen vom System
{
   from = 1 << 9;
   runde = actmap->time.a.turn;
   move = actmap->time.a.move;
   time = ::time( NULL );
   to = rec;
   text = txt;
   actmap->messageid++;
   id = actmap->messageid;
   next = NULL;
   if ( actmap->message ) {
      pmessage mes = actmap->message;
      while ( mes->next )
         mes = mes->next;
      mes->next = this;
   } else 
     actmap->message = this;

    for ( int i = 0; i < 8; i++ )
       if ( to & ( 1 << i )) {
          pmessagelist dst = new tmessagelist ( &actmap->player[ i ].unreadmessage );
          dst->message = this;
       }   
}


#endif

tmessage :: ~tmessage()
{
   if ( text ) {
      delete[] text;
      text = NULL;
   }
}


tmessagelist :: tmessagelist ( pmessagelist* prv )
{
   message = NULL;
   pmessagelist p = *prv;
   if ( p )
      while ( p->next )
         p = p->next;

   prev = p;

   if ( p ) {
      next = p->next;
      if ( next )
         next->prev = this;
      p->next = this;
   
   } else {
      next = NULL;
      *prv = this;
   }
   
}

tmessagelist :: ~tmessagelist ()
{

   if ( prev )
      prev->next = next;

   if ( next )
      next->prev = prev;

}

int tmessagelist :: getlistsize ( void )
{
   if ( next || prev )
      return 1;
   else
      return 0;
}

/*    
tfieldarray :: fieldl1 :: fieldl1 ( void )
{
   for (int i = 0; i < 1024; i++) {
      fielder[i] = NULL;
   } 
}


tfieldarray :: tfieldarray ( int size )
{
   fieldl2 = new fieldl1[ size / 1024 + 1 ];
}      

void tfieldarray :: alloc ( int size )
{
   fieldl2 = new fieldl1[ size / 1024 + 1 ];
}      

tfieldarray :: tfieldarray ( void )
{
   fieldl2 = NULL;
}      

void tfieldarray :: free ( void )
{
   if ( fieldl2 )
      delete fieldl2;
   fieldl2 = NULL;
}

tfieldarray :: ~tfieldarray()
{
//   fieldl2 = NULL;
//   delete fieldl2;
}
*/


int  ttechnology :: getlvl( void )
{
   if ( lvl == -1 ) {
      lvl = 0;
      for (int l = 0; l <= 5; l++) 
         if ( requiretechnology[l] ) 
            lvl += requiretechnology[l]->getlvl();
   }
   return lvl;
}

/*
tgameoptions :: tgameoptions ( void )
{
    setdefaults ( );
}

void tgameoptions :: setdefaults ( void )
{
   version = 102;
   fastmove = 1;
   mouse.scrollbutton = 0;
   mouse.fieldmarkbutton = 2;
   mouse.smallguibutton = 1;
   mouse.largeguibutton = -1;
   mouse.smalliconundermouse = 2;
   mouse.centerbutton = 4;
   container.autoproduceammunition = 1;
   smallguiiconopenaftermove = 0;
   startupcount = 0;

   visibility_calc_algo = 0;
   container.emptyeverything = 1;
   container.filleverything = 1;
   movespeed = 20;
   changed = 0;
   bi3.dir = NULL;
   strcpy ( filename, "sg.cfg" );
   onlinehelptime = 150;

   memset ( dummy, 0, sizeof ( dummy ));
   memset ( dummy2, 0, sizeof ( dummy2 ));
   memset ( container.dummy, 0, sizeof ( container.dummy ));
   memset ( mouse.dummy, 0, sizeof ( mouse.dummy ));

   bi3.interpolate.terrain = 1;
   bi3.interpolate.units = 0;
   bi3.interpolate.objects = 0;
   bi3.interpolate.buildings = 1; 

   dontMarkFieldsNotAccessible_movement = 0;

   mapzoom = 80;
   disablesound = 0;
}
*/

int twind::operator== ( const twind& b ) const
{
   return ( (speed == b.speed) && ( direction == b.direction));
}



tnetworkcomputer :: tnetworkcomputer ( void )
{
   #ifdef sgmain
   send.transfermethod = defaultnetworkconnection;
   send.transfermethodid = defaultnetworkconnection->getid();
   memset ( send.data, 0, sizeof ( send.data ));

   receive.transfermethod = defaultnetworkconnection;
   receive.transfermethodid = defaultnetworkconnection->getid();
   memset ( receive.data, 0, sizeof ( receive.data ));
   #endif

   name = NULL;
   existent = 0;
}

tnetworkcomputer :: ~tnetworkcomputer ( )
{
   #ifdef sgmain
   if ( send.transfermethod ) {
      if ( send.transfermethod->transferopen () )
         send.transfermethod->closetransfer();
      if ( send.transfermethod->connectionopen () )
         send.transfermethod->closeconnection();
   }

   if ( receive.transfermethod ) {
      if ( receive.transfermethod->transferopen () )
         receive.transfermethod->closetransfer();
      if ( receive.transfermethod->connectionopen () )
         receive.transfermethod->closeconnection();
   }
   #endif

}


tnetwork :: tnetwork ( void )
{
  for (int i = 0; i < 8; i++) {
     player[i].compposition = 0;
     player[i].codewordcrc = 0;
  } /* endfor */
  computernum = 0;
  turn = 0;
  globalparams.enablesaveloadofgames = 0;
  globalparams.reaskpasswords = 0;
}

void      twterraintype  ::   paint ( int x1, int y1 )
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

tobjectcontainercrcs :: tobjectcontainercrcs ( void )
{
   speedcrccheck = NULL;
}

tcrcblock :: tcrcblock ( void )
{
   crcnum = 0;
   crc = NULL;
   restricted = 0;
}

treplayinfo :: treplayinfo ( void )
{
   for (int i = 0; i < 8; i++) {
      guidata[i] = NULL;
      map[i] = NULL;
   }
   actmemstream = NULL;
}

treplayinfo :: ~treplayinfo ( )
{
   #ifndef converter
   for (int i = 0; i < 8; i++)  {
      if ( guidata[i] ) {
         delete guidata[i];
         guidata[i] = NULL;
      }
      if ( map[i] ) {
         delete map[i];
         map[i] = NULL;
      }
  }
  if ( actmemstream ) {
     delete actmemstream ;
     actmemstream = NULL;
  }
  #endif
}

int tterrainbits :: toand ( tterrainbits bts )
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

int tterrainaccess :: accessible ( tterrainbits bts )
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


               tterrainbits& operator~ ( tterrainbits &tb ) 
               { 
                  tterrainbits tb2 = tb;
                  tterrainbits tbs  ( ~(tb2.terrain1), ~(tb2.terrain2) );
                  // tterrainbits tbs2 ( ~tb.terrain1, ~tb.terrain2 );
                  #if (__WATCOM_CPLUSPLUS__ >= 1100 )
                     return tbs; 
                  #else
                     return tbs |= tbs;
                  #endif
               };


               tterrainbits& operator| ( tterrainbits tb2, tterrainbits tb3 ) 
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
               int operator& ( tterrainbits tb2, tterrainbits tb3 ) 
               { 
                  return tb2.toand ( tb3 ) ;
               };
               tterrainbits& operator^ ( tterrainbits tb2, tterrainbits tb3 ) 
               { 
                  tterrainbits tb = tb2;
                  return tb ^=tb3;
               };






int tresearch :: technologyresearched ( int id )
{                                                        
      pdevelopedtechnologies devtech = developedtechnologies;
      while (devtech) {
         if (devtech->tech->id == id )
               return 1;
         devtech = devtech->next;
      } /* endwhile */

      return 0;
}


/*
tmap :: tmap ( void )
{

}

tmap :: tmap ( const tmap &map )
{

}

tmap :: ~tmap ( )
{
}
*/

#ifndef converter
void tmap :: calculateAllObjects ( void )
{
   calculateallobjects();
}
#else
void tmap :: calculateAllObjects ( void )
{
}
#endif

pfield  tmap :: getField(int x, int y)
{
   if ((x < 0) || (y < 0) || (x >= xsize) || (y >= ysize))
      return NULL;
   else
      return (   &field[y * xsize + x] );
}

int tmap :: isResourceGlobal ( int resource )
{
   if ( resource != 1 && !(resource == 2 && getgameparameter(cgp_globalfuel)==0)  &&   _resourcemode == 1 )
      return 1;
   else
      return 0;
}

int tmap :: getgameparameter ( int num )
{
  if ( game_parameter && num < gameparameter_num ) {

     if ( num == cgp_maxminesonfield ) 
        if ( game_parameter[num] > maxminesonfield )
           return maxminesonfield;

     return game_parameter[num];
  } else 
     if ( num < gameparameternum )
        return gameparameterdefault[ num ];
     else
        return 0;
}

void tmap :: setgameparameter ( int num, int value )
{
   if ( game_parameter ) {
     if ( num < gameparameter_num ) 
        game_parameter[num] = value;
     else {
        int* oldparam = game_parameter;
        game_parameter = new int[num+1];
        for ( int i = 0; i < gameparameter_num; i++ )
           game_parameter[i] = oldparam[i];
        for ( int j = gameparameter_num; j < num; j++ )
           if ( j < gameparameternum )
              game_parameter[j] = gameparameterdefault[j];
           else
              game_parameter[j] = 0;
        game_parameter[num] = value;
        gameparameter_num = num + 1;
        delete[] oldparam;
     }
   } else {
       game_parameter = new int[num+1];
       for ( int j = 0; j < num; j++ )
          if ( j < gameparameternum )
             game_parameter[j] = gameparameterdefault[j];
          else
             game_parameter[j] = 0;
       game_parameter[num] = value;
       gameparameter_num = num + 1;
   }
}

void tmap :: setupResources ( void )
{
  #ifndef converter
   for ( int n = 0; n< 8; n++ ) {
      actmap->bi_resource[n].energy = 0;
      actmap->bi_resource[n].material = 0;
      actmap->bi_resource[n].fuel = 0;

     #ifdef sgmain

      for ( pbuilding bld = actmap->player[n].firstbuilding; bld ; bld = bld->next )
         for ( int r = 0; r < 3; r++ )
            if ( actmap->isResourceGlobal( r )) {
               actmap->bi_resource[n].resource(r) += bld->actstorage.resource(r);
               bld->actstorage.resource(r) = 0;
            }
     #endif
   }
  #endif
}

void tmap :: chainunit ( pvehicle eht )
{
   if ( eht ) {
      eht->next = player[ eht->color / 8 ].firstvehicle;
      if ( eht->next )
         eht->next->prev = eht;
      eht->prev = NULL;
      player[ eht->color / 8 ].firstvehicle = eht;
      if ( eht->typ->loadcapacity > 0) 
         for ( int i = 0; i <= 31; i++) 
            if ( eht->loading[i] ) 
               chainunit ( eht->loading[i] );
   }
}

void tmap :: chainbuilding ( pbuilding bld )
{
   if ( bld ) {
      bld->next = player[ bld->color / 8 ].firstbuilding;
      if ( bld->next )
         bld->next->prev = bld;
      bld->prev = NULL;
      player[ bld->color / 8 ].firstbuilding = bld;
   }
}

const char* tmap :: getPlayerName ( int playernum )
{
   if ( playernum >= 8 )
      playernum /= 8;

   switch ( player[playernum].stat ) {
      case 0: return humanplayername[playernum]; 
      case 1: return computerplayername[playernum]; 
      default: return "off";
   } /* endswitch */
}



int tmap :: eventpassed( int saveas, int action, int mapid )
{ 
   return eventpassed ( (action << 16) | saveas, mapid );
}



int tmap :: eventpassed( int id, int mapid )
{ 
  pevent       ev2; 
  peventstore  oldevent; 
  char      b; 
  word         i; 

   b = false; 
   if ( !mapid ) {
      ev2 = firsteventpassed; 
      while (ev2 != NULL) { 
         if (ev2->id == id)
            return 1; 
         ev2 = ev2->next; 
      } 
   }

   if (b == false) { 
      oldevent = oldevents; 
      while ( oldevent ) { 
         if (oldevent->num > 0) 
            for (i = 0; i < oldevent->num ; i++) 
               if (oldevent->eventid[i] == id && oldevent->mapid[i] == mapid )
                  return 1; 
      } 
   } 
   return 0; 
} 


pvehicle tmap :: getUnit ( pvehicle eht, int nwid )
{
   if ( !eht )
      return NULL;
   else {
      if ( eht->networkid == nwid )
         return eht;
      else
         for ( int i = 0; i < 32; i++ )
            if ( eht->loading[i] )
               if ( eht->loading[i]->networkid == nwid )
                  return eht->loading[i];
               else {
                  pvehicle ld = getUnit ( eht->loading[i], nwid );
                  if ( ld )
                     return ld;
               }
      return NULL;
   }
}

pvehicle tmap :: getUnit ( int nwid )
{
   for ( int i = 0; i < 9; i++ ) {
      pvehicle veh = player[i].firstvehicle;
      while ( veh ) {
         if ( veh->networkid == nwid )
            return veh;
         veh = veh->next;
      };
   }
   return NULL;
}


pvehicle tmap :: getUnit ( int x, int y, int nwid )
{
  #ifndef converter
   pfield fld  = getfield ( x, y );
   if ( !fld )
      return NULL;

   if ( !fld->vehicle )
      if ( fld->building ) {
         for ( int i = 0; i < 32; i++ ) {
            pvehicle ld = getUnit ( fld->building->loading[i], nwid );
            if ( ld )
               return ld;
         }
         return NULL;
      } else
         return NULL;
   else
      if ( fld->vehicle->networkid == nwid )
         return fld->vehicle;
      else
         return getUnit ( fld->vehicle, nwid );
 #else
  return NULL;
 #endif
}



pvehicletype tmap :: getVehicleType_byId ( int id )
{
   #ifdef converter
   return NULL;
   #else
   return getvehicletype_forid ( id, 0 );
   #endif
}




int tresearch :: vehicleclassavailable ( const pvehicletype fztyp , int classnm, pmap map  )
{
   if ( fztyp->classbound[classnm].techlevel )
      if ( fztyp->classbound[classnm].techlevel <= techlevel )
         return true;


   int  i;

   #ifndef karteneditor
   for (i=0;i<4 ;i++ ) 
      if (fztyp->classbound[classnm].techrequired[i])
         if ( !technologyresearched ( fztyp->classbound[classnm].techrequired[i] ))
            return false;

/*
   if ( fztyp->classbound[classnm].eventrequired )
      if (!map->eventpassed (fztyp->classbound[classnm].eventrequired ))
         return false;
*/
   #endif

   for (i=0; i< waffenanzahl; i++)
      if ( fztyp->classbound[classnm].weapstrength[i] > unitimprovement.weapons[i] )
         return false;


   if ( fztyp->classbound[classnm].armor > unitimprovement.armor )
      return false;

   return true;

}


int tresearch :: vehicletypeavailable ( const pvehicletype fztyp, pmap map  )
{
   if ( !fztyp )
      return 0;
   else
      return vehicleclassavailable( fztyp, 0, map );
}


tshareview :: tshareview ( const tshareview* org ) 
{ 
   memcpy ( mode, org->mode, sizeof ( mode ));
   recalculateview = org->recalculateview; 
}


teventtrigger_polygonentered :: teventtrigger_polygonentered ( void )
{
   size = 0;
   vehicle = NULL;
   vehiclenetworkid = 0;
   data = NULL;
   tempnwid = 0;
   tempxpos = 0;
   tempypos = 0;
   color = 0;
   memset ( reserved, 0, sizeof ( reserved ));
}

teventtrigger_polygonentered :: teventtrigger_polygonentered ( const teventtrigger_polygonentered& poly )
{
   size = poly.size;
   vehicle = poly.vehicle;
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
teventtrigger_polygonentered :: ~teventtrigger_polygonentered ( )
{
   if ( data ) {
      delete[] data;
      data = NULL;
   }
}


LargeTriggerData :: LargeTriggerData ( void )
{
   time.abstime = 0;
   xpos = -1;
   ypos = -1;
   building = NULL;
   vehicle = NULL;
   mapid = 0;  
   id = -1;  
   unitpolygon = NULL;  
   networkid = 0;
   memset ( reserved, 0, sizeof ( reserved ));
}

LargeTriggerData :: LargeTriggerData ( const LargeTriggerData& data )
{
   time = data.time;
   xpos = data.xpos;
   ypos = data.ypos;
   networkid = data.networkid;
   building = data.building;
   vehicle = data.vehicle;
   mapid = data.mapid;
   id = data.id;
   if ( data.unitpolygon ) {
      unitpolygon = new teventtrigger_polygonentered ( *data.unitpolygon );
   } else
     unitpolygon = NULL;
}

LargeTriggerData :: ~LargeTriggerData ( )
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

AiThreat& AiThreat::operator+= ( const AiThreat& t )
{
   for ( int i = 0; i < threatTypes; i++ )
      threat[i] += t.threat[i];
   return *this;
}


// is there any way to do this test at compile time ??
structure_size_tester sst1;

int getheightdelta ( int height1, int height2 )
{
   int ah = height1;
   int dh = height2;
   int hd = dh - ah;
 
   if ( ah >= 3 && dh <= 2 ) 
      hd++;
   if (dh >= 3 && ah <= 2 )
      hd--;
 
   return hd;
}
