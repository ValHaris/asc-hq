/***************************************************************************
                          containerbase.h  -  description
                             -------------------
    begin                : Fri Sep 29 2000
    copyright            : (C) 2000 by Martin Bickel
    email                : bickel@asc-hq.org
 ***************************************************************************/

/*! \file containerbase.h
    \brief The base class for buildings and vehicles
*/


/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef containerbaseH
 #define containerbaseH

 #include "typen.h"
 #include "containerbasetype.h"


//! The maximum number of units that are normally allowed in a building of transport. This limit is caused by the building dialog. There's an exception: if the building is conquered, the conquering unit will be still allowed in
const int maxloadableunits = 18;


class EventHook {
        public:
          enum Events { removal, conversion };
          virtual void receiveEvent ( Events ev, int data ) = 0;
          virtual ~EventHook() {};
       };



/** \brief The parent class of Vehicle and Building;
    The name Container originates from Battle Isle, where everything that could load units
    was a container
*/
class ContainerBase {
      list<EventHook*> eventHooks;
   protected:
      const pmap gamemap;
      virtual const ResourceMatrix& getRepairEfficiency ( void ) = 0;

      //! is called after a repair is perfored. Vehicles use this to reduce their experience.
      virtual void postRepair ( int oldDamage ) = 0;
      virtual bool isBuilding() const = 0;
      const ContainerBase* findUnit ( const Vehicle* veh ) const;
   public:
      ContainerBase ( const ContainerBaseType* bt, pmap map, int player );

      void addEventHook ( EventHook* eventHook );
      void removeEventHook ( const EventHook* eventHook );

      const ContainerBaseType*  baseType;

      Vehicle*     loading[32];

      //! regroup units. This is necessary for the building dialog which only displays the first 18 units, although more than 18 can be inside ( for example, there are 18 inside, and then the enemy conquers it using a trooper)
      void regroupUnits ();

      int damage;
      int color;

      int vehiclesLoaded ( void ) const;

      virtual void write ( tnstream& stream, bool includeLoadedUnits = true ) = 0;
      virtual void read ( tnstream& stream ) = 0;

      virtual void addview ( void ) = 0;
      virtual void removeview ( void ) = 0;


      virtual int putResource ( int amount, int resourcetype, int queryonly, int scope = 1 ) = 0;
      virtual int getResource ( int amount, int resourcetype, int queryonly, int scope = 1 ) = 0;

      Resources putResource ( const Resources& res, int queryonly, int scope = 1 );
      Resources getResource ( const Resources& res, int queryonly, int scope = 1 );

      virtual bool canRepair( const ContainerBase* item ) = 0;
      pmap getMap ( ) { return gamemap; };
      const pmap getMap ( ) const { return gamemap; };

      int getMaxRepair ( const ContainerBase* item );
      int getMaxRepair ( const ContainerBase* item, int newDamage, Resources& cost  );
      int repairItem   ( ContainerBase* item, int newDamage = 0 );

      //! returns the player this vehicle/building belongs to
      int getOwner() const { return color >> 3; };

      /** can the vehicle be loaded. If uheight is passed, it is assumed that vehicle is at
          the height 'uheight' and not the actual level of height
      */
      bool vehicleLoadable ( const Vehicle* vehicle, int uheight = -1, const bool* attacked = NULL ) const;

      //! returns the levels of height on which this unit can be unloaded; or 0 if no unloading is possible
      int  vehicleUnloadable ( const Vehicle* vehicle ) const;

      //! returns the unloading system
      const ContainerBaseType::TransportationIO* vehicleUnloadSystem ( const Vehicle* vehicle, int height );

      //! returns the levels of height on which this unit can be transfered by docking; or 0 if no unloading is possible
      int  vehicleDocking ( const Vehicle* vehicle, bool out  ) const;

      /** Does the vehicle fit into the container? This does not include checking if it can reach the entry
      */
      bool vehicleFit ( const Vehicle* vehicle ) const;

      //! weight of all loaded units
      int cargo ( void ) const;


      virtual MapCoordinate3D getPosition ( ) const = 0;
      virtual ~ContainerBase();
};

class TemporaryContainerStorage : public EventHook {
        ContainerBase* cb;
        tmemorystreambuf buf;
        bool _storeCargo;
     public:
        TemporaryContainerStorage ( ContainerBase* _cb, bool storeCargo = false );
        void restore();
        void receiveEvent ( Events ev, int data );
};


#endif
