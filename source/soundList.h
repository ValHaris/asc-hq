#ifndef soundList_h_included
#define soundList_h_included

/*! \file soundList.h
    \brief The collection of all sounds used by ASC
*/

#include <map>

#include "sdl/sound.h"
#include "ascstring.h"

/** A helper class for managing looping sounds.
    A sound may be preloaded, later activated and this class assures that it is closed again.
    Or it simply can be loaded with a running sound and stops it on destruction
*/
class SoundLoopManager {
      Sound* sound;
      bool active;
   public:
      SoundLoopManager ( Sound* snd, bool _active = true ) : sound( snd ), active ( _active ) {};
      // void setSound ( Sound* snd ) { sound = snd; };
      void activate ( ) { if ( !active && sound ) sound->playLoop(); active = true; };
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
    enum Sample { shooting, unitBlowsUp, moving, menu_ack, menu_fail, building_conq };

    static void init( );
    Sound* play ( Sample snd, int subType = 0, bool looping = false);
    Sound* getSound ( Sample snd, int subType = 0 );

   ~SoundList();
private:
     typedef map<ASCString,Sound*> SoundFiles;
     SoundFiles  soundFiles;
     void initialize();
};

#endif