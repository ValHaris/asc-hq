/*! \file sgstream.h
    \brief The IO for many basic classes and structurs of ACS
  
*/

//     $Id: sgstream.h,v 1.30.2.4 2004-10-26 16:35:05 mbickel Exp $

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
#include "graphics/surface.h"



extern void generatedirecpict ( void* orgpict, void* direcpict );
FieldQuickView* generateAverageCol ( void* image );
FieldQuickView* generateAverageCol ( const Surface& image );



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

         SingleUnitSet ( void ) : active ( 1 ), filterBuildings ( true ), ID(0) {};
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

extern Surface generate_gui_build_icon ( pobjecttype obj, int remove );
extern Surface generate_gui_build_icon ( pbuildingtype bld );
extern Surface generate_gui_build_icon ( pvehicletype tnk );

#endif
