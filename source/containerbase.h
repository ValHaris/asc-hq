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
   protected:
      pmap gamemap;
      virtual const ResourceMatrix& getRepairEfficiency ( void ) = 0;
   public:
      ContainerBase ( ContainerBaseType* bt );

      const ContainerBaseType*  baseType;
      Vehicle*     loading[32];
      int damage;

      int vehiclesLoaded ( void ) const;

      virtual int putResource ( int amount, int resourcetype, int queryonly, int scope = 1 ) = 0;
      virtual int getResource ( int amount, int resourcetype, int queryonly, int scope = 1 ) = 0;

      Resources putResource ( const Resources& res, int queryonly, int scope = 1 );
      Resources getResource ( const Resources& res, int queryonly, int scope = 1 );

      virtual bool canRepair( void ) = 0;
      void chainToMap ( pmap mp ) { gamemap = mp; };
      int getMaxRepair ( const ContainerBase* item );
      int getMaxRepair ( const ContainerBase* item, int newDamage, Resources& cost  );
      int repairItem   ( ContainerBase* item, int newDamage = 0 );
      virtual MapCoordinate getPosition ( ) = 0;
      virtual ~ContainerBase() {};
};




#endif