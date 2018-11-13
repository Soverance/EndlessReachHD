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

#pragma once

#include "GameFramework/Actor.h"
#include "Bomb.generated.h"

class AEndlessReachHDPawn;
class UProjectileMovementComponent;
class UStaticMeshComponent;

UCLASS(config=Game)
class ABomb : public AActor
{
	GENERATED_BODY()

	/** Sphere collision component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Bomb, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* ProjectileMesh;

	/* The bomb material */
	UPROPERTY(Category = Bomb, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UMaterialInstance* BombColor;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovement;

	// Shockwave Visual FX
	UPROPERTY(Category = Bomb, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UParticleSystemComponent* ShockwaveFX;
	UParticleSystem* P_ShockwaveFX;

	/** Sound to play when bomb shockwave goes off */
	UPROPERTY(Category = Bomb, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USoundCue* S_Shockwave;
	UAudioComponent* ShockwaveSound;

	// Shockwave Cam Shake
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Bomb, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UCameraShake> ShockwaveCamShake;

	/** Shockwave Force Feedback */
	UPROPERTY(Category = Bomb, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UForceFeedbackEffect* ShockwaveFeedback;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	ABomb();

	// Shockwave Radius Collider
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Bomb)
	USphereComponent* ShockwaveRadius;

	/** Function to handle the projectile hitting something */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/** Player reference */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	AEndlessReachHDPawn* Player;

	// the shockwave explosion function
	UFUNCTION(BlueprintCallable, Category = Bomb)
	void FireShockwave(AActor* OtherActor);

	// Delayed Destruction Function
	UFUNCTION(BlueprintCallable, Category = Bomb)
	void DelayedDestruction();

	// Delayed Self Destruction Function
	UFUNCTION(BlueprintCallable, Category = Bomb)
	void DelayedSelfDestruction();
};

