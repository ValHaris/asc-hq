/***************************************************************************
                          paradialog.cpp  -  description
                             -------------------
    begin                : Thu Feb 21 2002
    copyright            : (C) 2002 by Martin Bickel
    email                : bickel@asc-hq.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/



#include <iostream>
#include "resourceplacement.h"
// #include "notificationdialog.h"
#include "resourceplacementdialog.h"
#include "spfst.h"
#include "spfst-legacy.h"


const int ResourcePlacementDialog::xsize = 400;
const int ResourcePlacementDialog::ysize = 612;
const int ResourcePlacementDialog::xOffSet = xsize/10;
const int ResourcePlacementDialog::yOffSet = ysize/20;
const int ResourcePlacementDialog::leftIndent = xsize/50;;


ResourcePlacementDialog::ResourcePlacementDialog( ) :
    ASC_PG_Dialog(NULL, PG_Rect( 100,100, xsize, ysize ), "Place Resources", SHOW_CLOSE ),
    fuelRoughness(1), materialRoughness(1), maxFuelOffSet(100), maxMaterialOffSet(100), 
    fuelRoughnessWidget(NULL), materialRoughnessWidget(NULL), fuelOffsetWidget(NULL), materialOffsetWidget(NULL), d(0)
{
  int xFuelWidgetPos = leftIndent;
  int yFuelWidgetPos = ysize/10;
  int hFuelWidget = GetTextHeight() + 10;
  placeFuelWidget = new PG_CheckButton(this, PG_Rect(xFuelWidgetPos, yFuelWidgetPos, xsize/3 , GetTextHeight()*2), "Fuel");
  placeFuelWidget->SetSizeByText();



  int xMaterialWidgetPos = xFuelWidgetPos;
  int yMaterialWidgetPos = yFuelWidgetPos + hFuelWidget;
  placeMaterialWidget = new PG_CheckButton(this, PG_Rect(xMaterialWidgetPos, yMaterialWidgetPos, xsize/3, GetTextHeight()*2), "Material");
  placeMaterialWidget->SetSizeByText();

  int xFuelRoughnessLabelPos = 0;
  if(placeFuelWidget->Width() > placeMaterialWidget->Width()) {
    xFuelRoughnessLabelPos = xFuelWidgetPos + placeFuelWidget->Width() + xOffSet;
  } else {
    xFuelRoughnessLabelPos = xFuelWidgetPos + placeMaterialWidget->Width() + xOffSet;
  }
  fuelRoughnessLabel = new PG_Label(this, PG_Rect(xFuelRoughnessLabelPos, yFuelWidgetPos , 40, hFuelWidget), "Fuel Roughness: ");
  fuelRoughnessLabel->SetSizeByText();

  materialRoughnessLabel = new PG_Label(this, PG_Rect(xFuelRoughnessLabelPos, yMaterialWidgetPos , 40, hFuelWidget), "Material Roughness: ");
  materialRoughnessLabel->SetSizeByText();

  int xFuelRoughnessWidgetPos = 0;
  if(fuelRoughnessLabel->Width() > materialRoughnessLabel->Width()) {
    xFuelRoughnessWidgetPos = xFuelRoughnessLabelPos + fuelRoughnessLabel->Width();
  } else {
    xFuelRoughnessWidgetPos = xFuelRoughnessLabelPos + materialRoughnessLabel->Width();
  }
  fuelRoughnessWidget = new PG_LineEdit ( this, PG_Rect(xFuelRoughnessWidgetPos, yFuelWidgetPos, xsize - ( xFuelRoughnessLabelPos + fuelRoughnessLabel->Width() + xOffSet), hFuelWidget));
  fuelRoughnessWidget->SetValidKeys("1234567890.");
  fuelRoughnessWidget->SetText("1.0");



  materialRoughnessWidget = new PG_LineEdit ( this, PG_Rect(xFuelRoughnessWidgetPos, yMaterialWidgetPos, xsize - ( xFuelRoughnessLabelPos + fuelRoughnessLabel->Width() + xOffSet), hFuelWidget));
  materialRoughnessWidget->SetValidKeys("1234567890.");
  materialRoughnessWidget->SetText("1.0");



  int yFuelOffSetLabelPos = yMaterialWidgetPos + hFuelWidget + yOffSet;
  fuelOffSetLabel = new PG_Label(this, PG_Rect(xFuelWidgetPos, yFuelOffSetLabelPos, 40, hFuelWidget), "Fuel Offset: ");
  fuelOffSetLabel->SetSizeByText();

  int xFuelOffSetValue = xFuelWidgetPos + fuelOffSetLabel->Width();
  fuelOffSetValue = new PG_LineEdit(this, PG_Rect(xFuelOffSetValue, yFuelOffSetLabelPos, 40, hFuelWidget));
  fuelOffSetValue->SetText(ASCString::toString(ResourcePlacement::MAXFUELVALUE));
  fuelOffSetValue->SetValidKeys("1234567890");
  fuelOffSetValue->SetEditable(false);

  int yFuelOffSetSliderPos = int(yFuelOffSetLabelPos + hFuelWidget * 1.5);
  fuelOffSetSlider = new PG_Slider(this, PG_Rect(xFuelWidgetPos, yFuelOffSetSliderPos, xsize - (xFuelWidgetPos + leftIndent) , hFuelWidget), PG_ScrollBar::HORIZONTAL);
  fuelOffSetSlider->SetRange(ResourcePlacement::MINFUELVALUE, ResourcePlacement::MAXFUELVALUE);
  fuelOffSetSlider->SetPosition(atoi(fuelOffSetValue->GetText()));
  fuelOffSetSlider->sigSlide.connect(slot(*this, &ResourcePlacementDialog::scrollTrackEventFuel));


  //MaterialOffSet
  int yMaterialOffSetLabelPos = yFuelOffSetSliderPos + hFuelWidget;
  materialOffSetLabel = new PG_Label(this, PG_Rect(xFuelWidgetPos, yMaterialOffSetLabelPos, 40, hFuelWidget), "Material Offset: ");
  materialOffSetLabel->SetSizeByText();

  int xMaterialOffSetValue = xFuelWidgetPos + materialOffSetLabel->Width();
  materialOffSetValue = new PG_LineEdit(this, PG_Rect(xMaterialOffSetValue, yMaterialOffSetLabelPos, 40, hFuelWidget));
  materialOffSetValue->SetText(ASCString::toString(ResourcePlacement::MAXMATERIALVALUE));
  materialOffSetValue->SetValidKeys("1234567890");
  materialOffSetValue->SetEditable(false);

  int yMaterialOffSetSliderPos = int(yMaterialOffSetLabelPos + hFuelWidget * 1.5);
  materialOffSetSlider = new PG_Slider(this, PG_Rect(xFuelWidgetPos, yMaterialOffSetSliderPos, xsize - (xFuelWidgetPos + leftIndent) , hFuelWidget), PG_ScrollBar::HORIZONTAL);
  materialOffSetSlider->SetRange(ResourcePlacement::MINMATERIALVALUE, ResourcePlacement::MAXMATERIALVALUE);
  materialOffSetSlider->SetPosition(atoi(materialOffSetValue->GetText()));
  materialOffSetSlider->sigSlide.connect(slot(*this, &ResourcePlacementDialog::scrollTrackEventMaterial));

  //FuelFreeRatio
  int yFuelFreeRatioLabelPos = yMaterialOffSetSliderPos + hFuelWidget + yOffSet;
  fuelFreeRatioLabel = new PG_Label(this, PG_Rect(xFuelWidgetPos, yFuelFreeRatioLabelPos, 40, hFuelWidget), "Fuel Free Fields Ratio: ");
  fuelFreeRatioLabel->SetSizeByText();
  
  int xFuelFreeRatioValue = xFuelWidgetPos + fuelFreeRatioLabel->Width();
  fuelFreeRatioValue = new PG_LineEdit(this, PG_Rect(xFuelFreeRatioValue, yFuelFreeRatioLabelPos, 40, hFuelWidget));
  fuelFreeRatioValue->SetText(ASCString::toString(0));
  fuelFreeRatioValue->SetValidKeys("1234567890");
  fuelFreeRatioValue->SetEditable(false);
  
  int yFuelFreeRatioSliderPos = int(yFuelFreeRatioLabelPos + hFuelWidget * 1.5);
  fuelFreeRatioSlider = new PG_Slider(this, PG_Rect(xFuelWidgetPos, yFuelFreeRatioSliderPos, xsize - (xFuelWidgetPos + leftIndent) , hFuelWidget), PG_ScrollBar::HORIZONTAL);
  fuelFreeRatioSlider->SetRange(0, 100);
  fuelFreeRatioSlider->SetPosition(atoi(fuelFreeRatioValue->GetText()));
  fuelFreeRatioSlider->sigSlide.connect(slot(*this, &ResourcePlacementDialog::scrollTrackEventFuelFreeRatio));
  
  //MaterialFreeRatio
  int yMaterialFreeRatioLabelPos = yFuelFreeRatioSliderPos + hFuelWidget ;
  materialFreeRatioLabel = new PG_Label(this, PG_Rect(xFuelWidgetPos, yMaterialFreeRatioLabelPos, 40, hFuelWidget), "Material Free Fields Ratio: ");
  materialFreeRatioLabel->SetSizeByText();
  
  int xMaterialFreeRatioValue = xFuelWidgetPos + materialFreeRatioLabel->Width();
  materialFreeRatioValue = new PG_LineEdit(this, PG_Rect(xMaterialFreeRatioValue, yMaterialFreeRatioLabelPos, 40, hFuelWidget));
  materialFreeRatioValue->SetText(ASCString::toString(0));
  materialFreeRatioValue->SetValidKeys("1234567890");
  materialFreeRatioValue->SetEditable(false);
  
  int yMaterialFreeRatioSliderPos = int(yMaterialFreeRatioLabelPos + hFuelWidget * 1.5);
  materialFreeRatioSlider = new PG_Slider(this, PG_Rect(xFuelWidgetPos, yMaterialFreeRatioSliderPos, xsize - (xFuelWidgetPos + leftIndent) , hFuelWidget), PG_ScrollBar::HORIZONTAL);
  materialFreeRatioSlider->SetRange(0, 100);
  materialFreeRatioSlider->SetPosition(atoi(materialFreeRatioValue->GetText()));
  materialFreeRatioSlider->sigSlide.connect(slot(*this, &ResourcePlacementDialog::scrollTrackEventMaterialFreeRatio));
  
  //Buttons OK and Cancel

  (new PG_Button(this, PG_Rect(30, Height()-40, (Width()-70)/2, 30), "OK", 100))->sigClick.connect(sigc::mem_fun( *this, &ResourcePlacementDialog::buttonEvent ));
  (new PG_Button(this, PG_Rect(Width()/2+5, Height()-40, (Width()-70)/2, 30), "Cancel", 101))->sigClick.connect(sigc::mem_fun( *this, &ResourcePlacementDialog::closeWindow ));

  sigClose.connect( sigc::mem_fun( *this, &ResourcePlacementDialog::closeWindow ));
}



bool ResourcePlacementDialog::buttonEvent( PG_Button* button ) {
  if((atof(fuelRoughnessWidget->GetText())<=0) || (atof(fuelRoughnessWidget->GetText())>=4) ||
      (atof(materialRoughnessWidget->GetText())<=0) || (atof(materialRoughnessWidget->GetText())>=4)) {
     d = new PG_MessageBox(this, PG_Rect(0,0, Width()/2, int(Height()* 0.75)), "Error Report", "Condition 0.0 < roughness < 4.0 not met", PG_Rect(Width()/4 - 25, int(Height()*0.75) - 30, 50, 30), "OK");
    d->Show();
  
    return true;
  } else {
    quitModalLoop(1);
    ResourcePlacement rp (*actmap , atof(fuelRoughnessWidget->GetText()), atof(materialRoughnessWidget->GetText()), atoi(fuelOffSetValue->GetText()), atoi(materialOffSetValue->GetText()), 0, atoi(materialFreeRatioValue->GetText()));

    if((placeFuelWidget->GetPressed()) && (placeMaterialWidget->GetPressed())) {
      rp.placeResources();
    } else if(placeFuelWidget->GetPressed()) {
      rp.placeFuelResources();
    } else if(placeMaterialWidget->GetPressed()) {
      rp.placeMaterialResources();
    }
    return true;
  }
}



bool ResourcePlacementDialog::scrollTrackEventMaterial(long data) {
  materialOffSetValue->SetText( ASCString::toString( (int) data));
  return true;
}

bool ResourcePlacementDialog::scrollTrackEventFuel(long data) {
  fuelOffSetValue->SetText( ASCString::toString( (int) data ));
  return true;
}


bool ResourcePlacementDialog::scrollTrackEventMaterialFreeRatio(long data){
   materialFreeRatioValue->SetText( ASCString::toString((int)data));
  return true;
}


bool ResourcePlacementDialog::scrollTrackEventFuelFreeRatio(long data){
  fuelFreeRatioValue->SetText(ASCString::toString((int)data));
  return true;
}

bool ResourcePlacementDialog::buttonErrorOkEvent( PG_Button* button ){
  quitModalLoop(1);
  return true;
}

void resourcePlacementDialog() {
  ResourcePlacementDialog rpd;
  rpd.Show();
  rpd.RunModal();
}




