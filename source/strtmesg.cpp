//     $Id: strtmesg.cpp,v 1.256 2004-08-01 20:13:29 mbickel Exp $

/*
    This file is part of Advanced Strategic Command; http://www.asc-hq.de
    Copyright (C) 1994-2003  Martin Bickel  and  Marc Schellenberger

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


// These strings should be the same as the release tags in CVS !
const char* asc_release="ASC1.15.0.92";

#include <stdio.h>
#include "strtmesg.h"
#include "stringtokenizer.h"
#include "misc.h"


const char* getVersionString (  )
{
  return &asc_release[3];
}

int getNumericVersion ( )
{
   int vers = 0;
   const char* d = asc_release+3;
   for ( int i = 0; i < 4; i++ ) {
      vers *= 256;
      if ( *d ) {
         const char* start = d;
         do 
           d++;
         while ( *d != '.' && *d != 0 );

         ASCString s ( start, d-start );
         vers += atoi ( s.c_str() );
         if ( *d )
           d++;
      }
   }
   return vers;
}

const char* getFullVersionString (  )
{
  return asc_release;
}

ASCString getstartupmessage (  )
{
    char startupmessagebuffer[1000];
    sprintf( startupmessagebuffer, "\nAdvanced Strategic Command : %s\nmade %s      %s\n\n", asc_release, __DATE__, __TIME__);
    return ASCString(startupmessagebuffer);
}

ASCString getaboutmessage (  )
{
    char startupmessagebuffer[1000];
    sprintf( startupmessagebuffer, "Advanced Strategic Command : %s ; made %s ; %s", asc_release, __DATE__, __TIME__);
    return ASCString(startupmessagebuffer);
}

ASCString kgetstartupmessage (  )
{
    char startupmessagebuffer[1000];
    sprintf( startupmessagebuffer, "\n      Mapeditor for\nAdvanced Strategic Command \n%s\nmade %s      %s\n\n", asc_release, __DATE__, __TIME__);
    return ASCString(startupmessagebuffer);
}

ASCString kgetaboutmessage (  )
{
    char startupmessagebuffer[1000];
    sprintf( startupmessagebuffer, "Mapeditor for Advanced Strategic Command : %s ; made %s ; %s", asc_release, __DATE__, __TIME__);
    return ASCString(startupmessagebuffer);
}

ASCString getVersionString ( int version )
{
   ASCString s = strrr(version>>24);
   s += ".";
   s += strrr((version>>16) & 0xff);
   s += ".";
   s += strrr((version>>8) & 0xff);
   s += ".";
   s += strrr(version & 0xff);
   return s;
}

