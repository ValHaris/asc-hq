//
// C++ Interface: weatherdialog
//
// Description:
//
//
// Author: Martin Bickel <bickel@asc-hq.org>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef WEATHERDIALOG_H
#define WEATHERDIALOG_H

#include <sstream>
#include "global.h"
#include "paradialog.h"
#include "weatherdialog.h"
#include "events.h"
#include "gameoptions.h"
#include "sg.h"
#include "sdl/sound.h"

/**
@author Kevin Hirschmann
*/
class WeatherDialog :  public ASC_PG_Dialog {
public:
  WeatherDialog();
  

  virtual ~WeatherDialog();

  bool closeWindow() {
    quitModalLoop(2);
    return true;
  };
  
   static const char* int2String(int i) {
    stringstream s;
    s << i;
    return s.str().c_str();
  }
  
   static const char* float2String(float f) {
    stringstream s;
    s << f;
    return s.str().c_str();
  }
  
private:
  static const int xsize;
  static const int ysize;
  
  PG_CheckButton* randomMode;
  
  PG_Label* areaSpawnsLabel;
  PG_LineEdit* areaSpawnsValue;
  
  PG_Label* nthTurnLabel;
  PG_LineEdit* nthTurnValue;
  
  PG_Label* windSpeedFieldRatioLabel;
  PG_LineEdit* windSpeedFieldRatioValue;
  
  
  bool buttonEvent( PG_Button* button );
};

extern void weatherConfigurationDialog();

#endif

