
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef textrendererH
#define textrendererH

#include <list>
#include <pgscrollwidget.h>
#include "../global.h"
#include "../ascstring.h"
#include "../paradialog.h"

class TextRenderer : public PG_ScrollWidget {

      struct RenderingAttribute {
         RenderingAttribute() : spaceAfter(0), baseline(0),vspace(0), linebreak(false),firstLineIndent(-1), furtherLineIndent(-1) {};
         int spaceAfter;
         int baseline;
         int vspace;
         bool linebreak;
         int firstLineIndent;
         int furtherLineIndent;
      };
   
      struct TextAttributes {
         TextAttributes() : fontsize(12), textcolor(-1), backgroundcolor(-1) {};
         int fontsize;
         int textcolor;
         int backgroundcolor;
         void assign ( PG_Widget* w );
      };
   
      TextAttributes textAttributes;
      
      typedef std::map<PG_Widget*,RenderingAttribute> Attributes;
      Attributes attributes;
   
   
      typedef list<PG_Widget*> Widgets;
      Widgets widgets;
      PG_Widget* lastWidget;

      ASCString my_text;

      static const int scrollsize = 40;
      
   protected:
   

      bool isSpace( ASCString::charT character )
      {
         return character == ' ' || character == '\n' || character=='\r' || character == '\t';
      }

      bool isBreaker( ASCString::charT character )
      {
         return character == ':' || character == ',' || character=='.' || character == ';' || character == '-';
      }


      int arrangeLine( int y, const Widgets& line, int lineHeight, int indent );
     
      int AreaWidth();
      void layout();
      void addWidget( PG_Widget* w );
      void addSpace( int space );
      void addLinebreak( int pixel, int lines );
      void addIndentation( int firstLine, int furtherLines );
      ASCString substr( const ASCString& text, ASCString::const_iterator begin, ASCString::const_iterator end );
      ASCString::const_iterator token ( const ASCString& text, ASCString::const_iterator start );
      ASCString::const_iterator token_command ( const ASCString& text, ASCString::const_iterator start );
      void parse( const ASCString& text );
      PG_Widget* parsingError( const ASCString& errorMessage );
      
      virtual PG_Widget* render( const ASCString& token );
      virtual PG_Widget* eval_command( const ASCString& token );
      bool eventKeyDown(const SDL_KeyboardEvent* key);
      
   public:
      TextRenderer (PG_Widget *parent, const PG_Rect &r, const std::string& text, const std::string &style="ScrollWidget");
      TextRenderer (PG_Widget *parent, const PG_Rect &r=PG_Rect::null );
      void SetText( const std::string& text );
};


class ViewFormattedText : public ASC_PG_Dialog {
   protected:
      bool eventKeyDown(const SDL_KeyboardEvent* key);
   public:
      ViewFormattedText( const ASCString& title, const ASCString& text, const PG_Rect& pos );
};



#endif
