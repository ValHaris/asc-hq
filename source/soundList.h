/*! \file soundList.h
    \brief The collection of all sounds used by ASC
*/

#include <map>

#ifdef _DOS_
 #include "dos/sound.h"
#else
 #include "sdl/sound.h"
#endif

#include "ascstring.h"

/** This class provides all the sounds required by the games user interface.
    It uses the singleton design pattern.
*/
class SoundList {
    SoundList() {};
    static SoundList* instance;
    bool noSound;
public:
    static SoundList& getInstance();
    enum Sample { shooting, unitBlowsUp, moving, menu_ack, menu_fail, building_conq };

    void init( bool quiet );
    void play ( Sample snd, int subType = 0 );

   ~SoundList();
private:
     typedef map<ASCString,Sound*> SoundFiles;
     SoundFiles  soundFiles;
};

extern SoundList sound;
