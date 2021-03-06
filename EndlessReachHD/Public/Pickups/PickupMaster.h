// 2012 - 2019 Soverance Studios
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
#include "EndlessReachHDPawn.h"
#include "PickupMaster.generated.h"

// Event Dispatcher for Player Collision
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayerCollision);

UCLASS()
class ENDLESSREACHHD_API APickupMaster : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickupMaster();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** Reference to Player Pawn */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	AEndlessReachHDPawn* Player;

	// Event Dispatcher Player Collision
	UPROPERTY(BlueprintAssignable, Category = Gameplay)
	FPlayerCollision OnPlayerCollision;

	/** Function to handle the object overlapping something */
	UFUNCTION()
	void OnOverlap(UPrimitiveComponent * HitComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	/** Function to handle the object hitting something */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	/* Whether this orb was picked up */
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
	bool bPickedUp;

	/** Pickup Error Sound */
	UPROPERTY(Category = Audio, EditAnywhere, BlueprintReadWrite)
	USoundCue* S_PickupError;
	UAudioComponent* PickupErrorSound;

	// Start Destruction
	UFUNCTION(BlueprintCallable, Category = Gameplay)
	void StartDestruction();

	// Pickup Magnet Timeline.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Timeline)
	UTimelineComponent* MagnetTimeline;

	// Magnet External Curve.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Timeline)
	UCurveFloat* MagnetCurve;

	// Timeline Interp Function
	FOnTimelineFloat InterpFunction{};

	// Timeline Float Return
	UFUNCTION(BlueprintCallable, Category = Timeline)
	void TimelineFloatReturn(float val);
};
