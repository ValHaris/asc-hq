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

#ifndef global_h
 #define global_h

 #ifdef __WATCOM_CPLUSPLUS__
  #define HAVE_STRICMP 
  #define HAVE_KBHIT
  #define HAVE_ITOA
 #endif

 #ifndef HAVE_ITOA
 #define itoa(a, b, c) sprintf(b, "%##c##d", a);
 #endif



 #ifndef HAVE_STRICMP
 #define stricmp strcasecmp
 #define strcmpi strcasecmp
 #define strnicmp strncasecmp
 #define strncmpi strncasecmp
 
 char *strupr (const char *a)
 {
   int i;
   int j;
   char *b;
 
   j = strlen (a);
   b = (char *) malloc (j);
   for (i = 0; i < j; i++)
     b[i] = toupper (a[i]);
   return (b);
 }
 #endif


 #ifndef HAVE_KBHIT
 extern int kbhit (void);
 extern int getch (void);
 void set_keypress (void);
 void reset_keypress (void);
 #endif

#endif
