
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

#include "blitter.h"

 map<int,int*> RotationCache::cache;
 int RotationCache::xsize = -1;
 int RotationCache::ysize = -1;

 ZoomCache::ZoomMap ZoomCache::zoomCache;
  
 NullParamType nullParam;

 
 int foobar_func()
 {
    return 0;
 }

 
 // base 10:
 // const int ColorMerger_Alpha_XLAT_Table_shadings[8] = { 9, 8, 6, 4, 11, 12, 14, 17 };

 // base 16:
 const int ColorMerger_Alpha_XLAT_Table_shadings[8] = { 14, 12, 9, 6, 18, 20, 23, 28 };

