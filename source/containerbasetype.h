/***************************************************************************
                          containerbasetype.h  -  description
                             -------------------
    begin                : Sun Feb 18 2001
    copyright            : (C) 2001 by Martin Bickel
    email                : bickel@asc-hq.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef containerbasetypeH
 #define containerbasetypeH

 #include "typen.h"

class ContainerBaseType: public LoadableItemType {
   public:
     ContainerBaseType ();
     
     Resources productionCost;
     virtual ~ContainerBaseType() {};

     //! the maximum number of units that can be loaded
     int maxLoadableUnits;

     //! the maximum size of a unit that may enter the building
     int maxLoadableUnitSize;

     //! the maximum size of a unit that may enter the building
     int maxLoadableWeight;

     //! bitmapped: vehicle categories that can be stored the container
     int vehicleCategoriesStorable;

     class TransportationIO {
         public:
            TransportationIO ();
            static const int entranceModeNum = 3;
            enum { In = 1, Out = 2, Docking = 4 };
            int mode;

            // int functionbits

            //! a loadable unit must be on this level of height; 0 if not used
            int height_abs;

            //! a height of a loadable unit have this height relative to the container; -100 if not used
            int height_rel;

            //! the container must be on this height
            int container_height;

            //! bitmapped: vehicle categories that may enter the container
            int vehicleCategoriesLoadable;

            int dockingHeight_abs;

            int dockingHeight_rel;

            int requireUnitFunction;

            bool disableAttack;

            void runTextIO ( PropertyContainer& pc );
            void read ( tnstream& stream ) ;
            void write ( tnstream& stream ) const ;
     };

     typedef vector<TransportationIO> EntranceSystems;
     EntranceSystems   entranceSystems;

     void runTextIO ( PropertyContainer& pc );

     //! can units of the given type be moved into this buildtype? This is a prerequisite, but not the only requirement, for a real unit to move into a real building
     bool vehicleFit ( const Vehicletype* type ) const ;

     void read ( tnstream& stream ) ;
     void write ( tnstream& stream ) const ;

 };

#endif
