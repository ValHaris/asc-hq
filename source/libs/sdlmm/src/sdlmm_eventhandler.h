/*
 * SDLmm - a C++ wrapper for SDL and related libraries
 * Copyright © 2001 David Hedbor <david@hedbor.org>
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#ifndef SDLMM_EVENTHANDLER_H
#define SDLMM_EVENTHANDLER_H

namespace SDLmm {
  //! The base class used for custom Event handlers.
  /*!
    The event handling in SDLmm is rather different to that of SDL in 
    that it uses classes derived from the EventHandler. This allows
    for clean, type-safe code much closer to the spirit of C++ than the use
    of a union with a type field. To handle one or more events, you simply
    build a class derived from this, reimplementing the only functions for
    the events you need to handle.
    \sa Event::HandleEvents()
  */
  class  EventHandler {
  public:
    virtual ~EventHandler() { }
  
    //! \name Keyboard Events
    //@{
    //! Keyboard event callback.
    /*!
      This callback is used when an event of type SDL_KEYDOWN or SDL_KEYUP is
      reported. These occur when keys are pressed or released.
      
      \note Repeating SDL_KEYDOWN events will occur if key repeat is
      enabled (see EnableKeyRepeat()).
      \note This function is only called if the default HandleKeyPressEvent()
      and / or HandleKeyReleaseEvent() methods are used. 
      \param keysym information about the pressed key
      \param pressed true for SDL_KEYDOWN, false for SDL_KEYUP

      \return true if the event was handled, false if it was not. If
      the event wasn't handled, it will be handed over to the generic
      HandleEvent() method.
    */
    virtual bool HandleKeyboardEvent(const SDL_keysym &keysym, bool pressed) {
      return false;
    }

    //! Keyboard press event callback.
    /*!
      This callback is used when an event of type SDL_KEYDOWN is
      reported. A keyboard press event occurs when a key is pressed.  
      
      \note Repeating SDL_KEYDOWN events will occur if key repeat is
      enabled (see EnableKeyRepeat()).
      \note The default declaration of this method calls the
      HandleKeyboardEvent() method. If you want to handle both key
      presses and key releases in the same method, just redefine
      HandleKeyboardEvent(). 
      \param keysym information about the pressed key

      \return true if the event was handled, false if it was not. If
      the event wasn't handled, it will be handed over to the generic
      HandleEvent() method.
    */
    virtual bool HandleKeyPressEvent(const SDL_keysym &keysym) {
      return HandleKeyboardEvent(keysym, true);
    }
    
    //! Keyboard release event callback.
    /*!
      This callback is used when an event of type SDL_KEYUP is
      reported. A keyboard press event occurs when a key is released.
      
      \param keysym the information about the released key
      \note The default declaration of this method calls the
      HandleKeyboardEvent() method. If you want to handle both key
      presses and releases in the same method, just redefine
      HandleKeyboardEvent(). 

      \return true if the event was handled, false if it was not. If
      the event wasn't handled, it will be handed over to the generic
      HandleEvent() method.
    */
    virtual bool HandleKeyReleaseEvent(const SDL_keysym &keysym) {
      return HandleKeyboardEvent(keysym, false);
    }
    //@}

    //! \name Mouse Events
    //@{
    //! Mouse motion event callback.
    /*!
      This is used when an event of type SDL_MOUSEMOTION is reported.

      Simply put, a SDL_MOUSEMOTION type event occurs when a user
      moves the mouse within the application window or when
      SDL_WarpMouse is called. Both the absolute (\a x and \a y) and
      relative (\a xrel and \a yrel) coordinates are reported along with the
      current button states (\a state). The button \a state can be
      interpreted using the SDL_BUTTON macro (see GetMouseState()). 

      If the cursor is hidden (Display::HideCursor()) and the input is
      grabbed (Display::GrabInput(SDL_GRAB_ON)) then the mouse will
      give relative motion events even when the cursor reaches the
      edge of the screen. This is currently only implemented on
      Windows and Linux/Unix-a-likes.

      \param state the current button state.
      \param x, y the absolute x/y coordinates of the mouse pointer
      \param xrel, yrel relative motion in the x/y directions

      \return true if the event was handled, false if it was not. If
      the event wasn't handled, it will be handed over to the generic
      HandleEvent() method.
    */
    virtual bool HandleMouseMotionEvent(Uint8 state, Uint16 x, Uint16 y,
					Sint16 xrel, Sint16 yrel) {
      return false;
    } 

    //! Mouse button event callback.
    /*!
      This is used when an event of type SDL_MOUSEBUTTONDOWN or
      SDL_MOUSEBUTTONUP is reported.

      When a mouse button press is detected the number of the button
      pressed (from 1 to 255, with 1 usually being the left button and
      2 the right) is placed into \a button. The position of the mouse
      when this event occured is stored in \a x and \a y.
      
      \note This function is only called if the default
      HandleMouseButtonPressEvent() and / or
      HandleMouseButtonReleaseEvent() methods are used.

      \param button the mouse button index
      \param x, y the x/y coordinates of the mouse at press time
      \param pressed true for SDL_MOUSEBUTTONDOWN, false for
      SDL_MOUSEBUTTONUP.

      \return true if the event was handled, false if it was not. If
      the event wasn't handled, it will be handed over to the generic
      HandleEvent() method.
    */      
    virtual bool HandleMouseButtonEvent(Uint8 button, Uint16 x, Uint16 y,
					bool pressed) {
      return false;
    }

    //! Mouse button press event callback.
    /*!
      This is used when an event of type SDL_MOUSEBUTTONDOWN is reported.

      When a mouse button press is detected the number of the button
      pressed (from 1 to 255, with 1 usually being the left button and
      2 the right) is placed into \a button. The position of the mouse
      when this event occured is stored in \a x and \a y.

      \note The default declaration of this method calls the
      HandleMouseButtonEvent() method. If you want to handle both
      button presses and releases in the same method, just redefine
      HandleMouseButtonEvent(). 

      \param button the mouse button index
      \param x, y the x/y coordinates of the mouse at press time

      \return true if the event was handled, false if it was not. If
      the event wasn't handled, it will be handed over to the generic
      HandleEvent() method.
    */      
    virtual bool HandleMouseButtonPressEvent(Uint8 button,
					     Uint16 x, Uint16 y) {
      return HandleMouseButtonEvent(button, x, y, true);
    }
    
    //! Mouse button release event callback.
    /*!
      This is used when an event of type SDL_MOUSEBUTTONUP is reported.

      When a mouse button release is detected the number of the button
      release (from 1 to 255, with 1 usually being the left button and
      2 the right) is placed into \a button. The position of the mouse
      when this event occured is stored in \a x and \a y.

      \note The default declaration of this method calls the
      HandleMouseButtonEvent() method. If you want to handle both
      button presses and releases in the same method, just redefine
      HandleMouseButtonEvent(). 

      \param button the mouse button index
      \param x, y the x/y coordinates of the mouse at press time

      \return true if the event was handled, false if it was not. If
      the event wasn't handled, it will be handed over to the generic
      HandleEvent() method.
    */      
    virtual bool HandleMouseButtonReleaseEvent(Uint8 button,
					       Uint16 x, Uint16 y) {
      return HandleMouseButtonEvent(button, x, y, false);
    }
    //@}

    //! \name Joystick Events
    //@{
    //! Joystick axis motion event callback.
    /*!
      This is used when an event of type SDL_JOYAXISMOTION is reported.

      A SDL_JOYAXISMOTION event occurs when ever a user moves an axis
      on the joystick.
      \param joystick the index of the joystick that reported the event
      \param axis the index of the axis (for a more detailed explaination
      see the Joystick section of the SDL  manual). 
      \params value is the current position of the axis.

      \return true if the event was handled, false if it was not. If
      the event wasn't handled, it will be handed over to the generic
      HandleEvent() method.
    */
    virtual bool HandleJoyAxisEvent(Uint8 joystick, Uint8 axis,
				    Sint16 value) {
      return false;
    }
    
    //! Joystick button event callback.
    /*!
      This is used when an event of type SDL_JOYBUTTONDOWN or
      SDL_JOYBUTTONUP is reported. 

      These events occurs when ever a user presses or releases a
      button on a joystick.

      \note This function is only called if the default
      HandleJoyButtonPressEvent() and / or
      HandleJoyButtonReleaseEvent() methods are used.

      \param joystick the index of the joystick that reported the event
      \param button the index of the pressed button (for a more
      detailed explaination see the Joystick section of the SDL
      manual).
      \param pressed true for SDL_JOYBUTTONDOWN, false for SDL_JOYBUTTONUP.

      \return true if the event was handled, false if it was not. If
      the event wasn't handled, it will be handed over to the generic
      HandleEvent() method.
    */
    virtual bool HandleJoyButtonEvent(Uint8 joystick, Uint8 button,
				      bool pressed) {
      return false;
    } 

    //! Joystick button press event callback.
    /*!
      This is used when an event of type SDL_JOYBUTTONDOWN is reported. 

      A SDL_JOYBUTTONDOWN event occurs when ever a user presses a
      button on a joystick.

      \note The default declaration of this method calls the
      HandleJoyButtonEvent() method. If you want to handle both
      button presses and releases in the same method, just redefine
      HandleJoyButtonEvent(). 

      \param joystick the index of the joystick that reported the event
      \param button the index of the pressed button (for a more
      detailed explaination see the Joystick section of the SDL
      manual).

      \return true if the event was handled, false if it was not. If
      the event wasn't handled, it will be handed over to the generic
      HandleEvent() method.
    */
    virtual bool HandleJoyButtonPressEvent(Uint8 joystick, Uint8 button) {
      return HandleJoyButtonEvent(joystick, button, true);
    } 

    //! Joystick button release event callback.
    /*!
      This is used when an event of type SDL_JOYBUTTONUP is reported. 

      A SDL_JOYBUTTONUP event occurs when ever a user releases a
      button on a joystick.

      \note The default declaration of this method calls the
      HandleJoyButtonEvent() method. If you want to handle both
      button presses and releases in the same method, just redefine
      HandleJoyButtonEvent(). 

      \param joystick the index of the joystick that reported the event
      \param button the index of the released button (for a more
      detailed explaination see the Joystick section of the SDL
      manual).

      \return true if the event was handled, false if it was not. If
      the event wasn't handled, it will be handed over to the generic
      HandleEvent() method.
    */
    virtual bool HandleJoyButtonReleaseEvent(Uint8 joystick, Uint8 button) {
      return HandleJoyButtonEvent(joystick, button, false);
    } 

    //! Joystick hat position change event callback.
    /*!
      This is used when an event of type SDL_JOYHATMOTION is reported.
      
      A SDL_JOYHATMOTION event occurs when ever a user moves a hat on
      the joystick. The field \a joystick is the index of the joystick
      that reported the event and \a hat is the index of the hat (for
      a more detailed exlaination see the Joystick section in the SDL
      documentation). \a value is the current position of the hat. It is
      a logically OR'd combination of the following values (whose
      meanings should be pretty obvious):
      - SDL_HAT_CENTERED
      - SDL_HAT_UP
      - SDL_HAT_RIGHT
      - SDL_HAT_DOWN
      - SDL_HAT_LEFT
      
      The following defines are also provided:

      - SDL_HAT_RIGHTUP
      - SDL_HAT_RIGHTDOWN
      - SDL_HAT_LEFTUP
      - SDL_HAT_LEFTDOWN

      \param joystick joystick device index
      \param hat joystick hat index
      \param value hat position

      \return true if the event was handled, false if it was not. If
      the event wasn't handled, it will be handed over to the generic
      HandleEvent() method.
     */
    virtual bool HandleJoyHatEvent(Uint8 joystick, Uint8 hat, Uint8 value) {
      return false;
    } 

    //! Joystick trackball motion event callback.
    /*!
      Thyis is used when an event of type SDL_JOYBALLMOTION is reported.

      A SDL_JOYBALLMOTION event occurs when a user moves a trackball
      on the joystick. The field \a joystick is the index of the
      joystick that reported the event and \a ball is the index of the
      trackball (for a more detailed explaination see the Joystick
      section in the SDL documentation). Trackballs only return
      relative motion, this is the change in position on the ball
      since it was last polled (last cycle of the event loop) and it
      is stored in \a xrel and \a yrel.

      \param joystick joystick device index
      \param ball joystick trackbal index
      \param xrel, yrel relative motion in the x/y directions

      \return true if the event was handled, false if it was not. If
      the event wasn't handled, it will be handed over to the generic
      HandleEvent() method.
    */
    virtual bool HandleJoyBallEvent(Uint8 joystick, Uint8 ball,
				    Sint16 xrel, Sint16 yrel) {
      return false;
    } 

    //@}

    //! \name Window / Display Events
    //@{ 
    //! Application visibility event callback.
    /*!
      This callback is called when an event of type SDL_ACTIVEEVENT is
      reported.  

      When the mouse leaves or enters the window area a
      SDL_APPMOUSEFOCUS type activation event occurs, if the mouse
      entered the window then \a gain will be true, otherwise \a gain
      will be false. A SDL_APPINPUTFOCUS type activation event occurs
      when the application loses or gains keyboard focus. This usually
      occurs when another application is made active. Finally, a
      SDL_APPACTIVE \a type event occurs when the application is
      either minimised/iconified (\a gain=false) or restored.

      \param gain false if the event is a loss or true if it's a gain
      \param state SDL_APPMOUSEFOCUS if mouse focus was gained or
      lost, SDL_APPINPUTFOCUS if input focus was gained or lost, or
      SDL_APPACTIVE if the application was iconified (gain=false) or
      restored (gain=true)  

      \return true if the event was handled, false if it was not. If
      the event wasn't handled, it will be handed over to the generic
      HandleEvent() method.
    */
    virtual bool HandleActiveEvent(bool gain, Uint8 state) {
      return false;
    } 

    //! Window resize event callback.
    /*!
      This is used when an event of type SDL_VIDEORESIZE is reported.

      When SDL_RESIZABLE is passed as a flag to
      Display::SetVideoMode() the user is allowed to resize the
      applications window. When the window is resized an
      SDL_VIDEORESIZE is reported, with the new window width and
      height values stored in \a w and \a h, respectively. When an
      SDL_VIDEORESIZE is recieved the window should be resized to the
      new dimensions using Display::SetVideoMode();
      
      \param w, h new width and height of the window.

      \return true if the event was handled, false if it was not. If
      the event wasn't handled, it will be handed over to the generic
      HandleEvent() method.
    */
    virtual bool HandleResizeEvent(int w, int h) {
      return false;
    } 

    //! Platform-dependent window manager event callback.
    /*!
      The system window manager event contains a pointer to
      system-specific information about unknown window manager
      events. If you enable this event using Event::EventState(), it will
      be generated whenever unhandled events are received from the
      window manager. This can be used, for example, to implement
      cut-and-paste in your application.  

      If you want to obtain system-specific information about the
      window manager, you can fill the version member of a
      SDL_SysWMinfo structure (details can be found in SDL_syswm.h,
      which must be included) using the SDL_VERSION() macro found in
      SDL_version.h, and pass it to the function:

      \code
      int SDL_GetWMInfo(SDL_SysWMinfo *info);
      \endcode

      \return true if the event was handled, false if it was not. If
      the event wasn't handled, it will be handed over to the generic
      HandleEvent() method.
    */
    virtual bool HandleSysWMEvent() {
      return false;
    } 

    //! Callback for user-defined events.
    /*!
      This event is unique - it is never created by SDL but only by
      the user. The event can be pushed onto the event queue using
      Event::Push(). The contents of \a data1 and \a data2 are
      completely up to the programmer. The only requirement is that
      \a type is a value from SDL_USEREVENT to SDL_NUMEVENTS-1
      (inclusive).
      
      \param type SDL_USEREVENT through to SDL_NUMEVENTS-1
      \param code user defined event code
      \param data1, data2 user defined data pointers

      \return true if the event was handled, false if it was not. If
      the event wasn't handled, it will be handed over to the generic
      HandleEvent() method.
    */
    //@}

    //! \name Other Events
    //@{ 
    virtual bool HandleUserEvent(Uint8 type, int code,
				 void *data1, void *data2) {
      return false;
    } 

    //! Quit requested event callback.
    /*!
      This is used whan an event of type SDL_QUIT is reported.

      The SDL_QUIT event is very important. If you filter out or
      ignore a quit event it is impossible for the user to close the
      window. On the other hand, if you do accept a quit event, the
      application window will be closed, and screen updates will still
      report success, even though the application will no longer be
      visible.
      
      \note The macro SDL_QuitRequested will return non-zero if a
      quit event is pending.

      \return true if the event was handled, false if it was not. If
      the event wasn't handled, it will be handed over to the generic
      HandleEvent() method.
    */
    virtual bool HandleQuitEvent() {
      return false;
    }
    //@}

    //! Catch-all event handler
    /*!
      This method is called if none of the event specific callbacks
      handled event (i.e they returned false).

      \note It's both easier and more efficient to use the
      event-specific callbacks, so unless you really want to do all
      the work yourself, you want to use those.

      \param event the current event

      \return true if the event was handled, false if it was not
    */
    virtual bool HandleEvent(const SDL_Event &event) {
      return false;
    }

  };
}

#endif // SDLMM_EVENTHANDLER_H
