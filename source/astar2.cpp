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

const int longestPath = 10000000;

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

    return calcmovemalus ( MapCoordinate3D(x1, y1, vehicle->height), MapCoordinate3D(x2, y2, vehicle->height), vehicle ).first;
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

            if ( k == MAXIMUM_PATH_LENGTH )
               N2.gval = k;
            else
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



AStar3D :: AStar3D ( pmap actmap_, pvehicle veh_, bool markTemps_, int maxDistance, bool changeHeight_ )
{
   changeHeight = changeHeight_; 
   markTemps = markTemps_;
   tempsMarked = NULL;
   _path = NULL;
   veh = veh_;
   actmap = actmap_;

   MAXIMUM_PATH_LENGTH = maxDistance;
   if ( MAXIMUM_PATH_LENGTH > longestPath )
     MAXIMUM_PATH_LENGTH = 2* longestPath;


   if ( !veh->maxMovement() )
      fatalError ( "AStar3D :: AStar3D  -  trying to move a immobile unit");

   if ( veh->getPosition().getNumericalHeight() >= 0 )
      for ( int i = 0; i < 8; i++ )
          vehicleSpeedFactor[i] = float(veh->typ->movement[i]) / veh->maxMovement();
   else
      for ( int i = 0; i < 8; i++ )
          vehicleSpeedFactor[i] = float(veh->typ->movement[i]);

   int cnt = actmap->xsize*actmap->ysize*9;
   posDirs = new HexDirection[cnt];
   posHHops = new int[cnt];
   fieldAccess = new int[cnt];

   for ( int i = 0; i < cnt; i++ ) {
      posDirs[i] = DirNone;
      posHHops[i] = -20;
      fieldAccess[i] = 0;
   }

}


AStar3D :: ~AStar3D ( )
{
   if( tempsMarked )
      tempsMarked->cleartemps ( 3 );

   delete[] posDirs;
   delete[] posHHops;
   delete[] fieldAccess;
}

int AStar3D::dist( const MapCoordinate3D& a, const MapCoordinate3D& b )
{
   if ( b.valid() )
      return beeline ( a, b ) + abs ( b.getNumericalHeight() - a.getNumericalHeight() ) * minmalq;
   else
      return maxint/2;
}

int AStar3D::dist ( const MapCoordinate3D& a, const vector<MapCoordinate3D>& b )
{
   int d = maxint;
   for ( vector<MapCoordinate3D>::const_iterator i = b.begin(); i != b.end(); i++ )
      d = min(dist(a,*i), d);
   return d;
}


int AStar3D::getMoveCost ( const MapCoordinate3D& start, const MapCoordinate3D& dest, const pvehicle vehicle, bool& canStop )
{
    // since we are operating at different levels of height and the unit has different
    // speeds at different levels of height, we must not optimize for distance, but for
    // travel time.

    int fa = fieldaccessible ( actmap->getField ( dest ), vehicle, dest.getBitmappedHeight() );

    canStop = fa >= 2;

    if ( !fa )
       return maxint;

    int movecost = calcmovemalus ( start, dest, vehicle ).first;
    if ( start.getNumericalHeight() < 0 )
       return movecost;
    else
       if ( !vehicleSpeedFactor[start.getNumericalHeight()] )
          return maxint/2;
       else
          return int(movecost / vehicleSpeedFactor[start.getNumericalHeight()]);
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


void AStar3D :: nodeVisited ( const Node& N2, HexDirection direc, Container& open, int prevHeight, int heightChangeDist )
{
   // If this spot (hn) hasn't been visited, its mark is DirNone
   if( getPosDir(N2.h) == DirNone ) {

       // The space is not marked

       if ( N2.gval <= MAXIMUM_PATH_LENGTH && N2.gval <= longestPath ) {
          getPosDir(N2.h) = ReverseDirection(direc);
          getPosHHop(N2.h) = 10 + prevHeight + 1000 * heightChangeDist;
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
               getPosHHop(N2.h) = 10 + prevHeight + 1000 * heightChangeDist;
               // Replace N3 with N2 in the open list
               Container::iterator last = open.end() - 1;
               *find1 = *last;
               *last = N2;
               push_heap( open.begin(), open.end(), comp );
           }
       }
   }
}


void AStar3D::findPath( const MapCoordinate3D& A, const vector<MapCoordinate3D>& B, Path& path )
{
    _path = &path;

    Node N;
    Container open;

    if ( !veh->canMove() )
       return;

    if ( actmap->getField(A)->unitHere(veh) ) {
       // insert the original node
       N.h = A;
       N.gval = 0;
       N.hval = dist(A,B);
       open.push_back(N);
    } else {
       N.h.setnum(A.x, A.y, -1 );
       N.gval = 0;
       N.hval = dist(A,B);
       open.push_back(N);
    }

    // Remember which nodes we visited, so that we can clear the mark array
    // at the end.

    bool found = false;
    MapCoordinate3D endpos;

    // While there are still nodes to visit, visit them!
    while( !open.empty() ) {
        get_first( open, N );
        visited.push_back( N );
        // If we're at the goal, then exit
        for ( vector<MapCoordinate3D>::const_iterator i = B.begin(); i != B.end(); i++ )
           if( N.h == *i ) {
              found = true;
              endpos = *i;
              break;
           }
        if ( found )
           break;


        if ( N.h.getNumericalHeight() == -1 ) {
           // the unit is inside a container

          for ( int dir = 0; dir < 6; dir++ ) {
             MapCoordinate3D pos = getNeighbouringFieldCoordinate ( N.h, dir );
             int h = actmap->getField(N.h)->getContainer()->vehicleUnloadable(veh);
             for ( int i = 0; i < 8; i++ )
                if ( h & (1<<i)) {
                   Node N2;
                   N2.h.setnum ( pos.x, pos.y, i );

                   int k = getMoveCost( N.h, N2.h, veh, N2.canStop );
                   if ( k > veh->typ->movement[N2.h.getNumericalHeight()]  )
                      k = max(MAXIMUM_PATH_LENGTH,k);

                   if ( k > maxint / 2 || N.gval > maxint/2 )
                      N2.gval = maxint;
                   else
                      N2.gval = N.gval + k;

                   N2.hval = dist(pos,B);

                   nodeVisited ( N2, dir, open, N.h.getNumericalHeight(), maxmalq );
                }

             int dock = actmap->getField(A)->getContainer()->vehicleDocking(veh);
             if ( dock )
                for ( int dir = 0; dir < 6; dir++ ) {
                   MapCoordinate3D pos = getNeighbouringFieldCoordinate ( A, dir );
                   pfield fld = actmap->getField( pos );
                   if ( fld->getContainer() )
                      if ( fld->getContainer()->vehicleDocking(veh) & dock ) {
                        Node N2;
                        N2.h.setnum ( pos.x, pos.y, -1);
                        N2.canStop = true;
                        N2.gval = N.gval + 10;
                        N2.hval = dist ( pos, B );
                        nodeVisited ( N2, HexDirection(dir), open );
                      }
                }
          }


        } else {
           // the unit is not inside a container


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

               HexDirection dir = HexDirection(directions[dci]);
               MapCoordinate3D hn = getNeighbouringFieldCoordinate ( N.h, dir );

               // If it's off the end of the map, then don't keep scanning
               if( hn.x < 0 || hn.y < 0 || hn.x >= actmap->xsize || hn.y >= actmap->ysize || !fieldaccessible ( actmap->getField ( hn ), veh, hn.getBitmappedHeight() ))
                   continue;

               // cursor.gotoxy ( hn.m, hn.n );

               Node N2;
               int k = getMoveCost( N.h, hn, veh, N2.canStop );
               if ( k > veh->typ->movement[hn.getNumericalHeight()]  )
                  k = 10*longestPath;
               N2.h = hn;

               if ( k >= MAXIMUM_PATH_LENGTH )
                  N2.gval = k;
               else
                  N2.gval = N.gval + k;

               N2.hval = dist(hn,B);


               if ( N2.canStop && actmap->getField(N2.h)->getContainer() && actmap->getField(N2.h)->vehicle != veh) {
                  // there's an container on the field that can be entered. This means, the unit can't stop 'over' the container...
                  N2.canStop = false;
                  nodeVisited ( N2, dir, open );

                  // ... only inside it
                  N2.canStop = true;
                  N2.h.setnum ( N2.h.x, N2.h.y, -1 );
                  nodeVisited ( N2, dir, open, N.h.getNumericalHeight(), maxmalq );
               } else
                  nodeVisited ( N2, dir, open );
           }

           // and now change the units' height. That's only possible on fields where the unit can stop it's movement


           if ( changeHeight )
              if ( fieldaccessible ( actmap->getField(N.h), veh, N.h.getBitmappedHeight() ) == 2 )
                 for ( int heightDelta = -1; heightDelta <= 1; heightDelta += 2 ) {
                    const Vehicletype::HeightChangeMethod* hcm = veh->getHeightChange( heightDelta, N.h.getBitmappedHeight());
                    if ( hcm ) {
                       for ( int dir = 0; (dir < 6 && hcm->dist) || (dir < 1 && !hcm->dist); dir++ ) {
                          MapCoordinate3D newpos = N.h;
                          bool access = true;
                          for ( int step = 0; step < hcm->dist; step++ ) {
                             getnextfield ( newpos.x, newpos.y, dir );
                             pfield fld = actmap->getField(newpos);
                             if ( !fld || !fieldaccessible ( fld, veh, N.h.getBitmappedHeight() ) || !fieldaccessible( fld, veh, 1 << (N.h.getNumericalHeight() + hcm->heightDelta)) )
                                access = false;
                          }

                          pfield fld = actmap->getField( newpos );
                          if ( fld && access ) {
                             Node N2;
                             N2.h.setnum ( newpos.x, newpos.y, N.h.getNumericalHeight() + hcm->heightDelta );
                             if ( fieldaccessible( fld, veh, N2.h.getBitmappedHeight() ) == 2 ) {
                                N2.canStop = true;
                                N2.gval = N.gval + getMoveCost( N.h, N2.h, veh, N2.canStop );
                                N2.hval = dist(N2.h,B);
                                nodeVisited ( N2, HexDirection(dir), open, N.h.getNumericalHeight() , hcm->dist * maxmalq );
                             }
                          }
                       }
                    }
                 }


        }
    }

    if( found && N.gval <= MAXIMUM_PATH_LENGTH && N.gval <= longestPath ) {
        // We have found a path, so let's copy it into `path'

        MapCoordinate3D h = endpos;
        path.insert ( path.begin(), h );
        while( !(h == A) )
        {
            // pfield fld = actmap->getField ( h );
            HexDirection dir = HexDirection ( getPosDir(h) );

            int prevHeight = getPosHHop(h);
            int heightChangeDist = prevHeight / 1000;
            prevHeight -= 10 + heightChangeDist * 1000;

            if ( prevHeight > -10  ) {
               for ( int i = 0; i < heightChangeDist; i += maxmalq )
                  getnextfield ( h.x, h.y, dir );

               h.setnum ( h.x, h.y, prevHeight );
            } else
                getnextfield ( h.x, h.y, dir );
            path.insert ( path.begin(), h );
        }
    }
    else
    {
        // No path
    }
}

void AStar3D::findPath( Path& path, const MapCoordinate3D& dest )
{
  vector<MapCoordinate3D> d;
  d.push_back ( dest );
  findPath ( MapCoordinate3D ( veh->xpos, veh->ypos, veh->height ), d, path );
}

void AStar3D::findPath( Path& path, const vector<MapCoordinate3D>& dest )
{
  findPath ( MapCoordinate3D ( veh->xpos, veh->ypos, veh->height ), dest, path );
}


void AStar3D::findAllAccessibleFields ( )
{
   if ( markTemps )
      actmap->cleartemps ( 3 );

   Path dummy;
   findPath ( dummy, MapCoordinate3D(actmap->xsize, actmap->ysize, veh->height) );  //this field does not exist...
   for ( Container::iterator i = visited.begin(); i != visited.end(); i++ ) {
      int& fa = getFieldAccess( i->h );
      fa |= i->h.getBitmappedHeight();
      if ( markTemps )
         actmap->getField ( i->h )->a.temp  |= i->h.getBitmappedHeight();
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


int& AStar3D::getFieldAccess ( int x, int y )
{
   return fieldAccess[x + y * actmap->xsize];
}

int& AStar3D::getFieldAccess ( const MapCoordinate& mc )
{
   return fieldAccess[mc.x + mc.y * actmap->xsize];
}
