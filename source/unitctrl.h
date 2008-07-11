
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
#include "astar2.h"
#include "actions/servicing.h"


/** \file unitctrl.h
   system for actions performed by units
   This design is considered a failure, it won't be used for any new code
*/

#ifdef karteneditor
#error The mapeditor does not need any pathfinding
#endif


extern SigC::Signal0<void> fieldCrossed;


template<class T>
class FieldList {
       GameMap* localmap;
       int fieldnum;
       std::vector<int> xpos;
       std::vector<int> ypos;
       std::vector<T>   data;
     public:
       FieldList ( void );
       int getFieldNum ( void ) const;
       tfield* getField ( int num ) const;
       T& getData ( int num );
       T& getData ( int x, int y );
       void getFieldCoordinates ( int num, int* x, int* y ) const;
       MapCoordinate getFieldCoordinates ( int num ) const;
       void addField ( int x, int y, const T& _data );
       void addField ( const MapCoordinate& mc, const T& _data );
       void addField ( int x, int y );
       void addField ( const MapCoordinate& mc );
       void setMap ( GameMap* map );
       GameMap* getMap ( void );
       bool isMember ( int x, int y );
       bool isMember ( const MapCoordinate& mc );
     };


typedef FieldList<int> IntFieldList;
typedef FieldList<AttackWeap> AttackFieldList;


typedef class PendingVehicleActions* PPendingVehicleActions;


enum VehicleActionType { vat_nothing, vat_move, vat_ascent, vat_descent, vat_attack, vat_service, vat_newservice };

class VehicleAction {
           protected:
              PPendingVehicleActions pva; 
              VehicleActionType actionType;
           public:
              virtual int getStatus( void ) = 0;
              virtual int available ( Vehicle* veh ) const = 0;
              virtual int execute ( Vehicle* veh, int x, int y, int step, int param1, int param2 ) = 0;
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

class MapDisplayInterface;

class BaseVehicleMovement : public VehicleAction {
           protected:
              MapDisplayInterface* mapDisplay;
              int status;
           public:
              BaseVehicleMovement ( VehicleActionType _actionType, PPendingVehicleActions _pva, MapDisplayInterface* md ) : VehicleAction ( _actionType, _pva ),mapDisplay(md), status(0), attackedByReactionFire(false)  {};
              BaseVehicleMovement ( MapDisplayInterface* md ) : VehicleAction ( vat_move, NULL ),mapDisplay(md), status(0), attackedByReactionFire(false) {};
              AStar3D::Path path;
              int execute ( Vehicle* veh, int x, int y, int step, int height, int noInterrupt );
              bool attackedByReactionFire;
              Vehicle* getVehicle ( void ) { return vehicle; };
              void registerMapDisplay ( MapDisplayInterface* _mapDisplay ) { mapDisplay = _mapDisplay; };
              virtual int getStatus ( void ) { return status; };
              int available ( Vehicle* veh ) const;


            protected:
               Vehicle* vehicle;

               int moveunitxy ( AStar3D::Path& pathToMove, int noInterrupt = -1 );

               class PathFinder : public AStar3D {
                 public:
                   PathFinder ( GameMap* actmap, Vehicle* veh, int maxDistance ) : AStar3D(actmap, veh, false, maxDistance ) {};

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
              int available ( Vehicle* veh ) const { return status==0 && avail(veh); };
              static bool avail ( Vehicle* veh );

              enum  { NoInterrupt = 1, DisableHeightChange = 2 };
              int execute ( Vehicle* veh, int x, int y, int step, int height, int capabilities );

              virtual void registerPVA ( VehicleActionType _actionType, PPendingVehicleActions _pva );
              VehicleMovement ( MapDisplayInterface* md, PPendingVehicleActions _pva = NULL );
              ~VehicleMovement ( );
          };

/* VehicleMovement:
 *
 *   Step 0:   execute ( vehicle, -1, -1, step = 0 , height, capabilities );
 *                 height is usually -1, which means the aircraft will move on the same level of height it currently has
 *                        But there are situations like starting aircraft from carriers where it has to be set to the height
 *                        the aircraft should move on after start. Don't use this height for regular height changing!
 *                        If it is -2 then return fields on all levels of height
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
              int execute ( Vehicle* veh, int x, int y, int step, int noInterrupt, int disableMovement );
              ChangeVehicleHeight ( MapDisplayInterface* md, PPendingVehicleActions _pva , VehicleActionType vat, int dir_ );
          };

class IncreaseVehicleHeight : public ChangeVehicleHeight {
           public:
              IncreaseVehicleHeight ( MapDisplayInterface* md, PPendingVehicleActions _pva = NULL );
              static bool avail ( Vehicle* veh );
              int available ( Vehicle* veh ) const;
              ~IncreaseVehicleHeight();
          };

class DecreaseVehicleHeight : public ChangeVehicleHeight {
           public:
              DecreaseVehicleHeight ( MapDisplayInterface* md, PPendingVehicleActions _pva = NULL );
              static bool avail ( Vehicle* veh );
              int available ( Vehicle* veh ) const;
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
              Vehicle* vehicle;
              int status;
              int kamikaze;
              class tsearchattackablevehicles : public SearchFields {
                                  VehicleAttack* va;
                               public:
                                  int       anzahlgegner;
                                  const Vehicle*  angreifer;
                                  int       kamikaze;
                                  void            init ( const Vehicle* eht, int _kamikaze, VehicleAttack* _va );
                                  virtual void    testfield ( const MapCoordinate& mc );
                                  int             run ( void );
                                  tsearchattackablevehicles ( GameMap* _gamemap ) : SearchFields ( _gamemap ) {};
                          } search;

           protected:
              MapDisplayInterface* mapDisplay;
           public:
              AttackFieldList attackableVehicles;
              AttackFieldList attackableBuildings;
              AttackFieldList attackableObjects;

              int getStatus( void ) { return status; };
              Vehicle* getAttacker() { return vehicle; };
              virtual int available ( Vehicle* veh ) const { return status==0 && avail(veh); };
              static bool avail( Vehicle* veh );
              virtual int execute ( Vehicle* veh, int x, int y, int step, int _kamikaze, int weapnum );
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
              Vehicle* vehicle;
              Building* building;
              int status;

          public:
              class FieldSearch : public SearchFields {
                     VehicleService& vs;
                     Vehicle*        veh;
                     Building*       bld;
                     Resources       buildingResources;
                     Resources       resourcesCapacity;
                  public:
                     struct {
                       bool distance;
                       bool height;
                     } bypassChecks;
                     virtual void     testfield ( const MapCoordinate& mc );
                     void             checkVehicle2Vehicle ( Vehicle* veh, int xp, int yp );
                     void             checkBuilding2Vehicle ( Vehicle* veh );
                     bool             initrefuelling( int xp1, int yp1 );
                     void             init ( Vehicle* _veh, Building* _bld );
                     void             run (  );
                     FieldSearch ( VehicleService& _vs, GameMap* _gamemap ) : SearchFields ( _gamemap ), vs ( _vs ) { bypassChecks.distance = false; bypassChecks.height = false; };
                  } fieldSearch;


           protected:
              MapDisplayInterface* mapDisplay;
           public:
              Vehicle* getVehicle ( void ) { return vehicle; };
              Building* getBuilding ( void ) { return building; };

              enum Service { srv_repair, srv_resource, srv_ammo };
              class Target {
                 public:
                    Vehicle* dest;

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
              virtual int available ( Vehicle* veh ) const;
              static int avail ( const Vehicle* veh );
              static int getServices ( Vehicle* veh );
              int execute ( Vehicle* veh, int targetNWID, int dummy, int step, int pos, int amount );
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



         
#if 1

         class NewVehicleService : public VehicleAction {
            public:
               typedef ServiceTargetSearcher::Targets Targets;
               ServiceTargetSearcher::Targets targets;
            private:
               ContainerBase* container;
               int status;
               
               
            protected:
               MapDisplayInterface* mapDisplay;
            public:
               ContainerBase* getContainer() { return container; };

               int getStatus( void ) { return status; };
               virtual int available ( ContainerBase* veh ) const;
               virtual int available ( Vehicle* veh ) const;
               static int avail ( ContainerBase* veh );
               int executeContainer ( ContainerBase* veh, int x, int y, int step, int pos, int amount );
               int execute ( Vehicle* veh, int x, int y, int step, int pos, int amount );
               
               bool targetAvail( const ContainerBase* target );
               
               int fillEverything ( ContainerBase* target, bool repairsToo = false );
               virtual void registerPVA ( VehicleActionType _actionType, PPendingVehicleActions _pva );
               NewVehicleService ( MapDisplayInterface* md, PPendingVehicleActions _pva = NULL );
               virtual ~NewVehicleService ( );

              //! here the gui stores which icon initiated the service; thus preventing the occurance of an repair icon when a fuel transfer was initiated.
               int guimode;
         };



/* NewVehicleService:
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

#endif
         


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
            NewVehicleService* newservice;
            ~PendingVehicleActions ( );
            void reset();
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

template<class T> tfield* FieldList<T> :: getField ( int num ) const
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

template<class T> MapCoordinate FieldList<T> :: getFieldCoordinates ( int num ) const
{
   if ( num < fieldnum && num >= 0 ) {
      return MapCoordinate( xpos[num], ypos[num] );
   } else {
      return MapCoordinate( -1, -1 );
   }
}


template<class T> void FieldList<T> :: addField ( const MapCoordinate& mc, const T& _data )
{
   addField ( mc.x, mc.y, _data );
}

template<class T> void FieldList<T> :: addField ( int x, int y, const T& _data )
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


template<class T> void FieldList<T> :: setMap ( GameMap* map )
{
   localmap = map;
}

template<class T> GameMap* FieldList<T> :: getMap ( void )
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

