/*! \file gui.cpp
    \brief All the buttons of the user interface with which the unit actions are
      controlled.
*/

/*
    This file is part of Advanced Strategic Command; http://www.asc-hq.de
    Copyright (C) 1994-2005  Martin Bickel  and  Marc Schellenberger

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
#include <cstring>
#include <stdlib.h>

#include "vehicletype.h"
#include "buildingtype.h"
#include "basegfx.h"
#include "typen.h"
#include "newfont.h"
#include "misc.h"
#include "gui.h"

#include "spfst.h"
#include "events.h"
#include "loaders.h"
#include "controls.h"
#include "dlg_box.h"
#include "dialog.h"
#include "building.h"
#include "attack.h"
#include "stack.h"
#include "sg.h"
#include "gameoptions.h"
#include "replay.h"
#include "gamedlg.h"
#include "itemrepository.h"
#include "viewcalculation.h"


tguihoststandard          gui;
// tselectbuildingguihost    selectbuildinggui;
// tselectobjectcontainerguihost      selectobjectcontainergui;
// tselectvehiclecontainerguihost     selectvehiclecontainergui;
// tselectweaponguihost      selectweaponguihost;
BasicGuiHost*        actgui = &gui;


int guixpos;


   #define guixdif 190     // Abstand zum RECHTEN Rand
   #define guigapx 13
   #define guigapy 4

int guiypos = 308 + guigapy;

void setguiposy ( int y )
{
   guiypos = y;
}


void*    BasicGuiHost::background[30][30];
int      BasicGuiHost::numpainted;


template<class T>
GuiHost<T>::GuiHost ( void )
{
   columncount = 3;
   iconspaintable = 12;
   firstshownline = 0;
   paintsize = 0;
   first_icon = NULL;
   numpainted = 0;
   infotextshown = NULL;
   firstpaint = 1;
   smalliconpos.buf = NULL;
}

template<class T>
void   GuiHost<T>::starticonload ( void )
{
   getfirsticon()->loaddata();
   getfirsticon()->sort ( NULL );
   chainiconstohost ( getfirsticon() );
}

template<class T>
void   GuiHost<T>::returncoordinates ( void* icn, int* x, int * y )
{
   T icon = (T) icn;
   int found = 0;

   icon->seticonsize ( paintsize );
   if ( paintsize == 0 ) {

      if ( firstpaint == 0)
         for ( int i = firstshownline*columncount; i < firstshownline*columncount + iconspaintable; i++ )
            if ( actshownicons[i % columncount][i / columncount] == icon ) {
              *x = guixpos + ( i % columncount ) * ( guiiconsizex + guigapx );
              *y = guiypos + ( i / columncount - firstshownline ) * ( guiiconsizey + guigapy );
              found = 1;
            }

      if ( found == 0) {
         if ( numpainted < firstshownline*columncount + iconspaintable   &&   numpainted >= firstshownline*columncount) {
            *x = guixpos + ( numpainted % columncount ) * ( guiiconsizex + guigapx );
            *y = guiypos + ( numpainted / columncount ) * ( guiiconsizey + guigapy );
         } else {
            *x = -1;
            *y = -1;
         }
         actshownicons[numpainted % columncount][numpainted / columncount] = icon;
         numpainted++;
      }
   } else {
      // icon->iconsize = 1;
      if ( firstpaint == 0)
         for ( int i = 0; i < iconspaintable; i++ )
            if ( actshownicons[i % columncount][i / columncount] == icon ) {
               *x = smalliconpos.x + i * ( guismallicongap + guismalliconsizex );
               *y = smalliconpos.y;
               found = 1;
            }

      if ( found == 0 ) {
         if ( numpainted < iconspaintable ) {
            *x = smalliconpos.x + numpainted * ( guismallicongap + guismalliconsizex );
            *y = smalliconpos.y;
         } else {
            *x = -1;
            *y = -1;
         }
         numpainted++;
      }
   }
}

template<class T>
void   GuiHost<T>::chainiconstohost ( T icn )
{
   if ( icn ) {
      setfirsticon( (T) icn->frst() );
      getfirsticon()->sethost ( this );
   }
}


template<class T>
void   GuiHost<T>::savebackground ( void )
{
   int size = imagesize ( 0, 0, guiiconsizex, guiiconsizey );
   for (int j = 0; j < iconspaintable/columncount ; j++ )
      for (int i = 0; i< columncount ; i++) {
         background[i][j] = new char [ size ];
         getimage ( guixpos + i*(guiiconsizex + guigapx), guiypos + j*(guiiconsizey + guigapy), guixpos + i*(guiiconsizex + guigapx) + guiiconsizex, guiypos + j*(guiiconsizey + guigapy) + guiiconsizey,  background[i][j] );
         actshownicons[i][j] = NULL;
      } /* endfor */
}


template<class T>
void GuiHost<T> :: putbackground ( int xx , int yy )
{
     int x = guixpos + xx * ( guiiconsizex + guigapx );
     int y = guiypos + yy * ( guiiconsizey + guigapy );
     if ( background[ xx ][ yy ] )
        putimage ( x, y, background[ xx ][ yy ] );
     else
        displaymessage("Warning: GuiHost :: putbackground ; no background avail", 1);
}

template<class T>
int    GuiHost<T>::painticons ( void )
{
   collategraphicoperations cgo ( guixpos, guiypos, guixpos + columncount*(guiiconsizex + guigapx), guiypos + iconspaintable/columncount*(guiiconsizey + guigapy) + guiiconsizey );
   numpainted = 0;

   firstpaint = 1;

   getfirsticon()->paintifavail();

   firstpaint = 0;

   for (int j = numpainted; j < iconspaintable ; j++ ) {
      if ( actshownicons[j % columncount][j / columncount] ) {
         putbackground ( j % columncount , j / columncount );
         actshownicons[j % columncount][j / columncount] = NULL;
      }
   } /* endfor */
   return numpainted;
}


template<class T>
void   GuiHost<T> :: cleanup ( void )    // wird zum entfernen der kleinen guiicons aufgerufen, bevor das icon ausgef\uFFFDhrt wird
{
   if ( smalliconpos.buf ) {
      collategraphicoperations cgo ( smalliconpos.x, smalliconpos.y, smalliconpos.x + smalliconpos.xsize, smalliconpos.y + guismalliconsizey );
      putimage ( smalliconpos.x, smalliconpos.y, smalliconpos.buf );

//      delete[] smalliconpos.buf;
      smalliconpos.buf = NULL;
      paintsize = 0;
   }
}


template<class T>
void   GuiHost<T>::paintsmallicons ( int taste, int up )
{
   int num;
   {

      npush ( paintsize );

      paintsize = 1;
      num = getfirsticon()->count();
      smalliconpos.xsize = num * guismalliconsizex + ( num - 1 ) * guismallicongap;

      if ( mouseparams.x + smalliconpos.xsize - guismalliconsizex/2 > hgmp->resolutionx )
         smalliconpos.x = hgmp->resolutionx - smalliconpos.xsize;
      else
         smalliconpos.x = mouseparams.x - guismalliconsizex/2;
/*
      if ( (CGameOptions::Instance()->mouse.smalliconundermouse == 0)  || ((CGameOptions::Instance()->mouse.smalliconundermouse == 2) && up ))
         smalliconpos.y = mouseparams.y - 5 - guismalliconsizey;
      else
         smalliconpos.y = mouseparams.y - guismalliconsizey / 2;
*/
      if ( smalliconpos.y < 0 )
         smalliconpos.y = 0;

      collategraphicoperations cgo ( smalliconpos.x, smalliconpos.y, smalliconpos.x + smalliconpos.xsize, smalliconpos.y + guismalliconsizey );

      smalliconpos.buf = new char[ imagesize ( smalliconpos.x, smalliconpos.y, smalliconpos.x + smalliconpos.xsize, smalliconpos.y + guismalliconsizey )];

      getimage ( smalliconpos.x, smalliconpos.y, smalliconpos.x + smalliconpos.xsize, smalliconpos.y + guismalliconsizey, smalliconpos.buf );

      numpainted = 0;
      firstpaint = 1;
      getfirsticon()->paintifavail();
      firstpaint = 0;

   }
   int mousestat = 0;

   int msg = 0;

   do {
      if ( mouseparams.taste != taste  &&  !mousestat )
         mousestat++;

      int fnd = 0;
      for ( int j = 0; j < num; j++ )
         if ( mouseinrect ( smalliconpos.x + j * ( guismallicongap + guismalliconsizex ), smalliconpos.y, smalliconpos.x + j * ( guismallicongap + guismalliconsizex ) + guismalliconsizex, smalliconpos.y + guismalliconsizey )) {
            fnd = 1;
            if ( actshownicons[j % columncount][j / columncount] ) {
                if ( actshownicons[j % columncount][j / columncount] != infotextshown ) {
                   msg = 0;
                   displaymessage2 ( actshownicons[j % columncount][j / columncount]->getinfotext() );
                }   

                infotextshown = actshownicons[j % columncount][j / columncount];
             }
         }
      if ( !fnd  && infotextshown  && msg == actdisplayedmessage ) {
         displaymessage2 ( "" );
         infotextshown = NULL;
      }

      if ( mouseparams.taste == 1) {
         int hit = -1;
         for ( int i = 0; i < num; i++ ) {
            pnguiicon acticon = actshownicons[i % columncount][i / columncount];
            int mx = mouseparams.x;
            int my = mouseparams.y;
            if ( mx >= smalliconpos.x + i * ( guismallicongap + guismalliconsizex )  &&      my  >=  smalliconpos.y &&
                 mx <  smalliconpos.x + (i + 1) * ( guismallicongap + guismalliconsizex ) && my  <   smalliconpos.y + guismalliconsizey )
            {
               if ( acticon->pressedbymouse() )
                  hit = 2;
               else
                  hit = 1;

               break;
            }


         }
         if ( hit >= 2 )
            mousestat = 100;

         if ( hit == -1   &&  mousestat && !mouseinrect ( smalliconpos.x, smalliconpos.y, smalliconpos.x + num * ( guismallicongap + guismalliconsizex ) - guismallicongap, smalliconpos.y + guismalliconsizey ) )
            mousestat = 101;

      }

//      if ( /*mouseparams.taste != 1   &&*/   mouseparams.taste != 0   &&   (mouseparams.taste != taste  || mousestat == 1 ))
//         mousestat = 102;

      releasetimeslice();
   } while ( !keypress() &&  mousestat < 100 ); /* enddo */

   cleanup();

   npop ( paintsize );

   if ( infotextshown  && msg == actdisplayedmessage )
      displaymessage2 ( "" );

   infotextshown = NULL;
}


template<class T>
void   GuiHost<T>::checkformouse ( void )
{
//   int msg;
   int found = 0;
   for (int j = 0; j < iconspaintable/columncount ; j++ )
      for (int i = 0; i< columncount ; i++) {
         if ( ( mouseparams.x >= guixpos + i*(guiiconsizex + guigapx))    &&
              ( mouseparams.y >= guiypos + j*(guiiconsizey + guigapy))    &&
              ( mouseparams.x <= guixpos + i*(guiiconsizex + guigapx) + guiiconsizex)    &&
              ( mouseparams.y <= guiypos + j*(guiiconsizey + guigapy) + guiiconsizey) )
              {
                 if ( actshownicons[i][j] ) {
                    if ( actshownicons[i][j] != infotextshown )
                       // msg =
                       displaymessage2 ( actshownicons[i][j]->getinfotext() );
                    found = 1;
                    if ( mouseparams.taste == 1)
                       actshownicons[i][j]->pressedbymouse();

                    infotextshown = actshownicons[i][j];
                 }
              }
      } /* endfor */
   if ( (found==0) && infotextshown ) {
      // if ( actdisplayedmessage == msg )
         displaymessage2 ( "" );
      infotextshown = NULL;
   }
}


template<class T>
void   GuiHost<T> :: runpressedmouse ( int taste  )
{

   mousevisible ( false );
   int oldmousex = mouseparams.x;
   int oldmousey = mouseparams.y;
   setmouseposition ( guixpos + guiiconsizex / 2, guiypos + guiiconsizey / 2);
   mousevisible ( true );

   int zy = -1;
   int zx = -1;

   int xp = 0;
   int yp = 0;

   while ( mouseparams.taste == taste ) {

      for (int j = 0; j < iconspaintable/columncount ; j++ )
          for (int i = 0; i< columncount ; i++)
             if ( ( mouseparams.x >= guixpos + i*(guiiconsizex + guigapx))    &&
                  ( mouseparams.y >= guiypos + j*(guiiconsizey + guigapy))    &&
                  ( mouseparams.x <= guixpos + i*(guiiconsizex + guigapx) + guiiconsizex)    &&
                  ( mouseparams.y <= guiypos + j*(guiiconsizey + guigapy) + guiiconsizey) )
              {
                 xp = i;
                 yp = j;
              }


       if ((zx != xp) || (zy != yp)) {
          if ( zx != -1  && zy != -1  &&  actshownicons[zx][zy] )
             actshownicons[zx][zy]->display();

          if ( actshownicons[xp][yp] ) {
             actshownicons[xp][yp]->iconpressed();

             displaymessage2( actshownicons[xp][yp]->getinfotext() );
             infotextshown = actshownicons[xp][yp];
          }
          zx = xp;
          zy = yp;
       }
       releasetimeslice();
   }

   if ( actshownicons[xp][yp] ) {
      actshownicons[zx][zy]->display();
      if ( actshownicons[xp][yp]->available() )
         actshownicons[xp][yp]->exec();
   }

   mousevisible ( false );
   setmouseposition ( oldmousex, oldmousey );
   mousevisible ( true );

}



template<class T>
void   GuiHost<T>::checkforkey ( tkey key, int keyprn )
{
   if ( key == ct_enter ) {
      if ( actshownicons[0][0] )
         bi2control();
   } else
      getfirsticon()->checkforkey( keyprn );
}




template<class T>
void   GuiHost<T>::bi2control (  )
{

      int xp = 0;
      int yp = 0;

      char mss = getmousestatus ();
      if (mss == 2)
         mousevisible(false);

      actshownicons[xp][yp]->iconpressed();

      int msg = 0;
      displaymessage2( actshownicons[xp][yp]->getinfotext() );
      infotextshown = actshownicons[xp][yp];

      int zx = xp;
      int zy = yp;

      int newfirstshownline = firstshownline;

      tkey ky;

      #ifdef NEWKEYB
      while (skeypress(ct_enter) | skeypress(ct_enterk)) {
      #else
      do {
      #endif

            if (keypress()) {
               ky = r_key();
               if ((ky == ct_up) || (ky == ct_8k)) {
                  if (yp > 0)
                     yp--;
                  if ( yp < newfirstshownline )
                     newfirstshownline = yp;
               }
               if ((ky == ct_down) || (ky == ct_2k)) {
                  if ( actshownicons[xp][yp+1] )
                     yp++;
                  if ( yp > newfirstshownline+iconspaintable/columncount-1 )
                     newfirstshownline = yp - (iconspaintable/columncount-1);
               }
               if ((ky == ct_right) || (ky == ct_6k)) {
                  if (xp < columncount-1) {
                     if ( actshownicons[xp+1][yp] )
                        xp++;
                  }
                  else
                     if ( actshownicons[0][yp+1] ) {
                        xp = 0;
                        yp++;
                        if ( yp > newfirstshownline+iconspaintable/columncount-1 )
                           newfirstshownline = yp - (iconspaintable/columncount-1);
                     }
               }
               if ((ky == ct_left) || (ky == ct_4k)) {
                  if (xp > 0) {
                     xp--;
                  }
                  else
                     if (yp > 0) {
                        yp--;
                        xp = columncount-1;
                        if ( yp < newfirstshownline )
                           newfirstshownline = yp;
                     }
               }
               if ( ky == ct_esc )
                  break;
            }
            if ((zx != xp) || (zy != yp)) {
               actshownicons[zx][zy]->display();
               if ( firstshownline == newfirstshownline )
                  actshownicons[xp][yp]->iconpressed();
               else {
                  firstshownline = newfirstshownline;
                  for ( int iy = firstshownline; iy < firstshownline+iconspaintable/columncount; iy++ )
                    for ( int ix = 0; ix < columncount; ix++ )
                       if ( iy == yp && ix == xp )
                          actshownicons[ix][iy]->iconpressed();
                       else
                          if ( actshownicons[ix][iy] )
                             actshownicons[ix][iy]->display();
                          else
                             putbackground ( ix, iy - firstshownline );


               }
               msg = 0;
               displaymessage2( actshownicons[xp][yp]->getinfotext() );
               infotextshown = actshownicons[xp][yp];
               zx = xp;
               zy = yp;
            }
            releasetimeslice();
      #ifdef NEWKEYB
      }
      #else
      } while ( ky != ct_enter );
      #endif

      actshownicons[xp][yp]->display();
      if ( ky != ct_esc )
         if ( actshownicons[xp][yp]->available() )
            actshownicons[xp][yp]->exec();


      if ( msg == actdisplayedmessage )
         displaymessage2("");
      infotextshown = NULL;

      if (mss == 2)
         mousevisible(true);

      firstshownline = 0;

      actgui->painticons();

}



template<class T>
void   GuiHost<T>::init ( int resolutionx, int resolutiony )
{
   guixpos = resolutionx - guixdif;
   iconspaintable =  (resolutiony - 360) / ( guiiconsizey + guigapy ) * columncount;
}

template<class T>
void   GuiHost<T>::restorebackground ( void )
{
   collategraphicoperations cgo ( guixpos, guiypos, guixpos + columncount*(guiiconsizex + guigapx) + guiiconsizex, guiypos + iconspaintable/columncount*(guiiconsizey + guigapy) + guiiconsizey );
   for (int j = 0; j < iconspaintable/columncount ; j++ )
      for (int i = 0; i< columncount ; i++) {
         putbackground ( i, j );
         actshownicons[i][j] = NULL;
      } /* endfor */
}


template<class T>
T GuiHost<T> :: getfirsticon( void )
{
   return first_icon;
}


template<class T>
void GuiHost<T> :: setfirsticon( T ic )
{
   first_icon = ic;
}

template<class T>
void GuiHost<T>::reset ( void )
{
   restorebackground();
   numpainted = 0;
   infotextshown = NULL;
   for (int i = 0; i < iconspaintable ; i++ )
      actshownicons[i%columncount][i/columncount] = NULL;
}



void   tguihoststandard     :: init ( int resolutionx, int resolutiony )
{
//   chainiconstohost ( &icons.movement );
   StandardBaseGuiHost::init ( resolutionx, resolutiony );
}


void   tguihoststandard :: bi2control (  )
{
//   icons.cancel.display();
   StandardBaseGuiHost::bi2control (  );
}



tnguiicon::tnguiicon ( void )
{
   for (int i = 0; i < 8; i++) {
      picture[i] = NULL;
      picturepressed[i] = NULL;
   }

   host = NULL;
   priority = 0;
   x = -1;
   lasticonsize = -1;
}


void      tnguiicon::setnxt   ( pnguiicon ts )
{
   next = ts ;
}

pnguiicon   tnguiicon::nxt( void )
{
   return next;
}

/*
pnguiicon   tnguiicon::frst( void )
{
   return first;
}

void        tnguiicon::setfrst  ( pnguiicon ts )
{
   first = ts;
}
*/

void        tnguiicon ::  seticonsize  ( int size )
{
   lasticonsize = size;
}


int         tnguiicon ::  count ( void )
{
   if ( available() )
      if ( nxt() )
         return nxt()->count() + 1;
      else
         return 1;
   else
      if ( nxt() )
         return nxt()->count();
      else
         return 0;
}



void tnguiicon::sort( pnguiicon last )
{
   pnguiicon temp ;

   if ( frst()->priority < priority ) {
      temp = nxt();
      setnxt( frst() );
      setfrst ( this );
      last->setnxt( temp );
      if ( temp )
         temp->sort ( last );
   } else {
      temp = frst();
      if ( temp != this ) {
         while ( temp->nxt() != this  && ( temp->nxt()->priority >= priority ) )
            temp = temp->nxt();

         if ( temp->nxt()->priority < priority ) {
            pnguiicon temp2 = nxt();
            setnxt ( temp->nxt() );
            temp->setnxt( this  );
            last->setnxt( temp2 );
            if ( temp2 )
               temp2->sort ( last );
         } else
            if ( nxt() )
               nxt()->sort ( this );
      } else
         if ( nxt () )
            nxt()->sort ( this );
   }
}

void   tnguiicon::loaddata ( void )
{
   std::string tempfilename = filename + ".nic";

   {
     tnfilestream stream ( tempfilename.c_str() , tnstream::reading );
     loadspecifics( &stream );
   }

   if ( nxt () )
      nxt()->loaddata ();
}


void  tnguiicon::loadspecifics( pnstream stream )
{
   tnguiiconfiledata  rawdata;
   for ( int i = 0; i < 8; ++i )
      rawdata.picture[i] = (void*) stream->readInt();
   for ( int i = 0; i < 8; ++i )
      rawdata.picturepressed[i] = (void*) stream->readInt();

   rawdata.infotext = (char*) stream->readInt();
   for ( int i = 0; i < 6; ++i )
      for ( int j = 0; j < 6; ++j )
         rawdata.keys[i][j] = stream->readInt();
   rawdata.priority = stream->readInt();

   if ( rawdata.infotext )
      stream->readTextString ( infotext );

   priority = rawdata.priority;
   for (int i = 0; i < 8; i++) {
      int  w;

      if ( rawdata.picture[i] )
         stream->readrlepict ( &picture[i], false, &w );

      if ( rawdata.picturepressed[i] )
         stream->readrlepict ( &picturepressed[i], false, &w );
   }
   memcpy ( keys, rawdata.keys, sizeof ( keys ));
}


const char* tnguiicon::getinfotext  ( void )
{
   return infotext.c_str();
}

void  tnguiicon::iconpressed ( void )
{
//   if ( host->paintsize != lasticonsize )
      host->returncoordinates ( this, &x,  &y );

   if ( host->paintsize == 0 ) {
      char* buf = new char [ imagesize ( 8,8, 40,26 )];

      collategraphicoperations cgo ( x, y, x + guiiconsizex, y + guiiconsizey );
      display();
      getimage ( x+8, y+8, x+40, y+26, buf );
      putspriteimage ( x, y, icons.guiknopf );
      putimage ( x+9, y+9, buf );

      delete[] buf;

   } else {
      char* buf = new char [ imagesize ( 4,4, 20,13 )];

      collategraphicoperations cgo ( x, y, x + guismalliconsizex, y + guismalliconsizey );
      getimage ( x+4, y+4, x+20, y+13, buf );
      putspriteimage ( x, y, halfpict ( icons.guiknopf ) );
      putimage ( x+5, y+5, buf );

      delete[] buf;
   }
}

int  tnguiicon::pressedbymouse( void )
{
   if ( host->paintsize != lasticonsize )
      host->returncoordinates ( this, &x,  &y );

   if ( x > 0 ){
      int taste = mouseparams.taste;

      int xs;
      int ys;
      if ( host->paintsize == 0 ) {
         xs = guiiconsizex;
         ys = guiiconsizey;
      } else {
         xs = guismalliconsizex;
         ys = guismalliconsizey;
      }

      if ( ! ( mouseparams.x >= x   &&  mouseparams.y >= y   &&  mouseparams.x <= x+xs   &&  mouseparams.y <= y+ys  ))
         return 0;

      iconpressed ();

      while ( mouseparams.x >= x   &&
              mouseparams.y >= y   &&
              mouseparams.x <= x+xs   &&
              mouseparams.y <= y+ys   &&
              mouseparams.taste == taste ) {
                 releasetimeslice();
     }
     display();
     if ( mouseparams.x >= x   &&
          mouseparams.y >= y   &&
          mouseparams.x <= x+xs   &&
          mouseparams.y <= y+ys &&
          mouseparams.taste != taste )
     {
            host->cleanup();
            SoundList::getInstance().playSound( SoundList::menu_ack );
            if ( available() )
               exec();
            return 1;
     }
   }
   return 0;
}

void  tnguiicon::checkforkey  ( tkey key )
{
   if ( available () )
      for (int i = 0; i < 6 ; i++ )

         if ( keys[0][i] && key )
           if ( tolower(char2key( keys[0][i] )) == tolower(key) ) {
              if ( available() )
                 exec();
              return;
            }

   if ( nxt () )
      nxt()->checkforkey( key );
}

void   tnguiicon::paintifavail ( void )
{
   if ( available() )
      display();
   else
      x = -1;

   if ( nxt () )
      nxt()->paintifavail();
}


void   tnguiicon::sethost ( BasicGuiHost* hst )
{
   host = hst;
   if ( nxt () )
      nxt()->sethost( hst );
}

void tnguiicon::display      ( void )
{
   host->returncoordinates ( this, &x, &y );

   if ( x == -1   ||    y == -1 )
      return;

   putpict ( picture[0] );
}




#ifndef _NoStaticClassMembers_
pnsguiicon tnsguiicon ::first = NULL;
#endif

tnsguiicon::tnsguiicon ( void )
{
   setnxt ( first );
   first = this;
}



pnguiicon   tnsguiicon::frst( void )
{
   return first;
}

void        tnsguiicon::setfrst  ( pnguiicon ts )
{
   first = (pnsguiicon) ts;
}














void tnguiicon::putpict ( void* buf )
{
   if ( host->paintsize != lasticonsize )
      host->returncoordinates ( this, &x,  &y );

   if ( host->paintsize == 0 ) {
      collategraphicoperations cgo ( x, y, x + guiiconsizex, y + guiiconsizey );
      putspriteimage ( x, y, buf );
   } else {
      collategraphicoperations cgo ( x, y, x + guismalliconsizex, y + guismalliconsizey );
      putspriteimage ( x, y, halfpict ( buf ) );
   }
}


tnguiicon:: ~tnguiicon (  )
{
   for (int i = 0; i < 8; i++) {
      if ( picture[i] )
         delete[] picture[i];

      if ( picturepressed[i] )
         delete[] picturepressed[i];
   }
}



#if 0



tnsguiiconmove::tnsguiiconmove  ( void )
               :vehicleMovement ( NULL )
{
   filename = "movement";
}


int   tnsguiiconmove::available    ( void )
{
   if (moveparams.movestatus == 0 && pendingVehicleActions.actionType == vat_nothing ) {
      Vehicle* eht = getactfield()->vehicle;
      if ( eht )
         if ( eht->color == actmap->actplayer * 8)
            if ( vehicleMovement.available ( eht ))
               return 1;
   }
   else
      if ( pendingVehicleActions.actionType == vat_move ) {
         switch ( pendingVehicleActions.move->getStatus() ) {
           case 2: return pendingVehicleActions.move->reachableFields.isMember ( getxpos(), getypos() );
           case 3: return pendingVehicleActions.move->path.rbegin()->x == getxpos() && pendingVehicleActions.move->path.rbegin()->y == getypos();
         } /* endswitch */
      }

   return 0;
}

void  tnsguiiconmove::exec         ( void )
{
   if ( moveparams.movestatus == 0 && pendingVehicleActions.actionType == vat_nothing ) {
      new VehicleMovement ( &defaultMapDisplay, &pendingVehicleActions );

      int mode = 0;
      if (  skeypress( ct_lshift ) ||  skeypress ( ct_rshift ))
         mode |= VehicleMovement::DisableHeightChange;

      int res = pendingVehicleActions.move->execute ( getactfield()->vehicle, -1, -1, 0, -1, mode );
      if ( res < 0 ) {
         dispmessage2 ( -res, NULL );
         delete pendingVehicleActions.action;
         return;
      }
      for ( int i = 0; i < pendingVehicleActions.move->reachableFields.getFieldNum(); i++ )
         pendingVehicleActions.move->reachableFields.getField( i ) ->a.temp = 1;

      // if ( !CGameOptions::Instance()->dontMarkFieldsNotAccessible_movement )
      for ( int j = 0; j < pendingVehicleActions.move->reachableFieldsIndirect.getFieldNum(); j++ )
         pendingVehicleActions.move->reachableFieldsIndirect.getField( j ) ->a.temp2 = 2;
      displaymap();

   } else {
     if ( !pendingVehicleActions.move )
        return;

     int ms = pendingVehicleActions.move->getStatus();
     if ( moveparams.movestatus == 0 && pendingVehicleActions.actionType == vat_move &&  (ms == 2 || ms == 3 )) {
        int res;
        res = pendingVehicleActions.move->execute ( NULL, getxpos(), getypos(), pendingVehicleActions.move->getStatus(), -1, 0 );
        if ( res >= 0 && CGameOptions::Instance()->fastmove && ms == 2 ) {
           actmap->cleartemps(7);
           displaymap();
           res = pendingVehicleActions.move->execute ( NULL, getxpos(), getypos(), pendingVehicleActions.move->getStatus(), -1, 0 );
        } else {
           if ( ms == 2 ) {
              actmap->cleartemps(7);
              for ( int i = 0; i < pendingVehicleActions.move->path.size(); i++ )
                 actmap->getField( pendingVehicleActions.move->path[i]) ->a.temp = 1;
              displaymap();
           } else {
              actmap->cleartemps(7);
              displaymap();
           }
        }


        if ( res < 0 ) {
           dispmessage2 ( -res, NULL );
           delete pendingVehicleActions.action;
           return;
        }

        if ( pendingVehicleActions.move->getStatus() == 1000 ) {
           delete pendingVehicleActions.move;

           if ( CGameOptions::Instance()->smallguiiconopenaftermove ) {
              actgui->painticons();
              actgui->paintsmallicons ( CGameOptions::Instance()->mouse.smallguibutton, 0 );
           }
        }

     }
   }
   updateFieldInfo();

}

void  tnsguiiconmove::display      ( void )
{
   host->returncoordinates ( this, &x, &y );

   if ( x == -1   ||    y == -1 )
      return;

   int h = chfahrend;
   if ( !pendingVehicleActions.move  ) {
      if ( getactfield()->vehicle )
         h = getactfield()->vehicle->typ->height ;
   } else
      h = pendingVehicleActions.move->getVehicle()->typ->height ;

   if ( h >= chtieffliegend )
      putpict ( picture[1] );
   else
      if ( h & ( chschwimmend | chgetaucht | chtiefgetaucht ) )
         putpict ( picture[2] );
      else
         putpict ( picture[0] );

}







tnsguiiconattack::tnsguiiconattack ( void )
                 :vehicleAttack ( NULL )
{
   filename =  "attack" ;
}



int   tnsguiiconattack::available    ( void )
{
   if (moveparams.movestatus == 0 && pendingVehicleActions.actionType == vat_nothing ) {
      Vehicle* eht = getactfield()->vehicle;
      if ( eht )
         if ( eht->color == actmap->actplayer * 8)
            if ( vehicleAttack.available ( eht ))
               return 1;
   }
   return 0;
}

void  tnsguiiconattack::exec         ( void )
{
   if ( moveparams.movestatus == 0 && pendingVehicleActions.actionType == vat_nothing ) {
      new VehicleAttack ( &defaultMapDisplay, &pendingVehicleActions );

      int res;
      res = pendingVehicleActions.attack->execute ( getactfield()->vehicle, -1, -1, 0, 0, -1 );
      if ( res < 0 ) {
         dispmessage2 ( -res, NULL );
         delete pendingVehicleActions.action;
         return;
      }

      int i;
      for ( i = 0; i < pendingVehicleActions.attack->attackableVehicles.getFieldNum(); i++ )
         pendingVehicleActions.attack->attackableVehicles.getField( i ) ->a.temp = 1;
      for ( i = 0; i < pendingVehicleActions.attack->attackableBuildings.getFieldNum(); i++ )
         pendingVehicleActions.attack->attackableBuildings.getField( i ) ->a.temp = 1;
      for ( i = 0; i < pendingVehicleActions.attack->attackableObjects.getFieldNum(); i++ )
         pendingVehicleActions.attack->attackableObjects.getField( i ) ->a.temp = 1;

      displaymap();

      actgui->restorebackground();
      selectweaponguihost.init( hgmp->resolutionx, hgmp->resolutiony );
      //selectweaponguihost.restorebackground();
      actgui = &selectweaponguihost;
      actgui->painticons();
   }
}





tnsguiiconascent::tnsguiiconascent ( void )
                 :increaseVehicleHeight ( NULL )
{
   filename = "ascent";
}

int   tnsguiiconascent::available    ( void )
{
   if ( moveparams.movestatus == 0 && !pendingVehicleActions.action ) {
      Vehicle* eht = getactfield()->vehicle;
      if ( !eht )
         return 0;
      if (eht->color == actmap->actplayer * 8)
         return increaseVehicleHeight.available ( eht );
   } else
      if ( pendingVehicleActions.actionType == vat_ascent ) {
         switch ( pendingVehicleActions.ascent->getStatus() ) {
           case 2: return pendingVehicleActions.ascent->reachableFields.isMember ( getxpos(), getypos() );
           case 3: return pendingVehicleActions.ascent->path.rbegin()->x == getxpos() && pendingVehicleActions.ascent->path.rbegin()->y == getypos();
         } /* endswitch */
      }
   return 0;
}

void  tnsguiiconascent::exec         ( void )
{
   if ( moveparams.movestatus == 0 && pendingVehicleActions.actionType == vat_nothing ) {
      new IncreaseVehicleHeight ( &getDefaultMapDisplay(), &pendingVehicleActions );

      bool simpleMode = false;
      if (  skeypress( ct_lshift ) ||  skeypress ( ct_rshift ))
         simpleMode = true;

      int res = pendingVehicleActions.ascent->execute ( getactfield()->vehicle, -1, -1, 0, getactfield()->vehicle->height << 1, simpleMode );
      if ( res < 0 ) {
         dispmessage2 ( -res, NULL );
         delete pendingVehicleActions.action;
         return;
      }

      if ( res == 1000 )
         delete pendingVehicleActions.action;
      else {
         for ( int i = 0; i < pendingVehicleActions.ascent->reachableFields.getFieldNum(); i++ )
            pendingVehicleActions.ascent->reachableFields.getField( i ) ->a.temp = 1;
         displaymap();
      }

   } else
     if ( moveparams.movestatus == 0 && pendingVehicleActions.actionType == vat_ascent &&  (pendingVehicleActions.ascent->getStatus() == 2 || pendingVehicleActions.ascent->getStatus() == 3 )) {
        int xdst = getxpos();
        int ydst = getypos();
        int res = pendingVehicleActions.ascent->execute ( NULL, xdst, ydst, pendingVehicleActions.ascent->getStatus(), -1, 0 );
        if ( res >= 0 && CGameOptions::Instance()->fastmove ) {
           actmap->cleartemps(7);
           displaymap();
           // if the status is 1000 at this position, the unit has been shot down by reactionfire before initiating the height change
           if ( res < 1000 )
              res = pendingVehicleActions.ascent->execute ( NULL, xdst, ydst, pendingVehicleActions.ascent->getStatus(), -1, 0 );
        } else {
           actmap->cleartemps(7);
           if ( res < 1000 )
              for ( int i = 0; i < pendingVehicleActions.ascent->path.size(); i++ )
                 actmap->getField( pendingVehicleActions.ascent->path[i]) ->a.temp = 1;
           displaymap();
        }

        if ( res < 0 ) {
           dispmessage2 ( -res, NULL );
           delete pendingVehicleActions.action;
           updateFieldInfo();
           return;
        }

        if ( pendingVehicleActions.ascent->getStatus() == 1000 ) {
           delete pendingVehicleActions.ascent;

/*           if ( CGameOptions::Instance()->smallguiiconopenaftermove ) {
              actgui->painticons();
              actgui->paintsmallicons ( CGameOptions::Instance()->mouse.smallguibutton, 0 );
           }
           */
        }
     }

   updateFieldInfo();
}

void  tnsguiiconascent::display      ( void )
{
   host->returncoordinates ( this, &x, &y );

   if ( x == -1   ||    y == -1 )
      return;

   int h = chfahrend;
   if ( !pendingVehicleActions.ascent ) {
      if ( getactfield()->vehicle )
         h = getactfield()->vehicle->typ->height ;
   } else
      h = pendingVehicleActions.ascent->getVehicle()->typ->height ;

   if ( h >= chtieffliegend )
      putpict ( picture[0] );
   else
      putpict ( picture[1] );

}












tnsguiicondescent::tnsguiicondescent ( void )
                 : decreaseVehicleHeight ( NULL )
{
   filename = "descent";
}



int   tnsguiicondescent::available    ( void )
{
   if ( moveparams.movestatus == 0 && !pendingVehicleActions.action ) {
      Vehicle* eht = getactfield()->vehicle;
      if ( !eht )
         return 0;

      if (eht->color == actmap->actplayer * 8)
         return decreaseVehicleHeight.available ( eht );
   } else
      if ( pendingVehicleActions.actionType == vat_descent ) {
         switch ( pendingVehicleActions.descent->getStatus() ) {
           case 2: return pendingVehicleActions.descent->reachableFields.isMember ( getxpos(), getypos() );
           case 3: return pendingVehicleActions.descent->path.rbegin()->x == getxpos() && pendingVehicleActions.descent->path.rbegin()->y == getypos();
         } /* endswitch */
      }
   return 0;
}

void  tnsguiicondescent::exec         ( void )
{
   if ( moveparams.movestatus == 0 && pendingVehicleActions.actionType == vat_nothing ) {
      new DecreaseVehicleHeight ( &getDefaultMapDisplay(), &pendingVehicleActions );


      bool simpleMode = false;
      if (  skeypress( ct_lshift ) ||  skeypress ( ct_rshift ))
         simpleMode = true;

      int res = pendingVehicleActions.descent->execute ( getactfield()->vehicle, -1, -1, 0, getactfield()->vehicle->height >> 1, simpleMode );
      if ( res < 0 ) {
         dispmessage2 ( -res, NULL );
         delete pendingVehicleActions.action;
         return;
      }

      if ( res == 1000 )
         delete pendingVehicleActions.action;
      else {
         for ( int i = 0; i < pendingVehicleActions.descent->reachableFields.getFieldNum(); i++ )
            pendingVehicleActions.descent->reachableFields.getField( i ) ->a.temp = 1;
         displaymap();
      }

   } else
     if ( moveparams.movestatus == 0 && pendingVehicleActions.actionType == vat_descent &&  (pendingVehicleActions.descent->getStatus() == 2 || pendingVehicleActions.descent->getStatus() == 3 )) {
        int res = pendingVehicleActions.descent->execute ( NULL, getxpos(), getypos(), pendingVehicleActions.descent->getStatus(), -1, 0 );
        if ( res >= 0 && CGameOptions::Instance()->fastmove ) {
           actmap->cleartemps(7);
           displaymap();
           // if the status is 1000 at this position, the unit has been shot down by reactionfire before initiating the height change
           if ( res < 1000 )
              res = pendingVehicleActions.descent->execute ( NULL, getxpos(), getypos(), pendingVehicleActions.descent->getStatus(), -1, 0 );
        } else {
           actmap->cleartemps(7);
           if ( res < 1000 )
              for ( int i = 0; i < pendingVehicleActions.descent->path.size(); i++ )
                 actmap->getField( pendingVehicleActions.descent->path[i]) ->a.temp = 1;
           displaymap();
        }


        if ( res < 0 ) {
           dispmessage2 ( -res, NULL );
           delete pendingVehicleActions.action;
           updateFieldInfo();
           return;
        }

        if ( pendingVehicleActions.descent->getStatus() == 1000 ) {
           delete pendingVehicleActions.descent;

/*           if ( CGameOptions::Instance()->smallguiiconopenaftermove ) {
              actgui->painticons();
              actgui->paintsmallicons ( CGameOptions::Instance()->mouse.smallguibutton, 0 );
           }
           */
        }
     }
   updateFieldInfo();
}

void  tnsguiicondescent::display      ( void )
{
   host->returncoordinates ( this, &x, &y );

   if ( x == -1   ||    y == -1 )
      return;

   int h = chfahrend;
   if ( !pendingVehicleActions.descent ) {
      if ( getactfield()->vehicle )
         h = getactfield()->vehicle->typ->height ;
   } else
      h = pendingVehicleActions.descent->getVehicle()->typ->height ;

   if ( h >= chtieffliegend )
      putpict ( picture[0] );
   else
      putpict ( picture[1] );

}







tnsguiiconexternalloading::tnsguiiconexternalloading ( void )
{
   filename = "extload";
}

int   tnsguiiconexternalloading::available    ( void )
{
   if (moveparams.movestatus == 130)
      if ( getactfield()->a.temp == 123 )
         return 1;

   return 0;
}

void  tnsguiiconexternalloading::exec         ( void )
{
   moveparams.movestatus++;
}









tnsguiiconputmine::tnsguiiconputmine ( void )
{
   filename = "putmine" ;
}



int   tnsguiiconputmine::available    ( void )
{
   pfield fld = getactfield();
   if (moveparams.movestatus == 0 && pendingVehicleActions.actionType == vat_nothing)
      if ( fld->vehicle )
         if (fld->vehicle->color == actmap->actplayer * 8)
            if (fld->vehicle->typ->functions & cfminenleger )
               if ( !fld->vehicle->attacked )
                  return true;
   return 0;
}

void  tnsguiiconputmine::exec         ( void )
{
   legemine(0, 0);
   updateFieldInfo();
   displaymap();
}

void tnsguiiconputmine::loadspecifics ( pnstream stream )
{
   char buf[1000];
   char buf2[1000];
   tnsguiicon::loadspecifics ( stream );
   itoa ( mineputmovedecrease, buf2, 10 );
   strcat ( buf2, "/10" );
   sprintf( buf, infotext.c_str(), buf2 );
   infotext = buf;
}


int   tnsguiiconputgroundmine::available    ( void )
{
   if (moveparams.movestatus == 90) {
      pfield fld = getactfield();
      if ( (fld->bdt & (getTerrainBitType( cbwater ).flip())).any() )
         if ( fld->a.temp & 1)
            if ( fld->mines.empty() || fld->mineowner() == actmap->actplayer )
               return true;
   }
   return 0;
}



tnsguiiconputantitankmine::tnsguiiconputantitankmine ( void )
{
   filename = "tankmine" ;
}

void  tnsguiiconputantitankmine::exec         ( void )
{
   legemine(cmantitankmine, 1 );
   displaymap();
}




tnsguiiconputantipersonalmine::tnsguiiconputantipersonalmine ( void )
{
   filename = "persmine";
}

void  tnsguiiconputantipersonalmine::exec         ( void )
{
   legemine(cmantipersonnelmine, 1);
}




tnsguiiconputfloatingmine::tnsguiiconputfloatingmine ( void )
{
   filename = "floatmin" ;
}

void  tnsguiiconputfloatingmine::exec         ( void )
{
   legemine(cmfloatmine, 1);
   displaymap();
}

int   tnsguiiconputfloatingmine::available    ( void )
{
   if (moveparams.movestatus == 90) {
      pfield fld = getactfield();
      if ( (fld->bdt & getTerrainBitType(cbwater0)).any()
           || (fld->bdt & getTerrainBitType(cbwater1)).any()
           || (fld->bdt & getTerrainBitType(cbwater2)).any()
           || (fld->bdt & getTerrainBitType(cbwater3)).any() )
         if (fld->a.temp & 1 )
            if ( fld->mines.empty() || fld->mineowner() == actmap->actplayer )
               return true;
   }
   return 0;
}


tnsguiiconputmooredmine::tnsguiiconputmooredmine ( void )
{
   filename = "moormine" ;
}

int   tnsguiiconputmooredmine::available    ( void )
{
   if (moveparams.movestatus == 90) {
      pfield fld = getactfield();
      if ( (fld->typ->art & getTerrainBitType(cbwater2)).any()
           || (fld->typ->art & getTerrainBitType(cbwater3)).any() )
         if (fld->a.temp & 1 )
            if ( fld->mines.empty() || fld->mineowner() == actmap->actplayer )
               return true;
   }
   return 0;
}


void  tnsguiiconputmooredmine::exec         ( void )
{
   legemine(cmmooredmine,1 );
   displaymap();
}












tnsguiiconremovemine::tnsguiiconremovemine ( void )
{
   filename = "remomine";
}



int   tnsguiiconremovemine::available    ( void )
{
   if (moveparams.movestatus == 90) {
      pfield fld = getactfield();
      if ( fld->a.temp )
         return !fld->mines.empty();
   }
   return 0;
}

void  tnsguiiconremovemine::exec         ( void )
{
   legemine(0, -1);
   displaymap();
}







tnsguiiconbuildany::tnsguiiconbuildany ( void )
{
   filename = "buildany" ;
}



int   tnsguiiconbuildany::available    ( void )
{
   pfield fld = getactfield();
   if ( fld->vehicle )
      if (fld->vehicle->color == actmap->actplayer * 8)
         if ( fld->vehicle->typ->objectsBuildable.size() || fld->vehicle->typ->objectsRemovable.size() || fld->vehicle->typ->objectGroupsBuildable.size() || fld->vehicle->typ->objectGroupsRemovable.size())
            if (moveparams.movestatus == 0 && pendingVehicleActions.actionType == vat_nothing)
               if ( !fld->vehicle->attacked )
                  return true;
   return 0;
}

void  tnsguiiconbuildany::exec         ( void )
{
//   setspec(0);
   displaymap();
}




tnsguiiconrepair::tnsguiiconrepair ( void )
                 :service(NULL, NULL )
{
   filename = "repair" ;
}

int   tnsguiiconrepair::available    ( void )
{
   if (moveparams.movestatus == 0 && pendingVehicleActions.actionType == vat_nothing) {
      pfield fld = getactfield();
      if ( fld->vehicle )
         if (fld->vehicle->color == actmap->actplayer * 8)
            if ( service.available ( fld->vehicle ))
               if ( service.getServices( fld->vehicle) & (1 << VehicleService::srv_repair ))
                  return 1;
   } else
      if ( pendingVehicleActions.actionType == vat_service && pendingVehicleActions.service->guimode == 1 ) {
         pfield fld = getactfield();
         if ( fld->vehicle ) {
            // if ( pendingVehicleActions.service->getServices ( fld->vehicle) & ( 1 << VehicleService::srv_repair) ) {
            VehicleService::TargetContainer::iterator i = pendingVehicleActions.service->dest.find(fld->vehicle->networkid);
            if ( i != pendingVehicleActions.service->dest.end() )
               for ( int j = 0; j < i->second.service.size(); j++ )
                  if ( i->second.service[j].type == VehicleService::srv_repair )
                     return 1;
         }
      }

   return 0;
}

void  tnsguiiconrepair::exec         ( void )
{
   if ( pendingVehicleActions.actionType == vat_nothing ) {
      VehicleService* vs = new VehicleService ( &getDefaultMapDisplay(), &pendingVehicleActions );
      vs->guimode = 1;
      int res = vs->execute ( getactfield()->vehicle, -1, -1, 0, -1, -1 );
      if ( res < 0 ) {
         dispmessage2 ( -res );
         delete vs;
         return;
      }
      int fieldCount = 0;
      for ( VehicleService::TargetContainer::iterator i = pendingVehicleActions.service->dest.begin(); i != pendingVehicleActions.service->dest.end(); i++ ) {
         pfield fld = getfield ( i->second.dest->xpos, i->second.dest->ypos );
         if ( fld != getactfield())
            for ( int j = 0; j < i->second.service.size(); j++ )
               if ( i->second.service[j].type == VehicleService::srv_repair ) {
                  fieldCount++;
                  fld->a.temp = 1;
               }
      }
      if ( !fieldCount ) {
         delete vs;
         dispmessage2 ( 211 );
      } else
         displaymap();
   } else {
      for ( VehicleService::TargetContainer::iterator i = pendingVehicleActions.service->dest.begin(); i != pendingVehicleActions.service->dest.end(); i++ )
         if ( i->second.dest == getactfield()->vehicle )
            // for ( vector<VehicleService::Service>::iterator j = i->second->service.begin(); j != i->second->service.end(); j++ )
            for ( int j = 0; j < i->second.service.size(); j++ )
               if ( i->second.service[j].type == VehicleService::srv_repair )
                  pendingVehicleActions.service->execute ( NULL, getactfield()->vehicle->networkid, -1, 2, j, i->second.service[j].minAmount );


      delete pendingVehicleActions.service;
      actmap->cleartemps(7);
      displaymap();
      updateFieldInfo();
   }
}

void tnsguiiconrepair::loadspecifics ( pnstream stream )
{
   char buf[1000];
   char buf2[1000];
   tnsguiicon::loadspecifics ( stream );
   itoa ( movement_cost_for_repairing_unit, buf2, 10 );
   strcat ( buf2, "/10" );
   sprintf( buf, infotext.c_str(), buf2 );
   infotext = buf;
}






tnsguiiconrefuel::tnsguiiconrefuel ( void )
                 :service(NULL, NULL )
{
   filename =  "refuel" ;
}



int   tnsguiiconrefuel::available    ( void )
{

   if (moveparams.movestatus == 0 && pendingVehicleActions.actionType == vat_nothing) {
      pfield fld = getactfield();
      if ( fld->vehicle )
         if (fld->vehicle->color == actmap->actplayer * 8)
            if ( service.available ( fld->vehicle ))
               if ( service.getServices( fld->vehicle) & ((1 << VehicleService::srv_resource ) | (1 << VehicleService::srv_ammo )) )
                  return 1;

      if ( fld->building )
         if ( fld->building->color == actmap->actplayer * 8)
             if ( fld->building->typ->special & (cgexternalloadingb | cgexternalresourceloadingb | cgexternalammoloadingb ))
                return 1;
   } else
      if ( pendingVehicleActions.actionType == vat_service && pendingVehicleActions.service->guimode == 2) {
         pfield fld = getactfield();
         if ( fld->vehicle ) {
            VehicleService::TargetContainer::iterator i = pendingVehicleActions.service->dest.find(fld->vehicle->networkid);
            if ( i != pendingVehicleActions.service->dest.end() )
               for ( int j = 0; j < i->second.service.size(); j++ )
                  if (   i->second.service[j].type == VehicleService::srv_resource
                      || i->second.service[j].type == VehicleService::srv_ammo )
                     return 1;
         }
      }

   return 0;
}

void  tnsguiiconrefuel::exec         ( void )
{
   if ( pendingVehicleActions.actionType == vat_nothing ) {
      VehicleService* vs = new VehicleService ( &getDefaultMapDisplay(), &pendingVehicleActions );
      pendingVehicleActions.service->guimode = 2;
      int res = vs->execute ( getactfield()->vehicle, getxpos(), getypos(), 0, -1, -1 );
      if ( res < 0 ) {
         dispmessage2 ( -res );
         delete vs;
         return;
      }
      int fieldCount = 0;
      for ( VehicleService::TargetContainer::iterator i = pendingVehicleActions.service->dest.begin(); i != pendingVehicleActions.service->dest.end(); i++ ) {
         pfield fld = getfield ( i->second.dest->xpos, i->second.dest->ypos );
         if ( fld != getactfield())
            for ( int j = 0; j < i->second.service.size(); j++ )
               if (  i->second.service[j].type == VehicleService::srv_ammo
                  || i->second.service[j].type == VehicleService::srv_resource ) {
                  fieldCount++;
                  fld->a.temp = 1;
               }
      }
      if ( !fieldCount ) {
         delete vs;
         dispmessage2 ( 211 );
      } else
         displaymap();
   } else {
      for ( VehicleService::TargetContainer::iterator i = pendingVehicleActions.service->dest.begin(); i != pendingVehicleActions.service->dest.end(); i++ )
         if ( i->second.dest == getactfield()->vehicle )
            for ( int j = 0; j < i->second.service.size(); j++ )
               if (  i->second.service[j].type == VehicleService::srv_ammo
                  || i->second.service[j].type == VehicleService::srv_resource )
                  pendingVehicleActions.service->execute ( NULL, getactfield()->vehicle->networkid, -1, 2, j, i->second.service[j].maxAmount );


      delete pendingVehicleActions.service;
      actmap->cleartemps(7);
      displaymap();
      updateFieldInfo();
   }
}


void  tnsguiiconrefuel::display      ( void )
{
   host->returncoordinates ( this, &x, &y );

   if ( x == -1   ||    y == -1 )
      return;

   int pict = 0;

   /*
   if (moveparams.movestatus == 0 && pendingVehicleActions.actionType == vat_nothing) {
      pfield fld = getactfield();
      if ( fld->vehicle )
         if ( fld->vehicle->typ->functions & cffuelref  )
            pict = 0;
   } else
      if ( getfield(moveparams.movesx,moveparams.movesy)->vehicle->typ->functions & cffuelref  )
         pict = 0;
   */
   putpict ( picture[pict] );
}







tnsguiiconrefueldialog::tnsguiiconrefueldialog ( void )
{
   filename =  "refueld" ;
}



int   tnsguiiconrefueldialog::available    ( void )
{
   priority = 20; // !!
   if ( pendingVehicleActions.service && pendingVehicleActions.service->guimode == 2 && getactfield()->a.temp && getactfield()->vehicle )
         return true;

   return 0;
}

void  tnsguiiconrefueldialog::exec         ( void )
{
   verlademunition( pendingVehicleActions.service, getactfield()->vehicle->networkid );
   delete pendingVehicleActions.service;
   actmap->cleartemps ( 7 );
   displaymap();
   updateFieldInfo();
}







tnsguiiconputbuilding::tnsguiiconputbuilding ( void )
{
   filename = "building";
}



int   tnsguiiconputbuilding::available    ( void )
{
   if ( actmap->getgameparameter(cgp_forbid_building_construction) )
      return 0;

    pfield fld = getactfield();
    if (moveparams.movestatus == 0 && pendingVehicleActions.actionType == vat_nothing) {
       if ( fld->vehicle )
          if ( fld->vehicle->attacked == false && !fld->vehicle->hasMoved() )
             if (fld->vehicle->color == actmap->actplayer * 8)
               if (fld->vehicle->typ->functions & (cfputbuilding | cfspecificbuildingconstruction))
                  return 1;
    }
    else
       if (moveparams.movestatus == 111) {
          if (fld->a.temp == 20)
             return 2;
       }
       else
          if (moveparams.movestatus == 112)
             if (fld->a.temp == 23)
                return 3;

   return 0;
}

void  tnsguiiconputbuilding::exec         ( void )
{
   if (moveparams.movestatus == 0 && pendingVehicleActions.actionType == vat_nothing) {
      putbuildinglevel1();
      displaymap();
   }
   else
      if (moveparams.movestatus == 111) {
         putbuildinglevel2(moveparams.buildingtobuild, getxpos(), getypos());
         displaymap();
      } else
         if (moveparams.movestatus == 112) {
            putbuildinglevel3( getxpos(), getypos());
            displaymap();
            updateFieldInfo();
         }
}


tnsguiicondestructbuilding::tnsguiicondestructbuilding ( void )
{
   filename = "abriss";
}



int   tnsguiicondestructbuilding::available    ( void )
{
    pfield fld = getactfield();
    if (moveparams.movestatus == 0 && pendingVehicleActions.actionType == vat_nothing) {
       if ( fld->vehicle )
          if ( fld->vehicle->attacked == false && !fld->vehicle->hasMoved() )
             if (fld->vehicle->color == actmap->actplayer * 8)
               if ((fld->vehicle->typ->functions & cfputbuilding) || !fld->vehicle->typ->buildingsBuildable.empty() )
                  if ( fld->vehicle->getTank().fuel >= destruct_building_fuel_usage * fld->vehicle->typ->fuelConsumption )
                     return 1;
    }
    else
       if (moveparams.movestatus == 115) {
          if (fld->a.temp == 20)
             return 2;
       }

   return 0;
}

void  tnsguiicondestructbuilding::exec         ( void )
{
   if (moveparams.movestatus == 0 && pendingVehicleActions.actionType == vat_nothing) {
      destructbuildinglevel1( getxpos(), getypos() );
      displaymap();
   }
   else
      if (moveparams.movestatus == 115) {
         destructbuildinglevel2( getxpos(), getypos());
         updateFieldInfo();
         displaymap();
      }
}



tnsguiicondig::tnsguiicondig ( void )
{
   filename =  "dig";
}



int   tnsguiicondig::available    ( void )
{
   pfield fld = getactfield();
   if (fld->vehicle != NULL)
      if (fld->vehicle->color == actmap->actplayer * 8)
         if ( (fld->vehicle->typ->functions &  cfmanualdigger) && !(fld->vehicle->typ->functions &  cfautodigger) )
            if (moveparams.movestatus == 0 && pendingVehicleActions.actionType == vat_nothing)
               if ( actmap->_resourcemode == 0 ) 
                  return fld->vehicle->searchForMineralResourcesAvailable();
   return 0;
}

void  tnsguiicondig::exec         ( void )
{
    getactfield()->vehicle->searchForMineralResources( ) ;
    showresources = 1;
    updateFieldInfo();
    displaymap();
}





tnsguiiconviewmap::tnsguiiconviewmap ( void )
{
   filename = "viewmap";
}



int   tnsguiiconviewmap::available    ( void )
{
   if ( moveparams.movestatus == 0  && pendingVehicleActions.actionType == vat_nothing)
      return true;
   return false;
}

void  tnsguiiconviewmap::exec         ( void )
{
    cursor.hide();
    showmap ();
    displaymap();
    cursor.show();
}





tnsguiiconenablereactionfire::tnsguiiconenablereactionfire ( void )
{
   filename = "reacfire";
}

int   tnsguiiconenablereactionfire::available    ( void )
{
   Vehicle* eht = getactfield()->vehicle;
   if ( eht )
      if ( eht->color == actmap->actplayer * 8)
         if ( eht->reactionfire.getStatus() == Vehicle::ReactionFire::off )
            if ( moveparams.movestatus == 0  && pendingVehicleActions.actionType == vat_nothing)
               if ( eht->weapexist() )
                  for ( int i = 0; i < eht->typ->weapons.count; ++i )
                      if ( eht->typ->weapons.weapon[i].offensive() && eht->typ->weapons.weapon[i].reactionFireShots )
                  return 1;

   return 0;

}

void  tnsguiiconenablereactionfire::exec         ( void )
{
   int res = getactfield()->vehicle->reactionfire.enable();
   if ( res < 0 )
      dispmessage2 ( -res, NULL );
   updateFieldInfo();
   displaymap();
}



tnsguiicondisablereactionfire::tnsguiicondisablereactionfire ( void )
{
   filename = "reacfoff";
}

int   tnsguiicondisablereactionfire::available    ( void )
{
   Vehicle* eht = getactfield()->vehicle;
   if ( eht )
      if ( eht->color == actmap->actplayer * 8)
         if ( eht->reactionfire.getStatus() != Vehicle::ReactionFire::off )
            if ( moveparams.movestatus == 0  && pendingVehicleActions.actionType == vat_nothing)
               // if ( eht->weapexist() )
                  return 1;

   return 0;

}

void  tnsguiicondisablereactionfire::exec         ( void )
{
   getactfield()->vehicle->reactionfire.disable();
   updateFieldInfo();
}







int tnsguiiconcontainer :: containeractive;

tnsguiiconcontainer ::tnsguiiconcontainer ( void )
{
   filename = "loading" ;
   containeractive = 0;
}

int tnsguiiconcontainer :: available    ( void )
{
  pfield fld = getactfield();
  if ( fieldvisiblenow ( fld ))
     if ( !containeractive && !moveparams.movestatus && pendingVehicleActions.actionType == vat_nothing && !pendingVehicleActions.action )
        if ( fld->building  &&  ((fld->building->color == actmap->actplayer * 8) || (fld->building->color == 8*8) ))
           if ( fld->building->getCompletion() == fld->building->typ->construction_steps-1 )
              return 1;
           else
              return 0;
         else
           if ( fld->vehicle && fld->vehicle->typ->maxLoadableUnits  &&  fld->vehicle->color == actmap->actplayer * 8 )
              return 1;
           else
              return 0;

  return 0;
}

void tnsguiiconcontainer :: exec         ( void )
{
   containeractive++;
   pfield fld = getactfield ();
   /*
   if ( fld->vehicle && fld->building )
      displaymessage( "gui.cpp   tnsguiiconcontainer :: exec  ; both unit and building on a field", 1 );
   */
   container( fld->vehicle, fld->building );
   containeractive--;
}


#endif



tnsguiiconcancel::tnsguiiconcancel ( void )
{
   filename = "cancel";
   forcedeneable = 0;
}



int   tnsguiiconcancel::available    ( void )
{
   if ( moveparams.movestatus || pendingVehicleActions.action )
      return 1;
   if ( forcedeneable )
      return 2;
   return 0;
}

void  tnsguiiconcancel::exec         ( void )
{
   if ( moveparams.movestatus || pendingVehicleActions.action ) {
      moveparams.movestatus = 0;
      if ( pendingVehicleActions.action )
         delete pendingVehicleActions.action;

      actmap->cleartemps(7);
      updateFieldInfo();
      displaymap();
   }
}










#if 0

void    tselectobjectcontainerguihost :: init ( int resolutionx, int resolutiony )
{
   SelectObjectBaseGuiHost :: init ( resolutionx, resolutiony );
   icons = new pnputobjectcontainerguiicon[ 1 + objectTypeRepository.getNum() * 2 ] ;
   icons[0] = new tnputobjectcontainerguiicon ( NULL, 1 );
   for (int i = 0; i < objectTypeRepository.getNum() ; i++ )
      if ( objectTypeRepository.getObject_byPos( i ) ) {
         icons[1+i*2] = new tnputobjectcontainerguiicon ( objectTypeRepository.getObject_byPos( i ), 1 );
         icons[1+i*2+1] = new tnputobjectcontainerguiicon ( objectTypeRepository.getObject_byPos( i ), 0 );
      }


   icons[0]->frst()->sethost ( this );
   setfirsticon ( (tnputobjectcontainerguiicon*) icons[0]->frst() );
}


void tselectobjectcontainerguihost ::reset ( void )
{
   SelectObjectBaseGuiHost::reset();
   cancel = 0;
}








void    tselectvehiclecontainerguihost :: init ( int resolutionx, int resolutiony )
{
   SelectVehicleBaseGuiHost :: init ( resolutionx, resolutiony );
   icons = new pnputvehiclecontainerguiicon[ 1 + vehicleTypeRepository.getNum() ] ;
   icons[0] = new tnputvehiclecontainerguiicon ( NULL );
   for (int i = 0; i < vehicleTypeRepository.getNum() ; i++ )
      if ( vehicleTypeRepository.getObject_byPos( i ) )
         icons[1+i] = new tnputvehiclecontainerguiicon ( vehicleTypeRepository.getObject_byPos( i ) );


   icons[0]->frst()->sethost ( this );
   setfirsticon ( (pnputvehiclecontainerguiicon) icons[0]->frst() );

}

void tselectvehiclecontainerguihost ::reset ( Vehicle* _constructingvehicle )
{
   constructingvehicle = _constructingvehicle;

   SelectVehicleBaseGuiHost::reset();
   cancel = 0;
}





tselectbuildingguihost :: tselectbuildingguihost( void )
{
    selectedbuilding = NULL;
    vehicle = NULL;
}

void    tselectbuildingguihost :: init ( int resolutionx, int resolutiony )
{
   SelectBuildingBaseGuiHost :: init ( resolutionx, resolutiony );
   icons = new ( pnputbuildingguiicon[ buildingTypeRepository.getNum() ] );
   for (int i = 0; i < buildingTypeRepository.getNum() ; i++ )
      if ( buildingTypeRepository.getObject_byPos( i ) )
         icons[i] = new tnputbuildingguiicon ( buildingTypeRepository.getObject_byPos( i ) );

   if ( !buildingTypeRepository.getNum() )
      fatalError ( "No buildings found !");
   icons[0]->frst()->sethost ( this );
   setfirsticon ( pnputbuildingguiicon( icons[0]->frst() ));

}

void tselectbuildingguihost ::reset ( Vehicle* v )
{
   vehicle = v;
   SelectBuildingBaseGuiHost::reset();
   selectedbuilding = NULL;

   cancel = 0;
}


#ifndef _NoStaticClassMembers_
pnputbuildingguiicon tnputbuildingguiicon :: first    = NULL;
int             tnputbuildingguiicon :: buildnum = 0;
#endif

tnputbuildingguiicon :: tnputbuildingguiicon ( pbuildingtype bld )
{
   setnxt ( first );
   first = this;

   building = bld;
   buildnum++;
   picture[0]  = building->guibuildicon.toBGI();
   char buf[10000];
   sprintf ( buf, "%s : %d material and %d fuel needed", building->name.c_str(), building->productionCost.material, building->productionCost.fuel );
   infotext = buf;
   if ( building->construction_steps > 1 ) {
      sprintf(buf, " ; %d turns required", building->construction_steps );
      infotext += buf;
   }

}



pnguiicon   tnputbuildingguiicon::frst( void )
{
   return first;
}

void        tnputbuildingguiicon::setfrst  ( pnguiicon ts )
{
   first = (pnputbuildingguiicon) ts;
}

void              tnputbuildingguiicon::exec( void )
{
   tselectbuildingguihost* bldhost = (tselectbuildingguihost*) host;
   bldhost->selectedbuilding = building;
}


int         tnputbuildingguiicon::available( void )
{
   tselectbuildingguihost* bldhost = (tselectbuildingguihost*) host;
   return bldhost->vehicle->buildingconstructable ( building );
}




#ifndef _NoStaticClassMembers_
pnputobjectcontainerguiicon tnputobjectcontainerguiicon :: first    = NULL;
int             tnputobjectcontainerguiicon :: buildnum = 0;
#endif



tnputobjectcontainerguiicon :: tnputobjectcontainerguiicon ( pobjecttype obj, int bld )
{
   setnxt ( first );
   first = this;
   object = obj;
   forcedeneable = 0;
   if ( obj ) {
      priority = 100;
      buildnum++;
      build = bld;
/*
      char buf[10000];
      if ( bld ) {
         picture[0]    = object->buildIcon.toBGI();
         sprintf ( buf, "%s : %d material and %d fuel needed", object->name.c_str(), object->buildcost.material, object->buildcost.fuel );
      } else {
         picture[0]    = object->removeIcon.toBGI();
         sprintf ( buf, "%s : %d material and %d fuel needed", object->name.c_str(), object->removecost.material, object->removecost.fuel );
      }

      infotext = buf;*/
   } else {
      picture[0] = icons.selectweaponguicancel;
      infotext = "Cancel (~ESC~)";
      priority = 10;
      keys[0][0] = ct_esc;
      keys[0][1] = ct_c;
   }

}



pnguiicon   tnputobjectcontainerguiicon::frst( void )
{
   return first;
}

void        tnputobjectcontainerguiicon::setfrst  ( pnguiicon ts )
{
   first = (pnputobjectcontainerguiicon) ts;
}

void              tnputobjectcontainerguiicon::exec( void )
{
/*
   if ( object )
      setspec ( object );
   else
      build_objects_reset();
      */
}


int         tnputobjectcontainerguiicon::available( void )
{/*
   if ( object ) {
      if ( build ) {
         if ( object_constructable ( getxpos(), getypos(), object ))
            return true;
      } else {
         if ( object_removeable ( getxpos(), getypos(), object ))
            return true;
      }

      return false;
   } else*/
      return 1;
}




pnputvehiclecontainerguiicon tnputvehiclecontainerguiicon :: first    = NULL;
int                          tnputvehiclecontainerguiicon :: buildnum = 0;

tnputvehiclecontainerguiicon :: tnputvehiclecontainerguiicon ( pvehicletype obj )
{
   setnxt ( first );
   first = this;

   vehicle = obj;
   forcedeneable = 0;
   if ( obj ) {
      priority = 100;
      buildnum++;
      picture[0]    = vehicle->buildicon.toBGI();
      infotext.format ( "%s : %d material and %d fuel needed", vehicle->getName().c_str(), vehicle->productionCost.material, vehicle->productionCost.energy );
   } else {
      picture[0] = icons.selectweaponguicancel;
      infotext = "Cancel";
      priority = 10;
   }

}


pnguiicon   tnputvehiclecontainerguiicon::frst( void )
{
   return first;
}

void        tnputvehiclecontainerguiicon::setfrst  ( pnguiicon ts )
{
   first = (pnputvehiclecontainerguiicon) ts;
}

void              tnputvehiclecontainerguiicon::exec( void )
{
   if ( vehicle )
      constructvehicle ( vehicle );
   else
      build_vehicles_reset();
}


int         tnputvehiclecontainerguiicon::available( void )
{
   if ( vehicle ) {
      pfield fld = getactfield();
      if ( !fld->vehicle ) {

         tselectvehiclecontainerguihost* bldhost = (tselectvehiclecontainerguihost*) host;
         Vehicle* actvehicle = bldhost->constructingvehicle;

         for ( int i = 0; i < actvehicle->typ->vehiclesBuildable.size(); i++ )
           for ( int j = actvehicle->typ->vehiclesBuildable[i].from; j <= actvehicle->typ->vehiclesBuildable[i].to; j++ )
              if ( j == vehicle->id )
                 if ( actvehicle->vehicleconstructable ( vehicle, getxpos(), getypos() ))
                    return 1;

      }
      return false;
   } else
      return 1;
}



tselectweaponguihost::tselectweaponguihost ( void )
{
   chainiconstohost ( &icon[0] );
}

void tselectweaponguihost :: init ( int resolutionx, int resolutiony )
{
    SelectWeaponBaseGuiHost :: init ( resolutionx, resolutiony );

    // Vehicle* eht = getfield(moveparams.movesx,moveparams.movesy)->vehicle;
    // pattackweap atw = attackpossible(eht, getxpos(),getypos());

    pattackweap atw = NULL;
    if ( pendingVehicleActions.attack->attackableVehicles.isMember ( getxpos(), getypos() ))
       atw = &pendingVehicleActions.attack->attackableVehicles.getData( getxpos(), getypos() );
    else
       if ( pendingVehicleActions.attack->attackableBuildings.isMember ( getxpos(), getypos() ))
          atw = &pendingVehicleActions.attack->attackableBuildings.getData( getxpos(), getypos() );
       else
          if ( pendingVehicleActions.attack->attackableObjects.isMember ( getxpos(), getypos() ))
             atw = &pendingVehicleActions.attack->attackableObjects.getData( getxpos(), getypos() );

    getfirsticon()->setup ( atw, 0 );

    x = getxpos();
    y = getypos();
}


int     tselectweaponguihost ::  painticons ( void )
{
   checkcoordinates();
   return SelectWeaponBaseGuiHost::painticons();
}

void    tselectweaponguihost ::  checkforkey ( tkey key, int keyprn )
{
   checkcoordinates();
   SelectWeaponBaseGuiHost::checkforkey( key, keyprn );
}

void    tselectweaponguihost ::  checkformouse ( void )
{
   checkcoordinates();
   SelectWeaponBaseGuiHost::checkformouse ( );
}

void    tselectweaponguihost ::  checkcoordinates ( void )
{
   if ( x != getxpos()   ||  y != getypos()  )
      init ( hgmp->resolutionx, hgmp->resolutiony );

}


pnweapselguiicon tnweapselguiicon::first = NULL;


tnweapselguiicon::tnweapselguiicon ( void )
{
   setnxt ( first );
   first = this;
   iconnum = -1;
   weapnum = -1;
   typ = -1;
   strength = -1;
}


pnguiicon   tnweapselguiicon::frst( void )
{
   return first;
}

void        tnweapselguiicon::setfrst  ( pnguiicon ts )
{
   first = (tnweapselguiicon*) ts;
}


int         tnweapselguiicon::available    ( void )
{
   if ( typ != -1 )
      return 1;
   else
      return 0;
}

const char*       tnweapselguiicon::getinfotext  ( void )
{
   if ( weapnum > -1 ) {
      Vehicle* eht = getfield ( moveparams.movesx, moveparams.movesy ) -> vehicle;

      infotext = cwaffentypen[typ];

      pfield fld = getactfield();

      tfight* battle;

      if ( fld->vehicle )
         battle = new tunitattacksunit ( eht, fld->vehicle, 1, weapnum );
      else
      if ( fld->building )
         battle = new tunitattacksbuilding ( eht, getxpos(), getypos(), weapnum );
      else
      if ( !fld->objects.empty() )
         battle = new tunitattacksobject ( eht, getxpos(), getypos(), weapnum );


      int dd = battle->dv.damage;
      int ad = battle->av.damage;
      battle->calc ( );

      char buf[10000];
      sprintf(buf, "(~%c~) %s; eff strength: %d; damage inflicted to enemy: %d, making a total of ~%d~; own damage will be +%d = %d", (int)('A'+weapnum), infotext.c_str(), battle->av.strength, battle->dv.damage-dd, battle->dv.damage, battle->av.damage-ad, battle->av.damage );
      infotext = buf;

      return infotext.c_str();
   } else
      return "cancel ( ~ESC~ )";

}


void  tnweapselguiicon::exec         ( void )
{
   if ( available() ) {
      if ( weapnum >= 0 ) {
         int res = pendingVehicleActions.attack->execute ( NULL, getxpos(), getypos(), 2, 0, weapnum );
         if ( res < 0 )
            dispmessage2 ( -res, NULL );

      }
      delete pendingVehicleActions.attack;
      actmap->cleartemps ( 0xff );
      displaymap();
      updateFieldInfo();

      actgui = &gui;
      actgui->restorebackground();
   }
}

void  tnweapselguiicon::checkforkey  ( tkey key )
{
   if ( available () )
      switch (key) {
         case ct_a: if ( weapnum == 0 ) {
                       exec();
                       return;
                    }
         case ct_b: if ( weapnum == 1 ) {
                       exec();
                       return;
                    }
         case ct_c: if ( weapnum == 2 ) {
                       exec();
                       return;
                    }
         case ct_d: if ( weapnum == 3 ) {
                       exec();
                       return;
                    }
         case ct_e: if ( weapnum == 4 ) {
                       exec();
                       return;
                    }
         case ct_f: if ( weapnum == 5 ) {
                       exec();
                       return;
                    }
         case ct_g: if ( weapnum == 6 ) {
                       exec();
                       return;
                    }
         case ct_h: if ( weapnum == 7 ) {
                       exec();
                       return;
                    }
         case ct_esc: if ( weapnum < 0 ) {
                       exec();
                       return;
                    }
        }

   if ( nxt () )
      nxt()->checkforkey( key );
}


void  tnweapselguiicon::setup        ( pattackweap atw, int n )
{
   iconnum  = n;
   if ( atw && n < atw->count ) {
      typ      = log2( atw->typ[n] );
      strength = atw->strength[n];
      weapnum  = atw->num[n];
      Vehicle* eht = getfield ( moveparams.movesx, moveparams.movesy ) -> vehicle;
      if ( atw->typ[n] & cwlaserb )
         picture[0]  = icons.selectweapongui[ 12 ];
      else
         if ( eht->height >= chtieffliegend ) {
            if ( typ == cwairmissilen )
               picture[0]  = icons.selectweapongui[ 9 ];
            else
               picture[0]  = icons.selectweapongui[ typ ];
         } else
             if ( typ == cwgroundmissilen )
                picture[0]  = icons.selectweapongui[ 10 ];
             else
                picture[0]  = icons.selectweapongui[ typ ];
   } else
      if ( (!atw && n == 0 ) || (atw && n == atw->count) ) {
        typ      = -2;
        strength = -1;
        weapnum  = -1;
        picture[0] = icons.selectweaponguicancel;
      } else {
        typ      = -1;
        strength = -1;
        weapnum  = -1;
        picture[0] = NULL;
      }

   if ( nxt() ) {
      tnweapselguiicon* ne = (tnweapselguiicon*) nxt();
      ne->setup ( atw, ++n );
   }
}

#endif


#ifndef _NoStaticClassMembers_
preplayguiicon treplayguiicon ::first = NULL;
#endif

treplayguiicon::treplayguiicon ( void )
{
   setnxt ( first );
   first = this;
}


pnguiicon   treplayguiicon::frst( void )
{
   return first;
}

void        treplayguiicon::setfrst  ( pnguiicon ts )
{
   first = (preplayguiicon) ts;
}

trguiicon_play :: trguiicon_play ( void )
{
   filename = "rp_play";
}

int trguiicon_play :: available ( void )
{
   if ( runreplay.status == 1 )
      return 1;

   return 0;
}

void trguiicon_play :: exec ( void )
{
  runreplay.status = 2;
  updateFieldInfo();

}

trguiicon_pause :: trguiicon_pause ( void )
{
   filename = "rp_pause" ;
}

int trguiicon_pause :: available ( void )
{
   if ( runreplay.status == 2 )
      return 1;

   return 0;
}

void trguiicon_pause :: exec ( void )
{
   runreplay.status = 1;
   updateFieldInfo();
}




trguiicon_faster :: trguiicon_faster ( void )
{
   filename = "rp_fast";
}

int trguiicon_faster :: available ( void )
{
   if ( runreplay.status == 2 )
      if ( CGameOptions::Instance()->replayspeed > 0 )
        return 1;

   return 0;
}

void trguiicon_faster :: exec ( void )
{
   if ( CGameOptions::Instance()->replayspeed > 20 )
      CGameOptions::Instance()->replayspeed -= 20;
   else
      CGameOptions::Instance()->replayspeed = 0;

   CGameOptions::Instance()->setChanged ( 1 );
   displaymessage2 ( "delay set to %d / 100 sec", CGameOptions::Instance()->replayspeed );
   updateFieldInfo();
}


trguiicon_slower :: trguiicon_slower ( void )
{
   filename = "rp_slow";
}

int trguiicon_slower :: available ( void )
{
   if ( runreplay.status == 2 )
      return 1;

   return 0;
}

void trguiicon_slower :: exec ( void )
{
   CGameOptions::Instance()->replayspeed += 20;
   CGameOptions::Instance()->setChanged ( 1 );
   displaymessage2 ( "delay set to %d / 100 sec", CGameOptions::Instance()->replayspeed );
   updateFieldInfo();
}



trguiicon_back :: trguiicon_back ( void )
{
   filename = "rp_back";
}

int trguiicon_back :: available ( void )
{
   if ( runreplay.status == 1  ||  runreplay.status == 10 )
      return 1;

   return 0;
}

void trguiicon_back :: exec ( void )
{
   runreplay.status = 101;
}



trguiicon_exit :: trguiicon_exit ( void )
{
   filename = "rp_exit";
}

int trguiicon_exit :: available ( void )
{
   if ( runreplay.status == 1 || runreplay.status == 10 || runreplay.status == 11 )
      return 1;

   return 0;
}

void trguiicon_exit :: exec ( void )
{
   runreplay.status = 100;

}

trguiicon_cancel::trguiicon_cancel ( void )
{
   filename = "cancel";
   forcedeneable = 0;
}



int   trguiicon_cancel::available    ( void )
{
   if ( forcedeneable )
      return 2;
   return 0;
}

void  trguiicon_cancel::exec         ( void )
{
}







void   treplayguihost    :: init ( int resolutionx, int resolutiony )
{
   chainiconstohost ( &icons.play );
   ReplayBaseGuiHost::init ( resolutionx, resolutiony );
}

void   treplayguihost :: bi2control (  )
{
   icons.cancel.display();
   ReplayBaseGuiHost::bi2control (  );
}

treplayguihost replayGuiHost;

// ContainerBaseGuiHost Borland_Cpp_builder_sucks2;
GuiHost<pgeneralicon_c> gcc_sucks;

ContainerBaseGuiHost :: ContainerBaseGuiHost()
{
  chainiconstohost(NULL);
}; // just a hack to avoid some gcc bugs; chainiconstohost wouldn't be instantiated else ...

ContainerBaseGuiHost foobar;


