// Â© 2014 - 2017 Soverance Studios
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

	// The name of current target
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
	FText TargetName;

	// The target's level
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
	int32 TargetLevel;

	// The target's current hp
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
	float Target_CurrentHP;

	// The target's maximum hp
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
	float Target_MaxHP;

	// The active item icon
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
	FText ActiveItemName;

	// The active item icon
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
	FSlateBrush ActiveItemIcon;
};
