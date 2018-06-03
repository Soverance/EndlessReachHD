// © 2014 - 2017 Soverance Studios
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

#include "GameFramework/GameModeBase.h"
#include "Management/Maps/LevelNode.h"
#include "EndlessReachHDGameMode.generated.h"


UCLASS(minimalapi)
class AEndlessReachHDGameMode : public AGameModeBase
{
	GENERATED_BODY()

private:

	/** Contains all the available transforms of LevelMovePointActors. */
	TArray<FTransform> NodeTransforms;

	/** Map name to spawn */
	FName MapName;

	/** The desired transform of our map */
	FTransform MapTransform;

	/** Loads the desired streaming level based on the MapName and the MapTransform */
	UFUNCTION()
	void LoadMap();

public:
	AEndlessReachHDGameMode();

	// Loads a streaming level into the persistent map.
	// param MapName - the name of the map we want to move
	// param Position - the index of the Transforms array we want to use
	UFUNCTION(Exec, Category = LevelStreaming)
	void ReloadMap(FName MapToReload, int32 Position);

	// Unloads a streaming level.
	// param MapName - the name of the map we want to move
	// param Position - the index of the Transforms array we want to use
	UFUNCTION(Exec, Category = LevelStreaming)
	void UnloadMap(FName MapToUnload, int32 Position);

	// Get the streaming name of the specified map
	// param Position - the index of the Transforms array we want to use
	UFUNCTION(Exec, Category = LevelStreaming)
	FName GetMapName(int32 Position);

	/** The number of the currently loaded map */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Data)
	int32 CurrentMap;

	/** An array of level nodes */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Data)
	TArray<ALevelNode*> LevelNodes;

	virtual void BeginPlay() override;
};



