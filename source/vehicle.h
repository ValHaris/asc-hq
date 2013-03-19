/***************************************************************************
                          vehicle.h  -  description
                             -------------------
    begin                : Fri Sep 29 2000
    copyright            : (C) 2000 by Martin Bickel
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

#ifndef vehicleH
 #define vehicleH

 #include "typen.h"
 #include "vehicletype.h"
 #include "containerbase.h"
 #include "ascstring.h"

class BuildingType;

class Context;

class UnitHooveringLogic {
   //! the percentage of fuel that is required for a flying plane just hoovering in the air (unit: percentage)
      static const int FuelConsumption; 

   public:
      static int calcFuelUsage( const Vehicle* veh );

      /** calculates the time until the unit crashes because of lack of fuel
          \returns time in turns or -1 if the unit can't crash because of missing fuel in its current height
      */
      static int getEndurance ( const Vehicle* veh );
      static int getEndurance ( const VehicleType* veh, int height = -1, int resourceModel = 0 );

};

 
 class Vehicle : public ContainerBase {
    bool cleanRemove;

    //! a vehicle must not be created without a type or map
    Vehicle (  );

    //! cloning of vehicles is not allowed too
    Vehicle ( const Vehicle& v );

    //! is  the vehicle currently viewing the map? if yes, the view has to be removed on destruction
    bool viewOnMap;

    /** remaining movement for this turn.
        Note that the usable movement can be less due to lack of fuel.
        Use #getmovement to optain the usable movement
    */
    int  _movement;

    void init ( void );

    ResourceMatrix repairEfficiency;

    bool generatoractive;

    bool movementLeft() const;

    Vehicle ( const VehicleType* t, GameMap* actmap, int player, int networkID  );


   public:

    Vehicle ( const VehicleType* t, GameMap* actmap, int player );

    int repairableDamage() const { return damage; };

    bool isBuilding() const { return false; };

    const VehicleType* typ;
    
    const VehicleType* getType() const { return typ; };
    
    int          ammo[16];
    int          weapstrength[16];
    int          experience;

    //! did the unit already attack this turn
    bool         attacked;

    //! the the unit and all cargo to #attacked = true
    void setAttacked();
    
    //! the the unit and (optionally) all cargo to #attacked = true
    void setAttacked( bool recursive, const Context& context );

    //! the current level of height ( BITMAPPED ! )
    int          height;

    //! the direction in which the unit is facing
    Uint8         direction;

    //! the position on the map
    int          xpos, ypos;
#ifndef karteneditor
  private:
#endif
    //! the resources that the unit is carrying
    Resources    tank;
  public:

    //! if events are triggered by this unit (if it is lost for example), this will set connection != 0
    int          connection;

    int          getArmor() const;

    /** a unique identification of the unit that is used everywhere in ASC
       (and not only the network protocols where is was first used, hence the name)
    */
    int          networkid;

    //! The class that manages the reaction fire which causes a unit to attack approaching enemies even if it is not the unit's turn
    class  ReactionFire {
         friend class Vehicle;
         friend class UnitPropertyEditor;
         friend class ChangeUnitProperty;

         Vehicle* unit;
       public:
         ReactionFire ( Vehicle* _unit );
         enum Status { off, init1a, init2, ready, init1b };

         //! for each player that can still be attacked one bit is set
         // int enemiesAttackable;
         Status getStatus() const { return status;};

         /** enables the reaction fire or returns an error code ( result < 0 = -errorNr ) 
         \deprecated
         */
         int enable();
         
         //! \deprecated 
         void disable();

         vector<int> weaponShots;
         vector<int> nonattackableUnits;

         //! hook that is called when a turn ends
         void endOwnTurn();
         void endAnyTurn();
         void checkData();

         bool canPerformAttack( Vehicle* target );
         
         bool canMove() const;
       private:
         Status status;
         void resetShotCount();

    } reactionfire;

    AiParameter* aiparam[8];


    //! @name Movement related functions
    //@{
    //! can the unit move from its current position (does not check neighbouring fields)
    bool canMove ( void ) const;

    //! returns the movement points the unit has left for this turn. CheckFuel should almost always be true.
    int getMovement ( bool checkFuel = true, bool checkRF = true ) const ;

    /** sets a new distance that the unit can move
    \param cargoDivisor : the cargo of this unit gets 1/cargodivisor the change that this unit is getting; if 0 the cargo is not touched ; -1 is default
     */
    void setMovement ( int newmove, double cargoDivisor = -1 );

    //! did the unit move this turn
    bool hasMoved ( void ) const;

    //! the maximum distance that the unit can drive in a single turn on the current level of height
    int maxMovement ( ) const;

    /** reduces the movement by the given amount. Negative values are possible.
    Don't use something like "setmovement ( getmovement() - amount )", because getmovement may return a lower amount due to lack of fuel. */
    void decreaseMovement ( int movement );

    //! resets a units movement. This is called at the beginning of each turn.
    void resetMovement( void );

    //@}
    
    
    /** add the objects like tracks or broken ice
        \returns true if any objects were created
    */
    bool spawnMoveObjects( const MapCoordinate& start, const MapCoordinate& dest, const Context& context );

    //! @name Resource related functions
    //@{
    
    int putResource ( int amount, int resourcetype, bool queryonly, int scope = 1, int player = -1 );
    int getResource ( int amount, int resourcetype, bool queryonly, int scope = 1, int player = -1 );
    int getAvailableResource ( int amount, int resourcetype, int scope = 1 ) const;
    Resources getResource ( const Resources& res ) const;

    Resources putResource ( const Resources& res, bool queryonly, int scope = 1, int player = -1 ) { return ContainerBase::putResource ( res, queryonly, scope, player ); };
    Resources getResource ( const Resources& res, bool queryonly, int scope = 1, int player = -1 ) { return ContainerBase::getResource ( res, queryonly, scope, player ); };

    //! returns the resources that the unit is carrying
    Resources getTank() const;

    //@}
    
    //! returns the bitmapped level of height. Only one bit will be set, of course
    int getHeight() const { return height; };
    


    //! returns the units name or, if it does not exist, the unit type's name or description
    ASCString    getName() const;

   
    int getAmmo( int type, int num, bool queryOnly );
    int getAmmo( int type, int num ) const;
    int putAmmo( int type, int num, bool queryOnly );
    int maxAmmo( int type ) const ;

    //! @name Cargo related functions
    //@{

    //! weight of unit including cargo
    int weight( void ) const;
    
    //! returns the free weight that can be used for cargo
    int freeWeight();

    //@}

  protected:
    const ResourceMatrix& getRepairEfficiency() const { return repairEfficiency; };

    virtual void postRepair ( int oldDamage );

  private:
    /** if a unit (searchedInnerVehicle) is inside a transport, its payload depends also on the transport.
        Since transports can be arbitrarily stacked, this function is used to search the entire
        stack from outmost to the searched inner vehicle */
    int searchstackforfreeweight ( Vehicle* searchedInnerVehicle );

  public:

    //! Returns the size of a unit. A size is equal to the weight of the unit without any cargo or carried resources.
    int size ( void );

    //! hook that called when the turn for a player starts
    void beginTurn();
    
    //! hook that is called when a player ends his turn
    void endOwnTurn( void );

    //! hook that is called when any player (including owner) ends turn
    void endAnyTurn( void );

    //! hook that is called the next round begins ( active player switching from player8 to player1 )
    void endRound ( void );

    int getIdentification() const { return networkid; };


    //! @name Construction related functions
    //@{
    //! checks whether the unit can construct a vehicle of the given type at the given position.
    bool vehicleconstructable ( const VehicleType* tnk, int x, int y );
    
    Resources getExternalVehicleConstructionCost( const VehicleType* tnk ) const;

    /** checks whether the unit can construct a building of the given type.
    This method does not check if there is enough space around the unit to place
    the building */
    //bool buildingconstructable ( const BuildingType* bld, bool checkResources = true );
    //@}
    
    //! sets the unit (and its cargo) the a new position (the unit will not be chained to a field)
    void setnewposition ( int x, int y );
    void setnewposition ( const MapCoordinate& mc );
    void setnewposition ( const MapCoordinate& mc, const Context& context );

    /** converts the unit so it is owned by 'player'. Note that the player is passed
        as parameter (values [0..8]) )
    */
    void convert ( int player, bool recursive = true );
    
    
    //! this is a low level functions that changes the registration in the map. It's called by convert(int,bool)
    void registerForNewOwner( int player );

    //! sets the status of the unit's energy generator
    void setGeneratorStatus( bool status );
    bool getGeneratorStatus () const { return generatoractive; };

    //! callback that is called after the unit has attacked
    void postAttack( bool reactionFire, const Context& context );
    void postAttack( bool reactionFire );

    /** adds the units view to the map. The view must then be evaluated by functions like #evaluateviewcalculation ( GameMap*, int)
        \sa viewcalculation.cpp */
    void addview();

    /** removes the units view to the map. The view must then be evaluated by functions like #evaluateviewcalculation ( GameMap*, int)
        \sa viewcalculation.cpp */
    void removeview();
    
    /** resets the internal view state, so that addview() can be executed again */
    void resetview();

    /** returns true if the units view is currently added to the maps global visibility.
        \sa viewcalculation.cpp */
    bool isViewing ( ) const { return viewOnMap; };
    
    const SingleWeapon *getWeapon( unsigned weaponNum ) const;

    //! returns the units position
    MapCoordinate3D getPosition ( ) const;

    //! returns the units position; if  inside building then Height is -1
    MapCoordinate3D getPosition3D ( ) const;


    /** can the unit repair anything? This does not necessarily mean that the unit can repair
        other units, because a service weapon is additionally needed for that. It may be the
        case that the unit can only repair itself.
    */
    bool canRepair( const ContainerBase* item ) const;

    /** fills a unit with all resources it can carry 
        This function should only be called in the mapeditor !
    */
    void fillMagically( bool ammo = true, bool resources = true );


    /** generates a new unit, loads its parameters from the stream (where they have been
        previously written with #write) and returns this new unit.
    */
    static Vehicle* newFromStream ( GameMap* gamemap, tnstream& stream, int forceNetworkID = -1 );

    /** replaces the parameters of the unit by the ones form the stream. The VehicleType on
        the stream and the current one must be the same!
    */
    void read ( tnstream& stream );

    void write ( tnstream& stream, bool includeLoadedUnits = true ) const ;

    //! displays the unit at position spos on s
    void paint ( Surface& s, SPoint pos, int shadowDist = -1 ) const;
    void paint ( Surface& s, SPoint pos, bool shaded, int shadowDist = -1 ) const;

    virtual Surface getImage() const;


  private:
    void readData ( tnstream& stream );
  public:

    //! For the AI: calculating the ValueType if the unit was on the height uheight
    int getValueType ( int uheight ) const { return getFirstBit(uheight); };

    //! For the AI: calculating the ValueType
    int getValueType ( ) const { return getFirstBit(height); };


    //! to be used with EXTREME caution, and only in the mapeditor !!
    void transform ( const VehicleType* type );

    /** returns the method for changing the height in the specified direction, or none if there is none.
        \param dir  +1 is up;  -1 is down
        \param height if != 0:  assume this level of height instead of the actual one
    */
    const VehicleType::HeightChangeMethod* getHeightChange( int dir, int height = 0 ) const;

    //! Is the unit able to shoot ?
    bool weapexist() const;
    void prepareForCleanRemove() { cleanRemove = true; };
    ~Vehicle ( );

     int getMemoryFootprint() const;

  protected:
     vector<MapCoordinate> getCoveredFields();
       
};


#endif



