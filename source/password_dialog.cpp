/*! \file password_dialog.cpp
    \brief Dialog for entering and asking for passwords
*/

/***************************************************************************
                          password_dialog.cpp  -  description
                             -------------------
    begin                : Mon Nov 27 2000
    copyright            : (C) 2000 by Martin Bickel
    email                : bickel@asc-hq.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <string>

#include "typen.h"
#include "spfst.h"
#include "dlg_box.h"
#include "gameoptions.h"

 class tenterpassword : public tdialogbox {
               Password& password;
             protected:
               bool firstTime;
               bool cancelAllowed;
               bool defaultAllowed;
               char strng1[40];
               char strng2[40];

               int status;
               int confirm;

               void dispeditstring ( char* st , int   x1, int   y1 );

               void lne(int          x1, int          y1, char *       s, int          position, char      einfuegen);

               virtual int    gettextwdth_stredit ( char* txt, pfont font );
            public:
               int   getcapabilities ( void );
               void  init ( char* title = NULL );
               void  run ( bool* result );
               void  buttonpressed ( int id );
               tenterpassword ( Password& crc, bool _firstTime, bool _cancelAllowed, bool _defaultAllowed ) : password ( crc ), firstTime ( _firstTime ), cancelAllowed ( _cancelAllowed ), defaultAllowed ( _defaultAllowed ) {};
           };

int   tenterpassword :: gettextwdth_stredit ( char* txt, pfont font )
{
  char* ss2 = strdup ( txt );

  int i = 0;
  while ( ss2[i] )
     ss2[i++] = '*';

  ss2 [ i ] = 0;

  i = gettextwdth(ss2, font);
  asc_free ( ss2 );

  return i;
}


int   tenterpassword :: getcapabilities ( void )
{
   return 0;
}


void         tenterpassword ::lne(int          x1,
                 int          y1,
                 char *       s,
                 int          position,
                 char      einfuegen)
{
  char* ss2 = strdup ( s );

  int i = 0;
  while ( ss2[i] )
     ss2[i++] = '*';

  ss2 [ position ] = 0;
  i = x1 + gettextwdth(ss2,activefontsettings.font);
  int j = y1;
  int k = y1 + activefontsettings.font->height;
  collategraphicoperations cgo ( i-1, j, i+1, k );
  xorline(i,j,i,k,3);
  if (einfuegen == false) {
     xorline(i + 1,j,i + 1,k,3);
     xorline(i - 1,j,i - 1,k,3);
  }
  asc_free ( ss2 );
}

void tenterpassword :: dispeditstring ( char* st, int x1, int y1 )
{
   char* ss2 = strdup ( st );

   int i = 0;
   while ( ss2[i] )
     ss2[i++] = '*';

   showtext2(ss2,x1,y1);

  asc_free ( ss2 );
}

/*
int tenterpassword :: checkforreask ( int crc )
{
   if ( crc == 0 || actmap->network && actmap->network->globalparams.reaskpasswords )
      return 1;
   else
      return 0;
}
*/

void tenterpassword :: init ( char* _title  )
{
   tdialogbox::init();
   xsize = 200;
   if ( !firstTime )
     ysize = 150;
   else
     ysize = 180;

   y1 = -1;
   x1 = -1;

   windowstyle ^= dlg_in3d;

   bool reask = firstTime;

   if ( firstTime && defaultAllowed ) {
      if ( cancelAllowed ) {
         addbutton ( "~O~k", 10, ysize - 35, xsize / 3 - 5, ysize - 10, 0, 1, 1, reask );
         addbutton ( "~D~efault", xsize / 3 + 5,   ysize - 35, 2 * xsize / 3 - 5, ysize - 10, 0, 1,7, !CGameOptions::Instance()->getDefaultPassword().empty() );
         addbutton ( "~C~ancel", 2 * xsize / 3 + 5, ysize - 35, xsize - 10, ysize - 10, 0, 1, 6, true );
      } else {
         addbutton ( "~O~k", 10, ysize - 35, xsize / 2 - 5, ysize - 10, 0, 1, 1, reask );
         addbutton ( "~D~efault", xsize / 2 + 5, ysize - 35, xsize - 10, ysize - 10, 0, 1,7, !CGameOptions::Instance()->getDefaultPassword().empty() );
      }
   } else {
      if ( cancelAllowed ) {
         addbutton ( "~O~k", 10, ysize - 35, xsize / 2 - 5, ysize - 10, 0, 1, 1, reask );
         addbutton ( "~C~ancel", xsize / 2 + 5, ysize - 35, xsize - 10, ysize - 10, 0, 1, 6, true );
      } else {
         addbutton ( "~O~k", 10, ysize - 35, xsize / 2 - 5, ysize - 10, 0, 1, 1, reask );
         addbutton ( "e~x~it", xsize / 2 + 5, ysize - 35, xsize - 10, ysize - 10, 0, 1,8, true );
      }
   }

   addkey ( 1, ct_enter );
   addkey ( 1, ct_enterk );


   if ( !_title )
      title = "enter password";
   else
      title = _title;


   addbutton ( "~p~assword:", 10, 50, xsize - 10, 75, 1, 0, 2, true );
   addeingabe ( 2, strng1, 0, 39 );

   if ( firstTime ) {
      addbutton ( "confirmation:", 10, 95, xsize - 10, 120, 1, 0, 3, true );
      addeingabe ( 3, strng2, 0, 39 );
      confirm = 1;
   } else
      confirm = 0;

   strng1[0] = 0;
   strng2[0] = 0;
   status = 0;

   buildgraphics();
}


void tenterpassword :: buttonpressed ( int id )
{
   tdialogbox::buttonpressed ( id );
   if ( id == 1 )   // OK
      status = 1;
   else
      if ( id == 6 )  // cancel
         status = 10;
      else
         if ( id == 7 ) {   // default
            password =CGameOptions::Instance()->getDefaultPassword();
            status = 2;
         } else
            if ( id == 8 ) {    // exit
               delete actmap;
               actmap = NULL;
               throw NoMapLoaded();
            } else
               if ( !firstTime ) {
                  Password pwd;
                  pwd.setUnencoded ( strng1 );
                  if ( pwd == password )
                     enablebutton ( 1 );
                  else
                     disablebutton ( 1 );
               } else
                  if ( strcmp ( strng1, strng2 ) == 0)
                     enablebutton ( 1 );
                  else
                     disablebutton ( 1 );
}

void tenterpassword :: run ( bool* result )
{
   tdialogbox::run ();

   pbutton pb = getbutton ( 2 );

   if ( pb )
      execbutton( pb , false );

   if ( !mouseparams.taste ) {
      if ( strng1[0] ) {
         pb = getbutton ( 3 );

         if ( pb )
            execbutton( pb , false );
      }

      pbutton pb2 = getbutton ( 1 );
      if ( pb2->active )
         execbutton ( pb2, false );
   }

   mousevisible ( true );
   while ( status == 0 ) {
      tdialogbox::run ();
   };

   if ( firstTime )
      if ( status != 2 )  // default not pressed
          password.setUnencoded ( strng1 );
      else
          password = CGameOptions::Instance()->getDefaultPassword();

   if ( result )
      if ( status == 10 ) // cancel pressed
         *result = false;
      else
         *result = true;

}


bool enterpassword ( Password& pwd, bool firstTime, bool cancelAllowed, bool defaultAllowed )
{
   Password def = CGameOptions::Instance()->getDefaultPassword();

   if ( !pwd.empty() && !def.empty() && pwd==def && !firstTime )
      return true;

   bool stat;
   tenterpassword epw ( pwd, firstTime, cancelAllowed, defaultAllowed );
   epw.init ( );
   epw.run ( &stat);
   epw.done ();

   return stat;
}
