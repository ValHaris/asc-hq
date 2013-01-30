/*! \file astar2.cpp
    \brief Pathfinding routines using the A* algorithm.
*/

#include <stack>
#include <vector>
#include <algorithm>
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


// The mark array marks directions on the map.  The direction points
// to the spot that is the previous spot along the path.  By starting
// at the end, we can trace our way back to the start, and have a path.

// mark -> temp3

bool AStar::Node::operator< ( const AStar::Node& a ) const
{
    // To compare two nodes, we compare the `f' value, which is the
    // sum of the g and h values.
    return (gval+hval) < (a.gval+a.hval);
}

bool AStar::Node::operator> ( const AStar::Node& a ) const
{
    return (gval+hval) > (a.gval+a.hval);
}


bool operator == ( const AStar::HexCoord& a, const AStar::HexCoord& b )
{
   return (a.m == b.m) && (a.n == b.n );
}


bool operator == ( const AStar::Node& a, const AStar::Node& b )
{
    // Two nodes are equal if their components are equal
    return (a.h == b.h) && (a.gval == b.gval ) && (a.hval == b.hval );
}



HexDirection ReverseDirection( HexDirection d )
{
    // With hexagons, I'm numbering the directions 0 = N, 1 = NE,
    // and so on (clockwise).  To flip the direction, I can just
    // add 3, mod 6.
    return HexDirection( ( 3+int(d) ) % 6 );
}


AStar :: AStar ( GameMap* actmap, Vehicle* veh )
{
   tempsMarked = NULL;
   _path = NULL;
   _veh = veh;
   gamemap = actmap;
   MAXIMUM_PATH_LENGTH = maxint;
}

AStar :: ~AStar ( )
{
   if ( tempsMarked )
      tempsMarked->cleartemps( 1 );
}

int AStar::dist( HexCoord a, HexCoord b )
{
   /*
    // The **Manhattan** distance is what should be used in A*'s heuristic
    // distance estimate, *not* the straight-line distance.  This is because
    // A* wants to know the estimated distance for its paths, which involve
    // steps along the grid.  (Of course, if you assign 1.4 to the cost of
    // a diagonal, then you should use a distance function close to the
    // real distance.)

    // Here I compute a ``Manhattan'' distance for hexagons.  Nifty, eh?
    int a1 = a.m*2;
    int a2 = a.n*2-(a.m%2)-2*(a.m/2);
    int a3 = -a1-a2;
    int b1 = b.m*2;
    int b2 = b.n*2-(b.m%2)-2*(b.m/2);
    int b3 = -b1-b2;


    // 2*D/path_div lets me scale the value.  Scaling is nice because you
    // can adjust the accuracy/speed tradeoff.  If you want a faster
    // search, you can get an approximate answer.
    return 2*(abs(a1-b1)+abs(a2-b2)+abs(a3-b3))/path_div;
    */
    return beeline ( a.m, a.n, b.m, b.n );
}

int AStar::getMoveCost ( int x1, int y1, int x2, int y2, const Vehicle* vehicle )
{
    if ( !fieldAccessible ( gamemap->getField ( x2, y2 ), vehicle ))
       return MAXIMUM_PATH_LENGTH;

    return calcMoveMalus ( MapCoordinate3D(x1, y1, vehicle->height), MapCoordinate3D(x2, y2, vehicle->height), vehicle ).first;
}



// greater(Node) is an STL thing to create a 'comparison' object out of
// the greater-than operator, and call it comp.


// I'm using a priority queue implemented as a heap.  STL has some nice
// heap manipulation functions.  (Look at the source to `priority_queue'
// for details.)  I didn't use priority_queue because later on, I need
// to traverse the entire data structure to update certain elements; the
// abstraction layer on priority_queue wouldn't let me do that.

inline void AStar::get_first( Container& v, Node& n )
{
    n = v.front();
    pop_heap( v.begin(), v.end(), comp );
    v.pop_back();
}



// Here's the algorithm.  I take a map, two points (A and B), and then
// output the path in the `path' vector.



void AStar::findPath( HexCoord A, HexCoord B, Path& path )
{
    _path = &path;
    Vehicle* veh = _veh;

    if ( gamemap->getField(A.m, A.n)->unitHere(veh) )
       if ( !veh->canMove() )
          return;

    for ( int y = gamemap->xsize * gamemap->ysize -1; y >= 0; y-- )
       gamemap->field[y].setTemp3(DirNone);

    Node N;
    Container open;
    {
        // insert the original node
        N.h = A;
        N.gval = 0;
        N.hval = dist(A,B);
        open.push_back(N);
    }


    // Remember which nodes we visited, so that we can clear the mark array
    // at the end.

    // While there are still nodes to visit, visit them!
    while( !open.empty() )
    {
        get_first( open, N );
        visited.push_back( N );
        // If we're at the goal, then exit
        if( N.h == B )
            break;

        // Every other column gets a different order of searching dirs
        // (Alternatively, you could pick one at random).  I don't want
        // to be too biased by my choice of order in which I look at the
        // neighboring grid spots.

        int directions1[6] = {0,1,2,3,4,5};
        int directions2[6] = {5,4,3,2,1,0};
        int *directions;
        if( N.h.m % 2 == 0 )
            directions = directions1;
        else
            directions = directions2;

        // Look at your neighbors.
        for( int dci = 0; dci < 6; dci++ )
        {
#if 0
            {
                // Random permutation of directions
                int i = random(6-dci);
                swap( directions[dci+i], directions[dci] );
            }
#endif

            HexDirection d = HexDirection(directions[dci]);
            HexCoord hn = N.h;
            hn.m += getnextdx ( d, hn.n );
            hn.n += getnextdy ( d );
            // If it's off the end of the map, then don't keep scanning
            if( hn.m < 0 || hn.n < 0 || hn.m >= gamemap->xsize || hn.n >= gamemap->ysize || !fieldAccessible ( gamemap->getField ( hn.m, hn.n ), veh ))
                continue;

            // cursor.gotoxy ( hn.m, hn.n );
            int k = getMoveCost( N.h.m, N.h.n, hn.m, hn.n, veh );
            if ( k > veh->maxMovement() )
               k = MAXIMUM_PATH_LENGTH;

            Node N2;
            N2.h = hn;

            if ( k == MAXIMUM_PATH_LENGTH )
               N2.gval = k;
            else
               N2.gval = N.gval + k;

            N2.hval = dist( hn, B );
            // If this spot (hn) hasn't been visited, its mark is DirNone
            if( gamemap->getField (hn.m,hn.n)->getTemp3() == DirNone ) {

                // The space is not marked

                if ( N.gval < MAXIMUM_PATH_LENGTH ) {
                   gamemap->getField (hn.m,hn.n)->setTemp3(ReverseDirection(d));
                   open.push_back( N2 );
                   push_heap( open.begin(), open.end(), comp );
                }
            }
            else
            {
                // Search for hn in open
                Container::iterator find1 = open.end();
                for( Container::iterator i = open.begin(); i != open.end(); i++ )
                    if( (*i).h == hn )
                    {
                        find1 = i;
                        break;
                    }
                // if found, call it N3
                if( find1 != open.end() )
                {
                    Node N3 = *find1;
                    if( N3.gval > N2.gval )
                    {
                        gamemap->getField (hn.m,hn.n)->setTemp3(ReverseDirection(d));
                        // Replace N3 with N2 in the open list
                        Container::iterator last = open.end() - 1;
                        *find1 = *last;
                        *last = N2;
                        push_heap( open.begin(), open.end(), comp );
                    }
                }
            }
        }
    }

    if( N.h == B && N.gval < MAXIMUM_PATH_LENGTH )
    {
        // We have found a path, so let's copy it into `path'
        std::vector<int> tempPath;

        HexCoord h = B;
        while( !(h == A) )
        {
            HexDirection dir = HexDirection ( gamemap->getField (h.m, h.n)->getTemp3() );
            tempPath.push_back( int( ReverseDirection( dir ) ) );
            h.m += getnextdx ( dir, h.n );
            h.n += getnextdy ( dir );
        }

        // tempPath now contains the directions the unit must travel .. backwards
        // (like a stack)

        int x = A.m;
        int y = A.n;

        for ( int i = tempPath.size()-1; i >= 0 ; i-- ) {
           int direc = tempPath[i];
           x += getnextdx ( direc, y );
           y += getnextdy ( direc );
           path.push_back ( MapCoordinate ( x, y ));
        }
    }
    else
    {
        // No path
    }

    gamemap->cleartemps ( 4 ); 
}

int AStar::getDistance ( )
{
   if ( !_path || !_veh || ! gamemap )
      return -1;

   if ( _path->size() <= 0 )
      return -1;

   int dist = 0;
   for ( Path::iterator i = _path->begin(); i != _path->end(); i++ )
      dist += gamemap->getField ( *i )->getmovemalus ( _veh->typ->movemalustyp );

   return dist;
}

int AStar::getTravelTime( )
{
   int dist = getDistance();
   if ( dist < 0 )
      return dist;
   else
      return dist / _veh->typ->movement[ getFirstBit ( _veh->height ) ];
}

bool AStar::fieldVisited ( int x, int y)
{
   HexCoord hn ( x,y );
   for( Container::iterator i = visited.begin(); i != visited.end(); i++ )
       if( (*i).h == hn )
          return true;

   return false;
}

void AStar::findAllAccessibleFields ( int maxDist )
{
   gamemap->cleartemps ( 1 );

   MAXIMUM_PATH_LENGTH = maxDist;

   Path dummy;
   findPath ( dummy, gamemap->xsize, gamemap->ysize );  //this field does not exist...
   for ( Container::iterator i = visited.begin(); i != visited.end(); i++ )
      gamemap->getField ( (*i).h.m, (*i).h.n )->setaTemp(1);

   tempsMarked = gamemap;
}


void AStar::findPath( Path& path, int x, int y )
{
  findPath ( AStar::HexCoord ( _veh->xpos, _veh->ypos ), AStar::HexCoord ( x, y ), path );
}




void findPath( GameMap* actmap, AStar::Path& path, Vehicle* veh, int x, int y )
{
  AStar as ( actmap, veh );
  as.findPath ( AStar::HexCoord ( veh->xpos, veh->ypos ), AStar::HexCoord ( x, y ), path );
//   int og = godview;
//   godview = 1;
//   AStar ( actmap, HexCoord ( veh->xpos, veh->ypos ), HexCoord ( x, y ), path, veh );
//   godview = og;
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
   
   int cnt = actmap->xsize*actmap->ysize*9;
   fieldAccess = new int[cnt](); // initializes with 0

   if ( (veh->typ->height & ( chtieffliegend | chfliegend | chhochfliegend )) && actmap->weather.windSpeed ) {
      wind = new WindMovement ( veh );
   } else
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

   delete[] fieldAccess;
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
   if ( N2.gval <= MAXIMUM_PATH_LENGTH && N2.gval < longestPath ) {
      OpenContainerIndex::iterator i = open.get<1>().find(N2.h);
      if ( i == open.get<1>().end()) {
         if ( !visited.contains(N2.h))
            open.insert ( N2 );
      } else {
         if (i->gval > N2.gval || (i->gval == N2.gval && i->hasAttacked && !N2.hasAttacked))
            open.get<1>().replace(i, N2);
      }
   }
}

bool AStar3D::initNode ( Node& newN,
                Node* oldN_ptr,
                int dir,
                const vector<MapCoordinate3D>& B,
                int newNumericalHeight,
                bool disableAttack,
                bool keepEnterHeight) {

   if (dir != -1)
      newN.h = getNeighbouringFieldCoordinate (oldN_ptr->h, dir);
   else
      newN.h = oldN_ptr->h;

   if (newNumericalHeight != -2)
      newN.h.setNumericalHeight(newNumericalHeight);

   // No need to reassess already visited Node
   if ( (oldN_ptr->previous) && (newN.h == oldN_ptr->previous->h) )
      return false;

   if ( visited.contains(newN.h) )
      return false;

   // If it's off the end of the map, then don't keep scanning
   if( newN.h.x < 0 || newN.h.y < 0 || newN.h.x >= actmap->xsize || newN.h.y >= actmap->ysize )
      return false;

   newN.hasAttacked = oldN_ptr->hasAttacked || disableAttack;

   int fa = fieldAccessible ( actmap->getField ( newN.h ), veh, newN.h.getBitmappedHeight(), &newN.hasAttacked );
   if ( !fa )
      return false;
   newN.canStop = fa >= 2;

   newN.gval = oldN_ptr->gval + getMoveCost ( oldN_ptr->h, newN.h, veh, newN.hasAttacked );
   if ( (newN.gval >= longestPath) || ( newN.gval > MAXIMUM_PATH_LENGTH ) )
      return false;

   newN.enterHeight = keepEnterHeight ? oldN_ptr->enterHeight : -1;
   newN.previous = oldN_ptr;
   newN.hval = dist(newN.h, B);
   return true;
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

bool AStar3D::naivePathFinder(const MapCoordinate3D& a, const MapCoordinate3D& b) {
   MapCoordinate3D current = a;
   MapCoordinate3D next;
   int dir;
   while (current != b) {
      dir = getdirection(current, b);
      next = getNeighbouringFieldCoordinate (current, dir);
      if (fieldAccessible(actmap->getField(next), veh)) {
         current = next;
      } else {
         if (b.y > (current.y + 1))
            dir = 3;
         else if ( b.y < (current.y - 1))
            dir = 0;
         else
            return false;
         next = getNeighbouringFieldCoordinate (current, dir);
         if (fieldAccessible(actmap->getField(next), veh)) {
            current = next;
         } else {
            return false;
         }
      }
   }
   return true;
}

void AStar3D::findPath( const MapCoordinate3D& A, const vector<MapCoordinate3D>& B, Path& path )
{
    _path = &path;

    OpenContainer open;

    /* this should be checked outside by giving the pathfinder the right movement
    if ( !veh->canMove() )
       return;
    */

    Node N = Node(0, dist(A, B), -1, false, veh->attacked);
    N.previous = NULL;
    // insert the original node
    N.h = A;
    if ( !(actmap->getField(A)->unitHere(veh)) ) {
       N.h.setNumericalHeight(-1);
    }
    open.insert(N);

    // Remember which nodes we visited, so that we can clear the mark array
    // at the end.

    bool found = false;

    // While there are still nodes to visit, visit them!
    while( !open.empty() ) {
        N = *open.begin();
        Node* N_ptr = visited.add(N);
        open.erase(open.begin());
        // If we're at the goal, then exit
        MapField* oldFld = actmap->getField(N.h);
        for ( vector<MapCoordinate3D>::const_iterator i = B.begin(); i != B.end(); i++ )
           if( N.h == *i ) {
              //MapField* fld = actmap->getField(N.h);
              if ( N.h.getNumericalHeight() == -1 || !(oldFld->building || (oldFld->vehicle && oldFld->vehicle != veh ))) {
                 found = true;
                 break;
              }
           }
        if ( found )
           break;

        if ( N.h.getNumericalHeight() == -1 ) {
           // the unit is inside a container

          if ( !operationLimiter || operationLimiter->allowLeavingContainer() ) {
             for ( int dir = 0; dir < 6; dir++ ) {
                MapCoordinate3D pos = getNeighbouringFieldCoordinate ( N.h, dir );
                if ( actmap->getField(pos)) {
                   int h = oldFld->getContainer()->vehicleUnloadable(veh->typ);
                   for ( int i = 0; i < 8; i++ )
                      if ( h & (1<<i)) {
                         const ContainerBaseType::TransportationIO* tio = oldFld->getContainer()->vehicleUnloadSystem( veh->typ, 1<<i);
                         if ( tio ) {
                            Node N2;
                            if (!initNode(N2, N_ptr, dir, B, i, tio->disableAttack))
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
             int dock = actmap->getField(N.h)->getContainer()->vehicleDocking(veh, true );
             for ( int dir = 0; dir < 6; dir++ ) {
                if ( dock ) {
                   MapCoordinate3D pos = getNeighbouringFieldCoordinate ( N.h, dir );
                   MapField* fld = actmap->getField( pos );
                   if ( fld && fld->getContainer() && ( fld->getContainer() != oldFld->getContainer() ))
                      if ( fld->getContainer()->vehicleDocking(veh, false ) & dock )
                         if ( fld->getContainer()->getOwner() == oldFld->getContainer()->getOwner() 
                              && veh->getMap()->getPlayer(veh).diplomacy.isAllied( fld->getContainer() ))
                            if ( !fld->building || (fld->bdt & getTerrainBitType(cbbuildingentry) ).any()) {
                               pos.setNumericalHeight(-1);
                               Node N2 = Node(N.gval + 10, dist(pos, B), N.enterHeight, N.canStop, N.hasAttacked);
                               N2.previous = N_ptr;
                               N2.h = pos;
                               addToOpen ( N2, open );
                            }
                }
             }
          }


        } else {
           // the unit is not inside a container

           const int* directions;
           if ( B.begin()->valid() ) {
              directions = getDirectionOrder ( N.h.x, N.h.y, B.begin()->x, B.begin()->y );
           } else {
              static const int d[6] = {0,1,2,3,4,5};
              directions = d;
           }

           if ( !operationLimiter || operationLimiter->allowMovement() )
              for( int dci = 0; dci < 6; dci++ ) {

                 HexDirection dir = HexDirection(directions[dci]);
                 Node N2;
                 if ( !initNode(N2, N_ptr, dir, B) )
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
              if ( (fieldAccessible ( actmap->getField(N.h), veh, N.h.getBitmappedHeight() ) == 2 ) || actmap->getgameparameter( cgp_movefrominvalidfields) )
                 for ( int heightDelta = -1; heightDelta <= 1; heightDelta += 2 ) {
                    const VehicleType::HeightChangeMethod* hcm = veh->getHeightChange( heightDelta, N.h.getBitmappedHeight());
                    if ( hcm ) {
                       for ( int dir = 0; (dir < 6 && hcm->dist) || (dir < 1 && !hcm->dist); dir++ ) {
                          MapCoordinate3D newpos = N.h;
                          bool access = true;
                          for ( int step = 0; step <= hcm->dist; step++ ) {
                             MapField* fld = actmap->getField(newpos);
                             if ( !fld ) {
                                access = false;
                                break;
                             }

                             if (  !fieldAccessible ( fld, veh, N.h.getBitmappedHeight()) && (actmap->getgameparameter( cgp_movefrominvalidfields)==0 || step>0))
                                access = false;

                             if ( !fieldAccessible( fld, veh, 1 << (N.h.getNumericalHeight() + hcm->heightDelta)) )
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
                             bool hasAttacked = N.hasAttacked;
                             int fa = fieldAccessible ( fld, veh, newpos.getBitmappedHeight(), &hasAttacked );
                             if ( fa == 2 ) {
                                bool canStop = true;
                                DistanceType gval = N.gval + getMoveCost( N.h, newpos, veh, hasAttacked );
                                Node N2 = Node(gval, dist(newpos, B), -1, canStop, hasAttacked);
                                N2.previous = N_ptr;
                                N2.h = newpos;

                                if ( actmap->getField(newpos)->getContainer() && actmap->getField(newpos)->vehicle != veh ) {
                                   N2.enterHeight = N2.h.getNumericalHeight() ;
                                   N2.h.setNumericalHeight (-1);
                                }

                                addToOpen ( N2, open );
                             }
                          }
                       }
                    }
                 }


        }
    }

    if ( found ) {
        Node* n = &N;
        while ( n ) {
           path.push_front ( PathPoint(n->h, ceil(n->gval), n->enterHeight, n->hasAttacked) );
           n = n->previous; 
        }
    } else {
        // No path
    }
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
      int& fa = getFieldAccess( node.h );
      fa |= node.h.getBitmappedHeight();
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


int& AStar3D::getFieldAccess ( int x, int y )
{
   return fieldAccess[x + y * actmap->xsize];
}

int& AStar3D::getFieldAccess ( const MapCoordinate& mc )
{
   return fieldAccess[mc.x + mc.y * actmap->xsize];
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

