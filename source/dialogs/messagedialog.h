/*
     This file is part of Advanced Strategic Command; http://www.asc-hq.de
     Copyright (C) 1994-1999  Martin Bickel  and  Marc Schellenberger
 
     This program is free software; you can redistribute it and/or modify
     it under the terms of the GNU General Public License as published by
     the Free Software Foundation; either version 2 of the License, or
     (at your option) any later version.
 
     This program is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU General Public License for more details.
 
     You should have received a copy of the GNU General Public License
     along with this program; see the file COPYING. If not, write to the 
     Free Software Foundation, Inc., 59 Temple Place, Suite 330, 
     Boston, MA  02111-1307  USA
*/


#ifndef messagedialogH
#define messagedialogH

#include <map>

#include "../paradialog.h"
class TextRenderer;

class PG_RichEdit;
class PG_CheckButton;

class  MessageDialog : public ASC_PG_Dialog {
      bool defaultKeysActive;
   public:
   /**
      Creates a PopUp with 2 Buttons
      
      @param parent Parent widget
      @param r rectangle of PopUp
      @param windowtitle Title of window
      @param windowtext Text to appear in window
      @param btn1text Text to appear in Button 1
      @param btn2text Text to appear in Button 2
      @param textalign Alignment for windowtext
      @param style widgetstyle to use (default "MessageBox")
      @param rememberCheckbox true if a "remember choice" checkbox shall be shown
    */
      MessageDialog(PG_Widget* parent, const PG_Rect& r, const std::string& windowtitle, const std::string& windowtext, const std::string& btn1text, const std::string& btn2text, PG_Label::TextAlign textalign = PG_Label::CENTER, const std::string& style="MessageBox", bool rememberCheckbox = false );


   /**
      Creates a PopUp with 1 Button

      @param parent Parent widget
      @param r rectangle of PopUp
      @param windowtitle Title of window
      @param windowtext Text to appear in window
      @param btn1text Text to appear in Button 1
      @param textalign Alignment for windowtext
      @param style widgetstyle to use (default "MessageBox")
    */
      MessageDialog(PG_Widget* parent, const PG_Rect& r, const std::string& windowtitle, const std::string& windowtext, const std::string& btn1text, PG_Label::TextAlign textalign = PG_Label::CENTER, const std::string& style="MessageBox");

   /**
      Creates a PopUp without Buttons

      @param parent Parent widget
      @param r rectangle of PopUp
      @param windowtitle Title of window
      @param windowtext Text to appear in window
      @param textalign Alignment for windowtext
      @param style widgetstyle to use (default "MessageBox")
    */
      MessageDialog(PG_Widget* parent, const PG_Rect& r, const std::string& windowtitle, const std::string& windowtext, PG_Label::TextAlign textalign = PG_Label::CENTER, const std::string& style="MessageBox");
        
      ~MessageDialog();

      void LoadThemeStyle(const std::string& widgettype);

      PG_Widget* getTextBox();

      void EnableDefaultKeys( bool enable );

      void SetText( const std::string& text ) ;
      
      
      bool remberChoice();

   protected:

   /**
      Checks if button is pressed

      @param button pointer to PG_BUtton
    */
      virtual bool handleButton(PG_Button* button);

      PG_Button* my_btnok;
      PG_Button* my_btncancel;

      PG_CheckButton* checkbox;

      bool eventKeyDown (const SDL_KeyboardEvent *key);
        
         
   private:

      TextRenderer* my_textbox;
      int my_msgalign;

      void Init(const std::string& windowtext, int textalign, const std::string& style) ;
};

extern int  new_choice_dlg(const ASCString& title, const ASCString& leftButton, const ASCString& rightButton );
extern int  new_choice_dlg(const ASCString& title, const ASCString& shortTitle, const ASCString& leftButton, const ASCString& rightButton, bool& saveResult );
extern PG_Rect calcMessageBoxSize( const ASCString& message );

#endif

