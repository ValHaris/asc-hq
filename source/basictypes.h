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

#ifndef basictypes_H
#define basictypes_H

#include <vector>
#include <list>
#include <bitset>

//! A vector that stores pointers, but deletes the objects (and not only the pointers) on destruction. The erase method does NOT delete the objects !
template <class T> class PointerVector : public std::vector<T> {
   public:
     ~PointerVector() {
        for ( typename std::vector<T>::iterator it = std::vector<T>::begin(); it!=std::vector<T>::end(); it++ )
            delete *it;
     };
};


//! A list that stores pointers, but deletes the objects (and not only the pointers) on destruction. The erase method does NOT delete the objects !
template <class T> class PointerList : public std::list<T> {
   public:
     ~PointerList() {
        for ( typename std::list<T>::iterator it= std::list<T>::begin(); it!= std::list<T>::end(); it++ )
            delete *it;
     };
};


typedef std::bitset<64> BitSet;


#endif
