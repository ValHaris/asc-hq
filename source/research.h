/***************************************************************************
                          research.h  -  description
                             -------------------
    begin                : Tue Jan 23 2001
    copyright            : (C) 2001 by Martin Bickel
    email                : bickel@asc-hq.org
 ***************************************************************************/

/*! \file research.h
    \brief Interface for everything related to research
*/


/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef researchH
 #define researchH

 #include <cstring>
 #include <map>

 #include "loki/Functor.h"
 #include "loki/Typelist.h"

 #include "basestreaminterface.h"
 #include "basictypes.h"
 #include "typen.h"


class Research;
class Technology;
class Building;
class Vehicle;
class VehicleType;
class GameMap;

enum ResearchAvailabilityStatus { Researched, Available, UnavailableNow, NeverAvailable };


class TechDependency: public LoadableItemType {
     typedef vector<IntRange> RequiredTechnologies;
     //! if one of these technologies has been researched, this tech will be never be researchable. This allows exclusive technology branches
     RequiredTechnologies blockingTechnologies;
     
     RequiredTechnologies requiredTechnologies;
     bool         requireAllListedTechnologies;
     
     static bool eventually_available_single( const Research& res, list<const Technology*>* dependencies, list<int>& stack, int id );
   public:
     TechDependency(){ requireAllListedTechnologies = true; };

     bool available( const Research& research ) const;
     typedef Loki::Functor<bool, LOKI_TYPELIST_1(int) > CheckTechAvailabilityFunctor;
     ResearchAvailabilityStatus available( CheckTechAvailabilityFunctor checkTechAvailability ) const;

     /** this will recursively scan through the tech tree to check if this item will be available sometime.
     \param dependencies if not NULL, the technologies which have to be developed prior to this one will be added here
     */
     bool eventually_available( const Research& res, list<const Technology*>* dependencies ) const;
     bool eventually_available( const Research& res, list<const Technology*>* dependencies, list<int>& stack ) const;


     ASCString showDebug( const Research& research ) const;
     
     void read ( tnstream& stream );
     void write ( tnstream& stream ) const;
     void runTextIO ( PropertyContainer& pc);
     bool empty() const { return requiredTechnologies.size() == 0; } ;

     //! outputs the dependencies in text format for processing by GraphViz
     void writeTreeOutput ( const ASCString& sourceTechName, tnstream& stream, bool reduce ) const;

     void writeInvertTreeOutput ( const Technology* tech,   tnstream& stream, vector<int>& history, vector<pair<int,int> >& blockedPrintList, const vector<IntRange>* onlyWithBaseTechs = NULL ) const;

     void writeInvertTreeOutput ( const ASCString techName, tnstream& stream, vector<int>& history, vector<pair<int,int> >& blockedPrintList, const vector<IntRange>* onlyWithBaseTechs = NULL ) const;

     int findInheritanceLevel( int id, vector<int>& stack, const ASCString& sourceTechName ) const;
};

class TechAdapter: public LoadableItemType {
     ASCString name;
   public:
     TechDependency techDependency;

     TechAdapter();
     bool available( const Research& research ) const;
     const ASCString& getName() const { return name; } ;

     void read ( tnstream& stream );
     void write ( tnstream& stream ) const;
     void runTextIO ( PropertyContainer& pc);
};

class TechAdapterDependency {
   public:
     typedef vector<ASCString> RequiredTechAdapter;
   private:  
     RequiredTechAdapter requiredTechAdapter;
     bool         requireAllListedTechAdapter;
   public:
     TechAdapterDependency();
     bool available( const Research& research ) const;
     ASCString showDebug( const Research& research ) const;

     void read ( tnstream& stream );
     void write ( tnstream& stream ) const;
     void runTextIO ( PropertyContainer& pc, const ASCString& defaultTechAdapter = "");

     void writeInvertTreeOutput ( const ASCString& tech, tnstream& stream, const vector<IntRange>* onlyWithBaseTechs = NULL ) const;
     
     const RequiredTechAdapter& listAdapters() const { return requiredTechAdapter; };
};


 class  Technology: public LoadableItemType {
   public:
     Technology();

     void*        icon;
     ASCString    infotext;

     int          id;
     //! when loading a file and these IDs are encountered, this object will be used.
     vector<int> secondaryIDs;


     int          researchpoints;

     ASCString    name;
     int          techlevel;
     int          relatedUnitID;

     bool         requireEvent;

     TechDependency techDependency;


     typedef vector<IntRange> BlockingOtherTechnologies;
     //! if this Technology has been researched, it will not be possible to research the technologies given here. Only available for root technologies!
     BlockingOtherTechnologies blockingOtherTechnologies;

     bool eventually_available( const Research& res, list<const Technology*>* dependencies ) const;
     bool eventually_available( const Research& res, list<const Technology*>* dependencies, list<int>& stack ) const;
     

     void read ( tnstream& stream );
     void write ( tnstream& stream ) const;
     void runTextIO ( PropertyContainer& pc );
};




 class Research {
     friend class DirectResearchCommand;
     GameMap* map;
     int player;

     int ___loadActiveTech;
     bool ___oldVersionLoader;

     int multiplier;
     
   #ifdef karteneditor
   public:
   #endif   

     typedef std::set<ASCString> TriggeredTechAdapter;
     TriggeredTechAdapter triggeredTechAdapter;


     vector<ASCString> predefinedTechAdapter;
   public:

     vector<int> developedTechnologies;

     ASCString listTriggeredTechAdapter() const;

     bool techResearched ( int id ) const;

     int  progress;
     //! the technology that is currently being researched
     const Technology* activetechnology;

     //! the technology that is defined as long-term goal
     const Technology* goal;

     void read ( tnstream& stream );
     void write ( tnstream& stream );

     void read_struct ( tnstream& stream, bool merge = false );
     void read_techs ( tnstream& stream, bool merge = false );

     /** checks for TechAdapters whose preconditions are now satisfied.
         These TechAdapters will be activated.
         \returns a list of TechAdapters that were activated 
      */
     vector<ASCString> evalTechAdapter();
     bool techAdapterAvail( const ASCString& ta ) const;

     void settechlevel ( int techlevel );

     void chainToMap ( GameMap* _map, int _player ) { map = _map; player = _player; };

     /** adds the technology to the list of available technologies
         \returns a list of TechAdapters that were activated due to this new technology 
     */
     vector<ASCString> addanytechnology ( const Technology* tech );

     ResearchAvailabilityStatus techAvailable ( const Technology* tech ) const;

     bool isBlocked( const Technology* tech ) const;

     /** is used by the chooseTechnology dialog: the first time no techs are available this variable is still true,
         so the dialog shows "no techs avail". THen it sets techAvail to false, preventing the same message at the
         beginning of each turn */
     bool techsAvail;


     int getResearchPerTurn() const;
     int currentTechAvailableIn() const;

     void setMultiplier( int m ) { multiplier = m; };
     int getMultiplier() { return multiplier; };
     
     const vector<ASCString>& getPredefinedTechAdapter() { return predefinedTechAdapter; };
     void setPredefinedTechAdapter( const set<ASCString>& adapter );
     void addPredefinedTechAdapter( const ASCString& techAdapter );
     
     Research ( );
     void clear();
     ~Research ();
 };

 class ContainerBase;
 
//! Calculates the resources that are needed to research the given number of research
extern Resources returnResourcenUseForResearch ( const ContainerBase* bld, int research );
extern Resources returnResourcenUseForResearch ( const ContainerBase* bld );

#endif

