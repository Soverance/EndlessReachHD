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

#include "EndlessReachHD.h"
#include "EndlessReachHDGameMode.h"
#include "Management/Maps/LevelNode.h"
#include "EndlessReachHDPawn.h"

AEndlessReachHDGameMode::AEndlessReachHDGameMode()
{
	// set default pawn
	DefaultPawnClass = AEndlessReachHDPawn::StaticClass();	
}

void AEndlessReachHDGameMode::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> OutNodes;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ALevelNode::StaticClass(), OutNodes);  // get all streaming level location nodes

	for (auto Node : OutNodes)
	{
		// Store the transform of each node
		Transforms.Add(Node->GetTransform());
	}
}

void AEndlessReachHDGameMode::LoadMap()
{
	FLatentActionInfo info;

	//We load the stream level but we make sure not to make it visible before we assign a transform to it
	//The reason we use *MapName in the parameters is because the name param requires an FName so
	//essentially we're type casting the FString input to FName
	UGameplayStatics::LoadStreamLevel(GetWorld(), *MapName, false, false, info);
	ULevelStreaming* level = UGameplayStatics::GetStreamingLevel(GetWorld(), *MapName);

	//Assign a new transform to our level
	level->LevelTransform = MapTransform;

	//Make the level visible
	level->bShouldBeVisible = true;
}

void AEndlessReachHDGameMode::ReloadMap(FString MapName, int32 Position)
{
	if (Transforms.IsValidIndex(Position))
	{
		ULevelStreaming* level = UGameplayStatics::GetStreamingLevel(GetWorld(), *MapName);

		//store the new map name and the new transform
		this->MapName = MapName;
		MapTransform = Transforms[Position];

		if (level->IsLevelVisible())
		{
			//If the level is visible we need to unload it first
			//and then spawn it in a new location

			//The unload stream level is essentially an async operation
			//Using the Latent Action Info we're able to create a callback
			//which is fired when the UnloadStreamLevel finishes its execution
			FLatentActionInfo info;
			info.CallbackTarget = this;
			info.ExecutionFunction = FName("SpawnMap");
			info.UUID = 0;
			info.Linkage = 0;

			UGameplayStatics::UnloadStreamLevel(GetWorld(), *MapName, info);
		}
		//If the level is not visible just load the map
		else LoadMap();
	}
}

void AEndlessReachHDGameMode::UnloadMap(FString MapName, int32 Position)
{
	if (Transforms.IsValidIndex(Position))
	{
		ULevelStreaming* level = UGameplayStatics::GetStreamingLevel(GetWorld(), *MapName);

		//store the new map name and the new transform
		this->MapName = MapName;
		MapTransform = Transforms[Position];

		if (level->IsLevelVisible())
		{
			//If the level is visible we need to unload it first
			//and then spawn it in a new location

			//The unload stream level is essentially an async operation
			//Using the Latent Action Info we're able to create a callback
			//which is fired when the UnloadStreamLevel finishes its execution
			FLatentActionInfo info;
			info.CallbackTarget = this;
			//info.ExecutionFunction = FName("SpawnMap");
			info.UUID = 0;
			info.Linkage = 0;

			// for now we're simply unloading the map, but we might eventually do something in-game when this occurs.
			UGameplayStatics::UnloadStreamLevel(GetWorld(), *MapName, info);
		}
	}
}