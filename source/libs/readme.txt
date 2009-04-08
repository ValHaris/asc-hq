Overview of the included libraries

 bzlib  
    used by: windows only
    
 getopt:
    used by: windows only

 loki-0.1.6
    used by: windows
             unix only if configure can't find loki installed on the system
               
 paragui
    used by: windows
             unix
    This is a heavily patched and updated version of Paragui, which is neither
    compatible with the last relaese nor with the current CVS head of paragui
    
 rand
    used by: windows
             unix
    Just a random number generator, pinched from glibc
 
 revel
    used by: windows
             unix, if configure can find xvid installed on the system
                   without xvid, the whole video functionality is disabled
    website: http://revel.sourceforge.net/
    
 sdlmm
    used by: windows
             unix
    This contains a few bugfixes and minor enhancements compared to the latest
    release sdlmm 0.1.8
    website: http://sdlmm.sourceforge.net/
 
 sdl_sound
    used by: windows
             unix only if configure can't find sdl_sound installed on the system
    This is a stripped down version of sdl_sound which only has ogg vorbis support.
    Several Linux distros don't provide sdl_sound, probably because of licensing issues
    regarding its MP3 support. To allow ASC to output sound on those distros too, we 
    provide this sdl_sound version which doesn't have MP3 support
 
 triangul
    used by: windows
             unix
