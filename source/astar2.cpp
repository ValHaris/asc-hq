#include <stack>
#include <vector>
#include "spfst.h"
#include "controls.h"


// Path_div is used to modify the heuristic.  The lower the number,
// the higher the heuristic value.  This gives us worse paths, but
// it finds them faster.  This is a variable instead of a constant
// so that I can adjust this dynamically, depending on how much CPU
// time I have.  The more CPU time there is, the better paths I should
// search for.
int path_div = 8;

#define MAXIMUM_PATH_LENGTH 100000

// The mark array marks directions on the map.  The direction points
// to the spot that is the previous spot along the path.  By starting
// at the end, we can trace our way back to the start, and have a path.


// mark -> temp3

enum HexDirection { DirN, DirNE, DirSE, DirS, DirSW, DirNW, DirNone };


struct HexCoord
{
    int m, n;
    HexCoord(): m(0), n(0) {}
    HexCoord( int m_, int n_ ): m(m_), n(n_) {}
    ~HexCoord() {}
/*
    int x() const { return m * fielddistx + (n&1) * fielddisthalfx; }
    int y() const { return n * fielddisty ; }
    
    int left() const { return x() - HexWidth/2; }
    int right() const { return x() + HexWidth/2; }
    int bottom() const { return y() - HexHeight/2; }
    int top() const { return y() + HexHeight/2; }
    
    // Rect rect() const { return Rect(left(),bottom(),right(),top()); }
*/
};


struct Node
{
    HexCoord h;        // location on the map, in hex coordinates
    int gval;        // g in A* represents how far we've already gone
    int hval;        // h in A* represents an estimate of how far is left
    Node(): h(0,0), gval(0), hval(0) {}
};

bool operator < ( const Node& a, const Node& b )
{
    // To compare two nodes, we compare the `f' value, which is the
    // sum of the g and h values.
    return (a.gval+a.hval) < (b.gval+b.hval);
}

bool operator == ( const HexCoord& a, const HexCoord& b )
{
   return (a.m == b.m) && (a.n == b.n );
}


bool operator == ( const Node& a, const Node& b )
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



int dist( HexCoord a, HexCoord b )
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



int kost( HexCoord a, HexDirection d, HexCoord b, pvehicle vehicle )
{
    // This is the cost of moving one step.  To get completely accurate
    // paths, this must be greater than or equal to the change in the
    // distance function when you take a step.

    /*
    Terrain t = m.terrain(b);
    // No walking through walls!
    if( t == Wall || t == Tower )
        return MAXIMUM_PATH_LENGTH;
    // I take the difference in altitude and use that as a cost
    int da = (m.altitude(b)-m.altitude(a)+ALTITUDE_SCALE/2)/ALTITUDE_SCALE;
    // Roads are faster
    int rd = int(t == Road);
    return (2-rd) + (da>0?da:0);
    */
    int movecost, fuelcost;
    calcmovemalus ( a.m, a.n, b.m, b.n, vehicle, -1, movecost, fuelcost );
    return movecost;
}



// greater(Node) is an STL thing to create a 'comparison' object out of
// the greater-than operator, and call it comp.
typedef vector<Node> Container;
greater<Node> comp;


// I'm using a priority queue implemented as a heap.  STL has some nice
// heap manipulation functions.  (Look at the source to `priority_queue'
// for details.)  I didn't use priority_queue because later on, I need
// to traverse the entire data structure to update certain elements; the
// abstraction layer on priority_queue wouldn't let me do that.

inline void get_first( Container& v, Node& n )
{
    n = v.front();
    pop_heap( v.begin(), v.end(), comp );
    v.pop_back();
}


// Here's the algorithm.  I take a map, two points (A and B), and then
// output the path in the `path' vector.



void AStar( pmap actmap, HexCoord A, HexCoord B, vector<int>& path, pvehicle veh )
{
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
    Container visited;

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
            if( hn.m < 0 || hn.n < 0 || hn.m >= actmap->xsize || hn.n >= actmap->ysize )
                continue;

            int k = kost( N.h, d, hn, veh );
            Node N2;
            N2.h = hn;
            N2.gval = N.gval + k;
            N2.hval = dist( hn, B );
            // If this spot (hn) hasn't been visited, its mark is DirNone
            if( getfield (hn.m,hn.n)->temp3 == DirNone ) {

                // The space is not marked
                getfield (hn.m,hn.n)->temp3 = ReverseDirection(d);
                open.push_back( N2 );
                push_heap( open.begin(), open.end(), comp );
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
        HexCoord h = B;
        while( h != A )
        {
            HexDirection dir = HexDirection ( getfield (h.m, h.n)->temp3 );
            path.push_back( int( ReverseDirection( dir ) ) );
            getnextfield ( h.m, h.n, dir );
        }
        // path now contains the directions the unit must travel .. backwards
        // (like a stack)
    }
    else
    {
        // No path
    }

    actmap->cleartemps ( 4 ); 
}

void AStar( pmap actmap, vector<int>& path, pvehicle veh, int x, int y )
{
   AStar ( actmap, HexCoord ( veh->xpos, veh->ypos ), HexCoord ( x, y ), path, veh );
}

