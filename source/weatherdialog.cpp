
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
#include "typen.h"

const int WeatherDialog::xsize = 400;
const int WeatherDialog::ysize = 512;


WeatherDialog::WeatherDialog() :  ASC_PG_Dialog(NULL, PG_Rect( 100, 100, xsize, ysize ), "Weather Configuration", SHOW_CLOSE ) {

  //Random Mode
  randomMode = new PG_CheckButton(this, PG_Rect( GuiDimension::getLeftIndent(), GuiDimension::getTopOffSet(), xsize/3 , GetTextHeight()*2), "Random Mode");
  randomMode->SetSizeByText();
  if(actmap->weatherSystem->getEventMode() == RANDOMMODE) {
    randomMode->SetPressed();
  }

  //xPosition for edit Fields
  int valueXPos = static_cast<int>(xsize * 0.75);

  //Amount of Area Spawns
  int areaSpawnsLabelYPos = GuiDimension::getTopOffSet() + GetTextHeight()*2;
  areaSpawnsLabel = new PG_Label(this, PG_Rect(GuiDimension::getLeftIndent(), areaSpawnsLabelYPos, xsize/3 , GetTextHeight()*2),
                                 "Amount of Spawns:");
  areaSpawnsLabel->SetSizeByText();

  areaSpawnsValue = new PG_LineEdit(this, PG_Rect(valueXPos, areaSpawnsLabelYPos, GuiDimension::getLineEditWidth() , GetTextHeight()*2));
  areaSpawnsValue->SetText(int2String(actmap->weatherSystem->getSpawnsAmount()));


  //Weather-Update each nth turn
  int nthTurnLabelYPos = areaSpawnsLabelYPos + GetTextHeight()*2;
  nthTurnLabel = new PG_Label(this, PG_Rect(GuiDimension::getLeftIndent(), nthTurnLabelYPos, xsize/3 , GetTextHeight()*2),
                              "Weather Update each nth Turn:");
  nthTurnLabel->SetSizeByText();

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
  fallOutLabel->SetSizeByText();

  fallOutButton = new PG_Button(this, PG_Rect(valueXPos, fallOutYPos, (Width()- valueXPos)/2, 30), "Edit", 100);
  fallOutButton->sigClick.connect(SigC::slot( *this, &WeatherDialog::editFallOut ));

  //WindSpeed
  int windSpeedYPos =  fallOutYPos + GetTextHeight() * 2;
  windSpeedLabel = new PG_Label(this, PG_Rect(GuiDimension::getLeftIndent(), windSpeedYPos, xsize/3 , GetTextHeight()*2), "WindSpeed Chances:");
  windSpeedLabel->SetSizeByText();

  windSpeedButton = new PG_Button(this, PG_Rect(valueXPos, windSpeedYPos, (Width()- valueXPos)/2, 30), "Edit", 100);
  windSpeedButton->sigClick.connect(SigC::slot( *this, &WeatherDialog::editWindSpeed));

  //WindDirection
  int windDirYPos =  windSpeedYPos + GetTextHeight() * 2;
  windDirectionLabel = new PG_Label(this, PG_Rect(GuiDimension::getLeftIndent(), windDirYPos, xsize/3 , GetTextHeight()*2), "WindDirection Chances:");
  windDirectionLabel->SetSizeByText();

  windDirectionButton = new PG_Button(this, PG_Rect(valueXPos, windDirYPos, (Width()- valueXPos)/2, 30), "Edit", 100);
  windDirectionButton->sigClick.connect(SigC::slot( *this, &WeatherDialog::editWindDirection));

  //eventAreas
  int eventAreasYPos = windDirYPos + GetTextHeight() * 2 + GuiDimension::getTopOffSet();
  eventAreasLabel = new PG_Label(this, PG_Rect(GuiDimension::getLeftIndent(), eventAreasYPos, xsize/3 , GetTextHeight()*2), "Event driven weather:");
  eventAreasLabel->SetSizeByText();
  eventAreasButton = new PG_Button(this, PG_Rect(valueXPos, eventAreasYPos, (Width()- valueXPos)/2, 30), "Edit", 100);
  eventAreasButton->sigClick.connect(SigC::slot( *this, &WeatherDialog::editEventAreas));

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
  if(randomMode->GetPressed()) {
    actmap->weatherSystem->setEventMode(RANDOMMODE);
  } else {
    actmap->weatherSystem->setEventMode(EVENTMODE);
  }
  actmap->weatherSystem->setTimeInterval(atoi(nthTurnValue->GetText()));
  actmap->weatherSystem->setWindSpeed2FieldRatio(atof(windSpeedFieldRatioValue->GetText()));
  actmap->weatherSystem->setSpawnAmount(atoi(areaSpawnsValue->GetText()));
  quitModalLoop(1);
  return true;
}

bool WeatherDialog::editFallOut( PG_Button* button ) {
  FallOutSettingsDialog fd;
  fd.Show();
  fd.Run();
  return true;
}

bool WeatherDialog::editWindSpeed( PG_Button* button ) {
  WindSpeedSettingsDialog wsd;
  wsd.Show();
  wsd.Run();
  return true;
}

bool WeatherDialog::editWindDirection( PG_Button* button ) {
  WindDirectionSettingsDialog wdd;
  wdd.Show();
  wdd.Run();
  return true;
}

bool WeatherDialog::editEventAreas(PG_Button* button ) {
  EventAreasDialog ead;
  ead.Show();
  ead.Run();
  return true;
}

//*********************************************************************************************************
const int EventAreasDialog::xSize = 400;
const int EventAreasDialog::ySize = 400;

EventAreasDialog::EventAreasDialog() : ASC_PG_Dialog(NULL, PG_Rect( 100, 100, xSize, ySize ), "Edit eventdriven Areas", SHOW_CLOSE ) {
  int xPos1 = 20;
  //EventBox
  eventList = new PG_ListBox(this, PG_Rect(xPos1, 20, 350, 200));
  for(int i =0; i< actmap->weatherSystem->getQueuedWeatherAreasSize(); i ++) {
    pair<int, WeatherArea*> p = actmap->weatherSystem->getNthWeatherArea(i);
    WeatherAreaInformation* wai = new WeatherAreaInformation(p.second, p.first);
    currentList.push_back(wai);
    addNewWeatherAreaInformation(wai);

  }
  //updateAreaList();

  //add Button
  int yPosButtons = 20 + eventList->Height() + GuiDimension::getTopOffSet();
  addButton = new PG_Button(this, PG_Rect(xPos1, yPosButtons, 50, 35), "Add", 90);
  addButton->sigClick.connect(SigC::slot( *this, &EventAreasDialog::buttonAdd ));

  //remove Button
  removeButton = new PG_Button(this, PG_Rect(Width() - (xPos1 + 35), yPosButtons, 50, 35), "Remove", 90);
  removeButton->sigClick.connect(SigC::slot( *this, &EventAreasDialog::buttonRemove ));

  //Ok and Cancel Button
  (new PG_Button(this, PG_Rect(30, Height()-40, (Width()-70)/2, 30), "OK", 100))->sigClick.connect(SigC::slot( *this, &EventAreasDialog::buttonEvent ));
  (new PG_Button(this, PG_Rect(Width()/2+5, Height()-40, (Width()-70)/2, 30), "Cancel", 101))->sigClick.connect(SigC::slot( *this, &EventAreasDialog::closeWindow ));

  sigClose.connect( SigC::slot( *this, &EventAreasDialog::closeWindow ));

}

EventAreasDialog::~EventAreasDialog() {
}

bool EventAreasDialog::closeWindow() {
  quitModalLoop(2);
  return true;
};


bool EventAreasDialog::buttonEvent( PG_Button* button ) {
  for(list<WeatherAreaInformation*>::iterator it = addList.begin(); it != addList.end(); it++){
    WeatherAreaInformation* wai = *it;
    actmap->weatherSystem->addWeatherArea(wai->getWeatherArea(), wai->getTurn());
  
  }
  quitModalLoop(2);
  return true;
}

bool EventAreasDialog::buttonAdd( PG_Button* button ) {
  AddWeatherAreaDialog awad(this);
  awad.Show();
  awad.Run();  
  return true;
}

bool EventAreasDialog::buttonRemove( PG_Button* button ) {

  return true;
}

void EventAreasDialog::addNewWeatherAreaInformation(WeatherAreaInformation* wai){
  addList.push_back(wai);
  currentList.push_back(wai);
  new PG_ListBoxItem(eventList, 20, wai->getInformation(), NULL, wai);
}

//*********************************************************************************************************
WeatherAreaInformation::WeatherAreaInformation(WeatherArea* wa, int t, int d, FalloutType f): weatherArea(wa), turn(t), duration(d), fallOut(f){

}
WeatherAreaInformation::WeatherAreaInformation(WeatherArea* wa, int t): weatherArea(wa), turn(t){


}
WeatherAreaInformation::~WeatherAreaInformation(){

}
  
int WeatherAreaInformation::getTurn() const{
  return turn;
}
int WeatherAreaInformation::getDuration() const{
  return duration;

}
FalloutType WeatherAreaInformation::getFalloutType() const{
  return fallOut;
}

std::string WeatherAreaInformation::getInformation() const{
  std::string info = "turn ";
  info += WeatherDialog::int2String(turn);
  info.append("; ");
  info.append(cwettertypen[weatherArea->getFalloutType()]);
  info.append("; ");
  info.append("x: ");
  info.append(WeatherDialog::int2String(weatherArea->getCenterPos().getX()));
  info.append("; ");
  info.append("y: ");
  info.append(WeatherDialog::int2String(weatherArea->getCenterPos().getY()));
  
  return info;
}
//*********************************************************************************************************

const int AddWeatherAreaDialog::xSize = 400;
const int AddWeatherAreaDialog::ySize = 400;

const string SEPERATOR = ";";

AddWeatherAreaDialog::AddWeatherAreaDialog(EventAreasDialog* ead):  ASC_PG_Dialog(ead, PG_Rect( 0, 0, xSize, ySize ), "Add Weather Area", SHOW_CLOSE ) {
  int valueXPos = static_cast<int>(xSize * 0.75);
  
  turnLabel = new PG_Label(this, PG_Rect(GuiDimension::getLeftOffSet(), GuiDimension::getTopOffSet(), 10, GetTextHeight() * 2), "Turn");
  turnLabel->SetSizeByText();
  turnValue = new PG_LineEdit(this, PG_Rect(valueXPos,  GuiDimension::getTopOffSet(), GuiDimension::getLineEditWidth(), GetTextHeight() * 2));
  turnValue->SetValidKeys("1234567890");
  
  int durationYPos = GuiDimension::getTopOffSet() + GetTextHeight() * 2;
  durationLabel = new PG_Label(this, PG_Rect(GuiDimension::getLeftOffSet(), durationYPos, 10, GetTextHeight() * 2), "Duration");
  durationLabel->SetSizeByText();
  durationValue = new PG_LineEdit(this, PG_Rect(valueXPos,  durationYPos, GuiDimension::getLineEditWidth(), GetTextHeight() * 2));
  durationValue->SetValidKeys("1234567890");
  
  int xCoordYPos = durationYPos + + GetTextHeight() * 2;
  xCoordLabel = new PG_Label(this, PG_Rect(GuiDimension::getLeftOffSet(), xCoordYPos, 10, GetTextHeight() * 2), "xPosition");
  xCoordLabel->SetSizeByText();
  xCoordValue = new PG_LineEdit(this, PG_Rect(valueXPos,  xCoordYPos, GuiDimension::getLineEditWidth(), GetTextHeight() * 2));
  xCoordValue->SetValidKeys("1234567890");
  
  int yCoordYPos = xCoordYPos + + GetTextHeight() * 2;
  yCoordLabel = new PG_Label(this, PG_Rect(GuiDimension::getLeftOffSet(), yCoordYPos, 10, GetTextHeight() * 2), "yPosition");  yCoordLabel->SetSizeByText();
  yCoordValue = new PG_LineEdit(this, PG_Rect(valueXPos,  yCoordYPos, GuiDimension::getLineEditWidth(), GetTextHeight() * 2));
  yCoordValue->SetValidKeys("1234567890");
  
  int widthYPos = yCoordYPos + + GetTextHeight() * 2;
  widthLabel = new PG_Label(this, PG_Rect(GuiDimension::getLeftOffSet(), widthYPos, 10, GetTextHeight() * 2), "Width");  widthLabel->SetSizeByText();
  widthValue = new PG_LineEdit(this, PG_Rect(valueXPos, widthYPos, GuiDimension::getLineEditWidth(), GetTextHeight() * 2));
  widthValue->SetValidKeys("1234567890");
  
  int heightYPos = widthYPos + + GetTextHeight() * 2;
  heightLabel = new PG_Label(this, PG_Rect(GuiDimension::getLeftOffSet(), heightYPos, 10, GetTextHeight() * 2), "Height");  heightLabel->SetSizeByText();
  heightValue = new PG_LineEdit(this, PG_Rect(valueXPos, heightYPos, GuiDimension::getLineEditWidth(), GetTextHeight() * 2));
  heightValue->SetValidKeys("1234567890");
  
  int wTypesYPos = heightYPos + + GetTextHeight() * 2;
  wTypesLabel = new PG_Label(this, PG_Rect(GuiDimension::getLeftOffSet(), wTypesYPos, 10, GetTextHeight() * 2), "Weather Type"); 
  wTypesLabel->SetSizeByText();
  weatherTypes = new PG_DropDown(this, PG_Rect(valueXPos - valueXPos/2, wTypesYPos, GuiDimension::getLineEditWidth() *4, GetTextHeight() * 2));
  
  
  for(int i = 0; i < cwettertypennum; i++){
    weatherTypes->AddItem(cwettertypen[i]);
  
  }
  
  //Ok and Cancel Button
  (new PG_Button(this, PG_Rect(30, Height()-40, (Width()-70)/2, 30), "OK", 100))->sigClick.connect(SigC::slot( *this, &AddWeatherAreaDialog::buttonEvent ));
  (new PG_Button(this, PG_Rect(Width()/2+5, Height()-40, (Width()-70)/2, 30), "Cancel", 101))->sigClick.connect(SigC::slot( *this, &AddWeatherAreaDialog::closeWindow ));

  sigClose.connect( SigC::slot( *this, &AddWeatherAreaDialog::closeWindow ));

}

AddWeatherAreaDialog::~AddWeatherAreaDialog() {

}

bool AddWeatherAreaDialog::closeWindow() {
  quitModalLoop(2);
  return true;
}

bool AddWeatherAreaDialog::buttonEvent( PG_Button* button ) {
  WeatherArea* newWeatherArea = new WeatherArea(actmap, atoi(xCoordValue->GetText()), atoi(yCoordValue->GetText()), atoi(widthValue->GetText()),
  atoi(heightValue->GetText()), atoi(durationValue->GetText()), LRAIN);
  WeatherAreaInformation* wai = new WeatherAreaInformation(newWeatherArea, atoi(turnValue->GetText()));
  EventAreasDialog* ead = dynamic_cast<EventAreasDialog*>(this->GetParent());
  ead->addNewWeatherAreaInformation(wai);
  quitModalLoop(2);
  return true;
}

//*********************************************************************************************************

const int ChanceSettingsDialog::xsize = 400;
const int ChanceSettingsDialog::ysize = 512;


ChanceSettingsDialog::ChanceSettingsDialog(std::string title): ASC_PG_Dialog(NULL, PG_Rect( 100, 100, xsize, ysize ), title, SHOW_CLOSE ) {}

ChanceSettingsDialog::~ChanceSettingsDialog() {}

void ChanceSettingsDialog::buildUpForm(const vector<string>& labelVec) {
  int yPos = 0;
  int valueXPos = static_cast<int>(xsize * 0.75);
  for(int i = 0; i < labelVec.size(); i++) {
    yPos += GetTextHeight()*2;
    PG_Label* label = new PG_Label(this, PG_Rect(GuiDimension::getLeftIndent(), yPos, xsize/3 , GetTextHeight()*2), labelVec[i]);
    label->SetSizeByText();
    labels.push_back(label);
    PG_LineEdit* value = new PG_LineEdit(this, PG_Rect(valueXPos , yPos, GuiDimension::getLineEditWidth() , GetTextHeight()*2));
    value->SetText(WeatherDialog::int2String(getNthChanceValue(i)));
    chances.push_back(value);
  }
  int noteYPos = yPos + GuiDimension::getTopOffSet() + GetTextHeight()*2;
  note = new PG_Label(this, PG_Rect(GuiDimension::getLeftIndent(), noteYPos, xsize/3 , GetTextHeight()*2), "The sum of chances must equal 100");
  note->SetSizeByText();
  //Ok and Cancel Button
  (new PG_Button(this, PG_Rect(30, Height()-40, (Width()-70)/2, 30), "OK", 100))->sigClick.connect(SigC::slot( *this, &ChanceSettingsDialog::buttonEvent ));
  (new PG_Button(this, PG_Rect(Width()/2+5, Height()-40, (Width()-70)/2, 30), "Cancel", 101))->sigClick.connect(SigC::slot( *this, &ChanceSettingsDialog::closeWindow ));

  sigClose.connect( SigC::slot( *this, &ChanceSettingsDialog::closeWindow ));

}


bool ChanceSettingsDialog::buttonEvent( PG_Button* button ) {

  quitModalLoop(2);
  return true;
}

//*********************************************************************************************************++
FallOutSettingsDialog::FallOutSettingsDialog():ChanceSettingsDialog("Fallout Chances") {
  vector<string> labels;
  labels.push_back("Dry");
  labels.push_back("Light Rain");
  labels.push_back("Heavy Rain");
  labels.push_back("Light Snowfall");
  labels.push_back("Heavy Snowfall");
  labels.push_back("Icy");

  buildUpForm(labels);

}

FallOutSettingsDialog::~FallOutSettingsDialog() {}

void FallOutSettingsDialog::setChanceValues(const vector<PG_LineEdit*>& p) {
  try {
    Percentages c;
    for(int i = 0; i < chances.size(); i++) {
      c.push_back(atoi(chances[i]->GetText()));
    }
    actmap->weatherSystem->setLikelihoodFallOut(c);
  } catch(IllegalValueException& e) {
    cout << e.getMessage() << endl;
  }
}

int FallOutSettingsDialog::getNthChanceValue(int n) {
  return actmap->weatherSystem->getFalloutPercentage(n);
}

bool FallOutSettingsDialog::buttonEvent( PG_Button* button ) {
  setChanceValues(chances);
  quitModalLoop(2);
  return true;
}


//****************************************************************************************************************

WindSpeedSettingsDialog::WindSpeedSettingsDialog():ChanceSettingsDialog("Windspeed Chances") {
  vector<string> labels;
  int size = WeatherSystem::WINDSPEEDDETAILLEVEL;
  int slotSize = maxwindspeed / size;
  int value =0;
  for (int i = 0; i < size; i++) {
    string label = WeatherDialog::int2String(value);
    label += " - ";
    label += WeatherDialog::int2String(value + slotSize);
    labels.push_back(label);
    value+= slotSize;
  }
  buildUpForm(labels);
}


WindSpeedSettingsDialog::~WindSpeedSettingsDialog() {}

void WindSpeedSettingsDialog::setChanceValues(const vector<PG_LineEdit*>& p) {
  try {
    Percentages c;
    for(int i = 0; i < chances.size(); i++) {
      c.push_back(atoi(chances[i]->GetText()));
    }
    actmap->weatherSystem->setLikelihoodWindSpeed(c);
  } catch(IllegalValueException& e) {
    cout << e.getMessage() << endl; //If available rewrite to use of MsgBox
  }
}

int WindSpeedSettingsDialog::getNthChanceValue(int n) {
  return actmap->weatherSystem->getWindSpeedPercentage(n);
}

bool WindSpeedSettingsDialog::buttonEvent( PG_Button* button ) {
  setChanceValues(chances);
  quitModalLoop(2);
  return true;
}


//*********************************************************************************************************++
WindDirectionSettingsDialog::WindDirectionSettingsDialog():ChanceSettingsDialog("Fallout Chances") {
  vector<string> labels;
  labels.push_back("North");
  labels.push_back("NorthEast");
  labels.push_back("East");
  labels.push_back("SouthEast");
  labels.push_back("South");
  labels.push_back("SouthWest");
  labels.push_back("West");
  labels.push_back("NorthWest");

  buildUpForm(labels);

}

WindDirectionSettingsDialog::~WindDirectionSettingsDialog() {}

void WindDirectionSettingsDialog::setChanceValues(const vector<PG_LineEdit*>& p) {
  try {
    Percentages c;
    for(int i = 0; i < chances.size(); i++) {
      c.push_back(atoi(chances[i]->GetText()));
    }
    actmap->weatherSystem->setLikelihoodWindDirection(c);
  } catch(IllegalValueException& e) {
    cout << e.getMessage() << endl;
  }
}

int WindDirectionSettingsDialog::getNthChanceValue(int n) {
  return actmap->weatherSystem->getWindDirPercentage(n);
}

bool WindDirectionSettingsDialog::buttonEvent( PG_Button* button ) {
  setChanceValues(chances);
  quitModalLoop(2);
  return true;
}

