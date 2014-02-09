/*! \file astar2.cpp
    \brief Pathfinding routines using the A* algorithm.
*/



#include <vector>
#include <cmath>

#include <boost/heap/pairing_heap.hpp>

#include "vehicletype.h"
#include "spfst.h"
#include "controls.h"

#include "astar2.h"

#include <iostream>

void dumpNode(const AStar3D::Node& n) {
   cout << "(" << n.h.x << "," << n.h.y << "," << n.h.getNumericalHeight() << ")@" << n.gval << "\n";
}

class OpenContainer {
      typedef boost::heap::pairing_heap<
         AStar3D::Node,
         boost::heap::compare<greater<AStar3D::Node> >,
         boost::heap::stable<true>,
         boost::heap::constant_time_size<false>
      > storage_t;
      typedef boost::unordered_map<
         MapCoordinate3D,
         storage_t::handle_type,
         AStar3D::hash_MapCoordinate3D
      > index_t;
      typedef storage_t::handle_type handle_type;
      storage_t storage;
      index_t index;
   public:
      const AStar3D::Node& top() const { return storage.top(); }
      void pop() { index.erase(storage.top().h); storage.pop(); }
      int empty() const { return storage.empty(); }
      void pushOrUpdate ( const AStar3D::Node& n ) {
         const index_t::iterator i = index.find(n.h);
         if (i == index.end()) {
            index[n.h] = storage.push(n);
         } else if (((*i->second).gval > n.gval) || ((*i->second).gval == n.gval && (*i->second).hasAttacked && !n.hasAttacked)) {
            storage.increase(i->second, n);
         }
      }
};

static inline int windbeeline ( const MapCoordinate& start, const MapCoordinate& dest, const WindMovement* wm ) {
   int distance = 0;
   int dx = dest.x - start.x;
   int dy = dest.y - start.y;
   int dyOdd = ((start.y & 1) - (dest.y & 1));

   if (dx == 0) {
      if (dy > 0) {
         distance = (minmalq - wm->getDist(2)) * ((dy - (dyOdd != 0)) / 2);
         if (dyOdd == -1) {
            distance += minmalq - wm->getDist(2);
         } else if (dyOdd == 1) {
            distance += minmalq - wm->getDist(4);
         }
      } if (dy < 0) {
         distance = (minmalq - wm->getDist(2)) * -((dy + (dyOdd != 0)) / 2);
         if (dyOdd == -1) {
            distance += minmalq - wm->getDist(1);
         } else if (dyOdd == 1) {
            distance += minmalq - wm->getDist(5);
         }
      } // if dy == 0 then all values stay 0
   } else if (dx > 0) {
      if (dy >= (dx * 2)) {
         distance = (minmalq - wm->getDist(3)) * (((dy + dyOdd) / 2) - dx)
                  + (minmalq - wm->getDist(2)) * (-dyOdd + dx * 2);
      } else if (-dy >= (dx * 2)) {
         distance = (minmalq - wm->getDist(0)) * (-((dy - dyOdd) / 2) - dx)
                  + (minmalq - wm->getDist(1)) * (-dyOdd + dx * 2);
      } else {
         distance = (minmalq - wm->getDist(2)) * (((dy - dyOdd) / 2) + dx)
                  + (minmalq - wm->getDist(1)) * (((-dy - dyOdd) / 2) + dx);
      }
   } else if (dx < 0) {
      if (dy >= -(dx * 2)) {
         distance = (minmalq - wm->getDist(3)) * (((dy - dyOdd) / 2) + dx)
                  + (minmalq - wm->getDist(4)) * (dyOdd - dx * 2);
      } else if (-dy >= -(dx * 2)) {
         distance = (minmalq - wm->getDist(0)) * (-((dy + dyOdd) / 2) + dx)
                  + (minmalq - wm->getDist(5)) * (dyOdd - dx * 2);
      } else {
         distance = (minmalq - wm->getDist(4)) * (((dy + dyOdd) / 2) - dx)
                  + (minmalq - wm->getDist(5)) * (((-dy + dyOdd) / 2) - dx);
      }
   }

   return distance;
}


///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////


bool AStar3D::Node::operator< ( const AStar3D::Node& b ) const
{
    // To compare two nodes, we compare the `f' value, which is the
    // sum of the g and h values.
   return (gval+hval) < (b.gval+b.hval);
}
bool AStar3D::Node::operator> ( const AStar3D::Node& b ) const
{
    // To compare two nodes, we compare the `f' value, which is the
    // sum of the g and h values.
   return (gval+hval) > (b.gval+b.hval);
}

bool operator == ( const AStar3D::Node& a, const AStar3D::Node& b )
{
    // Two nodes are equal if their components are equal
    return (a.h == b.h) && (a.gval == b.gval ) && (a.hval == b.hval );
}

AStar3D :: AStar3D ( GameMap* actmap_, Vehicle* veh_, bool markTemps_, int maxDistance )
         : operationLimiter ( NULL )
{
   markTemps = markTemps_;
   tempsMarked = NULL;
   _path = NULL;
   veh = veh_;
   actmap = actmap_;

   MAXIMUM_PATH_LENGTH = maxDistance;
   if ( MAXIMUM_PATH_LENGTH > longestPath )
      MAXIMUM_PATH_LENGTH =  int(2* longestPath);

/*
   if ( !veh->maxMovement() ) {
      // fatalError ( "AStar3D :: AStar3D  -  trying to move a immobile unit");
      int foo = veh->maxMovement()*2;
   }
*/

   if ( veh->getPosition().getNumericalHeight() >= 0 )
      for ( int i = 0; i < 8; i++ )
          if ( veh->maxMovement() )
             vehicleSpeedFactor[i] = float(veh->typ->movement[i]) / veh->maxMovement();
          else
             vehicleSpeedFactor[i] = 0.00001;

   else
      for ( int i = 0; i < 8; i++ )
          vehicleSpeedFactor[i] = float(veh->typ->movement[i]);

   maxVehicleSpeedFactor = 0;
   for ( int i = 0; i < 8; ++i )
      maxVehicleSpeedFactor = max( maxVehicleSpeedFactor, vehicleSpeedFactor[i] );
   
   if ( (veh->typ->height & ( chtieffliegend | chfliegend | chhochfliegend )) && actmap->weather.windSpeed )
      wind = new WindMovement ( veh );
   else
      wind = NULL;

}

const AStar3D::DistanceType AStar3D::longestPath = 1e9;


AStar3D :: ~AStar3D ( )
{
   if( tempsMarked )
      tempsMarked->cleartemps ( 3 );

   if ( wind ) {
      delete wind;
      wind = NULL;
   }
}

AStar3D::DistanceType AStar3D::dist( const MapCoordinate3D& a, const MapCoordinate3D& b )
{
   int heightDiff;
   if ( b.getNumericalHeight() >= 0 && a.getNumericalHeight() >= 0 )
      heightDiff = abs ( b.getNumericalHeight() - a.getNumericalHeight() ) * minmalq/2;
   else
      heightDiff = 0;

   DistanceType dist;
   if ( b.valid() ) {
      if  ( wind && ((b.getBitmappedHeight() | a.getBitmappedHeight()) & ( chtieffliegend | chfliegend | chhochfliegend ))) {
         dist = windbeeline(a, b, wind ) + heightDiff;
      } else {
         dist = beeline ( a, b ) + heightDiff;
      }
   } else {
      dist = 0;
   }
   return dist;
}

AStar3D::DistanceType AStar3D::dist ( const MapCoordinate3D& a, const vector<MapCoordinate3D>& b )
{
   if (b.empty())
      return 0;

   DistanceType e2 = longestPath;
   for ( vector<MapCoordinate3D>::const_iterator i = b.begin(); i != b.end(); ++i ) {
       DistanceType e = dist(a,*i);
       if ( maxVehicleSpeedFactor )
          e /= maxVehicleSpeedFactor;
       e2 = min( e2,e );
    }
   return min(e2, longestPath);
}


AStar3D::DistanceType AStar3D::getMoveCost ( const MapCoordinate3D& start, const MapCoordinate3D& dest, const Vehicle* vehicle, bool& hasAttacked )
{
    // since we are operating at different levels of height and the unit has different
    // speeds at different levels of height, we must not optimize for distance, but for
    // travel time.
    int movecost = calcMoveMalus ( start, dest, vehicle, wind, &hasAttacked ).first;

    if ( start.getNumericalHeight() >= 0 )
       if ( movecost > veh->typ->movement[start.getNumericalHeight()]  )
          return longestPath;


    if ( start.getNumericalHeight() < 0 )
       return movecost;
    else
       if ( !vehicleSpeedFactor[start.getNumericalHeight()] )
          return longestPath;
       else
          return movecost / vehicleSpeedFactor[start.getNumericalHeight()];
}

int AStar3D::initNode ( Node& newN,
                const Node* oldN_ptr,
                const MapCoordinate3D& newpos,
                const vector<MapCoordinate3D>& B,
                bool disableAttack,
                bool enter,
                bool dock) {
   int fa;

   newN.h = newpos;

   // No need to reassess already visited Node
   if ( (oldN_ptr->previous) && (newN.h == oldN_ptr->previous->h) )
      return 0;

   // If it's off the end of the map, then don't keep scanning
   if( newN.h.x < 0 || newN.h.y < 0 || newN.h.x >= actmap->xsize || newN.h.y >= actmap->ysize )
      return 0;

   if (enter) {
      newN.enterHeight = newN.h.getNumericalHeight();
      newN.h.setNumericalHeight(-1);
   } else {
      newN.enterHeight = -1;
   }

   if ( visited.find(newN.h) )
      return 0;

   newN.hasAttacked = oldN_ptr->hasAttacked || disableAttack;

   if ( !dock ) {
      fa = fieldAccessible ( actmap->getField ( newN.h ), veh, newN.h.getBitmappedHeight(), &newN.hasAttacked );
      if ( !fa )
         return 0;
      newN.canStop = fa >= 2;

      newN.gval = oldN_ptr->gval + getMoveCost ( oldN_ptr->h, newN.h, veh, newN.hasAttacked );
   } else {
      fa = 1;
      newN.canStop = true;
      newN.gval = oldN_ptr->gval + 10;
   }
   if ( (newN.gval >= longestPath) || ( newN.gval > MAXIMUM_PATH_LENGTH ) )
      return 0;

   newN.previous = oldN_ptr;
   newN.hval = dist(newN.h, B);
   return fa;
}

const int* getDirectionOrder ( int x, int y, int x2, int y2 )
{
    typedef const int Directions[sidenum];
    static const Directions  directions[3][3]  = {{{5, 0, 4, 1, 3, 2 }, {0, 1, 5, 2, 4, 3 }, {1, 0, 2, 5, 3, 4 }},
                                                   {{5, 4, 0, 3, 1, 2 }, {0, 1, 2, 3, 4, 5 }, {1, 2, 0, 3, 5, 4 }},
                                                   {{4, 3, 5, 2, 0, 1 }, {3, 4, 2, 5, 1, 0 }, {2, 3, 1, 4, 0, 5 }}};
    int a,b;

    int dx = (2 * x2 + (y2 & 1)) - (2 * x + (y & 1));
    int dy = y2 - y;

    if (dx < 0)
       a = 0;
    else
       if (dx == 0)
          a = 1;
       else
          a = 2;

    if (dy < 0)
       b = 0;
    else
       if (dy == 0)
          b = 1;
       else
          b = 2;

    return (const int*)(&directions[b][a]);
}

bool AStar3D::findPath( Path& path, const vector<MapCoordinate3D>& B ) {
   _path = &path;

   if (findPath (B)) {
      constructPath ( path, &visited.back() );
      return true;
   } else {
      return false;
   }
         
}

bool AStar3D::findPath( const MapCoordinate3D& B ) {
   vector<MapCoordinate3D> vB;
   vB.push_back(B);
   return findPath(vB);
}

bool AStar3D::findPath( const vector<MapCoordinate3D>& B ) {

    MapCoordinate3D A = veh->getPosition3D();

    OpenContainer open;

    /* this should be checked outside by giving the pathfinder the right movement
    if ( !veh->canMove() )
       return;
    */

    if ( !visited.find(A) ) {
       Node firstNode = Node();
       firstNode.previous = NULL;
       firstNode.gval = 0;
       firstNode.hval = dist(A, B);
       firstNode.enterHeight = -1;
       firstNode.canStop = true;
       firstNode.hasAttacked = veh->attacked;

       firstNode.h = A;
       if ( !(actmap->getField(A)->unitHere(veh)) ) {
          firstNode.h.setNumericalHeight(-1);
       }
       // insert the original node
       open.pushOrUpdate(firstNode);
    }
    bool found = false;
    const Node* N_ptr;

    // While there are still nodes to visit, visit them!
    while( !open.empty() ) {
        //N_ptr = visited.add(*open.begin());
        N_ptr = visited.add(open.top());
        open.pop();
        // If we're at the goal, then exit
        MapField* oldFld = actmap->getField(N_ptr->h);
        for ( vector<MapCoordinate3D>::const_iterator i = B.begin(); i != B.end(); i++ )
           if( N_ptr->h == *i ) {
              if ( N_ptr->h.getNumericalHeight() == -1 || !(oldFld->building || (oldFld->vehicle && oldFld->vehicle != veh ))) {
                 found = true;
                 break;
              }
           }
        if ( found )
           break;

        if ( N_ptr->h.getNumericalHeight() == -1 ) {
           // the unit is inside a container

          if ( !operationLimiter || operationLimiter->allowLeavingContainer() ) {
             for ( int dir = 0; dir < 6; dir++ ) {
                MapCoordinate3D pos = getNeighbouringFieldCoordinate ( N_ptr->h, dir );
                if ( actmap->getField(pos)) {
                   int h = oldFld->getContainer()->vehicleUnloadable(veh->typ);
                   for ( int i = 0; i < 8; i++ )
                      if ( h & (1<<i)) {
                         const ContainerBaseType::TransportationIO* tio = oldFld->getContainer()->vehicleUnloadSystem( veh->typ, 1<<i);
                         if ( tio ) {
                            pos.setNumericalHeight(i);
                            Node N2 = Node();
                            if (!initNode(N2, N_ptr, pos, B, tio->disableAttack))
                               continue;

                            if ( N2.canStop && actmap->getField(N2.h)->getContainer() && actmap->getField(N2.h)->vehicle != veh) {
                                 // there's an container on the field that can be entered. This means, the unit can't stop 'over' the container...
                                 N2.canStop = false;
                                 open.pushOrUpdate ( N2 );

                                 // ... only inside it
                                 N2.canStop = true;
                                 N2.enterHeight = N2.h.getNumericalHeight() ;
                                 N2.h.setNumericalHeight(-1);
                                 // N2.hasAttacked = true;
                                 if (!visited.find(N2.h))
                                    open.pushOrUpdate ( N2 );
                            } else
                                 open.pushOrUpdate ( N2 );
                         }

                      }
                }
             }
          }

          if ( !operationLimiter || operationLimiter->allowDocking() ) {
             int dock = oldFld->getContainer()->vehicleDocking(veh, true );
             for ( int dir = 0; dir < 6; dir++ ) {
                if ( dock ) {
                   MapCoordinate3D pos = getNeighbouringFieldCoordinate ( N_ptr->h, dir );
                   MapField* fld = actmap->getField( pos );
                   if ( fld && fld->getContainer() && ( fld->getContainer() != oldFld->getContainer() ))
                      if ( fld->getContainer()->vehicleDocking(veh, false ) & dock )
                         if ( fld->getContainer()->getOwner() == oldFld->getContainer()->getOwner() 
                              && veh->getMap()->getPlayer(veh).diplomacy.isAllied( fld->getContainer() ))
                            if ( !fld->building || (fld->bdt & getTerrainBitType(cbbuildingentry) ).any()) {
                               pos.setNumericalHeight(-1);
                               Node N2 = Node();
                               if ( !initNode(N2, N_ptr, pos, B, false, false, true) )
                                  continue;
                               open.pushOrUpdate ( N2 );
                            }
                }
             }
          }


        } else {
           // the unit is not inside a container

           const int* directions;
           if ( !B.empty() && B.begin()->valid() ) {
              directions = getDirectionOrder ( N_ptr->h.x, N_ptr->h.y, B.begin()->x, B.begin()->y );
           } else {
              static const int d[6] = {0,1,2,3,4,5};
              directions = d;
           }

           if ( !operationLimiter || operationLimiter->allowMovement() ) {

              // this check is actually only necessary for the very first field in which the unit starts.
              // The unit may not be able to leave it, for example if a ship is frozen in ice
              if ( terrainaccessible ( oldFld, veh) || veh->getMap()->getgameparameter(cgp_movefrominvalidfields)) {

                 for( int dci = 0; dci < 6; dci++ ) {
                    HexDirection dir = HexDirection(directions[dci]);
                    Node N2 = Node();
                    if ( !initNode(N2, N_ptr, getNeighbouringFieldCoordinate(N_ptr->h, dir), B) )
                       continue;
                    if ( N2.canStop && actmap->getField(N2.h)->getContainer() && actmap->getField(N2.h)->vehicle != veh) {
                        // there's an container on the field that can be entered. This means, the unit can't stop 'over' the container...
                        if ( !veh->typ->hasFunction( ContainerBaseType::OnlyMoveToAndFromTransports  ) ) {
                           N2.canStop = false;
                           open.pushOrUpdate ( N2 );
                        }

                        // ... only inside it
                        N2.canStop = true;
                        N2.enterHeight = N2.h.getNumericalHeight() ;
                        N2.h.setNumericalHeight (-1);
                        // N2.hasAttacked = true;
                        if (!visited.find(N2.h))
                           open.pushOrUpdate ( N2 );
                     } else
                        if ( !veh->typ->hasFunction( ContainerBaseType::OnlyMoveToAndFromTransports  ) )
                           open.pushOrUpdate ( N2 );
                  }
              }
           }
           // and now change the units' height. That's only possible on fields where the unit can stop it's movement


           if ( (!operationLimiter || operationLimiter->allowHeightChange()) && !(veh->typ->hasFunction( ContainerBaseType::OnlyMoveToAndFromTransports  )) )
              if ( (fieldAccessible ( oldFld, veh, N_ptr->h.getBitmappedHeight() ) == 2 ) || actmap->getgameparameter( cgp_movefrominvalidfields) )
                 for ( int heightDelta = -1; heightDelta <= 1; heightDelta += 2 ) {
                    const VehicleType::HeightChangeMethod* hcm = veh->getHeightChange( heightDelta, N_ptr->h.getBitmappedHeight());
                    if ( hcm ) {
                       for ( int dir = 0; (dir < 6 && hcm->dist) || (dir < 1 && !hcm->dist); dir++ ) {
                          MapCoordinate3D newpos = N_ptr->h;
                          bool access = true;
                          for ( int step = 0; step <= hcm->dist; step++ ) {
                             MapField* fld = actmap->getField(newpos);
                             if ( !fld ) {
                                access = false;
                                break;
                             }

                             if (  !fieldAccessible ( fld, veh, N_ptr->h.getBitmappedHeight()) && (actmap->getgameparameter( cgp_movefrominvalidfields)==0 || step>0))
                                access = false;

                             if ( !fieldAccessible( fld, veh, 1 << (N_ptr->h.getNumericalHeight() + hcm->heightDelta)) )
                                access = false;

                             if ( fld && fld->building )
                                access = false;

                             if ( step < hcm->dist ) {
                                newpos.x += getnextdx( dir, newpos.y );
                                newpos.y += getnextdy( dir );
                             } else {
                                if ( fld && (fld->building || (fld->vehicle && fld->vehicle != veh)))
                                   access = false;
                             }
                          }

                          MapField* fld = actmap->getField( newpos );
                          if ( fld && access ) {
                             newpos.setNumericalHeight(newpos.getNumericalHeight() + hcm->heightDelta);
                             bool enter = actmap->getField(newpos)->getContainer() && actmap->getField(newpos)->vehicle != veh;
                             Node N2 = Node();
                             if ( initNode(N2, N_ptr, newpos, B, false, enter ) != 2)
                                continue;
                            open.pushOrUpdate ( N2 );
                          }
                       }
                    }
                 }


        }
    }

    return found;
}

bool AStar3D::constructPath( Path& path, const Node* n_ptr) {
   if ( n_ptr == NULL )
      return false;
   for ( const Node* n = n_ptr; n != NULL; n = n->previous )
      path.push_front ( PathPoint(n->h, int(n->gval), n->enterHeight, n->hasAttacked) );
   return true;
}

void AStar3D::findPath( Path& path, const MapCoordinate3D& dest )
{
  vector<MapCoordinate3D> d;
  d.push_back ( dest );
  findPath ( path, d );
}


void AStar3D::findAllAccessibleFields ()
{
   if ( markTemps )
      actmap->cleartemps ( 3 );

   vector<MapCoordinate3D> v;
   findPath ( v );
   for ( VisitedContainer::iterator i = visited.begin(); i != visited.end(); ++i ) {
      fieldAccess[i->h] |= i->h.getBitmappedHeight();

      if ( markTemps ) {
         char atemp = actmap->getField ( i->h )->getaTemp();
         actmap->getField ( i->h )->setaTemp( atemp | i->h.getBitmappedHeight());
      }
   }
   
   if ( markTemps )
      tempsMarked = actmap;
}

int AStar3D::getFieldAccess ( int x, int y )
{
   return getFieldAccess( MapCoordinate(x, y) );
}

int AStar3D::getFieldAccess ( const MapCoordinate& mc )
{
   fieldAccessType::iterator i = fieldAccess.find(mc);
   return (i == fieldAccess.end()) ? 0 : i->second;
}
             
void AStar3D::PathPoint::write( tnstream& stream ) const
{
   stream.writeInt(1);
   MapCoordinate::write( stream );
   stream.writeInt( dist );
   stream.writeInt( enterHeight );
   stream.writeInt( hasAttacked );
}

void AStar3D::PathPoint::read( tnstream& stream )
{
   stream.readInt(); // version  
   MapCoordinate::read ( stream );
   dist = stream.readInt();
   enterHeight = stream.readInt(),
   hasAttacked = stream.readInt();
}


AStar3D::PathPoint AStar3D::PathPoint::newFromStream( tnstream& stream )
{
   PathPoint pp;
   pp.read(stream);
   return pp;  
}

void AStar3D::dumpVisited()
{
   for ( VisitedContainer::iterator i = visited.begin(); i != visited.end(); ++i ) {
      dumpNode(*i);
   }
}

