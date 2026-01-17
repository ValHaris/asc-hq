/*
    This file is part of Advanced Strategic Command; http://www.asc-hq.de
    Copyright (C) 1994-2010  Martin Bickel

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


#include "pgprogressbar.h"
#include "../itemrepository.h"
#include "../paradialog.h"
#include "../gamemap.h"
#include "../events.h"
#include "../widgets/textrenderer.h"


class ResearchInfo : public ASC_PG_Dialog {
   public:
       ResearchInfo( const Player& player ) : ASC_PG_Dialog( NULL, PG_Rect(-1, -1, 500,500), "Research Status" ), player(player)
       {
            PG_ProgressBar* progress = new PG_ProgressBar(this, PG_Rect(20, 70, Width()-40, 30));
            progress->SetProgress(0);

            int rowHeight = 25;

            new PG_Label(this, PG_Rect(20, 110, 200, 25), "Name:" );
            new PG_Label(this, PG_Rect(20, 110+1*rowHeight, 200, 25), "Research points total:" );
            new PG_Label(this, PG_Rect(20, 110+2*rowHeight, 200, 25), "Research points completed:" );
            new PG_Label(this, PG_Rect(20, 110+3*rowHeight, 200, 25), "Research points remaining:" );
            new PG_Label(this, PG_Rect(20, 110+4*rowHeight, 200, 25), "Research points gained per turn:" );
            new PG_Label(this, PG_Rect(20, 110+5*rowHeight, 200, 25), "Time left (turns):" );
            new PG_Label(this, PG_Rect(20, 110+6*rowHeight, 200, 25), "Technologies completed:" );

            const Research& res = player.research;

            if ( res.activetechnology ) {
                if (res.activetechnology->researchpoints != 0)
                    progress->SetProgress(100*double(res.progress) / double(res.activetechnology->researchpoints));

                int rp = res.activetechnology->researchpoints;
                int prog = res.progress;

                new PG_Label(this, PG_Rect(250, 110, 200, 25), res.activetechnology->name );
                new PG_Label(this, PG_Rect(250, 110+1*rowHeight, 200, 25), ASCString::toString(res.activetechnology->researchpoints) );
                new PG_Label(this, PG_Rect(250, 110+2*rowHeight, 200, 25), ASCString::toString(res.progress) );
                new PG_Label(this, PG_Rect(250, 110+3*rowHeight, 200, 25), ASCString::toString(res.activetechnology->researchpoints-res.progress) );
                new PG_Label(this, PG_Rect(250, 110+5*rowHeight, 200, 25), ASCString::toString(res.currentTechAvailableIn()) );

            }
            new PG_Label(this, PG_Rect(250, 110+4*rowHeight, 200, 25), ASCString::toString(res.getResearchPerTurn() ));

            TextRenderer* pr = new TextRenderer( this, PG_Rect( 250, 110+6*rowHeight, Width() - 20, Height()-50));
            pr->SetText( getTechs(res) );


             PG_Button* ok = new PG_Button( this, PG_Rect( Width() - 100, Height() - 30, 90, 20 ), "OK" );
             ok->sigClick.connect( sigc::hide( sigc::mem_fun( *this, &ResearchInfo::QuitModal )));

             PG_Button* ta = new PG_Button( this, PG_Rect( 30, Height() - 30, 60, 20 ), "TechAdapters" );
             ta->sigClick.connect( sigc::hide( sigc::mem_fun( *this, &ResearchInfo::ShowAdapters)));

          }

   private:
       const Player& player;
       ASCString getTechs(const Research& res) {
           vector<ASCString> sv;
           for ( vector<int>::const_iterator i = res.developedTechnologies.begin(); i != res.developedTechnologies.end(); ++i ) {
              Technology* t = technologyRepository.getObject_byID( *i );
              if ( t )
                 sv.push_back ( t->name );
           }
           sort ( sv.begin(), sv.end() );

           ASCString s;
           for ( vector<ASCString>::iterator i = sv.begin(); i != sv.end(); ++i )
              s += *i + "\n";
           return s;
       }

       bool ShowAdapters() {
		  ASCString s = player.research.listTriggeredTechAdapter();
		  ViewFormattedText vft ( "Triggered TechAdapter", s, PG_Rect( -1, -1, 500, 500 ));
		  vft.Show();
		  vft.RunModal();
		  return true;
       }
    };




void researchinfo ( const Player& player )
{
	ResearchInfo ri(player);
	ri.Show();
	ri.RunModal();
}
