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
 #include "research.h"
 #include "mapitemtype.h"

class ContainerBaseType: public MapItemType, public LoadableItemType {
   protected:
      std::bitset<64> features;
   public:
     ContainerBaseType ();

     enum ContainerFunctions { TrainingCenter,
        InternalVehicleProduction,
        AmmoProduction,
        InternalUnitRepair,
        RecycleUnits,
        Research,
        Sonar,
        SatelliteView,
        MineView,
        WindPowerPlant,
        SolarPowerPlant,
        MatterConverter,
        MiningStation,
        ProduceNonLeavableUnits,
        ResourceSink,
        ExternalEnergyTransfer,
        ExternalMaterialTransfer,
        ExternalFuelTransfer,
        ExternalAmmoTransfer,
        ExternalRepair,
        NoObjectChaining,
        SelfDestructOnConquer,
        Paratrooper,
        PlaceMines,
        CruiserLanding,
        ConquerBuildings,
        MoveAfterAttack,
        ExternalVehicleProduction,
        ConstructBuildings,
        IceBreaker,
        NoInairRefuelling,
        MakesTracks,
        DetectsMineralResources,
        NoReactionfire,
        AutoRepair,
        KamikazeOnly,
        ImmuneToMines,
        JamsOnlyOwnField,
        MoveWithReactionFire,
        OnlyMoveToAndFromTransports };

     
     static const int functionNum = 41;

     bool hasFunction( ContainerFunctions function ) const;
     bool hasAnyFunction( std::bitset<64> functions ) const;
     static const char* getFunctionName( ContainerFunctions function );
     
     int id;

     //! a short name, for example B-52
     ASCString    name;

     ASCString getName() const { return name; };
     int getID() const { return id; };
     
     
     //! an extensive information about the unit/building which may be several paragraphs long
     ASCString    infotext;

     //! the radar jamming power
     int          jamming;

     //! the visibility range
     int          view;


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

            std::bitset<64> requiresUnitFeature;

            int movecost;

            bool disableAttack;

            void runTextIO ( PropertyContainer& pc );
            void read ( tnstream& stream ) ;
            void write ( tnstream& stream ) const ;
     };

     typedef vector<TransportationIO> EntranceSystems;
     EntranceSystems   entranceSystems;

     TechAdapterDependency techDependency;

     //! the filename of an image that is shown in info dialogs
     ASCString infoImageFilename;

     void runTextIO ( PropertyContainer& pc );

     //! can units of the given type be moved into this buildtype? This is a prerequisite - but not the only requirement - for a real unit to move into a real building
     bool vehicleFit ( const Vehicletype* type ) const ;

     void read ( tnstream& stream ) ;
     void write ( tnstream& stream ) const ;

 };

#endif
