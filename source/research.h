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
 #include "pointers.h"
 #include "typen.h"


class Research;

class TechDependency: public LoadableItemType {
     typedef vector<IntRange> RequiredTechnologies;
     RequiredTechnologies requiredTechnologies;
     bool         requireAllListedTechnologies;
   public:
     TechDependency(){ requireAllListedTechnologies = true; };
     bool available( const Research& research ) const;
     void read ( tnstream& stream );
     void write ( tnstream& stream ) const;
     void runTextIO ( PropertyContainer& pc);

};

class TechAdapter: public LoadableItemType {
     ASCString name;
     TechDependency techDependency;

   public:
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

     //! if one of these technologies has been researched, this tech will be never be researchable. This allows exclusive technology branches
     vector<IntRange> blockingTechnologies;


//     static Technology* newFromStream ( tnstream& stream );

     void read ( tnstream& stream );
     void write ( tnstream& stream ) const;
     void runTextIO ( PropertyContainer& pc );
};




 class Research {

     pmap map;
     int player;

     int ___loadActiveTech;
     bool ___oldVersionLoader;

     typedef std::map<ASCString,bool> TriggeredTechAdapter;
     TriggeredTechAdapter triggeredTechAdapter;

   #ifdef karteneditor
   public:
   #endif 
     vector<int> developedTechnologies;


   public:
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

     void initchoosentechnology();

     /** is used by the chooseTechnology dialog: the first time no techs are available this variable is still true,
         so the dialog shows "now techs avail". THen it sets techAvail to false, preventing the same message at the
         beginning of each turn */
     bool techsAvail;

     Research ( );
     ~Research ();
 };

//! Calculates the resources that are needed to research the given number of research
extern Resources returnResourcenUseForResearch ( const pbuilding bld, int research );

#endif
