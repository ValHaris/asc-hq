/*! \file gamemap.h
    \brief Definition of THE central asc class: GameMap 
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
 #include <time.h>

 #include "typen.h"
 #include "baseaiinterface.h"
 #include "vehicle.h"
 #include "buildings.h"
 #include "basestrm.h"
 #include "messages.h"
 #include "research.h"
 #include "password.h"
#ifdef WEATHERGENERATOR
 # include "weatherarea.h"
#endif
 #include "objects.h"
 #include "mapfield.h"
 #include "networkinterface.h"
 #include "player.h"
 
 #include "actions/actioncontainer.h"
 
 class RandomGenerator{
   public:

      RandomGenerator(int seedValue);
      ~RandomGenerator();
      
      unsigned int getPercentage();
      unsigned int getRandomValue(int upperLimit);
      unsigned int getRandomValue (int lowerLimit, int upperLimit);
 };
 
  
#ifdef WEATHERGENERATOR
class WeatherSystem;
#endif

//! The number of game paramters that can be specified for each map.
const int gameparameternum = 35;

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
       cgp_objectGrowOnOtherObjects,
       cgp_researchOutputMultiplier,
       cgp_produceOnlyResearchedStuffInternally,
       cgp_produceOnlyResearchedStuffExternally };


struct GameParameterSettings {
         const char* name;
         int defaultValue;
         int minValue;
         int maxValue;
         bool changeableByEvent;
         bool legacy;
         const char* longName;
};
extern GameParameterSettings gameParameterSettings[gameparameternum ];
       
class LoadNextMap {
       public:
          int id;
          LoadNextMap( int ID ) : id(ID) {};
};






class OverviewMapHolder : public SigC::Object {
      GameMap& map;
      Surface overviewMapImage;
      Surface completedMapImage;
      bool initialized;
      bool secondMapReady;
      bool completed;
      bool connected;
      int x;
      int y;
      Surface createNewSurface();
      
   protected:   
      bool idleHandler( );
      bool init();
      void drawNextField( bool signalOnCompletion = true );
      bool updateField( const MapCoordinate& pos );
      
   public:
      OverviewMapHolder( GameMap& gamemap );
   
      /** 
      returns the overview surface for the map. 
      \param complete complete the image if it is not ready (might take several seconds)
      */
      const Surface& getOverviewMap( bool complete = true );
      
      static void clearmap( GameMap* actmap );
      
      static SigC::Signal0<void> generationComplete;
      
      void resetSize();
            
      void startUpdate();
      void clear( bool allImages = false );
      void connect();
};


class Event;


//! The map. THE central structure of ASC, which holds everything not globally available together
class GameMap {
      void operator= ( const GameMap& map );
      bool dialogsHooked;
   public:
      //! the size of the map
      int          xsize, ysize;

      //! the coordinate of the map that is displayed on upper left corner of the screen
      // int          xpos, ypos;

      //! the array of fields
      tfield*       field;

      //! the codeword for accessing a map in a campaign
      ASCString    codeWord;

      //! the title of the map
      ASCString    maptitle;


      struct Campaign {
          //! is this a campaign map?
          bool avail;
         
          //! an identification for identifying a map in the chain of maps that make up a campaign
          int         id;

          //! can the map be loaded just by knowing its filenmae? If 0, the codeword is required
          bool      directaccess;
          Campaign();
      } campaign;

      //! the player who is currently making his moves (may be human or AI)
      signed char  actplayer; 

      //! the time in the game, mesured in a turns and moves
      GameTime    time;

      struct Weather {
         Weather():windSpeed(0),windDirection(0){};
         int windSpeed;
         int windDirection;
      } weather;

#ifdef WEATHERGENERATOR
      WeatherSystem* weatherSystem;
#endif
      /** how are Resources handled on this map
             0= "ASC mode": complex system with mineral resources etc
             1= "BI mode": simpler system like in Battle Isle
      **/
      int _resourcemode;


      Player player[9];

      int getPlayerCount() const { return 8; };
      
      Player& getPlayer( PlayerID p )
      {
         return player[p.getID() ];
      }

      const Player& getPlayer( PlayerID p ) const
      {
         return player[p.getID() ];
      }


      Player& getCurrentPlayer()
      {
         return player[actplayer];
      }

      int getNeutralPlayerNum() const { return 8; };
      
      MapCoordinate& getCursor();
      

      int eventID;

      typedef PointerList<Event*> Events;
      Events events;

      vector<GameTime> eventTimes;

      int eventpassed ( int saveas, int action, int mapid );
      int eventpassed ( int id, int mapid );

      class IDManager {
            int          unitnetworkid;
            typedef map<int, Vehicle*> VehicleLookupCache;
            VehicleLookupCache vehicleLookupCache; 
            
            friend class SpawnUnit;
            friend class GameMap;
                        
            
         public:
            IDManager() : unitnetworkid(0) {};
            
            //! returns a new and unique ID 
            int getNewNetworkID();
            void registerUnitNetworkID( Vehicle* veh );
            void unregisterUnitNetworkID( Vehicle* veh );
            
            void readData ( tnstream& stream );
            void writeData ( tnstream& stream ) const;
      } idManager;


      char         levelfinished;

      enum State { 
         Normal,
         // NormalAuto,    /*!< normal operation, without user interaction */
         // NormalManual,  /*!< normal operation, with user interaction  */
         Replay,        /*!< map is currently used to run a replay. Some ingame events are deactivated */
         Destruction    /*!< only to be used by units and buildings. To speed up map destruction, the view won't be recalculated. No signals will be send when units & buildings are destroyed, either  */
      } state;


      /** The tribute can not only be used to demand resources from enemies but also to transfer resources to allies.
            tribute.avail[S][D].energy is the ammount of energy that player D may (still) extract from the net of player S
            tribute.paid[D][S].energy is the amount of energy that player D has already extracted from player S's net
            S is source player, D is destination player
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

      //! these are the messages themselfs. A MessagePntrContainer only stores pointers to message body which are archived here
      MessageContainer messages;
      bool __loadmessages;

      //! each message has an identification number (which is incremented with each message) for referencing it in files. The id of the last message is stored here
      int           messageid;

      //! temporary variables for loading the map
      bool ___loadJournal, ___loadNewJournal, ___loadtitle, ___loadLegacyNetwork;

      void allocateFields ( int x, int y, TerrainType::Weather* terrain = NULL );

      ASCString     gameJournal;
      ASCString     newJournal;
      
      Password      supervisorpasswordcrc;

      class ReplayInfo {
      public:
          ReplayInfo ( void );

          tmemorystreambuf* guidata[8];
          tmemorystreambuf* map[8];
          tmemorystream*    actmemstream;

          //! if stopRecordingActions > 0 then no actions will be recorded. \see LockReplayRecording
          int stopRecordingActions;
          void read ( tnstream& stream );
          void write ( tnstream& stream );
          
           //! Close the replay logging at the end of a players or the ai's turn.
          void closeLogging();
          ~ReplayInfo ( );
          
        };

      //! Records all action that have been done for undo/redo purposes
      ActionContainer actions;

      ReplayInfo*  replayinfo;

      //! a helper variable to store some information during the loading process. No usage outside.
      bool          __loadreplayinfo;


   private:
      int           playerView;
   public:

      /** the player which is currently viewing the map. 
          During replays, for example, this will be different from the player that moves units
          -1 means: everything is visible
          -2 means: nothing is visible */
      int   getPlayerView() const;
      void  setPlayerView( int player );

      GameTime     lastjournalchange;

      //! in BI resource mode ( see #_resourcemode , #isResourceGlobal ) , this is where the globally available resources are stored. Note that not all resources are globally available.
      Resources     bi_resource[8];

      struct PreferredFilenames {
        ASCString mapname[8];
        ASCString savegame[8];
      } preferredFileNames;

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

      GameMap ( void );

      Vehicle* getUnit ( int x, int y, int nwid );
      Vehicle* getUnit ( int nwid, bool consistencyCheck = true );
      const Vehicle* getUnit ( int nwid, bool consistencyCheck = true ) const;
      ContainerBase* getContainer ( int nwid );
      const ContainerBase* getContainer ( int nwid ) const;
      int  getgameparameter ( GameParameter num ) const;
      void setgameparameter ( GameParameter num, int value );
      void cleartemps( int b = -1, int value = 0 );
      bool isResourceGlobal ( int resource );
      tfield* getField ( int x, int y );
      tfield* getField ( const MapCoordinate& pos );
      
      
      /** @name Turn Management
        *  These methods control the game advance of a player to the next player
        */
      //@{
      
      //! prepares a new game for being played
      void startGame ( );

      //! called when a player starts his turn
      void beginTurn();
      
      
      //! called after a player ends his turn
      void endTurn();

      //! called between endTurn() of player 7 and the next turn of player 0
      void endRound();


      SigC::Signal1<void,Player&> sigPlayerTurnBegins;
      SigC::Signal1<void,Player&> sigPlayerUserInteractionBegins;
      SigC::Signal1<void,Player&> sigPlayerUserInteractionEnds;
      SigC::Signal1<void,Player&> sigPlayerTurnEnds;
      SigC::Signal1<void,Player&> sigPlayerTurnHasEnded;

      static SigC::Signal1<void,GameMap&> sigMapDeletion;
      static SigC::Signal2<void,GameMap*,Player&> sigPlayerTurnEndsStatic;
      
      //! called when a new round starts (after switching from player 7 to player 0 )
      SigC::Signal0<void> newRound;


      //! if a player has won a singleplayer map, but wants to continue playing without any enemies, this will be set to 1
      bool  continueplaying;
      
      
      //@}

      
            
      
      VisibilityStates getInitialMapVisibility( int player );

      //! resizes the map. Positive numbers enlarge the map in that direction
      int  resize( int top, int bottom, int left, int right );

      // bool compareResources( GameMap* replaymap, int player, ASCString* log = NULL );

      void calculateAllObjects ( void );

      void read ( tnstream& stream );
      void write ( tnstream& stream );

      OverviewMapHolder overviewMapHolder;

      /** @name Access to item types
       *  
       */
      //@{

      
      pterraintype getterraintype_byid ( int id );
      ObjectType* getobjecttype_byid ( int id );
      const ObjectType* getobjecttype_byid ( int id ) const;
      Vehicletype* getvehicletype_byid ( int id );
      const Vehicletype* getvehicletype_byid ( int id ) const;
      BuildingType* getbuildingtype_byid ( int id );
      const BuildingType* getbuildingtype_byid ( int id ) const;
      const Technology* gettechnology_byid ( int id );

      pterraintype getterraintype_bypos ( int pos );
      ObjectType* getobjecttype_bypos ( int pos );
      Vehicletype* getvehicletype_bypos ( int pos );
      BuildingType* getbuildingtype_bypos ( int pos );
      const Technology* gettechnology_bypos ( int pos );

      int getTerrainTypeNum ( );
      int getObjectTypeNum ( );
      int getVehicleTypeNum ( );
      int getBuildingTypeNum ( );
      int getTechnologyNum ( );

      //@}
      
      ~GameMap();

      //! just a helper variable for loading the map; no function outside;
      bool loadOldEvents;

      //! generated a pseudo-random number with the map-internal seed
      int random( int max );
      
      void guiHooked();
      bool getGuiHooked() { return dialogsHooked; };

      
      GameTransferMechanism* network;

      int getMemoryFootprint() const;

   private:
      Vehicle* getUnit ( Vehicle* eht, int nwid );

      void objectGrowth();
      void setupResources ( void );
      
      //! adds the current players new journal entries to the map journal
      void processJournal();

      unsigned int randomSeed;

      static void setPlayerMode( Player& p, State s );
};



#endif

