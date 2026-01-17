/*! \file dlg_box.cpp
    \brief Some basic classes from which all of ASC's dialogs are derived
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
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <cstring>
#include <iostream>
#include <vector>

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

#include "spfst-legacy.h"


#ifdef _WIN32_
 #include <windows.h>
 #include <winuser.h>
#endif


#include <pgpropertyeditor.h>
#include <pgpropertyfield_integer.h>


char strrstring[200];



char*  strrrd8d(int  l)
{ 
 itoa ( l / minmalq, strrstring, 10);

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
   buf = malloc( size );
}

tvirtualscreenbuf:: ~tvirtualscreenbuf ()
{
   free( buf );
   buf = NULL;
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



// num   0: Box bleibt aufgeklappt,
//       1 box wird geschlossen , text rot (Fehler),
//       2 : Programm wird beendet;
//       3 : normaler text ( OK)

void displaymessage( const char* formatstring, int num, ... )
{

   va_list paramlist;
   va_start ( paramlist, num );

   char tempbuf[1000];

   int lng = vsnprintf( tempbuf, sizeof(tempbuf), formatstring, paramlist );
   if ( lng < 0 || (size_t)lng >= sizeof(tempbuf) ) {
      // Truncated or formatting error; ensure null termination and warn
      tempbuf[sizeof(tempbuf) - 1] = '\0';
      displaymessage ( "dlg_box.cpp / displaymessage:   string too long or formatting error!\nPlease report this error", 1 );
   }

   va_end ( paramlist );

   displaymessage ( ASCString ( tempbuf ), num  );
}

void displaymessage( const ASCString& text, int num  )
{
   std::vector<ASCString> lines;
   ASCString current;

   for (size_t idx = 0; idx < text.size(); ++idx) {
      char ch = text[idx];
      if (ch == '\n') {
         lines.push_back(current);
         current.clear();
      } else {
         current.push_back(ch);
      }
   }
   lines.push_back(current);


   bool displayInternally = true;

   /*
   if ( num == 2 )
      displayLogMessage ( 0, "fatal error" + text + "\n" );
   else
      displayLogMessage ( 0, text + "\n" );
      */


   #ifndef NoStdio
   if ( num == 2 )
      displayInternally = false;
   #endif


   if ( num == 2 )
      displayLogMessage(1, text );



   if ( !displayInternally ) {
      for (size_t i = 0; i < lines.size(); ++i)
          fprintf(stderr,"%s\n", lines[i].c_str());
      fflush( stderr );
   } else {
      #ifdef _WIN322_
        if ( !gameStartupComplete && num==2 ) {
           MessageBox(NULL, text.c_str(), "Fatal Error", MB_ICONERROR | MB_OK | MB_TASKMODAL );
           exit(1);
        }
      #endif

     MessagingHub::Instance().warning( text );
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






class ViewTextQuery : public ASC_PG_Dialog {

   public:
      ViewTextQuery( int id, const ASCString& title, const ASCString& button1, const ASCString& button2 ) : ASC_PG_Dialog( NULL, PG_Rect(-1, -1, 450, 500 ), title )
      {
         PG_Rect r;
         if ( button2.length() ) {
            r = PG_Rect( 10, Height() - 40, Width()/2-15, 30 );
            PG_Button* b = new PG_Button( this, PG_Rect( Width()/2+5, Height() - 40, Width()/2-15, 30 ), button2 );
            b->sigClick.connect( sigc::bind( sigc::mem_fun( *this, &ViewTextQuery::quitModalLoopW ), 1));
            b->activateHotkey(0);
         } else 
            r = PG_Rect( 10, Height() - 40, Width() - 20, 30 );
         


        PG_Button* b = new PG_Button( this, r, button1 );
        b->sigClick.connect( sigc::bind( sigc::mem_fun( *this, &ViewTextQuery::quitModalLoopW ), 0));
        b->activateHotkey(0);

        new TextRenderer( this, PG_Rect( 10, 30, Width()-20, Height() - 70 ), readtextmessage( id ));
      }
};


int         viewtextquery( int          id,
                           const char *       title,
                           const char *       s1,
                           const char *       s2)
{ 
  ViewTextQuery vtq( id, title, s1, s2 );
  vtq.Show();
  return vtq.RunModal();
} 



class GetInt : public  ASC_PG_Dialog {
    int value;
    int originalvalue;

    int minvalue;
    int maxvalue;

    ASC_PropertyEditor* propertyEditor;

    bool cancel() {
        if ( onCancel == ReturnZero )
            value = 0;
        else
            value = originalvalue;
        QuitModal();
        return true;
    }

    bool ok() {
        propertyEditor->Apply();
        if ( value < minvalue || value > maxvalue) {
            warningMessage(ASCString("value must be between ") + ASCString::toString(minvalue) + " and " + ASCString::toString(maxvalue));
            return false;
        }
        sigValueSet(value);
        QuitModal();
        return true;
    }
public:
    sigc::signal<void,int> sigValueSet;

    int getValue() { return value; };
    enum CancelMode { ReturnZero, ReturnOriginal } onCancel;

    GetInt(const ASCString& title, const ASCString& name, int original, int minvalue, int maxvalue, CancelMode onCancel = ReturnZero )
    : ASC_PG_Dialog(NULL, PG_Rect(-1, -1, 300, 150), title), value(original), originalvalue(original), minvalue(minvalue), maxvalue(maxvalue), onCancel(onCancel) {

        propertyEditor = new ASC_PropertyEditor( this, PG_Rect( 10, GetTitlebarHeight(), Width() - 20, Height() - GetTitlebarHeight() - 50 ), "PropertyEditor", name.empty()? 10 : 70 );

        (new PG_PropertyField_Integer<int>( propertyEditor, name, &value ))->SetRange(minvalue, maxvalue);

        AddStandardButton("Cancel")->sigClick.connect( sigc::hide( sigc::mem_fun( *this, &GetInt::cancel )));
        AddStandardButton("OK")->sigClick.connect( sigc::hide( sigc::mem_fun( *this, &GetInt::ok)));
    }
};



int      getid( const ASCString& title, int lval,int min,int max, const ASCString& valueName)
{
    GetInt editor(title, valueName, lval, min, max, GetInt::ReturnOriginal );
    editor.Show();
    editor.RunModal();
    return editor.getValue();
}




int chooseString ( const ASCString& title, const vector<ASCString>& entries, int defaultEntry  )
{
   vector<ASCString> b;
   b.push_back ( "~O~K");
   return chooseString ( title, entries, b, defaultEntry).second;
}




pair<int,int> chooseString ( const ASCString& title, const vector<ASCString>& entries, const vector<ASCString>& buttons, int defaultEntry  )
{

  return new_chooseString ( title, entries, buttons, defaultEntry );
}



class StringEditDialog : public ASC_PG_Dialog {
    PG_LineEdit* text;
    bool apply() {
        QuitModal();
        return true;
    }

public:
    StringEditDialog (const ASCString& title, const ASCString& defaultValue ) : ASC_PG_Dialog(NULL, PG_Rect(-1, -1, 300, 140), title) {
        text = new PG_LineEdit(this, PG_Rect(10, 30, Width()-20, 20));
        text->SetText(defaultValue);
        StandardButtonDirection(Horizontal);
        AddStandardButton("OK")->sigClick.connect( sigc::hide( sigc::mem_fun( *this, &StringEditDialog::apply )));
    }

    ASCString getValue() {
        return text->GetText();
    }
};


ASCString editString( const ASCString& title, const ASCString& defaultValue  )
{
    StringEditDialog sed(title, defaultValue);
    sed.Show();
    sed.RunModal();
    return sed.getValue();
}

