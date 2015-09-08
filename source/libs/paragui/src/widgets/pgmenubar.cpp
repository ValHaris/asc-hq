#include "pgmenubar.h"
#include "pgbutton.h"
#include "pgpopupmenu.h"
#include "pglog.h"
#include "pgapplication.h"
#include "pgeventsupplier.h"

PG_MenuBar::PG_MenuBar(PG_Widget* parent, const PG_Rect& rect, const std::string& style) : PG_ThemeWidget(parent, rect, style) {
	my_btnOffsetY = 2;
	my_style = style;
	my_active = NULL;
}

PG_MenuBar::~PG_MenuBar() {
	Cleanup();
}

void PG_MenuBar::Add(const std::string& text, PG_PopupMenu* menu, Uint16 indent, Uint16 width) {
	MenuBarItem* last = NULL;
	Uint16 xpos = 0;
	Uint16 height = 0;

	if(menu == NULL) {
		return;
	}

	if(ItemList.size() > 0) {
		last = ItemList.back();
	}

	if(last != NULL) {
		xpos = (last->button->x + last->button->w) - my_xpos;
	}

	xpos += indent;

	if(width == 0) {
		GetTextSize(width, height, text);
		width += 6;
	}

	last = new MenuBarItem;

	last->button = new PG_Button(
	                   this,
	                   PG_Rect(xpos, my_btnOffsetY, width, my_height - my_btnOffsetY*2),
	                   text, -1,
	                   my_style);

	last->button->SetBehaviour( PG_Button::SIGNALONCLICK );
	last->button->SetFontSize(GetFontSize());

	last->button->sigClick.connect(sigc::bind( sigc::mem_fun(*this, &PG_MenuBar::handle_button), (PG_Pointer)last));
	last->button->sigMouseLeave.connect(sigc::bind(sigc::mem_fun(*this, &PG_MenuBar::leaveButton), (PG_Pointer)last ));
	last->button->sigMouseEnter.connect(sigc::bind(sigc::mem_fun(*this, &PG_MenuBar::enterButton), (PG_Pointer)last ));
    last->button->activateHotkey( getHotkeyModifier() );

	last->popupmenu = menu;

	ItemList.push_back(last);

   menu->sigSelectMenuItem.connect( sigc::mem_fun( *this, &PG_MenuBar::deactivateItem ));
}

bool PG_MenuBar::deactivateItem(PG_PopupMenu::MenuItem* item)
{
   my_active->Hide();
   my_active = NULL;
   return true;
}


bool PG_MenuBar::leaveButton( PG_Pointer last ) {
	MenuBarItem* item = static_cast<MenuBarItem*>(last);
	if( my_active ) {
		if ( my_active->IsMouseInside() ) {
			my_active->trackMenu( item->button->x, item->button->y + item->button->h );
		} else {
			my_active->Hide();
			my_active = NULL;
		}
	}
	return true;
}

bool PG_MenuBar::enterButton( PG_Pointer last ) {
	MenuBarItem* item = static_cast<MenuBarItem*>(last);
	int x, y;
	if ( PG_Application::GetEventSupplier()->GetMouseState(x,y) & SDL_BUTTON_LEFT ) {
		handle_button( item->button, last );
	}
	return true;
}


bool PG_MenuBar::handle_button(PG_Button* button, PG_Pointer last) {
	MenuBarItem* item = static_cast<MenuBarItem*>(last);

	// check if we are visible
	if(item->popupmenu->IsVisible()) {
		item->popupmenu->Hide();
		my_active = NULL;
		return true;
	}

	if(my_active != NULL) {
		my_active->Hide();
		my_active = NULL;
	}

	my_active = item->popupmenu;
	my_active->openMenu(button->x, button->y + button->h);

	return true;
}

void PG_MenuBar::Cleanup() {
	for(std::vector<MenuBarItem*>::iterator i = ItemList.begin(); i != ItemList.end(); i = ItemList.begin()) {
		delete (*i)->button;
		delete (*i);
		ItemList.erase(i);
	}
}
