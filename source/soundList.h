#ifndef soundListH
#define soundListH

/*! \file soundList.h
    \brief The collection of all sounds used by ASC
*/

#include <map>

#include "global.h"
#include "sdl/sound.h"
#include "ascstring.h"
#include "textfile_evaluation.h"

/** A helper class for managing looping sounds.
    A sound may be preloaded, later activated and this class assures that it is closed again.
    Or it simply can be loaded with a running sound and stops it on destruction
*/
class SoundLoopManager {
      Sound* sound;
      bool active;
   public:
      SoundLoopManager ( Sound* snd, bool _active = true );
      // void setSound ( Sound* snd ) { sound = snd; };
      void activate ( int dummy );
      void fadeOut ( int ms ) { if ( sound && active ) sound->fadeOut( ms ); };
      ~SoundLoopManager() { if ( sound && active ) sound->stop(); };
};



/** This class provides all the sounds required by the games user interface.
    It uses the singleton design pattern.
*/
class SoundList {
    SoundList() {};
    static SoundList* instance;
public:
    static SoundList& getInstance();
    enum Sample { shooting, unitExplodes, buildingCollapses, moving, menu_ack, conquer_building, repair, refuel, jumpdrive };

    static void init( );
    Sound* playSound ( Sample snd, int subType = 0, bool looping = false, const ASCString& label = "" );
    Sound* getSound ( Sample snd, int subType = 0, const ASCString& label = "", int height = -1);

   ~SoundList();
private:
     void initialize();

     typedef map<ASCString,Sound*> SoundFiles;
     SoundFiles  soundFiles;

     Sound* getSound( const ASCString& filename );

      struct SoundAssignment {
           SoundList::Sample sample;
           int subType;
           Sound* defaultSound;
           map<ASCString,Sound*> snd;
      };
     vector<SoundAssignment> soundAssignments;
     void readLine( PropertyContainer& pc, const ASCString& name, SoundList::Sample sample, int subtype = 0 );
};

#endif
