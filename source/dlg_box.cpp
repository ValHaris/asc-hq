/*! \file dlg_box.cpp
    \brief Some basic classes from which all of ASC's dialogs are derived
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
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <cstring>
#include <iostream>

#include "typen.h"
#include "basegfx.h"
#include "newfont.h"
#include "spfst.h"
#include "loaders.h"
#include "misc.h"
#include "events.h"
#include "stack.h"
#include "dlg_box.h"
#include "paradialog.h"
#include "widgets/textrenderer.h"


#ifdef _WIN32_
 #include <windows.h>
 #include <winuser.h>
#endif



char strrstring[200];

void  strrd8d(int  l, char* s)
{ 
  itoa ( l / minmalq, s, 10);
} 

void  strrd8u(int  l, char* s)
{ 
  itoa ( (l + minmalq - 1) / minmalq, s, 10);
} 

void  strrd8n(int  l, char* s)
{ 
  itoa ( (l + minmalq/2) / minmalq, s, 10);
} 


char*  strrrd8d(int  l)
{ 
 itoa ( l / minmalq, strrstring, 10);

 return strrstring;
} 

char*  strrrd8u(int  l)
{ 
  itoa ( (l + minmalq - 1) / minmalq, strrstring, 10);

 return strrstring;
} 

char*  strrrd8n(int  l)
{ 
 itoa ( (l + minmalq/2) / minmalq, strrstring, 10);

 return strrstring;
} 


void *dialogtexture = NULL;

int actdisplayedmessage = 0;
long int lastdisplayedmessageticker = 0xffffff;


tvirtualscreenbuf virtualscreenbuf; 


tvirtualscreenbuf :: tvirtualscreenbuf ( void )
{
   buf = NULL;
   size = 0;
}                       

void tvirtualscreenbuf:: init ( void )
{
   size = hgmp->bytesperscanline * hgmp->resolutiony;
   buf = asc_malloc( size );
}

tvirtualscreenbuf:: ~tvirtualscreenbuf ()
{
   asc_free( buf );
   buf = NULL;
}


char         getletter( const char *       s)
{ 
  const char*   c = s;

  while ( *c && (*c != 126 )) 
     c++;

  if (*c == '~' ) {
     c++;
     return *c;
  } else {
     return 0;
  } /* endif */
}


collategraphicoperations* tdialogbox::pcgo = NULL;

tdialogbox::tdialogbox()
{
   eventQueue = setEventRouting ( false, true );

   npush ( activefontsettings );
   activefontsettings.font = schriften.smallarial;
   activefontsettings.color = black;
   activefontsettings.background = 255;
   activefontsettings.markcolor = red;


   virtualbufoffset = 0;
   boxstatus = 0;

   dlg_mode =  0;
   int rdw = 0;

   if ( first ) {
     pdialogbox rn = first;
     rdw &= rn->getcapabilities() & 1;
     while ( rn->next ) {
        rn = rn->next;
        rdw &= rn->getcapabilities() & 1;
     }


     prev = rn;
     rn->next = this;
   } else {
      first = this;
      prev = NULL;
   }
   next = NULL;

   if ( rdw )
     dlg_mode |= 2;
}


int  tdialogbox::getcapabilities ( void )
{
   return 0;
}

extern void repaintdisplay ( void );


void          tdialogbox::repaintdisplay( void )
{/*
   int ms = getmousestatus();
   if ( ms == 2 )
      mousevisible ( false );

   npush ( *agmp );
   npush ( *hgmp );

   hgmp->linearaddress = (PointerSizedInt) virtualscreenbuf.buf;
   hgmp->windowstatus = 100;

   *agmp = *hgmp;
   
   ::repaintdisplay();



   npop  ( *hgmp );
   setvirtualframebuf();
   if ( first )
      first->setclipping();

   *agmp = *hgmp;

   putspritetexture ( 0, 0, hgmp->resolutionx-1, hgmp->resolutiony-1, virtualscreenbuf.buf );


   npop  ( *agmp );

   if ( ms == 2 )
      mousevisible ( true );
      */

}

void          tdialogbox::redrawall ( void )
{
   paint();
   if ( prev )
      prev->redrawall(); 
   else
      repaintDisplay();
}

void           tdialogbox::redrawall2 ( int xx1, int yy1, int xx2, int yy2 )
{
   paint ();
   if ( x1 > xx1 || y1 > yy1 || x1+xsize < xx2 || y1+ysize < yy2 )
      if ( prev )
         prev->redrawall2 ( xx1, yy1, xx2, yy2 ); 
      else
         repaintDisplay();
}


pdialogbox tdialogbox::first = NULL; 

void          tdialogbox::setvirtualframebuf ( void )
{
   agmp->linearaddress = (PointerSizedInt) virtualscreenbuf.buf;
   agmp->windowstatus = 100;
   agmp->scanlinelength = hgmp->scanlinelength;
}

void          tdialogbox::setclipping ( void )
{
   bar ( x1, y1, x1 + xsize, y1 + ysize, 255 );
   if ( next )
      next->setclipping ( );
}

void          tdialogbox::copyvirtualframebuf ( void )
{
   ms = getmousestatus(); 
   if (ms == 2)
      mousevisible(false);

   npush ( *agmp );
   void* buf = (void*) agmp->linearaddress;
   *agmp = *hgmp;

   putspritetexture ( x1, y1, x1 + xsize, y1 + ysize, buf );

   /*
   for ( int y = y1; y <= y1 + ysize; y++ )
      for ( int x = x1; x <= x1 + xsize; x++ ) {
         char c = buf[ virtualbufoffset + y * agmp->scanlinelength + x ];
         char d = buf[ virtualbufoffset + y * agmp->scanlinelength + x+1 ];
         if ( d != 255 )
            putpixel ( x+1, y, lightblue );
         if ( c != 255 )
            putpixel ( x, y, c );
      }
   */

   npop  ( *agmp );

   if (ms == 2)
      mousevisible(true);

}

void         tdialogbox::paint     ( void )
{
   setvirtualframebuf();
   redraw();
   if ( next )
     next->setclipping();
   copyvirtualframebuf();
   *agmp = *hgmp;
}


int getplayercolor ( int i )
{
   if ( actmap ) {
      int textcolor =  i * 8 + 21;
      if ( i == 7 || i == 2 )
         textcolor += 1;
      return textcolor;
   } else
      return 20;
}

void         tdialogbox::init(void)
{ 
   imagesaved = false; 
   if ( actmap && actmap->actplayer != -1 ) {
      textcolor = getplayercolor ( actmap->actplayer );
   }
   else
      textcolor = 20;

   firstbutton = NULL; 
   windowstyle = dlg_in3d | dlg_3dtitle; 
   x1 = 50; 
   xsize = 540; 
   y1 = 50; 
   ysize = 380; 
   starty = 40; 
   title = "dialogbox";
   npush( activefontsettings );
   activefontsettings.height = 0;
   activefontsettings.length = 0;
   memset(taborder, 0, sizeof(taborder));
   tabcount = 0; 
   markedtab = 0; 
   disablecolor = darkgray; 
   boxstatus = 1;
   dlg_mode = 0; // |= getcapabilities();
} 





#include "dlgraph.cpp"









void         tdialogbox::changecoordinates(void)
{ 
     /* runerror(211); */ 
} 

tdlgengine::pbutton tdlgengine :: getbutton ( int id )
{
   pbutton pb = firstbutton;
   while ( pb &&  (pb->id != id)) 
       pb = pb->next;
   return pb;
}

void         tdlgengine::buttonpressed(int         id)
{ 
  pbutton      pb; 
  int          w;
  Uint16 *pw, *pw2;

  pb = firstbutton;
  while ( pb ) {
    if ((pb->art == 5)) {       /* Scrollbar */
      pw = (Uint16*) pb->data;
      pw2 = (Uint16*) pb->data2;
      w = *pw; 
      if ((pb->id + 1 == id)) { 
        if (taste == ct_pup) { 
          if (*pw >= pb->max) 
            *pw -= pb->max - 1;
          else 
            *pw = 0; 
        } 
        else 
         if (taste == ct_pos1) { 
            *pw = 0; 
         } 
         else 
           if (*pw > 0) {
              if (  pb->newpressed == 0 )          // Tastatur
                  (*pw) --;
              else
                 if ( pb->newpressed == 1) {
                    (*pw) --;
                    pb->lasttick = ticker;
                 } else
                    if ( pb->newpressed == 2) {
                       int dst = ticker - pb->lasttick;
                       if  ( *pw  < dst )
                          (*pw) = 0;
                       else
                          (*pw) -= dst;
                       pb->lasttick = ticker;
                    }
           }
      } 
      if ((pb->id + 2 == id)) { 
        if (taste == ct_pdown) { 
          if (*pw + (pb->max * 2) - 1 <= *pw2) 
            *pw += pb->max - 1;
          else 
            *pw = *pw2 - pb->max; 
        } 
        else 
          if (taste == ct_ende) { 
            *pw = *pw2 - pb->max; 
          } 
          else 
            if ( *pw + pb->max < *pw2 ) {

               if (  pb->newpressed == 0 )          // Tastatur
                  (*pw)++;
               else
                  if ( pb->newpressed == 1) {
                     (*pw) ++;
                     pb->lasttick = ticker;
                  } else
                     if ( pb->newpressed == 2) {
                        int dst = ticker - pb->lasttick;
                        if  ( *pw + dst + pb->max >= *pw2 )
                           *pw = *pw2 - pb->max; 

                        else
                           (*pw) += dst;
                        pb->lasttick = ticker;
                     }
            }
      } 
      if (w != *pw) { 
         char mss = getmousestatus();
         if (mss == 2 ) 
            mousevisible(false);
        showbutton(pb->id); 
        buttonpressed(pb->id); 
         if (mss == 2 ) 
            mousevisible(true);
      } 

    } 
    pb = pb->next; 
  } 
} 


void         tdlgengine::addbutton( const char *       ltxt,
                       tmouserect   rect1,
                       int         lart,
                       int         lstyle,
                       int         lid,
                       char      enabled)
{
   addbutton ( ltxt, rect1.x1, rect1.y1, rect1.x2, rect1.y2, lart, lstyle, lid, enabled );
}

const char* emptystring = "";

void         tdlgengine::addbutton(  const char *       ltxt,
                                 int          lx1,
                                 int          ly1,
                                 int          lx2,
                                 int          ly2,
                                 int         lart,
                                 int         lstyle,
                                 int         lid,
                                 char      enabled)
{ 
  pbutton      pb; 
  char         ch; 

   pb = firstbutton; 
   if (lid <= 0) 
      displaymessage("tdialogbox: \n id equal or less then 0\n:%d\n",2, lid);
   while ( pb ) { 
      if (pb->id == lid) 
         displaymessage("tdialogbox: duplicate button id: %d\n",2, lid);
      
      pb = pb->next; 
   } 
   pb = new tbutton;
   pb->x1 = lx1; 
   pb->x2 = lx2; 
   pb->y1 = ly1; 
   pb->y2 = ly2; 
   pb->style = lstyle; 
   pb->id = lid; 
   pb->next = firstbutton; 
   if ( ltxt )
      pb->text = ltxt;
   else
      pb->text = emptystring;
   pb->art = lart; 
   pb->active = enabled; 
   pb->status = 1; 
   pb->scrollspeed = 30;
   pb->pressed = 0;
   pb->newpressed = 0;

   firstbutton = pb; 

   ch = getletter(pb->text); 
   if (ch != 0) { 
      pb->key[0] = char2key( tolower(ch) );
      pb->keynum = 1; 
   } 
   else 
      pb->keynum = 0; 
   pb->markedkeynum = 1; 
   pb->markedkey[0] = ct_enter;

} 


void         tdlgengine::addscrollbar(tmouserect rec,
                          int*         numberofitems,
                          int          itemsvisible,
                          int*         actitem,
                          int         lid,
                          int         keys)
{
   addscrollbar ( rec.x1, rec.y1, rec.x2, rec.y2, numberofitems, itemsvisible, actitem, lid, keys );
}

void         tdialogbox :: bar ( tmouserect rect, int color )
{
   ::bar ( rect.x1, rect.y1, rect.x2, rect.y2, color );
}

void         tdialogbox :: bar ( int x1, int y1, int x2, int y2, int color )
{
   ::bar ( x1, y1, x2, y2, color );
}


void         tdlgengine::addDropDown( int x1, int y1, int x2, int y2, int ID, const char** entries, int entrynum, int* pos )
{
   pbutton      pb = firstbutton;
   if ( ID <= 0)
      displaymessage("tdialogbox: id equal or less then 0", 2);
   while (pb != NULL) {
      if (pb->id == ID)
         displaymessage("tdialogbox: duplicate button id: %d\n",2, ID);
      pb = pb->next;
   }
   pb = new tbutton;
   pb->x1 = x1;
   pb->x2 = x2;
   pb->y1 = y1;
   pb->y2 = y2;
   pb->style = 0;
   pb->id = ID;
   pb->next = firstbutton;
   pb->text = NULL;
   pb->art = 6;
   pb->active = true;
   pb->status = 1;
   pb->keynum = 0;
   pb->markedkeynum = 0;
   pb->scrollspeed = 30;
   pb->pressed = 0;
   pb->newpressed = 0;
   pb->entries = entries;
   pb->entrynum = entrynum;
   pb->data = pos;
   firstbutton = pb;
}


void         tdlgengine::addscrollbar(int          lx1,
                          int          ly1,
                          int          lx2,
                          int          ly2,
                          int*         numberofitems,
                          int          itemsvisible,
                          int*         actitem,
                          int         lid,
                          int         keys)
{ 
  pbutton      pb; 

   pb = firstbutton; 
   if (lid <= 0) 
      displaymessage("tdialogbox: id equal or less then 0", 2);
   while (pb != NULL) { 
      if (pb->id == lid) 
         displaymessage("tdialogbox: duplicate button id: %d\n",2, lid);
      pb = pb->next; 
   } 
   pb = new tbutton;
   pb->x1 = lx1; 
   pb->x2 = lx2; 
   pb->y1 = ly1 + 13; 
   pb->y2 = ly2 - 13; 
   pb->style = 0; 
   pb->id = lid; 
   pb->next = firstbutton; 
   pb->text = NULL;
   pb->art = 5; 
   pb->active = true; 
   pb->status = 1; 
   pb->keynum = 0; 
   pb->markedkeynum = 0; 
   pb->scrollspeed = 30;
   pb->pressed = 0;
   pb->newpressed = 0;
   firstbutton = pb; 

   addbutton("",lx1+1, ly1 +  1 , lx2 - 1, ly1 + 10, 0, 2, lid + 1, true);
   addbutton("",lx1+1, ly2 - 10 , lx2 - 1, ly2 -  1, 0, 2, lid + 2, true);

   if (keys == 2) { 
     addmarkedkey(lid + 1,ct_up); 
     addmarkedkey(lid + 1,ct_pup); 
     addmarkedkey(lid + 1,ct_pos1); 

     addmarkedkey(lid + 2,ct_down); 
     addmarkedkey(lid + 2,ct_pdown); 
     addmarkedkey(lid + 2,ct_ende); 
   } 
   if (keys == 1) { 
     addkey(lid + 1,ct_up); 
     addkey(lid + 1,ct_pup); 
     addkey(lid + 1,ct_pos1); 

     addkey(lid + 2,ct_down); 
     addkey(lid + 2,ct_pdown); 
     addkey(lid + 2,ct_ende); 
   } 


   pb->data = actitem; 
   pb->data2 = numberofitems;
   pb->max = itemsvisible; 

} 


void         tdialogbox::rebuildtaborder(void)
{ 
  pbutton      pb;
  pbutton      pb2;
  int      i = 0;
  ttaborder    b; 

   pb = firstbutton; 
   tabcount = 0; 
   while (pb != NULL) { 
      if ((pb->art >= 0) && (pb->art <= 3)) 
        if ((pb->status == 1) && pb->active) { 
            tabcount++; 
            taborder[tabcount].id = pb->id; 
            taborder[tabcount].x1 = pb->x1; 
            taborder[tabcount].y1 = pb->y1; 
            taborder[tabcount].x2 = pb->x2; 
            taborder[tabcount].y2 = pb->y2; 
            taborder[tabcount].button = pb; 
         
      } 
      pb = pb->next; 
   } 

   if (tabcount > 1) 
   for (i = 1; i <= tabcount - 1; i++) { 
      if ((taborder[i].y1 > taborder[i + 1].y1) || ((taborder[i].y1 == taborder[i + 1].y1) && (taborder[i].x1 > taborder[i + 1].x1))) 
         { 
            b = taborder[i]; 
            taborder[i] = taborder[i + 1]; 
            taborder[i + 1] = b; 
            if (i > 1) 
               i -= 2;
         } 
   } 

   if (i > 0) { 
      if (markedtab > 0) 
         pb2 = taborder[i].button; 
   } 
   else 
      pb2 = NULL; 

   showtabmark(markedtab); 

   if ( markedtab ) {
      markedtab = 0; 
      if (tabcount > 0) { 
         for (i = 1; i <= tabcount; i++) 
            if (taborder[i].button == pb2) 
               markedtab = i; 
      } 
   }

   showtabmark(markedtab); 
} 


void         tdialogbox::showbutton(int         id)
{ 
  pbutton      pb; 
  int         c; 
  char mss = getmousestatus();

  if (mss == 2) 
     mousevisible(false);
   pb = firstbutton; 
   while (pb != NULL) { 
      if (pb->id == id) 
        if ( pb->pressed == 0) {
            c = pb->status; 
            pb->status = 1; 
            if (pb->active) 
               enablebutton(id); 
            else 
               disablebutton(id); 
   
            if (pb->art == 5) {
               showbutton ( id + 1);
               showbutton ( id + 2);
            } /* endif */
   
            if (c != 1) 
              rebuildtaborder(); 
         } 

      pb = pb->next; 
   } 
  if (mss == 2) 
     mousevisible(true);
} 


void         tdialogbox::hidebutton(int         id)
{ 
  pbutton      pb; 

   pb = firstbutton; 
   while (pb != NULL) { 
      if (pb->id == id) {
         pb->status = 0; 
         if (pb->art == 5) {
            hidebutton ( id + 1);
            hidebutton ( id + 2);
         } /* endif */
      }
      pb = pb->next; 
   } 
   rebuildtaborder(); 
} 



void         tdlgengine::clearkey ( char id )
{
   pbutton pb = firstbutton; 
   while ( pb ) { 
      if ( pb->id == id ) { 
         pb->keynum = 0; 
         pb->markedkeynum = 0; 
      } 
      pb = pb->next; 
   } 
} 

void         tdlgengine::addkey( int         id, tkey         key )
{ 
   pbutton pb = firstbutton; 
   while ( pb ) { 
      if (pb->id == id) { 
         int exist = 0;
         for ( int i = 0; i < pb->keynum; i++ )
            if ( pb->key[i] == key )
               exist++;
         if ( !exist ) {
            pb->key [ pb->keynum ] = key; 
            pb->keynum++; 
         }
      } 
      pb = pb->next; 
   } 
} 


void         tdlgengine::addmarkedkey(int   id, tkey         key)
{ 
  pbutton      pb; 

   pb = firstbutton; 
   while ( pb ) { 
      if (pb->id == id) { 
         int exist = 0;
         for ( int i = 0; i < pb->markedkeynum; i++ )
            if ( pb->markedkey[i]== key )
               exist++;
         if ( !exist ) {
            pb->markedkey [ pb->markedkeynum ] = key; 
            pb->markedkeynum++; 
         }
      } 
      pb = pb->next; 
   } 
} 


void         tdialogbox::setscrollspeed(char        id , int  speed)
{ 
  pbutton      pb; 

   pb = firstbutton; 
   while (pb != NULL) { 
      if (pb->id == id) 
         if (pb->art == 5) {
            setscrollspeed( id + 1, speed );
            setscrollspeed( id + 2, speed );
         } else
            pb->scrollspeed = speed;

      pb = pb->next; 
   } 
} 


void         tdlgengine::addeingabe(int         lid,
                                  void*      data,
                                  int      min,
                                  int      max)
{ 
  pbutton      pb; 

   pb = firstbutton; 
   while (pb != NULL) { 
      if (pb->id == lid) { 
         pb->data = data; 
         if ( min > max ) {
            pb->min = max; 
            pb->max = min; 
         } else {
            pb->min = min; 
            pb->max = max;
         }
         addmarkedkey(pb->id,ct_enter); 
         addmarkedkey(pb->id,ct_space); 
      } 
      pb = pb->next; 
   } 
} 



void         tdialogbox::enablebutton(int         id)
{ 

   pbutton pb = firstbutton; 
   if ( !pb ) 
      return;


   while ( pb   &&  pb->id != id ) 
      pb = pb->next; 

   if ( !pb ) 
      return;

   npush( activefontsettings ); 

   collategraphicoperations cgo ( x1 + pb->x1, max ( y1 + pb->y1 - 20, 0 ), x1 + pb->x2, y1 + pb->y2 );

   char strng[200];
   activefontsettings.font = schriften.smallarial; 
   activefontsettings.justify = lefttext; 
   activefontsettings.length =  pb->x2 - pb->x1 - 10;
   activefontsettings.markcolor = textcolor - 2; 
   activefontsettings.color = textcolor;
   activefontsettings.background = 255;


   if (pb->art == 0) {
      if ((pb->style == 1) || (pb->style == 2))
         newknopf(x1 + pb->x1,y1 + pb->y1,x1 + pb->x2,y1 + pb->y2); 
                  
      if ( pb->text )  
         if (pb->text[0] ) {
            activefontsettings.justify = centertext; 
            showtext3( pb->text,x1 + pb->x1,y1 + (pb->y1 + pb->y2) / 2 - activefontsettings.font->height / 2); 
          }
   } 

   if ((pb->art == 1) || ( pb->art == 2 )) {
      rahmen(true,x1 + pb->x1,y1 + pb->y1,x1 + pb->x2,y1 + pb->y2); 
      paintsurface( pb->x1 + 1, pb->y1 + 1, pb->x2 - 1, pb->y2 - 1 ); 
      if ( pb->text )  
         if (pb->text[0] )
            if ( pb->style != 3 ) {
               showtext3(pb->text,x1 + pb->x1,y1 + pb->y1 - activefontsettings.font->height);
            } else {
               npush ( activefontsettings.length );
               activefontsettings.length = 300;
               cgo.off();
               showtext3(pb->text,x1 + pb->x2 + 10,y1 + pb->y1 );
               cgo.on();
               npop ( activefontsettings.length );
            }

      if (pb->art == 1)  
         showtext2((char*) pb->data , x1 + pb->x1 + 5,y1 + pb->y1 + 2);
      
      if (pb->art == 2) { 
         if (pb->max <= 255 && pb->min >= 0) { 
            char* pbt = (char*) pb->data;
            itoa ( *pbt, strng, 10 );
            showtext2( strng, x1 + pb->x1 + 5,y1 + pb->y1 + 2);
         } 
         else 
            if (pb->max <= 65535 && pb->min >= 0) { 
               Uint16* pw = (Uint16*) pb->data;
               itoa ( *pw, strng, 10 );
               showtext2(strng, x1 + pb->x1 + 5,y1 + pb->y1 + 2);
            }     
            else {
               int* pl = (int*) pb->data;
               itoa ( *pl, strng, 10 );
               showtext2(strng, x1 + pb->x1 + 5,y1 + pb->y1 + 2);
            } 
      } 
   } 
   if (pb->art == 3) { 
      rahmen(true,x1 + pb->x1,y1 + pb->y1,x1 + pb->x1 + (pb->y2 - pb->y1),y1 + pb->y2);

      showtext3(pb->text,x1 + pb->x1 + (pb->y2 - pb->y1) + 5,y1 + (pb->y1 + pb->y2 - activefontsettings.font->height) / 2);

      char* pbl = (char*) pb->data;

      int cl;
      if (*pbl)
         cl = pb->min;
      else
         cl = pb->max;

      line(x1 + pb->x1,y1 + pb->y1,x1 + pb->x1 + (pb->y2 - pb->y1),y1 + pb->y2, cl);
      line(x1 + pb->x1 + (pb->y2 - pb->y1),y1 + pb->y1,x1 + pb->x1,y1 + pb->y2, cl);
   } 

   if (pb->art == 5) {      // Scrollbar
     Uint16* pw = (Uint16*) pb->data;
     Uint16* pw2 =(Uint16*) pb->data2; 
     rahmen(true,x1 + pb->x1,y1 + pb->y1,x1 + pb->x2,y1 + pb->y2);

     paintsurface2(x1 + pb->x1 + 1,y1 + pb->y1 + 1,x1 + pb->x2 - 1,y1 + pb->y2 - 1 );

    // if (pb->max <= *pw2) {
       int l = pb->y2 - pb->y1 - 2;
       rahmen(false, x1 + pb->x1 + 1, y1 + pb->y1 + 1 + l * *pw / *pw2,
                     x1 + pb->x2 - 1, y1 + pb->y1 + 1 + l * (*pw + pb->max) / *pw2);
    // }
    // else
    //   rahmen(true,x1 + pb->x1 + 1,y1 + pb->y1 + 1,x1 + pb->x2 - 1,y1 + pb->y2 - 1);

    }

   if ( pb->art == 6 ) {
      bar ( x1 + pb->x1,y1 + pb->y1,x1 + pb->x2,y1 + pb->y2, lightgray );
      rahmen(true,x1 + pb->x1,y1 + pb->y1,x1 + pb->x2,y1 + pb->y2);
      int pos = *( (int*) pb->data );
      if ( pos < 0 || pos >= pb->entrynum )
         fatalError("invalid pos in dropdown",2);

      showtext3(pb->entries[pos], x1 + pb->x1 + (pb->y2 - pb->y1) + 5, y1 + (pb->y1 + pb->y2 - activefontsettings.font->height) / 2);

      line ( x1 + pb->x2 - 20, y1 + pb->y1 + 4, x1 + pb->x2 - 10, y1 + pb->y1 + 4, textcolor );
      line ( x1 + pb->x2 - 20, y1 + pb->y1 + 4, x1 + pb->x2 - 15, y1 + pb->y1 + 14, textcolor );
      line ( x1 + pb->x2 - 10, y1 + pb->y1 + 4, x1 + pb->x2 - 15, y1 + pb->y1 + 14, textcolor );
   }

   pb->active = true;
   npop( activefontsettings ); 
   rebuildtaborder(); 
} 




void         tdialogbox::disablebutton(int         id)
{ 
  pbutton      pb; 
  int      *pl; 
  Uint16         *pw, *pw2; 
  char         *pbt;
  char      *pbl;
  char*         s;
  char*         t;
  int      l; 
  char cl;

   pb = firstbutton; 
   if ( !pb ) 
     return;
   while ((pb != NULL) && (pb->id != id)) 
      pb = pb->next; 
   if (pb == NULL) 
      return;

   npush(activefontsettings); 
   activefontsettings.font = schriften.smallarial;
   activefontsettings.justify = lefttext; 
   activefontsettings.length =  pb->x2 - pb->x1 - 10;
   activefontsettings.color = disablecolor; 
   
   s = new char[200];
   strcpy(s, pb->text);

   t = strchr( s, 126 );
   while ( t != NULL ) {
      do {
         t[0] = t[1];
         t++;
      } while ( t[0] !=0 ) ;
      t = strchr( s, 126 );
   }

   if (pb->art == 0) { 
      if (pb->text != NULL)  
        if (pb->text[0] != 0) {
           activefontsettings.justify = centertext; 
           showtext2(s, x1 + pb->x1, y1 + (pb->y1 + pb->y2) / 2 - activefontsettings.font->height / 2); 
        }
      if ((pb->style == 1) || (pb->style == 2)) {
         rectangle(x1 + pb->x1,y1 + pb->y1,x1 + pb->x2,y1 + pb->y2,disablecolor); 
         rectangle(x1 + pb->x1 - 1,y1 + pb->y1 - 1,x1 + pb->x2 + 1,y1 + pb->y2 + 1,disablecolor); 
      } 
   } 
   if ((pb->art == 1) || (pb->art == 2)) {
      rectangle(x1 + pb->x1,y1 + pb->y1,x1 + pb->x2,y1 + pb->y2,disablecolor); 
      if (pb->text != NULL)  
        if (pb->text[0] != 0)
           if ( pb->style != 3 )

              showtext2(pb->text,x1 + pb->x1,y1 + pb->y1 - activefontsettings.font->height);
           else {
              npush ( activefontsettings );
              activefontsettings.length = 0;
              showtext2(pb->text,x1 + pb->x2 + 10,y1 + pb->y1 );
              npop ( activefontsettings );
           }

      if (pb->art == 1) { 
         showtext2( (char*)pb->data,x1 + pb->x1 + 5,y1 + pb->y1 + 2);
      } 
      if (pb->art == 2) { 
         if (pb->max <= 255 && pb->min >= 0) { 
            pbt = (char*) pb->data;
            itoa ( *pbt, s, 10 );
            showtext2(s,x1 + pb->x1 + 5,y1 + pb->y1 + 2); 
         } 
         else 
            if (pb->max <= 65535 && pb->min >= 0) { 
               pw = (Uint16*) pb->data;
               itoa ( *pw, s , 10);
               showtext2(s, x1 + pb->x1 + 5,y1 + pb->y1 + 2);
            } 
            else { 
               pl = (int*) pb->data;
               itoa ( *pl, s, 10);
               showtext2(s, x1 + pb->x1 + 5,y1 + pb->y1 + 2);
            } 
      } 
   } 
   if (pb->art == 3) { 
       rectangle(x1 + pb->x1,y1 + pb->y1,x1 + pb->x1 + (pb->y2 - pb->y1),y1 + pb->y2,disablecolor);
       showtext2(s,x1 + pb->x1 + (pb->y2 - pb->y1) + 5,y1 + (pb->y1 + pb->y2 - activefontsettings.font->height) / 2);
       pbl = (char*) pb->data;
       if (*pbl)
         cl = disablecolor; 
       else
         cl = pb->max;
       line(x1 + pb->x1,y1 + pb->y1,x1 + pb->x1 + (pb->y2 - pb->y1),y1 + pb->y2, cl);
       line(x1 + pb->x1 + (pb->y2 - pb->y1),y1 + pb->y1,x1 + pb->x1,y1 + pb->y2, cl);
   } 
   if (pb->art == 5) { 
     pw = (Uint16*) pb->data;
     pw2 = (Uint16*) pb->data2;
     // waitretrace(); 
     rectangle(x1 + pb->x1,y1 + pb->y1,x1 + pb->x2,y1 + pb->y2,disablecolor); 

     paintsurface2(x1 + pb->x1 + 1,y1 + pb->y1 + 1,x1 + pb->x2 - 1,y1 + pb->y2 - 1);

     if (pb->max <= *pw2) { 
       l = pb->y2 - pb->y1 - 2;
       rectangle(x1 + pb->x1 + 1,y1 + pb->y1 + 1 + l * *pw / *pw2,x1 + pb->x2 - 1,y1 + pb->y1 + 1 + l * (*pw + pb->max) / *pw2,disablecolor);
     } 
     else 
       rectangle(x1 + pb->x1 + 1,y1 + pb->y1 + 1,x1 + pb->x2 - 1,y1 + pb->y2 - 1,disablecolor); 

   } 
   delete[] s;
   pb->active = false; 
   npop( activefontsettings );
   rebuildtaborder(); 
} 


void         tdialogbox::redraw(void)
{
  pbutton      pb; 


   knopf(x1,y1,x1 + xsize,y1 + ysize); 


   rahmen(false, x1,y1,x1 + xsize, y1 + ysize);


   activefontsettings.color = white; 
   activefontsettings.background = 255;
   activefontsettings.font = schriften.smallarial; 
   activefontsettings.justify = centertext; 
   activefontsettings.length = xsize - 10;
   activefontsettings.height = 00;
   
   if (windowstyle & dlg_wintitle ) { 
      starty = 25; 
      rahmen(true,x1 + 5,y1 + 3,x1 + xsize - 5,y1 + 20); 
      bar(x1 + 6,y1 + 4,x1 + xsize - 6,y1 + 19,blue); 

      activefontsettings.color = textcolor;
      showtext2(title, x1 + 5,y1 + 3);
   } 
   else {
      starty = 5;
      if ((windowstyle & dlg_notitle) == 0)
         if (title != NULL)
            if (title[0]  ) {
              activefontsettings.font = schriften.large;
              if ( windowstyle & dlg_3dtitle ) {
                 if ( actmap && actmap->actplayer == 7 ) {
                    activefontsettings.color = xlattables.a.dark1[textcolor];
                    showtext2(title, x1 + 4, y1 + 4 );
                    activefontsettings.color = xlattables.a.light[textcolor];
                    showtext2(title, x1 + 6, y1 + 6 );
                 } else {
                    activefontsettings.color = textcolor + 2;
                    showtext2(title, x1 + 4, y1 + 4 );
                    activefontsettings.color = textcolor - 2;
                    showtext2(title, x1 + 6, y1 + 6 );
                 }
              }

              activefontsettings.color = textcolor;
              showtext2(title, x1 + 5, y1 + 5 );
              starty = 40;
            }
       }
   if (windowstyle & dlg_in3d ) 
      rahmen(true,x1 + 5,y1 + starty,x1 + xsize - 5,y1 + ysize - 5); 

   pb = firstbutton; 
   while (pb != NULL) { 
      if (pb->status) {
         if (pb->active) 
            enablebutton(pb->id); 
         else 
            disablebutton(pb->id); 
      }
      pb = pb->next; 
   } 
}


void         tdialogbox::buildgraphics(void)
{ 

  if ( x1 == -1 ) 
     x1 = ( agmp->resolutionx - xsize ) / 2;
  else
    if ( x1 + xsize < 640 )
       x1 += (agmp->resolutionx - 640) / 2;

  if ( y1 == -1 )
     y1 = ( agmp->resolutiony - ysize ) / 2;
  else
     if ( y1 + ysize < 480 )
         y1 += (agmp->resolutiony - 480) / 2;

  if ( xsize == -1)
     xsize = agmp->resolutionx - xsize*2;
  if ( ysize == -1)
     ysize = agmp->resolutiony - ysize*2;

  if ( pcgo )
     delete pcgo;
  pcgo = new collategraphicoperations ( x1, y1, x1 + xsize, y1 + ysize );

   if (windowstyle & dlg_notitle )
      if (windowstyle & dlg_wintitle ) 
         windowstyle ^= dlg_wintitle;

   ms = getmousestatus(); 
   if (ms == 2)
      mousevisible(false);

   if ( !(dlg_mode & 2) ) {
      tp = asc_malloc ( imagesize (x1,y1,x1 + xsize,y1 + ysize ) );
      getimage(x1,y1,x1 + xsize,y1 + ysize,tp);
      imagesaved = true; 
   }

   if ( dlg_mode & 1 ) {
     paint ();
   } else {
     redraw ();
   }

   rebuildtaborder(); 
   activefontsettings.color = textcolor; 
   boxstatus = 2;

//   if (ms == 2)
//      mousevisible(true);

} 



void         tdialogbox::done(void)
{ 
   pbutton      pb;
   pbutton      pb2;

   if ( boxstatus ) {
      ms = getmousestatus();
      if (ms == 2)
         mousevisible(false);
      pb = firstbutton;
      while (pb != NULL) {
         pb2 = pb->next;
         delete ( pb );
         pb = pb2;
      }

      if (imagesaved) {
         if ( first == this )
            ::repaintDisplay();
         else    
            putimage(x1,y1,tp);
         asc_free ( tp );
      }
      npop( activefontsettings );
      if (ms == 2)
         mousevisible(true);
   }
   boxstatus = 0;
}


void         tdialogbox::execbutton( pbutton      pb, char      mouse )
{
  int      t, l;
  Uint16         *pw, *pw2;


   if (mouse == false) {
      if (pb->art == 0) {
         if ((pb->style == 1) || (pb->style == 2))
            buttonpressed(pb->id);
      }
      if (pb->art == 3)
         toggleswitch(pb);
   }
   else {
      if (pb->art == 0) {
         if (pb->style == 1) {
            newknopfdruck(x1 + pb->x1,y1 + pb->y1,x1 + pb->x2,y1 + pb->y2);
            if (knopfsuccessful)
               buttonpressed(pb->id);
         }
         if (pb->style == 2) {
            mousevisible(false);
            pb->pressed = 1;
            pb->newpressed = 1;
            newknopfdruck4(x1 + pb->x1,y1 + pb->y1,x1 + pb->x2,y1 + pb->y2);
            mousevisible(true);
            t = ticker;
            buttonpressed(pb->id);
            pb->newpressed = 2;
            while ((mouseparams.x >= x1 + pb->x1) && (mouseparams.x <= x1 + pb->x2) && (mouseparams.y >= y1 + pb->y1) && (mouseparams.y <= y1 + pb->y2) && (mouseparams.taste & 1)) {
               if (ticker - t > pb->scrollspeed ) {
                  t = ticker;
                  buttonpressed(pb->id);
               }
               releasetimeslice();
            }
            mousevisible(false); 
            newknopfdruck3(x1 + pb->x1,y1 + pb->y1,x1 + pb->x2,y1 + pb->y2); 
            pb->newpressed = 0;
            pb->pressed = 0;
            mousevisible(true); 
         } 

      } 
      if (pb->art == 3) { 
         toggleswitch(pb); 
         do {
            releasetimeslice();
         }  while (!(mouseparams.taste == 0)); 
      } 

      if (pb->art == 5) {         // Scrollbar
         pw = (Uint16*) pb->data;
         pw2 = (Uint16*) pb->data2; 

         l = pb->y2 - pb->y1 - 2;

         int xb1 = x1 + pb->x1;
         int yb1 = y1 + pb->y1;
         int xb2 = x1 + pb->x2;
         int yb2 = y1 + pb->y2;


         if ( mouseparams.y >= yb1 + 1 + l * *pw / *pw2          &&        mouseparams.y <= yb1 + 1 + l * (*pw + pb->max) / *pw2) {
            char mss = getmousestatus ();
            if (mss == 2)
               mousevisible(false);

            int ys1 = yb1 + 1 + l * *pw / *pw2;
            int ys1a = ys1;
            int ys1b = ys1a;
            int ysd = yb1 + 1 + l * (*pw + pb->max) / *pw2 - ys1;
            rahmen(true, xb1 + 1, ys1,
                         xb2 - 1, ys1 + ysd);
            int mousestarty = mouseparams.y;

            mousevisible( true );

            while (mouseparams.taste & 1) {
                int i = mouseparams.y - mousestarty ;
                ys1 = ys1a + i;
                if ( ys1 <= yb1)
                   ys1 = yb1+1;
                if ( ys1 + ysd >= yb2 )
                   ys1 = yb2 - ysd - 1;

                if ( ys1 != ys1b ) {
                   collategraphicoperations cgo ( x1 + pb->x1, y1 + pb->y1, x1 + pb->x2, y1 + pb->y2 );
                   ys1b = ys1;
                   // waitretrace();
                   mousevisible(false);
                   paintsurface2(x1 + pb->x1 + 1,y1 + pb->y1 + 1,x1 + pb->x2 - 1,y1 + pb->y2 - 1 );
                   rahmen(true, xb1 + 1, ys1, xb2 - 1, ys1 + ysd);
                   cgo.off();
                   int j = (ys1 - yb1 - 1) * (*pw2) / l;
                   if (j != *pw) {
                      *pw = j;
                      buttonpressed(pb->id);
                   }
                   mousevisible( true );
                }
                releasetimeslice();
            }
            mousevisible( false );
            showbutton( pb->id );
            if (mss == 2)
               mousevisible(true);
         } else {

            l = *pw; 
            t = mouseparams.y - (pb->y1 + y1);
            *pw = (10 * t * (*pw2 - pb->max) / (pb->y2 - pb->y1 - 2) + 5) / 10;
            if (*pw != l) {
              l = getmousestatus();
              if (l == 2)
                mousevisible(false);
              showbutton(pb->id);
              buttonpressed(pb->id);
              if (l == 2)
                mousevisible(true);
            }
         }
      }

      if (pb->art == 6) {

            int height = pb->entrynum * 25 + 10;
            int starty =  y1 + pb->y2;
            if ( starty + height > agmp->resolutiony )
               starty = agmp->resolutiony - height;

            mousevisible(false);

            int* pos = (int*) pb->data ;

            activefontsettings.font = schriften.smallarial;
            activefontsettings.background = lightgray;
            activefontsettings.justify = lefttext;
            activefontsettings.length = (Uint16) (pb->x2 - pb->x1 - 20 );

            void* buf = asc_malloc ( imagesize ( x1 + pb->x1, starty, x1 + pb->x2, starty + height ));
            getimage( x1 + pb->x1, starty, x1 + pb->x2, starty + height, buf );

            bar ( x1 + pb->x1, starty, x1 + pb->x2, starty + height, lightgray );
            rectangle ( x1 + pb->x1, starty, x1 + pb->x2, starty + height, black );

            bool first = true;
            int oldpos = *pos;

            mousevisible(true);
            do {
               int p = (mouseparams.y - starty - 5);
               if ( p < 0 )
                  p = -1;
               else
                  p /= 25;

               if ( (p >= 0 && p < pb->entrynum ) || first )
                  if ( p != *pos  || first ) {
                     if ( p != *pos && p >= 0 && p < pb->entrynum )
                        *pos = p;
                     for ( int i = 0; i < pb->entrynum; ++i ) {
                        if ( i == *pos )
                           activefontsettings.color = textcolor;
                        else
                           activefontsettings.color = black ;
                        showtext2 ( pb->entries[i], x1 + pb->x1 + 5, starty + i * 25 + 5 );
                     }
                  }
               releasetimeslice();
               first = false;
            } while ((mouseparams.x >= x1 + pb->x1) && (mouseparams.x <= x1 + pb->x2) && (mouseparams.y >= min(starty, y1+pb->y1)) && (mouseparams.y <= starty + height) && (mouseparams.taste & 1)) ;
            mousevisible(false);

            putimage ( x1 + pb->x1, starty, buf );
            asc_free ( buf );
            mousevisible(true);
            enablebutton( pb->id );
            if ( oldpos != *pos )
               buttonpressed( pb->id );
      }

      if ( pb->art > 10 ) {
         buttonpressed(pb->id);
         while ( mouseinrect ( x1 + pb->x1 ,  y1 + pb->y1 ,  x1 + pb->x2 ,  y1 + pb->y2 ) && ( mouseparams.taste & 1))
            releasetimeslice();
      }

  /* 
     rahmen(true,x1+pb^.x1,y1+pb^.y1,x1+pb^.x2,y1+pb^.y2);
     nbar(x1+pb^.x1+1,y1+pb^.y1+1,x1+pb^.x2-1,y1+pb^.y2-1,dblue);
     rahmen(false,x1 + pb^.x1+1,y1 + pb^.y1+1+ l * pw^ div pw2^ ,
          x1 + pb^.x2-1,y1 + pb^.y1+1+ l * (pw^+ pb^.max) div pw2^);  */ 


   } 
   if ((pb->art == 1) || (pb->art == 2)) { 
      editfield(pb); 
      buttonpressed(pb->id); 
   } 
} 


void         tdialogbox::showtabmark(int         b)
{ 
   if (b != 0)
      xorrectangle(x1 + taborder[b].x1 - 2,y1 + taborder[b].y1 - 2,x1 + taborder[b].x2 + 2,y1 + taborder[b].y2 + 2,15);
} 


void         tdialogbox::run(void)
{
  if ( pcgo ) {
     delete pcgo;
     pcgo = NULL;
  }

  int          xm, ym, xp, yp;
  int          i, oldx, oldy, xp2, yp2;
  pbutton      pb; 

   taste = ct_invvalue; 
   if (getmousestatus() == 2) { 

      if ((mouseparams.x > x1 + 5) && (mouseparams.y > y1 + 3) && (mouseparams.x < x1 + xsize - 5) && (mouseparams.y < y1 + 20) && (mouseparams.taste == 1) && (dlg_mode & 3) == 3 ) {

         oldx = x1; 
         oldy = y1; 
         xp2 = oldx; 
         yp2 = oldy; 

         xm = mouseparams.x; 
         ym = mouseparams.y; 
         xp = xm; 
         yp = ym;

         while (mouseparams.taste == 1) { 
            if ((mouseparams.x != xp) || (mouseparams.y != yp)) { 
               x1 = oldx + (mouseparams.x - xm); 
               y1 = oldy + (mouseparams.y - ym); 
               if (x1 < 0) x1 = 0; 
               if (y1 < 0) y1 = 0; 
               if (x1 + xsize >= agmp->resolutionx) x1 = agmp->resolutionx - xsize - 1; 
               if (y1 + ysize >= agmp->resolutiony) y1 = agmp->resolutiony - ysize - 1; 
               if ((x1 != xp2) || (y1 != yp2)) { 
                  
                  mousevisible(false); 
                  xorrectangle(xp2,yp2,xp2 + xsize,yp2 + ysize,14); 
                  xorrectangle(x1,y1,x1 + xsize,y1 + ysize,14); 
                  mousevisible(true); 

                  xp2 = x1; 
                  yp2 = y1; 

/*                  
                  int ox = xp2;
                  int oy = yp2;


                  paint();

                  if ( prev )
                     prev->redrawall2( ox, oy, ox + xsize, oy + ysize ); 
                  else
                     repaintdisplay();    
*/
               } 
               xp = mouseparams.x; 
               yp = mouseparams.y; 
            }
            releasetimeslice();
         } 

         
         if ((oldx != x1) || (oldy != y1)) { 

            paint();

            if ( prev )
               prev->redrawall2(  oldx, oldy, oldx + xsize, oldy + ysize ); 
            else
               repaintDisplay();

         } 

      }
      if (mouseparams.taste == 1) { 
         pb = firstbutton; 
         while (pb != NULL) { 
            if (pb->status)
               if (pb->active) { 
                  if ((mouseparams.x >= x1 + pb->x1) && (mouseparams.x <= x1 + pb->x2) && (mouseparams.y >= y1 + pb->y1) && (mouseparams.y <= y1 + pb->y2)) {
                     execbutton(pb,true); 
                     break; 
                  } 
               } 
            pb = pb->next; 
         } 
      } 
   } 
   if (keypress()) {
      getkeysyms ( &taste, &prntkey );
   }
   else {
      taste = ct_invvalue;
      prntkey = cto_invvalue;
   }

   releasetimeslice();

   if (((taste == ct_tab) || (taste == ct_shift_tab)) && (tabcount > 0)) { 
      showtabmark(markedtab); 
      if (taste == ct_tab) 
         markedtab++; 
      else 
         markedtab--; 
      if (markedtab > tabcount) 
         markedtab = 1; 
      if (markedtab < 1) 
         markedtab = tabcount; 
      showtabmark(markedtab); 
   } 
   else { 
      pb = firstbutton; 
      while (pb != NULL) { 
         if (pb->markedkeynum > 0) 
            for (i = 0; i < pb->markedkeynum; i++)
              if (markedtab > 0) 
               if ((pb->markedkey[i] == prntkey) && (taborder[markedtab].id == pb->id))
                  if (pb->active) 
                     if (pb->status == 1) { 
                        execbutton(pb,false); 
                        taste = ct_invvalue; 
                        prntkey = ct_invvalue;
                     }
         if (pb->keynum > 0) 
            for (i = 0; i < pb->keynum; i++)
               if (pb->key[i] == prntkey)
                  if (pb->active) 
                     if (pb->status == 1) { 
                        execbutton(pb,false); 
                        taste = ct_invvalue; 
                        prntkey = ct_invvalue;
                     }
         pb = pb->next; 
      } 
   } 
}



void         tdialogbox::toggleswitch(pbutton      pb)
{ 
       
   char *pbl = (char*) pb->data;

   if ( pb->style == 10 ) 
      *pbl = 1;
   else
      *pbl = ! *pbl;


   int col;

   if (*pbl)  
      col = pb->min;
   else 
      col = pb->max;

   line(x1 + pb->x1,y1 + pb->y1,x1 + pb->x1 + (pb->y2 - pb->y1),y1 + pb->y2, col);
   line(x1 + pb->x1 + (pb->y2 - pb->y1),y1 + pb->y1,x1 + pb->x1,y1 + pb->y2, col);

   buttonpressed(pb->id); 

} 


char      tdialogbox::checkvalue(int         id,
                                  void*      p)
{ 
   return  true;
} 


void         tdialogbox::editfield( int id )
{
   pbutton pb = firstbutton; 
   while ( pb ) { 
      if (pb->id == id )
         editfield ( pb );
      pb = pb->next; 
   } 
}


void         tdialogbox::editfield(pbutton      pb)
{ 
  char         *ps;
  int      *pl; 
  Uint16         *pw; 
  char         *pbt;
  int      l;

  activefontsettings.font = schriften.smallarial;
  activefontsettings.color = textcolor;
  activefontsettings.background = dblue;
  activefontsettings.length = (Uint16) (pb->x2 - pb->x1 - 10 );

   if (pb->art == 1) {
      ps = (char*) pb->data;
      // mousevisible(false); 
      do { 
        stredit(ps, x1 + pb->x1 + 5,y1 + pb->y1 + 2,pb->x2 - pb->x1 - 10, pb->max);
      }  while ( !checkvalue(pb->id,ps) );
      // mousevisible(true); 
   } 
   if (pb->art == 2) { 
      if (pb->max <= 255 && pb->min >= 0) { 
         pbt = (char*) pb->data;
         l = *pbt; 
      } 
      else 
         if (pb->max <= 65535 && pb->min >= 0) { 
            pw = (Uint16*)  pb->data;
            l = *pw; 
         } 
         else { 
            pl = (int*) pb->data;
            l = *pl; 
         } 
      // mousevisible(false); 
      do { 
         intedit( &l,x1 + pb->x1 + 5,y1 + pb->y1 + 2,pb->x2 - pb->x1 - 10,pb->min,pb->max);
      }  while ( !checkvalue(pb->id, &l) );
      // mousevisible(true); 
      if (pb->max <= 255 && pb->min >= 0) { 
         *pbt = l; 
      } 
      else 
         if (pb->max <= 65535 && pb->min >= 0) { 
            *pw = l; 
         } 
         else { 
            *pl = l; 
         } 
   } 
} 




void         tdialogbox::rahmen3(char *       txt,
                     int      x1,
                     int      y1,
                     int      x2,
                     int      y2,
                     int         style)
{ 
   collategraphicoperations cgs( x1, y1, x2, y2 );

  Uint16         w;

   npush( activefontsettings );
   activefontsettings.font = schriften.smallarial;
   w = gettextwdth(txt,NULL); 
   if (style == 1) { 
      line(x1,y1,x1,y2,black); 
      line(x1,y2,x2,y2,black); 
      line(x2,y1,x2,y2,black); 
      line(x1,y1,x1 + 9,y1,black); 
      line(x1 + 10 + w + 5,y1,x2,y1,black); 
   } 
   activefontsettings.justify = lefttext; 
   activefontsettings.length = 0;
   showtext2(txt,x1 + 12,y1 - activefontsettings.font->height / 2); 

   npop( activefontsettings );
} 


typedef char* tstringa[30];
typedef tstringa* pstringa;


class tdisplaymessage : public tdialogbox {
                        char status;
                        int  mode;
                    public:
                        void init ( tstringa a, int md, int linenum, char* buttonText = NULL );
                        virtual void buttonpressed ( int id );
                        virtual void run ( void );
                   };

extern tdisplaymessage* messagebox;


void tdisplaymessage::init ( tstringa a, int md, int linenum, char* buttonText )
{
   if ( !buttonText )
      buttonText = "~O~K";

   tdialogbox::init();

   int i,j;
   int maxlength = 0;

   mode = md;

   for (i=0;i<=linenum ;i++ ) {
      j = gettextwdth ( a[i], schriften.smallarial );
      while ( j > agmp->resolutionx ) {
         a[i][strlen(a[i])-1] = 0;
         j = gettextwdth ( a[i], schriften.smallarial );
      }
      if (maxlength < j)
         maxlength = j;
   } /* endfor */

   x1 = 50;
   y1 = 50;
   if (maxlength < 150)
     xsize = 200;
   else
     xsize = maxlength + 50;

   if ( maxlength > agmp->resolutionx - 100 )
      x1 = 0;

   if ( xsize > agmp->resolutionx  )
      xsize = agmp->resolutionx;

   ysize = 55 + linenum * 20;
   windowstyle |= dlg_notitle;
   if (mode != 0) {
      ysize+=25;
      addbutton ( buttonText, 10, ysize - 35 , xsize - 10 , ysize -  10 ,0,1,1,true );
      addkey(1, ct_enter);
      addkey(1, ct_esc);
      addkey(1, ct_enterk );
      addkey(1, ct_space );
   };

   buildgraphics();
   activefontsettings.justify = lefttext;
   if (mode == 1) {
      activefontsettings.color = lightred;
   } else {
      activefontsettings.color = black;
   } /* endif */

   activefontsettings.font = schriften.smallarial;
   for (i=0;i <= linenum ;i++ ) {
      showtext2( a[i], x1+10, y1+20+i*20 );
   } /* endfor */
   status = 0;
   if ( pcgo ) {
      delete pcgo;
      pcgo = NULL;
   }
}

void tdisplaymessage::buttonpressed ( int id )
{
   if ( id == 1) {
      status = 1;
   } /* endif */
}

void tdisplaymessage::run ( void )
{
   mousevisible(true);
   do {
      tdialogbox::run();
   } while ( status == 0 ); /* enddo */
}


// num   0: Box bleibt aufgeklappt,
//       1 box wird geschlossen , text rot (Fehler),
//       2 : Programm wird beendet;
//       3 : normaler text ( OK)

void displaymessage( const char* formatstring, int num, ... )
{

   va_list paramlist;
   va_start ( paramlist, num );

   char tempbuf[1000];

   int lng = vsprintf( tempbuf, formatstring, paramlist );
   if ( lng >= 1000 )
      displaymessage ( "dlg_box.cpp / displaymessage:   string to long !\nPlease report this error",1 );

   va_end ( paramlist );

   displaymessage ( ASCString ( tempbuf ), num  );
}

void displaymessage( const ASCString& text, int num  )
{
   const char* a = text.c_str();

   tstringa stringtooutput;
   memset (stringtooutput, 0, sizeof ( stringtooutput ));
   stringtooutput[0] =  new char[200];
   stringtooutput[0][0] = 0;
   char* b = stringtooutput[0];

   int linenum = 0;

   while ( *a ) {
      if (*a == '\n') {
         *b = 0;
         linenum++;
         stringtooutput[linenum] = new char[200];
         b = stringtooutput[linenum];
         *b = 0;
      } else {
        *b = *a;
        b++;
      }
      a++;
   }

   *b = 0;


   bool displayInternally = true;

   if ( num == 2 )
      displayLogMessage ( 0, "fatal error" + text + "\n" );
   else
      displayLogMessage ( 0, text + "\n" );


   #ifndef NoStdio
   if ( num == 2 )
      displayInternally = false;
   #endif


   if ( num == 2 )
      displayLogMessage(1, text );



   if ( !displayInternally ) {
      for ( int i=0; i<= linenum ;i++ )
          fprintf(stderr,"%s\n",stringtooutput[i]);
      fflush( stderr );
   } else {
      #ifdef _WIN322_
        if ( !gameStartupComplete && num==2 ) {
           MessageBox(NULL, text.c_str(), "Fatal Error", MB_ICONERROR | MB_OK | MB_TASKMODAL );
           exit(1);
        }
      #endif

      if ( legacyEventSystemActive() ) {
         static int messageboxopen = 0;
         if ( messageboxopen )
            return;
   
         messageboxopen++;
         if ( messagebox ) {
            if ( messagebox->boxstatus )
               messagebox->done();
            delete messagebox;
            messagebox = NULL;
         }
   
         messagebox = new tdisplaymessage;
   
         if ( num== 2 )
            messagebox->init( stringtooutput, num, linenum, "~q~uit program");
         else
            messagebox->init( stringtooutput, num, linenum);
   
         if (num != 0 ) {
            messagebox->run();
            messagebox->done();
            delete messagebox;
            messagebox = NULL;
         }
   
         messageboxopen--;
      } else {
         MessagingHub::Instance().warning( text );
      }
   } /* endif */

   if ( num == 2 ) {
      #ifdef _WIN32_
//        if ( !gameStartupComplete ) {
           MessageBox(NULL, text.c_str(), "Fatal Error", MB_ICONERROR | MB_OK | MB_TASKMODAL );
           exit(1);
  //      }
      #endif
      exit ( 1 );
   }

   for ( int i=linenum; i>=0 ;i-- )
      delete[]  stringtooutput[i];
}


void removemessage( void )
{
   if ( messagebox ) {
     if ( messagebox->boxstatus == 2) 
        messagebox->done();
     delete messagebox;
     messagebox = NULL;

   }
}



void tdialogbox::dispeditstring ( char* st, int x1, int y1 )
{
   showtext2(st,x1,y1);
}


void         tdialogbox::stredit(char *       s,
                      int          x1,
                      int          y1,
                      int          wdth,
                      int          max)
{ 

   char         *ss, *ss2, *ss3;
   char      einfuegen; 
   int         position; 
   int          i;

   if ( strlen ( s ) > max )
      max = strlen ( s );

   activefontsettings.justify = lefttext;
   ss = new char[max+5];
   ss2 = new char[max+5];
   strcpy(ss,s); 

   activefontsettings.length = wdth;
   activefontsettings.height = activefontsettings.font->height;
   {
     collategraphicoperations cgo ( x1, y1, x1 + wdth, y1 + activefontsettings.height );
     dispeditstring ( ss, x1, y1 );
     position = strlen(s);
     einfuegen = true;
     lne(x1,y1,ss,position,einfuegen);
   }

   int  cc;
   tkey symkey;
   do {
     if ( keypress() ) {
       getkeysyms ( &symkey, &cc );
       // cc = rp_key();
     } else {
        cc = cto_invvalue;
        symkey = ct_invvalue;
        releasetimeslice();
     }

     if (cc != cto_invvalue ) {
       lne(x1,y1,ss,position,einfuegen);

#ifdef _DOS_          // I hate this DOS keyboard stuff !!!!!!
       switch (symkey) {
            case cto_einf: 
                 {
                    if (einfuegen == false) 
                       einfuegen = true; 
                    else 
                       einfuegen = false; 
                 } 
            break; 
            
            case cto_left:   if (position >= 1)
                           position--;
            break; 
            
            case cto_right:  if (position < strlen ( ss ) )
                         position++;
            break; 
            
            case cto_pos1:  position = 0;
            break; 
            
            case cto_ende:  position = strlen ( ss ) ;
            break; 
            
            case cto_entf:  if ( ss[ position ] != 0 ) {
                         for (i=0; i< position ;i++ ) {
                            ss2[i] = ss[i];
                         } /* endfor */
                         while ( ss[i] != 0 ) {
                            ss2[i] = ss [ i + 1 ];
                            i++;
                         } /* endwhile */
                         ss3 = ss2;
                         ss2 = ss;
                         ss = ss3;

                         dispeditstring (ss,x1,y1); /* ? */
                       }
            break; 
            
            case cto_right + cto_stp:  if ( position < strlen ( ss ) ) {
                        do { 
                           position++;
                        }  while ( (ss[ position ] != ' ') && ( ss[ position ] != 0 ) );
                     } 
            break; 
            
            case cto_left + cto_stp:  if ( position > 0 ) {
                        do { 
                           position--;
                        }  while ( ( position > 0 ) && ( ss [ position - 1 ] != ' ') );
                     } 
            break;
         } 
#else
       switch (symkey) {
            case ct_einf: 
                 {
                    if (einfuegen == false) 
                       einfuegen = true; 
                    else 
                       einfuegen = false; 
                 } 
            break; 
            
            case ct_left:   if (position >= 1)
                           position--;
            break; 
            
            case ct_right:  if (position < strlen ( ss ) )
                         position++;
            break; 
            
            case ct_pos1:  position = 0;
            break; 
            
            case ct_ende:  position = strlen ( ss ) ;
            break; 
            
            case ct_entf:  if ( ss[ position ] != 0 ) {
                         for (i=0; i< position ;i++ ) {
                            ss2[i] = ss[i];
                         } /* endfor */
                         while ( ss[i] != 0 ) {
                            ss2[i] = ss [ i + 1 ];
                            i++;
                         } /* endwhile */
                         ss3 = ss2;
                         ss2 = ss;
                         ss = ss3;

                         dispeditstring (ss,x1,y1); /* ? */
                       }
            break; 
            
            case ct_right + ct_stp:  if ( position < strlen ( ss ) ) {
                        do { 
                           position++;
                        }  while ( (ss[ position ] != ' ') && ( ss[ position ] != 0 ) );
                     } 
            break; 
            
            case ct_left + ct_stp:  if ( position > 0 ) {
                        do { 
                           position--;
                        }  while ( ( position > 0 ) && ( ss [ position - 1 ] != ' ') );
                     } 
            break;
         } 
#endif

         lne(x1,y1,ss,position,einfuegen);

         if ( ( cc > 31 ) && ( cc < 256 ) && (strlen(ss) < max ) ) {       // plain ascii
            i=0;
            while ( (ss[i] != 0) && ( i < position ) ) {
               ss2[i] = ss[i];
               i++;
            }
            ss2 [ i ] = cc;
            if (einfuegen) {
              while ( ss[i] != 0) {
                 ss2[i+1] = ss[i];
                 i++;
              } /* endwhile */
              ss2[i+1] = 0;
            } else {
               if (ss[i] != 0) {
                 i++;
                 while ( ss[i] != 0) {
                   ss2[i+1] = ss[i];
                   i++;
                 } /* endwhile */
                 ss2[i] = 0;
               } else {
                  ss2[i+1] = 0;
               }
            } /* endif */
            
            if (gettextwdth_stredit( ss2, NULL ) < wdth ) {
              collategraphicoperations cgo ( x1, y1, x1 + wdth, y1 + activefontsettings.height );
              lne(x1,y1,ss,position,einfuegen);
              ss3 = ss2;
              ss2 = ss;
              ss = ss3;
              dispeditstring (ss,x1,y1);
              position++;
              lne(x1,y1,ss,position,einfuegen);
            } /* endif */
         } 
         if ((cc == cto_bspace ) && (position > 0)) {   /* Backspace */
            collategraphicoperations cgo ( x1, y1, x1 + wdth, y1 + activefontsettings.height );
            lne(x1,y1,ss,position,einfuegen);
            for (i=0; i+1< position ; i++ ) {
               ss2[i] = ss[i];
            } /* endfor */
            i--;
            do {
               i++;
               ss2[i] = ss[i+1];
            } while (ss[i+1] != 0 );
            ss3 = ss2;
            ss2 = ss;
            ss = ss3;
            position--;
            dispeditstring (ss,x1,y1); 
            lne(x1,y1,ss,position,einfuegen); 
         } 
      } 
   }  while ( cc != cto_enter && cc != cto_esc && !( !mouseinrect ( x1, y1, x1 + wdth, y1 + activefontsettings.height) && mouseparams.taste > 0 ) );

   lne(x1,y1,ss,position,einfuegen); 
   if (cc != cto_esc )
      strcpy(s,ss);
   delete[] ss;
   delete[] ss2;
} 


int         tdialogbox::gettextwdth_stredit ( char* txt, pfont font )
{
   return gettextwdth ( txt, font );
}


void         tdialogbox::lne(int          x1,
                 int          y1,
                 char *       s,
                 int          position,
                 char      einfuegen)
{
 int          i, j, k;
 char* ss2;

  ss2 = strdup ( s );
  ss2 [ position ] = 0;
   i = x1 + gettextwdth(ss2,activefontsettings.font);
   j = y1; 
   k = y1 + activefontsettings.font->height; 
   collategraphicoperations cgo ( i-1, j, i+1, k );
   xorline(i,j,i,k,3);
   if (einfuegen == false) { 
      xorline(i + 1,j,i + 1,k,3); 
      xorline(i - 1,j,i - 1,k,3); 
   }  
   asc_free ( ss2 );
} 




void         tdialogbox::intedit(int *    st,
                     int          x1,
                     int          y1,
                     int          wdth,
                     int          min,
                     int          max)
{ 
   char         *ss, *ss2, *ss3;
   char      einfuegen; 
   int          position;
   int          j;
   int          i;
   int          ml;
   char ok;

   ml =  12 ;
   activefontsettings.justify = lefttext; 

   ss2 = new char[ml];
   ss =  new char[ml];
   itoa ( *st, ss, 10 );
   activefontsettings.height = activefontsettings.font->height; 
   dispeditstring (ss,x1,y1); 
   position = strlen(ss) ; 
   einfuegen = true; 

   ok = false;

   int  cc;
   tkey symkey;
   do {
      lne(x1,y1,ss,position,einfuegen); 
      do {
         if ( keypress() ) {
            getkeysyms ( &symkey, &cc );
         } else {
            cc = cto_invvalue;
            releasetimeslice();
         }

         if (cc != cto_invvalue ) {
            collategraphicoperations cgo ( x1, y1, x1 + wdth, y1 + activefontsettings.height );

            lne(x1,y1,ss,position,einfuegen);
#ifdef _DOS_
            switch (symkey) {
        
                case cto_einf: {
                        if (einfuegen == false)
                           einfuegen = true;
                        else
                           einfuegen = false;
                     }
                break;

                case cto_left:   if (position >= 1)
                               position--;
                break;

                case cto_right:  if (position < strlen ( ss ) )
                             position++;
                break;

                case cto_pos1:  position = 0;
                break;

                case cto_ende:  position = strlen ( ss ) ;
                break;

                case cto_entf:  if ( ss[ position ] != 0 ) {
                             for (i=0; i< position ;i++ ) {
                                ss2[i] = ss[i];
                             } /* endfor */
                             while ( ss[i] != 0 ) {
                                ss2[i] = ss [ i + 1 ];
                                i++;
                             } /* endwhile */
                             ss3 = ss2;
                             ss2 = ss;
                             ss = ss3;

                             dispeditstring (ss,x1,y1); /* ? */
                           }
                break;

                case cto_right + cto_stp:  if ( position < strlen ( ss ) ) {
                            do {
                               position++;
                            }  while ( (ss[ position ] != ' ') && ( ss[ position ] != 0 ) );
                         }
                break;

                case cto_left + cto_stp:  if ( position > 0 ) {
                            do {
                               position--;
                            }  while ( ( position > 0 ) && ( ss [ position - 1 ] != ' ') );
                         }
                break;
             }
#else
            switch (symkey) {
        
                case ct_einf: {
                        if (einfuegen == false)
                           einfuegen = true;
                        else
                           einfuegen = false;
                     }
                break;

                case ct_left:   if (position >= 1)
                               position--;
                break;

                case ct_right:  if (position < strlen ( ss ) )
                             position++;
                break;

                case ct_pos1:  position = 0;
                break;

                case ct_ende:  position = strlen ( ss ) ;
                break;

                case ct_entf:  if ( ss[ position ] != 0 ) {
                             for (i=0; i< position ;i++ ) {
                                ss2[i] = ss[i];
                             } /* endfor */
                             while ( ss[i] != 0 ) {
                                ss2[i] = ss [ i + 1 ];
                                i++;
                             } /* endwhile */
                             ss3 = ss2;
                             ss2 = ss;
                             ss = ss3;

                             dispeditstring (ss,x1,y1); /* ? */
                           }
                break;

                case ct_right + ct_stp:  if ( position < strlen ( ss ) ) {
                            do {
                               position++;
                            }  while ( (ss[ position ] != ' ') && ( ss[ position ] != 0 ) );
                         }
                break;

                case ct_left + ct_stp:  if ( position > 0 ) {
                            do {
                               position--;
                            }  while ( ( position > 0 ) && ( ss [ position - 1 ] != ' ') );
                         }
                break;
             }
#endif
             if ( (( cc >=  '0' &&  cc <= '9' ) || ( cc == '-' && !position)) && (strlen(ss) < ml-1 ) ) {
                i=0;
                while ( (ss[i] != 0) && ( i < position ) ) {
                   ss2[i] = ss[i];
                   i++;
                }
                ss2 [ i ] = cc;
                if (einfuegen) {
                  while ( ss[i] != 0) {
                     ss2[i+1] = ss[i];
                     i++;
                  } /* endwhile */
                  ss2[i+1] = 0;
                } else {
                   if (ss[i] != 0) {
                     i++;
                     while ( ss[i] != 0) {
                       ss2[i+1] = ss[i];
                       i++;
                     } /* endwhile */
                   } /* endif */
                   ss2[i] = 0;
                } /* endif */

                if (gettextwdth( ss2, NULL ) < wdth ) {
                  ss3 = ss2;
                  ss2 = ss;
                  ss = ss3;
                  dispeditstring (ss,x1,y1);
                  position++;
                } /* endif */
             }
             if ((cc == 8 ) && (position > 0)) {   /* Backspace */

                for (i=0; i+1< position ; i++ )
                   ss2[i] = ss[i];

                i--;
                do {
                   i++;
                   ss2[i] = ss[i+1];
                } while (ss[i+1] != 0 );
                ss3 = ss2;
                ss2 = ss;
                ss = ss3;
                position--;

                dispeditstring (ss,x1,y1);
             }
             lne(x1,y1,ss,position,einfuegen);
          }

      }  while ( (cc != cto_enter) && (cc != cto_esc) && !( !mouseinrect ( x1, y1, x1 + wdth, y1 + activefontsettings.height) && mouseparams.taste > 0 ) );
      lne(x1,y1,ss,position,einfuegen);
      if (cc != cto_esc ) {
        j = strtol ( ss, &ss3, 10 );
        if ((ss3 != NULL) && ( (ss3 - ss ) < strlen ( ss ) )) {

           position = (ss3 - ss );
           lne(x1,y1,ss,position,einfuegen);
           ok = false;

        } else {

           if ( j > max || j < min ) {
              ok = false;
              displaymessage( "Invalid range ! \n range is %d to %d !", 1, min, max  );
           } else {
             *st = j;
             ok = true;
           }

        } /* endif */

      } else
         ok = true;

   } while ( ! ok );

   delete[] ss;
   delete[] ss2;

} 


void         tdialogbox::paintsurface2 ( int xx1, int yy1, int xx2, int yy2 )
{
     if ( dialogtexture ) 
       puttexture( xx1, yy1, xx2, yy2, dialogtexture);
     else
       bar( xx1, yy1, xx2, yy2, lightgray);

}

void         tdialogbox::paintsurface ( int xx1, int yy1, int xx2, int yy2 )
{
   paintsurface2( xx1 + x1, yy1 + y1, xx2 + x1, yy2 + y1 );
}






tdialogbox::~tdialogbox()
{
   if ( pcgo ) {
      delete pcgo;
      pcgo = NULL;
   }

   if ( boxstatus )
      done();
   boxstatus = 0;

   if ( prev ) {
      prev->next = NULL;

      if ( dlg_mode & 2 )
         prev->redrawall2( x1, y1, x1 + xsize, y1 + ysize );
   } else {
      first = NULL;
      repaintDisplay();
   }
   npop ( activefontsettings );

   setEventRouting ( eventQueue, !eventQueue );
}












void tviewtext::setnewlineheight ( int h )
{
      activefontsettings.height = h;

      if (tvt_maxlineheight > activefontsettings.font->height + 5) 
         activefontsettings.height = tvt_maxlineheight;
      else {
         if ( tvt_dispactive ) 
            if ( tvt_yp + activefontsettings.height > tvt_starty ) {
               int tvtny1 = tvt_y1 + tvt_yp - tvt_starty + tvt_maxlineheight;
               int tvtny2 = tvt_y1 + tvt_yp - tvt_starty + activefontsettings.height;
               int tvtnx1 = tvt_x1;
               int tvtnx2 = tvt_x1 + tvt_xp;
               if (tvtny1 < tvt_y1) {
                  tvtny1 =  tvt_y1;
                  tvtnx1 = tvt_x1;
               }
               if (tvtny2 > tvt_y2)
                  tvtny2 =  tvt_y2;
               if (tvtnx2 > tvtnx1)
                 if (tvtny2 > tvtny1)
                 bar ( tvtnx1, tvtny1, tvtnx2, tvtny2, tvt_background);
            };
         tvt_maxlineheight = activefontsettings.height ;
      }
}

void tviewtext::fillline ( int x1, int x2 )
{
  if ( tvt_dispactive ) 
      if ( tvt_yp + activefontsettings.height > tvt_starty ) {
         int tvtny1 = tvt_y1 + tvt_yp - tvt_starty;
         int tvtny2 = tvt_y1 + tvt_yp - tvt_starty + tvt_maxlineheight;
         if (tvtny1 < tvt_y1) {
            tvtny1 =  tvt_y1;
            // x1 = tvt_x1;
         } 
         if (tvtny2 > tvt_y2)
            tvtny2 =  tvt_y2;
         if (x2 > x1)
           if (tvtny2 > tvtny1)
              bar ( x1, tvtny1, x2, tvtny2, tvt_background);
      }
}

void tviewtext::nextline ( int einzug, const char* txtptr )
{
   if ( tvt_dispactive == 0 && txtptr ) {
      if (  ( tvt_startpoint == NULL  &&  (tvt_yp > 500) ) || ( tvt_startpoint && ( tvt_yp > (tvt_startpoint->ypos+500) ))) {
         pstartpoint newstartpoint = new ( tstartpoint );
         newstartpoint->ypos = tvt_yp;
         newstartpoint->xpos = tvt_xp;
         newstartpoint->textcolor = tvt_color;
         newstartpoint->background = tvt_background;
         newstartpoint->font = activefontsettings.font;
         newstartpoint->eeinzug = eeinzug;
         newstartpoint->aeinzug = aeinzug;
         newstartpoint->height = activefontsettings.height;
         newstartpoint->next = tvt_startpoint;
         newstartpoint->textpointer = txtptr ;
         newstartpoint->maxlineheight = tvt_maxlineheight ;
         tvt_startpoint = newstartpoint;
      }

   }

   fillline ( tvt_x1 + tvt_xp, tvt_x2 );
   tvt_yp += activefontsettings.height;
   tvt_xp = einzug;
   fillline ( tvt_x1, tvt_x1 + tvt_xp );
   *actline = 0;
   activefontsettings.height = tvt_maxlineheight = activefontsettings.font->height + 5;

}

void tviewtext::displaysingleline ( char* t)
{
   int twdth = gettextwdth ( t , NULL );
   if ( tvt_dispactive )
      if ( tvt_yp  >= tvt_starty ) {
         if ( tvt_yp <  tvt_starty + tvt_y2 - tvt_y1) {
            if ( tvt_yp + activefontsettings.height  + 1 >  tvt_starty + tvt_y2 - tvt_y1)
               activefontsettings.height = tvt_y2 - tvt_yp + tvt_starty - tvt_y1 + 1;
            if (tvt_x1 + tvt_xp + twdth > tvt_x2) 
               twdth = activefontsettings.length = tvt_x2 - tvt_x1 - tvt_xp;
            else
               activefontsettings.length = 0;
            showtext2( t,  tvt_x1 +  tvt_xp, tvt_y1 +  tvt_yp - tvt_starty );
         } 
      } else 
         if ( t[0] ) 
           if ( tvt_yp + activefontsettings.height  > tvt_starty ) {
              npush ( *agmp );
              *agmp = tvt_firstlinebufparm;
              if ( activefontsettings.height > tvt_firstlinebufheight )
                 activefontsettings.height = tvt_firstlinebufheight;
              if (tvt_x1 + tvt_xp + twdth > tvt_x2) 
                 twdth = activefontsettings.length = tvt_x2 - tvt_x1 - tvt_xp;
              else
                 activefontsettings.length = 0;
              showtext2( t, tvt_xp, 0 );
              npop ( *agmp );
              putimageprt ( tvt_x1 + tvt_xp, tvt_y1, tvt_x1 + tvt_xp + twdth, tvt_y1 + activefontsettings.height + ( tvt_yp - tvt_starty ), tvt_firstlinebuf, tvt_xp, tvt_starty - tvt_yp );
          }

   tvt_xp += twdth ;
}


void tviewtext::setpos ( int xx1, int yy1, int xx2, int yy2 )
{
   tvt_x1 = xx1;
   tvt_y1 = yy1;
   tvt_x2 = xx2;
   tvt_y2 = yy2;
}

void tviewtext::setparams ( int xx1, int yy1, int xx2, int yy2, const char* ttxt, char clr, char bkgr)
{
   tvt_x1 = xx1;
   tvt_y1 = yy1;
   tvt_x2 = xx2;
   tvt_y2 = yy2;
   tvt_text = ttxt;
   tvt_color = clr;
   defaulttextcolor = clr;
   tvt_background = bkgr;
   tvt_maxlineheight = activefontsettings.font->height + 5;
   eeinzug = 0;
   aeinzug = 0;
   tvt_dispactive = 0;
   tvt_starty = 0;
   tvt_firstlinebufheight = 35;

   tvt_firstlinebufparm.resolutionx = xx2 - xx1 + 1;
   tvt_firstlinebufparm.resolutiony = tvt_firstlinebufheight;
   tvt_firstlinebufparm.windowstatus = 100;
   tvt_firstlinebufparm.scanlinelength = xx2 - xx1 + 1;
   tvt_firstlinebufparm.scanlinenumber = tvt_firstlinebufheight;
   tvt_firstlinebufparm.bytesperscanline = xx2 - xx1 + 1;
   tvt_firstlinebufparm.byteperpix = 1;

   tvt_startpoint = NULL;
}

void tviewtext::displaytext ( void )
{
  auto_ptr<collategraphicoperations> cgo;
  if ( tvt_dispactive )
    cgo.reset( new collategraphicoperations ( tvt_x1, tvt_y1, tvt_x2, tvt_y2 ) );

   tvt_color = defaulttextcolor;
   tvt_maxlineheight = activefontsettings.font->height + 5;
   eeinzug = 0;
   aeinzug = 0;


  char         *actword, *s5;
  const char* s1;
  int      i;

   tvt_xp = 0;
   tvt_yp = 0;

   npush ( activefontsettings );

   actline  = new char[200];
   actword  = new char[200];
   s5       = new char[200];
   
   {
      tvt_firstlinebuf = new char [ ( tvt_x2 - tvt_x1) * ( tvt_firstlinebufheight + 5 ) ];
      memset ( tvt_firstlinebuf , tvt_background, (tvt_x2 - tvt_x1) * ( tvt_firstlinebufheight +  5 ) );
   
      Uint16* pw = (Uint16*) tvt_firstlinebuf;
      *pw = (tvt_x2 - tvt_x1);
      pw++;
      *pw = tvt_firstlinebufheight;
      pw++;
      tvt_firstlinebufparm.linearaddress = (PointerSizedInt) pw;
   }


   s1 = tvt_text;
   activefontsettings.background = tvt_background;
   activefontsettings.color = tvt_color;
   activefontsettings.font = schriften.smallarial;
   activefontsettings.justify = lefttext;
   activefontsettings.height = activefontsettings.font->height + 5;


   if (tvt_dispactive) {

      if ( tvt_startpoint ) {
         pstartpoint startpoint = tvt_startpoint;
         while ( startpoint && ( startpoint->ypos+100 > tvt_starty ) )
            startpoint = startpoint->next;

         if ( startpoint ) {
            tvt_color = startpoint->textcolor;
            tvt_background = startpoint->background ;
            activefontsettings.font = startpoint->font ;
            eeinzug = startpoint->eeinzug ;
            aeinzug = startpoint->aeinzug ;
            tvt_xp = startpoint->xpos;
            tvt_yp = startpoint->ypos;
            tvt_maxlineheight = startpoint->maxlineheight;
            activefontsettings.height = startpoint->height ;
            s1 = startpoint->textpointer ;
         }
      }

   }

   if ( tvt_text ) {
       *actline = 0;
       *actword = 0;
       do { 
          i = 0;
          while (*s1 != '#'   &&   *s1 != 0   &&    *s1 != '\n'   &&   *s1 != ' '    &&   *s1 != '-') {
             actword[i] = *s1;
             i++;
             s1++;
          } /* endwhile */
          if ( *s1 == ' '    ||   *s1 == '-' ) {
             actword[i] = *s1;
             i++;
             s1++;
          }
   
          actword[i] = 0;
   
          strcpy ( s5, actline );
          strcat ( s5, actword );
   
          if ( tvt_xp + gettextwdth ( s5, NULL ) < tvt_x2 - tvt_x1 )
             strcat ( actline, actword );
          else {
             displaysingleline ( actline );
             nextline ( aeinzug, s1 );
             activefontsettings.length = 0;
             strcpy ( actline, actword );
          }

          if (*s1 == '\n') {
             displaysingleline ( actline );
             nextline ( eeinzug, s1 );
             s1++;
          }
          if (*s1 == 0) {
             displaysingleline ( actline );
             nextline ( 0, s1 );
          }
   
          if (*s1 == '#') {
             displaysingleline ( actline );
             *actline = 0;
             evalcommand ( &s1 );
          }
       } while ((*s1 != 0) && ((tvt_yp - tvt_starty < tvt_y2 - tvt_y1) || !tvt_dispactive ));
       if ( tvt_dispactive )
          if (tvt_yp - tvt_starty < tvt_y2 - tvt_y1)
             bar ( tvt_x1, tvt_yp + tvt_y1 - tvt_starty, tvt_x2, tvt_y2, tvt_background);

    }

   delete[] s5;
   delete[] actword;
   delete[] actline;

   delete[] tvt_firstlinebuf;

   npop ( activefontsettings );

}



void tviewtext::evalcommand ( const char** s)
{
   const char* s3 = *s;
   char s4[100];
   int k, i = 0,
          j = 0;
   int tvt_oldx;


   memset( s4, 0, sizeof( s4 ));
   if (strnicmp(s3, "#COLOR", 6) == 0) {
      i+=6;
      while (s3[i] != '#') {
         s4[j] = s3[i];
         j++;
         i++;
      }
      s4[j] = 0;
      k = atoi( s4 );
      if (k == 256)
         activefontsettings.color = tvt_color;
      else
         activefontsettings.color = k;
   } 

   if (strnicmp(s3, "#BACKGROUND", 11) == 0) {
      i+=11;
      while (s3[i] != '#') {
         s4[j] = s3[i];
         j++;
         i++;
      }
      s4[j] = 0;
      k = atoi( s4 );
      if (k == 256)
         activefontsettings.background = tvt_background;
      else
         activefontsettings.background = k;
   } 

   if (strnicmp(s3, "#FONT", 5) == 0) {
      i+=5;
      while (s3[i] != '#') {
         s4[j] = s3[i];
         j++;
         i++;
      }
      s4[j] = 0;

      k = atoi ( s4 );
      switch (k) {
      case 1: activefontsettings.font = schriften.smallarial; 
         break;
      case 2: activefontsettings.font = schriften.arial8; 
         break;
      default: activefontsettings.font = schriften.smallarial; 
         break;
      } /* endswitch */
      setnewlineheight ( activefontsettings.font->height + 5 );
   } 

   if (strnicmp(s3, "#TAB", 4) == 0) {
      tvt_oldx = tvt_xp;
      i+=4;
      while (s3[i] != '#') {
         s4[j] = s3[i];
         j++;
         i++;
      }
      s4[j] = 0;

      k = atoi ( s4 );

      int tvtxp;
      if (k)
        tvtxp = (tvt_xp / k + 1) * k;

      fillline ( tvt_x1 + tvt_xp, tvt_x1 + tvtxp );
      tvt_xp = tvtxp;

   } 
   if (strnicmp(s3, "#POS", 4) == 0) {
      tvt_oldx = tvt_xp;
      i+=4;
      while (s3[i] != '#') {
         s4[j] = s3[i];
         j++;
         i++;
      }
      s4[j] = 0;

      k = atoi ( s4 );
      fillline ( tvt_x1 + tvt_xp, tvt_x1 + k );
      tvt_xp = k;
   } 

   if (strnicmp(s3, "#EEINZUG", 8) == 0) {
      tvt_oldx = tvt_xp;
      i+=8;
      while (s3[i] != '#') {
         s4[j] = s3[i];
         j++;
         i++;
      }
      s4[j] = 0;

      eeinzug = atoi ( s4 );
   } 
   if (strnicmp(s3, "#AEINZUG", 8) == 0) {
      tvt_oldx = tvt_xp;
      i+=8;
      while (s3[i] != '#') {
         s4[j] = s3[i];
         j++;
         i++;
      }
      s4[j] = 0;

      aeinzug = atoi ( s4 );
   } 

   if (strnicmp(s3, "#CRT#", 5) == 0) {
      nextline ( eeinzug, NULL );
      i=4;
   } 
   if (strnicmp(s3, "#CRTP", 5) == 0) {
      i+=5;
      while (s3[i] != '#') {
         s4[j] = s3[i];
         j++;
         i++;
      }
      s4[j] = 0;
      if ( s3[i+1] == '\n' )
         i++;
      k = atoi ( s4 );
      setnewlineheight ( activefontsettings.height + k );
      nextline ( eeinzug, NULL );
   } 


  (*s)+=i+1;
}


tviewtext::tviewtext()
{
   tvt_startpoint = NULL;
}


tviewtext::~tviewtext()
{
   while ( tvt_startpoint ) {
         pstartpoint startpoint = tvt_startpoint;
         tvt_startpoint = tvt_startpoint->next;
         delete startpoint;
    }
}



void tviewtextwithscrolling::checkscrolling ( void )
{
         int pagepressed_scrollspeedmultiplicator = 8;
         int tick = ticker;
         while ( (skeypress ( ct_down ) ||  skeypress ( ct_2k )) && (tvt_starty + textsizey < textsizeycomplete)) {
            tvt_starty += ticker - tick;
            tick = ticker;
            if ( tvt_starty + textsizey > textsizeycomplete )
               tvt_starty = textsizeycomplete - textsizey;
   
            displaytext();
            repaintscrollbar();
         }

         tick = ticker;
         while ( (skeypress ( ct_up ) ||  skeypress ( ct_8k )) && (tvt_starty > 0) ) {
            tvt_starty -= ticker - tick;
            tick = ticker;
            if ( tvt_starty < 0  )
               tvt_starty = 0;
   
            displaytext();
            repaintscrollbar();
         }

         if ( (skeypress ( ct_pos1 ) || skeypress ( ct_7k )) && (tvt_starty > 0)) {
            tvt_starty = 0;
            displaytext();
            repaintscrollbar();
         }

         if ( (skeypress ( ct_ende ) || skeypress ( ct_1k )) && (tvt_starty < textsizeycomplete - textsizey )) {
            tvt_starty = textsizeycomplete - textsizey;
            displaytext();
            repaintscrollbar();
         }


         tick = ticker;
         while ( (skeypress ( ct_pdown ) ||  skeypress ( ct_3k )) && (tvt_starty + textsizey < textsizeycomplete)) {
            tvt_starty += (ticker - tick) * pagepressed_scrollspeedmultiplicator;
            tick = ticker;
            if ( tvt_starty + textsizey > textsizeycomplete )
               tvt_starty = textsizeycomplete - textsizey;
   
            displaytext();
            repaintscrollbar();
         }

         tick = ticker;
         while ( (skeypress ( ct_pup ) ||  skeypress ( ct_9k )) && (tvt_starty > 0) ) {
            tvt_starty -= (ticker - tick) * pagepressed_scrollspeedmultiplicator;
            tick = ticker;
            if ( tvt_starty < 0  )
               tvt_starty = 0;
   
            displaytext();
            repaintscrollbar();
         }

}





ASCString  readtextmessage( int id )
{
   char         s1[10];
   sprintf ( s1, "##%4d", id );
   {
      char* b = s1;
      while ( *b ) {
         if ( *b == ' ' )
            *b = '0';
         b++;
      }
   }

  ASCString txt;

  int wldcrdnum = 3;

  ASCString tmpstr;
  if ( actmap )
     tmpstr = actmap->preferredFileNames.mapname[0];

  while ( tmpstr.find ( ".map") != string::npos )
     tmpstr.replace ( tmpstr.find ( ".map"), 4, ".msg" );

  while( tmpstr.find ( ".MAP") != string::npos )
     tmpstr.replace ( tmpstr.find ( ".MAP"), 4, ".msg" );

  displayLogMessage(7, ASCString("Retrieving message ") + strrr(id) + "; looking for message file " + tmpstr + "\n");


  tfindfile ff3 ( tmpstr.c_str() );
  tfindfile ff2 ( "*.msg" );
  tfindfile ff ( "helpsys?.txt" );


  tfindfile* ffa[3] = { &ff3, &ff, &ff2 };

  for ( int m = 0; m < wldcrdnum; m++ ) {

     tfindfile* pff = ffa[m];

     ASCString filefound = pff->getnextname();

     while( !filefound.empty() ) {

         tnfilestream stream ( filefound.c_str(), tnstream::reading );

         ASCString tempstr;

         bool data = stream.readTextString ( tempstr );
         int started = 0;

         while ( data  ) {
            if ( started ) {
               if ( tempstr[0] != ';' ) {
                  if ( tempstr[0] == '#'  &&  tempstr[1] == '#' ) {
                     started = 0;
                     return txt;
                  } else
                     txt += tempstr + "\n";
               }
            } else
               if ( tempstr == s1 )
                  started = 1;
   
            data = stream.readTextString ( tempstr );
         } /* endwhile */

         if ( started ) {
            started = 0;
            return txt;
         }
   
         filefound = pff->getnextname();
   
     } /* endwhile */
  }

  return txt;
}




   #define ppicture tpicture* 

   struct tpicture { 
                 void*        pict;
                 int          size;
                 int          width, height;
                 int          x, y;
                 ppicture     next; 
                 char         descrip[20]; 
              };

  class   thelpsystem : public tdialogbox, public tviewtextwithscrolling {
               public:
                   ASCString            txt;
                   char                 ok;
                    
                   char                 scrollbarvisible;
                   ppicture             firstpict;
                   char                 action;
                   int                  textstart;

                   void                 init( int id, char* titlet );
                   virtual void         setup();
                   void                 buildgraphics ( void );
                   virtual void         run ( void );
                   //void                 viewtext ( void );
                   virtual void         buttonpressed( int id);
                   void                 repaintscrollbar( void );
                   void                 done ( void );
                };



void         thelpsystem::setup( void )
{
   addbutton ( "~O~K", 10, ysize - 30, xsize - 10, ysize - 10, 0, 1, 5, 1 );
   addkey ( 5, ct_esc );
   addkey ( 5, ct_space );
   addkey ( 5, ct_enter );
}

void         thelpsystem::init(int id, char* titlet )
{ 

   // dialogbox initialisieren

   tdialogbox::init();
   x1 = 70; 
   y1 = 20; 
   xsize = 500; 
   ysize = 440; 
   textstart = 42; 
   textsizey = (ysize - textstart - 40); 
   starty = starty + 10; 
   title = titlet;
   windowstyle ^= dlg_in3d;
   action=0;



   txt = readtextmessage( id );

   if ( txt.empty() ) {
      txt =  "help topic not found : ";
      txt += strrr (id );
   }

   setup();                    

   // Koordinaten auch bei setpos „ndern
   setparams ( x1 + 13, y1 + textstart, x1 + xsize - 41, y1 + ysize - 40, txt.c_str(), black, dblue);

   tvt_dispactive = 0;
   // buildgraphics();
   displaytext(  );
   textsizeycomplete = tvt_yp;
   tvt_dispactive = 1;


   if (textsizeycomplete >= textsizey) {
      scrollbarvisible = true; 
      addscrollbar(xsize - 30,starty,xsize - 15,ysize - 40,&textsizeycomplete, textsizey, &tvt_starty,1,0);
      setscrollspeed ( 1 , 1 );

   }                                                                                       
   else 
      scrollbarvisible = false; 

   tvt_starty = 0; 
   
} 

void         thelpsystem::repaintscrollbar( void )
{
   enablebutton( 1 );
}


void         thelpsystem::buildgraphics(void)
{ 
   tdialogbox::buildgraphics();
   setpos ( x1 + 13, y1 + textstart, x1 + xsize - 41, y1 + ysize - 40 );
   rahmen(true,x1 + 10,y1 + textstart - 2,x1 + xsize - 40,y1 + textstart + textsizey + 2); 

   mousevisible(true);

   activefontsettings.font = schriften.smallarial; 
   activefontsettings.color = black; 
   activefontsettings.justify = lefttext; 
   activefontsettings.background = dblue; 
   activefontsettings.height = textsizey / linesshown; 

   firstpict = NULL; 
} 


void         thelpsystem::buttonpressed( int id )
{ 
   tdialogbox::buttonpressed(id);
   if (id == 1) 
      displaytext();

   if ( id == 5 )
      action = 11;
} 


void         thelpsystem::run(void)
{ 
   displaytext(); 
   do { 
      tdialogbox::run();
      checkscrolling();

   }  while (action < 10);
} 


void         thelpsystem::done(void)
{ 
  ppicture     pic1; 

   tdialogbox::done();
   while (firstpict != NULL) { 
      pic1 = firstpict; 
      firstpict = firstpict->next; 
      asc_free ( pic1->pict );
      delete ( pic1 );
   }
}

            


void  help( int id)
{ 
   ASCString s = readtextmessage( id );
                     
   ViewFormattedText vft( "Help System", s, PG_Rect(-1,-1,450,550));
   vft.Show();
   vft.RunModal();
} 


void  viewtext2 ( int id)
{ 

   ASCString s = readtextmessage( id );
                     
   ViewFormattedText vft( "Message", s, PG_Rect(-1,-1,450,550));
   vft.Show();
   vft.RunModal();
} 




class  tviewtextquery : public thelpsystem {
                      char* st1;
                      char* st2;
                 public:
                      void  init( int id, char* titel, char* s1, char* s2);
                      virtual void buttonpressed( int id);
                      void         setup( void );
                   };


void         tviewtextquery::setup( void )
{

   if ( st2 ) {
      addbutton ( st1, 10, ysize - 30, xsize / 2 - 5, ysize - 10, 0, 1,11, 1 );
      addbutton ( st2, xsize / 2 + 5, ysize - 30, xsize - 10, ysize - 10, 0, 1,12, 1 );
   } 
   else 
      addbutton(st1,10, ysize - 30, xsize - 10, ysize - 10, 0, 1, 11, 1 );

}


void         tviewtextquery::init( int id, char* titel, char* s1, char* s2 )
{ 
   st1 = s1;
   st2 = s2;

   thelpsystem::init(id,titel);

   buildgraphics(); 
} 


void         tviewtextquery::buttonpressed( int id)
{ 
   thelpsystem::buttonpressed(id);
   if (id == 12) 
      action = 12; 
   if (id == 11) 
      action = 11; 
} 


int         legacy_viewtextquery( int          id,
                           char *       title,
                           char *       s1,
                           char *       s2)
{ 
  tviewtextquery vtq; 
  char result;

   vtq.init(id,title,s1,s2);
   vtq.run(); 
   result = vtq.action - 11;
   vtq.done(); 
   return result;
} 


class ViewTextQuery : public ASC_PG_Dialog {

   public:
      ViewTextQuery( int id, const ASCString& title, const ASCString& button1, const ASCString& button2 ) : ASC_PG_Dialog( NULL, PG_Rect(-1, -1, 450, 500 ), title )
      {
         PG_Rect r;
         if ( button2.length() ) {
            r = PG_Rect( 10, Height() - 40, Width()/2-15, 30 );
            PG_Button* b = new PG_Button( this, PG_Rect( Width()/2+5, Height() - 40, Width()/2-15, 30 ), button2 );
            b->sigClick.connect( SigC::bind( SigC::slot( *this, &ViewTextQuery::quitModalLoop ), 1));
            b->activateHotkey(0);
         } else 
            r = PG_Rect( 10, Height() - 40, Width() - 20, 30 );
         


        PG_Button* b = new PG_Button( this, r, button1 );
        b->sigClick.connect( SigC::bind( SigC::slot( *this, &ViewTextQuery::quitModalLoop ), 0));
        b->activateHotkey(0);

        new TextRenderer( this, PG_Rect( 10, 30, Width()-20, Height() - 70 ), readtextmessage( id ));
      }
};


int         viewtextquery( int          id,
                           char *       title,
                           char *       s1,
                           char *       s2)
{ 
   if ( legacyEventSystemActive() ) 
      return legacy_viewtextquery( id, title, s1, s2 );
   else {
      ViewTextQuery vtq( id, title, s1, s2 );
      vtq.Show();
      return vtq.RunModal();
   }
} 


tdisplaymessage* messagebox = NULL;





tstringselect :: tstringselect ( )
{
   numberoflines = 0;
   firstvisibleline = 0;
   redline = 0;
   startpos = 0;
}


void         tstringselect::init(void)
{
   tdialogbox::init();

   xsize = 570;
   ysize = 320;
   x1 = ( 640 -xsize ) / 2;
   y1 = ( 480 -ysize ) / 2;

   sy = 45;

   sx = 20;
   title = "Text-Box";
   windowstyle = windowstyle ^ dlg_in3d;
   lnshown = 10;
   ey = ysize - 50;
   ex = xsize - 30;
   setup();

   if (startpos >= numberoflines ) startpos = numberoflines-1;
   if (startpos > lnshown -1 ) {
      firstvisibleline = startpos - ( lnshown - 1 );
      redline = startpos;
   } else {
      redline = startpos;
   } /* endif */

   dk = 0;
   action = 0;
   dx = (ey - sy) / lnshown;
   if (numberoflines > lnshown) {
      scrollbarvisible = true;
      addscrollbar(ex + 10 ,sy ,ex + 20,ey ,&numberoflines,lnshown,&firstvisibleline,1,0);
   }
   else scrollbarvisible = false;
   buildgraphics();
   rahmen(true,x1 + sx , y1 + sy,x1 + ex,y1 + ey);
   mousevisible(true);
   activefontsettings.font = schriften.smallarial;
   activefontsettings.color = black;
   activefontsettings.justify = lefttext;
   activefontsettings.background = lightgray;
   activefontsettings.height = 15;
   viewtext();
}


void         tstringselect::setup(void)
{
}


void         tstringselect::buttonpressed(int         id)
{
   tdialogbox::buttonpressed(id);
   if (id == 1) {
      if (firstvisibleline > redline) redline = firstvisibleline;
      if (firstvisibleline + lnshown - 1 < redline) redline = firstvisibleline + lnshown - 1;
      viewtext();
   }
}


void         tstringselect::run(void)
{
  char      view;
  int      my;
  int         ms;

   tdialogbox::run();
   if (numberoflines > 0) {
      msel = 0;
      if (getmousestatus() == 2) {
         if ((( ms = mouseparams.taste ) == 0) && (dk == 1)) dk = 2;
         if ((mouseparams.x > x1 + 10) & (mouseparams.x < x1 + xsize - 40) && (ms != 0)) {
            my = mouseparams.y - y1 - sy;
            my = my / dx;
            if ((my >= 0) && (my <= lnshown - 1) && (my <= numberoflines - 1)) {
               mouseselect = firstvisibleline + my;
               if ((mouseselect == redline) && (dk == 2)) {
                  msel = ms;
                  dk = 0;
               }
               else {
                  redline = mouseselect;
                  dk = 1;
                  ms =0;
                  viewtext();
               }
            }
         }
      }
      switch (taste) {

         case ct_up:   {
                   view = true;
                   if (redline > 0) redline--;
                   else view = false;
                   if ((redline < firstvisibleline) && (firstvisibleline > 0)) {
                      firstvisibleline--;
                      showbutton( 1 );
                   }
                   if (view) viewtext();
                }
         break;
         case ct_pos1:   {
                   view = false;
                   if  ( (redline > 0) || (firstvisibleline > 0) ) {
                      view = true;
                      redline = 0;
                      firstvisibleline = 0;
                   }
                   if (view) viewtext();
                }
         break;

         case ct_ende:   {
                   view = false;
                   if (redline < numberoflines -1 ) {
                      view = true;
                      redline = numberoflines -1 ;
                      firstvisibleline = numberoflines - lnshown;
                   }
                   if (view) viewtext();
                }
         break;

         case ct_down:   {
                     view = true;
                     if (redline < numberoflines - 1) redline++;
                     else view = false;
                     if ((redline > firstvisibleline + lnshown - 1) && (firstvisibleline + lnshown - 1 <= numberoflines)) {
                        firstvisibleline++;
                        showbutton( 1 );
                     }
                     if (view) viewtext();
                  }
      break;
      }
   }
   else redline = -1;
}


void         tstringselect::resettextfield(void)
{
   bar(x1 + sx,y1 + sy,x1 + ex,y1 + ey,lightgray);
   rahmen(true,x1 + sx ,y1 + sy,x1 + ex,y1 + ey);
}

void   tstringselect::get_text(int nr) //gibt in txt den string zur?ck
{
  strcpy(txt,"");
  nr = 0;
}

void tstringselect::scrollbar_on(void)
{
   scrollbarvisible = true;
   addscrollbar(ex + 10 ,sy - 10,ex + 30,ey + 10,&numberoflines,lnshown,&firstvisibleline,1,0);
}


void         tstringselect::viewtext(void)
{
  char         s1[200];
  Uint16         yp;
  int      l;

   mousevisible(false);
   //showbutton(1);
   npush(activefontsettings.length);
   activefontsettings.length = ex - sx - 10;
   yp = y1 + sy + 5;
   l = firstvisibleline;
   if (numberoflines > 0) {
         while ((l<numberoflines) && (l-firstvisibleline < lnshown)) {
            get_text(l);
            strcpy(s1,txt);
            if (l == redline ) activefontsettings.color=red;
            else activefontsettings.color=lightblue;
            showtext2(s1,x1+ sx + 5,yp+( l-firstvisibleline ) * dx );
            l++;
         } /* endwhile */

   }
   // else showtext2("No text available !",x1 + 50,yp + 50);

   //rahmen(true,x1  + sx ,y1 + sy,x1  + ex ,y1 + ey );
   npop(activefontsettings.length);
   mousevisible(true);
}


void         tstringselect::done(void)
{
   tdialogbox::done();
   while ( mouseparams.taste )
     releasetimeslice();
}


class  tgetid : public tdialogbox {
          public :
              tgetid () { onCancel = ReturnZero; };
              enum CancelMode { ReturnZero, ReturnOriginal } onCancel;
              int action;
              int mid;
              char nt[200];
              void init(void);
              int max,min;
              virtual void run(void);
              virtual void buttonpressed(int id);
          };

void         tgetid::init(void)
{
   tdialogbox::init();
   title = nt;
   x1 = 200;
   xsize = 220;
   y1 = 150;
   ysize = 140;
   action = 0;

   if ((mid < min) || (mid > max)) mid = 42;   /* ! */

   windowstyle = windowstyle ^ dlg_in3d;


   addbutton("~D~one",20,ysize - 40,100,ysize - 20,0,1,1,true);
   addkey(1,ct_enter);
   addbutton("~C~ancel",120,ysize - 40,200,ysize - 20,0,1,2,true);
   addbutton("",20,60,xsize - 20,80,2,1,3,true);
   addeingabe(3,&mid,min,max);

   buildgraphics();

   mousevisible(true);
}


void         tgetid::run(void)
{
   int orig = mid;
   tdialogbox::run ();
   pbutton pb = firstbutton;
   while ( pb &&  (pb->id != 3))
      pb = pb->next;

   if ( pb )
      if ( pb->id == 3 )
         execbutton( pb , false );

   do {
      tdialogbox::run();
   }  while (!((taste == ct_esc) || ((action == 1) || (action == 2))));
   if ((action == 2) || (taste == ct_esc)){
       if ( onCancel == ReturnZero )
          mid = 0;
       else
          mid = orig;
   }
}


void         tgetid::buttonpressed(int         id)
{
   tdialogbox::buttonpressed(id);
   switch (id) {

      case 1:
      case 2:   action = id;
   break;
   }
}


int      getid( const char*  title, int lval,int min,int max)

{ tgetid     gi;
   gi.onCancel = tgetid::ReturnOriginal;

   strcpy( gi.nt, title );
   gi.max = max;
   gi.min = min;
   gi.mid = lval;
   gi.init();
   gi.run();
   gi.done();
   return gi.mid;
}


class   ChooseString : public tstringselect {
   private:
      const vector<ASCString>& strings;
      const vector<ASCString>& buttons;
      char buf[10000];
   public :
      ChooseString ( const ASCString& _title, const vector<ASCString>& _strings , const vector<ASCString>& _buttons, int defaultEntry );
      void setup( );
      virtual void buttonpressed(int id);
      void run(void);
      virtual void get_text(int nr);
};



ChooseString :: ChooseString ( const ASCString& _title, const vector<ASCString>& _strings, const vector<ASCString>& _buttons, int defaultEntry )
              : strings ( _strings ), buttons ( _buttons )
{
   strcpy ( buf, _title.c_str() );
   startpos = defaultEntry;
}


void         ChooseString ::setup( )
{
   action = 0;
   title = buf;
   numberoflines = strings.size();
   ey = ysize - 50;
   if ( buttons.size() > 4 )
      xsize = 640;
   int width = (xsize-40)/buttons.size();
   for ( int i = 0; i< buttons.size(); ++i )
      addbutton( buttons[i].c_str(),25 + i*width,ysize - 45,15 + (i+1)*width,ysize - 20,0,1,20+i,true);
}


void         ChooseString ::buttonpressed(int         id)
{
   tstringselect::buttonpressed(id);
   if ( id >= 20 )
      action =id ;
}


void         ChooseString ::get_text(int nr)
{
   strcpy ( txt, strings[nr].c_str());
}


void         ChooseString ::run(void)
{
   do {
      tstringselect::run();
      /*
      if ( taste == ct_enter )
         if ( redline >= 0 )
            action = 2;
      */
   }  while ( action == 0 );
}









int chooseString ( const ASCString& title, const vector<ASCString>& entries, int defaultEntry  )
{
   vector<ASCString> b;
   b.push_back ( "~O~K");
   return chooseString ( title, entries, b, defaultEntry).second;
}




pair<int,int> chooseString ( const ASCString& title, const vector<ASCString>& entries, const vector<ASCString>& buttons, int defaultEntry  )
{

   if ( legacyEventSystemActive() ) {
      ChooseString  gps ( title, entries, buttons, defaultEntry );

      gps.init();
      gps.run();
      gps.done();
      return make_pair(gps.action-20,gps.redline);
   } else {
      return new_chooseString ( title, entries, buttons, defaultEntry );
   }
   

}





class  StringEdit : public tdialogbox {
          public :
              ASCString org;
              char nt[200];
              StringEdit () { onCancel = ReturnZero; };
              enum { ReturnZero, ReturnOriginal } onCancel;
              int action;
              int mid;
              char text[200];
              void init(void);
              int max,min;
              virtual void run(void);
              virtual void buttonpressed(int id);
          };

void         StringEdit::init(void)
{
   tdialogbox::init();
   strcpy ( text, org.c_str() );
   title = nt;
   x1 = 200;
   xsize = 220;
   y1 = 150;
   ysize = 140;
   action = 0;

   windowstyle = windowstyle ^ dlg_in3d;


   addbutton("~D~one",20,ysize - 40,100,ysize - 20,0,1,1,true);
   addkey(1,ct_enter);
   addbutton("~C~ancel",120,ysize - 40,200,ysize - 20,0,1,2,true);
   addbutton("",20,60,xsize - 20,80,1,1,3,true);
   addeingabe(3,text,0,99);

   buildgraphics();

   mousevisible(true);
}


void         StringEdit::run(void)
{
   tdialogbox::run ();
   pbutton pb = firstbutton;
   while ( pb &&  (pb->id != 3))
      pb = pb->next;

   if ( pb )
      if ( pb->id == 3 )
         execbutton( pb , false );

   do {
      tdialogbox::run();
   }  while (!((taste == ct_esc) || ((action == 1) || (action == 2))));
   if ((action == 2) || (taste == ct_esc)){
       if ( onCancel == ReturnZero )
          text[0] = 0;
       else
           strcpy( text, org.c_str() );
   }
}


void         StringEdit::buttonpressed(int         id)
{
   tdialogbox::buttonpressed(id);
   switch (id) {

      case 1:
      case 2:   action = id;
   break;
   }
}

ASCString editString( const ASCString& title, const ASCString& defaultValue  )
{
   StringEdit     gi;
   gi.org = defaultValue;
   gi.onCancel = StringEdit::ReturnOriginal ;

   strcpy( gi.nt, title.c_str() );
   gi.init();
   gi.run();
   gi.done();
   return gi.text;
}





#if 0

class   StringSelect : public tdialogbox {
                     const vector<ASCString>& entries;
                     const vector<ASCString>& buttons;
                     ASCString myTitle;
                public :
                     StringSelect( const ASCString& title, const vector<ASCString>& entries_, const vector<ASCString>& buttons_ );
                protected:
                     char  ok;
                     int sy,ey,sx,ex,action,dx;
                     int dk;
                     int msel,mouseselect,redline,lnshown,numberoflines,firstvisibleline,startpos;
                     char scrollbarvisible;
                     void init(void);
                     tstringselect ( );
                     virtual void run(void);
                     virtual void buttonpressed(int id);
                     void scrollbar_on(void);
                     void viewtext(void);
                     virtual void resettextfield(void);
                     virtual void get_text( Uint16 nr);
                     void done(void);
                 };


StringSelect :: StringSelect( const ASCString& title, const vector<ASCString>& entries_, const vector<ASCString>& buttons_ )
              : entries( entries_ ), buttons ( buttons_ )
{
   numberoflines = 0;
   firstvisibleline = 0;
   redline = 0;
   startpos = 0;
   myTitle = title;
}


void         tstringselect::init(void)
{
   tdialogbox::init();

   xsize = 570;
   ysize = 320;
   x1 = ( 640 -xsize ) / 2;
   y1 = ( 480 -ysize ) / 2;

   sy = 45;

   sx = 20;
   title = myTitle.c_str();
   windowstyle = windowstyle ^ dlg_in3d;
   lnshown = 10;
   ey = ysize - 50;
   ex = xsize - 30;
   setup();

   if (startpos >= numberoflines )
      startpos = numberoflines-1;
   if (startpos > lnshown -1 ) {
      firstvisibleline = startpos - ( lnshown - 1 );
      redline = startpos;
   } else {
      redline = startpos;
   } /* endif */

   dk = 0;
   action = 0;
   dx = (ey - sy) / lnshown;
   if (numberoflines > lnshown) {
      scrollbarvisible = true;
      addscrollbar(ex + 10 ,sy ,ex + 20,ey ,&numberoflines,lnshown,&firstvisibleline,1,0);
   }
   else
      scrollbarvisible = false;
   buildgraphics();
   rahmen(true,x1 + sx , y1 + sy,x1 + ex,y1 + ey);
   mousevisible(true);
   activefontsettings.font = schriften.smallarial;
   activefontsettings.color = black;
   activefontsettings.justify = lefttext;
   activefontsettings.background = lightgray;
   activefontsettings.height = 15;
   viewtext();
}



void         tstringselect::buttonpressed(int         id)
{
   tdialogbox::buttonpressed(id);
   if (id == 1) {
      if (firstvisibleline > redline) redline = firstvisibleline;
      if (firstvisibleline + lnshown - 1 < redline) redline = firstvisibleline + lnshown - 1;
      viewtext();
   }
}


void         tstringselect::run(void)
{
  char      view;
  int      my;
  int         ms;

   tdialogbox::run();
   if (numberoflines > 0) {
      msel = 0;
      if (getmousestatus() == 2) {
         if ((( ms = mouseparams.taste ) == 0) && (dk == 1)) dk = 2;
         if ((mouseparams.x > x1 + 10) & (mouseparams.x < x1 + xsize - 40) && (ms != 0)) {
            my = mouseparams.y - y1 - sy;
            my = my / dx;
            if ((my >= 0) && (my <= lnshown - 1) && (my <= numberoflines - 1)) {
               mouseselect = firstvisibleline + my;
               if ((mouseselect == redline) && (dk == 2)) {
                  msel = ms;
                  dk = 0;
               }
               else {
                  redline = mouseselect;
                  dk = 1;
                  ms =0;
                  viewtext();
               }
            }
         }
      }
      switch (taste) {

         case ct_up:   {
                   view = true;
                   if (redline > 0) redline--;
                   else view = false;
                   if ((redline < firstvisibleline) && (firstvisibleline > 0)) {
                      firstvisibleline--;
                      showbutton( 1 );
                   }
                   if (view) viewtext();
                }
         break;
         case ct_pos1:   {
                   view = false;
                   if  ( (redline > 0) || (firstvisibleline > 0) ) {
                      view = true;
                      redline = 0;
                      firstvisibleline = 0;
                   }
                   if (view) viewtext();
                }
         break;

         case ct_ende:   {
                   view = false;
                   if (redline < numberoflines -1 ) {
                      view = true;
                      redline = numberoflines -1 ;
                      firstvisibleline = numberoflines - lnshown;
                   }
                   if (view) viewtext();
                }
         break;

         case ct_down:   {
                     view = true;
                     if (redline < numberoflines - 1) redline++;
                     else view = false;
                     if ((redline > firstvisibleline + lnshown - 1) && (firstvisibleline + lnshown - 1 <= numberoflines)) {
                        firstvisibleline++;
                        showbutton( 1 );
                     }
                     if (view) viewtext();
                  }
      break;
      }
   }
   else redline = -1;
}


void         tstringselect::resettextfield(void)
{
   bar(x1 + sx,y1 + sy,x1 + ex,y1 + ey,lightgray);
   rahmen(true,x1 + sx ,y1 + sy,x1 + ex,y1 + ey);
}

void   tstringselect::get_text(Uint16 nr) //gibt in txt den string zur?ck
{
  strcpy(txt,"");
  nr = 0;
}

void tstringselect::scrollbar_on(void)
{
   scrollbarvisible = true;
   addscrollbar(ex + 10 ,sy - 10,ex + 30,ey + 10,&numberoflines,lnshown,&firstvisibleline,1,0);
}


void         tstringselect::viewtext(void)
{
  char         s1[200];
  Uint16         yp;
  int      l;

   mousevisible(false);
   //showbutton(1);
   npush(activefontsettings.length);
   activefontsettings.length = ex - sx - 10;
   yp = y1 + sy + 5;
   l = firstvisibleline;
   if (numberoflines > 0) {
         while ((l<numberoflines) && (l-firstvisibleline < lnshown)) {
            get_text(l);
            strcpy(s1,txt);
            if (l == redline ) activefontsettings.color=red;
            else activefontsettings.color=lightblue;
            showtext2(s1,x1+ sx + 5,yp+( l-firstvisibleline ) * dx );
            l++;
         } /* endwhile */

   }
   // else showtext2("No text available !",x1 + 50,yp + 50);

   //rahmen(true,x1  + sx ,y1 + sy,x1  + ex ,y1 + ey );
   npop(activefontsettings.length);
   mousevisible(true);
}


void         tstringselect::done(void)
{
   tdialogbox::done();
   while ( mouseparams.taste )
     releasetimeslice();
}
#endif
