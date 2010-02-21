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

#include <pglistboxitem.h>
#include "../gamemap.h"
#include "fieldmarker.h"
#include "../mapdisplay.h"
#include "../overviewmappanel.h"
#include "../spfst.h"
#include "../mainscreenwidget.h"

class CoordinateItem : public PG_ListBoxItem {
   ASCString toString( const MapCoordinate& pos ) {
      ASCString s;
      s.format( "%d / %d", pos.x, pos.y );
      return s;
   };
   MapCoordinate p;
   
   public:
      CoordinateItem( PG_Widget *parent, const MapCoordinate& pos ) : PG_ListBoxItem( parent, 20, toString( pos ) ), p(pos) {};
      MapCoordinate getPos() const { return p; };
};


SelectFromMap::SelectFromMap( CoordinateList& list, GameMap* map, bool justOne, bool readOnly ) : ASC_PG_Dialog( NULL, PG_Rect( PG_Application::GetScreenWidth() - 150, PG_Application::GetScreenHeight() - 300, 150, 300 ), "Select Fields" ), listbox(NULL), actmap( map ), coordinateList (list)
{
   this->justOne = justOne;
   this->readOnly = readOnly;

   
   listbox = new PG_ListBox( this, PG_Rect( 10, 30, 130, readOnly ? 210 : 180 ));
   listbox->sigSelectItem.connect( SigC::slot( *this, &SelectFromMap::listItemClicked ));

   if ( !readOnly ) {
      PG_Button* m = new PG_Button ( this, PG_Rect( 10, 230, 130, 20 ), "mark (~space~)");
      m->sigClick.connect( SigC::slot( *this, &SelectFromMap::mark ));
   }


   PG_Button* b = new PG_Button ( this, PG_Rect( 10, 270, 130, 20 ), "~O~K");
   b->sigClick.connect( SigC::slot( *this, &SelectFromMap::closeDialog ));

   omp = getMainScreenWidget()->getOverviewMapPanel();
   md  = getMainScreenWidget()->getMapDisplay();
   md->mouseButtonOnField.connect( SigC::slot( *this, &SelectFromMap::markField2 ));
   oldprio = md->setSignalPriority( 2 );
   

   showFieldMarking( coordinateList );
   if ( !list.empty() ) {
      md->cursor.goTo( *list.begin() );
      actmap->getCursor() =  *list.begin();
      cursorMoved();
   }

   updateList();
};

   
bool SelectFromMap::ProcessEvent ( const SDL_Event *   event,bool   bModal   )
{
   if ( md->ProcessEvent( event, bModal ) )
      return true;

   if ( omp && omp->ProcessEvent( event, bModal ) )
      return true;

   return ASC_PG_Dialog::ProcessEvent( event, bModal );
}

bool SelectFromMap::accept( const MapCoordinate& pos ) {
   return !readOnly && actmap->getField( pos ) != NULL;
};

void SelectFromMap::showFieldMarking( const CoordinateList& coordinateList )
{
   actmap->cleartemps( 7 );
   for ( CoordinateList::const_iterator i = coordinateList.begin(); i != coordinateList.end(); ++i ) {
      MapField* fld = actmap->getField( *i );
      if ( fld )
         fld->a.temp = 1;
   }
   repaintMap();
}

bool SelectFromMap::markField2( const MapCoordinate& pos, const SPoint& mouse, bool cursorChanged, int button, int prio )
{
   if ( prio <= 2 )
      markField( pos );
   return true;
}


bool SelectFromMap::markField( const MapCoordinate& pos )
{
   if  ( !accept(pos))
      return false;

   if( justOne ) {
      coordinateList.clear();
      coordinateList.push_back ( pos );
   } else {
      CoordinateList::iterator i = find( coordinateList.begin(), coordinateList.end(), pos );
      if ( i == coordinateList.end() ) {
         coordinateList.push_back ( pos );
      } else {
         coordinateList.erase ( i );
      }
   }
   showFieldMarking( coordinateList );
   updateList();
   return true;
}


bool SelectFromMap::mark()
{
   MapCoordinate pos = actmap->getCursor();
   return markField( pos );
}

bool SelectFromMap::eventKeyDown (const SDL_KeyboardEvent *key)
{
   if ( key->type == SDL_KEYDOWN ) {
      if ( key->keysym.sym == SDLK_SPACE  ) {
         mark();
         return true;
      }
      if ( key->keysym.sym == SDLK_RETURN || key->keysym.sym == SDLK_KP_ENTER ) {
         if ( isOk() )
            QuitModal();
         return true;
      }
   }
   return ASC_PG_Dialog::eventKeyDown( key );
}

void SelectFromMap::updateList()
{
   listbox->DeleteAll();
   for ( CoordinateList::iterator i = coordinateList.begin(); i != coordinateList.end(); ++i )
      new CoordinateItem( listbox, *i );

   listbox->Show();
}

bool SelectFromMap::listItemClicked( PG_ListBoxBaseItem* item )
{
   if ( item ) {
      CoordinateItem* i = dynamic_cast<CoordinateItem*>(item);
      if ( i ) {
         md->cursor.goTo( i->getPos() );
         actmap->getCursor() =  i->getPos();
         cursorMoved();
      }
   }
   return true;
}



void SelectFromMap::Show( bool fade)
{
   getMainScreenWidget()->BringToFront();
   md->BringToFront();
   md->Show();
   ASC_PG_Dialog::Show( fade );
   repaintMap();
}

SelectFromMap::~SelectFromMap()
{
   md->setSignalPriority( oldprio );
         
   getMainScreenWidget()->SendToBack();
   actmap->cleartemps(7);
   repaintMap();
}


bool SelectBuildingFromMap::accept( const MapCoordinate& pos ) {
   return actmap->getField( pos ) && actmap->getField( pos )->building;
};

bool SelectUnitFromMap::accept( const MapCoordinate& pos ) {
   return actmap->getField( pos ) && actmap->getField( pos )->vehicle;
};

