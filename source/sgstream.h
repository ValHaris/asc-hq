/*! \file sgstream.h
    \brief The IO for many basic classes and structurs of ACS
  
*/

//     $Id: sgstream.h,v 1.17 2001-01-28 14:04:19 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.16  2001/01/21 16:37:20  mbickel
//      Moved replay code to own file ( replay.cpp )
//      Fixed compile problems done by cleanup
//
//     Revision 1.15  2001/01/21 12:48:36  mbickel
//      Some cleanup and documentation
//
//     Revision 1.14  2001/01/04 15:14:07  mbickel
//      configure now checks for libSDL_image
//      AI only conquers building that cannot be conquered back immediately
//      tfindfile now returns strings instead of char*
//
//     Revision 1.13  2000/11/26 22:18:56  mbickel
//      Added command line parameters for setting the verbosity
//      Increased verbose output
//
//     Revision 1.12  2000/11/21 20:27:07  mbickel
//      Fixed crash in tsearchfields (used by object construction for example)
//      AI improvements
//      configure.in: added some debug output
//                    fixed broken check for libbz2
//
//     Revision 1.11  2000/10/17 12:12:23  mbickel
//      Improved vehicletype loading/saving routines
//      documented some global variables
//
//     Revision 1.10  2000/09/01 15:47:50  mbickel
//      Added Astar path finding code
//      Fixed: viewid crashed
//      Fixed display error in ynswitch ( krkr.cpp )
//
//     Revision 1.9  2000/08/12 09:17:34  gulliver
//     *** empty log message ***
//
//     Revision 1.8  2000/08/02 15:53:04  mbickel
//      New unit set definition files
//      demount accepts now more than one container file
//      Unitset information dialog added
//
//     Revision 1.7  2000/08/01 10:39:17  mbickel
//      Updated documentation
//      Refined configuration file handling
//
//     Revision 1.6  2000/07/31 18:02:54  mbickel
//      New configuration file handling
//      ASC searches its data files in all directories specified in ascrc
//      Renamed all tools so they begin with asc
//
//     Revision 1.5  2000/07/29 14:54:46  mbickel
//      plain text configuration file implemented
//
//     Revision 1.4  2000/05/23 20:40:49  mbickel
//      Removed boolean type
//
//     Revision 1.3  2000/03/29 09:58:49  mbickel
//      Improved memory handling for DOS version
//      Many small changes I can't remember ;-)
//
//     Revision 1.2  1999/11/16 03:42:30  tmwilson
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


#ifndef sgstream_h
#define sgstream_h

#include <vector>

#include "basestrm.h"
#include "typen.h"


extern pvehicletype loadvehicletype( const char *       name);
extern pvehicletype loadvehicletype( tnstream& stream );
extern void writevehicle( pvehicletype ft, tnstream& stream );

extern pterraintype loadterraintype( const char *       name);
extern pterraintype loadterraintype( pnstream stream );
extern void writeterrain ( pterraintype bdt, pnstream stream );

extern pbuildingtype loadbuildingtype( const char *       name);
extern pbuildingtype loadbuildingtype( pnstream stream );
extern void writebuildingtype ( pbuildingtype bld, pnstream stream );

extern ptechnology loadtechnology( const char *       name);
extern ptechnology loadtechnology( pnstream stream );
extern void writetechnology ( ptechnology tech, pnstream stream );

extern pobjecttype   loadobjecttype( const char *       name);
extern pobjecttype   loadobjecttype( pnstream stream );
void writeobject ( pobjecttype object, pnstream stream, int compressed = 0 );

extern void generatedirecpict ( void* orgpict, void* direcpict );
extern pquickview generateaveragecol ( TerrainType::Weather* bdn );


extern pobjecttype streetobjectcontainer ;
extern pobjecttype pathobject;
extern pobjecttype railroadobject ;
extern pobjecttype eisbrecherobject;
extern pobjecttype fahrspurobject;

class t_carefor_containerstream {       // not necessary any more, now that we have the containercollector
       public:
         t_carefor_containerstream ( void );
       };

extern void logtofile ( char* strng, ... );


extern void loadpalette ( void );
extern int readgameoptions ( const char* filename = NULL );
extern int writegameoptions ( void );
extern char* getConfigFileName ( char* buffer );
extern void loadguipictures( void );
extern void checkFileLoadability ( const char* filename );
extern void initFileIO ( const char* configFileName );





class SingleUnitSet {
      public:
         struct IdRange {
                 int from;
                 int to;
                };


         class TranslationTable {
                  public:
                     std::vector<IdRange> translation;
                     std::string name;
                     void parseString ( const char* s );
               };


         int active;
          std::string name;
          std::string maintainer;
          std::string information;

         std::vector<IdRange> ids;
         std::vector<TranslationTable*> transtab;
         bool filterBuildings;

         SingleUnitSet ( void ) : active ( 1 ), filterBuildings ( true ) {};
         int isMember ( int id );
         void read ( pnstream stream );
         void parseIDs ( const char* s );

     };

extern std::vector<SingleUnitSet*> unitSets;

extern void loadUnitSets ( void );


//! displays a message on the log. If msgVerbosity is greater than the game verbosity, the message is NOT printed
extern void displayLogMessage ( int msgVerbosity, char* message, ... );

#endif
