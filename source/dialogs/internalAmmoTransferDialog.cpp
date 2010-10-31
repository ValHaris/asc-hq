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

#include <sigc++/sigc++.h>
#include <sstream>

#include "internalAmmoTransferDialog.h"

// #include "../unitctrl.h"
#include "../containercontrols.h"
#include "../gameoptions.h"
#include "../actions/servicing.h"
#include "../iconrepository.h"
#include "../replay.h"
#include "../actions/internalammotransfercommand.h"
#include "../sg.h"
#include "../dialog.h"


/** !!! wichtig: serviceweapon in dieser klasse bezieht sich auf das refuel-flag 
	* der waffe, _nicht_ auf das servicewaffen flag.
	*/
class InternalAmmoTransferWidget;
class InternalAmmoTransferHandler
{
	
	int serviceWeaponAmmoInTransfer[ weaponTypeNum ]; 
	int weaponAmmo[ 16 ]; // weapon ammo when done; might need to have *AmmoInTransfer added
	int weaponAmmoInTransfer[ 16 ];
	int weaponAmmoTransferSlot[ 16 ];
	std::vector<InternalAmmoTransferWidget*> widgets;
	
	bool weaponAmmoTransferable[ 16 ];
	bool serviceweaponAmmoTransferable[ weaponTypeNum ];
	int equalWeaponsTypeCounter;
	
	Vehicle* _vehicle;
	
	public:
		InternalAmmoTransferHandler( Vehicle* vehicle );
		bool isAmmoTransferable( int weaponID );
		int* weaponAmmoTransferSource( int weaponID );
		int* weaponAmmoTransferBuffer( int weaponID );
		int getMaxAmmo( int weaponID );
		std::string getName( int weaponID );
		
		void addWidget( InternalAmmoTransferWidget* widget );
		void updateBufferTexts();
		
		void performTransfer(); 
};

class InternalAmmoTransferWidget : public PG_Widget 
{
  PG_Slider* slider;
	InternalAmmoTransferHandler* _handler;
	int _weaponID;
	int* amount;
	int* buffer;
	PG_Label* bufferAmount;
	PG_Label* weaponAmount;
	
	std::string bufferLabel;
	std::string unitLabel;
	
	public:
		InternalAmmoTransferWidget( PG_Widget* parent, const PG_Rect& pos, InternalAmmoTransferHandler* handler, int weaponID );
		
		bool slide( long amount );
		bool slideEnd( long amount );
		void updateTexts();
};

InternalAmmoTransferHandler::InternalAmmoTransferHandler(  Vehicle* vehicle )
{
	_vehicle = vehicle;
	equalWeaponsTypeCounter = 0;

	for( int i=0; i<16; i++ )
	{
		weaponAmmo[ i ] = vehicle->ammo[ i ];
		weaponAmmoInTransfer[ i ] = 0;
		weaponAmmoTransferSlot[ i ] = -1;
		weaponAmmoTransferable[ i ] = false;
	}
	
	for( int i=0; i<weaponTypeNum; i++ )
	{
		serviceWeaponAmmoInTransfer[ i ] = 0;
		serviceweaponAmmoTransferable[ i ] = false;
	}
	
	for( int i=0; i<vehicle->typ->weapons.count; i++ )
	{
		const SingleWeapon* weapon = vehicle->getWeapon( i );
		if( weapon->canRefuel() )
		{
			serviceweaponAmmoTransferable[ weapon->getScalarWeaponType() ] = true;
			weaponAmmoTransferable[ i ] = true;
		}
	}
	
	for( int i=0; i<vehicle->typ->weapons.count; i++ )
	{
		const SingleWeapon* weapon = vehicle->getWeapon( i );
		if( ! weapon->canRefuel() )
		{
			if( serviceweaponAmmoTransferable[ weapon->getScalarWeaponType() ] )
			{
				weaponAmmoTransferable[ i ] = true;
			}else
			{
				for( int j=0; j<i; j++ )
				{
					if( weapon->equals( vehicle->getWeapon( j ) ) )
					{
						weaponAmmoTransferable[ i ] = true;
						weaponAmmoTransferable[ j ] = true;
//std::cout << "weaponAmmoTransferable: " << i << ", " << j << endl;
						
						if( weaponAmmoTransferSlot[ j ] == -1 )
						{
							weaponAmmoTransferSlot[ j ] = equalWeaponsTypeCounter;
							equalWeaponsTypeCounter++;
						}
						weaponAmmoTransferSlot[ i ] = weaponAmmoTransferSlot[ j ];
					}
				}
			}
		}
	}
}

void InternalAmmoTransferHandler::addWidget( InternalAmmoTransferWidget* widget )
{
	widgets.push_back( widget );
}

void InternalAmmoTransferHandler::updateBufferTexts()
{
	for( int i=0; i<widgets.size(); i++ )
	{
		widgets.at( i ) -> updateTexts();
	}
}


bool InternalAmmoTransferHandler::isAmmoTransferable( int weaponID )
{
	return weaponAmmoTransferable[ weaponID ];
}

int* InternalAmmoTransferHandler::weaponAmmoTransferSource( int weaponID )
{
	return &weaponAmmo[ weaponID ];
}

int InternalAmmoTransferHandler::getMaxAmmo( int weaponID )
{
	return _vehicle->getWeapon( weaponID )->count;
}

std::string InternalAmmoTransferHandler::getName( int weaponID )
{
	return _vehicle->getWeapon( weaponID )->getName();
}

int* InternalAmmoTransferHandler::weaponAmmoTransferBuffer( int weaponID )
{
	if( weaponAmmoTransferSlot[ weaponID ] >= 0 )
	{
		return &weaponAmmoInTransfer[ weaponAmmoTransferSlot[ weaponID ] ];
	}else
	{
		return &serviceWeaponAmmoInTransfer[ _vehicle->getWeapon( weaponID )->getScalarWeaponType() ];
	}
}

void InternalAmmoTransferHandler::performTransfer()
{
	// zuerst die munition von speziellen waffentypen verteilen, die noch in transfer ist
	for( int i=0; i<16; i++ )
	{
		if( weaponAmmoInTransfer[ i ] > 0 )
		{
			for( int j=0;j<16; j++ )
			{
				if( weaponAmmoTransferSlot[ j ] == i )
				{
					const SingleWeapon* weapon = _vehicle->getWeapon( j );
					if( weaponAmmo[ j ] < weapon->count )
					{
						int ammoTransfer = weapon->count - weaponAmmo[ j ];
						if( weaponAmmoInTransfer[ i ] < ammoTransfer )
						{
							ammoTransfer = weaponAmmoInTransfer[ i ];
						}
						weaponAmmo[ j ] += ammoTransfer;
						weaponAmmoInTransfer[ i ] -= ammoTransfer;
					}
				}
			}
		}
	}
	
	// dann die munition in transfer auf servicewaffen aufteilen
	for( int i=0; i<weaponTypeNum; i++ )
	{
		if( serviceWeaponAmmoInTransfer[ i ] > 0 )
		{
			// zuerst den servicewaffen zuweisen
			for( int j=0; j<_vehicle->typ->weapons.count; j++ )
			{
				const SingleWeapon* weapon = _vehicle->getWeapon( j );
				if( weapon->getScalarWeaponType() == i )
				{
					if( weapon->canRefuel() )
					{
						if( weaponAmmo[ j ] < weapon->count )
						{
							int ammoTransfer = weapon->count - weaponAmmo[ j ];
							if( serviceWeaponAmmoInTransfer[ i ] < ammoTransfer )
							{
								ammoTransfer = serviceWeaponAmmoInTransfer[ i ];
							}
							weaponAmmo[ j ] += ammoTransfer;
							serviceWeaponAmmoInTransfer[ i ] -= ammoTransfer;
						}
					}
				}
			}
			// ... dann den anderen waffen zuteilen
			for( int j=0; j<_vehicle->typ->weapons.count; j++ )
			{
				const SingleWeapon* weapon = _vehicle->getWeapon( j );
				if( weapon->getScalarWeaponType() == i )
				{
					if( ! weapon->canRefuel() )
					{
						if( weaponAmmo[ j ] < weapon->count )
						{
							int ammoTransfer = weapon->count - weaponAmmo[ j ];
							if( serviceWeaponAmmoInTransfer[ i ] < ammoTransfer )
							{
								ammoTransfer = serviceWeaponAmmoInTransfer[ i ];
							}
							weaponAmmo[ j ] += ammoTransfer;
							serviceWeaponAmmoInTransfer[ i ] -= ammoTransfer;
						}
					}
				}
			}
		}
	}
	
	// munition sollte jetzt im "legalen" bereich sein, 
	// und alles was im transfer war den echten waffen zugewiesen,
	// also jetzt dem fahrzeug zuweisen
        vector<int> am;
        for( int i=0; i<16; i++ )
           am.push_back( weaponAmmo[i] );
        
        auto_ptr<InternalAmmoTransferCommand> iatc ( new InternalAmmoTransferCommand( _vehicle));
        iatc->setAmmounts( am );
        ActionResult res = iatc->execute( createContext( _vehicle->getMap() ));
        if ( res.successful() )
           iatc.release();
        else
           displayActionError( res );
        
}



InternalAmmoTransferWidget::InternalAmmoTransferWidget( PG_Widget* parent, const PG_Rect& pos, InternalAmmoTransferHandler* handler, int weaponID ) : PG_Widget( parent,pos ), slider(NULL), _weaponID( weaponID )
{
	_handler = handler;
	_handler->addWidget( this );
	amount = _handler->weaponAmmoTransferSource( _weaponID );
	buffer = _handler->weaponAmmoTransferBuffer( _weaponID );
	bufferLabel = "Buffer ";
	unitLabel = "Unit ";
	
	slider = new PG_Slider( this, PG_Rect( 0, 25, pos.w, 15 ),  PG_ScrollBar::HORIZONTAL );
	slider->SetRange( 0, _handler->getMaxAmmo( weaponID ) );
	slider->SetPosition( *amount );
	
	slider->sigSlide.connect( SigC::slot( *this, &InternalAmmoTransferWidget::slide ));
	slider->sigSlideEnd.connect( SigC::slot( *this, &InternalAmmoTransferWidget::slideEnd));
	
	PG_Rect labels = PG_Rect( 0, 0, pos.w, 20 );
	std::stringstream nameStream;
	nameStream << handler->getName( weaponID ) << " (" << weaponID << ")";
	PG_Label* l = new PG_Label ( this, labels, nameStream.str() );
	l->SetAlignment( PG_Label::CENTER );
	
	bufferAmount = new PG_Label ( this, labels );
	bufferAmount->SetAlignment( PG_Label::LEFT );
	
	weaponAmount = new PG_Label ( this, labels );
	weaponAmount->SetAlignment( PG_Label::RIGHT );

	updateTexts();
}


bool InternalAmmoTransferWidget::slide( long newAmount )
{
	int difference = newAmount - (*amount);
	if( difference > (*buffer) )
	{
		difference = (*buffer);
	}
	(*amount) += difference;
	(*buffer) -= difference;
	
	if( difference != 0 )
	{
		_handler->updateBufferTexts();
		return true;
	}
	return false;
}

void InternalAmmoTransferWidget::updateTexts()
{
	std::stringstream ssStream;
	ssStream << bufferLabel << (*buffer);
	bufferAmount->SetText( ssStream.str() );
	
	std::stringstream ssStream2;
	ssStream2 << unitLabel << (*amount);
	weaponAmount->SetText( ssStream2.str() );
}

bool InternalAmmoTransferWidget::slideEnd( long newAmount )
{
	int difference = newAmount - (*amount);
	if( difference > (*buffer) )
	{
		difference = (*buffer);
	}
	(*amount) += difference;
	(*buffer) -= difference;
	
	if( difference != 0 )
	{
		_handler->updateBufferTexts();
		return true;
	}
	return false;
}


class InternalAmmoTransferWindow : public ASC_PG_Dialog {
   private:
      Vehicle* vehicle;
			InternalAmmoTransferHandler handler;
      //ContainerBase* second;
      //TransferHandler handler;


      Surface img1,img2;

      bool ok()
      {
         handler.performTransfer();
         QuitModal();
         return true;
      }
      
   public:
      InternalAmmoTransferWindow ( Vehicle* source, PG_Widget* parent );

      bool somethingToTransfer() { return InternalAmmoTransferCommand::avail( vehicle ); };
      
      bool eventKeyDown(const SDL_KeyboardEvent* key)
      {
         if ( key->keysym.sym == SDLK_ESCAPE )  {
            QuitModal();
            return true;
         }
         return false;
      }
			
      
};


InternalAmmoTransferWindow :: InternalAmmoTransferWindow ( Vehicle* source, PG_Widget* parent ) : ASC_PG_Dialog( NULL, PG_Rect( 30, 30, 400, 400 ), "Transfer" ), vehicle( source ), handler( source )//, second( destination ), handler( source, destination )
{
	int ypos = 30;
	int border = 10;
	
	img1 = IconRepository::getIcon( "container.png" );//source->getImage();
	img2 = source->getImage();
	
	const int singleTransferHeight = 60;
	
	int transferableWeaponsCounter = 0;
	
	for( int i=0; i<16; i++ )
	{
		if( handler.isAmmoTransferable( i ) ) transferableWeaponsCounter++;
	}
	
	int expectedHeight = transferableWeaponsCounter * singleTransferHeight;
	
	int newHeight = min( PG_Application::GetScreen()->h - 60, expectedHeight + 130 );
	SizeWidget( w, newHeight );
	
	PG_ScrollWidget* area = new PG_ScrollWidget ( this, PG_Rect( border, ypos, w - border, h - 80 ));
	area->SetTransparency( 255 );
	
	(new PG_ThemeWidget( area, PG_Rect( 5,3, fieldsizex, fieldsizey)))->SetBackground( img1.getBaseSurface(), PG_Draw::STRETCH );
	(new PG_ThemeWidget( area, PG_Rect( area->Width() - 5 - fieldsizex, 3, fieldsizex, fieldsizey)))->SetBackground( img2.getBaseSurface(), PG_Draw::STRETCH );
	
	ypos = fieldsizex + 5;
	
	for( int i=0; i<16; i++ )
	{
		if( handler.isAmmoTransferable( i ) )
		{
			new InternalAmmoTransferWidget( area, PG_Rect( 0, ypos, area->w - 30, 50 ), &handler, i );
			ypos += singleTransferHeight;
		}
	}
	
	int buttonWidth = 150;
	PG_Button* b = new PG_Button( this, PG_Rect( w - buttonWidth - border, h - 30 - border, buttonWidth, 30), "OK" );
	b->sigClick.connect( SigC::slot( *this, &InternalAmmoTransferWindow::ok ));
}

void internalAmmoTransferWindow ( Vehicle* vehicle )
{
   InternalAmmoTransferWindow iatw( vehicle, NULL );
   if ( iatw.somethingToTransfer() ) {
      iatw.Show();
      iatw.RunModal();
   }
}
