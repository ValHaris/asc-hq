/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/***************************************************************************
                          Preiskalkulation  -  description
                             -------------------
    begin                : So Aug 15 2004
    copyright            : (C) 2001 by Martin Bickel & Steffen Froelich
    email                : bickel@asc-hq.org

Part I   -description
Part II  -beginn calculation
Part III -typecost
part IV  -weaponcost
Part V   -specialcost
Part VI  -addition
Part VII -malus

 ***************************************************************************/


#include "unitcostcalculator-pbp.h"

#include "vehicletype.h"

//Part II beginn calculation


ASCString PBPUnitCostCalculator::getCalculatorName()
{
   return "PBP";
}

Resources PBPUnitCostCalculator :: productionCost( const VehicleType* vehicle )
{
   Resources res;
   // Anfang -> Abschluss
   res.energy = 0;
   res.material = 0;
   res.fuel = 0;
   // int typecoste = 0;
   float typecostm = 0;
   // int weaponcoste = 0;
   int weaponcostm = 0;
   //int specialcoste = 0;
   int specialcostm = 0;
   int maxmoverange = 0;
   int maxweaponrange = 0;
   int unitfactor = 0;

   // Check Flugzeugtraeger
   bool carrierCharge = false;
   for ( int T=0; T < vehicle->entranceSystems.size(); ++T )
      if ((( vehicle->entranceSystems[T].container_height < chtieffliegend && (vehicle->entranceSystems[T].height_abs & (chtieffliegend | chfliegend | chhochfliegend | chsatellit))) ||
             (( vehicle->entranceSystems[T].container_height & ( chfliegend | chhochfliegend | chsatellit)) && (vehicle->entranceSystems[T].height_abs & ( chfliegend | chhochfliegend | chsatellit))))
             && vehicle->maxLoadableUnits > 6 )
         carrierCharge = true;

   // Check maximale Bewegungsreichweite
   for ( int M=0; M < vehicle->movement.size(); ++M ) {
      if ( maxmoverange < vehicle->movement[M] ) {
         maxmoverange = vehicle->movement[M];
      }
   }
   // Check maximale Waffenreichweite
   if ( vehicle->weapons.count > 0 ) {
      for ( int WR=0; WR < vehicle->weapons.count; ++WR ) {
         if ( maxweaponrange < vehicle->weapons.weapon[WR].maxdistance ) {
            maxweaponrange = vehicle->weapons.weapon[WR].maxdistance;
         }
      }
   }


// Part III typecost

	// Setzen der Typfaktoren
   if ( vehicle->movemalustyp == MoveMalusType::trooper) {
	// Soldaten
      unitfactor = 4;
   } else if ( vehicle->movemalustyp == MoveMalusType::light_wheeled_vehicle ) {
	// Bugies, Trikes, Motorraeder, Jeeps
      unitfactor = 7;
   } else if ( vehicle->movemalustyp == MoveMalusType::medium_wheeled_vehicle ) {
	// Servicefahrzeuge, LKWs
      unitfactor = 8;
   } else if ( vehicle->movemalustyp == MoveMalusType::heavy_wheeled_vehicle ) {
	// Selbstfahrlafetten, leichte Panzer, Servicekettenfahrzeuge
      unitfactor = 9;
   } else if ( vehicle->movemalustyp == MoveMalusType::light_tracked_vehicle  ) {
	//  Kampfroboter, kleine Panzer (Wiesel)
      unitfactor = 8;
   } else if ( vehicle->movemalustyp == MoveMalusType::medium_tracked_vehicle  ) {
	// Selbstfahrlafetten, leichte Panzer, Servicekettenfahrzeuge
      unitfactor = 9;
   } else if ( vehicle->movemalustyp == MoveMalusType::heavy_tracked_vehicle ) {
        // Kampfpanzer, fahrende Bunker, grosse Kettenfahrzeuge
      unitfactor = 10;
   } else if ( vehicle->movemalustyp == MoveMalusType::hoovercraft) {
        // Luftkissenfahrzeuge
      unitfactor = 10;
   } else if ( vehicle->movemalustyp == MoveMalusType::light_ship ) {
        // Boote und sehr kleine Schiffe (Boote, Korvette, Mini-U-Boote)
      unitfactor = 9;
   } else if ( vehicle->movemalustyp == MoveMalusType::medium_ship ) {
        // mittlere Schiffe (Fregatten, Zerst�rer, U-Boote)
      unitfactor = 11;
   } else if ( vehicle->movemalustyp == MoveMalusType::heavy_ship ) {
        // gro�e Schiffe (Kreuzer, Schlachtschiffe, Atom-U-Boote)
      unitfactor = 13;
   } else if ( vehicle->movemalustyp == MoveMalusType::helicopter ) {
        // Rotorgetriebene Luftfahrzeuge
      unitfactor = 15;
   } else if ( vehicle->movemalustyp == MoveMalusType::light_aircraft ) {
   	// kleine Flugzeuge ( Drohnen, Satelliten )
      unitfactor = 14;
   } else if ( vehicle->movemalustyp == MoveMalusType::medium_aircraft ) {
      unitfactor = 16;
   } else if ( vehicle->movemalustyp == MoveMalusType::heavy_aircraft ) {
      unitfactor = 17;
   } else if ( vehicle->movemalustyp == MoveMalusType::rail_vehicle ) {
	// Schienenfahrzeuge
      unitfactor = 9;
   } else if ( vehicle->movemalustyp == MoveMalusType::structure) {
	// Gebaeude, Bunker, Geschuetztuerme
      unitfactor = 9;

   } else {
	// Geraete und andere
      unitfactor = 6;
   }

   // Basiskosten Chassis
   typecostm += vehicle->armor*unitfactor;

   // Zuschlag fuer U-Boote / Druckhuelle
   if ( vehicle->height & chgetaucht ) {
      if ( vehicle->movemalustyp == MoveMalusType::light_ship || vehicle->movemalustyp == MoveMalusType::medium_ship || vehicle->movemalustyp == MoveMalusType::heavy_ship ) {
         typecostm += vehicle->armor*unitfactor/4;
      } else {
         if ( vehicle->movemalustyp == MoveMalusType::trooper ) {
            typecostm += vehicle->armor*unitfactor;
         } else {
            typecostm += vehicle->armor*unitfactor/2;
         }
      }
   }
   // Zuschlag fuer U-Boote / Druckhuelle
   if ( vehicle->height & chtiefgetaucht ) {
      if ( vehicle->movemalustyp == MoveMalusType::light_ship || vehicle->movemalustyp == MoveMalusType::medium_ship || vehicle->movemalustyp == MoveMalusType::heavy_ship ) {
         typecostm += vehicle->armor*unitfactor/4;
      } else {
         if ( vehicle->movemalustyp == MoveMalusType::trooper ) {
            typecostm += vehicle->armor*unitfactor;
         } else {
            typecostm += vehicle->armor*unitfactor/2;
         }
      }
   }
   // Zuschlag fuer orbitalfaehige Einheiten / Druckhuelle
   if ( vehicle->height & chsatellit ) {
      typecostm += vehicle->armor*unitfactor/3;
   }
   // Zuschlag fuer hochfliegende Einheiten
   if ( vehicle->height & chhochfliegend ) {
      typecostm += vehicle->armor*unitfactor/3;
   }
   // Zuschlag fuer normal fliegende Einheiten
   if ( vehicle->height & chfliegend ) {
      typecostm += vehicle->armor*unitfactor/4;
   }
   // Zuschlag fuer tief fliegende Einheiten
   if ( vehicle->height & chtieffliegend ) {
      typecostm += vehicle->armor*unitfactor/4;
   }
   // Zuschlag fuer Transportkapazitaet
   if ( vehicle->entranceSystems.size() > 0 ) {
      typecostm += vehicle->maxLoadableUnits*unitfactor*10;
      // Zuschlag fr Flugzeugtraeger / Start- und Landeeinrichtungen
      if ( carrierCharge ) {
         typecostm += vehicle->maxLoadableUnits*unitfactor*100;
      }
   }
   // Zuschlag fuer Tankkapazitaet
   if ( vehicle->getStorageCapacity(0).fuel > 20000 ) {
      typecostm += (vehicle->getStorageCapacity(0).fuel-20000)*unitfactor/100;
   }
   // Zuschlag fuer Materialkapazitaet
   if ( vehicle->getStorageCapacity(0).material > 20000 ) {
      typecostm += (vehicle->getStorageCapacity(0).material-20000)*unitfactor/100;
   }
  
   // Zuschlag fuer Triebwerke
   if (maxmoverange > 40 ) {
      typecostm += (maxmoverange-40)*unitfactor*0.6;
   }
   // Zuschlag fuer Triebwerke
   if (maxmoverange > 80 ) {
      typecostm += (maxmoverange-80)*unitfactor*0.6;
   }
   // Zuschlag fuer Triebwerke 2
   if (maxmoverange > 120 ) {
      typecostm += (maxmoverange-120)*unitfactor*0.6;
   }
   // Zuschlag fuer Triebwerke 3
   if (maxmoverange > 160 ) {
      typecostm += (maxmoverange-160)*unitfactor*0.6;
   }
   // Zuschlag fuer Triebwerke 4
   if (maxmoverange > 200 ) {
      typecostm += (maxmoverange-200)*unitfactor*0.6;
   }
   // Zuschlag fuer Triebwerke 5
   if (maxmoverange > 240 ) {
      typecostm += (maxmoverange-240)*unitfactor*0.6;
   }



   // Part IV - weaponcost

   // generelle Waffenreichweite, einmaliger Zuschlag
   
   // Kostenbegrenzung
   if (maxweaponrange > 200 ) {
      maxweaponrange = 200;
   }
   // Waffenreichweitenzuschlag Kurzstrecke
   if (maxweaponrange > 19 ) {
      weaponcostm += (maxweaponrange-10)*80;
   }
   // Waffenreichweitenzuschlag Mittelstrecke
   if (maxweaponrange > 69 ) {
      weaponcostm += (maxweaponrange-60)*140;
   }
   // Waffenreichweitenzuschlag Langstrecke
   if (maxweaponrange > 99 ) {
      weaponcostm += (maxweaponrange-90)*200;
   }
   // Waffenreichweitenzuschlag Interkontinental
   if (maxweaponrange > 129 ) {
      weaponcostm += (maxweaponrange-120)*250;
   }


   if ( vehicle->weapons.count > 0 ) {
      for ( int W=0; W < vehicle->weapons.count; ++W ) {
         int weaponsinglecostm = 0;
         if (vehicle->weapons.weapon[W].getScalarWeaponType() == cwminen) {
            if ( vehicle->weapons.weapon[W].shootable() ) {
               weaponsinglecostm += vehicle->weapons.weapon[W].maxstrength*unitfactor/10;
            } else {
               weaponsinglecostm += 100;
            }
         }
         if (vehicle->weapons.weapon[W].getScalarWeaponType() == cwmachinegunn || vehicle->weapons.weapon[W].getScalarWeaponType() == cwsmallmissilen || vehicle->weapons.weapon[W].getScalarWeaponType() == cwbombn) {
            if ( vehicle->weapons.weapon[W].shootable() ) {
               weaponsinglecostm += vehicle->weapons.weapon[W].maxstrength*unitfactor/10*5;
            } else {
               weaponsinglecostm += 100;
            }
         }
         if (vehicle->weapons.weapon[W].getScalarWeaponType() == cwlargemissilen || vehicle->weapons.weapon[W].getScalarWeaponType() == cwtorpedon) {
            if ( vehicle->weapons.weapon[W].shootable() ) {
               weaponsinglecostm += vehicle->weapons.weapon[W].maxstrength*unitfactor/10*10;
            } else {
               weaponsinglecostm += 100;
            }
         }
         if (vehicle->weapons.weapon[W].getScalarWeaponType() == cwcannonn || vehicle->weapons.weapon[W].getScalarWeaponType() == cwcruisemissile) {
            if ( vehicle->weapons.weapon[W].shootable() ) {
               weaponsinglecostm += vehicle->weapons.weapon[W].maxstrength*unitfactor/10*15;
            } else {
               weaponsinglecostm += 100;
            }
         }
         if (vehicle->weapons.weapon[W].getScalarWeaponType() == cwlasern && vehicle->weapons.weapon[W].shootable() ) {
            weaponsinglecostm += vehicle->weapons.weapon[W].maxstrength*unitfactor/10*12;
         }
         if (vehicle->weapons.weapon[W].service() ) {
            weaponsinglecostm += 1000;
         }
         // Waffenreichweitenzuschlag
         weaponsinglecostm += vehicle->weapons.weapon[W].maxdistance*unitfactor/2;
         //Move during reaction fire(MDRF) - Move After Attack(MAM) - No Attack After Move(NAAM) - ReactionFire(RF)
         int weaponspecial = 0;
         int weaponRF = vehicle->weapons.weapon[W].reactionFireShots*weaponsinglecostm/2;
         int weaponMAM = maxmoverange*weaponsinglecostm/150;
         int weaponNAAM = weaponsinglecostm/2;
         int weaponMDRF = vehicle->weapons.weapon[W].reactionFireShots*maxmoverange/100;
         int weaponMDRF_J ;
         if ( vehicle->jumpDrive.height ) {
            weaponMDRF_J = vehicle->weapons.weapon[W].reactionFireShots*vehicle->jumpDrive.maxDistance/100;
         } else {
            weaponMDRF_J = 0 ;
         }
         if ( vehicle->weapons.weapon[W].shootable() ) {
            if ( vehicle->hasFunction( ContainerBaseType::MoveWithReactionFire ) ) {
               if ( vehicle->wait ) {
                  if ( vehicle->hasFunction( ContainerBaseType::MoveAfterAttack ) ) {  // MDRF+NAAM+MAM (Defkind,Spear,Stahlschwein)
                     weaponspecial += weaponRF+weaponMDRF+weaponMDRF_J+weaponMAM-weaponNAAM;
                  } else {                                 // MDRF+NAAM     (Coma, CM-U-Boote, Def-Panzer,Turrets)
                     weaponspecial += weaponRF+weaponMDRF+weaponMDRF_J-weaponNAAM;
                  }
               } else {
                  if ( vehicle->hasFunction( ContainerBaseType::MoveAfterAttack ) ) {  // MDRF+MAM      (Druk, Innocence, Skjold, PHM, Jub-O)
                     weaponspecial += weaponRF+weaponMDRF+weaponMDRF_J+weaponMAM;
                  } else {                                 // MDRF          (Schiffe, fahrende Bunker, Luftabwehrpanzer/Trooper)
                     weaponspecial += weaponRF+weaponMDRF+weaponMDRF_J;
                  }
               }
            } else {
               if ( vehicle->wait ) {
                  if ( vehicle->hasFunction( ContainerBaseType::MoveAfterAttack ) ) {  // NAAM+MAM      (Coma2, K5, PzH2000, Pulsar)
                     weaponspecial += weaponRF+weaponMAM-weaponNAAM;
                  } else {                                 // NAAM          (Coma3, BodenCMs, Schienengesch�tze)
                     weaponspecial += weaponRF-weaponNAAM;
                  }
               } else {                                    // MAM           (FAV,Flugzeug,U-Boot,WIG)
                  if ( vehicle->hasFunction( ContainerBaseType::MoveAfterAttack ) ) {
                     weaponspecial += weaponRF+weaponMAM;
                  } else {                                 // ohne alles    (Panzer/Trooper)
                     weaponspecial += weaponRF;
                  }
               }
            }
         }
         // Aufrechnung
         weaponcostm += weaponsinglecostm + weaponspecial ;
      }
   }

   // Part V Specialcost
   // stealth (typecost) oder jamming (specialcost)

   if ( vehicle->jamming > 0 && vehicle->hasFunction( ContainerBaseType::JamsOnlyOwnField ) ) {
      if (vehicle->jamming < 40 ) {
         typecostm += vehicle->jamming*unitfactor*2;  //  fuer Trooper oder eigenschaftsbedingt (klein, schnell)
      } else {
         typecostm += vehicle->jamming*unitfactor*5;  //  fuer alle hoeherwirkenden Stealthverfahren, Anstrich, besondere Konstruktion, tarnfeld usw.
      }
   } else { // JAMMING
      specialcostm += vehicle->jamming*unitfactor*20;
      // Armorzuschlag
      specialcostm += vehicle->jamming*vehicle->armor*unitfactor/100;
      // Bewegungszuschlag
      if (maxmoverange > 70 ) {
         specialcostm += vehicle->jamming*unitfactor/10*(maxmoverange-70);
      }
      if (maxmoverange > 120 ) {
         specialcostm += vehicle->jamming*unitfactor/10*(maxmoverange-120);
      }
      if (maxmoverange > 170 ) {
         specialcostm += vehicle->jamming*unitfactor/10*(maxmoverange-170);
      }
      if (maxmoverange > 200 ) {
         specialcostm += vehicle->jamming*unitfactor/10*(maxmoverange-200);
      }
   }

   // Baufunktionen
   if ( vehicle->hasFunction( ContainerBaseType::ConstructBuildings )) {
      specialcostm += 1000;
   }
   if ( vehicle->hasFunction( ContainerBaseType::InternalVehicleProduction ) || vehicle->hasFunction( ContainerBaseType::ExternalVehicleProduction )) {
      specialcostm += 1000;
   }
   if ( vehicle->objectsBuildable.size() > 0 ||vehicle-> objectGroupsBuildable.size() > 0 ) {
      specialcostm += 1000;
   }
   // Res Search
   if ( vehicle->hasFunction( ContainerBaseType::DetectsMineralResources )) {
      specialcostm += vehicle->digrange*150;
   }
   // Generator
   if ( vehicle->hasFunction( ContainerBaseType::MatterConverter) ) {
      specialcostm += 1000;
   }
   //ParaTrooper
   if ( vehicle->hasFunction( ContainerBaseType::Paratrooper ) ) {
      specialcostm += vehicle->armor*unitfactor/10*2 ;
   }
   // Reparaturfahrzeug
   if ( vehicle->hasFunction(ContainerBaseType::InternalUnitRepair) || vehicle->hasFunction(ContainerBaseType::ExternalRepair) ) {
      specialcostm += 1000;
   }
   // Selbstreparatur / Heilung
   if ( vehicle->hasFunction( ContainerBaseType::AutoRepair ) ) {
      specialcostm += vehicle->autorepairrate*vehicle->armor*unitfactor/10 / 15;
   }
   // Radar
   if ( vehicle->view > 41 ) {
      specialcostm += (vehicle->view-40)*90;
      specialcostm += maxmoverange*(vehicle->view)/4;
   }
   // Satview
   if ( vehicle->hasFunction( ContainerBaseType::SatelliteView ) ) {
      specialcostm += vehicle->view*10;
   }
   //Sonar
   if ( vehicle->hasFunction(ContainerBaseType::Sonar) && (vehicle->movemalustyp == MoveMalusType::trooper) ) {
      specialcostm += vehicle->view*5;
   } else {
      if ( vehicle->hasFunction(ContainerBaseType::Sonar)) {
         specialcostm += vehicle->view*10;
      }
   }

   if ( vehicle->jumpDrive.height ) {
      int distance = vehicle->jumpDrive.maxDistance;
      if ( distance > 1000 )
         distance = 1000;
      if ( vehicle->hasFunction( ContainerBaseType::MoveWithReactionFire ) && (maxmoverange < 30)) {
         specialcostm += distance * 20;
      } else {
         specialcostm += distance * 10;
      }
   }

   // Part VI - Addition
   res.material += (int)typecostm + weaponcostm + specialcostm;

   // Part VII Abschlaege
   // keine Luftbetankung
   if ( vehicle->hasFunction( ContainerBaseType::NoInairRefuelling )) {
      res.material -= typecostm/7;
   }

   // Kamikazeeinheiten
   if ( vehicle->hasFunction( ContainerBaseType::KamikazeOnly )) {
      res.material -= (typecostm+weaponcostm)/2;
   }

   // low movement
   if (maxmoverange < 20 ) {
      res.material -= typecostm/4;
   }

   // low movement
   if (maxmoverange < 11 ) {
      res.material -= typecostm/4;
      res.energy = res.material/20;
   } else {
      res.energy = res.material/2;
   }
   // Part VIII Abschluss

   return res;
}
