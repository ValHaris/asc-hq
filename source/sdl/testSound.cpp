/** This program provides a quick and dirty test for the Sound class
 *  defined in sound.cpp.
 *
 *  usage : testSound soundfilelist
 *
 *  Where soundfilelist is a space seperated list of sound (.wav) files.
 */
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <stdio.h>
#include <SDL/SDL.h>
#include <unistd.h>
#include <sound.h>

void main( int argc, char **argv ) {
  initSound();
  for( int i=1; i<argc; ++i ) {
    printf( "Loading %s.\n", argv[i] );
    Sound *s = new Sound(argv[i]);
    printf( "Playing %s.\n", argv[i] );
    s->play();
//      printf( "Pausing 1 second." );
//      sleep(1);
    printf( " Press a key for next sound . " );
    getchar();
    
    printf( "Freeing %s.\n", argv[i] );
    delete s;
  }
  closeSound();
}

