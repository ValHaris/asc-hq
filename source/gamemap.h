/*! \file gamemap.h
    \brief Definition of THE central asc class: tmap 
*/

/***************************************************************************
                          gamemap.h  -  description
                             -------------------
    begin                : Tue Feb 17 2001
    copyright            : (C) 2001 by Martin Bickel
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


#ifndef gamemapH
 #define gamemapH

 #include <vector>

 #include "typen.h"
 #include "baseaiinterface.h"
 #include "vehicle.h"
 #include "buildings.h"
 #include "basestrm.h"
 #include "time.h"
 #include "messages.h"

//! The number of game paramters that can be specified for each map.
const int gameparameternum = 23;

//! The names of the game paramter. \sa gameparameterdefault
extern const char* gameparametername[ gameparameternum ];

extern const int gameparameterdefault [ gameparameternum ];

extern const int gameParameterLowerLimit [ gameparameternum ];
extern const int gameParameterUpperLimit [ gameparameternum ];
extern const bool gameParameterChangeableByEvent [ gameparameternum ];

enum { cgp_fahrspur,
       cgp_eis,
       cgp_movefrominvalidfields,
       cgp_building_material_factor,
       cgp_building_fuel_factor,
       cgp_forbid_building_construction,
       cgp_forbid_unitunit_construction,
       cgp_bi3_training,
       cgp_maxminesonfield,
       cgp_antipersonnelmine_lifetime,
       cgp_antitankmine_lifetime,
       cgp_mooredmine_lifetime,
       cgp_floatingmine_lifetime,
       cgp_buildingarmor,
       cgp_maxbuildingrepair,
       cgp_buildingrepairfactor,
       cgp_globalfuel,
       cgp_maxtrainingexperience,
       cgp_initialMapVisibility,
       cgp_attackPower,
       cgp_jammingAmplifier,
       cgp_jammingSlope,
       cgp_superVisorCanSaveMap };


//! an instance of an object type (#tobjecttype) on the map
class Object {
    public:
       pobjecttype typ;
       int damage;
       int dir;
       int time;
       // int dummy[4];
       Object ( void );
       Object ( pobjecttype t );
       void display ( int x, int y, int weather = 0 );
       void setdir ( int dir );
       int  getdir ( void );
};

#define cminenum 4
extern const char* MineNames[cminenum] ;
extern const int MineBasePunch[cminenum]  ;

enum MineTypes { cmantipersonnelmine = 1 , cmantitankmine, cmmooredmine, cmfloatmine  };


class Mine {
   public:
      MineTypes type;

      //! the effective punch of the mine
      int strength;

      //! the turnnumber in which the mine was placed
      int time;

      //! the player who placed the mine; range 0 .. 7      
      int player;

      //! can the mine attack this unit
      bool attacksunit ( const pvehicle veh );
};




//! a single field of the map
class  tfield {
    pmap gamemap;
    void init();
  protected:
    tfield (  );
    friend class tmap;
  public:
    tfield ( pmap gamemap_ );
    void operator= ( const tfield& f );

    void setMap ( pmap gamemap_ ) { gamemap = gamemap_; };

    //! the terraintype (#pwterraintype) of the field
    TerrainType::Weather* typ;

    //! mineral resources on this field (should be changed to #ResourcesType sometime...)
    char         fuel, material;

    //! can this field be seen be the player. Variable is bitmapped; two bits for each player. These two bits can have the states defined in ::VisibilityStates
    Word         visible;

    //! in the old octagonal version of ASC it was possible to rotate the terraintype; this is not used in the hexagonal version any more
    char         direction;

    void*      picture;

    //@{ 
    //! Various algorithms need to store some information in the fields they process. These variables are used for this.
    union  {
      struct {
        char         temp;
        char         temp2;
      }a;
      word tempw;
    };
    int          temp3;
    int          temp4;
    //@}

    pvehicle     vehicle;
    pbuilding    building;

    struct Resourceview {
      Resourceview ( void );
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


    typedef vector<Object> ObjectContainer;
    ObjectContainer objects;

    /** add an object to the field
         \param obj The object type
         \param dir The direction of the object type; -1 to use default direction
         \param force Put the object there even if it cannot normally be placed on this terrain
    **/
    void addobject ( pobjecttype obj, int dir = -1, bool force = false );

    //! removes all objects of the given type from the field
    void removeobject ( pobjecttype obj );

    //! sorts the objects. Since objects can be on different levels of height, the lower one must be displayed first
    void sortobjects ( void );

    //! checks if there are objects from the given type on the field and returns them
    pobject checkforobject ( pobjecttype o );


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

    //! the radar jamming that is on this field
    int getjamming ( void );
    int getmovemalus ( const pvehicle veh );
    int getmovemalus ( int type );

    //! can any of the mines on this field attack this unit
    int mineattacks ( const pvehicle veh );

    //! the player who placed the mines on this field.
    int mineowner ( void );

    //! mines may have a limited lifetime. This methods removes all mines whose maxmimum lifetime is exeeded
    void checkminetime ( int time );

    //! checks if the unit is standing on this field. Since units are being cloned for some checks, this method should be used instead of comparing the pointers to the unit
    bool unitHere ( const pvehicle veh );

    //! put a mine of type typ for player col (0..7) and a punch of strength on the field. Strength is an absolute value (unlike the basestrength of a mine or the punch of the mine-weapon, which are just factors)
    bool  putmine ( int col, int typ, int strength );

    /** removes a mine
         \param num The position of the mine; if num is -1, the last mine is removed)
    **/
    void  removemine ( int num ); 

    //! some variables for the viewcalculation algorithm. see #viewcalculation.cpp for details
    struct View {
      int view;
      int jamming;
      char mine, satellite, sonar, direct;
    } view[8];

   /** The visibility status for all players is stored in a bitmapped variable. This functions changes the status in this variable for a single player
      \param valtoset the value that is going to be written into the visibility variable
      \param actplayer the player for which the view is changed
   */
   void setVisibility ( int valtoset, int actplayer ) {
       int newval = (valtoset ^ 3) << ( 2 * actplayer );
       int oneval = 3 << ( 2 * actplayer );

       visible |= oneval;
       visible ^= newval;
   };


    ~tfield();
  private:
    int getx( void );
    int gety( void );
    TerrainType::MoveMalus __movemalus;
};






//! The map. THE central structure of ASC, which holds everything not globally available together
class tmap {
      void operator= ( const tmap& map );
   public:
      //! the size of the map
      word         xsize, ysize;

      //! the coordinate of the map that is displayed on upper left corner of the screen
      word         xpos, ypos;

      //! the array of fields
      pfield       field;

      //! the codeword for accessing a map in a campaign
      char         codeword[11]; 

      //! the title of the map
      ASCString    maptitle;


      struct Campaign {
          //! an identification for identifying a map in the chain of maps that make up a campaign
          Word         id;

          //! the id of the previous map in the campaign. This is only used as a fallback mechanism if the event based chaining fails. It will probably be discared sooner or later
          word         prevmap;   

          //! a campaign is usually designed to be played by a specific player
          unsigned char         player;   

          //! can the map be loaded just by knowing its filenmae? If 0, the codeword is required
          char      directaccess;   

      };

      //! the campaign properties of map
      Campaign*    campaign;

      //! the player who is currently making his moves (may be human or AI)
      signed char  actplayer; 

      //! the time in the game, mesured in a turns and moves
      GameTime    time;

      struct Weather {
         //! the idea of fog is to reduce the visibility, but this is currently not used
         char fog;

         //! the speed of wind, for the different levels of height ( 0=low level flight, ..., 2 = high level flight)
         class Wind {
            public:
              char speed;
              char direction;
              bool operator== ( const Wind& b ) const;
              Wind ( ) : speed ( 0 ), direction ( 0 ) {};
         } wind[3];
      } weather;

      /** how are Resources handled on this map
             0= "ASC mode": complex system with mineral resources etc
             1= "BI mode": simpler system like in Battle Isle
      **/
      int _resourcemode;

      //! the diplomatic status between the players
      char         alliances[8][8];


      //! the different players in ASC. There may be 8 players (0..7) and neutral units (8)
      class Player {
         public:
            //! does the player exist at all
            bool         exist();

            //! did the player exist when the turn started? Required for checking if a player has been terminated
            bool existanceAtBeginOfTurn;

            typedef list<pvehicle> VehicleList;
            //! a list of all units
            VehicleList  vehicleList;

            typedef list<pbuilding> BuildingList;
            //! a list of all units
            BuildingList  buildingList;

            //! the status of the scientific research
            tresearch    research;

            //! if the player is run by an AI, this is the pointer to it
            BaseAI*      ai;

            //! the status of the player: 0=human ; 1=AI ; 2=off
            enum tplayerstat { human, computer, off } stat;

            //! the name of the player that is used if the player is human
            ASCString       humanname;

            //! the name of the player that is used if the player is the AI
            ASCString       computername;

            //! returns the name of the player depending on the status
            const ASCString& getName( );

            //! the Password required for playing this player
            Password passwordcrc;

            class Dissection {
               public:
                  pvehicletype  fzt;
                  ptechnology   tech;
                  int           orgpoints;
                  int           points;
                  int           num;
            };

            //! the list of dissected units
            typedef list<Dissection> DissectionContainer;
            DissectionContainer dissections;

            bool __dissectionsToLoad;

            //! the list of messages that haven't been read by the player yet
            MessagePntrContainer  unreadmessage;
            bool __loadunreadmessage;

            //! the list of messages that already have been read by the player yet
            MessagePntrContainer  oldmessage;
            bool __loadoldmessage;

            //! the list of messages that have been sent yet
            MessagePntrContainer  sentmessage;
            bool __loadsentmessage;

            //! if ASC should check all events for fullfilled triggers, this variable will be set to true. This does not mean that there really ARE events that are ready to be executed
            int queuedEvents;

            //! the version of ASC that this player has used to make his last turn 
            int ASCversion;
      } player[9];

      //! a container for events that were executed during previous maps of the campaign
      peventstore  oldevents;

      //! the list of events that haven't been triggered yet.
      pevent       firsteventtocome;

      //! the list of events that already have been triggered.
      pevent       firsteventpassed;

      // required for loading the old map file format; no usage outside the loading routine
      // bool loadeventstore,loadeventstocome,loadeventpassed;

      int eventpassed ( int saveas, int action, int mapid );
      int eventpassed ( int id, int mapid );

      int          unitnetworkid;
      char         levelfinished;
      pnetwork     network;
      // int          alliance_names_not_used_any_more[8];

      struct tcursorpos {
        struct {
          integer cx;
          integer sx;
          integer cy;
          integer sy;
        } position[8];
      } cursorpos;


      /** The tribute can not only be used to demand resources from enemies but also to transfer resources to allies.
            tribute.avail[a][b].energy is the ammount of energy that player b may (still) extract from the net of player a
            tribute.paid[a][b].energy is the amount of energy that player b has already extracted from player a's net
            a is source player, b is destination player
       **/
      class ResourceTribute {
        public:
           Resources avail[8][8];
           Resources paid[8][8];
           bool empty ( );
           void read ( tnstream& stream );
           void write ( tnstream& stream );
      } tribute;

      //! the list of messages that were written this turn and are waiting to be processed at the end of the turn
      MessagePntrContainer  unsentmessage;
      bool __loadunsentmessage;

      //! these are the messages themselfs. A #pmessagelist only stores pointers to message body which are archived here
      MessageContainer messages;
      bool __loadmessages;

      //! each message has an identification number (which is incremented with each message) for referencing it in files. The id of the last message is stored here
      int           messageid;

      //! temporary variables for loading the map
      bool ___loadJournal, ___loadNewJournal, ___loadtitle;

      void allocateFields ( int x, int y );

      ASCString     gameJournal;
      ASCString     newJournal;
      Password      supervisorpasswordcrc;

      char          alliances_at_beginofturn[8];
      // pobjectcontainercrcs   objectcrc;

      class  Shareview {
         public:
            Shareview ( void ) { recalculateview = 0; };
            Shareview ( const Shareview* org );
            bool mode[8][8];
            int recalculateview;
       };
       // mode[1][6] = visible_all    =>  Spieler 1 gibt Spieler 6 die view frei

      Shareview*    shareview;

      //! if a player has won a singleplayer map, but wants to continue playing without any enemies, this will be set to 1
      int           continueplaying;

      class ReplayInfo {
      public:
          ReplayInfo ( void );

          pmemorystreambuf guidata[8];
          pmemorystreambuf map[8];
          pmemorystream    actmemstream;

          //! if stopRecordingActions > 0 then no actions will be recorded. \see LockReplayRecording
          int stopRecordingActions;
          void read ( tnstream& stream );
          void write ( tnstream& stream );
          ~ReplayInfo ( );
        };


      ReplayInfo*  replayinfo;

      //! a helper variable to store some information during the loading process. No usage outside.
      bool          __loadreplayinfo;


      //! the player which is currently viewing the map. During replays, for example, this will be different from the player that moves units
      int           playerView;
      GameTime     lastjournalchange;

      //! in BI resource mode ( see #_resourcemode , #isResourceGlobal ) , this is where the globally available resources are stored. Note that not all resources are globally available.
      Resources     bi_resource[8];

      struct PreferredFilenames {
        ASCString mapname[8];
        ASCString savegame[8];
      } preferredFileNames;

      //! for tutorial missions it is possible to highlight elements of the interface with an ellipse using the event system.
      EllipseOnScreen* ellipse;

      //! the ID of the graphic set
      int           graphicset;
      int           gameparameter_num;
      int*          game_parameter;

      int           mineralResourcesDisplayed;

      class ArchivalInformation {
         public:
            ArchivalInformation() : modifytime ( 0 ) {};
            ASCString author;
            ASCString description;
            ASCString tags;
            ASCString requirements;
            time_t modifytime;
      } archivalInformation;

      //! specifies which vehicle types can be constructed by construction units
      class UnitProduction {
         public:
            typedef vector<int> IDsAllowed;
            bool check ( int id );
            IDsAllowed idsAllowed;
      } unitProduction;

      tmap ( void );

      pvehicle getUnit ( int x, int y, int nwid );
      pvehicle getUnit ( int nwid );
      int  getgameparameter ( int num );
      void setgameparameter ( int num, int value );
      void cleartemps( int b, int value = 0 );
      bool isResourceGlobal ( int resource );
      void setupResources ( void );
      const ASCString& getPlayerName ( int playernum );
      pfield getField ( int x, int y );
      pfield getField ( const MapCoordinate& pos );
      void startGame ( );

      VisibilityStates getInitialMapVisibility( int player );

      //! resizes the map. Positive numbers enlarge the map in that direction
      int  resize( int top, int bottom, int left, int right );


      void calculateAllObjects ( void );

      void read ( tnstream& stream );
      void write ( tnstream& stream );


      pterraintype getterraintype_byid ( int id );
      pobjecttype getobjecttype_byid ( int id );
      pvehicletype getvehicletype_byid ( int id );
      pbuildingtype getbuildingtype_byid ( int id );
      ptechnology gettechnology_byid ( int id );

      pterraintype getterraintype_bypos ( int pos );
      pobjecttype getobjecttype_bypos ( int pos );
      pvehicletype getvehicletype_bypos ( int pos );
      pbuildingtype getbuildingtype_bypos ( int pos );
      ptechnology gettechnology_bypos ( int pos );

      int getTerrainTypeNum ( );
      int getObjectTypeNum ( );
      int getVehicleTypeNum ( );
      int getBuildingTypeNum ( );
      int getTechnologyNum ( );

      ~tmap();


      //! just a helper variable for loading the map; no function outside;
      bool loadOldEvents;
   private:
      pvehicle getUnit ( pvehicle eht, int nwid );
};

typedef tmap::Player Player;



#endif
