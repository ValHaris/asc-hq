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

 #include "basestreaminterface.h"
 #include "basictypes.h"
 #include "typen.h"


class Research;
class Technology;
class Building;
class Vehicle;

class TechDependency: public LoadableItemType {
     typedef vector<IntRange> RequiredTechnologies;
     RequiredTechnologies blockingTechnologies;
     
     RequiredTechnologies requiredTechnologies;
     bool         requireAllListedTechnologies;
     //! if one of these technologies has been researched, this tech will be never be researchable. This allows exclusive technology branches
   public:
     TechDependency(){ requireAllListedTechnologies = true; };
     bool available( const Research& research ) const;
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
     typedef vector<ASCString> RequiredTechAdapter;
     RequiredTechAdapter requiredTechAdapter;
     bool         requireAllListedTechAdapter;
   public:
     TechAdapterDependency();
     bool available( const Research& research ) const;

     void read ( tnstream& stream );
     void write ( tnstream& stream ) const;
     void runTextIO ( PropertyContainer& pc, const ASCString& defaultTechAdapter = "");

     void writeInvertTreeOutput ( const ASCString& tech, tnstream& stream, const vector<IntRange>* onlyWithBaseTechs = NULL ) const;
};


 class  Technology: public LoadableItemType {
   public:
     Technology();

     void*        icon;
     ASCString    infotext;
     int          id;
     int          researchpoints;
     ASCString    name;
     int          techlevel;
     int          relatedUnitID;

     bool         requireEvent;

     TechDependency techDependency;


     typedef vector<IntRange> BlockingOtherTechnologies;
     //! if this Technology has been researched, it will not be possible to research the technologies given here. Only available for root technologies!
     BlockingOtherTechnologies blockingOtherTechnologies;


     void read ( tnstream& stream );
     void write ( tnstream& stream ) const;
     void runTextIO ( PropertyContainer& pc );
};




 class Research {

     pmap map;
     int player;

     int ___loadActiveTech;
     bool ___oldVersionLoader;
   #ifdef karteneditor
   public:
   #endif   

     typedef std::map<ASCString,bool> TriggeredTechAdapter;
     TriggeredTechAdapter triggeredTechAdapter;


     vector<ASCString> predefinedTechAdapter;
   public:

     vector<int> developedTechnologies;

     ASCString listTriggeredTechAdapter() const;

     bool techResearched ( int id ) const;

     enum AvailabilityStatus { researched, available, unavailable };

     int  progress;
     const Technology* activetechnology;

     bool vehicletypeavailable ( const Vehicletype* fztyp );

     void read ( tnstream& stream );
     void write ( tnstream& stream );

     void read_struct ( tnstream& stream );
     void read_techs ( tnstream& stream );

     void evalTechAdapter();
     bool techAdapterAvail( const ASCString& ta ) const;

     void settechlevel ( int techlevel );

     void chainToMap ( pmap _map, int _player ) { map = _map; player = _player; };

     //! Move the technology that is currently being reseached to the list of discovered technologies
     void addtechnology();

     void addanytechnology ( const Technology* tech );

     AvailabilityStatus techAvailable ( const Technology* tech );

     /** is used by the chooseTechnology dialog: the first time no techs are available this variable is still true,
         so the dialog shows "now techs avail". THen it sets techAvail to false, preventing the same message at the
         beginning of each turn */
     bool techsAvail;

     Research ( );
     ~Research ();
 };

//! Calculates the resources that are needed to research the given number of research
extern Resources returnResourcenUseForResearch ( const Building* bld, int research );

#endif

