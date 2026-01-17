/*! \file dialog.cpp
    \brief Many many dialog boxes used by the game and the mapeditor
*/

/*
    This file is part of Advanced Strategic Command; http://www.asc-hq.de
    Copyright (C) 1994-2010  Martin Bickel  and  Marc Schellenberger

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
#include <ctype.h>
#include <stdlib.h>
#include <stdarg.h>

#include "typen.h"
#include "buildingtype.h"
#include "vehicletype.h"
#include "basegfx.h"
#include "newfont.h"
#include "spfst.h"
#include "loaders.h"
#include "misc.h"
#include "events.h"
#include "stack.h"
#include "dlg_box.h"
#include "dialog.h"
#include "sgstream.h"
#include "attack.h"
#include "gameoptions.h"
#include "errors.h"
// #include "password_dialog.h"
#include "itemrepository.h"
#include "mapdisplay.h"
#include "graphicset.h"
#include "viewcalculation.h"
#include "paradialog.h"
#include "cannedmessages.h"

#include "dialogs/messagedialog.h"
#include "dialogs/fileselector.h"
#include "widgets/textrenderer.h"
#include "unitset.h"
#include "pgpropertyfield_integer.h"
#include "spfst-legacy.h"


#define markedlettercolor red

#define klickconst 100
#define delayconst 10


void  dispmessage2( const ActionResult& result )
{
   dispmessage2( result.getCode(), result.getMessage().c_str() );  
}


void         dispmessage2(int          id,
                          const char *       st)
{
   char          *s2;

   const char* sp = getmessage(id);
   const char* s1 = sp;
   if (sp != NULL) {
      char s[200];
      s2 = s;
      while (*s1 !=0) {
        if ( *s1 != 35 /* # */ ) {
           *s2 = *s1;
           s2++;
           s1++;
        } else {
           if (st != NULL) {
              while (*st != 0) {
                 *s2 = *st;
                 s2++;
                 st++;
              } /* endwhile */
           } /* endif */
        } /* endif */
      } /* endwhile */
      *s2 = 0;

      displaymessage2(s); 
   }
} 




class  tchoice_dlg : public tdialogbox {
               public:
                  int      result;
                  void      init( const char* a, const char* b, const char* c );
                  virtual void buttonpressed( int id );
                  virtual void run( void );
                };


void         tchoice_dlg::init( const char* a, const char* b, const char* c )
{
  tdialogbox::init();
  windowstyle |= dlg_notitle;
  int wdth = gettextwdth ( a, schriften.arial8 ) ;
  if ( wdth > 280 )
     xsize = wdth + 20;
  else
     xsize = 300; 

  ysize = 100; 
  addbutton(b,10,60,xsize/2-5,90,0,1,1,true); 
  addbutton(c,xsize/2+5,60,xsize-10,90,0,1,2,true);

  x1 = (640 - xsize) / 2;
  y1 = (480 - ysize) / 2;

  buildgraphics();
  activefontsettings.font = schriften.arial8; 
  activefontsettings.justify = centertext; 
  activefontsettings.length = xsize - 20;
  activefontsettings.background = 255;
  if ( actmap && actmap->actplayer >= 0 )
     activefontsettings.color = actmap->actplayer * 8 + 20;
  else
     activefontsettings.color = 20;
  mousevisible ( false );
  showtext2(a,x1 + 10,y1 + 15);
  mousevisible ( true );
  result = 0; 
} 


void         tchoice_dlg::buttonpressed( int id )
{ 
  if (id == 1) result = 1; 
  if (id == 2) result = 2; 
} 


void         tchoice_dlg::run(void)
{ 
  while ( mouseparams.taste )
     releasetimeslice();
  mousevisible(true); 
  do { 
    tdialogbox::run();
  }  while (result  == 0);
} 


int         legacy_choice_dlg( const char *       title,
                        const char *       s1,
                        const char *       s2,
                        ... )
{

   va_list paramlist;
   va_start ( paramlist, s2 );

   char tempbuf[1000];

   int lng = vsprintf( tempbuf, title, paramlist );
   va_end ( paramlist );
   if ( lng >= 1000 )
      displaymessage ( "dialog.cpp / choice_dlg:   string to long !\nPlease report this error",1 );

  tchoice_dlg  c;
  int a;

  c.init(tempbuf,s1,s2);
  c.run();
  a = c.result;
  c.done();
  return a;
}


int  choice_dlg(const ASCString& title, const ASCString& leftButton, const ASCString& rightButton )
{
   if ( legacyEventSystemActive() ) {
      return legacy_choice_dlg( title.c_str(), leftButton.c_str(), rightButton.c_str() );
   } else {
      return new_choice_dlg( title, leftButton, rightButton );
   }
   return 0;
}



/*********************************************************************************************/
/* unpack_date :  entpackt das datum (von zb dateien)                                        */
/*********************************************************************************************/
void        unpack_date (unsigned short packed, int &day, int &month, int &year)
{
   year = packed >> 9;
   month = (packed - year*512) >> 5;
   day = packed - year*512 - month*32;
   year +=1980;
}




/*********************************************************************************************/
/* unpack_time :  entpackt die zeit (von zb dateien)                                         */
/*********************************************************************************************/
void        unpack_time (unsigned short packed, int &sec, int &min, int &hour)
{
   hour = packed >> 11;
   min = (packed - hour*2048) >> 5;
   sec = packed - hour*2048 - min*32;       // sekunden sind noch nicht getestet !!!
}





void viewVisibilityStatistics()
{
/*
   ASCString msg;

   tmap::Shareview* sv = actmap->shareview;
   actmap->shareview = NULL;
   computeview ( actmap );

   for ( int i = 0; i < 8; i++ )
      if ( actmap->player[i].exist() ) {
         msg += ASCString("#font02#Player ") + strrr ( i ) + "#font01#\n" ;
         int notVisible = 0;
         int fogOfWar = 0;
         int visible = 0;
         for ( int x = 0; x < actmap->xsize; x++ )
            for ( int y = 0; y < actmap->ysize; y++ ) {
                VisibilityStates vs = fieldVisibility  ( actmap->getField ( x, y ), i );
                switch ( vs ) {
                   case visible_not: ++notVisible;
                   break;
                   case visible_ago: ++fogOfWar;
                   break;
                   default: ++visible;
                }
            }
         msg += ASCString("  not visible: ") + strrr(notVisible ) + " fields\n";
         msg += ASCString("  fog of war: ") + strrr(fogOfWar ) + " fields\n";
         msg += ASCString("  visible: ") + strrr(visible ) + " fields\n\n";
      }


   actmap->shareview = sv;
   computeview ( actmap );

*/
   tviewanytext vat;
   vat.init ( "Visibility Statistics", "Sorry, not available in ASC2 " );
   vat.run();
   vat.done();
}





void         tviewanytext:: init( const char* _title, const char* text , int xx1 , int yy1  , int xxsize , int yysize  )
{ 

   // dialogbox initialisieren

   tdialogbox::init();
   x1 = xx1; 
   y1 = yy1; 
   xsize = xxsize; 
   ysize = yysize; 
   textstart = 42 ; 
   textsizey = (ysize - textstart - 40); 
   starty = starty + 10; 
   title = _title;
   windowstyle ^= dlg_in3d;
   action=0;


   txt = text;
                       
   setparams ( x1 + 13, y1 + textstart, x1 + xsize - 41, y1 + ysize - 40, txt, black, dblue);
   addbutton ( "~O~K", 10, ysize - 30, xsize - 10, ysize - 10, 0, 1, 10, true );
   addkey ( 10, ct_space );
   addkey ( 10, ct_enter );
   addkey ( 10, ct_esc );
   tvt_dispactive = 0;
   displaytext(  );
   textsizeycomplete = tvt_yp;
   tvt_dispactive = 1;


   if (textsizeycomplete >= textsizey) {
      scrollbarvisible = true; 

      addscrollbar(xsize - 30,starty,xsize - 15,ysize - 35,&textsizeycomplete, textsizey, &tvt_starty,1,0);
      setscrollspeed ( 1 , 1 );

      rightspace = 40;
   }                                                                                       
   else {
      rightspace = 10;
      scrollbarvisible = false; 
   }

   tvt_starty = 0; 
   buildgraphics();
} 


void         tviewanytext::repaintscrollbar ( void )
{
   enablebutton( 1 );
}


void         tviewanytext::redraw(void)
{ 
  collategraphicoperations cgo ( x1, y1, x1 + xsize, y1 + ysize);

   tdialogbox::redraw();
   rahmen(true,x1 + 10,y1 + textstart - 2,x1 + xsize - rightspace,y1 + textstart + textsizey + 2); 


   activefontsettings.font = schriften.smallarial; 
   activefontsettings.justify = lefttext; 
   activefontsettings.background = dblue; 
   activefontsettings.length =0;

   activefontsettings.color = black; 
   activefontsettings.height = textsizey / linesshown; 

   int xd = 15 - (rightspace - 10) / 2;

   setpos ( x1 + 13 + xd, y1 + textstart, x1 + xsize - 41 + xd, y1 + ysize - 40 );
   displaytext(); 

} 


void         tviewanytext::buttonpressed( int id )
{     
   tdialogbox::buttonpressed(id);
   if (id == 1) 
      displaytext();
   if ( id== 10 )
      action = 11;
} 


void         tviewanytext::run(void)
{ 
   mousevisible(true);
   do { 
      tdialogbox::run();
      checkscrolling ( );
      if ( prntkey == 's' )
         printf("%s", txt);
   }  while (action < 10);
} 






class ResizeMapDialog : public ASC_PG_Dialog {
    int top, bottom, left, right;
    PG_PropertyEditor* editor;

    bool check() {
        if ( (top & 1) || (bottom & 1)  ) {
           warningMessage( "values must be even !" );
           return false;
        }
        if ( (actmap->xsize + left + right < 4 ) || (actmap->ysize + top + bottom < 4 )) {
            warningMessage( "remaining size too small ! !" );
            return false;
        }
        if ( (top + bottom + actmap->ysize > 32000 ) || (left + right + actmap->xsize > 32000)) {
            warningMessage ( "new map too large !");
            return false;
        }
        return true;
    }

    bool apply() {
        editor->Apply();
        if ( check() ) {
            int result = actmap->resize ( top, bottom, left, right );
            if ( result ) {
              displaymessage ( "resizing failed" , 1 );
              return false;
            }
            QuitModal();
            return true;
        }
        return false;
    }

public:
    ResizeMapDialog() : ASC_PG_Dialog(NULL, PG_Rect(-1, -1, 400, 300), "Resize Map") , top(0), bottom(0), left(0), right(0) {

        new PG_Label( this, PG_Rect( 25, 220, 150, 20), "positive values: enlarge map");
        new PG_Label( this, PG_Rect( 25, 245, 150, 20), "negative values: shrink map");

        AddStandardButton("Apply")->sigClick.connect( sigc::hide( sigc::mem_fun(*this, &ResizeMapDialog::apply )));
        AddStandardButton("cancel")->sigClick.connect( sigc::hide( sigc::mem_fun(*this, &ResizeMapDialog::QuitModal )));

        editor = new PG_PropertyEditor(this, PG_Rect(10, 30, Width()-20, 200));
        (new PG_PropertyField_Integer<int>(editor, "Top", &top))->SetRange(-1000,30000);
        (new PG_PropertyField_Integer<int>(editor, "Left", &left))->SetRange(-1000,30000);
        (new PG_PropertyField_Integer<int>(editor, "Right", &right))->SetRange(-1000,30000);
        (new PG_PropertyField_Integer<int>(editor, "Bottom", &bottom))->SetRange(-1000,30000);
    }
};



void resizemap ( void )
{
    ResizeMapDialog rmd;
    rmd.Show();
    rmd.RunModal();
    displaymap();
}






void showbdtbits( void )
{
   MapField* fld = actmap->getField(actmap->getCursor());
   ASCString m;
   for (int i = 0; i < terrainPropertyNum ; i++) {
      TerrainBits bts;
      bts.set ( i );

      if ( (fld->bdt & bts).any() ) {
         m = m + terrainProperty[i] + "\n";
      }
   } /* endfor */
   displaymessage( m, 3 );
}

void appendTerrainBits ( char* text, const TerrainBits* bdt )
{
   for (int i = 0; i < terrainPropertyNum ; i++) {
      TerrainBits bts;
      bts.set ( i );

      if ( (*bdt & bts).any() ) {
         strcat ( text, "    " );
         strcat ( text, terrainProperty[i] );
         strcat  ( text, "\n" );
      }
   } /* endfor */
}


void viewUnitSetinfo ( void )
{
   ASCString s;
   MapField* fld = actmap->getField( actmap->getCursor() );
   if ( fld && fieldvisiblenow  ( fld ) && fld->vehicle ) {

         s += "#fontsize=18#Unit Information:#fontsize=14##aeinzug20##eeinzug20##crtp20#" ;

         const VehicleType* typ = fld->vehicle->typ;
/*
         s += "\nreactionfire.Status: ";
         s += strrr( getactfield()->vehicle->reactionfire.status );
         s += "\nreactionfire.enemiesattackable: ";
         s += strrr ( getactfield()->vehicle->reactionfire.enemiesAttackable );
*/
         s += "Unit name: " ;
         if ( !typ->name.empty() )
            s += typ->name ;
         else
            s += typ->description;

         s += "\nUnit owner: " + ASCString::toString( fld->vehicle->getOwner() ) + " - " + actmap->getPlayer(fld->vehicle).getName();

         s += "\nUnit ID: " + ASCString::toString( typ->id );

         if ( !typ->location.empty() ) 
            s += typ->location;

         if ( unitSets.size() > 0 )
            for ( unsigned int i = 0; i < unitSets.size(); i++ )
               if ( unitSets[i]->isMember ( typ->id,SingleUnitSet::unit )) {
                  s += "Unit is part of this unit set:";
                  if ( unitSets[i]->name.length()) {
                     s += "#aeinzug20##eeinzug20#\nName: #aeinzug60##eeinzug60#\n";
                     s += unitSets[i]->name;
                  }
                  if ( unitSets[i]->maintainer.length()) {
                     s += "#aeinzug20##eeinzug20#\nMaintainer: #aeinzug60##eeinzug60#\n";
                     s += unitSets[i]->maintainer;
                  }
                  if ( unitSets[i]->information.length()) {
                     s += "#aeinzug20##eeinzug20#\nInformation: #aeinzug60##eeinzug60#\n";
                     s += unitSets[i]->information;
                  }
                  s += "#aeinzug0##eeinzug0#\n";
               }


   } else
      s += "\nNo unit selected";

   ViewFormattedText vft( "Unit information", s, PG_Rect( -1, -1, 400, 350) );
   vft.Show();
   vft.RunModal();
}






#define blocksize 256


int tparagraph :: winy1;
int tparagraph :: winy2;
int tparagraph :: winx1;
int tparagraph :: maxlinenum;

tparagraph :: tparagraph ( void )
{
   text = new char [ blocksize ];
   text[0] = 0;
   size = 1;
   allocated = blocksize;

   next = NULL;
   prev = NULL;
   cursor = -1;
   cursorstat = 0;
   ps.line1num = 0;
   linenum = -1;
   searchcursorpos = 0;
}

tparagraph :: tparagraph ( pparagraph prv )
{
   searchcursorpos = 0;
   linenum = -1;
   text = new char [ blocksize ];
   text[0] = 0;
   size = 1;
   allocated = blocksize;
   cursorstat = 0;
   cursor = -1;

   prev = prv;

   if ( prv ) {
      next = prv->next;
      if ( next )
         next->prev = this;
      prv->next = this;

      ps.line1num = prv->ps.line1num + 1 + prv->linenum;
   } else {
      next = NULL;
      ps.line1num = 0;
   }
}



tparagraph :: ~tparagraph ()
{
   if ( text ) {
      delete[] text;
      text = NULL;
   }

   if ( prev )
      prev->next = next;

   if ( next )
      next->prev = prev;

}


void tparagraph :: changesize ( int newsize )
{
   newsize = (newsize / blocksize + 1) * blocksize;
   char* temp = new char[newsize];
   if ( text ) {
      strcpy ( temp, text );
      delete[] text;
   } else
      temp[0] = 0;

   allocated = newsize;
   size = strlen ( temp )+1;
   text = temp;
}


void tparagraph :: join ( void )
{
   cursor = size-1;
   addtext ( next->text );
   delete next;
   if ( reflow() ) {
       display();
       if ( checkcursorpos() ) {
          checkscrollup();
          checkscrolldown();
       }
   }


}



void tparagraph :: addtext ( const ASCString& txt )
{
    int nsize = txt.length();
    if ( allocated < nsize + size )
       changesize ( nsize + size );

    strcat ( text, txt.c_str() );
    size = strlen ( text ) + 1;
}

void tparagraph :: checkcursor( void )
{
   if ( cursor != -1 ) {
      if ( cursor >= size )
         cursor = size-1;
      if ( cursor < 0 )
         cursor = 0;
   } else
      displaymessage ( " void tparagraph :: checkcursor ( void ) \ncursor not in paragraph !\n text is : %s", 2, text );
}



void tparagraph :: addchar ( char c )
{
   checkcursor();
   if ( size + 1 > allocated )
      changesize ( size + 1 );

    for ( int i = size; i > cursor; i--)
       text[i] = text[i-1];

    text[cursor] = c;
    cursor++;
    size++;

    if ( reflow() ) {
       display();
       checkcursorpos();
    }
}


pparagraph tparagraph :: erasechar ( int c )
{
   checkcursor();
   if ( c == 1 ) {     // backspace
      if ( cursor ) {
         for ( int i = cursor-1; i < size; i++)
            text[i] = text[i+1];
         cursor--;
         size--;
         if ( reflow() )
            display();
      } else
         if ( prev ) {
            pparagraph temp = prev;
            prev->join();
            return temp;              //  !!!###!!!   gef�llt mir eigentlich ?berhauptnicht, wird aber wohl laufen. Sonst m?�te ich halt erasechar von au�en managen
         }

   }
   if ( c == 2 ) {     // del
      if ( cursor < size-1 ) {
         for ( int i = cursor; i < size; i++)
            text[i] = text[i+1];
         size--;
         if ( reflow() )
            display();
      } else
         if ( next )
            join ( );
   }
   return this;
}

int  tparagraph :: checkcursorpos ( void )
{
   if ( cursor >= 0 ) {
      checkcursor();
      if ( ps.line1num + cursory >= maxlinenum ) {
         ps.line1num = maxlinenum - cursory - 1;
         display();
         checkscrollup();
         checkscrolldown();
         return 0;
      }

      if( ps.line1num + cursory < 0 ) {
         ps.line1num = -cursory;
         display();
         checkscrollup();
         checkscrolldown();
         return 0;
      }
   }
   return 1;
}

pparagraph tparagraph :: cut ( void )
{
   checkcursor();
   displaycursor();
   char* tempbuf = strdup ( &text[cursor] );
   text[cursor] = 0;
   size = strlen ( text ) + 1;
   reflow ( 0 );
   new tparagraph ( this );
   if ( tempbuf[0] )
      next->addtext ( tempbuf );

   free ( tempbuf );
   cursor = -1;
   next->cursor = 0;
   reflow( 0 );
   display();
   if ( next->reflow() )
      next->display();

   return next;
}

void tparagraph :: checkscrollup ( void )
{
   if ( prev ) {
      if ( prev->ps.line1num + prev->linenum + 1 != ps.line1num ) {
           prev->ps.line1num = ps.line1num - 1 - prev->linenum ;
           prev->display();
           prev->checkscrollup();
      }
   }
}

void tparagraph :: checkscrolldown ( void )
{
   if ( next ) {
      if ( ps.line1num + linenum + 1 != next->ps.line1num ) {
         next->ps.line1num = ps.line1num + linenum + 1;
         next->display();
         next->checkscrolldown ();
      }
   } else {
      if ( ps.line1num + linenum < maxlinenum )
         bar ( winx1, winy1 + (ps.line1num + linenum + 1) * activefontsettings.height, winx1 + activefontsettings.length, winy1 + maxlinenum * activefontsettings.height, dblue );

   }
}


int  tparagraph :: reflow( int all  )
{
   int oldlinenum = linenum;

   pfont font = activefontsettings.font;
   linenum = 0;
   int pos = 0;
   linestart [ linenum ] = text;
   int length = 0;

   do {

     if ( font->character[int(text[pos])].size ) {
        length += font->character[int(text[pos])].width + 2 ;
        if ( pos )
           length += font->kerning[int(text[pos])][int(text[pos-1])];
     }

     if ( length > activefontsettings.length ) {
        int pos2 = pos;
        while ( ( text[pos2] != ' ' )  &&  ( &text[pos2] - linestart[linenum] )  )
           pos2--;

        if ( &text[pos2] == linestart[linenum] )
           pos2 = pos;
        else
           pos2++;

        linelength[linenum] = &text[pos2] - linestart[linenum];
        if ( linelength[linenum] > 200 ) {
           printf("\a");
        }
        linenum++;
        linestart[linenum] = &text[pos2];
        length = 0;
        pos = pos2;
     } else
        pos++;
   } while ( pos <= size ); /* enddo */

   linelength[linenum] = &text[pos] - linestart[linenum];
        if ( linelength[linenum] > 200 ) {
           printf("\a");
        }


   int oldlength = 0;
   for (int i = 0; i <= linenum; i++) {
      length = 0;
      for ( int j = 0; j < linelength[i]; j++ ) {
         pos = linestart[i] - text + j;

         if ( pos == cursor  && !searchcursorpos ) {
             cursorx = length;
             normcursorx = cursorx;
             cursory = i;
         }

         if ( searchcursorpos ) {
            if ( i == cursory )
               if ( length >= normcursorx ) {
                  cursorx = length;
                  cursor = pos;
                  searchcursorpos = 0;
               }
            if ( i > cursory ) {
               cursorx = oldlength;
               cursor = pos-1;
               searchcursorpos = 0;
            }
        }

        if ( font->character[int(text[pos])].size ) {
             length += font->character[int(text[pos])].width + 2 ;
             if ( pos )
                length += font->kerning[int(text[pos])][int(text[pos-1])];
        }

     }
     oldlength = length;
   } /* endfor */


   if ( searchcursorpos ) {
      cursorx = length;
      cursor = size-1;
      searchcursorpos = 0;
   }


   if ( all ) {
      if ( linenum != oldlinenum ) {
         display ( );
         if ( checkcursorpos() ) {
            checkscrollup();
            checkscrolldown();
         }
         return 0;
      }
   }
   return 1;

}


pparagraph tparagraph :: movecursor ( int dx, int dy )
{
   pparagraph newcursorpos = this;

   if ( cursorstat )
      displaycursor();

   if ( dx == -1 ) {
      if ( cursor > 0 )
         cursor--;
      else
         if ( prev ) {
            cursor = -1;
            newcursorpos = prev;
            prev->cursor = prev->size - 1;
         }
   }
   
   if ( dx == 1 ) {
      if ( cursor < size-1 )
         cursor++;
      else
         if ( next ) {
            cursor = -1;
            next->cursor = 0;
            newcursorpos = next;
         }
   }

   if ( dy == 1 ) {
      if ( cursory < linenum ) {
         cursory++;
         searchcursorpos++;
      } else
        if ( next ) {
           cursor = -1;
           next->cursor = 0;
           next->normcursorx = normcursorx;
           next->cursorx = normcursorx;
           next->cursory = 0;
           next->searchcursorpos = 1;
           newcursorpos = next;
        }
   }

   if ( dy == -1 ) {
      if ( cursory > 0 ) {
         cursory--;
         searchcursorpos++;
      } else
        if ( prev ) {
           cursor = -1;
           prev->cursor = 0;
           prev->normcursorx = normcursorx;
           prev->cursorx = normcursorx;
           prev->cursory = prev->linenum;
           prev->searchcursorpos = 1;
           newcursorpos = prev;
        }
   }

   newcursorpos->reflow( 0 );
   newcursorpos->checkcursorpos();
   newcursorpos->displaycursor();
   return newcursorpos;
}


void tparagraph :: displaycursor ( void )
{
   if ( cursor >= 0 ) {
      int starty;
      if ( ps.line1num < 0 ) {
         starty = winy1 + (ps.line1num + cursory) * activefontsettings.height;
         // startline = -ps.line1num;
      } else {
         starty = winy1 + (ps.line1num + cursory) * activefontsettings.height;
         // startline = 0;
      }

      if ( starty  < winy2 ) {
         collategraphicoperations cgo ( winx1 + cursorx-2,   starty, winx1 + cursorx+2,   starty + activefontsettings.height );
         xorline ( winx1 + cursorx,   starty+1, winx1 + cursorx,   starty + activefontsettings.height - 1, blue  );
         if ( cursorx )
           xorline ( winx1 + cursorx-1, starty+1, winx1 + cursorx-1, starty + activefontsettings.height - 1, blue  );
         cursorstat = !cursorstat;
      }
   }
}

void tparagraph :: setpos ( int x1, int y1, int y2, int linepos, int linenum  )
{
   winx1 = x1;
   winy1 = y1;
   winy2 = y2;
   ps.line1num = linepos;
   maxlinenum = linenum;
}

void tparagraph :: display ( void )
{
   if ( cursorstat )
      displaycursor();
   cursorstat = 0;

   int startline;
   int starty;
   if ( ps.line1num < 0 ) {
      starty = winy1;
      startline = -ps.line1num;
   } else {
      starty = winy1 + ps.line1num * activefontsettings.height;
      startline = 0;
   }

   while ( startline <= linenum  && starty < winy2 ) {
      char *c = &linestart[ startline ] [ linelength [ startline ]] ;
      char d = *c;
      *c = 0;

      showtext2 ( linestart[ startline ], winx1, starty );

      *c = d;
      starty += activefontsettings.height;
      startline++;
   } ; /* enddo */

   if ( cursor >= 0 )
      displaycursor();
}




tmessagedlg :: tmessagedlg ( void )
{
    firstparagraph = NULL;
    lastcursortick = 0;
    blinkspeed = 80;
}

void tmessagedlg :: inserttext ( const ASCString& txt )
{
   if ( txt.empty() )
      firstparagraph = new tparagraph;
   else {
      firstparagraph = NULL;
      actparagraph = NULL;
      int pos = 0;
      int start = 0;
      const char* c = txt.c_str();
      while ( c[pos] ) {
         int sz = 0;
         if ( strnicmp(&c[pos], "#CRT#" , 5 ) == 0 )
            sz = 5;
         if ( strnicmp(&c[pos], "\r\n" , 2 ) == 0 )
            sz = 2;
         if ( strnicmp(&c[pos], "\n" , 2 ) == 0 )
            sz = 1;
         if ( sz ) {
            ASCString s = txt;
            if ( start ) {
               s.erase ( 0, start );
               s.erase ( pos-start );
            } else
               s.erase ( pos );
            actparagraph = new tparagraph ( actparagraph );
            if ( !firstparagraph )
               firstparagraph = actparagraph;

            actparagraph->addtext ( s );
            pos+=sz;
            start = pos;
         } else
            pos++;
      } /* endwhile */

      if ( start < txt.length() ) {
         actparagraph = new tparagraph ( actparagraph );
         if ( !firstparagraph )
             firstparagraph = actparagraph;
         actparagraph->addtext ( &(txt.c_str()[start]) );
      }

   }
}

void tmessagedlg :: run ( void )
{
   tdialogbox::run ( );
   if ( prntkey != cto_invvalue ) {
      if ( prntkey == cto_bspace )
         actparagraph = actparagraph->erasechar ( 1 );
      else
      if ( taste == cto_entf )
         actparagraph->erasechar ( 2 );
      else
      if ( taste == cto_left )
         actparagraph = actparagraph->movecursor ( -1, 0 );
      else
      if ( taste == cto_right )
         actparagraph = actparagraph->movecursor ( 1, 0 );
      else
      if ( taste == cto_up )
         actparagraph = actparagraph->movecursor ( 0, -1 );
      else
      if ( taste == cto_down )
         actparagraph = actparagraph->movecursor ( 0, 1 );
      else
      if ( taste == cto_enter )
         actparagraph = actparagraph->cut ( );
      else
      if ( taste == cto_esc )
         printf("\a");
      else
      if ( prntkey > 31 && prntkey < 256 )
         actparagraph->addchar ( prntkey );
   }
   if ( lastcursortick + blinkspeed < ticker ) {
      actparagraph->displaycursor();
      lastcursortick = ticker;
   }
}

ASCString tmessagedlg :: extracttext ()
{
   tparagraph text;

   actparagraph = firstparagraph;
   while ( actparagraph ) {
      text.addtext ( actparagraph->text );
      text.addtext ( "#crt#" );
      actparagraph = actparagraph->next;
   }
   return text.text;

}

tmessagedlg :: ~tmessagedlg ( )
{
   actparagraph = firstparagraph;
   while ( actparagraph ) {
      pparagraph temp = actparagraph->next;
      delete actparagraph;
      actparagraph = temp;
   }
}

void tmessagedlg :: setup ( void )
{
   xsize = 500;
   ysize = 450;
   ok = 0;
   tx1 = 20;
   ty1 = starty + 10;
   tx2 = xsize - 40;
   ty2 = ysize - ty1 - 80;

   addbutton ( "~S~end", 10, ysize - 30, xsize / 2 - 5, ysize - 10 , 0, 1, 1, true );
   clearkey ( 1 );
   addkey ( 1, cto_stp + cto_s );

   addbutton ( "~C~ancel", xsize / 2 + 5, ysize - 30, xsize - 10, ysize - 10 , 0, 1, 2, true );
   clearkey ( 2 );
   addkey ( 2, cto_stp + cto_c );

   int num = 0;
   for ( int i = 0; i < 8; i++ ) {
      if ( actmap->player[i].exist() )
         if ( actmap->actplayer != i ) {
            int x = 20 + ( num % 2 ) * 200;
            int y = ty2 + 10 + ( num / 2 ) * 20;
            // this is a leak! As this dialog will be scrapped before the ASC2 release,  I don't care :->
            addbutton ( strdup( actmap->player[i].getName().c_str()), x, y, x+ 180, y+15, 3, 0, num+3, true );
            addeingabe ( num+3, &to[i], 0, dblue );
            num++;
         }
   }


}




void MultilineEdit :: init ( void )
{
   tdialogbox :: init ( );
   title = dlg_title.c_str();

   setup();
   buildgraphics();


   rectangle ( x1 + tx1 - 2, y1 + ty1 - 2, x1 + tx2 + 2, y1 + ty2 + 2 ,black );


   activefontsettings.font = schriften.smallarial;
   activefontsettings.length = tx2 - tx1 -1 ;
   activefontsettings.height = activefontsettings.font->height+5;
   activefontsettings.background = dblue;
   activefontsettings.color = black;
   activefontsettings.justify = lefttext;

   inserttext ( text.c_str() );
   actparagraph = firstparagraph;
   actparagraph->cursor = 0;
   actparagraph->setpos( x1 + tx1, y1 + ty1, y1 + ty2, 0, 13 );

   while ( actparagraph ) {
      actparagraph->reflow( 0 );
      actparagraph = actparagraph->next;
   }
   actparagraph = firstparagraph;
   actparagraph->checkscrolldown();
   actparagraph->display();


}


void MultilineEdit :: setup ( void )
{
   xsize = 500;
   ysize = 400;
   ok = 0;
   tx1 = 20;
   ty1 = starty + 10;
   tx2 = xsize - 40;
   ty2 = ysize - ty1 - 40;

   addbutton ( "~O~k", 10, ysize - 30, xsize / 2 - 5, ysize - 10 , 0, 1, 1, true );
   clearkey ( 1 );
   addkey ( 1, cto_stp + cto_o );

   addbutton ( "~C~ancel", xsize / 2 + 5, ysize - 30, xsize - 10, ysize - 10 , 0, 1, 2, true );
   clearkey ( 2 );
   addkey ( 2, cto_stp + cto_c );

}


void MultilineEdit :: buttonpressed ( int id )
{
   tmessagedlg :: buttonpressed ( id );
   if ( id == 1 )
      ok = 1;
   if ( id == 2 )
      ok = 2;
}


void MultilineEdit :: run ( void )
{
   mousevisible ( true );
   do {
      tmessagedlg::run ( );
   } while ( !ok );

   if ( ok == 1 ) {
      text = extracttext();
      textchanged =  true;
   }
}



void selectgraphicset ( void )
{
   ASCString filename = selectFile( "*.gfx", true );
   if ( !filename.empty() ) {
      int id = getGraphicSetIdFromFilename ( filename.c_str() );
      if ( id != actmap->graphicset ) {
         actmap->graphicset = id;
         displaymap();
      }
   }
}

int editInt( const ASCString& title, int defaultValue, int minValue, int maxValue )
{
   return getid( title.c_str(), defaultValue, minValue, maxValue );
}

void displayActionError( const ActionResult& result, const ASCString& additionalInfo  )
{
   ASCString s = getmessage(result.getCode());
   s += "\n" + result.getMessage();
   if ( !additionalInfo.empty() )
      s += "\n" + additionalInfo;
   
   errorMessage( s );  
}





