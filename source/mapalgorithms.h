/***************************************************************************
                          mapalgorithms.h  -  description
                             -------------------
    begin                : Thu Oct 5 2000
    copyright            : (C) 2000 by Martin Bickel
    email                : bickel@asc-hq.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef mapalgorithms_h_included
 #define mapalgorithms_h_included

 #include "typen.h"

  /** searches fields in hexagonal "circles" around a field and calls testfield for each field
  */
  class   tsearchfields {
                protected:
                    pmap gamemap;
                public:
                    int         startx, starty;
                    int         abbruch;
                    int         maxdistance, mindistance;
                    int         xp, yp;
                    int         dist;
                    tsearchfields ( void );
                    virtual void testfield ( void ) = 0;
                    void initsuche ( pmap _gamemap, int sx, int sy, int max, int min );
                    virtual void startsuche ( void );
                 };

  /** draws a straight line on the hexagonal map and calls putpix8 for each field.
      Awfully unoptimized!
  */
  class tdrawgettempline  {
                  int freefields;
                  int num;
                  int sx, sy;

                  static int initialized;
                  static double dirs[ sidenum ];
                  static double offset;

                  void init ( void );

               protected:
                  pmap gamemap;

               public:
                   int tempsum;
                   tdrawgettempline ( int _freefields, pmap _gamemap );

                   void start ( int x1, int y1, int x2, int y2 );

                   virtual void putpix8 ( int x, int y );
                   double winkel ( int x, int y );
                   int winkelcomp ( double w1, double w2 );
              };

//! changes x and y to the coordinates of the neighbouring field of (x/y) in the direction direc
extern void  getnextfield(int &    x,
                          int &    y,
                          int direc);

extern void  getnextfielddir(int &    x,
                             int &    y,
                             int direc,
                             int sdir);

/** gets the direction from x1/y1 to x2/y2
  \returns -1 if the fields are identical
*/
extern int   getdirection(    int      x1,
                              int      y1,
                              int      x2,
                              int      y2);

//! returns the distance between (x1/y1) and (x2/y2); Awfully unoptimized !!
extern int beeline ( int x1, int y1, int x2, int y2 );

//! returns the distance the units a and b; Awfully unoptimized !!
extern int beeline ( const pvehicle a, const pvehicle b );


#endif

