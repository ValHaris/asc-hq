/***************************************************************************
                          graphicset.h  -  description
                             -------------------
    begin                : Sun Jan 28 2001
    copyright            : (C) 2001 by Martin Bickel
    email                : bickel@asc-hq.org
 ***************************************************************************/

/*! \file graphicset.h
    \brief A system that provides a set of images for vehicles, buildings, etc.
*/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef graphicset_h_included
 #define graphicset_h_included

 extern int getGraphicSetIdFromFilename ( const char* filename );
 extern int activateGraphicSet ( int id  );
 extern void loadbi3graphics( void );

 extern int keeporiginalpalette;

 extern int  loadbi3pict_double ( int num, void** pict, int interpolate = 0, int reference = 1 );
   // returns: 1 if picture is a reference
   //          0 if picture is a copy

 extern void loadbi3pict ( int num, void** pict );

 class ActiveGraphicPictures {
   public:
     int activeId;
     int maxnum;
     int currentnum;
     void** bi3graphics;
     int* bi3graphmode;        // 0: no picture available ( should not happen ingame )
                               // 1: picture has BI size
                               // 2: picture has ASC size
                               // +0xff : picture is dummy picture
     int absoluteMaxPicSize;
     int setActive ( int id );

     ActiveGraphicPictures ( void ) {
        activeId = -1;
     };

     void alloc ( void );
     int picAvail ( int num ) const;
     void* getPic ( int num );
     int getMode ( int num ) const;
 };

 extern const ActiveGraphicPictures* getActiveGraphicSet();

#endif