//     $Id: artint.h,v 1.29 2001-01-23 21:05:09 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.28  2001/01/21 12:48:35  mbickel
//      Some cleanup and documentation
//
//     Revision 1.27  2001/01/19 13:33:46  mbickel
//      The AI now uses hemming
//      Several bugfixes in Vehicle Actions
//      Moved all view calculation to viewcalculation.cpp
//      Mapeditor: improved keyboard support for item selection
//
//     Revision 1.26  2001/01/04 15:13:27  mbickel
//      configure now checks for libSDL_image
//      AI only conquers building that cannot be conquered back immediately
//      tfindfile now returns strings instead of char*
//
//     Revision 1.25  2000/12/31 15:25:25  mbickel
//      The AI now conqueres neutral buildings
//      Removed "reset password" buttons when starting a game
//
//     Revision 1.24  2000/12/28 16:58:36  mbickel
//      Fixed bugs in AI
//      Some cleanup
//      Fixed crash in building construction
//
//     Revision 1.23  2000/12/21 11:00:44  mbickel
//      Added some code documentation
//
//     Revision 1.22  2000/11/26 14:39:02  mbickel
//      Added Project Files for Borland C++
//      Some modifications to compile source with BCC
//
//     Revision 1.21  2000/11/21 20:26:52  mbickel
//      Fixed crash in tsearchfields (used by object construction for example)
//      AI improvements
//      configure.in: added some debug output
//                    fixed broken check for libbz2
//
//     Revision 1.20  2000/11/15 19:28:33  mbickel
//      AI improvements
//
//     Revision 1.19  2000/11/14 20:36:38  mbickel
//      The AI can now use supply vehicles
//      Rewrote objecttype IO routines to make the structure independant of
//       the memory layout
//
//     Revision 1.18  2000/11/11 11:05:15  mbickel
//      started AI service functions
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

/*! \file artint.h
   The interface for the artificial intelligence of ASC. 
*/


#include <utility>
#include <map>
#include <vector>
#include <list>

#include "typen.h"
#include "spfst.h"
#include "unitctrl.h"
#include "building_controls.h"
#include "buildingtype.h"


    class AI : public BaseAI {
           bool _isRunning;
           int _vision;
           int unitCounter;

           int maxTrooperMove; 
           int maxTransportMove;
           int maxUnitMove;
           int maxWeapDist[8]; 
           int baseThreatsCalculated;

           pmap activemap;
           MapDisplayInterface* mapDisplay;

           class ServiceOrder {
                  AI* ai;
                  int targetUnitID;
                  int serviceUnitID;

                  //! if service == ammo: weapon number ; if service == resource : resource type
                  int position;
                  tgametime time;
               public:
                  VehicleService::Service requiredService;

                  ServiceOrder ( ) : ai ( NULL ), targetUnitID ( 0 ), serviceUnitID ( 0 ) {};
                  ServiceOrder ( AI* _ai, VehicleService::Service _requiredService, int UnitID, int _pos = -1 );
                  pvehicle getTargetUnit ( ) const { return ai->getMap()->getUnit ( targetUnitID );};
                  pvehicle getServiceUnit ( ) const { return ai->getMap()->getUnit ( serviceUnitID );};
                  void setServiceUnit ( pvehicle veh ) { serviceUnitID = veh->networkid; };
                  int possible ( pvehicle supplier );
                  bool execute1st ( pvehicle supplier );

                  static bool targetDestroyed ( const ServiceOrder& so )
                  {
                     return !so.getTargetUnit();
                  };
                  bool operator==( const ServiceOrder& );

                  ~ServiceOrder ();
           };

           class ServiceTargetEquals : public unary_function<ServiceOrder,bool> {
                 const pvehicle target;
              public:
                 explicit ServiceTargetEquals ( const pvehicle _target ) : target ( _target ) {};
                 bool operator() (const ServiceOrder& so ) const { return !so.getTargetUnit(); };
           };

           void removeServiceOrdersForUnit ( const pvehicle veh );


           /*
           struct ServiceOrderRating {
             float val;
             AI::ServiceOrder* so;
             bool operator> ( const ServiceOrderRating& a ) { return val > a.val; };
           };
           std::greater<ServiceOrderRating> scomp;
           */

           // typedef PointerList<ServiceOrder*> ServiceOrderContainer;

           static bool vehicleValueComp ( const pvehicle v1, const pvehicle v2 );
           static bool buildingValueComp ( const pbuilding v1, const pbuilding v2 );

           typedef list<ServiceOrder> ServiceOrderContainer;
           ServiceOrderContainer serviceOrders;
           void issueServices ( );
           void runServiceUnit ( pvehicle supplyUnit );

           bool runUnitTask ( pvehicle veh );
           // void searchServices ( );

           AiThreat* fieldThreats;
           pbuilding findServiceBuilding ( const ServiceOrder& so );
           int fieldNum;

           void checkConquer( );

           //! checks whether a building can be conquered by the enemy during the next turn
           bool checkReConquer ( pbuilding bld, pvehicle veh );
           void conquerBuilding ( pvehicle veh );
           float getCaptureValue ( const pbuilding bld, int travelTime );
           float getCaptureValue ( const pbuilding bld, const pvehicle veh );

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


                    BuildingCapture ( ) {
                       state = conq_noUnit;
                       unit = 0;
                       nearestUnit = 0;
                       captureValue = 0;
                    };
           };

           class BuildingValueComp : public binary_function<pbuilding,pbuilding,bool> {
                 AI* ai;
              public:
                 explicit BuildingValueComp ( AI* _ai ) : ai ( _ai ) {};
                 bool operator() (const pbuilding& a, const pbuilding& b ) const {
                     return ai->buildingCapture[ a->getEntry() ].captureValue > ai->buildingCapture[ b->getEntry() ].captureValue;
                 };
           };
           friend class BuildingValueComp;

           map<MapCoordinate,BuildingCapture> buildingCapture;


           void calculateFieldThreats ( void );
           void calculateFieldThreats_SinglePosition ( pvehicle eht, int x, int y );
           class WeaponThreatRange : public tsearchfields {
                     pvehicle veh;
                     int weap, height;
                     AiThreat* threat;
                     AI*       ai;
                  public:
                     void run ( pvehicle _veh, int x, int y, AiThreat* _threat );
                     void testfield ( void );
                     WeaponThreatRange( AI* _ai ) : tsearchfields ( _ai->getMap()), ai ( _ai ) {};
           };


           struct Config {
               // int movesearchshortestway;   /*  kÅrzesten oder nur irgendeinen  */
               int lookIntoTransports;   /*  gegnerische transporter einsehen  */
               int lookIntoBuildings;
               int wholeMapVisible;
               float aggressiveness;   // 1: units are equally worth ; 2
               int damageLimit;
               Resources resourceLimit;
               int ammoLimit;
               //! the maximum number of turns a unit may need to reach a town to capture
               int maxCaptureTime;
            } config;

          public:
            class MoveVariant {
               public:
                  int orgDamage;
                  int damageAfterMove;
                  int damageAfterAttack;
                  int movex, movey;
                  int attackx, attacky;
                  pvehicle enemy;
                  pvehicle attacker;
                  int enemyOrgDamage;
                  int enemyDamage;
                  int weapNum;
                  int result;
                  int moveDist;
                  bool neighbouringFieldsReachable[ sidenum ]; // used for the hemming tactic
            };
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

            bool moveUnit ( pvehicle veh, const MapCoordinate& destination, bool intoBuildings = false, bool intoTransports = false );

            void getAttacks ( VehicleMovement& vm, pvehicle veh, TargetVector& tv, int hemmingBonus );
            void searchTargets ( pvehicle veh, int x, int y, TargetVector& tl, int moveDist, VehicleMovement& vm, int hemmingBonus );
            AiResult executeMoveAttack ( pvehicle veh, TargetVector& tv );
            int getDirForBestTacticsMove ( const pvehicle veh, TargetVector& tv );
            MapCoordinate getDestination ( const pvehicle veh );
            AiResult moveToSavePlace ( pvehicle veh, VehicleMovement& vm );
            int  getBestHeight ( const pvehicle veh );

            /** chenges a vehicles height
                \returns 1 = height change successful ;
                         0 = no need to change height;
                         -1 = no space to change height
                         -2 = cannot change height here principially
            */
            int changeVehicleHeight ( pvehicle veh, VehicleMovement* vm, int preferredDirection = -1 );

            void  calculateThreat ( pvehicletype vt);
            void  calculateThreat ( pvehicle eht );
            void  calculateThreat ( pbuilding bld );
            void  calculateThreat ( pbuilding bld, int player );

            void  calculateAllThreats( void );
            AiResult  tactics( void );
            void tactics_findBestAttackOrder ( pvehicle* units, int* attackOrder, pvehicle enemy, int depth, int damage, int& finalDamage, int* finalOrder, int& finalAttackNum );
            void tactics_findBestAttackUnits ( const MoveVariantContainer& mvc, MoveVariantContainer::iterator& m, pvehicle* positions, float value, pvehicle* finalposition, float& finalvalue, int unitsPositioned );
            /** a special path finding where fields occupied by units get an addidional movemalus.
                This helps finding a path that is not thick with units and prevents units to queue all one after another
            */
            void findStratPath ( vector<MapCoordinate>& path, pvehicle veh, int x2, int y2 );

            AiResult  strategy ( void );
            AiResult  buildings ( int process );
            AiResult  transports ( int process );
            AiResult  container ( ccontainercontrols& cc );
            AiResult  executeServices ( void );
            void  setup( void );

            void reset ( void );

            class Section {
                  AI* ai;
               public:
                  int x1,y1,x2,y2;
                  int xp, yp;
                  int centerx, centery;

                  int numberOfFields;
                  AiThreat absUnitThreat;
                  AiThreat avgUnitThreat;
                  AiThreat absFieldThreat;
                  AiThreat avgFieldThreat;
                  float value[ aiValueTypeNum ];

                  int numberOfAccessibleFields ( const pvehicle veh );
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
                  Section* getBest ( const pvehicle veh, int* xtogo = NULL, int* ytogo = NULL );
                  Sections ( AI* _ai );
                  void reset( void );
            } sections;
            friend class Sections;

            void checkKeys ( void );

           AiThreat& getFieldThreat ( int x, int y );

        public:
           AI ( pmap _map ) ;
           void  run ( void );
           pmap getMap ( void ) { return activemap; };
           int getPlayer ( void ) { return activemap->actplayer; };
           void showFieldInformation ( int x, int y );
           bool isRunning ( void );
           int getVision ( void );
           ~AI ( );
    };

