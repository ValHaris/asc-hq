//     $Id: unitctrl.h,v 1.9 2000-09-24 19:57:06 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
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

#ifndef unitctrl_h_included
#define unitctrl_h_included

#include <vector>

#include "typen.h"
#include "basestrm.h"
#include "spfst.h"
#include "attack.h"

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
       void addField ( int x, int y );
       void setMap ( pmap map );
       pmap getMap ( void );
       int isMember ( int x, int y );
     };


typedef FieldList<int> IntFieldList;
typedef FieldList<AttackWeap> AttackFieldList;


typedef class PendingVehicleActions* PPendingVehicleActions;


enum VehicleActionType { vat_nothing, vat_move, vat_ascent, vat_descent, vat_attack };

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
           public:
              BaseVehicleMovement ( VehicleActionType _actionType, PPendingVehicleActions _pva ) : VehicleAction ( _actionType, _pva ) {};
              IntFieldList path;
              pvehicle getVehicle ( void ) { return vehicle; };

            protected:
               pvehicle vehicle;
               int newheight;

               //! Moves the unit to xt1 / yt1 along #path
               int moveunitxy ( int xt1, int yt1, IntFieldList& pathToMove, int noInterrupt = -1 );

                  //! finding the shortest path for a unit to a given position
                  class FieldReachableRek {

                       struct tstrecke {
                                 vector<int> x;
                                 vector<int> y;
                                 int tiefe;
                              }; 

                        int          distance;
                        int          fuelusage;
                        int          maxwegstrecke; 
                        tstrecke     shortestway; 
                        tstrecke     strecke; 
                        int          zielerreicht; 
                        int          x1, y1, x2, y2;
                        pvehicle     vehicle;
                        int          height;
                        void         move(int          x,
                                          int          y,
                                          int          direc,
                                          int          streck,
                                          int          fuel);
                       public:
                         void         run(int          x22,
                                          int          y22,
                                          pvehicle     eht,
                                          int          _height,
                                          IntFieldList*   path );
                  } fieldReachableRek;
          };


class VehicleMovement : public BaseVehicleMovement {
              int status;
           public:
              IntFieldList reachableFields;
              IntFieldList reachableFieldsIndirect;
              int available ( pvehicle veh ) const;
              int getStatus ( void ) { return status; };
              int execute ( pvehicle veh, int x, int y, int step, int height, int noInterrupt );

              //! returns the distance for the registered unit to x,y ; quite slow; could be optimized...
              int getDistance ( int x, int y );

              virtual void registerPVA ( VehicleActionType _actionType, PPendingVehicleActions _pva );
              VehicleMovement ( MapDisplayInterface* md, PPendingVehicleActions _pva = NULL );
              ~VehicleMovement ( );

            // the calculation part
            protected:

               //! searches for all fields that are reachable and stores them in #reachableFields and #reachableFieldsIndirect
               void searchstart( int x1, int y1, int hgt );

               //! recursive method for searching all reachable fields; is called by #searchstart
               void searchmove( int x,  int y, int direc, int streck, int fuelneeded );

               struct {
                  int  strck;
                  int  fieldnum;
   
                  int  mode;
                  int  tiefe;
                  int  startx, starty;
                  int  height;
               } search;

          };

/* VehicleMovement:
 *
 *   Step 0:   execute ( vehicle, -1, -1, step = 0 , height, -1 );
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
              int status;
              IntFieldList path1;
           public:
              struct StartPosition {
                 int x;
                 int y;
                 int dist;
              };
              FieldList<StartPosition> reachableFields;
              int getStatus ( void ) { return status; };
              int execute ( pvehicle veh, int x, int y, int step, int height, int allFields );
              ChangeVehicleHeight ( MapDisplayInterface* md, PPendingVehicleActions _pva , VehicleActionType vat );
           protected:
              int verticalHeightChange ( void );

              int moveunitxy ( int xt1, int yt1, IntFieldList& pathToMove );
              int execute_withmove ( int allFields );
              int moveheight ( int allFields );

          };

class IncreaseVehicleHeight : public ChangeVehicleHeight {
           public:
              IncreaseVehicleHeight ( MapDisplayInterface* md, PPendingVehicleActions _pva = NULL );
              virtual void registerPVA ( VehicleActionType _actionType, PPendingVehicleActions _pva );
              int available ( pvehicle veh ) const;
              ~IncreaseVehicleHeight ( );
          };

class DecreaseVehicleHeight : public ChangeVehicleHeight {
           public:
              DecreaseVehicleHeight ( MapDisplayInterface* md, PPendingVehicleActions _pva = NULL );
              virtual void registerPVA ( VehicleActionType _actionType, PPendingVehicleActions _pva );
              int available ( pvehicle veh ) const;
              ~DecreaseVehicleHeight ( );
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
 *   (Step 3)  see VehicleMovement
 */



class VehicleAttack : public VehicleAction {
              pvehicle vehicle;
              int status;
              int kamikaze;
              class tsearchattackablevehicles : public tsearchfields {
                                  VehicleAttack* va;
                               public:
                                  int       anzahlgegner;
                                  pvehicle  angreifer;
                                  int       kamikaze;
                                  void            init ( const pvehicle eht, int _kamikaze, VehicleAttack* _va );
                                  virtual void    testfield ( void );
                                  int             run ( void );
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



class PendingVehicleActions {
          public:
            int actionType;
            PendingVehicleActions ( void );
            VehicleAction* action;

            VehicleMovement* move;
            IncreaseVehicleHeight* ascent;
            DecreaseVehicleHeight* descent;
            VehicleAttack* attack;
            ~PendingVehicleActions ( );
         };

extern PendingVehicleActions pendingVehicleActions;


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Template implementations
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

class asc_out_of_range : public terror {};


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

   throw asc_out_of_range();
}

template<class T> T& FieldList<T> :: getData ( int x, int y )
{
   for ( int i = 0; i < fieldnum; i++ )
      if ( xpos[i] == x && ypos[i] == y )
         return data[i];

   throw asc_out_of_range();
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

template<class T> int FieldList<T> :: isMember ( int x, int y )
{
   for ( int i = 0; i < fieldnum; i++ )
      if ( xpos[i] == x && ypos[i] == y )
         return 1;
   return 0;
}

#endif

