#ifndef astar2_h_included
 #define astar2_h_included

 #include <vector>

/*! \file astar2.h
    Interface for the A* pathfinding algorithm
*/


class AStar {
    public:
       typedef vector<MapCoordinate> Path;

    protected:
       int MAXIMUM_PATH_LENGTH;
       pmap tempsMarked;
       Path *_path;
       pvehicle _veh;
       pmap _actmap;


       virtual int getMoveCost ( int x1, int y1, int x2, int y2, const pvehicle vehicle );
    public:


       struct HexCoord{
           int m, n;
           HexCoord(): m(0), n(0) {}
           HexCoord( int m_, int n_ ): m(m_), n(n_) {}
           ~HexCoord() {}
       };

       struct Node {
           HexCoord h;        // location on the map, in hex coordinates
           int gval;        // g in A* represents how far we've already gone
           int hval;        // h in A* represents an estimate of how far is left
           Node(): h(0,0), gval(0), hval(0) {}
           bool operator< ( const Node& a );
       };
       int dist( HexCoord a, HexCoord b );

       typedef std::vector<Node> Container;
       greater<Node> comp;

       inline void get_first( Container& v, Node& n );


       Container visited;

       void findPath( HexCoord A, HexCoord B, Path& path );
       void findPath( Path& path, int x, int y );
       void findAllAccessibleFields ( int maxDist = maxint );  // all accessible fields will have a.temp set to 1
       int getDistance( );
       int getTravelTime( );
       bool fieldVisited ( int x, int y);
       AStar ( pmap actmap, pvehicle veh );
       virtual ~AStar ( );
 };

//bool operator< ( const AStar::Node& a, const AStar::Node& b );

 //! finding a path for unit veh to position x, y on map actmap.
extern void findPath( pmap actmap, AStar::Path& path, pvehicle veh, int x, int y );

#endif
