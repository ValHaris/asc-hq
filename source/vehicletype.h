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

#ifndef vehicletype_h_included
 #define vehicletype_h_included

 #include "typen.h"
 #include "containerbase.h"
 #include "ascstring.h"
 #include "baseaiinterface.h"
 #include "terraintype.h"


//! The number of 'special' vehicle functions
const int cvehiclefunctionsnum = 27;
extern const char*  cvehiclefunctions[];
 #define cfsonar 1
 #define cfparatrooper 2
 #define cfminenleger 4
 #define cf_trooper 8
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
 #define cfwindantrieb ( 1 << 19 )
 #define cfautorepair ( 1 << 20 )
 #define cfgenerator ( 1 << 21 )
 #define cfautodigger ( 1 << 22 )
 #define cfkamikaze ( 1 << 23 )
 #define cfmineimmune ( 1 << 24 )
 #define cfenergyref ( 1 << 25 )

 #define cfvehiclefunctionsanzeige 0xFFFFFFFF

extern const char* AItasks[];
extern const char* AIjobs[];


 //! A single weapon of a #Vehicletype
 class SingleWeapon {
   #ifdef converter
    public:
   #else
    private:
   #endif
     int          typ;
    public:
     int          targ;           /*  BM      <= CHoehenstufen  */
     int          sourceheight;   /*  BM  "  */
     int          maxdistance;
     int          mindistance;
     int          count;
     int          maxstrength;    // Wenn der Waffentyp == Mine ist, dann ist hier die Minenst„rke als Produkt mit der Bassi 64 abgelegt.
     int          minstrength;
     int          efficiency[13]; // floating and driving are the same ; 0-5 is lower ; 6 is same height ; 7-12 is higher
     int          targets_not_hittable; // BM   <=  cmovemalitypes
    public:
     int          getScalarWeaponType(void) const;
     bool          requiresAmmo(void) const;
     bool          shootable( void ) const;
     bool          service( void ) const;
     bool          canRefuel ( void ) const;
     void         set ( int type );  // will be enhanced later ...
     int          gettype ( void ) const { return typ; };
     bool         offensive( void ) const;
     ASCString    getName ( void );
     void         runTextIO ( PropertyContainer& pc );
 };

 //! all the weapons of a #Vehicletype
 class  UnitWeapon {
   public:
     int count;
     SingleWeapon weapon[16];
     UnitWeapon ( void );
 };



 class Vehicletype : public ContainerBaseType {
    public:
        ASCString    name;          /* z.B. Exterminator  */
        ASCString    description;   /* z.B. Jagdbomber    */
        ASCString    infotext;      /* optional, kann sehr ausf?hrlich sein. Empfehlenswert ?ber eine Textdatei einzulesen */
        const ASCString&    getName() const;
        int armor;

        void*        picture[8];    /*  0ø  ,  45ø   */
        int          height;        /*  BM  Besteht die Moeglichkeit zum Hoehenwechseln  */
        int          researchid;    // inzwischen ?berfl?ssig, oder ?
        int          steigung;      /*  max. befahrbare Hoehendifferenz zwischen 2 fieldern  */
        int          jamming;      /*  St„rke der Stoerstrahlen  */
        int          view;         /*  viewweite  */
        bool         wait;        /*  Kann vehicle nach movement sofort schiessen ?  */
        int          loadcapacity;      /*  Transportmoeglichkeiten  */
        int          maxunitweight; /*  maximales Gewicht einer zu ladenden vehicle */
        int          loadcapability;     /*  BM     CHoehenStufen   die zu ladende vehicle muss sich auf einer dieser Hoehenstufen befinden */
        int          loadcapabilityreq;  /*  eine vehicle, die geladen werden soll, muss auf eine diese Hoehenstufen kommen koennen */
        int          loadcapabilitynot;  /*  eine vehicle, die auf eine dieser Hoehenstufen kann, darf NICHT geladen werden. Beispiel: Flugzeuge in Transportflieger */
        int          id;
        Resources    tank;
        int          fuelConsumption;
        int          functions;
        vector<int>  movement;      /*  max. movementsstrecke  */
        int          movemalustyp;     /*  wenn ein Bodentyp mehrere Movemali fuer unterschiedliche vehiclearten, wird dieser genommen.  <= cmovemalitypes */

        char         classnum;         /* Anzahl der Klassen, max 8, min 0 ;  Der EINZIGE Unterschied zwischen 0 und 1 ist der NAME ! */
        ASCString    classnames[8];    /* Name der einzelnen Klassen */

        struct tclassbound {
         word         weapstrength[8];
         word         armor;
         word         techlevel;             //  Techlevel ist eine ALTERNATIVE zu ( techrequired und envetrequired )
         word         techrequired[4];
         char         eventrequired;
         int          vehiclefunctions;
       } classbound[8];    /* untergrenze (minimum), die zum erreichen dieser Klasse notwendig ist, classbound[0] gilt fuer vehicletype allgemein*/

        int          maxwindspeedonwater;
        int          digrange;        // Radius, um den nach bodensch„tzen gesucht wird.
        int          initiative;      // 0 ist ausgeglichen // 256 ist verdoppelung

        int           weight;           // basic weight, without fuel etc.
        TerrainAccess terrainaccess;
        int           bipicture;

        void*        buildicon;

        vector<IntRange> buildingsBuildable;
        vector<IntRange> vehiclesBuildable;
        vector<IntRange> objectsBuildable;
        vector<IntRange> objectsRemovable;

        UnitWeapon   weapons;
        int          autorepairrate;

        AiValue* aiparam[8];

        ASCString    filename;
        ASCString    location;    // just for information purposes in the main program

        int          vehicleCategoriesLoadable;

        int maxweight ( void ) const ;     // max. weight including fuel and material
        int maxsize   ( void ) const ;     // without fuel and material
        int vehicleloadable ( pvehicletype fzt ) const;
        Vehicletype ( void );
        void read ( tnstream& stream ) ;
        void write ( tnstream& stream ) const ;
        void runTextIO ( PropertyContainer& pc );
        ~Vehicletype ( );
     private:
        void setupPictures();
        void setupRemovableObjectsFromOldFileLayout();
 };


#endif



