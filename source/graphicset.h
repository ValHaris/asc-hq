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

#ifndef graphicsetH
 #define graphicsetH

#include <map>
#include "libs/loki/Singleton.h"

#include "global.h" 
#include "graphics/surface.h" 
#include "typen.h"
#include "overviewmapimage.h"

 extern int getGraphicSetIdFromFilename ( const ASCString& filename );


 class GraphicSetManager_Base {

        class GraphicSet {
              public:
                int id;
                vector<Surface> image;
                vector<int>     picmode;
                map<int,OverviewMapImage> quickViewImages;

                bool  picAvail ( int num ) const 
                { 
                   if ( picmode.size() > num ) 
                      return picmode[num] < 256;
                   else
                      return false;
                };        
        };

 
     GraphicSet* activeSet;
     typedef vector<GraphicSet*> GraphicSets;
     GraphicSets graphicSets;

     GraphicSetManager_Base ();   // should be made private
   public:
     int setActive ( int id );
     int getActiveID ( ) { if ( activeSet ) return activeSet->id; else return -1; };

     void loadData();
     
    
     bool picAvail ( int num ) const;
     int  getMode( int num ) const;
     Surface& getPic ( int num );
     const OverviewMapImage* getQuickView( int id );
     friend class Loki::CreateUsingNew<GraphicSetManager_Base>;
     // friend struct CreateUsingNew;

     ~GraphicSetManager_Base();
 };

 typedef Loki::SingletonHolder<GraphicSetManager_Base> GraphicSetManager;
  
#endif
