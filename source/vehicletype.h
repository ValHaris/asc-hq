/***************************************************************************
                          vehicletype.h  -  description
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

#ifndef vehicletypeH
 #define vehicletypeH

 #include "typen.h"
 #include "containerbase.h"
 #include "ascstring.h"
 #include "baseaiinterface.h"
 #include "terraintype.h"
 #include "research.h"

/*
//! The number of 'special' vehicle functions
extern const char*  cvehiclefunctions[];
 #define cfsonar 1
 #define cfparatrooper 2
 #define cfminenleger 4
 #define cfrepair 16
 #define cf_conquer 32
 #define cf_moveafterattack 64
 #define cfsatellitenview 128
 #define cfputbuilding 256
 #define cfmineview 512
 #define cfvehicleconstruction 1024
 #define cfspecificbuildingconstruction 2048
 #define cffuelref 4096
 #define cficebreaker 8192
 #define cfnoairrefuel 16384
 #define cfmaterialref 32768
 #define cffahrspur ( 1 << 17 )
 #define cfmanualdigger ( 1 << 18 )
 #define cfno_reactionfire ( 1 << 19 )
 #define cfautorepair ( 1 << 20 )
 #define cfgenerator ( 1 << 21 )
 #define cfautodigger ( 1 << 22 )
 #define cfkamikaze ( 1 << 23 )
 #define cfmineimmune ( 1 << 24 )
 #define cfenergyref ( 1 << 25 )
 #define cfownFieldJamming ( 1 << 26 )
 #define cfmovewithRF ( 1 << 27 )
 #define cfonlytransmove ( 1 << 28 )

 #define cfvehiclefunctionsanzeige 0xFFFFFFFF
*/

#define cwaffentypennum 13
 extern const char*  cwaffentypen[cwaffentypennum] ;
 #define cwcruisemissile 0
 #define cwcruisemissileb ( 1 << cwcruisemissile )
 #define cwminen 1
 #define cwmineb ( 1 << cwminen   )
 #define cwbombn 2
 #define cwbombb ( 1 << cwbombn  )
 #define cwlargemissilen 3
 #define cwlargemissileb ( 1 << cwlargemissilen  )
 #define cwsmallmissilen 4
 #define cwsmallmissileb ( 1 << cwsmallmissilen  )
 #define cwtorpedon 5
 #define cwtorpedob ( 1 << cwtorpedon  )
 #define cwmachinegunn 6
 #define cwmachinegunb ( 1 << cwmachinegunn )
 #define cwcannonn 7
 #define cwcannonb ( 1 << cwcannonn )
 #define cwweapon ( cwcruisemissileb | cwbombb | cwlargemissileb | cwsmallmissileb | cwtorpedob | cwmachinegunb | cwcannonb | cwlaserb )
 #define cwshootablen 11
 #define cwshootableb ( 1 << cwshootablen  )
 #define cwlasern 10
 #define cwlaserb ( 1 << cwlasern  )
 #define cwammunitionn 9
 #define cwammunitionb ( 1 << cwammunitionn )
 #define cwservicen 8
 #define cwserviceb ( 1 << cwservicen )
 #define cwobjectplacementn 12
 #define cwobjectplacementb ( 1 << cwobjectplacementn )
 extern const int cwaffenproduktionskosten[cwaffentypennum][3];  /*  Angabe: Waffentyp; energy - Material - Sprit ; jeweils fuer 5er Pack */


 extern const bool weaponAmmo[cwaffentypennum];



 //! A single weapon of a #Vehicletype
 class SingleWeapon {
     int          typ;
    public:
     SingleWeapon ();
     //! the weapon can attack targets at these levels of height (bitmapped)
     int          targ;

     //! the weapon can be shot from these levels of height (bitmapped)
     int          sourceheight;

     //! the maximum distance the weapon can shoot
     int          maxdistance;

     //! the minimal distance the weapon can shoot
     int          mindistance;

     //! amount of ammunition the unit having this weapon can carry
     int          count;

     //! strength of the weapon when fired over the minimal distance
     int          maxstrength;

     //! strength of the weapon when fired over the maximum distance
     int          minstrength;

     /** the targeting accuracy of the weapon over different height differences between the attacking unit and the target.
         The levels "ground" and "floating" are assumed to be the same.
         All values are in percent.
         The index for this array is the height difference+6
         Example: low flying airplane attacking a submerged submarine:
                  height difference is -2 ; index here is 4
     */
     int          efficiency[13];

     /** the effectiveness of the weapon against different targets.
         All values are in percent. \see cmovemalitypes
     */
     int          targetingAccuracy[cmovemalitypenum];

     //! the number of shots that the laser is recharged each turn
     int          laserRechargeRate;

     //! the resources that recharging the laser for a single shot requires
     Resources    laserRechargeCost;

     int          reactionFireShots;

     ASCString    soundLabel;

     ASCString    name;

     int          getScalarWeaponType(void) const;
     bool         requiresAmmo(void) const;
     bool         shootable( void ) const;
     bool         service( void ) const;
     bool         placeObjects() const;
     bool         canRefuel ( void ) const;
     void         set ( int type );  // will be enhanced later ...
     int          gettype ( void ) const { return typ; };
     bool         offensive( void ) const;
     ASCString    getName ( void ) const;
     static ASCString   getIconFileName( int weaponType );
     void         runTextIO ( PropertyContainer& pc );
     bool         equals( const SingleWeapon* otherWeapon ) const;
 };

 //! all the weapons of a #Vehicletype
 class  UnitWeapon {
   public:
     int count;
     SingleWeapon weapon[16];
     UnitWeapon ( void );
 };


 //! The class describing properties that are common to all vehicles of a certain kind. \sa Vehicle
 class Vehicletype : public ContainerBaseType {
        //! the image of the unit.
        Surface  image;
    public:
        static const int legacyVehicleFunctionNum = 29;
        
        //! short description of the units role, for example "strategic bomber"
        ASCString    description;

        ASCString    getName() const;

        int armor;

        
        //! the levels of height which this unit can enter
        int          height;

        //! if a transport moves the movement for the units inside a transport is decreased by 1/n of the tranport's distance
        double     cargoMovementDivisor;

        //! If the unit cannot attack in the same turn after it has moved, it has to wait
        bool         wait;

        //! the fuel consumption to move a single field
        int          fuelConsumption;

        //! the distance a unit can travel each round. One value for each of the 8 levels of height
        vector<int>  movement;

        //! The category of the unit. Original used only to distinguish only between different movement costs for a field, this categorization is now used for many more things. \see cmovemalitypes
        int          movemalustyp;

        //! the maximum speed of the wind that the unit can survive when on open water without sinking
        int          maxwindspeedonwater;

        //! radius of the circle in which a unit can search for mineral resolures (measured in number of fields, not distance !)
        int          digrange;

        //! unused
        int          initiative;

        //! the weight of the unit, without fuel or other cargo
        int           weight;

        //! the terrain this unit can move to
        TerrainAccess terrainaccess;

        //! the image index from the GraphicSet , or -1 if no graphics from graphic sets are used.
        int           bipicture;

        //! the ids of buildings this unit can construct
        vector<IntRange> buildingsBuildable;

        //! the ids of units this unit can construct
        vector<IntRange> vehiclesBuildable;

        //! the ids of objects this unit can construct
        vector<IntRange> objectsBuildable;

        //! the ids of objects this unit can remove
        vector<IntRange> objectsRemovable;

        //! the group-ids of objects this unit can construct
        vector<IntRange> objectGroupsBuildable;

        //! the group-ids of objects this unit can remove
        vector<IntRange> objectGroupsRemovable;

        //! the IDs of objects that are automatically layed by moving the movement
        vector<IntRange> objectLayedByMovement;

        //! The weapons
        UnitWeapon   weapons;

        //! the damage this unit can repair itself automatically each turn.
        int          autorepairrate;

        //! if the unit is destroyed, it can leave an wreckage object behind ( < 0 to disable )
        vector<int> wreckageObject;

        //! some information the AI stores about this unit
        mutable AiValue* aiparam[8];

        //! the recommended task for the unit, set by the unit creater
        AiParameter::Job recommendedAIJob;

        //! returns the maximum weight of this unit without fuel and material
        int maxsize   ( void ) const ;

        //! this label can select a special sound to be played when moving
        ASCString    movementSoundLabel;

        //! this label can select a special sound to be played when this unit is killed
        ASCString    killSoundLabel;

        vector<int> guideSortHelp;

        int heightChangeMethodNum;
        class HeightChangeMethod{
            public:
              int startHeight;
              int heightDelta;
              int moveCost;
              bool canAttack;
              int dist;

              void runTextIO ( PropertyContainer& pc );
              void read ( tnstream& stream ) ;
              void write ( tnstream& stream ) const ;
        };
        vector<HeightChangeMethod> heightChangeMethod;

        int maxSpeed ( ) const;

        Vehicletype ( void );
        void read ( tnstream& stream ) ;
        void write ( tnstream& stream ) const ;
        void runTextIO ( PropertyContainer& pc );
        ~Vehicletype ( );
        Resources calcProductionsCost();

        int getMoveMalusType() const {
           return movemalustyp;
        }

        struct JumpDrive {
           JumpDrive() : height(0), maxDistance(maxint) {};
           //! bitmapped: on these levels of height the jump drive can be activated
           int height; 
           Resources consumption;
           TerrainAccess targetterrain;
           int maxDistance;
        } jumpDrive;
           
        
        
        void  paint ( Surface& s, SPoint pos, int player, int direction = 0 ) const;
        const Surface&  getImage () const { return image;};
        Surface&  getImage () { return image;};
        
        int getMemoryFootprint() const;

        static BitSet convertOldFunctions( int abilities, const ASCString& location );
    private:
        void setupRemovableObjectsFromOldFileLayout();
 };


extern ASCString getUnitReference ( Vehicle* veh );

#endif



