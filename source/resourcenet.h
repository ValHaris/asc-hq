/***************************************************************************
                          resourcenet.cpp  -  description
                             -------------------
    begin                : Tue Jan 28 2001
    copyright            : (C) 2001 by Martin Bickel
    email                : bickel@asc-hq.org
 ***************************************************************************/

/*! \file resourcenet.h
    \brief Handling the connection of buildings by pipelines, powerlines etc.
*/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef resourcenetH
 #define resourcenetH

#include "gamemap.h"

//! Base class for the management of resource networks and globally stored resources
class MapNetwork {
                static int instancesrunning;
             protected:
                GameMap* actmap;
                int pass;

                MapCoordinate startposition;

                virtual int fieldavail ( int x, int y ) = 0;
                virtual int searchfinished ( void ) = 0;
                virtual void checkvehicle ( Vehicle* v ) = 0;
                virtual void checkbuilding ( Building* b ) = 0;
                virtual int globalsearch ( void ) = 0;

                virtual void searchbuilding ( int x, int y );
                virtual void searchvehicle  ( int x, int y );
                virtual void searchfield ( int x, int y, int dir );
                void searchAllVehiclesNextToBuildings ( int player );
             public:
                enum Scope { singleField, net, wholeMap, globalPool };
                virtual void start ( int x, int y );
                MapNetwork ( GameMap* gamemap, int checkInstances = 1 );
                virtual ~MapNetwork();
           };

class ResourceNet : public MapNetwork {
               public:
                  ResourceNet ( GameMap* gamemap, int _scope = -1 ) : MapNetwork ( gamemap, _scope != 0 ), resourcetype(-1), scope(_scope) {};
               protected:
                  int resourcetype;
                  int scope;

                  virtual int fieldavail ( int x, int y );
                  virtual int globalsearch ( void ) { return scope; };
             };

class GetConnectedBuildings : public ResourceNet {
                protected:
                   void checkvehicle ( Vehicle* v ) {};
                   void checkbuilding ( Building* b ) { buildingContainer.push_back ( b ); };
                   int searchfinished ( void ) { return false; };

                public:
                   typedef vector<Building*> BuildingContainer;
                   BuildingContainer& buildingContainer;
                   GetConnectedBuildings ( BuildingContainer& buildingContainer_, GameMap* gamemap, int resourceType ) : ResourceNet ( gamemap, gamemap->isResourceGlobal(resourceType)?2:1), buildingContainer ( buildingContainer_) { resourcetype = 0; };
};

class StaticResourceNet : public ResourceNet {
               protected:
                  int need;
                  int got;
                  int queryonly;
                  int player;

                  virtual int searchfinished ( void );

              public:
                  StaticResourceNet ( GameMap* gamemap, int scope = -1 ) : ResourceNet ( gamemap, scope ) {};
                  int getresource ( int x, int y, int resource, int _need, int _queryonly, int _player, int _scope );
                       /* _scope:  0 : only this field
                                   1 : net
                                   2 : global
                       */
       };

class GetResource : public StaticResourceNet {
              protected:
                  int tributegot[3][8];
                  virtual void checkvehicle ( Vehicle* v );
                  virtual void checkbuilding ( Building* b );
                  virtual void start ( int x, int y );
              public:
                  GetResource ( GameMap* gamemap, int scope = -1 );
   };

class PutResource : public StaticResourceNet {
              protected:
                  virtual void checkbuilding ( Building* b );
                  virtual void checkvehicle ( Vehicle* v ) {};
                  virtual void start ( int x, int y );
               public:
                   PutResource ( GameMap* gamemap, int scope = -1 ) : StaticResourceNet ( gamemap, scope ) {};
   };

class PutTribute : public StaticResourceNet {
              protected:
                  int targplayer;
                  Building* startbuilding;
                  virtual void checkbuilding ( Building* b );
                  virtual void checkvehicle ( Vehicle* v ) {};
                  virtual void start ( int x, int y );
              public:
                  PutTribute ( GameMap* gamemap ) : StaticResourceNet( gamemap ) {};
                  int puttribute ( Building* start, int resource, int _queryonly, int _forplayer, int _fromplayer, int _scope );
   };


class GetResourceCapacity : public StaticResourceNet {
              protected:
                  virtual void checkbuilding ( Building* b );
                  virtual void checkvehicle ( Vehicle* v ) {};
                  virtual void start ( int x, int y );
                  virtual int searchfinished ( void ) { return 0; };
              public:
                  GetResourceCapacity( GameMap* gamemap ) : StaticResourceNet ( gamemap ) {};
   };

class ResourceChangeNet : public ResourceNet {
               protected:
                  int got;
                  int player;

                  virtual int searchfinished ( void ) { return 0; };

              public:
                  int getresource ( int x, int y, int resource, int _player, int _scope );
                       /* _scope:  0 : only this field
                                   1 : net
                                   2 : global
                       */
                  ResourceChangeNet ( GameMap* gamemap ) : ResourceNet ( gamemap ) {};

       };


class GetResourcePlus : public ResourceChangeNet {
              protected:
                  virtual void checkbuilding ( Building* b );
                  virtual void checkvehicle ( Vehicle* v );
              public:
                  GetResourcePlus ( GameMap* gamemap ) : ResourceChangeNet ( gamemap ) {};
   };

class GetResourceUsage : public ResourceChangeNet {
              protected:
                  virtual void checkbuilding ( Building* b );
                  virtual void checkvehicle ( Vehicle* v ) {};
              public:
                  GetResourceUsage ( GameMap* gamemap ) : ResourceChangeNet ( gamemap ) {};
   };

extern void transfer_all_outstanding_tribute( Player& player );

extern sigc::signal<void> tributeTransferred;

extern bool compareMapResources( GameMap* currentMap, GameMap* replaymap, int player, ASCString* log );


#endif
