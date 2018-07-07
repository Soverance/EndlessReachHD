// © 2014 - 2018 Soverance Studios
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
#include "Environment/Asteroid.h"
#include "Management/AsteroidManager.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AAsteroidManager::AAsteroidManager()
{
	TotalAsteroids = 50;
	MinDistance = 1000;
	MaxDistance = 10000;
	MinScale = 0.5f;
	MaxScale = 2.0f;
	ScaleMultiplier = 1;

	// Creates a scene component and sets it as the root
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;
}

// Called when the game starts or when spawned
void AAsteroidManager::BeginPlay()
{
	Super::BeginPlay();	

	PopulateAsteroidField();
}

// Called every frame
void AAsteroidManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Destroy all asteroids
void AAsteroidManager::ClearAsteroidField()
{
	TArray<AActor*> AsteroidField;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAsteroid::StaticClass(), AsteroidField);  // get all asteroids

	for (auto Rock : AsteroidField)
	{
		Rock->Destroy();  // destroy all rocks in asteroid field
	}
}

// Spawn all asteroids
void AAsteroidManager::PopulateAsteroidField()
{
	// Asteroid Procedural Generation
	for (int32 i = 0; i < TotalAsteroids; i++)
	{
		// Generate random settings for each asteroid in the field
		FVector Location = FMath::VRand() * FMath::FRandRange(MinDistance, MaxDistance);  // random location
		FVector CorrectedLocation = FVector(Location.X, Location.Y, 0);  // force Z value to 0 for all asteroid locations (so they're on the same plane as the player)
		FRotator Rotation = FRotator(FMath::FRandRange(-180.0f, 180.0f), FMath::FRandRange(-180.0f, 180.0f), FMath::FRandRange(-180.0f, 180.0f));  // random rotation
		FVector Scale = UKismetMathLibrary::Conv_FloatToVector((ScaleMultiplier * FMath::FRandRange(MinScale, MaxScale)));  // random scale
		const FTransform Settings = FTransform(Rotation, CorrectedLocation, Scale);
		FActorSpawnParameters Params;
		Params.OverrideLevel = GetLevel();  // make asteroids spawn within the streaming level so they can be properly unloaded		
		AAsteroid* Asteroid = GetWorld()->SpawnActor<AAsteroid>(AAsteroid::StaticClass(), Settings, Params);  // Spawn procedurally generated asteroids
	}
}
