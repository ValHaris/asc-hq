/***************************************************************************
                             dropdownselector
                             -------------------
    copyright            : (C)  2006 by Martin Bickel
    email                : <bickel@asc-hq.org>
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef autoprogressbarH
#define autoprogressbarH

#include <vector>
#include <pgprogressbar.h>
#include "../global.h"

class AutoProgressBar: public PG_ProgressBar {

      int starttime;
      int time;
      int lastticktime;
      int lastdisplaytime;
      int counter;
      vector<int> newTickTimes;
      vector<int> prevTickTimes;

      void tick();
   public:
      AutoProgressBar( sigc::signal<void>& tickSignal, PG_Widget *parent, const PG_Rect &r=PG_Rect::null_rect, const std::string &style="Progressbar" );
      void close( );
};

#endif
