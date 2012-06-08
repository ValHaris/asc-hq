/*
   ParaGUI - crossplatform widgetset
   Copyright (C) 2000,2001,2002  Alexander Pipelka
 
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
 
   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.
 
   You should have received a copy of the GNU Library General Public
   License along with this library; if not, write to the Free
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 
   Alexander Pipelka
   pipelka@teleweb.at
 
   Last Update:      $Author: mbickel $
   Update Date:      $Date: 2009-04-18 13:48:40 $
   Source File:      $Source: /home/martin/asc/v2/svntest/games/asc/source/libs/paragui/src/widgets/pgrichedit.cpp,v $
   CVS/RCS Revision: $Revision: 1.3 $
   Status:           $State: Exp $
*/


#include "pgapplication.h"
#include "pgrichedit.h"
#include "pgscrollarea.h"
#include "pglog.h"

const Uint32 PG_RichEdit::my_Marks[PG_RichEdit::MARKS_COUNT] = { ' ', 0x01, '\n', '\t', 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0xF
                                                               };
const Uint32 PG_RichEdit::my_FontBeginMark = 010;


PG_RichEdit::PG_RichEdit(PG_Widget* parent, const PG_Rect& r, bool autoResize, Uint32 linewidth, Uint32 tabSize, Uint32 childsborderwidth, const std::string& style) :
PG_ScrollWidget(parent, r, style) {

	EnableScrollBar(true, PG_ScrollBar::HORIZONTAL);

	my_LineWidth = (linewidth != 0) ? linewidth : r.my_width;
	my_scrollarea->SetAreaWidth(my_LineWidth);
	my_ChildsBorderWidth = childsborderwidth;

	//TO-DO : Value 5 is font size, witch is currently unknown ...
	my_objHorizontalScrollbar->SetLineSize(5);
	my_TabSize = tabSize;
	my_Align = my_Marks[PG_RichEdit::PG_TEXT_LEFT];
	my_AutoVerticalResize = autoResize;
	my_AutoHorizontalResize = autoResize;

	my_objVerticalScrollbar->sigScrollTrack.connect(slot(*this, &PG_RichEdit::handleScrollTrack));
	my_objVerticalScrollbar->sigScrollPos.connect(slot(*this, &PG_RichEdit::handleScrollTrack));
	my_objHorizontalScrollbar->sigScrollTrack.connect(slot(*this, &PG_RichEdit::handleScrollTrack));
	my_objHorizontalScrollbar->sigScrollPos.connect(slot(*this, &PG_RichEdit::handleScrollTrack));
}

void PG_RichEdit::SetAlignment(Uint8 align) {
	my_Align = my_Marks[align];
}

void PG_RichEdit::SetAutoResize(bool bHorizontal, bool bVertical) {
	my_AutoHorizontalResize = bHorizontal;
	my_AutoVerticalResize = bVertical;
}

/*void PG_RichEdit::UpdateScrollBarsPos() {
	//PG_WidgetList::UpdateScrollBarsPos();
 
	//TO-DO : Value 5 is font size, witch is currently unknown ...
	my_objVerticalScrollbar->SetLineSize(5);
	my_objHorizontalScrollbar->SetLineSize(5);
}*/

void PG_RichEdit::eventBlit(SDL_Surface* srf, const PG_Rect& src, const PG_Rect& dst) {

	PG_ScrollWidget::eventBlit(srf, src, dst);

	RichLineArray::iterator line;
	int deltax = (my_objHorizontalScrollbar->IsVisible()) ? my_scrollarea->GetScrollPosX() : 0;
	int deltay = (my_objVerticalScrollbar->IsVisible()) ? my_scrollarea->GetScrollPosY() : 0;

	for (line = my_RichText.begin(); line != my_RichText.end(); line++) {
		if ((Sint32)(line->my_BaseLine - deltay) < 0) {
			continue;
		}

		RichLinePartArray::iterator linePart;

		for (linePart = line->my_LineParts.begin(); linePart != line->my_LineParts.end(); linePart++) {
			Size_tArray::iterator word;
			Uint32 width = 0;

			for (word = linePart->my_WordIndexes.begin(); word != linePart->my_WordIndexes.end(); word++) {
				PG_FontEngine::RenderText(PG_Application::GetScreen(), dst, my_xpos - deltax + width + linePart->my_Left,  my_ypos + line->my_BaseLine - deltay, my_ParsedWords[*word].my_Word, GetFont());
				width += my_ParsedWords[*word].my_WidthAfterFormating;
			}
		}

		if ((Sint32)(line->my_BaseLine - deltay - line->my_LineSpace) >= Height()) {
			break;
		}
	}
}

void PG_RichEdit::AddChild(PG_Widget* child) {
	PG_ScrollWidget::AddChild(child);
	CompleteLines();
}

bool PG_RichEdit::RemoveChild(PG_Widget* child) {
	bool result = PG_ScrollWidget::RemoveChild(child);
	CompleteLines();
	return result;
}

void PG_RichEdit::SetText(const std::string& text) {
	my_scrollarea->SetAreaWidth(my_LineWidth);
	my_scrollarea->SetAreaHeight(0);

	my_text = text;

	// trim the string
	bool bStop = false;
	char c;

	while(!my_text.empty() && !bStop) {
		c = my_text[my_text.size()-1];
		bStop = true;
		switch(c) {
			case '\n':
			case '\r':
			case '\t':
			case ' ':
				my_text = my_text.substr(0, my_text.size()-1);
				bStop = false;
		}
	}

	ParseWords();
	CompleteLines();
}

void PG_RichEdit::ParseWords() {
	size_t searchFrom = 0;
	static PG_String blank = " ";

	my_ParsedWords.clear();

	Uint16 my_sw = 0;
	PG_FontEngine::GetTextSize(blank, GetFont(), &my_sw);

	RichWordDescription wordDescr;
	PG_String word;

	Uint16 w, h, sw = 0;
	int bl, ls;
	bool space = false;
	int length = 0;

	do {
		sw = 0;
		space = false;

		searchFrom = GetWord(searchFrom, &word, &wordDescr.my_EndMark);

		length = word.length();
		if (length > 0) {
			if (word[length - 1] == ' ') {
				space = true;
				word.erase(length-1, 1);
			}
		}
		PG_FontEngine::GetTextSize(word, GetFont(), &w, &h, &bl, &ls);
		wordDescr.my_Width = w;
		if (space) {
			word += ' ';
			sw = my_sw;
		}
		wordDescr.my_Word = word;
		wordDescr.my_EndSpaceWidth = sw;
		wordDescr.my_WidthAfterFormating = w + sw;
		wordDescr.my_Height = h;
		wordDescr.my_BaseLine = bl;
		wordDescr.my_LineSkip = ls;

		my_ParsedWords.push_back(wordDescr);

		if ((searchFrom != std::string::npos) && (searchFrom < my_text.length()))
			searchFrom++;
		else
			break;
	} while (true);
}

size_t PG_RichEdit::GetWord(size_t searchFrom, PG_String *word, Uint32 *endMark) {
	size_t ik, length = my_text.length(), result = PG_String::npos;

	*endMark = my_Marks[MARK_SPACE];

	if (searchFrom >= my_text.length()) {
		return std::string::npos;
	}

	for (ik = searchFrom; ik < length; ik++) {
		for (Uint32 jk = 0; jk < MARKS_COUNT; jk++) {
			if ((Uint32)my_text[ik] == my_Marks[jk]) {
				*endMark = my_Marks[jk];
				result = ik;
				jk = MARKS_COUNT;
				ik = length;
				break;
			}
		}
	}

	if (result != std::string::npos) {
		size_t increment = 1;

		if ((Uint32)my_text[result] != my_Marks[MARK_SPACE]) {
			increment = 0;
		}
		*word = my_text.substr(searchFrom, result - searchFrom + increment);

		// Candid - cut too long words, if wanted
		if (!my_AutoHorizontalResize) {
			Uint16 width, i = word->size() - 1, w;
			PG_FontEngine::GetTextSize(*word, GetFont(), &width);

			if (width > my_width) {
				for (; width > my_width && i > 0; --i) {
					PG_FontEngine::GetTextSize(word->substr(i, 1), GetFont(), &w);
					width -= w;
				}
				result -= word->size() - i + 1;
				*word = word->substr(0, i);
			}
		}

		if ((Uint32)my_text[result] == my_Marks[MARK_NONBREAKABLE_SPACE]) {
			PG_String newword;

			*word += " ";
			result = GetWord(result + 1, &newword, endMark);
			*word += newword;
		}
	} else {
		*word = my_text.substr(searchFrom);
		// Candid - cut too long words, if wanted
		if (!my_AutoHorizontalResize) {
			Uint16 width, i = word->size() - 1, w;
			PG_FontEngine::GetTextSize(*word, GetFont(), &width);

			if (width > my_width) {
				for (; width > my_width && i > 0; --i) {
					PG_FontEngine::GetTextSize(word->substr(i, 1), GetFont(), &w);
					width -= w;
				}
				*word = word->substr(0, i);
			}
		}
	}

	return result;
}

Sint32 PG_RichEdit::CompleteLines() {
	my_RichText.clear();

	if (my_text.length() == 0)
		return 0;

	Uint32 lineSpace = 0;
	Sint32 top = 0;
	size_t searchFrom = 0;
	bool changeAlign = false;

	do {
		Uint32 lineAscent = 0;
		Uint32 lineSpaceOld = lineSpace;
		size_t searchFromOld = searchFrom;

		RichLineArray::iterator actualLine = my_RichText.insert(my_RichText.end(), RichLine(top));

		searchFrom = CompleteLine(actualLine, top, searchFrom, lineSpace, lineAscent, changeAlign);

		if (lineSpace != lineSpaceOld) {
			my_RichText.erase(actualLine);
			searchFrom = searchFromOld;
			changeAlign = true;
		} else {
			actualLine->my_BaseLine += lineAscent;

			actualLine->my_LineSpace = lineSpace;
			top += lineSpace;
			lineSpace = 0;
			changeAlign = false;
		}

	} while(searchFrom < my_ParsedWords.size());

	if (top > my_scrollarea->GetAreaHeight()) {
		my_scrollarea->SetAreaHeight(top);
		if( my_scrollarea->GetAreaWidth() != my_width - my_objVerticalScrollbar->my_width) { // my_objVerticalScrollbar->IsVisible() &&
			SetLineWidth(my_width - my_objVerticalScrollbar->my_width);
		}
	}

	if(my_AutoVerticalResize || my_AutoHorizontalResize) {
		Uint16 w = my_width, h = my_height;
		if (my_AutoVerticalResize) {
			h = GetListHeight();
		}
		if(my_AutoHorizontalResize) {
			w = GetListWidth();
		}
		SizeWidget(w, h, false);
	} else {
		CheckScrollBars();
	}

	Update();

	return top;
}

size_t PG_RichEdit::CompleteLine(RichLineArray::iterator actualLine, Sint32 &lineTop, size_t searchFrom, Uint32 &lineSpace, Uint32 &lineAscent, bool changeAlign) {
	bool breakLine = false;

	Sint32 linePartLeft = 0;
	Sint32 linePartWidthMax = my_scrollarea->GetAreaWidth();
	WidgetMap widgetsOnLine;

	Uint32 align = my_Align;

	GetWidgetsOnLine(lineTop, lineSpace, widgetsOnLine, true);

	WidgetMap::iterator childOnLine = widgetsOnLine.begin();

	if ((align == my_Marks[MARK_ALL_LEFT]) || (align == my_Marks[MARK_ALL_CENTER]) || (align == my_Marks[MARK_ALL_RIGHT])) {
		size_t size;
		Uint32 maxHeight = lineSpace;

		do {
			WidgetMap::iterator widget;

			for (widget = widgetsOnLine.begin(); widget != widgetsOnLine.end(); widget++) {
				PG_Point coord = ScreenToClient(widget->second->my_xpos, widget->second->my_ypos);
				if (widget->second->my_height > maxHeight)
					maxHeight = widget->second->my_height;
				//align widgets to line top
				widget->second->MoveWidget(coord.x, lineTop);
			}
			for (widget = widgetsOnLine.begin(); widget != widgetsOnLine.end(); widget++) {
				//align widgets to line bottom
				PG_Point coord = ScreenToClient(widget->second->my_xpos, widget->second->my_ypos);
				widget->second->MoveWidget(coord.x, coord.y + maxHeight - widget->second->my_height);
				coord = ScreenToClient(widget->second->my_xpos, widget->second->my_ypos);
				lineTop = (coord.y + widget->second->my_height) - lineSpace;
			}
			size = widgetsOnLine.size();
			GetWidgetsOnLine(lineTop, maxHeight, widgetsOnLine, false);
		} while (size != widgetsOnLine.size());

		actualLine->my_BaseLine = lineTop;

		childOnLine = widgetsOnLine.begin();
	}

	do {
		// !!!
		if (my_objVerticalScrollbar->IsVisible())
			linePartWidthMax = my_scrollarea->GetAreaWidth() - linePartLeft - my_objVerticalScrollbar->my_width;
		else
			linePartWidthMax = my_scrollarea->GetAreaWidth() - linePartLeft;
		if (childOnLine != widgetsOnLine.end())
			linePartWidthMax = childOnLine->first - linePartLeft;
		RichLinePartArray::iterator actualLinePart = actualLine->my_LineParts.insert(actualLine->my_LineParts.end(), RichLinePart(linePartLeft, linePartWidthMax));

		searchFrom = CompleteLinePart(searchFrom, lineTop, lineSpace, actualLine, actualLinePart, breakLine, lineAscent, changeAlign);
		if (childOnLine == widgetsOnLine.end())
			break;

		if ((align == my_Marks[MARK_TEXT_LEFT]) || (align == my_Marks[MARK_TEXT_CENTER]) || (align == my_Marks[MARK_TEXT_RIGHT]) || (align == my_Marks[MARK_TEXT_BLOCK])) {
			PG_Point coord = ScreenToClient(childOnLine->second->my_xpos, childOnLine->second->my_ypos);

			if (coord.y >= lineTop) {
				childOnLine->second->MoveWidget(coord.x, lineTop);
			}
		}

		linePartLeft = childOnLine->first + childOnLine->second->my_width + (2 * my_ChildsBorderWidth);
		childOnLine++;
	} while ((searchFrom < my_ParsedWords.size()) && (breakLine == false));

	AlignLine(actualLine, widgetsOnLine, align);


	return searchFrom;
}

void PG_RichEdit::AlignLine(RichLineArray::iterator actualLine, WidgetMap &widgetsOnLine, Uint32 align) {
	if ((align == my_Marks[MARK_ALL_LEFT]) || (align == my_Marks[MARK_ALL_CENTER]) || (align == my_Marks[MARK_ALL_RIGHT])) {
		WidgetMap::iterator widget;
		RichLinePartArray::iterator linePart;
		Sint32 lineWidth = 0, delta = 0;

		for (linePart = actualLine->my_LineParts.begin(); linePart < actualLine->my_LineParts.end(); linePart++) {
			Sint32 width = linePart->Width(my_ParsedWords);

			if (((Sint32)linePart->my_Left + width) > lineWidth)
				lineWidth = linePart->my_Left + width;
		}
		for (widget = widgetsOnLine.begin(); widget != widgetsOnLine.end(); widget++) {
			PG_Point coord = ScreenToClient(widget->second->my_xpos, widget->second->my_ypos);

			if ((coord.x + widget->second->my_width) > lineWidth)
				lineWidth = coord.x + widget->second->my_width;
		}

		if (align == my_Marks[MARK_ALL_CENTER])
			delta = (my_scrollarea->GetAreaWidth() / 2) - (lineWidth / 2);
		else if (align == my_Marks[MARK_ALL_RIGHT])
			delta = my_scrollarea->GetAreaWidth() - lineWidth;

		if (align != my_Marks[MARK_ALL_LEFT]) {
			for (linePart = actualLine->my_LineParts.begin(); linePart < actualLine->my_LineParts.end(); linePart++) {
				linePart->my_Left += delta;
			}
			for (widget = widgetsOnLine.begin(); widget != widgetsOnLine.end(); widget++) {
				PG_Point coord = ScreenToClient(widget->second->my_xpos, widget->second->my_ypos);

				widget->second->MoveWidget(coord.x + delta, coord.y);
			}
		}
	}
}

Sint32 PG_RichEdit::CompleteLinePart(size_t searchFrom, Sint32 lineTop, Uint32 &lineSpace, RichLineArray::iterator actualLine, RichLinePartArray::iterator actualLinePart, bool &breakLine, Uint32 &lineAscent, bool changeAlign) {
	breakLine = false;

	Sint32 w = 0, ls = 0, lb = 0;
	Sint32 lineWidth = 0;

	int align = my_Align;

	bool linePartEnd = false;

	while ((!linePartEnd) && (!breakLine)) {
		Uint32          oldFind = searchFrom, tabSize = 0;

		w = my_ParsedWords[searchFrom].my_Width + my_ParsedWords[searchFrom].my_EndSpaceWidth;
		lb = my_ParsedWords[searchFrom].my_BaseLine;
		ls = my_ParsedWords[searchFrom].my_LineSkip;

		if (my_ParsedWords[searchFrom].my_EndMark == my_Marks[MARK_ENTER]) {
			breakLine = true;
		} else if (my_ParsedWords[searchFrom].my_EndMark == my_Marks[MARK_TAB]) {
			if (my_TabSize > 0) {
				tabSize = my_TabSize - ((actualLinePart->my_Left + lineWidth + w) % my_TabSize);
				w += tabSize;
			}
		} else if ((my_ParsedWords[searchFrom].my_EndMark == my_Marks[MARK_TEXT_LEFT]   ||
		            my_ParsedWords[searchFrom].my_EndMark == my_Marks[MARK_TEXT_CENTER] ||
		            my_ParsedWords[searchFrom].my_EndMark == my_Marks[MARK_TEXT_RIGHT]  ||
		            my_ParsedWords[searchFrom].my_EndMark == my_Marks[MARK_TEXT_BLOCK]) &&
		           (my_ParsedWords[searchFrom].my_EndMark != my_Align)) {
			if (changeAlign) {
				my_Align = my_ParsedWords[searchFrom].my_EndMark;
				if ((lineWidth + w) > 0)    //if align change mark is not first character on the line part
				{
					linePartEnd = true;
				} else {
					align = my_Align;
				}
			}
		} else if ((my_ParsedWords[searchFrom].my_EndMark == my_Marks[MARK_ALL_LEFT]   ||
		            my_ParsedWords[searchFrom].my_EndMark == my_Marks[MARK_ALL_CENTER] ||
		            my_ParsedWords[searchFrom].my_EndMark == my_Marks[MARK_ALL_RIGHT]) &&
		           (my_ParsedWords[searchFrom].my_EndMark != my_Align)) {
			if (changeAlign) {
				my_Align = my_ParsedWords[searchFrom].my_EndMark;
				breakLine = true;
			}
		}

		if ((lineWidth + w) > actualLinePart->my_WidthMax) {
			if (my_ParsedWords[searchFrom].my_EndSpaceWidth > 0) {
				w = my_ParsedWords[searchFrom].my_Width;
			} else if (my_ParsedWords[searchFrom].my_EndMark == my_Marks[MARK_TAB]) {
				w -= tabSize;
			}
			linePartEnd = true;
		}

		lineWidth += w;

		if (lineWidth > actualLinePart->my_WidthMax) {
			my_Align = align;
			breakLine = false;
			searchFrom = oldFind - 1;

			if (w > actualLinePart->my_WidthMax) {
				if(my_AutoHorizontalResize) {
					searchFrom--;
					linePartEnd = false;
					my_scrollarea->SetAreaWidth(lineWidth);
				}
			} else if (actualLinePart->my_WordIndexes.size() == 0) {
				//searchFrom++;
				lineSpace = my_ParsedWords[searchFrom + 1].my_LineSkip;
			}
			//linePartEnd = true;
		} else {
			if ((my_ParsedWords[searchFrom].my_Word.length() > 0) || (tabSize > 0)) {
				if ((Uint32)ls > lineSpace)
					lineSpace = ls;
				if ((Uint32)lb > lineAscent)
					lineAscent = lb;

				actualLinePart->my_WordIndexes.push_back(searchFrom);
				my_ParsedWords[searchFrom].my_WidthAfterFormating = w;
			}
		}
		if (++searchFrom >= my_ParsedWords.size())
			linePartEnd = true;
	}

	//remove last space character in last word
	Sint32 nwords = actualLinePart->my_WordIndexes.size();

	if (nwords > 0)
		my_ParsedWords[actualLinePart->my_WordIndexes[nwords - 1]].my_WidthAfterFormating = my_ParsedWords[actualLinePart->my_WordIndexes[nwords - 1]].my_Width;

	AlignLinePart(actualLinePart, align, breakLine);
	return searchFrom;
}

void PG_RichEdit::AlignLinePart(RichLinePartArray::iterator actualLinePart, Uint32 align, bool breakLine) {
	Uint32 width = 0;

	if ((align == my_Marks[PG_TEXT_CENTER]) || (align == my_Marks[PG_TEXT_RIGHT]) || (align == my_Marks[PG_TEXT_BLOCK])) {
		width = actualLinePart->Width(my_ParsedWords);

		if (align == my_Marks[PG_TEXT_CENTER])
			actualLinePart->my_Left += (actualLinePart->my_WidthMax / 2) - (width / 2);
		else if (align == my_Marks[PG_TEXT_RIGHT])
			actualLinePart->my_Left += actualLinePart->my_WidthMax - width;
		else if (align == my_Marks[PG_TEXT_BLOCK]) {
			if (!breakLine) {
				Sint32 nwords = actualLinePart->my_WordIndexes.size();

				if (nwords > 1) {
					Uint32 addToSpace = (actualLinePart->my_WidthMax - width) / (nwords - 1);
					Sint32 rest = (actualLinePart->my_WidthMax - width) % (nwords - 1);

					for (Sint32 ik = 0, jk = 0; ik < (nwords - 1); ik++) {
						my_ParsedWords[actualLinePart->my_WordIndexes[ik]].my_WidthAfterFormating += (addToSpace + (jk++ < rest));
					}
				}
			}
		}
	}
}

void PG_RichEdit::GetWidgetsOnLine(Sint32 lineTop, Uint32 lineHeight, WidgetMap &widgetsOnLine, bool clear) {
	PG_Widget* child;

	if (clear)
		widgetsOnLine.clear();

	PG_RectList* list = my_scrollarea->GetChildList();
	if(list == NULL) {
		return;
	}

	for (child = list->first(); child != NULL; child = child->next()) {
		PG_Point coord = ScreenToClient(child->my_xpos, child->my_ypos);

		if ((lineTop < (coord.y + child->my_height)) && ((lineTop + (Sint32)lineHeight) >= coord.y)) {
			bool insert = true;

			if (!clear) {
				WidgetMap::iterator header;

				for (header = widgetsOnLine.begin(); header != widgetsOnLine.end(); header++) {
					if (child == header->second) {
						insert = false;
						break;
					}
				}
			}
			if (insert) {
				widgetsOnLine.insert(WidgetMap::value_type(coord.x - my_ChildsBorderWidth, child));
			}
		}
	}
}

bool PG_RichEdit::ProcessLongLine(PG_String &word, size_t &searchFrom, Uint32 oldFind, Sint32 lineTop, Uint32 &lineSpace, bool normalLine, RichLineArray::iterator actualLine, RichLinePartArray::iterator actualLinePart, Uint32 &lineAscent) {
	/*Sint32          w = 0, h = 0, ls = 0, la = 0;
	PG_FontParam    newFont; 
	PG_FontParam    CurFont = my_font;

	WidgetMap widgetsOnLine;
	   
	for (size_t ik = 0; ik < word.length(); ik++) 
	{                            
	    if (word[ik] == my_FontBeginMark)        
	    {
	    ik += CurFont.FormatTagProc(&word[ik], 0);
	    }
	    
	    string newword = word.substr(0, ((ik == string::npos) ? ik : ik + 1));           
	    PG_FontEngine::GetTextSize(newword.c_str(), GetFont(), &w, &h, &la, &ls, NULL, NULL, NULL);

	    if (w > (Sint32)my_listwidth)
	    {                                                                                                         
	        if (normalLine) 
	        {                                        
	            GetWidgetsOnLine(lineTop + lineSpace, lineSpace, widgetsOnLine, true);
	            
	            if (widgetsOnLine.size() > 0)
	            {
	                searchFrom = oldFind - 1;                                                                 
	                return false;
	            }
	        }
	        else my_RichText.erase(actualLine);
	        newword = word.substr(0, ik);                        
	        PG_FontEngine::GetTextSize(newword.c_str(), GetFont(), &w, &h, &la, &ls, NULL, NULL, NULL);
	        actualLine = my_RichText.insert(my_RichText.end(), RichLine(lineTop));            
	        actualLinePart = actualLine->my_LineParts.insert(actualLine->my_LineParts.end(), RichLinePart(0, my_listwidth));
	        actualLinePart->my_Left = 0;                    
	        actualLinePart->my_Words.push_back(RichWordDescription(newword, (Uint32)w));               
	        *defaultFont = newFont;            
	                    
	        if ((Uint32)ls > lineSpace) lineSpace = ls;
	        if ((Uint32)la > lineAscent) lineAscent = la;

	        if (searchFrom == my_text.npos) searchFrom = my_text.length() - 1;                                                        
	        searchFrom -= (word.length() - ik);  
	        break;            
	    }  
	}    */
	return true;
}

bool PG_RichEdit::LoadText(const std::string& textfile) {
	PG_DataContainer* text = PG_FileArchive::ReadFile(textfile);

	if(text == NULL) {
		return false;
	}

	// Hmm,...
	// size() returns the number of loaded bytes. We have to terminate the text with 0.
	// Last character will be truncated by this (i love valgrind).
	text->data()[text->size()-1] = 0;
	SetText(text->data());

	delete text;
	return true;
}

void PG_RichEdit::SetTabSize(Uint16 tabSize) {
	my_TabSize = tabSize;
}

bool PG_RichEdit::handleScrollTrack() {
	my_scrollarea->Update();
	return true;
}

void PG_RichEdit::SetLineWidth(Uint16 lineWidth) {
	my_LineWidth = lineWidth;
	SetText(GetText());
}

void PG_RichEdit::eventSizeWidget(Uint16 w, Uint16 h) {
	PG_ScrollWidget::eventSizeWidget(w, h);
	SetLineWidth(w);
}
