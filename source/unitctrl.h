//     $Id: unitctrl.h,v 1.30 2003-02-19 19:47:26 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.29  2002/09/19 20:20:06  mbickel
//      Cleanup and various bug fixes
//
//     Revision 1.28  2002/03/26 22:23:09  mbickel
//      Fixed: music was started even if turned off in ini file
//      Fixed: crash in reaction fire
//      Fixed: reaction fire not occuring when changing height
//
//     Revision 1.27  2001/11/28 13:03:16  mbickel
//      Fixed: attack selectable although 0% hit accuracy
//      Fixed: refuelling not possible if weapon had wrong target height
//      Fixed: repair icon displayed when refuelling
//
//     Revision 1.26  2001/11/15 20:46:05  mbickel
//      Fixed: replay not working when moving units out of carriers
//
//     Revision 1.25  2001/09/25 15:13:07  mbickel
//      New version number
//      Fixed crash when reaction fire during ascend
//
//     Revision 1.24  2001/03/30 12:43:16  mbickel
//      Added 3D pathfinding
//      some cleanup and documentation
//      splitted the ai into several files, now located in the ai subdirectory
//      AI cares about airplane servicing and range constraints
//
//     Revision 1.23  2001/02/04 21:27:02  mbickel
//      The AI status is written to savegames -> new savegame revision
//      Lots of bug fixes
//
//     Revision 1.22  2001/01/25 23:45:06  mbickel
//      Moved map displaying routins to own file (mapdisplay.cpp)
//      Wrote program to create pcx images from map files (map2pcx.cpp)
//      Fixed bug in repair function: too much resource consumption
//      AI improvements and bug fixes
//      The BI3 map import function now evaluates the player status (human/
//       computer)
//
//     Revision 1.21  2001/01/19 13:33:57  mbickel
//      The AI now uses hemming
//      Several bugfixes in Vehicle Actions
//      Moved all view calculation to viewcalculation.cpp
//      Mapeditor: improved keyboard support for item selection
//
//     Revision 1.20  2000/12/28 16:58:39  mbickel
//      Fixed bugs in AI
//      Some cleanup
//      Fixed crash in building construction
//
//     Revision 1.19  2000/11/21 20:27:11  mbickel
//      Fixed crash in tsearchfields (used by object construction for example)
//      AI improvements
//      configure.in: added some debug output
//                    fixed broken check for libbz2
//
//     Revision 1.18  2000/11/14 20:36:47  mbickel
//      The AI can now use supply vehicles
//      Rewrote objecttype IO routines to make the structure independant of
//       the memory layout
//
//     Revision 1.17  2000/11/11 11:05:21  mbickel
//      started AI service functions
//
//     Revision 1.16  2000/11/08 19:31:18  mbickel
//      Rewrote IO for the tmap structure
//      Fixed crash when entering damaged building
//      Fixed crash in AI
//      Removed item CRCs
//
//     Revision 1.15  2000/10/31 10:42:48  mbickel
//      Added building->vehicle service to vehicle controls
//      Moved tmap methods to gamemap.cpp
//
//     Revision 1.14  2000/10/11 15:33:48  mbickel
//      Adjusted small editors to the new ASC structure
//      Watcom compatibility
//
//     Revision 1.13  2000/10/11 14:26:53  mbickel
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
//     Revision 1.12  2000/09/27 16:08:32  mbickel
//      AI improvements
//
//     Revision 1.11  2000/09/25 20:04:43  mbickel
//      AI improvements
//
//     Revision 1.10  2000/09/25 13:25:55  mbickel
//      The AI can now change the height of units
//      Heightchaning routines improved
//
//     Revision 1.9  2000/09/24 19:57:06  mbickel
//      ChangeUnitHeight functions are now more powerful since they use
//        UnitMovement on their own.
//
//     Revision 1.8  2000/09/17 15:20:38  mbickel
//      AI is now automatically invoked (depending on gameoptions)
//      Some cleanup
//
//     Revision 1.7  2000/07/16 14:20:06  mbickel
//      AI has now some primitive tactics implemented
//      Some clean up
//        moved weapon functions to attack.cpp
//      Mount doesn't modify PCX files any more.
//
//     Revision 1.6  2000/06/09 10:51:00  mbickel
//      Repaired keyboard control of pulldown menu
//      Fixed compile errors at fieldlist with gcc
//
//     Revision 1.5  2000/06/08 21:03:44  mbickel
//      New vehicle action: attack
//      wrote documentation for vehicle actions
//
//     Revision 1.4  2000/06/04 21:39:22  mbickel
//      Added OK button to ViewText dialog (used in "About ASC", for example)
//      Invalid command line parameters are now reported
//      new text for attack result prediction
//      Added constructors to attack functions
//
//     Revision 1.3  2000/05/23 20:40:53  mbickel
//      Removed boolean type
//
//     Revision 1.2  2000/05/06 20:25:26  mbickel
//      Fixed: -recognition of a second mouse click when selection a pd menu item
//             -movement: fields the unit can only pass, but not stand on them,
//                        are marked darker
//             -intedit/stredit: mouseclick outside is like hitting enter
//
//     Revision 1.1  2000/04/27 16:25:31  mbickel
//      Attack functions cleanup
//      New vehicle categories
//      Rewrote resource production in ASC resource mode
//      Improved mine system: several mines on a single field allowed
//      Added unitctrl.* : Interface for vehicle functions
//        currently movement and height change included
//      Changed timer to SDL_GetTicks
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

#ifndef unitctrlH
#define unitctrlH

#include <vector>
#include <map>

#include "typen.h"
#include "mapalgorithms.h"
#include "basestrm.h"
#include "spfst.h"
#include "attack.h"
#include "mapdisplay.h"
#include "astar2.h"


/** \file unitctrl.h
   New system for actions performed by units
*/



template<class T>
class FieldList {
       pmap localmap;
       int fieldnum;
       std::vector<int> xpos;
       std::vector<int> ypos;
       std::vector<T>   data;
     public:
       FieldList ( void );
       int getFieldNum ( void ) const;
       pfield getField ( int num ) const;
       T& getData ( int num );
       T& getData ( int x, int y );
       void getFieldCoordinates ( int num, int* x, int* y ) const;
       void addField ( int x, int y, T& _data );
       void addField ( const MapCoordinate& mc, T& _data );
       void addField ( int x, int y );
       void addField ( const MapCoordinate& mc );
       void setMap ( pmap map );
       pmap getMap ( void );
       bool isMember ( int x, int y );
       bool isMember ( const MapCoordinate& mc );
     };


typedef FieldList<int> IntFieldList;
typedef FieldList<AttackWeap> AttackFieldList;


typedef class PendingVehicleActions* PPendingVehicleActions;


enum VehicleActionType { vat_nothing, vat_move, vat_ascent, vat_descent, vat_attack, vat_service };

class VehicleAction {
           protected:
              PPendingVehicleActions pva; 
              VehicleActionType actionType;
           public:
              virtual int getStatus( void ) = 0;
              virtual int available ( pvehicle veh ) const = 0;
              virtual int execute ( pvehicle veh, int x, int y, int step, int param1, int param2 ) = 0;
              virtual void registerPVA ( VehicleActionType _actionType, PPendingVehicleActions _pva );
              VehicleAction ( VehicleActionType _actionType, PPendingVehicleActions _pva  );
              virtual ~VehicleAction ( );
         };

/* Usage of VehicleActions:
 *   The contstructor usually requires passing of MapDisplayInterface* which is a class that controls the visual 
 *    output of the vehicle action. "defaultMapDisplay" is a global object that handles the output. When NULL is passed, 
 *    any graphical output is disabled. 
 *   available ( unit ) checks, whether this functions is available or not. If it is available, it can be exectuted
 *    with several calls to:
 *   execute ( ... ). Most vehicle actions are divided into several tasks, which have to be executed one after another.
 *    getStatus() return the current task number, which has to be passed to execute. I recommend to pass hardcoded constants
 *    to execute instead of getStatus() to make sure the caller and the action are in sync.
 *    The parameters for execute are explained for each vehicle function; -1 is used for parameters that are not used
 *    If an error occurs, the result of execute will be negative and indicate an appropriate error message 
 *    (file message1.txt). You can get it with getmessage( -result ) or directly print it to the status line with 
 *    dispmessage2 ( -result );
 *    A result of 1000 shows that the action is completed.
 *   registerPVA is usually not necessary to be called externally
 */



typedef int trichtungen[sidenum];


class BaseVehicleMovement : public VehicleAction {
           protected:
              MapDisplayInterface* mapDisplay;
              int status;
           public:
              BaseVehicleMovement ( VehicleActionType _actionType, PPendingVehicleActions _pva, MapDisplayInterface* md ) : VehicleAction ( _actionType, _pva ),attackedByReactionFire(false), mapDisplay(md), status(0) {};
              AStar3D::Path path;
              int execute ( pvehicle veh, int x, int y, int step, int height, int noInterrupt );
              bool attackedByReactionFire;
              pvehicle getVehicle ( void ) { return vehicle; };
              void registerMapDisplay ( MapDisplayInterface* _mapDisplay ) { mapDisplay = _mapDisplay; };
              virtual int getStatus ( void ) { return status; };
              int available ( pvehicle veh ) const;


            protected:
               pvehicle vehicle;

               int moveunitxy ( AStar3D::Path& pathToMove, int noInterrupt = -1 );

               class PathFinder : public AStar3D {
                 public:
                   PathFinder ( pmap actmap, pvehicle veh, int maxDistance, bool changeHeight_ ) : AStar3D(actmap, veh, false, maxDistance, changeHeight_ ) {};

                   /** searches for all fields that are within the range of maxDist and marks them.
                       On each field one bit for each level of height will be set.
                       The Destructor removes all marks.
                   */
                   void getMovementFields ( IntFieldList& reachableFields, IntFieldList& reachableFieldsIndirect, int height );
              };
          };


class VehicleMovement : public BaseVehicleMovement {
           public:
              IntFieldList reachableFields;
              IntFieldList reachableFieldsIndirect;
              int available ( pvehicle veh ) const;
              int execute ( pvehicle veh, int x, int y, int step, int height, int noInterrupt );

              virtual void registerPVA ( VehicleActionType _actionType, PPendingVehicleActions _pva );
              VehicleMovement ( MapDisplayInterface* md, PPendingVehicleActions _pva = NULL );
              ~VehicleMovement ( );
          };

/* VehicleMovement:
 *
 *   Step 0:   execute ( vehicle, -1, -1, step = 0 , height, startheight );
 *                 height is usually -1, which means the aircraft will move on the same level of height it currently has
 *                        But there are situations like starting aircraft from carriers where it has to be set to the height 
 *                        the aircraft should move on after start. Don't use this height for regular height changing!
 *
 *             the fields that are reachable will be stored in "reachableFields". "reachableFieldsIndirect" contains fields
 *                 the unit could move to if the field was empty
 *
 *   Step 2:   execute ( NULL, destination-x, destination-y, step = 2, -1, -1 );
 *                The destination must be one of the fields in "reachableFields"
 *                the effect of this step is that "path" contains the fields that the unit will move over to reach the 
 *                 destination.
 *            
 *   Step 3:   execute ( NULL, destination-x, destination-y, step = 3, -1, NoInterrupt );
 *                The destination must be one of the fields in "path"
 *                If NoInterrupt is != -1 the unit will not stop when being attacked by mines and reactionfire.
 */



class ChangeVehicleHeight : public BaseVehicleMovement {
              int newheight;
              int dir;
           public:
              IntFieldList reachableFields;
              int execute ( pvehicle veh, int x, int y, int step, int noInterrupt, int dummy2 );
              ChangeVehicleHeight ( MapDisplayInterface* md, PPendingVehicleActions _pva , VehicleActionType vat, int dir_ );
          };

class IncreaseVehicleHeight : public ChangeVehicleHeight {
           public:
              IncreaseVehicleHeight ( MapDisplayInterface* md, PPendingVehicleActions _pva = NULL );
              int available ( pvehicle veh ) const;
              ~IncreaseVehicleHeight();
          };

class DecreaseVehicleHeight : public ChangeVehicleHeight {
           public:
              DecreaseVehicleHeight ( MapDisplayInterface* md, PPendingVehicleActions _pva = NULL );
              int available ( pvehicle veh ) const;
              ~DecreaseVehicleHeight();
          };


/* IncreaseVehicleHeight / DecreaseVehicleHeight:
 *
 *   Step 0:   execute ( vehicle, -1, -1, step = 0 , newheight, allFields );
 *                 newheight should be vehicle->height >> 1 for DecreaseVehicleHeight and
 *                                     vehicle->height << 1 for IncreaseVehicleHeight.
 *                 allFields: 1 all fields are reported that can be reached with moving the unit first
 *                            <= 0 ; only fields are reported that the unit can reach changing its
 *                                   height without moving first
 *                 Depending of the units ability to change its height vertically (for example helicopter and submarine) or
 *                   by moving a distance ( normal airplanes ), execute will either immediatly change the units height and
 *                   finish (status == 1000), or follow the same procedure as VehicleMovement.
 *   (Step 2)  see VehicleMovement
 *               if a movement is executed prior to changing the height (allFields == 1 in step 0),
 *               step 2 and step 3 of the movement are executed here !
 *   (Step 3)  see VehicleMovement
 */



class VehicleAttack : public VehicleAction {
              pvehicle vehicle;
              int status;
              int kamikaze;
              class tsearchattackablevehicles : public SearchFields {
                                  VehicleAttack* va;
                               public:
                                  int       anzahlgegner;
                                  pvehicle  angreifer;
                                  int       kamikaze;
                                  void            init ( const pvehicle eht, int _kamikaze, VehicleAttack* _va );
                                  virtual void    testfield ( const MapCoordinate& mc );
                                  int             run ( void );
                                  tsearchattackablevehicles ( pmap _gamemap ) : SearchFields ( _gamemap ) {};
                          } search;

           protected:
              MapDisplayInterface* mapDisplay;
           public:
              AttackFieldList attackableVehicles;
              AttackFieldList attackableBuildings;
              AttackFieldList attackableObjects;

              int getStatus( void ) { return status; };
              virtual int available ( pvehicle veh ) const;
              virtual int execute ( pvehicle veh, int x, int y, int step, int _kamikaze, int weapnum );
              virtual void registerPVA ( VehicleActionType _actionType, PPendingVehicleActions _pva );
              VehicleAttack ( MapDisplayInterface* md, PPendingVehicleActions _pva = NULL );
              virtual ~VehicleAttack ( );
         };



/* VehicleAttack:
 *
 *   Step 0:   execute ( vehicle, -1, -1, step = 0 , kamikaze, -1 );
 *                 kamikaze attack is not implemented yet. Always pass 0
 *                        
 *                 "attackableVehicles", "attackableBuildings", "attackableObjects" contains the possible targets of the 
 *                   unit, along with information about the weapon(s) which can be used for the attack
 *
 *   Step 2:   execute ( NULL, target-x, target-y, step = 2, -1, weapnum );
 *                The target destination must be part of one of the "attackable*" fields.
 *                weapnum may either be a specific weapon or -1 if the most powerful one should be used
 */



class VehicleService : public VehicleAction {
              pvehicle vehicle;
              pbuilding building;
              int status;

          public:
              class FieldSearch : public SearchFields {
                     VehicleService& vs;
                     pvehicle        veh;
                     pbuilding       bld;
                  public:
                     struct {
                       bool distance;
                       bool height;
                     } bypassChecks;
                     virtual void     testfield ( const MapCoordinate& mc );
                     void             checkVehicle2Vehicle ( pvehicle veh, int xp, int yp );
                     void             checkBuilding2Vehicle ( pvehicle veh );
                     bool             initrefuelling( int xp1, int yp1 );
                     void             init ( pvehicle _veh, pbuilding _bld );
                     void             run (  );
                     FieldSearch ( VehicleService& _vs, pmap _gamemap ) : SearchFields ( _gamemap ), vs ( _vs ) { bypassChecks.distance = false; bypassChecks.height = false; };
                  } fieldSearch;


           protected:
              MapDisplayInterface* mapDisplay;
           public:
              pvehicle getVehicle ( void ) { return vehicle; };
              pbuilding getBuilding ( void ) { return building; };

              enum Service { srv_repair, srv_resource, srv_ammo };
              class Target {
                 public:
                    pvehicle dest;

                    struct Service {
                      VehicleService::Service type;
                      int sourcePos;  //!< weapon position; resourceType
                      int targetPos;
                      int curAmount;  //!< current amount at target
                      int maxAmount;  //!< maximum amount at target
                      int minAmount;  //!< minimum amount at target
                      int maxPercentage;
                      int orgSourceAmount;
                    };
                    vector<Service> service;
              };
              typedef map<int,Target> TargetContainer;
              TargetContainer dest;

              int getStatus( void ) { return status; };
              virtual int available ( pvehicle veh ) const;
              int getServices ( pvehicle veh ) const;
              int execute ( pvehicle veh, int targetNWID, int dummy, int step, int pos, int amount );
              int fillEverything ( int targetNWID, bool repairsToo );
              virtual void registerPVA ( VehicleActionType _actionType, PPendingVehicleActions _pva );
              VehicleService ( MapDisplayInterface* md, PPendingVehicleActions _pva = NULL );
              virtual ~VehicleService ( );

              //! here the gui stores which icon initiated the service; thus preventing the occurance of an repair icon when a fuel transfer was initiated.
              int guimode;
         };



/* VehicleService:
 *
 *   Step 0:   execute ( vehicle, -1, -1, step = 0 , -1, -1 );
 *                 if vehicle is going to service some units
 *                  OR
 *             execute ( NULL, xpos, ypos, step = 0 , -1, -1 );
 *                 if the building at xpos, ypos is going to service some units
 *
 *                 dest then contains all units that can be services together with the services
 *                 Use the units networkID as Index
 *
 *
 *   Step 2:   execute ( NULL, targetNWID, -1, step = 2, pos, amount );
 *                 targetNWID is the unit that is going to be serviced
 *                 pos is the position of the service: dest[targetNWID].service[pos]
 *                 amount is the amount of the service that is going to be transferred
 */




class PendingVehicleActions {
          public:
            int actionType;
            PendingVehicleActions ( void );
            VehicleAction* action;

            VehicleMovement* move;
            IncreaseVehicleHeight* ascent;
            DecreaseVehicleHeight* descent;
            VehicleAttack* attack;
            VehicleService* service;
            ~PendingVehicleActions ( );
         };

//! a structure to keep track of the vehicleActions that are currently running
extern PendingVehicleActions pendingVehicleActions;


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Template implementations
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

template<class T> FieldList<T> :: FieldList ( void )
{
   fieldnum = 0;
   localmap = NULL;
}

template<class T> int FieldList<T> :: getFieldNum ( void ) const
{
   return fieldnum;
}

template<class T> pfield FieldList<T> :: getField ( int num ) const
{
   if ( num < fieldnum && num >= 0 )
      return getfield ( xpos[num], ypos[num] );
   else
      return NULL;
}


template<class T> T& FieldList<T> :: getData ( int num )
{
   if ( num < fieldnum && num >= 0 )
      return data[num] ;

   throw OutOfRange();
}

template<class T> T& FieldList<T> :: getData ( int x, int y )
{
   for ( int i = 0; i < fieldnum; i++ )
      if ( xpos[i] == x && ypos[i] == y )
         return data[i];

   throw OutOfRange();
}


template<class T> void FieldList<T> :: getFieldCoordinates ( int num, int* x, int* y ) const
{
   if ( num < fieldnum && num >= 0 ) {
      *x = xpos[num];
      *y = ypos[num];
   } else {
      *x = -1;
      *y = -1;
   }
}


template<class T> void FieldList<T> :: addField ( const MapCoordinate& mc, T& _data )
{
   addField ( mc.x, mc.y, _data );
}

template<class T> void FieldList<T> :: addField ( int x, int y, T& _data )
{
   int found = 0;
   for( int i = 0; i < fieldnum; i++ )
      if ( xpos[i] == x && ypos[i] == y )
         found = 1;
   if ( !found ) {
      xpos.push_back ( x );
      ypos.push_back ( y );
      data.push_back( _data );

      fieldnum++;
   }
}



template<class T> void FieldList<T> :: addField ( const MapCoordinate& mc )
{
   addField ( mc.x, mc.y );
}

template<class T> void FieldList<T> :: addField ( int x, int y )
{
   int found = 0;
   for( int i = 0; i < fieldnum; i++ )
      if ( xpos[i] == x && ypos[i] == y )
         found = 1;
   if ( !found ) {
      xpos.push_back ( x );
      ypos.push_back ( y );
      T t;
      data.push_back ( t );
      fieldnum++;
   }
}


template<class T> void FieldList<T> :: setMap ( pmap map )
{
   localmap = map;
}

template<class T> pmap FieldList<T> :: getMap ( void )
{
   return localmap;
}

template<class T> bool FieldList<T> :: isMember ( int x, int y )
{
   for ( int i = 0; i < fieldnum; i++ )
      if ( xpos[i] == x && ypos[i] == y )
         return true;
   return false;
}

template<class T> bool FieldList<T> :: isMember ( const MapCoordinate& mc )
{
   for ( int i = 0; i < fieldnum; i++ )
      if ( xpos[i] == mc.x && ypos[i] == mc.y )
         return true;
   return false;
}

#endif

