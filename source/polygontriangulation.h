/*! \file polygontriangulation.h
    \brief Interface to the polygon triangulation library
*/

//     $Id: polygontriangulation.h,v 1.2 2004-01-16 21:17:23 mbickel Exp $
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

#ifndef polygontriangulationH
 #define polygontriangulationH

 #include <math.h>
 #include <sys/types.h>
 #include <stdlib.h>
 #include <stdio.h>

 namespace PolygonTriangulationLibrary {
 #include "libs/triangul/interfac.h"
 #include "libs/triangul/triangul.h"
 };

template <class Poly>
class  PolygonPainter {
         public:
             bool  paintPolygon ( const Poly& poly );
         protected:
             virtual void   sortpolygon    ( Poly& a );
             virtual void   painttriangle  ( typename Poly::Point p[] );
             virtual void   painthorizline ( int x1, int x2, int y );
             virtual void   paintline      ( int x1, int y1, int x2, int y2 );
             virtual void   setpoint       ( int x,  int y  ) = 0;
             virtual int    getpolydir     ( const Poly& a);
             virtual double getsegmentdir  ( int dx, int dy );
             //! returns true if polygon correct
             virtual bool checkpolygon   ( const Poly& poly );
             //! returns true if lines a-b and c-d don't intersect
             virtual bool checkline      ( typename Poly::Point a, typename Poly::Point b, typename Poly::Point d, typename Poly::Point e );
          };

template <class Poly>
bool  PolygonPainter<Poly>::paintPolygon ( const Poly& poly )
{
   if ( !poly.vertex.size() )
      return true;

//   displayLogMessage ( 10, "pp1 ");
   if (checkpolygon ( poly )) {
      if ( poly.vertex.size() == 1 ) {
         setpoint ( poly.vertex[0].x, poly.vertex[0].y );
         return true;
       }
       if ( poly.vertex.size() == 2 ) {
         paintline ( poly.vertex[0].x, poly.vertex[0].y, poly.vertex[1].x, poly.vertex[1].y  );
         return true;
       }

//       displayLogMessage ( 10, "pp2 ");
       Poly pol2 = poly;
       sortpolygon ( pol2 );


       typedef double double2[2];
       double2*  floatpoints = new double2 [ pol2.vertex.size()+1 ];
       for ( int i=0; i<pol2.vertex.size() ; i++ ) {
          floatpoints[i+1][0] = (double) pol2.vertex[i].x;
          floatpoints[i+1][1] = (double) pol2.vertex[i].y;
       } /* endfor */

       typedef int int3[3];

       int3* triangles = new int3  [ pol2.vertex.size() ];

       int contours = pol2.vertex.size();

//       displayLogMessage ( 10, "pp3 ");
       PolygonTriangulationLibrary::triangulate_polygon ( 1, &contours, floatpoints, triangles );
//       displayLogMessage ( 10, "pp4 ");

       for ( int i=0; i<pol2.vertex.size()-2 ;i++ ) {
          typename Poly::Point tripos[3];
          tripos[0] = pol2.vertex[triangles[i][0]-1];
          tripos[1] = pol2.vertex[triangles[i][1]-1];
          tripos[2] = pol2.vertex[triangles[i][2]-1];
          painttriangle ( tripos );
       } /* endfor */

       delete[] triangles;
       delete[] floatpoints;

       return true;
   } else
     return false;
}

static const double pi = 3.141592654;


template <class Poly>
bool PolygonPainter<Poly>::checkline( typename Poly::Point a, typename Poly::Point b, typename Poly::Point d, typename Poly::Point e )
{
   double r,s,t;

   b.x -= a.x;
   b.y -= a.y;

   e.x -= d.x;
   e.y -= d.y;

   /*
   if ( b.y != 0 && e.y != 0 ) {
      if ( abs( b.x / b.y - e.x / e.y ) < 1e-6 )
         return false;
   } else
      if ( b.y == 0 && e.y == 0 )
         return false;
   */


   t = b.x * e.y - b.y * e.x;
   if (t != 0) {
     double m = ( a.x * b.y - a.y * b.x + d.y * b.x - d.x * b.y );
     r = m / -t;

     double n = ( a.y * e.x - a.x * e.y + d.x * e.y - d.y * e.x );
     s = n / t;
     if ( s>=0 && s <= 1  &&  r>=0 && r <= 1)
        return false;

   } /* endif */
   return true;
}


template <class Poly>
bool PolygonPainter<Poly>::checkpolygon   ( const Poly& poly )
{
    if ( poly.vertex.size() > 1  && poly.vertex.size() < 3 )
       return true;
    else {
       int i,j,k;

       for ( i = 0; i < poly.vertex.size(); i++ )
          for ( j = 0; j < poly.vertex.size(); j++ )
             if ( i != j )
               if ( poly.vertex[i].x == poly.vertex[j].x  &&
                    poly.vertex[i].y == poly.vertex[j].y )
                  return false;

       k = poly.vertex.size() -1;
       for (i=0; i < k ; i++) {
          for (j = i +2; j < k ; j++)
              if (checkline ( poly.vertex[i], poly.vertex[i+1], poly.vertex[j], poly.vertex[j+1] ) == false)
                  return false;
          if (i > 0 && j == k)
            if (checkline ( poly.vertex[i], poly.vertex[i+1], poly.vertex[j], poly.vertex[0] ) == false)
               return false;
       } /* endfor */
       return true;
    }
}



template <class Poly>
double  PolygonPainter<Poly>::getsegmentdir   ( int dx, int dy )
{
   double alpha;

   if (dx) {
      alpha = atan ( (double) dy / (double) dx );
      if ( dy < 0 ) {
         if ( dx < 0)
            alpha = -pi + alpha;
      } else
        if ( dx < 0)
           alpha = pi - alpha;

   } else {
      if (dy > 0)
         alpha = pi/2;
      else
         alpha = -pi/2;
   } /* endif */

   return alpha;
}

template <class Poly>
int PolygonPainter<Poly>::getpolydir     ( const Poly& a)
{
    int p ;
    double alpha, beta, gamma;
    int   r = 0;
    for (p=0; p< a.vertex.size(); p++) {

       if (p == 0)
          alpha = getsegmentdir( a.vertex[p].x - a.vertex[a.vertex.size()-1].x , a.vertex[p].y - a.vertex[a.vertex.size()-1].y );
       else
          alpha = getsegmentdir( a.vertex[p].x - a.vertex[p-1].x, a.vertex[p].y - a.vertex[p-1].y );

       if (p+1 >= a.vertex.size())
          beta =  getsegmentdir( a.vertex[0].x - a.vertex[p].x , a.vertex[0].y - a.vertex[p].y  );
       else
          beta =  getsegmentdir( a.vertex[p+1].x - a.vertex[p].x,  a.vertex[p+1].y - a.vertex[p].y );

       gamma = beta - alpha ;
       if (gamma > pi)
          gamma = pi - gamma;
       if (gamma < -pi)
          gamma = -pi - gamma;
       r+= (int) ( gamma * 1000 );
    }
    return r;
}


template <class Poly>
void PolygonPainter<Poly>::sortpolygon   ( Poly& a )
{
   if ( a.vertex.size() >= 3 )
      if ( getpolydir(a) < 0 ) {
         for ( int i=0;i< a.vertex.size()/2 ;i++ ) {
            typename Poly::Point p = a.vertex[i];
            a.vertex[i] = a.vertex[a.vertex.size()-i-1];
            a.vertex[a.vertex.size()-i-1] = p;
         } /* endfor */
      }
}


template <class Poly>
void PolygonPainter<Poly>::painttriangle  ( typename Poly::Point p[] )
{
   int x,y,dx1,dx2,dy1,dy2;

   for ( int i=0; i<2 ; ) {
      if ( p[i+1].y < p[i].y || ( p[i+1].y == p[i].y && p[i+1].x < p[i].x )) {
         x        = p[i].x;
         p[i].x   = p[i+1].x;
         p[i+1].x = x;
         y        = p[i].y;
         p[i].y   = p[i+1].y;
         p[i+1].y = y;
         i        = 0;
       } else
         i++;

   } /* endfor */


   if ( p[0].y < p[1].y ) {
      dx1 = p[1].x - p[0].x;
      dx2 = p[2].x - p[0].x;
      dy1 = p[1].y - p[0].y;
      dy2 = p[2].y - p[0].y;

      for (y=p[0].y; y<=p[1].y ; y++)
          painthorizline ( p[0].x + dx1 * (y - p[0].y) / dy1 , p[0].x + dx2 * (y - p[0].y) / dy2, y );

      if (p[1].y < p[2].y) {
         dx1 = p[2].x - p[1].x;
         dy1 = p[2].y - p[1].y;

         for ( ; y<=p[2].y ; y++)
             painthorizline ( p[1].x + dx1 * (y - p[1].y) / dy1 , p[0].x + dx2 * (y - p[0].y) / dy2, y );
      }
   } else {
      if ( p[1].y < p[2].y ) {

         dx1 = p[2].x - p[1].x;
         dx2 = p[2].x - p[0].x;
         dy1 = p[2].y - p[0].y;

         for (y=p[0].y; y<=p[2].y ; y++)
             painthorizline ( p[1].x + dx1 * (y - p[0].y) / dy1 , p[0].x + dx2 * (y - p[0].y) / dy1, y );
      } else
        painthorizline ( p[0].x, p[2].x, p[0].y );
   }

}


template <class Poly>
void   PolygonPainter<Poly>::painthorizline ( int x1, int x2, int y )
{
   int dx;
   if (x1 > x2)
      dx = -1;
   else
      if (x1 < x2)
         dx = 1;
      else {
         setpoint ( x2, y );
         return;
      }

   int x= x1;

   do {
      setpoint ( x, y );
      x += dx;
   } while ( x != x2 ); /* enddo */
   setpoint ( x, y );
}


template <class Poly>
void   PolygonPainter<Poly>::paintline   ( int x1, int y1, int x2, int y2 )
{
        float           m, b;
        int             w;
        float           yy1, yy2, xx1, xx2;



    if ( x1 == x2) {
        for (w=y1;w<=y2 ;w++ )
           setpoint ( x1, w );
    } else {
       if ( y1 == y2) {
          for (w=x1;w<=x2 ;w++ )
               setpoint( w, y1 );
       } else {
                yy1 = y1;
                yy2 = y2;
                xx1 = x1;
                xx2 = x2;
                m = (yy2 - yy1) / (xx2 - xx1);
                b = y1 - m * x1;
                if ((m <= 1) && (m >= -1)) {
                        if (x2 < x1) {
                                w = x2;
                                x2 = x1;
                                x1 = w;
                                w = y2;
                                y2 = y1;
                                y1 = w;
                        }
                        for (w = x1; w <= x2; w++)
                                setpoint(w, (int) (m * w + b) );

                } else {
                        if (y2 < y1) {
                                w = x2;
                                x2 = x1;
                                x1 = w;
                                w = y2;
                                y2 = y1;
                                y1 = w;
                        }
                        for (w = y1; w <= y2; w++) {
                                setpoint( (int) ((w - b) / m), w );
                        }

                }
         } /* endif */
    }
}





#endif
