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


#include "global.h"

#include "paradialog.h"
#include "events.h"
#include "gameoptions.h"
#include "sg.h"
#include "sdl/sound.h"

#include "paradialog.h"
#include "resourceplacement.h"



 class ResourcePlacementDialog : public ASC_PG_Dialog {
 public:
 	ResourcePlacementDialog( );
 protected:
 
        double fuelRoughness;
        double materialRoughness;
        int maxFuelOffSet;
        int maxMaterialOffSet; 
 
        PG_LineEdit* fuelRoughnessWidget;
        PG_LineEdit* materialRoughnessWidget;
        PG_LineEdit* fuelOffsetWidget;
        PG_LineEdit* materialOffsetWidget;
        
        
        
        bool radioButtonEvent( PG_RadioButton* button, bool state);
        bool buttonEvent( PG_Button* button );
 	bool eventScrollTrack(PG_Slider* slider, long data);
        bool closeWindow()
        {
           quitModalLoop(2);
           return true;
        };   
        
 };


ResourcePlacementDialog::ResourcePlacementDialog( ) :
               ASC_PG_Dialog(NULL, PG_Rect( 100,100, 400, 300 ), "Place Resources", SHOW_CLOSE ),
               fuelRoughness(1), materialRoughness(1), maxFuelOffSet(100), maxMaterialOffSet(100),
               fuelRoughnessWidget(NULL), materialRoughnessWidget(NULL), fuelOffsetWidget(NULL), materialOffsetWidget(NULL)

{

        fuelRoughnessWidget     = new PG_LineEdit ( this, PG_Rect(50,50,150,30) );
        materialRoughnessWidget = new PG_LineEdit ( this, PG_Rect(200,50,150,30) );
   
        fuelOffsetWidget     = new PG_LineEdit ( this, PG_Rect(50,100,150,30) );
        materialOffsetWidget = new PG_LineEdit ( this, PG_Rect(200,100,150,30) );
        
        fuelOffsetWidget->SetValidKeys("1234567890");
        materialOffsetWidget->SetValidKeys("1234567890");
        fuelRoughnessWidget->SetValidKeys("1234567890.-");
        materialRoughnessWidget->SetValidKeys("1234567890.-");
        
        fuelOffsetWidget->SetText("100");
        materialOffsetWidget->SetText("100");
        fuelRoughnessWidget->SetText("1");
        materialRoughnessWidget->SetText("1");
        
	(new PG_Button(this, PG_Rect(30, Height()-40, (Width()-70)/2, 30), "OK", 100))->sigClick.connect(SigC::slot( *this, &ResourcePlacementDialog::buttonEvent ));
	(new PG_Button(this, PG_Rect(Width()/2+5, Height()-40, (Width()-70)/2, 30), "Cancel", 101))->sigClick.connect(SigC::slot( *this, &ResourcePlacementDialog::closeWindow ));
        
        sigClose.connect( SigC::slot( *this, &ResourcePlacementDialog::closeWindow ));
}

bool ResourcePlacementDialog::buttonEvent( PG_Button* button ) 
{
           quitModalLoop(1);
           ResourcePlacement rp (*actmap , atof(fuelRoughnessWidget->GetText()), atof(materialRoughnessWidget->GetText()), atoi(fuelOffsetWidget->GetText()), atoi(fuelOffsetWidget->GetText()));
           rp.placeResources();
           return true;
}

 
 
void resourcePlacementDialog()
{
  ResourcePlacementDialog rpd;
  rpd.Show();
  rpd.Run();
}

