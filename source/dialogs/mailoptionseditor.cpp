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

#include <boost/regex.hpp>

#include "../paradialog.h"
#include "../widgets/dropdownselector.h"
#include "../widgets/textrenderer.h"
#include "../gameoptions.h"
#include "../dlg_box.h"

class MailOptionsDialog : public ASC_PG_Dialog {
      typedef map<ASCString,ASCString> ClientSetups;
      ClientSetups clientSetups;
      map<int,ASCString> clientSetupIndex;
      
      PG_LineEdit* command;
      DropDownSelector* dds;
      
      bool cannedConfigSelected( ) {
         int index = dds->GetSelectedItemIndex();
         if ( clientSetupIndex.find(index) != clientSetupIndex.end() ) {
            command->SetText( clientSetupIndex[index]);
            return true;
         } else
            return false;
      }
      
      bool ok() {
         CGameOptions::Instance()->mailProgram = command->GetText();
         CGameOptions::Instance()->setChanged();
         QuitModal();
         return true;
      }
      
      bool cancel() {
         QuitModal();
         return true;
      }
      
      
   public:
      MailOptionsDialog() : ASC_PG_Dialog( NULL, PG_Rect( -1, -1, 600, 350), "Email Options")
      {
         StandardButtonDirection( Horizontal );
         if ( exist("email.clients")) {
            tnfilestream stream ( "email.clients", tnstream::reading);
            bool finished = false;
            while ( !finished) {
               ASCString line;
               finished = !stream.readTextString(line, false);
               
               if ( line.find('#') != 0 ) {
                  // this is not a comment
               
                  boost::smatch what;
                  static boost::regex parser( "([^:]+):\\s*(\\S.*)");
                  if( boost::regex_match( line, what, parser)) {
                     ASCString client;
                     client.assign( what[1].first, what[1].second );

                     ASCString command;
                     command.assign( what[2].first, what[2].second );
                     
                     clientSetups[client] = command;
                  }
               }
            }
         }
         
         if ( clientSetups.size() > 0 ) {
            dds = new DropDownSelector( this, PG_Rect( 10, 30, Width() / 2 , 20));
            dds->AddItem("-- default configurations --");
            int counter = 1;
            for ( ClientSetups::iterator i = clientSetups.begin(); i != clientSetups.end(); ++i) {
               dds->AddItem(i->first);
               clientSetupIndex[counter++] = i->second;
            }
            
            PG_Button* apply = new PG_Button( this, PG_Rect( Width()/2 + 20, 30, Width()/2-30, 20),"Apply");
            apply->sigClick.connect( SigC::slot( *this, &MailOptionsDialog::cannedConfigSelected));
            
           
            // dds->selectionSignal.connect( SigC::slot( *this, &MailOptionsDialog::cannedConfigSelected));
         } else
            dds = NULL;
         
         command = new PG_LineEdit( this, PG_Rect( 10, 60, Width() - 20, 25 ));
         command->SetText( CGameOptions::Instance()->mailProgram );
         
         ASCString help = readtextmessage( 80 );
         if ( !help.empty() ) 
            new TextRenderer( this, PG_Rect( 10, 100, Width()-20, 160 ), help); 
         
         AddStandardButton("OK")->sigClick.connect( SigC::slot( *this, &MailOptionsDialog::ok));
         AddStandardButton("Cancel")->sigClick.connect( SigC::slot( *this, &MailOptionsDialog::cancel));
         
      }
};

void editEmailOptions()
{
   MailOptionsDialog mod;
   mod.Show();
   mod.RunModal();
}
