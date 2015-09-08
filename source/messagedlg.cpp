/***************************************************************************
                          messagedlg.cpp  -  description
                             -------------------
    begin                : Mon M� 24 2003
    copyright            : (C) 2003 by Martin Bickel
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

#include "messagedlg.h"
#include "gamemap.h"



#include "paradialog.h"
#include "gameeventsystem.h"


#include "dialog.h"
#include "dlg_box.h"
#include "spfst.h"

#include "widgets/textrenderer.h"

#include "messages.h"
#include "pgwidget.h"
#include "widgets/playerselector.h"
#include "pgrichedit.h"
#include "pgmultilineedit.h"
#include "pgtooltiphelp.h"

#include "dialogs/fieldmarker.h"
#include "dialogs/selectionwindow.h"

#include "spfst-legacy.h"

class  NewMessage : public ASC_PG_Dialog {
      GameMap* gamemap;
      Message* message;
      PG_MultiLineEdit* editor;
      PlayerSelector* to;
      PlayerSelector* cc;
      bool reminder;
      
      bool ok()
      {
         if ( !message ) {
            message = new Message ( editor->GetText(), gamemap, 0, 1 << actmap->actplayer );
            gamemap->unsentmessage.push_back ( message );
         } else {
            message->text = editor->GetText();
         }

         if ( reminder ) {
            message->cc = 0;
            message->to = 1 << actmap->actplayer;
         } else {
            message->cc = cc->getSelectedPlayers();
            message->to = to->getSelectedPlayers();
         }
         message->reminder = reminder;
         
         QuitModal();
         return true;
      }
      
      bool cancel()
      {
         QuitModal();
         return true;
      }

      bool insertCursorCoordinates()
      {
         MapCoordinate pos = gamemap->getCursor();
         if ( pos.valid() ) {
            editor->InsertText( pos.toString() );
            return true;
         } else
            return false;
      }

      bool insertCoordinates()
      {
         SelectFromMap::CoordinateList coordinates;
         
         SelectFromMap sfm( coordinates, gamemap );
         sfm.Show();
         sfm.RunModal();
         
         ASCString text = "#coord("; 
         for ( SelectFromMap::CoordinateList::iterator i = coordinates.begin(); i != coordinates.end(); ++i ) {
            if ( i != coordinates.begin() )
               text += ";";
            text += ASCString::toString( i->x ) + "/" + ASCString::toString( i->y );
         }
         text += ")#";
         editor->InsertText( text );
         return true;
      }

      bool eventKeyDown(const SDL_KeyboardEvent* key)
      {
         int mod = SDL_GetModState() & ~(KMOD_NUM | KMOD_CAPS | KMOD_MODE);
         if ( (mod & KMOD_CTRL) &&( key->keysym.sym == SDLK_r )) 
            return insertCursorCoordinates();

         return ASC_PG_Dialog::eventKeyDown(key);
      }

   public:
      NewMessage ( GameMap* gamemap, Message* msg = NULL, bool reminder = false ); 
      
      void Show(bool fade = false ) {
         editor->EditBegin();
         ASC_PG_Dialog::Show(fade);
      }
      
};


NewMessage :: NewMessage ( GameMap* gamemap, Message* msg, bool reminder ) : ASC_PG_Dialog( NULL, PG_Rect( -1, -1, 600, 500 ), "new message" )
{
   this->gamemap = gamemap;
   message = msg;
   if ( message )
      reminder = message->reminder;
   this->reminder = reminder;
    
   int startY = 30;
   
   if ( !reminder )  {
      new PG_Label ( this, PG_Rect( 20, 30, 30, 20 ), "TO:");
      to = new PlayerSelector ( this, PG_Rect( 50, 30, 150, 150 ), gamemap, true, 1 << gamemap->actplayer );
      if ( msg )
         to->setSelection( msg->to );
      
      new PG_Label ( this, PG_Rect( 210, 30, 30, 20 ), "CC:" );
      cc = new PlayerSelector ( this, PG_Rect( 240, 30, 150, 150 ), gamemap, true, 1 << gamemap->actplayer );
      if ( msg )
         cc->setSelection( msg->cc );
      
      startY += 170;
   }
   
   editor = new PG_MultiLineEdit( this, PG_Rect(20, startY, Width() - 140, Height() - startY - 10 ));
   if ( message )
      editor->SetText( message->text );
   editor->SetInputFocus();
    
   AddStandardButton("OK")->sigClick.connect( sigc::mem_fun( *this, &NewMessage::ok ));
   AddStandardButton("Cancel")->sigClick.connect( sigc::mem_fun( *this, &NewMessage::cancel ));
   AddStandardButton("");
   AddStandardButton("Coordinates")->sigClick.connect( sigc::mem_fun( *this, &NewMessage::insertCoordinates ));
   PG_Button* coord = AddStandardButton("Cursor Coord");
   coord->sigClick.connect( sigc::mem_fun( *this, &NewMessage::insertCursorCoordinates ));
   new PG_ToolTipHelp ( coord, "ctrl-r");
  
}
      

class IngameMessageViewer : public ASC_PG_Dialog {
      TextRenderer* textViewer;
      const Message* message;
      PG_Label* from;
      PG_Label* to;
      PG_Label* cc;
      bool keepMessage;

      bool ok()
      {
         keepMessage = false;
         Hide();
         QuitModal();
         return true;
      };

      bool keep()
      {
         keepMessage = true;
         Hide();
         QuitModal();
         return true;
      };
      

      PG_Label* addHeaderLine( int y, const ASCString& name )
      {
         PG_Rect f( 10, y, Width() - 20, 25 );

         new PG_Label( this, PG_Rect( f.x, f.y, 50, f.h ), name );
         Emboss* emb = new Emboss( this, PG_Rect( f.x + 50, f.y, f.w - 50, f.h ), true );
         return new PG_Label( emb, PG_Rect( 2, 2, emb->w - 4, emb->h - 4) );
      }

   public:
      IngameMessageViewer ( const ASCString& title, const Message& msg, PG_Rect rect = PG_Rect( 50, 50, 500, 400 ), bool autoHeader = true ) : ASC_PG_Dialog ( NULL, rect, title ), message(NULL), from(NULL), to(NULL), cc(NULL)
      {

         keepMessage = false;
         int footerHeight;
         /* if ( !buttonText.empty() ) { */
         if ( msg.reminder ) {
            PG_Button* b = new PG_Button( this, PG_Rect( Width() - 70, Height() - 40, 60, 30), "Done" );
            b->sigClick.connect( sigc::mem_fun( *this, &IngameMessageViewer::ok) );
            
            PG_Button* b2 = new PG_Button( this, PG_Rect( Width() - 140, Height() - 40, 60, 30), "Keep" );
            b2->sigClick.connect( sigc::mem_fun( *this, &IngameMessageViewer::keep) );
         } else {
            PG_Button* b = new PG_Button( this, PG_Rect( Width() - 110, Height() - 40, 100, 30), "OK" );
            b->sigClick.connect( sigc::mem_fun( *this, &IngameMessageViewer::ok) );
         }
            footerHeight = 50;
         /* } else
            footerHeight = 10; */

         int y = 40;
         if ( !msg.getFromText( actmap ).empty()  ) {


            from = addHeaderLine( y, "From:");
            y += 30;

            if ( !autoHeader || (msg.to && !(msg.from & 512))  ) {
               to = addHeaderLine( y, "To:");
               y += 30;
            }

            if ( !autoHeader || msg.cc ) {
               cc = addHeaderLine( y, "CC:");
               y += 30;
            }


         }

         PG_Rect r ( 10, y, Width() - 20, Height() - (y + footerHeight ));
         new Emboss( this, r, true );

#ifdef RICHEDIT
         PG_RichEdit* re = new PG_RichEdit( this, PG_Rect(r.x + 2, r.y+2, r.w-4, r.h-4));

         ASCString text  = msg.text;
         while ( text.find ( "#crt#" ) != ASCString::npos )
            text.replace ( text.find  ("#crt#"), 5, " \n");

         re->SetText( text );
         re->SetTransparency(255);
         textViewer = re;
#else
         TextRenderer* tr = new TextRenderer( this, PG_Rect(r.x + 2, r.y+2, r.w-4, r.h-4));
         tr->SetText( msg.text );
         textViewer = tr;
#endif
         SetMessage( msg );
      };

      void SetMessage( const Message& msg )
      {
         if ( &msg != message) {
            PG_Application::BulkModeActivator bulk;
            textViewer->SetText( msg.text );

            if ( from )
               from->SetText( msg.getFromText( actmap ));

            if ( cc )
               cc->SetText( msg.getCcText( actmap ));

            if ( to )
               to->SetText( msg.getToText( actmap ));


            bulk.disable();
            Show();
            message = &msg;
         } else
            Show();
      };


      void 	Hide (bool fade=false)
      {
         ASC_PG_Dialog::Hide(fade);
         getPGApplication().queueWidgetForDeletion( this );
      }

      bool eventKeyDown (const SDL_KeyboardEvent *key)
      {
         if (  key->keysym.sym == SDLK_ESCAPE ) {
            quitModalLoop(10);
            return true;
         }
         if (  key->keysym.sym == SDLK_RETURN || key->keysym.sym == SDLK_KP_ENTER ) {
            quitModalLoop(11);
            return true;
         }
         if (  key->keysym.sym == SDLK_SPACE ) {
            quitModalLoop(12);
            return true;
         }
         
         int mod = SDL_GetModState() & ~(KMOD_NUM | KMOD_CAPS | KMOD_MODE);
         if ( mod & KMOD_CTRL ) 
            if ( key->keysym.sym == 's' )
               textViewer->saveText( mod & KMOD_SHIFT );
         
         
         return ASC_PG_Dialog::eventKeyDown( key );
      };

      bool getKeepMessage() 
      {
         return keepMessage;
      }
      
      ~IngameMessageViewer()
      {
         displayLogMessage ( 9, "~IngameMessageViewer\n" );
      }
      
};




void newmessage()
{
   if ( Player::getHumanPlayerNum( actmap ) < 2 ) {
      infoMessage( "nobody is listening to our transmissions");
      return;
   }
   
   // PG_LineEdit::SetBlinkingTime(-1);
        
   NewMessage  nm ( actmap );
   nm.Show();
   nm.RunModal();
}

void newreminder()
{
   NewMessage  nm ( actmap, NULL, true );
   nm.Show();
   nm.RunModal();
}




class MessageLineWidget: public SelectionWidget
{
       Message* message;
      ASCString msgtime;
      GameMap* map;
   public:
      MessageLineWidget( PG_Widget* parent, const PG_Point& pos, int width, Message* msg, GameMap* gamemap );

      ASCString getName() const
      {
         if ( message->from > 0 && message->from <= 255 )
            return map->getPlayer(getFirstBit(message->from)).getName();
         else
            if ( message->from & ( 1 << 9 ))
               return "system";
         return "";
      };

      Message* getMessage() const { return message; };

   protected:

      void display( SDL_Surface * surface, const PG_Rect & src, const PG_Rect & dst )
      {
      }
      ;
};



MessageLineWidget::MessageLineWidget( PG_Widget* parent, const PG_Point& pos, int width,  Message* msg, GameMap* gamemap ) : SelectionWidget( parent, PG_Rect( pos.x, pos.y, width, 20 )), message( msg ), map(gamemap)
{
#ifndef ctime_r
   msgtime = ctime( &msg->time);
#else

   char c[100];
   ctime_r( &msg->time, c );
   msgtime  = c;
#endif

   int col1 =        width * 3 / 10;
   int col2 = col1 + width * 4 / 10;


   PG_Label* lbl1 = new PG_Label( this, PG_Rect( 0, 0, col1 - 10, Height() ), getName() );
   lbl1->SetFontSize( lbl1->GetFontSize() -2 );

   PG_Label* lbl2 = new PG_Label( this, PG_Rect( col1, 0, col2-col1-10, Height() ), msgtime );
   lbl2->SetFontSize( lbl2->GetFontSize() -2 );

   int x = 0;
   for ( int i = 0; i< gamemap->getPlayerCount(); ++i )
      if ( msg->to & (1 << i)) {
         new ColoredBar( gamemap->getPlayer(i).getColor(), this, PG_Rect( col2 + x, 0, 15 , 15 ));
         x += 18;
      }

   SetTransparency( 255 );
};


class MessageListItemFactory: public SelectionItemFactory  {
   protected:
      const MessagePntrContainer& messageContainer;
      MessagePntrContainer::const_iterator it;
      GameMap* gamemap;

   public:
      MessageListItemFactory( const MessagePntrContainer& messages, GameMap* g );
      
      void restart();
      
      SelectionWidget* spawnNextItem( PG_Widget* parent, const PG_Point& pos );
      
      void itemMarked( const SelectionWidget* widget );
      void itemSelected( const SelectionWidget* widget, bool mouse );

      sigc::signal<void, Message* > messageSelected;
};


MessageListItemFactory::MessageListItemFactory( const MessagePntrContainer& messages, GameMap* map ) : messageContainer ( messages ), gamemap(map)
{
   restart();
};


void MessageListItemFactory::restart()
{
   it = messageContainer.begin();
};

SelectionWidget* MessageListItemFactory::spawnNextItem( PG_Widget* parent, const PG_Point& pos )
{
   if ( it != messageContainer.end() )
      return new MessageLineWidget( parent, pos, parent->Width() - 15, *(it++), gamemap );
   else
      return NULL;
};


void MessageListItemFactory::itemMarked( const SelectionWidget* widget )
{
   if ( !widget )
      return;

   const MessageLineWidget* mlw = dynamic_cast< const MessageLineWidget*>(widget);
   assert( mlw );
   messageSelected( mlw->getMessage() );
}

void MessageListItemFactory::itemSelected( const SelectionWidget* widget, bool mouse )
{
   if ( !widget )
      return;

   const MessageLineWidget* fw = dynamic_cast<const MessageLineWidget*>(widget);
   assert( fw );
   messageSelected( fw->getMessage() );
}




class MessageSelectionWindow : public ASC_PG_Dialog {
   private:
      bool edit;

      bool viewerDeleted( const PG_MessageObject* obj )
      {
         if ( viewer == obj )
            viewer = NULL;
         return true;
      };

      bool ProcessEvent ( const SDL_Event *   event, bool bModal = false  )
      {
         if ( ASC_PG_Dialog::ProcessEvent( event, bModal ) )
            return true;

         if ( viewer ) 
            if ( viewer->ProcessEvent( event, bModal ))
               return true;
         
         return false;
      };

      bool eventKeyDown(const SDL_KeyboardEvent* key)
      {
         
         int mod = SDL_GetModState() & ~(KMOD_NUM | KMOD_CAPS | KMOD_MODE);
         if ( mod & KMOD_CTRL ) 
            if ( key->keysym.sym == 's' )
               if ( viewer )
                  return viewer->eventKeyDown( key );
         
         return ASC_PG_Dialog::eventKeyDown(key);
      }

      

   protected:
      void messageSelected( Message* msg );
      IngameMessageViewer* viewer;

   public:
      MessageSelectionWindow( PG_Widget *parent, const PG_Rect &r, const MessagePntrContainer& messages, GameMap* g, bool editable );
};

void MessageSelectionWindow::messageSelected(  Message* msg )
{
   if ( edit ) {
      NewMessage  nm ( actmap, msg );
      nm.Show();
      nm.RunModal();
   } else {
      if ( !viewer ) {
         PG_Rect r ( my_xpos + Width(), my_ypos, min( PG_Application::GetScreenWidth()/2, 500), Height() );
         viewer = new IngameMessageViewer( "Message", *msg, r );
         viewer->Show();
         viewer->sigDelete.connect( sigc::mem_fun( *this, &MessageSelectionWindow::viewerDeleted ));
      } else 
         viewer->SetMessage( *msg );
   }
};


MessageSelectionWindow::MessageSelectionWindow( PG_Widget *parent, const PG_Rect &r, const MessagePntrContainer& messages, GameMap* gamemap, bool editable ) 
   : ASC_PG_Dialog( parent, r, "Messages" ), edit( editable), viewer(NULL)
{
  
   MessageListItemFactory* factory = new MessageListItemFactory( messages, gamemap );
   factory->messageSelected.connect ( sigc::mem_fun( *this, &MessageSelectionWindow::messageSelected ));

   ItemSelectorWidget* isw = new ItemSelectorWidget( this, PG_Rect(10, GetTitlebarHeight(), r.Width() - 10, r.Height() - GetTitlebarHeight()), factory );
   isw->sigQuitModal.connect( sigc::mem_fun( *this, &ItemSelectorWindow::QuitModal));
};




void viewmessages ( const char* title, const MessagePntrContainer& msg, bool editable  )    // mode : 0 verschickte ; 1 empfangene
{
   int ww = min( PG_Application::GetScreenWidth()/2, 500 );
   PG_Rect r ( max(PG_Application::GetScreenWidth()/2,0) - ww, -1, ww, 500 );
   MessageSelectionWindow msw ( NULL, r, msg, actmap, editable );
   msw.Show();
   msw.RunModal();
}



bool viewmessage ( const Message& message )
{
   //! maybe we are running in text mode
   if ( PG_Application::GetScreen()==NULL )
      return false;
   
   assert( !legacyEventSystemActive() );
   IngameMessageViewer igm( "incoming message...", message );
   igm.Show();
   igm.RunModal();
   return igm.getKeepMessage();
}



void checkJournal( Player& player )
{
   GameMap* actmap = player.getParentMap();
   
   if ( actmap->lastjournalchange.abstime )
      if ( (actmap->lastjournalchange.turn() == actmap->time.turn() ) ||
            (actmap->lastjournalchange.turn() == actmap->time.turn()-1  &&  actmap->lastjournalchange.move() > actmap->actplayer ) )
               viewjournal( false );
}

void viewjournal ( bool showEmptyDlg )
{
   if ( !actmap->gameJournal.empty() ) {
      tviewanytext vat;
      vat.init ( "journal", actmap->gameJournal.c_str() );
      vat.run();
      vat.done();
   } else
      if ( showEmptyDlg )
         infoMessage("no entries to journal yet");

}

void editjournal ( void )
{
   MultilineEdit ej ( actmap->newJournal, "Journal" );
   ej.init ();
   ej.run ();
   if ( ej.changed() )
      actmap->lastjournalchange.set ( actmap->time.turn(), actmap->actplayer );
   ej.done ();
}


void viewunreadmessages ( Player& player )
{

   /** this is hack to prevent several message windows popping up at once.
       The problem is that the idle handler of the first message viewer may trigger or evaluate further events,
       which may themself view new messages 
       */

   static bool isRunning = false;
   if ( isRunning )
      return;

   VariableLocker l( isRunning );
   
   if( (player.stat == Player::human || player.stat == Player::supervisor) && player.getParentMap()->getPlayerView() == player.getPosition() ) {
      MessagePntrContainer::iterator mi = player.unreadmessage.begin();
      while ( mi != player.unreadmessage.end()  ) {
         Message* msg = *mi;
         bool keep = viewmessage ( *msg );
         
         if ( keep )
            ++mi;
         else {
            player.oldmessage.push_back ( *mi );
            mi = player.unreadmessage.erase ( mi );
         }
      }
   }
}
