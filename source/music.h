#ifndef music_h_included
#define music_h_included

/*! \file soundList.h
    \brief The collection of all sounds used by ASC
*/

#include <vector>
#include <map>
#include <SDL_mixer.h>

#include "global.h"
#include "sdl/sound.h"
#include "ascstring.h"
#include "textfileparser.h"


/** A List containing several tracks of music.
*/
class MusicPlayList {
       ASCString name;
       typedef vector<ASCString> TrackList;
       TrackList fileNameList;
       TrackList::iterator iter;
   public:
      const ASCString& getName ( ) { return name; };

      ASCString filename,location;

      //! loads a PlayList from a .ASCTXT file
      void runTextIO ( PropertyContainer& pc );

      //! resets the internal track iterator
      void reset();

      //! returns the filename of the next music track
      const ASCString& getNextTrack();
};


//! start playing the first playlist
extern void startMusic ();

//! loads all .asctxt files containing play lists
extern void loadAllMusicPlayLists ( );


//! opens a dialog to select a playlist
extern void selectPlayList( );

#endif
