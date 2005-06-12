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
 #include "gameeventsystem.h"
 #include "research.h"
 #include "password.h"
 #include "weatherarea.h"
 #include "objects.h"
 #include "mapfield.h"
 
 class RandomGenerator{
   public:

      RandomGenerator(int seedValue);
      ~RandomGenerator();
      
      unsigned int getPercentage();
      unsigned int getRandomValue(int upperLimit);
      unsigned int getRandomValue (int lowerLimit, int upperLimit);
 };
 
 
class WeatherSystem;

//! The number of game paramters that can be specified for each map.
const int gameparameternum = 32;

//! The names of the game paramter. \sa gameparameterdefault
extern const char* gameparametername[ gameparameternum ];

extern const int gameparameterdefault [ gameparameternum ];

extern const int gameParameterLowerLimit [ gameparameternum ];
extern const int gameParameterUpperLimit [ gameparameternum ];
extern const bool gameParameterChangeableByEvent [ gameparameternum ];

enum GameParameter { cgp_fahrspur,
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
       cgp_superVisorCanSaveMap,
       cgp_objectsDestroyedByTerrain,
       cgp_trainingIncrement,
       cgp_experienceDivisorAttack,
       cgp_disableDirectView,
       cgp_disableUnitTransfer,
       cgp_experienceDivisorDefense,
       cgp_debugEvents,
       cgp_objectGrowthMultiplier,
       cgp_objectGrowOnOtherObjects };


class LoadNextMap {
       public:
          int id;
          LoadNextMap( int ID ) : id(ID) {};
};






class OverviewMapHolder : public SigC::Object {
      tmap& map;
      Surface overviewMapImage;
      bool initialized;
      bool completed;
      int x;
      int y;
      
   protected:   
      bool idleHandler( );
      bool init();
      void drawNextField( bool signalOnCompletion = true );
      void updateField( const MapCoordinate& pos );
      
   public:
      OverviewMapHolder( tmap& gamemap );
   
      /** 
      returns the overview surface for the map. 
      \param complete complete the image if it is not ready (might take several seconds)
      */
      const Surface& getOverviewMap( bool complete = true );
      
      static void clear( tmap* actmap );
      
      static SigC::Signal0<void> generationComplete;
      
      void startUpdate();
      void clear();
};




//! The map. THE central structure of ASC, which holds everything not globally available together
class tmap {
      void operator= ( const tmap& map );
   public:
      //! the size of the map
      int          xsize, ysize;

      //! the coordinate of the map that is displayed on upper left corner of the screen
      int          xpos, ypos;

      //! the array of fields
      pfield       field;

      //! the codeword for accessing a map in a campaign
      char         codeword[11];

      //! the title of the map
      ASCString    maptitle;


      struct Campaign {
          //! an identification for identifying a map in the chain of maps that make up a campaign
          int         id;

          //! the id of the previous map in the campaign. This is only used as a fallback mechanism if the event based chaining fails. It will probably be discared sooner or later
          int         prevmap;   

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

      /*struct Weather {
         Weather():fog(0),windSpeed(0),windDirection(0){};
         //! the idea of fog is to reduce the visibility, but this is currently not used
         int fog;

         int windSpeed;
         int windDirection;
      } weather;*/

      WeatherSystem* weatherSystem;
      /** how are Resources handled on this map
             0= "ASC mode": complex system with mineral resources etc
             1= "BI mode": simpler system like in Battle Isle
      **/
      int _resourcemode;

      //! the diplomatic status between the players
      char         alliances[8][8];


      //! the different players in ASC. There may be 8 players (0..7) and neutral units (8)
      class Player {
            friend class tmap;
            int player;
         public:
            //! does the player exist at all
            bool         exist();

            //! did the player exist when the turn started? Required for checking if a player has been terminated
            bool existanceAtBeginOfTurn;

            typedef list<Vehicle*> VehicleList;
            //! a list of all units
            VehicleList  vehicleList;

            typedef list<Building*> BuildingList;
            //! a list of all units
            BuildingList  buildingList;

            //! the status of the scientific research
            Research    research;

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
                  const Technology*   tech;
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

            struct PlayTime {
              int turn;
              time_t date;
            };
            typedef list<PlayTime> PlayTimeContainer;

            //! The time this player ended his turns. This is very informative in email games with > 2 players to find out who is delaying the game.
            PlayTimeContainer playTime;

            MapCoordinate cursorPos;

            DI_Color getColor();
            
      } player[9];

      MapCoordinate& getCursor();
      
      typedef map<int, Vehicle*> VehicleLookupCache;
      VehicleLookupCache vehicleLookupCache; 

      int eventID;

      typedef PointerList<Event*> Events;
      Events events;

      vector<GameTime> eventTimes;

      /*
      //! a container for events that were executed during previous maps of the campaign
      peventstore  oldevents;

      //! the list of events that haven't been triggered yet.
      pevent       firsteventtocome;

      //! the list of events that already have been triggered.
      pevent       firsteventpassed;
      */

      // required for loading the old map file format; no usage outside the loading routine
      // bool loadeventstore,loadeventstocome,loadeventpassed;

      int eventpassed ( int saveas, int action, int mapid );
      int eventpassed ( int id, int mapid );

      int          unitnetworkid;
      char         levelfinished;
      pnetwork     network;
      // int          alliance_names_not_used_any_more[8];

      //! only to be used by units and buildings. To speed up map destruction, the view won't be recalculated. No signals will be send when units & buildings are destroyed, either 
      bool __mapDestruction;

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
            tribute.paid[b][a].energy is the amount of energy that player b has already extracted from player a's net
            a is source player, b is destination player
       **/
      class ResourceTribute {
        public:
           Resources avail[8][8];
           Resources paid[8][8];

           //! for the messages that are send each turn it is necessary to record how much of the transfer happend during the last turn
           Resources payStatusLastTurn[8][8];
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

      class  Shareview {
         public:
            Shareview ( void );
            Shareview ( const Shareview* org );
            bool mode[8][8];
            int recalculateview;
            void read ( tnstream& stream );
            void write( tnstream& stream );
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

      Vehicle* getUnit ( int x, int y, int nwid );
      Vehicle* getUnit ( int nwid );
      ContainerBase* getContainer ( int nwid );
      int  getgameparameter ( GameParameter num );
      void setgameparameter ( GameParameter num, int value );
      void cleartemps( int b = -1, int value = 0 );
      bool isResourceGlobal ( int resource );
      void setupResources ( void );
      const ASCString& getPlayerName ( int playernum );
      pfield getField ( int x, int y );
      pfield getField ( const MapCoordinate& pos );
      void startGame ( );

      //! called after a player ends his turn
      void endTurn();

      //! called between endTurn() of player 7 and the next turn of player 0
      void endRound();


      //! called when a new round starts (after switching from player 7 to player 0 )
      SigC::Signal0<void> newRound;



      //! changes to the next player and calls endRound() if necessary. \Returns false if there are no players left
      bool nextPlayer();

      VisibilityStates getInitialMapVisibility( int player );

      //! resizes the map. Positive numbers enlarge the map in that direction
      int  resize( int top, int bottom, int left, int right );

      bool compareResources( tmap* replaymap, int player, ASCString* log = NULL );

      void calculateAllObjects ( void );

      void read ( tnstream& stream );
      void write ( tnstream& stream );

      OverviewMapHolder overviewMapHolder;

      pterraintype getterraintype_byid ( int id );
      pobjecttype getobjecttype_byid ( int id );
      pvehicletype getvehicletype_byid ( int id );
      pbuildingtype getbuildingtype_byid ( int id );
      const Technology* gettechnology_byid ( int id );

      pterraintype getterraintype_bypos ( int pos );
      pobjecttype getobjecttype_bypos ( int pos );
      pvehicletype getvehicletype_bypos ( int pos );
      pbuildingtype getbuildingtype_bypos ( int pos );
      const Technology* gettechnology_bypos ( int pos );

      int getTerrainTypeNum ( );
      int getObjectTypeNum ( );
      int getVehicleTypeNum ( );
      int getBuildingTypeNum ( );
      int getTechnologyNum ( );

      ~tmap();

      //! just a helper variable for loading the map; no function outside;
      bool loadOldEvents;

      //! generated a pseudo-random number with the map-internal seed
      int random( int max );

   private:
      Vehicle* getUnit ( Vehicle* eht, int nwid );

      void objectGrowth();

      unsigned int randomSeed;
};

typedef tmap::Player Player;



#endif

