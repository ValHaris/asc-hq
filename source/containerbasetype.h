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

 #include "objects.h"
 #include "typen.h"
 #include "research.h"
 #include "mapitemtype.h"
 #include "memsize_interface.h"

class ContainerBaseType: public MapItemType, public LoadableItemType, public MemorySizeQueryInterface {
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
        OnlyMoveToAndFromTransports,
        AutoHarvestObjects,
        NoProductionCustomization,
        ManualSelfDestruct,
        NoMineRemoval,
        NoImageRotation };

     
     static const int functionNum = 45;

     bool hasFunction( ContainerFunctions function ) const;
     bool hasAnyFunction( std::bitset<64> functions ) const;
     static const char* getFunctionName( ContainerFunctions function );

   protected:
      void setFunction( ContainerFunctions function );
   public:
     
     int id;

     //! when loading a file and these IDs are encountered, this object will be used.
     vector<int> secondaryIDs;


     //! a short name, for example B-52
     ASCString    name;

     //! short description of the units role, for example "strategic bomber"
     ASCString    description;
     
     ASCString getName() const { return name; };
     int getID() const { return id; };
     
     
     //! an extensive information about the unit/building which may be several paragraphs long
     ASCString    infotext;

     //! the radar jamming power
     int          jamming;

     //! the visibility range
     int          view;

     //! the levels of height which this unit can enter
     int          height;
     int getHeight() const { return height; };

     Resources productionCost;
     virtual ~ContainerBaseType() {};

     //! the maximum number of units that can be loaded
     int maxLoadableUnits;

     //! the maximum size of a unit that may enter the building
     int maxLoadableUnitSize;

     //! the maximum total weight of all loaded units
     int maxLoadableWeight;

     //! bitmapped: vehicle categories that can be stored the container
     int vehicleCategoriesStorable;

     //! bitmapped: vehicle categories that can be produced in the container
     int vehicleCategoriesProduceable;

     /** the default production capabilities of this container.
         Please be aware that a VehicleType has additional production for building units externally */
     vector<IntRange> vehiclesInternallyProduceable;

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
     ASCString infoImageSmallFilename;

     void runTextIO ( PropertyContainer& pc );

     //! can units of the given type be moved into this buildtype? This is a prerequisite - but not the only requirement - for a real unit to move into a real building
     bool vehicleFit ( const VehicleType* type ) const ;

     /** checks the unloading of a unit type
         \param vehicleType the vehicletype for which the unloading is checked
         \param carrierHeight assume the carrier was on this height (numerical: 0 - 7).
         \return the levels of height on which this unit can be unloaded; or 0 if no unloading is possible
     */
     int  vehicleUnloadable ( const VehicleType* vehicleType, int carrierHeight ) const;


     void read ( tnstream& stream ) ;
     void write ( tnstream& stream ) const ;

     

        //! currently only used by mining stations: the efficiency of the resource extraction from the ground. Base is 1024
     int          efficiencyfuel;

        //! currently only used by mining stations: the efficiency of the resource extraction from the ground. Base is 1024
     int          efficiencymaterial;

        //! the maximum number of research points a research center may produce
     int          maxresearchpoints;

        //! when a building of this type is placed on a map, its maxResearch property will be set to this value
     int          defaultMaxResearchpoints;

        //! the number of reseach points for which the plus settings apllies
     int          nominalresearchpoints;

     //! if the unit is repaired in the field (that is, not inside a transport or building), it can only be repaired up to this level
     //! 0 would result in a complete repair being possible, 20 that it can only be restored to 20% damage (80% health).
     int  minFieldRepairDamage;
     
     Resources    maxplus;

        //! if a new building is constructed, this will be the resource production of the building
     Resources    defaultProduction;

     virtual int getMoveMalusType() const = 0;
     
     /**  returns the Storage capacity of the unit
          \param mode: 0 = ASC Resource mode ; 1 = BI Resource mode
     */
     Resources   getStorageCapacity( int mode ) const;
   protected:
     Resources    asc_mode_tank;
     Resources    bi_mode_tank;
     
   public:

      ResourceMatrix productionEfficiency;
      
      struct {
         int range;
         
         int maxFieldsPerTurn;
         
        //! the ids of objects this unit can construct
        vector<IntRange> objectsHarvestable;

        //! the group-ids of objects this unit can construct
        vector<IntRange> objectGroupsHarvestable;
      } autoHarvest;
     
 };

#endif
