//     $Id: strtmesg.cpp,v 1.6 2000-04-04 08:31:41 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.5  2000/04/03 09:52:16  mbickel
//      Fixed crash in mine strength calculation
//
//     Revision 1.4  2000/04/02 21:51:09  mbickel
//      Fixed bugs graphic set loading routines
//
//     Revision 1.3  2000/04/01 11:38:39  mbickel
//      Updated the small editors
//      Added version numbering
//
//     Revision 1.2  1999/11/16 03:42:39  tmwilson
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


// These strings should be the same as the release tags in CVS !
const char* asc_release="ASC1.0.3";

#include <stdio.h>

char startupmessagebuffer[200];

char* getstartupmessage ( void )
{
    sprintf( startupmessagebuffer, "\nAdvanced Strategic Command : %s\nmade %s      %s\n\n", asc_release, __DATE__, __TIME__);
    return startupmessagebuffer;
}

char* getaboutmessage ( void )
{
    sprintf( startupmessagebuffer, "Advanced Strategic Command : %s ; made %s ; %s", asc_release, __DATE__, __TIME__);
    return startupmessagebuffer;
}

char* kgetstartupmessage ( void )
{
    sprintf( startupmessagebuffer, "\n      Mapeditor for\nAdvanced Strategic Command \n%s\nmade %s      %s\n\n", asc_release, __DATE__, __TIME__);
    return startupmessagebuffer;
}

char* kgetaboutmessage ( void )
{
    sprintf( startupmessagebuffer, "Mapeditor for Advanced Strategic Command : %s ; made %s ; %s", asc_release, __DATE__, __TIME__);
    return startupmessagebuffer;
}


