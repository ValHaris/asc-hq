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
  return getPGApplication().GetScreenHeight()/40;
}

int GuiDimension::getLeftOffSet(){
  return getPGApplication().GetScreenWidth()/50;
}

int GuiDimension::getLineEditWidth(){
  return getPGApplication().GetScreenWidth()/20;
}
