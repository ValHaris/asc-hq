/***************************************************************************
                          research.h  -  description
                             -------------------
    begin                : Tue Jan 23 2001
    copyright            : (C) 2001 by Martin Bickel
    email                : bickel@asc-hq.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef research_h_included
 #define research_h_included

 #include <string.h>
 #include <map>

 #include "basestreaminterface.h"
 #include "tpascal.inc"
 #include "pointers.h"

 const int researchableWeaponImprovements = 8;

 class tresearchdatachange {
    public:
      word         weapons[researchableWeaponImprovements];   /*  Basis 1024  */
      word         armor;         /*  Basis 1024  */
      unsigned char         dummy[20+(12-researchableWeaponImprovements)*2];
      tresearchdatachange ( void ) {
         for ( int i = 0; i< researchableWeaponImprovements; i++ )
            weapons[i] = 1024;
         armor = 1024;
         memset ( dummy, 0, sizeof(dummy ));
      };
 };


 class  ttechnology {
   public:
     void*      icon;
     char*        infotext;
     int          id;
     int          researchpoints;
     char*        name;

     tresearchdatachange unitimprovement;

     char      requireevent;

     union {
       ptechnology  requiretechnology[6];
       int      requiretechnologyid[6];
     };

     int          techlevelget;  // sobald dieser technologylevel erreicht ist, ist die Technologie automatisch verf?gbar
     char* pictfilename;
     int lvl;     // wird nur im Spiel benoetigt: "Level" der benoetigten Techologie. Gibt an, wieviele Basistechnologien insgesamt benoetogt werden.
     int techlevelset;
     int dummy[7];
     int  getlvl( void );
 };




 class tresearch {

     typedef map<int, ptechnology> DevelopedTechnologies;
     DevelopedTechnologies developedTechnologies;

     pmap map;
     int player;

     bool __loader_techsAvail;

   public:
     int                     progress;
     ptechnology             activetechnology;
     tresearchdatachange     unitimprovement;
     int                     techlevel;

     bool technologyresearched ( int id );
     int vehicletypeavailable ( const pvehicletype fztyp );
     int vehicleclassavailable ( const pvehicletype fztyp , int classnm );

     void read ( tnstream& stream );
     void write ( tnstream& stream );

     void read_struct ( tnstream& stream );
     void write_struct ( tnstream& stream );

     void read_techs ( tnstream& stream );
     void write_techs ( tnstream& stream );

     void chainToMap ( pmap _map, int _player ) { map = _map; player = _player; };

     void settechlevel ( int _techlevel );

     //! Move the technology that is currently being reseached to the list of discovered technologies
     void addtechnology ( void );

     void addanytechnology ( const ptechnology tech );

     //! just an very ugly hack to keep the replay compatible with older versions; called from treplayloaders :: savereplay
     void ____setDevTechToNULL ( )  { developedTechnologies.clear(); };

     tresearch ( );
     ~tresearch ();
 };

#endif