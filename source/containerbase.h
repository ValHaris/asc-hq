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

#ifndef containerbase_h_included
 #define containerbase_h_included

 #include "typen.h"


class EventHook {
        public:
          enum Events { removal, conversion };
          virtual void receiveEvent ( Events ev, int data ) = 0;
       };

class ContainerBaseType {
   public:
     Resources productionCost;
     virtual ~ContainerBaseType() {};
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
   public:
      ContainerBase ( const ContainerBaseType* bt, pmap map, int player );

      void addEventHook ( EventHook* eventHook );
      void removeEventHook ( const EventHook* eventHook );

      const ContainerBaseType*  baseType;

      Vehicle*     loading[32];
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

      virtual bool canRepair( void ) = 0;
      pmap getMap ( ) { return gamemap; };

      int getMaxRepair ( const ContainerBase* item );
      int getMaxRepair ( const ContainerBase* item, int newDamage, Resources& cost  );
      int repairItem   ( ContainerBase* item, int newDamage = 0 );


      virtual MapCoordinate getPosition ( ) = 0;
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