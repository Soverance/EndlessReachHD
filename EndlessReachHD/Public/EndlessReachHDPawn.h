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
#include "Management/CombatText/CombatTextComponent.h"
#include "Widgets/PlayerHUD.h"
#include "Widgets/HangarMenu.h"
#include "EndlessReachHDPawn.generated.h"

UCLASS(Blueprintable)
class AEndlessReachHDPawn : public APawn
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	AEndlessReachHDPawn();  // construction

	////////////////////////////////////////////////////
	//
	// SHIP
	//
	////////////////////////////////////////////////////

	// Configure the player ship with default settings
	// (for configs that can only occur after the world comes online)
	UFUNCTION(BlueprintCallable, Category = Ship)
	void ConfigureShip();

	/* The mesh component */
	UPROPERTY(Category = Ship, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* ShipMeshComponent;

	// Distortion visual effect "hover trail effect"
	UPROPERTY(Category = Ship, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UParticleSystemComponent* DistortionFX;
	UParticleSystem* P_DistortionFX;

	/* Whether the ship is capable of moving */
	UPROPERTY(Category = Ship, EditAnywhere, BlueprintReadWrite)
	bool bCanMove;

	/* The speed our ship moves around the level */
	UPROPERTY(Category = Ship, EditAnywhere, BlueprintReadWrite)
	float MoveSpeed;

	/* The ship's current HP level */
	UPROPERTY(Category = Ship, EditAnywhere, BlueprintReadWrite)
	float CurrentHP;

	/* The ship's maximum HP level */
	UPROPERTY(Category = Ship, EditAnywhere, BlueprintReadWrite)
	float MaxHP;

	/** Engine Idle Sound */
	UPROPERTY(Category = Ship, EditAnywhere, BlueprintReadWrite)
	USoundCue* S_EngineIdle;
	UAudioComponent* EngineIdleSound;

	////////////////////////////////////////////////////
	//
	// FANS
	//
	////////////////////////////////////////////////////

	// Left Fan
	UPROPERTY(Category = Fans, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* ShipMeshFanL;

	// Left Fan Physics Constraint
	UPROPERTY(Category = Fans, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UPhysicsConstraintComponent* ShipConstraintFanL;

	// Rotating Movement Component - Left Fan
	UPROPERTY(Category = Fans, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	URotatingMovementComponent* RotatingMovement_FanL;

	// Right Fan
	UPROPERTY(Category = Fans, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* ShipMeshFanR;

	// Right Fan Physics Constraint
	UPROPERTY(Category = Fans, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UPhysicsConstraintComponent* ShipConstraintFanR;

	// Rotating Movement Component - Right Fan
	UPROPERTY(Category = Fans, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	URotatingMovementComponent* RotatingMovement_FanR;

	// Tail Fan
	UPROPERTY(Category = Fans, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* ShipMeshFanT;

	// Tail Fan Physics Constraint
	UPROPERTY(Category = Fans, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UPhysicsConstraintComponent* ShipConstraintFanT;

	// Rotating Movement Component - Tail Fan
	UPROPERTY(Category = Fans, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	URotatingMovementComponent* RotatingMovement_FanT;

	/* The ship's fan speed */
	UPROPERTY(Category = Fans, EditAnywhere, BlueprintReadWrite)
	float FanSpeed;

	// Update the fan speed
	UFUNCTION(BlueprintCallable, Category = Fans)
	void UpdateFanSpeed();

	////////////////////////////////////////////////////
	//
	// DOCKING CONTROL
	//
	////////////////////////////////////////////////////

	/* Whether the player is currently docked in the hangar */
	UPROPERTY(Category = Docking, EditAnywhere, BlueprintReadWrite)
	bool bIsDocked;

	/* Engage Docking Clamps */
	UFUNCTION(BlueprintCallable, Category = Docking)
	void EngageDockingClamps();

	/* Release Docking Clamps */
	UFUNCTION(BlueprintCallable, Category = Docking)
	void ReleaseDockingClamps();

	////////////////////////////////////////////////////
	//
	// CAMERAS
	//
	////////////////////////////////////////////////////

	/* The Camera Look Sensitivity */
	UPROPERTY(Category = Camera, EditAnywhere, BlueprintReadWrite)
	float LookSensitivity;

	/* The Camera Rotation Speed */
	UPROPERTY(Category = Camera, EditAnywhere, BlueprintReadWrite)
	float CamRotSpeed;

	/* The Camera Minimum Degree Clamp */
	UPROPERTY(Category = Camera, EditAnywhere, BlueprintReadWrite)
	float ClampDegreeMin;

	/* The Camera Maximum Degree Clamp */
	UPROPERTY(Category = Camera, EditAnywhere, BlueprintReadWrite)
	float ClampDegreeMax;

	/* The Rotational Camera Boom Roll X Value - Updated Each Frame while docked */
	UPROPERTY(Category = Camera, EditAnywhere, BlueprintReadWrite)
	float RollX;

	/* The Rotational Camera Pitch Y Value - Updated Each Frame while docked */
	UPROPERTY(Category = Camera, EditAnywhere, BlueprintReadWrite)
	float PitchY;

	/* The Rotational Camera Yaw Z Value - Updated Each Frame while docked */
	UPROPERTY(Category = Camera, EditAnywhere, BlueprintReadWrite)
	float YawZ;

	/** The camera */
	UPROPERTY(Category = Camera, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* Camera_TopDown;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(Category = Camera, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom_TopDown;

	/** The camera */
	UPROPERTY(Category = Camera, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* Camera_Rotational;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(Category = Camera, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom_Rotational;

	/* Rotate Camera Vertically */
	UFUNCTION(BlueprintCallable, Category = Camera)
	void CameraControl_RotateVertical(float vertical);

	/* Rotate Camera Horizontally */
	UFUNCTION(BlueprintCallable, Category = Camera)
	void CameraControl_RotateHorizontal(float horizontal);
		
	////////////////////////////////////////////////////
	//
	// BASIC GUNS
	//
	////////////////////////////////////////////////////
	
	/* Whether the player can use the input function which fires the main cannon */
	UPROPERTY(Category = BaseGuns, EditAnywhere, BlueprintReadWrite)
	bool bCanFire;

	/** Offset from the ships location to spawn projectiles */
	UPROPERTY(Category = BaseGuns, EditAnywhere, BlueprintReadWrite )
	FVector GunOffset;
	
	/* How fast the weapon will fire */
	UPROPERTY(Category = BaseGuns, EditAnywhere, BlueprintReadWrite)
	float FireRate;

	/** Sound to play each time we fire */
	UPROPERTY(Category = BaseGuns, EditAnywhere, BlueprintReadWrite)
	class USoundBase* FireSound;

	/* Fire a shot in the specified direction */
	UFUNCTION(BlueprintCallable, Category = BaseGuns)
	void FireShot(FVector FireDirection);

	/* Handler for the fire timer expiry */
	UFUNCTION(BlueprintCallable, Category = BaseGuns)
	void ShotTimerExpired();

	////////////////////////////////////////////////////
	//
	// FUEL + THRUSTERS
	//
	////////////////////////////////////////////////////

	/* The ship's current fuel level */
	UPROPERTY(Category = Thrusters, EditAnywhere, BlueprintReadWrite)
	float FuelLevel;

	/* The ship's maximum fuel level */
	UPROPERTY(Category = Thrusters, EditAnywhere, BlueprintReadWrite)
	float MaxFuel;

	/* The maximum velocity our ship can achieve with standard impulse movement */
	UPROPERTY(Category = Thrusters, EditAnywhere, BlueprintReadWrite)
	float MaxVelocity;

	/* The maximum velocity our ship can achieve while under thrust */
	UPROPERTY(Category = Thrusters, EditAnywhere, BlueprintReadWrite)
	float MaxThrustVelocity;

	/* Whether the thrusters are active */
	UPROPERTY(Category = Thrusters, EditAnywhere, BlueprintReadWrite)
	bool bThustersActive;

	/* Whether the ship is low on fuel */
	UPROPERTY(Category = Thrusters, EditAnywhere, BlueprintReadWrite)
	bool bLowFuel;

	// Thruster visual fx
	UPROPERTY(Category = Thrusters, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UParticleSystemComponent* ThrusterFX;
	UParticleSystem* P_ThrusterFX;

	/** Thruster Force Feedback */
	UPROPERTY(Category = Thrusters, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UForceFeedbackEffect* ThrusterFeedback;

	/** Sound to play when we're out of fuel */
	UPROPERTY(Category = Thrusters, EditAnywhere, BlueprintReadWrite)
	USoundCue* S_LowFuelWarning;
	UAudioComponent* LowFuelWarningSound;

	/** Sound to play when thrusters are active */
	UPROPERTY(Category = Thrusters, EditAnywhere, BlueprintReadWrite)
	USoundCue* S_EngineThrust;
	UAudioComponent* EngineThrustSound;

	// Thrusters Cam Shake
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Thrusters)
	TSubclassOf<UCameraShake> ThrusterCamShake;

	// Thrusters Control
	UFUNCTION(BlueprintCallable, Category = Thrusters)
	void FireThrusters();
	UFUNCTION(BlueprintCallable, Category = Thrusters)
	void StopThrusters();
	UFUNCTION(BlueprintCallable, Category = Thrusters)
	void LowFuelSafety();
	UFUNCTION(BlueprintCallable, Category = Thrusters)
	void EnableThrusterFX();
	UFUNCTION(BlueprintCallable, Category = Thrusters)
	void DisableThrusterFX();

	////////////////////////////////////////////////////
	//
	// WIDGETS
	//
	////////////////////////////////////////////////////

	// Player HUD Widget.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Widgets)
	TSubclassOf<UUserWidget> W_PlayerHUD;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Widgets)
	UPlayerHUD* PlayerHUD;

	// Initialize all widget components
	UFUNCTION(BlueprintCallable, Category = Widgets)
	void InitializeAllWidgets();

	// Update the HUD with player information
	UFUNCTION(BlueprintCallable, Category = Widgets)
	void UpdatePlayerHUD();

	// Update the Hangar Menu with Player information
	UFUNCTION(BlueprintCallable, Category = Widgets)
	void UpdateHangarMenu();

	// Hangar Menu Widget.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Widgets)
	TSubclassOf<UUserWidget> W_HangarMenu;
	// This bug existed for months, until I figued out that UWidgets declared in this manner MUST be marked as a UPROPERTY,
	// otherwise they get Garbage Collected when they're removed from the viewport... so the widget reference would occasionally be null
	// by the time the player entered the hangar (because it had been GC'd)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Widgets)
	UHangarMenu* HangarMenu;

	// The combat text component
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CombatText)
	UCombatTextComponent* CombatTextComponent;

	// Show Combat Damage Text
	UFUNCTION(BlueprintCallable, Category = CombatText)
	void ShowCombatDamageText(bool bIsCritical, float Damage);

	// Add Status Effect Icon to HUD
	UFUNCTION(BlueprintCallable, Category = CombatText)
	void AddStatusEffectIcon(FName ID, UTexture2D* Icon, bool bShouldBlink);

	////////////////////////////////////////////////////
	//
	// AGGRO RADIUS and MISSILES
	//
	////////////////////////////////////////////////////

	// AGGRO Physics Constraint
	UPROPERTY(Category = Aggro, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UPhysicsConstraintComponent* AggroConstraint;

	// Aggro Radius Collider
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Aggro)
	USphereComponent* AggroRadius;

	/* Whether the missiles are enabled */
	UPROPERTY(Category = Missiles, EditAnywhere, BlueprintReadWrite)
	bool bMissilesEnabled;

	////////////////////////////////////////////////////
	//
	// MAGNET
	//
	////////////////////////////////////////////////////

	// Magnet Physics Constraint
	UPROPERTY(Category = Magnet, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UPhysicsConstraintComponent* MagnetConstraint;

	// Magnet Radius Collider
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Magnet)
	USphereComponent* MagnetRadius;

	/* Whether the magnet is enabled */
	UPROPERTY(Category = Magnet, EditAnywhere, BlueprintReadWrite)
	bool bMagnetEnabled;

	////////////////////////////////////////////////////
	//
	// LASER BEAM CANNON
	//
	////////////////////////////////////////////////////

	// Gun Attachments
	UPROPERTY(Category = Laser, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* ShipMeshGuns;

	// Gun Attachment Physics Constraint
	UPROPERTY(Category = Laser, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UPhysicsConstraintComponent* ShipConstraintGuns;

	// Beam Cannon Visual FX
	UPROPERTY(Category = Laser, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UParticleSystemComponent* LaserFX;
	UParticleSystem* P_LaserFX;

	// Beam Cannon Physics Constraint
	UPROPERTY(Category = Magnet, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UPhysicsConstraintComponent* LaserConstraint;

	// Beam Cannon Radius Collider
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Laser)
	UBoxComponent* LaserRadius;

	/* The number of charges for the laser beam cannon*/
	UPROPERTY(Category = Laser, EditAnywhere, BlueprintReadWrite)
	int32 LaserChargeCount;

	/* The maximum number of charges for the laser beam cannon*/
	UPROPERTY(Category = Laser, EditAnywhere, BlueprintReadWrite)
	int32 LaserChargeMax;

	/* Whether the beam cannon is unlocked */
	UPROPERTY(Category = Laser, EditAnywhere, BlueprintReadWrite)
	bool bLaserUnlocked;

	/* Whether the player is pressing the input button which fires the laser */
	UPROPERTY(Category = Laser, EditAnywhere, BlueprintReadWrite)
	bool bLaserEnabled;

	/** Sound to play when beam cannon is active */
	UPROPERTY(Category = Laser, EditAnywhere, BlueprintReadWrite)
	USoundCue* S_Laser;
	UAudioComponent* LaserSound;

	// Beam Cannon Cam Shake
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Laser)
	TSubclassOf<UCameraShake> LaserCamShake;

	/** Beam Cannon Force Feedback */
	UPROPERTY(Category = Laser, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UForceFeedbackEffect* LaserFeedback;

	// Beam Cannon Control Functions
	UFUNCTION(BlueprintCallable, Category = Laser)
	void FireLaser();
	UFUNCTION(BlueprintCallable, Category = Laser)
	void StopLaser();
	UFUNCTION(BlueprintCallable, Category = Laser)
	void UseLaserCharge();
	UFUNCTION(BlueprintCallable, Category = Laser)
	void LaserManualCutoff();

	/** Function to handle the beam cannon hitting something */
	UFUNCTION(BlueprintCallable, Category = Laser)
	void LaserBeginOverlap(UPrimitiveComponent * HitComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	////////////////////////////////////////////////////
	//
	// BOMBS
	//
	////////////////////////////////////////////////////

	/* Whether the player can use the input function which fires a bomb */
	UPROPERTY(Category = Bomb, EditAnywhere, BlueprintReadWrite)
	bool bCanFireBomb;

	/* Whether the bombs have been unlocked */
	UPROPERTY(Category = Bomb, EditAnywhere, BlueprintReadWrite)
	bool bBombsUnlocked;

	/* The number of bombs available - maximum of 5 */
	UPROPERTY(Category = Bomb, EditAnywhere, BlueprintReadWrite)
	int32 BombCount;

	/* The number of bombs available - maximum of 5 */
	UPROPERTY(Category = Bomb, EditAnywhere, BlueprintReadWrite)
	int32 BombMax;

	// Bomb Control Functions
	UFUNCTION(BlueprintCallable, Category = Bomb)
	void FireBomb();

	/** Sound to play when bomb is fired */
	UPROPERTY(Category = Bomb, EditAnywhere, BlueprintReadWrite)
	class USoundBase* BombSound;

	////////////////////////////////////////////////////
	//
	// UPGRADE LEVELS
	//
	////////////////////////////////////////////////////

	/* These values represent the current upgrade level of a specific feature */
	UPROPERTY(Category = Upgrade, EditAnywhere, BlueprintReadWrite)
	int32 ShipTypeLevel;
	UPROPERTY(Category = Upgrade, EditAnywhere, BlueprintReadWrite)
	int32 HealthLevel;	
	UPROPERTY(Category = Upgrade, EditAnywhere, BlueprintReadWrite)
	int32 ThrustersLevel;
	UPROPERTY(Category = Upgrade, EditAnywhere, BlueprintReadWrite)
	int32 CannonLevel;
	UPROPERTY(Category = Upgrade, EditAnywhere, BlueprintReadWrite)
	int32 LaserLevel;
	UPROPERTY(Category = Upgrade, EditAnywhere, BlueprintReadWrite)
	int32 MagnetLevel;
	UPROPERTY(Category = Upgrade, EditAnywhere, BlueprintReadWrite)
	int32 MissilesLevel;
	UPROPERTY(Category = Upgrade, EditAnywhere, BlueprintReadWrite)
	int32 ShieldLevel;
	UPROPERTY(Category = Upgrade, EditAnywhere, BlueprintReadWrite)
	int32 BombLevel;

	////////////////////////////////////////////////////
	//
	// UPGRADE FUNCTIONS
	//
	////////////////////////////////////////////////////

	UFUNCTION(BlueprintCallable, Category = Upgrade)
	int32 GetUpgradeCost(int32 UpgradeIndex, int32 PowerLevel);

	UFUNCTION(BlueprintCallable, Category = Upgrade)
	void UpgradeHealth(int32 UpgradeCost, int32 Level, int32 NextUpgradeCost);
	UFUNCTION(BlueprintCallable, Category = Upgrade)
	void UpgradeThrusters(int32 UpgradeCost, int32 Level, int32 NextUpgradeCost);
	UFUNCTION(BlueprintCallable, Category = Upgrade)
	void UpgradeCannon(int32 UpgradeCost, int32 Level, int32 NextUpgradeCost, float NewFireRate);
	UFUNCTION(BlueprintCallable, Category = Upgrade)
	void UpgradeLaser(int32 UpgradeCost, int32 Level, int32 NextUpgradeCost);
	UFUNCTION(BlueprintCallable, Category = Upgrade)
	void UpgradeMagnet(int32 UpgradeCost, int32 Level, int32 NextUpgradeCost, int32 NewMagnetRadius);
	UFUNCTION(BlueprintCallable, Category = Upgrade)
	void UpgradeBomb(int32 UpgradeCost, int32 Level, int32 NextUpgradeCost);

	////////////////////////////////////////////////////
	//
	// DEFAULTS
	//
	////////////////////////////////////////////////////

	// Basic Functions
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	/* The number of orbs the player has collected since the last time they returned to the outpost */
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
	int32 OrbCount;

	////////////////////////////////////////////////////
	//
	// DEBUG
	//
	////////////////////////////////////////////////////

	UFUNCTION(BlueprintCallable, Category = Debug)
	void StartDebug();
	UFUNCTION(BlueprintCallable, Category = Debug)
	void StopDebug();

	////////////////////////////////////////////////////
	//
	// INPUT BINDINGS
	//
	////////////////////////////////////////////////////

	// AXIS
	static const FName MoveForwardBinding;
	static const FName MoveRightBinding;
	static const FName FireForwardBinding;
	static const FName FireRightBinding;
	// ACTIONS
	static const FName LaserBinding;
	static const FName ThrustersBinding;
	static const FName ActionBinding;
	static const FName BackBinding;
	static const FName DebugBinding;
	static const FName MenuBinding;
	static const FName LeftBinding;
	static const FName RightBinding;

	// Input Control
	UFUNCTION(BlueprintCallable, Category = Menu)
	void MenuLeft();
	UFUNCTION(BlueprintCallable, Category = Menu)
	void MenuRight();
	UFUNCTION(BlueprintCallable, Category = Menu)
	void ActionInput();
	UFUNCTION(BlueprintCallable, Category = Menu)
	void BackInput();

private:

	/** Handle for efficient management of ShotTimerExpired timer */
	FTimerHandle TimerHandle_ShotTimerExpired;
};

