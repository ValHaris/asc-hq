#ifdef _DOS_
 #include "dos\sound.h"
#else
 #include "sdl/sound.h"
#endif

#define WEAPON_SOUND_COUNT 10
#define WEAPON_SOUND_FILENAME_TEMPLATE "weapon%02u.wav"
/** This class provides all the sounds required by the games user interface */
class tSoundList {
public:
  void init(unsigned weaponCount);
  Sound *soundFail; // noise to make if the sound list fails
  Sound *boom;
  Sound *menuPopup;
  Sound *menuSelect;
  Sound *menuAbort;
  /** Get the sound matching a specific weapon type.  Weapon numbers for which
   *  there is no sound return the fail sound.
   *  To play weapon sounds */
  Sound *weaponSound( unsigned int weaponType );
private:
  /** The number of weapon sounds available.  Note that some of these sounds
   *  may be silent if their sound files are absent */
  unsigned weaponSoundCount;
  /** Weapons sounds.  This array is dynamically allocated at run-time hence
   *  the pointer to pointer structure.  To access the weapon sounds, use
   *  the weaponSound(unsigned) function. */
  Sound **weaponSoundArray;
};

/** Due to initialisation ordering problems in some C++ implementations, you
 *  must always call initSoundList before using this object */
extern tSoundList sound;
void initSoundList();
