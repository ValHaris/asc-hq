/*! \file sgstream.h
    \brief The IO for many basic classes and structurs of ACS
  
*/

//     $Id: sgstream.h,v 1.33 2005-04-02 13:57:06 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.32  2004/09/26 19:53:21  mbickel
//      Reversed merge of branch
//
//     Revision 1.30  2004/07/25 15:13:42  mbickel
//      Updated weapon guide
//
//     Revision 1.29  2004/05/23 12:54:28  mbickel
//      Updated campaign maps
//      improved tech tree generation
//
//     Revision 1.28  2004/05/11 20:22:33  mbickel
//      Readded research system to ASC
//
//     Revision 1.27  2003/02/13 00:56:07  mbickel
//      Updated weaponguide
//      ASC compiles with gcc again
//
//     Revision 1.26  2002/12/23 12:50:25  mbickel
//      Fixed AI crash
//      Minimap now working with partially defined graphic sets
//
//     Revision 1.25  2002/10/09 16:58:46  mbickel
//      Fixed to GrafikSet loading
//      New item filter for mapeditor
//
//     Revision 1.24  2002/03/25 18:48:15  mbickel
//      Applications can now specify which data checks to perform
//
//     Revision 1.23  2001/12/19 11:46:36  mbickel
//      Applied patches from Michael Moerz:
//       - 64bit cleanup of demount.cpp, mount.cpp
//       - removal of #ifdef converter and moved conveter specific functions
//         to independant lib
//
//     Revision 1.22  2001/08/06 20:54:43  mbickel
//      Fixed lots of crashes related to the new text files
//      Fixed delayed events
//      Fixed crash in terrin change event
//      Fixed visibility of mines
//      Fixed crashes in event loader
//
//     Revision 1.21  2001/08/02 18:50:43  mbickel
//      Corrected Error handling in Text parsers
//      Improved version information
//
//     Revision 1.20  2001/07/28 11:19:12  mbickel
//      Updated weaponguide
//      moved item repository from spfst to itemrepository
//
//     Revision 1.19  2001/07/27 21:13:35  mbickel
//      Added text based file formats
//      Terraintype and Objecttype restructured
//
//     Revision 1.18  2001/07/14 19:13:16  mbickel
//      Rewrote sound system
//      Moveing units make sounds
//      Added sound files to data
//
//     Revision 1.17  2001/01/28 14:04:19  mbickel
//      Some restructuring, documentation and cleanup
//      The resource network functions are now it their own files, the dashboard
//       as well
//      Updated the TODO list
//
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


#ifndef sgstreamH
#define sgstreamH

#include <vector>

#include "basestrm.h"
#include "typen.h"
#include "terraintype.h"
#include "objecttype.h"



extern void generatedirecpict ( void* orgpict, void* direcpict );
FieldQuickView* generateAverageCol ( void* image );



extern void loadpalette ( void );
extern int readgameoptions ( const char* filename = NULL );
extern int writegameoptions ( void );
extern char* getConfigFileName ( char* buffer );
extern void loadguipictures( void );
extern void checkFileLoadability ( const char* filename );

extern void initFileIO ( const ASCString& configFileName, int skipChecks = 0 );





class SingleUnitSet {
      public:

         enum Type { unit, building };

         class TranslationTable {
                  public:
                     std::vector<IntRange> translation;
                     std::string name;
                     void parseString ( const char* s );
               };


         int active;
         int ID;
         ASCString name;
         ASCString maintainer;
         ASCString information;

         std::vector<IntRange> unitIds;
         std::vector<IntRange> buildingIds;
         std::vector<TranslationTable*> transtab;
         bool filterBuildings;

         SingleUnitSet ( void ) : active ( 1 ), ID(0), filterBuildings ( true ) {};
         bool isMember ( int id, Type type );
         void read ( pnstream stream );
         std::vector<IntRange> parseIDs ( const char* s );

     };

extern std::vector<SingleUnitSet*> unitSets;

extern void loadUnitSets ( void );


//! displays a message on the log. If msgVerbosity is greater than the game verbosity, the message is NOT printed
extern void displayLogMessage ( int msgVerbosity, char* message, ... );
extern void displayLogMessage ( int msgVerbosity, const ASCString& message );

extern int dataVersion;
extern void checkDataVersion( );

extern void* generate_object_gui_build_icon ( pobjecttype obj, int remove );

#endif
