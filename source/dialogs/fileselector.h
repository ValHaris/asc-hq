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

#ifndef FILESELECTOR_H
#define FILESELECTOR_H

#include "selectionwindow.h"
#include "../paradialog.h"
#include "../graphics/blitter.h"
#include "../loaders.h"

class FileSelectionItemFactory;
class SavegameSelectionItemFactory;

class FileInfo
{
   public:
      ASCString name;
      ASCString location;
      time_t modificationTime;
      int level;
      FileInfo( const ASCString& filename, const ASCString& filelocation, time_t time, int directorylevel  ) : name( filename), location( filelocation ), modificationTime( time ), level( directorylevel )
      {}
      FileInfo( const FileInfo& fi ) : name( fi.name ), location( fi.location ), modificationTime( fi.modificationTime ), level ( fi.level )
      {}
}
;

class SavegameWidget: public SelectionWidget {
      FileInfo fileInfo;
      ASCString time;
      bool fileInformationLoaded;
      GameFileInformation gfi;
      PG_Label* mapTitleLabel;
      static Surface alternateImage;
   public:
      SavegameWidget( PG_Widget* parent, const PG_Point& pos, int width, const FileInfo* fi );
      ASCString getName() const;
   protected:
      void display( SDL_Surface * surface, const PG_Rect & src, const PG_Rect & dst );

};


class FileSelectionWindow : public ASC_PG_Dialog {
      ASCString filename;
      ASCString wildcard;
      bool saveFile;
      bool overwriteMessage;
      FileSelectionItemFactory* factory;

   protected:
      void fileNameSelected( const ASCString& filename );
      void fileNameEntered( ASCString filename );
   public:
      /*! \param wildcard may be several wildcards concatenated by ';' */
      FileSelectionWindow( PG_Widget *parent, const PG_Rect &r, const ASCString& fileWildcard, bool save, bool overwriteMessage );
      ASCString getFilename() { return filename; };
};


class FileSelectionItemFactory: public SelectionItemFactory  {
   protected:
      typedef deallocating_vector<FileInfo*> Items;
      Items::iterator it;

   private:      
      Items items;
      
   public:
      /** \param wildcard may be several wildcards concatenated by ';' */
      FileSelectionItemFactory( const ASCString& wildcard );
      
      static bool comp ( const FileInfo* i1, const FileInfo* i2 );

      void restart();

      int getLevel( const ASCString& name );
      
      SelectionWidget* spawnNextItem( PG_Widget* parent, const PG_Point& pos );
      
      sigc::signal<void,const ASCString& > filenameSelectedMouse;
      sigc::signal<void,const ASCString& > filenameSelectedKeyb;
      sigc::signal<void,const ASCString& > filenameMarked;
      
      void itemMarked( const SelectionWidget* widget );
      
      void itemSelected( const SelectionWidget* widget, bool mouse );
};

extern ASCString  selectFile( const ASCString& ext, bool load, bool overwriteMessage = true );


class SavegameSelectionWindow : public ASC_PG_Dialog {
      ASCString filename;
      ASCString wildcard;
      bool saveFile;
      bool overwriteMessage;
      SavegameSelectionItemFactory* factory;

   protected:
      void fileNameSelected( const ASCString& filename );
      void fileNameEntered( ASCString filename );
   public:
      /*! \param wildcard may be several wildcards concatenated by ';' */
      SavegameSelectionWindow( PG_Widget *parent, const PG_Rect &r, const ASCString& fileWildcard, bool save, bool overwriteMessage );
      ASCString getFilename() { return filename; };
};


class SavegameSelectionItemFactory: public SelectionItemFactory  {
   protected:
      typedef deallocating_vector<FileInfo*> Items;
      Items::iterator it;

   private:
      Items items;

   public:
      /** \param wildcard may be several wildcards concatenated by ';' */
      SavegameSelectionItemFactory( const ASCString& wildcard );

      static bool comp ( const FileInfo* i1, const FileInfo* i2 );

      void restart();

      int getLevel( const ASCString& name );

      SelectionWidget* spawnNextItem( PG_Widget* parent, const PG_Point& pos );

      sigc::signal<void,const ASCString& > filenameSelectedMouse;
      sigc::signal<void,const ASCString& > filenameSelectedKeyb;
      sigc::signal<void,const ASCString& > filenameMarked;

      void itemMarked( const SelectionWidget* widget );

      void itemSelected( const SelectionWidget* widget, bool mouse );
};



extern ASCString  selectSavegame( const ASCString& ext, bool load, bool overwriteMessage = true );

#endif
