/*! \file astar2.cpp
    \brief Pathfinding routines using the A* algorithm. 
*/



#include <stack>
#include <vector>
#include <functional>
#include <algorithm>

#include "vehicletype.h"
#include "spfst.h"
#include "controls.h"

#include "astar2.h"


// The mark array marks directions on the map.  The direction points
// to the spot that is the previous spot along the path.  By starting
// at the end, we can trace our way back to the start, and have a path.


// mark -> temp3


bool operator < ( const AStar::Node& a, const AStar::Node& b )
{
    // To compare two nodes, we compare the `f' value, which is the
    // sum of the g and h values.
    return (a.gval+a.hval) < (b.gval+b.hval);
}

bool operator > ( const AStar::Node& a, const AStar::Node& b )
{
    return (a.gval+a.hval) > (b.gval+b.hval);
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


AStar :: AStar ( pmap actmap, pvehicle veh )
{
   tempsMarked = NULL;
   _path = NULL;
   _veh = veh;
   _actmap = actmap;
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

int AStar::getMoveCost ( int x1, int y1, int x2, int y2, const pvehicle vehicle )
{
    if ( !fieldaccessible ( getfield ( x2, y2 ), vehicle ))
       return MAXIMUM_PATH_LENGTH;

    int movecost, fuelcost;
    calcmovemalus ( x1, y1, x2, y2, vehicle, -1, fuelcost, movecost );
    return movecost;
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
    pvehicle veh = _veh;
    pmap actmap = _actmap;

    if ( actmap->getField(A.m, A.n)->unitHere(veh) )
       if ( !veh->canMove() )
          return;

    for ( int y = actmap->xsize * actmap->ysize -1; y >= 0; y-- )
       actmap->field[y].temp3 = DirNone;

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
            getnextfield ( hn.m, hn.n, d );
            // If it's off the end of the map, then don't keep scanning
            if( hn.m < 0 || hn.n < 0 || hn.m >= actmap->xsize || hn.n >= actmap->ysize || !fieldaccessible ( getfield ( hn.m, hn.n ), veh ))
                continue;

            // cursor.gotoxy ( hn.m, hn.n );
            int k = getMoveCost( N.h.m, N.h.n, hn.m, hn.n, veh );
            if ( k > veh->maxMovement() )
               k = MAXIMUM_PATH_LENGTH;

            Node N2;
            N2.h = hn;
            N2.gval = N.gval + k;
            N2.hval = dist( hn, B );
            // If this spot (hn) hasn't been visited, its mark is DirNone
            if( getfield (hn.m,hn.n)->temp3 == DirNone ) {

                // The space is not marked

                if ( N.gval < MAXIMUM_PATH_LENGTH ) {
                   getfield (hn.m,hn.n)->temp3 = ReverseDirection(d);
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
                        getfield (hn.m,hn.n)->temp3 = ReverseDirection(d);
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
            HexDirection dir = HexDirection ( getfield (h.m, h.n)->temp3 );
            tempPath.push_back( int( ReverseDirection( dir ) ) );
            getnextfield ( h.m, h.n, dir );
        }

        // tempPath now contains the directions the unit must travel .. backwards
        // (like a stack)

        int x = A.m;
        int y = A.n;

        for ( int i = tempPath.size()-1; i >= 0 ; i-- ) {
           getnextfield ( x, y, tempPath[i] );
           path.push_back ( MapCoordinate ( x, y ));
        }
    }
    else
    {
        // No path
    }

    actmap->cleartemps ( 4 ); 
}

int AStar::getDistance ( )
{
   if ( !_path || !_veh || ! _actmap )
      return -1;

   if ( _path->size() <= 0 )
      return -1;

   int dist = 0;
   for ( Path::iterator i = _path->begin(); i != _path->end(); i++ )
      dist += _actmap->getField ( *i )->getmovemalus ( _veh->typ->movemalustyp );

   return dist;
}

int AStar::getTravelTime( )
{
   int dist = getDistance();
   if ( dist < 0 )
      return dist;
   else
      return dist / _veh->typ->movement[ log2 ( _veh->height ) ];
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
   _actmap->cleartemps ( 1 );

   MAXIMUM_PATH_LENGTH = maxDist;

   Path dummy;
   findPath ( dummy, _actmap->xsize, _actmap->ysize );  //this field does not exist...
   for ( Container::iterator i = visited.begin(); i != visited.end(); i++ )
      _actmap->getField ( (*i).h.m, (*i).h.n )->a.temp = 1;

   tempsMarked = _actmap;
}


void AStar::findPath( Path& path, int x, int y )
{
  findPath ( AStar::HexCoord ( _veh->xpos, _veh->ypos ), AStar::HexCoord ( x, y ), path );
}




void findPath( pmap actmap, AStar::Path& path, pvehicle veh, int x, int y )
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


bool operator < ( const AStar3D::Node& a, const AStar3D::Node& b )
{
    // To compare two nodes, we compare the `f' value, which is the
    // sum of the g and h values.
    return (a.gval+a.hval) < (b.gval+b.hval);
}

bool operator > ( const AStar3D::Node& a, const AStar3D::Node& b )
{
    return (a.gval+a.hval) > (b.gval+b.hval);
}


bool operator == ( const AStar3D::Node& a, const AStar3D::Node& b )
{
    // Two nodes are equal if their components are equal
    return (a.h == b.h) && (a.gval == b.gval ) && (a.hval == b.hval );
}



AStar3D :: AStar3D ( pmap actmap_, pvehicle veh_, bool markTemps_ )
{
   markTemps = markTemps_;
   tempsMarked = NULL;
   _path = NULL;
   veh = veh_;
   actmap = actmap_;

   MAXIMUM_PATH_LENGTH = maxint;

   float maxVehicleSpeed = 0;
   for ( int i = 0; i < 8; i++ )
      if ( veh->typ->movement[i] > maxVehicleSpeed )
         maxVehicleSpeed = veh->typ->movement[i];

   if ( !maxVehicleSpeed )
      fatalError ( "AStar3D :: AStar3D  -  trying to move a immobile unit");

   for ( int i = 0; i < 8; i++ )
       vehicleSpeedFactor[i] = float(veh->typ->movement[i]) / maxVehicleSpeed;

   int cnt = actmap->xsize*actmap->ysize*8;
   posDirs = new HexDirection[cnt];
   posHHops = new int[cnt];
   fieldAccess = new char[cnt];

   for ( int i = 0; i < cnt; i++ ) {
      posDirs[i] = DirNone;
      posHHops[i] = 0;
      fieldAccess[i] = 0;
   }

}


AStar3D :: ~AStar3D ( )
{
   if( tempsMarked )
      tempsMarked->cleartemps ( 1 );

   delete[] posDirs;
   delete[] posHHops;
   delete[] fieldAccess;
}

int AStar3D::dist( const MapCoordinate3D& a, const MapCoordinate3D& b )
{
    return beeline ( a, b ) + abs ( log2(b.z) - log2(a.z) ) * minmalq;
}

int AStar3D::getMoveCost ( const MapCoordinate3D& start, const MapCoordinate3D& dest, const pvehicle vehicle )
{
    // since we are operating at different levels of height and the unit has different
    // speeds at different levels of height, we must not optimize for distance, but for
    // travel time.

    int fa = fieldaccessible ( actmap->getField ( dest ), vehicle, dest.z );

    if ( !fa )
       return MAXIMUM_PATH_LENGTH;

    int movecost, fuelcost;
    calcmovemalus ( start.x, start.y, dest.x, dest.y, vehicle, -1, fuelcost, movecost, dest.z );
    return int(movecost / vehicleSpeedFactor[log2(dest.z)]);
}



// greater(Node) is an STL thing to create a 'comparison' object out of
// the greater-than operator, and call it comp.


// I'm using a priority queue implemented as a heap.  STL has some nice
// heap manipulation functions.  (Look at the source to `priority_queue'
// for details.)  I didn't use priority_queue because later on, I need
// to traverse the entire data structure to update certain elements; the
// abstraction layer on priority_queue wouldn't let me do that.

// Wouldn't maps be fast ?? [MB]

inline void AStar3D::get_first( Container& v, Node& n )
{
    n = v.front();
    pop_heap( v.begin(), v.end(), comp );
    v.pop_back();
}


void AStar3D :: nodeVisited ( pfield fld, const Node& N2, HexDirection direc, Container& open, int heightDelta )
{
   // If this spot (hn) hasn't been visited, its mark is DirNone
   if( getPosDir(N2.h) == DirNone ) {

       // The space is not marked

       if ( N2.gval < MAXIMUM_PATH_LENGTH ) {
          getPosDir(N2.h) = ReverseDirection(direc);
          getPosHHop(N2.h) = heightDelta;
          open.push_back( N2 );
          push_heap( open.begin(), open.end(), comp );
       }

       /* some debug code
       fld->a.temp = 1;
       cursor.gotoxy ( N2.h.x, N2.h.y );
       displaymap();
       */

   } else {
       // Search for hn in open
       Container::iterator find1 = open.end();
       for( Container::iterator i = open.begin(); i != open.end(); i++ )
           if( i->h == N2.h ) {
               find1 = i;
               break;
           }

       // if found, call it N3
       if( find1 != open.end() ) {
           Node N3 = *find1;
           if( N3.gval > N2.gval ) {
               getPosDir(N2.h) = ReverseDirection(direc);
               getPosHHop(N2.h) = heightDelta;
               // Replace N3 with N2 in the open list
               Container::iterator last = open.end() - 1;
               *find1 = *last;
               *last = N2;
               push_heap( open.begin(), open.end(), comp );
           }
       }
   }
}


void AStar3D::findPath( const MapCoordinate3D& A, const MapCoordinate3D& B, Path& path )
{
    _path = &path;

    Node N;
    Container open;

    if ( actmap->getField(A)->unitHere(veh) )
       if ( !veh->canMove() )
          return;


    // insert the original node
    N.h = A;
    N.gval = 0;
    N.hval = dist(A,B);
    open.push_back(N);


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
        if( N.h.x % 2 == 0 )
            directions = directions1;
        else
            directions = directions2;

        // Look at your neighbors.
        for( int dci = 0; dci < 6; dci++ ) {

            HexDirection d = HexDirection(directions[dci]);
            MapCoordinate3D hn = N.h;
            getnextfield ( hn.x, hn.y, d );
            // If it's off the end of the map, then don't keep scanning
            if( hn.x < 0 || hn.y < 0 || hn.x >= actmap->xsize || hn.y >= actmap->ysize || !fieldaccessible ( actmap->getField ( hn ), veh, hn.z ))
                continue;

            // cursor.gotoxy ( hn.m, hn.n );
            int k = getMoveCost( N.h, hn, veh );
            if ( k > veh->typ->movement[log2(hn.z)]  )
               k = MAXIMUM_PATH_LENGTH;
            Node N2;
            N2.h = hn;
            N2.gval = N.gval + k;
            N2.hval = dist( hn, B );

            pfield fld = actmap->getField(hn);
            nodeVisited ( fld, N2, d, open );
        }

        // and now change the units' height. That's only possible on fields where the unit can stop it's movement

        if ( fieldaccessible ( actmap->getField(N.h), veh, N.h.z ) == 2 ) {
           for ( int heightDelta = -1; heightDelta <= 1; heightDelta += 2 ) {
              for ( int dir = 0; (dir < 6 && veh->typ->steigung) || (dir < 1 && !veh->typ->steigung); dir++ ) {
                 pair<int, MapCoordinate3D> mcres = ChangeVehicleHeight::getMoveCost ( veh, N.h, dir, heightDelta );
                 if ( mcres.first >= 0 ) {
                    // the operation was successfull; mcres->first now contains the moveCost
                    Node N2;
                    N2.h = mcres.second;
                    N2.gval = N.gval + mcres.first;
                    N2.hval = dist ( mcres.second, B );
                    pfield fld = actmap->getField(N2.h);
                    nodeVisited ( fld, N2, HexDirection(dir), open, (10 + beeline ( N2.h, N.h)) * heightDelta );
                 }
              }
           }
        }

    }

    if( N.h == B && N.gval < MAXIMUM_PATH_LENGTH ) {
        // We have found a path, so let's copy it into `path'
        std::vector<int> tempPath;
        std::vector<int> heightHops;

        MapCoordinate3D h = B;
        while( !(h == A) )
        {
            // pfield fld = actmap->getField ( h );
            HexDirection dir = HexDirection ( getPosDir(h) );
            tempPath.push_back( int( ReverseDirection( dir ) ) );

            int heightDelta = getPosHHop(h);
            heightHops.push_back ( heightDelta );

            if ( heightDelta ) {
               for ( int i = 10; i < abs(heightDelta); i += maxmalq )
                  getnextfield ( h.x, h.y, dir );

               if ( heightDelta > 0 )
                  h.z >>= 1;
               else
                  h.z <<= 1;
            } else
               getnextfield ( h.x, h.y, dir );
        }

        // tempPath now contains the directions the unit must travel .. backwards
        // (like a stack)

        h = A;

        for ( int i = tempPath.size()-1; i >= 0 ; i-- ) {
           if ( heightHops[i] ) {
              for ( int j = 10; j < abs (heightHops[i]); j += maxmalq )
                  getnextfield ( h.x, h.y, tempPath[i] );

           } else
              getnextfield ( h.x, h.y, tempPath[i] );

           if ( heightHops[i] > 0 )
              h.z <<= 1;
           else
              if ( heightHops[i] < 0 )
                 h.z >>= 1;

           path.push_back ( h );
        }
    }
    else
    {
        // No path
    }
}

void AStar3D::findPath( Path& path, const MapCoordinate3D& dest )
{
  findPath ( MapCoordinate3D ( veh->xpos, veh->ypos, veh->height ), dest, path );
}

void AStar3D::findAllAccessibleFields ( int maxDist )
{
   actmap->cleartemps ( 1 );

   MAXIMUM_PATH_LENGTH = maxDist;

   Path dummy;
   findPath ( dummy, MapCoordinate3D(actmap->xsize, actmap->ysize, veh->height) );  //this field does not exist...
   for ( Container::iterator i = visited.begin(); i != visited.end(); i++ ) {
      getFieldAccess( i->h ) |= i->h.z;
      if ( markTemps )
         actmap->getField ( i->h )->a.temp  |= i->h.z;
   }
   if ( markTemps )
      tempsMarked = actmap;
}

AStar3D::Node* AStar3D::fieldVisited ( const MapCoordinate3D& pos )
{
   for( Container::iterator i = visited.begin(); i != visited.end(); i++ )
       if( i->h == pos  )
          return &(*i);

   return NULL;
}


char& AStar3D::getFieldAccess ( int x, int y )
{
   return fieldAccess[x + y * actmap->xsize];
}

char& AStar3D::getFieldAccess ( const MapCoordinate& mc )
{
   return fieldAccess[mc.x + mc.y * actmap->xsize];
}
