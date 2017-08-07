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

#pragma once

#include "GameFramework/Character.h"
#include "Management/CommonLibrary.h"
#include "Widgets/PlayerHUD.h"
#include "EndlessReachHDPawn.generated.h"

UCLASS(Blueprintable)
class AEndlessReachHDPawn : public APawn
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	AEndlessReachHDPawn();

	/* The mesh component */
	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* ShipMeshComponent;

	// Gun Attachments
	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* ShipMeshGuns;

	// Gun Attachment Physics Constraint
	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UPhysicsConstraintComponent* ShipConstraintGuns;

	// Left Fan
	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* ShipMeshFanL;

	// Left Fan Physics Constraint
	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UPhysicsConstraintComponent* ShipConstraintFanL;

	// Right Fan
	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* ShipMeshFanR;

	// Right Fan Physics Constraint
	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UPhysicsConstraintComponent* ShipConstraintFanR;

	// Tail Fan
	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* ShipMeshFanT;

	// Tail Fan Physics Constraint
	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UPhysicsConstraintComponent* ShipConstraintFanT;

	/** The camera */
	UPROPERTY(Category = Camera, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* CameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(Category = Camera, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	// Rotating Movement Component - Left Fan
	UPROPERTY(Category = Rotators, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	URotatingMovementComponent* RotatingMovement_FanL;

	// Rotating Movement Component - Right Fan
	UPROPERTY(Category = Rotators, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	URotatingMovementComponent* RotatingMovement_FanR;

	// Rotating Movement Component - Tail Fan
	UPROPERTY(Category = Rotators, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	URotatingMovementComponent* RotatingMovement_FanT;

	/** Thruster Force Feedback */
	UPROPERTY(Category = Gameplay, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UForceFeedbackEffect* ThrusterFeedback;

	UPROPERTY(Category = Gameplay, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UParticleSystemComponent* ThrusterFX;
	UParticleSystem* P_ThrusterFX;

	UPROPERTY(Category = Gameplay, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UParticleSystemComponent* DistortionFX;
	UParticleSystem* P_DistortionFX;

	/** Offset from the ships location to spawn projectiles */
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite )
	FVector GunOffset;
	
	/* How fast the weapon will fire */
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
	float FireRate;

	/* Whether the ship is capable of moving */
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
	bool bCanMove;

	/* The speed our ship moves around the level */
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
	float MoveSpeed;

	/* The maximum velocity our ship can achieve with standard impulse movement */
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
	float MaxVelocity;

	/* The maximum velocity our ship can achieve while under thrust */
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
	float MaxThrustVelocity;

	/* Whether the forward guns are active */
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
	bool bForwardGunsActive;

	/* Whether the thrusters are active */
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
	bool bThustersActive;

	/* Whether the ship is low on fuel */
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
	bool bLowFuel;

	/* The ship's fan speed */
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
	float FanSpeed;

	/* The ship's current HP level */
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
	float CurrentHP;

	/* The ship's maximum HP level */
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
	float MaxHP;

	/* The ship's current fuel level */
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
	float FuelLevel;

	/* The ship's maximum fuel level */
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
	float MaxFuel;

	/* The number of orbs the player has collected since the last time they returned to the outpost */
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
	int32 OrbCount;

	/** Sound to play each time we fire */
	UPROPERTY(Category = Audio, EditAnywhere, BlueprintReadWrite)
	class USoundBase* FireSound;

	/** Sound to play when we're out of fuel */
	UPROPERTY(Category = Audio, EditAnywhere, BlueprintReadWrite)
	USoundCue* S_LowFuelWarning;
	UAudioComponent* LowFuelWarningSound;

	/** Engine Idle Sound */
	UPROPERTY(Category = Audio, EditAnywhere, BlueprintReadWrite)
	USoundCue* S_EngineIdle;
	UAudioComponent* EngineIdleSound;

	/** Sound to play when thrusters are active */
	UPROPERTY(Category = Audio, EditAnywhere, BlueprintReadWrite)
	USoundCue* S_EngineThrust;
	UAudioComponent* EngineThrustSound;

	// Player HUD Widget.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
	TSubclassOf<UUserWidget> W_PlayerHUD;
	UPlayerHUD* PlayerHUD;

	// Magnet Physics Constraint
	UPROPERTY(Category = Magnet, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UPhysicsConstraintComponent* MagnetConstraint;

	// Magnet Radius Collider
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Magnet)
	USphereComponent* MagnetRadius;

	/* Whether the magnet is enabled */
	UPROPERTY(Category = Magnet, EditAnywhere, BlueprintReadWrite)
	bool bMagnetEnabled;

	/** Function to handle the magnet overlapping something */
	//UFUNCTION(Category = Magnet)
	//void OnMagnetOverlap(UPrimitiveComponent * HitComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	// Begin Actor Interface
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End Actor Interface

	// Update the player hud
	UFUNCTION(BlueprintCallable, Category = HUD)
	void UpdatePlayerHUD();

	/* Fire a shot in the specified direction */
	UFUNCTION(BlueprintCallable, Category = Weapons)
	void FireShot(FVector FireDirection);

	/* Handler for the fire timer expiry */
	UFUNCTION(BlueprintCallable, Category = Weapons)
	void ShotTimerExpired();

	// Update the fan speed
	UFUNCTION(BlueprintCallable, Category = Ship)
	void UpdateFanSpeed();

	// Forward Gun Control
	UFUNCTION(BlueprintCallable, Category = Weapons)
	void FireForwardGuns();
	UFUNCTION(BlueprintCallable, Category = Weapons)
	void StopForwardGuns();

	// Thrusters Control
	UFUNCTION(BlueprintCallable, Category = Thrusters)
	void FireThrusters();
	UFUNCTION(BlueprintCallable, Category = Thrusters)
	void StopThrusters();
	UFUNCTION(BlueprintCallable, Category = Thrusters)
	void LowFuelSafety();

	// Configure the player ship with default settings
	// (for configs that can only occur after the world comes online)
	UFUNCTION(BlueprintCallable, Category = Ship)
	void ConfigureShip();

	// Static names for input bindings
	// AXIS
	static const FName MoveForwardBinding;
	static const FName MoveRightBinding;
	static const FName FireForwardBinding;
	static const FName FireRightBinding;
	// ACTIONS
	static const FName ForwardGunsBinding;
	static const FName ThrustersBinding;

private:

	/* Flag to control firing  */
	uint32 bCanFire : 1;

	/** Handle for efficient management of ShotTimerExpired timer */
	FTimerHandle TimerHandle_ShotTimerExpired;
};

