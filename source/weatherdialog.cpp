//
// C++ Implementation: weatherdialog
//
// Description:
//
//
// Author: Martin Bickel <bickel@asc-hq.org>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "weatherdialog.h"
#include "weatherarea.h"
#include "guidimension.h"

const int WeatherDialog::xsize = 400;
const int WeatherDialog::ysize = 512;


WeatherDialog::WeatherDialog() :  ASC_PG_Dialog(NULL, PG_Rect( 100, 100, xsize, ysize ), "Weather Configuration", SHOW_CLOSE ) {
  
  //Random Mode
  randomMode = new PG_CheckButton(this, PG_Rect( GuiDimension::getLeftIndent(), GuiDimension::getTopOffSet(), xsize/3 , GetTextHeight()*2), "Random Mode");
  randomMode->SetSizeByText();
  if(actmap->weatherSystem->getEventMode() == RANDOMMODE){
    randomMode->SetPressed();
  }
  
  //xPosition for edit Fields
  int valueXPos = xsize * 0.75;

  //Amount of Area Spawns
  int areaSpawnsLabelYPos = GuiDimension::getTopOffSet() + GetTextHeight()*2;
  areaSpawnsLabel = new PG_Label(this, PG_Rect(GuiDimension::getLeftIndent(), areaSpawnsLabelYPos, xsize/3 , GetTextHeight()*2),
                                 "Amount Of Spawns:");
  areaSpawnsLabel->SetSizeByText();

  int areaSpawnsLineEditXPos = GuiDimension::getLeftIndent() + areaSpawnsLabel->Width() + GuiDimension::getLeftOffSet();
  areaSpawnsValue = new PG_LineEdit(this, PG_Rect(valueXPos, areaSpawnsLabelYPos, GuiDimension::getLineEditWidth() , GetTextHeight()*2));
  areaSpawnsValue->SetText(int2String(actmap->weatherSystem->getSpawnsAmount()));   
  
  
  //Weather-Update each nth turn
  int nthTurnLabelYPos = areaSpawnsLabelYPos + GetTextHeight()*2;
  nthTurnLabel = new PG_Label(this, PG_Rect(GuiDimension::getLeftIndent(), nthTurnLabelYPos, xsize/3 , GetTextHeight()*2),
                              "Weather update each nth turn:");			      
  nthTurnLabel->SetSizeByText();  

  int nthTurnLineEditXPos = GuiDimension::getLeftIndent() + nthTurnLabel->Width() + GuiDimension::getLeftOffSet();
  nthTurnValue = new PG_LineEdit(this, PG_Rect(valueXPos, nthTurnLabelYPos, GuiDimension::getLineEditWidth() , GetTextHeight()*2));
  nthTurnValue->SetText(int2String(actmap->weatherSystem->getTimeInterval()));
  
  //Ratio windspeed to passed fields
  int windSpeedFieldRatioLabelYPos = nthTurnLabelYPos + GetTextHeight() * 2;
  windSpeedFieldRatioLabel = new PG_Label(this, PG_Rect(GuiDimension::getLeftIndent(), windSpeedFieldRatioLabelYPos, xsize/3 , GetTextHeight()*2), "WindSpeed to Fields ratio:");
  windSpeedFieldRatioLabel->SetSizeByText();    
  windSpeedFieldRatioValue = new PG_LineEdit(this, PG_Rect(valueXPos, windSpeedFieldRatioLabelYPos, GuiDimension::getLineEditWidth() , GetTextHeight()*2));
  windSpeedFieldRatioValue->SetText(float2String(actmap->weatherSystem->getWindspeed2FieldRatio()));
  

  
  //Ok and Cancel Button
  (new PG_Button(this, PG_Rect(30, Height()-40, (Width()-70)/2, 30), "OK", 100))->sigClick.connect(SigC::slot( *this, &WeatherDialog::buttonEvent ));
  (new PG_Button(this, PG_Rect(Width()/2+5, Height()-40, (Width()-70)/2, 30), "Cancel", 101))->sigClick.connect(SigC::slot( *this, &WeatherDialog::closeWindow ));

  sigClose.connect( SigC::slot( *this, &WeatherDialog::closeWindow ));

}


WeatherDialog::~WeatherDialog() {}

void weatherConfigurationDialog() {
  WeatherDialog wd;
  wd.Show();
  wd.Run();
}


bool WeatherDialog::buttonEvent( PG_Button* button ) {  
  if(randomMode->GetPressed()){
    actmap->weatherSystem->setEventMode(RANDOMMODE);
  }else{
     actmap->weatherSystem->setEventMode(EVENTMODE);
  }
  quitModalLoop(1);

}

