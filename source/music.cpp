/*! \file music.cpp
    \brief The music playing system of ASC
*/

#include <stdio.h>
#include <stdlib.h>

#include "sound.h"
#include "soundList.h"
#include "basestrm.h"
#include "stringtokenizer.h"
#include "music.h"
#include "itemrepository.h"
#include "dlg_box.h"


typedef vector<MusicPlayList*> PlayLists;
PlayLists playLists;

void loadAllMusicPlayLists ( )
{
    TextPropertyList& tpl = textFileRepository["playlist"];
    for ( TextPropertyList::iterator i = tpl.begin(); i != tpl.end(); i++ ) {
      PropertyReadingContainer pc ( "playlist", *i );

      MusicPlayList* mpl = new MusicPlayList;
      mpl->runTextIO ( pc );
      pc.run();

      mpl->filename = (*i)->fileName;
      mpl->location = (*i)->location;
      playLists.push_back ( mpl );
   }
}

void MusicPlayList :: runTextIO ( PropertyContainer& pc )
{
   pc.addString( "Name", name );
   pc.addStringArray( "Tracks", fileNameList ).evaluate();
   reset();
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
    iter = fileNameList.begin();
}


void startMusic ()
{
  if ( !playLists.empty() )
     SoundSystem::getInstance()->playMusic ( playLists.front() );

}





class   PlayListSelector : public tstringselect {
           public :
                 int lastchoice;
                 virtual void setup(void);
                 virtual void buttonpressed(int id);
                 virtual void run(void);
                 virtual void gettext(word nr);
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
                      SoundSystem::getInstance()->playMusic ( playLists[redline] );
                      action = 1;
                 } else
                    displaymessage ( "Please select a play list", 3);
                 break;
   }
}


void         PlayListSelector ::gettext(word nr)
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

