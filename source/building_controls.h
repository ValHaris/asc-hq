/*! \file building_controls.h
    \brief The interface for the building functions
     This is for use by the AI, since the user interface to these functions
     are within building.cpp itself.
*/


//     $Id: building_controls.h,v 1.17 2003-02-27 16:10:47 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.16  2003/02/19 19:47:25  mbickel
//      Completely rewrote Pathfinding code
//      Wind not different any more on different levels of height
//
//     Revision 1.15  2003/02/12 20:11:53  mbickel
//      Some significant changes to the Transportation code
//
//     Revision 1.14  2003/01/28 17:48:42  mbickel
//      Added sounds
//      Rewrote soundsystem
//      Fixed: tank got stuck when moving from one transport ship to another
//
//     Revision 1.13  2002/12/12 11:34:17  mbickel
//      Fixed: ai crashing when weapon has no ammo
//      Fixed: ASC crashed when loading game with ID not found
//      Fixed: more ai problems
//      AI now faster
//      Fixed: removing objects overfill a units tank
//
//     Revision 1.12  2002/09/19 20:20:04  mbickel
//      Cleanup and various bug fixes
//
//     Revision 1.11  2001/11/15 20:46:05  mbickel
//      Fixed: replay not working when moving units out of carriers
//
//     Revision 1.10  2001/10/31 18:34:30  mbickel
//      Some adjustments and fixes for gcc 3.0.2
//
//     Revision 1.9  2001/09/23 23:06:20  mbickel
//      Fixed:
//       - ascent/descent during reactionfire
//       - movement with nearly empty fuel tank
//       - production icon displayed although unit could not be produced
//       - invisible building becoming visible in fog of war
//
//     Revision 1.8  2001/07/29 21:26:37  mbickel
//      Fixed: Torpedo-units could not leave submerged submarine
//
//     Revision 1.7  2001/04/01 12:59:35  mbickel
//      Updated win32 project files to new ai file structure
//      Added viewid win32-project
//      Improved AI : production and service path finding
//
//     Revision 1.6  2001/01/28 14:04:06  mbickel
//      Some restructuring, documentation and cleanup
//      The resource network functions are now it their own files, the dashboard
//       as well
//      Updated the TODO list
//
//     Revision 1.5  2001/01/24 11:53:13  mbickel
//      Fixed some compilation problems with gcc
//
//     Revision 1.4  2000/11/08 19:30:56  mbickel
//      Rewrote IO for the tmap structure
//      Fixed crash when entering damaged building
//      Fixed crash in AI
//      Removed item CRCs
//
//     Revision 1.3  2000/10/11 14:26:21  mbickel
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
//     Revision 1.2  2000/09/25 20:04:37  mbickel
//      AI improvements
//
//     Revision 1.1  2000/09/17 15:20:29  mbickel
//      AI is now automatically invoked (depending on gameoptions)
//      Some cleanup
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


#if defined(karteneditor) && !defined(pbpeditor)
 #error the mapeditor should not need to use building* !
#endif


#ifndef building_controls_h
#define building_controls_h



#include "containerbase.h"
#include "unitctrl.h"

enum tcontainermode { mbuilding, mtransport };
enum tunitmode { mnormal, mproduction, mloadintocontainer };


typedef  class ccontainercontrols*  pcontainercontrols;
typedef  class cbuildingcontrols*   pbuildingcontrols;
typedef  class ctransportcontrols*  ptransportcontrols;



class    ccontainercontrols
{
   public :

      class    crefill
      {
         public :
            void     fuel (pvehicle eht, int newfuel);                     // der neue Werte darf durchaus ?ber dem Maximum liegen. Eine entsprechende Pr?fung wird durchgef?hrt.
            void     material (pvehicle eht, int newmaterial);
            void     ammunition (pvehicle eht, char weapon, int ammun );
            void     filleverything ( pvehicle eht );
            void     emptyeverything ( pvehicle eht );
      }
      refill;

      class   cmove_unit_in_container
      {
         public:
            bool     moveupavail ( pvehicle eht );
            bool     movedownavail ( pvehicle eht, pvehicle into );
            void     moveup ( pvehicle eht );
            void     movedown ( pvehicle eht, pvehicle into );
            pvehicle unittomove;
      }
      move_unit_in_container;

      ccontainercontrols (void);

      virtual char   getactplayer (void) = 0;

      
      virtual int    putenergy (int e, int abbuchen = 1 ) = 0;
      virtual int    putmaterial (int m, int abbuchen = 1 ) = 0;
      virtual int    putfuel (int f, int abbuchen = 1) = 0;

      int            getResource ( int need, int resourceType, int abbuchen );
      virtual int    getenergy ( int need, int abbuchen ) = 0;
      virtual int    getmaterial ( int need, int abbuchen ) = 0;
      virtual int    getfuel ( int need, int abbuchen ) = 0;

      virtual int    putammunition (int  weapontype, int  ammunition, int abbuchen) = NULL;
      virtual int    getammunition ( int weapontype, int num, int abbuchen, int produceifrequired = 0 ) = 0;
      virtual int    ammotypeavail ( int type ) = 0;

      virtual int    getxpos (void) = 0;
      virtual int    getypos (void) = 0;

      virtual VehicleMovement*  movement ( pvehicle eht, bool simpleMode = false );
      virtual int    getHeight ( void ) = 0;


      virtual int    getspecfunc ( tcontainermode mode ) = 0;

      virtual pvehicle getloadedunit (int num) = NULL;

      ContainerBase* baseContainer;
};



class    cbuildingcontrols : public virtual ccontainercontrols
{
   private :

      char  getactplayer (void);


      int   getxpos (void);
      int   getypos (void);


   public :
      int    putenergy (int e , int abbuchen = 1);
      int    putmaterial (int m, int abbuchen = 1 );
      int    putfuel (int f, int abbuchen = 1 );
      int    getenergy ( int need, int abbuchen );
      int    getmaterial ( int need, int abbuchen );
      int    getfuel ( int need, int abbuchen );
      int    putammunition ( int weapontype, int  ammunition, int abbuchen);
      int    getspecfunc ( tcontainermode mode );
      int    getammunition ( int weapontype, int num, int abbuchen, int produceifrequired = 0 );
      int    ammotypeavail ( int type );
      int    getHeight ( void );

      pbuilding   building;

      class    crecycling
      {                           // RECYCLING
         public :
            Resources resourceuse ( pvehicle eht );
            void      recycle (pvehicle eht);
      }
      recycling;

      class    ctrainunit
      {
         public :
            int  available  ( pvehicle eht );
            void trainunit ( pvehicle eht );
      }
      training;

      class    cproduceunit
      {                         // PRODUCEUNIT
         public :
            //! lack : the reason why the unit is not produceable; bitmapped: bit 0 - 2  Resource N lacking; bit 10 : not researched yet
            int         available ( pvehicletype fzt, int* lack = NULL );
            pvehicle    produce ( pvehicletype fzt, bool forceRefill = false );
            pvehicle    produce_hypothetically ( pvehicletype fzt );
      }
      produceunit ;

      class    cdissectunit
      {
         public :
            int  available  ( pvehicle eht );
            void dissectunit ( pvehicle eht );
      }
      dissectunit;

      class    cnetcontrol
      {
         public:
            void setnetmode (  int category, int stat );  // stat  1: setzen    0: l”schen
            int  getnetmode ( int mode );
            void emptyeverything ( void );
            void reset ( void );
      }
      netcontrol;


      class    cproduceammunition
      {
         public:
            int baseenergyusage;
            int basematerialusage;
            int basefuelusage;
            int materialneeded;
            int energyneeded;
            int fuelneeded;
            int checkavail ( int weaptype, int num );
            void produce ( int weaptype, int num );
            cproduceammunition ( void );
      }
      produceammunition;


      void  removevehicle ( pvehicle *peht );

      pvehicle getloadedunit (int num);

      cbuildingcontrols (void);
      void  init (pbuilding bldng);
};

class    ctransportcontrols : public virtual ccontainercontrols
{
   private :

      char  getactplayer (void);


      int   getxpos (void);
      int   getypos (void);



   public :
      int    putenergy (int e, int abbuchen = 1 );
      int    putmaterial (int m, int abbuchen = 1 );
      int    putfuel (int f, int abbuchen = 1 );
      int    getenergy ( int need, int abbuchen );
      int    getmaterial ( int need, int abbuchen );
      int    getfuel ( int need, int abbuchen );
      int    putammunition ( int weapontype, int  ammunition, int abbuchen);
      int    getspecfunc ( tcontainermode mode );
      int    getammunition ( int weapontype, int num, int abbuchen, int produceifrequired = 0 );
      int    ammotypeavail ( int type );
      int    getHeight ( void );


      pvehicle vehicle;

      void  removevehicle ( pvehicle *peht );

      pvehicle getloadedunit (int num);

      ctransportcontrols ( void );
      void  init ( pvehicle eht );
};

#endif // building_controls_h

