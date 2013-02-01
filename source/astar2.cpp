/*! \file astar2.cpp
    \brief Pathfinding routines using the A* algorithm.
*/



#include <vector>
#include <cmath>

#include "vehicletype.h"
#include "spfst.h"
#include "controls.h"

#include "astar2.h"

#include <iostream>

inline int windbeeline ( const MapCoordinate& start, const MapCoordinate& dest, const WindMovement* wm ) {
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
    if ( hval >= AStar3D::longestPath || b.hval >= AStar3D::longestPath )
       return gval < b.gval;
    else
       return (gval+hval) < (b.gval+b.hval);
}
/*
bool operator< ( const AStar3D::Node& a, const AStar3D::Node& b )
{
    // To compare two nodes, we compare the `f' value, which is the
    // sum of the g and h values.
    if ( a.hval >= AStar3D::longestPath || b.hval >= AStar3D::longestPath )
       return a.gval < b.gval;
    else
       return (a.gval+a.hval) < (b.gval+b.hval);
}
*/
bool operator> ( const AStar3D::Node& a, const AStar3D::Node& b )
{
    if ( a.hval >= AStar3D::longestPath || b.hval >= AStar3D::longestPath )
       return a.gval > b.gval;
    else
       return (a.gval+a.hval) > (b.gval+b.hval);
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


AStar3D :: ~AStar3D ( )
{
   if( tempsMarked )
      tempsMarked->cleartemps ( 3 );

   if ( wind ) {
      delete wind;
      wind = NULL;
   }

   //delete[] fieldAccess;
}

AStar3D::DistanceType AStar3D::dist( const MapCoordinate3D& a, const MapCoordinate3D& b )
{
   int heightDiff;
   if ( b.getNumericalHeight() >= 0 && a.getNumericalHeight() >= 0 )
      heightDiff = abs ( b.getNumericalHeight() - a.getNumericalHeight() ) * minmalq;
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
      dist = longestPath;
   }
   return dist;
}

AStar3D::DistanceType AStar3D::dist ( const MapCoordinate3D& a, const vector<MapCoordinate3D>& b )
{
   if (b.empty())
      return longestPath;
   DistanceType e;
   for ( vector<MapCoordinate3D>::const_iterator i = b.begin(); i != b.end(); ++i ) {
      e = dist(a,*i);
      if ( maxVehicleSpeedFactor )
         e /= maxVehicleSpeedFactor;
   }
   return min(e, longestPath);
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


void AStar3D :: addToOpen ( const Node& N2, OpenContainer& open )
{
   OpenContainerIndex::iterator i = open.get<1>().find(N2.h);
   if ( i == open.get<1>().end()) {
      open.insert ( N2 );
   } else {
      if (i->gval > N2.gval || (i->gval == N2.gval && i->hasAttacked && !N2.hasAttacked))
         open.get<1>().replace(i, N2);
   }
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

   if ( visited.find(newN.h) )
      return 0;

   // If it's off the end of the map, then don't keep scanning
   if( newN.h.x < 0 || newN.h.y < 0 || newN.h.x >= actmap->xsize || newN.h.y >= actmap->ysize )
      return 0;

   newN.hasAttacked = oldN_ptr->hasAttacked || disableAttack;

   if ( !dock ) {
      fa = fieldAccessible ( actmap->getField ( newN.h ), veh, newN.h.getBitmappedHeight(), &newN.hasAttacked );
      if ( !fa )
         return 0;
      newN.canStop = fa >= 2;

      newN.gval = oldN_ptr->gval + getMoveCost ( oldN_ptr->h, newN.h, veh, newN.hasAttacked );
   } else {
      newN.gval = oldN_ptr->gval + 10;
      newN.canStop = true;
      fa = 1;
   }
   if ( (newN.gval >= longestPath) || ( newN.gval > MAXIMUM_PATH_LENGTH ) )
      return 0;

   if (enter) {
      newN.enterHeight = newN.h.getNumericalHeight();
      newN.h.setNumericalHeight(-1);
   } else {
      newN.enterHeight = -1;
   }
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

void AStar3D::findPath( const MapCoordinate3D& A, const vector<MapCoordinate3D>& B, Path& path )
{
    _path = &path;

    OpenContainer open;

    /* this should be checked outside by giving the pathfinder the right movement
    if ( !veh->canMove() )
       return;
    */

    Node firstNode;
    firstNode.previous = NULL;
    firstNode.gval = 0;
    firstNode.hval = dist(A, B);

    firstNode.h = A;
    if ( !(actmap->getField(A)->unitHere(veh)) ) {
       firstNode.h.setNumericalHeight(-1);
    }
    // insert the original node
    open.insert(firstNode);

    bool found = false;
    const Node* N_ptr;

    // While there are still nodes to visit, visit them!
    while( !open.empty() ) {
        N_ptr = visited.add(*open.begin());
        open.erase(open.begin());
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
                            Node N2;
                            if (!initNode(N2, N_ptr, pos, B, tio->disableAttack))
                               continue;

                            if ( N2.canStop && actmap->getField(N2.h)->getContainer() && actmap->getField(N2.h)->vehicle != veh) {
                                 // there's an container on the field that can be entered. This means, the unit can't stop 'over' the container...
                                 N2.canStop = false;
                                 addToOpen ( N2, open );

                                 // ... only inside it
                                 N2.canStop = true;
                                 N2.enterHeight = N2.h.getNumericalHeight() ;
                                 N2.h.setNumericalHeight(-1);
                                 // N2.hasAttacked = true;
                                 addToOpen ( N2, open );
                            } else
                                 addToOpen ( N2, open );
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
                               Node N2;
                               if ( !initNode(N2, N_ptr, pos, B, false, false, true) )
                                  continue;
                               addToOpen ( N2, open );
                            }
                }
             }
          }


        } else {
           // the unit is not inside a container

           const int* directions;
           if ( B.begin()->valid() ) {
              directions = getDirectionOrder ( N_ptr->h.x, N_ptr->h.y, B.begin()->x, B.begin()->y );
           } else {
              static const int d[6] = {0,1,2,3,4,5};
              directions = d;
           }

           if ( !operationLimiter || operationLimiter->allowMovement() )
              for( int dci = 0; dci < 6; dci++ ) {

                 HexDirection dir = HexDirection(directions[dci]);
                 Node N2;
                 if ( !initNode(N2, N_ptr, getNeighbouringFieldCoordinate(N_ptr->h, dir), B) )
                    continue;
                 if ( N2.canStop && actmap->getField(N2.h)->getContainer() && actmap->getField(N2.h)->vehicle != veh) {
                     // there's an container on the field that can be entered. This means, the unit can't stop 'over' the container...
                     if ( !veh->typ->hasFunction( ContainerBaseType::OnlyMoveToAndFromTransports  ) ) {
                        N2.canStop = false;
                        addToOpen ( N2, open );
                     }

                     // ... only inside it
                     N2.canStop = true;
                     N2.enterHeight = N2.h.getNumericalHeight() ;
                     N2.h.setNumericalHeight (-1);
                     // N2.hasAttacked = true;
                     addToOpen ( N2, open );
                  } else
                     if ( !veh->typ->hasFunction( ContainerBaseType::OnlyMoveToAndFromTransports  ) )
                        addToOpen ( N2, open );
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
                             Node N2;
                             if ( initNode(N2, N_ptr, newpos, B, false, enter ) != 2)
                                continue;
                            addToOpen ( N2, open );
                          }
                       }
                    }
                 }


        }
    }

    if ( found ) {
       constructPath ( path, N_ptr );
    } else {
        // No path
    }
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
  findPath ( veh->getPosition3D() , d, path );
}

void AStar3D::findPath( Path& path, const vector<MapCoordinate3D>& dest )
{
  findPath ( veh->getPosition3D(), dest, path );
}


void AStar3D::findAllAccessibleFields ( vector<MapCoordinate3D>* path )
{
   if ( markTemps )
      actmap->cleartemps ( 3 );

   Path dummy;
   findPath ( dummy, MapCoordinate3D(actmap->xsize, actmap->ysize, veh->height) );  //this field does not exist...
   for ( VisitedContainer::iterator i = visited.begin(); i != visited.end(); ++i ) {
      AStar3D::Node node = *i;
      fieldAccess[node.h] |= node.h.getBitmappedHeight();

      if ( markTemps ) {
         char atemp = actmap->getField ( node.h )->getaTemp();
         actmap->getField ( node.h )->setaTemp( atemp | node.h.getBitmappedHeight());
      }
      
      if ( path )
         path->push_back( node.h );
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

