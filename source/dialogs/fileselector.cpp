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


#include "fileselector.h"

class FileInfo
{
   public:
      ASCString name;
      ASCString location;
      time_t modificationTime;
      FileInfo( const ASCString& filename, const ASCString& filelocation, time_t time  ) : name( filename), location( filelocation ), modificationTime( time )
      {}
      FileInfo( const FileInfo& fi ) : name( fi.name ), location( fi.location ), modificationTime( fi.modificationTime )
      {}
}
;



class FileWidget: public SelectionWidget
{
      FileInfo fileInfo;
      ASCString time;
   public:
      FileWidget( PG_Widget* parent, const PG_Point& pos, int width, const FileInfo* fi ) : SelectionWidget( parent, PG_Rect( pos.x, pos.y, width, 18 )), fileInfo( *fi )
      {
#ifndef ctime_r
         time = ctime( &fileInfo.modificationTime );
#else

         char c[100];
         ctime_r( &fileInfo.modificationTime, c );
         time  = c;
#endif

         int col1 =        width * 3 / 9;
         int col2 = col1 + width * 3 / 9;

         PG_Label* lbl1 = new PG_Label( this, PG_Rect( 0, 0, col1 - 10, Height() ), fileInfo.name );
         lbl1->SetFontSize( lbl1->GetFontSize() -2 );

         PG_Label* lbl2 = new PG_Label( this, PG_Rect( col1, 0, col2-col1-10, Height() ), time );
         lbl2->SetFontSize( lbl2->GetFontSize() -2 );

         PG_Label* lbl3 = new PG_Label( this, PG_Rect( col2, 0, Width() - col2, Height() ), fileInfo.location );
         lbl3->SetFontSize( lbl3->GetFontSize() -2 );

         SetTransparency( 255 );
      };

      ASCString getName() const
      {
         return fileInfo.name;
      };

   protected:

      void display( SDL_Surface * surface, const PG_Rect & src, const PG_Rect & dst )
      {
      }
      ;
};



FileSelectionItemFactory::FileSelectionItemFactory( const ASCString& wildcard )
{
   ASCString::size_type begin = 0;
   ASCString::size_type pos = wildcard.find( ";" );
   do {
      ASCString w = wildcard.substr( begin, pos );
      if ( pos != ASCString::npos && pos+1 < wildcard.length() ) {
         begin = pos + 1;
         pos = wildcard.find( ";", begin );
      } else
         begin = pos = ASCString::npos;
         
      tfindfile ff ( w );
   
      tfindfile::FileInfo fi;
      while ( ff.getnextname( fi) ) {
         FileInfo* fi2 = new FileInfo( fi.name, fi.location, fi.date );
         items.push_back ( fi2 );
      }
   } while ( begin != ASCString::npos );

   sort( items.begin(), items.end(), comp );
   restart();
};

bool FileSelectionItemFactory::comp ( const FileInfo* i1, const FileInfo* i2 )
{
   return  i1->modificationTime > i2->modificationTime || ( (i1->modificationTime == i2->modificationTime) &&  (i1->name < i2->name) );
   // return  i1->name < i2->name;
};

void FileSelectionItemFactory::restart()
{
   it = items.begin();
};

SelectionWidget* FileSelectionItemFactory::spawnNextItem( PG_Widget* parent, const PG_Point& pos )
{
   if ( it != items.end() )
      return new FileWidget( parent, pos, parent->Width() - 15, *(it++) );
   else
      return NULL;
};


void FileSelectionItemFactory::itemMarked( const SelectionWidget* widget )
{
   if ( !widget )
      return;

   const FileWidget* fw = dynamic_cast<const FileWidget*>(widget);
   assert( fw );
   filenameMarked( fw->getName() );
}

void FileSelectionItemFactory::itemSelected( const SelectionWidget* widget, bool mouse )
{
   if ( !widget )
      return;

   const FileWidget* fw = dynamic_cast<const FileWidget*>(widget);
   assert( fw );
   if ( mouse )
      filenameSelectedMouse( fw->getName() );
   else
      filenameSelectedKeyb( fw->getName() );
}



void FileSelectionWindow::fileNameSelected( const ASCString& filename )
{
   // if ( !patimat ( wildcard.c_str(), filename.c_str() ))
   this->filename = filename;
   if ( this->filename.find('.') == ASCString::npos )
      this->filename += wildcard.substr( wildcard.find_first_not_of("*") );
   quitModalLoop(0);
};

void FileSelectionWindow::fileNameEntered( ASCString filename )
{
   if ( !patimat( wildcard, filename ))
      filename += wildcard.substr(1);
   fileNameSelected(filename);
};

FileSelectionWindow::FileSelectionWindow( PG_Widget *parent, const PG_Rect &r, const ASCString& fileWildcard, bool save ) : ASC_PG_Dialog( parent, r, "" ), wildcard( fileWildcard)
{
   if ( save )
      SetTitle( "Enter Filename" );
   else
      SetTitle( "Choose File" );
   
   FileSelectionItemFactory* factory = new FileSelectionItemFactory( fileWildcard );
   factory->filenameSelectedMouse.connect ( SigC::slot( *this, &FileSelectionWindow::fileNameSelected ));
   factory->filenameSelectedKeyb.connect ( SigC::slot( *this, &FileSelectionWindow::fileNameSelected ));
   // factory->filenameMarked.connect   ( SigC::slot( *this, &FileSelectionWindow::fileNameSelected ));
   ItemSelectorWidget* isw = new ItemSelectorWidget( this, PG_Rect(10, GetTitlebarHeight(), r.Width() - 20, r.Height() - GetTitlebarHeight()), factory );
   if ( save ) {
      isw->constrainNames( false );
      isw->nameEntered.connect( SigC::slot( *this, &FileSelectionWindow::fileNameEntered ));
   }
   isw->sigQuitModal.connect( SigC::slot( *this, &ItemSelectorWindow::QuitModal));

};



ASCString  selectFile( const ASCString& ext, bool load )
{
   FileSelectionWindow fsw( NULL, PG_Rect( 10, 10, 700, 500 ), ext, !load );
   fsw.Show();
   fsw.RunModal();
   return fsw.getFilename();
}

