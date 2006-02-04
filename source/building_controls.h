/*! \file building_controls.h
    \brief The interface for the building functions
     This is for use by the AI, since the user interface to these functions
     are within building.cpp itself.
*/

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
            void     resource (pvehicle eht, int resourcetype, int newamount );
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

      virtual int    putammunition (int  weapontype, int  ammunition, int abbuchen) = 0;
      virtual int    getammunition ( int weapontype, int num, int abbuchen, int produceifrequired = 0 ) = 0;
      virtual int    ammotypeavail ( int type ) = 0;

      virtual int    getxpos (void) = 0;
      virtual int    getypos (void) = 0;

      virtual VehicleMovement*  movement ( pvehicle eht, bool simpleMode = false );
      virtual int    getHeight ( void ) = 0;

      Resources getResource ( Resources res, bool queryOnly );
      int  getResource ( int amount, int resourceType, bool queryOnly );

      virtual int    getspecfunc ( tcontainermode mode ) = 0;

      virtual pvehicle getloadedunit (int num) = 0;

      ContainerBase* baseContainer;
   protected:
      virtual void repaintResources() {};
};



class    cbuildingcontrols : public virtual ccontainercontrols
{
   private :

      char  getactplayer (void);


      int   getxpos (void);
      int   getypos (void);


   public :
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

      class    BuildProductionLine
      {
         public:
            Resources resourcesNeeded( Vehicletype* veh );
            int build( Vehicletype* veh  );
      } buildProductionLine;
      class    RemoveProductionLine
      {
         public:
            Resources resourcesNeeded( Vehicletype* veh );
            int remove( Vehicletype* veh  );
      } removeProductionLine;



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

