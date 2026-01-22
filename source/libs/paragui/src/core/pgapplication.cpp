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
    Update Date:      $Date: 2010-04-17 17:46:45 $
    Source File:      $Source: /home/martin/asc/v2/svntest/games/asc/source/libs/paragui/src/core/pgapplication.cpp,v $
    CVS/RCS Revision: $Revision: 1.7 $
    Status:           $State: Exp $
*/

#include "pgapplication.h"
#include "pgwidget.h"
#include "pglayout.h"
#include "pglog.h"
#include "pgdraw.h"
#include "pgtheme.h"
#include "pgeventsupplier.h"
#include "pgsdleventsupplier.h"
#include "pgsdlscreenupdater.h"

#include <iostream>
#include <cstring>
#include <cassert>
#include <cmath>

// usually PARAGUI_THEMEDIR is defined by the configure script
// or passed to the compiler. This is just a kind of last resort.

#ifndef PARAGUI_THEMEDIR
#ifdef __MACOS__
#define PARAGUI_THEMEDIR        ""
#else
#define PARAGUI_THEMEDIR        "./"
#endif  // macintosh
#endif  // PARAGUI_THEMEDIR

SDL_mutex* PG_Application::mutexScreen = NULL;
PG_Application* PG_Application::pGlobalApp = NULL;
SDL_Surface* PG_Application::screen = NULL;
SDL_Window* PG_Application::mainWindow = NULL;
SDL_Renderer* PG_Application::mainWindowRenderer = NULL;
SDL_Texture* PG_Application::mainWindowTexture = NULL;
//std::string PG_Application::app_path = "";
PG_Theme* PG_Application::my_Theme = NULL;
bool PG_Application::bulkMode = false;
//bool PG_Application::glMode = false;
//bool PG_Application::emergencyQuit = false;
bool PG_Application::enableBackground = true;
//bool PG_Application::enableAppIdleCalls = false;
SDL_Surface *PG_Application::my_mouse_pointer = NULL;
SDL_Surface *PG_Application::my_mouse_backingstore = NULL;
PG_Rect PG_Application::my_mouse_position = PG_Rect(0,0,0,0);
PG_Application::CursorMode PG_Application::my_mouse_mode = PG_Application::HARDWARE;
PG_Font* PG_Application::DefaultFont = NULL;
SDL_Surface* PG_Application::my_background = NULL;
SDL_Surface* PG_Application::my_scaled_background = NULL;
PG_Color PG_Application::my_backcolor;
PG_Draw::BkMode PG_Application::my_backmode = PG_Draw::TILE;
bool PG_Application::disableDirtyUpdates = false;
//bool PG_Application::my_quitEventLoop = false;
bool PG_Application::defaultUpdateOverlappingSiblings = true;
PG_Char PG_Application::highlightingTag = 0;
PG_SDLScreenUpdater defaultScreenUpdater;
PG_ScreenUpdater* PG_Application::my_ScreenUpdater = &defaultScreenUpdater;
PG_Application::ScreenInitialization PG_Application::screenInitialized = PG_Application::None;

/**
	new shutdown procedure (called at application termination
*/
void PARAGUI_ShutDownCode() {

    // If screen initialization fails and XError is shutting down the application, we observed that SDL_Quit could hang
    // so we only do it after the screen has been successfully setup
    if ( PG_Application::isScreenInitialized() == PG_Application::Finished )
        SDL_Quit();
    else if ( PG_Application::isScreenInitialized() == PG_Application::Trying ){
        std::cerr << "If ASC fails to start up with graphics, try running \"asc -w\" to use windowed mode\n";
        std::cerr << "You can specify the resolution too: asc -w -x 1024 -y 740 \n";
    }
}

PG_Application::ScreenInitialization PG_Application::isScreenInitialized() {
    return screenInitialized;
}




PG_Application::PG_Application()
: my_quitEventLoop(false), emergencyQuit(false), enableAppIdleCalls(false) {

	// set UTF8 encoding if UNICODE support is enabled
	// we use the "C" locale because it's hard to get the current locale setting
	// in a portableway (Win32 doesn't support nl_langinfo).
	// The "C" locale is only set for character encoding.
#ifdef ENABLE_UNICODE
	setlocale(LC_CTYPE, "C.UTF-8");
#endif

	if(pGlobalApp != NULL) {
		PG_LogWRN("PG_Application Object already exists !");
		exit(-1);
	}

	atexit(PARAGUI_ShutDownCode);

	/* We need to kludge a bit for keyboards work under windows */
	/* we'll call SDL_RegisterApp right before SDL_Init()       */
	/* Pete Shinners, Feb 1, 2001                               */

#ifdef WIN32
#ifndef __GNUC__

	SDL_RegisterApp("ParaGUI", 0, GetModuleHandle(NULL));
#endif
#endif

	// -- see above

	/* Initialize the SDL library */
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_NOPARACHUTE) < 0) {
		std::cerr << "Could not initialize SDL: " << SDL_GetError() << std::endl;
		exit(-1);
	}

	pGlobalApp = this;
	screen = NULL;

	mutexScreen = SDL_CreateMutex();
	my_background = NULL;
	my_freeBackground = false;
	my_backmode = PG_Draw::TILE;

	// add our base dir to the searchpath
	AddArchive(GetBaseDir());
}

PG_Application::~PG_Application() {
	// shutdown log (before deleting all the widgets)
	PG_LogConsole::Done();

	// remove remaining widgets
	Shutdown();

	pGlobalApp = NULL;

	// remove all archives from PG_FileArchive
	PG_FileArchive::RemoveAllArchives();
   SDL_Quit();

}

/**  */
bool PG_Application::InitScreen(int w, int h, bool fullscreen) {

	if ( fullscreen ) {
		SDL_CreateWindowAndRenderer(0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP, &mainWindow, &mainWindowRenderer);
		SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
		if ( SDL_RenderSetLogicalSize(mainWindowRenderer,w,h) != 0 )
			throw PG_Exception("PG_Application::InitScreen renderSetLogicalSIze", SDL_GetError());
	} else {
		SDL_CreateWindowAndRenderer(w, h, 0, &mainWindow, &mainWindowRenderer);
	}
	if ( !mainWindow || !mainWindowRenderer )
		throw PG_Exception("PG_Application::InitScreen", SDL_GetError());


	screen = SDL_CreateRGBSurfaceWithFormat(0, w, h, 32, SDL_PIXELFORMAT_ARGB8888);
	mainWindowTexture = SDL_CreateTexture(mainWindowRenderer,
	                                            SDL_PIXELFORMAT_ARGB8888,
	                                            SDL_TEXTUREACCESS_STREAMING,
	                                            w, h);



	screenInitialized = Finished;

	eventInit();

	PG_LogConsole::SetMethod( PG_LogConsole::GetMethod() |PG_LOGMTH_CONSOLE);
	PG_LogDBG("Screen initialized !");

	return true;
}

void PG_Application::UpdateScreen(const SDL_Rect * srcrect, int numRects)
{
	SDL_UpdateTexture(mainWindowTexture, NULL, screen->pixels, screen->pitch);
	// SDL_RenderClear(mainWindowRenderer);
	if ( !srcrect || numRects==0 )
		SDL_RenderCopy(mainWindowRenderer, mainWindowTexture, NULL, NULL);
	else
		for (int i = 0; i < numRects; ++i)
			SDL_RenderCopy(mainWindowRenderer, mainWindowTexture, &srcrect[i], &srcrect[i]);

	SDL_RenderPresent(mainWindowRenderer);
}


/**  */
void PG_Application::Run() {
	RunEventLoop();
}

void PG_Application::EnableAppIdleCalls(bool enable) {
	enableAppIdleCalls = enable;
}

bool PG_Application::GetAppIdleCallsEnabled() {
	return enableAppIdleCalls;
}

/** Event processing loop */

void PG_Application::RunEventLoop() {
	SDL_Event event;
	my_quitEventLoop = false;

	FlushEventQueue();

	while(!my_quitEventLoop) {
		ClearOldMousePosition();

		if(enableAppIdleCalls) {
			if ( GetEventSupplier()->PollEvent(&event) == 0) {
				eventIdle();
			} else {
				PumpIntoEventQueue(&event);
			}
		} else {
			if( GetEventSupplier()->WaitEvent(&event) != 1) {
				SDL_Delay(10);
				continue;
			}
			PumpIntoEventQueue(&event);
		}

		DrawCursor();
	}
}


void PG_Application::ClearOldMousePosition() {
	if(!my_mouse_position.my_width) {
		return;
	}

	if(GetBulkMode() || my_mouse_backingstore==NULL || my_mouse_mode != SOFTWARE ) {
		return;
	}

   SDL_BlitSurface(my_mouse_backingstore, NULL, GetScreen(), &my_mouse_position);

	return;
}

void PG_Application::DrawCursor(bool update) {
	int x, y;

	if(!my_mouse_pointer || my_mouse_mode != SOFTWARE) {
		return;
	}

	if(SDL_ShowCursor(SDL_QUERY) == SDL_ENABLE) {
		// Hide hardware cursor if visible
		SDL_ShowCursor(SDL_DISABLE);
	}

	GetEventSupplier()->GetMouseState(x, y);

	Sint16 dx = x - my_mouse_position.my_xpos;
	Sint16 dy = y - my_mouse_position.my_ypos;

	// calculate vertical update rect
	PG_Rect vertical(0, my_mouse_position.my_ypos, abs(dx), my_mouse_pointer->h + abs(dy));
	if(dx >= 0) {
		vertical.my_xpos = my_mouse_position.my_xpos;
	} else {
		vertical.my_xpos = my_mouse_position.my_xpos + my_mouse_pointer->w + dx;
	}

	// calculate horizontal update rect
	PG_Rect horizontal(my_mouse_position.my_xpos, 0, my_mouse_pointer->w + abs(dx), abs(dy));
	if(dy >= 0) {
		horizontal.my_ypos = my_mouse_position.my_ypos;
	} else {
		horizontal.my_ypos = my_mouse_position.my_ypos + my_mouse_pointer->h + dy;
	}

	// clipping
	if(vertical.my_xpos + vertical.my_width > screen->w) {
		if(vertical.my_xpos >= screen->w) {
			vertical.my_xpos = screen->w - 1;
		}
		vertical.my_width = screen->w - vertical.my_xpos;
	}
	if(vertical.my_ypos + vertical.my_height > screen->h) {
		if(vertical.my_ypos >= screen->h) {
			vertical.my_ypos = screen->h - 1;
		}
		vertical.my_height = screen->h - vertical.my_ypos;
	}

	if(horizontal.my_xpos + horizontal.my_width > screen->w) {
		if(horizontal.my_xpos >= screen->w) {
			horizontal.my_xpos = screen->w - 1;
		}
		horizontal.my_width = screen->w - horizontal.my_xpos;
	}
	if(horizontal.my_ypos + horizontal.my_height > screen->h) {
		if(horizontal.my_ypos >= screen->h) {
			horizontal.my_ypos = screen->h - 1;
		}
		horizontal.my_height = screen->h - horizontal.my_ypos;
	}

	my_mouse_position.my_xpos = x;
	my_mouse_position.my_ypos = y;
	my_mouse_position.my_width = my_mouse_pointer->w;
	my_mouse_position.my_height = my_mouse_pointer->h;

	// backup current cursor area
	if(my_mouse_backingstore == NULL) {
		my_mouse_backingstore = PG_Draw::CreateRGBSurface(my_mouse_pointer->w, my_mouse_pointer->h);
	}
	SDL_BlitSurface(GetScreen(), &my_mouse_position, my_mouse_backingstore, NULL);

	// draw cursor
	SDL_BlitSurface(my_mouse_pointer, 0, screen, &my_mouse_position);

	if(!GetBulkMode() && update) {
		SDL_Rect rects[3] = {horizontal, vertical, my_mouse_position};
		UpdateRects(screen, 3, rects);
	}
}

void PG_Application::Quit() {
	sigQuit(this);
	eventQuit(0, this, 0);
}

/**  */
bool PG_Application::eventKeyDown(const SDL_KeyboardEvent* key) {
	SDL_KeyCode ckey = PG_LogConsole::GetConsoleKey();

	if(ckey == 0) {
		return false;
	}

	if (key->keysym.sym == ckey) {
		PG_LogConsole::Update();
		PG_LogConsole::Toggle();
		return true;
	}

	return false;
}

bool PG_Application::eventKeyUp(const SDL_KeyboardEvent* key) {

	if((key->keysym.sym == SDLK_ESCAPE) && emergencyQuit) {
		Quit();
		return true;
	}

	return false;
}

void PG_Application::SetCursor(SDL_Surface *image) {
	if(image == 0) {
		if(!my_mouse_pointer) {
			return;
		}
		UnloadSurface(my_mouse_backingstore);
		my_mouse_backingstore = NULL;
		UnloadSurface(my_mouse_pointer);
		my_mouse_pointer = NULL;
		ClearOldMousePosition();
		UpdateRects(screen, 1, &my_mouse_position);
		SDL_ShowCursor(SDL_ENABLE);
		return;
	}
	if(!my_mouse_pointer) {
		my_mouse_pointer = image;
	} else {
		UnloadSurface(my_mouse_pointer);
		my_mouse_pointer = image;
	}

	UnloadSurface(my_mouse_backingstore);
	my_mouse_backingstore = NULL;
	image->refcount++;
	DrawCursor();
}

PG_Application::CursorMode PG_Application::ShowCursor(CursorMode mode) {
	switch(mode) {
		case NONE:
			SDL_ShowCursor(SDL_DISABLE);
			break;
		case HARDWARE:
			SDL_ShowCursor(SDL_ENABLE);
			break;
		case SOFTWARE:
			DrawCursor();
			break;
		case QUERY:
			return my_mouse_mode;

	}
	CursorMode orig = my_mouse_mode;
	if(mode != SOFTWARE && my_mouse_mode == SOFTWARE) {
		ClearOldMousePosition();
		UpdateRects(screen, 1, &my_mouse_position);
	}
	my_mouse_mode = mode;
	return orig;
}


/**  */
bool PG_Application::SetBackground(const std::string& filename, PG_Draw::BkMode mode) {
	if (filename.empty()) {
		return false;
	}

	if ( !SetBackground ( LoadSurface(filename), mode, true ))
		PG_LogWRN("Failed to load '%s'",(char*)filename.c_str());

	return false;
}

/**  */
bool PG_Application::SetBackground(SDL_Surface* surface, PG_Draw::BkMode mode, bool freeBackground ) {
	if(surface == NULL)
		return false;

	if(my_scaled_background) {
		// Destroyed scaled background if present
		SDL_FreeSurface(my_scaled_background);
		my_scaled_background = 0;
	}
	if(my_freeBackground && my_background) {
		UnloadSurface(my_background);
		my_freeBackground = freeBackground;
	}

	my_background = surface;
	my_backmode = mode;

	RedrawBackground(PG_Rect(0,0,screen->w,screen->h));
	PG_Widget::GetWidgetList()->Blit();
	if(!GetBulkMode() )
		UpdateRect(screen,0,0,GetScreen()->w,GetScreen()->h);
	return true;
}

/**  */
void PG_Application::RedrawBackground(const PG_Rect& rect) {
	if(GetBulkMode()) {
		return;
	}

	PG_Rect fillrect = rect;

	if(!my_background || !enableBackground) {
		SDL_FillRect(screen, const_cast<PG_Rect*>(&fillrect), my_backcolor.MapRGB(screen->format));
		return;
	}
	if(my_backmode == PG_Draw::STRETCH &&
	        (my_background->w != screen->w ||
	         my_background->h != screen->h)) {
		if(my_scaled_background &&
		        (my_scaled_background->w != screen->w ||
		         my_scaled_background->h != screen->h)) {
			UnloadSurface(my_scaled_background); // size mismatch
			my_scaled_background = NULL;
		}
		if(!my_scaled_background) {
			my_scaled_background = PG_Draw::ScaleSurface(my_background, static_cast<Uint16>(screen->w), static_cast<Uint16>(screen->h));
		}
		SDL_GetClipRect(screen, const_cast<PG_Rect*>(&fillrect));
		SDL_SetClipRect(screen, const_cast<PG_Rect*>(&rect));
		SDL_SetSurfaceAlphaMod(my_scaled_background, 0);
		SDL_BlitSurface(my_scaled_background, const_cast<PG_Rect*>(&rect), screen, const_cast<PG_Rect*>(&rect));
		SDL_SetClipRect(screen, const_cast<PG_Rect*>(&fillrect));

	} else {
		SDL_SetSurfaceAlphaMod(my_background, 0);
		PG_Draw::DrawTile(screen, PG_Rect(0,0,screen->w,screen->h), rect, my_background);
	}
}

/**  */
const std::string& PG_Application::GetRelativePath(const std::string& file) {
	static std::string buffer = "";

	if(Exists(file)) {
		buffer = (std::string)GetRealDir(file) + file;
	}

	return buffer;
}

#ifdef DEBUG
// All calls to this are ifdefd out also
void PG_Application::PrintVideoTest() {
	const SDL_VideoInfo *info;
	int i;
	SDL_Rect **modes;

	info = SDL_GetVideoInfo();
	PG_LogDBG("Current display: %d bits-per-pixel", info->vfmt->BitsPerPixel);

	if ( info->vfmt->palette == NULL ) {
		// FIXME: did I screw this up? :) -Dave
		PG_LogDBG(" - Red Mask = 0x%x", info->vfmt->Rmask);
		PG_LogDBG(" - Green Mask = 0x%x", info->vfmt->Gmask);
		PG_LogDBG(" - Blue Mask = 0x%x", info->vfmt->Bmask);
	}
	/* Print available fullscreen video modes */
	modes = SDL_ListModes(NULL, SDL_FULLSCREEN);
	if ( modes == (SDL_Rect **)0 ) {
		PG_LogDBG("No available fullscreen video modes");
	} else
		if ( modes == (SDL_Rect **)-1 ) {
			PG_LogDBG("No special fullscreen video modes");
		} else {
			PG_LogDBG("Fullscreen video modes:");
			for ( i=0; modes[i]; ++i ) {
				PG_LogDBG(" - %d x %d", modes[i]->w, modes[i]->h);
			}
		}
	if ( info->wm_available ) {
		PG_LogDBG("A window manager is available");
	}
	if ( info->hw_available ) {
		PG_LogDBG("Hardware surfaces are available (%d K video memory)", info->video_mem);
	}
	if ( info->blit_hw ) {
		PG_LogDBG("Copy blits between hardware surfaces are accelerated");
	}
	if ( info->blit_hw_CC ) {
		PG_LogDBG("Colorkey blits between hardware surfaces are accelerated");
	}
	if ( info->blit_hw_A ) {
		PG_LogDBG("Alpha blits between hardware surfaces are accelerated");
	}
	if ( info->blit_sw ) {
		PG_LogDBG("Copy blits from software surfaces to hardware surfaces are accelerated");
	}
	if ( info->blit_sw_CC ) {
		PG_LogDBG("Colorkey blits from software surfaces to hardware surfaces are accelerated");
	}
	if ( info->blit_sw_A ) {
		PG_LogDBG("Alpha blits from software surfaces to hardware surfaces are accelerated");
	}
	if ( info->blit_fill ) {
		PG_LogDBG("Color fills on hardware surfaces are accelerated");
	}
}
#endif // DEBUG

void PG_Application::eventInit() {}

PG_Theme* PG_Application::LoadTheme(const std::string& xmltheme, bool asDefault, const std::string& searchpath) {
	PG_Theme* theme = NULL;

	PG_LogDBG("Locating theme '%s' ...", xmltheme.c_str());

	if(!searchpath.empty()) {
		if(AddArchive(searchpath)) {
			PG_LogDBG("'%s' added to searchpath", searchpath.c_str());
		}
	}

#ifdef __MACOS__

	if(AddArchive("")) {
		PG_LogDBG("'' added to searchpath");
	}

	if(AddArchive(":")) {
		PG_LogDBG("':' added to searchpath");
	}

	if(AddArchive(":data:")) {
		PG_LogDBG("':data:' added to searchpath");
	}

	if(AddArchive("::data:")) {
		PG_LogDBG("'::data:' added to searchpath");
	}

	if(PARAGUI_THEMEDIR != NULL) {
		PG_LogDBG("'"PARAGUI_THEMEDIR"' added to searchpath");
	}

#else

	if(AddArchive("./")) {
		PG_LogDBG("'./' added to searchpath");
	}

	if(AddArchive("../")) {
		PG_LogDBG("'../' added to searchpath");
	}

	if(AddArchive("./data/")) {
		PG_LogDBG("'./data/' added to searchpath");
	}

	if(AddArchive("../data/")) {
		PG_LogDBG("'../data/' added to searchpath");
	}

	if(getenv("PARAGUIDIR") != NULL) {
		if(AddArchive(getenv("PARAGUIDIR"))) {
			PG_LogDBG("'%s' added to searchpath", getenv("PARAGUIDIR"));
		}
	}

#endif // __MACOS__

	if(AddArchive(PARAGUI_THEMEDIR)) {
		PG_LogDBG("'" PARAGUI_THEMEDIR "' added to searchpath");
	}

	theme = PG_Theme::Load(xmltheme);

	if(theme && asDefault) {

		const std::string& c = theme->FindDefaultFontName();
		if(c.empty()) {
			PG_LogWRN("Unable to load default font ...");
			delete theme;
			return NULL;
		}

		DefaultFont = new PG_Font(c, theme->FindDefaultFontSize());
		DefaultFont->SetStyle(theme->FindDefaultFontStyle());

		PG_LogMSG("defaultfont: %s", c.c_str());
		PG_LogMSG("size: %i", DefaultFont->GetSize());

		my_background = theme->FindSurface("Background", "Background", "background");
		theme->GetProperty("Background", "Background", "backmode", my_backmode);
		theme->GetColor("Background", "Background", "backcolor", my_backcolor);

		if(my_scaled_background) {
			// Destroyed scaled background if present
			SDL_FreeSurface(my_scaled_background);
			my_scaled_background = 0;
		}
	} else {

		PG_LogWRN("Failed to load !");
	}

	if((my_Theme != NULL) && asDefault) {
		delete my_Theme;
		my_Theme = NULL;
	}

	if(asDefault && theme) {
		my_Theme = theme;
	}

	return theme;
}

bool PG_Application::eventQuit(int id, PG_MessageObject* widget, unsigned long data) {
	if(GetBulkMode()) {
		return false;
	}

	my_quitEventLoop = true;

	if(my_mouse_pointer) {
		UnloadSurface(my_mouse_pointer);
		my_mouse_pointer = 0;
		SDL_ShowCursor(SDL_ENABLE);
	}

	return true;
}

void PG_Application::SetBulkMode(bool bulk) {
	bulkMode = bulk;
}

void PG_Application::Shutdown() {
	DeleteBackground();

	// destroy still existing objects
	PG_Widget* list = PG_Widget::GetWidgetList()->first();

	while((list = PG_Widget::GetWidgetList()->first()) != NULL) {
		delete list;
	}

	// unload theme (and free the allocated mem)
	if(my_Theme) {
		delete my_Theme;
		my_Theme = NULL;
	}

	// destroy screen mutex
	SDL_DestroyMutex(mutexScreen);

	// delete the default font
	delete DefaultFont;
	DefaultFont = NULL;

	// remove cursor backing store
	UnloadSurface(my_mouse_backingstore);
	my_mouse_backingstore = NULL;
}

void PG_Application::SetEmergencyQuit(bool esc) {
	emergencyQuit = esc;
}

void PG_Application::SetIcon(const std::string& filename) {
	SDL_Surface* icon;
	Uint8* pixels;
	Uint8* mask;
	int mlen, i;

	// Load the icon surface
	icon = LoadSurface(filename);
	if ( icon == NULL ) {
		PG_LogWRN("Failed to load icon!");
		return;
	}

	// Check width and height
	if ( (icon->w%8) != 0 ) {
		PG_LogWRN("Icon width must be a multiple of 8!");
		UnloadSurface(icon);
		return;
	}

	//Check the palette
	if ( icon->format->palette == NULL ) {
		PG_LogWRN("Icon must have a palette!");
		UnloadSurface(icon);
		return;
	}

	// Set the colorkey
	SDL_SetColorKey(icon, SDL_TRUE, *((Uint8 *)icon->pixels));

	//Set icon
	if ( icon != NULL ) {
		SDL_SetWindowIcon(mainWindow, icon);
	}
}


PG_SDLEventSupplier sdleventSupplier;

PG_EventSupplier* PG_Application::GetEventSupplier()
{
	return &sdleventSupplier;
}


void PG_Application::SetIcon(SDL_Surface* icon) {
	if ( icon != NULL ) {
		SDL_SetWindowIcon(mainWindow, icon);
	}
}


void PG_Application::SetCaption(const std::string& title, const std::string& icon) {
	SDL_SetWindowTitle(mainWindow, title.c_str());
	if (!icon.empty()) {
		SetIcon(icon);
	}
}


bool PG_Application::LoadLayout(const std::string& name) {
	return PG_Layout::Load(NULL, name, NULL, NULL);
}

bool PG_Application::LoadLayout(const std::string& name, void (* WorkCallback)(int now, int max)) {
	return PG_Layout::Load(NULL, name, WorkCallback, NULL);
}

bool PG_Application::LoadLayout(const std::string& name, void (* WorkCallback)(int now, int max), void *UserSpace) {
	return PG_Layout::Load(NULL, name, WorkCallback, UserSpace);
}

static PG_Widget *FindInChildObjects(PG_RectList *RectList, const std::string& Name) {
	PG_Widget *retWidget = NULL;

	if (RectList == NULL) {
		return NULL;
	}

	if(Name.empty()) {
		return NULL;
	}

	PG_Widget* list = RectList->first();

	while(list != NULL) {
		if(list->GetName() == Name) {
			return list;
		}

		PG_Widget* result = list->FindChild(Name);
		if(result != NULL) {
			return result;
		}

		retWidget = FindInChildObjects(list->GetChildList(), Name);
		if (retWidget != NULL) {
			return retWidget;
		}

		list = list->next();
	}

	return NULL;
}

static inline PG_Widget *FindInChildObjects(PG_RectList *RectList, int id) {
	PG_Widget *retWidget = NULL;

	if (RectList == NULL) {
		return NULL;
	}

	if (id < 0)
		return 0;

	PG_Widget* list = RectList->first();

	while(list != NULL) {
		if (list->GetID() == id) {
			return list;
		}


		PG_Widget* result = list->FindChild(id);
		if(result != NULL) {
			return result;
		}

		retWidget = FindInChildObjects(list->GetChildList(), id);
		if (retWidget != NULL) {
			return retWidget;
		}

		list = list->next();
	}

	return NULL;
}

PG_Widget* PG_Application::GetWidgetByName(const std::string& Name) {
	return (FindInChildObjects(PG_Widget::GetWidgetList(), Name));
}

PG_Widget *PG_Application::GetWidgetById(int id) {
	return (FindInChildObjects(PG_Widget::GetWidgetList(), id));
}

void PG_Application::SetFontColor(const PG_Color& Color) {
	DefaultFont->SetColor(Color);
}

void PG_Application::SetFontAlpha(int Alpha) {
	DefaultFont->SetAlpha(Alpha);
}

void PG_Application::SetFontStyle(PG_Font::Style Style) {
	DefaultFont->SetStyle(Style);
}

void PG_Application::SetFontSize(int Size) {
	DefaultFont->SetSize(Size);
}

void PG_Application::SetFontIndex(int Index) {
	//DefaultFont->SetIndex(Index);
}

void PG_Application::SetFontName(const std::string& Name) {
	DefaultFont->SetName(Name);
}

/*SDL_Surface* PG_Application::GetScreen() {
	return screen;
}*/

int PG_Application::GetScreenHeight() {
	return screen->h;
}

int PG_Application::GetScreenWidth() {
	return screen->w;
}

PG_Theme* PG_Application::GetTheme() {
	return my_Theme;
}

bool PG_Application::GetBulkMode() {
	return bulkMode;
}

/*bool PG_Application::GetGLMode() {
		return glMode;
}*/

void PG_Application::EnableBackground(bool enable) {
	enableBackground = enable;
}

void PG_Application::DeleteBackground() {
	enableBackground = false;

	if(my_scaled_background) {
		// Destroyed scaled background if present
		SDL_FreeSurface(my_scaled_background);
		my_scaled_background = 0;
	}
	if(my_freeBackground && my_background) {
		UnloadSurface(my_background);
		my_freeBackground = false;
	}

	my_background = 0;
}

void PG_Application::DisableDirtyUpdates(bool disable) {
	disableDirtyUpdates = disable;
}

bool PG_Application::GetDirtyUpdatesDisabled() {
	return disableDirtyUpdates;
}

PG_Application* PG_Application::GetApp() {
	return pGlobalApp;
}

void PG_Application::FlushEventQueue() {
	SDL_Event event;

	while(GetEventSupplier()->PollEvent(&event)) {
		/*if(event.type == SDL_USEREVENT) {
			delete (MSG_MESSAGE*)(event.user.data1);
		}*/
	}
}

void PG_Application::eventIdle() {
	sigAppIdle( this );
	SDL_Delay(1);
}

void print(const SDL_WindowEvent& event) {
	std::cout << "Window Event: " << (int)event.event << "\n";
}

void print(const SDL_TextInputEvent& event) {
	std::cout << "Text Input: " << event.text << "\n";
}

void print(const SDL_MouseButtonEvent& event) {
	std::cout << "Mouse Button Event: X=" << (int)event.x << " Y=" << (int)event.y << " State=" << (int)event.state << " Button=" << (int)event.button << "\n";
}

void print(const SDL_MouseMotionEvent& event) {
//    std::cout << "Mouse Motion Event: X=" << (int)event.x << " Y=" << (int)event.y << "\n";
}


void print(const SDL_Event* event) {
	switch ( event->type ) {
	case SDL_WINDOWEVENT:
		print(event->window);
		break;
	case SDL_TEXTINPUT:
		print(event->text);
		break;
	case SDL_MOUSEBUTTONDOWN:
	case SDL_MOUSEBUTTONUP:
		print(event->button);
		break;
	case SDL_MOUSEMOTION:
	   print(event->motion);
	   break;
	}
}

bool PG_Application::PumpIntoEventQueue(const SDL_Event* event) {
	PG_Widget* widget = NULL;

	print(event);

	// do we have a capture hook?
	if((event->type != SDL_USEREVENT) && (event->type != SDL_WINDOWEVENT)) {
		if(captureObject) {
			return captureObject->ProcessEvent(event);
		}
	}

	switch(event->type) {

	    case SDL_TEXTINPUT:
		case SDL_KEYDOWN:
		case SDL_KEYUP:
			if(inputFocusObject) {
				// first send it to the focus object
				if(inputFocusObject->ProcessEvent(event)) {
					return true;
				}
				// if the focus object doesn't respond -> pump it into the queue
			}
			break;

		case SDL_MOUSEMOTION:
			widget = PG_Widget::FindWidgetFromPos(event->motion.x, event->motion.y);

			if(lastwidget && (lastwidget != widget)) {
				lastwidget->eventMouseLeave();
				lastwidget = NULL;
			}

			if(widget) {
				lastwidget = widget;
				widget->ProcessEvent(event);
				return true;
			}
			return true;

		case SDL_MOUSEBUTTONUP:
		case SDL_MOUSEBUTTONDOWN:
			widget = PG_Widget::FindWidgetFromPos(event->button.x, event->button.y);
			if(widget) {
				widget->ProcessEvent(event);
				return true;
			}
			break;
	}

	// first let's try to process the event ourself
	if(ProcessEvent(event)) {
		return true;
	}

	// send to all receivers ( = all top level widgdets )
	bool processed = false;
	PG_Widget* list = PG_Widget::GetWidgetList()->first();
	//vector<PG_MessageObject*>::iterator list = objectList.begin();
	PG_Widget* o = NULL;
	while(list != NULL) {
		o = list;
		/*if(o == NULL) {
			list = o->next();
			PG_Widget::GetWidgetList()->Remove(o);
			continue;
		}*/
		if(o->ProcessEvent(event)) {
			processed = true;
			break;		// exit loop if an object responds
		}
		list = list->next();
	}

	return processed;
}


bool PG_Application::eventWindow(const SDL_WindowEvent* event)
{
	if (event->event == SDL_WINDOWEVENT_EXPOSED) {
		UpdateScreen(NULL, 0);
		return true;
	} else
		return false;

}


void PG_Application::SetUpdateOverlappingSiblings(bool update) {
	defaultUpdateOverlappingSiblings = update;
}

bool PG_Application::GetUpdateOverlappingSiblings() {
	return defaultUpdateOverlappingSiblings;
}

volatile int lockCount = 0;
/*
bool PG_Application::LockScreen() {
   
   int res = SDL_mutexP(mutexScreen);
   ++lockCount;
   printf("Lock screen %d\n", lockCount);
   fflush(stdout);
   return (res == 0);
}

bool PG_Application::UnlockScreen() {
   printf("UnLock screen %d\n", lockCount);
   --lockCount;
   fflush(stdout);
   
   return (SDL_mutexV(mutexScreen) == 0);
}
*/


void PG_Application::SetHighlightingTag( PG_Char c )
{
   highlightingTag = c;
}
   
PG_Char PG_Application::GetHighlightingTag()
{
   return highlightingTag;
}


/*
 * Local Variables:
 * c-basic-offset: 8
 * End:
 */
