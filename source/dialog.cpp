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


int  choice_dlg(const ASCString& title, const ASCString& leftButton, const ASCString& rightButton )
{
  return new_choice_dlg( title, leftButton, rightButton );
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

        new PG_Label( this, PG_Rect( 25, 220, 200, 20), "positive values: enlarge map");
        new PG_Label( this, PG_Rect( 25, 245, 200, 20), "negative values: shrink map");

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



class TipDialog : public ASC_PG_Dialog {
   PG_CheckButton* dismissed;
   const ASCString& key;

   bool ok() {
      if ( dismissed->GetPressed())
         CGameOptions::Instance()->tipsDismissed.push_back(key);
      QuitModal();
      return true;
   }

public:
   TipDialog(const ASCString& text, const ASCString& key) : ASC_PG_Dialog(NULL, PG_Rect(-1,-1, 400, 300), "Tip"), key(key) {
      new TextRenderer(this, PG_Rect(10, 30, Width()-20, Height() - 80), text);
      dismissed = new PG_CheckButton(this, PG_Rect(10, Height()-60, 200, 20), "Don't show again");
      AddStandardButton("OK")->sigClick.connect( sigc::hide( sigc::mem_fun( *this, &TipDialog::ok)));
   }
};

void showTipDialog(const ASCString& text, const ASCString& key) {

   vector<ASCString>& dt = CGameOptions::Instance()->tipsDismissed;
   if ( std::find( dt.begin(), dt.end(), key ) != dt.end() )
      return;

   TipDialog td(text, key);
   td.Show();
   td.RunModal();
}




