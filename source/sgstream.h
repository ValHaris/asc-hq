/*! \file sgstream.h
    \brief The IO for many basic classes and structurs of ACS
  
*/

/*
    This file is part of Advanced Strategic Command; http://www.asc-hq.de
    Copyright (C) 1994-2010  Martin Bickel  and  Marc Schellenberger

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
// #include "graphics/surface.h"

#include "loki/Singleton.h"



class ConfigurationFileLocatorCore {
      ASCString cmdline;
      ASCString exePath;
      int configFileType;
   protected:
      vector<ASCString> getDefaultDirectory();


   public:
      ConfigurationFileLocatorCore();
      void setCommandLineParam( const ASCString& path );
      void setExecutableLocation( const ASCString& path );
      ASCString getExecutableLocation();
      ASCString getSpecialPath( int type );

      ASCString getConfigFileName();
      ASCString getConfigForPrinting();
      void writeDefaultPathsToOptions();
};


typedef Loki::SingletonHolder< ConfigurationFileLocatorCore > ConfigurationFileLocator;


extern void generatedirecpict ( void* orgpict, void* direcpict );



extern void loadpalette ( );
extern int readgameoptions ( const ASCString& filename= "" );
extern bool writegameoptions ( ASCString filename = "" );
extern ASCString getConfigFileName ();

extern void checkFileLoadability ( const ASCString& filename );

extern void initFileIO ( const ASCString& configFileName, int skipChecks = 0 );


#endif
