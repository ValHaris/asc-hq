
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "autoprogressbar.h"

#include <pgapplication.h>
#include "../events.h"

#include "../basestrm.h"

void AutoProgressBar :: tick()
{

   newTickTimes.push_back ( SDL_GetTicks() - starttime );

   // limit to 25 Hz to reduce graphic updates
   if ( lastdisplaytime + 4 < SDL_GetTicks() ) {
      double p;
      // double p = double(ticker - starttime) * 100  / time;
      if ( counter < prevTickTimes.size() && time ) {
         int a = prevTickTimes[counter];
         p = 100 * a / time;
      } else
         p = counter / 100;

      if ( p > 99 )
         p = 99;

      SetProgress( p );
      lastdisplaytime = SDL_GetTicks();
   }

   ++counter;


   lastticktime = SDL_GetTicks();
};

AutoProgressBar :: AutoProgressBar( sigc::signal<void>& tickSignal, PG_Widget *parent, const PG_Rect &r, const std::string &style ) : PG_ProgressBar( parent, r, style ), lastticktime(-1), counter(0)
{
   lastdisplaytime = starttime = SDL_GetTicks();

   tickSignal.connect( sigc::mem_fun( *this, &AutoProgressBar::tick ));

   try {
      tnfilestream stream ( "progress.dat", tnstream::reading  );
      stream.readInt(); // version
      time = stream.readInt( );
      readClassContainer( prevTickTimes, stream );
   }
   catch ( ... ) {
      time = 200;
   };
};

void AutoProgressBar :: close( )
{
   try {
      tnfilestream stream ( "progress.dat", tnstream::writing  );
      stream.writeInt( 1 );
      stream.writeInt( lastticktime - starttime );
      writeClassContainer( newTickTimes, stream );
   }
   catch ( ... ) {
   }
}

