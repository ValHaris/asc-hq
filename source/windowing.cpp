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


#include <SDL_image.h>
#include <signal.h>


#include <paragui.h>
#include <pgapplication.h>
#include <pgmessagebox.h>
#include <pgdropdown.h>
#include "pgbutton.h"
#include "pgwidgetlist.h"
#include "pglabel.h"
#include "pgwindow.h"
#include "pgmaskedit.h"
#include "pgscrollbar.h"
#include "pgprogressbar.h"
#include "pgradiobutton.h"
#include "pgthemewidget.h"
#include "pgcheckbutton.h"
#include "pgslider.h"
#include "pglistbox.h"
#include "pgcolumnitem.h"
#include "pgpopupmenu.h"
#include "pgspinnerbox.h"
#include "pglog.h"
#include "pgmenubar.h"
#include "pgimage.h"
#include "pgmessagebox.h"
#include "pgwindow.h"
#include "pgrichedit.h"
#include "pgsdleventsupplier.h"
#include "pgmultilineedit.h"
#include "pgtooltiphelp.h"


#include "global.h"

#include "windowing.h"

#include "gameoptions.h"
#include "spfst.h"


#include "resourceplacement.h"

#include "iconrepository.h"
#include "graphics/drawing.h"
#include "widgets/textrenderer.h"

#include "messaginghub.h"


const int widgetTypeNum = 16;
const char* widgetTypes[widgetTypeNum]
=
   { "image",
     "area",
     "statictext",
     "textoutput",
     "bargraph",
     "specialDisplay",
     "specialInput",
     "dummy",
     "multilinetext",
     "scrollarea",
     "button",
     "radiobutton",
     "checkbox",
     "lineedit",
     "slider",
     "plain"
   };

enum  WidgetTypes  { Image,
                     Area,
                     StaticLabel,
                     TextOutput,
                     BarGraph,
                     SpecialDisplay,
                     SpecialInput,
                     Dummy,
                     MultiLineText,
                     ScrollArea,
                     Button,
                     RadioButton,
                     CheckBox,
                     LineEdit,
                     Slider,
                     Plain };

const int imageModeNum = 5;
const char* imageModes[imageModeNum]
    = { "tile",
        "stretch",
        "tile3h",
        "tile3v",
        "tile9" };


const int textAlignNum = 3;
const char* textAlignment[textAlignNum]
=
   { "left",
     "center",
     "right"
   };


const int barDirectionNum = 4;
const char* barDirections[barDirectionNum]
=
   { "left2right",
     "right2left",
     "top2buttom",
     "buttom2top"
   };

const int sliderDirectionNum = 2;
const char* sliderDirections[sliderDirectionNum]
=
   { "vertical",
     "horizontal"
   };
   


ASCGUI_Window::WidgetParameters::WidgetParameters()
      : backgroundMode(PG_Draw::TILE),  
        textAlign( PG_Label::LEFT ), textAlign_defined(false),
        fontColor(0xffffff), fontColor_defined(false),
        fontAlpha(255), fontAlpha_defined(false),
        fontSize(8), fontSize_defined(false),
        backgroundColor_defined(false),
        transparency(0), transparency_defined(false),
        bordersize(1), bordersize_defined(false),
        hidden(false)
{
}

int ASCGUI_Window::RunModal()
{
   WindowCounter wc;
   return PG_Window::RunModal();
}


void  ASCGUI_Window::WidgetParameters::runTextIO ( PropertyReadingContainer& pc )
{
   if ( pc.find( "BackgroundImage" )) {
      pc.addString( "BackgroundImage", backgroundImage, backgroundImage );
      int i = backgroundMode;
      pc.addNamedInteger( "BackgroundMode", i, imageModeNum, imageModes, i);
      backgroundMode = PG_Draw::BkMode( i );
   }

   if ( pc.find( "TextAlign" )) {
      int ta = textAlign;
      pc.addNamedInteger( "TextAlign", ta, textAlignNum, textAlignment, ta );
      textAlign = PG_Label::TextAlign( ta );
      textAlign_defined = true;
   }

   if ( pc.find( "FontColor")) {
      pc.addInteger("FontColor", fontColor, fontColor );
      fontColor_defined = true;
   }
   
   pc.addString("FontName", fontName, fontName );
   
   if ( pc.find( "FontAlpha")) {
      pc.addInteger("FontAlpha", fontAlpha, fontAlpha );
      fontAlpha_defined = true;
   }    
   
   if ( pc.find( "FontSize")) {
      pc.addInteger("FontSize", fontSize, fontSize );
      fontSize_defined = true;
   }
   
   if ( pc.find( "BackgroundColor" )) {
      pc.addInteger("BackgroundColor", backgroundColor,  backgroundColor );
      backgroundImage.clear();
      backgroundColor_defined = true;
   };
   
   if ( pc.find("Transparency")) {
      pc.addInteger("Transparency", transparency, transparency );
      transparency_defined = true;
   }
   
   if ( pc.find("Bordersize")) {
      pc.addInteger("Bordersize", bordersize, bordersize );
      bordersize_defined = true;
   }

   pc.addBool( "hidden", hidden, hidden );
   pc.addString("Style", style, style );
}


void  ASCGUI_Window::WidgetParameters::assign( BarGraphWidget* widget )
{
   if ( !widget )
      return;

   if ( backgroundColor_defined )
      widget->setColor( backgroundColor );

   assign( (PG_ThemeWidget*)widget );

}


bool hasTransparency( const Surface& surf )
{
   if ( surf.GetPixelFormat().BytesPerPixel() == 4 ) {
      const Uint32* p = (const Uint32*) surf.pixels();
      int aMask = surf.GetPixelFormat().Amask();
      int aShift = surf.GetPixelFormat().Ashift();
      for ( int y = 0; y < surf.h() ; ++y ) {
         for ( int x = 0; x < surf.w() ; ++x)
            if ( ((p[x] & aMask) >> aShift ) != Surface::opaque )
               return true;
         p += surf.pitch() / 4;
      }
   }
   return false;
}


void  ASCGUI_Window::WidgetParameters::assign( PG_ThemeWidget* widget )
{
   if ( !widget )
      return;

   
   if ( !backgroundImage.empty() ) {
      widget->SetBackground( IconRepository::getIcon(backgroundImage).getBaseSurface(), backgroundMode );
      widget->SetDirtyUpdate( !hasTransparency( IconRepository::getIcon(backgroundImage) ));
   } else {
      if ( backgroundColor_defined ) {
         widget->SetBackground( NULL );
         widget->SetSimpleBackground( true );
      }
   }

   if ( backgroundColor_defined )
      widget->SetBackgroundColor( backgroundColor );
      
   assign( (PG_Widget*)widget );
}

void  ASCGUI_Window::WidgetParameters::assign( PG_Label* widget )
{
   if ( !widget )
      return;

   if ( textAlign_defined )
      widget->SetAlignment( textAlign );

   assign( (PG_Widget*)widget );
}


void  ASCGUI_Window::WidgetParameters::assign( PG_Widget* widget )
{
   if ( !widget )
      return;

   if ( fontColor_defined ) 
      widget->SetFontColor( fontColor, true );
      
   if ( !fontName.empty() )
      widget->SetFontName( fontName, true );
      
   if ( fontAlpha_defined )
      widget->SetFontAlpha( fontAlpha, true );
   
   if ( fontSize_defined )
      widget->SetFontSize( fontSize, true );
   
   if ( transparency_defined )
      widget->SetTransparency( transparency, true );

   if( bordersize_defined )
      widget->SetBorderSize( bordersize );
   
   if ( hidden )
      widget->Hide(false);
}


ASCGUI_Window::ASCGUI_Window ( PG_Widget *parent, const PG_Rect &r, const ASCString& panelName_, const ASCString& baseStyle, bool loadTheme )
   : PG_Window ( parent, r, "", DEFAULT, baseStyle, 9 ), panelName( panelName_ ), textPropertyGroup(NULL)
{
      // FIXME Hide button does not delete Panel      
}


PG_Rect ASCGUI_Window::parseRect ( PropertyReadingContainer& pc, PG_Widget* parent )
{
   int x,y,w,h,x2,y2;
   // pc.openBracket( "position" );
   pc.addInteger( "x", x );
   pc.addInteger( "y", y );
   pc.addInteger( "width", w, 0 );
   pc.addInteger( "height", h, 0 );
   pc.addInteger( "x2", x2, 0 );
   pc.addInteger( "y2", y2, 0 );
   // pc.closeBracket();

   PG_Rect r ( x,y,w,h);

   if ( x < 0 )
      r.x = parent->Width() - w + x;

   if ( r.y < 0 )
      r.y = parent->Height() - h + y;

   if ( x2 != 0 ) {
      if ( x2 < 0 )
         x2 = parent->Width() + x2;

      w = x2 - r.x;
   }

   if ( y2 != 0 ) {
      if ( y2 < 0 )
         y2 = parent->Height() + y2;

      h = y2 - r.y;
   }


   if ( w <= 0 )
      r.w = parent->Width() - r.x;
   else
      r.w = w;

   if ( h <= 0 )
      r.h = parent->Height() - r.y;
   else
      r.h = h;

   return r;
}




void ASCGUI_Window::parsePanelASCTXT ( PropertyReadingContainer& pc, PG_Widget* parent, WidgetParameters widgetParams )
{
   ASCString name;
   pc.addString( "name", name, "" );
   parent->SetName( name );
   
   int transparency;
   pc.addInteger("localtransparency", transparency, -1 );
   if ( transparency != -1 ) {
      parent->SetTransparency( transparency );
   }
   
   if ( pc.find( "userHandler" )) {
      ASCString label;
      pc.addString( "userHandler", label);
      userHandler( label, pc, parent, widgetParams );
   }


   vector<ASCString> childNames;


   if ( pc.find( "ChildWidgets" )) 
      pc.addStringArray( "ChildWidgets", childNames );


   int widgetNum;
   pc.addInteger( "WidgetNum", widgetNum, 0 );

   for ( int i = 0; i < widgetNum; ++i) 
      childNames.push_back( ASCString("Widget") + ASCString::toString( i ));

      
   PG_RadioButton* radioButtonGroup = NULL;


   WidgetParameters originalWidgetParams = widgetParams;
            
   for ( int i = 0; i < childNames.size(); ++i) {

      widgetParams  = originalWidgetParams;
      
      pc.openBracket( childNames[i] );

      ASCString toolTipHelp;
      pc.addString( "ToolTipHelp", toolTipHelp, "" );
      

      PG_Rect r = parseRect( pc, parent );

      widgetParams.runTextIO( pc );
      
      bool hasStyle = pc.find( "style" );
      ASCString style;
      pc.addString( "style", style, "Panel" );


      int type;
      pc.addNamedInteger( "type", type, widgetTypeNum, widgetTypes );

      PG_Widget* newWidget = NULL;
      
      if ( type == Image ) {
         ASCString filename;
         pc.addString( "FileName", filename, "" );
         int imgMode;
         pc.addNamedInteger( "mode", imgMode, imageModeNum, imageModes, 0 );

         
         if ( !filename.empty() ) {
            try {
               Surface& surf = IconRepository::getIcon(filename);
               PG_Image* img = new PG_Image( parent, PG_Point(r.x, r.y ), surf.getBaseSurface(), false, PG_Draw::BkMode(imgMode) );
               newWidget = img;
               img->SetDirtyUpdate( !hasTransparency( surf ));

               widgetParams.assign ( img );
               parsePanelASCTXT( pc, img, widgetParams );
            } catch ( tfileerror ) {
               warning( "unable to load " + filename );
            }
         } else {
            PG_Image* img = new PG_Image( parent, PG_Point(r.x, r.y ), NULL, false, PG_Draw::BkMode(imgMode) );
            newWidget = img;
            widgetParams.assign ( img );
            parsePanelASCTXT( pc, img, widgetParams );
         }

      }
      if ( type == Area ) {
         bool mode;
         pc.addBool( "in", mode, true );

         Emboss* tw = new Emboss ( parent, r );
         // PG_ThemeWidget* tw = new PG_ThemeWidget ( parent, r, style );
         widgetParams.assign ( tw );
         parsePanelASCTXT( pc, tw, widgetParams );
         newWidget = tw;
      }

      if ( type == StaticLabel ) {
         ASCString text;
         pc.addString( "text", text );

         PG_Label* lb = new PG_Label ( parent, r, text, style );
         if ( !hasStyle )
            widgetParams.assign ( lb );
         parsePanelASCTXT( pc, lb, widgetParams );
         newWidget = lb;
      }
      if ( type == TextOutput ) {
         PG_Label* lb = new PG_Label ( parent, r, PG_NULLSTR, style );

         if ( !hasStyle )
            widgetParams.assign ( lb );
         parsePanelASCTXT( pc, lb, widgetParams );
         newWidget = lb;
      }
      if ( type == MultiLineText ) {
         /*
         PG_MultiLineEdit* lb = new PG_MultiLineEdit ( parent, r, style );

         lb->SetEditable(false);
         if ( !hasStyle ) {
            lb->SetBorderSize(0);
            widgetParams.assign ( lb );
         }
         */

         TextRenderer* tr = new TextRenderer( parent, r, style );
         if ( !hasStyle ) 
            widgetParams.assign ( tr );
         
         parsePanelASCTXT( pc, tr, widgetParams );
         newWidget = tr;
      }

      if ( type == BarGraph ) {
         int dir;
         pc.addNamedInteger( "direction", dir, barDirectionNum, barDirections, 0 );

         int cnum;
         pc.addInteger("Colors",cnum, 0 );
         BarGraphWidget::Colors colorRange;
         for ( int i = 0; i < cnum; ++i ) {
            pc.openBracket("Color" + ASCString::toString(i));
            int col;
            pc.addInteger("color", col);
            double f;
            pc.addDFloat("fraction", f );
            colorRange.push_back( make_pair(f,col) );
            pc.closeBracket();
         }

         BarGraphWidget* bg = new BarGraphWidget ( parent, r, BarGraphWidget::Direction(dir) );
         bg->setColor( colorRange );
         widgetParams.assign ( bg );
         parsePanelASCTXT( pc, bg, widgetParams );
         newWidget = bg;
      }


      if ( type == SpecialDisplay ) {
         SpecialDisplayWidget* sw = new SpecialDisplayWidget ( parent, r );
         widgetParams.assign ( sw );

         parsePanelASCTXT( pc, sw, widgetParams );
         newWidget = sw;
      }

      if ( type == SpecialInput ) {
         SpecialInputWidget* sw = new SpecialInputWidget ( parent, r );

         parsePanelASCTXT( pc, sw, widgetParams );
         newWidget = sw;
      }

      if ( type == Dummy ) {
         SpecialInputWidget* sw = new SpecialInputWidget ( parent, r );
         parsePanelASCTXT( pc, sw, widgetParams );
         newWidget = sw;
      }
      if ( type == ScrollArea ) {
         PG_ScrollWidget* sw = new PG_ScrollWidget( parent, r, style );
         ASCString scrollbar;
         pc.addString( "horizontal_scollbar", scrollbar, "true" );
         if ( scrollbar.compare_ci( "false" ) == 0)
            sw->EnableScrollBar(false, PG_ScrollBar::HORIZONTAL );

         pc.addString( "vertical_scollbar", scrollbar, "true" );
         if ( scrollbar.compare_ci( "false" ) == 0)
            sw->EnableScrollBar(false, PG_ScrollBar::VERTICAL );

         if ( !hasStyle )
            widgetParams.assign ( sw );
            
         parsePanelASCTXT( pc, sw, widgetParams );
         newWidget = sw;
      }
      
      if ( type == Button ) {
         PG_Button* sw = new PG_Button( parent, r, style );

         ASCString text;
         pc.addString( "text", text, "" );
         
         // if ( !text.empty() )
            sw->SetText( text );
         
         
         if ( !hasStyle )
            widgetParams.assign ( sw );
            
         parsePanelASCTXT( pc, sw, widgetParams );
         newWidget = sw;
      }

      if ( type == RadioButton ) {
         PG_RadioButton* sw = new PG_RadioButton( parent, r, style );
         if ( radioButtonGroup )
            radioButtonGroup->AddToGroup( sw );
            
         radioButtonGroup = sw;

         ASCString text;
         pc.addString( "text", text );
         
         if ( !text.empty() )
            sw->SetText( text );
         
         if ( !hasStyle )
            widgetParams.assign ( sw );
            
         parsePanelASCTXT( pc, sw, widgetParams );
         newWidget = sw;
         
      }
      if ( type == CheckBox ) {
         PG_CheckButton* sw = new PG_CheckButton( parent, r, style );

         ASCString text;
         pc.addString( "text", text );
         
         if ( !text.empty() )
            sw->SetText( text );
         
         if ( !hasStyle )
            widgetParams.assign ( sw );
            
         parsePanelASCTXT( pc, sw, widgetParams );
         newWidget = sw;
         
      }
      if ( type == LineEdit ) {
         PG_LineEdit* sw = new PG_LineEdit( parent, r, style );
         
         ASCString text;
         pc.addString( "text", text, "" );
         
         if ( !text.empty() )
            sw->SetText( text );
         
         if ( !hasStyle )
            widgetParams.assign ( sw );
            
         parsePanelASCTXT( pc, sw, widgetParams );
         newWidget = sw;
      }
      if ( type == Slider ) {
         
         int direction = 0;
         pc.addNamedInteger( "Orientation", direction, sliderDirectionNum, sliderDirections, direction);
        
         
         PG_ScrollBar* sb = new PG_Slider( parent, r, PG_ScrollBar::ScrollDirection(direction), -1, style );
         
         if ( !hasStyle )
            widgetParams.assign ( sb );
            
         parsePanelASCTXT( pc, sb, widgetParams );
         newWidget = sb;
      }
      if ( type == Plain ) {
         
         PG_ThemeWidget* w = new PG_ThemeWidget( parent, r );
         
         if ( !hasStyle )
            widgetParams.assign ( w );
            
         parsePanelASCTXT( pc, w, widgetParams );
         newWidget = w;
      }
            
      if ( newWidget && newWidget->GetName().empty() ) 
         newWidget->SetName( childNames[i] );
      
      if ( newWidget && newWidget->GetName() == "$pos" )
         newWidget->SetName( pc.getNameStack() );
      

      if ( newWidget && !toolTipHelp.empty() )
         new PG_ToolTipHelp( newWidget, toolTipHelp );

      pc.closeBracket();
   }
}

void ASCGUI_Window::rename( const ASCString& widgetName, const ASCString& newname, PG_Widget* parent )
{
   if ( !parent )
      parent = this;

   PG_Widget* w = parent->FindChild( widgetName, true );
   if ( w )
      w->SetName( newname );
}

void ASCGUI_Window::setLabelText ( const ASCString& widgetName, const ASCString& text, PG_Widget* parent )
{
   if ( !parent )
      parent = this;

   PG_Label* l = dynamic_cast<PG_Label*>( parent->FindChild( widgetName, true ) );
   if ( l )
      l->SetText( text );
   else {
      PG_LineEdit* l = dynamic_cast<PG_LineEdit*>( parent->FindChild( widgetName, true ) );
      if ( l )
         l->SetText( text );
      else {
         PG_MultiLineEdit* l = dynamic_cast<PG_MultiLineEdit*>( parent->FindChild( widgetName, true ) );
         if ( l )
            l->SetText( text );
         else {
            TextRenderer* l = dynamic_cast<TextRenderer*>( parent->FindChild( widgetName, true ) );
            if ( l )
               l->SetText( text );
         }
      }
   }
}

void ASCGUI_Window::setLabelColor ( const ASCString& widgetName, PG_Color color, PG_Widget* parent )
{
   if ( !parent )
      parent = this;

   PG_Label* l = dynamic_cast<PG_Label*>( parent->FindChild( widgetName, true ) );
   if ( l )
      l->SetFontColor ( color );
   else {
      PG_MultiLineEdit* l = dynamic_cast<PG_MultiLineEdit*>( parent->FindChild( widgetName, true ) );
      if ( l )
         l->SetFontColor ( color );
   }
}


void ASCGUI_Window::setLabelText ( const ASCString& widgetName, int i, PG_Widget* parent )
{
   ASCString s = ASCString::toString(i);
   setLabelText ( widgetName, s, parent );
}


void ASCGUI_Window::setImage ( const ASCString& widgetName, const ASCString& filename, PG_Widget* parent )
{
   setImage( widgetName, IconRepository::getIcon( filename ), parent );
}


void ASCGUI_Window::setImage ( const ASCString& widgetName, Surface& image, PG_Widget* parent )
{
   setImage( widgetName, image.getBaseSurface(), parent);
}

void ASCGUI_Window::setImage ( const ASCString& widgetName, SDL_Surface* image, PG_Widget* parent )
{
   if ( !parent )
      parent = this;

   PG_Image* i = dynamic_cast<PG_Image*>( parent->FindChild( widgetName, true ) );
   if ( i ) {
      i->SetImage( image, false );
      if ( image )
         i->SizeWidget( image->w, image->h);
   }
}

void ASCGUI_Window::setWidgetTransparency ( const ASCString& widgetName, int transparency, PG_Widget* parent  )
{
   if ( !parent )
      parent = this;

   PG_Widget* i = parent->FindChild( widgetName, true );
   if ( i )
      i->SetTransparency( transparency );
}


void ASCGUI_Window::hide ( const ASCString& widgetName, PG_Widget* parent )
{
   if ( !parent )
      parent = this;

   PG_Widget* i = parent->FindChild( widgetName, true );
   if ( i )
      i->Hide();
}

void ASCGUI_Window::show ( const ASCString& widgetName, PG_Widget* parent )
{
   if ( !parent )
      parent = this;

   PG_Widget* i = parent->FindChild( widgetName, true );
   if ( i )
      i->Show();
}



void ASCGUI_Window::setBargraphValue( const ASCString& widgetName, float fraction, PG_Widget* parent )
{
   if ( !parent )
      parent = this;

   BarGraphWidget* bgw = dynamic_cast<BarGraphWidget*>( parent->FindChild( widgetName, true ) );
   if ( bgw )
      bgw->setFraction( fraction );
}


void ASCGUI_Window::setBarGraphColor( const ASCString& widgetName, PG_Color color, PG_Widget* parent )
{
   if ( !parent )
      parent = this;

   BarGraphWidget* bgw = dynamic_cast<BarGraphWidget*>( parent->FindChild( widgetName, true ) );
   if ( bgw )
      bgw->setColor( color );
}



ASCGUI_Window::GuiCache ASCGUI_Window::guiCache;


bool ASCGUI_Window::setup()
{
   try {
      WidgetParameters widgetParameters = getDefaultWidgetParams();

      panelName.toLower();

      if ( !CGameOptions::Instance()->cacheASCGUI || guiCache.find(panelName) == guiCache.end() ) {
         tnfilestream s ( panelName + ".ascgui", tnstream::reading );

         TextFormatParser tfp ( &s );
         textPropertyGroup = tfp.run();

         if ( CGameOptions::Instance()->cacheASCGUI ) 
            guiCache[panelName] = textPropertyGroup;
      } else 
         textPropertyGroup = guiCache[panelName];

      PropertyReadingContainer pc ( "panel", textPropertyGroup );

      int w, h;
      pc.addInteger( "width", w, 0 );
      pc.addInteger( "height", h, 0 );

      if ( w > 0 && h > 0 )
         SizeWidget( w, h, false );
      else
         if ( h > 0 )
            SizeWidget( Width(), h, false );
         else
            if ( w > 0 )
               SizeWidget( w, Height(), false );


      if ( pc.find("x") && pc.find("y" )) {
         int x1,y1;
         pc.addInteger( "x", x1 );
         pc.addInteger( "y", y1 );

         if ( x1 < 0 )
            x1 = GetParent()->Width() - Width() + x1;

         if ( y1 < 0 )
            y1 = GetParent()->Height() - Height() + y1;

         MoveWidget( x1, y1, false );
      }

      int titlebarHeight;
      pc.addInteger("Titlebarheight", titlebarHeight, -1 );
      if ( titlebarHeight != -1 )
         SetTitlebarHeight( titlebarHeight );

      ASCString title;
      pc.addString("Title", title, "" );
      if ( !title.empty() )
         SetTitle( title );
         
      widgetParameters.runTextIO( pc );
      widgetParameters.assign ( this );

      parsePanelASCTXT( pc, this, widgetParameters );

      return true;
   } catch ( ParsingError err ) {
      warning( "parsing error: " + err.getMessage());
   } catch ( tfileerror err ) {
      warning( "could not acces file: " + err.getFileName() );
   }
   return false;

}


void ASCGUI_Window::destruct()
{
   if ( textPropertyGroup ) {
      for ( GuiCache::iterator i = guiCache.begin(); i != guiCache.end(); ++i )
         if ( i->second == textPropertyGroup )
            return;

      delete textPropertyGroup;
   }
}

ASCGUI_Window::~ASCGUI_Window()
{
   destruct();
}


Panel :: Panel ( PG_Widget *parent, const PG_Rect &r, const ASCString& panelName_, bool loadTheme )
       : ASCGUI_Window( parent, r, panelName_, "Panel", loadTheme )
{
   if ( loadTheme )
      setup();
      
   BringToFront();
}

ASCGUI_Window::WidgetParameters Panel::getDefaultWidgetParams()
{
    static WidgetParameters defaultWidgetParameters;
    static ASCString panelBackgroundImage;
    static bool defaultsLoaded = false;
    if ( !defaultsLoaded ) {
       tnfilestream s ( "default.ascgui", tnstream::reading );

       TextFormatParser tfp ( &s );
       auto_ptr<TextPropertyGroup> tpg ( tfp.run());

       PropertyReadingContainer pc ( "panel", tpg.get() );

       defaultWidgetParameters.runTextIO ( pc );
       pc.addString("PanelBackgroundImage", panelBackgroundImage );
       defaultsLoaded = true;
    }
    return defaultWidgetParameters;
}



LayoutablePanel :: LayoutablePanel ( PG_Widget *parent, const PG_Rect &r, const ASCString& panelName_, bool loadTheme )
   : Panel ( parent, PositionCalculator( r, panelName_), panelName_, loadTheme ), name( panelName_ )
{
}


PG_Rect LayoutablePanel::PositionCalculator( const PG_Rect& rect, const ASCString& panelName )
{
   CGameOptions::PanelDataContainer::iterator option = CGameOptions::Instance()->panelData.find( panelName );
   if ( option != CGameOptions::Instance()->panelData.end() ) {
      PG_Rect r ( option->second.x, option->second.y, DashBoardWidth, rect.Height() );
      
      if ( r.x + r.w >  PG_Application::GetScreenWidth() )
         r.x = PG_Application::GetScreenWidth() - r.w;
         
      if ( r.y + r.h >  PG_Application::GetScreenHeight() )
         r.y = PG_Application::GetScreenHeight() - r.y;
      
      return r;
   } else
      return PG_Rect( rect.x, rect.y, DashBoardWidth, rect.Height() );
}

void LayoutablePanel::eventMoveWidget (int x, int y)
{
   Panel::eventMoveWidget(x,y);
   CGameOptions::PanelData pos;
   pos.x = x;
   pos.y = y;
   pos.visible = true;

   CGameOptions::Instance()->updatePanelData( name, pos );
}

void LayoutablePanel::eventHide()
{
   Panel::eventHide();
   
   CGameOptions::PanelData pos;
   pos.x = my_xpos;
   pos.y = my_ypos;
   pos.visible = false;

   CGameOptions::Instance()->updatePanelData( name, pos );
}

void LayoutablePanel::eventShow()
{
   Panel::eventShow();
   
   CGameOptions::PanelData pos;
   pos.x = my_xpos;
   pos.y = my_ypos;
   pos.visible = true;

   CGameOptions::Instance()->updatePanelData( name, pos );
}






ConfigurableWindow :: ConfigurableWindow ( PG_Widget *parent, const PG_Rect &r, const ASCString& panelName_, bool loadTheme )
       : ASCGUI_Window( parent, r, panelName_, "Window", loadTheme )
{
   if ( loadTheme )
      setup();
      
   BringToFront();
}

ASCGUI_Window::WidgetParameters ConfigurableWindow ::getDefaultWidgetParams()
{
   return  WidgetParameters();
}

