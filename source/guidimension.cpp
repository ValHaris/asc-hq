//
// C++ Implementation: guidimension
//
// Description: 
//
//
// Author: Martin Bickel <bickel@asc-hq.org>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "guidimension.h"
#include "paradialog.h"
GuiDimension::GuiDimension()
{
}



GuiDimension::~GuiDimension()
{
}

int GuiDimension::getLeftIndent() {
  return getPGApplication().GetScreenWidth()/50;
}

int GuiDimension::getTopOffSet(){
  return 20;
}

int GuiDimension::getLeftOffSet(){
  return getPGApplication().GetScreenWidth()/50;
}

int GuiDimension::getLineEditWidth(){
  return getPGApplication().GetScreenWidth()/20;
}

int GuiDimension::getLineOffset(){
   return getPGApplication().GetScreenWidth()/40;
}

int GuiDimension::getButtonHeight(){
   return 40;
}

int GuiDimension::getButtonWidth(){
   return 170;
}

int GuiDimension::getSliderWidth(){
   return 20;
}




