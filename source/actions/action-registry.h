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


#ifndef actionRegistryH
#define actionRegistryH

namespace ActionRegistry {
   //! Every action must have a unique ID. Adding the actions to this enum ensures it
   enum Actions { Root, 
                  ConsumeAmmo,
                  InflictDamage,
                  SpawnObject,
                  DestructContainer,
                  VehicleAttack,
                  ChangeUnitMovement,
                  ChangeUnitAttackFlag,
                  ChangeUnitProperty,
                  RegisterUnitRFTarget,
                  MoveUnit,                  // 10
                  UnitFieldRegistration,
                  ConsumeResource,
                  ChangeContainerProperty,
                  ChangeView,
                  RemoveMine,
                  AttackCommand,
                  MoveUnitCommand,
                  RemoveObject,
                  ChangeObjectProperty,
                  ConvertContainer,         // 20
                  PutObjectCommand,
                  PutMineCommand,
                  SpawnMine,
                  DiscoverResources,
                  ViewRegistration,
                  ConstructUnitCommand,
                  SpawnUnit,
                  CargoMoveCommand,
                  ServiceCommand,
                  RecycleUnitCommand,      // 30
                  DestructUnitCommand,
                  ReactionFireSwitchCommand,
                  RepairUnitCommand,
                  ConstructBuildingCommand,
                  SpawnBuilding,
                  DestructBuildingCommand,
                  JumpDriveCommand,
                  PowerGenerationSwitchCommand,
                  TrainUnitCommand,
                  RepairBuildingCommand, // 40
                  TransferControlCommand,
                  BuildProductionLineCommand,
                  RemoveProductionLineCommand,
                  CancelResearchCommand,
                  SetResourceProcessingRateCommand,
                  DiplomacyCommand,
                  ChangeDiplomaticState,
                  DirectResearchCommand,
                  InternalAmmoTransferCommand,
                  RenameContainerCommand
                        
   };
};

#endif

