/*! \file astar2.h
    \brief Interface for the A* pathfinding algorithm
*/


#ifndef astar2_h_included
 #define astar2_h_included

 #include <vector>

enum HexDirection { DirN, DirNE, DirSE, DirS, DirSW, DirNW, DirNone };



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

       //! searches for a path from A to B and stores it in path
       void findPath( HexCoord A, HexCoord B, Path& path );

       //! searches for a path from the units current position to dest and stores it in path
       void findPath( Path& path, int x, int y );

       /** searches for all fields that are within the range of maxDist and marks them.
           On each field one bit for each level of height will be set.
           The Destructor removes all marks.
       */
       void findAllAccessibleFields ( int maxDist = maxint );  // all accessible fields will have a.temp set to 1

       //! returns the distance of the last found path, or -1 on any error
       int getDistance( );

       //! returns the number of turns that the unit will need to travel along the last found path
       int getTravelTime( );

       //! checks weather the field fld was among the visited fields during the last search
       bool fieldVisited ( int x, int y);
       AStar ( pmap actmap, pvehicle veh );
       virtual ~AStar ( );
 };


 //! finding a path for unit veh to position x, y on map actmap.
extern void findPath( pmap actmap, AStar::Path& path, pvehicle veh, int x, int y );





class AStar3D {
    public:
       typedef vector<MapCoordinate3D> Path;
       struct Node {
           MapCoordinate3D h;        // location on the map, in hex coordinates
           int gval;        // g in A* represents how far we've already gone
           int hval;        // h in A* represents an estimate of how far is left
           Node(): gval(0), hval(0) {}
           bool operator< ( const Node& a );
       };

    protected:
       int MAXIMUM_PATH_LENGTH;
       pmap tempsMarked;
       Path *_path;
       pvehicle veh;
       pmap actmap;
       float vehicleSpeedFactor[8];
       bool markTemps;


       virtual int getMoveCost ( const MapCoordinate3D& start, const MapCoordinate3D& dest, const pvehicle vehicle );

       HexDirection* posDirs;
       int*          posHHops;
       char*         fieldAccess;
       HexDirection& getPosDir ( const MapCoordinate3D& pos ) { return posDirs [(pos.y * actmap->xsize + pos.x) * 8 + log2(pos.z)]; };
       int& getPosHHop ( const MapCoordinate3D& pos )         { return posHHops[(pos.y * actmap->xsize + pos.x) * 8 + log2(pos.z)]; };

       int dist( const MapCoordinate3D& a, const MapCoordinate3D& b );

       typedef std::vector<Node> Container;
       greater<Node> comp;

       void get_first( Container& v, Node& n );

       void nodeVisited ( pfield fld, const Node& n, HexDirection direc, Container& open, int heightDelta = 0 );

       Container visited;

    public:

       //! searches for a path from A to B and stores it in path
       void findPath( const MapCoordinate3D& A, const MapCoordinate3D& B, Path& path );

       //! searches for a path from the units current position to dest and stores it in path
       void findPath( Path& path, const MapCoordinate3D& dest );

       /** searches for all fields that are within the range of maxDist and marks them.
           On each field one bit for each level of height will be set.
           The Destructor removes all marks.
       */
       void findAllAccessibleFields ( int maxDist = maxint );

       //! returns the distance of the last found path, or -1 on any error
       int getDistance( );

       //! returns the number of turns that the unit will need to travel along the last found path
       int getTravelTime( );

       //! checks weather the field fld was among the visited fields during the last search
       Node* fieldVisited ( const MapCoordinate3D& fld );

       char& getFieldAccess ( int x, int y );
       char& getFieldAccess ( const MapCoordinate& mc );

       AStar3D ( pmap actmap, pvehicle veh, bool markTemps_ = true );
       virtual ~AStar3D ( );
 };

#endif
