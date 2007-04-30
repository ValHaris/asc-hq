//
// C++ Interface: gamedialog
//
// Description: 
//
//
// Author: Kevin Hirschmann <hirsch@dhcppc0>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef GAMEDIALOG_H
#define GAMEDIALOG_H

#include "gameoptions.h"
#include "paradialog.h"
#include "pgmultilineedit.h"
#include "gamemap.h"
/**
@author Kevin Hirschmann
*/
class GameDialog: public ASC_PG_Dialog{
public:    

    ~GameDialog();
    
    static bool gameDialog();    
protected:
  bool handleEventKeyDown (const SDL_KeyboardEvent *key);
private:
  static const int xSize;
  static const int ySize;
  static const int buttonIndent;
  static GameDialog* instance;
  PG_Button* singlePlayerButton;
  PG_Button* multiPlayerButton;
  PG_Button* loadGameButton;
  PG_Button* saveGameButton;
  PG_Button* optionsButton;
  PG_Button* exitButton;
  PG_Button* continueButton;
  
  GameDialog();
  
  bool closeWindow();
  bool newGame(PG_Button* button);
  bool singleGame(PG_Button* button);
  bool multiGame(PG_Button* button);
  bool saveGame(PG_Button* button);
  bool loadGame(PG_Button* button);
  bool showOptions(PG_Button* button);
  bool exitGame(PG_Button* button);
  bool supervise(PG_Button* button);
 
  
};



class SinglePlayerDialog: public ASC_PG_Dialog{
public:  
  static void singlePlayerDialog(PG_MessageObject* caller);
    
  ~SinglePlayerDialog();
private:
  static const int xSize;
  static const int ySize;
  static const int buttonIndent;
  static SinglePlayerDialog* instance;
  
  PG_Button* campaignButton;
  PG_Button* singleLevelButton;
  PG_Button* cancelButton;
  
  SinglePlayerDialog(PG_MessageObject* parent);
  
  bool campaign(PG_Button* button);
  bool singleLevel(PG_Button* button);
  bool closeWindow();
};



class MultiPlayerDialog: public ASC_PG_Dialog{
public:    
  static void multiPlayerDialog(PG_MessageObject* c);  
  ~MultiPlayerDialog();
private:
  static const int xSize;
  static const int ySize;
  static const int buttonIndent;
  
  PG_Button* continueGameButton;
  PG_Button* superViseButton;
  PG_Button* setupNWButton;
  PG_Button* changeMapPasswordButton;
  PG_Button* cancelButton;
  
  MultiPlayerDialog(PG_MessageObject* c);
  
  bool continueGame(PG_Button* button);
  bool superVise(PG_Button* button);
  bool setupNWGame(PG_Button* button);
  bool changeMapPassword(PG_Button* button);
  bool closeWindow();
};



class ConfirmExitDialog: public ASC_PG_Dialog{
public:  
  static void confirmExitDialog(PG_MessageObject* c);
  
  
  ~ConfirmExitDialog();
private:
  static const int xSize;
  static const int ySize;
  
  PG_Button* okButton;
  PG_Button* cancelButton;
  
  ConfirmExitDialog(PG_MessageObject* c);
  
  bool exitGame(PG_Button* button);
  bool closeWindow();
};



class OptionsDialog: public ASC_PG_Dialog{
public:  
  
  static void optionsDialog(PG_MessageObject* c);
    
  ~OptionsDialog();
private:
  static const int xSize;
  static const int ySize;
  static const int buttonIndent;
  
  PG_Button* okButton;
  PG_Button* soundButton;
  PG_Button* mouseButton;
  PG_Button* otherButton;
  PG_Button* displayButton;
  
  OptionsDialog(PG_MessageObject* c);
  
  bool showSoundOptions(PG_Button* button);
  bool showOtherOptions(PG_Button* button);
  bool closeWindow();
};




class GameOptionsDialog: public ASC_PG_Dialog{
public:    
   static void gameOptionsDialog(PG_MessageObject* c);  
  ~GameOptionsDialog();
private:
  static const int xSize;
  static const int ySize;
  static const int buttonIndent;    
  
  PG_Label* autoAmmunitionLabel;
  PG_CheckButton* autoAmmunitionCButton;
  
  PG_Label* autoUnitTrainingLabel;
  PG_CheckButton* autoUnitTrainingCButton;    
  
  PG_Label* promptEndOfTurnLabel;
  PG_CheckButton* promptEndOfTurnCButton;
  
  PG_Button* okButton;
  PG_Button* cancelButton;
  
  PG_Button* changePasswordButton;
  
  GameOptionsDialog(PG_MessageObject* c);
  
  bool changePassword(PG_Button* button);
  
  bool ok(PG_Button* button);
  bool closeWindow();
};

class ChangePasswordDialog: public ASC_PG_Dialog{
  public:    
    virtual ~ChangePasswordDialog();
    
    protected:
    PG_LineEdit* passwordValue;
	
    ChangePasswordDialog(PG_MessageObject* c);
  private:  
    static const int xSize;
    static const int ySize;
    static const int buttonIndent;
    
    PG_Button* okButton;
    PG_Button* cancelButton;
    PG_Label* passwordLabel;
    
    virtual bool ok(PG_Button* button) = 0;


};
/*
class MousePreferencesDialog: public ASC_PG_Dialog{
  public:
   static void mousePreferencesDialog(PG_MessageObject* c);
   virtual ~MousePreferencesDialog();
private:
   static const int xSize;
   static const int ySize;
    
   MousePreferencesDialog(PG_MessageObject* c);
   bool ok(PG_Button* button);
};
*/

class ChangeDefaultPasswordDialog: public ChangePasswordDialog{
public:
   static void changeDefaultPasswordDialog(PG_MessageObject* c);
   virtual ~ChangeDefaultPasswordDialog();
private:
   ChangeDefaultPasswordDialog(PG_MessageObject* c);
   bool ok(PG_Button* button);
};

class ChangeMapPasswordDialog: public ChangePasswordDialog{
public:
   static void changeMapPasswordDialog(PG_MessageObject* c);
   virtual ~ChangeMapPasswordDialog();
private:
   ChangeMapPasswordDialog(PG_MessageObject* c);
   bool ok(PG_Button* button);
};


class SaveGameBaseDialog: public ASC_PG_Dialog{
public:  
  
  virtual ~SaveGameBaseDialog();
  
protected:  
PG_LineEdit* fileNameValue;
PG_ScrollBar* slider;
PG_ListBox* fileList;

SaveGameBaseDialog(const ASCString& title, PG_MessageObject* c);

private:
  static const int xSize;
  static const int ySize;
  static const int buttonIndent;      
    
  PG_RadioButton* sortNameButton;
  PG_RadioButton* sortDateButton;
  
  PG_Label* fileNameLabel;
    
  PG_Button* okButton;
  PG_Button* cancelButton;
  
  virtual bool ok(PG_Button* button) = 0;
  virtual bool handleSlider(long data);
  virtual bool closeWindow();
};

class SaveGameDialog: public SaveGameBaseDialog{
public:  
  static void saveGameDialog(PG_MessageObject* caller);  
  
  virtual ~SaveGameDialog();
private:
  SaveGameDialog(PG_MessageObject* caller);
    
  bool ok(PG_Button* button);  
};

#if 0

class LoadGameDialog: public SaveGameBaseDialog{
public:  
  
  static void loadGameDialog(PG_MessageObject* caller);  
  
  virtual ~LoadGameDialog();
private:
  LoadGameDialog(PG_MessageObject* caller);  
    
  bool ok(PG_Button* button);  
};
#endif


#endif

