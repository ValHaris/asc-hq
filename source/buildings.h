/***************************************************************************
                          buildings.h  -  description
                             -------------------
    begin                : Sat Feb 17 2001
    copyright            : (C) 2001 by Martin Bickel
    email                : bickel@asc-hq.org
 ***************************************************************************/

/*! \file buildingtype.h
    \brief The interface for the Building class
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

//! An actual building on the map, which references a #BuildingType
class  Building : public ContainerBase {
    MapCoordinate entryPosition;

    char         _completion;

    friend class tprocessminingfields;
  protected:
    bool isBuilding() const { return true; };

  public:
    class Work {
        public:
          virtual bool finished() = 0;
          virtual bool run() = 0;
          virtual Resources getPlus() = 0;
          virtual Resources getUsage() = 0;
    };


    class MatterConverter : public Work {
          Building* bld;
          int percentage;
        public:
          MatterConverter( Building* _bld ) ;
          virtual bool finished();
          virtual bool run();
          virtual Resources getPlus();
          virtual Resources getUsage();
    };

    class ResourceSink : public Work {
          Building* bld;
          Resources toGet;
        public:
          ResourceSink( Building* _bld ) ;
          virtual bool finished();
          virtual bool run();
          virtual Resources getPlus();
          virtual Resources getUsage();
    };


    /*
    class Research : public Work {
          Building* bld;
          int percentage;
        public:
          MatterConverter( Building* _bld ) ;
          virtual bool finished();
          virtual bool run();
          virtual Resources getPlus() {return Resources; };
          virtual Resources getUsage();
    };
    */


    class RegenerativePowerPlant : public Work {
        protected:
          Building* bld;
          Resources toProduce;
        public:
          RegenerativePowerPlant( Building* _bld ) ;
          virtual bool finished();
          virtual bool run();
          virtual Resources getUsage();
    };

    class WindPowerplant : public RegenerativePowerPlant {
        public:
          WindPowerplant( Building* _bld ) : RegenerativePowerPlant ( _bld ) { toProduce = getPlus(); };
          virtual Resources getPlus();
    };

    class SolarPowerplant : public RegenerativePowerPlant {
        public:
          SolarPowerplant( Building* _bld ) : RegenerativePowerPlant ( _bld ) { toProduce = getPlus(); };
          virtual Resources getPlus();
    };

    class BiResourceGeneration: public RegenerativePowerPlant {
        public:
          BiResourceGeneration ( Building* bld_ ) : RegenerativePowerPlant ( bld_ ) { toProduce = getPlus(); };
          virtual Resources getPlus();
    };

    class MiningStation : public Work, protected SearchFields {
         Building* bld;
         bool justQuery;
         bool hasRun;
         Resources toExtract_thisTurn;
         Resources toExtract_thisLoop;
         Resources extracted;
         Resources consumed;
      protected:
          void testfield ( const MapCoordinate& mc );
      public:
          MiningStation( Building* _bld, bool justQuery_ ) ;
          virtual bool finished();
          virtual bool run();
          virtual Resources getPlus();
          virtual Resources getUsage();
    };

    Work* spawnWorkClasses( bool justQuery );
    const pbuildingtype typ;

    pvehicletype      production[32];

    //! the Resources that are produced each turn
    Resources   plus;

    //! the maximum amount of Resources that the building can produce each turn in the ASC resource mode ; see also #bi_resourceplus
    Resources   maxplus;

    //! the current storage of Resources
    Resources   actstorage;

    //! the ammo that is stored in the building
    int         ammo[waffenanzahl];

    //! the maximum amount of research that the building can conduct every turn
    word         maxresearchpoints;

    //! the current amount of research that the building conducts every turn
    word         researchpoints;

    //! the building's name
    ASCString    name;

    //! a bitmapped variable containing the status of the resource-net connection. \see execnetcontrol()
    int          netcontrol;

    //! bitmapped: are there events that are triggered by actions affecting this building
    int          connection;

    //! is the building visible? Building can be made invisible, but this feature should be used only in some very special cases
    bool         visible;

    //! the vehicletype that the building can produce
    pvehicletype  productionbuyable[32];

    //! the maximum amount of Resources that the building can produce each turn in the BI resource mode ; see also #maxplus
    Resources    bi_resourceplus;

    //! the percantage that this build has already been repaired this turn. The maximum percentage may be limited by a gameparameter
    int           repairedThisTurn;

    AiValue*      aiparam[8];

    Building( pmap map, const MapCoordinate& entryPosition, const pbuildingtype type , int player, bool setupImages = true );

    int lastmineddist;

    bool canRepair ( const ContainerBase* item );

    static Building* newFromStream ( pmap gamemap, tnstream& stream );
    void write ( tnstream& stream, bool includeLoadedUnits = true );
    void read ( tnstream& stream );
  private:
    void readData ( tnstream& stream, int version );
  public:
 
    //! executes the resource net operations, like filling the tanks with fuel. \see netcontrol 
    void execnetcontrol ( void );
    // int  getmininginfo ( int res );

    int  putResource ( int amount,    int resourcetype, int queryonly, int scope = 1 );
    int  getResource ( int amount,    int resourcetype, int queryonly, int scope = 1 );
    Resources putResource ( const Resources& res, int queryonly, int scope = 1 ) { return ContainerBase::putResource ( res, queryonly, scope ); };
    Resources getResource ( const Resources& res, int queryonly, int scope = 1 ) { return ContainerBase::getResource ( res, queryonly, scope ); };

    //! returns the resource that the building consumes for its operation.
    Resources getResourceUsage ( );

    Resources getResourcePlus ( );

    int getIdentification();


    //! returns the picture of the building. It may depend on the current weather of the fields the building is standing on
    void* getpicture ( const BuildingType::LocalCoordinate& localCoordinate );
    
    //! changes the building's owner. \param player range: 0 .. 8
    void convert ( int player );

    //! Adds the view and jamming of the building to the player's global radar field
    void addview ( void );

    //! Removes the view and jamming of the building from the player's global radar field
    void removeview ( void );

    //! returns the local storage capacity for the given resource, which depends on the resource mode of the map. \see tmap::_resourcemode
    int  gettank ( int resource );
    
    //! returns the armor of the building. \see BuildingType::_armor
    int  getArmor( void );

    //! returns the field the buildings entry is standing on
    pfield getEntryField ( ) const;

    //! returns the position of the buildings entry
    MapCoordinate3D getEntry ( ) const;

    //! returns the pointer to the field which the given part of the building is standing on
    pfield getField( const BuildingType::LocalCoordinate& localCoordinates ) const;

    //! returns the absolute map coordinate of the given part of the building
    MapCoordinate getFieldCoordinates( const BuildingType::LocalCoordinate& localCoordinates ) const;

    //! produces ammunition and stores it in #ammo
    void produceAmmo ( int type, int num );

    //! updates the pointers to the pictures , which are part of tfield (to speed up displaying)
    void resetPicturePointers ( void );

    //! returns the position of the buildings entry
    MapCoordinate3D getPosition ( ) const { return getEntry(); };

    //! returns the position of the buildings entry
    MapCoordinate3D getPosition3D( ) const;

    //! registers the building at the given position on the map
    int  chainbuildingtofield ( const MapCoordinate& entryPos, bool setupImages = true );

    //! unregister the building from the map position
    int  unchainbuildingfromfield ( void );

    // void getpowerplantefficiency ( int* material, int* fuel );

    //! returns the completion of the building. \see setCompletion(int,bool)
    int getCompletion() const { return _completion; };

    /** Sets the level of completion of the building.
        \param completion range: 0 .. typ->construction_steps-1 . If completion == typ->construction_steps-1 the building is completed and working.
        \param setupImages Are the building image pointer of the fields the building is standing on updated?
        \see Buildingtype::construction_steps
    **/
    void setCompletion ( int completion, bool setupImages = true  );

    //! hook that is called when a turn ends
    void endTurn( void );

    //! returns a name for the building. If the building itself has a name, it will be returned. If it doesn't, the name of the building type will be returned.
    const ASCString& getName ( ) const;

    //! returns the amount of resources that the net which the building is connected to produces each turn
    Resources netResourcePlus( ) const;

    ~Building();

  protected:
     ResourceMatrix repairEfficiency;
     const ResourceMatrix& getRepairEfficiency ( void ) { return repairEfficiency; };
     virtual void postRepair ( int oldDamage ) {};
};


//! calculates some mining statistics for a mining station
class GetMiningInfo : public SearchFields {
          protected:
             void testfield ( const MapCoordinate& mc );
          public:
             class MiningInfo {
                  public:
                     MiningInfo ( );
                     Resources avail[maxminingrange+2];
                     int efficiency[maxminingrange+2];
                     Resources max[maxminingrange+2];            // the theoretical maximum of the mineral resources in the given distance
             };
             GetMiningInfo ( pmap _gamemap );
             const MiningInfo& getMiningInfo() {return miningInfo; };
             void run ( const pbuilding bld );
          protected:
             MiningInfo miningInfo;
         };




#endif
