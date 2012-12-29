
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef mapfieldH
 #define mapfieldH

 #include <vector>

 #include "typen.h"
 #include "vehicle.h"
 #include "basestrm.h"
 #include "explosivemines.h"
 
 
class Context;

//! a single field of the map
class  MapField {
    GameMap* gamemap;
    void init();
    int index;
  protected:
    MapField (  ) { init(); };
    friend class GameMap;
  public:
    MapField ( GameMap* gamemap_, int index_ ) { init(); setMap(gamemap_, index_); };
    void operator= ( const MapField& f );

    void setMap ( GameMap* gamemap_, int index_ ) { gamemap = gamemap_; index = index_; };
    GameMap* getMap() const { return gamemap; };

    //! the terraintype of the field
    TerrainType::Weather* typ;
    
    //! returns the terrain of the field
    const TerrainType* getTerrainType() const;
    
    
    //! changes the terrain type to a new one; weather is not changed
    void changeTerrainType( const TerrainType* terrain );
    

    /** mineral resources on this field. 
        \note that mineral resources are different from #Resources , there is a factor of #resource_fuel_factor and #resource_material_factor in between
    */
    char         fuel, material;

    int getMineralMaterial() const;
    int getMineralFuel() const;
    
    void setMineralMaterial( int material );
    void setMineralFuel( int fuel );
   
    //! can this field be seen be the player. Variable is bitmapped; two bits for each player. These two bits can have the states defined in ::VisibilityStates
    Uint16       visible;

    //! units standing on this object will get a bonus to their view
    int          viewbonus;


    //@{ 
    //! Various algorithms need to store some information in the fields they process.
    // These getters/setters are used for this.
    void setaTemp (char atemp);
    char getaTemp ();

    void setaTemp2 (char atemp2);
    char getaTemp2 ();

    void setTempw (Uint16 tempw);
    Uint16 getTempw ();

    void setTemp3 (int temp3);
    int getTemp3 ();

    void setTemp4 (int temp4);
    int getTemp4 ();
    
    //@}

    Vehicle*     vehicle;
    
    //! two units and the same field are only allowed temporary during movement
    Vehicle*     secondvehicle;
    
    inline Vehicle* getVehicle() const {
         if ( secondvehicle )
            return secondvehicle;
         else
            return vehicle;
      }

    
    Building*    building;

    struct Resourceview {
      Resourceview ( void );
      void setview( int player, int material, int fuel );
      void resetview( int player );
      char    visible;      // BM
      char    fuelvisible[8];
      char    materialvisible[8];
    };

    //! the mineral resources that were seen by a player on this field; since the actual amount may have decreased since the player looked, this value is not identical to the fuel and material fields.
    Resourceview*  resourceview;

    typedef list<Mine> MineContainer;
    MineContainer mines;

    //! returns the nth mine. This function should only be used by legacy code; new code should store an iterator instead of an index
    Mine& getMine ( int n );


    typedef vector< ::Object> ObjectContainer;
    ObjectContainer objects;

    //! Interface for removing objects from a field when it turns out that they can no longer exist
    class ObjectRemovalStrategy {
       public:
          virtual void removeObject( MapField* fld, const ObjectType* obj ) = 0;
          virtual ~ObjectRemovalStrategy() {};
    };
    
    
    /** add an object to the field
         \param obj The object type
         \param dir The direction of the object type; -1 to use default direction
         \param force Put the object there even if it cannot normally be placed on this terrain
         \returns true on success, false if the object could not be build
    **/
    bool addobject ( const ObjectType* obj, int dir = -1, bool force = false, ObjectRemovalStrategy* objectRemovalStrategy = NULL );

    /** removes all objects of the given type from the field
        \param obj the object type to remove
        \param force remove the object even if there are obstacles on the field (like a building standing on the object)
        \returns if the removal was successful
    */
    bool removeObject ( const ObjectType* obj, bool force = false, ObjectRemovalStrategy* objectRemovalStrategy = NULL );

    //! sorts the objects. Since objects can be on different levels of height, the lower one must be displayed first
    void sortobjects ( void );

    //! checks if there are objects from the given type on the field and returns them
    Object* checkForObject ( const ObjectType*  o );


    //! the terraintype properties. They determine which units can move over the field. This variable is recalculated from the terraintype and objects each time something on the field changes (#setparams)
    TerrainBits  bdt;

    //! are any events connected to this field
    int connection;


    //! deletes everything placed on the field
    void deleteeverything ( void );

   
    //! recalculates the terrain properties, movemalus etc from the terraintype and the objects,
    void setparams ( ObjectRemovalStrategy* objectRemovalStrategy );
    
    //! uses the SimpleObjectRemoval strategy
    void setparams (  );

    //! the defense bonus that unit get when they are attacked
    int getdefensebonus ( void );

    //! the attack bonus that unit get when they are attacking
    int getattackbonus  ( void );

    //! the weather that is on this field
    int getWeather();
    void setWeather( int weather );

    ASCString getName();
    
    //! the radar jamming that is on this field
    int getjamming ( void );
    int getmovemalus ( const Vehicle* veh );
    int getmovemalus ( int type );

    //! can any of the mines on this field attack this unit
    int mineattacks ( const Vehicle* veh );

    //! the player who placed the mines on this field.
    int mineowner ( void );

    //! checks if the unit is standing on this field. Since units are being cloned for some checks, this method should be used instead of comparing the pointers to the unit
    bool unitHere ( const Vehicle* veh );

    //! returns a pointer to the #ContainerBase of the field or NULL if there is none
    ContainerBase* getContainer();
    const ContainerBase* getContainer() const;

    
    //! returns the building if there is one with its entrance on this field
    Building* getBuildingEntrance();
    
    //! if there is a building on the field, the building is deleted (may affect othere fields too, depending on building size)
    void removeBuilding();
    
    //! if there is a unit on the field, the unit will be deleted 
    void removeUnit();
    
    //! put a mine of type typ for player owner and a punch of strength on the field. Strength is an absolute value (unlike the basestrength of a mine or the punch of the mine-weapon, which are just factors)
    bool  putmine ( int owner, MineTypes typ, int strength );

    /** removes a mine
         \param num The position of the mine; if num is -1, the last mine is removed)
    **/
    void  removemine ( int num ); 

    void endRound( int turn );

    //! some variables for the viewcalculation algorithm. see viewcalculation.cpp for details
    struct View {
      int view;
      int jamming;
      char mine, satellite, sonar, direct;
    } view[8];

   /** The visibility status for all players is stored in a bitmapped variable. This functions changes the status in this variable for a single player
      \param valtoset the value that is going to be written into the visibility variable
      \param actplayer the player for which the view is changed
   */
   void setVisibility ( VisibilityStates valtoset, int actplayer );

   VisibilityStates getVisibility( int actplayer ) {
       return VisibilityStates((visible >> (2*actplayer)) & 3);
   };

   static void resetView( GameMap* gamemap, int playersToReset );

    int getx();
    int gety();
    MapCoordinate getPosition();

    int getMemoryFootprint() const;

    ~MapField();
  private:
    TerrainType::MoveMalus __movemalus;
};





#endif

