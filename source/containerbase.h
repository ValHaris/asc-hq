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

 #include <sigc++/sigc++.h>

 #include "typen.h"
 #include "containerbasetype.h"
 #include "graphics/surface.h"
 


//! The maximum number of units that are normally allowed in a building of transport. This limit is caused by the building dialog. There's an exception: if the building is conquered, the conquering unit will be still allowed in
const int maxloadableunits = 18;

class Vehicle;


/** \brief The parent class of Vehicle and Building;
    The name Container originates from Battle Isle, where everything that could load units
    was a container
*/
class ContainerBase {
   protected:
      const pmap gamemap;
      virtual const ResourceMatrix& getRepairEfficiency ( void ) = 0;

      //! is called after a repair is perfored. Vehicles use this to reduce their experience.
      virtual void postRepair ( int oldDamage ) = 0;

      virtual bool isBuilding() const = 0;
      
      void paintField ( const Surface& src, Surface& dest, SPoint pos, int dir, bool shaded, int shadowDist = -1 ) const;
      
   public:
      ContainerBase ( const ContainerBaseType* bt, pmap map, int player );

      const ContainerBaseType*  baseType;


      typedef vector<Vehicletype*> Production;
      Production unitProduction;
      

    //! @name Cargo related functions
    //@{

      typedef vector<Vehicle*> Cargo;
   protected:  
      Cargo cargo;
   public:
      const Cargo& getCargo() const { return cargo; };

      //! removes ALL units from cargo
      void clearCargo();

      SigC::Signal0<void> cargoChanged;
      
      void addToCargo( Vehicle* veh );
      
      //! removes the given unit from the container. \Returns true of the unit was found, false otherwise
      bool removeUnitFromCargo( Vehicle* veh, bool recursive = false );
      bool removeUnitFromCargo( int nwid, bool recursive = false );
      
      bool unitLoaded( int nwid );
      
      //! if the unit is inside this container, returns the container which the unit is directly in (which may not be the current one as containers may be nested arbitrarily).
      const ContainerBase* findParent ( const ContainerBase* veh ) const;
      ContainerBase* findParent ( const ContainerBase* veh );
      

      //! returns the number of loaded units
      int vehiclesLoaded ( void ) const;
      
      //! if this is a unit and it is inside a building or transport, returns the transport. NULL otherwise.
      ContainerBase* getCarrier() const;

      //! searches for a the unit in the whole stack
      Vehicle* findUnit ( int nwid );
      
      /** can the vehicle be loaded. If uheight is passed, it is assumed that vehicle is at
      the height 'uheight' and not the actual level of height
       */
      bool vehicleLoadable ( const Vehicle* vehicle, int uheight = -1, const bool* attacked = NULL ) const;

      //! returns the levels of height on which this unit can be unloaded; or 0 if no unloading is possible
      int  vehicleUnloadable ( const Vehicletype* vehicleType ) const;

      //! returns the unloading system
      const ContainerBaseType::TransportationIO* vehicleUnloadSystem ( const Vehicletype* vehicle, int height );

      //! returns the levels of height on which this unit can be transfered by docking; or 0 if no unloading is possible
      int  vehicleDocking ( const Vehicle* vehicle, bool out  ) const;

      /** Does the vehicle fit into the container? This does not include checking if it can reach the entry
       */
      bool vehicleFit ( const Vehicle* vehicle ) const;

      //! weight of all loaded units
      int cargoWeight() const;

      
    //@}

      
     
      int damage;
      
      int color;
      //! returns the player this vehicle/building belongs to
      int getOwner() const { return color >> 3; };
      
      virtual void convert ( int player ) = 0;
      


      virtual void write ( tnstream& stream, bool includeLoadedUnits = true ) = 0;
      virtual void read ( tnstream& stream ) = 0;

      virtual void addview ( void ) = 0;
      virtual void removeview ( void ) = 0;


      

    //! @name Resource related functions
    //@{
      
      virtual int putResource ( int amount, int resourcetype, bool queryonly, int scope = 1 ) = 0;
      virtual int getResource ( int amount, int resourcetype, bool queryonly, int scope = 1 ) = 0;
      virtual int getResource ( int amount, int resourcetype ) const = 0;

      Resources putResource ( const Resources& res, bool queryonly, int scope = 1 );
      Resources getResource ( const Resources& res, bool queryonly, int scope = 1 );
      Resources getResource ( const Resources& res ) const;

    //! returns the resource that the building consumes for its operation.
      Resources getResourceUsage ( );

      Resources getResourcePlus ( );
      
      //! returns the local storage capacity for the given resource, which depends on the resource mode of the map. \see tmap::_resourcemode
      Resources getStorageCapacity() const;
      
    //! returns the amount of resources that the net which the building is connected to produces each turn
      Resources netResourcePlus( ) const;


    //! the Resources that are produced each turn
      Resources   plus;

    //! the maximum amount of Resources that the building can produce each turn in the ASC resource mode ; see also #bi_resourceplus
      Resources   maxplus;

    //! the maximum amount of Resources that the building can produce each turn in the BI resource mode ; see also #maxplus
      Resources    bi_resourceplus;
      
    //@}
      

      
      
      virtual bool canRepair( const ContainerBase* item ) const = 0;

      int getMaxRepair ( const ContainerBase* item );
      int getMaxRepair ( const ContainerBase* item, int newDamage, Resources& cost, bool ignoreCost = false  );
      int repairItem   ( ContainerBase* item, int newDamage = 0 );
      
      //! returns the amount of damate that can still be repaired this turn
      virtual int repairableDamage() = 0;


      pmap getMap ( ) { return gamemap; };
      const pmap getMap ( ) const { return gamemap; };
      
      virtual int getIdentification() = 0;

      virtual int getHeight() const = 0;

      virtual ASCString getName ( ) const = 0;

      virtual int getAmmo( int type, int num, bool queryOnly )  = 0;
      virtual int putAmmo( int type, int num, bool queryOnly )  = 0;
      virtual int maxAmmo( int type ) const = 0 ;


      SigC::Signal0<void> conquered;
      SigC::Signal0<void> destroyed;
      static SigC::Signal0<void> anyContainerDestroyed;

      static int calcShadowDist( int binaryHeight );


    //! the current amount of research that the building conducts every turn
      int         researchpoints;

      int         maxresearchpoints;

    //! hook that is called when a player ends his turn
      virtual void endOwnTurn( void );

    //! hook that is called when any player (including owner) ends turn
      virtual void endAnyTurn( void );

    //! hook that is called the next round begins ( active player switching from player8 to player1 )
      virtual void endRound ( void );
      

      
      class Work {
         public:
            virtual bool finished() = 0;
            virtual bool run() = 0;
            virtual Resources getPlus() = 0;
            virtual Resources getUsage() = 0;
            virtual ~Work() {};
      };

      class WorkClassFactory {
         public:
            virtual bool available( const ContainerBase* cnt ) = 0;
            virtual Work* produce( ContainerBase* cnt, bool queryOnly ) = 0;
            virtual ~WorkClassFactory() {};
      };

      static bool registerWorkClassFactory( WorkClassFactory* wcf, bool ASCmode = true );
   private:
      typedef list<WorkClassFactory*> WorkerClassList;
      static WorkerClassList* workClassFactoriesASC;
      static WorkerClassList* workClassFactoriesBI;
   public:
      
      Work* spawnWorkClasses( bool justQuery );

      
      virtual MapCoordinate3D getPosition ( ) const = 0;
      virtual ~ContainerBase();

      virtual vector<MapCoordinate> getCoveredFields() = 0;
};

class TemporaryContainerStorage  {
        ContainerBase* cb;
        tmemorystreambuf buf;
        bool _storeCargo;
     public:
        TemporaryContainerStorage ( ContainerBase* _cb, bool storeCargo = false );
        void restore();
};


#endif
