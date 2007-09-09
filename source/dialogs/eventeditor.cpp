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


#include <sstream>
#include <pgimage.h>

#include <pgpropertyeditor.h>
#include <pgpropertyfield_integer.h>

#include "eventeditor.h"
#include "../paradialog.h"
#include "../gamemap.h"
#include "../spfst.h"
#include "../gameeventsystem.h"

#include "../widgets/dropdownselector.h"
#include "../widgets/multilistbox.h"

class EventEditor : public ASC_PG_Dialog {
   private:
      vector<ASCString> triggerNames;
      vector<ASCString> actionNames;

      DropDownSelector* eventType;
      DropDownSelector* triggerConnection;
      PG_LineEdit* description;
      Event* event;
      bool result;
      PG_ListBox* playerlistbox;
      PG_PropertyEditor* properties;

      bool setupEvent()
      {
         if ( event->action )
            event->action->setup();
         return true;
      }

      void actionSelected( int i )
      {
         if ( i < 0 )
            return;

         EventActionID eai = actionFactory::Instance().getID( actionNames.at(i) );

         if ( !event->action )
            event->spawnAction( eai );
         else
            if ( event->action->getActionID() != eai ) {
               delete event->action;
               event->action = NULL;
               event->spawnAction( eai );
            }

         event->action->setup();
      }

      bool setupTrigger( int num )
      {
         if ( event->trigger.size() > num )
            if ( event->trigger[num] )
               event->trigger[num]->setup();
         
         return true;
      }

      void triggerSelected( int type, int num )
      {
         if ( type>= 0 ) {
            EventTriggerID triggerID = triggerFactory::Instance().getID(  triggerNames.at(type) );

            if ( event->trigger.size() <= num ) {
               event->trigger.resize(num+1);
               event->trigger[num] = event->spawnTrigger( triggerID );
               event->trigger[num]->setup();
            } else {
               if ( event->trigger[num]->getTriggerID() == triggerID )
                  event->trigger[num]->setup();
               else {
                  delete event->trigger[num];
                  event->trigger[num] = event->spawnTrigger( triggerID );
                  event->trigger[num]->setup();
               }
            }
         }
      }
      
      bool invertTrigger( bool invert, int e )
      {
         if ( event->trigger.size() > e ) {
            event->trigger[e]->invert = invert;
            return true;
         } else
            return false;
      }
      

      bool ok()
      {
         if ( !properties->Apply() )
            return false;

         if ( event->trigger.size() < 1 ) {
            warning( "no trigger defined !" );
            return false;
         }
         
         if ( !event->action ) {
            warning ( "no action defined !" );
            return false;
         }
         if ( event->reArmNum && event->delayedexecution.turn < 1 ) {
            warning ( "without delayed execution, reArming the event makes no sense !" );
            return false;
         }

         event->description = description->GetText();

         if ( triggerConnection->GetSelectedItemIndex() == 0 )
            event->triggerConnection = Event::AND;
         else
            event->triggerConnection = Event::OR;

         event->playerBitmap = 0;
         for ( int i = 0; i < playerlistbox->GetWidgetCount(); ++i ) {
            PG_ListBoxBaseItem* bi = dynamic_cast<PG_ListBoxBaseItem*>(playerlistbox->FindWidget(i));
            if ( bi && bi->IsSelected() )
               event->playerBitmap |= 1 << i;
         }

         result = true;
         QuitModal();
         
         return true;
      }
      
   public:
      EventEditor( Event* event ) : ASC_PG_Dialog( NULL, PG_Rect( -1, -1, 700, 500 ), "Edit Event" ), result(false)
      {
         this->event = event;

         int labelWidth = 80;
         int ypos = 30;
         
         new PG_Label( this, PG_Rect( 10, ypos, labelWidth, 25 ), "Action:" );

         triggerNames = triggerFactory::Instance().getNames();
         actionNames = actionFactory::Instance().getNames();

         eventType = new DropDownSelector( this, PG_Rect( labelWidth+30, ypos, 300, 25 ), actionNames );
         if ( event->action ) 
            for ( int i = 0; i < actionNames.size(); ++i )
               if ( event->action->getName() == actionNames[i] )
                  eventType->SelectItem ( i );

         eventType->selectionSignal.connect( SigC::slot( *this, &EventEditor::actionSelected ));
         (new PG_Button( this, PG_Rect( labelWidth + 50 + 300, ypos, 100, 25 ), "setup"))->sigClick.connect( SigC::slot( *this, &EventEditor::setupEvent ));
         ypos += 40;

         new PG_Label( this, PG_Rect( 10, ypos, labelWidth, 25 ), "Description:" );
         description = new PG_LineEdit( this, PG_Rect( labelWidth+30, ypos, 420, 25 ) );
         description->SetText( event->description );
         ypos += 40;

         for ( int e = 0; e < 4; ++e ) {
            new PG_Label( this, PG_Rect( 10, ypos, labelWidth, 25), "Trigger " + ASCString::toString(e));
            
            DropDownSelector* trigger = new DropDownSelector( this, PG_Rect( labelWidth+30, ypos, 200,25 ), triggerNames );
                 
            (new PG_Button( this, PG_Rect( labelWidth+50+200, ypos, 100, 25 ), "setup"))->sigClick.connect( SigC::bind( SigC::slot( *this, &EventEditor::setupTrigger ), e));
            
            PG_CheckButton* inv = new PG_CheckButton( this, PG_Rect( labelWidth+70+300, ypos, 100, 25 ), "invert");
            inv->sigClick.connect( SigC::bind( SigC::slot( *this, &EventEditor::invertTrigger ), e));
            if ( event->trigger.size() > e ) {
               if ( event->trigger[e]->invert )
                  inv->SetPressed();

               for ( int i = 0; i < triggerNames.size(); ++i )
                  if ( event->trigger[e]->getName() == triggerNames[i] )
                     trigger->SelectItem ( i );
            } else
               for ( int i = 0; i < triggerNames.size(); ++i )
                  if ( triggerNames[i] == "Nothing (always false)"  )
                     trigger->SelectItem ( i );
               
            trigger->selectionSignal.connect( SigC::bind( SigC::slot( *this, &EventEditor::triggerSelected), e ));
            ypos += 40;
         }

         new PG_Label( this, PG_Rect( 10, ypos, labelWidth, 25 ), "Players:" );
         playerlistbox = (new MultiListBox( this, PG_Rect( labelWidth+30, ypos, 150, 160 )))->getListBox();
         for ( int i = 0; i < event->getMap()->getPlayerCount(); ++i ) {
            PG_ListBoxItem* item;
            if ( actmap->player[i].exist() )
               item = new PG_ListBoxItem( playerlistbox, 20, event->getMap()->player[i].getName() );
            else
               item = new PG_ListBoxItem( playerlistbox, 20, "inactive player " + ASCString::toString(i) );
            
            if ( event->playerBitmap & (1 << i))
               item->Select();
         }

         properties = new PG_PropertyEditor( this, PG_Rect( labelWidth + 80 + 160, ypos, 150, 160 ));
         new PG_PropertyField_Integer<int>( properties, "Rearm Num", &event->reArmNum );
         new PG_PropertyField_Integer<int>( properties, "Offset Turn", &event->delayedexecution.turn );
         new PG_PropertyField_Integer<int>( properties, "Offset Move", &event->delayedexecution.move );
         ypos += 200;
         

         new PG_Label( this, PG_Rect( 10, ypos, labelWidth, 25 ), "Logic:" );
         static const char* connectionNames[2] = {"all trigger must be triggered", "only one trigger must be triggered" };
         triggerConnection = new DropDownSelector( this, PG_Rect( labelWidth+30, ypos, 300, 25 ), 2, connectionNames );
         if ( event->triggerConnection == Event::AND )
            triggerConnection->SelectItem( 0 );
         else
            triggerConnection->SelectItem( 1 );
         ypos += 40;

         
         AddStandardButton ( "Cancel" )->sigClick.connect( SigC::slot( *this, &EventEditor::QuitModal ));
         AddStandardButton ( "OK" )->sigClick.connect( SigC::slot( *this, &EventEditor::ok ));
         
      };

      bool GetResult()
      {
         return result;
      }
};





bool   createevent( Event* event ) 
{
   EventEditor ee ( event );
   ee.Show();
   ee.RunModal();
   return ee.GetResult();
}

// õS EventSel


class EventList : public ASC_PG_Dialog {
   private:
      bool ButtonNew()
      {
         Event* ev = new Event(*actmap);
         if ( !createevent( ev ) ) {
            delete ev;
            return true;
         }

         actmap->events.push_back( ev );

         updateListbox();
         return true;
      }

      bool ButtonDelete()
      {
         int marked = listbox->GetSelectedIndex();
         if ( marked < 0 )
            return false;
         
         GameMap::Events::iterator e = actmap->events.begin();
         for ( int t = 0; t < marked && e != actmap->events.end(); t++ )
            ++e;

         if ( e != actmap->events.end() ) {
            delete *e;
            actmap->events.erase ( e );
            updateListbox();
         }
         
         return true;
      }

      bool ButtonEdit()
      {
         if ( actmap->events.size() ) {
            tmemorystreambuf buf;
            

            int marked = listbox->GetSelectedIndex();
            if ( marked < 0 )
               return false;
            
            GameMap::Events::iterator e = actmap->events.begin();
            for ( int t = 0; t < marked && e != actmap->events.end(); t++ )
               ++e;

            {
               tmemorystream stream ( &buf, tnstream::writing );
               (*e)->write ( stream );
            }

            if ( ! createevent( *e ) ) {
                     // cancel pressed, we are restoring the original event
               tmemorystream stream ( &buf, tnstream::reading );
               (*e)->read ( stream );
            }

            updateListbox();
            return true;
         } else
            return false;
         
      }
      
      bool ButtonOK()
      {
         QuitModal();
         return true;
      }
      
      PG_ListBox* listbox;


      void updateListbox()
      {
         listbox->DeleteAll();
         for ( GameMap::Events::iterator i = actmap->events.begin(); i != actmap->events.end(); ++i ) {
            ASCString text = (*i)->action->getName();
            text += " - " + (*i)->description;
            
            new PG_ListBoxItem( listbox, 20, text );
         }
         listbox->Update();
      }

      
      
   public:
      EventList() : ASC_PG_Dialog( NULL, PG_Rect( -1, -1, 600, 400 ), "Edit Events" )
      {
         int w = 500;
         PG_Button* b = new PG_Button( this, PG_Rect ( w, 40, 90, 25 ), "~N~ew" );
         b->sigClick.connect( SigC::slot( *this, &EventList::ButtonNew ));

         b = new PG_Button( this, PG_Rect ( w, 80, 90, 25 ), "~E~dit" );
         b->sigClick.connect( SigC::slot( *this, &EventList::ButtonEdit ));
         
         b = new PG_Button( this, PG_Rect ( w, 120, 90, 25 ), "~D~elete" );
         b->sigClick.connect( SigC::slot( *this, &EventList::ButtonDelete ));
         
         b = new PG_Button( this, PG_Rect ( w, 160, 90, 25 ), "~O~K" );
         b->sigClick.connect( SigC::slot( *this, &EventList::ButtonOK ));
         

         listbox = new PG_ListBox( this, PG_Rect( 20, 40, w - 30, Height()-50 ));
         listbox->SetTransparency(255);
         updateListbox();
      };

};


void    eventEditor(void)
{
   EventList    te;
   te.Show();
   te.RunModal();
}


