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


#include <stdarg.h>
#include <string>
#include "krkr.h"
#include "..\basestrm.h"


#define write(a) cprintf(a)
#define left 0
#define right 1
#define up 1
#define down -1
#define reset 0
#define insert 1
#define on 1
#define off 0
#define sortname 1
#define sorttime 2
#define max_long 2147483647

#define fgcolor 15
#define bkcolor 1



/*********************************************************************************************/
/* setcurpos   :  setzt den Cursor an die Position x y                                       */
/* getcurpos   :  liefert die Cursorposition                                                 */
/*********************************************************************************************/
      extern void setcurpos( int col, int row );
      #pragma aux setcurpos =         \
              "push bp"               \ 
              "shl edx,8"             \
              "or edx, ebx"           \
              "mov ah,2"              \ 
              "mov bx,0"              \
              "int 10h"               \ 
              "pop bp"                \ 
              parm   [ebx] [edx]      \
              modify [ax ebx];

      extern int biosgetcurpos( void );
      #pragma aux biosgetcurpos =     \
              "push bp"               \ 
              "mov ah,3"              \
              "mov bx,0"              \
              "int 10h"               \ 
              "movzx eax, dx"         \
              "pop bp"                \ 
              modify [bx];



tcurpos   getcurpos (void)
{
   tcurpos  pos;
   int a = biosgetcurpos();
   pos.y = (a >> 8);
   pos.x = (a & 0xff);
   return pos;
}



/*********************************************************************************************/
/* Wait  :  wartet auf einen Tastendruck                                                     */
/*********************************************************************************************/
void         _wait(void)
{ 
   while ( kbhit() ) getch();
   getch();
   while ( kbhit() ) getch();
} 



/*********************************************************************************************/
/* charcat  :  hÑngt einen Buchstaben an einen string an                                     */
/*********************************************************************************************/
void        charcat (char s[], char c)
{
   int   i = 0;

   while (s[i] != 0) {
      i++;
   } /* endwhile */
   s[i] = c;
   s[i+1] = 0;
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




/*********************************************************************************************/
/* num_ed   :  Editor fÅr Zahlen                                                             */
/*********************************************************************************************/
void         num_ed  ( long &sel, long min, long max)
{ 
   tcurpos   pos;     // Pos des Eingabefieldes

   if (min>max) {
      int i;
      i = min;
      min = max;
      max = i;
   } /* endif */
   printf ("\n");
   cprintf (" (%i  <=  x  <=  %i)   :   ", min, max);
   pos = getcurpos ();
   _settextposition (pos.y+1, pos.x+1);
   _settextcolor (fgcolor);
   _setbkcolor (bkcolor);
   _outtext ("            ");
   _settextcolor (7);
   _setbkcolor (0);
   setcurpos (pos.x, pos.y);
   cprintf ("%11i", sel);

   do {
      int   input;
         if ((sel<min) || (sel>max)) {
            if (sel <min) sel = min;
            if (sel >max) sel = max;
            setcurpos (pos.x, pos.y);
            cprintf ("          ");
            setcurpos (pos.x, pos.y);
            cprintf ("%11i", sel);
         } /* endif */
      do {

         input = getch ();

         if (isdigit(input) && (-214748363<=sel) && (sel<=214748363)) {
            if (sel>=0) {
               sel = sel*10 + (input-48);
            } else {
               sel = sel*10 - (input-48);
            } /* endif */
            setcurpos (pos.x, pos.y);
            cprintf ("%11i", sel);
         } /* endif */

         if ((input==8) && (sel!=0)) {
            sel /= 10;
            setcurpos (pos.x, pos.y);
            cprintf ("          ");
            setcurpos (pos.x, pos.y);
            cprintf ("%11i", sel);
         } /* endif */
 
         if ((input==27) || (sel==0)) {
            sel = 0;
            setcurpos (pos.x, pos.y);
            cprintf ("%11i", sel);
         } /* endif */

         if ((input=='-') && (sel!=0) && (min<0)) {
            sel *= -1;
            setcurpos (pos.x, pos.y);
            cprintf ("          ");
            setcurpos (pos.x, pos.y);
            cprintf ("%11i", sel);
         } /* endif */

      } while (input!=13); /* enddo */

   } while ((sel<min) || (sel>max)); /* enddo */

   printf("\n");
} 


void         num_ed  ( unsigned short &sel, long min, long max)
{
   long tmp = sel;
   num_ed ( tmp, min, max );
   sel = tmp;
}

void         num_ed  ( char &sel, long min, long max)
{
   long tmp = sel;
   num_ed ( tmp, min, max );
   sel = tmp;
}

void         num_ed  ( int &sel, long min,  long max)
{
   long tmp = sel;
   num_ed ( tmp, min, max );
   sel = tmp;
}




/*********************************************************************************************/
/* stredit  :  Editor fÅr dynamische strings (ruft die Funktionen von class cstredit auf)    */
/*********************************************************************************************/
void           stredit (pchar     &string, char  maxlength, char xpos, char ypos)
{
   cstredit    se;
 
   se.init (&string, maxlength, xpos, ypos);
   se.edit ();
   se.done ();
}


 
/*********************************************************************************************/
/* stredit2  :  Editor fÅr statische strings (ruft die Funktionen von class cstredit auf)    */
/*********************************************************************************************/
void           stredit2 (char     *string, char  maxlength, char xpos, char ypos)
{
   char *c = strdup ( string );
   cstredit    se;
 
   se.init (&c, maxlength, xpos, ypos);
   se.edit ();
   se.done ();

   strcpy ( string, c );
   free (c);
}
 


/*********************************************************************************************/
/* bitselect   :  Bit-Editor  (ruft die Funktionen von class cbitselect auf)                 */
/*********************************************************************************************/
void           bitselect (long   &bits, const char *selection[], char bitnum)
{
   cbitselect  bs;
   tterrainbits bts ( bits );

   bs.init (bts, (char**) selection, bitnum);
   bs.select ();
   bs.done ();
   bits = bts.terrain1;
}
 
void           bitselect ( unsigned short  &bits, const char *selection[], char bitnum)
{
   cbitselect  bs;
   tterrainbits bts ( bits );

/*   char  _selection[50][50];                             // ??????????????????
   for (int i=0; *selection[i]!=NULL; i++) {
      strcpy(_selection[i] , selection[i] );
   }*/
 
   bs.init (bts, (char**) selection, bitnum);
   bs.select ();
   bs.done ();

   bits = bts.terrain1;
}

void           bitselect (char   &bits, const char *selection[], char bitnum)
{
   cbitselect  bs;
   tterrainbits bts ( bits );

   bs.init (bts, (char**) selection, bitnum);
   bs.select ();
   bs.done ();

   bits = bts.terrain1;
}

void           bitselect (int   &bits, const char *selection[], char bitnum)
{
   cbitselect  bs;
   tterrainbits bts ( bits );

/*   char  _selection[50][50];                             // ??????????????????
   for (int i=0; *selection[i]!=NULL; i++) {
      strcpy(_selection[i] , selection[i] );
   }
*/
   bs.init (bts, (char**) selection, bitnum);
   bs.select ();
   bs.done ();

   bits = bts.terrain1 ;
}

void           bitselect ( tterrainbits &bits, const char *selection[], char bitnum)
{
   cbitselect  bs;

   bs.init (bits, (char**) selection, bitnum);
   bs.select ();
   bs.done ();

}


/*********************************************************************************************/
/* yn_switch   :  schalter  (ruft die Funktionen von class cyn_switch auf)                   */
/*********************************************************************************************/
void           yn_switch (char *s1, char *s2, char v1, char v2, char &oldvalue)
{
   fflush ( stdout );
   cyn_switch  sw;
 
   sw.init (s1, s2, v1, v2, oldvalue);
   sw.select ();
   sw.done ();
}



/*********************************************************************************************/
/* fileselect :  dateimenÅ  (ruft Funktionen von class cfileselect auf)                     */
/*********************************************************************************************/
void        fileselect (char  *searchstr, unsigned attributes, tfile &file)
{
   cfileselect fs;
 
   fs.init (searchstr, attributes);
   fs.select ();
   fs.done (file);
}




/*********************************************************************************************/
/* class cstredit :  Editor fÅr strings                                                      */
/*********************************************************************************************/
void     cstredit :: init (char **_string, char _maxlength, char x, char y)
{
   string = (char*) malloc ( _maxlength );
   oldstring = _string ;

   if ( *_string != NULL ) {
      strcpy ( string , *_string );
   } else {
      string[0]=0;
      *oldstring = (char*) malloc ( 1 ) ;
      (*oldstring)[0]=0;
   }

   maxlength = _maxlength;
   if ((x==255) || (y==255)) {
      printf ("\n         ");
      textpos = getcurpos ();
   } else {
      textpos.x = x;
      textpos.y = y;
   } /* endif */
   pos = strlen (string);
   status = insert;

   _settextcolor (fgcolor);
   _setbkcolor (bkcolor);
   _settextposition (textpos.y+1, textpos.x+1);
   for (int i=0; i<=maxlength; i++) {
      _outtext(" ");
   } /* endfor */
   setcurpos (textpos.x, textpos.y);
   cprintf (string);
}


/*void     cstredit :: showstr ()
{
   setcurpos (textpos.x, textpos.y);
   cprintf ("%s", string);
   setcurpos (textpos.x+pos, textpos.y);
}*/


void     cstredit :: addchar (char c)
{
   int   i=0;
   char  s[255];
   strcpy (s, string);
   string[0] = 0;

   while (i<pos) {
      charcat (string, s[i]);
      i++;
   } /* endwhile */

   charcat (string, c);
   setcurpos (textpos.x + pos, textpos.y);
   putch (c);

   while ((i<=maxlength) && (i<=strlen (s))) {
      charcat (string, s[i]);
      putch (s[i]);
      i++;
   } /* endwhile */

   pos++;
   setcurpos (textpos.x + pos, textpos.y);
}


void     cstredit :: delchar (char leftright)
{
   int   i=0;
   char  s[255];
   strcpy (s, string);
   string[0] = 0;
   pos += leftright-1;
 
   while (i<pos) {
      charcat (string, s[i]);
      i++;
   } /* endwhile */
   i++;
   setcurpos(textpos.x + pos, textpos.y);
   while ((i<=maxlength) && (i<=strlen (s))) {
      charcat (string, s[i]);
      putch (s[i]);
      i++;
   } /* endwhile */

   putch (' ');
   setcurpos (textpos.x + pos, textpos.y);
}
 

void     cstredit :: overwrite (char c)
{
   if (string[pos]==0) {
      string[pos+1] = 0;
   } /* endif */
   string[pos] = c;
   putch (c);
   pos++;
}


void     cstredit :: changecursor (void)
{
   status *= -1;
 
   if (status==insert) {
      _settextcursor (0x0607);            // normal underline cursor
   } else {
      _settextcursor (0x0007);            // full block cursor
   } /* endif */
}


void     cstredit :: setoldstr (void)
{
   strcpy (string, *oldstring);
   pos = strlen (string);

   setcurpos (textpos.x, textpos.y);
   for (int i=0; i<maxlength; i++) {
      putch (' ');
   } /* endfor */

   setcurpos (textpos.x, textpos.y);
   cprintf (string);
}


void     cstredit :: edit (void)
{
   char  input;
   do {
      input = getch ();
 
      if ((input!=8) && (input!=0) && (input!=27) && (input!=255) && (input != 13)) {
         if ((status == insert) && (strlen(string)<maxlength))  addchar (input);
         if ((status != insert) && (pos<maxlength))  overwrite (input);
      } /* endif */

      if ((input==8) && (pos>0)) {
         delchar (left);
      } /* endif */

      if ((input==27) && (strcmp (string, *oldstring)!=0)) {
         setoldstr ();
      } /* endif */

      if (input==255) {
         putch (7);
      } /* endif */

      if (input==0) {

         input = getch ();

         if ((input==75) && (pos>0))  pos--;
         if ((input==77) && (pos<strlen(string)))  pos++;
         if (input==71)  pos=0;
         if (input==79)  pos=strlen(string);
         setcurpos (textpos.x+pos, textpos.y);

         if (input==82) {
            changecursor ();
         } /* endif */
         if ((input==83) && (pos<strlen(string))) {
            delchar (right);
         } /* endif */

      } /* endif */

   } while (input != 13); /* enddo */
}


void     cstredit :: done ()
{
   free (*oldstring);
   *oldstring = string;

   int length=strlen (string);
   setcurpos (textpos.x+length, textpos.y);
   printf("\n");
   _settextcolor (7);
   _setbkcolor (0);
   _settextcursor (0x0607);            // normal underline cursor
};





/*********************************************************************************************/
/* class cbitselect  :  Bit-Editor                                                           */
/*********************************************************************************************/
void     cbitselect :: init (tterrainbits &_bits, char *selection[], char bitnum)
{
   int i = 0;
   
   oldbits = &_bits;
   bits = _bits;
   sel = 0;
   quantity = bitnum;
 
   for (i=0; i<quantity+4; i++)    printf ("\n");
   position = getcurpos ();
   position.y -= quantity;

   for (i=0; i<quantity; i++) {
      _settextposition (position.y+i+1, position.x+5);
      _settextcolor (fgcolor);
      _setbkcolor (bkcolor);

      tterrainbits bts;
      if ( i < 32 )
         bts.set ( 1 << i, 0 );
      else
         bts.set ( 0, 1 << ( i - 32));

      if (bits & bts) {
         _outtext ("[X]");
      } else {
         _outtext ("[ ]");
      } /* endif */

      setcurpos (position.x+7, position.y+i);
      cprintf (" ƒƒ %s", selection[i]);
   } /* endfor */

   _settextcursor (0x0007);            // full block cursor

   setpos (reset);
}
 

void     cbitselect :: setpos (char updown)
{
   switch (updown) {
   case up:
      if (sel>0) {
         sel--;
      } else {
         sel=quantity-1;
      } /* endif */
      break;
   case down:
      if (sel<quantity-1) {
         sel++;
      } else {
         sel=0;
      } /* endif */
      break;
   } /* endswitch */

   setcurpos (position.x+5, position.y+sel);
}


void     cbitselect :: changebit (void)
{
   tterrainbits bts;
   int i = sel;
   if ( i < 32 )
      bts.set ( 1 << i, 0 );
   else
      bts.set ( 0, 1 << ( i - 32));

   if (bits & bts) {
      bits = bits ^ bts;
      cprintf (" ");
   } else {
      bits = bits | bts;
      cprintf ("X");
   } /* endif */
   setpos (reset);
}


void     cbitselect :: setoldbits (void)
{
//   if (bits != *oldbits) {
      bits = *oldbits;
      for (int i=0; i<quantity; i++) {
         setcurpos (position.x+5, position.y+i);
         tterrainbits bts;
         if ( i < 32 )
            bts.set ( 1 << i, 0 );
         else
            bts.set ( 0, 1 << ( i - 32));
   
         if (bits & bts) {
            _outtext ("[X]");
         } else {
            _outtext ("[ ]");
         } /* endif */
      } /* endfor */
//   } /* endif */
   setcurpos (position.x+5, position.y+sel);

}


void     cbitselect :: select (void)
{
   char  input;
   do {
      input = getch ();
 
      if (input==32)  changebit ();
      if (input==27)  setoldbits ();

      if (input==0) {
         input = getch ();
 
         if (input==72)  setpos (up);
         if (input==80)  setpos (down);
      } /* endif */
   } while (input != 13); /* enddo */
}


void     cbitselect :: done ()
{
   setcurpos (0, position.y+quantity);
   _settextcursor (0x0607);            // normal underline cursor
   _settextcolor (7);
   _setbkcolor (0);
   *oldbits = bits;
}




/*********************************************************************************************/
/* class cyn_switch  :  YES - NO Schalter                                                    */
/*********************************************************************************************/
void     cyn_switch :: init (char *s1, char *s2, char v1, char v2, char &_oldvalue)
{
   cprintf ("\n\n");
   position = getcurpos ();

   if (s1[0] != 0) {                               
      strncpy (button[0].string, s1, 13);
      button[0].string[13] = 0;
   } else {
      strcpy (button[0].string, " YES ");
   } /* endif */
   if (s2[0] != 0) {
      strncpy (button[1].string, s2, 13);
      button[1].string[13] = 0;
   } else {
      strcpy (button[1].string, " NO ");
   } /* endif */

   button[0].value = v1;
   button[1].value = v2;
   if (v1==v2) {
      button[0].value++;
   } /* endif */

   oldvalue = &_oldvalue;
   if (_oldvalue==v1) {
      sel = left;
   } else {
      sel = right;
   } /* endif */

   paintbutton (left);
   paintbutton (right);

   _settextcursor (0x2000);            // cursor off
}


void     cyn_switch :: paintbutton (char pbutton)
{
   if (pbutton==sel) {
      _settextcolor(fgcolor);
      _setbkcolor(bkcolor);
   } else {
      _settextcolor(8);
      _setbkcolor(0);
   } /* endif */

   if (pbutton==left) {
      _settextposition (position.y, 10);
      _outtext (button[0].string);
   } else {
      _settextposition (position.y, 35);
      _outtext (button[1].string);
   } /* endif */
}


void     cyn_switch :: select (void)
{
   char  input;
   do {
      input = getch ();
 
      if (input == 0) {
         input = getch();
 
         if ((input==75) && (sel != left)) {
            sel=left;
            paintbutton (left);
            paintbutton (right);
         } /* endif */
         if ((input==77) && (sel != right)) {
            sel=right;
            paintbutton (left);
            paintbutton (right);
         } /* endif */
      } /* endif */
   } while (input != 13); /* enddo */
}


void     cyn_switch :: done (void)
{
   _settextcolor(7);
   _setbkcolor(0);
   _settextcursor (0x0607);            // normal underline cursor
   setcurpos (79, position.y);
   cprintf ("\n");
   *oldvalue = button[sel].value;
}




#define num_odf 45       // Anzahl der angezeigten dateien  (20 bei 25 zeilen-modus)

/*********************************************************************************************/
/* class cfileselect :  dateimenÅ                                                            */
/*********************************************************************************************/
void     cfileselect :: init (char  *searchstr, unsigned attributes)
{
   search_files (searchstr, attributes);

   if (quantity>0) {
      position = 0;
      sel = 0;
      if (quantity>=num_odf) {
         maxposition = quantity-num_odf;
         maxsel = num_odf-1;
      } else {
         maxposition = 0;
         maxsel = quantity -1;
      } /* endif */

      clearscreen ();
      sort (sortname);
      setcurpos (0,0);
      printf (" ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ\n");
      printf ("    filename           size         date         time \n");
      printf (" ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ\n");
      displayallfiles ();
      printf (" ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ\n");
      cprintf ("  %3i files  of  %-12s ", quantity, searchstr);
      _settextcursor (0x2000);            // cursor off

      bar (on);
   } /* endif */
};


void     cfileselect :: search_files (char  *searchstr, unsigned attributes)
{ 
   quantity = 0;
   {   // count

      tfindfile ff ( searchstr );
      string filename = ff.getnextname();
   
      while( !filename.empty() ) {
        quantity++;
        filename = ff.getnextname();
      }
   }


   
   if (quantity>0) {

      int i=0;

      file = new tfile[quantity];

      tfindfile ff ( searchstr );
      string  filename = ff.getnextname();
   
      while( !filename.empty() ) {

         strcpy (file[i].name, filename.c_str() );

         /*
         file[i].size = fileinfo.size;
         file[i].date = fileinfo.wr_date;
         file[i].time = fileinfo.wr_time;
         */

         file[i].size = 0;
         file[i].date = 0;
         file[i].time = 0;

         i++;

         filename = ff.getnextname();
      }
   } /* endif */
} 
 
   
void     cfileselect :: displayallfiles (void)
{
   char  string[100];
   setcurpos (0,3);
   for (long i=position; i<position+num_odf; i++) {
      if (i<quantity) {
         getinfostr (string, i);
         printf (" %s", string);
      } else {
         printf ("\n");
      } /* endif */
   } /* endfor */
}


void     cfileselect :: getinfostr (char   *displaystr, long   filenum)
{
   if (filenum<quantity) {
      int   day, month, year;
      int   min, hour, sec;
      unpack_date (file[filenum].date, day, month, year);
      unpack_time (file[filenum].time, sec, min, hour);
 
      char drive[_MAX_DRIVE], dir[_MAX_DIR], fname[_MAX_FNAME], ext[_MAX_EXT];
      _splitpath (file[filenum].name, drive, dir, fname, ext );

      sprintf (displaystr, " %-8s%-4s    %10ld     %2i.%2i.%4i     %2i:%2i \n", fname, ext, file[filenum].size, day, month, year, hour,min );
   } else {
      strcpy (displaystr, "");
   } /* endif */
}


void     cfileselect :: bar (char onoff)
{
   if (onoff==on) {
      _settextcolor (fgcolor);
      _setbkcolor (bkcolor);
   } else {
      _settextcolor (7);
      _setbkcolor (0);
   } /* endif */

   char  string[80];
   getinfostr (string, sel+position);
   _settextposition (4+sel, 2);
   _outtext (string);
}


void     cfileselect :: sort (char  option)
{
   tfile *sortfile = (tfile*) malloc (sizeof(tfile)*quantity);
   char  lastname[13] = "";
   char  foundname[13] = "˛˛˛˛˛˛˛˛˛˛˛˛";
   long  lastdate = max_long;
   long  founddate = 0;

   for (long _new=0; _new<quantity; _new++) {

      long   found;
      for (long searcher=0; searcher<quantity; searcher++) {
         int   cond1, cond2;
         if (option==sortname) {
            cond1 = strcmp (file[searcher].name, lastname);
            cond2 = strcmp (foundname, file[searcher].name);
         } else {
            cond1 = lastdate >= file[searcher].date * 100000 + file[searcher].time;
            cond2 =  founddate <= file[searcher].date * 100000 + file[searcher].time;
         } /* endif */
 
         if ((cond1 == 1) && (cond2 == 1)) {
            found = searcher;
            strcpy (foundname, file[found].name);
            founddate = file[searcher].date * 100000 + file[searcher].time;
         } /* endif */

      } /* endfor */

      strcpy (sortfile[_new].name, file[found].name);
      sortfile[_new].size = file[found].size;
      sortfile[_new].date = file[found].date;
      sortfile[_new].time = file[found].time;

      strcpy (file[found].name, "");           // dateiinfos werden "entwertet" 
      file[found].date = 65535;              // dateiinfos werden "entwertet"
 
      strcpy (lastname, foundname);
      strcpy (foundname, "˛˛˛˛˛˛˛˛˛˛˛˛");
      lastdate = founddate;
      founddate = 0;
   } /* endfor */         

   free (file);
   file = sortfile;

   _settextcolor (fgcolor);
   _setbkcolor (0);
   if (option==sortname) {
      _settextposition (2,5);
      _outtext ("filename");
      _settextcolor (7);
      _settextposition (2,37);
      _outtext ("date");
      sortstatus = sortname;
   } else {
      _settextposition (2,37);
      _outtext ("date");
      _settextcolor (7);
      _settextposition (2,5);
      _outtext ("filename");
      sortstatus = sorttime;
   } /* endif */
}


void     cfileselect :: select (void)
{
   if (quantity>0) {
      char  input;
      do {
         input = getch ();

         if ((toupper(input) == 'N') && (sortstatus != sortname)) {           // left key
            sort (sortname);
            displayallfiles ();
         } /* endif */

         if ((toupper(input) == 'D') && (sortstatus != sorttime)) {           // right key
            sort (sorttime);
            displayallfiles ();
         } /* endif */

         if (input==0) {
            input = getch ();
 
            if (input==72) {                                         // up key
               if (sel>0) {
                  bar (off);
                  sel--;
                  bar (on);
               } else {
                  if (position>0) {
                     position--;
                     displayallfiles ();
                  } /* endif */
               } /* endif */
            } /* endif */

            if (input==80) {                                         // down key
               if (sel<maxsel) {
                  bar (off);
                  sel++;
                  bar (on);
               } else {
                  if (position<maxposition) {
                     position++;
                     displayallfiles ();
                  } /* endif */
               } /* endif */
            } /* endif */
 
            if (input==71) {                                         // pos1
               if (sel != 0) {
                  bar (off);
                  sel=0;
                  bar (on);
               } /* endif */
               if (position>0) {
                  position=0;
                  displayallfiles ();
               } /* endif */
            } /* endif */
 
            if (input==79) {                                         //ende
               if (sel != maxsel) {
                  bar (off);
                  sel=maxsel;
                  bar (on);
               } /* endif */
               if (position != maxposition) {
                  position = maxposition;
                  displayallfiles ();
               } /* endif */
            } /* endif */

            if (input==73) {                                         //Bild up
               if (position>0) {
                  if (position>=num_odf) {
                     position -= num_odf;
                  } else {
                     position = 0;
                  } /* endif */
                  displayallfiles ();
               } else {
                  if (sel>0) {
                     bar (off);
                     sel=0;
                     bar (on);
                  } /* endif */
               } /* endif */
            } /* endif */

            if (input==81) {                                         //Bild down
               if (position<maxposition) {
                  if (position<=maxposition-num_odf) {
                     position += num_odf;
                  } else {
                     position = maxposition;
                  } /* endif */
                  displayallfiles ();
               } else {
                  if (sel<maxsel) {
                     bar (off);
                     sel=maxsel;
                     bar (on);
                  } /* endif */
               } /* endif */
            } /* endif */

            if ((input==75) && (sortstatus != sortname)) {           // left key
               sort (sortname);
               displayallfiles ();
            } /* endif */

            if ((input==77) && (sortstatus != sorttime)) {           // right key
               sort (sorttime);
               displayallfiles ();
            } /* endif */

         } /* endif    input == 0 */

      } while (input != 13); /* enddo */

   } else {

      printf ("\n cfileselect - Error : no files found !  \n");
      putch (7);
      char  terminate=1;
      yn_switch (" CONTINUE ", " TERMINATE ", 0,1, terminate);
      if (terminate==1)  exit (1);

   } /* endif */
}


void     cfileselect :: done (tfile &selected_file)
{
   if (quantity>0) {
      strcpy (selected_file.name, file[position+sel].name);
      selected_file.size = file[position+sel].size;
      selected_file.date = file[position+sel].date;
      selected_file.time = file[position+sel].time;

      free (file);
      _settextcolor (7);
      _setbkcolor (0);
      _settextcursor (0x0607);            // normal underline cursor
      clearscreen ();
   } else {
      strcpy (selected_file.name, "");
      selected_file.size = 0;
      selected_file.date = 0;
      selected_file.time = 0;
   } /* endif */

}




/*
//------------------------------------------------------------------------------

void    settxtmode33;

char    z12 [255,11];
char    z14 [255,13];
char    a;

var az12:z12;pz14:^z14;a:byte;r: registers ;pz12:pointer;
{
     r.ax:=$1130;
     r.bh:=2;
     intr($10,r);
     pz14:=ptr(r.es,r.bp);
     for a:=0 to 255 do move(pz14^[a,1],az12[a,0],12);
     pz12:=@az12;
     asm
        push    bp
        mov     ax,$1110
        mov     bx,$0C00
        mov     cx,256
        mov     dx,0
        les     bp,pz12
        int     $10
        pop     bp
     end;
};
//------------------------------------------------------------------------------
*/






















/*

boolean      AnhangTest(char *       s,
                        Byte         i)
{ 
   int          * untyped file * F1; 
   Byte         Counter; 
   Char         c; 
   int      size; 
   Integer      i2; 

   return false; 
   size = 0; 
   assign(F1,paramstr(0)); 
   reset(&F1,1); 
   for (i2 = 1; i2 <= i; i2++) { 
      Counter = 0; 
      do { 
         BlockRead(F1,c,1); 
         if (c == 'ù') inc(Counter); 
         else Counter = 0; 
         inc(size); 
      }  while (!((Counter == 10) || (size > FileSize(F1)))); 
      if (Counter == 10) 
         return true; 
      else { 
         return false; 
         fclose(F1); 
         return;
      } 
   } 
   fclose(F1); 
} 


void         Anhaengen(char *       s,
                       Byte         Pos)
{ 
   int          * untyped file / F1, F2, F3; 
   Pointer      p; 
   Word         NumRead, NumWritten; 

   if (AnhangTest(s,Pos) == false) { 
      GetMem(p,500); 
      assign(F3,paramstr(0)); 
      assign(F2,s); 
      assign(F1,paramstr(0)); 
      Rename(F3,"*.tmp"); 
      reset(&F2); 
      rewrite(&F1); 
      do { 
         BlockRead(F3,*p,500,NumRead); 
         BlockWrite(F1,*p,NumRead,NumWritten); 
      }  while (!((NumRead == 0) || (NumWritten != NumRead))); 
      fclose(F3); 
      sbld(s,"ùùùùùùùùùù%s",s); 
      BlockWrite(F1,s,22); 
      do { 
         BlockRead(F2,*p,500,NumRead); 
         BlockWrite(F1,*p,NumRead,NumWritten); 
      }  while (!((NumRead == 0) || (NumWritten != NumRead))); 
   } 
} 



   */



void settxt50mode ( void )
{
   /*
ah = 11
al = 12
bl = 0
int 10
     */


   REGS regs;

   regs.w.ax = 0x3;
   int386 ( 0x10, &regs, &regs );


   regs.w.ax = 0x1112;
   regs.w.bx = 0x0;
   int386 ( 0x10, &regs, &regs );


   /*
   regs.w.ax = 0x1202;
   regs.w.bx = 0x30;
   int386 ( 0x10, &regs, &regs );

   regs.w.ax = 0x03;
   int386 ( 0x10, &regs, &regs );

   regs.w.ax = 0x1112;
   int386 ( 0x10, &regs, &regs );
   */
/*   initsvga ( 0x108 ); */
}


void terrainaccess_ed ( pterrainaccess ft, char* name  )
{
      printf ("\n    Terrain the %s can drive on / be build on \n"
              "           (for many %ss it is enough to specify only this field): \n", name, name );
      bitselect (ft->terrain, cbodenarten, cbodenartennum);

      printf ("\n    ALL these selected terrain-bits are necessary for the %s to drive / be build there : \n", name);
      bitselect (ft->terrainreq, cbodenarten, cbodenartennum);

      printf ("\n    NONE of these bits must be set for the %s to drive / be build there\n"
              "          if you are creating a vehicle:  \n"
              "           all ships (except icebreaker) must have the ice/snow bits set here) \n", name);
      bitselect (ft->terrainnot, cbodenarten, cbodenartennum);

      printf ("\n    the %s dies on ... ( only if it cannot enter, see enntries above): \n"
              "           most land based %ss should have the water-bits set here \n", name, name);
      bitselect (ft->terrainkill, cbodenarten, cbodenartennum);
}

#if 0
void fatalError ( const char* formatstring, ... )
{
   va_list paramlist;
   va_start ( paramlist, formatstring );

   char tempbuf[1000];

   int lng = vsprintf( tempbuf, formatstring, paramlist );
   va_end ( paramlist );
   fprintf(stderr, tempbuf );
   exit(1);
}
#endif
