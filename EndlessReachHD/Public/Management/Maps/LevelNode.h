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
#include "EndlessReachHDPawn.h"
#include "LevelName.h"
#include "LevelNode.generated.h"

USTRUCT(BlueprintType)
struct FLevelStruct
{
	GENERATED_USTRUCT_BODY()

	// Realm Enum
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Data)
	ELevelName LevelName;

	// Realm Name used for legible display
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Data)
	FText NameText;

	// Actual Map Name - used for streaming
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Data)
	FName MapName;

	// The level's node number - used for streaming, and more generally as a level identifier shorthand. i.e., "Level 1", "Level 2", etc.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Data)
	int32 NodeNumber;

	// Whether or not this level is currently LOCKED
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Data)
	bool bIsLocked;

	// Level Description
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Data)
	FText Description;

	// List Icon
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Data)
	UTexture2D* ListIcon;

	// Realm Preview Art
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Data)
	UTexture2D* RealmPreview;
};

UCLASS()
class ENDLESSREACHHD_API ALevelNode : public AActor
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Construction
	ALevelNode();

	/** Reference to Player Pawn */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	AEndlessReachHDPawn* Player;

	/** Function to handle the object overlapping something */
	UFUNCTION()
	void OnOverlap(UPrimitiveComponent * HitComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	/* Whether this node is currently in use */
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
	bool bInUse;

	/** The root scene component */
	UPROPERTY(Category = Config, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FLevelStruct LevelConfig;

	// Node Entry Radius Collider
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Components)
	USphereComponent* NodeEntryRadius;

	// Portal Visual Effect
	UPROPERTY(Category = Components, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UParticleSystemComponent* NodePortalFX;
	UParticleSystem* P_NodePortalFX;

	// Teleport Visual Effect
	UPROPERTY(Category = Components, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UParticleSystemComponent* NodeTeleportFX;
	UParticleSystem* P_NodeTeleportFX;

	// Start Location of Wormhole
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Timeline)
	FVector WormholeStartLocation;

	// Wormhole Timeline.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Timeline)
	UTimelineComponent* WormholeTimeline;

	// Wormhole External Curve.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Timeline)
	UCurveFloat* WormholeCurve;

	// Timeline Interp Function
	FOnTimelineFloat InterpFunction{};

	// Timeline Float Return
	UFUNCTION(BlueprintCallable, Category = Default)
	void TimelineFloatReturn(float val);

	// Unloads the current map via the wormhole
	UFUNCTION(BlueprintCallable, Category = Default)
	void WormholeUnload();

	// Loads the current map via the wormhole
	UFUNCTION(BlueprintCallable, Category = Default)
	void WormholeLoad();
};
