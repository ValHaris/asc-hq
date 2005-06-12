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
#include "sdl/sound.h"
#include "misc.h"
//#include "basestrm.h"

const int GameDialog::xSize = 450;
const int GameDialog::ySize = 500;
const int GameDialog::buttonIndent = 150;
GameDialog* GameDialog::instance = 0;

GameDialog::GameDialog():  ASC_PG_Dialog(NULL, PG_Rect( 200, 100, xSize, ySize ), "Game", SHOW_CLOSE ) {
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
    SetInputFocus();
}



GameDialog::~GameDialog() {
}
bool GameDialog::handleEventKeyUp (const SDL_KeyboardEvent *key) {    
   if(key->keysym.sym == SDLK_ESCAPE) {
        closeWindow();
    }
    return true;
}



bool GameDialog::closeWindow() {
    quitModalLoop(1);

    return true;
}

bool GameDialog::exitGame(PG_Button* button) {
    Hide();
    ConfirmExitDialog::confirmExitDialog(this);
    quitModalLoop(1);
    return true;
}

bool GameDialog::showOptions(PG_Button* button) {
    Hide();
    OptionsDialog::optionsDialog(this);
    Show();
    return true;
}

bool GameDialog::saveGame(PG_Button* button) {
    Hide();
    SaveGameDialog::saveGameDialog(this);
    Show();
    return true;
}

bool GameDialog::loadGame(PG_Button* button) {
    Hide();
    LoadGameDialog::loadGameDialog(this);
    Show();
    return true;
}

bool GameDialog::singleGame(PG_Button* button) {
    Hide();
    SinglePlayerDialog::singlePlayerDialog(this);
    Show();
    return true;
}

bool GameDialog::multiGame(PG_Button* button) {
    Hide();
    MultiPlayerDialog::multiPlayerDialog(this);
    Show();
    return true;
}


bool GameDialog::gameDialog(const SDL_KeyboardEvent *key) {
    if(GameDialog::instance == 0) {
        GameDialog* gd = new GameDialog();
        instance = gd;
        gd->Show();
        gd->RunModal();
        delete gd;
        GameDialog::instance = 0;
    }
    return true;
}


//*******************************************************************************************************************+
const int ConfirmExitDialog::xSize = 450;
const int ConfirmExitDialog::ySize = 120;


ConfirmExitDialog::ConfirmExitDialog(PG_MessageObject* c): ASC_PG_Dialog(NULL, PG_Rect( 200, 100, xSize, ySize ), "End Game", MODAL ) {

    okButton = new PG_Button(this, PG_Rect(80, GuiDimension::getTopOffSet()*2, 150, GuiDimension::getButtonHeight()), "OK", 90);
    okButton->sigClick.connect( SigC::slot( *this, &ConfirmExitDialog::exitGame ));

    PG_Point p = ScreenToClient(okButton->x, okButton->y);
    cancelButton = new PG_Button(this, PG_Rect(p.x + 150 + 10, p.y, 150, GuiDimension::getButtonHeight()), "Cancel", 90);
    cancelButton->sigClick.connect( SigC::slot( *this, &ConfirmExitDialog::closeWindow ));
}


ConfirmExitDialog::~ConfirmExitDialog() {
}

bool ConfirmExitDialog::closeWindow() {
    quitModalLoop(1);

    return true;
}

bool ConfirmExitDialog::exitGame(PG_Button* button) {
    return true;
}

void ConfirmExitDialog::confirmExitDialog(PG_MessageObject* caller) {
    ConfirmExitDialog ced(caller);
    ced.Show();
    ced.RunModal();
}

//*******************************************************************************************************************+

const int SinglePlayerDialog::xSize = 450;
const int SinglePlayerDialog::ySize = 250;
const int SinglePlayerDialog::buttonIndent = 150;
SinglePlayerDialog* SinglePlayerDialog::instance = 0;

SinglePlayerDialog::SinglePlayerDialog( PG_MessageObject * c): ASC_PG_Dialog(NULL, PG_Rect( 200, 100, xSize, ySize ), "Single Player", SHOW_CLOSE ) {

    campaignButton = new PG_Button(this, PG_Rect(buttonIndent, GuiDimension::getTopOffSet()*2, 150, GuiDimension::getButtonHeight()), "Campaign", 90);
    campaignButton->sigClick.connect( SigC::slot( *this, &SinglePlayerDialog::campaign ));

    PG_Point p = ScreenToClient(campaignButton->x, campaignButton->y);
    singleLevelButton = new PG_Button(this, PG_Rect(p.x,  p.y + GuiDimension::getButtonHeight() + GuiDimension::getTopOffSet(), 150, GuiDimension::getButtonHeight()), "Single Level", 90);
    singleLevelButton->sigClick.connect( SigC::slot( *this, &SinglePlayerDialog::singleLevel ));

    p = ScreenToClient(singleLevelButton->x, singleLevelButton->y);
    cancelButton = new PG_Button(this, PG_Rect(p.x,  p.y + GuiDimension::getButtonHeight() + GuiDimension::getTopOffSet() * 2, 150, GuiDimension::getButtonHeight()), "Cancel", 90);
    cancelButton->sigClick.connect( SigC::slot( *this, &SinglePlayerDialog::closeWindow ));

    sigClose.connect( SigC::slot( *this, &SinglePlayerDialog::closeWindow ));
    caller = c;
    SetInputFocus();
}


SinglePlayerDialog::~SinglePlayerDialog() {
}

bool SinglePlayerDialog::closeWindow() {
    quitModalLoop(1);
    caller->SetInputFocus();
    return true;
}

bool SinglePlayerDialog::campaign(PG_Button* button) {
    Hide();

    Show();
    return true;
}

bool SinglePlayerDialog::singleLevel(PG_Button* button) {
    Hide();

    Show();
    return true;
}


void SinglePlayerDialog::singlePlayerDialog(PG_MessageObject* caller) {
    if(SinglePlayerDialog::instance == 0) {
        SinglePlayerDialog* spd = new SinglePlayerDialog(caller);
        SinglePlayerDialog::instance = spd;
        spd->Show();
        spd->RunModal();
        delete spd;
        SinglePlayerDialog::instance = 0;
    }
}

//*******************************************************************************************************************+

const int MultiPlayerDialog::xSize = 450;
const int MultiPlayerDialog::ySize = 350;
const int MultiPlayerDialog::buttonIndent = 140;

MultiPlayerDialog::MultiPlayerDialog(PG_MessageObject* c): ASC_PG_Dialog(NULL, PG_Rect( 200, 100, xSize, ySize ), "Multi Player", SHOW_CLOSE ) {

    continueGameButton = new PG_Button(this, PG_Rect(buttonIndent, GuiDimension::getTopOffSet()*2, GuiDimension::getButtonWidth(), GuiDimension::getButtonHeight()), "Continue Network Game", 90);
    continueGameButton->sigClick.connect( SigC::slot( *this, &MultiPlayerDialog::continueGame ));

    PG_Point p = ScreenToClient(continueGameButton->x, continueGameButton->y);
    superViseButton = new PG_Button(this, PG_Rect(p.x,  p.y + GuiDimension::getButtonHeight() + GuiDimension::getTopOffSet(), GuiDimension::getButtonWidth(), GuiDimension::getButtonHeight()), "Supervise Network Game", 90);
    superViseButton->sigClick.connect( SigC::slot( *this, &MultiPlayerDialog::superVise ));

    p = ScreenToClient(superViseButton->x, superViseButton->y);
    setupNWButton = new PG_Button(this, PG_Rect(p.x,  p.y + GuiDimension::getButtonHeight() + GuiDimension::getTopOffSet(), GuiDimension::getButtonWidth(), GuiDimension::getButtonHeight()), "Setup Network Game", 90);
    setupNWButton->sigClick.connect( SigC::slot( *this, &MultiPlayerDialog::closeWindow ));
    
     p = ScreenToClient(setupNWButton->x, setupNWButton->y);
    changeMapPasswordButton = new PG_Button(this, PG_Rect(p.x,  p.y + GuiDimension::getButtonHeight() + GuiDimension::getTopOffSet(), GuiDimension::getButtonWidth(), GuiDimension::getButtonHeight()), "Change Password", 90);
    changeMapPasswordButton->sigClick.connect( SigC::slot( *this, &MultiPlayerDialog::changeMapPassword ));

    p = ScreenToClient(changeMapPasswordButton->x, changeMapPasswordButton->y);
    cancelButton = new PG_Button(this, PG_Rect(p.x,  p.y + GuiDimension::getButtonHeight() + GuiDimension::getTopOffSet() * 2, GuiDimension::getButtonWidth(), GuiDimension::getButtonHeight()), "Cancel", 90);
    cancelButton->sigClick.connect( SigC::slot( *this, &MultiPlayerDialog::closeWindow ));

    cancelButton->sigClick.connect( SigC::slot( *this, &MultiPlayerDialog::closeWindow ));

    sigClose.connect( SigC::slot( *this, &MultiPlayerDialog::closeWindow ));

    caller = c;
    SetInputFocus();
}


MultiPlayerDialog::~MultiPlayerDialog() {
}

bool MultiPlayerDialog::closeWindow() {
    quitModalLoop(1);
    caller->SetInputFocus();
    return true;
}

bool MultiPlayerDialog::continueGame(PG_Button* button) {
    Hide();

    Show();
    return true;
}

bool MultiPlayerDialog::changeMapPassword(PG_Button* button) {
    Hide();
    ChangeMapPasswordDialog::changeMapPasswordDialog(this);
    Show();
    return true;
}

bool MultiPlayerDialog::superVise(PG_Button* button) {
    Hide();
    Show();
    return true;
}


void MultiPlayerDialog::multiPlayerDialog(PG_MessageObject* c) {
    MultiPlayerDialog spd(c);
    spd.Show();
    spd.RunModal();
}


//*******************************************************************************************************************+
const int OptionsDialog::xSize = 450;
const int OptionsDialog::ySize = 400;
const int OptionsDialog::buttonIndent = 150;

OptionsDialog::OptionsDialog(PG_MessageObject* c ): ASC_PG_Dialog(NULL, PG_Rect( 200, 100, xSize, ySize ), "Options", SHOW_CLOSE ) {

    soundButton = new PG_Button(this, PG_Rect(buttonIndent, GuiDimension::getTopOffSet()*2, 150, GuiDimension::getButtonHeight()), "Sound Options", 90);
    soundButton->sigClick.connect( SigC::slot( *this, &OptionsDialog::showSoundOptions ));

    PG_Point p = ScreenToClient(soundButton->x, soundButton->y);
    mouseButton = new PG_Button(this, PG_Rect(p.x,  p.y + GuiDimension::getButtonHeight() + GuiDimension::getTopOffSet(), 150, GuiDimension::getButtonHeight()), "Mouse Options", 90);
    mouseButton->sigClick.connect( SigC::slot( *this, &OptionsDialog::showMouseOptions ));

    p = ScreenToClient(mouseButton->x, mouseButton->y);
    otherButton = new PG_Button(this, PG_Rect(p.x,  p.y + GuiDimension::getButtonHeight() + GuiDimension::getTopOffSet(), 150, GuiDimension::getButtonHeight()), "Game Options", 90);
    otherButton->sigClick.connect( SigC::slot( *this, &OptionsDialog::showOtherOptions ));

    p = ScreenToClient(otherButton->x, otherButton->y);
    displayButton = new PG_Button(this, PG_Rect(p.x,  p.y + GuiDimension::getButtonHeight() + GuiDimension::getTopOffSet(), 150, GuiDimension::getButtonHeight()), "Display Options", 90);
    displayButton->sigClick.connect( SigC::slot( *this, &OptionsDialog::showDisplayOptions ));

    p = ScreenToClient(displayButton->x, displayButton->y);
    PG_Button* okButton = new PG_Button(this, PG_Rect(p.x,  p.y + GuiDimension::getButtonHeight() + GuiDimension::getTopOffSet() * 2, 150, GuiDimension::getButtonHeight()), "Back", 90);
    okButton->sigClick.connect( SigC::slot( *this, &OptionsDialog::closeWindow ));

    sigClose.connect( SigC::slot( *this, &OptionsDialog::closeWindow ));

    caller = c;
    SetInputFocus();
}


OptionsDialog::~OptionsDialog() {
}

bool OptionsDialog::closeWindow() {
    quitModalLoop(1);
    caller->SetInputFocus();
    return true;
}

bool OptionsDialog::showSoundOptions(PG_Button* button) {
    Hide();
    SoundSettings::soundSettings(this);
    Show();
    return true;
}

bool OptionsDialog::showDisplayOptions(PG_Button* button) {
    Hide();
    DisplayOptionsDialog::displayOptionsDialog(this);
    Show();
    return true;
}

bool OptionsDialog::showMouseOptions(PG_Button* button) {
    Hide();
    MousePreferencesDialog::mousePreferencesDialog(this);
    Show();
    return true;
}

bool OptionsDialog::showOtherOptions(PG_Button* button) {
    Hide();
    GameOptionsDialog::gameOptionsDialog(this);
    Show();
    return true;
}

void OptionsDialog::optionsDialog(PG_MessageObject* caller) {
    OptionsDialog od(caller);
    od.Show();
    od.RunModal();
}

//*******************************************************************************************************************+
const int DisplayOptionsDialog::xSize = 450;
const int DisplayOptionsDialog::ySize = 220;
const int DisplayOptionsDialog::buttonIndent = 60;

DisplayOptionsDialog::DisplayOptionsDialog(PG_MessageObject* c): ASC_PG_Dialog(NULL, PG_Rect( 200, 100, xSize, ySize ), "Display Options",
        SHOW_CLOSE ) {

    mouseTipLabel = new PG_Label(this, PG_Rect(GuiDimension::getLeftIndent(), static_cast<int>(GuiDimension::getTopOffSet() * 1.5), 10, GetTextHeight() * 2), "Mouse Tip Delay (1/100 sec)");
    mouseTipLabel->SetSizeByText();

    PG_Point p = ScreenToClient(mouseTipLabel->x, mouseTipLabel->y);
    PG_LineEdit* mouseTipValue = new PG_LineEdit(this, PG_Rect(xSize - (GuiDimension::getLineEditWidth() + GuiDimension::getLeftIndent()), p.y, GuiDimension::getLineEditWidth(), GetTextHeight() * 2));
    mouseTipValue->SetText(strrr(CGameOptions::Instance()->CGameOptions::Instance()->onlinehelptime));
    mouseTipValue->SetValidKeys("1234567890");

    p = ScreenToClient(mouseTipLabel->x, mouseTipLabel->y);
    movementSpeedLabel = new PG_Label(this, PG_Rect(GuiDimension::getLeftIndent(), p.y + GuiDimension::getTopOffSet() + GetTextHeight() * 2 , 10, GetTextHeight() * 2), "Movement Speed (1/100 sec)");
    movementSpeedLabel->SetSizeByText();    

    movementSpeedValue = new PG_LineEdit(this, PG_Rect(xSize - (GuiDimension::getLineEditWidth() + GuiDimension::getLeftIndent()), p.y + GuiDimension::getTopOffSet() + static_cast<int>(GetTextHeight() * 1.5), GuiDimension::getLineEditWidth(), GetTextHeight() * 2));
    movementSpeedValue->SetText(strrr(CGameOptions::Instance()->CGameOptions::Instance()->movespeed));
    movementSpeedValue->SetValidKeys("1234567890");

    okButton = new PG_Button(this, PG_Rect( buttonIndent, ySize - (GuiDimension::getButtonHeight() + GuiDimension::getTopOffSet()), GuiDimension::getButtonWidth(), GuiDimension::getButtonHeight()), "OK", 90);
    okButton->sigClick.connect( SigC::slot( *this, &DisplayOptionsDialog::ok ));

    p = ScreenToClient(okButton->x, okButton->y);
    cancelButton = new PG_Button(this, PG_Rect(p.x + GuiDimension::getButtonWidth() + 10, p.y, GuiDimension::getButtonWidth(), GuiDimension::getButtonHeight()), "Cancel", 90);
    cancelButton->sigClick.connect( SigC::slot( *this, &DisplayOptionsDialog::closeWindow ));

    sigClose.connect( SigC::slot( *this, &DisplayOptionsDialog::closeWindow ));

    caller = c;
    SetInputFocus();
}


DisplayOptionsDialog::~DisplayOptionsDialog() {
}

bool DisplayOptionsDialog::closeWindow() {
    quitModalLoop(1);
    caller->SetInputFocus();
    return true;
}

bool DisplayOptionsDialog::ok(PG_Button* button) {
    CGameOptions::Instance()->CGameOptions::Instance()->onlinehelptime = atoi(mouseTipValue->GetText());
    CGameOptions::Instance()->CGameOptions::Instance()->movespeed = atoi(movementSpeedValue->GetText());
    return true;
}

void DisplayOptionsDialog::displayOptionsDialog(PG_MessageObject* c) {
    DisplayOptionsDialog ced(c);
    ced.Show();
    ced.RunModal();
}

//*******************************************************************************************************************+
const int GameOptionsDialog::xSize = 450;
const int GameOptionsDialog::ySize = 320;
const int GameOptionsDialog::buttonIndent = 60;

GameOptionsDialog::GameOptionsDialog(PG_MessageObject* c): ASC_PG_Dialog(NULL, PG_Rect( 200, 100, xSize, ySize ), "Game Options",
        SHOW_CLOSE ) {

    autoAmmunitionLabel = new PG_Label(this, PG_Rect(GuiDimension::getLeftIndent(), static_cast<int>(GuiDimension::getTopOffSet() * 1.5), 10, GetTextHeight() * 2), "Auto Ammunition");
    autoAmmunitionLabel->SetSizeByText();

    PG_Point p = ScreenToClient(autoAmmunitionLabel->x, autoAmmunitionLabel->y);
    autoAmmunitionCButton = new PG_CheckButton(this, PG_Rect(xSize - (GuiDimension::getLineEditWidth() + GuiDimension::getLeftIndent()), p.y, GuiDimension::getLineEditWidth(), GetTextHeight() * 2));
    if(CGameOptions::Instance()->CGameOptions::Instance()->container.autoproduceammunition){
      autoAmmunitionCButton->SetPressed();
    }
    
    p = ScreenToClient(autoAmmunitionLabel->x, autoAmmunitionLabel->y);
    autoUnitTrainingLabel = new PG_Label(this, PG_Rect(GuiDimension::getLeftIndent(), p.y + GuiDimension::getTopOffSet() + GetTextHeight() * 2 , 10, GetTextHeight() * 2), "Automatic Unit Training");
    autoUnitTrainingLabel->SetSizeByText();

    autoUnitTrainingCButton = new PG_CheckButton(this, PG_Rect(xSize - (GuiDimension::getLineEditWidth() + GuiDimension::getLeftIndent()), p.y + GuiDimension::getTopOffSet() + static_cast<int>(GetTextHeight() * 1.5), GuiDimension::getLineEditWidth(), GetTextHeight() * 2));
    if(CGameOptions::Instance()->CGameOptions::Instance()->automaticTraining){
      autoUnitTrainingCButton->SetPressed();
    }
    
    p = ScreenToClient(autoUnitTrainingLabel->x, autoUnitTrainingLabel->y);
    promptEndOfTurnLabel = new PG_Label(this, PG_Rect(GuiDimension::getLeftIndent(), p.y + GuiDimension::getTopOffSet() + GetTextHeight() * 2 , 10, GetTextHeight() * 2), "Prompt End of Turn");
    promptEndOfTurnLabel->SetSizeByText();

    promptEndOfTurnCButton = new PG_CheckButton(this, PG_Rect(xSize - (GuiDimension::getLineEditWidth() + GuiDimension::getLeftIndent()), p.y + GuiDimension::getTopOffSet() + static_cast<int>(GetTextHeight() * 1.5), GuiDimension::getLineEditWidth(), GetTextHeight() * 2));
    if(CGameOptions::Instance()->CGameOptions::Instance()->endturnquestion){
      promptEndOfTurnCButton->SetPressed();
    }

    p = ScreenToClient(promptEndOfTurnLabel->x, promptEndOfTurnLabel->y);
    changePasswordButton = new PG_Button(this, PG_Rect((xSize - GuiDimension::getButtonWidth())/2, p.y + GuiDimension::getTopOffSet() + GetTextHeight() * 2 , GuiDimension::getButtonWidth(), GuiDimension::getButtonHeight()), "Change Password");
    changePasswordButton->sigClick.connect( SigC::slot( *this, &GameOptionsDialog::changePassword ));

    okButton = new PG_Button(this, PG_Rect( buttonIndent, ySize - (GuiDimension::getButtonHeight() + GuiDimension::getTopOffSet()), GuiDimension::getButtonWidth(), GuiDimension::getButtonHeight()), "OK", 90);
    okButton->sigClick.connect( SigC::slot( *this, &GameOptionsDialog::ok ));

    p = ScreenToClient(okButton->x, okButton->y);
    cancelButton = new PG_Button(this, PG_Rect(p.x + GuiDimension::getButtonWidth() + 10, p.y, GuiDimension::getButtonWidth(), GuiDimension::getButtonHeight()), "Cancel", 90);
    cancelButton->sigClick.connect( SigC::slot( *this, &GameOptionsDialog::closeWindow ));

    sigClose.connect( SigC::slot( *this, &GameOptionsDialog::closeWindow ));

    caller = c;
    SetInputFocus();

}


GameOptionsDialog::~GameOptionsDialog() {
}

bool GameOptionsDialog::closeWindow() {
    quitModalLoop(1);
    caller->SetInputFocus();
    return true;
}

bool GameOptionsDialog::changePassword(PG_Button* button){
Hide();
ChangeDefaultPasswordDialog::changeDefaultPasswordDialog(this);
Show();
return true;
}
bool GameOptionsDialog::ok(PG_Button* button) {
    CGameOptions::Instance()->CGameOptions::Instance()->automaticTraining = autoUnitTrainingCButton->GetPressed();
    CGameOptions::Instance()->CGameOptions::Instance()->container.autoproduceammunition = autoAmmunitionCButton->GetPressed();
    CGameOptions::Instance()->CGameOptions::Instance()->endturnquestion = promptEndOfTurnCButton->GetPressed();
    quitModalLoop(1);
    return true;
}

void GameOptionsDialog::gameOptionsDialog(PG_MessageObject* caller) {
    GameOptionsDialog god(caller);
    god.Show();
    god.RunModal();
}


//*******************************************************************************************************************+
const int ChangePasswordDialog::xSize = 450;
const int ChangePasswordDialog::ySize = 220;
const int ChangePasswordDialog::buttonIndent = 60;

ChangePasswordDialog::ChangePasswordDialog(PG_MessageObject* c): ASC_PG_Dialog(NULL, PG_Rect( 200, 100, xSize, ySize ), "Change Password",
        SHOW_CLOSE ) {
	
    passwordLabel = new PG_Label(this, PG_Rect(GuiDimension::getLeftIndent(), static_cast<int>(GuiDimension::getTopOffSet() * 1.5), 10, GetTextHeight() * 2), "New Password: ");
    passwordLabel->SetSizeByText();
    
    PG_Point p = ScreenToClient(passwordLabel->x, passwordLabel->y);
    passwordValue = new PG_LineEdit(this, PG_Rect((xSize - xSize/2)/2, p.y + GuiDimension::getTopOffSet(), xSize/2, GetTextHeight() * 2));
    passwordValue->SetText(CGameOptions::Instance()->CGameOptions::Instance()->defaultPassword);
    

    okButton = new PG_Button(this, PG_Rect( buttonIndent, ySize - (GuiDimension::getButtonHeight() + GuiDimension::getTopOffSet()), GuiDimension::getButtonWidth(), GuiDimension::getButtonHeight()), "OK", 90);
    okButton->sigClick.connect( SigC::slot( *this, &ChangePasswordDialog::ok ));

    p = ScreenToClient(okButton->x, okButton->y);
    cancelButton = new PG_Button(this, PG_Rect(p.x + GuiDimension::getButtonWidth() + 10, p.y, GuiDimension::getButtonWidth(), GuiDimension::getButtonHeight()), "Cancel", 90);
    cancelButton->sigClick.connect( SigC::slot( *this, &ChangePasswordDialog::closeWindow ));
    
    sigClose.connect( SigC::slot( *this, &ChangePasswordDialog::closeWindow ));

    caller = c;
    SetInputFocus();		
}

ChangePasswordDialog::~ChangePasswordDialog(){

}


bool ChangePasswordDialog::ok(PG_Button* button){
  CGameOptions::Instance()->CGameOptions::Instance()->defaultPassword = passwordValue->GetText();
  quitModalLoop(1);
  caller->SetInputFocus();
  return true;
}

//*******************************************************************************************************************+

ChangeDefaultPasswordDialog::ChangeDefaultPasswordDialog(PG_MessageObject* c) : ChangePasswordDialog(c){


}

ChangeDefaultPasswordDialog::~ChangeDefaultPasswordDialog() {


}

bool ChangeDefaultPasswordDialog::ok(PG_Button* button){
  CGameOptions::Instance()->CGameOptions::Instance()->defaultPassword = passwordValue->GetText();
  quitModalLoop(1);
  caller->SetInputFocus();
  return true;
}


void ChangeDefaultPasswordDialog::changeDefaultPasswordDialog(PG_MessageObject* caller) {
    ChangeDefaultPasswordDialog cdpd(caller);
    cdpd.Show();
    cdpd.RunModal();
}

//*******************************************************************************************************************
 const int MousePreferencesDialog::xSize = 450;
 const int MousePreferencesDialog::ySize = 220;
   
 MousePreferencesDialog::MousePreferencesDialog(PG_MessageObject* c) : ASC_PG_Dialog(NULL, PG_Rect( 200, 100, xSize, ySize ), "Mouse Options",
        SHOW_CLOSE ) {
   
   sigClose.connect( SigC::slot( *this, &MousePreferencesDialog::closeWindow ));

    caller = c;
    SetInputFocus();
   }
   
   
   MousePreferencesDialog::~MousePreferencesDialog(){
   
   
   }

   
   
   bool MousePreferencesDialog::ok(PG_Button* button){
      return true;  
   }
   
   void MousePreferencesDialog::mousePreferencesDialog(PG_MessageObject* caller){
     MousePreferencesDialog mpd(caller);
     mpd.Show();
     mpd.RunModal();
   }
//*******************************************************************************************************************


ChangeMapPasswordDialog::ChangeMapPasswordDialog(PG_MessageObject* c) : ChangePasswordDialog(c){


}

ChangeMapPasswordDialog::~ChangeMapPasswordDialog() {


}

bool ChangeMapPasswordDialog::ok(PG_Button* button){  
  actmap->player[actmap->actplayer].passwordcrc.setUnencoded(passwordValue->GetText());
  quitModalLoop(1);
  caller->SetInputFocus();
  return true;
}


void ChangeMapPasswordDialog::changeMapPasswordDialog(PG_MessageObject* caller) {
    ChangeMapPasswordDialog cdpd(caller);
    cdpd.Show();
    cdpd.RunModal();
}



//*******************************************************************************************************************+
const int SaveGameBaseDialog::xSize = 450;
const int SaveGameBaseDialog::ySize = 360;
const int SaveGameBaseDialog::buttonIndent = 60;

SaveGameBaseDialog::SaveGameBaseDialog(const ASCString& title, PG_MessageObject* c): ASC_PG_Dialog(NULL, PG_Rect( 200, 100, xSize, ySize ), title.c_str(), SHOW_CLOSE ) {


    fileList = new PG_ListBox(this, PG_Rect(GuiDimension::getLeftIndent(), GuiDimension::getTopOffSet(), xSize - (2 *GuiDimension::getLeftIndent() + GuiDimension::getSliderWidth()), 200));

    for(int i=0; i<17; i++) {
        PG_ListBoxItem* item = new PG_ListBoxItem(fileList, 25, "");
        item->SetTextFormat("Item %i", i+1);
    }
    fileList->EnableScrollBar(true, PG_ScrollBar::VERTICAL);
    fileList->Update();
    /*
    tfindfile ff ( "*.sav" ); //wildcard

    ASCString location;
    string fileName = ff.getnextname(NULL, NULL, &location );
    PG_ListBoxItem* item;
    while( !fileName.empty() ) {
        item = new PG_ListBoxItem(fileList, 20, fileName.c_str());       
       /*tfiledata f;
       f.name = filename.c_str();

       time_t tdate = get_filetime( filename.c_str() );
       f.time = tdate;
       if ( tdate != -1 )
          f.sdate = ctime ( &tdate );

       f.location = location;

       fileList->
       //files.push_back ( f );

       numberoffiles++;*/

    /* fileName = ff.getnextname(NULL, NULL, &location );
    }
    fileList->EnableScrollBar(true, PG_ScrollBar::VERTICAL);
    */

    PG_Point p = ScreenToClient(fileList->x, fileList->y);
    /* slider = new PG_ScrollBar(this, PG_Rect(xSize - (GuiDimension::getLeftIndent() + GuiDimension::getSliderWidth()) , p.y, GuiDimension::getSliderWidth(), fileList->Height()), PG_ScrollBar::VERTICAL);
     slider->SetRange(0,fileList->GetWidgetCount());
     slider->sigScrollTrack.connect( SigC::slot( *this, &SaveGameBaseDialog::handleSlider));*/


    fileNameLabel = new PG_Label(this, PG_Rect(GuiDimension::getLeftIndent(), p.y + fileList->Height() + GuiDimension::getTopOffSet(), 10, 10), "Filename: ");
    fileNameLabel->SetSizeByText();

    p = ScreenToClient(fileNameLabel->x, fileNameLabel->y);
    fileNameValue = new PG_LineEdit(this, PG_Rect(p.x + fileNameLabel->Width(), p.y, xSize - (GuiDimension::getLeftIndent()*2 + fileNameLabel->Width()), GetTextHeight()*2));

    p = ScreenToClient(fileNameValue->x, fileNameValue->y);
    sortNameButton = new PG_RadioButton(this, PG_Rect(p.x , p.y + + GuiDimension::getTopOffSet(), 20, 20), "by name");
    sortNameButton->SetSizeByText();

    sortDateButton = new PG_RadioButton(this, PG_Rect(p.x + sortNameButton->Width() + GuiDimension::getLineOffset() , p.y + + GuiDimension::getTopOffSet(), 20, 20), "by date");
    sortDateButton->SetSizeByText();




    okButton = new PG_Button(this, PG_Rect( buttonIndent, ySize - (GuiDimension::getButtonHeight() + GuiDimension::getTopOffSet()), GuiDimension::getButtonWidth(), GuiDimension::getButtonHeight()), "OK", 90);
    okButton->sigClick.connect( SigC::slot( *this, &SaveGameBaseDialog::ok ));

    p = ScreenToClient(okButton->x, okButton->y);
    cancelButton = new PG_Button(this, PG_Rect(p.x + GuiDimension::getButtonWidth() + 10, p.y, GuiDimension::getButtonWidth(), GuiDimension::getButtonHeight()), "Cancel", 90);
    cancelButton->sigClick.connect( SigC::slot( *this, &SaveGameBaseDialog::closeWindow ));

    sigClose.connect( SigC::slot( *this, &SaveGameBaseDialog::closeWindow ));

    caller = c;
    SetInputFocus();
}


SaveGameBaseDialog::~SaveGameBaseDialog() {
}

bool SaveGameBaseDialog::closeWindow() {
    quitModalLoop(1);
    caller->SetInputFocus();
    return true;
}

bool SaveGameBaseDialog::handleSlider(long data) {

    return true;
}

//*************************************************************************************************************************

SaveGameDialog::SaveGameDialog(PG_MessageObject* c):SaveGameBaseDialog("Save Game", c) {
}

SaveGameDialog::~SaveGameDialog() {
}
bool SaveGameDialog::ok(PG_Button* button) {
    savegame(fileNameValue->GetText().c_str());
    quitModalLoop(1);
    return true;
}

void SaveGameDialog::saveGameDialog(PG_MessageObject* c) {
    SaveGameDialog sgd(c);
    sgd.Show();
    sgd.RunModal();
}

//*************************************************************************************************************************

LoadGameDialog::LoadGameDialog(PG_MessageObject* c):SaveGameBaseDialog("Load Game", c) {
}

LoadGameDialog::~LoadGameDialog() {
}
bool LoadGameDialog::ok(PG_Button* button) {
    vector<PG_ListBoxBaseItem*> selectedFileList;
    PG_ListBoxItem* fileName = dynamic_cast<PG_ListBoxItem*>(selectedFileList.front());
    cout << fileName->GetText().c_str() << endl;
    loadgame(fileName->GetText().c_str());
    quitModalLoop(1);
    return true;
}

void LoadGameDialog::loadGameDialog(PG_MessageObject* caller) {
    LoadGameDialog lgd(caller);
    lgd.Show();
    lgd.RunModal();
}

//*************************************************************************************************************************

SoundSettings::SoundSettings(PG_Widget* parent, const PG_Rect& r, PG_MessageObject* c ) :
      ASC_PG_Dialog(parent, r, "Sound Settings", SHOW_CLOSE )
{
   sSettings = CGameOptions::Instance()->sound;

   PG_CheckButton* musb = new PG_CheckButton(this, PG_Rect( 30, 50, 200, 20 ), "Enable Music", 1 );
   musb->sigClick.connect(SigC::slot( *this, &SoundSettings::radioButtonEvent ));
   new PG_Label ( this, PG_Rect(30, 80, 150, 20), "Music Volume" );
   PG_Slider* mus = new PG_Slider(this, PG_Rect(180, 80, 200, 20), PG_Slider::HORIZONTAL, 21);
   mus->SetRange(0,100);
   mus->SetPosition(sSettings.musicVolume);
   if ( sSettings.muteMusic )
      musb->SetUnpressed();
   else
      musb->SetPressed();


   PG_CheckButton* sndb = new PG_CheckButton(this, PG_Rect( 30, 150, 200, 20 ), "Enable Sound", 2 );
   sndb->sigClick.connect(SigC::slot( *this, &SoundSettings::radioButtonEvent ));
   new PG_Label ( this, PG_Rect(30, 180, 150, 20), "Sound Volume" );
   PG_Slider* snd = new PG_Slider(this, PG_Rect(180, 180, 200, 20), PG_Slider::HORIZONTAL, 31);
   snd->SetRange(0,100);
   snd->SetPosition(sSettings.soundVolume);
   if ( sSettings.muteEffects )
      sndb->SetUnpressed();
   else
      sndb->SetPressed();


   PG_Button* b1 = new PG_Button(this, PG_Rect(30,r.h-40,(r.w-70)/2,30), "OK", 100);
   b1->sigClick.connect(SigC::slot( *this, &SoundSettings::closeWindow ));

   PG_Button* b2 = new PG_Button(this, PG_Rect(r.w/2+5,r.h-40,(r.w-70)/2,30), "Cancel", 101);
   b2->sigClick.connect(SigC::slot( *this, &SoundSettings::buttonEvent ));

   sigClose.connect( SigC::slot( *this, &SoundSettings::closeWindow ));
   
   caller = c;
   SetInputFocus();
}


void SoundSettings::updateSettings()
{
   SoundSystem::getInstance()->setMusicVolume( CGameOptions::Instance()->sound.musicVolume );
   SoundSystem::getInstance()->setEffectVolume( CGameOptions::Instance()->sound.soundVolume );
   SoundSystem::getInstance()->setEffectsMute( CGameOptions::Instance()->sound.muteEffects );
   if ( CGameOptions::Instance()->sound.muteMusic )
      SoundSystem::getInstance()->pauseMusic();
   else
      SoundSystem::getInstance()->resumeMusic();

}

bool SoundSettings::radioButtonEvent( PG_RadioButton* button, bool state)
{
   if ( button->GetID() == 1 )
      CGameOptions::Instance()->sound.muteMusic = !state;
   if ( button->GetID() == 2 )
      CGameOptions::Instance()->sound.muteEffects = !state;
   updateSettings();
   return true;
}


bool SoundSettings::eventScrollTrack(PG_Slider* slider, long data)
{
   if(slider->GetID() == 21) {
      CGameOptions::Instance()->sound.musicVolume = data;
      CGameOptions::Instance()->setChanged();
      updateSettings();
      return true;
   }

   if(slider->GetID() == 31) {
      CGameOptions::Instance()->sound.soundVolume = data;
      CGameOptions::Instance()->setChanged();
      updateSettings();
      return true;
   }
   return false;
}



bool SoundSettings::buttonEvent( PG_Button* button )
{
   quitModalLoop(2);
   CGameOptions::Instance()->sound = sSettings;
   updateSettings();
   return true;
}

void SoundSettings::soundSettings(PG_MessageObject* caller)
{
   // printf("c1c %d \n", ticker );
   SoundSettings wnd1(NULL, PG_Rect(50,50,500,300), caller);
   // printf("c2c %d \n", ticker );
   wnd1.Show();
   // printf("c3c %d \n", ticker );
   wnd1.RunModal();
   // printf("c4c %d \n", ticker );
}
