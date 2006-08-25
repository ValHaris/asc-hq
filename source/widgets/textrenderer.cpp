/***************************************************************************
                          paradialog.cpp  -  description
                             -------------------
    begin                : Thu Feb 21 2002
    copyright            : (C) 2002 by Martin Bickel
    email                : bickel@asc-hq.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/



#include <pglabel.h>
#include <pgimage.h>
#include <boost/regex.hpp>

#include "../global.h"

#include "textrenderer.h"
#include "../graphics/surface.h"
#include "../iconrepository.h"
#include "../palette.h"
#include "../dialogs/fileselector.h"

void TextRenderer :: TextAttributes :: assign ( PG_Widget* w )
{

   static PG_ThemeWidget* theme = NULL;
   if ( !theme )
      theme = new PG_ThemeWidget( NULL );

   w->SetFontSize( fontsize );
   if ( textcolor > 0 )
      w->SetFontColor ( textcolor );
   else
      w->SetFontColor ( theme->GetFontColor() );
}

TextRenderer :: TextRenderer (PG_Widget *parent, const PG_Rect &r ) : PG_ScrollWidget( parent, r, "ScrollWidget" ), lastWidget(NULL)
{
   SetTransparency(255);
   SetLineSize( scrollsize );
};

TextRenderer :: TextRenderer (PG_Widget *parent, const PG_Rect &r, const std::string& text, const std::string &style) : PG_ScrollWidget( parent, r, style ), lastWidget(NULL)
{
   SetTransparency(255);
   SetText( text );
};


int TextRenderer :: arrangeLine( int y, const Widgets& line, int lineHeight, int indent )
{
   int x = indent;
   for ( Widgets::const_iterator i = line.begin(); i != line.end(); ++i ) {
      (*i)->MoveWidget( x, y + lineHeight- (*i)->Height(), false );
      x += (*i)->Width();
      if ( attributes.find(*i ) != attributes.end() )
         x += attributes[*i].spaceAfter;
   }
   return x;
}

int TextRenderer :: AreaWidth()
{
   return max( int(GetListWidth()), Width() - 18 );
}

void TextRenderer :: layout()
{
   int maxx = 0;
   int x = 0;
   int y = 0;
   int lineHeight = 0;
   Widgets currentLine;

   bool breakNow = false;

   int firstLineIndent = 0;
   int furtherLineIndent = 0;
   int indentation = 0;
   int vspace = 0;

   for ( Widgets::iterator i = widgets.begin(); i != widgets.end(); ++i ) {
      if ( (x + (*i)->Width() >= AreaWidth()-2 && x > 0) || breakNow ) {
         maxx = max( arrangeLine( y, currentLine, lineHeight, indentation ), maxx);
         
         y += lineHeight + vspace;

         if ( breakNow )
            indentation = firstLineIndent;
         else
            indentation = furtherLineIndent;

         x = indentation;
         
         lineHeight = 0;
         currentLine.clear();
         breakNow = false;
         vspace = 0;
      }

      currentLine.push_back ( *i );

      if ( lineHeight < (*i)->Height() )
         lineHeight = (*i)->Height();

      x += (*i)->Width();
      Attributes::iterator at = attributes.find(*i );
      if ( at != attributes.end()) {
         x += at->second.spaceAfter;
         if ( at->second.linebreak )
            breakNow = true;
         
         if ( at->second.vspace )
            vspace  = at->second.vspace;

         if ( at->second.firstLineIndent >= 0 )
            firstLineIndent = at->second.firstLineIndent;

         if ( at->second.furtherLineIndent >= 0 )
            furtherLineIndent = at->second.furtherLineIndent;
      }

   }
   maxx = max(arrangeLine( y, currentLine, lineHeight, indentation ), maxx );

   //! this is a hack to get the scrollbars updated
   new PG_Widget( this, PG_Rect( maxx, y + lineHeight, 1, 1 ));
}

void TextRenderer :: addWidget( PG_Widget* w )
{
   if ( w ) {
      widgets.push_back( w );
      lastWidget = w;
   }
}

void TextRenderer :: addSpace( int space )
{
   if ( lastWidget )
      attributes[lastWidget].spaceAfter += space * 5;
}

void TextRenderer :: addLinebreak( int pixel, int lines )
{
   if ( lastWidget ) {
      if ( attributes[lastWidget].linebreak ) {
         attributes[lastWidget].vspace += pixel + lines * (textAttributes.fontsize+3);
      } else {
         attributes[lastWidget].linebreak = true;
         attributes[lastWidget].vspace += pixel + (lines-1) * (textAttributes.fontsize+3);
      }
   }
}

void TextRenderer :: addIndentation( int firstLine, int furtherLines )
{
   if ( !lastWidget ) 
      addWidget( new PG_Widget( this, PG_Rect(0,0,0,1)));
   
   if ( firstLine >= 0 )
      attributes[lastWidget].firstLineIndent = firstLine;

   if ( furtherLines >= 0 )
      attributes[lastWidget].furtherLineIndent = furtherLines;

}


ASCString TextRenderer :: substr( const ASCString& text, ASCString::const_iterator begin, ASCString::const_iterator end )
{
   return text.substr( begin-text.begin(), end-begin+1);
}

ASCString::const_iterator TextRenderer :: token_command ( const ASCString& text, ASCString::const_iterator start )
{
   ASCString::const_iterator end = start;
   while( end+1 != text.end() && (*end != '#' || end == start ) && !isSpace(*end))
      ++end;

   addWidget( eval_command( substr( text, start, end )));

   return end+1;
}


ASCString::const_iterator TextRenderer :: token ( const ASCString& text, ASCString::const_iterator start )
{
   ASCString::const_iterator end = start;

   while( end+1 != text.end() && !isBreaker(*end) && !isSpace(*end) && *end != '#' )
      ++end;

   if ( isSpace( *end )) {
      addWidget( render ( substr( text, start, end-1)));

      int hspace = 0;
      int vspace = 0;

      while ( end != text.end() && isSpace( *end)) {
         if ( *end == ' ' )
            hspace += 1;
         else
            if ( *end == '\n' ) {
               hspace = 0;
               vspace += 1;
            }

         ++end;
      }
      if ( hspace )
         addSpace( hspace );

      if ( vspace )
         addLinebreak( 0, vspace );

      return end;
   } else {
      if ( *end == '#' ) {
         addWidget( render( substr( text, start, end-1)));
         return end;
      } else {
         addWidget( render( substr( text, start, end)));
         return end+1;
      }
   }
}

void TextRenderer :: parse( const ASCString& text )
{
   textAttributes.fontsize = GetFontSize();
   textAttributes.textcolor = GetFontColor();
   
   ASCString::const_iterator pos = text.begin();

   // skip spaces at beginning to text
   while ( pos != text.end() && isSpace(*pos) )
      ++pos;

   if ( pos == text.end() )
      return;

   while ( pos != text.end() )
      if ( *pos == '#' )
         pos = token_command ( text, pos );
      else
         pos = token ( text, pos );

}

PG_Widget* TextRenderer :: parsingError( const ASCString& errorMessage )
{
   PG_Widget* w = new PG_Label( this );
   textAttributes.assign( w );
   w->SetText( errorMessage );
   w->SetSizeByText();
   w->SetFontColor( 0xff0000 );
   return w;
}

PG_Widget* TextRenderer :: eval_command( const ASCString& token )
{
   assert ( token[0] == '#' );
   boost::smatch what;

   static boost::regex size( "#fontsize=(\\d+)#");
   if( boost::regex_match( token, what, size)) {
      ASCString s;
      s.assign( what[1].first, what[1].second );
      textAttributes.fontsize = strtol(s.c_str(), NULL, 0 );
      return NULL;
   }

   static boost::regex image( "#image=(\\S+)#");
   if( boost::regex_match( token, what, image)) {
      ASCString s;
      s.assign( what[1].first, what[1].second );
      Surface& surf = IconRepository::getIcon(s);
      PG_Widget* w = new PG_Image( this, PG_Point(0,0), surf.getBaseSurface(), false );
      return w;
   }

   static boost::regex color( "#fontcolor=((0x[a-fA-F\\d]+)|\\d+)#");
   if( boost::regex_match( token, what, color)) {
      ASCString s;
      s.assign( what[1].first, what[1].second );
      textAttributes.textcolor = strtol(s.c_str(), NULL, 0 );
      return NULL;
   }

   static boost::regex defcolor( "#fontcolor=default#");
   if( boost::regex_match( token, what, defcolor)) {
      textAttributes.textcolor = GetFontColor();
      return NULL;
   }

   static boost::regex legacycolor( "#color(\\d+)#");
   if( boost::regex_match( token, what, legacycolor)) {
      ASCString s;
      s.assign( what[1].first, what[1].second );
      int index = strtol(s.c_str(), NULL, 0 );
      if ( index > 0 )
         textAttributes.textcolor = (pal[index][0] << 18) + (pal[index][1] << 10) + (pal[index][2] << 2);
      else
         textAttributes.textcolor = GetFontColor();
      return NULL;
   }

   static boost::regex crtp( "#crtp=?(\\d+)#");
   if( boost::regex_match( token, what, crtp)) {
      ASCString s;
      s.assign( what[1].first, what[1].second );
      addLinebreak( strtol(s.c_str(), NULL, 0 ), 0);
      return NULL;
   }

   static boost::regex crt( "#crt#");
   if( boost::regex_match( token, what, crt)) {
      addLinebreak( 0, 1 );
      return NULL;
   }

   static boost::regex firstindent( "#eeinzug(\\d+)#");
   if( boost::regex_match( token, what, firstindent)) {
      ASCString s;
      s.assign( what[1].first, what[1].second );
      addIndentation( strtol(s.c_str(), NULL, 0 ), -1 );
      return NULL;
   }

   static boost::regex furtherindent( "#aeinzug(\\d+)#");
   if( boost::regex_match( token, what, furtherindent)) {
      ASCString s;
      s.assign( what[1].first, what[1].second );
      addIndentation( -1, strtol(s.c_str(), NULL, 0 ) );
      return NULL;
   }

   static boost::regex indent( "#indent=(\\d+)\\,(\\d+)#");
   if( boost::regex_match( token, what, indent)) {
      ASCString s1;
      s1.assign( what[1].first, what[1].second );
      ASCString s2;
      s2.assign( what[2].first, what[2].second );
      addIndentation( strtol(s1.c_str(), NULL, 0 ), strtol(s2.c_str(), NULL, 0 ) );
      return NULL;
   }

   static boost::regex legacyfont1( "#font0*[1|0]#");
   if( boost::regex_match( token, what, legacyfont1)) {
      textAttributes.fontsize = 12;
      return NULL;
   }

   static boost::regex legacyfont2( "#font0*2#");
   if( boost::regex_match( token, what, legacyfont2)) {
      textAttributes.fontsize = 20;
      return NULL;
   }

   return parsingError ( "unknown token: " + token );
}

      
PG_Widget* TextRenderer :: render( const ASCString& token )
{     
   PG_Widget* w = new PG_Label( this );
   textAttributes.assign( w );
   w->SetText( token );
   w->SetSizeByText();
   w->SizeWidget( w->Width(), textAttributes.fontsize*4/3, false );
   return w;
};


void TextRenderer::SetText( const string& text )
{
   parse(text);
   layout();
   my_text = text;
}

bool TextRenderer :: eventKeyDown(const SDL_KeyboardEvent* key)
{
   int mod = SDL_GetModState() & ~(KMOD_NUM | KMOD_CAPS | KMOD_MODE);
   if ( (mod & KMOD_CTRL) &&( key->keysym.sym == SDLK_s )) {
      ASCString name = selectFile( "*.txt", false );
      if ( !name.empty() ) {
         tn_file_buf_stream s( name, tnstream::writing );
         s.writeString( my_text, true );
      }
      
      return true;
   }

   int keyStateNum;
   Uint8* keyStates = SDL_GetKeyState ( &keyStateNum );

   if ( (key->keysym.sym == SDLK_UP  && keyStates[SDLK_UP] ) || ( key->keysym.sym == SDLK_KP8  && keyStates[SDLK_KP8] )) {
      ScrollTo( GetScrollPosX (), GetScrollPosY () - scrollsize );
      return true;
   }
   if ( (key->keysym.sym == SDLK_DOWN  && keyStates[SDLK_DOWN]) || (key->keysym.sym == SDLK_KP2  && keyStates[SDLK_KP2] )) {
      ScrollTo( GetScrollPosX (), GetScrollPosY () + scrollsize );
      return true;
   }

   if ( key->keysym.sym == SDLK_PAGEUP  && keyStates[SDLK_PAGEUP] ) {
      ScrollTo( GetScrollPosX (), GetScrollPosY() - (Height() - 10) );
      return true;
   }
   if ( key->keysym.sym == SDLK_PAGEDOWN  && keyStates[SDLK_PAGEDOWN] ) {
      ScrollTo( GetScrollPosX (), GetScrollPosY () + (Height() - 10) );
      return true;
   }


   return false;
}



ViewFormattedText :: ViewFormattedText( const ASCString& title, const ASCString& text, const PG_Rect& pos) : ASC_PG_Dialog( NULL, pos, title )
{
   TextRenderer* pr = new TextRenderer( this, PG_Rect( 10, 40, Width() - 20, Height()-50));
   pr->SetText( text );
};

bool ViewFormattedText :: eventKeyDown(const SDL_KeyboardEvent* key)
{
   switch ( key->keysym.sym ) {
      case SDLK_ESCAPE:
      case SDLK_RETURN:
      case SDLK_SPACE:
         QuitModal();
         return true;
      default:
         return false;
   }
   return false;
}

