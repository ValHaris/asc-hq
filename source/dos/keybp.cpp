//     $Id: keybp.cpp,v 1.4 2000-10-18 15:10:07 mbickel Exp $
//
//     $Log: not supported by cvs2svn $
//     Revision 1.3  2000/10/16 14:34:12  mbickel
//      Win32 port is now running fine.
//      Removed MSVC project files and put them into a zip file in
//        asc/source/win32/msvc/
//
//     Revision 1.2  2000/06/23 09:48:33  mbickel
//      Improved key handling in intedit/stredit
//
//     Revision 1.1  2000/05/30 18:39:28  mbickel
//      Added support for multiple directories
//      Moved DOS specific files to a separate directory
//
//     Revision 1.9  2000/01/19 22:03:36  mbickel
//      Fixed a bug in the DOS keyboard code
//
//     Revision 1.8  2000/01/04 19:43:52  mbickel
//      Continued Linux port
//
//     Revision 1.7  2000/01/02 20:23:38  mbickel
//      Improved keyboard handling in dialog boxes under DOS
//
//     Revision 1.6  1999/12/30 21:04:45  mbickel
//      Restored DOS compatibility again.
//
//     Revision 1.5  1999/12/28 22:04:25  mbickel
//      Had to make some changes again to compile it for DOS...
//
//     Revision 1.4  1999/12/27 12:51:36  mbickel
//      Cleanup for Linux port
//
//     Revision 1.3  1999/11/16 17:04:07  mbickel
//     Made ASC compilable for DOS again :-)
//     Merged all the bug fixes in that I did last week
//
//     Revision 1.2  1999/11/16 03:41:51  tmwilson
//      Added CVS keywords to most of the files.
//      Started porting the code to Linux (ifdef'ing the DOS specific stuff)
//      Wrote replacement routines for kbhit/getch for Linux
//      Cleaned up parts of the code that gcc barfed on (char vs unsigned char)
//      Added autoconf/automake capabilities
//      Added files used by 'automake --gnu'
//
//
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

#include <dos.h>
#include <conio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include "../events.h"

    void             (__interrupt __far *bioskeyboardhandler)();
    void             (__interrupt __far *keybpointer)()=NULL;
    char             callbioshandler,keyboardinit=0;
    char             pufferpos,maxpufferkeys;
    // long             oldticker,ticker;
    char             normkey[256];
    int                puffer[256];
    char             keyreleasefrequence = 100;

    unsigned short int keyport = 0x60;
    unsigned char key;
    unsigned char pressed;
    unsigned char p;
    unsigned char exkey;
    
    int exitprogram = 0;



char  skeypress( tkey keynr)
{ 
   #ifdef NEWKEYB   
     return normkey[keynr];
   #else
      keynr = 0;
      return 0;
   #endif
} 



char *get_key(unsigned char keynr)
{ 
   switch (keynr) {
      
      case 1:   return "Esc"; 
      case 2:   return "1"; 
      case 3:   return "2"; 
      case 4:   return "3"; 
      case 5:   return "4"; 
      case 6:   return "5"; 
      case 7:   return "6"; 
      case 8:   return "7"; 
      case 9:   return "8"; 
      case 10:   return "9";  
      case 11:   return "0";  
      case 12:   return "·";  
      case 13:   return "`";  
      case 14:   return "Backspace";  
      case 15:   return "Tab";  
      case 16:   return "Q";  
      case 17:   return "W";  
      case 18:   return "E";  
      case 19:   return "R";  
      case 20:   return "T";  
      case 21:   return "Z";  
      case 22:   return "U";  
      case 23:   return "I";  
      case 24:   return "O";  
      case 25:   return "P";  
      case 26:   return "ö";  
      case 27:   return "+";  
      case 28:   return "Return";  
      case 29:   return "Left Control";  
      case 30:   return "A";  
      case 31:   return "S";  
      case 32:   return "D";  
      case 33:   return "F";  
      case 34:   return "G";  
      case 35:   return "H";  
      case 36:   return "J";  
      case 37:   return "K";  
      case 38:   return "L";  
      case 39:   return "ô";  
      case 40:   return "é";  
      case 41:   return "^";  
      case 42:   return "Left Shift";  
      case 43:   return "#";  
      case 44:   return "Y"; 
      case 45:   return "X";  
      case 46:   return "C";  
      case 47:   return "V";  
      case 48:   return "B";  
      case 49:   return "N";  
      case 50:   return "M";  
      case 51:   return ",";  
      case 52:   return ".";  
      case 53:   return "-";  
      case 54:   return "Right Shift";  
      case 55:   return "Keypad *"; 
      case 56:   return "Left Alt";  
      case 57:   return "Space";  
      case 58:   return "CapsLock";  
      case 59:   return "F1";  
      case 60:   return "F2";  
      case 61:   return "F3";  
      case 62:   return "F4";  
      case 63:   return "F5";  
      case 64:   return "F6";  
      case 65:   return "F7";  
      case 66:   return "F8";  
      case 67:   return "F9";  
      case 68:   return "F10";  
      case 69:   return "NumLock";  
      case 70:   return "Scroll Lock";  
      case 71:   return "Keypad 7";  
      case 72:   return "Keypad 8";  
      case 73:   return "Keypad 9";  
      case 74:   return "Keypad -";  
      case 75:   return "Keypad 4"; 
      case 76:   return "Keypad 5";  
      case 77:   return "Keypad 6";  
      case 78:   return "Keypad +";  
      case 79:   return "Keypad 1";  
      case 80:   return "Keypad 2";  
      case 81:   return "Keypad 3";  
      case 82:   return "Keypad 0";  
      case 83:   return "Keypad ."; 
      case 86:   return "<";  
      case 87:   return "F11";  
      case 88:   return "F12";  
      case 128:   return "Keypad Return";  
      case 129:   return "Right Control";  
      case 130:   return "Pause";  
      case 153:   return "Keypad /";  
      case 155:   return "Print Screen";  
      case 156:   return "Right Alt";  
      case 171:   return "Home";  
      case 172:   return "UpKey";  
      case 173:   return "Page Up";  
      case 175:   return "LeftKey";  
      case 177:   return "RightKey";  
      case 179:   return "End";  
      case 180:   return "DownKey";  
      case 181:   return "Page Down";  
      case 182:   return "Insert";  
      case 183:   return "Delete";  
      default:    return "";
   } 
} 


void clearpuffer(void)
{ char         i;

   for (i = 0; i <= maxpufferkeys; i++) puffer[i] = 0; 
   pufferpos = 0; 
} 

/*extern void getkeyinfo( void );
#pragma aux getkeyinfo =  \
      "push ax"                 \
      "push dx"                 \
                                    \
      "mov dx, 0x60"         \
      "in al,dx"                  \
      "mov key,al"             \
                                    \
      "cmp p,0"                 \
      "jnz PAUSE"               \
                                     \
      "cmp key,225"           \
      "jz PAUSE"                \
                                    \
      "cmp exkey,1"           \
      "jz EXTENDEDKEYS"     \                 
                                     \
      "cmp al,224"              \             
      "jnz NORMKEY"           \
                                     \
      "mov exkey,1"           \
      "mov key,0"              \
      "jmp ENDE"               \
                                    \
      "PAUSE :"                 \
      "cmp p,5"                 \
      "jz PAUSEENDE"         \
      "inc p"                     \
      "jmp KILLKEY"           \
                                    \
      "PAUSEENDE :"           \
      "mov p,0"                 \
      "mov exkey,1"           \
      "mov key,30"             \
      "mov pressed,1"         \
      "jmp ENDE"                \
                                     \
      "EXTENDEDKEYS :"       \
      "cmp key,42"              \
      "jz KILLKEY"               \
                                      \
      "cmp key,128"             \
      "jnc RELEASE"             \
      "mov pressed,1"          \
      "jmp ENDE"                 \
                                       \
      "NORMKEY :"                \
      "cmp key,128"             \
      "jnc RELEASE"             \
      "mov pressed,1"          \
      "jmp ENDE"                 \
                                      \
      "RELEASE :"                \
      "sub key,128"              \
      "mov pressed,0"           \
      "jmp ENDE"                  \
                                       \
      "KILLKEY :"                  \
      "mov key,0"                 \
      "mov ExKey,0"              \
      "jmp ENDE"                   \
                                        \
      "ENDE :"                       \
      "mov dx,0x20"               \
      "mov al,0x20"               \
      "out dx,al"                    \
      "pop dx"                       \
      "pop ax"                       \
      modify [ax dx];*/

void keyintr( void )
{
   key = inp(keyport);

   if ( (p == 0) && (key != 225 ) )  { // keine Pause
      if (exkey == 1) {
         if (key == 42) {
            key = 0;
            exkey = 0;
            //outp(0x20,0x20);
            return;
         }
         if (key >= 128 ) {
            key -= 128;
            pressed = 0;
            //outp(0x20,0x20);
            return;
         } else {
            pressed = 1;
            //outp(0x20,0x20);
            return;
         } /* endif */
      } else {
         if (key == 224) {
            exkey = 1;
            key = 0;
            //outp(0x20,0x20);
            return;
         } else {
            if (key >= 128 ) {
               key -= 128;
               pressed = 0;
               //outp(0x20,0x20);
               return;
            } else {
               pressed = 1;
               //outp(0x20,0x20);
               return;
            } /* endif */
         } /* endif */
      } /* endif */
   } else {
      if (p < 5) {
         p++;
         key = 0;
         exkey = 0;
         //outp(0x20,0x20);
         return;
      } else {
         p = 0;
         exkey = 1;
         key = 30;
         pressed = 1;
         //outp(0x20,0x20);
         return;
      } /* endif */
   } /* endif */
}
static void __interrupt __far keyboard()
{ 
   int           ekey;
   char          ende;

   _disable();

   keyintr();

   if (key > 0 ) {
      ekey = 0;
      normkey[130] = 0;   /* Pause ausschalten */
      ekey = key; 
      if ((skeypress(42) || skeypress(54)) && (key != 42) && (key != 54)) ekey = key + 256;   /* Shift */ 
      if ((skeypress(29) || skeypress(129)) && (key != 29) && (key != 129)) ekey = key + 512;   /* Strg */ 
      if ((skeypress(56) || skeypress(156)) && (key != 56) && (key != 156)) ekey = key + 1024;   /* Alt */ 
      if (exkey > 0) {
         exkey = 0; 
         if (key <= 83) { 
            if (pressed == 0) normkey[key + 100] = 0;
            else
               {
               normkey[key + 100] = 1;
               if (pufferpos <= maxpufferkeys - 1) { 
                  puffer[pufferpos] = ekey + 100; 
                  pufferpos++;
               } 
            } 
         } 
      } 
      else if (key <= 88) { 
         if (pressed == 0 ) normkey[key] = 0;
         else {
            normkey[key] = 1;
            if (pufferpos <= maxpufferkeys) { 
               puffer[pufferpos] = ekey; 
               pufferpos++;
            } 
         } 
      } 
      //_chain_intr(keybpointer);
      //_chain_intr(bioskeyboardhandler); // alten HÑndler aufrufen
      //outp(0x20,0x20);
      ende = 0;
      /*if ( keyreleasefrequence >= 0 ) do {
        if ((100 / keyreleasefrequence)+oldticker<=ticker) {
         oldticker=ticker;
         ende=1;
        }
     } while (ende == 0);*/
   } 
   outp(0x20,0x20);
   _enable();
} 


void shleft()

{ int  i; 

   for (i = 0; i <= 255; i++) {
      puffer[i] = puffer[i + 1];
      if (puffer[i] == 0) break;
   } 
   puffer[255] = 0;
} 

int rp_key ( void )
{
   int cch = getch();
   if ( cch == 0 ) 
      cch = getch() + 256;

   return cch;
}


tkey r_key(void)

{   
 
#ifdef NEWKEYB

   int            key;

   if (keyboardinit == 1) 
      { 
      key = 0; 
      do { 
           key = puffer[0];
          }  while (key == 0);
      shleft(); 
      if (pufferpos > 0 ) pufferpos--;
      while ( kbhit ()  ) { // vom alten HÑndler Taste holen damit kein PufferÅberlauf
         int cch = getch();
      } /* endwhile */
      return key;
   } else 
#endif
   {

      int  cch;
      cch = getch();
      if ( cch == 0 ) 
         cch = getch() + 256;

      return cch;
   }
}  


int keypress(void)

{ 

   #ifdef NEWKEYB   
   if (keyboardinit == 1) { 
      return puffer[0];
   } 
   else 
      #endif 
             {  
               if (  kbhit() )
                  return 1;
               else
                  return 0;
              }
} 

void initkeyb(void)
{ 
   #ifdef NEWKEYB   

   if (keyboardinit == 0) {
      exkey = 0; 
      p = 0; 
      pufferpos = 0; 
      maxpufferkeys = 128; 
      keyreleasefrequence = 10; 
      //for (int i =0; i <= 255; i++) normkey[i] = 0;
      bioskeyboardhandler = _dos_getvect(0x9);
      _dos_setvect(0x9,keyboard);
      keyboardinit = 1; 
      for ( int i = 0; i< 256; i++ )
         normkey[i] = 0;
   } 
   #endif   
} 


void getkeysyms ( tkey* keysym, int* keyprnt )
{
   if (keyboardinit == 0) {
      tkey k = r_key();
     #ifdef NEW_KEYBB
      *keysym = ct_invvalue;
     #else
      *keysym = k;
     #endif
      *keyprnt = k;
   } else {
      *keysym = r_key();
      *keyprnt = *keysym;
   }
}


void  closekeyb(void)
{ 
   #ifdef NEWKEYB   
   if (keyboardinit == 1) { 
      _dos_setvect(0x9,bioskeyboardhandler);
      keyboardinit = 0; 
      for ( int i = 0; i< 256; i++ )
         normkey[i] = 0;
   } 
   #endif   
} 


void wait(void)
{ 
//   tkey   cha; 

   //   do {
   //      cha = r_key();
      //     } while (kbhit() != 0);
   r_key(); 
} 


tkey char2key (int ch)
   {
   #ifdef NEWKEYB   
   switch (toupper ( ch ) ) {
      case 'A':   return 30; 
      case 'B':   return 48;  
      case 'C':   return 46; 
      case 'D':   return 32; 
      case 'E':   return 18; 
      case 'F':   return 33; 
      case 'G':   return 34; 
      case 'H':   return 35; 
      case 'I':   return 23; 
      case 'J':   return 36; 
      case 'K':   return 37; 
      case 'L':   return 38; 
      case 'M':   return 50; 
      case 'N':   return 49; 
      case 'O':   return 24; 
      case 'P':   return 25; 
      case 'Q':   return 16; 
      case 'R':   return 19; 
      case 'S':   return 31; 
      case 'T':   return 20; 
      case 'U':   return 22; 
      case 'V':   return 47; 
      case 'W':   return 17; 
      case 'X':   return 45; 
      case 'Y':   return 44; 
      case 'Z':   return 21; 
      case ' ':   return 57; 
      case '#':   return 43; 
      case '-':   return 53; 
      case '+':   return 27; 
      case '.':   return 52; 
      case ',':   return 51; 
      case '0':   return 11; 
      case '1':   return 2; 
      case '2':   return 3; 
      case '3':   return 4; 
      case '4':   return 5; 
      case '5':   return 6; 
      case '6':   return 7; 
      case '7':   return 8; 
      case '8':   return 9; 
      case '9':   return 10; 
      case 'î':   return 39; 
      case 'Ñ':   return 40; 
      case 'Å':   return 26; 
      case '·':   return 12; 
      case '*':   return 55; 
      case 27:  return ct_esc;
      default:    return 0; 
   } 
   #else   
     return ch;
   #endif   
} 


extern void inittimer ( int freq );
extern void closetimer ();

extern int initmousehandler ( void* pic );
extern void removemousehandler ();

void initializeEventHandling ( int (*gamethread)(void *) , void *data, void* mousepointer )
{
   mouseparams.xsize = 10;
   mouseparams.ysize = 10;

   initkeyb();
   atexit(closekeyb);

   inittimer ( 100);
   atexit ( closetimer );

   initmousehandler ( mousepointer );
   atexit ( removemousehandler );
   
   gamethread( data );

}


