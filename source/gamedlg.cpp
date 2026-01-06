/*! \file gamedlg.cpp    
    \brief Tons of dialog boxes which are used in ASC only (and not in the mapeditor)
*/
/*
    This file is part of Advanced Strategic Command; http://www.asc-hq.de
    Copyright (C) 1994-2010  Martin Bickel  and  Marc Schellenberger

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

#include <stdio.h>                        
#include <cstring>
#include <ctype.h>
#include <stdlib.h>
#include <math.h>

#if TIME_WITH_SYS_TIME
# include <sys/time.h>
# include <time.h>
#else
# if HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif


#include "vehicletype.h"
#include "buildingtype.h"

#include "basegfx.h"
#include "gamedlg.h"
#include "stack.h"
#include "controls.h"
#include "gameoptions.h"
#include "errors.h"
#include "viewcalculation.h"
#include "itemrepository.h"
#include "strtmesg.h"
#include "dialogs/editmapparam.h"
#include "dlg_box.h"
#include "spfst.h"
#include "dialog.h"
#include "actions/transfercontrolcommand.h"
#include "widgets/textrenderer.h"
#include "widgets/playerselector.h"
#include "sg.h"
#include "spfst-legacy.h"
#include "pglabel.h"

class ResourceBlock : public PG_Widget {
    PG_Widget* label[Resources::count];
    bool editable;
public:
    ResourceBlock(PG_Widget* parent, const PG_Rect& size, Resources resources, bool editable = false, const ASCString& postfix = "") : PG_Widget(parent, size), editable(editable) {
        const int height = 22;
        const int border = 2;
        const int left = Width() * 33 / 100;
        const int right = Width() * 66 / 100;
        ASCString p = postfix.length()? ASCString(" " + postfix) : "";
        for ( int i = 0; i < Resources::count; ++i) {
            int y = border + i * (height + border);
            if ( editable ) {
                PG_LineEdit* l = new PG_LineEdit(this, PG_Rect(border, y, left-2*border, height));
                l->SetText(ASCString::toString(resources.resource(i)));
                l->SetValidKeys("0123456789");
                label[i] = l;
            } else {
                PG_Label* l = new PG_Label(this, PG_Rect(border, y, left-2*border, height), ASCString::toString(resources.resource(i)));
                l->SetAlignment(PG_Label::RIGHT);
                label[i] = l;
            }
            new PG_Label(this, PG_Rect(left+border, y, right-2*border, height ), Resources::name(i) + p);
        }
    }

    Resources get() {
        Resources r;
        if ( editable )
            for ( int i = 0; i < Resources::count; ++i) {
                char* p;
                ASCString text = label[i]->GetText();
                if ( text.length( )) {
                    long converted = strtol(text.c_str(), &p, 10);
                    if (!*p)
                        r.resource(i) = converted;
                }
            }
        return r;
    }

    void update(const Resources& res) {
        for ( int i = 0; i < Resources::count; ++i)
            label[i]->SetText(ASCString::toString(res.resource(i)));
    }
};

class TributePayments : public ASC_PG_Dialog {
    GameMap* gameMap;
    ResourceBlock* outgoingTransferred;
    ResourceBlock* incomingDue;
    ResourceBlock* incomingTransferred;
    ResourceBlock* outgoingOpen[GameMap::maxTotalPlayers];

    bool apply() {
        for ( int p = 0; p < GameMap::maxTotalPlayers; ++p)
            gameMap->tribute.avail[gameMap->actplayer][p] = outgoingOpen[p]->get();
        QuitModal();
        return true;
    }

    bool selectPlayer(PG_ListBoxBaseItem* item ) {
        PlayerSelector::Item* i = dynamic_cast<PlayerSelector::Item*>(item);
        if ( i ) {
            int player = i->getData();
            outgoingTransferred->update(gameMap->tribute.paid[player][gameMap->actplayer]);
            incomingDue->update(gameMap->tribute.avail[player][gameMap->actplayer]);
            incomingTransferred->update(gameMap->tribute.paid[gameMap->actplayer][player]);
            for ( int p = 0; p< GameMap::maxTotalPlayers; ++p)
                if (p != player)
                    outgoingOpen[p]->Hide();
            outgoingOpen[player]->Show();
        }
        return true;
    }

public:
    TributePayments (GameMap* gamemap) : ASC_PG_Dialog(NULL, PG_Rect(-1, -1, 640, 430), "Transfer Resources"), gameMap(gamemap)
    {
        PlayerSelector* selector = new PlayerSelector(this, PG_Rect(10, 30, 170, 300), gamemap, false, 1 <<gamemap->actplayer, 5 );
        selector->sigSelectItem.connect( sigc::mem_fun(*this, &TributePayments::selectPlayer));

        Emboss* outgoing = new Emboss(this, PG_Rect(190, 30, 430, 150), true);
        new PG_Label(outgoing, PG_Rect(5,5,100,20), "Outgoing");
        for ( int i = 0; i < GameMap::maxTotalPlayers; ++i) {
            outgoingOpen[i] = new ResourceBlock(outgoing, PG_Rect(  5, 30,180,100),gamemap->tribute.avail[gamemap->actplayer][i], true);
            outgoingOpen[i]->Hide();
        }
        outgoingTransferred = new ResourceBlock(outgoing, PG_Rect(190, 30,230,100),Resources(), false, "transferred");

        Emboss* incoming = new Emboss(this, PG_Rect(190, 220, 430, 150), true);
        new PG_Label(incoming, PG_Rect(5,5,100,20), "Incoming");
        incomingDue         = new ResourceBlock(incoming, PG_Rect(  5,30,180,100),Resources(), false, "due");
        incomingTransferred = new ResourceBlock(incoming, PG_Rect(190,30,230,100),Resources(), false, "transferred");

        StandardButtonDirection(Horizontal);
        AddStandardButton("Cancel")->sigClick.connect( sigc::hide( sigc::mem_fun( *this, &TributePayments::QuitModal )));
        AddStandardButton("OK")->sigClick.connect( sigc::hide( sigc::mem_fun( *this, &TributePayments::apply )));
    }
};


void settributepayments ( void )
{
    TributePayments tp(actmap);
    tp.Show();
    tp.RunModal();
}




ASCString writeGameParametersToString ( GameMap* actmap )
{
   ASCString s = "The game has been set up with the following game parameters:\n";
   s += "(black line: parameter has default value)\n\n";
   for ( int i = 0; i< gameparameternum; i++ ) {
   
      int d = actmap->getgameparameter(GameParameter(i)) != gameParameterSettings[i].defaultValue;

      if ( d )
         s+= "#color4#";

      s += gameParameterSettings[i].longName;
      s += " = ";
      s += strrr ( actmap->getgameparameter(GameParameter(i)) );

      if ( d )
         s+= "#color0#";
      s += "\n";
   }
   return s;
}

void sendGameParameterAsMail ( GameMap* actmap )
{
   ASCString s = writeGameParametersToString ( actmap );

   if ( actmap->getgameparameter( cgp_superVisorCanSaveMap) ) {
      s = "#color4#Warning ! This game has been started with the option enabled that allows the Supervisor to save the map. This will allow him to spy on everything. \n"
           "If you did not initiate this game yourself, don't play with this option unless you were specifically informed why this option had to be enabled. \n"
           "#color0# \n" + s;
   }

   new Message ( s, actmap, 255 );
}


void showGameParameters ( void )
{
   ViewFormattedText vft( "Game Parameters", writeGameParametersToString(actmap), PG_Rect( -1, -1, 500, 550 ) );
   vft.Show();
   vft.RunModal();
}


class TransferControlDialog : public ASC_PG_Dialog {

    ContainerBase* unit;
    int receiving_player;
    const Context& context;

    bool apply() {
        if ( receiving_player >= 0) {

            auto_ptr<TransferControlCommand> tcc ( new TransferControlCommand( unit ));
                 tcc->setReceiver( &unit->getMap()->getPlayer(receiving_player) );
                 ActionResult res = tcc->execute( context );
                 if ( res.successful() ) {
                    tcc.release();
                    QuitModal();
                 } else {
                    displayActionError(res );
                 }
        }
        return true;
    }

    bool selectPlayer(PG_ListBoxBaseItem* item ) {
        PlayerSelector::Item* i = dynamic_cast<PlayerSelector::Item*>(item);
        if ( i )
            receiving_player = i->getData();

        return true;
    }

public:
    TransferControlDialog (ContainerBase* unit, const Context& context) : ASC_PG_Dialog(NULL, PG_Rect(-1, -1, 250, 300), "Transfer Resources"), unit(unit), receiving_player(-1), context(context)
    {
        TransferControlCommand tcc( unit );
        TransferControlCommand::Receivers receivers = tcc.getReceivers();
        Uint8 allowed_players = 0;
        for ( TransferControlCommand::Receivers::iterator i = receivers.begin(); i != receivers.end();++i )
            allowed_players |= 1 << (*i)->getPosition();

        PlayerSelector* selector = new PlayerSelector(this, PG_Rect(10, 30, Width()-20, Height() - 80), unit->getMap(), false, 0xff ^ allowed_players, 5 );
        selector->sigSelectItem.connect( sigc::mem_fun(*this, &TransferControlDialog::selectPlayer));

        StandardButtonDirection(Horizontal);
        AddStandardButton("Cancel")->sigClick.connect( sigc::hide( sigc::mem_fun( *this, &TransferControlDialog::QuitModal )));
        AddStandardButton("OK")->sigClick.connect( sigc::hide( sigc::mem_fun( *this, &TransferControlDialog::apply )));
    }
};



void giveunitaway ( MapField* fld, const Context& context )
{
   ContainerBase* c = fld->getContainer();

   int not_avail = TransferControlCommand::not_avail_reason( c );

   if ( c->getOwner() != c->getMap()->actplayer ) {
      dispmessage2( 451, NULL );
      return;
   }
   if ( not_avail > 0 ) {
      dispmessage2( not_avail, NULL );
      return;
   }

   TransferControlDialog tcd(c, context);
   tcd.Show();
   tcd.RunModal();
}

void showPlayerTime()
{
   ASCString text;

   int p = actmap->actplayer;
   for ( int i = 0; i < 8; i++ ) {
       if ( actmap->player[p].exist() && actmap->player[p].playTime.size() ) {
          text += actmap->player[p].getName();
          text += " completed turn no. ";
          text += strrr(actmap->player[p].playTime.rbegin()->turn);
          text += " using ASC ";
          text += getVersionString( actmap->player[p].ASCversion );
          text += "\non ";
          text += asctime ( localtime ( &actmap->player[p].playTime.rbegin()->date));
          text += "\n";
       }


       ++p;
       if ( p >= 8 )
          p = 0;
   }

   ViewFormattedText vft( "Play Time Information", text, PG_Rect( -1, -1 , 500, 400 ));
   vft.Show();
   vft.RunModal();
}

void displaywindspeed( void )
{
#ifdef WEATHERGENERATOR
   displaymessage2("wind speed is %d which equals %s fields / turn", actmap->weatherSystem->getCurrentWindSpeed(), strrrd8d ( actmap->weatherSystem->getCurrentWindSpeed() * maxwindspeed / 256 ));
#else
   displaymessage2("wind speed is %d which equals %s fields / turn", actmap->weather.windSpeed, strrrd8d ( actmap->weather.windDirection * maxwindspeed / 256 ));
#endif
}

