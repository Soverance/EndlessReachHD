// © 2012 - 2019 Soverance Studios
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

#include "EndlessReachHD.h"
#include "EndlessReachHDGameMode.h"
#include "Kismet/KismetMathLibrary.h"
#include "Management/EnemyManager.h"
#include "Enemies/EnemyMaster.h"
#include "Enemies/Standard/Drone.h"

// Sets default values
AEnemyManager::AEnemyManager()
{
	TotalEnemies = 50;
	MinDistance = 1000;
	MaxDistance = 10000;

	// Creates a scene component and sets it as the root
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;
}

// Called when the game starts or when spawned
void AEnemyManager::BeginPlay()
{
	Super::BeginPlay();	

	PopulateEnemyField();
}

// Called every frame
void AEnemyManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Destroy all enemies
void AEnemyManager::ClearEnemyField()
{
	TArray<AActor*> EnemyField;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemyMaster::StaticClass(), EnemyField);  // get all enemies

	for (auto Enemy : EnemyField)
	{
		Enemy->Destroy();  // destroy all enemies in the enemy field
	}
}

// Spawn all asteroids
void AEnemyManager::PopulateEnemyField()
{
	AEndlessReachHDGameMode* GameMode = Cast<AEndlessReachHDGameMode>(GetWorld()->GetAuthGameMode());  // collect game mode reference

	// Enemy Procedural Generation
	for (int32 i = 0; i < TotalEnemies; i++)
	{
		// Generate random settings for each enemy in the field
		FVector Location = FMath::VRand() * FMath::FRandRange(MinDistance, MaxDistance);  // random location
		FVector CorrectedLocation = FVector(Location.X, Location.Y, 0);  // force Z value to 0 for all enemy locations (so they're on the same plane as the player)
		const FTransform Settings = FTransform(FRotator(0, 0, 0), CorrectedLocation, FVector(1, 1, 1));
		FActorSpawnParameters Params;
		Params.OverrideLevel = GetLevel();  // make enemies spawn within the streaming level so they can be properly unloaded along with the level

		// spawn enemies based on current map
		switch (GameMode->CurrentMap)
		{
			case 0:
				break;
			case 1:				
				AEnemyMaster* Enemy = GetWorld()->SpawnActor<ADrone>(ADrone::StaticClass(), Settings, Params);  // Spawn procedurally generated Drones

				if (Enemy)
				{
					Enemy->Level = GetEnemyLevel(GameMode);
					Enemy->SetBaseStats();
				}
				break;
		}		
	}
}

int32 AEnemyManager::GetEnemyLevel(AEndlessReachHDGameMode* GameMode)
{
	int32 EnemyLevel = 0;

	switch (GameMode->CurrentMap)
	{
		case 0:
			EnemyLevel = 1;
			break;
		case 1:
			EnemyLevel = FMath::RandRange(1, 2);			
			break;
	}

	return EnemyLevel;
}
