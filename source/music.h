#ifndef musicH
#define musicH

/*! \file soundList.h
    \brief The collection of all sounds used by ASC
*/

#include <vector>
#include <map>

#include "global.h"
#include "ascstring.h"
#include "textfile_evaluation.h"
#include "itemrepository.h"


/** A List containing several tracks of music.
*/
class MusicPlayList {
       ASCString name;
       typedef vector<ASCString> TrackList;

       //! carries the resolved filenames
       TrackList fileNameList;

       //! may contain wildcards like *.ogg referencing several files
       TrackList fileGroups;

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

      void read ( tnstream& stream );
      void write ( tnstream& stream ) const;

      ASCString getDiagnosticText();
};

class PlayListLoader : public TextFileDataLoader {
    public:
      virtual void readTextFiles(PropertyReadingContainer& prc, const ASCString& fileName, const ASCString& location);
      virtual void read ( tnstream& stream );
      virtual void write ( tnstream& stream );
      virtual ASCString getTypeName() { return "playlist"; };
};

//! start playing the first playlist
extern void startMusic ();


#endif
