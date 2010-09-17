#include <sigc++/sigc++.h>
#include <sstream>

#include "unitguidedialog.h"

#include "../typen.h"
#include "../terraintype.h"
#include "../vehicletype.h"
#include "../spfst.h"
#include "../spfst-legacy.h"
#include "../gameoptions.h"
#include "../iconrepository.h"
#include "../unitset.h"
#include <iostream>
#include <boost/algorithm/string/trim.hpp>

/** todo:
	* beschreibung und infotext in eigenem fenster
	* heightchange
	* production
	* transport
	* waffen detailinfos fenster
	*/

class DataTab : public PG_Widget
{
	public:
		enum Page { movement, weapons1, weapons2, production, transport, general, terrainAccess1, terrainAccess2 };
		
		DataTab( PG_Widget* parent, const PG_Rect& pos, bool showDescription );
		~DataTab();
		void setPage( Page page );
		void setUnit( VehicleType* type );
		void setTab0( DataTab* tab0 );
		void setCompare( bool compare );
		void setData();
		void HideAllLocal();
		
		
	private:
		Page currentPage;
		VehicleType* currentUnit;
		
		PG_Rect parentPos;
		DataTab* tabCompare;
		bool compareData;
		bool showDescriptionOnly;
		
		//std::vector< PG_Widget* > children;
		PG_Widget *movementTab, *weapons1Tab, *weapons2Tab, *productionTab, *transportTab, *generalTab, *terrainAccessTab1, *terrainAccessTab2;
		PG_Label *nameGeneral, *iconGeneral;//, *categoryIcon;
		PG_Label *id, *category, *armor, *view, *jamming, *maxWind, *tankEnery, *tankMat, *tankFuel, *fuelConsuption, *maxSpeed, *weight, *autoRepairRate, *productionCostEnergy, *productionCostMaterial, *productionCostFuel;
		
		PG_Label **movementIcons, *jumpRange, *jumpCost;
		PG_Label **weaponNames, **weaponPunch, **weaponRange, **weaponRechargeCost;
		PG_Label **terrainAccessLabel;
};

DataTab::DataTab( PG_Widget* parent, const PG_Rect& pos, bool showDescription ) : PG_Widget( parent, pos )
{
	currentPage = general;
	currentUnit = NULL;
	parentPos = pos;
	compareData = false;
	tabCompare = NULL;
	showDescriptionOnly = showDescription;
	
	int xPos = 10;
	int yPos = 10;
	int width = parentPos.Width() - 20;
	int height = parentPos.Height() - 20;
	int lineHeight = 12;
	int labelHeight = 12;
	//int heightLevelWidth = 20;
	int fontSize = 10;
	
	iconGeneral = new PG_Label( this, PG_Rect( xPos, yPos, 48, 48 ) );
	yPos += 55;
	nameGeneral = new PG_Label( this, PG_Rect( xPos, yPos, width, labelHeight ) );
	yPos += lineHeight;

	movementTab = new PG_Widget( this, PG_Rect( xPos, yPos, width, height ) );
	weapons1Tab = new PG_Widget( this, PG_Rect( xPos, yPos, width, height ) );
	weapons2Tab = new PG_Widget( this, PG_Rect( xPos, yPos, width, height ) );
	productionTab = new PG_Widget( this, PG_Rect( xPos, yPos, width, height ) );
	transportTab = new PG_Widget( this, PG_Rect( xPos, yPos, width, height ) );
	generalTab = new PG_Widget( this, PG_Rect( xPos, yPos, width, height ) );
	terrainAccessTab1 = new PG_Widget( this, PG_Rect( xPos, yPos, width, height ) );
	terrainAccessTab2 = new PG_Widget( this, PG_Rect( xPos, yPos, width, height ) );
	
	HideAllLocal();
	generalTab->Show();
	
	
	xPos = 0;
	yPos = 0;
	
	id = new PG_Label( generalTab, PG_Rect( xPos, yPos, width, labelHeight ) );
	yPos += lineHeight;
	//categoryIcon = new PG_Label( generalTab, PG_Rect( xPos, yPos, 20, 20 ) );
	category = new PG_Label ( generalTab, PG_Rect( xPos/* + 30*/, yPos, width - 30, labelHeight ) );
	yPos += lineHeight;
	armor = new PG_Label( generalTab, PG_Rect( xPos, yPos, width, labelHeight ) );
	yPos += lineHeight;
	view = new PG_Label( generalTab, PG_Rect( xPos, yPos, width, labelHeight ) );
	yPos += lineHeight;
	jamming = new PG_Label( generalTab, PG_Rect( xPos, yPos, width, labelHeight ) );
	yPos += lineHeight;
	maxWind = new PG_Label( generalTab, PG_Rect( xPos, yPos, width, labelHeight ) );
	yPos += lineHeight;
	tankEnery = new PG_Label( generalTab, PG_Rect( xPos, yPos, width, labelHeight ) );
	yPos += lineHeight;
	tankMat = new PG_Label( generalTab, PG_Rect( xPos, yPos, width, labelHeight ) );
	yPos += lineHeight;
	tankFuel = new PG_Label( generalTab, PG_Rect( xPos, yPos, width, labelHeight ) );
	yPos += lineHeight;
	fuelConsuption = new PG_Label( generalTab, PG_Rect( xPos, yPos, width, labelHeight ) );
	yPos += lineHeight;
	maxSpeed = new PG_Label( generalTab, PG_Rect( xPos, yPos, width, labelHeight ) );
	yPos += lineHeight;
	weight = new PG_Label( generalTab, PG_Rect( xPos, yPos, width, labelHeight ) );
	yPos += lineHeight;
	autoRepairRate = new PG_Label( generalTab, PG_Rect( xPos, yPos, width, labelHeight ) );
	yPos += lineHeight;
	productionCostEnergy = new PG_Label( generalTab, PG_Rect( xPos, yPos, width, labelHeight ) );
	yPos += lineHeight;
	productionCostMaterial = new PG_Label( generalTab, PG_Rect( xPos, yPos, width, labelHeight ) );
	yPos += lineHeight;
	productionCostFuel = new PG_Label( generalTab, PG_Rect( xPos, yPos, width, labelHeight ) );
	yPos += lineHeight;
	
	xPos = 0;
	yPos = 0;
	
	movementIcons =  new PG_Label*[ choehenstufennum ];
	
	for( int i=0; i<choehenstufennum; i++ )
	{
		movementIcons[ i ] = new PG_Label( movementTab, PG_Rect( xPos, yPos, width, labelHeight ) );
		yPos += lineHeight;
	}
	jumpRange = new PG_Label( movementTab, PG_Rect( xPos, yPos, width, labelHeight ) );
	yPos += lineHeight;
	jumpCost = new PG_Label( movementTab, PG_Rect( xPos, yPos, width, labelHeight ) ); 
	yPos += lineHeight;
	
	xPos = 0;
	yPos = 0;
	
	weaponNames =  new PG_Label*[ 16 ];
	weaponPunch =  new PG_Label*[ 16 ];
	weaponRange =  new PG_Label*[ 16 ];
	weaponRechargeCost =  new PG_Label*[ 16 ];
	for( int i=0; i<8; i++ )
	{
		weaponNames[ i ] = new PG_Label( weapons1Tab, PG_Rect( xPos, yPos, width, labelHeight ) );
		yPos += lineHeight;
		weaponPunch[ i ] = new PG_Label( weapons1Tab, PG_Rect( xPos, yPos, width, labelHeight ) );
		yPos += lineHeight;
		weaponRange[ i ] = new PG_Label( weapons1Tab, PG_Rect( xPos, yPos, width, labelHeight ) );
		yPos += lineHeight;
		weaponRechargeCost[ i ] = new PG_Label( weapons1Tab, PG_Rect( xPos, yPos, width, labelHeight ) );
		yPos += lineHeight;
	}

	xPos = 0;
	yPos = 0;
	
	for( int i=8; i<16; i++ )
	{
		weaponNames[ i ] = new PG_Label( weapons2Tab, PG_Rect( xPos, yPos, width, labelHeight ) );
		yPos += lineHeight;
		weaponPunch[ i ] = new PG_Label( weapons2Tab, PG_Rect( xPos, yPos, width, labelHeight ) );
		yPos += lineHeight;
		weaponRange[ i ] = new PG_Label( weapons2Tab, PG_Rect( xPos, yPos, width, labelHeight ) );
		yPos += lineHeight;
		weaponRechargeCost[ i ] = new PG_Label( weapons2Tab, PG_Rect( xPos, yPos, width, labelHeight ) );
		yPos += lineHeight;
	}

	xPos = 0;
	yPos = 0;
	
	terrainAccessLabel =  new PG_Label*[ terrainPropertyNum ];
	for( int i=0; i<30; i++ )
	{
		terrainAccessLabel[ i ] = new PG_Label( terrainAccessTab1, PG_Rect( xPos, yPos, width, labelHeight ) );
		yPos += lineHeight;
	}

	xPos = 0;
	yPos = 0;
	
	for( int i=30; i<terrainPropertyNum; i++ )
	{
		terrainAccessLabel[ i ] = new PG_Label( terrainAccessTab2, PG_Rect( xPos, yPos, width, labelHeight ) );
		yPos += lineHeight;
	}

	xPos = 0;
	yPos = 0;
	
	SetFontSize( fontSize, true );
}

DataTab::~DataTab()
{
	delete movementIcons;
}

void DataTab::setTab0( DataTab* tab0 )
{
	tabCompare = tab0;
}

void DataTab::setCompare( bool compare )
{
	compareData = compare;
	setData();
}

void DataTab::HideAllLocal()
{
	movementTab->Hide();
	weapons1Tab->Hide();
	weapons2Tab->Hide();
	productionTab->Hide();
	transportTab->Hide();
	terrainAccessTab1->Hide();
	terrainAccessTab2->Hide();
	generalTab->Hide();
}

void DataTab::setPage( Page page )
{
	currentPage = page;
	HideAllLocal();
	switch( page )
	{
		case movement:
			movementTab->Show();
			break;
		case weapons1:
			weapons1Tab->Show();
			break;
		case weapons2:
			weapons2Tab->Show();
			break;
		case production:
			productionTab->Show();
			break;
		case transport:
			transportTab->Show();
			break;
		case terrainAccess1:
			terrainAccessTab1->Show();
			break;
		case terrainAccess2:
			terrainAccessTab2->Show();
			break;
		default:
			generalTab->Show();
			break;
	}
}

void DataTab::setUnit( VehicleType* type )
{
	currentUnit = type;
	setData();
}

void DataTab::setData()
{
	
	
	if( currentUnit == NULL && ! showDescriptionOnly )
		return;
	
	VehicleType* compareType = NULL;
	if( compareData && tabCompare != NULL )
	{
		compareType = tabCompare->currentUnit;
	}
	
	char buffer[ 500 ];
	std::string value = "";

	if( showDescriptionOnly )
	{
		iconGeneral->SetText( "Icon" );
		nameGeneral->SetText( "Name" );
		id->SetText( "ID" );
		category->SetText( "Category" );
		armor->SetText( "Armor" );
		view->SetText( "View" );
		jamming->SetText( "Jamming" );
		maxWind->SetText( "Max Wind Resistance" );
		tankEnery->SetText( "Tank Energy ASC|BI" );
		tankMat->SetText( "Tank Material ASC|BI" );
		tankFuel->SetText( "Tank Fuel ASC|BI" );
		fuelConsuption->SetText( "Fuel Consumption" );
		maxSpeed->SetText( "Max Speed" );
		weight->SetText( "Weight" );
		autoRepairRate->SetText( "Auto Repair Rate" );
		productionCostEnergy->SetText( "Production Cost Energy" );
		productionCostMaterial->SetText( "Production Cost Material" );
		productionCostFuel->SetText( "Production Cost Fuel" );
		
		for( int i=0; i<choehenstufennum; i++ )
		{
			std::string iconName = "height-a";
			char buffer[ 5 ];
			itoa( i, buffer, 10 );
			iconName += buffer;
			iconName += ".png";
			movementIcons[ i ]->SetIcon( IconRepository::getIcon( iconName ).getBaseSurface() );
		}
		
		for( int i=0; i<16; i++ )
		{
			value = "Weapon ";
			itoa( i, buffer, 10 );
			value += buffer;
			value += " Name";
			weaponNames[ i ]->SetText( value );
			weaponPunch[ i ]->SetText( "Punch" );
			weaponRange[ i ]->SetText( "Range" );
			weaponRechargeCost[ i ]->SetText( "Laser Recharge" );
		}
		
		for( int i=0; i<terrainPropertyNum; i++ )
		{
			terrainAccessLabel[ i ]->SetText( terrainProperty[ i ] );
		}
		
		jumpRange->SetText( "Jump Range" );
		jumpCost->SetText( "Jump Cost" );
		
	}else
	{
		iconGeneral->SetIcon( currentUnit->getImage().getBaseSurface() );
		nameGeneral->SetText( currentUnit->getName() );
		itoa( currentUnit->getID(), buffer, 10 );
		id->SetText( buffer );
		//categoryIcon->SetIcon( IconRepository::getIcon( moveMaliTypeIcons[ currentUnit->movemalustyp ] ).getBaseSurface() );
		category->SetText( cmovemalitypes[ currentUnit->movemalustyp ] );		

		{
			int armor = currentUnit->armor;
			if( compareType != NULL )
			{
				armor = armor - compareType->armor;
			}
			itoa( armor, buffer, 10 );
			this->armor->SetText( buffer );
		}
		
		{
			int view = currentUnit->view;
			if( compareType != NULL )
			{
				view = view - compareType->view;
			}
			itoa( view, buffer, 10 );
			this->view->SetText( buffer );
		}
		
		{
			int jamming = currentUnit->jamming;
			if( compareType != NULL )
			{
				jamming = jamming - compareType->jamming;
			}
			itoa( jamming, buffer, 10 );
			this->jamming->SetText( buffer );
		}

		{
			int maxwindspeedonwater = currentUnit->maxwindspeedonwater;
			if( compareType != NULL )
			{
				maxwindspeedonwater = maxwindspeedonwater - compareType->maxwindspeedonwater;
			}
			itoa( maxwindspeedonwater, buffer, 10 );
			this->maxWind->SetText( buffer );
		}
		
		{
			Resources biTank = currentUnit->getStorageCapacity( 1 );
			Resources ascTank = currentUnit->getStorageCapacity( 0 );
			value = "";

			int tank = ascTank.resource( 0 );
			if( compareType != NULL )
			{
				tank = tank - compareType->getStorageCapacity( 0 ).resource( 0 );
			}
			itoa( tank, buffer, 10 );
			
			value += buffer;
			value += " | ";
			
			tank = biTank.resource( 0 );
			if( compareType != NULL )
			{
				tank = tank - compareType->getStorageCapacity( 1 ).resource( 0 );
			}
			itoa( tank, buffer, 10 );
			
			value += buffer;
			this->tankEnery->SetText( value );
			
			value = "";
			tank = ascTank.resource( 1 );
			if( compareType != NULL )
			{
				tank = tank - compareType->getStorageCapacity( 0 ).resource( 1 );
			}
			itoa( tank, buffer, 10 );
			
			value += buffer;
			value += " | ";
			
			tank = biTank.resource( 1 );
			if( compareType != NULL )
			{
				tank = tank - compareType->getStorageCapacity( 1 ).resource( 1 );
			}
			itoa( tank, buffer, 10 );
			
			value += buffer;
			this->tankMat->SetText( value );

			
			value = "";
			tank = ascTank.resource( 2 );
			if( compareType != NULL )
			{
				tank = tank - compareType->getStorageCapacity( 0 ).resource( 2 );
			}
			itoa( tank, buffer, 10 );
			
			value += buffer;
			value += " | ";
			
			tank = biTank.resource( 2 );
			if( compareType != NULL )
			{
				tank = tank - compareType->getStorageCapacity( 1 ).resource( 2 );
			}
			itoa( tank, buffer, 10 );
			
			value += buffer;
			this->tankFuel->SetText( value );
		}

		{
			int fuelConsumption = currentUnit->fuelConsumption;
			if( compareType != NULL )
			{
				fuelConsumption = fuelConsumption - compareType->fuelConsumption;
			}
			itoa( fuelConsumption, buffer, 10 );
			this->fuelConsuption->SetText( buffer );
		}

		{
			int movement = currentUnit->maxSpeed();
			if( compareType != NULL )
			{
				movement = movement - compareType->maxSpeed();
			}
			itoa( movement, buffer, 10 );
			this->maxSpeed->SetText( buffer );
		}

		{
			int weight = currentUnit->weight;
			if( compareType != NULL )
			{
				weight = weight - compareType->weight;
			}
			itoa( weight, buffer, 10 );
			this->weight->SetText( buffer );
		}

		{
			int autorepairrate = currentUnit->autorepairrate;
			if( compareType != NULL )
			{
				autorepairrate = autorepairrate - compareType->autorepairrate;
			}
			itoa( autorepairrate, buffer, 10 );
			this->autoRepairRate->SetText( buffer );
		}

		
		{
			int tank = currentUnit->calcProductionCost().resource( 0 );
			if( compareType != NULL )
			{
				tank = tank - compareType->calcProductionCost().resource( 0 );
			}
			itoa( tank, buffer, 10 );
			
			this->productionCostEnergy->SetText( buffer );
		}

		{
			int tank = currentUnit->calcProductionCost().resource( 1 );
			if( compareType != NULL )
			{
				tank = tank - compareType->calcProductionCost().resource( 1 );
			}
			itoa( tank, buffer, 10 );
			
			this->productionCostMaterial->SetText( buffer );
		}

		{
			int tank = currentUnit->calcProductionCost().resource( 2 );
			if( compareType != NULL )
			{
				tank = tank - compareType->calcProductionCost().resource( 2 );
			}
			itoa( tank, buffer, 10 );
			
			this->productionCostFuel->SetText( buffer );
		}

		for( int i=0; i<choehenstufennum; i++ )
		{
			int movement = currentUnit->movement[ i ];
			if( compareType != NULL )
			{
				movement = movement - compareType->movement[ i ];
			}
			itoa( movement, buffer, 10 );
			value = buffer;
			if( currentUnit->jumpDrive.height & (1<<i) )
				value += " Jump";
			movementIcons[ i ]->SetText( value );
		}
		
		{
			int maxDistance = currentUnit->jumpDrive.maxDistance;
			if( compareType != NULL )
			{
				maxDistance = maxDistance - compareType->jumpDrive.maxDistance;
			}
			maxDistance /= 10;
			itoa( maxDistance, buffer, 10 );
			this->jumpRange->SetText( buffer );
		}
		
		for( int i=0; i<16; i++ )
		{
			weaponNames[ i ]->SetText( "N/A" );
			weaponPunch[ i ]->SetText( "0-0" );
			weaponRange[ i ]->SetText( "0-0" );
			weaponRechargeCost[ i ]->SetText( "0x 0|0|0" );
		}
		
		for( int i=0; i<currentUnit->weapons.count; i++ )
		{
			weaponNames[ i ]->SetText( currentUnit->weapons.weapon[ i ].name );
			
			value = "";
			int maxPunch = currentUnit->weapons.weapon[ i ].maxstrength;
			if( compareType != NULL )
			{
				maxPunch = maxPunch - compareType->weapons.weapon[ i ].maxstrength;
			}
			itoa( maxPunch, buffer, 10 );
			value = buffer;
			value += "-";
			int minPunch = currentUnit->weapons.weapon[ i ].minstrength;
			if( compareType != NULL )
			{
				minPunch = minPunch - compareType->weapons.weapon[ i ].minstrength;
			}
			itoa( minPunch, buffer, 10 );
			value += buffer;
			weaponPunch[ i ]->SetText( value );
			
			value = "";
			int minRange = currentUnit->weapons.weapon[ i ].mindistance;
			if( compareType != NULL )
			{
				minRange = minRange - compareType->weapons.weapon[ i ].mindistance;
			}
			minRange /= 10;
			itoa( minRange, buffer, 10 );
			value = buffer;
			value += "-";
			int maxRange = currentUnit->weapons.weapon[ i ].maxdistance;
			if( compareType != NULL )
			{
				maxRange = maxRange - compareType->weapons.weapon[ i ].maxdistance;
			}
			maxRange /= 10;
			itoa( maxRange, buffer, 10 );
			value += buffer;
			weaponRange[ i ]->SetText( value );
			
			value = "";
			int rechargeRate = currentUnit->weapons.weapon[ i ].laserRechargeRate;
			if( compareType != NULL )
			{
				rechargeRate = rechargeRate - compareType->weapons.weapon[ i ].laserRechargeRate;
			}
			itoa( rechargeRate, buffer, 10 );
			value = buffer;
			value += "x  ";
			int costEnergy = currentUnit->weapons.weapon[ i ].laserRechargeCost.resource( 0 );
			if( compareType != NULL )
			{
				costEnergy = costEnergy - compareType->weapons.weapon[ i ].laserRechargeCost.resource( 0 );
			}
			itoa( costEnergy, buffer, 10 );
			value += buffer;
			value += "|";
			int costMaterial = currentUnit->weapons.weapon[ i ].laserRechargeCost.resource( 1 );
			if( compareType != NULL )
			{
				costMaterial = costMaterial - compareType->weapons.weapon[ i ].laserRechargeCost.resource( 1 );
			}
			itoa( costMaterial, buffer, 10 );
			value += buffer;
			value += "|";
			int costFuel = currentUnit->weapons.weapon[ i ].laserRechargeCost.resource( 2 );
			if( compareType != NULL )
			{
				costFuel = costFuel - compareType->weapons.weapon[ i ].laserRechargeCost.resource( 2 );
			}
			itoa( costFuel, buffer, 10 );
			value += buffer;
			weaponRechargeCost[ i ]->SetText( value );
		}

		for( int i=0; i<terrainPropertyNum; i++ )
		{
			if( currentUnit->terrainaccess.terrainkill[ i ] )
			{
				terrainAccessLabel[ i ]->SetText( "kill" );
			}else if( currentUnit->terrainaccess.terrainnot[ i ] )
			{
				terrainAccessLabel[ i ]->SetText( "not accessible" );
			}else if( currentUnit->terrainaccess.terrainreq[ i ] )
			{
				terrainAccessLabel[ i ]->SetText( "required" );
			}else if( currentUnit->terrainaccess.terrain[ i ] )
			{
				terrainAccessLabel[ i ]->SetText( "yes" );
			}else
			{
				terrainAccessLabel[ i ]->SetText( "" );
			}
			
			// narf!
			if( currentUnit->jumpDrive.height != 0 )
			{
				if( currentUnit->jumpDrive.targetterrain.terrainreq[ i ] )
				{
					value = terrainAccessLabel[ i ]->GetText();
					value += " JumpReq";
					terrainAccessLabel[ i ]->SetText( value );
				}else if( currentUnit->jumpDrive.targetterrain.terrain[ i ] )
				{
					value = terrainAccessLabel[ i ]->GetText();
					value += " Jump";
					terrainAccessLabel[ i ]->SetText( value );
				}
			}
		}
		
		{
			value = "";
			int cost = currentUnit->jumpDrive.consumption.resource( 0 );
			if( compareType != NULL )
			{
				cost = cost - compareType->jumpDrive.consumption.resource( 0 );
			}
			itoa( cost, buffer, 10 );
			value += buffer;
			value += "|";
			
			cost = currentUnit->jumpDrive.consumption.resource( 1 );
			if( compareType != NULL )
			{
				cost = cost - compareType->jumpDrive.consumption.resource( 1 );
			}
			itoa( cost, buffer, 10 );
			value += buffer;
			value += "|";
			
			cost = currentUnit->jumpDrive.consumption.resource( 2 );
			if( compareType != NULL )
			{
				cost = cost - compareType->jumpDrive.consumption.resource( 2 );
			}
			itoa( cost, buffer, 10 );
			value += buffer;
			
			jumpCost->SetText( value );
		}
	}
		
	
}



class UnitGuideWidget : public PG_Widget
{
	private:
    DropDownSelector* mainCategory;
    DropDownSelector* subCategory;
		
    DropDownSelector* unitSet;
    DropDownSelector* displayTab;
    DropDownSelector* unit;
    DropDownSelector* page;
		
		PG_Button* display;
		PG_CheckButton* showDifference;

		PG_Label* mainCategoryLabel;
		PG_Label* subCategoryLabel;
		PG_Label* unitSetLabel;
		PG_Label* displayTabLabel;
		PG_Label* unitLabel;
		PG_Label* showDifferenceLabel;
		
		PG_ScrollArea* content;
		
		DataTab* descriptionTab;
		//--------------------------
		struct TSubCategory{
			int category;
			std::string name;
		};
		typedef std::vector< TSubCategory > TSubCategories;
		
		struct TMainCategory{
			std::string name;
			TSubCategories subCategories;
		};
		typedef std::vector< TMainCategory > TMainCategories;
		
		TMainCategories mainCategories;
		//--------------------------
		
		void loadCategories();
		
		void selectUnits();
		
		// signal connectors
		void mainCategoryChanged( int i );
		void subCategoryChanged( int i );
		void unitSetChanged( int i );
		void pageChanged( int i );
		bool displayUnit();
		bool showDifferenceTrigger( bool pressed );
		//---------------------------
		typedef std::vector< VehicleType* > TVehicleTypes;
		TVehicleTypes selectedUnits;
		
		typedef std::vector< DataTab* > TDataTabs;
		TDataTabs dataTabs;
		
	public:
		UnitGuideWidget( PG_Widget* parent, const PG_Rect& pos, int tabs );
};



UnitGuideWidget::UnitGuideWidget( PG_Widget* parent, const PG_Rect& pos, int tabs ) : PG_Widget( parent, pos )
{
	loadCategories();
	
	int xPos = 20;
	int yPos = 20;
	
	mainCategoryLabel = new PG_Label ( this, PG_Rect( xPos, yPos, 100, 20 ), "Main Category" );
	xPos += 120;
	
	mainCategory = new DropDownSelector( this, PG_Rect( xPos, yPos, 200, 20 ) );
	for( int i=0; i<mainCategories.size(); i++ )
	{
		mainCategory->AddItem( mainCategories[ i ].name );
	}
	mainCategory->selectionSignal.connect( SigC::slot( *this, &UnitGuideWidget::mainCategoryChanged ) );
	xPos += 220;
	
	unitSetLabel = new PG_Label ( this, PG_Rect( xPos, yPos, 100, 20 ), "Unit Set" );
	xPos += 120;
	
	unitSet = new DropDownSelector( this, PG_Rect( xPos, yPos, 200, 20 ) );
	unitSet->AddItem( "All Sets" );
	for( int i=0; i<unitSets.size(); i++ )
	{
		unitSet->AddItem( unitSets[ i ]->name );
	}
	unitSet->selectionSignal.connect( SigC::slot( *this, &UnitGuideWidget::unitSetChanged ) );
	xPos = 20;
	yPos += 25;
	
	
	subCategoryLabel = new PG_Label ( this, PG_Rect( xPos, yPos, 100, 20 ), "Sub Category" );
	xPos += 120;
	
	subCategory = new DropDownSelector( this, PG_Rect( xPos, yPos, 200, 20 ) );
	subCategory->selectionSignal.connect( SigC::slot( *this, &UnitGuideWidget::subCategoryChanged ) );
	xPos += 220;
	
	unitLabel = new PG_Label ( this, PG_Rect( xPos, yPos, 100, 20 ), "Unit" );
	xPos += 120;
	
	unit = new DropDownSelector( this, PG_Rect( xPos, yPos, 200, 20 ) );
	xPos = 20;
	yPos += 25;
	
	displayTabLabel = new PG_Label ( this, PG_Rect( xPos, yPos, 100, 20 ), "Display on tab" );
	xPos += 120;
	
	displayTab = new DropDownSelector( this, PG_Rect( xPos, yPos, 50, 20 ) );
	for( int i=0; i<tabs; i++ )
	{
		std::stringstream nameStream;
		nameStream << i;
		displayTab->AddItem( nameStream.str() );
	}
	xPos += 70;
	
	showDifferenceLabel = new PG_Label ( this, PG_Rect( xPos, yPos, 100, 20 ), "show difference" );
	xPos += 120;
	
	showDifference = new PG_CheckButton( this, PG_Rect( xPos, yPos, 20, 20 ) );
  showDifference->sigClick.connect( SigC::slot( *this, &UnitGuideWidget::showDifferenceTrigger ));
	xPos += 40;
	
	display = new PG_Button( this, PG_Rect( xPos, yPos, 100, 20 ), "display" );
  display->sigClick.connect( SigC::slot( *this, &UnitGuideWidget::displayUnit ));
	xPos += 120;
	
	page = new DropDownSelector( this, PG_Rect( xPos, yPos, 200, 20 ) );
	page->AddItem( "General" );
	page->AddItem( "Weapons 1" );
	page->AddItem( "Weapons 2" );
	page->AddItem( "Movement" );
	page->AddItem( "Transport" );
	page->AddItem( "TerrainAccess 1" );
	page->AddItem( "TerrainAccess 2" );
	page->AddItem( "Production" );
	page->selectionSignal.connect( SigC::slot( *this, &UnitGuideWidget::pageChanged ) );
	
	xPos = 0;
	yPos += 25;
	/*
	content = new PG_ScrollArea( this, PG_Rect( xPos, yPos, pos.Width(), pos.Height() - yPos - 10 ) );
	content->ScrollTo( 0, 0 );
	*/
	descriptionTab = new DataTab( this, PG_Rect( xPos, yPos, 200, pos.Height() - yPos - 10 ), true );
	descriptionTab->setData();

	xPos += 200;
	for( int i=0; i<tabs; i++ )
	{
		DataTab* dataTab = new DataTab( this, PG_Rect( xPos, yPos, 150, pos.Height() - yPos - 10 ), false );
		dataTabs.push_back( dataTab );
		xPos += 150;
	}
	for( int i=1; i<tabs; i++ )
	{
		dataTabs[ i ]->setTab0( dataTabs[ 0 ] );
	}
	mainCategory->SelectFirstItem();
	unitSet->SelectFirstItem();
	
}

bool UnitGuideWidget::showDifferenceTrigger( bool pressed )
{
	//Hide();
	for( int i=1; i<dataTabs.size(); i++ )
	{
		dataTabs[ i ]->setCompare( pressed );
	}
	//Show();
	return true;
}

void UnitGuideWidget::pageChanged( int i )
{
	//Hide();
	switch( i )
	{
		case 0: 
			for( int tab=0; tab<dataTabs.size(); tab++ )
				dataTabs[ tab ]->setPage( DataTab::general );
			descriptionTab->setPage( DataTab::general );
			break;
		case 1:
			for( int tab=0; tab<dataTabs.size(); tab++ )
				dataTabs[ tab ]->setPage( DataTab::weapons1 );
			descriptionTab->setPage( DataTab::weapons1 );
			break;
		case 2:
			for( int tab=0; tab<dataTabs.size(); tab++ )
				dataTabs[ tab ]->setPage( DataTab::weapons2 );
			descriptionTab->setPage( DataTab::weapons2 );
			break;
		case 3:
			for( int tab=0; tab<dataTabs.size(); tab++ )
				dataTabs[ tab ]->setPage( DataTab::movement );
			descriptionTab->setPage( DataTab::movement );
			break;
		case 4:
			for( int tab=0; tab<dataTabs.size(); tab++ )
				dataTabs[ tab ]->setPage( DataTab::transport );
			descriptionTab->setPage( DataTab::transport );
			break;
		case 5:
			for( int tab=0; tab<dataTabs.size(); tab++ )
				dataTabs[ tab ]->setPage( DataTab::terrainAccess1 );
			descriptionTab->setPage( DataTab::terrainAccess1 );
			break;
		case 6:
			for( int tab=0; tab<dataTabs.size(); tab++ )
				dataTabs[ tab ]->setPage( DataTab::terrainAccess2 );
			descriptionTab->setPage( DataTab::terrainAccess2 );
			break;
		case 7:
			for( int tab=0; tab<dataTabs.size(); tab++ )
				dataTabs[ tab ]->setPage( DataTab::production );
			descriptionTab->setPage( DataTab::production );
			break;
		default:
			for( int tab=0; tab<dataTabs.size(); tab++ )
				dataTabs[ tab ]->setPage( DataTab::general );
			descriptionTab->setPage( DataTab::general );
			break;
	}
	//Show();
}

bool UnitGuideWidget::displayUnit()
{
//	Hide();
	dataTabs[ displayTab->GetSelectedItemIndex() ]->setUnit( selectedUnits[ unit->GetSelectedItemIndex() ] );
	
	if( showDifference->GetPressed() && displayTab->GetSelectedItemIndex() == 0 )
	{
		for( int i=1; i<dataTabs.size(); i++ )
			dataTabs[ i ]->setData();
	}
	
//	Show();
	return true;
}

void UnitGuideWidget::selectUnits()
{
	selectedUnits.clear();
	int allUnits = actmap->getVehicleTypeNum();
	int filter = mainCategories[ mainCategory->GetSelectedItemIndex() ].subCategories[ subCategory->GetSelectedItemIndex() ].category;
	
	SingleUnitSet* selectedUnitSet = NULL;
	if( unitSet->GetSelectedItemIndex() > 0 )
		selectedUnitSet = unitSets[ unitSet->GetSelectedItemIndex() - 1 ];
	
	
	for( int i=0; i<allUnits; i++ )
	{
		VehicleType *type = actmap->getvehicletype_bypos( i );
		
		if( selectedUnitSet != NULL )
		{
			if( ! selectedUnitSet->isMember( type->id, SingleUnitSet::unit ) )
				continue;
		}
		
		for( int j=0; j<type->guideSortHelp.size(); j++ )
		{
			if( type->guideSortHelp[ j ] == filter )
			{
				// yay!
				selectedUnits.push_back( type );
				
				break;
			}
		}
	}
	
	unit->DeleteAll();
	for( int i=0; i<selectedUnits.size(); i++ )
	{
		unit->AddItem( selectedUnits[ i ]->getName() );
	}
	unit->SelectFirstItem();
}

void UnitGuideWidget::mainCategoryChanged( int i )
{
	subCategory->DeleteAll();
	
	TSubCategories subCategories = mainCategories[ i ].subCategories;
	for( int j=0; j<subCategories.size(); j++ )
	{
		subCategory->AddItem( subCategories[ j ].name );
	}
	subCategory->SelectFirstItem();
}

void UnitGuideWidget::subCategoryChanged( int i )
{
	selectUnits();
}

void UnitGuideWidget::unitSetChanged( int i )
{
	selectUnits();
}


void UnitGuideWidget::loadCategories()
{
	ASCString lineBuffer;
	tnfilestream file( "guidesorthelp.properties", tnstream::reading );

	TMainCategory mainCategory;
	while( file.readTextString( lineBuffer ) )
	{
		std::string line = lineBuffer;
		boost::algorithm::trim( line );

		if( line.size() == 0 ) 
			continue;
		
		if( line[ 0 ] == '[' )
		{
			if( mainCategory.subCategories.size() > 0 )
			{
				mainCategories.push_back( mainCategory );
			}
			mainCategory.name = line.substr( 1, line.length() - 2 );
			mainCategory.subCategories.clear();
		}else
		{
			if( line.find( "=" ) > 0 )
			{
				std::string key = line.substr( 0, line.find( "=" ) );
				line = line.substr( line.find( "=" ) + 1 );
				boost::algorithm::trim( key );
				boost::algorithm::trim( line );
				TSubCategory subCat;
				subCat.name = line;
				subCat.category = atoi( key );
				mainCategory.subCategories.push_back( subCat );
			}
		}
		
	}
	if( mainCategory.subCategories.size() > 0 )
	{
		mainCategories.push_back( mainCategory );
	}
	/*
	TSubCategory subCategory;
	subCategory.name = "Heavy Trooper";
	subCategory.category = 203;
	
	TSubCategories infantry;
	infantry.push_back( subCategory );

	subCategory.name = "jumps";
	subCategory.category = 505;
	infantry.push_back( subCategory );
	
	TMainCategory mainCategory;
	mainCategory.name = "Infantry";
	mainCategory.subCategories = infantry;
	
	mainCategories.push_back( mainCategory );

	TSubCategories tanks;
	subCategory.name = "A-Sat Multi";
	subCategory.category = 232;
	tanks.push_back( subCategory );

	mainCategory.name = "Sats";
	mainCategory.subCategories = tanks;
	
	mainCategories.push_back( mainCategory );
	*/
}

class UnitGuideWindow : public ASC_PG_Dialog
{
	private:
		UnitGuideWidget *w;
	public: 
		UnitGuideWindow( PG_Widget* parent, const PG_Rect& pos, int tabs );
		~UnitGuideWindow();
};

UnitGuideWindow::UnitGuideWindow( PG_Widget* parent, const PG_Rect& pos, int tabs ) : ASC_PG_Dialog( parent, pos, "UnitGuide" )
{
	w = new UnitGuideWidget( this, PG_Rect( 0, 0, pos.Width(), pos.Height() ), tabs );
}

UnitGuideWindow::~UnitGuideWindow()
{
	delete w;
}

void unitGuideWindow( int tabs )
{
   try {
      UnitGuideWindow w( NULL, PG_Rect( 50, 50, 760, 600 ), tabs );
      w.Show();
      w.RunModal();
   } catch ( tfileerror err ) {
      errorMessage("error loading file " + err.getFileName() );
   }
}
