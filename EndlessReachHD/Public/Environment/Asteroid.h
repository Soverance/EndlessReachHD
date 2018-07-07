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
#include "Management/CombatText/CombatTextComponent.h"
#include "DestructibleComponent.h"
#include "DestructibleMesh.h"
#include "Asteroid.generated.h"

// Event Dispatcher for Asteroid Destruction
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FHitAsteroid);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDestroyAsteroid);

UCLASS()
class ENDLESSREACHHD_API AAsteroid : public AActor
{
	GENERATED_BODY()
	
	/** The root scene component */
	UPROPERTY(Category = Asteroids, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* Root;

	/* The mesh component */
	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UDestructibleComponent* Asteroid;
	UDestructibleMesh* DM_Asteroid;

	/* The rock material */
	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UMaterialInstance* RockColor;

	/* The rock hit material */
	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UMaterialInstance* RedColor;

	UPROPERTY(Category = Gameplay, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UParticleSystemComponent* ExplosionFX;
	UParticleSystem* P_ExplosionFX;
		

public:	
	// Sets default values for this actor's properties
	AAsteroid();

	/* The health of this asteroid */
	UPROPERTY(Category = Combat, VisibleDefaultsOnly, BlueprintReadWrite)
	int32 Health;

	// Event Dispatcher Hit Asteroid
	UPROPERTY(BlueprintAssignable, Category = "Dispatcher")
	FHitAsteroid OnHitAsteroid;

	// Event Dispatcher Hit Asteroid
	UPROPERTY(BlueprintAssignable, Category = "Dispatcher")
	FDestroyAsteroid OnDestroyAsteroid;

	/* Whether the ship is low on fuel */
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
	bool bWasDestroyed;

	/** Asteroid Explosion Sound */
	UPROPERTY(Category = Audio, EditAnywhere, BlueprintReadWrite)
	USoundCue* S_AsteroidExplosion;
	UAudioComponent* AsteroidExplosionSound;

	// Add the destructible rock when necessary
	UFUNCTION(BlueprintCallable, Category = Controls)
	void AddDestructible();

	// When the Asteroid is hit by player bullets
	UFUNCTION(BlueprintCallable, Category = Controls)
	void HitAsteroid();

	// When the Asteroid is destroyed
	UFUNCTION(BlueprintCallable, Category = Controls)
	void DestroyAsteroid();

	// The combat text component
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CombatText)
	UCombatTextComponent* CombatTextComponent;

	// Show Combat Damage Text
	UFUNCTION(BlueprintCallable, Category = Combat)
	void ShowCombatDamageText(bool IsCritical, float Damage);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
