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

#include <paragui.h>
#include <pgapplication.h>
#include <pgmessagebox.h>
#include <pgdropdown.h>
#include "pgbutton.h"
#include "pgwidgetlist.h"
#include "pglabel.h"
#include "pgwindow.h"
#include "pgmaskedit.h"
#include "pgscrollbar.h"
#include "pgprogressbar.h"
#include "pgradiobutton.h"
#include "pgthemewidget.h"
#include "pgcheckbutton.h"
#include "pgslider.h"
#include "pglistbox.h"
#include "pgcolumnitem.h"
#include "pgeventobject.h"
#include "pgpopupmenu.h"
#include "pgspinnerbox.h"
#include "pglog.h"
#include "pgmenubar.h"

#include "paradialog.h"
#include "events.h"

#include "sg.h"


//! A Paragui widget that fills the whole screen and redraws it whenever Paragui wants to it.
class MainScreenWidget : public PG_ThemeWidget, public PG_EventObject {
    bool gameInitialized;
public:
    MainScreenWidget( )
       : PG_ThemeWidget(NULL, PG_Rect ( 0, 0, ::getScreen()->w, ::getScreen()->h ), true),
         gameInitialized (false){};

    //! to be called after ASC has completed loading and repaintdisplay() is available and working.
    void gameReady() { gameInitialized = true; Show(); };
protected:
    void eventDraw (SDL_Surface* surface, const PG_Rect& rect);
};


void MainScreenWidget::eventDraw (SDL_Surface* surface, const PG_Rect& rect) {
    PG_ThemeWidget::eventDraw(surface, rect);

    if ( gameInitialized ) {
       SDL_Surface* screen = ::getScreen();
       initASCGraphicSubsystem( surface, NULL );
       repaintdisplay();
       initASCGraphicSubsystem( screen, NULL );
    }
}



void setupMainScreenWidget()
{
   MainScreenWidget* mainScreenWidget = new MainScreenWidget();
   mainScreenWidget->gameReady();
}


//! Adapter class for using Paragui Dialogs in ASC. This class transfers the event control from ASC to Paragui and back. All new dialog classes should be derived from this class
class ASC_PG_Dialog : public PG_Window, public PG_EventObject {
       SDL_Surface* background;
    protected:
       ASC_PG_Dialog ( PG_Widget *parent, const PG_Rect &r, const char *windowtext, Uint32 flags=WF_DEFAULT, const char *style="Window", int heightTitlebar=25);
       ~ASC_PG_Dialog();
};



ASC_PG_Dialog :: ASC_PG_Dialog ( PG_Widget *parent, const PG_Rect &r, const char *windowtext, Uint32 flags, const char *style, int heightTitlebar )
       :PG_Window ( parent, r, windowtext, flags, style, heightTitlebar )
{
   SDL_mutexP ( eventHandlingMutex );
}

ASC_PG_Dialog::~ASC_PG_Dialog ()
{
   SDL_mutexV ( eventHandlingMutex );
}



   // A testwindow class

 class TestWindow : public ASC_PG_Dialog {
 public:

 	TestWindow(PG_Widget* parent, const PG_Rect& r, char* windowtext);
 	virtual ~TestWindow() {};

 	void Dummy() {};

 	/** a new style callback member */
 	PARAGUI_CALLBACK(handle_show_window) {
 		Show(true);
 		return true;
 	}

 	/** callback handler in another flavor */
 	PARAGUI_CALLBACK(handler_slider_btntrans) {
 		PG_Button* b = (PG_Button*)clientdata;
 	
 		// set transparency of passed in button
 		b->SetTransparency(data, data, data);
 		b->Update();

 		// set transparency of class member (progress)
 		progress->SetTransparency(data);
 		progress->Update();
 		return true;
 	}

 protected:

 	bool eventButtonClick(int id, PG_Widget* widget);
 	bool eventScrollPos(int id, PG_Widget* widget, unsigned long data);
 	bool eventScrollTrack(int id, PG_Widget* widget, unsigned long data);

 private:
 	PG_ProgressBar* progress;
 	PG_WidgetList* WidgetList;
 };

TestWindow::TestWindow(PG_Widget* parent, const PG_Rect& r, char* windowtext) :
ASC_PG_Dialog(parent, r, windowtext, WF_SHOW_CLOSE | WF_SHOW_MINIMIZE)
{
/*
	WidgetList = new PG_WidgetList(this, PG_Rect(30, 40, 220, 250));
	WidgetList->SetTransparency(255);
	WidgetList->EnableScrollBar(true, PG_SB_VERTICAL);
	WidgetList->EnableScrollBar(true, PG_SB_HORIZONTAL);
		
	new PG_Button(this, 100, PG_Rect(260,130,110,30), "<< ADD");
	new PG_Button(this, 101, PG_Rect(260,165,110,30), ">> REMOVE");
	
	PG_Button* b = new PG_Button(NULL, BTN_ID_YES, PG_Rect(0,0, 400,50), "YES");
	b->SetTransparency(128,128,128);
	WidgetList->AddWidget(b);

	PG_Slider* s = new PG_Slider(NULL, 20, PG_Rect(0, 0, 200,20), PG_SB_HORIZONTAL);
	s->SetRange(0,255);
	s->SetTransparency(200);
	s->SetPosition(50);

	s->SetEventObject(MSG_SLIDE, this, (MSG_CALLBACK_OBJ)&TestWindow::handler_slider_btntrans, b);

	WidgetList->AddWidget(s);
		
	WidgetList->AddWidget(new PG_LineEdit(NULL, PG_Rect(0,0,80,30)));

	PG_CheckButton* check = new PG_CheckButton(NULL, 10, PG_Rect(0,0,200,25), "CheckButton 2");
	WidgetList->AddWidget(check);

	progress = new PG_ProgressBar(this, PG_Rect(260, 90, 150, 25));
	progress->SetTransparency(128);
	progress->SetName("MyProgressBar");
	progress->SetID(1001);
	progress->SetFontAlpha(128);
		
	PG_ScrollBar* scroll = new PG_ScrollBar(this, 1, PG_Rect(415,90,20,150));
	scroll->SetRange(0, 100);

	PG_ScrollBar* scroll1 = new PG_ScrollBar(this, 2, PG_Rect(435,90,20,150));
	scroll1->SetRange(0, 255);

	PG_DropDown* drop = new PG_DropDown(this, 15, PG_Rect(260, 60, 200,25));
	drop->SetIndent(5);
	drop->AddItem("Under construction");
	drop->AddItem("Item 1");
	drop->AddItem("Item 2");
	drop->AddItem("Item 3");
	drop->AddItem("Item 4");
	drop->AddItem("Item 5");
	drop->AddItem("Item 6");
  */
}

bool TestWindow::eventScrollPos(int id, PG_Widget* widget, unsigned long data){
	if(id == 1){
		progress->SetProgress(data);
		return true;
	}

	if(id == 2){
		SetTransparency((unsigned char)data);
		Update();
		return true;
	}

	return false;
}

bool TestWindow::eventScrollTrack(int id, PG_Widget* widget, unsigned long data) {
	if(id == 1){
		progress->SetProgress(data);
		return true;
	}

	if(id == 2){
		SetTransparency((unsigned char)data);
		Update();
		return true;
	}

	return false;
}

bool TestWindow::eventButtonClick(int id, PG_Widget* widget) {
	static int label=0;

	if (id==PG_WINDOW_CLOSE ) {
	   //hide();
	   eventQuitModal(0,NULL,0);
	   return true;
	}
	
	if(id == 100) {
		PG_Label* l = new PG_Label(NULL, PG_Rect(0,0,220,25), "");
		l->SetAlignment(PG_TA_CENTER);
		l->SetTextFormat("Label %i", ++label);
		WidgetList->AddWidget(l);
	
		return true;
	}

	if(id == 101) {
		PG_Widget* w = WidgetList->FindWidget(4);
		if(w != NULL) {
			WidgetList->RemoveWidget(w, true, true);
			delete w;
		}
		
		return true;
	}
	
	return PG_Window::eventButtonClick(id, widget);
}

PARAGUI_CALLBACK_SELECTMENUITEM(handle_menu_click) {
	std::cout << "menu item '" << id << "' (\""
		<< item->getCaption() << "\") clicked" << std::endl;

	switch (id) {
		case 5:
			static_cast<TestWindow*>(clientdata)->Show();
			break;

		case 6:
			static_cast<PG_Application*>(clientdata)->Quit();
			break;
	}

	return true;
}

void paraguiTest( )
{               /*
            SDL_mutexP ( eventHandlingMutex );
            PG_MessageBox *msgbox =new PG_MessageBox(
                                      NULL,
                                      PG_Rect(200,50,240,200),
                                      "Modal Messagebox", "Click \"Ok\" to close me",
                                      PG_Rect(90, 120, 50, 50),
                                      "Ok",
                                      PG_TA_CENTER);

            PG_DropDown* drop = new PG_DropDown(msgbox, 15, PG_Rect(5, 60, 200,25));
            drop->SetIndent(5);
            drop->AddItem("Under construction");
            drop->AddItem("Item 1");
            drop->AddItem("Item 2");
            drop->AddItem("Item 3");

            msgbox->Show();
            msgbox->WaitForClick();

            delete msgbox;
            SDL_mutexV ( eventHandlingMutex );
            */
  TestWindow wnd1(NULL, PG_Rect(50,50,500,300), "My 2nd Testwindow");
/*	wnd1.SetTransparency(0);
	wnd1.SetName("WindowTwo");
	wnd1.SetID(101);   	*/
	wnd1.Show();
	wnd1.RunModal();
}
