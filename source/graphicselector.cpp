/***************************************************************************
                          graphicselector.cpp  -  description
                             -------------------
    begin                : Sun Jan 28 2001
    copyright            : (C) 2001 by Martin Bickel
    email                : bickel@asc-hq.org
 ***************************************************************************/

/*! \file graphicselector.cpp
    \brief Functions for selecting an image from a graphic set
*/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "graphicset.h"
#include "graphicselector.h"
#include "newfont.h"
#include "palette.h"
#include "sgstream.h"
#include "basegfx.h"
#include "events.h"
#include "stack.h"
#include "events.h"

int marked = -1;

class tgetbi3pict {
   int start ;
//   int marked ;

   int xn ;
   int xs ;
   int yn ;
   int ys ;
   int picsize ;
   static pfont fnt;
   
   public:
      tgetbi3pict ( void );
      void paint ( void );
      void run ( int* num );

};

pfont tgetbi3pict :: fnt = NULL;

tgetbi3pict :: tgetbi3pict ( void )
{
   start = 0;
//   marked = -1;

   xn = 10;
   xs = 50;
   yn = 8;
   ys = 50;
   picsize = 24;
         
   if ( !asc_paletteloaded ) 
     loadpalette();

   setvgapalette256(pal);

}


void tgetbi3pict :: paint ( void )
{
   for ( int y = 0; y < yn; y++ ) {
      showtext2 ( strrr ( start + y * xn ), 10, 20 + y * ys );
      for (int x = 0; x < xn; x++ ) {
         int num = start + y * xn + x;
         bar ( 50 + x * xs, 10 + y * ys, 80 + x * xs, 40 + y * ys, black );
         if ( marked == num )
            rectangle ( 50 + x * xs, 10 + y * ys, 80 + x * xs, 40 + y * ys, yellow );

         if ( num < getActiveGraphicSet()->maxnum &&  getActiveGraphicSet()->picAvail ( num ) ) {
            void* buf;
            loadbi3pict ( num, &buf );
            putspriteimage ( 50 + x * xs + 30/2 - picsize/2, 10 + y * ys + 30/2 - picsize/2, buf );
            delete buf;
         }

      } /* endfor */
   }
}

void tgetbi3pict :: run ( int* num )
{
   if ( !fnt ) {
      tnfilestream stream ( "monogui.fnt", 1 );
      fnt = loadfont  ( &stream );
   }

   npush ( activefontsettings );

   activefontsettings.color = white;
   activefontsettings.justify = lefttext;
   activefontsettings.height = 0;
   activefontsettings.font = fnt;
   activefontsettings.length = 40;
   activefontsettings.background = black;


   if ( marked >= 0 ) {
     while ( marked < start )
        start -= xn;
     while ( marked >= start + yn * xn )
        start += xn;
   }

   paint();
   tkey ch;
   do {
      ch = r_key ();
      int oldmark = marked;
      int oldstart = start;

      if ( ch == ct_up ) {
         marked -= xn;
         if ( marked < 0 )
            marked = 0;
      }
      if ( ch == ct_down ) {
         marked += xn;
         if ( marked >= getActiveGraphicSet()->maxnum )
            marked = getActiveGraphicSet()->maxnum-1;
      }
      if ( ch == ct_right ) {
         marked += 1;
         if ( marked >= getActiveGraphicSet()->maxnum )
            marked = getActiveGraphicSet()->maxnum-1;
      }
      if ( ch == ct_left ) {
         marked -= 1;
         if ( marked < 0 )
            marked = 0;
      }
      if ( marked >= 0 ) {
        while ( marked < start )
           start -= xn;
        while ( marked >= start + yn * xn )
           start += xn;
      }
      if ( marked != oldmark || oldstart != start )
         paint();

   } while ( ch != ct_enter || marked < 0 ); /* enddo */

//   *picture = bi3graphics[marked];
   *num = marked;

   npop ( activefontsettings );

}


void getbi3pict ( int* num, void** picture )
{
   tgetbi3pict gbi3p;
   gbi3p.run ( num );
   loadbi3pict_double ( *num, picture );
}


void getbi3pict_double ( int* num, void** picture )
{
   tgetbi3pict gbi3p;
   gbi3p.run ( num );

   loadbi3pict_double ( *num, picture );

}
