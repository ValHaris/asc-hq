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


#include <sstream>
#include <pgimage.h>

#include "../gameoptions.h"
#include "../gamemap.h"
#include "../paradialog.h"
#include "importbi3map.h"
#include "fileselector.h"
#include "../itemrepository.h"
#include "../loadbi3.h"
#include "../widgets/textrenderer.h"
#include "pglistboxitem.h"


class ImportBI3MapDialog : public ASC_PG_Dialog {

      GameMap* actmap;
      bool insert;

      PG_LineEdit* directory;
      PG_ListBox* importTable;

      vector<ASCString> importTables;
      

      bool checkBI3Path( ASCString filename ) 
      {
         filename += "mis";
         filename += pathdelimitterstring;
         filename += "*.dat";

         if ( exist( filename ))
            return true;
         else {
            errorMessage("Could not find files in BI3 directory\nlooking for " + filename );
            return false;
         }
      }

      bool ok() {

         ASCString bi3Path = directory->GetText();
         appendbackslash( bi3Path );

         if ( !checkBI3Path( bi3Path ))
            return false;


         if ( importTable->GetSelectedIndex() < 0 ) {
            if ( importTables.size() > 0 ) {
               errorMessage("No import table selected" );
               return false;
            } else {
               errorMessage("No import tables available. Aborting" );
               QuitModal();
            }
         }

         if ( bi3Path != CGameOptions::Instance()->BI3directory ) {
            CGameOptions::Instance()->setChanged ( 1 );
            CGameOptions::Instance()->BI3directory = bi3Path;
         }

         ASCString wildcard = bi3Path + "mis" + pathdelimitterstring + "*.dat" ;

         ASCString filename = selectFile( wildcard, true );
         if ( !filename.empty() ) {
            ASCString errorOutput;
            try {
               if ( insert==false )  {
                  importbattleislemap ( bi3Path, filename, terrainTypeRepository.getObject_byID(9999)->weather[0], getImportTable(), &errorOutput );
                  if ( !errorOutput.empty() ) {
                     ViewFormattedText vft( "Results of Import", errorOutput, PG_Rect(-1,-1,400,400));
                     vft.Show();
                     vft.RunModal();
                  }
               } else {
                  int x = actmap->getCursor().x;
                  int y = actmap->getCursor().y;
                  if ( x < 0 )
                     x = 0;

                  if ( y < 0 )
                     y = 0;
                  insertbattleislemap ( x, y, bi3Path, filename, getImportTable() );
               }
            } catch ( ASCexception ) {
               errorMessage("importing the map failed");
            }
         }

         QuitModal();
         return true;
      }

      bool cancel() {
         QuitModal();
         return true;
      }

   public:
      ImportBI3MapDialog( GameMap* gamemap, bool insert );

      ASCString getImportTable() {
         if ( importTable->GetSelectedIndex() >= 0 && importTable->GetSelectedIndex() < importTables.size() )
            return importTables[importTable->GetSelectedIndex()];
         else
            return "";
      }
};

 
ImportBI3MapDialog::ImportBI3MapDialog( GameMap* gamemap, bool insert ) : ASC_PG_Dialog( NULL, PG_Rect(-1,-1,450,370), "Import BI Map") , actmap(gamemap)
{
   this->insert = insert;
   AddStandardButton("OK")->sigClick.connect( SigC::slot( *this, &ImportBI3MapDialog::ok ));
   AddStandardButton("Cancel")->sigClick.connect( SigC::slot( *this, &ImportBI3MapDialog::cancel ));

   int ypos = 30;
   new PG_Label( this, PG_Rect( 20, ypos, Width()-40,20), "Directory of BI3 installation:");
   ypos += 25;

   directory = new PG_LineEdit( this, PG_Rect( 20, ypos, Width() - 40, 20 ));
   directory->SetText( CGameOptions::Instance()->BI3directory );
   ypos += 40;



   new PG_Label( this, PG_Rect( 20, ypos, Width()-40,20), "BI3 item translation table (note that data is only read at startup of ASC):");
   ypos += 25;

   importTable = new PG_ListBox( this, PG_Rect(20, ypos, Width() - 40, 150 ));

   importTables = getBI3ImportTables();
   for ( vector<ASCString>::iterator i = importTables.begin(); i != importTables.end(); ++i )
      new PG_ListBoxItem( importTable, 20, *i );

};

void importBI3Map( GameMap* gamemap, bool insert )
{
   ImportBI3MapDialog ibi3md ( gamemap, insert );
   ibi3md.Show();
   ibi3md.RunModal();
}


