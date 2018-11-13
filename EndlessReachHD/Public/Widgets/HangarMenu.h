// 2012 - 2019 Soverance Studios
// https://soverance.com

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

// http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include "Blueprint/UserWidget.h"
#include "PlayerStatAttribute.h"
#include "Environment/Hangar.h"
#include "Management/CommonLibrary.h"
#include "HangarMenu.generated.h"

UCLASS()
class ENDLESSREACHHD_API UHangarMenu : public UUserWidget
{
	GENERATED_BODY()
	
public:
	
	UHangarMenu(const FObjectInitializer& ObjectInitializer);

	// Player Stat Attributes Array
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
	TArray<FPlayerStatAttribute> Attributes;

	// The Ship Type Attribute - used to determine the ship's physical appearance
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
	FPlayerStatAttribute ShipType;

	// The Health Attribute - used to upgrade the ship's maximum health value
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
	FPlayerStatAttribute Health;

	// The Thruster Attribute - used to upgrade the ship's thrusters
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
	FPlayerStatAttribute Thruster;

	// The Cannon Attribute - used to upgrade the ship's main cannons
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
	FPlayerStatAttribute Cannon;

	// The Laser Attribute - used to upgrade the ship's forward laser
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
	FPlayerStatAttribute Laser;

	// The Magnet Attribute - used to upgrade the ship's magnet feature
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
	FPlayerStatAttribute Magnet;

	// The Missiles Attribute - used to upgrade the ship's homing missles
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
	FPlayerStatAttribute Missiles;

	// The Shield Attribute - used to upgrade the ship's energy shield
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
	FPlayerStatAttribute Shield;

	// The Bomb Attribute - used to upgrade the ship's nuclear bomb
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Attributes)
	FPlayerStatAttribute Bomb;

	// The number of orbs the player has collected since the last time they returned to the outpost
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Player)
	FText Player_OrbCount;

	// The index of the menu, indicating which panel is currently active
	UPROPERTY(Category = Controls, EditAnywhere, BlueprintReadWrite)
	int32 MenuIndex;

	/* Whether the hanger menu can be navigated; i.e., the player is allowed to press left or right to move the menu panels in that direction */
	UPROPERTY(Category = Controls, EditAnywhere, BlueprintReadWrite)
	bool bCanNavigate;

	// Move the menu index to the left by one
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = Controls)
	void MoveLeft();

	// Move the menu index to the right by one
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = Controls)
	void MoveRight();

	// Move the menu to the appropriate index
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = Controls)
	void SetMenuIndex();

	// Reconfigures the display of the corresponding feature to match the current upgrade level
	UFUNCTION(BlueprintCallable, Category = Controls)
	void SetUpgradeLevel(int32 Level, int32 UpgradeCost);

	// Plays the appropriate upgrade UI animation
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = Controls)
	void PlayUpgradeAnim(int32 Level, int32 NewCost);

	// Called to notify the user if the menu experiences an error
	UFUNCTION(BlueprintCallable, Category = Controls)
	void NotifyError();

	/** Sound to play when the menu encounters an error */
	UPROPERTY(Category = Audio, EditAnywhere, BlueprintReadWrite)
	USoundCue* S_ErrorNotify;

	/** Sound to play when the menu switches panels */
	UPROPERTY(Category = Audio, EditAnywhere, BlueprintReadWrite)
	USoundCue* S_CursorNotify;

	/** Sound to play when an upgrade occurs */
	UPROPERTY(Category = Audio, EditAnywhere, BlueprintReadWrite)
	USoundCue* S_UpgradeNotify;

	/** Reference to the Hangar object itself in game */
	UPROPERTY(Category = Audio, EditAnywhere, BlueprintReadWrite)
	AHangar* Hangar;

	// Prompt the player to exit the hangar
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = Controls)
	void PromptExit();

	// Returns to the upgrade menu from the exit prompt, when the player selects "NO"
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = Controls)
	void ReturnToUpgradeMenu();

	// The index of the exit prompt menu - basically just yes or no, so it's binary  (0 = yes, 1 = no)
	UPROPERTY(Category = Controls, EditAnywhere, BlueprintReadWrite)
	int32 ExitPromptIndex;

	/* Whether the hanger menu is prompting for exit */
	UPROPERTY(Category = Controls, EditAnywhere, BlueprintReadWrite)
	bool bIsPromptingExit;

	// Begin the launch sequence to exit the hangar
	UFUNCTION(BlueprintCallable, Category = Controls)
	void InitLaunchSequence();
};
