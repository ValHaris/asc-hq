/***************************************************************************
                          paradialog.h  -  description
                             -------------------
    begin                : Thu Feb 21 2002
    copyright            : (C) 2002 by Martin Bickel
    email                : bickel@asc-hq.org
 ***************************************************************************/

/*! \file paradialog.h
    \brief Dialog classes based on the Paragui library
*/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef resourceplacementdialogH
 #define resourceplacementdialogH

  #include <sstream>
  #include "global.h"
  #include "paradialog.h"
  #include "events.h"
  #include "gameoptions.h"
  #include "sg.h"
  #include "sdl/sound.h"  


class ResourcePlacementDialog : public ASC_PG_Dialog {
public:
  ResourcePlacementDialog( );
protected:
  static const int xsize;
  static const int ysize;
  static const int xOffSet;
  static const int yOffSet;
  static const char* int2String(int i) {
    stringstream s;
    s << i;
    return s.str().c_str();
  }

  static const int leftIndent;

  double fuelRoughness;
  double materialRoughness;
  int maxFuelOffSet;
  int maxMaterialOffSet;


  PG_LineEdit* fuelRoughnessWidget;
  PG_LineEdit* materialRoughnessWidget;
  PG_LineEdit* fuelOffsetWidget;
  PG_LineEdit* materialOffsetWidget;
  PG_CheckButton* placeFuelWidget;
  PG_CheckButton* placeMaterialWidget;
  PG_Label* materialRoughnessLabel;
  PG_Label* fuelRoughnessLabel;

  PG_Slider* materialOffSetSlider;
  PG_Label* materialOffSetLabel;
  PG_LineEdit* materialOffSetValue;

  PG_Slider* fuelOffSetSlider;
  PG_Label* fuelOffSetLabel;
  PG_LineEdit* fuelOffSetValue;
  
  PG_Slider* fuelFreeRatioSlider;
  PG_Label* fuelFreeRatioLabel;
  PG_LineEdit* fuelFreeRatioValue;
  
  PG_Slider* materialFreeRatioSlider;
  PG_Label* materialFreeRatioLabel;
  PG_LineEdit* materialFreeRatioValue;

  PG_MessageBox* d;

  bool radioButtonEvent( PG_RadioButton* button, bool state);
  bool buttonErrorOkEvent( PG_Button* button );
  bool buttonEvent( PG_Button* button );
  bool scrollTrackEventMaterial(long data);
  bool scrollTrackEventFuel(long data);
  bool scrollTrackEventMaterialFreeRatio(long data);
  bool scrollTrackEventFuelFreeRatio(long data);
  bool closeWindow() {
    quitModalLoop(2);
    return true;
  };

};


extern void resourcePlacementDialog();

#endif

