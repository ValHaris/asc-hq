//     $Id: loaders.h,v 1.6 2000-08-11 12:24:04 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
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

#ifndef loaders_h
#define loaders_h


#include "sgstream.h"
#include "spfst.h"

extern   char*         cloaderrormsg[] ; 
extern ticons icons;
extern word fileterminator;

extern const char* savegameextension;
extern const char* mapextension;
extern const char* tournamentextension;



extern pguiicon loadguiicon(char *       name);

extern void  savemap(char *       name,
                     char *       description);

extern void  loadmap(char *       name );

extern void  savegame(char *       name,
                      char *       description);

extern void  loadgame(char *       name );

extern void  loadreplay( pmemorystreambuf streambuf );
extern void  savereplay( int num );



extern void  loadallbuildingtypes(void);

extern void  loadallvehicletypes(void);

extern void  loadallterraintypes(void);

extern void  loadalltechnologies(void);

extern void  loadstreets(void);

extern void  loadicons(void);

extern char * loaderrormsg(byte         b);

extern void loadallobjecttypes ( void );

extern void  savecampaignrecoveryinformation(char*        name,
                                             word         id);

extern void         erasemap( tmap* spfld = actmap );
extern void         erasemap_unchained( tmap* spfld = actmap );



const int actsavegameversion  = 0xff33;
const int minsavegameversion  = 0xff31;
const int actmapversion       = 0xfe26;
const int minmapversion       = 0xfe24;
const int actnetworkversion   = 0x0006;
const int minnetworkversion   = 0x0004;
const int actreplayversion    = 0x0001;
const int minreplayversion    = 0x0001;
  


class tinvalidid : public terror {
             public:
               char msg[200];
               char orgmsg[200];
               int id;
               tinvalidid ( char* s, int iid ) ;
            };


class  tspfldloaders {
       public:
           pnstream        stream;
           tmap*     spfld;


           void            writebuilding  ( pbuilding bld );
           void            readbuilding ( pbuilding &bld );

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

           void            writetechnologies ( void );
           void            readtechnologies  ( void );

           virtual void    initmap ( void ) = 0;

           void            writemap ( void );
           void            readmap  ( void );
           void            freespfld( void );

         //  void            setcachingarrays ( void );
         //  void            freecachingarrays ( void );

           void            writefields ( void );
           void            readfields  ( void );

           void            writemessages ( void );
           void            writemessagelist( pmessagelist lst ); 
           void            readmessages ( void );
           void            readmessagelist( pmessagelist* lst ); 

           void            writereplayinfo ( void );
           void            readreplayinfo ( void );

           void            chainitems ( void );
           void            setplayerexistencies ( void );
           virtual ~tspfldloaders();
           tspfldloaders ( void );
};

class  tmaploaders : public tspfldloaders {
           pmap oldmap;
           void            initmap ( void );
         public:
           int             loadmap ( char* name );
           int             savemap ( char* name, char* description );
           tmaploaders (void ) { oldmap = NULL; };
           ~tmaploaders();
};



class  tgameloaders : public tspfldloaders {
        protected:
           void            initmap ( void );
};

class tnetworkloaders : public tgameloaders {
        public:
           int             loadnwgame ( pnstream strm );
           int             savenwgame ( pnstream strm , char* description );
           void            checkcrcs ( void );
};

class tsavegameloaders : public tgameloaders {
        public:
           int             loadgame ( char* name );
           int             savegame ( char* name, char* description );
};


class treplayloaders : public tspfldloaders {
           void            initmap ( void );
         public:
           void            loadreplay ( pmemorystreambuf streambuf );
           void            savereplay ( int num );
};


extern int validatemapfile ( char* s );       //
extern int validatesavfile ( char* s );       // result:   0  file invalid 
extern int validateemlfile ( char* s );       //         > 0  file valid



class tspeedcrccheck {
              char* strng;
              int strnglen;


              pobjectcontainercrcs list;

              dynamic_array<int> bdt;              // 0: noch nicht ÅberprÅft     1 : in ordnung;   2 : UNGöLTIG;   4 : in Liste nicht vorhanden
              dynamic_array<int> fzt; 
              dynamic_array<int> bld; 
              dynamic_array<int> tec; 
              dynamic_array<int> obj;

              void appendstring ( char* s, char* d, int id, int mode  );  // mode  0 : note included     1 : error crc check failed    2 : error missing crc      3 : error wrong crc
              int status;

              void additemtolist ( pcrcblock lst, int id, int crc );

              int  checkunit     ( pvehicletype f,     int add = 1 );
              int  checkbuilding ( pbuildingtype b, int add = 1 );
              int  checktech     ( ptechnology t,      int add = 1 );
              int  checkobj      ( pobjecttype o,   int add = 1 ); 
              int  checkterrain  ( pterraintype b,    int add = 1 );

           public:
              // tspeedcrccheck ( pobjectcontainercrcs crclist, tcachedtypes* cache );
              tspeedcrccheck ( pobjectcontainercrcs crclist );


              int  checkunit2     ( pvehicletype f,      int add = 1  );
              int  checkbuilding2 ( pbuildingtype b,  int add = 1  );
              int  checktech2     ( ptechnology t,       int add = 1  );
              int  checkobj2      ( pobjecttype o,    int add = 1  ); 
              int  checkterrain2  ( pterraintype b,     int add = 1  );

              int getstatus       ( void );
          };




#endif
