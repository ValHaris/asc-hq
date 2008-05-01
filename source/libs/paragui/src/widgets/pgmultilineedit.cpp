#include "pgapplication.h"
#include "pgmultilineedit.h"
#include "pglog.h"

using namespace std;

PG_MultiLineEdit::PG_MultiLineEdit(PG_Widget* parent, const PG_Rect& r, const std::string& style, int maximumLength)
		: PG_LineEdit(parent, r, style, maximumLength) {
	my_vscroll = new PG_ScrollBar(this, PG_Rect(r.w-16,0,16,r.h));
	my_vscroll->MoveWidget(PG_Rect(r.w-my_vscroll->w, 0, my_vscroll->w, r.h));
	my_isCursorAtEOL = false;
	my_allowHiddenCursor = false;
	my_firstLine = 0;
	my_vscroll->sigScrollPos.connect(slot(*this, &PG_MultiLineEdit::handleScroll));
	my_vscroll->sigScrollTrack.connect(slot(*this, &PG_MultiLineEdit::handleScroll));
	my_vscroll->Hide();
	my_mark = -1;
}

bool PG_MultiLineEdit::handleScroll(PG_ScrollBar* widget, long data) {
	SetVPosition(my_vscroll->GetPosition());
	my_allowHiddenCursor = true;
	return true;
}

void PG_MultiLineEdit::SetVPosition(int line) {
	if (line < 0) {
		line = 0;
	}

	if (line > my_vscroll->GetMaxRange()) {
		line = my_vscroll->GetMaxRange();
	}

	my_firstLine = line;

	if (my_vscroll->GetPosition() != line) {
		my_vscroll->SetPosition(line);
	}

	Update();
}

void PG_MultiLineEdit::eventBlit(SDL_Surface* surface, const PG_Rect& src, const PG_Rect& dst) {
	PG_ThemeWidget::eventBlit(surface, src, dst);
	DrawText(dst);
}

void PG_MultiLineEdit::DrawText(const PG_Rect& dst) {
	int _x = 3;
	int _y = 3;

	// figure out the cursor position that we start at
	int pos = 0;
	for (unsigned int i = 0; i < (unsigned int)my_firstLine; ++i) {
		pos += my_textdata[i].size();
	}

	// draw text
	int maxLines = my_height/GetFontSize() + 1;
	int endpos, start, end;

	int x1 = 0;
	Uint16 w = 0;
	int offset = 0;
	PG_Color color(GetFontColor());
	PG_Color inv_color(255 - color.r, 255 - color.g, 255 - color.b);
	SDL_Surface* screen = PG_Application::GetScreen();
	Uint32 highlightColor = SDL_MapRGB(screen->format, color.r, color.g, color.b);
	PG_String s, middlepart;

	for (unsigned int i = my_firstLine; i < (unsigned int)my_firstLine + maxLines && i < my_textdata.size(); ++i) {
		endpos = pos + my_textdata[i].size();
		start = (my_cursorPosition < my_mark ? my_cursorPosition : my_mark);
		end  = (my_cursorPosition >= my_mark ? my_cursorPosition : my_mark);

		// check if we are in the highlighted section
		if (my_mark != -1 && my_mark != my_cursorPosition && pos <= end && endpos >= start) {
			x1 = _x;
			offset = 0;

			// draw the initial unhighlighted part
			if (pos < start) {
				s = my_textdata[i].substr(0, start-pos);
				PG_Widget::DrawText(x1, _y, s);
				PG_FontEngine::GetTextSize(s, GetFont(), &w);
				x1 += w;
				offset = start-pos;
			}

			middlepart = my_textdata[i].substr(offset);

			// check if the end part is unhighlighted
			if (endpos > end) {
				middlepart = middlepart.substr(0, middlepart.size() - (endpos-end));
				s = my_textdata[i].substr(end - pos, my_textdata[i].size() - (end - pos));
				PG_FontEngine::GetTextSize(middlepart, GetFont(), &w);
				PG_Widget::DrawText(x1+w, _y, s);
			}

			SetFontColor(inv_color);
			PG_FontEngine::GetTextSize(middlepart, GetFont(), &w);
			if(_y < my_height) {
				SDL_Rect rect = {x + x1, y + _y, w, (_y + GetFontHeight() > my_height) ? my_height - _y : GetFontHeight()};
				SDL_FillRect(screen, &rect, highlightColor);
			}
			PG_Widget::DrawText(x1, _y, middlepart);
			SetFontColor(color);
		} else {
			PG_Widget::DrawText(_x, _y, my_textdata[i]);
		}
		_y += GetFontHeight();
		pos += my_textdata[i].size();
	}

	// should we draw the cursor ?
	if(IsCursorVisible()) {
		DrawTextCursor();
	}
}

void PG_MultiLineEdit::DrawTextCursor() {
   
  
	int x = my_xpos + 1;
	int y = my_ypos + 1;
	int xpos, ypos;
	GetCursorPos(xpos, ypos);

	// check for a hidden cursor
	if(!my_allowHiddenCursor) {
		// scroll up for cursor
		while (ypos < 0 && my_firstLine > 0) {
			SetVPosition(--my_firstLine);
			GetCursorPos(xpos, ypos);
		}

		// scroll down for cursor
		while (ypos + GetFontHeight() > my_height && my_firstLine < my_vscroll->GetMaxRange()) {
			SetVPosition(++my_firstLine);
			GetCursorPos(xpos, ypos);
		}
	}

	// draw simple cursor
	if(my_srfTextCursor == NULL) {
		DrawVLine(xpos + 2, ypos + 2, GetFontHeight()-4, PG_Color());
	}
	// draw a nice cursor bitmap
	else {
		PG_Rect src, dst;
		PG_Rect rect(x + xpos, y + ypos + GetFontHeight()/2 - my_srfTextCursor->h/2,
		             my_srfTextCursor->w, my_srfTextCursor->h);
		GetClipRects(src, dst, rect);
		PG_Widget::eventBlit(my_srfTextCursor, src, dst);
	}
}

void PG_MultiLineEdit::FindWordRight() {
	unsigned int currentPos = my_cursorPosition;

	// step off the initial space
	++currentPos;

	// find the next space
	while (currentPos-1 <= my_text.size() && my_text[currentPos-1] != ' ' && my_text[currentPos-1] != '\n') {
		++currentPos;
	}

	// go to the end of multiple spaces
	while (currentPos <= my_text.size() && (my_text[currentPos] == ' ' || my_text[currentPos] == '\n')) {
		++currentPos;
	}

	SetCursorPos(currentPos);
}

void PG_MultiLineEdit::FindWordLeft() {
	unsigned int currentPos = my_cursorPosition;

	// step off the initial space(s)
	while (currentPos-1 >= 0 && (my_text[currentPos-1] == ' ' || my_text[currentPos-1] == '\n')) {
		--currentPos;
	}

	// find the next space
	while (currentPos-1 >= 0 && my_text[currentPos-1] != ' ' && my_text[currentPos-1] != '\n') {
		--currentPos;
	}

	SetCursorPos(currentPos);
}

void PG_MultiLineEdit::GetCursorTextPosFromScreen(int x, int y, unsigned int& horzOffset, unsigned int& lineOffset) {
	// check for an empty text box
	if (my_textdata.size() == 0) {
		horzOffset = 0;
		lineOffset = 0;
		return;
	}

	// get the line number
	int ypos = (y - my_ypos - 3) / GetFontHeight() + my_firstLine;

	// stay within limits
	if (ypos < 0) {
		ypos = 0;
	}

	if ((unsigned int)ypos >= my_textdata.size()) {
		ypos = my_textdata.size()-1;
	}

	unsigned int min = static_cast<unsigned int>(-1);
	unsigned int min_xpos = 0;

	// loop through to find the closest x position
	PG_String temp;

	for (Uint16 i = 0; i <= my_textdata[ypos].size(); ++i) {
		// get the string up to that point
		temp = my_textdata[ypos].substr(0, i);

		// get the distance for that section
		Uint16 w;
		PG_FontEngine::GetTextSize(temp, GetFont(), &w);
		unsigned int dist = abs(x - (my_xpos + 3 + w));

		// update minimum
		if (dist < min) {
			min = dist;
			min_xpos = i;
		}
	}

	// set return data
	horzOffset = min_xpos;
	lineOffset = static_cast<unsigned int>(ypos);
}

void PG_MultiLineEdit::GetCursorTextPos(unsigned int& horzOffset, unsigned int& lineOffset) {
	// check for an empty text box
	if (my_textdata.size() == 0) {
		horzOffset = 0;
		lineOffset = 0;
		return;
	}

	unsigned int currentPos = my_cursorPosition;
	unsigned int line = 0;

	// cycle through the lines, finding where our cursor lands
	for (vector<PG_String>::iterator i = my_textdata.begin(); i != my_textdata.end(); ++i) {
      if(currentPos < i->size() ) 
			break;
		
      if ( currentPos <= i->size() && currentPos > 0 && (*i)[currentPos-1] != '\n')
         break;
      
      if ( !i->size() )
         break;
      
		currentPos -= i->size();
		line++;
	}

	// if we're too far, assume we're at the end of the string
	if (line >= my_textdata.size()) {
		line = my_textdata.size()-1;
		currentPos = my_textdata[line].size();
	}

	// if we're too far on this line, assum we're at the end of line
	if (currentPos > my_textdata[line].size()) {
		currentPos = my_textdata[line].size();
	}

	horzOffset = currentPos;
	lineOffset = line;
}

void PG_MultiLineEdit::GetCursorPos(int& x, int& y) {
	// check for an empty text box
	if (my_textdata.size() == 0) {
		x = 0;
		y = 0;
		return;
	}

	// get the cursor text position
	unsigned int currentPos, line;
	GetCursorTextPos(currentPos, line);

	// now get the x,y position
	PG_String temp = my_textdata[line].substr(0, currentPos);

	Uint16 w;
	PG_FontEngine::GetTextSize(temp, GetFont(), &w);

	x = w;
	y = (line - my_firstLine)*GetFontHeight();
}

void PG_MultiLineEdit::CreateTextVector(bool bSetupVScroll) {
	int w = my_width - 6 - ((my_vscroll->IsVisible() || !my_vscroll->IsHidden()) ? my_vscroll->w : 0);

	// now split the text into lines
	my_textdata.clear();
	unsigned int start = 0, end = 0, last = 0;

	PG_String temp;

	do {
		Uint16 lineWidth = 0;
		temp = my_text.substr(start, end-start);
		PG_FontEngine::GetTextSize(temp, GetFont(), &lineWidth);

		if (lineWidth > w) {
			if (last == start) {
				PG_String s = my_text.substr(start, end-start-1);
				my_textdata.push_back(s);
				start = --end;
			} else {
				temp = my_text.substr(start, last-start);
				my_textdata.push_back(temp);
				start = last;
				end = last-1;
			}
			last = start;
		} else if (my_text[end] == ' ') {
			last = end+1;
		} else if (my_text[end] == '\n' || my_text[end] == '\0' || end == my_text.size()-1) {
			temp = my_text.substr(start, end-start+1);
			my_textdata.push_back(temp);
			start = end+1;
			last = start;
		}
	} while (end++ < my_text.size());

	// setup the scrollbar
	if(bSetupVScroll) {
		SetupVScroll();
	}
}

void PG_MultiLineEdit::SetupVScroll() {
	if (my_textdata.size()*GetFontHeight() < my_height) {
		my_vscroll->SetRange(0, 0);
		my_vscroll->Hide();
		SetVPosition(0);
		CreateTextVector(false);
	} else {
		my_vscroll->SetRange(0, my_textdata.size() - my_height/GetFontHeight() + 1);
		my_vscroll->SetPageSize( my_height/GetFontHeight() );
		if (my_firstLine > my_vscroll->GetMaxRange()) {
			SetVPosition(my_vscroll->GetMaxRange());
		}

		if (!my_vscroll->IsVisible() || my_vscroll->IsHidden()) {
			// scrollbar makes the window less wide, so we have to redo the text
			// (note: don't switch these next two lines, unless you like infinite loops)
			my_vscroll->Show();
			CreateTextVector(false);
		}
	}
}

bool PG_MultiLineEdit::eventKeyDown(const SDL_KeyboardEvent* key) {
	PG_Char c;

	if(!IsCursorVisible()) {
		return false;
	}

	SDL_KeyboardEvent key_copy = *key; // copy key structure
	PG_Application::TranslateNumpadKeys(&key_copy);

	if ((key_copy.keysym.mod & KMOD_SHIFT) && my_mark == -1) {
		my_mark = my_cursorPosition;
	}

	if(key_copy.keysym.mod & KMOD_CTRL) {
		switch(key_copy.keysym.sym) {
			case SDLK_HOME:
				if (!(key_copy.keysym.mod & KMOD_SHIFT)) {
					my_mark = -1;
				}
				SetCursorPos(0);
				return true;

			case SDLK_END:
				if (!(key_copy.keysym.mod & KMOD_SHIFT)) {
					my_mark = -1;
				}
				SetCursorPos(my_text.length());
				return true;

			case SDLK_RIGHT:
				if (!(key_copy.keysym.mod & KMOD_SHIFT)) {
					my_mark = -1;
				}
				FindWordRight();
				return true;

			case SDLK_LEFT:
				if (!(key_copy.keysym.mod & KMOD_SHIFT)) {
					my_mark = -1;
				}
				FindWordLeft();
				return true;

			case SDLK_UP:
				my_allowHiddenCursor = true;
				SetVPosition(--my_firstLine);
				return true;

			case SDLK_DOWN:
				my_allowHiddenCursor = true;
				SetVPosition(++my_firstLine);
				return true;

			default:
				break;
		}
	} else if(key_copy.keysym.mod & (KMOD_ALT | KMOD_META)) {}
	else {
		unsigned int currentPos, line;
		int x, y;

		switch(key_copy.keysym.sym) {
			case SDLK_LEFT:
				if (!(key_copy.keysym.mod & KMOD_SHIFT)) {
					my_mark = -1;
				}
				PG_LineEdit::SetCursorPos(--my_cursorPosition);
				return true;

			case SDLK_RIGHT:
				if(!(key_copy.keysym.mod & KMOD_SHIFT)) {
					my_mark = -1;
				}
				PG_LineEdit::SetCursorPos(++my_cursorPosition);
				return true;

			case SDLK_UP:
				if (!(key_copy.keysym.mod & KMOD_SHIFT)) {
					my_mark = -1;
				}
				GetCursorPos(x, y);
				GetCursorTextPosFromScreen(my_xpos + x + 3, my_ypos + y + 3 - GetFontHeight(), currentPos, line);
				SetCursorTextPos(currentPos, line);
				return true;

			case SDLK_DOWN:
				if (!(key_copy.keysym.mod & KMOD_SHIFT)) {
					my_mark = -1;
				}
				GetCursorPos(x, y);
				GetCursorTextPosFromScreen(my_xpos + x + 3, my_ypos + y + 3 + GetFontHeight(), currentPos, line);
				SetCursorTextPos(currentPos, line);
				return true;

			case SDLK_PAGEUP:
				if (!(key_copy.keysym.mod & KMOD_SHIFT)) {
					my_mark = -1;
				}
				GetCursorPos(x, y);
				GetCursorTextPosFromScreen(my_xpos + x + 3, my_ypos + y + 3 - (my_height - GetFontHeight()), currentPos, line);
				SetCursorTextPos(currentPos, line);
				return true;

			case SDLK_PAGEDOWN:
				if (!(key_copy.keysym.mod & KMOD_SHIFT)) {
					my_mark = -1;
				}

				GetCursorPos(x, y);
				GetCursorTextPosFromScreen(my_xpos + x + 3, my_ypos + y + 3 + (my_height - GetFontHeight()), currentPos, line);
				SetCursorTextPos(currentPos, line);
				return true;

			case SDLK_HOME:
				if (!(key_copy.keysym.mod & KMOD_SHIFT)) {
					my_mark = -1;
				}
				GetCursorTextPos(currentPos, line);
				SetCursorTextPos(0, line);
				return true;

			case SDLK_END: {
					if (!(key_copy.keysym.mod & KMOD_SHIFT)) {
						my_mark = -1;
					}
					GetCursorTextPos(currentPos, line);
					int cursorPos = my_textdata[line].size() - (my_textdata[line][my_textdata[line].size()-1] == '\n' ? 1 : 0);
					SetCursorTextPos(cursorPos, line);
				}
				return true;

			case SDLK_KP_ENTER:
			case SDLK_RETURN:
				c = '\n';
				InsertChar(c);
				SetCursorPos(my_cursorPosition);
				return true;

			default:
				break;
		}
	}

	return PG_LineEdit::eventKeyDown(key);
}

bool PG_MultiLineEdit::eventMouseButtonDown(const SDL_MouseButtonEvent* button) {
	// check for mousewheel
	if ((button->button == 4 || button->button == 5) && my_vscroll->IsVisible()) {
		if (button->button == 4) {
			SetVPosition(--my_firstLine);
		} else {
			SetVPosition(++my_firstLine);
		}
		return true;
	}

	if (!GetEditable()) {
		return false;
	}

	if (!IsCursorVisible()) {
		EditBegin();
	}

	// if we're clicking the scrollbar....
	if (my_vscroll->IsVisible() && button->x > my_xpos + my_width - my_vscroll->w) {
		return false;
	}

	if (button->button == 1) {
		Uint8* keys = SDL_GetKeyState(NULL);

		if (!(keys[SDLK_LSHIFT] || keys[SDLK_RSHIFT])) {
			my_mark = -1;
		}

		unsigned int currentPos, line;
		GetCursorTextPosFromScreen(button->x, button->y, currentPos, line);
		SetCursorTextPos(currentPos, line);
		if (!(keys[SDLK_LSHIFT] || keys[SDLK_RSHIFT])) {
			my_mark = my_cursorPosition;
		}
	}

	return true;
}

bool PG_MultiLineEdit::eventMouseMotion(const SDL_MouseMotionEvent* motion) {
	if (motion->state & SDL_BUTTON(1)) {
		unsigned int currentPos, line;
		GetCursorTextPosFromScreen(motion->x, motion->y, currentPos, line);
		SetCursorTextPos(currentPos, line);
	}

	return PG_LineEdit::eventMouseMotion(motion);
}

bool PG_MultiLineEdit::eventMouseButtonUp(const SDL_MouseButtonEvent* button) {
	return true;
}

void PG_MultiLineEdit::SetCursorTextPos(unsigned int offset, unsigned int line) {
	my_allowHiddenCursor = false;
	if (line < 0) {
		SetCursorPos(0);
	} else if (line >= my_textdata.size()) {
		SetCursorPos(my_text.size());
		my_isCursorAtEOL = false;
	} else {
		PG_LineEdit::SetCursorPos(ConvertCursorPos(offset, line));
		my_isCursorAtEOL = (offset == my_textdata[line].size() && my_textdata[line].size() != 0);
		Update();
	}
}

int PG_MultiLineEdit::ConvertCursorPos(unsigned int offset, unsigned int line) {
	unsigned int charCount = 0;
	for (unsigned int i = 0; i < line; ++i) {
		charCount += my_textdata[i].size();
	}

	return charCount+offset;
}

void PG_MultiLineEdit::SetCursorPos(int p) {
	my_isCursorAtEOL = false;
	my_allowHiddenCursor = false;
	PG_LineEdit::SetCursorPos(p);
	//Update();
}

void PG_MultiLineEdit::InsertText(const std::string& c) {
	my_allowHiddenCursor = false;
	if (my_mark != -1 && my_mark != my_cursorPosition) {
		DeleteSelection();
	}

   PG_Application::BulkModeActivator bulk;
   for ( int i = 0; i < c.length(); ++i )
	  PG_LineEdit::InsertChar(c[i]);
   bulk.disable();
	my_mark = -1;
	CreateTextVector();
	Update();
}

void PG_MultiLineEdit::InsertChar(const PG_Char& c) {
   std::string s;
   s += c;
   InsertText( s );
}


void PG_MultiLineEdit::DeleteChar(Uint16 pos) {
	my_allowHiddenCursor = false;
	if (my_mark != -1 && my_mark != my_cursorPosition) {
		Uint16 oldpos = my_cursorPosition;
		DeleteSelection();
		// check if backspace was pressed
		if (pos == oldpos-1) {
			my_cursorPosition++;
		}
	} else {
		PG_LineEdit::DeleteChar(pos);
	}

	my_mark = -1;
	CreateTextVector();
	// Update();
}

void PG_MultiLineEdit::DeleteSelection() {
	if (my_mark != -1 && my_mark != my_cursorPosition) {
		int start = (my_cursorPosition < my_mark ? my_cursorPosition : my_mark);
		int end  = (my_cursorPosition >= my_mark ? my_cursorPosition : my_mark);
		my_text.erase(start, end-start);
		if (my_mark < my_cursorPosition) {
			SetCursorPos(my_mark);
		}
		my_mark = -1;
	}
}

void PG_MultiLineEdit::SetText(const std::string& new_text) {
	PG_LineEdit::SetText(new_text);
	CreateTextVector();
	my_isCursorAtEOL = false;
	my_allowHiddenCursor = false;
	my_mark = -1;
	SetVPosition(0);
}
