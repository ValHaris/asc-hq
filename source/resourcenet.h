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


#ifndef resourcenet_h_included
 #define resourcenet_h_included

class MapNetwork {
                static int instancesrunning;
             protected:
                int pass;

                struct tposition {
                   int x, y ;
                };

                tposition startposition;


                virtual int fieldavail ( int x, int y ) = 0;
                virtual int searchfinished ( void ) = 0;
                virtual void checkvehicle ( pvehicle v ) = 0;
                virtual void checkbuilding ( pbuilding b ) = 0;
                virtual int globalsearch ( void ) = 0;

                virtual void searchbuilding ( int x, int y );
                virtual void searchvehicle  ( int x, int y );
                virtual void searchfield ( int x, int y, int dir );
             public:
                virtual void start ( int x, int y );
                MapNetwork ( int checkInstances = 1 );
                virtual ~MapNetwork();
           };

class ResourceNet : public MapNetwork {
               public:
                  ResourceNet ( int _scope = -1 ) : MapNetwork ( _scope != 0 ) {};
               protected:
                  int resourcetype;
                  int scope;

                  virtual int fieldavail ( int x, int y );
                  virtual int globalsearch ( void ) { return scope; };
             };

class StaticResourceNet : public ResourceNet {
               protected:
                  int need;
                  int got;
                  int queryonly;
                  int player;

                  virtual int searchfinished ( void );

              public:
                  StaticResourceNet ( int scope = -1 ) : ResourceNet ( scope ) {};
                  int getresource ( int x, int y, int resource, int _need, int _queryonly, int _player, int _scope );
                       /* _scope:  0 : only this field
                                   1 : net
                                   2 : global
                       */
       };

class GetResource : public StaticResourceNet {
              protected:
                  int tributegot[3][8];
                  virtual void checkvehicle ( pvehicle v );
                  virtual void checkbuilding ( pbuilding b );
                  virtual void start ( int x, int y );
              public:
                  GetResource ( int scope = -1 );
   };

class PutResource : public StaticResourceNet {
              protected:
                  virtual void checkbuilding ( pbuilding b );
                  virtual void checkvehicle ( pvehicle v ) {};
                  virtual void start ( int x, int y );
               public:
                   PutResource ( int scope = -1 ) : StaticResourceNet ( scope ) {};
   };

class PutTribute : public StaticResourceNet {
              protected:
                  int targplayer;
                  pbuilding startbuilding;
                  virtual void checkbuilding ( pbuilding b );
                  virtual void checkvehicle ( pvehicle v ) {};
                  virtual void start ( int x, int y );
              public:
                  int puttribute ( pbuilding start, int resource, int _queryonly, int _forplayer, int _fromplayer, int _scope );
   };


class GetResourceCapacity : public StaticResourceNet {
              protected:
                  virtual void checkbuilding ( pbuilding b );
                  virtual void checkvehicle ( pvehicle v ) {};
                  virtual void start ( int x, int y );
                  virtual int searchfinished ( void ) { return 0; };
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

       };


class GetResourcePlus : public ResourceChangeNet {
              protected:
                  virtual void checkbuilding ( pbuilding b );
                  virtual void checkvehicle ( pvehicle v );
   };

class GetResourceUsage : public ResourceChangeNet {
              protected:
                  virtual void checkbuilding ( pbuilding b );
                  virtual void checkvehicle ( pvehicle v ) {};
   };

extern void transfer_all_outstanding_tribute( void );

#endif