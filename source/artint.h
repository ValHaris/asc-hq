//     $Id: artint.h,v 1.17 2000-10-26 18:55:28 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.16  2000/10/26 18:14:55  mbickel
//      AI moves damaged units to repair
//      tmap is not memory layout sensitive any more
//
//     Revision 1.15  2000/09/25 20:04:35  mbickel
//      AI improvements
//
//     Revision 1.14  2000/09/25 13:25:52  mbickel
//      The AI can now change the height of units
//      Heightchaning routines improved
//
//     Revision 1.13  2000/09/17 15:17:44  mbickel
//      some restructuring; moving units out of buildings
//
//     Revision 1.12  2000/09/07 16:42:27  mbickel
//      Made some adjustments so that ASC compiles with Watcom again...
//
//     Revision 1.11  2000/09/07 15:42:09  mbickel
//     *** empty log message ***
//
//     Revision 1.10  2000/09/02 13:59:48  mbickel
//      Worked on AI
//      Started using doxygen
//
//     Revision 1.9  2000/07/29 14:54:08  mbickel
//      plain text configuration file implemented
//
//     Revision 1.8  2000/07/23 17:59:51  mbickel
//      various AI improvements
//      new terrain information window
//
//     Revision 1.7  2000/07/16 14:19:59  mbickel
//      AI has now some primitive tactics implemented
//      Some clean up
//        moved weapon functions to attack.cpp
//      Mount doesn't modify PCX files any more.
//
//     Revision 1.6  2000/07/06 11:07:25  mbickel
//      More AI work
//      Started modularizing the attack formula
//
//     Revision 1.5  2000/06/28 18:30:57  mbickel
//      Started working on AI
//      Started making loaders independent of memory layout
//      Destroyed buildings can now leave objects behind.
//
//     Revision 1.4  2000/06/19 20:05:02  mbickel
//      Fixed crash when transfering ammo to vehicle with > 8 weapons
//
//     Revision 1.3  2000/05/23 20:40:36  mbickel
//      Removed boolean type
//
//     Revision 1.2  1999/11/16 03:41:01  tmwilson
//     	Added CVS keywords to most of the files.
//     	Started porting the code to Linux (ifdef'ing the DOS specific stuff)
//     	Wrote replacement routines for kbhit/getch for Linux
//     	Cleaned up parts of the code that gcc barfed on (char vs unsigned char)
//     	Added autoconf/automake capabilities
//     	Added files used by 'automake --gnu'
//
//
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

#include "typen.h"
#include "spfst.h"
#include "unitctrl.h"
#include "building_controls.h"

#ifdef __WATCOM_CPLUSPLUS__
 typedef less<int> lessint;
#endif




    class AI : public BaseAI {
           bool _isRunning;
           int unitCounter;

           int maxTrooperMove; 
           int maxTransportMove; 
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
               public:
                  VehicleService::Service requiredService;

                  ServiceOrder ( AI* _ai, VehicleService::Service _requiredService, int UnitID, int _pos = -1 );
                  pvehicle getTargetUnit ( ) const { return ai->getMap()->getUnit ( targetUnitID );};
                  pvehicle getServiceUnit ( ) const { return ai->getMap()->getUnit ( serviceUnitID );};
           };
           typedef PointerList<ServiceOrder*> ServiceOrderContainer;
           ServiceOrderContainer serviceOrders;
           void issueServices ( );
           // void searchServices ( );

           AiThreat* fieldThreats;
           pbuilding findServiceBuilding ( const ServiceOrder& so );
           int fieldNum;

           void calculateFieldThreats ( void );
           void calculateFieldThreats_SinglePosition ( pvehicle eht, int x, int y );
           class WeaponThreatRange : public tsearchfields {
                     pvehicle veh;
                     int weap, height;
                     AiThreat* threat;
                     AI*       ai;
                  public:
                     void run ( pvehicle _veh, int x, int y, AiThreat* _threat, AI* _ai );
                     void testfield ( void );
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
            } config; 


            struct MoveVariant {
               int orgDamage;
               int damageAfterMove;
               int damageAfterAttack;
               int movex, movey;
               int attackx, attacky;
               pvehicle enemy;
               int enemyOrgDamage;
               int enemyDamage;
               int weapNum;
               int result;
               int moveDist;
            };

            class AiResult {
               public:
                  int unitsMoved;
                  int unitsWaiting;
                  AiResult ( ) : unitsMoved ( 0 ), unitsWaiting ( 0 ) {};

                  AiResult& operator+= ( const AiResult& a ) {
                     unitsMoved += a.unitsMoved;
                     unitsWaiting += a.unitsWaiting;
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

            bool moveUnit ( pvehicle veh, const MapCoordinate& destination, bool intoContainers );

            void getAttacks ( VehicleMovement& vm, pvehicle veh, TargetVector& tv );
            void searchTargets ( pvehicle veh, int x, int y, TargetVector& tl, int moveDist );
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

            void  calculateAllThreats( void );
            AiResult  tactics( void );

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
           ~AI ( );
    };

