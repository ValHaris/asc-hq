/*! \file loaders.h
    \brief procedure for loading and writing savegames, maps etc.

    IO for basic types like vehicletype, buildingtype etc which are also used by the small editors are found in sgstream

*/

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

#include <sigc++/sigc++.h>

#include "libs/loki/Functor.h"

#include "ascstring.h"
#include "sgstream.h"
#include "messages.h"



extern const int fileterminator;

extern const char* savegameextension;
extern const char* mapextension;
extern const char* tournamentextension;



//! saves the map located in #actmap to the map file name
extern void  savemap( const char *       name, GameMap* gamemap );

//! saves the game located in #actmap to the savegame file name
extern void  savegame( const ASCString& name );


typedef Loki::Functor<GameMap*, TYPELIST_1(const ASCString&) > MapLoadingFunction;
extern GameMap* mapLoadingExceptionChecker( const ASCString& filename, MapLoadingFunction loader );



extern GameMap*  loadreplay( pmemorystreambuf streambuf );

//! writes all replay relevant map information of player num to the replay variable of #actmap
extern void  savereplay( GameMap* gamemap, int num );


class  tspfldloaders {
       public:
           pnstream        stream;
           GameMap*     spfld;

           static SigC::Signal1<void,GameMap*> mapLoaded; 

           void            readoldevents     ( void );


           void            writedissections ( void );
           void            readdissections ( void );

           void            readLegacyNetwork  ( void );

           virtual void    initmap ( void ) = 0;

           void            writemap ( void );
           void            readmap  ( void );

           void            writefields ( void );
           void            readfields  ( void );

           void            writemessages ( void );
           void            writemessagelist( MessagePntrContainer& lst );
           void            readmessages ( void );
           void            readmessagelist( MessagePntrContainer& lst );

           void            chainitems ( GameMap* actmap );
           void            setplayerexistencies ( void );
           virtual ~tspfldloaders();
           tspfldloaders ( void );
};



class  tmaploaders : public tspfldloaders {
           void            initmap ( void );
           GameMap*           _loadmap ( const ASCString& name );
         public:
           static GameMap* loadmap ( const ASCString& name );
           
           
           int             savemap ( const ASCString& name, GameMap* gamemap );
           tmaploaders (void ) {};
};



class  tgameloaders : public tspfldloaders {
        protected:
           void            initmap ( void );
           void            readAI( );
           void            writeAI( );
};

class tnetworkloaders : public tgameloaders {
        public:
           GameMap*           loadnwgame ( pnstream strm );
           int             savenwgame ( pnstream strm, const GameMap* gamemap );
};

class tsavegameloaders : public tgameloaders {
        public:
           GameMap*        loadgame ( pnstream strm );
           static GameMap*  loadGameFromFile ( const ASCString& name );

           void            savegame ( pnstream strm, GameMap* gamemap, bool writeReplays = true );

           void            savegame ( const ASCString& name );
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
extern bool validatemapfile ( const ASCString& filename );

//! checks, whether filename is a valid savegame
extern bool validatesavfile ( const ASCString& filename );

//! checks, whether filename is a valid email game
extern bool validateemlfile ( const ASCString& filename );


struct MapConinuationInfo {
   ASCString title;
   ASCString codeword;
   ASCString filename;
};

extern MapConinuationInfo findNextCampaignMap( int id = -1 );


#endif
