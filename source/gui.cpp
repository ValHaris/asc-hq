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
#include <string.h>
#include <stdlib.h>

#include "tpascal.inc"
#include "vesa.h"
#include "typen.h"
#include "newfont.h"
#include "keybp.h"
#include "misc.h"
#include "gui.h"
#include "spfldutl.h"

#include "spfst.h"
#include "mousehnd.h"
#include "loaders.h"
#include "controls.h"
#include "dlg_box.h"
#include "dialog.h"
#include "building.h"
#include "attack.h"
#include "stack.h"
#include "sg.h"


tguihoststandard          gui;
tselectbuildingguihost    selectbuildinggui;
tselectobjectcontainerguihost      selectobjectcontainergui;
tselectvehiclecontainerguihost     selectvehiclecontainergui;
tselectweaponguihost      selectweaponguihost;
tguihost*        actgui = &gui;


int guixpos;


   #define guixdif 190     // Abstand zum RECHTEN Rand
   #define guigapx 13
   #define guigapy 4

int guiypos = 308 + guigapy;

void setguiposy ( int y )
{
   guiypos = y;
}

void         repositionmouse ( void )
{
   /*
   if (doubleclickparams.stat == 3) {
      char ms = getmousestatus();
      if (ms == 2)
         mousevisible ( false );
      setmouseposition ( doubleclickparams.mx, doubleclickparams.my );
      if (ms == 2)
         mousevisible ( true );
   }
   */
}

void setmouseongui( void )
{
   setmouseposition ( guixpos + guiiconsizex / 2, guiypos + guiiconsizey / 2);

   while (mouseparams.taste & 1) ;
}



static void*    tguihost::background[30][30];


tguihost::tguihost ( void )
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

             
void   tguihost::starticonload ( void )
{
   getfirsticon()->loaddata();
   getfirsticon()->sort ( NULL );
   chainiconstohost ( getfirsticon() );
}


void   tguihost::returncoordinates ( pnguiicon icon, int* x, int * y )
{
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

void   tguihost::chainiconstohost ( pnguiicon icn )
{
   setfirsticon( icn->frst() );
   getfirsticon()->sethost ( this );
}


void   tguihost::savebackground ( void )
{
   int size = imagesize ( 0, 0, guiiconsizex, guiiconsizey );
   for (int j = 0; j < iconspaintable/columncount ; j++ ) 
      for (int i = 0; i< columncount ; i++) {
         background[i][j] = new char [ size ];
         getimage ( guixpos + i*(guiiconsizex + guigapx), guiypos + j*(guiiconsizey + guigapy), guixpos + i*(guiiconsizex + guigapx) + guiiconsizex, guiypos + j*(guiiconsizey + guigapy) + guiiconsizey,  background[i][j] );
         actshownicons[i][j] = NULL;
      } /* endfor */
}


void tguihost :: putbackground ( int xx , int yy )
{
     int x = guixpos + xx * ( guiiconsizex + guigapx );
     int y = guiypos + yy * ( guiiconsizey + guigapy );
     putimage ( x, y, background[ xx ][ yy ] );
}

int    tguihost::painticons ( void )
{
   int oldnumpainted = numpainted;
   numpainted = 0;

   firstpaint = 1;

   getfirsticon()->paintifavail();

   firstpaint = 0;

   setinvisiblemouserectanglestk ( guixpos, guiypos, guixpos + columncount*(guiiconsizex + guigapx) + guiiconsizex, guiypos + iconspaintable/columncount*(guiiconsizey + guigapy) + guiiconsizey );
   for (int j = numpainted; j < iconspaintable ; j++ ) {
      if ( actshownicons[j % columncount][j / columncount] ) {
         putbackground ( j % columncount , j / columncount );
         actshownicons[j % columncount][j / columncount] = NULL;
      }
   } /* endfor */
   getinvisiblemouserectanglestk ();

   return numpainted;
}


void   tguihost :: cleanup ( void )    // wird zum entfernen der kleinen guiicons aufgerufen, bevor das icon ausgefhrt wird
{
   if ( smalliconpos.buf ) {
      setinvisiblemouserectanglestk ( smalliconpos.x, smalliconpos.y, smalliconpos.x + smalliconpos.xsize, smalliconpos.y + guismalliconsizey );
      putimage ( smalliconpos.x, smalliconpos.y, smalliconpos.buf );
      getinvisiblemouserectanglestk (  );

      delete[] smalliconpos.buf;
      smalliconpos.buf = NULL;
      paintsize = 0;
   }
}


void   tguihost::paintsmallicons ( int taste, int up )
{
   npush ( paintsize );

   paintsize = 1;

   int num = getfirsticon()->count();

   smalliconpos.xsize = num * guismalliconsizex + ( num - 1 ) * guismallicongap;

   if ( mouseparams.x + smalliconpos.xsize - guismalliconsizex/2 > hgmp->resolutionx )
      smalliconpos.x = hgmp->resolutionx - smalliconpos.xsize;
   else 
      smalliconpos.x = mouseparams.x - guismalliconsizex/2;


   {
      if ( (gameoptions.mouse.smalliconundermouse == 0)  || ((gameoptions.mouse.smalliconundermouse == 2) && up ))
         smalliconpos.y = mouseparams.y - 5 - guismalliconsizey;
      else
         smalliconpos.y = mouseparams.y - guismalliconsizey / 2;
   }


   if ( smalliconpos.y < 0 )
      smalliconpos.y = 0;

   setinvisiblemouserectanglestk ( smalliconpos.x, smalliconpos.y, smalliconpos.x + smalliconpos.xsize, smalliconpos.y + guismalliconsizey );

   smalliconpos.buf = new char[ imagesize ( smalliconpos.x, smalliconpos.y, smalliconpos.x + smalliconpos.xsize, smalliconpos.y + guismalliconsizey )];

   getimage ( smalliconpos.x, smalliconpos.y, smalliconpos.x + smalliconpos.xsize, smalliconpos.y + guismalliconsizey, smalliconpos.buf );

   numpainted = 0;

   firstpaint = 1;

   getfirsticon()->paintifavail();

   firstpaint = 0;
   int mousestat = 0;

   int msg = 0;

   getinvisiblemouserectanglestk ( );

   do {
      if ( mouseparams.taste != taste  &&  !mousestat )
         mousestat++;

      int fnd = 0;
      for ( int j = 0; j < num; j++ ) 
         if ( mouseinrect ( smalliconpos.x + j * ( guismallicongap + guismalliconsizex ), smalliconpos.y, smalliconpos.x + j * ( guismallicongap + guismalliconsizex ) + guismalliconsizex, smalliconpos.y + guismalliconsizey )) {
            fnd = 1;
            if ( actshownicons[j % columncount][j / columncount] ) {
                if ( actshownicons[j % columncount][j / columncount] != infotextshown ) 
                   msg = displaymessage2 ( actshownicons[j % columncount][j / columncount]->getinfotext() );

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


   } while ( !keypress() &&  mousestat < 100 ); /* enddo */



   cleanup();

   npop ( paintsize );

   if ( infotextshown  && msg == actdisplayedmessage ) 
      displaymessage2 ( "" );

   infotextshown = NULL;
   

}



void   tguihost::checkformouse ( void )
{
   int msg;
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
                       msg = displaymessage2 ( actshownicons[i][j]->getinfotext() );
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


void   tguihost :: runpressedmouse ( int taste  )
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
   }

   if ( actshownicons[xp][yp] ) {
      actshownicons[zx][zy]->display();
      actshownicons[xp][yp]->exec();
   }

   mousevisible ( false );
   setmouseposition ( oldmousex, oldmousey );
   mousevisible ( true );

}




void   tguihost::checkforkey ( tkey key )
{
   if ( key == ct_enter ) {
      if ( actshownicons[0][0] )
         bi2control();
   } else
      getfirsticon()->checkforkey( key );
}









void   tguihost::bi2control (  )
{
      int xp = 0;
      int yp = 0;

      char mss = getmousestatus ();
      if (mss == 2)
         mousevisible(false);
   
      actshownicons[xp][yp]->iconpressed();

      int msg = displaymessage2( actshownicons[xp][yp]->getinfotext() );
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
               msg = displaymessage2( actshownicons[xp][yp]->getinfotext() );
               infotextshown = actshownicons[xp][yp];
               zx = xp;
               zy = yp;
            } 
      #ifdef NEWKEYB
      }  
      #else
      } while ( ky != ct_enter );
      #endif
   
      actshownicons[xp][yp]->display();
      if ( ky != ct_esc )
         actshownicons[xp][yp]->exec();

      if ( msg == actdisplayedmessage )
         displaymessage2("");
      infotextshown = NULL;

      if (mss == 2)
         mousevisible(true);

      firstshownline = 0;

      actgui->painticons();

}










void   tguihost::init ( int resolutionx, int resolutiony )
{
   guixpos = resolutionx - guixdif;
   iconspaintable =  (resolutiony - 360) / ( guiiconsizey + guigapy ) * columncount;
}

int    tguihost::numpainted;


void   tguihost::restorebackground ( void )
{
   setinvisiblemouserectanglestk ( guixpos, guiypos, guixpos + columncount*(guiiconsizex + guigapx) + guiiconsizex, guiypos + iconspaintable/columncount*(guiiconsizey + guigapy) + guiiconsizey );
   for (int j = 0; j < iconspaintable/columncount ; j++ ) 
      for (int i = 0; i< columncount ; i++) {
         putbackground ( i, j );
         actshownicons[i][j] = NULL;
      } /* endfor */
   getinvisiblemouserectanglestk ();
}





void   tguihoststandard     :: init ( int resolutionx, int resolutiony )
{
   chainiconstohost ( &icons.movement );
   tguihost::init ( resolutionx, resolutiony );
}


void   tguihoststandard :: bi2control (  )
{
   icons.cancel.display();
   tguihost::bi2control (  );
}





pnguiicon tguihost :: getfirsticon( void )
{
   return first_icon;
}

void      tguihost :: setfirsticon( pnguiicon ic )
{
   first_icon = ic;
}



static pnguiicon  tnguiicon::first = NULL;


tnguiicon::tnguiicon ( void )
{
   for (int i = 0; i < 8; i++) {
      picture[i] = NULL;
      picturepressed[i] = NULL;
   } 

   infotext    = NULL;
   filename[0] = NULL;
   host = NULL;
   priority = 0;
   next = first;
   first = this;
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

pnguiicon   tnguiicon::frst( void )
{
   return first;
}

void        tnguiicon::setfrst  ( pnguiicon ts )
{
   first = ts;
}


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
   char tempfilename[15];
   strcpy ( tempfilename, filename );
   strcat ( tempfilename, ".nic" );
   
   {
     tnfilestream stream ( tempfilename ,1 );
     loadspecifics( &stream );
   }

   if ( nxt () )
      nxt()->loaddata ();
}


void  tnguiicon::loadspecifics( pnstream stream )
{
   tnguiiconfiledata  rawdata;
   stream->readdata ( &rawdata, sizeof ( rawdata ));
   if ( rawdata.infotext )
      stream->readpchar ( &infotext );

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


char* tnguiicon::getinfotext  ( void )
{
   return infotext;
}

void  tnguiicon::iconpressed ( void )
{
//   if ( host->paintsize != lasticonsize )
      host->returncoordinates ( this, &x,  &y );

   if ( host->paintsize == 0 ) {
      void* buf = new char [ imagesize ( 8,8, 40,26 )];
   
      setinvisiblemouserectanglestk ( x, y, x + guiiconsizex, y + guiiconsizey );
      display();
      getimage ( x+8, y+8, x+40, y+26, buf );
      putspriteimage ( x, y, icons.guiknopf );
      putimage ( x+9, y+9, buf );
      getinvisiblemouserectanglestk  ( );
   
      delete[] buf;
      
   } else {
      void* buf = new char [ imagesize ( 4,4, 20,13 )];
   
      setinvisiblemouserectanglestk ( x, y, x + guismalliconsizex, y + guismalliconsizey );
      getimage ( x+4, y+4, x+20, y+13, buf );
      putspriteimage ( x, y, halfpict ( icons.guiknopf ) );
      putimage ( x+5, y+5, buf );
      getinvisiblemouserectanglestk  ( );
   
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

     }
     display();
     if ( mouseparams.x >= x   &&
          mouseparams.y >= y   &&
          mouseparams.x <= x+xs   &&
          mouseparams.y <= y+ys &&
          mouseparams.taste != taste )
     {
            host->cleanup();
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
         if ( keys[0][i] )
           if ( char2key( keys[0][i] ) == key ) {
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


void   tnguiicon::sethost ( pguihost hst )
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





static pnsguiicon tnsguiicon ::first = NULL;

tnsguiicon::tnsguiicon ( void )
{
   next = first;
   first = this;
}


pnguiicon   tnsguiicon::nxt( void )
{
   return next;
}
void      tnsguiicon::setnxt   ( pnguiicon ts )
{
   next = (pnsguiicon) ts ;
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
      setinvisiblemouserectanglestk ( x, y, x + guiiconsizex, y + guiiconsizey );
      putspriteimage ( x, y, buf );
      getinvisiblemouserectanglestk ( );
   } else {
      setinvisiblemouserectanglestk ( x, y, x + guismalliconsizex, y + guismalliconsizey );
      putspriteimage ( x, y, halfpict ( buf ) );
      getinvisiblemouserectanglestk ( );
   }
}


tnguiicon:: ~tnguiicon (  )
{
   for (int i = 0; i < 8; i++) {
      if ( picture[i] );
         delete[] picture[i];
      if ( picturepressed[i] )
         delete[] picturepressed[i];
   } 
   if ( infotext )
      delete[] infotext;
}







tnsguiiconmove::tnsguiiconmove ( void )
{
   strcpy ( filename, "movement" );
}


int   tnsguiiconmove::available    ( void ) 
{
   if (moveparams.movestatus == 0) { 
      pvehicle eht = getactfield()->vehicle; 
      if ( eht ) 
         if ( eht->color == actmap->actplayer * 8) 
            if ( eht->movement >= minmalq ) 
               if ( terrainaccessible ( getfield ( eht->xpos, eht->ypos ), eht ) || actmap->gameparameter[ cgp_movefrominvalidfields] )
                  return 1; 
   } 
   else 
     if (((moveparams.movestatus >= 0) && (moveparams.movestatus <= 2)) ||
         ((moveparams.movestatus >=11) && (moveparams.movestatus <=12))) {
         if ( getactfield()->a.temp ) 
            return 1; 
      } 
   return 0;

}

void  tnsguiiconmove::exec         ( void ) 
{
   if (((moveparams.movestatus >= 0) && (moveparams.movestatus <= 2)) ||
       ((moveparams.movestatus >=11) && (moveparams.movestatus <=12)))
      movement(getactfield()->vehicle); 
      displaymap();
      if ( (moveparams.movestatus == 2  ||  moveparams.movestatus == 12 ) && gameoptions.fastmove ) {
         movement(getactfield()->vehicle); 
         displaymap();
      }

      if (moveparams.movestatus == 0 )
         // repositionmouse ();
         if ( gameoptions.smallguiiconopenaftermove ) {
            actgui->painticons();
            actgui->paintsmallicons ( gameoptions.mouse.smallguibutton, 0 );
         }
   dashboard.x = 0xffff;
}

void  tnsguiiconmove::display      ( void ) 
{
   host->returncoordinates ( this, &x, &y );

   if ( x == -1   ||    y == -1 ) 
      return;

   int h;
   if ( moveparams.movestatus == 0)
      h = getactfield()->vehicle->typ->height ;
   else
      h = moveparams.vehicletomove->typ->height ;

   if ( h >= chtieffliegend )
      putpict ( picture[1] );
   else
      if ( h & ( chschwimmend | chgetaucht | chtiefgetaucht ) )
         putpict ( picture[2] );
      else
         putpict ( picture[0] );

}











tnsguiiconattack::tnsguiiconattack ( void )
{
   strcpy ( filename, "attack" );
}



int   tnsguiiconattack::available    ( void ) 
{
   if (moveparams.movestatus == 0) { 
      pvehicle eht = getactfield()->vehicle; 
      if (eht != NULL) 
         if (eht->color == actmap->actplayer * 8) 
            if (eht->attacked == false)
              if ((eht->typ->wait == false) || (eht->movement == eht->typ->movement[log2(eht->height)]  ||  eht->reactionfire_active >= 3 ))
                if (weapexist(eht))
                   return 1;
 
   } 
   return 0;
}

void  tnsguiiconattack::exec         ( void ) 
{
   attack(false); 
   // repositionmouse ();
}










tnsguiiconascent::tnsguiiconascent ( void )
{
   strcpy ( filename, "ascent" );
}

int   tnsguiiconascent::available    ( void ) 
{
   if (moveparams.movestatus == 0) { 
      pvehicle eht = getactfield()->vehicle; 
       if ( eht ) 
          if ( eht->movement )
             if (eht->color == actmap->actplayer * 8) 
                if (eht->height < 128) 
                   if ((eht->height << 1) & eht->typ->height ) 
                      return 1; 
   } 
   return 0;
}

void  tnsguiiconascent::exec         ( void ) 
{
   int res = changeheight(getactfield()->vehicle,'h', 1 ); 
   if ( res )
      dispmessage2( res, NULL );
   displaymap();
   // repositionmouse ();
}

void  tnsguiiconascent::display      ( void ) 
{
   host->returncoordinates ( this, &x, &y );

   if ( x == -1   ||    y == -1 ) 
      return;

   int h;
   if ( moveparams.movestatus == 0)
      h = getactfield()->vehicle->typ->height ;
   else
      h = moveparams.vehicletomove->typ->height ;

   if ( h >= chtieffliegend )
      putpict ( picture[0] );
   else
      putpict ( picture[1] );

}












tnsguiicondescent::tnsguiicondescent ( void )
{
   strcpy ( filename, "descent" );
}



int   tnsguiicondescent::available    ( void ) 
{
   if (moveparams.movestatus == 0) { 
      pvehicle eht = getactfield()->vehicle; 
      if ( eht ) 
         if ( eht->movement )
            if (eht->color == actmap->actplayer * 8) 
               if (eht->height > 1) 
                  if ((eht->height >> 1) & eht->typ->height ) 
                     return 1;
 
   } 
   return 0;
}

void  tnsguiicondescent::exec         ( void ) 
{
   int res = changeheight(getactfield()->vehicle,'t', 1 ); 
   if ( res )
      dispmessage2( res, NULL );
   displaymap();
   // repositionmouse ();
}

void  tnsguiicondescent::display      ( void ) 
{
   host->returncoordinates ( this, &x, &y );

   if ( x == -1   ||    y == -1 ) 
      return;

   int h;
   if ( moveparams.movestatus == 0)
      h = getactfield()->vehicle->typ->height ;
   else
      h = moveparams.vehicletomove->typ->height ;

   if ( h >= chtieffliegend )
      putpict ( picture[0] );
   else
      putpict ( picture[1] );

}









tnsguiiconinformation::tnsguiiconinformation ( void )
{
   strcpy ( filename, "informat" );
}



int   tnsguiiconinformation::available    ( void ) 
{
    pvehicle eht = getactfield()->vehicle; 
    if (moveparams.movestatus == 0) 
    if (eht != NULL)      
       if (fieldvisiblenow(getactfield()))
          return 1; 
   return 0;
}

void  tnsguiiconinformation::exec         ( void ) 
{
 //  repositionmouse ();
   vehicle(); 
}

void  tnsguiiconinformation::display      ( void ) 
{
   host->returncoordinates ( this, &x, &y );

   if ( x == -1   ||    y == -1 ) 
      return;

   int h = getactfield()->vehicle->typ->height ;

   if ( h >= chtieffliegend )
      putpict ( picture[1] );
   else
      if ( h & ( chschwimmend | chgetaucht | chtiefgetaucht ) )
         putpict ( picture[2] );
      else
         putpict ( picture[0] );

}













tnsguiiconendturn::tnsguiiconendturn ( void )
{
   strcpy ( filename, "endturn" );
}



int   tnsguiiconendturn::available    ( void ) 
{
   if (moveparams.movestatus == 0) 
      if (actmap->levelfinished == false) 
         return 1; 
   return 0;
}

int autosave = 0;

void  tnsguiiconendturn::exec         ( void ) 
{
   if ( !gameoptions.endturnquestion || (choice_dlg("do you really want to end your turn ?","~y~es","~n~o") == 1)) {

      cursor.hide();

      char s[100];
      sprintf( s, "automatic savegame turn %d ; player %d; move %d ", actmap->time.a.turn, actmap->actplayer, actmap->time.a.move );
  
      char name[100];
      if ( autosave )
         strcpy ( name, "autosav1");
      else
         strcpy ( name, "autosav2");

      strcat ( name, &savegameextension[1] );
      savegame ( name, s );
  
      autosave = !autosave;
  
      next_turn();
  
     displaymap();
     cursor.show();
   }
}


tnsguiiconexternalloading::tnsguiiconexternalloading ( void )
{
   strcpy ( filename, "extload" );
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




tnsguiiconpoweron::tnsguiiconpoweron ( void )
{
   strcpy ( filename, "poweron" );
}

int   tnsguiiconpoweron::available    ( void ) 
{
   if ( getactfield()->vehicle )
      if ( getactfield()->vehicle->color == actmap->actplayer*8  &&
           (getactfield()->vehicle->functions & cfgenerator))
           if ( !getactfield()->vehicle->generatoractive )
              return 1;

  return 0;         
}

void  tnsguiiconpoweron::exec         ( void ) 
{
   getactfield()->vehicle->setpower ( 1 );
   dashboard.x = 0xffff;
}



tnsguiiconpoweroff::tnsguiiconpoweroff ( void )
{
   strcpy ( filename, "poweroff" );
}

int   tnsguiiconpoweroff::available    ( void ) 
{
   if ( getactfield()->vehicle )
      if ( getactfield()->vehicle->color == actmap->actplayer*8  &&
           (getactfield()->vehicle->functions & cfgenerator))
           if ( getactfield()->vehicle->generatoractive )
              return 1;

  return 0;         
}

void  tnsguiiconpoweroff::exec         ( void ) 
{
   getactfield()->vehicle->setpower ( 0 );
   dashboard.x = 0xffff;
}




tnsguiiconconstructvehicle::tnsguiiconconstructvehicle ( void )
{
   strcpy ( filename, "consttnk" );
}



int   tnsguiiconconstructvehicle::available    ( void ) 
{
   if ( actmap->gameparameter ) 
      if ( actmap->gameparameter[cgp_forbid_unitunit_construction] )
         return 0;


   pfield fld = getactfield();
   if ( fld )
      if ( fld->vehicle )
         if (fld->vehicle->color == actmap->actplayer * 8) 
            if ( fld->vehicle->typ->vehiclesbuildablenum )
               if (moveparams.movestatus == 0) 
                  if ( !fld->vehicle->attacked )
                     return 1;

   return 0;
}

void  tnsguiiconconstructvehicle::exec         ( void ) 
{
   constructvehicle( NULL ); 
   displaymap();
}











tnsguiiconputmine::tnsguiiconputmine ( void )
{
   strcpy ( filename, "putmine" );
}



int   tnsguiiconputmine::available    ( void ) 
{
   pfield fld = getactfield(); 
   if (moveparams.movestatus == 0) 
      if ( fld->vehicle ) 
         if (fld->vehicle->color == actmap->actplayer * 8) 
            if (fld->vehicle->functions & cfminenleger ) 
               if ( !fld->vehicle->attacked )
                  return true; 
   return 0;
}

void  tnsguiiconputmine::exec         ( void ) 
{
   legemine(0); 
   displaymap();
 // repositionmouse ();
}










int   tnsguiiconputgroundmine::available    ( void ) 
{
   if (moveparams.movestatus == 90) { 
      pfield fld = getactfield(); 
      if ((fld->typ->art & cbwater) == 0)
         if ( fld->a.temp ) 
            if ( !fld->mineexist() ) 
               return true; 
   } 
   return 0;
}



tnsguiiconputantitankmine::tnsguiiconputantitankmine ( void )
{
   strcpy ( filename, "tankmine" );
}

void  tnsguiiconputantitankmine::exec         ( void ) 
{
   legemine(cmantitankmine); 
   displaymap();
 // repositionmouse ();
}











tnsguiiconputantipersonalmine::tnsguiiconputantipersonalmine ( void )
{
   strcpy ( filename, "persmine" );
}

void  tnsguiiconputantipersonalmine::exec         ( void ) 
{
   legemine(cmantipersonnelmine); 
   displaymap();
 // repositionmouse ();
}










int   tnsguiiconputseamine::available    ( void ) 
{
   if (moveparams.movestatus == 90) { 
      pfield fld = getactfield(); 
      if (fld->typ->art & cbwater ) 
         if (fld->a.temp ) 
            if ( !fld->mineexist() ) 
               return true; 
   } 
   return 0;
}



tnsguiiconputfloatingmine::tnsguiiconputfloatingmine ( void )
{
   strcpy ( filename, "floatmin" );
}

void  tnsguiiconputfloatingmine::exec         ( void ) 
{
   legemine(cmfloatmine); 
   displaymap();
 // repositionmouse ();
}













tnsguiiconputmooredmine::tnsguiiconputmooredmine ( void )
{
   strcpy ( filename, "moormine" );
}

void  tnsguiiconputmooredmine::exec         ( void ) 
{
   legemine(cmmooredmine); 
   displaymap();
 // repositionmouse ();
}












tnsguiiconremovemine::tnsguiiconremovemine ( void )
{
   strcpy ( filename, "remomine" );
}



int   tnsguiiconremovemine::available    ( void ) 
{
   if (moveparams.movestatus == 90) { 
      pfield fld = getactfield(); 
      if ( fld->a.temp ) 
         if ( fld->mineexist() ) 
            return true; 
   } 
   return 0;
}

void  tnsguiiconremovemine::exec         ( void ) 
{
   legemine(0);  
   displaymap();
 // repositionmouse ();
}








tnsguiiconbuildany::tnsguiiconbuildany ( void )
{
   strcpy ( filename, "buildany" );
}



int   tnsguiiconbuildany::available    ( void ) 
{
   pfield fld = getactfield();
   if ( fld->vehicle ) 
      if (fld->vehicle->color == actmap->actplayer * 8) 
         if ( fld->vehicle->typ->objectsbuildablenum ) 
            if (moveparams.movestatus == 0) 
               if ( !fld->vehicle->attacked )
                  return true; 
   return 0;
}

void  tnsguiiconbuildany::exec         ( void ) 
{
   setspec(0); 
   displaymap();
 // repositionmouse ();
}




tnsguiiconrepair::tnsguiiconrepair ( void )
{
   strcpy ( filename, "repair" );
}



int   tnsguiiconrepair::available    ( void ) 
{
   if (moveparams.movestatus == 0) { 
      pfield fld = getactfield(); 
      if ( fld->vehicle ) 
         if (fld->vehicle->color == actmap->actplayer * 8) 
            if (fld->vehicle->functions & cfrepair ) 
               for ( int i = 0; i < fld->vehicle->typ->weapons->count; i++ )
                  if ( fld->vehicle->typ->weapons->weapon[i].typ & cwserviceb )
                     if ( !fld->vehicle->attacked)
                        return 1; 
   } 

   if (moveparams.movestatus == 66) { 
      pfield fld = getactfield(); 
      if ( fld->a.temp ) 
         return true; 
   } 

   return 0;
}

void  tnsguiiconrepair::exec         ( void ) 
{
   refuelvehicle(1); 
   displaymap();
 // repositionmouse ();
}









tnsguiiconrefuel::tnsguiiconrefuel ( void )
{
   strcpy ( filename, "refuel" );
}



int   tnsguiiconrefuel::available    ( void ) 
{
   if (moveparams.movestatus == 0) { 
      pfield fld = getactfield(); 
      if ( fld->vehicle ) 
         if ( !fld->vehicle->attacked )
            if (fld->vehicle->color == actmap->actplayer * 8) { 
               pvehicletype fzt = fld->vehicle->typ; 
               for ( int i = 0; i < fzt->weapons->count; i++ )
                  if ( fzt->weapons->weapon[i].typ & cwserviceb )
                     for ( int j = 0; j < fzt->weapons->count ; j++) {
                        if (fzt->weapons->weapon[j].typ & cwammunitionb )
                           return 1;
                        if ( fld->vehicle->functions & (cffuelref | cfmaterialref) )
                           return 1; 
                     }
            }     
   } 
   if (moveparams.movestatus == 65) { 
      pfield fld = getactfield(); 
      if ( fld->a.temp ) 
         return 2; 
   } 
   return 0;
}

void  tnsguiiconrefuel::exec         ( void ) 
{
   if ( moveparams.movestatus == 0 ) {
      refuelvehicle(3); 
      displaymap();
   } else
     if (moveparams.movestatus == 65)  {
        refuelvehicle(2); 
        displaymap();
      // repositionmouse ();
     }
}


void  tnsguiiconrefuel::display      ( void ) 
{
   host->returncoordinates ( this, &x, &y );

   if ( x == -1   ||    y == -1 ) 
      return;

   int pict = 1;
   if (moveparams.movestatus == 0) { 
      pfield fld = getactfield(); 
      if ( fld->vehicle ) 
         if ( fld->vehicle->functions & cffuelref  )
            pict = 0; 
   } else 
      if ( getfield(moveparams.movesx,moveparams.movesy)->vehicle->functions & cffuelref  )
         pict = 0; 

   putpict ( picture[pict] );
}








tnsguiiconrefueldialog::tnsguiiconrefueldialog ( void )
{
   strcpy ( filename, "refueld" );
}



int   tnsguiiconrefueldialog::available    ( void ) 
{
   priority = 20; // !!
   if (moveparams.movestatus == 65) { 
      pfield fld = getactfield(); 
      if ( fld->a.temp ) 
         return true; 
   } 
   return 0;
}

void  tnsguiiconrefueldialog::exec         ( void ) 
{
   refuelvehicle(3); 
   displaymap();
 // repositionmouse ();
}








tnsguiiconputbuilding::tnsguiiconputbuilding ( void )
{
   strcpy ( filename, "building" );
}



int   tnsguiiconputbuilding::available    ( void ) 
{
   if ( actmap->gameparameter ) 
      if ( actmap->gameparameter[cgp_forbid_building_construction] )
         return 0;

    pfield fld = getactfield();
    if (moveparams.movestatus == 0) { 
       if ( fld->vehicle )
          if ( fld->vehicle->attacked == false && fld->vehicle->movement == fld->vehicle->typ->movement[log2(fld->vehicle->height)]) 
             if (fld->vehicle->color == actmap->actplayer * 8)
               if (fld->vehicle->functions & (cfputbuilding | cfspecificbuildingconstruction))
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
   if (moveparams.movestatus == 0) {
      putbuildinglevel1(); 
      displaymap();
    // repositionmouse ();
   }
   else 
      if (moveparams.movestatus == 111) {
         putbuildinglevel2(moveparams.buildingtobuild, getxpos(), getypos()); 
         displaymap();
      } else 
         if (moveparams.movestatus == 112) {
            putbuildinglevel3( getxpos(), getypos()); 
            displaymap();
            dashboard.x = 0xffff;
          // repositionmouse ();
         }
}




tnsguiicondestructbuilding::tnsguiicondestructbuilding ( void )
{
   strcpy ( filename, "abriss" );
}



int   tnsguiicondestructbuilding::available    ( void ) 
{
    pfield fld = getactfield();
    if (moveparams.movestatus == 0) { 
       if ( fld->vehicle )
          if ( fld->vehicle->attacked == false && fld->vehicle->movement == fld->vehicle->typ->movement[log2(fld->vehicle->height)]) 
             if (fld->vehicle->color == actmap->actplayer * 8)
               if (fld->vehicle->functions & cfputbuilding )
                  if ( fld->vehicle->fuel >= destruct_building_fuel_usage * fld->vehicle->typ->fuelconsumption )
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
   if (moveparams.movestatus == 0) {
      destructbuildinglevel1( getxpos(), getypos() ); 
      displaymap();
   }
   else 
      if (moveparams.movestatus == 115) {
         destructbuildinglevel2( getxpos(), getypos()); 
         displaymap();
      }
}




tnsguiicondig::tnsguiicondig ( void )
{
   strcpy ( filename, "dig" );
}



int   tnsguiicondig::available    ( void ) 
{
   pfield fld = getactfield();
   if (fld->vehicle != NULL) 
      if (fld->vehicle->color == actmap->actplayer * 8) 
         if ( (fld->vehicle->functions &  cfmanualdigger) && !(fld->vehicle->functions &  cfautodigger) )
            if (moveparams.movestatus == 0) 
               if ((fld->vehicle->typ->wait==false && fld->vehicle->movement >= searchforresorcesmovedecrease ) || fld->vehicle->movement == fld->vehicle->typ->movement[log2(fld->vehicle->height)])
                 return true;
   return 0;
}

void  tnsguiicondig::exec         ( void ) 
{
    searchforminablefields( getactfield()->vehicle ) ;
    showresources = 1;
    dashboard.x = 0xffff;
    displaymap();
  // repositionmouse ();
}







tnsguiiconviewmap::tnsguiiconviewmap ( void )
{
   strcpy ( filename, "viewmap" );
}



int   tnsguiiconviewmap::available    ( void ) 
{
   if ( moveparams.movestatus == 0 )
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
   strcpy ( filename, "reacfire" );
}

int   tnsguiiconenablereactionfire::available    ( void ) 
{
   pvehicle eht = getactfield()->vehicle;
   if ( eht )
      if ( eht->color == actmap->actplayer * 8) 
         if ( !eht->reactionfire_active )
            if ( moveparams.movestatus == 0 )
               if ( weapexist ( eht ))
                  return 1;

   return 0;

}

void  tnsguiiconenablereactionfire::exec         ( void ) 
{
   getactfield()->vehicle->enablereactionfire();
   dashboard.x = 0xffff;
   displaymap();
}



tnsguiicondisablereactionfire::tnsguiicondisablereactionfire ( void )
{
   strcpy ( filename, "reacfoff" );
}

int   tnsguiicondisablereactionfire::available    ( void ) 
{
   pvehicle eht = getactfield()->vehicle;
   if ( eht )
      if ( eht->color == actmap->actplayer * 8) 
         if ( eht->reactionfire_active )
            if ( moveparams.movestatus == 0 )
               if ( weapexist ( eht ))
                  return 1;

   return 0;

}

void  tnsguiicondisablereactionfire::exec         ( void ) 
{
   getactfield()->vehicle->disablereactionfire();
   dashboard.x = 0xffff;
}







int tnsguiiconcontainer :: containeractive;

tnsguiiconcontainer ::tnsguiiconcontainer ( void )
{
   strcpy ( filename, "loading" );
   containeractive = 0;
}

int tnsguiiconcontainer :: available    ( void ) 
{
  pfield fld = getactfield();
  if ( fieldvisiblenow ( fld ))
     if ( !containeractive && !moveparams.movestatus )
        if ( fld->building  &&  ((fld->building->color == actmap->actplayer * 8) || (fld->building->color == 8*8) )) 
           if ( fld->building->completion == fld->building->typ->construction_steps-1 )
              return 1;
           else
              return 0;
         else 
           if ( fld->vehicle && fld->vehicle->typ->loadcapacity  &&  fld->vehicle->color == actmap->actplayer * 8 )
              return 1;
           else
              return 0;
  return 0;
}

void tnsguiiconcontainer :: exec         ( void ) 
{
   containeractive++;
   mousecontrol->reset();
   pfield fld = getactfield ();
   if ( fld->vehicle && fld->building )
      displaymessage( "gui.cpp   tnsguiiconcontainer :: exec  ; both unit and building on a field", 2 ); 
   container( fld->vehicle, fld->building );
   containeractive--;
}






tnsguiiconcancel::tnsguiiconcancel ( void )
{
   strcpy ( filename, "cancel" );
   forcedeneable = 0;
}



int   tnsguiiconcancel::available    ( void ) 
{
   if ( moveparams.movestatus )
      return 1;
   if ( forcedeneable )
      return 2;
   return 0;
}

void  tnsguiiconcancel::exec         ( void ) 
{
   if ( moveparams.movestatus ) {
      moveparams.movestatus = 0; 
      cleartemps(7); 
      dashboard.x = 0xffff;
      displaymap(); 
   }
}












void    tselectobjectcontainerguihost :: init ( int resolutionx, int resolutiony )
{
   tguihost :: init ( resolutionx, resolutiony );
   icons = new pnputobjectcontainerguiicon[ 1 + objecttypenum * 2 ] ;
   icons[0] = new tnputobjectcontainerguiicon ( NULL, 1 );
   for (int i = 0; i < objecttypenum ; i++ ) 
      if ( getobjecttype_forpos( i ) ) {
         icons[1+i*2] = new tnputobjectcontainerguiicon ( getobjecttype_forpos( i ), 1 );
         icons[1+i*2+1] = new tnputobjectcontainerguiicon ( getobjecttype_forpos( i ), 0 );
      }


   icons[0]->bfrst()->sethost ( this );
   setfirsticon ( icons[0]->frst() );


}

void tselectobjectcontainerguihost ::reset ( void )
{
   tguihost::reset();

   cancel = 0;
}  










void    tselectvehiclecontainerguihost :: init ( int resolutionx, int resolutiony )
{
   tguihost :: init ( resolutionx, resolutiony );
   icons = new pnputvehiclecontainerguiicon[ 1 + vehicletypenum ] ;
   icons[0] = new tnputvehiclecontainerguiicon ( NULL );
   for (int i = 0; i < vehicletypenum ; i++ ) 
      if ( getvehicletype_forpos( i ) ) 
         icons[1+i] = new tnputvehiclecontainerguiicon ( getvehicletype_forpos( i ) );


   icons[0]->bfrst()->sethost ( this );
   setfirsticon ( icons[0]->frst() );

}

void tselectvehiclecontainerguihost ::reset ( pvehicle _constructingvehicle )
{
   constructingvehicle = _constructingvehicle;

   tguihost::reset();
   cancel = 0;
}  







tselectbuildingguihost :: tselectbuildingguihost( void )
{
    selectedbuilding = NULL;
    vehicle = NULL;
}   

void    tselectbuildingguihost :: init ( int resolutionx, int resolutiony )
{                                      
   tguihost :: init ( resolutionx, resolutiony );
   icons = new ( pnputbuildingguiicon[ buildingtypenum ] );
   for (int i = 0; i < buildingtypenum ; i++ ) 
      if ( getbuildingtype_forpos( i ) )
         icons[i] = new tnputbuildingguiicon ( getbuildingtype_forpos( i ) );


   icons[0]->bfrst()->sethost ( this );
   setfirsticon ( icons[0]->frst() );

}

void tselectbuildingguihost ::reset ( pvehicle v )
{
   vehicle = v;
   tguihost::reset();
   selectedbuilding = NULL;
              
   cancel = 0;
}

void tguihost::reset ( void )
{
   restorebackground();
   numpainted = 0;
   infotextshown = NULL;
   for (int i = 0; i < iconspaintable ; i++ ) 
      actshownicons[i/columncount][i%columncount] = NULL;
}











static pnputbuildingguiicon tnputbuildingguiicon :: first    = NULL;
static int             tnputbuildingguiicon :: buildnum = 0;

tnputbuildingguiicon :: tnputbuildingguiicon ( pbuildingtype bld )
{
   next = first;
   first = this;
   building = bld;
   buildnum++;
   picture[0]    = building->guibuildicon;
   infotext      = new char[100];
   sprintf ( infotext, "%s : %d material and %d fuel needed", building->name, building->produktionskosten.material, building->produktionskosten.sprit );
   if ( building->construction_steps > 1 ) {
      char tmp[50];
      sprintf(tmp, " ; %d turns required", building->construction_steps );
      strcat ( infotext, tmp );
   }

}

tnputbuildingguiicon ::  ~tnputbuildingguiicon ( )
{
   delete[] infotext;
}



pnguiicon   tnputbuildingguiicon::nxt( void )
{
   return next;
}

void      tnputbuildingguiicon::setnxt   ( pnguiicon ts )
{
   next = (pnputbuildingguiicon) ts ;
}

pnguiicon   tnputbuildingguiicon::frst( void )
{
   return first;
}

void        tnputbuildingguiicon::setfrst  ( pnguiicon ts )
{
   first = (pnputbuildingguiicon) ts;
}

pnputbuildingguiicon   tnputbuildingguiicon::bnxt( void )
{
   return next;
}

pnputbuildingguiicon   tnputbuildingguiicon::bfrst( void )
{
   return first;
}

void              tnputbuildingguiicon::exec( void )
{
   bldhost->selectedbuilding = building;
}


int         tnputbuildingguiicon::available( void )
{
   return bldhost->vehicle->buildingconstructable ( building );
}




void   tnputbuildingguiicon :: sethost      ( pselectbuildingguihost hst )
{
   bldhost = hst;
   host = hst;
   if ( bnxt () )
      bnxt()->sethost( hst );
}








static pnputobjectcontainerguiicon tnputobjectcontainerguiicon :: first    = NULL;
static int             tnputobjectcontainerguiicon :: buildnum = 0;

tnputobjectcontainerguiicon :: tnputobjectcontainerguiicon ( pobjecttype obj, int bld )
{
   next = first;
   first = this;
   object = obj;
   forcedeneable = 0;
   if ( obj ) {
      priority = 100;
      buildnum++;
      build = bld;
      infotext      = new char[100];
      if ( bld ) {
         picture[0]    = object->buildicon;
         sprintf ( infotext, "%s : %d material and %d fuel needed", object->name, object->buildcost.a.material, object->buildcost.a.fuel );
      } else {
         picture[0]    = object->removeicon;
         sprintf ( infotext, "%s : %d material and %d fuel needed", object->name, object->removecost.a.material, object->removecost.a.fuel );
      }
   } else {
        picture[0] = icons.selectweaponguicancel;
        infotext = "Cancel";
        priority = 10;
   }

}
                                                                                                                             
tnputobjectcontainerguiicon ::  ~tnputobjectcontainerguiicon ( )                                      
{
   delete[] infotext;
}



pnguiicon   tnputobjectcontainerguiicon::nxt( void )
{
   return next;
}

void      tnputobjectcontainerguiicon::setnxt   ( pnguiicon ts )
{
   next = (pnputobjectcontainerguiicon) ts ;
}

pnguiicon   tnputobjectcontainerguiicon::frst( void )
{
   return first;
}

void        tnputobjectcontainerguiicon::setfrst  ( pnguiicon ts )
{
   first = (pnputobjectcontainerguiicon) ts;
}

pnputobjectcontainerguiicon   tnputobjectcontainerguiicon::bnxt( void )
{
   return next;
}

pnputobjectcontainerguiicon   tnputobjectcontainerguiicon::bfrst( void )
{
   return first;
}

void              tnputobjectcontainerguiicon::exec( void )
{
   if ( object ) 
      setspec ( object );
   else
      build_objects_reset();
}


int         tnputobjectcontainerguiicon::available( void )
{
   if ( object ) {
      if ( build ) {
         if ( object_constructable ( getxpos(), getypos(), object )) 
            if ( actmap->objectcrc ) {
               if ( actmap->objectcrc->speedcrccheck->checkobj2 ( object, 0 ))
                  return true;                          
            } else
               return true;
      } else {
         if ( object_removeable ( getxpos(), getypos(), object )) 
            if ( actmap->objectcrc ) {
               if ( actmap->objectcrc->speedcrccheck->checkobj2 ( object, 0 ))
                  return true;
            } else
               return true;
      }     
   
      return false;
   } else 
      return 1;
}




void   tnputobjectcontainerguiicon :: sethost      ( pselectobjectcontainerguihost hst )
{
   bldhost = hst;
   host = hst;
   if ( bnxt () )
      bnxt()->sethost( hst );
}









static pnputvehiclecontainerguiicon tnputvehiclecontainerguiicon :: first    = NULL;
static int                          tnputvehiclecontainerguiicon :: buildnum = 0;


tnputvehiclecontainerguiicon :: tnputvehiclecontainerguiicon ( pvehicletype obj )
{
   next = first;
   first = this;
   vehicle = obj;
   forcedeneable = 0;
   if ( obj ) {
      priority = 100;
      buildnum++;
      infotext      = new char[100];
      picture[0]    = vehicle->buildicon;
      char* c;
      if ( vehicle->name && vehicle->name[0] )
         c = vehicle->name;
      else
         c = vehicle->description;

      sprintf ( infotext, "%s : %d material and %d fuel needed", c, vehicle->production.material, vehicle->production.energy );
   } else {
      picture[0] = icons.selectweaponguicancel;
      infotext = "Cancel";
      priority = 10;
   }

}
                                                                                                                             
tnputvehiclecontainerguiicon ::  ~tnputvehiclecontainerguiicon ( )                                      
{
   delete[] infotext;
}



pnguiicon   tnputvehiclecontainerguiicon::nxt( void )
{
   return next;
}

void      tnputvehiclecontainerguiicon::setnxt   ( pnguiicon ts )
{
   next = (pnputvehiclecontainerguiicon) ts ;
}

pnguiicon   tnputvehiclecontainerguiicon::frst( void )
{
   return first;
}

void        tnputvehiclecontainerguiicon::setfrst  ( pnguiicon ts )
{
   first = (pnputvehiclecontainerguiicon) ts;
}

pnputvehiclecontainerguiicon   tnputvehiclecontainerguiicon::bnxt( void )
{
   return next;
}

pnputvehiclecontainerguiicon   tnputvehiclecontainerguiicon::bfrst( void )
{
   return first;
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

         int r = 0;
         pvehicle actvehicle = bldhost->constructingvehicle;

         for ( int i = 0; i < actvehicle->typ->vehiclesbuildablenum; i++ ) 
            if ( actvehicle->typ->vehiclesbuildableid[i] == vehicle->id ) {
               if ( actvehicle->vehicleconstructable ( vehicle, getxpos(), getypos() ))
                  return 1;
            }
      }
      return false;
   } else 
      return 1;
}




void   tnputvehiclecontainerguiicon :: sethost      ( pselectvehiclecontainerguihost hst )
{
   bldhost = hst;
   host = hst;
   if ( bnxt () )
      bnxt()->sethost( hst );
}








tselectweaponguihost::tselectweaponguihost ( void )
{
   chainiconstohost ( &icon[0] );
}

void tselectweaponguihost :: init ( int resolutionx, int resolutiony )
{
   tguihost :: init ( resolutionx, resolutiony );
   
    pvehicle eht = getfield(moveparams.movesx,moveparams.movesy)->vehicle;
    
    pattackweap atw = attackpossible(eht, getxpos(),getypos());
 
    getfirsticon()->setup ( atw, 0 );
    
    delete atw;

    x = getxpos();
    y = getypos();
}




int     tselectweaponguihost ::  painticons ( void )
{
   checkcoordinates();
   return tguihost::painticons();
}


void    tselectweaponguihost ::  checkforkey ( tkey key )
{
   checkcoordinates();
   tguihost::checkforkey( key );            
}

void    tselectweaponguihost ::  checkformouse ( void )
{
   checkcoordinates();        
   tguihost::checkformouse ( );
}

void    tselectweaponguihost ::  checkcoordinates ( void )
{
   if ( x != getxpos()   ||  y != getypos()  )
      init ( hgmp->resolutionx, hgmp->resolutiony );
                                                          
}


pnweapselguiicon tselectweaponguihost :: getfirsticon( void )
{
   return first_icon;
}
void      tselectweaponguihost :: setfirsticon( pnguiicon ic )
{
   first_icon = (pnweapselguiicon)  ic;
}



pnguiicon tselectbuildingguihost :: getfirsticon( void )
{
   return first_icon;
}
void      tselectbuildingguihost :: setfirsticon( pnguiicon ic )
{
   first_icon = ic;
}


pnguiicon tselectobjectcontainerguihost :: getfirsticon( void )
{
   return first_icon;
}
void      tselectobjectcontainerguihost :: setfirsticon( pnguiicon ic )
{
   first_icon = ic;
}


pnguiicon tselectvehiclecontainerguihost :: getfirsticon( void )
{
   return first_icon;
}
void      tselectvehiclecontainerguihost :: setfirsticon( pnguiicon ic )
{
   first_icon = ic;
}


static pnweapselguiicon tnweapselguiicon::first = NULL;

tnweapselguiicon::tnweapselguiicon ( void )
{
   next = first;
   first = this;
   iconnum = -1;
   weapnum = -1;
   typ = -1;                   
   strength = -1;
   infotext = &infotextbuf[0];
}


pnweapselguiicon   tnweapselguiicon::nxt( void )
{
   return next;
}
void      tnweapselguiicon::setnxt   ( pnguiicon ts )
{
   next = (pnweapselguiicon) ts ;
}

pnweapselguiicon   tnweapselguiicon::frst( void )
{
   return first;
}

void        tnweapselguiicon::setfrst  ( pnguiicon ts )
{
   first = (pnweapselguiicon) ts;
}


int         tnweapselguiicon::available    ( void )
{
   if ( typ != -1 )
      return 1;
   else
      return 0;
}

char*       tnweapselguiicon::getinfotext  ( void )
{
   if ( weapnum > -1 ) {
      infotext[0] = '(';
      infotext[1] = '~';
      infotext[2] = 'A'+weapnum;
      infotext[3] = '~';
      infotext[4] = ')';
      infotext[5] = ' ';
      infotext[6] = 0;
   
      pvehicle eht = getfield ( moveparams.movesx, moveparams.movesy ) -> vehicle;
   
      if ( typ == cwairmissilen   ||   typ == cwgroundmissilen ) {
         if ( eht->height >= chtieffliegend ) 
            strcat( infotext, "air - ");
         else 
            strcat( infotext, "ground -  ");
      } /* endif */
   
      strcat( infotext, cwaffentypen[typ] );

/*
      strcat( infotext, "; abs strength: " );
      int strength = eht->weapstrength [ weapnum ];
      strcat( infotext, strrr( strength ) );
*/

      strcat( infotext, "; eff strength: " );
/*
      int dist = beeline ( moveparams.movesx, moveparams.movesy , getxpos(), getypos() );
      strength = eht->weapstrength [ weapnum ] * weapdist->getweapstrength ( &eht->typ->weapons->weapon[ weapnum ], dist  ) / 256;
      strcat( infotext, strrr( strength ) );
      strcat( infotext, "; expected enemy damage: " );
*/   
      pfield fld = getactfield();

      int dam = 0;
      if ( fld->vehicle ) {
         tunitattacksunit battle;
         battle.setup ( eht, fld->vehicle, 1, weapnum );
         battle.calc ( );

         strcat( infotext, strrr ( battle.av.strength) );
         strcat( infotext, "; expected enemy damage: " );

         dam = battle.dv.damage;
      } else
      if ( fld->building ) {
         tunitattacksbuilding battle;
         battle.setup ( eht, getxpos(), getypos(), weapnum );
         battle.calc ( );
         strcat( infotext, strrr ( battle.av.strength) );
         strcat( infotext, "; expected enemy damage: " );
         dam = battle.dv.damage;
      } else
      if ( fld->object ) {
         tunitattacksobject battle;
         battle.setup ( eht, getxpos(), getypos(), weapnum );
         battle.calc ( );
         strcat( infotext, strrr ( battle.av.strength) );
         strcat( infotext, "; expected enemy damage: " );
         dam = battle.dv.damage;
      }

      strcat( infotext, strrr( dam ) );
   
      return infotext;
   } else
      return "cancel ( ~ESC~ )";

}


void  tnweapselguiicon::exec         ( void )
{
   if ( available() ) {
      if ( weapnum >= 0 ) {
         attack ( false, weapnum );
         displaymap();
      } else {
         moveparams.movestatus = 0;
         cleartemps ( 0xff );
         displaymap();
         dashboard.x = 0xffff;
      }
      actgui = &gui;
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
   if ( n < atw->count ) {
      typ      = log2( atw->typ[n] );
      strength = atw->strength[n];
      weapnum  = atw->num[n];
      pvehicle eht = getfield ( moveparams.movesx, moveparams.movesy ) -> vehicle;
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
      if ( n == atw->count ) {
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

   if ( next )
      next->setup ( atw, ++n );
}

















static preplayguiicon treplayguiicon ::first = NULL;

treplayguiicon::treplayguiicon ( void )
{
   next = first;
   first = this;
}


pnguiicon   treplayguiicon::nxt( void )
{
   return next;
}
void      treplayguiicon::setnxt   ( pnguiicon ts )
{
   next = (preplayguiicon) ts ;
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
   strcpy ( filename, "rp_play" );
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
  dashboard.x = -1;

}




trguiicon_pause :: trguiicon_pause ( void )
{
   strcpy ( filename, "rp_pause" );
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
   dashboard.x = -1;
}




trguiicon_faster :: trguiicon_faster ( void )
{
   strcpy ( filename, "rp_fast" );
}

int trguiicon_faster :: available ( void )
{
   if ( runreplay.status == 2 )
      if ( gameoptions.replayspeed > 0 )
        return 1;

   return 0;
}

void trguiicon_faster :: exec ( void )
{
   if ( gameoptions.replayspeed > 20 )
      gameoptions.replayspeed -= 20;
   else
      gameoptions.replayspeed = 0;

   gameoptions.changed = 1;
   displaymessage2 ( "delay set to %d / 100 sec", gameoptions.replayspeed );
   dashboard.x = -1;
}


trguiicon_slower :: trguiicon_slower ( void )
{
   strcpy ( filename, "rp_slow" );
}

int trguiicon_slower :: available ( void )
{
   if ( runreplay.status == 2 )
      return 1;

   return 0;
}

void trguiicon_slower :: exec ( void )
{
   gameoptions.replayspeed += 20;
   gameoptions.changed = 1;
   displaymessage2 ( "delay set to %d / 100 sec", gameoptions.replayspeed );
   dashboard.x = -1;
}



trguiicon_back :: trguiicon_back ( void )
{
   strcpy ( filename, "rp_back" );
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
   strcpy ( filename, "rp_exit" );
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
   strcpy ( filename, "cancel" );
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
   tguihost::init ( resolutionx, resolutiony );
}

void   treplayguihost :: bi2control (  )
{
   icons.cancel.display();
   tguihost::bi2control (  );
}

