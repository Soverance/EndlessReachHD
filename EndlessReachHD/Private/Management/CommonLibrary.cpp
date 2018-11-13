// 2014 - 2018 Soverance Studios
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
#include "CommonLibrary.h"

UCommonLibrary::UCommonLibrary()
{

}

AActor* UCommonLibrary::SpawnActorIntoLevel(UObject* WorldContextObject, TSubclassOf<AActor> ActorClass, FName Level, FVector Location, FRotator Rotation, bool SpawnEvenIfColliding)
{
	if (!ActorClass) return NULL;  // return if no actor

	// use a context object to get the world
	UWorld* const World = GEngine->GetWorldFromContextObject(WorldContextObject);
	if (!World) return NULL;  // return if no world

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.bDeferConstruction = false;
	if (SpawnEvenIfColliding)
	{
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	}	

	// Get Level from Name
	ULevel* FoundLevel = NULL;

	for (const ULevelStreaming* EachLevel : World->GetStreamingLevels())
	{
		if (!EachLevel) continue;

		ULevel* LevelPtr = EachLevel->GetLoadedLevel();

		//Valid?
		if (!LevelPtr) continue;

		if (EachLevel->GetWorldAssetPackageFName() == Level)
		{
			FoundLevel = LevelPtr;
			break;
		}
	}

	if (FoundLevel)
	{
		SpawnParameters.OverrideLevel = FoundLevel;
	}

	return World->SpawnActor(ActorClass, &Location, &Rotation, SpawnParameters);
}