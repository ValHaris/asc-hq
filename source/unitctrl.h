//     $Id: unitctrl.h,v 1.2 2000-05-06 20:25:26 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
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

#include "typen.h"
#include "basestrm.h"
#include "spfst.h"

class FieldList {
       pmap localmap;
       int fieldnum;
       dynamic_array<int> xpos;
       dynamic_array<int> ypos;
     public:
       FieldList ( void );
       int getFieldNum ( void );
       pfield getField ( int num );
       void getFieldCoordinates ( int num, int* x, int* y );
       void addField ( int x, int y );
       void setMap ( pmap map );
       pmap getMap ( void );
       int isMember ( int x, int y );
     };


typedef class PendingVehicleActions* PPendingVehicleActions;


enum VehicleActionType { vat_nothing, vat_move, vat_ascent, vat_descent };

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

typedef int trichtungen[sidenum]; 


class BaseVehicleMovement : public VehicleAction {
           protected:
              MapDisplayInterface* mapDisplay;
           public:
              BaseVehicleMovement ( VehicleActionType _actionType, PPendingVehicleActions _pva ) : VehicleAction ( _actionType, _pva ) {};
              FieldList path;
              pvehicle getVehicle ( void ) { return vehicle; };

            protected:
               pvehicle vehicle;
               int newheight;
 
               int moveunitxy ( int xt1, int yt1 );

                  class FieldReachableRek {
                        int          distance;
                        int          fuelusage;
                        int          maxwegstrecke; 
                        tstrecke     shortestway; 
                        tstrecke     strecke; 
                        boolean      zielerreicht; 
                        int          x1, y1, x2, y2;
                        pvehicle     vehicle;
                        int          mode;
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
                                          int          md,
                                          int          _height,
                                          FieldList*   path );
                  } fieldReachableRek;
          };


class VehicleMovement : public BaseVehicleMovement {
              int status;
           public:
              FieldList reachableFields;
              FieldList reachableFieldsIndirect;
              int available ( pvehicle veh ) const;
              int getStatus ( void ) { return status; };
              int execute ( pvehicle veh, int x, int y, int step, int height, int param2 );

              virtual void registerPVA ( VehicleActionType _actionType, PPendingVehicleActions _pva );
              VehicleMovement ( MapDisplayInterface* md, PPendingVehicleActions _pva = NULL );
              ~VehicleMovement ( );

            // the calculation part
            protected:

               void searchstart( int x1, int y1, int hgt );
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


class ChangeVehicleHeight : public BaseVehicleMovement {
              int status;
           public:
              FieldList reachableFields;
              int getStatus ( void ) { return status; };
              int execute ( pvehicle veh, int x, int y, int step, int height, int param2 );
              ChangeVehicleHeight ( MapDisplayInterface* md, PPendingVehicleActions _pva , VehicleActionType vat );
           protected:
              int verticalHeightChange ( void );

              int moveunitxy ( int xt1, int yt1 );
              int execute_withmove ( void );
              int moveheight ( void );

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


class PendingVehicleActions {
          public:
            int actionType;
            PendingVehicleActions ( void );
            VehicleAction* action;

            VehicleMovement* move;
            IncreaseVehicleHeight* ascent;
            DecreaseVehicleHeight* descent;
            ~PendingVehicleActions ( );
         };

extern PendingVehicleActions pendingVehicleActions;
#endif
