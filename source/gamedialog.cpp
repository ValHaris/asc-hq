//
// C++ Implementation: gamedialog
//
// Description: 
//
//
// Author: Kevin Hirschmann <hirsch@dhcppc0>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "gamedialog.h"
#include "gamedlg.h"
#include "guidimension.h"

const int GameDialog::xSize = 450;
const int GameDialog::ySize = 500;
const int GameDialog::buttonIndent = 150;

GameDialog::GameDialog():  ASC_PG_Dialog(NULL, PG_Rect( 200, 100, xSize, ySize ), "Game", SHOW_CLOSE )
{
  singlePlayerButton = new PG_Button(this, PG_Rect(buttonIndent, GuiDimension::getTopOffSet()*2, 150, GuiDimension::getButtonHeight()), "Single Player", 90);
  singlePlayerButton->sigClick.connect( SigC::slot( *this, &GameDialog::singleGame)); 
 
 PG_Point p = ScreenToClient(singlePlayerButton->x, singlePlayerButton->y);
 multiPlayerButton = new PG_Button(this, PG_Rect(p.x, p.y + GuiDimension::getButtonHeight() + GuiDimension::getTopOffSet() , 150, GuiDimension::getButtonHeight()), "MulitPlayer", 90);
 multiPlayerButton->sigClick.connect( SigC::slot( *this, &GameDialog::multiGame)); 
 
 p = ScreenToClient(multiPlayerButton->x, multiPlayerButton->y);
 loadGameButton = new PG_Button(this, PG_Rect(p.x, p.y + GuiDimension::getButtonHeight() + GuiDimension::getTopOffSet() , 150, GuiDimension::getButtonHeight()), "Load Game", 90);
 loadGameButton->sigClick.connect( SigC::slot( *this, &GameDialog::loadGame)); 
 
 p = ScreenToClient(loadGameButton->x, loadGameButton->y);
 saveGameButton = new PG_Button(this, PG_Rect(p.x, p.y + GuiDimension::getButtonHeight() + GuiDimension::getTopOffSet() , 150, GuiDimension::getButtonHeight()), "Save Game", 90);
 saveGameButton->sigClick.connect( SigC::slot( *this, &GameDialog::saveGame)); 
 
 p = ScreenToClient(saveGameButton->x, saveGameButton->y);
 optionsButton = new PG_Button(this, PG_Rect(p.x, p.y + GuiDimension::getButtonHeight() + GuiDimension::getTopOffSet() , 150, GuiDimension::getButtonHeight()), "Options", 90);
 optionsButton->sigClick.connect( SigC::slot( *this, &GameDialog::showOptions));
 
 
 p = ScreenToClient(optionsButton->x, optionsButton->y);
 exitButton  = new PG_Button(this, PG_Rect(p.x, p.y + GuiDimension::getButtonHeight() + GuiDimension::getTopOffSet() , 150, GuiDimension::getButtonHeight()), "End Game", 90);
 exitButton->sigClick.connect( SigC::slot( *this, &GameDialog::exitGame));
 
 p = ScreenToClient(exitButton->x, exitButton->y);
 continueButton = new PG_Button(this, PG_Rect(p.x, p.y + GuiDimension::getButtonHeight() + 2 * GuiDimension::getTopOffSet() , 150, GuiDimension::getButtonHeight()), "Continue Game", 90);
 continueButton->sigClick.connect( SigC::slot( *this, &GameDialog::closeWindow ));
 
  sigClose.connect( SigC::slot( *this, &GameDialog::closeWindow ));
}



GameDialog::~GameDialog(){


}

bool GameDialog::closeWindow(){
quitModalLoop(1);

return true;
}

bool GameDialog::exitGame(PG_Button* button){
Hide();
confirmExitDialog();
quitModalLoop(1);
return true;
}

bool GameDialog::showOptions(PG_Button* button){
Hide();
optionsDialog();
Show();
return true;
}

bool GameDialog::saveGame(PG_Button* button){
Hide();

Show();
return true;
}
  
bool GameDialog::loadGame(PG_Button* button){
Hide();

Show();
return true;
}

bool GameDialog::singleGame(PG_Button* button){
Hide();
singlePlayerDialog();
Show();
return true;
}

bool GameDialog::multiGame(PG_Button* button){
Hide();
multiPlayerDialog();
Show();
return true;
}
  
extern void gameDialog() {
    GameDialog gd;
    gd.Show();
    gd.Run();
}


//*******************************************************************************************************************+
const int ConfirmExitDialog::xSize = 450;
const int ConfirmExitDialog::ySize = 150;


ConfirmExitDialog::ConfirmExitDialog(): ASC_PG_Dialog(NULL, PG_Rect( 200, 100, xSize, ySize ), "End Game", MODAL ){

   okButton = new PG_Button(this, PG_Rect(80, GuiDimension::getTopOffSet()*2, 150, GuiDimension::getButtonHeight()), "OK", 90);
   okButton->sigClick.connect( SigC::slot( *this, &ConfirmExitDialog::exitGame ));
   
   PG_Point p = ScreenToClient(okButton->x, okButton->y);  
   cancelButton = new PG_Button(this, PG_Rect(p.x + 150, p.y, 150, GuiDimension::getButtonHeight()), "Cancel", 90);
   cancelButton->sigClick.connect( SigC::slot( *this, &ConfirmExitDialog::closeWindow ));
}


ConfirmExitDialog::~ConfirmExitDialog(){


}

bool ConfirmExitDialog::closeWindow(){
quitModalLoop(1);

return true;
}

bool ConfirmExitDialog::exitGame(PG_Button* button){

return true;
}

extern void confirmExitDialog() {
    ConfirmExitDialog ced;
    ced.Show();
    ced.Run();
}

//*******************************************************************************************************************+

const int SinglePlayerDialog::xSize = 450;
const int SinglePlayerDialog::ySize = 250;
const int SinglePlayerDialog::buttonIndent = 150;

SinglePlayerDialog::SinglePlayerDialog(): ASC_PG_Dialog(NULL, PG_Rect( 200, 100, xSize, ySize ), "Single Player", SHOW_CLOSE ){
   
    campaignButton = new PG_Button(this, PG_Rect(buttonIndent, GuiDimension::getTopOffSet()*2, 150, GuiDimension::getButtonHeight()), "Campaign", 90);
    campaignButton->sigClick.connect( SigC::slot( *this, &SinglePlayerDialog::campaign ));
 
    PG_Point p = ScreenToClient(campaignButton->x, campaignButton->y);    
    singleLevelButton = new PG_Button(this, PG_Rect(p.x,  p.y + GuiDimension::getButtonHeight() + GuiDimension::getTopOffSet(), 150, GuiDimension::getButtonHeight()), "Single Level", 90);
    singleLevelButton->sigClick.connect( SigC::slot( *this, &SinglePlayerDialog::singleLevel ));
   
    p = ScreenToClient(singleLevelButton->x, singleLevelButton->y);    
    cancelButton = new PG_Button(this, PG_Rect(p.x,  p.y + GuiDimension::getButtonHeight() + GuiDimension::getTopOffSet() * 2, 150, GuiDimension::getButtonHeight()), "Cancel", 90);
    cancelButton->sigClick.connect( SigC::slot( *this, &SinglePlayerDialog::closeWindow ));
   
    sigClose.connect( SigC::slot( *this, &SinglePlayerDialog::closeWindow ));
}


SinglePlayerDialog::~SinglePlayerDialog(){


}

bool SinglePlayerDialog::closeWindow(){
quitModalLoop(1);
return true;
}

bool SinglePlayerDialog::campaign(PG_Button* button){
  Hide();

  Show();
return true;
}

bool SinglePlayerDialog::singleLevel(PG_Button* button){
Hide();

Show();
return true;
}


extern void singlePlayerDialog() {
    SinglePlayerDialog spd;
    spd.Show();
    spd.Run();
}

//*******************************************************************************************************************+

const int MultiPlayerDialog::xSize = 450;
const int MultiPlayerDialog::ySize = 300;
const int MultiPlayerDialog::buttonIndent = 140;

MultiPlayerDialog::MultiPlayerDialog(): ASC_PG_Dialog(NULL, PG_Rect( 200, 100, xSize, ySize ), "Multi Player", SHOW_CLOSE ){
   
    continueGameButton = new PG_Button(this, PG_Rect(buttonIndent, GuiDimension::getTopOffSet()*2, GuiDimension::getButtonWidth(), GuiDimension::getButtonHeight()), "Continue Network Game", 90);
    continueGameButton->sigClick.connect( SigC::slot( *this, &MultiPlayerDialog::continueGame ));
 
    PG_Point p = ScreenToClient(continueGameButton->x, continueGameButton->y);    
    superViseButton = new PG_Button(this, PG_Rect(p.x,  p.y + GuiDimension::getButtonHeight() + GuiDimension::getTopOffSet(), GuiDimension::getButtonWidth(), GuiDimension::getButtonHeight()), "Supervise Network Game", 90);
    superViseButton->sigClick.connect( SigC::slot( *this, &MultiPlayerDialog::superVise ));
   
    p = ScreenToClient(superViseButton->x, superViseButton->y);    
    setupNWButton = new PG_Button(this, PG_Rect(p.x,  p.y + GuiDimension::getButtonHeight() + GuiDimension::getTopOffSet(), GuiDimension::getButtonWidth(), GuiDimension::getButtonHeight()), "Setup Network Game", 90);
    setupNWButton->sigClick.connect( SigC::slot( *this, &MultiPlayerDialog::closeWindow ));
    
    p = ScreenToClient(setupNWButton->x, setupNWButton->y);    
    cancelButton = new PG_Button(this, PG_Rect(p.x,  p.y + GuiDimension::getButtonHeight() + GuiDimension::getTopOffSet() * 2, GuiDimension::getButtonWidth(), GuiDimension::getButtonHeight()), "Cancel", 90);
    cancelButton->sigClick.connect( SigC::slot( *this, &MultiPlayerDialog::closeWindow ));
    
    cancelButton->sigClick.connect( SigC::slot( *this, &MultiPlayerDialog::closeWindow ));
   
    sigClose.connect( SigC::slot( *this, &MultiPlayerDialog::closeWindow ));
}


MultiPlayerDialog::~MultiPlayerDialog(){


}

bool MultiPlayerDialog::closeWindow(){
quitModalLoop(1);
return true;
}

bool MultiPlayerDialog::continueGame(PG_Button* button){
  Hide();
  
  Show();
return true;
}

bool MultiPlayerDialog::superVise(PG_Button* button){
Hide();
Show();
return true;
}
/*
bool MultiPlayerDialog::showOtherOptions(PG_Button* button){
Hide();
gamepreferences();
Show();
return true;
}*/

extern void multiPlayerDialog() {
    MultiPlayerDialog spd;
    spd.Show();
    spd.Run();
}


//*******************************************************************************************************************+
const int OptionsDialog::xSize = 450;
const int OptionsDialog::ySize = 300;
const int OptionsDialog::buttonIndent = 150;

OptionsDialog::OptionsDialog(): ASC_PG_Dialog(NULL, PG_Rect( 200, 100, xSize, ySize ), "End Game", SHOW_CLOSE ){
   
    soundButton = new PG_Button(this, PG_Rect(buttonIndent, GuiDimension::getTopOffSet()*2, 150, GuiDimension::getButtonHeight()), "Sound Options", 90);
    soundButton->sigClick.connect( SigC::slot( *this, &OptionsDialog::showSoundOptions ));
 
    PG_Point p = ScreenToClient(soundButton->x, soundButton->y);    
    mouseButton = new PG_Button(this, PG_Rect(p.x,  p.y + GuiDimension::getButtonHeight() + GuiDimension::getTopOffSet(), 150, GuiDimension::getButtonHeight()), "Mouse Options", 90);
    mouseButton->sigClick.connect( SigC::slot( *this, &OptionsDialog::showMouseOptions ));
   
   p = ScreenToClient(mouseButton->x, mouseButton->y);    
   otherButton = new PG_Button(this, PG_Rect(p.x,  p.y + GuiDimension::getButtonHeight() + GuiDimension::getTopOffSet(), 150, GuiDimension::getButtonHeight()), "Other Options", 90);
   otherButton->sigClick.connect( SigC::slot( *this, &OptionsDialog::showOtherOptions ));
   
   sigClose.connect( SigC::slot( *this, &OptionsDialog::closeWindow ));
}


OptionsDialog::~OptionsDialog(){


}

bool OptionsDialog::closeWindow(){
quitModalLoop(1);

return true;
}

bool OptionsDialog::showSoundOptions(PG_Button* button){
  Hide();
  soundSettings();
  Show();
return true;
}

bool OptionsDialog::showMouseOptions(PG_Button* button){
Hide();
mousepreferences();
Show();
return true;
}

bool OptionsDialog::showOtherOptions(PG_Button* button){
Hide();
gamepreferences();
Show();
return true;
}

extern void optionsDialog() {
    OptionsDialog od;
    od.Show();
    od.Run();
}


