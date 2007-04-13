/*
    This file is part of Advanced Strategic Command; http://www.asc-hq.de
    Copyright (C) 1994-1999  Martin Bickel  and  Alexander SchÑfer

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

#ifndef krkr_h
#define krkr_h


#include "stdlib.h"
#include "i86.h"
#include "stdio.h"
#include "conio.h"
#include "ctype.h"
#include "graph.h"
#include "string.h"
#include "stdlib.h"
#include "dos.h"
#include "..\tpascal.inc"
#include "..\typen.h"
//typedef char* pchar;

#define YES 1
#define NO  0
#define clearscreen() (_clearscreen (_GCLEARSCREEN))
#define path(x)  (_fullpath (x, "", 255))



typedef     struct {
                      char  x,y;
                   } tcurpos   ;

typedef     struct {
                      char              name[13];
                      unsigned short    date;
                      unsigned short    time;
                      long              size;
                      FILE              *fp;
                   } tfile;



   void        _wait (void);
   void        charcat (char s[], char c);

   void        num_ed (long &old, long min, long max);
   void        num_ed (unsigned short &old, long min, long max);
   void        num_ed (char &old, long min, long max);
   void        num_ed  ( int &sel, long min,  long max);

   void        unpack_date (unsigned short packed, int &day, int &month, int &year);
   void        unpack_time (unsigned short packed, int &sec, int &min, int &hour);


   void        stredit (pchar &string, char maxlength, char xpos, char ypos);    // fÅr dynamische-strings
   void        stredit2 (char  *string, char maxlength, char xpos, char ypos);   // fÅr statische Strings

   void        bitselect (long &bits, const char *selection[], char bitnum);
   void        bitselect (unsigned short &bits, const char *selection[], char bitnum);
   void        bitselect (char &bits, const char *selection[], char bitnum);
   void        bitselect (int  &bits, const char *selection[], char bitnum);
   void        bitselect ( tterrainbits &bits, const char *selection[], char bitnum);

   void        yn_switch (char *s1, char *s2, char v1, char v2, char &oldvalue);
   void        fileselect (char *searchstr, unsigned attributes, tfile &file);




class    cstredit {
   protected :
      char     *string, maxlength;     // string und seine max. LÑnge
      tcurpos  textpos;                // Anfangspos von string
      char     pos;                    // Position des Cursors
      char     status;                 // Text einfuegen=1 / Åberschreiben=-1
      char     **oldstring;            // speichert den alten string ab
      void     addchar (char c);
      void     delchar (char leftright);
      void     overwrite (char c);
      void     changecursor (void);
      void     setoldstr (void);
   public :
      void     init (char **_string, char _maxlength, char x, char y);
      void     edit (void);
      void     done ();
} ;
 

class    cbitselect {
   protected :
      tterrainbits     bits, *oldbits;   // Speichervariable fÅr bits
      char     quantity;         // Anzahl der mîglichen Auswahlen
      tcurpos  position;         // position der 1. reihe
      char     sel;              // position des Auswahlcursors
      void     setpos (char updown);
      void     changebit (void);
      void     cbitselect :: setoldbits (void);
   public :
      void     init (tterrainbits  &_bits, char *selection[], char bitnum);
      void     select (void);
      void     done ();
} ;


class    cyn_switch {
   protected :
      tcurpos  position;               // Position der Knîpfe
      char     sel;                    // angewÑhlter knopf
      char     *oldvalue;              // Zeiger auf die zu verÑndernde Variable
      struct   {
                  char  value;         // wert des knopfes
                  char  string[15];    // label des knopfes
               } button [1];
      void     paintbutton (char button);
   public :
      void     init (char *s1, char *s2, char v1, char v2, char &_oldvalue);
      void     select (void);
      void     done (void);
} ;


class    cfileselect {
   protected :
      tfile          *file;                  // Dateien (Name, Datum, Grîsse)
      long           quantity;               // Anzahl der gefundenen Dateien
      char           sel, maxsel;            // Position der Markierung (0-19)
      long           position, maxposition;  // nummer der obersten datei
      char           sortstatus;             // speichert oob nach name oder datum sortiert ist
      void     search_files (char *searchstr, unsigned attributes);
      void     displayallfiles (void);
      void     getinfostr (char *displaystr, long filenum);
      void     bar (char onoff);
      void     sort (char option);
   public :
      void     init (char *searchstr, unsigned attributes);
      void     select (void);
      void     done (tfile &selected_file);
} ;

extern void settxt50mode ( void );
extern void terrainaccess_ed ( TerrainAccess* ft, char* name );


#endif
