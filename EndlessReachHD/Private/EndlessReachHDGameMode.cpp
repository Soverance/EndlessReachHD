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
#include "EndlessReachHDPawn.h"
#include "EndlessReachHDGameMode.h"

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
		ALevelNode* LevelNode = Cast<ALevelNode>(Node);  // cast to the level node class to ensure validity and grant node functionality

		if (LevelNode)  // validity check
		{
			LevelNodes.Add(LevelNode);  // add the node to the LevelNodes array for later usage
			NodeTransforms.Add(LevelNode->GetTransform());  // add the node transforms to the transform array
		}		
	}

	ReloadMap("00-AlphaSector", 0);  // load the alpha sector map... hacky way to get the game started until I build main menus, loading screens and whatnot.
	CurrentMap = 0;  // set current map to alpha sector
}

// load a streaming map - DO NOT CALL THIS FUNCTION DIRECTLY!  call ReloadMap() instead.
void AEndlessReachHDGameMode::LoadMap()
{
	FLatentActionInfo info;

	//We load the stream level but we make sure not to make it visible before we assign a transform to it
	UGameplayStatics::LoadStreamLevel(GetWorld(), MapName, false, false, info);
	ULevelStreaming* level = UGameplayStatics::GetStreamingLevel(GetWorld(), MapName);

	//Assign a new transform to our level
	level->LevelTransform = MapTransform;

	//Make the level visible
	level->bShouldBeVisible = true;
}

void AEndlessReachHDGameMode::ReloadMap(FName MapName, int32 Position)
{
	if (NodeTransforms.IsValidIndex(Position))
	{
		ULevelStreaming* level = UGameplayStatics::GetStreamingLevel(GetWorld(), MapName);

		//store the new map name and the new transform
		this->MapName = MapName;
		MapTransform = NodeTransforms[Position];

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

			UGameplayStatics::UnloadStreamLevel(GetWorld(), MapName, info);
		}
		//If the level is not visible just load the map
		else LoadMap();
	}
}

void AEndlessReachHDGameMode::UnloadMap(FName MapName, int32 Position)
{
	if (NodeTransforms.IsValidIndex(Position))
	{
		ULevelStreaming* level = UGameplayStatics::GetStreamingLevel(GetWorld(), MapName);

		//store the new map name and the new transform
		this->MapName = MapName;
		MapTransform = NodeTransforms[Position];

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
			UGameplayStatics::UnloadStreamLevel(GetWorld(), MapName, info);

			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("UNLOADING MAP!"));
		}
	}
}

FName AEndlessReachHDGameMode::GetMapName(int32 Position)
{
	if (LevelNodes.IsValidIndex(Position))
	{
		return LevelNodes[Position]->LevelConfig.MapName;
	}
	else
	{
		FName Name = "LEVEL NODE INVALID - MAP NAME NOT FOUND!";
		return Name;
	}
}