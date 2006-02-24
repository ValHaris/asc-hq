/***************************************************************************
                          messagedlg.cpp  -  description
                             -------------------
    begin                : Mon Mär 24 2003
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


#include "gamedlg.h"
#include "messagedlg.h"
#include "gamemap.h"
#include "paradialog.h"
#include "gameeventsystem.h"

#include "dialog.h"
#include "dlg_box.h"
#include "spfst.h"

class tnewmessage : public tmessagedlg  {
            protected:
               char* rtitle;
            public:
               tnewmessage( void );
               void init ( void );
               void buttonpressed ( int id );
               void run ( void );
       };

tnewmessage :: tnewmessage ( void )
{
   rtitle = "new message";
}

void tnewmessage :: init ( void )
{
   tdialogbox :: init ( );
   title = rtitle;
   for ( int i = 0; i < 8; i++ )
      if ( actmap->player[i].exist() &&  actmap->actplayer != i )
         to[i] = 1;
      else
         to[i] = 0;

   setup();
   buildgraphics();


   rectangle ( x1 + tx1 - 2, y1 + ty1 - 2, x1 + tx2 + 2, y1 + ty2 + 2 ,black );


   activefontsettings.font = schriften.smallarial;
   activefontsettings.length = tx2 - tx1 -1 ;
   activefontsettings.height = activefontsettings.font->height+5;
   activefontsettings.background = dblue;
   activefontsettings.color = black;
   activefontsettings.justify = lefttext;

   inserttext ( "" );
   actparagraph = firstparagraph;
   actparagraph->cursor = 0;
   actparagraph->setpos( x1 + tx1, y1 + ty1, y1 + ty2, 0, 13 );

   while ( actparagraph ) {
      actparagraph->reflow( 0 );
      actparagraph = actparagraph->next;
   }
   actparagraph = firstparagraph;
   actparagraph->checkscrolldown();
   actparagraph->display();


}


void tnewmessage :: buttonpressed ( int id )
{
   tmessagedlg :: buttonpressed ( id );
   if ( id == 1 )
      ok = 1;
   if ( id == 2 )
      ok = 2;
}


void tnewmessage :: run ( void )
{
   mousevisible ( true );

   do {

      tmessagedlg::run ( );

   } while ( !ok ); /* enddo */
   if ( ok == 1 ) {
      Message* message = new Message ( extracttext(), actmap, 0, 1 << actmap->actplayer );
      for ( int i = 0; i < 8; i++ ) {
         if ( actmap->player[i].exist() )
            if ( actmap->actplayer != i )
               message->to |= to[i] << i;
      }

      actmap->unsentmessage.push_back ( message );
   }
}

void newmessage ( void )
{
  tnewmessage nm;
  nm.init();
  nm.run();
  nm.done();
}



class teditmessage : public tmessagedlg  {
            protected:
               Message* message;
            public:
               void init ( Message* msg );
               void buttonpressed ( int id );
               void run ( void );
       };


void teditmessage :: init ( Message* msg  )
{
   message = msg;
   tdialogbox :: init ( );
   title = "edit message";

   for ( int i = 0; i < 8; i++ )
      to[i] = (message->to & ( 1 << i )) > 0;

   setup();
   buildgraphics();


   rectangle ( x1 + tx1 - 2, y1 + ty1 - 2, x1 + tx2 + 2, y1 + ty2 + 2 ,black );


   activefontsettings.font = schriften.smallarial;
   activefontsettings.length = tx2 - tx1 -1 ;
   activefontsettings.height = activefontsettings.font->height+5;
   activefontsettings.background = dblue;
   activefontsettings.color = black;
   activefontsettings.justify = lefttext;

   inserttext ( msg->text );
   actparagraph = firstparagraph;
   actparagraph->cursor = 0;
   actparagraph->setpos( x1 + tx1, y1 + ty1, y1 + ty2, 0, 13 );

   while ( actparagraph ) {
      actparagraph->reflow( 0 );
      actparagraph = actparagraph->next;
   }
   actparagraph = firstparagraph;
   actparagraph->checkscrolldown();
   actparagraph->display();


}


void teditmessage :: buttonpressed ( int id )
{
   tmessagedlg :: buttonpressed ( id );
   if ( id == 1 )
      ok = 1;
   if ( id == 2 )
      ok = 2;
}


void teditmessage :: run ( void )
{
   mousevisible ( true );

   do {
      tmessagedlg::run ( );
   } while ( !ok ); /* enddo */

   if ( ok == 1 ) {
      message->text = extracttext();
      message->to = 0;
      for ( int i = 0; i < 8; i++ ) {
         if ( actmap->player[i].exist() )
            if ( actmap->actplayer != i )
               message->to |= to[i] << i;
      }
   }
}


void editmessage ( Message& msg )
{
  teditmessage nm;
  nm.init( &msg );
  nm.run();
  nm.done();
}

class tviewmessages : public tdialogbox {
               typedef vector<Message*> MsgVec;
               MsgVec msg;
               MsgVec::iterator firstdisplayed;
               MsgVec::iterator marked;
               int player[8];
               int mode;
               int ok;
               int dispnum;
               int scrollpos;
               int __num;

            public:
               tviewmessages ( const MessagePntrContainer& _msglist );
               void init ( char* ttl, bool editable, int md  );    // mode : 0 verschickte ; 1 empfangene
               int  getcapabilities ( void ) { return 1; };
               void redraw ( void );
               void run ( void );
               void buttonpressed ( int id );
               void paintmessages ( void );
               void checkforscroll ( bool mouse );
          };


tviewmessages :: tviewmessages ( const MessagePntrContainer& msglist )
{
   msg.insert ( msg.end(), msglist.begin(), msglist.end());
   ok = 0;
   dispnum = 10;
   marked = msg.end();
   firstdisplayed = msg.begin();
   __num = msg.size();
   scrollpos = 0;
}

void tviewmessages :: init ( char* ttl, bool editable, int md )
{
   mode = md;
   tdialogbox :: init ( );

   xsize =  400;
   ysize =  300;
   title = ttl;
   windowstyle &= ~dlg_in3d;
   if ( editable ) {
     addbutton ( "~V~iew",    10,              ysize - 30, xsize / 3 - 5, ysize - 10 ,   0, 1, 1, msg.size() > 0 );
     addkey    ( 1, ct_enter );

     addbutton ( "~E~dit",    xsize / 3 + 5,   ysize - 30, 2*xsize / 3 - 5, ysize - 10 , 0, 1, 2, msg.size() > 0 );

     addbutton ( "e~x~it",  2*xsize / 3 + 5, ysize - 30, xsize - 10, ysize - 10 ,      0, 1, 3, true );
     addkey    ( 3, ct_esc );
   } else {
     addbutton ( "~V~iew",    10,            ysize - 30, xsize / 2 - 5, ysize - 10 ,   0, 1, 1, msg.size() > 0  );
     addkey    ( 1, ct_enter );

     addbutton ( "e~x~it",  xsize / 2 + 5, ysize - 30, xsize - 10, ysize - 10 ,      0, 1, 3, true );
     addkey    ( 3, ct_esc );
   }

   if ( msg.size() > dispnum )
     addscrollbar ( xsize - 25, starty, xsize - 10, ysize - 40 , &__num, dispnum, &scrollpos, 4, 0 );

   int b = 0;

   for ( int i = 0; i < 8; i++ )
      if ( actmap->player[i].exist()  &&  actmap->actplayer != i )
         player[i] = b++;
      else
         player[i] = -1;

   buildgraphics();

}

void tviewmessages :: buttonpressed ( int id )
{
   if ( id == 1 )
      if ( marked != msg.end() )
         viewmessage ( **marked );

   if ( id == 2 )
      if ( marked != msg.end() )
         editmessage ( **marked );

   if ( id == 3 )
      ok = 1;

   if ( id == 4 ) {
      checkforscroll( true );
      paintmessages();
   }
}

void tviewmessages :: paintmessages ( void )
{
    MsgVec::iterator a = firstdisplayed;
    activefontsettings.font = schriften.smallarial;
    activefontsettings.background = dblue;
    activefontsettings.justify = lefttext;
    int displayed = 0;
    while ( a != msg.end()  &&  displayed < dispnum ) {
       if ( a == marked )
          activefontsettings.color= white;
       else
          activefontsettings.color= black;

       activefontsettings.length = 190;

       tm *tmbuf;
       tmbuf = localtime ( &( (*a)->time ) );
       int y = y1 + starty + 10 + ( a - firstdisplayed ) * 20 ;

       showtext2 (asctime (tmbuf), x1 + 20, y);

       activefontsettings.length = 100;
       if ( mode ) {
          int fr = log2 ( (*a)->from );
          if ( fr < 8 )
             showtext2 ( actmap->player[ fr ].getName().c_str(), x1 + 220, y );
          else
             showtext2 ( "system", x1 + 220, y );
       } else {
          for ( int i = 0; i < 8; i++ )
             if ( player[i] >= 0 ) {
                int x = x1 + 220 + player[i] * 15;
                int color;
                if ( (*a)->to & ( 1 << i ) )
                   color = 20 + i * 8;
                else
                   color = dblue ;

                bar ( x, y, x + 10, y + 10, color );

             }
       }

       a++;
       displayed++;
    }
}

void tviewmessages :: checkforscroll ( bool mouse )
{
   firstdisplayed = msg.begin() + scrollpos;
   if ( firstdisplayed >= msg.end() )
      displaymessage (" tviewmessages :: checkforscroll  -- invalid scrolling operation ", 2 );

   MsgVec::iterator oldfirst = firstdisplayed;
   if ( mouse ) {
      // if called by scrollbar the marked position is changed according to
      // scrolling position; else vice versa
      if ( marked < firstdisplayed )
        marked = firstdisplayed;
      if ( marked > firstdisplayed + (dispnum-1) )
         marked = firstdisplayed + (dispnum-1) ;
   } else {
      if ( marked < firstdisplayed )
        firstdisplayed = marked;
      if ( marked > firstdisplayed + (dispnum-1) )
         firstdisplayed = marked - (dispnum-1) ;
   }

   if ( oldfirst != firstdisplayed )
      if ( msg.size() > dispnum )
         enablebutton ( 4 );
}


void tviewmessages :: redraw ( void )
{
   tdialogbox::redraw();
   if ( msg.size() > dispnum )
     rahmen ( true, x1 + 10, y1 + starty, x1 + xsize - 30, y1 + ysize - 40 );
   else
     rahmen ( true, x1 + 10, y1 + starty, x1 + xsize - 10, y1 + ysize - 40 );

   paintmessages();
}

void tviewmessages :: run ( void )
{
   int mstatus = 0;

   mousevisible( true );
   do {
      tdialogbox :: run ( );
         if ( taste == ct_down  &&  marked+1 < msg.end() ) {
            marked++;
            checkforscroll( false );
            paintmessages();
         }
         if ( taste == ct_up  &&  marked > msg.begin() ) {
            marked--;
            checkforscroll( false );
            paintmessages();
         }
         if ( taste == ct_pos1  && marked > msg.begin() ) {
            marked=msg.begin();
            checkforscroll( false );
            paintmessages();
         }
         if ( taste == ct_ende  &&  marked+1 < msg.end() ) {
            marked = msg.end()-1;
            checkforscroll( false );
            paintmessages();
         }

         if ( mouseinrect ( x1 + 10, y1 + starty, x1 + xsize - 30, y1 + ysize - 40 )) {
            if ( mouseparams.taste == 1 ) {
                int ps = (mouseparams.y - (y1 + starty + 10)) / 20;
                if ( ps < dispnum && ps >= 0 ) {
                   MsgVec::iterator n = firstdisplayed + ps;
                   if ( n < msg.end() ) {
                      if ( n != marked ) {
                         marked = n;
                         paintmessages();
                         mstatus = 1;
                      }
                      if ( n == marked  &&  mstatus == 2 ) {
                         buttonpressed ( 1 );
                         mstatus = 0;
                      }
                   }
                }
            }

         }
         if ( mouseparams.taste == 0  && mstatus == 1 )
            mstatus = 2;

   } while ( !ok ); /* enddo */

}


void viewmessages ( char* title, const MessagePntrContainer& msg, bool editable, int md  )    // mode : 0 verschickte ; 1 empfangene
{
   tviewmessages vm ( msg );
   vm.init  ( title, editable, md );
   vm.run ();
   vm.done();
}








  class   tviewmessage : public tdialogbox, public tviewtextwithscrolling {
               public:
                   const char*          txt;
                   char                 ok;

                   char                 scrollbarvisible;
                   char                 action;
                   int                  textstart;
                   int                  from;
                   int                  cc;
                   int                  rightspace;

                   void                 init( const Message& msg );
                   virtual void         run ( void );
                   virtual void         buttonpressed( int id);
                   void                 redraw ( void );
                   int                  getcapabilities ( void ) { return 1; };
                   void                 repaintscrollbar( void );
                };



void         tviewmessage::init( const Message& msg )
{

   // dialogbox initialisieren

   tdialogbox::init();
   x1 = 20;
   y1 = 50;
   xsize = 500;
   ysize = 360;
   textstart = 42 + 45;
   textsizey = (ysize - textstart - 35);
   starty = starty + 10;
   title = "message";
   windowstyle ^= dlg_in3d;
   action=0;


   txt = msg.text.c_str();
   from = msg.from;
   cc = msg.to;


   setparams ( x1 + 13, y1 + textstart, x1 + xsize - 41, y1 + ysize - 35, txt, black, dblue);
   tvt_dispactive = 0;
   displaytext(  );
   textsizeycomplete = tvt_yp;
   tvt_dispactive = 1;


   if (textsizeycomplete >= textsizey) {
      scrollbarvisible = true;

      addscrollbar(xsize - 30,starty,xsize - 15,ysize - 35,&textsizeycomplete, textsizey, &tvt_starty,1,0);
      setscrollspeed ( 1 , 1 );

      rightspace = 40;
   }
   else {
      rightspace = 10;
      scrollbarvisible = false;
   }

   addbutton ( "~o~k", 10, ysize - 25, xsize - 10, ysize - 5, 0, 1, 4, true );

   tvt_starty = 0;
   buildgraphics();
}


void         tviewmessage::repaintscrollbar( void )
{
   enablebutton ( 1 );
}


void         tviewmessage::redraw(void)
{
  int i;

   tdialogbox::redraw();
   rahmen(true,x1 + 10,y1 + textstart - 2,x1 + xsize - rightspace,y1 + textstart + textsizey + 2);


   activefontsettings.font = schriften.smallarial;
   activefontsettings.color = black;
   activefontsettings.justify = lefttext;
   activefontsettings.background = dblue;
   activefontsettings.length =0;

   int yp = 36;
   if ( from != 1 << actmap->actplayer ) {
      showtext2 ( "from: ", x1 + 13, y1 + textstart - 45 );
      int n = log2 ( from );
      if ( n < 8 ) {
         activefontsettings.color = 20 + 8 * n;
         showtext2 ( actmap->player[n].getName().c_str(), x1 + 60, y1 + textstart - 45 );
      } else
         showtext2 ( "system", x1 + 60, y1 + textstart - 45 );

      yp = 26;
   }

   activefontsettings.color = black;
   if ( from != 1 << actmap->actplayer )
      showtext2 ( "cc: ", x1 + 13, y1 + textstart - yp );
   else
      showtext2 ( "to: ", x1 + 13, y1 + textstart - yp );

   int n = 0;
   for (i = 0; i < 8; i++ )
      if ( actmap->player[i].exist()  && i != actmap->actplayer )
         if ( cc & ( 1 << i ))
            n++;
   if ( n ) {
      activefontsettings.length = (xsize - (n+1) * 5 - 60 - rightspace ) / n;

      n = 0;
      for ( i = 0; i < 8; i++ )
         if ( actmap->player[i].exist()  && i != actmap->actplayer )
            if ( cc & ( 1 << i )) {
               activefontsettings.color = 20 + 8 * i;
               //activefontsettings.background = 17 + 8 * i;
               showtext2 ( actmap->player[i].getName().c_str(), x1 + 60 + n * activefontsettings.length + n * 5 , y1 + textstart - yp );
               n++;
            }
   }
   activefontsettings.color = black;
   activefontsettings.height = textsizey / linesshown;

   rahmen(true,x1 + 10,y1 + textstart - 47,x1 + xsize - rightspace,y1 + textstart - 7 );

   int xd = 15 - (rightspace - 10) / 2;

   setpos ( x1 + 13 + xd, y1 + textstart, x1 + xsize - 41 + xd, y1 + ysize - 35 );
   displaytext();

}


void         tviewmessage::buttonpressed( int id )
{
   tdialogbox::buttonpressed(id);
   if (id == 1)
      displaytext();

   if (id == 4)
      action = 12;
}


void         tviewmessage::run(void)
{
   mousevisible(true);
   do {
      tdialogbox::run();
      checkscrolling ( );
      if (taste == ct_esc || taste == ct_enter || taste == ct_space )
         action = 11;
   }  while (action < 10);
}


class IngameMessageViewer : public ASC_PG_Dialog {

   
   public:
      IngameMessageViewer ( const ASCString& title, const Message& msg, const ASCString& buttonText = "ok" ) : ASC_PG_Dialog ( NULL, PG_Rect( 50, 50, 500, 400 ), title )
      {
         PG_Rect r;
         if ( !msg.getFromText( actmap ).empty()  ) {
            PG_Rect f( 10, 40, Width() - 20, 25 );

            new PG_Label( this, PG_Rect( f.x, f.y, 50, f.h ), "From:");
            Emboss* emb = new Emboss( this, PG_Rect( f.x + 50, f.y, f.w - 50, f.h ), true );
            new PG_Label( emb, PG_Rect( emb->x +2, emb->y + 2, emb->w - 4, emb->h - 4), msg.getFromText( actmap ) );
   
            
            r = PG_Rect ( 10, 75, Width() - 20, Height() - 125 );
         } else
            r = PG_Rect ( 10, 40, Width() - 20, Height() - 90 );
            
         new Emboss( this, r, true );
         PG_RichEdit* re = new PG_RichEdit( this, PG_Rect(r.x + 2, r.y+2, r.w-4, r.h-4));

         ASCString text  = msg.text;
         while ( text.find ( "#crt#" ) != ASCString::npos )
            text.replace ( text.find  ("#crt#"), 5, " \n");

         re->SetText( text );
         re->SetTransparency(255);

         
         PG_Button* b = new PG_Button( this, PG_Rect( Width() - 110, Height() - 40, 100, 30), buttonText );
         b->sigClick.connect( SigC::slot( *this, &IngameMessageViewer::QuitModal) );
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
         return ASC_PG_Dialog::eventKeyDown( key );
      }

      
};


void viewmessage ( const Message& message )
{
   if ( legacyEventSystemActive() ) {
   
      tviewmessage vm;
      vm.init ( message );
      vm.run();
      vm.done();
   } else {
      IngameMessageViewer igm( "incoming message...", message );
      igm.Show();
      igm.RunModal();
   }
   
}



void checkJournal( Player& player )
{
   GameMap* actmap = player.getParentMap();
   
   if ( actmap->lastjournalchange.abstime )
      if ( (actmap->lastjournalchange.turn() == actmap->time.turn() ) ||
            (actmap->lastjournalchange.turn() == actmap->time.turn()-1  &&  actmap->lastjournalchange.move() > actmap->actplayer ) )
               viewjournal();
}

void viewjournal ( void )
{
   if ( !actmap->gameJournal.empty() ) {
      tviewanytext vat;
      vat.init ( "journal", actmap->gameJournal.c_str() );
      vat.run();
      vat.done();
   }
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

   static bool isRunning = false;
   if ( isRunning )
      return;

   VariableLocker l( isRunning );
   
   MessagePntrContainer::iterator mi = player.unreadmessage.begin();
   while ( mi != player.unreadmessage.end()  ) {
      Message* msg = *mi;
      player.oldmessage.push_back ( *mi );
      player.unreadmessage.erase ( mi );
      viewmessage ( *msg );
      mi = player.unreadmessage.begin();
   }
}




