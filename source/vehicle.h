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

#ifndef vehicle_h_included
 #define vehicle_h_included

 #include "typen.h"
 #include "vehicletype.h"
 #include "containerbase.h"
 #include "ascstring.h"

 #pragma pack(1)

 class Vehicle : public ContainerBase {
    Vehicle (  );
    Vehicle ( const Vehicle& v );

    //! is  the vehicle currently viewing the map? if yes, the view has to be removed on destruction
    bool viewOnMap;

    /** remaining movement for this turn.
        Note that the usable movement can be less due to lack of fuel.
        Use #getmovement to optain the usable movement
    */
    char  _movement;

    //! the amount of energy that the unit has spent this turn
    int   energyUsed;

    void init ( void );

    ResourceMatrix repairEfficiency;

    bool generatoractive;


   public:

    Vehicle ( const Vehicletype* t, pmap actmap, int player );


    const Vehicletype* typ;
    int*         ammo;
    int*         weapstrength;
    int          experience;

    //! did the unit already attack this turn
    bool         attacked;

    //! the current level of height ( BITMAPPED ! )
    char         height;

    //! the direction in which the unit is facing
    char         direction;

    //! the position on the map
    Integer      xpos, ypos;

    //! the resources that the unit is carrying
    Resources    tank;

    //! if events are triggered by this unit (if it is lost for example), this will set connection != 0
    int          connection;

    //! the generation of the vehicletype (also called 'class', as in 'salmon class')
    char         klasse;
    word         armor;

    /** a unique identification of the unit that is used everywhere in ASC
       (and not only the network protocols where is was first used, hence the name)
    */
    int          networkid;
    ASCString    name;
    int          functions;
    class  ReactionFire {
         friend class Vehicle;

         Vehicle* unit;
       public:
         ReactionFire ( Vehicle* _unit ) : unit ( _unit ) {};
         enum Status { off, init1, init2, ready };

         //! for each player that can still be attacked one bit is set
         int enemiesAttackable;
         Status getStatus()  { return status;};

         //! enables the reaction fire or returns an error code ( result < 0 = -errorNr ) 
         int enable();
         void disable();

         //! hook that is called when a turn ends
         void endTurn ( void );

       private:
         Status status;

    } reactionfire;

    AiParameter* aiparam[8];

    //! can the unit move from its current position (does not check neighbouring fields)
    bool canMove ( void );

    //! returns the movement points the unit has left for this turn. CheckFuel should almost always be true. 
    int getMovement ( bool checkFuel = true );

    /** sets a new distance that the unit can move
        \param cargoDivisor : the cargo of this unit gets 1/cargodivisor the change that this unit is getting; if 0 the cargo is not touched
    */
    void setMovement ( int newmove, int cargoDivisor = 2 );

    //! did the unit move this turn
    bool hasMoved ( void ) const;

    //! the maximum distance that the unit can drive in a single turn on the current level of height
    int maxMovement ( void );

    /** reduces the movement by the given amount. Negative values are possible.
        Don't use something like "setmovement ( getmovement() - amount )", because getmovement may return a lower amount due to lack of fuel. */
    void decreaseMovement ( int movement );


    void resetMovement( void );


    int putResource ( int amount, int resourcetype, int queryonly, int scope = 1 );
    int getResource ( int amount, int resourcetype, int queryonly, int scope = 1 );


    //! weight of unit including cargo, fuel and material
    int weight( void ) const;

    //! weight of all loaded units
    int cargo ( void ) const;

  protected:
    /** should not be called except from freeweight
        \param what: 0=cargo ; 1=material/fuel
    */
    int searchstackforfreeweight( pvehicle eht, int what );

    //! returns the maximum amount of the resource 'resourcetype' that the unit can carry
    int getMaxResourceStorageForWeight ( int resourcetype );

    const ResourceMatrix& getRepairEfficiency ( void ) { return repairEfficiency; };

    virtual void postRepair ( int oldDamage );


  public:

    /** returns the free weight that can be used for cargo
        \param what: 0=cargo ; 1=material/fuel
    */
    int freeweight ( int what = 0 );

    int size ( void );

    //! hook that is called when a turn ends
    void endTurn( void );

    //! hook that is called the next round begins ( active player switching from player8 to player1 )
    void endRound ( void );


    void constructvehicle ( pvehicletype tnk, int x, int y );      // current cursor position will be used
    bool vehicleconstructable ( pvehicletype tnk, int x, int y );

    //! displays the unit at position x/y on the screen
    void putimage ( int x, int y );

    /** can the vehicle be loaded. If uheight is passed, it is assumed that vehicle is at
        the height 'uheight' and not the actual level of height
    */
    bool vehicleloadable ( pvehicle vehicle, int uheight = -1 ) const;

    //! sets the unit (and its cargo) the a new position (the unit will not be chained to a field)
    void setnewposition ( int x, int y );
    void setnewposition ( const MapCoordinate& mc );

    void setup_classparams_after_generation ( void );

    /** converts the unit so it is owned by 'player'. Note that the player is passed
        as parameter (values [0..8]), and not his color (which would be the values
        [0,8,16,..,64] )
    */
    void convert ( int player );

    //! sets the status of the unit's energy generator
    void setGeneratorStatus( bool status );
    bool getGeneratorStatus () const { return generatoractive; };

    void addview ( void );
    void removeview ( void );
    bool isViewing ( ) const { return viewOnMap; };
    const SingleWeapon *getWeapon( unsigned weaponNum );
    bool buildingconstructable ( pbuildingtype bld );

    /** searches for mineral resources.
        \returns > 0 on success ; < 0 on failure (error number is returned => message.txt )
    */
    int searchForMineralResources( void );

    MapCoordinate3D getPosition ( );

    /** can the unit repair anything? This does not necessarily mean that the unit can repair
        other units, because a service weapon is additionally needed for that. It may be the
        case that the unit can only repair itself.
    */
    bool canRepair( void );

    /** fills a unit with all resources it can carry and sets it class to "to be determined on map load".
        This function should only be called in the mapeditor !
    */
    void fillMagically( void );


    /** generates a new unit, loads its parameters from the stream (where they have been
        previously written with #write) and returns this new unit.
    */
    static Vehicle* newFromStream ( pmap gamemap, tnstream& stream );


    /** replaces the parameters of the unit by the ones form the stream. The VehicleType on
        the stream and the current one must be the same!
    */
    void read ( tnstream& stream );

    void write ( tnstream& stream, bool includeLoadedUnits = true );
  private:
    void readData ( tnstream& stream );
  public:

    //! For the AI: calculating the ValueType if the unit was on the height uheight
    int getValueType ( int uheight ) { return log2(uheight); };

    //! For the AI: calculating the ValueType
    int getValueType ( ) { return log2(height); };


    //! to be used with EXTREME caution, and only in the mapeditor !!
    void transform ( const pvehicletype type );

    //! Is the unit able to shoot ?
    bool weapexist ( void );
    ~Vehicle ( );
};

 #pragma pack()

#endif



