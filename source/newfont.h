//     $Id: newfont.h,v 1.3 1999-11-22 18:27:45 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.2  1999/11/16 03:42:16  tmwilson
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

#ifndef newfont_h

#define newfont_h 1
#include "tpascal.inc"
#include "palette.h"
#include "basestrm.h"

typedef char charr[49];

struct  toldcharacter {
           char    width;
           word    size;
           int     diskposition;
           char*   memposition;
           char    dummy;
        };

typedef signed char    tkerning[101][101];
typedef char           tkernchartable[256];

// tkerning=Array[0..100,0..100] of Shortint;

struct toldfont {
         charr            id;
         char             name[256];
         char             number;
         boolean          color;
         toldcharacter    character[256];
         integer          height;
         tkernchartable   kernchartable;
         tkerning         kerning;
         word             dummy;
         boolean          useems;
         boolean          caps;
         dacpalette256*   palette;
         byte             groundline;
      };



struct  tcharacter {
           word    width;
           word    size;
           char*   memposition;
        };

struct tfont {
         char*            name;
         boolean          color;
         boolean          caps;
         integer          height;
         tcharacter       character[256];
         signed char      kerning[256][256]; 
         dacpalette256*   palette;
         byte             groundline;
      };


typedef tfont* pfont;





struct tfontsettings {
         pfont   font;
         char    color;
         char    background;
         char    markcolor;
         boolean colorfont;
         word    length;
         char    direction;
         char    justify;
         signed char italic;
         char    height;
         char    compmode;
         pfont   markfont;
     };


#define lefttext 0
#define centertext 1
#define righttext 2



#ifdef _NOASM_
 extern void expand(void* p1, void* q1, int size);
 extern void showtext2( const char* TextToOutput, int x1, int x2 );
 extern void showtext2c( const char* TextToOutput, int x1, int x2 );

 extern tfontsettings activefontsettings;

#else
 extern "C" void expand(void* p1, void* q1, int size);
 #pragma aux expand parm [ eax ] [ ebx ] [ ecx ] modify [ edx edi esi ]

 extern "C" void showtext2( const char* TextToOutput, int x1, int x2 );
 #pragma aux showtext2 parm [ ecx ] [ ebx ] [ eax ] modify [ edx esi edi ]
 
 extern "C" void showtext2c( const char* TextToOutput, int x1, int x2 );
 #pragma aux showtext2c parm [ ecx ] [ ebx ] [ eax ] modify [ edx esi edi ]

 extern "C" tfontsettings activefontsettings;

#endif



extern pfont loadfont(char* filename);
extern pfont loadfont( pnstream stream );

extern void showtext4 ( const char* TextToOutput, int x1, int y1, ... );


extern void         showtext3( const char *       txt,
                       word         xpos,
                       word         ypos);
extern void         showtext3c( const char *       txt,
                       word         xpos,
                       word         ypos);

extern int gettextwdth ( char* txt, pfont font );
extern void shrinkfont ( pfont font, int diff );


extern const char* fontid;

#endif
