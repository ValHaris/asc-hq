
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
                                 "Amount of Spawns:");
  areaSpawnsLabel->SetSizeByText();

  int areaSpawnsLineEditXPos = GuiDimension::getLeftIndent() + areaSpawnsLabel->Width() + GuiDimension::getLeftOffSet();
  areaSpawnsValue = new PG_LineEdit(this, PG_Rect(valueXPos, areaSpawnsLabelYPos, GuiDimension::getLineEditWidth() , GetTextHeight()*2));
  areaSpawnsValue->SetText(int2String(actmap->weatherSystem->getSpawnsAmount()));   
  
  
  //Weather-Update each nth turn
  int nthTurnLabelYPos = areaSpawnsLabelYPos + GetTextHeight()*2;
  nthTurnLabel = new PG_Label(this, PG_Rect(GuiDimension::getLeftIndent(), nthTurnLabelYPos, xsize/3 , GetTextHeight()*2),
                              "Weather Update each nth Turn:");			      
  nthTurnLabel->SetSizeByText();  

  int nthTurnLineEditXPos = GuiDimension::getLeftIndent() + nthTurnLabel->Width() + GuiDimension::getLeftOffSet();
  nthTurnValue = new PG_LineEdit(this, PG_Rect(valueXPos, nthTurnLabelYPos, GuiDimension::getLineEditWidth() , GetTextHeight()*2));
  nthTurnValue->SetText(int2String(actmap->weatherSystem->getTimeInterval()));
  
  //Ratio windspeed to passed fields
  int windSpeedFieldRatioLabelYPos = nthTurnLabelYPos + GetTextHeight() * 2;
  windSpeedFieldRatioLabel = new PG_Label(this, PG_Rect(GuiDimension::getLeftIndent(), windSpeedFieldRatioLabelYPos, xsize/3 , GetTextHeight()*2), "WindSpeed to Fields Ratio:");
  windSpeedFieldRatioLabel->SetSizeByText();    
  windSpeedFieldRatioValue = new PG_LineEdit(this, PG_Rect(valueXPos, windSpeedFieldRatioLabelYPos, GuiDimension::getLineEditWidth() , GetTextHeight()*2));
  windSpeedFieldRatioValue->SetText(float2String(actmap->weatherSystem->getWindspeed2FieldRatio()));
  
  //LowerSize
  int lowerSizeLimitsYPos = windSpeedFieldRatioLabelYPos + GetTextHeight() * 2;
  lowerSizeLimitsLabel = new PG_Label(this, PG_Rect(GuiDimension::getLeftIndent(), lowerSizeLimitsYPos, xsize/3 , GetTextHeight()*2), "Lower Area Size rel. to Mapsize:"); 
  lowerSizeLimitsLabel->SetSizeByText();
  lowerSizeLimitsValue = new PG_LineEdit(this, PG_Rect(valueXPos ,lowerSizeLimitsYPos, GuiDimension::getLineEditWidth() , GetTextHeight()*2));
  lowerSizeLimitsValue->SetText(float2String(actmap->weatherSystem->getLowerSizeLimit()));
  
  //UpperSize
  int upperSizeLimitsYPos =  lowerSizeLimitsYPos + GetTextHeight() * 2;
  upperSizeLimitsLabel = new PG_Label(this, PG_Rect(GuiDimension::getLeftIndent(), upperSizeLimitsYPos, xsize/3 , GetTextHeight()*2), "Upper Area Size rel. to Mapsize:"); 
  upperSizeLimitsLabel->SetSizeByText();
  upperSizeLimitsValue = new PG_LineEdit(this, PG_Rect(valueXPos ,upperSizeLimitsYPos, GuiDimension::getLineEditWidth() , GetTextHeight()*2));
  upperSizeLimitsValue->SetText(float2String(actmap->weatherSystem->getUpperSizeLimit()));
  
  //FallOut
  int fallOutYPos = upperSizeLimitsYPos + GetTextHeight() * 2 + GuiDimension::getTopOffSet();;
  fallOutLabel = new PG_Label(this, PG_Rect(GuiDimension::getLeftIndent(), fallOutYPos, xsize/3 , GetTextHeight()*2), "Fallout Chances:");
  
  fallOutButton = new PG_Button(this, PG_Rect(valueXPos, fallOutYPos, (Width()- valueXPos)/2, 30), "Edit", 100);
  fallOutButton->sigClick.connect(SigC::slot( *this, &WeatherDialog::editFallOut ));
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
  actmap->weatherSystem->setTimeInterval(atoi(nthTurnValue->GetText()));  
  actmap->weatherSystem->setWindSpeed2FieldRatio(atof(windSpeedFieldRatioValue->GetText()));
  actmap->weatherSystem->setSpawnAmount(atoi(areaSpawnsValue->GetText()));
  quitModalLoop(1);

}

bool WeatherDialog::editFallOut( PG_Button* button ){

vector<string> labels;
labels.push_back("Dry");
labels.push_back("Light Rain");
labels.push_back("Heavy Rain");
labels.push_back("Light Snowfall");
labels.push_back("Heavy Snowfall");
labels.push_back("Icy");
ChanceSettingDialog fd(labels);
fd.Show();
fd.Run();

}
//*********************************************************************************************************++

const int ChanceSettingDialog::xsize = 400;
const int ChanceSettingDialog::ysize = 512;


ChanceSettingDialog::ChanceSettingDialog(const vector<string>& labelVec): ASC_PG_Dialog(NULL, PG_Rect( 100, 100, xsize, ysize ), "Weather Configuration", SHOW_CLOSE ){
  int yPos = 0;
  int valueXPos = xsize * 0.75;
  for(int i = 0; i < labelVec.size(); i++){
    yPos += GetTextHeight()*2;
    labels.push_back(new PG_Label(this, PG_Rect(GuiDimension::getLeftIndent(), yPos, xsize/3 , GetTextHeight()*2), labelVec[i]));
    chances.push_back(new PG_LineEdit(this, PG_Rect(valueXPos , yPos, GuiDimension::getLineEditWidth() , GetTextHeight()*2)));       
  }

 //Ok and Cancel Button
 (new PG_Button(this, PG_Rect(30, Height()-40, (Width()-70)/2, 30), "OK", 100))->sigClick.connect(SigC::slot( *this, &ChanceSettingDialog::buttonEvent ));
  (new PG_Button(this, PG_Rect(Width()/2+5, Height()-40, (Width()-70)/2, 30), "Cancel", 101))->sigClick.connect(SigC::slot( *this, &ChanceSettingDialog::closeWindow ));

  sigClose.connect( SigC::slot( *this, &ChanceSettingDialog::closeWindow ));
}

ChanceSettingDialog::~ChanceSettingDialog(){
  
}

bool ChanceSettingDialog::buttonEvent( PG_Button* button ){

    quitModalLoop(2);
    return true;
}
