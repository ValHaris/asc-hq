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

#ifndef vehicletype_h_included
 #define vehicletype_h_included

 #include "typen.h"
 #include "containerbase.h"


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
     int          requiresAmmo(void) const;
     int          shootable( void ) const;
     int          service( void ) const;
     int          canRefuel ( void ) const;
     void         set ( int type );  // will be enhanced later ...
     int          gettype ( void ) { return typ; };
     int          offensive( void ) const;
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
        char*        name;          /* z.B. Exterminator  */
        char*        description;   /* z.B. Jagdbomber    */
        char*        infotext;      /* optional, kann sehr ausf?hrlich sein. Empfehlenswert ?ber eine Textdatei einzulesen */
        struct tweapons {
          char         weaponcount;
          struct tweapon {
            word         typ;            /*  BM      <= CWaffentypen  */
            char         targ;           /*  BM      <= CHoehenstufen  */
            char         sourceheight;   /*  BM  "  */
            Word         maxdistance;
            Word         mindistance;
            char         count;
            char         maxstrength;    // Wenn der Waffentyp == Mine ist, dann ist hier die Minenst„rke als Produkt mit der Bassi 64 abgelegt.
            char         minstrength;
          } waffe[8];
        } oldattack;

        int armor;

        void*        picture[8];    /*  0ø  ,  45ø   */
        char         height;        /*  BM  Besteht die Moeglichkeit zum Hoehenwechseln  */
        word         researchid;    // inzwischen ?berfl?ssig, oder ?
        int          _terrain;    /*  BM     befahrbare terrain: z.B. Schiene, Wasser, Wald, ...  */
        int          _terrainreq; /*  BM     diese Bits MšSSEN in ( field->typ->art & terrain ) gesetzt sein */
        int          _terrainkill;  /* falls das aktuelle field nicht befahrbar ist, und bei field->typ->art eine dieser Bits gesetzt ist, verschwindet die vehicle */
        char         steigung;      /*  max. befahrbare Hoehendifferenz zwischen 2 fieldern  */
        char         jamming;      /*  St„rke der Stoerstrahlen  */
        int          view;         /*  viewweite  */
        char         wait;        /*  Kann vehicle nach movement sofort schiessen ?  */
        Word         loadcapacity;      /*  Transportmoeglichkeiten  */
        word         maxunitweight; /*  maximales Gewicht einer zu ladenden vehicle */
        char         loadcapability;     /*  BM     CHoehenStufen   die zu ladende vehicle muss sich auf einer dieser Hoehenstufen befinden */
        char         loadcapabilityreq;  /*  eine vehicle, die geladen werden soll, muss auf eine diese Hoehenstufen kommen koennen */
        char         loadcapabilitynot;  /*  eine vehicle, die auf eine dieser Hoehenstufen kann, darf NICHT geladen werden. Beispiel: Flugzeuge in Transportflieger */
        Word         id;
        Resources    tank;
        Word         fuelConsumption;
        int          functions;
        char         movement[8];      /*  max. movementsstrecke  */
        char         movemalustyp;     /*  wenn ein Bodentyp mehrere Movemali fuer unterschiedliche vehiclearten, wird dieser genommen.  <= cmovemalitypes */
        char         classnum;         /* Anzahl der Klassen, max 8, min 0 ;  Der EINZIGE Unterschied zwischen 0 und 1 ist der NAME ! */
        char*        classnames[8];    /* Name der einzelnen Klassen */

        struct tclassbound {
         word         weapstrength[8];
         word         armor;
         word         techlevel;             //  Techlevel ist eine ALTERNATIVE zu ( techrequired und envetrequired )
         word         techrequired[4];
         char         eventrequired;
         int          vehiclefunctions;
       } classbound[8];    /* untergrenze (minimum), die zum erreichen dieser Klasse notwendig ist, classbound[0] gilt fuer vehicletype allgemein*/

        char         maxwindspeedonwater;
        char         digrange;        // Radius, um den nach bodensch„tzen gesucht wird.
        int          initiative;      // 0 ist ausgeglichen // 256 ist verdoppelung
        int          _terrainnot;    /*  BM     sobald eines dieser Bits gesetzt ist, kann die vehicle NICHT auf das field fahren  */
        int          _terrainreq1;  // wie terrainreq, es braucht aber nur 1 bit gesetzt zu sein
        int          objectsbuildablenum;
        int*         objectsbuildableid;

        int          weight;           // basic weight, without fuel etc.
        pterrainaccess terrainaccess;
        int          bipicture;
        int          vehiclesbuildablenum;
        int*         vehiclesbuildableid;

        void*        buildicon;
        int          buildingsbuildablenum;
        pbuildrange  buildingsbuildable;
        UnitWeapon*  weapons;
        int          autorepairrate;

        AiParameter* aiparam[8];
        char*        filename;    // just for information purposes in the main program
        int          vehicleCategoriesLoadable;

        int maxweight ( void );     // max. weight including fuel and material
        int maxsize   ( void );     // without fuel and material
        int vehicleloadable ( pvehicletype fzt ) const;
        Vehicletype ( void );
        ~Vehicletype ( );
 };



 class Vehicle : public ContainerBase {
  public:
    Vehicletype* typ;          /*  vehicleart: z.B. Schwere Fusstruppe  */
    char         color;
    int*         ammo;
    int*         weapstrength;
    char         experience;    // 0 .. 15
    char         attacked;
    char         height;       /* BM */   /*  aktuelle Hoehe: z.B. Hochfliegend  */
   private:
    char         _movement;     /*  ?briggebliebene movement fuer diese Runde  */
   public:
    char         direction;    /*  Blickrichtung  */
    Integer      xpos, ypos;   /*  Position auf map  */
    Resources    tank;
    int          energyUsed;
    Vehicle      *next;
    Vehicle      *prev;         /*  fuer lineare Liste der vehicle */

    int          connection;
    char         klasse;
    word         armor;
    int          networkid;
    char*        name;
    int          functions;
    class  ReactionFire {
         Vehicle* unit;
       public:
         ReactionFire ( Vehicle* _unit ) : unit ( _unit ) {};
         enum Status { off, init1, init2, ready };
         int enemiesAttackable;     // BM   ; gibt an, gegen welche Spieler die vehicle noch reactionfiren kann.
         int status;
         int getStatus()	{	return status;};
		 void enable ( void );
         void disable( void );
         void endTurn ( void ); // is called when the player hits the "end turn" button
    } reactionfire;
    int          generatoractive;
    AiParameter* aiparam[8];

    int getMovement ( void );

    /** sets a new distance that the unit can move
        \param cargoDivisor : the cargo of this unit gets 1/cargodivisor the change that this unit is getting; if 0 the cargo is not touched
    */
    void setMovement ( int newmove, int cargoDivisor = 2 );
    int hasMoved ( void );
    void resetMovement( void );

    void read ( pnstream stream );
    void write ( pnstream stream );

    int putResource ( int amount, int resourcetype, int queryonly, int scope = 1 );
    int getResource ( int amount, int resourcetype, int queryonly, int scope = 1 );


    int weight( void );   //!< weight of unit including cargo, fuel and material
    int cargo ( void ) const;   //!< return weight of all loaded units
    int freeweight ( int what = 0 );      // what: 0 = cargo ; 1 = material/fuel
    int size ( void );
    void endTurn( void );    //!< is executed when the player hits "end turn"
    void turnwrap ( void );   //!< is executed when the game starts a new turn ( player8 -> player1 )
    // void repairunit ( pvehicle vehicle, int maxrepair = 100 );
    void constructvehicle ( pvehicletype tnk, int x, int y );      // current cursor position will be used
    int  vehicleconstructable ( pvehicletype tnk, int x, int y );
    void putimage ( int x, int y );
    int  vehicleloadable ( pvehicle vehicle, int uheight = -1 ) const;
    void setnewposition ( int x, int y );
    void setup_classparams_after_generation ( void );
    void convert ( int col );
    void setpower( int status );
    void addview ( void );
    void removeview ( void );
    SingleWeapon *getWeapon( unsigned weaponNum );
    int buildingconstructable ( pbuildingtype bld );
    int searchForMineralResources( void );

    // int attackpossible ( int x, int y );
    // int getstrongestweapon( int aheight, int distance );

    /** should not be called except from freeweight
        \param what: 0=cargo ; 1=material/fuel
    */
    int searchstackforfreeweight( pvehicle eht, int what );

    bool canRepair( void );

    /** fills a unit with all resources it can carry and sets it class to "to be determined on map load".
        This function should only be called in the mapeditor !
    */
    void fillMagically( void );


    Vehicle ( Vehicletype* t );
    Vehicle ( Vehicletype* t, pmap actmap, int player );
    Vehicle ( pvehicle src, pmap actmap ); // if actmap == NULL  ==> unit will not be chained
    Vehicle ( pnstream strm, pmap actmap );

    void clone ( pvehicle src, pmap actmap ); // if actmap == NULL  ==> unit will not be chained
    void transform ( const pvehicletype type );     //!< to be used with EXTREME caution, and only in the mapeditor !!
    int weapexist ( void );     // Is the unit able to shoot ?
    ~Vehicle ( );
  private:
    void init ( void );

  protected:
      ResourceMatrix repairEfficiency;
      const ResourceMatrix& getRepairEfficiency ( void ) { return repairEfficiency; };

      int getMaxResourceStorageForWeight ( int resourcetype );

};


#endif



