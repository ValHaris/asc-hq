/*! \file pwd_dlg.cpp
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

#include "../paradialog.h"
#include "../gameoptions.h"
#include "../password.h"

 class PasswordDialog : public ASC_PG_Dialog {
               Password& password;
               PG_LineEdit* line1;
               PG_LineEdit* line2;
               
               bool ok()
               {
                  if ( firstTime ) {
                     assert( line2 );
                     if ( line1->GetText() != line2->GetText() )
                        return false;
                     else {
                        password.setUnencoded( line1->GetText() );
                        success = true;
                        QuitModal();
                        return true;
                     }
                  } else {
                     Password p2;
                     p2.setUnencoded( line1->GetText() );
                     static bool dbg = true;
                     if ( p2 != password && dbg )
                        return false;
                     else {
                        success = true;
                        QuitModal();
                        return true;
                     }
                  }
               }
               
               bool cancel()
               {
                  QuitModal();
                  return true;
               }

               bool def()
               {
                  if ( CGameOptions::Instance()->getDefaultPassword().empty() ) {
                     warningMessage ( "no default password setup!" );
                     return false;
                  }

                  password = CGameOptions::Instance()->getDefaultPassword();
                  success = true;
                  QuitModal();
                  return true;
               }

               static const int border  = 20;

               int buttonNum;
               PG_Button* addButton( const ASCString& label, int totalNum )
               {
                  int width = (Width() - (totalNum+1)*border) / totalNum;
                  return new PG_Button( this, PG_Rect( border + buttonNum++ * (width + border), Height()-40, width , 30 ), label );
               }

               bool success;
               
               bool line1completed()
               {
                  if ( firstTime && line2 )
                     line2->EditBegin();
                  else
                     ok();
                  return true;
               }
               
            protected:
               bool firstTime;
               bool cancelAllowed;
               bool defaultAllowed;
            public:
               PasswordDialog ( Password& crc, bool _firstTime, bool _cancelAllowed, bool _defaultAllowed, const ASCString& username ) : ASC_PG_Dialog( NULL, PG_Rect( -1, -1, 300, 190), "Enter Password"),
                     password ( crc ), buttonNum(0), success(false), firstTime ( _firstTime ), cancelAllowed ( _cancelAllowed ), defaultAllowed ( _defaultAllowed )
               {
                  
                  if ( username.length() )
                     new PG_Label( this, PG_Rect( border, 25, Width() - 2 * border, 20 ), "Player: " + username );
                  
                  line1 = new PG_LineEdit( this, PG_Rect( border, 50, Width() - 2 * border, 20));
                  line1->SetPassHidden('*');
                  line1->sigEditReturn.connect( sigc::mem_fun( *this, &PasswordDialog::line1completed ));

                  if ( firstTime ) {
                     line2 = new PG_LineEdit( this, PG_Rect( border, 80, Width() - 2 * border, 20));
                     line2->SetPassHidden('*');
                     line2->sigEditReturn.connect( sigc::mem_fun( *this, &PasswordDialog::ok ));
                  } else {
                     line2 = NULL;
                  }

                  int bnum;
                  if ( firstTime && defaultAllowed && cancelAllowed )
                     bnum = 3;
                  else
                     bnum = 2;

                  
                  addButton ( "~O~k", bnum ) -> sigClick.connect( sigc::hide( sigc::mem_fun( *this, &PasswordDialog::ok )));
                  
                  if ( firstTime && defaultAllowed ) {
                     addButton ( "~D~efault", bnum ) -> sigClick.connect( sigc::hide( sigc::mem_fun( *this, &PasswordDialog::def )));
                     if ( cancelAllowed ) {
                        addButton ( "~C~ancel", bnum ) -> sigClick.connect( sigc::hide( sigc::mem_fun( *this, &PasswordDialog::cancel )));
                     }
                  } else {
                     if ( cancelAllowed ) {
                        addButton ( "~C~ancel", bnum ) -> sigClick.connect( sigc::hide( sigc::mem_fun( *this, &PasswordDialog::cancel )));
                     } else {
                        addButton ( "~A~bort", bnum ) -> sigClick.connect( sigc::hide( sigc::mem_fun( *this, &PasswordDialog::cancel )));
                     }
                  }
               };

               bool getSuccess()
               {
                  return success;
               }

               int RunModal()
               {
                  line1->EditBegin();
                  return ASC_PG_Dialog::RunModal();
               }
           };


bool enterpassword ( Password& pwd, bool firstTime, bool cancelAllowed, bool defaultAllowed, const ASCString& username )
{
   Password def = CGameOptions::Instance()->getDefaultPassword();

   if ( !pwd.empty() && !def.empty() && pwd==def && !firstTime )
      return true;

   PasswordDialog pwod ( pwd, firstTime, cancelAllowed, defaultAllowed, username );
   pwod.Show();
   pwod.RunModal();
   return pwod.getSuccess();
}
