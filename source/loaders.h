/*! \file loaders.h
    \brief procedure for loading and writing savegames, maps etc.

    IO for basic types like vehicletype, buildingtype etc which are also used by the small editors are found in sgstream

*/

//     $Id: loaders.h,v 1.24 2003-04-23 18:31:10 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.23  2003/04/09 17:39:10  mbickel
//      Fixed: AI not moving units that can not be refuelled
//      Fixed: AI parameters not saved in .ASC files
//
//     Revision 1.22  2002/12/08 21:53:39  mbickel
//      Fixed: mining station produces twice the resources
//      Updated file version numbers
//
//     Revision 1.21  2002/09/19 20:20:05  mbickel
//      Cleanup and various bug fixes
//
//     Revision 1.20  2002/05/07 21:32:49  mbickel
//      Fixed crash in mapeditor
//      Fixed: conquering of allied buildings
//      Fixed: ambiguity in pulldown menu keys
//
//     Revision 1.19  2001/10/11 10:41:06  mbickel
//      Restructured platform fileio handling
//      Added map archival information to mapeditor
//
//     Revision 1.18  2001/07/28 11:19:12  mbickel
//      Updated weaponguide
//      moved item repository from spfst to itemrepository
//
//     Revision 1.17  2001/02/26 12:35:20  mbickel
//      Some major restructuing:
//       new message containers
//       events don't store pointers to units any more
//       tfield class overhauled
//
//     Revision 1.16  2001/02/11 11:39:39  mbickel
//      Some cleanup and documentation
//
//     Revision 1.15  2001/02/04 21:26:58  mbickel
//      The AI status is written to savegames -> new savegame revision
//      Lots of bug fixes
//
//     Revision 1.14  2001/02/01 22:48:45  mbickel
//      rewrote the storing of units and buildings
//      Fixed bugs in bi3 map importing routines
//      Fixed bugs in AI
//      Fixed bugs in mapeditor
//
//     Revision 1.13  2001/01/31 14:52:41  mbickel
//      Fixed crashes in BI3 map importing routines
//      Rewrote memory consistency checking
//      Fileselect dialog now uses ASCStrings
//
//     Revision 1.12  2001/01/23 21:05:19  mbickel
//      Speed up of AI
//      Lot of bugfixes in AI
//      Moved Research to own files (research.*)
//      Rewrote storing of developed technologies
//      Some cleanup and documentation
//
//     Revision 1.11  2000/11/08 19:31:10  mbickel
//      Rewrote IO for the tmap structure
//      Fixed crash when entering damaged building
//      Fixed crash in AI
//      Removed item CRCs
//
//     Revision 1.10  2000/10/11 14:26:43  mbickel
//      Modernized the internal structure of ASC:
//       - vehicles and buildings now derived from a common base class
//       - new resource class
//       - reorganized exceptions (errors.h)
//      Split some files:
//        typen -> typen, vehicletype, buildingtype, basecontainer
//        controls -> controls, viewcalculation
//        spfst -> spfst, mapalgorithm
//      bzlib is now statically linked and sources integrated
//
//     Revision 1.9  2000/09/07 15:49:44  mbickel
//      some cleanup and documentation
//
//     Revision 1.8  2000/09/02 15:36:50  mbickel
//      Some minor cleanup and documentation
//
//     Revision 1.7  2000/08/12 12:52:49  mbickel
//      Made DOS-Version compile and run again.
//
//     Revision 1.6  2000/08/11 12:24:04  mbickel
//      Fixed: no movement after refuelling unit
//      Restructured reading/writing of units
//
//     Revision 1.5  2000/08/03 13:12:17  mbickel
//      Fixed: on/off switching of generator vehicle produced endless amounts of energy
//      Repairing units now reduces their experience
//      negative attack- and defenseboni possible
//      changed attackformula
//
//     Revision 1.4  2000/05/25 11:07:44  mbickel
//      Added functions to check files for valid mail / savegame files.
//
//     Revision 1.3  2000/04/27 16:25:26  mbickel
//      Attack functions cleanup
//      New vehicle categories
//      Rewrote resource production in ASC resource mode
//      Improved mine system: several mines on a single field allowed
//      Added unitctrl.* : Interface for vehicle functions
//        currently movement and height change included
//      Changed timer to SDL_GetTicks
//
//     Revision 1.2  1999/11/16 03:41:59  tmwilson
//     	Added CVS keywords to most of the files.
//     	Started porting the code to Linux (ifdef'ing the DOS specific stuff)
//     	Wrote replacement routines for kbhit/getch for Linux
//     	Cleaned up parts of the code that gcc barfed on (char vs unsigned char)
//     	Added autoconf/automake capabilities
//     	Added files used by 'automake --gnu'
//
//
/*
    This file is part of Advanced Strategic Command; http://www.asc-hq.de
    Copyright (C) 1994-1999  Martin Bickel  and  Marc Schellenberger

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; see the file COPYING. If not, write to the
    Free Software Foundation, Inc., 59 Temple Place, Suite 330,
    Boston, MA  02111-1307  USA
*/

#ifndef loadersH
#define loadersH

#include "ascstring.h"
#include "sgstream.h"
#include "spfst.h"



extern ticons icons;
extern const word fileterminator;

extern const char* savegameextension;
extern const char* mapextension;
extern const char* tournamentextension;



extern pguiicon loadguiicon( const char *       name);


//! saves the map located in #actmap to the map file name
extern void  savemap( const char *       name );

//! loads the map from the file name to #actmap
extern void  loadmap( const char *       name );

//! saves the game located in #actmap to the savegame file name
extern void  savegame( const char *       name );

//! loads the game from the file name to  #actmap
extern void  loadgame( const char *       name );



extern void  loadreplay( pmemorystreambuf streambuf );

//! writes all replay relevant map information of player num to the replay variable of #actmap
extern void  savereplay( int num );


extern void  loadstreets(void);

extern void  loadicons(void);

extern void  savecampaignrecoveryinformation( const ASCString& filename, int id);

const int actsavegameversion  = 0xff37;
const int minsavegameversion  = 0xff31;
const int actmapversion       = 0xfe27;
const int minmapversion       = 0xfe24;
const int actnetworkversion   = 0x0009;
const int minnetworkversion   = 0x0004;
const int actreplayversion    = 0x0003;
const int minreplayversion    = 0x0001;



class  tspfldloaders {
       public:
           pnstream        stream;
           tmap*     spfld;

           void            writeevent  ( pevent event );
           void            readevent ( pevent& event1 );

           void            writeeventstocome ( void );
           void            readeventstocome  ( void );
           void            writeeventspassed ( void );
           void            readeventspassed  ( void );
           void            writeoldevents    ( void );
           void            readoldevents     ( void );


           void            writedissections ( void );
           void            readdissections ( void );

           void            writenetwork ( void );
           void            readnetwork  ( void );

           virtual void    initmap ( void ) = 0;

           void            writemap ( void );
           void            readmap  ( void );

           void            writefields ( void );
           void            readfields  ( void );

           void            writemessages ( void );
           void            writemessagelist( MessagePntrContainer& lst );
           void            readmessages ( void );
           void            readmessagelist( MessagePntrContainer& lst );

           void            chainitems ( pmap actmap );
           void            setplayerexistencies ( void );
           virtual ~tspfldloaders();
           tspfldloaders ( void );
};

class  tmaploaders : public tspfldloaders {
           pmap oldmap;
           void            initmap ( void );
         public:
           int             loadmap ( const char* name );
           int             savemap ( const ASCString& name );
           tmaploaders (void ) { oldmap = NULL; };
           ~tmaploaders();
};



class  tgameloaders : public tspfldloaders {
        protected:
           void            initmap ( void );
           void            readAI( );
           void            writeAI( );
};

class tnetworkloaders : public tgameloaders {
        public:
           int             loadnwgame ( pnstream strm );
           int             savenwgame ( pnstream strm );
};

class tsavegameloaders : public tgameloaders {
        public:
           tmap*           loadgame ( pnstream strm );
           void            savegame ( pnstream strm, pmap gamemap, bool writeReplays = true );

           int             loadgame ( const char* name );
           void            savegame ( const char* name );
};

/*
class treplayloaders : public tspfldloaders {
           void            initmap ( void );
         public:
           void            loadreplay ( pmemorystreambuf streambuf );
           void            savereplay ( int num );
};
*/

//! checks, whether filename is a valid map file
extern bool validatemapfile ( const char* filename );

//! checks, whether filename is a valid savegame
extern bool validatesavfile ( const char* filename );

//! checks, whether filename is a valid email game
extern bool validateemlfile ( const char* filename );


#endif
