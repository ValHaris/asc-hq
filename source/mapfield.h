
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
 


//! a single field of the map
class  tfield {
    GameMap* gamemap;
    void init();
  protected:
    tfield (  );
    friend class GameMap;
  public:
    tfield ( GameMap* gamemap_ );
    void operator= ( const tfield& f );

    void setMap ( GameMap* gamemap_ ) { gamemap = gamemap_; };

    //! the terraintype of the field
    TerrainType::Weather* typ;

    //! mineral resources on this field (should be changed to #Resources sometime...)
    char         fuel, material;

    //! can this field be seen be the player. Variable is bitmapped; two bits for each player. These two bits can have the states defined in ::VisibilityStates
    Uint16       visible;

    //! units standing on this object will get a bonus to their view
    int          viewbonus;


    //@{ 
    //! Various algorithms need to store some information in the fields they process. These variables are used for this.
    union  {
      struct {
        char         temp;
        char         temp2;
      }a;
      Uint16 tempw;
    };
    int          temp3;
    int          temp4;
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

    /** add an object to the field
         \param obj The object type
         \param dir The direction of the object type; -1 to use default direction
         \param force Put the object there even if it cannot normally be placed on this terrain
         \returns true on success, false if the object could not be build
    **/
    bool addobject ( const ObjectType* obj, int dir = -1, bool force = false );

    //! removes all objects of the given type from the field
    void removeobject ( const ObjectType* obj, bool force = false );

    //! sorts the objects. Since objects can be on different levels of height, the lower one must be displayed first
    void sortobjects ( void );

    //! checks if there are objects from the given type on the field and returns them
    Object* checkforobject ( const ObjectType*  o );


    //! the terraintype properties. They determine which units can move over the field. This variable is recalculated from the terraintype and objects each time something on the field changes (#setparams)
    TerrainBits  bdt;

    //! are any events connected to this field
    int connection;


    //! deletes everything placed on the field
    void deleteeverything ( void );

    //! recalculates the terrain properties, movemalus etc from the terraintype and the objects,
    void setparams ( void );

    //! the defense bonus that unit get when they are attacked
    int getdefensebonus ( void );

    //! the attack bonus that unit get when they are attacking
    int getattackbonus  ( void );

    //! the weather that is on this field
    int getweather ( void );
    void setweather( int weather );

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

    //! put a mine of type typ for player col (0..7) and a punch of strength on the field. Strength is an absolute value (unlike the basestrength of a mine or the punch of the mine-weapon, which are just factors)
    bool  putmine ( int col, int typ, int strength );

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

    int getMemoryFootprint() const;

    ~tfield();
  private:
    TerrainType::MoveMalus __movemalus;
};





#endif

