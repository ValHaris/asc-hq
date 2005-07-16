/*! \file pd.cpp
    \brief The implementation of the Pulldown Menu
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

#include <cstring>
#include <stdlib.h>
#include <cstdlib>
#include <ctype.h>

#include "tpascal.inc"
#include "newfont.h"
#include "basegfx.h"
#include "events.h"
#include "pd.h"
#include "misc.h"


   pfont        pulldownfont;
   tfontsettings      savefont;

tpulldown :: tpulldown ( void )
{
   pdb.count = 0;
   alwaysOpen = false;
   barstatus = false;
}


void tpulldown::init(void)
{
   int ml = 0;
   int mr = 0;
   int rlang;


   action2execute = -1;
   bkgcolor = lightgray;            
   rcolor1 = white;
   rcolor2 = darkgray;
   textcolor = black;             
   shortkeycolor = red;     
   pdb.pdbreite = 20;
   pdfieldtextdistance = 50;
   textstart = 10;
   righttextdifference = 45;

   pdfieldnr = 255;

   for (int i = 0; i < pdb.count; i++)
   {
      pdb.field[i].height = getpdfieldheight(i,pdb.field[i].count);
      pdb.field[i].xwidth = gettextwdth(pdb.field[i].name,pulldownfont) - 4;
      for (int j = 0; j < pdb.field[i].count; j++)
         { 
            getleftrighttext(pdb.field[i].button[j].name,lt,rt);
            int llang = gettextwdth(lt,pulldownfont);
            if (rt != NULL ) rlang = gettextwdth(rt,pulldownfont);
            else rlang = 0;
            if (llang > ml) ml = llang;
            if (rlang > mr) mr = rlang;
         } 
      pdb.field[i].xwidth = textstart + ml + textdifference + mr + textstart;
      pdb.field[i].rtextstart =textstart + ml + textdifference;
      ml = 0;
      mr = 0;
    } 
}

void tpulldown::setvars(void)
{
   activefontsettings.color = textcolor;
   activefontsettings.font = pulldownfont; 
   activefontsettings.background = bkgcolor;
   activefontsettings.length = 300;
   activefontsettings.markcolor = shortkeycolor;
   activefontsettings.justify = lefttext;

}


void tpulldown::addbutton ( char* name, int id )
{
   tpdbutton* btn = &pdb.field[ pdb.count-1 ].button[ pdb.field[pdb.count-1].count++ ];
   strcpy ( btn->name , name );
   btn->actionid = id;
}

void tpulldown::addfield ( char* name )
{
   pdb.field[ pdb.count ].count = 0;
   strcpy ( pdb.field[ pdb.count ].name , name );
   pdb.count++;
}


void tpulldown::run(void)
{
   mousestat = mouseparams.taste; 
   char cancel = false;
   do { 
      if (mouseparams.y <= pdb.pdbreite) {
         if ( (pdfieldnr < pdb.count-1 ) && (mouseparams.x > pdb.field[pdfieldnr+1].xstart)) {
            // collategraphicoperations cgo;
            closepdfield();
            pdfieldnr++;
            openpdfield();
         } else if ( (pdfieldnr > 0 ) && (mouseparams.x < pdb.field[pdfieldnr].xstart) ) {
            // collategraphicoperations cgo;
            closepdfield();
            pdfieldnr --;
            openpdfield();
         } /* endif */
      } else if (mouseparams.y <= pdb.pdbreite + pdb.field[pdfieldnr].height) {
         if ( (mouseparams.x >= anf ) && (mouseparams.x <= ende ) ) {
            if ( (buttonnr > 0 ) && (mouseparams.y < pdb.pdbreite + 4 + getpdfieldheight(pdfieldnr,buttonnr) ) ) {
               // collategraphicoperations cgo;
               hidebutton();
               buttonnr--;
               showbutton();
            } else if ( (buttonnr < pdb.field[pdfieldnr].count-1 ) && (mouseparams.y > pdb.pdbreite + 4 + getpdfieldheight(pdfieldnr,buttonnr+1) ) ) {
               // collategraphicoperations cgo;
               hidebutton();
               buttonnr++;
               showbutton();
            } /* endif */
         } /* endif */
      } /* endif */

      if (mousestat != mouseparams.taste) {
         if (mouseparams.y <= pdb.pdbreite) 
            mousestat = mouseparams.taste;
         else {
            if ( mouseparams.taste == 1 )
               mousestat = mouseparams.taste;
            else
               cancel = true;
           /*
            while ( mouseparams.taste )
               releasetimeslice();
           */
         }
      }

      if (keypress() ) {
         tkey ch = r_key();
         switch (ch) {
         case ct_esc : {
               pdfieldnr = 255;
               buttonnr = 255;
               return;
            }
         case ct_up : {
                hidebutton();
                if ( buttonnr>0 ) buttonnr--;
                else buttonnr = pdb.field[pdfieldnr].count-1;
                if (strcmp(pdb.field[pdfieldnr].button[buttonnr].name,"seperator") == 0) buttonnr--;
                showbutton();
            }
            break;
            case ct_down : {
                hidebutton();
                if (buttonnr < pdb.field[pdfieldnr].count-1 ) buttonnr++;
                else buttonnr = 0;
                if (strcmp(pdb.field[pdfieldnr].button[buttonnr].name,"seperator") == 0) buttonnr++;
                showbutton();
            }
            break;
         case ct_left : {
               closepdfield();
               if (pdfieldnr > 0 ) pdfieldnr--;
               else pdfieldnr = pdb.count-1;
               openpdfield();
            }
            break;
         case ct_right : {
               closepdfield();
               if (pdfieldnr < pdb.count-1 ) pdfieldnr++;
               else pdfieldnr = 0;
               openpdfield();
            }
            break;
         case ct_space:
         case ct_enter: action2execute = pdb.field [ pdfieldnr] .button [ buttonnr].actionid;
                        return;

         default : {
                for (int k = 0; k < pdb.field[pdfieldnr].count; k++) 
                    if (pdb.field[pdfieldnr].button[k].shortkey == ch ) {
                       action2execute = pdb.field [ pdfieldnr] .button [k].actionid;
                       buttonnr = k;
                       return;
                    }
            }
            break;
         } /* endswitch */
      }
      releasetimeslice();
   }  while (cancel == false);
   if ( (mouseparams.y > pdb.pdbreite  + pdb.field[pdfieldnr].height ) || (mouseparams.x < anf ) || (mouseparams.x > ende ) ) { 
      pdfieldnr = 255;
      buttonnr = 255;
      action2execute = -1;
   } else
      action2execute = pdb.field [ pdfieldnr] .button [ buttonnr].actionid;
}

void tpulldown::getleftrighttext(char *qtext, char *ltext,char *rtext)
{
   strcpy ( ltext, qtext );
   rtext[0] = 0;
   for (int j = 0 ; j <= strlen(qtext) ; j++ ) {
      if ( (ltext[j] == rz) || (ltext[j] == rz2)){
         ltext[j]=0;
         strcpy ( rtext, &ltext[j+1] );
      } /* endif */
   } /* endfor */
}

void tpulldown::checkpulldown(void)
{
   if (mouseparams.y <= pdb.pdbreite || skeypress(ct_lalt) || skeypress(ct_ralt)) {
      baron();
      if (mouseparams.taste == 1 ) {
         pdfieldnr = 0;
         for (int i=0;i < pdb.count-1  ;i++ )
            if (mouseparams.x > pdb.field[i+1].xstart)
               pdfieldnr++;
         openpdfield();
         run();
         done();
      } else
         if ( key != ct_invvalue )
            for (int i = 0; i < pdb.count; i++)
               if (key == pdb.field[i].shortkey + ct_altp ) {
                  baron();
                  pdfieldnr=i;
                  openpdfield();
                  run();
                  done();
                  key = ct_invvalue;
                  return;
               }

   } 
   else baroff(); 
}


void tpulldown::lines(int x1,int y1,int x2,int y2)
{ 
   line(x1,y1,x2,y1,rcolor1);
   line(x2,y1,x2,y2,rcolor2);
   line(x1,y2,x2,y2,rcolor2);
   line(x1,y1,x1,y2,rcolor1);
} 


void tpulldown::nolines(int x1,int y1,int x2,int y2)
{ 
   rectangle(x1,y1,x2,y2,bkgcolor);
} 


void tpulldown::done(void)
{
   closepdfield();
   activefontsettings = savefont; 
}

void tpulldown::redraw( )
{
   barstatus = false;
   baron();
}


void tpulldown::baron(void)
{ 
   if (barstatus == false ) {
      collategraphicoperations cgo ( 0,0, agmp->resolutionx-1 ,pdb.pdbreite );

      savefont = activefontsettings; 
      setvars();
      mousevisible(false);

      barbackgrnd = asc_malloc (imagesize(0,0, agmp->resolutionx-1 ,pdb.pdbreite) );
      getimage(0,0, agmp->resolutionx-1 ,pdb.pdbreite,barbackgrnd);

      bar(0,0, agmp->resolutionx-1 ,pdb.pdbreite,bkgcolor);

      bar(0,0, agmp->resolutionx-1 ,pdb.pdbreite,bkgcolor);
      lines(0,0, agmp->resolutionx-1 ,pdb.pdbreite); 
      int x = textstart;
      for (int i = 0; i < pdb.count; i++) {
         activefontsettings.length = gettextwdth(pdb.field[i].name, pulldownfont);
         showtext3(pdb.field[i].name,x+pdfieldtextdistance /2,pdb.pdbreite / 2 - 6);
         pdb.field[i].xstart = x;
         x += gettextwdth(pdb.field[i].name,pulldownfont) + pdfieldtextdistance;
      } 
      mousevisible(true);
      barstatus = true;
   }
} 

void tpulldown::baroff(void)
{ 
   if (barstatus == true && !alwaysOpen) {
      collategraphicoperations cgo ( 0,0, agmp->resolutionx-1 ,pdb.pdbreite );

      mousevisible(false);
      putimage(0,0,barbackgrnd);
      asc_free( barbackgrnd );
      mousevisible(true);
      barstatus = false;
   }
}


int tpulldown::getpdfieldheight(int pdfieldnr,int pos)
{
  int         i, j;

   j = 0;
   for (i = 0; i < pos; i++)
      if (strcmp(pdb.field[pdfieldnr].button[i].name,"seperator") == 0) j = j + 7;
      else j = j + pulldownfont->height + 6;
   return j; 
} 


void         tpulldown::openpdfield(void)
{

   int zw;
   setvars();

   activefontsettings.length = pdb.field[pdfieldnr].xwidth;
   // char abbrch = false;
   anf = pdb.field[pdfieldnr].xstart + pdfieldtextdistance / 2 - pdfieldenlargement;
   ende =  anf + pdb.field[pdfieldnr].xwidth + pdfieldenlargement*2;
   bool umbau = false;
   if (ende > agmp->resolutionx-4 ) { 
      umbau = true; 
      zw = ende - (agmp->resolutionx-4);
      ende -= zw;
      anf -= zw;
   } 

   mousevisible(false);
   collategraphicoperations   cgo ( anf - 3, 0 ,ende + 3, pdb.pdbreite + 6 + pdb.field[pdfieldnr].height );
   backgrnd = asc_malloc( imagesize(anf - 3, 0 ,ende + 3, pdb.pdbreite + 6 + pdb.field[pdfieldnr].height) );
   getimage(                        anf - 3, 0 ,ende + 3, pdb.pdbreite + 6 + pdb.field[pdfieldnr].height,backgrnd);

   bar( anf - 3,pdb.pdbreite,ende + 3,pdb.pdbreite + 6 + pdb.field[pdfieldnr].height,bkgcolor);
   lines(anf - 3,pdb.pdbreite,ende + 3,pdb.pdbreite + 6 + pdb.field[pdfieldnr].height);
   int lang = gettextwdth(pdb.field[pdfieldnr].name,pulldownfont) + 13;

   if (umbau == true) {
      line(anf - 2, pdb.pdbreite, anf + lang -1 + zw , pdb.pdbreite,bkgcolor);
      line(anf - 3,1,anf - 3,pdb.pdbreite + 1,rcolor1);
      line(anf - 3,1,anf + lang + zw ,1,rcolor1);
      line(anf +  lang + zw ,1,anf + lang + zw ,pdb.pdbreite ,rcolor2);
   } else {
      line(anf - 2, pdb.pdbreite, anf + lang -1, pdb.pdbreite,bkgcolor);
      line(anf - 3,1,anf - 3,pdb.pdbreite + 1,rcolor1);
      line(anf - 3,1,anf + lang,1,rcolor1);
      line(anf +  lang,1,anf + lang,pdb.pdbreite ,rcolor2);
   }

   for (int i = 0; i < pdb.field[pdfieldnr].count; i++) {
         if (strcmp(pdb.field[pdfieldnr].button[i].name,"seperator") != 0) {
            getleftrighttext(pdb.field[pdfieldnr].button[i].name,lt,rt);
            activefontsettings.justify = lefttext;
            activefontsettings.length = gettextwdth(lt,pulldownfont);
            showtext3( lt ,anf + textstart ,pdb.pdbreite + 7 + getpdfieldheight(pdfieldnr,i));
            if (rt[0]) {
               activefontsettings.justify = lefttext;
               activefontsettings.length = gettextwdth(rt,pulldownfont);
               showtext3(rt, anf + pdb.field[pdfieldnr].rtextstart,pdb.pdbreite + 7 + getpdfieldheight(pdfieldnr,i));
            }
         }
         else
            line(anf,pdb.pdbreite + 7 + getpdfieldheight(pdfieldnr,i),ende + 1,pdb.pdbreite + 7 + getpdfieldheight(pdfieldnr,i),rcolor2);
      }

   buttonnr = 0;
   showbutton();

   mousevisible(true); 
} 

void         tpulldown::closepdfield(void)
{
   int w, h;
   getpicsize ( backgrnd, w,h );
   collategraphicoperations cgo ( anf-3, 0, anf-3 + w-1, h);
   mousevisible(false);
   putimage(anf - 3,0,backgrnd);
   asc_free(backgrnd); 
   mousevisible(true); 
}

void tpulldown::hidebutton(void)
{ 

   if (strcmp(pdb.field[pdfieldnr].button[buttonnr].name,"seperator") == 0) return;
   mousevisible(false); 
   collategraphicoperations cgo ( anf,pdb.pdbreite + 4 + getpdfieldheight(pdfieldnr,buttonnr),ende,pdb.pdbreite + 4 + getpdfieldheight(pdfieldnr,buttonnr+1));
   nolines(                       anf,pdb.pdbreite + 4 + getpdfieldheight(pdfieldnr,buttonnr),ende,pdb.pdbreite + 4 + getpdfieldheight(pdfieldnr,buttonnr+1));
   mousevisible(true); 
} 

void tpulldown::showbutton(void)
{ 

   if (strcmp(pdb.field[pdfieldnr].button[buttonnr].name,"seperator") == 0) return;
   mousevisible(false); 
   collategraphicoperations cgo ( anf,pdb.pdbreite + 4 + getpdfieldheight(pdfieldnr,buttonnr),ende,pdb.pdbreite + 4 + getpdfieldheight(pdfieldnr,buttonnr+1));
   lines(                         anf,pdb.pdbreite + 4 + getpdfieldheight(pdfieldnr,buttonnr),ende,pdb.pdbreite + 4 + getpdfieldheight(pdfieldnr,buttonnr+1));
   mousevisible(true); 
} 

void tpulldown::setshortkeys(void)
{ 
  int j;

   for (int i = 0; i < pdb.count; i++)
      { 
         pdb.field[i].shortkey = 0;
         for (j = 0; j < strlen(pdb.field[i].name); j++)
            if (pdb.field[i].name[j] == '~') {
               pdb.field[i].shortkey = char2key( pdb.field[i].name[j+1] );
               j = strlen(pdb.field[i].name); 
            } 
         for (int k = 0; k < pdb.field[i].count; k++) {
            pdb.field[i].button[k].shortkey = 0; 
            for (j = 0; j < strlen(pdb.field[i].button[k].name); j++) {
               if (pdb.field[i].button[k].name[j] == '~') { 
                  pdb.field[i].button[k].shortkey = char2key( pdb.field[i].button[k].name[j+1] );
                  j = strlen(pdb.field[i].button[k].name); 
               } 
            } 
         } 
      }
   } 

