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
	// DEFAULTS
	MenuIndex = 0;

	// ICONS
	static ConstructorHelpers::FObjectFinder<UTexture2D> HealthIcon(TEXT("Texture2D'/Game/Widgets/Images/Artifact_Health.Artifact_Health'"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> ThrusterIcon(TEXT("Texture2D'/Game/Widgets/Images/Artifact_Thrusters.Artifact_Thrusters'"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> CannonIcon(TEXT("Texture2D'/Game/Widgets/Images/Artifact_Cannon.Artifact_Cannon'"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> LaserIcon(TEXT("Texture2D'/Game/Widgets/Images/Artifact_Laser.Artifact_Laser'"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> MagnetIcon(TEXT("Texture2D'/Game/Widgets/Images/Artifact_Magnet.Artifact_Magnet'"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> MissileIcon(TEXT("Texture2D'/Game/Widgets/Images/Artifact_Missile.Artifact_Missile'"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> ShieldIcon(TEXT("Texture2D'/Game/Widgets/Images/Artifact_Shield.Artifact_Shield'"));
	static ConstructorHelpers::FObjectFinder<UTexture2D> BombIcon(TEXT("Texture2D'/Game/Widgets/Images/Artifact_Bomb.Artifact_Bomb'"));
	
	// PANEL DESCRIPTIONS
	// Ship Type - INDEX 0
	ShipType.Name = LOCTEXT("AttributeName_ShipType", "Type");
	ShipType.Locked = false;
	ShipType.UpgradeCost = 1000;
	ShipType.CurrentPower = 0;
	ShipType.UnlockDescription = LOCTEXT("AttributeDescription_UnlockShipType", "Improve the starship model.");
	ShipType.Description = LOCTEXT("AttributeDescription_ShipType", "Improve the starship model.");
	Attributes.Add(ShipType);
	// Health - INDEX 1
	Health.Name = LOCTEXT("AttributeName_Health", "Health");
	Health.Locked = false;
	Health.UpgradeCost = 250;
	Health.CurrentPower = 0;
	Health.UnlockDescription = LOCTEXT("AttributeDescription_UnlockHealth", "Increase the amount of damage the ship can take before destruction.");
	Health.Description = LOCTEXT("AttributeDescription_Health", "Increase the amount of damage the ship can take before destruction.");
	Health.Icon = HealthIcon.Object;
	Attributes.Add(Health);
	// Thrusters - INDEX 2
	Thruster.Name = LOCTEXT("AttributeName_Thruster", "Thruster");
	Thruster.Locked = false;
	Thruster.UpgradeCost = 250;
	Thruster.CurrentPower = 0;
	Thruster.UnlockDescription = LOCTEXT("AttributeDescription_UnlockThruster", "Increase the ship's fuel economy.");
	Thruster.Description = LOCTEXT("AttributeDescription_Thruster", "Increase the ship's fuel economy.");
	Thruster.Icon = ThrusterIcon.Object;
	Attributes.Add(Health);
	// Cannon - INDEX 3
	Cannon.Name = LOCTEXT("AttributeName_Cannon", "Cannon");
	Cannon.Locked = false;
	Cannon.UpgradeCost = 250;
	Cannon.CurrentPower = 0;
	Cannon.UnlockDescription = LOCTEXT("AttributeDescription_UnlockCannon", "Increase the firing rate of the main cannon.");
	Cannon.Description = LOCTEXT("AttributeDescription_Cannon", "Increase the firing rate of the main cannon.");
	Cannon.Icon = CannonIcon.Object;
	Attributes.Add(Cannon);
	// Laser - INDEX 4
	Laser.Name = LOCTEXT("AttributeName_Laser", "Laser");
	Laser.Locked = false;
	Laser.UpgradeCost = 500;
	Laser.CurrentPower = 0;
	Laser.UnlockDescription = LOCTEXT("AttributeDescription_UnlockLaser", "Unlock the laser, a heavy damage weapon. Can only be fired in the forward direction, and only if one of more charges is available.");
	Laser.Description = LOCTEXT("AttributeDescription_Laser", "Increase the laser's damage output.  Can only be fired if one or more charges are available.");
	Laser.Icon = LaserIcon.Object;
	Attributes.Add(Laser);
	// Magnet - INDEX 5
	Magnet.Name = LOCTEXT("AttributeName_Magnet", "Magnet");
	Magnet.Locked = false;
	Magnet.UpgradeCost = 500;
	Magnet.CurrentPower = 0;
	Magnet.UnlockDescription = LOCTEXT("AttributeDescription_UnlockMagnet", "Unlock the magnet, which automatically draws in pickups, making them easier to collect.");
	Magnet.Description = LOCTEXT("AttributeDescription_Magnet", "Increase the magent's radius.");
	Magnet.Icon = MagnetIcon.Object;
	Attributes.Add(Magnet);
	// Missiles - INDEX 6
	Missiles.Name = LOCTEXT("AttributeName_Missiles", "Missiles");
	Missiles.Locked = false;
	Missiles.UpgradeCost = 750;
	Missiles.CurrentPower = 0;
	Missiles.UnlockDescription = LOCTEXT("AttributeDescription_UnlockMissiles", "Unlock homing missiles, which are fired automatically.");
	Missiles.Description = LOCTEXT("AttributeDescription_Missiles", "Increase the effectiveness of homing missles.");
	Missiles.Icon = MissileIcon.Object;
	Attributes.Add(Missiles);
	// Shield - INDEX 7
	Shield.Name = LOCTEXT("AttributeName_Shield", "Shield");
	Shield.Locked = false;
	Shield.UpgradeCost = 750;
	Shield.CurrentPower = 0;
	Shield.UnlockDescription = LOCTEXT("AttributeDescription_UnlockShield", "Unlock the energy shield, which will help protect your ship from damage.");
	Shield.Description = LOCTEXT("AttributeDescription_Shield", "Increase the energy shield's durability.");
	Shield.Icon = ShieldIcon.Object;
	Attributes.Add(Shield);
	// Bomb - INDEX 8
	Bomb.Name = LOCTEXT("AttributeName_Bomb", "Bomb");
	Bomb.Locked = false;
	Bomb.UpgradeCost = 1000;
	Bomb.CurrentPower = 0;
	Bomb.UnlockDescription = LOCTEXT("AttributeDescription_UnlockBomb", "Unlock atomic bombs, which immediately destroy all targets in range (excluding bosses).");
	Bomb.Description = LOCTEXT("AttributeDescription_Bomb", "Increase the maximum number of bombs the ship is capable of carrying.");
	Bomb.Icon = BombIcon.Object;
	Attributes.Add(Bomb);

	// Menu Error Audio Notification
	static ConstructorHelpers::FObjectFinder<USoundCue> ErrorAudio(TEXT("SoundCue'/Game/Audio/Menu/Ethereal_MenuError_Cue.Ethereal_MenuError_Cue'"));
	S_ErrorNotify = ErrorAudio.Object;

	// Menu Cursor Audio Notification
	static ConstructorHelpers::FObjectFinder<USoundCue> CursorAudio(TEXT("SoundCue'/Game/Audio/Menu/Ethereal_MenuCursor_Cue.Ethereal_MenuCursor_Cue'"));
	S_CursorNotify = CursorAudio.Object;

	// Menu Upgrade Audio Notification
	static ConstructorHelpers::FObjectFinder<USoundCue> UpgradeAudio(TEXT("SoundCue'/Game/Audio/Menu/Menu_Upgrade_Cue.Menu_Upgrade_Cue'"));
	S_UpgradeNotify = UpgradeAudio.Object;
}

void UHangarMenu::MoveLeft_Implementation()
{

}

void UHangarMenu::MoveRight_Implementation()
{

}

void UHangarMenu::SetMenuIndex_Implementation()
{

}

void UHangarMenu::NotifyError()
{
	PlaySound(S_ErrorNotify);
}

void UHangarMenu::SetUpgradeLevel(int32 Level, int32 UpgradeCost)
{
	PlaySound(S_UpgradeNotify);

	switch (MenuIndex)
	{
		// SHIP TYPE
		case 0:
			Attributes[0].UpgradeCost = UpgradeCost;
			Attributes[0].CurrentPower = Level;
			break;
		// HEALTH
		case 1:
			Attributes[1].UpgradeCost = UpgradeCost;
			Attributes[1].CurrentPower = Level;
			break;
		// THRUSTERS
		case 2:
			Attributes[2].UpgradeCost = UpgradeCost;
			Attributes[2].CurrentPower = Level;
			break;
		// MAIN CANNON
		case 3:
			Attributes[3].UpgradeCost = UpgradeCost;
			Attributes[3].CurrentPower = Level;
			break;
		// LASER
		case 4:
			Attributes[4].UpgradeCost = UpgradeCost;
			Attributes[4].CurrentPower = Level;
			break;
		// MAGNET
		case 5:
			Attributes[5].UpgradeCost = UpgradeCost;
			Attributes[5].CurrentPower = Level;
			break;
		// MISSILES
		case 6:
			Attributes[6].UpgradeCost = UpgradeCost;
			Attributes[6].CurrentPower = Level;
			break;
		// ENERGY SHIELD
		case 7:
			Attributes[7].UpgradeCost = UpgradeCost;
			Attributes[7].CurrentPower = Level;
			break;
		// BOMB
		case 8:
			Attributes[8].UpgradeCost = UpgradeCost;
			Attributes[8].CurrentPower = Level;
			break;
	}
}

#undef LOCTEXT_NAMESPACE
