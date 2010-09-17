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


#include "unitcostcalculator-standard.h"

#include "vehicletype.h"

//Part II beginn calculation


ASCString StandardUnitCostCalculator::getCalculatorName()
{
   return "standard";
}

Resources StandardUnitCostCalculator :: productionCost( const VehicleType* vehicle )
{
   Resources res;
   // Anfang -> Abschluss
   res.energy = 0;
   res.material = 0;
   res.fuel = 0;
   int typecoste = 0;
   int typecostm = 0;
   int weaponcoste = 0;
   int weaponcostm = 0;
   int specialcoste = 0;
   int specialcostm = 0;
   int maxmoverange = 0;
   int maxweaponrange = 0;

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

   if ( vehicle->movemalustyp == MoveMalusType::trooper) {
      typecoste += vehicle->armor*3;
      typecostm += vehicle->armor*3;
   } else if ( vehicle->movemalustyp == MoveMalusType::light_wheeled_vehicle ) {
      typecoste += vehicle->armor*6;
      typecostm += vehicle->armor*6;
   } else if ( vehicle->movemalustyp == MoveMalusType::light_tracked_vehicle || vehicle->movemalustyp == MoveMalusType::medium_wheeled_vehicle ) {
      typecoste += vehicle->armor*7;
      typecostm += vehicle->armor*7;
   } else if ( vehicle->movemalustyp == MoveMalusType::medium_tracked_vehicle || vehicle->movemalustyp == MoveMalusType::heavy_wheeled_vehicle || vehicle->movemalustyp == MoveMalusType::rail_vehicle || vehicle->movemalustyp == MoveMalusType::structure) {
      typecoste += vehicle->armor*8;
      typecostm += vehicle->armor*8;
   } else if ( vehicle->movemalustyp == MoveMalusType::heavy_tracked_vehicle ) {
      typecoste += vehicle->armor*9;
      typecostm += vehicle->armor*9;
   } else if ( vehicle->movemalustyp == MoveMalusType::hoovercraft) {
      typecoste += vehicle->armor*9;
      typecostm += vehicle->armor*9;
   } else if ( vehicle->movemalustyp == MoveMalusType::light_ship ) {
      typecoste += vehicle->armor*8;
      typecostm += vehicle->armor*8;
   } else if ( vehicle->movemalustyp == MoveMalusType::medium_ship ) {
      typecoste += vehicle->armor*10;
      typecostm += vehicle->armor*10;
   } else if ( vehicle->movemalustyp == MoveMalusType::heavy_ship ) {
      typecoste += vehicle->armor*12;
      typecostm += vehicle->armor*12;
   } else if ( vehicle->movemalustyp == MoveMalusType::helicopter ) {
      typecoste += vehicle->armor*14;
      typecostm += vehicle->armor*14;
   } else if ( vehicle->movemalustyp == MoveMalusType::light_aircraft ) {
      typecoste += vehicle->armor*16;
      typecostm += vehicle->armor*16;
   } else if ( vehicle->movemalustyp == MoveMalusType::medium_aircraft || vehicle->movemalustyp == MoveMalusType::heavy_aircraft ) {
      typecoste += vehicle->armor*18;
      typecostm += vehicle->armor*18;

   } else {
      typecoste += vehicle->armor*6;
      typecostm += vehicle->armor*6;
   }

   // Zuschlag fuer Eisbrecher
   if ( vehicle->hasFunction( ContainerBaseType::IceBreaker ) ) {
      typecoste += vehicle->armor *2;
      typecostm += vehicle->armor *2;
   }
   // Zuschlag fuer U-Boote / Druckhuelle
   if ( vehicle->height & chgetaucht ) {
      if ( vehicle->movemalustyp == MoveMalusType::light_ship || vehicle->movemalustyp == MoveMalusType::medium_ship || vehicle->movemalustyp == MoveMalusType::heavy_ship ) {
         typecoste += vehicle->armor*2;
         typecostm += vehicle->armor*2;
      } else {
         if ( vehicle->movemalustyp == MoveMalusType::trooper ) {
            typecoste += vehicle->armor*10;
            typecostm += vehicle->armor*10;
         } else {
            typecoste += vehicle->armor*6;
            typecostm += vehicle->armor*6;
         }
      }
   }
   // Zuschlag fuer orbitalfaehige Einheiten / Druckhuelle
   if ( vehicle->height & chsatellit ) {
      typecoste += vehicle->armor*3;
      typecostm += vehicle->armor*2;
   }
   // Zuschlag fuer hochfliegende Einheiten
   if ( vehicle->height & chhochfliegend ) {
      typecoste += vehicle->armor*4;
      typecostm += vehicle->armor*4;
   }
   // Zuschlag fuer normal fliegende Einheiten
   if ( vehicle->height & chfliegend ) {
      typecoste += vehicle->armor*3;
      typecostm += vehicle->armor*3;
   }
   // Zuschlag fuer tief fliegende Einheiten
   if ( vehicle->height & chtieffliegend ) {
      typecoste += vehicle->armor*2;
      typecostm += vehicle->armor*2;
   }
   // Zuschlag fuer Transportkapazitaet
   if ( vehicle->entranceSystems.size() > 0 ) {
      typecoste += vehicle->maxLoadableUnits*100;
      typecostm += vehicle->maxLoadableUnits*100;
      // Zuschlag fr Flugzeugtraeger / Start- und Landeeinrichtungen
      if ( carrierCharge ) {
         typecoste += vehicle->maxLoadableUnits*1000;
         typecostm += vehicle->maxLoadableUnits*1000;
      }
   }
   // Zuschlag fuer Triebwerke
   if (maxmoverange > 60 ) {
      typecoste += (maxmoverange-60)*10;
      typecostm += (maxmoverange-60)*5;
   }
   // Zuschlag fr Flugzeugtriebwerke
   if (maxmoverange > 110 ) {
      typecoste += (maxmoverange-110)*10;
      typecostm += (maxmoverange-110)*5;
   }
   // Zuschlag fr Hochleistungsflugzeugtriebwerke
   if (maxmoverange > 150 ) {
      typecoste += (maxmoverange-150)*10;
      typecostm += (maxmoverange-150)*5;
   }
   // Zuschlag fr Spezialflugzeugtriebwerke
   if (maxmoverange > 190 ) {
      typecoste += (maxmoverange-190)*10;
      typecostm += (maxmoverange-190)*5;
   }



   // Part IV - weaponcost

   if ( vehicle->weapons.count > 0 ) {
      for ( int W=0; W < vehicle->weapons.count; ++W ) {
         int weaponsinglecoste = 0;
         int weaponsinglecostm = 0;
         if (vehicle->weapons.weapon[W].getScalarWeaponType() == cwminen) {
            if ( vehicle->weapons.weapon[W].shootable() ) {
               weaponsinglecoste += vehicle->weapons.weapon[W].maxstrength;
               weaponsinglecostm += vehicle->weapons.weapon[W].maxstrength;
            } else {
               weaponsinglecoste += 100;
               weaponsinglecostm += 50;
            }
         }
         if (vehicle->weapons.weapon[W].getScalarWeaponType() == cwmachinegunn || vehicle->weapons.weapon[W].getScalarWeaponType() == cwsmallmissilen || vehicle->weapons.weapon[W].getScalarWeaponType() == cwbombn) {
            if ( vehicle->weapons.weapon[W].shootable() ) {
               weaponsinglecoste += vehicle->weapons.weapon[W].maxstrength*5;
               weaponsinglecostm += vehicle->weapons.weapon[W].maxstrength*5;
            } else {
               weaponsinglecoste += 100;
               weaponsinglecostm += 50;
            }
         }
         if (vehicle->weapons.weapon[W].getScalarWeaponType() == cwlargemissilen || vehicle->weapons.weapon[W].getScalarWeaponType() == cwtorpedon) {
            if ( vehicle->weapons.weapon[W].shootable() ) {
               weaponsinglecoste += vehicle->weapons.weapon[W].maxstrength*10;
               weaponsinglecostm += vehicle->weapons.weapon[W].maxstrength*10;
            } else {
               weaponsinglecoste += 100;
               weaponsinglecostm += 50;
            }
         }
         if (vehicle->weapons.weapon[W].getScalarWeaponType() == cwcannonn || vehicle->weapons.weapon[W].getScalarWeaponType() == cwcruisemissile) {
            if ( vehicle->weapons.weapon[W].shootable() ) {
               weaponsinglecoste += vehicle->weapons.weapon[W].maxstrength*15;
               weaponsinglecostm += vehicle->weapons.weapon[W].maxstrength*15;
            } else {
               weaponsinglecoste += 100;
               weaponsinglecostm += 50;
            }
         }
         if (vehicle->weapons.weapon[W].getScalarWeaponType() == cwlasern && vehicle->weapons.weapon[W].shootable() ) {
            weaponsinglecoste += vehicle->weapons.weapon[W].maxstrength*12;
            weaponsinglecostm += vehicle->weapons.weapon[W].maxstrength*10;
         }
         if (vehicle->weapons.weapon[W].service() ) {
            weaponsinglecoste += 1000;
            weaponsinglecostm += 500;
         }
         if (vehicle->weapons.weapon[W].canRefuel() ) {
            weaponsinglecoste += 100;
            weaponsinglecostm += 50;
         }
         // Waffenreichweitenzuschlag Kurzstrecke
         if (vehicle->weapons.weapon[W].maxdistance > 19 ) {
            weaponsinglecoste += (vehicle->weapons.weapon[W].maxdistance-10)*80;
            weaponsinglecostm += (vehicle->weapons.weapon[W].maxdistance-10)*80;
         }
         // Waffenreichweitenzuschlag Mittelstrecke
         if (vehicle->weapons.weapon[W].maxdistance > 69 ) {
            weaponsinglecoste += (vehicle->weapons.weapon[W].maxdistance-60)*150;
            weaponsinglecostm += (vehicle->weapons.weapon[W].maxdistance-60)*140;
         }
         // Waffenreichweitenzuschlag Langstrecke
         if (vehicle->weapons.weapon[W].maxdistance > 99 ) {
            weaponsinglecoste += (vehicle->weapons.weapon[W].maxdistance-90)*220;
            weaponsinglecostm += (vehicle->weapons.weapon[W].maxdistance-90)*200;
         }
         // Waffenreichweitenzuschlag Interkontinental
         if (vehicle->weapons.weapon[W].maxdistance > 129 ) {
            weaponsinglecoste += (vehicle->weapons.weapon[W].maxdistance-120)*250;
            weaponsinglecostm += (vehicle->weapons.weapon[W].maxdistance-120)*250;
         }
         //Move during reaction fire(MDRF) - Move After Attack(MAM) - No Attack After Move(NAAM) - ReactionFire(RF)
         int weaponspecial = 0;
         int weaponRF = vehicle->weapons.weapon[W].reactionFireShots*weaponsinglecostm/10;
         int weaponMAM = maxmoverange*weaponsinglecostm/500;
         int weaponNAAM = weaponsinglecostm/4;
         int weaponMDRF = vehicle->weapons.weapon[W].reactionFireShots*weaponsinglecostm*maxmoverange/400;

         if ( vehicle->weapons.weapon[W].shootable() ) {
            if ( vehicle->hasFunction( ContainerBaseType::MoveWithReactionFire ) ) {
               if ( vehicle->wait ) {
                  if ( vehicle->hasFunction( ContainerBaseType::MoveAfterAttack ) ) {  // MDRF+NAAM+MAM (Defkind,Spear,Stahlschwein)
                     weaponspecial += weaponMDRF+weaponMAM-weaponNAAM;
                  } else {                                 // MDRF+NAAM     (Coma, CM-U-Boote, Def-Panzer,Turrets)
                     weaponspecial += weaponMDRF-weaponNAAM;
                  }
               } else {
                  if ( vehicle->hasFunction( ContainerBaseType::MoveAfterAttack ) ) {  // MDRF+MAM      (Druk, Innocence, Skjold, PHM, Jub-O)
                     weaponspecial += weaponMDRF+weaponMAM;
                  } else {                                 // MDRF          (Schiffe, fahrende Bunker, Luftabwehrpanzer/Trooper)
                     weaponspecial += weaponMDRF;
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
         weaponcoste += weaponsinglecoste + weaponspecial ;
         weaponcostm += weaponsinglecostm + weaponspecial ;
      }
   }

   // Part V Specialcost
   // stealth (typecost) oder jamming (specialcost)

   if ( vehicle->jamming > 0 && vehicle->hasFunction( ContainerBaseType::JamsOnlyOwnField ) ) {
      if (vehicle->jamming < 31 ) {
         typecoste += vehicle->jamming*20;  //  fuer Trooper oder eigenschaftsbedingt (klein, schnell)
         typecostm += vehicle->jamming*10;
      } else {
         typecoste += vehicle->jamming*60;  //  fuer alle hoeherwirkenden Stealthverfahren, Anstrich, besondere Konstruktion, tarnfeld usw.
         typecostm += vehicle->jamming*50;
      }
   } else { // JAMMING
      specialcoste += vehicle->jamming*200;
      specialcostm += vehicle->jamming*150;
      // Armorzuschlag
      specialcostm += vehicle->jamming*vehicle->armor/10;
      // Bewegungszuschlag
      if (maxmoverange > 70 ) {
         specialcostm += vehicle->jamming*(maxmoverange-70);
      }
      if (maxmoverange > 120 ) {
         specialcostm += vehicle->jamming*(maxmoverange-120);
      }
      if (maxmoverange > 170 ) {
         specialcostm += vehicle->jamming*(maxmoverange-170);
      }
      if (maxmoverange > 200 ) {
         specialcostm += vehicle->jamming*(maxmoverange-200);
      }
   }

   // Baufunktionen
   if ( vehicle->hasFunction( ContainerBaseType::ConstructBuildings )) {
      specialcoste += 1000;
      specialcostm += 500;
   }
   if ( vehicle->hasFunction( ContainerBaseType::InternalVehicleProduction ) || vehicle->hasFunction( ContainerBaseType::ExternalVehicleProduction )) {
      specialcoste += 1000;
      specialcostm += 500;
   }
   if ( vehicle->objectsBuildable.size() > 0 ||vehicle-> objectGroupsBuildable.size() > 0 ) {
      specialcoste += 1000;
      specialcostm += 500;
   }
   // Res Search
   if ( vehicle->hasFunction( ContainerBaseType::DetectsMineralResources )) {
      specialcoste += vehicle->digrange*150;
      specialcostm += vehicle->digrange*100;
   }
   // Generator
   if ( vehicle->hasFunction( ContainerBaseType::MatterConverter) ) {
      specialcoste += 1000;
      specialcostm += 500;
   }
   //ParaTrooper
   if ( vehicle->hasFunction( ContainerBaseType::Paratrooper ) ) {
      specialcoste += vehicle->armor*2 ;
      specialcostm += vehicle->armor ;
   }
   // Reparaturfahrzeug
   if ( vehicle->hasFunction(ContainerBaseType::InternalUnitRepair) || vehicle->hasFunction(ContainerBaseType::ExternalRepair) ) {
      specialcoste += 1000;
      specialcostm += 500;
   }
   // Selbstreparatur / Heilung
   if ( vehicle->hasFunction( ContainerBaseType::AutoRepair ) ) {
      specialcoste += vehicle->autorepairrate*vehicle->armor / 15;
      specialcostm += vehicle->autorepairrate*vehicle->armor / 15;
   }
   // Radar
   if ( vehicle->view > 40 ) {
      specialcoste += (vehicle->view-40)*90;
      specialcostm += (vehicle->view-40)*75;
      specialcoste += maxmoverange*(vehicle->view-40)/3;
      specialcostm += maxmoverange*(vehicle->view-40)/5;
   }
   // Satview
   if ( vehicle->hasFunction( ContainerBaseType::SatelliteView ) ) {
      specialcoste += vehicle->view*20;
      specialcostm += vehicle->view*8;
   }
   //Sonar
   if ( vehicle->hasFunction(ContainerBaseType::Sonar) && (vehicle->movemalustyp == MoveMalusType::trooper) ) {
      specialcoste += vehicle->view*10;
      specialcostm += vehicle->view*4;
   } else {
      if ( vehicle->hasFunction(ContainerBaseType::Sonar)) {
         specialcoste += vehicle->view*20;
         specialcostm += vehicle->view*10;
      }
   }
   //Move during reaction fire
//   if ( vehicle->hasFunction( MoveWithReactionFire )) {
//      int rfweapcount = 0;
//      for ( int i = 0; i < vehicle->weapons.count; ++i )
//         if ( vehicle->weapons.weapon[i].shootable() )
//            rfweapcount += vehicle->weapons.weapon[i].reactionFireShots;

//      specialcoste += rfweapcount * 100;
//      specialcostm += rfweapcount * 50;
//   }

   //move after attack
//   if ( vehicle->hasFunction( MoveAfterAttack ) ) {
//      specialcoste += maxmoverange*10;
//      specialcostm += maxmoverange*5;
//   }

   if ( vehicle->jumpDrive.height ) {
      int distance = vehicle->jumpDrive.maxDistance;
      if ( distance > 1000 )
         distance = 1000;
      if ( vehicle->hasFunction( ContainerBaseType::MoveWithReactionFire ) && (maxmoverange < 30)) {
         specialcoste += distance * 14;
         specialcostm += distance * 12;
      } else {
         specialcoste += distance * 7;
         specialcostm += distance * 5;
      }
   }

   // Part VI - Addition
   res.energy += typecoste + weaponcoste + specialcoste;
   res.material += typecostm + weaponcostm + specialcostm;

   // Part VII Abschlaege
   // keine Luftbetankung
   if ( vehicle->hasFunction( ContainerBaseType::NoInairRefuelling )) {
      res.energy -= typecoste/6;
      res.material -= typecostm/6;
   }

   // Kamikazeeinheiten
   if ( vehicle->hasFunction( ContainerBaseType::KamikazeOnly )) {
      res.energy -= (typecoste+weaponcoste)/2;
      res.material -= (typecostm+weaponcostm)/2;
   }

   // low movement
   if (maxmoverange < 20 ) {
      res.energy -= typecoste/4;
      res.material -= typecostm/4;
   }

   // low movement
   if (maxmoverange < 10 ) {
      res.energy -= typecoste/4;
      res.material -= typecostm/4;
   }
   // Part VIII Abschluss

   return res;
}
