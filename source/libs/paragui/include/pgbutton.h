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
    Update Date:      $Date: 2009-04-18 13:48:39 $
    Source File:      $Source: /home/martin/asc/v2/svntest/games/asc/source/libs/paragui/include/pgbutton.h,v $
    CVS/RCS Revision: $Revision: 1.3 $
    Status:           $State: Exp $
*/

/** \file pgbutton.h
	Header file for the PG_Button class.
	This include file defines the standard button ID's and the PG_Button class.
*/

#ifndef PG_BUTTON_H
#define PG_BUTTON_H

#include "pgwidget.h"
#include "pgsignals.h"
#include "pgdraw.h"
#include <string>


class PG_ButtonDataInternal;


/**
	@author Alexander Pipelka
 
	@short Button that uses themes.
 
	@image html pgbutton.png "button screenshot"
 
 
	Several predefined button IDs exist. All of them are associated with
	default icons used automatically when the given ID is assigned to a
	button:
 
	@image html button_ok_icon.png "PG_Button::OK"
 
	@image html button_yes_icon.png "PG_Button::YES"
	@image html button_no_icon.png "PG_Button::NO"
	@image html button_apply_icon.png "PG_Button::APPLY"
	@image html button_cancel_icon.png "PG_Button::CANCEL"
	@image html button_close_icon.png "PG_Button::CLOSE"
	@image html button_help_icon.png "PG_Button::HELP"
 
	\anchor theme_PG_Button
 
	<h2>Theme support</h2><p>
 
	<b>widget type:</b>		Button (default)<br>
	<b>object name:</b>		Button<br>
 
	<h3>Parent theme:</h3>
 
	\ref theme_PG_Widget "PG_Widget" theme<br>
 
	<h3>Theme sample:</h3>
 
	\verbatim
	<widget>
		<type value="Button"/>
		<object>
			<name value="Button"/>
			<filename name="background0" value="default/button_unpressed.png"/>
			<filename name="background1" value="default/button_pressed.png"/>
			<filename name="background2" value="default/button_highlighted.png"/>
			<color name="textcolor" value="0x00ff9807"/>
			<property name="backmode0" value="TILE"/>
			<property name="backmode1" value="TILE"/>
			<property name="backmode2" value="TILE"/>
			<gradient name="gradient0"
				color0="0x000000FF"
				color1="0x000000FF"
				color2="0x000000FF"
				color3="0x000000FF"/>
			<gradient name="gradient1"
				color0="0x000000FF"
				color1="0x000000FF"
				color2="0x000000FF"
				color3="0x000000FF"/>
			<gradient name="gradient2"
				color0="0x000000FF"
				color1="0x000000FF"
				color2="0x000000FF"
				color3="0x000000FF"/>
			<property name="blend0" value="0"/>
			<property name="blend1" value="0"/>
			<property name="blend2" value="0"/>
			<property name="bordersize0" value="0"/>
			<property name="bordersize1" value="0"/>
			<property name="bordersize2" value="0"/>
			<property name="transparency0" value="0"/>
			<property name="transparency1" value="0"/>
			<property name="transparency2" value="0"/>
			<filename name="iconup" value="none"/>
			<filename name="icondown" value="none"/>
		</object>
	</widget>
	\endverbatim
	<p>
	<h2>Code:</h2><p>
*/

class DECLSPEC PG_Button : public PG_Widget  {
public:

	/**
	Standard button IDs
	*/
	enum {
	    OK = 0x80000001,
	    YES = 0x80000002,
	    NO = 0x80000003,
	    APPLY = 0x80000004,
	    CANCEL = 0x80000005,
	    CLOSE = 0x80000006,
	    HELP = 0x80000007
	};

	/**
	Button states
	**/
	typedef enum {
	    PRESSED,
	    UNPRESSED,
	    HIGHLITED
	} STATE;

	/**
	Behaviour of the button. The default button captures the messages and fires on mouse button
	release. This can be changed with these flags.
	*/
	typedef enum {
	    MSGCAPTURE      = 0x00000001,
	    SIGNALONCLICK   = 0x00000002,
	    SIGNALONRELEASE = 0x00000004
	} BEHAVIOUR;


	/**
	Signal type declaration
	**/
	template<class datatype = PG_Pointer>
class SignalButtonClick : public PG_Signal1<PG_Button*, datatype> {}
	;

	/**
	Constructor for the PG_Button class
	@param parent	pointer to the parent widget or NULL
	@param r screen position of the button
	@param id id of the button (can be used in callbacks)
	@param text button label
	@param style themestyle of the button
	*/
	PG_Button(PG_Widget* parent, const PG_Rect& r = PG_Rect::null, const std::string& text = PG_NULLSTR, int id = -1, const std::string& style="Button");

	/**  */
	virtual ~PG_Button();

	/**  */
	void LoadThemeStyle(const std::string& widgettype);

	/**  */
	void LoadThemeStyle(const std::string& widgettype, const std::string& objectname);

	/**
		Set the gradient.
		@param state buttonstate to set
		@param gradient gradient to set

		This member function set's the buttons gradient for a specific state.
	*/
	void SetGradient(STATE state, const PG_Gradient& gradient);

	/**
		Set the background.
		@param state buttonstate to set
		@param background pointer to background surface
		@param mode background tiling mode

		This member function set's the buttons background and tiling mode for a specific state.
	*/
	void SetBackground(STATE state, SDL_Surface* background, PG_Draw::BkMode mode = PG_Draw::TILE);

	/**
	Set the color of the border
	@param b	borderindex 0 = upper-left / 1 = lower-right
	@param color	border color (e.g. 0xRRGGBB)
	*/
	void SetBorderColor(int b, const PG_Color& color);

	/**
	Set icons for the button
	@param filenameup icon for unpressed state
	@param filenamedown icon for pressed state
	@param filenameover icon for highlited state
	@return true on success
	*/
	bool SetIcon(const std::string& filenameup=PG_NULLSTR, const std::string& filenamedown=PG_NULLSTR, const std::string& filenameover = PG_NULLSTR);

	/**
	Set icons for the button
	@param filenameup icon for unpressed state
	@param filenamedown icon for pressed state
	@param filenameover icon for highlited state
	@param colorkey the colorkey assigned to the icons
	@return true on success
	*/
	bool SetIcon(const std::string& filenameup, const std::string& filenamedown, const std::string& filenameover, const PG_Color& colorkey);

	/**
	Set icons for the buttons
	@param icon_up the icon surface for the unpressed state
	@param icon_down the icon for the pressed state
	@param icon_over the icon for the highlited state
   @param freeSurface if true, the surfaces are automatically freed on destruction or when setting another icon.
                      If false, the user has to care himself for freeing the surfaces
	@return true on success

	*/
	bool SetIcon(SDL_Surface* icon_up, SDL_Surface* icon_down = NULL, SDL_Surface* icon_over = NULL, bool freeSurfaces = false );

	/**
	Returns an image for a given button state
	@param num	(NORM = icon for unpressed state | PRESSED = icon for pressed state | HIGH = icon for highlighted state)
	@return a pointer to an SDL_Surface for the given icon
	*/
	SDL_Surface* GetIcon(STATE num);

	/**
	Set the distance between the left border of the button and the icon
	@param indent spacing between widget border and icon
	*/
	void SetIconIndent(Uint16 indent);

	/**
	Set the bordersize of the button
	@param norm	bordersize for unpressed state
	@param pressed	bordersize for pressed state
	@param high	bordersize for highlighted state
	@return a pointer to an SDL_Surface for the given icon

	If you don't want to set the bordersize for one of the states, set it to -1.
	*/
	void SetBorderSize(int norm, int pressed, int high);

	/**
	Make the button a toggle button.
	A normal (push) button will return to an unpressed state when the user
	stops clicking it (releases the mouse button).
	A toggle button will stay pressed once it is pressed. The user
	must click the button a second time in order to unpress it.
	*/
	void SetToggle(bool bToggle);

	/**
	If the button is a toggle button you can modify the status of the button
	with this function
	*/
	void SetPressed(bool pressed);

	/**
	Set the transparency of the button
	@param t the transparency you want to assign
	@param bRecursive if set to true, apply the transparency to all child widgets
	*/
	void SetTransparency(Uint8 t, bool bRecursive = false);

	/**
	Set the transparency for the single button states
	@param norm the transparency of the normal (unpressed) state
	@param pressed the transparency of the pressed state
	@param high the transparency of the highlited state
	*/
	void SetTransparency(Uint8 norm, Uint8 pressed, Uint8 high);

	/**
	Set the moving distance of the image when we press on it
	@param pixelshift	int number of pixels to shift
	 */
	void SetShift(int pixelshift);

	/**
	Determine whether a given button is pressed. This can either mean
	that the user is clicking the button in the case of a push button, 
	or that the button is toggled in the case of a toggle button.
	@return bool is the button pressed
	*/
	bool GetPressed();

	/**
	Set the blend-level of gradient and background image
	@param blend blend-level
	@param mode one of BTN_STATE_NORMAL, BTN_STATE_PRESSED, BTN_STATE_HIGH
	This function sets the blend level of gradient and background image.

	If the blend-level is 0 only the background image is visible.
	At a level of 255 only the gradient is visible.
	*/
	void SetBlendLevel(STATE mode, Uint8 blend);

	/**
	Get the current blend level.
	@param mode button mode
	@return the current blend level
	*/
	Uint8 GetBlendLevel(STATE mode);

	/**
	Resizes the button so that a specified text fits on it
	@param Width additional width apart from the width required by the text
	@param Height additional height apart from the one required by the text
	@param Text the text which is to fit on the button
	*/
	void SetSizeByText(int Width = 0, int Height = 0, const std::string& Text = PG_NULLSTR);

	/**
	Set the behaviour of the button. 
	@param behaviour bitmapped field, see #BUTTONMODE
	*/
	void SetBehaviour( int behaviour );

   void SetText(const std::string& text);

   
	SignalButtonClick<> sigClick;

	static SignalButtonClick<> sigGlobalClick;

protected:

	/**  */
	virtual void eventButtonSurface(SDL_Surface** surface, STATE newstate, Uint16 w, Uint16 h);

	/**  */
	void eventBlit(SDL_Surface* surface, const PG_Rect& src, const PG_Rect& dst);

	/**  */
	void eventSizeWidget(Uint16 w, Uint16 h);

	/**  */
	void eventMouseLeave();

	/**  */
	void eventMouseEnter();

	/**  */
	bool eventMouseButtonUp(const SDL_MouseButtonEvent* button);

	/**  */
	bool eventMouseButtonDown(const SDL_MouseButtonEvent* button);

   /**  */
   bool eventKeyDown (const SDL_KeyboardEvent *key);
   
private:

	PG_Button(const PG_Button&);
	PG_Button& operator=(const PG_Button&);

	/**  */
	DLLLOCAL void FreeSurfaces();

	/**  */
	DLLLOCAL void FreeIcons();

	PG_ButtonDataInternal* _mid;
};

#endif // PG_BUTTON_H
