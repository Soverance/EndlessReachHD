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

#pragma once

#include "Blueprint/UserWidget.h"
#include "PlayerHUD.generated.h"

UCLASS()
class ENDLESSREACHHD_API UPlayerHUD : public UUserWidget
{
	GENERATED_BODY()
	
public:
	
	UPlayerHUD(const FObjectInitializer& ObjectInitializer);

	// The player's current hp
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Player)
	float Player_CurrentHP;

	// The player's maximum hp
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Player)
	float Player_MaxHP;

	// The player's current fuel
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Player)
	float Player_CurrentFuel;

	// The player's maximum fuel
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Player)
	float Player_MaxFuel;

	// The number of orbs the player has collected since the last time they returned to the outpost
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	FText Player_OrbCount;

	// The name of current target
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Target)
	FText TargetName;

	// The target's level
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Target)
	int32 TargetLevel;

	// The target's current hp
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Target)
	float Target_CurrentHP;

	// The target's maximum hp
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Target)
	float Target_MaxHP;

	// The active item icon
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	FText ActiveItemName;

	// The active item icon
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	FSlateBrush ActiveItemIcon;

	// The active item icon
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Save)
	ESlateVisibility SavingIndicatorVisibility;

	// Destroy them with lasers!
	UFUNCTION(BlueprintImplementableEvent, Category = Laser)
	void ChargeLaser_Stage1();
	UFUNCTION(BlueprintImplementableEvent, Category = Laser)
	void ChargeLaser_Stage2();
	UFUNCTION(BlueprintImplementableEvent, Category = Laser)
	void ChargeLaser_Stage3();
	UFUNCTION(BlueprintImplementableEvent, Category = Laser)
	void ChargeLaser_Stage4();
	UFUNCTION(BlueprintImplementableEvent, Category = Laser)
	void ChargeLaser_Stage5();

	// Stop destroying them with lasers!
	UFUNCTION(BlueprintImplementableEvent, Category = Laser)
	void DischargeLaser_Stage1();
	UFUNCTION(BlueprintImplementableEvent, Category = Laser)
	void DischargeLaser_Stage2();
	UFUNCTION(BlueprintImplementableEvent, Category = Laser)
	void DischargeLaser_Stage3();
	UFUNCTION(BlueprintImplementableEvent, Category = Laser)
	void DischargeLaser_Stage4();
	UFUNCTION(BlueprintImplementableEvent, Category = Laser)
	void DischargeLaser_Stage5();
};
