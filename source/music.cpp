/*! \file music.cpp
    \brief The music playing system of ASC
*/

#include <stdio.h>
#include <stdlib.h>
#include <SDL_mixer.h>

#ifndef karteneditor
#include "sdl/sound.h"
#endif


#include "basestrm.h"
#include "stringtokenizer.h"
#include "music.h"
#include "itemrepository.h"
#include "dlg_box.h"
#include "textfile_evaluation.h"
#include "sgstream.h"


typedef deallocating_vector<MusicPlayList*> PlayLists;
PlayLists playLists;

void PlayListLoader::readTextFiles ( PropertyReadingContainer& prc, const ASCString& fileName, const ASCString& location )
{
   MusicPlayList* mpl = new MusicPlayList;
   mpl->runTextIO ( prc );
   
   mpl->filename = fileName;
   mpl->location = location;
   playLists.push_back ( mpl );
}

void PlayListLoader::read ( tnstream& stream )
{
   readPointerContainer( playLists, stream );
}

void PlayListLoader::write ( tnstream& stream )
{
   writePointerContainer( playLists, stream );
}


void MusicPlayList :: runTextIO ( PropertyContainer& pc )
{
   pc.addString( "Name", name );
   pc.addStringArray( "Tracks", fileGroups );

   displayLogMessage ( 4, "Reading play list: " );

   reset();
}


void MusicPlayList :: read ( tnstream& stream )
{
   int version = stream.readInt(); // version
   if ( version != 1 ) fatalError ("invalid version for MusicPlayList" );
   name = stream.readString();
   filename = stream.readString();
   location = stream.readString();
   readClassContainer( fileGroups, stream );
   reset();
}

void MusicPlayList :: write ( tnstream& stream ) const
{
   stream.writeInt( 1 );
   stream.writeString ( name );
   stream.writeString ( filename );
   stream.writeString ( location );
   writeClassContainer( fileGroups, stream );
}


const ASCString& MusicPlayList :: getNextTrack()
{
   if ( fileNameList.empty() ) {
      static ASCString emptyString;
      return emptyString;
   }

   if ( iter == fileNameList.end() )
      reset();

   return *(iter++);
}

void MusicPlayList :: reset ( )
{
   fileNameList.clear();

   for ( TrackList::iterator i = fileGroups.begin(); i != fileGroups.end(); i++ ) {
      tfindfile ff ( *i );
      int loc;
      bool incontainer;
      ASCString location,name;
      name = ff.getnextname ( &loc, &incontainer, &location );
      while ( !name.empty()) {
         if ( !incontainer ) {
            ASCString filename = location + pathdelimitterstring + name;
            displayLogMessage ( 4, filename + ", "  );
            fileNameList.push_back ( filename );
         }
         name = ff.getnextname ( &loc, &incontainer, &location );
      };
   }
   displayLogMessage ( 4, "Finished \n" );

   iter = fileNameList.begin();
}


ASCString MusicPlayList::getDiagnosticText()
{
   ASCString text;

   text += "Play List location:\n";
   text += location + "\n\n";
   
   text += "File Patterns:\n";
   if ( fileGroups.empty() )
      text += "-none-\n";
   else
      for ( TrackList::iterator i = fileGroups.begin(); i != fileGroups.end(); i++ ) 
         text += *i + "\n";
   text += "\n";

   text += "Found Files:\n";
   if ( fileNameList.empty() )
      text += "-none-\n";
   else
      for ( TrackList::iterator i = fileNameList.begin(); i != fileNameList.end(); i++ ) 
         text += *i + "\n";
   text += "\n";

   return text;
}

void startMusic ()
{
#ifndef karteneditor
  if ( !playLists.empty() )
     SoundSystem::getInstance()->playMusic ( playLists.front() );
  else
     displayLogMessage ( 1, "No play lists available!\n" );

#endif
}





class   PlayListSelector : public tstringselect {
           public :
                 int lastchoice;
                 virtual void setup(void);
                 virtual void buttonpressed(int id);
                 virtual void run(void);
                 virtual void get_text( int nr);
              };

void         PlayListSelector ::setup(void)
{
   action = 0;
   title = "Select PlayList";
   numberoflines = playLists.size();
   ey = ysize - 90;
   startpos = lastchoice;
   addbutton("~O~K",20,ysize - 80,xsize/2-5,ysize - 60,0,1,12,true);
   addbutton("~C~ancel",  xsize/2+5,ysize - 80,xsize-20,ysize - 60,0,1,14,true);
   addkey ( 12, ct_enter );
   addkey ( 14, ct_esc );
}


void         PlayListSelector ::buttonpressed(int         id)
{
   tstringselect::buttonpressed(id);
   switch (id) {

      case 14:  action = -1;
                break;

      case 12:   if ( redline >= 0) {
#ifndef karteneditor
                      SoundSystem::getInstance()->playMusic ( playLists[redline] );
                      action = 1;
#endif
                 } else
                    displaymessage ( "Please select a play list", 3);
                 break;
   }
}


void         PlayListSelector ::get_text( int nr)
{
   strcpy(txt,playLists[nr]->getName().c_str() );
}


void         PlayListSelector ::run(void)
{
   do {
      tstringselect::run();
   }  while ( action == 0 );
}


void selectPlayList( )
{
   PlayListSelector  gps;

   gps.init();
   gps.run();
   gps.done();
}

