/*! \file sgstream.h
    \brief The IO for many basic classes and structurs of ACS
  
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


#ifndef sgstreamH
#define sgstreamH

#include <vector>

#include "basestrm.h"
#include "typen.h"
#include "graphics/surface.h"



extern void generatedirecpict ( void* orgpict, void* direcpict );



extern void loadpalette ( );
extern int readgameoptions ( const ASCString& filename= "" );
extern bool writegameoptions ( ASCString filename = "" );
extern ASCString getConfigFileName ();

extern void loadguipictures( void );
extern void checkFileLoadability ( const ASCString& filename );

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

typedef std::vector<SingleUnitSet*> UnitSets;
extern UnitSets unitSets;

extern void loadUnitSets ( void );

extern int dataVersion;
extern void checkDataVersion( );

extern Surface generate_gui_build_icon ( pvehicletype tnk );

#endif
