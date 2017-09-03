// 2014 - 2017 Soverance Studios
// http://www.soverance.com

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

// http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "EndlessReachHD.h"
#include "HangarMenu.h"

#define LOCTEXT_NAMESPACE "EndlessReachHD-TextLibrary"

UHangarMenu::UHangarMenu(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// ICONS
	//static ConstructorHelpers::FObjectFinder<UTexture2D> WaterIcon(TEXT("Texture2D'/Game/Blueprints/Widgets/UI-Images/Icons_Levels/LevelIcon-Water.LevelIcon-Water'"));
	// REALM PREVIEWS
	//static ConstructorHelpers::FObjectFinder<UTexture2D> ShiitakePreview(TEXT("Texture2D'/Game/Blueprints/Widgets/UI-Images/Icons_Levels/LevelBackground_ShiitakeTemple.LevelBackground_ShiitakeTemple'"));

	ShipType.Name = LOCTEXT("AttributeName_ShipType", "Type");
	ShipType.Locked = false;
	ShipType.UpgradeCost = 1000;
	ShipType.CurrentPower = 0;
	ShipType.Description = LOCTEXT("AttributeDescription_ShipType", "Starship model trim type.  ");
	Attributes.Add(ShipType);

	Health.Name = LOCTEXT("AttributeName_Health", "Health");
	Health.Locked = false;
	Health.UpgradeCost = 1000;
	Health.CurrentPower = 0;
	Health.Description = LOCTEXT("AttributeDescription_Health", "The ship's health value.  Upgrading this attribute will increase the amount of damage the ship can take before destruction.");
	Attributes.Add(Health);

	Thruster.Name = LOCTEXT("AttributeName_Thruster", "Thruster");
	Thruster.Locked = false;
	Thruster.UpgradeCost = 1000;
	Thruster.CurrentPower = 0;
	Thruster.Description = LOCTEXT("AttributeDescription_Thruster", "The ship's main thruster.  Upgrading this attribute will increase the ship's fuel economy.");
	Attributes.Add(Health);

	Cannon.Name = LOCTEXT("AttributeName_Cannon", "Cannon");
	Cannon.Locked = false;
	Cannon.UpgradeCost = 1000;
	Cannon.CurrentPower = 0;
	Cannon.Description = LOCTEXT("AttributeDescription_Cannon", "The ship's main cannon.  Upgrading this attribute will increase the cannon's firing rate.");
	Attributes.Add(Cannon);

	Laser.Name = LOCTEXT("AttributeName_Laser", "Laser");
	Laser.Locked = false;
	Laser.UpgradeCost = 1000;
	Laser.CurrentPower = 0;
	Laser.Description = LOCTEXT("AttributeDescription_Laser", "The ship's forward laser.  Can only be fired if one or more charges are available.  Upgrading this attribute will increase the laser's damage output.");
	Attributes.Add(Laser);

	Magnet.Name = LOCTEXT("AttributeName_Magnet", "Magnet");
	Magnet.Locked = false;
	Magnet.UpgradeCost = 1000;
	Magnet.CurrentPower = 0;
	Magnet.Description = LOCTEXT("AttributeDescription_Magnet", "The ship's magnet automatically draws in pickups, making them easier to collect.  Upgrading this attribute will increase the magent's range.");
	Attributes.Add(Magnet);

	Missiles.Name = LOCTEXT("AttributeName_Missiles", "Missiles");
	Missiles.Locked = false;
	Missiles.UpgradeCost = 1000;
	Missiles.CurrentPower = 0;
	Missiles.Description = LOCTEXT("AttributeDescription_Missiles", "The ship's automatic homing missle feature.  Upgrading this attribute will increase the effectiveness of homing missles.");
	Attributes.Add(Missiles);

	Shield.Name = LOCTEXT("AttributeName_Shield", "Shield");
	Shield.Locked = false;
	Shield.UpgradeCost = 1000;
	Shield.CurrentPower = 0;
	Shield.Description = LOCTEXT("AttributeDescription_Shield", "The ship's energy shield.  Upgrading this attribute will increase shield's durability.");
	Attributes.Add(Shield);

	Bomb.Name = LOCTEXT("AttributeName_Bomb", "Bomb");
	Bomb.Locked = false;
	Bomb.UpgradeCost = 1000;
	Bomb.CurrentPower = 0;
	Bomb.Description = LOCTEXT("AttributeDescription_Bomb", "The ship's nuclear bomb, which destroys all enemies (excluding bosses).  Upgrading this attribute will increase the maximum number of bombs the ship is capable of carrying.");
	Attributes.Add(Bomb);
}

#undef LOCTEXT_NAMESPACE
