/*! \file ai.h
    \brief The interface for the artificial intelligence of ASC. 
*/

/*
    This file is part of Advanced Strategic Command; http://www.asc-hq.de
    Copyright (C) 1994-1999  Martin Bickel  and  Marc Schellenberger

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; see the file COPYING. If not, write to the 
    Free Software Foundation, Inc., 59 Temple Place, Suite 330, 
    Boston, MA  02111-1307  USA
*/



#include <utility>
#include <map>
#include <vector>
#include <list>

#include "../typen.h"
#include "../terraintype.h"
#include "../objecttype.h"
#include "../spfst.h"
#include "../unitctrl.h"
#include "../buildingtype.h"
#include "../astar2.h"


    class AI : public BaseAI {
           bool benchMark;
           bool strictChecks;

           bool _isRunning;
           VisibilityStates _vision;
           int unitCounter;
           int player;

           int maxTrooperMove; 
           int maxTransportMove;
           int maxUnitMove;
           int maxWeapDist[8]; 
           int baseThreatsCalculated;

           GameMap* activemap;
           MapDisplayInterface* mapDisplay;
           MapDisplayInterface* rmd;

           class ServiceOrder {
                  AI* ai;
                  int targetUnitID;
                  int serviceUnitID;

                  //! if service == ammo: weapon number ; if service == resource : resource type
                  GameTime time;
                  int failure;
                  Building* nextServiceBuilding;
                  int nextServiceBuildingDistance;
                  bool active;
               public:
                  VehicleService::Service requiredService;
                  int position;

                  ServiceOrder ( ) : ai ( NULL ), targetUnitID ( 0 ), serviceUnitID ( 0 ), failure ( 0 ), nextServiceBuilding ( 0 ), active ( false ) {};
                  ServiceOrder ( AI* _ai, VehicleService::Service _requiredService, int UnitID, int _pos = -1 );
                  ServiceOrder ( AI* _ai, tnstream& stream );
                  AStar3D::Path::iterator lastmatchServiceOrder ( AI* _ai, tnstream& stream );
                  Vehicle* getTargetUnit ( ) const { return ai->getMap()->getUnit ( targetUnitID );};
                  Vehicle* getServiceUnit ( ) const { return ai->getMap()->getUnit ( serviceUnitID );};
                  void setServiceUnit ( Vehicle* veh );
                  int possible ( Vehicle* supplier );
                  bool execute1st ( Vehicle* supplier );
                  bool timeOut ( );
                  bool canWait ( );

                  void serviceFailed() { failure++; };
                  bool completelyFailed();

                  bool serviceUnitExists();

                  static void releaseServiceUnit ( ServiceOrder& so );

                  void write ( tnstream& stream ) const;
                  void read ( tnstream& read );

                  static bool targetDestroyed ( const ServiceOrder& so )
                  {
                     return !so.getTargetUnit();
                  };
                  static bool invalid ( const ServiceOrder& so );
                  bool valid ( ) const;
                  static void activate( ServiceOrder& so );

                  bool operator==( const ServiceOrder& so ) const;
                  bool operator!=( const ServiceOrder& so ) const;


                  ~ServiceOrder ();
           };
           friend class ServiceOrder;

           void removeServiceOrdersForUnit ( const Vehicle* veh );

           class ServiceTargetEquals : public unary_function<ServiceOrder&,bool> {
                 const Vehicle* target;
              public:
                 explicit ServiceTargetEquals ( const Vehicle* _target ) : target ( _target ) {};
                 bool operator() (const ServiceOrder& so ) const;
           };



           static bool vehicleValueComp ( const Vehicle* v1, const Vehicle* v2 );
           static bool buildingValueComp ( const Building* v1, const Building* v2 );

           typedef list<ServiceOrder> ServiceOrderContainer;
           ServiceOrderContainer serviceOrders;
           //! cycles through all units and checks for necessary services
           void issueServices ( );

           //! issues a single service. If the same service-order already exists, it will not be issued a second time
           ServiceOrder& issueService ( VehicleService::Service requiredService, int UnitID, int pos = -1 );

           ServiceOrder& issueRefuelOrder ( Vehicle* veh, bool returnImmediately );
           void runServiceUnit ( Vehicle* supplyUnit );

           class RefuelConstraint {
                   AI& ai;
                   Vehicle* veh;
                   AStar3D* ast;
                   typedef map<int, Building*> ReachableBuildings;
                   ReachableBuildings reachableBuildings;

                   typedef map<int, MapCoordinate3D> LandingPositions;
                   LandingPositions landingPositions;
                   bool positionsCalculated;
                   int maxMove;
                public:
                   RefuelConstraint ( AI& ai_, Vehicle* veh_, int maxMove_ = -1 ) : ai ( ai_ ), veh ( veh_ ), ast(NULL), positionsCalculated(false), maxMove ( maxMove_ ) {};
                   MapCoordinate3D getNearestRefuellingPosition ( bool buildingRequired, bool refuel, bool repair );
                   bool returnFromPositionPossible ( const MapCoordinate3D& pos, int theoreticalFuel = -1 );
                   //! checks whether the unit can crash do to lack of fuel; this is usually true for airplanes. A unit that does not crash does not need to care about landing positions.
                   void findPath();
                   static bool necessary (const Vehicle* veh, AI& ai );
                   ~RefuelConstraint() { if (ast) delete ast; };
           };
           friend class RefuelConstraint;

           bool runUnitTask ( Vehicle* veh );
           // void searchServices ( );

           class FieldInformation {
               public:
                  AiThreat threat;
                  int control;
                  int units[8];
                  void reset ( );
                  FieldInformation ( ) { reset(); };
           };

           FieldInformation* fieldInformation;
           int fieldNum;
           AiThreat& getFieldThreat ( int x, int y );
           FieldInformation& getFieldInformation ( int x, int y );

           class CheckFieldRecon : public SearchFields {
                protected:
                   void testfield ( const MapCoordinate& mc ) ;
                   int ownFields[3];
                   int enemyFields[3];
                   int player;
                   AI* ai;
                public:
                   CheckFieldRecon ( AI* _ai );
                   int run ( int x, int y);
           };
           friend class CheckFieldRecon;


           MapCoordinate3D findServiceBuilding ( const ServiceOrder& so, int* distance = NULL );

           void checkConquer( );
           void runReconUnits();

           //! checks whether a building can be conquered by the enemy during the next turn
           bool checkReConquer ( Building* bld, Vehicle* veh );
           float getCaptureValue ( const Building* bld, int travelTime );
           float getCaptureValue ( const Building* bld,  Vehicle* veh );

           class BuildingCapture {
                  public:
                    enum BuildingCaptureState { conq_noUnit,
                           conq_unitNotConq,
                           conq_conqUnit,
                           conq_unreachable } state;
                    int unit;
                    vector<int> guards;

                    float captureValue;
                    int nearestUnit;

                    void write ( tnstream& stream ) const;
                    void read ( tnstream& stream );

                    BuildingCapture ( ) {
                       state = conq_noUnit;
                       unit = 0;
                       nearestUnit = 0;
                       captureValue = 0;
                    };
           };

           class BuildingValueComp : public binary_function<Building*,Building*,bool> {
                 AI* ai;
              public:
                 explicit BuildingValueComp ( AI* _ai ) : ai ( _ai ) {};
                 bool operator() (const Building*& a, const Building*& b ) const {
                     return ai->buildingCapture[ a->getEntry() ].captureValue > ai->buildingCapture[ b->getEntry() ].captureValue;
                 };
           };
           friend class BuildingValueComp;

           typedef map<MapCoordinate,BuildingCapture> BuildingCaptureContainer;
           BuildingCaptureContainer buildingCapture;


           void calculateFieldInformation ( void );
           void calculateFieldThreats_SinglePosition ( Vehicle* eht, int x, int y );
           class WeaponThreatRange : public SearchFields {
                     Vehicle* veh;
                     int weap, height;
                     AiThreat* threat;
                     AI*       ai;
                     void testfield ( const MapCoordinate& mc );
                  public:
                     void run ( Vehicle* _veh, int x, int y, AiThreat* _threat );
                     WeaponThreatRange( AI* _ai ) : SearchFields ( _ai->getMap()), ai ( _ai ) {};
           };


           struct Config {
               // int movesearchshortestway;   /*  krzesten oder nur irgendeinen  */
               int lookIntoTransports;   /*  gegnerische transporter einsehen  */
               int lookIntoBuildings;
               int wholeMapVisible;
               float aggressiveness;   // 1: units are equally worth ; 2
               int damageLimit;
               Resources resourceLimit;
               int ammoLimit;
               //! the maximum number of turns a unit may need to reach a town to capture
               int maxCaptureTime;

               //! the maximum time in 1/100 sec that a the ai may try to optimize an attack
               int maxTactTime;
               int waitForResourcePlus;
            } config;

          public:
            class MoveVariant {
               public:
                  int orgDamage;
                  int damageAfterMove;
                  int damageAfterAttack;
                  MapCoordinate3D movePos;
                  int attackx, attacky;
                  Vehicle* enemy;
                  Vehicle* attacker;
                  int enemyOrgDamage;
                  int enemyDamage;
                  int weapNum;
                  float result;
                  int moveDist;
                  bool neighbouringFieldsReachable[ sidenum ]; // used for the hemming tactic
                  float positionThreat;
            };
            static bool moveVariantComp ( const AI::MoveVariant* mv1, const AI::MoveVariant* mv2 );
         private:

            typedef vector<MoveVariant> MoveVariantContainer;

            class AiResult {
               public:
                  int unitsMoved;
                  int unitsWaiting;
                  int unitsDestroyed;
                  AiResult ( ) : unitsMoved ( 0 ), unitsWaiting ( 0 ), unitsDestroyed( 0 ) {};

                  AiResult& operator+= ( const AiResult& a ) {
                     unitsMoved += a.unitsMoved;
                     unitsWaiting += a.unitsWaiting;
                     unitsDestroyed += a.unitsDestroyed;
                     return *this;
                  };
            };



            class TargetVector : public std::vector<MoveVariant*>	{
               public:
                  ~TargetVector() {
                     for ( iterator it = begin(); it != end(); it++ )
                         delete *it;
                  };
            };

            bool moveUnit ( Vehicle* veh, const MapCoordinate3D& destination, bool intoBuildings = true, bool intoTransports = true );

            /** \returns 1 = destination reached;
                         0 = everything ok, but not enough movement to reach destination;
                         -1 = error
             */
            int moveUnit ( Vehicle* veh, const AStar3D::Path& path, bool intoBuildings = true, bool intoTransports = true );

            void getAttacks ( AStar3D& vm, Vehicle* veh, TargetVector& tv, int hemmingBonus, bool justOne = false, bool executeService = true );
            void searchTargets ( Vehicle* veh, const MapCoordinate3D& pos, TargetVector& tl, int moveDist, AStar3D& vm, int hemmingBonus );
            bool targetsNear( Vehicle* veh );

            AiResult executeMoveAttack ( Vehicle* veh, TargetVector& tv );
            int getDirForBestTacticsMove ( const Vehicle* veh, TargetVector& tv );
            MapCoordinate getDestination ( Vehicle* veh );
            AiResult moveToSavePlace ( Vehicle* veh, VehicleMovement& vm, int preferredHeight = -1 );
            int  getBestHeight (  Vehicle* veh );
            float getAttackValue ( const tfight& battle, const Vehicle* attackingUnit, const Vehicle* attackedUnit, float factor = 1 );

            /** chenges a vehicles height
                \returns 1 = height change successful ;
                         0 = no need to change height;
                         -1 = no space to change height
                         -2 = cannot change height here principially
            */
            int changeVehicleHeight ( Vehicle* veh, VehicleMovement* vm, int preferredDirection = -1 );

            void  calculateThreat ( const Vehicletype* vt);
            void  calculateThreat ( Vehicle* eht );
            void  calculateThreat ( Building* bld );
            void  calculateThreat ( Building* bld, int player );

            static AiParameter::JobList chooseJob ( const Vehicletype* typ );
            friend class CalculateThreat_Vehicle;

            /** This structure helps identifying units which don't reach any
                action because the AI wants to put them into the wrong sections.

            map<int,int> wrongSectionDetection; */

            void  calculateAllThreats( void );
            AiResult  tactics( void );
            void tactics_findBestAttackOrder ( Vehicle** units, int* attackOrder, Vehicle* enemy, int depth, int damage, int& finalDamage, int* finalOrder, int& finalAttackNum );
            void tactics_findBestAttackUnits ( const MoveVariantContainer& mvc, MoveVariantContainer::iterator& m, Vehicle** positions, float value, Vehicle** finalposition, float& finalvalue, int unitsPositioned, int recursionDepth, int startTime );

            /** a special path finding where fields occupied by units get an addidional movemalus.
                This helps finding a path that is not thick with units and prevents units to queue all one after another
            */
            void findStratPath ( vector<MapCoordinate>& path, Vehicle* veh, int x2, int y2 );

            class  UnitDistribution {
               public:
                  static const int groupCount = 6;
                  enum Group { attack, rangeattack, conquer, other, recon, service };
                  bool calculated;
                  float group[groupCount];
                  UnitDistribution( ) : calculated ( false ) { for ( int i = 0; i < groupCount; i++ ) group[i] = 0; };
                  void read( tnstream& stream );
                  void write ( tnstream& stream ) const;
            };
            UnitDistribution originalUnitDistribution;
            UnitDistribution::Group getUnitDistributionGroup ( Vehicle* veh );
            UnitDistribution::Group getUnitDistributionGroup ( Vehicletype* veh );

            UnitDistribution calcUnitDistribution();
            struct ProductionRating {
               const Vehicletype* vt;
               Building*    bld;
               float        rating;
               bool operator< ( const ProductionRating& pr ) { return rating < pr.rating; };
            };

            typedef map<int,pair<float,float> > UnitTypeSuccess; // first: enemy value, second: own value
            UnitTypeSuccess unitTypeSuccess;

            class VehicleTypeEfficiencyCalculator {
                  AI& ai;
                  float ownValue;
                  int ownTypeID;
                  float enemyValue;
                  int orgOwnDamage;
                  int orgEnemyDamage;
                  int enemyID;
                  int ownID;
                  Vehicle* attacker;
                  Vehicle* target;
               public:
                  VehicleTypeEfficiencyCalculator ( AI& _ai, Vehicle* _attacker, Vehicle* _target );
                  void calc();
            };
            friend class VehicleTypeEfficiencyCalculator;

            void      production();

            AiResult  strategy();
            AiResult  buildings ( int process );
            AiResult  transports ( int process );
            AiResult  container ( ContainerBase* cb );
            AiResult  executeServices ();
            void      setup();

            void reset();

            typedef map<MapCoordinate,int> ReconPositions;
            ReconPositions reconPositions;
            void calcReconPositions();

            class Section {
                  AI* ai;
               public:
                  int x1,y1,x2,y2;
                  int xp, yp;
                  int centerx, centery;
                  int units_heading_here;

                  int numberOfFields;
                  AiThreat absUnitThreat;
                  AiThreat avgUnitThreat;
                  AiThreat absFieldThreat;
                  AiThreat avgFieldThreat;
                  float value[ aiValueTypeNum ];

                  int numberOfAccessibleFields ( const Vehicle* veh );
                  void init ( int _x, int _y, int xsize, int ysize, int _xp, int _yp );
                  void init ( AI* _ai, int _x, int _y, int xsize, int ysize, int _xp, int _yp );
                  Section ( AI* _ai ) : ai ( _ai ) {};
                  Section ( ) { ai = NULL; };    //
            };
            friend class Section;

            class Sections {
                  AI* ai;
                  Section* section;
               public:
                  int sizeX ;
                  int sizeY ;
                  int numX ;
                  int numY ;
                  void calculate ( void );
                  Section& getForCoordinate ( int xc, int yc );         //!< returns the section whose center is nearest to x,y
                  Section& getForPos ( int xn, int yn );                //!< returns the xth and yth section

                  // secondRun should only be used when this function calls itself recursively
                  Section* getBest ( int pass, Vehicle* veh, MapCoordinate3D* dest = NULL, bool allowRefuellOrder = false, bool secondRun = false );
                  Sections ( AI* _ai );
                  void reset( void );
            } sections;
            friend class Sections;

            void checkKeys ( void );

            void diplomacy ();

            
        public:
           AI ( GameMap* _map, int _player ) ;

           void run (  ) { run ( false );};
           //! starts the Ai. If benchmark is true, the AI might take longer since it is not time limited, it won't display any graphics and will output the time it needed to run completely
           void run ( bool benchmark );

           //! returns the map this AI runson
           GameMap* getMap ( void ) { return activemap; };

           //! returns the number of the player which is controlled by this ai
           int getPlayerNum ( void ) { return player; };

           Player& getPlayer ( void ) { return getMap()->player[player]; };
           Player& getPlayer ( int player ) { return getMap()->player[player]; };
           Player& getPlayer ( PlayerID id ) { return getMap()->player[id.getID()]; };
           void showFieldInformation ( int x, int y );
           bool isRunning ( void );

           /**  the AI uses a different vision than human player, to counter the fact
                that a human player can "know" a map and take a look before starting to
                play. This function returns the minimum visibility state of a field.
                \sa tfield::visible , VisibilityStates
           */
           VisibilityStates getVision ( void );

           void read ( tnstream& stream );
           void write ( tnstream& stream ) const ;
           ~AI ( );
    };
