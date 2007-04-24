/***************************************************************************
                          buildings.h  -  description
                             -------------------
    begin                : Sat Feb 17 2001
    copyright            : (C) 2001 by Martin Bickel
    email                : bickel@asc-hq.org
 ***************************************************************************/

/*! \file buildings.h
    \brief The buildings which a placed on the map
*/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef buildingsH
 #define buildingsH

 #include "typen.h"
 #include "containerbase.h"
 #include "ascstring.h"
 #include "buildingtype.h"
 #include "mapalgorithms.h"

class tfield;

//! An actual building on the map, which references a #BuildingType
class  Building : public ContainerBase {
    MapCoordinate entryPosition;

    char         _completion;

    friend class tprocessminingfields;
  protected:

    //! the percantage that this build has already been repaired this turn. The maximum percentage may be limited by a gameparameter
    int           repairedThisTurn;
    
   public:
    const BuildingType* typ;

    //! the ammo that is stored in the building
    int         ammo[waffenanzahl];

    //! the building's name
    ASCString    name;

    //! a bitmapped variable containing the status of the resource-net connection. \see execnetcontrol() \deprecated
    int          netcontrol;

    //! bitmapped: are there events that are triggered by actions affecting this building
    int          connection;

    //! is the building visible? Building can be made invisible, but this feature should be used only in some very special cases
    bool         visible;


    AiValue*      aiparam[8];

    Building( GameMap* map, const MapCoordinate& entryPosition, const BuildingType* type , int player, bool setupImages = true, bool chainToField = true);

    int lastmineddist;

    bool canRepair ( const ContainerBase* item ) const;

    static Building* newFromStream ( GameMap* gamemap, tnstream& stream, bool chainToField = true );
    void write ( tnstream& stream, bool includeLoadedUnits = true );
    void read ( tnstream& stream );
  private:
    void readData ( tnstream& stream, int version );
  public:
 
    //! executes the resource net operations, like filling the tanks with fuel. \see netcontrol 
    void execnetcontrol ( void );
    // int  getmininginfo ( int res );

    int  putResource ( int amount,    int resourcetype, bool queryonly, int scope = 1, int player = -1 );
    int  getResource ( int amount,    int resourcetype, bool queryonly, int scope = 1, int player = -1 );
    int  getResource ( int amount,    int resourcetype ) const;
    Resources putResource ( const Resources& res, bool queryonly, int scope = 1, int player = -1 ) { return ContainerBase::putResource ( res, queryonly, scope, player ); };
    Resources getResource ( const Resources& res, bool queryonly, int scope = 1, int player = -1 ) { return ContainerBase::getResource ( res, queryonly, scope, player ); };


    //! the current storage of Resources
    Resources   actstorage;
    
    int getIdentification();


    //! returns the picture of the building. It may depend on the current weather of the fields the building is standing on
    const Surface& getPicture ( const BuildingType::LocalCoordinate& localCoordinate ) const;

    void paintSingleField ( Surface& s, SPoint imgpos, BuildingType::LocalCoordinate pos ) const;

    bool isBuilding() const { return true; };
    
    
    //! changes the building's owner. \param player range: 0 .. 8
    void convert ( int player );

    //! Adds the view and jamming of the building to the player's global radar field
    void addview();

    //! Removes the view and jamming of the building from the player's global radar field
    void removeview();

    //! returns the armor of the building. \see BuildingType::_armor
    int  getArmor() const;

    //! returns the field the buildings entry is standing on
    tfield* getEntryField() const;

    //! returns the position of the buildings entry
    MapCoordinate3D getEntry ( ) const;

    //! returns the pointer to the field which the given part of the building is standing on
    tfield* getField( const BuildingType::LocalCoordinate& localCoordinates ) const;
    
    //! returns the absolute map coordinate of the given part of the building
    MapCoordinate getFieldCoordinates( const BuildingType::LocalCoordinate& localCoordinates ) const;

    
    //! converts a global coordinate into a local coordinate.
    BuildingType::LocalCoordinate getLocalCoordinate( const MapCoordinate& field ) const;
    
    //! returns the position of the buildings entry
    MapCoordinate3D getPosition ( ) const { return getEntry(); };

    //! returns the position of the buildings entry
    MapCoordinate3D getPosition3D( ) const;

    //! registers the building at the given position on the map
    int  chainbuildingtofield ( const MapCoordinate& entryPos, bool setupImages = true );

    //! unregister the building from the map position
    int  unchainbuildingfromfield ( void );

    //! returns the completion of the building. \see setCompletion(int,bool)
    int getCompletion() const { return _completion; };

    /** Sets the level of completion of the building.
        \param completion range: 0 .. typ->construction_steps-1 . If completion == typ->construction_steps-1 the building is completed and working.
        \param setupImages Are the building image pointer of the fields the building is standing on updated?
        \see Buildingtype::construction_steps
    **/
    void setCompletion ( int completion, bool setupImages = true  );

    //! hook that is called when a turn ends
    void endRound( void );

    //! returns a name for the building. If the building itself has a name, it will be returned. If it doesn't, the name of the building type will be returned.
    ASCString getName ( ) const;

    int getAmmo( int type, int num, bool queryOnly );
    int putAmmo( int type, int num, bool queryOnly );
    int maxAmmo( int type ) const { return maxint; };


    //! returns the bitmapped level of height. Only one bit will be set, of course
    int getHeight() const { return typ->buildingheight; };
    
    
    ~Building();

    virtual int repairableDamage();

     int getMemoryFootprint() const;

  protected:
     ResourceMatrix repairEfficiency;
     const ResourceMatrix& getRepairEfficiency ( void ) { return repairEfficiency; };
     virtual void postRepair ( int oldDamage );
     vector<MapCoordinate> getCoveredFields();
};


extern void doresearch ( GameMap* actmap, int player );

#endif
