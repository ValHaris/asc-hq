/***************************************************************************
                          graphicset.cpp  -  description
                             -------------------
    begin                : Sun Jan 28 2001
    copyright            : (C) 2001 by Martin Bickel
    email                : bickel@asc-hq.org
 ***************************************************************************/

/*! \file graphicset.cpp
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

#include <map>

#include "basestrm.h"
#include "misc.h"
#include "graphicset.h"
#include "basegfx.h"
#include "errors.h"
#include "typen.h"
#include "sgstream.h"

#include "loadpcx.h"



GraphicSetManager_Base::GraphicSetManager_Base() : activeSet(NULL)
{

}


bool GraphicSetManager_Base :: picAvail ( int num ) const
{
   if ( activeSet )
      return activeSet->picAvail(num);
   else
      return false;
}


int GraphicSetManager_Base :: getMode ( int num ) const
{
   if ( activeSet && activeSet->picmode.size() > num )
      return activeSet->picmode[num];
   else
      return 0;
}



Surface& GraphicSetManager_Base :: getPic ( int num )
{
   return activeSet->image[num];
}




int GraphicSetManager_Base :: setActive ( int id )
{
   if ( activeSet && id == activeSet->id )
      return id;

      
   for ( GraphicSets::iterator i = graphicSets.begin(); i != graphicSets.end(); ++i )
      if ( (*i)->id == id ) {
         activeSet = *i;
         return id;
      }

   if ( graphicSets.size() < 1 )
      fatalError( "no graphic sets (*.gfx) found!" );
   activeSet = *graphicSets.begin();   
   return 0;
}

const OverviewMapImage* GraphicSetManager_Base::getQuickView( int id )
{
   if ( picAvail ( id )) {
      map<int,OverviewMapImage>::iterator qv = activeSet->quickViewImages.find ( id );
      if ( qv == activeSet->quickViewImages.end()) {
         OverviewMapImage* fqv = new OverviewMapImage ( getPic( id ) );
         activeSet->quickViewImages[id] = *fqv;
         delete fqv;
         return &activeSet->quickViewImages[id];
      } else
         return &qv->second;

   } else {
      static OverviewMapImage* emptyFieldQuickView = NULL;
      if ( !emptyFieldQuickView )
         emptyFieldQuickView = new OverviewMapImage();

      return emptyFieldQuickView;
   }
}



int getGraphicSetIdFromFilename ( const ASCString& filename )
{
    tnfilestream stream ( filename, tnstream::reading );

    int magic = stream.readInt();
    if ( magic == -1 ) {
       return stream.readInt();
    } else
       return 0;
}



void GraphicSetManager_Base::loadData()
{
   if ( activeSet )
      return;

   #ifdef logging
   logtofile("loadbi3graphics");
   #endif

   loadpalette();

   Surface emptyField;
   {
      tnfilestream s ( "emptyfld.raw", tnstream::reading );
      emptyField.read( s );
   }
   
   
   /*

   #ifdef genimg
   void* mask;
   {
      int i ;
      tnfilestream s ( "largehex.raw", tnstream::reading );
      s.readrlepict ( &mask, false, & i );
   }
   #endif
*/

   ASCString location;
   tfindfile ff ( "*.gfx" );
   ASCString filename = ff.getnextname( NULL, NULL, &location);
   while ( !filename.empty() ) {

      tnfilestream s ( filename.c_str(), tnstream::reading );

      displayLogMessage ( 5, "loading graphic set " + location + filename + "\n" );

      int magic = s.readInt();
      if ( magic == -1 ) {

         GraphicSet* gs = new GraphicSet;

         gs->id = s.readInt();
         int picnum = s.readInt();
         s.readInt(); // maxPicSize

         int* picmode = new int[picnum];
         for ( int i = 0; i < picnum; ++i )
            picmode[i] = s.readInt();
            
         gs->image.resize   ( picnum );
         gs->picmode.resize ( picnum );
         for ( int i = 0; i < picnum; i++ ) {
            if ( picmode[i] >= 1 ) {
               Surface& surf = gs->image[i];
               surf.read ( s );
               if ( surf.w() != fieldsizex || surf.h() != fieldsizey ) 
                  surf.strech ( fieldsizex, fieldsizey );
               gs->picmode[i] = picmode[i];
            } else {
               gs->picmode[i] = 256 + 2;
               gs->image[i] = emptyField;
            }
           gs->image[i].assignDefaultPalette();
         }

         delete[] picmode;

         graphicSets.push_back ( gs );
      }

      filename = ff.getnextname();
   }

   setActive ( 0 );
}


GraphicSetManager_Base::~GraphicSetManager_Base()
{
   for ( GraphicSets::iterator i = graphicSets.begin(); i != graphicSets.end(); ++i)
      delete *i;
}

