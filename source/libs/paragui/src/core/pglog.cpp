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
    Update Date:      $Date: 2006-02-15 21:30:16 $
    Source File:      $Source: /home/martin/asc/v2/svntest/games/asc/source/libs/paragui/src/core/pglog.cpp,v $
    CVS/RCS Revision: $Revision: 1.1.2.1 $
    Status:           $State  
*/

#include "pgapplication.h"
#include "pgwindow.h"
#include "pgmultilineedit.h"
#include "pglog.h"

#include <iostream>
#include <string>
#include <list>
#include <cstring>
#include <cstdarg>
#include <ctime>

Uint32 PG_LogMaxMessages = 200;
int PG_LogMethod = PG_LOGMTH_STDOUT;
static PG_LOG_LEVEL PG_LogLevel = PG_LOG_DBG;
static SDLKey PG_LogConsoleKey = SDLK_F12;

class PG_LogMessage_t {
public:
	PG_LOG_LEVEL	Id;
	time_t	TimeStamp;
	std::string	Text;

	PG_LogMessage_t(PG_LOG_LEVEL _id, const char* txt)
			: Id(_id), TimeStamp(time(0)) {
		Text = txt;
	}

}
;

inline std::ostream& operator<<(std::ostream& os, PG_LogMessage_t *msg) {
	os << msg->Text;
	return os;
}

static std::list<PG_LogMessage_t*> PG_LogMessages;
static PG_Window* PG_LogWindow = NULL;
static PG_MultiLineEdit* PG_LogWindowData = NULL;
static std::string my_title = "ParaGUI Log Console";

void PG_LogConsole::SetLogLevel(PG_LOG_LEVEL newlevel) {
	PG_LogLevel = newlevel;
}

void PG_LogConsole::LogVA(PG_LOG_LEVEL id, const char *Text, va_list ap) {
	char buffer[1024];
	PG_LogMessage_t* NewMsg;

	if(id == PG_LOG_NONE || id > PG_LogLevel) {
		return; // Don't log this type.
	}

#ifdef HAVE_VSNPRINTF
	vsnprintf(buffer, sizeof(buffer), Text, ap);
#else
	// ERROR PRONE!!! VC++ doesn't have vsnprintf _I think_...
	vsprintf(buffer, Text, ap);
#endif

	//Create new log item
	NewMsg = new PG_LogMessage_t(id, buffer);

	PG_LogMessages.push_front(NewMsg);

	//Check if there aren`t too many log messages
	while (PG_LogMessages.size() >= PG_LogMaxMessages) {
		PG_LogMessage_t *tmp = PG_LogMessages.back();
		PG_LogMessages.pop_back();

		delete tmp;
	}

	//Print to stdout
	if (PG_LogMethod & PG_LOGMTH_STDOUT) {
		switch (NewMsg->Id) {
			case PG_LOG_MSG :
				std::cout << "MESSAGE [";
				break;

			case PG_LOG_ERR :
				std::cout << "ERROR [";
				break;

			case PG_LOG_WRN :
				std::cout << "WARNING [";
				break;

			case PG_LOG_DBG :
				std::cout << "DEBUG [";
				break;

			default :
				std::cout << "??? [";
		}

		strftime(buffer, sizeof(buffer), "%m/%d/%Y %X", localtime(&NewMsg->TimeStamp));
		std::cout << buffer << "] > " << NewMsg << std::endl;
	}

	//Print to stderr
	if (PG_LogMethod & PG_LOGMTH_STDERR) {
		switch (NewMsg->Id) {
			case PG_LOG_MSG :
				std::cerr << "MESSAGE [";
				break;

			case PG_LOG_ERR :
				std::cerr << "ERROR [";
				break;

			case PG_LOG_WRN :
				std::cerr << "WARNING [";
				break;

			case PG_LOG_DBG :
				std::cerr << "DEBUG [";
				break;

			default :
				std::cerr << "??? [";
		}

		strftime(buffer, sizeof(buffer), "%m/%d/%Y %X", localtime(&NewMsg->TimeStamp));
		std::cerr << buffer << "] > " << NewMsg << std::endl;
	}

	return;
}

void PG_LogConsole::Done() {
	std::list<PG_LogMessage_t*>::iterator it = PG_LogMessages.begin();

	while (it!=PG_LogMessages.end()) {
		delete *it;
		PG_LogMessages.erase(it);
		it = PG_LogMessages.begin();
	}
	PG_LogMessages.clear();

	if (PG_LogWindow) {
		delete PG_LogWindow;
		PG_LogWindow = NULL;
	}
}

void PG_LogConsole::Update() {
	if ((PG_LogMethod & PG_LOGMTH_CONSOLE) == 0) {
		return;
	}

	//If LogWindow is not initialized and it is possible, do it ...
	if (PG_LogWindow == NULL) {
		PG_Rect r(25,100,PG_Application::GetScreenWidth()-50,300);
		PG_LogWindow = new PG_Window(NULL, r, my_title.c_str(), PG_Window::SHOW_CLOSE, "Window", 25);
		PG_LogWindowData = new PG_MultiLineEdit(PG_LogWindow, PG_Rect(1,26,r.w-2,r.h-27));
		PG_LogWindowData->SetEditable(false);
	}

	std::string buffer;
	for(std::list<PG_LogMessage_t*>::reverse_iterator it = PG_LogMessages.rbegin();
	        it != PG_LogMessages.rend(); it++) {
		PG_LogMessage_t *Msg = *it;
		char timebuf[128];
		strftime(timebuf, sizeof(timebuf), "%m/%d/%Y %X", localtime(&Msg->TimeStamp));
		buffer += timebuf;

		switch (Msg->Id) {
			case PG_LOG_MSG :
				buffer += " MESSAGE >";
				break;

			case PG_LOG_ERR :
				buffer += " ERROR >";
				break;

			case PG_LOG_WRN :
				buffer += " WARNING >";
				break;

			case PG_LOG_DBG :
				buffer += " DEBUG >";
				break;

			default :
				buffer += " ????? >";
		}

		buffer += Msg->Text;
		buffer += "\n";
	}
	PG_LogWindowData->SetText(buffer);
}

void PG_LogConsole::SetTitle(const char* title, PG_Label::TextAlign alignment) {
	my_title = title;

	if (PG_LogWindow) {
		PG_LogWindow->SetTitle(title, alignment);
	}
}

void PG_LogConsole::Show() {
	if(PG_LogWindow) {
		PG_LogWindow->Show();
	}
}

void PG_LogConsole::Hide() {
	if(PG_LogWindow) {
		PG_LogWindow->Hide();
	}
}

void PG_LogConsole::Toggle() {
	if(PG_LogWindow == NULL) {
		return;
	}

	if (PG_LogWindow->IsVisible())
		PG_LogWindow->Hide();
	else {
		PG_LogWindow->Show();
	}
}

void PG_LogConsole::SetMethod(int method) {
	PG_LogMethod = method;
}

int PG_LogConsole::GetMethod() {
	return PG_LogMethod;
}

void PG_LogConsole::SetConsoleKey(SDLKey key) {
	PG_LogConsoleKey = key;
}

SDLKey PG_LogConsole::GetConsoleKey() {
	return PG_LogConsoleKey;
}

void PG_Log(PG_LOG_LEVEL id, const char *Text, ...) {
	va_list ap;
	va_start(ap, Text);
	PG_LogConsole::LogVA(id, Text, ap);
	va_end(ap);
}


void PG_LogMSG(const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	PG_LogConsole::LogVA(PG_LOG_MSG, fmt, ap);
	va_end(ap);
}

void PG_LogERR(const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	PG_LogConsole::LogVA(PG_LOG_ERR, fmt, ap);
	va_end(ap);
}

void PG_LogWRN(const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	PG_LogConsole::LogVA(PG_LOG_WRN, fmt, ap);
	va_end(ap);
}

void PG_LogDBG(const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	PG_LogConsole::LogVA(PG_LOG_DBG, fmt, ap);
	va_end(ap);
}

void PG_LogConsole::SetMaxLogLines(Uint32 max) {
	PG_LogMaxMessages = max;
}

/*
 * Local Variables:
 * c-basic-offset: 8
 * End:
 */
