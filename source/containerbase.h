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
 

class Vehicle;
class Player;

/** \brief The parent class of Vehicle and Building;
    The name Container originates from Battle Isle, where everything that could load units
    was a container
*/
class ContainerBase {
      friend class ConvertContainer; 
      friend class ChangeContainerProperty;
   protected:

     /** the percantage that this container has already been repaired this turn. 
        Currently only used for Buildings - and only buildings serialize this property to disk!
        The maximum percentage may be limited by a gameparameter
      */
      int           repairedThisTurn;

	  //! the map that this container is placed on
      GameMap* gamemap;
	  
	  /** if this container is transported inside a carrier, this is carrier
	      \note Only vehicles can be transported, building can't */
      ContainerBase* cargoParent;
	  

	  /** displays an image of this container on the surface. The different shaders 
	      (like semi-transparency for submerged stuff) is processed here 
		  \param src  The source surface, which is an image of the ContainerBase
		  \param dest The destination surface, onto which the src image is painted
		  \param pos  The position within dest at which src is painted
		  \param dir  Rotation of the image. Valid range is 0..5, resuling in 0�, 60� 120�, etc
		  \param shaded If true then the image will not be displayed in color, but in greyscale
		  \param shadowDist The offset of the shadow of the unit in pixels. 
                            Shadowdist will be added to the x and y coordinates if the position,
                            resulting in the shadow being in the lower right of the image  							
							0 = no shadow will be drawn
							If -1, the shadowDist will be calculated depending on the container's 
							    current height */
      void paintField ( const Surface& src, Surface& dest, SPoint pos, int dir, bool shaded, int shadowDist = -1 ) const;
      
      ContainerBase ( const ContainerBaseType* bt, GameMap* map, int player );
   public:

      virtual bool isBuilding() const = 0;

	  //! the type descriping all non-instance specific properties of the container
      const ContainerBaseType*  baseType;

	  /** returns an image for the Container. 
          \note Buildings have a size of several fields. The image returned here is therefore not suited
                to be painted on the map, as map-painting is done on a per-field basis */		  
      virtual Surface getImage() const = 0;

      typedef vector<const VehicleType*> Production;

      const Production& getProduction() const;
      Resources getProductionCost( const VehicleType* unit ) const;
      void deleteProductionLine( const VehicleType* type );
      void deleteAllProductionLines();
      void addProductionLine( const VehicleType* type  );
      bool hasProductionLine( const VehicleType* type );
      void setProductionLines( const Production& production  );
   private:
      mutable Production productionCache;
   protected:
      Production internalUnitProduction;
   public:
      

    //! @name Cargo related functions
    //@{

      typedef vector<Vehicle*> Cargo;
   protected:  
      Cargo cargo;

   private:
       //! removes all holes ( = NULL-Pointers) from the vector)
       void compactCargo();

   public:
      const Cargo& getCargo() const { return cargo; };

      int getCargoCount() const { return cargo.size(); };
      
      /** returns the cargo slot with index i
          Warning: the cargo may contain NULL items. This was deliberately, to allow easier navigation
                   when the user moves items successivly out of a carrier
      */
      Vehicle* getCargo( int i );
      
      //! removes ALL units from cargo
      void clearCargo();

      SigC::Signal0<void> cargoChanged;

      virtual int  getArmor() const = 0;
      
      
      //! a name given by the user or the map creator
      ASCString    name;
      
      /** adds the unit to the cargo
         \param veh the unit to add 
         \param position specifies a specific cargo slot. 
                         -1 assigns slot automatically. The automatic mode should be used in almost all cases */
      void addToCargo( Vehicle* veh, int position = -1 );
      
      //! removes the given unit from the container. \return true if the unit was found, false otherwise
      bool removeUnitFromCargo( Vehicle* veh, bool recursive = false );
      bool removeUnitFromCargo( int nwid, bool recursive = false );
      
      
      //! returns the number of loaded units
      int vehiclesLoaded ( void ) const;
      
      //! if this is a unit and it is inside a building or transport, returns the transport. NULL otherwise.
      ContainerBase* getCarrier() const;

      //! searches for a the unit in carrier and optionally all inner carriers
      Vehicle* findUnit ( int nwid, bool recursive = true ) const;
      
      /** can the vehicle be loaded. If uheight is passed, it is assumed that vehicle is at
      the height 'uheight' and not the actual level of height
       */
      bool vehicleLoadable ( const Vehicle* vehicle, int uheight = -1, const bool* attacked = NULL ) const;

      /** checks the unloading of a unit type
          \param vehicleType the vehicletype for which the unloading is checked
          \param carrierHeight assume the carrier ( = this) was on this height (numerical: 0 - 7). If -1, use current height
          \return the levels of height on which this unit can be unloaded; or 0 if no unloading is possible
      */
      int  vehicleUnloadable ( const VehicleType* vehicleType, int carrierHeight = -1 ) const;

      //! returns the unloading system
      const ContainerBaseType::TransportationIO* vehicleUnloadSystem ( const VehicleType* vehicle, int height );

      //! returns the levels of height on which this unit can be transfered by docking; or 0 if no unloading is possible
      int  vehicleDocking ( const Vehicle* vehicle, bool out  ) const;

      /** Does the vehicle fit into the container? This does not include checking if it can reach the entry
       */
      bool vehicleFit ( const Vehicle* vehicle ) const;

      //! weight of all loaded units
      int cargoWeight() const;

      //! returns the nesting depth of the cargo. The unit standing on the field is 0, its cargo is 1, the cargo's cargo 2 ...
      int cargoNestingDepth();

   private:
      //! checks if this vehicle can carry this additional weight and recursively checks all outer vehicle (in case of nested carriers)
      bool canCarryWeight( int additionalWeight, const Vehicle* vehicle ) const;
    //@}

   public:
     
	  //! Damage. 0 is no damage, when damage reaches 100 the container is destroyed
      int damage;
      
	  /** The owner of the container. For historical reasons, this is actually 8 times the player numer
	      Use getOwner() instead of directly accesing this variable */
      int color;
	  
      //! returns the number of the player this vehicle/building belongs to
      int getOwner() const { return color >> 3; };
      
      //! returns the player this vehicle/building belongs to
      Player& getOwningPlayer() const;
      
      //! this is a low level functions that changes the registration in the map. It's called by convert(int,bool)
      virtual void registerForNewOwner( int player ) = 0;
      


      virtual void write ( tnstream& stream, bool includeLoadedUnits = true ) const = 0;
      virtual void read ( tnstream& stream ) = 0;

	  //! registers the containers view (=radar) on the map
      virtual void addview ( void ) = 0;
	  
	  //! removes the containers view (=radar) on the map
      virtual void removeview ( void ) = 0;

      

    //! @name Resource related functions
    //@{

      /** scope: 0 = local
                 1 = resource network
                 2 = global in all buildings
                 3 = map wide pool( used only internally! )
      */
      virtual int putResource ( int amount, int resourcetype, bool queryonly, int scope = 1, int player = -1 ) = 0;
      virtual int getResource ( int amount, int resourcetype, bool queryonly, int scope = 1, int player = -1 ) = 0;
      virtual int getAvailableResource ( int amount, int resourcetype, int scope = 1 ) const = 0;

      Resources putResource ( const Resources& res, bool queryonly, int scope = 1, int player = -1 );
      Resources getResource ( const Resources& res, bool queryonly, int scope = 1, int player = -1 );
      Resources getResource ( const Resources& res ) const;

    //! returns the resource that the building consumes for its operation.
      Resources getResourceUsage ( );

      Resources getResourcePlus ( );
      
      //! returns the local storage capacity for the given resource, which depends on the resource mode of the map. \see GameMap::_resourcemode
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
      


    //! @name Repairing related functions
    //@{
      
	  /** when a ContainerBase is repair by this ContainerBase, the default cost
          can be customized with this matrix. 
          \note This effects both reparing another ContainerBase as well as self-repsir */		  
      virtual const ResourceMatrix& getRepairEfficiency() const = 0;

      //! is called after a repair is performed. Vehicles use this to reduce their experience.
      virtual void postRepair ( int oldDamage ) = 0;
      
      //! checks whether the item can be repaired provided that it is in range
      virtual bool canRepair( const ContainerBase* item ) const = 0;

	  /** returns the maximum amount of damage that the given item can be repaired
	      \return a value in the range 0 .. item->damage */
      int getMaxRepair ( const ContainerBase* item ) const;
      int getMaxRepair ( const ContainerBase* item, int newDamage, Resources& cost, bool ignoreCost = false  ) const;
      int repairItem   ( ContainerBase* item, int newDamage = 0 );
      
      //! returns the amount of damate that can still be repaired this turn
      virtual int repairableDamage() const = 0;

    //@}

      GameMap* getMap ( ) const { return gamemap; };
      
      virtual int getIdentification() const = 0;

      //! returns the bitmapped level of height. Only one bit will be set, of course
      virtual int getHeight() const = 0;

      virtual ASCString getName ( ) const = 0;
      virtual void setName ( const ASCString& name );

      virtual int getAmmo( int type, int num, bool queryOnly )  = 0;
      virtual int getAmmo( int type, int num ) const  = 0;
      virtual int putAmmo( int type, int num, bool queryOnly )  = 0;
      virtual int maxAmmo( int type ) const = 0 ;


      SigC::Signal0<void> conquered;
      SigC::Signal0<void> destroyed;
      static SigC::Signal1<void,ContainerBase*> anyContainerDestroyed;
      static SigC::Signal1<void,ContainerBase*> anyContainerConquered;

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
      
      int view;

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
