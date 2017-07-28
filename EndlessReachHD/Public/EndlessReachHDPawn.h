// � 2014 - 2017 Soverance Studios
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

	/** The root scene component */
	//UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	//class USceneComponent* Root;

	/* The mesh component */
	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* ShipMeshComponent;

	// Gun Attachments
	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* ShipMeshGuns;

	// Gun Attachment Physics Constraint
	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UPhysicsConstraintComponent* ShipConstraintGuns;

	// Left Fan
	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* ShipMeshFanL;

	// Left Fan Physics Constraint
	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UPhysicsConstraintComponent* ShipConstraintFanL;

	// Right Fan
	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* ShipMeshFanR;

	// Right Fan Physics Constraint
	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UPhysicsConstraintComponent* ShipConstraintFanR;

	// Tail Fan
	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* ShipMeshFanT;

	// Tail Fan Physics Constraint
	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UPhysicsConstraintComponent* ShipConstraintFanT;

	/** The camera */
	UPROPERTY(Category = Camera, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* CameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(Category = Camera, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	// Rotating Movement Component - Left Fan
	UPROPERTY(Category = Rotators, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class URotatingMovementComponent* RotatingMovement_FanL;

	// Rotating Movement Component - Right Fan
	UPROPERTY(Category = Rotators, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class URotatingMovementComponent* RotatingMovement_FanR;

	// Rotating Movement Component - Tail Fan
	UPROPERTY(Category = Rotators, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class URotatingMovementComponent* RotatingMovement_FanT;

	/** Thruster Force Feedback */
	UPROPERTY(Category = Gameplay, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UForceFeedbackEffect* ThrusterFeedback;

	UPROPERTY(Category = Gameplay, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UParticleSystemComponent* ThrusterFX;
	UParticleSystem* P_ThrusterFX;

	UPROPERTY(Category = Gameplay, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UParticleSystemComponent* DistortionFX;
	UParticleSystem* P_DistortionFX;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	AEndlessReachHDPawn();

	/** Offset from the ships location to spawn projectiles */
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite )
	FVector GunOffset;
	
	/* How fast the weapon will fire */
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
	float FireRate;

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

	// Begin Actor Interface
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End Actor Interface

	// Update the player hud
	void UpdatePlayerHUD();

	/* Fire a shot in the specified direction */
	void FireShot(FVector FireDirection);

	/* Handler for the fire timer expiry */
	void ShotTimerExpired();

	// Update the fan speed
	void UpdateFanSpeed();

	// Forward Gun Control
	void FireForwardGuns();
	void StopForwardGuns();

	// Thrusters Control
	void FireThrusters();
	void StopThrusters();
	void LowFuelSafety();

	// Configure the player ship with default settings
	// (for configs that can only occur after the world comes online)
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

public:
	/** Returns ShipMeshComponent subobject **/
	//FORCEINLINE class UStaticMeshComponent* GetShipMeshComponent() const { return ShipMeshComponent; }
	/** Returns CameraComponent subobject **/
	//FORCEINLINE class UCameraComponent* GetCameraComponent() const { return CameraComponent; }
	/** Returns CameraBoom subobject **/
	//FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
};

