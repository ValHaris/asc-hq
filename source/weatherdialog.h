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
class ChanceSettingDialog :  public ASC_PG_Dialog {
public:  
  ChanceSettingDialog(const vector<string>& labelVec);
  
  ~ChanceSettingDialog();
  
  bool closeWindow() {
    quitModalLoop(2);
    return true;
  };
  
  
private:  
  bool buttonEvent( PG_Button* button );
  
  vector<PG_LineEdit*> chances;
  vector<PG_Label*> labels;
  static const int xsize;
  static const int ysize;
};


/**
@author Kevin Hirschmann
*/
class WeatherDialog :  public ASC_PG_Dialog {
public:
  WeatherDialog();
  

  virtual ~WeatherDialog();

  
  
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
  
  PG_Label* upperSizeLimitsLabel;
  PG_LineEdit* upperSizeLimitsValue;
  
  PG_Label* lowerSizeLimitsLabel;
  PG_LineEdit* lowerSizeLimitsValue;
  
  PG_Label* fallOutLabel;
  PG_Button* fallOutButton;
  
  bool editFallOut( PG_Button* button );
  
  bool buttonEvent( PG_Button* button );
  
  bool closeWindow() {
    quitModalLoop(2);
    return true;
  };
};

extern void weatherConfigurationDialog();

#endif

