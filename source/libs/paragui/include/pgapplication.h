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
    Update Date:      $Date: 2010-04-17 17:46:44 $
    Source File:      $Source: /home/martin/asc/v2/svntest/games/asc/source/libs/paragui/include/pgapplication.h,v $
    CVS/RCS Revision: $Revision: 1.6 $
    Status:           $State: Exp $
*/

/** \file pgapplication.h
	Header file for the PG_Application class.
	This include file defines the PG_Application class the PG_Main function and some macros for source backward compatibility.
*/

#ifndef PG_APPLICATION_H
#define PG_APPLICATION_H

#include <list>
#include <utility>
#include <sigc++/sigc++.h>

#include "pgmessageobject.h"
#include "pgscreenupdater.h"
#include "pgfilearchive.h"
#include "pgfont.h"
#include "pgdraw.h"

class PG_EventSupplier;

class PG_Theme;
class PG_Widget;

/**
	@author Alexander Pipelka
 
	@short The class that represent an application in ParaGUI. It handles the main loop, screen attibutes etc.
 
	An application must have a maximum of one PG_Application. If you try to create more than one PG_Application
	the constructor will exit your application with an console error message.
 
        Every Paragui application needs to have a <code>PG_Application</code> object. The Application object is
        the controller for events, screen initialization, theme support and main loop.
        Here is an example of the steps necesary for creating a simple Paragui application that opens a 640x480
        window with a button in the middle.
 
        <pre>
        \code		
        #include <paragui.h>
        #include <pgapplication.h>
        #include <pgbutton.h>
        
        
        // Event handler for button click 
        PARAGUI_CALLBACK(buttonHandler) { 
          cout << "Ouch !\\n";
          return true;
        } 
          
        
        int main(  int argc, char **argv ) { 
          const int WIDTH  = 640; 
          const int HEIGHT = 480; 
          
          PG_Application app; 
          
          app.LoadTheme("qnx");                      // Load the QNX theme 
          app.InitScreen(640,480,16,SDL_SWSURFACE);  // SDL_SURFACE | SDL_FULLSCREEN for full screen support
 
          PG_Button btnOK(NULL,0, PG_Rect((WIDTH-100)/2,(HEIGHT-20)/2,100,20),"Push me !");
 
          btnOK.SetEventCallback(MSG_BUTTONCLICK,buttonHandler); // Set the event handler
          btnOK.Show(); 
           
          app.Run();                                 // Application loop, exit with Esc by default 
        } 
 
        \endcode
        </pre>
*/

class PG_XMLTag;

class DECLSPEC PG_Application : public PG_MessageObject, public PG_FileArchive, public PG_FontEngine  {
public:

	//! Cursor mode
	typedef enum {
	    QUERY, //!< Used to query the current mode
	    NONE,  //!< Show no cursor at all
	    HARDWARE, //!< Use hardware (standard SDL) cursor
	    SOFTWARE //!< Use ParaGUI software cursor (when possible)
	} CursorMode;

	enum {
	    IDAPPLICATION = PG_WIDGETID_INTERNAL + 100
	};

	/**
	Signal type declaration
	**/

class SignalXMLTag : public sigc::signal<bool, PG_XMLTag*> {}
	;
class SignalAppIdle : public sigc::signal<bool, PG_MessageObject*> {}
	;

	/**  */
	PG_Application();

	/**  */
	~PG_Application();

	/**
	Initialize the screen

	@param	w	screenwidth in pixels
	@param	h	screenheight in pixels
	@param	depth	screendepth in bits per pixel
	@param	flags	PG_ screen initialization flags
	*/
	bool InitScreen(int w, int h, int depth=0, Uint32 flags = SDL_SWSURFACE /* | SDL_FULLSCREEN*/ | SDL_HWPALETTE);

	/**
	Load a widget theme

	@param	xmltheme	name of the theme (e.g. default)
	@param	asDefault	apply the theme as default widgettheme
	@param	searchpath	path where the themefile is located
	*/
	PG_Theme* LoadTheme(const std::string& xmltheme, bool asDefault = true, const std::string& searchpath = PG_NULLSTR);

	/**
	Run the applications main eventloop.
	If theaded is false this function will exit when the eventloop quits (MSG_QUIT). If threaded is true
	it will return immediately and a thread processing events is started.
	CAUTION: Threaded eventloops are unsuported under Win32 (windows specific behavior)
	*/
	void Run();

	/**
	Run the modal message pump. This function will exit when the main window was closed.
	*/
	virtual void RunEventLoop();

	/**
	Exit the main eventloop
	*/
	void Quit();


	/**
	Waits the given amount of time, while still performing all necessary background tasks
	 */
	void Sleep ( int milliSeconds );

	/**
	Set a custom screen surface

	@param	screen	pointer to a surface
	@return		pointer the new screen surface
	*/
	SDL_Surface* SetScreen(SDL_Surface* screen, bool initialize = true);

	/**
	Get the current screen surface

	@return	pointer the screen surface
	*/
	static inline SDL_Surface* GetScreen() {
		return screen;
	}

	/**
	Set a lock on the screen surface (to avoid concurrent drawing operations)

	@return	true - the lock was established successfully
	*/
	inline static bool LockScreen() {
	   return SDL_mutexP(mutexScreen) == 0;
	}
	
	
	/**
	Unlock the screen surface

	@return	true - the unlock operation was successful
	*/
	inline static bool UnlockScreen() {
	   return (SDL_mutexV(mutexScreen) == 0);	   
	}

	/**
	Set the application's background image

	@param	filename	path to a bitmap file
	@param	mode		background mode (BKMODE_TILE | BKMODE_STRETCH | BKMODE_3TILEH | BKMODE_3TILEV | BKMODE_9TILE)
	@return			true - background image was altered successfully
	*/
	bool SetBackground(const std::string& filename, PG_Draw::BkMode mode=PG_Draw::TILE);

	/**
	Set the application's background image

	@param	surface	pointer to a background surface
	@param	mode	background mode (BKMODE_TILE | BKMODE_STRETCH | BKMODE_3TILEH | BKMODE_3TILEV | BKMODE_9TILE)
	@param	freeBackground true if the background surface shall be freed when it is no longer used
	@return		true - background image was altered successfully
	*/
	bool SetBackground(SDL_Surface* surface, PG_Draw::BkMode mode=PG_Draw::TILE, bool freeBackground = false );

	/**
	Redraw the application background

	@param rect the part of the screen to be redrawn
	*/
	static void RedrawBackground(const PG_Rect& rect);

	/**
	Enable / disable the emergency quit key ("ESC")

	@param	esc	true - "ESC" key actiavted
	*/
	void SetEmergencyQuit(bool esc);

	/**
	Tries to find a specifies file

	@param	file	file to look for
	@return		path where the file was found (or NULL if not found)
	*/
	static const std::string& GetRelativePath(const std::string& file);

	/**
	Get the current screen (or window) height

	@return	height in pixels
	*/
	static int GetScreenHeight();

	/**
	Get the current screen (or window) width

	@return	width in pixels
	*/
	static int GetScreenWidth();

	/**
	Do a page flip (only for double buffered screens)
	*/
	static void FlipPage();

	/**
	Outputs some information about the current video target (only with
	DEBUG enabled)
	*/
	void PrintVideoTest();

	/**
	Get the current default widgettheme

	@return	pointer to PG_Theme definition
	*/
	static PG_Theme* GetTheme();

	/**
	Check if the application is currently in bulk mode

	@return	true / false
	Bulkmode means that all widget are always blitted at once.
	*/
	static bool GetBulkMode();

	/**
	Set the bulk mode

	@param bulk whether or not enable bulk mode
	If bulk mode is enabled, all widgets are blitted at once. I.e.
	paragui will not draw the mouse pointer if the mouse is moved. Note
	however, that YOU have to care for the drawing process then. You
	need to call PG_Widget::BulkUpdate() every now and then to redraw
	the screen. See dblbuf.cpp for an example.
	*/
	static void SetBulkMode(bool bulk = true);

	//static bool GetGLMode();

	void EnableBackground(bool enable = true);

	void DeleteBackground();

	/**
	Set AppIdleCalls

	@param enable whether to enable or disable AppIdleCalls
	If you enable AppIdleCalls, the PGApplication object will emit
	a sigAppIdle signal whenever no events are in the queue and sleep
	for a short interval (this is the behaviour inherited from
	PG_MessageObject). However, if you disable it, the object will just
	call SDL_WaitEvent(). Enable this if you know something to do in
	idle periods and don't forget to connect the sigAppIdle signal then.
	*/
	void EnableAppIdleCalls(bool enable = true);

	/**
	Set application`s window-manager icon

	@param filename image file to load
	Set icon for application`s window-manager window. You must use bitmap  
	with sizes dividable by 8 transparent color with palete.
	Transparent color is the color of first up-left pixel.
	THIS FUNCTION MUST BE PROCESSED BEFORE PG_Application::InitScreen()
	*/
	void SetIcon(const std::string& filename);

	/**
	Set application`s window-manager title and icon name.

	@param title title name
	@param icon icon name
	Sets the title-bar and icon name of the display window.
	*/
	void SetCaption(const std::string& title, const std::string& icon = PG_NULLSTR);

	/**
	Get application`s window-manager title and icon name.

	@param title return place for title name pointer
	@param icon return place for icon name pointer
	Set pointers to the window title and icon name.
	*/
	void GetCaption(std::string& title, std::string& icon);

	/**
	Iconify/Minimise the window-manager window

	@return   returns non-zero on success or 0 if iconification is not support or was refused by the window manager.
	If the application is running in a window managed environment Iconify attempts to iconify/minimise it.=20
	*/
	int Iconify(void);

	/**
	Load layout from the XML file
	@param name name of the xml file
	@return   returns non-zero on success or 0 if not succes
	*/
	static bool LoadLayout(const std::string& name);

	/**
	Load layout from the XML file
	@param name name of the xml file
	@param WorkCallback address of the progress callback function
	@return   returns non-zero on success or 0 if not succes
	*/
	static bool LoadLayout(const std::string& name, void (* WorkCallback)(int now, int max));

	/**
	Load layout from the XML file
	@param name name of the xml file
	@param WorkCallback address of the progress callback function
	@param UserSpace address of user data with are returned by Processing instruction etc.
	@return   returns non-zero on success or 0 if not succes
	*/
	static bool LoadLayout(const std::string& name, void (* WorkCallback)(int now, int max), void *UserSpace);

	/**
	Get widget by name
	@param Name name of the widget
	@return   pointer to the requested widget or null if failed
	*/
	static PG_Widget *GetWidgetByName(const std::string& Name);

	template<class T>
	static T* GetWidget(const std::string& Name) {
		return static_cast<T*>(GetWidgetByName(Name));
	}

	template<class T>
	static T* GetWidget(int id) {
		return static_cast<T*>(GetWidgetById(id));
	}

	/**
	Get widget by id.
	@param id  id of the widget
	@return pointer to the requested widget or 0 if failed
	*/
	static PG_Widget *GetWidgetById(int id);

	/**
	Set default font color
	@param	Color PG_Color class contains color information (RGB)
	@return   0 when OK
	*/
	static void SetFontColor(const PG_Color& Color);

	/**
	Set default font transparency (!!!)
	@param	Alpha	Value 0 - 255 (0 = invisible, 255 = fully visible)
	@return   0 when OK
	*/
	static void SetFontAlpha(int Alpha);

	/**
	Set default font style
	@param	Style	Binary combination (OR) of PG_Font::NORMAL, PG_Font::BOLD, PG_Font::ITALIC and PG_Font::UNDERLINE
	@return   0 when OK
	*/
	static void SetFontStyle(PG_Font::Style Style);

	/**
	Set default font size
	@param	Size	size of the font
	@return   0 when OK
	*/
	static void SetFontSize(int Size);

	/**
	Set default font index
	@param	Index	Index of the desired font face in the font file (usually 0)
	@return   0 when OK
	*/
	static void SetFontIndex(int Index);

	/**
	Set default font name
	@param	Name	Filename of the font (this function doesn`t load the font - or check presention of the file !!!)
	@return   0 when OK
	*/
	static void SetFontName(const std::string& Name);

	//! Set a graphical mouse cursor.
	/*!

	  This function makes the cursor use software rendering with
	  the image specified. Note that this can be slow and result
	  in sluggish mouse performance. Note that calling this
	  function doesn't enable software rendering. An additional
	  call to PG_ShowCursor is required.

	  \note The image will receive an additional reference and
	  should be freed by the caller using SDL_FreeSurface when the
	  caller is done using it.
	  
	  \note If image is zero, the hardware cursor will be
	  restored. Note however that the cursor mode will remain at
	  PG_CURSOR_SOFTWARE, so any subsequent calls to this function
	  will reenable the software cursor automatically.

	  \param image the new cursor
	  \sa PG_ShowCursor, PG_DrawCursor
	*/
	static void SetCursor(SDL_Surface *image);

	//! Update the graphical mouse cursor
	/*!
	  This will redraw the graphical curser pointer, if enabled. You
	  might need to call this if you stop the normal event loop from running.
	  @param update true if you want to "Update" the screen content immediately.
	*/
	static void DrawCursor(bool update = true);

	//! Set or query the type of mouse cursor to use.
	/*!
	  This function is used to set or query the type of mouse cursor used.
	  See the documentation for PG_CURSOR_MODE for more details.
	\param mode the new mode for the request
	\return the previous cursor mode 
	*/
	static CursorMode ShowCursor(CursorMode mode);

	//! Disable dirty widget updates
	/*!
	 This function is used to disable dirty widget updates globally.
	 Dirty updates are usually used to speed up blitting. However, it can cause
	 problems with 'semitransparent' widgets.
	\param disable disable ditry updates

	\note All widgets created after this call wont use the dirty update anymore.
	Widgets created before will still use this mode if enabled.
	*/
	static void DisableDirtyUpdates(bool disable);

	//! Check if the 'dirty update' mode is disabled globally
	/*!
	\return true - if the 'dirty update' mode is disabled globally
	*/
	static bool GetDirtyUpdatesDisabled();

	static PG_Application* GetApp();

	static PG_Font* DefaultFont;

	/**
	Get the behaviour concerning empty queues (see EnableAppIdleCalls())

	@return the current behaviour of the application object
	*/
	bool GetAppIdleCallsEnabled();

	//! Flush the event queue
	/*
	This function savely discards all events in the eventqueue.
	All pending messages will be deleted and the memory freed.
	\note Your memory will leak if you just do a SDL_PollEvent
	because this won't delete pending usermessages.
	*/
	static void FlushEventQueue();

	//!Reblit the old mouse position
	/**
	If software cursors are enabled and you move the mouse, the old
	location has to be refreshed somehow with the underlying graphics.
	This is done by this function. Note that bulkmode has to be
	disabled for this function to be called.
	*/
	static void ClearOldMousePosition();

	/**
	Translates numeric keypad keys into other keys in dependency of NUM_LOCK state.
	Should be called in eventKeyDown() for proper numeric keypad behaviour.
	@param key SDL_KeyboardEvent* key to translate
	 */
	static void TranslateNumpadKeys(SDL_KeyboardEvent *key);

	/**
	Sends an event to the global message queue.

	@param event SDL_Event message
	@return true - the message was processed by the framework
	*/
	bool PumpIntoEventQueue(const SDL_Event* event);


	/**
	Registers a new source for obtaining SDL_Event objects from. This source will
	be used in all event loops in Paragui.

	@param eventSupplier the new event source. 
	Paragui will not delete this object. If NULL is passed, Paragui will obtain 
	its events directly from SDL 
	*/
	static void SetEventSupplier( PG_EventSupplier* eventSupplier );


	/**
	Registers a new class for handling the screen updates. This source will
	be used for all screen updates throughout Paragui.

	@param screenUpdater the new updater. 
	Paragui will not delete this object. If NULL is passed, Paragui will obtain 
	its events directly from SDL 
	*/
	static void SetScreenUpdater( PG_ScreenUpdater* screenUpdater ) ;

	/**
	Returns the EventSupplier that's currently active. \see SetEventSupplier
	       
	       @return the active EventSupplier
	*/
	static  PG_EventSupplier* GetEventSupplier();

	/**
	Set the default sibling update mode.
	@param enable true - enable / false - disable
	@param recursive true : the flag will also be set for all childs of the current widget
	If enabled (the default), an Update operation will not only Update the Widget itself, but also all overlapping siblings.
	Disabling this will gain a performance boost, but causes the overlapping parts of siblings to be overdrawn after Update operations.
	All widgets created after setting this option will use the selected update method.
	*/
	static void SetUpdateOverlappingSiblings(bool update = true);

	/**
	Get the default sibling update mode.
	@return default sibling update mode
	*/
	static bool GetUpdateOverlappingSiblings();

   static void SetHighlightingTag( PG_Char c );
   
   static PG_Char GetHighlightingTag();

   static void UpdateRect(SDL_Surface *screen, Sint32 x, Sint32 y, Sint32 w, Sint32 h) 
   {
	my_ScreenUpdater->UpdateRect( screen, x,y,w,h);   	
   }

   static void UpdateRects(SDL_Surface *screen, int numrects, SDL_Rect *rects)
   {
	my_ScreenUpdater->UpdateRects( screen, numrects, rects);   	
   }
   
	SignalXMLTag sigXMLTag;
	SignalAppIdle sigAppIdle;


   class BulkModeActivator {
      bool active;
   public:
      BulkModeActivator() {
         active = PG_Application::GetBulkMode();
         if ( !active )
            PG_Application::SetBulkMode( true );
      };
      void disable() {
         if ( !active )
            PG_Application::SetBulkMode( false );
         active = true;
      }
      ~BulkModeActivator() {
         disable();
      };
   };

   class ScreenLocker {
      bool active;
   public:
      ScreenLocker() {
         active = false;
      };
      ScreenLocker( bool doLock) {
         active = false;
         if ( doLock )
            lock();
      };
      void lock() {
         if ( !active ) {
            LockScreen();
            active = true;
         }
      }
      void unlock() {
         if ( active ) {
            UnlockScreen();
            active = false;
         }
      }
      ~ScreenLocker() {
         unlock();
      };
   };
   

protected:

	/**
	Cleanup the application data
	*/
	void Shutdown();

	/**  */
	bool eventKeyUp(const SDL_KeyboardEvent* key);

	/**  */
	bool eventKeyDown(const SDL_KeyboardEvent* key);

	/**  */
	bool eventQuit(int id, PG_MessageObject* widget, unsigned long data);

	/**  */
	bool eventResize(const SDL_ResizeEvent* event);

	/**  */
	virtual void eventInit();

	/** */
	virtual void eventIdle();

	bool my_quitEventLoop;



private:

	// disable the copy operators
	PG_Application(const PG_Application&);
	PG_Application& operator=(const PG_Application&);

	bool my_freeBackground;
	static SDL_Surface* my_background;
	static SDL_Surface* my_scaled_background;
	static PG_Color my_backcolor;
	static PG_Draw::BkMode my_backmode;

	static PG_Theme* my_Theme;

	//static std::string app_path;

	static PG_Application* pGlobalApp;
	static SDL_Surface* screen;

	static bool bulkMode;
	//static bool glMode;
	bool emergencyQuit;
	static bool enableBackground;
	bool enableAppIdleCalls;
	static PG_EventSupplier* my_eventSupplier;
	static PG_EventSupplier* my_defaultEventSupplier;

	static PG_ScreenUpdater* my_ScreenUpdater;

	static SDL_Surface* my_mouse_pointer;
	static SDL_Surface* my_mouse_backingstore;
	static PG_Rect my_mouse_position;
	static CursorMode my_mouse_mode;
	static SDL_mutex* mutexScreen;
	static bool disableDirtyUpdates;
	static bool defaultUpdateOverlappingSiblings;

   static PG_Char highlightingTag;

public:
   enum ScreenInitialization { None, Trying, Finished };
   static ScreenInitialization isScreenInitialized();
private:
   static ScreenInitialization screenInitialized;
};

/**
	runs the ParaGUI main loop.
	@param argc argument count (from main)
	@param argv array of arguments (from main)
	@param app the PG_Application object
	@return status code.
 
	This function is an encapsulation of app->Run().
	It also handles exceptions (if enabled).
*/
int PG_main(int argc, char **argv, PG_Application *app);

#endif // PG_APPLICATION_H
