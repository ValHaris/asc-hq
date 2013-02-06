/*! \file astar2.h
    \brief Interface for the A* pathfinding algorithm
*/


#ifndef astar2H
 #define astar2H

 #include <vector>
 #include "mapalgorithms.h"
 #include "gamemap.h"

 #include <boost/unordered_map.hpp>

 enum HexDirection { DirN, DirNE, DirSE, DirS, DirSW, DirNW, DirNone };


//! A 3D path finding algorithm, based on the 2D algorithm by Amit J. Patel
class AStar3D {
    public:
       typedef float DistanceType;
       static const DistanceType longestPath = 1e9;
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

       typedef deque<PathPoint> Path;

       struct hash_MapCoordinate3D {
          size_t operator()(const MapCoordinate3D &h) const{
             return static_cast<size_t>(h.x) ^ (static_cast<size_t>(h.y) << 12) ^ (static_cast<size_t>(h.getNumericalHeight())  << 24);
          }
       };

       struct hash_MapCoordinate {
          size_t operator()(const MapCoordinate &h) const{
             return static_cast<size_t>(h.x) ^ (static_cast<size_t>(h.y) << 16);
          }
       };

       struct Node {
          const Node* previous;
          MapCoordinate3D h;        // location on the map, in hex coordinates
          AStar3D::DistanceType gval;        // g in A* represents how far we've already gone
          AStar3D::DistanceType hval;        // h in A* represents an estimate of how far is left
          int enterHeight;
          bool canStop;
          bool hasAttacked;
          bool operator< ( const Node& b ) const;
          bool operator> ( const Node& b ) const;
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

       virtual DistanceType getMoveCost ( const MapCoordinate3D& start, const MapCoordinate3D& dest, const Vehicle* vehicle, bool& hasAttacked );

       typedef boost::unordered_map<MapCoordinate, int, hash_MapCoordinate> fieldAccessType;
       fieldAccessType fieldAccess;

       DistanceType dist( const MapCoordinate3D& a, const MapCoordinate3D& b );
       DistanceType dist( const MapCoordinate3D& a, const vector<MapCoordinate3D>& b );

    public:
       //! the reachable fields
       // pointers to nodes in this container need to stay valid when the
       // container grows, so we can't use a vector for this.
       class VisitedContainer: protected deque<Node> {
             typedef boost::unordered_map<MapCoordinate3D, Node*, hash_MapCoordinate3D> hMapType;
             hMapType hMap;
          public:
             typedef deque<Node> Parent;
             typedef Parent::iterator iterator;
             const Node* add ( const Node& n) {
                push_back(n);
                hMap[n.h] = &Parent::back();
                return &Parent::back();
             };
             const Node* find ( const MapCoordinate3D& pos ) {
                hMapType::iterator i = hMap.find(pos); 
                if (i == hMap.end()) return NULL;
                else return i->second;
             }

             iterator begin() { return Parent::begin(); };
             iterator end() { return Parent::end(); };
             const Node& back() { return Parent::back(); };
      };
       VisitedContainer visited;
    protected:

       int initNode ( Node& newN,
                      const Node* oldN_ptr,
                      const MapCoordinate3D& newpos,
                      const vector<MapCoordinate3D>& B,
                      bool disableAttack=false,
                      bool enter=false,
                      bool dock=false);

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

       //! construct a path from a pointer to a visited node, return false if pointer is NULL, else true
       bool constructPath( Path& path, const Node* n);

       //! construct a path from a pointer to a visited node; return false if position doesn't exist, else true
       inline bool constructPath( Path& path, const MapCoordinate3D& pos) { return constructPath ( path, visited.find (pos) ); }

       //! returns the distance of the last found path, or -1 on any error
       int getDistance( );

       //! returns the number of turns that the unit will need to travel along the last found path
       int getTravelTime( );

       int getFieldAccess ( int x, int y );
       int getFieldAccess ( const MapCoordinate& mc );

       //! for debugging: dumps the contents of the visited node to stdout
       void dumpVisited();

       virtual ~AStar3D ( );
 };

#endif
