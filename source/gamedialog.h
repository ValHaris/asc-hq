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

#include "paradialog.h"
/**
@author Kevin Hirschmann
*/
class GameDialog: public ASC_PG_Dialog{
public:
    GameDialog();

    ~GameDialog();

private:
  static const int xSize;
  static const int ySize;
  static const int buttonIndent;
  PG_Button* singlePlayerButton;
  PG_Button* multiPlayerButton;
  PG_Button* loadGameButton;
  PG_Button* saveGameButton;
  PG_Button* optionsButton;
  PG_Button* exitButton;
  PG_Button* continueButton;
  
  bool closeWindow();
  bool singleGame(PG_Button* button);
  bool multiGame(PG_Button* button);
  bool saveGame(PG_Button* button);
  bool loadGame(PG_Button* button);
  bool showOptions(PG_Button* button);
  bool exitGame(PG_Button* button);
  
};

extern void gameDialog();

class SinglePlayerDialog: public ASC_PG_Dialog{
public:  
  SinglePlayerDialog();
  
  ~SinglePlayerDialog();
private:
  static const int xSize;
  static const int ySize;
  static const int buttonIndent;
  
  PG_Button* campaignButton;
  PG_Button* singleLevelButton;
  PG_Button* cancelButton;
  
  bool campaign(PG_Button* button);
  bool singleLevel(PG_Button* button);
  bool closeWindow();
};

extern void singlePlayerDialog();

class MultiPlayerDialog: public ASC_PG_Dialog{
public:  
  MultiPlayerDialog();
  
  ~MultiPlayerDialog();
private:
  static const int xSize;
  static const int ySize;
  static const int buttonIndent;
  
  PG_Button* continueGameButton;
  PG_Button* superViseButton;
  PG_Button* setupNWButton;
  PG_Button* cancelButton;
  
  bool continueGame(PG_Button* button);
  bool superVise(PG_Button* button);
  bool setupNWGame(PG_Button* button);
  bool closeWindow();
};

extern void multiPlayerDialog();

class ConfirmExitDialog: public ASC_PG_Dialog{
public:  
  ConfirmExitDialog();
  
  ~ConfirmExitDialog();
private:
  static const int xSize;
  static const int ySize;
  
  PG_Button* okButton;
  PG_Button* cancelButton;
  
  bool exitGame(PG_Button* button);
  bool closeWindow();
};

extern void confirmExitDialog();

class OptionsDialog: public ASC_PG_Dialog{
public:  
  OptionsDialog();
  
  ~OptionsDialog();
private:
  static const int xSize;
  static const int ySize;
  static const int buttonIndent;
  
  PG_Button* okButton;
  PG_Button* soundButton;
  PG_Button* mouseButton;
  PG_Button* otherButton;
  
  bool showSoundOptions(PG_Button* button);
  bool showMouseOptions(PG_Button* button);
  bool showOtherOptions(PG_Button* button);
  bool closeWindow();
};
extern void optionsDialog();

#endif

