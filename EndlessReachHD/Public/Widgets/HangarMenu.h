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
#include "PlayerStatAttribute.h"
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
};
