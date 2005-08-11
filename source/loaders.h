/*! \file loaders.h
    \brief procedure for loading and writing savegames, maps etc.

    IO for basic types like vehicletype, buildingtype etc which are also used by the small editors are found in sgstream

*/

//     $Id: loaders.h,v 1.34 2005-08-11 19:10:32 mbickel Exp $

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
extern const int fileterminator;

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



extern tmap*  loadreplay( pmemorystreambuf streambuf );

//! writes all replay relevant map information of player num to the replay variable of #actmap
extern void  savereplay( int num );


extern void  loadicons(void);

extern void  savecampaignrecoveryinformation( const ASCString& filename, int id);

const int actsavegameversion  = 0xff43;
const int minsavegameversion  = 0xff31;
const int actmapversion       = 0xfe31;
const int minmapversion       = 0xfe24;
const int actnetworkversion   = 0x0016;
const int minnetworkversion   = 0x0004;
const int actreplayversion    = 0x0003;
const int minreplayversion    = 0x0001;



class  tspfldloaders {
       public:
           pnstream        stream;
           tmap*     spfld;

           void            writeevent  ( pevent event );

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
