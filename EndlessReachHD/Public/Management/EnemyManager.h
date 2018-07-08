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

#pragma once

#include "GameFramework/Actor.h"
#include "EnemyManager.generated.h"

UCLASS()
class ENDLESSREACHHD_API AEnemyManager : public AActor
{
	GENERATED_BODY()

	/** The root scene component */
	UPROPERTY(Category = Asteroids, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* Root;

public:	
	// Sets default values for this actor's properties
	AEnemyManager();

	/* Total Number of Enemies to spawn */
	UPROPERTY(Category = Asteroids, EditAnywhere, BlueprintReadWrite)
	int32 TotalEnemies;

	/* Minimum Spawn Distance */
	UPROPERTY(Category = Asteroids, EditAnywhere, BlueprintReadWrite)
	float MinDistance;

	/* Maximum Spawn Distance */
	UPROPERTY(Category = Asteroids, EditAnywhere, BlueprintReadWrite)
	float MaxDistance;

	// Spawn all Enemy objects
	UFUNCTION(BlueprintCallable, Category = Default)
	void PopulateEnemyField();

	// Destroys all Enemy objects
	UFUNCTION(BlueprintCallable, Category = Default)
	void ClearEnemyField();

	// Determine Enemy Level
	UFUNCTION(BlueprintCallable, Category = Default)
	int32 GetEnemyLevel(AEndlessReachHDGameMode* GameMode);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
