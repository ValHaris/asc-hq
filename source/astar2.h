#ifndef astar2_h_included
 #define astar2_h_included
 
 #include <vector>

/*! \file astar2.h
    finding a path with the A* algorithm
*/

 class AStar {
       pmap tempsMarked;
    protected:
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
       };
       int dist( HexCoord a, HexCoord b );

       typedef std::vector<Node> Container;
       std::greater<Node> comp;

       inline void get_first( Container& v, Node& n );


       Container visited;

       void findPath( pmap actmap, HexCoord A, HexCoord B, std::vector<int>& path, pvehicle veh );
       void findPath( pmap actmap, std::vector<int>& path, pvehicle veh, int x, int y );
       void findAllAccessibleFields ( pmap actmap, pvehicle veh );  // all accessible fields will have a.temp set to 1
       bool fieldVisited ( int x, int y);
       AStar ( void );
       ~AStar ( );
 };

 //! finding a path for unit veh to position x, y on map actmap. The path is copied to path in REVERSE ORDER ! (path[0] is the last field)
 extern void findPath( pmap actmap, std::vector<int>& path, pvehicle veh, int x, int y );

#endif
