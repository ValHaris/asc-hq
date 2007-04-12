/*! \file basetemp.h
   Some really ugly templates, which were written before I started using the
   STL. Should be scrapped and replaced by STL functions.
*/

//     $Id: basetemp.h,v 1.8 2007-04-12 20:52:44 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.7.2.1  2006/02/11 21:46:17  mbickel
//      Move cleanup
//
//     Revision 1.7  2004/05/16 15:40:31  mbickel
//      Fixed compilation problems with gcc
//      Included SDLmm library
//
//     Revision 1.6  2001/02/18 15:37:02  mbickel
//      Some cleanup and documentation
//      Restructured: vehicle and building classes into separate files
//         GameMap, tfield and helper classes into separate file (gamemap.h)
//      basestrm : stream mode now specified by enum instead of int
//
//     Revision 1.5  2001/01/28 14:04:04  mbickel
//      Some restructuring, documentation and cleanup
//      The resource network functions are now it their own files, the dashboard
//       as well
//      Updated the TODO list
//
//     Revision 1.4  2000/12/21 11:00:45  mbickel
//      Added some code documentation
//
//     Revision 1.3  1999/12/27 12:59:41  mbickel
//      new vehicle function: each weapon can now be set to not attack certain
//                            vehicles
//
//     Revision 1.2  1999/11/16 03:41:09  tmwilson
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

#include <stdio.h> 

/*
template<class T> dynamic_queue<T> :: dynamic_queue ( void )
{
   first = 0;
   last = 0;
   blksize = 10;
   size = blksize;
   buf = new T[size];
}

template<class T> void dynamic_queue<T> :: putval ( T a )
{
   if ( first >= size ) {
      int newsize = ((first+1) / blksize + 1) * blksize;
      T* temp = new T [ newsize ];
      for ( int i = 0; i < size; i++ )
         temp[i] = buf[i];

      size=newsize;
      delete[] buf;
      buf = temp;
   }
   buf[first] = a;
   first++;
} 

template<class T> int dynamic_queue<T> :: valavail ( void )
{
   if ( first > last )
      return 1;
   else 
      return 0;
} 

template<class T> T dynamic_queue<T> :: getval ( void )
{
   T b = buf[last];
   last++;
   if ( last >= first ) {
      first = 0;
      last = 0;
   }
   return b;
} 


template<class T> dynamic_queue<T> :: ~dynamic_queue()
{
   delete[] buf;
}
*/




template<class T> dynamic_array<T> :: dynamic_array ( void )
{
   maxaccessed = -1;
   blksize = 10;
   size = 0;
   buf = NULL;
//   resize ( blksize );
}
template<class T> dynamic_array<T> :: dynamic_array ( int sze )
{
   maxaccessed = -1;
   blksize = 10;
   size = 0;
   buf = NULL;
//   resize ( sze );
}

template<class T> void dynamic_array<T> :: reset ( void )
{
   maxaccessed = -1;
}


template<class T> void dynamic_array<T> :: resize ( int newsize )
{
   T* temp = new T [ newsize ];
   if ( buf ) {
      for ( int i = 0; i < size; i++ )
         temp[i] = buf[i];
    /*
      for ( int j = size; j < newsize; j++ )
         temp[j] = 0;
     */
      delete[] buf;
   }

   size=newsize;
   buf = temp;
}


template<class T> T& dynamic_array<T> :: operator[]( int a )
{
   if ( a > maxaccessed )
      maxaccessed = a;

   if ( a >= size ) {
      int newsize = ((a+1) / blksize + 1) * blksize;
      resize ( newsize );
   }
   return buf[a];
} 

template<class T> dynamic_array<T> :: ~dynamic_array()
{
   delete[] buf;
}


template<class T> int dynamic_array<T> :: getlength( void )
{
   return maxaccessed;
} 



template<class T> void dynamic_initialized_array<T> :: resize ( int newsize )
{
   int oldsize = T::size;
   dynamic_array<int> :: resize ( newsize );
   for ( int i = oldsize; i < newsize; i++ )
      T::buf[i] = initval;
}

template<class T> dynamic_initialized_array<T> :: dynamic_initialized_array ( T ival )
{
   initval = ival;
}

template<class T> dynamic_initialized_array<T> :: dynamic_initialized_array ( T ival, int sze )
                                             : dynamic_array<T> ( sze )
{
   initval = ival;
}
