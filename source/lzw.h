//     $Id: lzw.h,v 1.3 2009-04-18 13:48:38 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.2  1999/11/16 03:42:04  tmwilson
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

/*--- lzw.h ------------------------------- Listing 9-8 --------
 * Header file for LZW compression routines                    *
 *-------------------------------------------------------------*/

/* Critical sizes for LZW */
#define PRESET_CODE_MAX 256    /* codes like this are preset */
#define END_OF_INPUT    256    /* this code terminates input */
#define NEW_DICTIONARY  257    /* reinitialize the dictionary */
#define UNUSED_CODE     258    /* an invalid code */
#define STARTING_CODE   259    /* first code we can use */
#define MAX_CODE        65536  /* 2 ^ BITS */
#define DICTIONARY_SIZE 81901L /* a prime # > MAX_CODE * 1.2 */

typedef unsigned short CodeType; /* can hold MAX_CODE */
typedef unsigned long IndexType; /* can hold DICTIONARY_SIZE */
typedef unsigned long CountType; /* used for statistics only */

extern const char* LZ_SIGNATURE ;
extern const char* RLE_SIGNATURE ;
