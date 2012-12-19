/*! \file astar2.h
    \brief Interface for the A* pathfinding algorithm
*/


#ifndef astar2H
 #define astar2H


 #include <vector>
 #include <map>
 #include <set>
 #include "mapalgorithms.h"
 #include "gamemap.h"




 enum HexDirection { DirN, DirNE, DirSE, DirS, DirSW, DirNW, DirNone };


 //! A 2dimensional path finding algorithm, from Amit J. Patel
 class AStar {
    public:
       typedef vector<MapCoordinate> Path;

    protected:
       int MAXIMUM_PATH_LENGTH;
       GameMap* tempsMarked;
       Path *_path;
       Vehicle* _veh;
       GameMap* gamemap;


       //! returns the movement cost for the unit to travel from x1/y1 to x2/y2
       virtual int getMoveCost ( int x1, int y1, int x2, int y2, const Vehicle* vehicle );
    public:
       AStar ( GameMap* actmap, Vehicle* veh );

       //! A hexagonal Coordinate. This structure is used instead of MapCoordinate to reduce the amount of modifications to Amits path finding code.
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
           bool operator< ( const Node& a ) const;
           bool operator> ( const Node& a ) const;
       };

       int dist( HexCoord a, HexCoord b );

       typedef std::vector<Node> Container;
	   std::greater<Node> comp;

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
      
       virtual ~AStar ( );
 };


 //! finding a path for unit veh to position x, y on map actmap.
extern void findPath( GameMap* actmap, AStar::Path& path, Vehicle* veh, int x, int y );



//! A 3D path finding algorithm, based on the 2D algorithm by Amit J. Patel
class AStar3D {
    public:
       typedef float DistanceType;
       static const DistanceType longestPath;
       class OperationLimiter {
           public:
              virtual bool allowHeightChange() = 0;
              virtual bool allowMovement() = 0;
              virtual bool allowLeavingContainer() = 0;
              virtual bool allowDocking() = 0;
              virtual ~OperationLimiter() {};
       };


       class PathPoint: public MapCoordinate3D {
             PathPoint() {};
          public:
             PathPoint ( const MapCoordinate3D& mc, int dist_, int enterHeight_, bool hasAttacked_ ) : MapCoordinate3D(mc), dist(dist_), enterHeight(enterHeight_), hasAttacked(hasAttacked_) {};
             // PathPoint ( const MapCoordinate3D& mc ) : MapCoordinate3D(mc), dist(-1), enterHeight(-1), hasAttacked(false) {};
             int getRealHeight() { if ( getNumericalHeight() != -1 ) return getNumericalHeight(); else return enterHeight; };
             MapCoordinate3D getRealPos() { MapCoordinate3D p; p.setnum(x,y, getRealHeight()); return p; };
             int dist;
             int enterHeight;
             bool hasAttacked;
             void write( tnstream& stream ) const;
             void read( tnstream& stream );
             static PathPoint newFromStream( tnstream& stream );
       };

       typedef vector<PathPoint> Path;
       struct Node {
           MapCoordinate3D h;        // location on the map, in hex coordinates
           AStar3D::DistanceType gval;        // g in A* represents how far we've already gone
           AStar3D::DistanceType hval;        // h in A* represents an estimate of how far is left
           bool canStop;
           bool hasAttacked;
           int enterHeight;
           Node(): gval(0), hval(0), canStop(false), enterHeight(-1), deleted(false) {}
           bool deleted;
           bool operator< ( const Node& b ) const;
       };

    protected:
       OperationLimiter* operationLimiter;
       int MAXIMUM_PATH_LENGTH;
       GameMap* tempsMarked;
       Path *_path;
       Vehicle* veh;
       GameMap* actmap;
       float vehicleSpeedFactor[8];
       float maxVehicleSpeedFactor;
       bool markTemps;
       WindMovement* wind;

       virtual DistanceType getMoveCost ( const MapCoordinate3D& start, const MapCoordinate3D& dest, const Vehicle* vehicle, bool& canStop, bool& hasAttacked );

       HexDirection* posDirs;
       int*          posHHops;
       int*          fieldAccess;

       HexDirection& getPosDir ( const MapCoordinate3D& pos ) { return posDirs [(pos.y * actmap->xsize + pos.x) * 9 + 1+pos.getNumericalHeight()]; };
       int& getPosHHop ( const MapCoordinate3D& pos )         { return posHHops[(pos.y * actmap->xsize + pos.x) * 9 + 1+pos.getNumericalHeight()]; };

       DistanceType dist( const MapCoordinate3D& a, const MapCoordinate3D& b );
       DistanceType dist( const MapCoordinate3D& a, const vector<MapCoordinate3D>& b );

    public:

       class Container: protected list<Node> {
             map<MapCoordinate3D, list<Node>::iterator> hMap;
          public:
             typedef list<Node> Parent;

             // Container() {};
             void add ( const Node& node ) { push_back ( node ); hMap[node.h] = --(Parent::end()); };
             bool update ( const Node& node );
             Node getFirst() { Node n = Parent::front() ; Parent::pop_front(); hMap.erase(n.h); return n; };
             bool empty() { hMap.clear(); return Parent::empty(); };

             typedef Parent::iterator iterator;
             iterator find( const MapCoordinate3D& pos ) { return hMap[pos]; };

             iterator begin() { return Parent::begin(); };
             iterator end() { return Parent::end(); };

       };

       //! the reachable fields
       Container visited;
       // vector<Node> visited;
    protected:

       
       bool get_first( Container& v, Node& n );

       void nodeVisited ( const Node& n, HexDirection direc, Container& open, int prevHeight = -10, int heightChangeDist = 0 );


    public:
       AStar3D ( GameMap* actmap, Vehicle* veh, bool markTemps_ = true, int maxDistance = maxint );


       //! the search can be restricted to certain operations
       void registerOperationLimiter( OperationLimiter* ol ) { operationLimiter = ol; };

       //! searches for a path from A to B and stores it in path
       void findPath( const MapCoordinate3D& A, const vector<MapCoordinate3D>& B, Path& path );

       //! searches for a path from the unit's current position to dest and stores it in path
       void findPath( Path& path, const MapCoordinate3D& dest );

       //! searches for a path from the units current position to one of the dest fields and stores it in path
       void findPath( Path& path, const vector<MapCoordinate3D>& dest );

       /** searches for all fields that are within the range of maxDist and marks them.
           On each field one bit for each level of height will be set.
           The Destructor removes all marks.
           \param path if non-null, all fields will be stored there
       */
       void findAllAccessibleFields ( vector<MapCoordinate3D>* path = NULL );

       //! returns the distance of the last found path, or -1 on any error
       int getDistance( );

       //! returns the number of turns that the unit will need to travel along the last found path
       int getTravelTime( );

       //! checks weather the field fld was among the visited fields during the last search
       const Node* fieldVisited ( const MapCoordinate3D& fld );

       int& getFieldAccess ( int x, int y );
       int& getFieldAccess ( const MapCoordinate& mc );

       virtual ~AStar3D ( );
 };

#endif
