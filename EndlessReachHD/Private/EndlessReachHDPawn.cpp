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

#include "EndlessReachHD.h"
#include "EndlessReachHDPawn.h"
#include "EndlessReachHDProjectile.h"
#include "Pickups/PickupMaster.h"
#include "TimerManager.h"

// Create bindings for input - these are originally declared in DefaultInput.ini
// AXIS
const FName AEndlessReachHDPawn::MoveForwardBinding("MoveForward");
const FName AEndlessReachHDPawn::MoveRightBinding("MoveRight");
const FName AEndlessReachHDPawn::FireForwardBinding("FireForward");
const FName AEndlessReachHDPawn::FireRightBinding("FireRight");
// ACTIONS
const FName AEndlessReachHDPawn::ForwardGunsBinding("ForwardGuns");
const FName AEndlessReachHDPawn::ThrustersBinding("Thrusters");

// Construct pawn
AEndlessReachHDPawn::AEndlessReachHDPawn()
{	
	// Ship Default Specs
	CurrentHP = 1000;
	MaxHP = 1000;
	OrbCount = 0;
	bCanMove = true;
	MoveSpeed = 50.0f;
	MaxVelocity = 500.0f;
	MaxThrustVelocity = 1000.0f;
	FanSpeed = 50.0f;
	FuelLevel = 1000.0f;
	MaxFuel = 1000.0f;
	bThustersActive = false;
	bLowFuel = false;
	bMagnetEnabled = true;
	// Weapon Default Specs
	GunOffset = FVector(140.f, 0.f, 0.f);
	FireRate = 0.1f;
	bCanFire = true;
	bForwardGunsActive = false;
	
	// Creates a scene component and sets it as the root
	//Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	//RootComponent = Root;

	// Ship Body
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ShipMesh(TEXT("/Game/ShipScout_Upgrades/Meshes/SM_ShipScout_Set1_Body.SM_ShipScout_Set1_Body"));
	ShipMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShipBody"));
	RootComponent = ShipMeshComponent;
	ShipMeshComponent->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
	ShipMeshComponent->SetStaticMesh(ShipMesh.Object);
	ShipMeshComponent->SetRelativeRotation(FRotator(0, -90, 0));
	ShipMeshComponent->SetWorldScale3D(FVector(0.3f, 0.3f, 0.3f));
	ShipMeshComponent->SetSimulatePhysics(true);
	ShipMeshComponent->BodyInstance.bLockZTranslation = true;
	ShipMeshComponent->BodyInstance.bLockXRotation = true;
	ShipMeshComponent->BodyInstance.bLockYRotation = true;
	// Gun Attachments
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ShipGuns(TEXT("/Game/ShipScout_Upgrades/Meshes/SM_ShipScout_Set1_Attachments.SM_ShipScout_Set1_Attachments"));
	ShipMeshGuns = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShipGuns"));
	ShipMeshGuns->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ShipMeshGuns->SetStaticMesh(ShipGuns.Object);
	// Left Fan
	static ConstructorHelpers::FObjectFinder<UStaticMesh> LeftFan(TEXT("/Game/ShipScout_Upgrades/Meshes/SM_ShipScout_Set1_Fan.SM_ShipScout_Set1_Fan"));
	ShipMeshFanL = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftFan"));
	ShipMeshFanL->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ShipMeshFanL->SetStaticMesh(LeftFan.Object);
	// Left Fan Rotation
	RotatingMovement_FanL = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RotatingMovement_FanL"));
	RotatingMovement_FanL->SetUpdatedComponent(ShipMeshFanL);  // set the updated component
	RotatingMovement_FanL->RotationRate = FRotator(0,FanSpeed,0);
	// Right Fan
	static ConstructorHelpers::FObjectFinder<UStaticMesh> RightFan(TEXT("/Game/ShipScout_Upgrades/Meshes/SM_ShipScout_Set1_Fan.SM_ShipScout_Set1_Fan"));
	ShipMeshFanR = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightFan"));
	ShipMeshFanR->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ShipMeshFanR->SetStaticMesh(RightFan.Object);
	// Right Fan Rotation
	RotatingMovement_FanR = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RotatingMovement_FanR"));
	RotatingMovement_FanR->SetUpdatedComponent(ShipMeshFanR);  // set the updated component
	RotatingMovement_FanR->RotationRate = FRotator(0, (FanSpeed * -1), 0);
	// Tail Fan
	static ConstructorHelpers::FObjectFinder<UStaticMesh> TailFan(TEXT("/Game/ShipScout_Upgrades/Meshes/SM_ShipScout_Set1_Fan_Back.SM_ShipScout_Set1_Fan_Back"));
	ShipMeshFanT = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TailFan"));
	ShipMeshFanT->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ShipMeshFanT->SetStaticMesh(TailFan.Object);
	// Tail Fan Rotation
	RotatingMovement_FanT = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RotatingMovement_FanT"));
	RotatingMovement_FanT->SetUpdatedComponent(ShipMeshFanT);  // set the updated component
	RotatingMovement_FanT->RotationRate = FRotator(0, 0, (FanSpeed * -1));
	
	// Cache sound effects
	static ConstructorHelpers::FObjectFinder<USoundBase> FireAudio(TEXT("/Game/Audio/Guns/PlayerTurret_Pulse1_Cue.PlayerTurret_Pulse1_Cue"));
	FireSound = FireAudio.Object;
	static ConstructorHelpers::FObjectFinder<USoundCue> LowFuelAudio(TEXT("/Game/Audio/Ship/PlayerShip_LowFuelWarning_Cue.PlayerShip_LowFuelWarning_Cue"));
	S_LowFuelWarning = LowFuelAudio.Object;
	LowFuelWarningSound = CreateDefaultSubobject<UAudioComponent>(TEXT("LowFuelWarningSound"));
	LowFuelWarningSound->SetupAttachment(RootComponent);
	LowFuelWarningSound->Sound = S_LowFuelWarning;
	LowFuelWarningSound->bAutoActivate = false;
	static ConstructorHelpers::FObjectFinder<USoundCue> EngineIdleAudio(TEXT("/Game/Audio/Ship/PlayerShip_EngineIdle_Cue.PlayerShip_EngineIdle_Cue"));
	S_EngineIdle = EngineIdleAudio.Object;
	EngineIdleSound = CreateDefaultSubobject<UAudioComponent>(TEXT("EngineIdleSound"));
	EngineIdleSound->SetupAttachment(RootComponent);
	EngineIdleSound->Sound = S_EngineIdle;
	EngineIdleSound->bAutoActivate = true;
	EngineIdleSound->VolumeMultiplier = 0.4f;
	static ConstructorHelpers::FObjectFinder<USoundCue> EngineThrustAudio(TEXT("/Game/Audio/Ship/PlayerShip_EngineThrust_Cue.PlayerShip_EngineThrust_Cue"));
	S_EngineThrust = EngineThrustAudio.Object;
	EngineThrustSound = CreateDefaultSubobject<UAudioComponent>(TEXT("EngineThrustSound"));
	EngineThrustSound->SetupAttachment(RootComponent);
	EngineThrustSound->Sound = S_EngineThrust;
	EngineThrustSound->bAutoActivate = false;

	// Thruster Visual Effect
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ThrusterParticleObject(TEXT("/Game/Particles/Emitter/P_BlossomJet.P_BlossomJet"));
	P_ThrusterFX = ThrusterParticleObject.Object;
	ThrusterFX = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ThrusterFX"));
	ThrusterFX->SetupAttachment(ShipMeshComponent, FName("ThrusterEffectSocket"));
	ThrusterFX->Template = P_ThrusterFX;
	ThrusterFX->SetWorldScale3D(FVector(1.0f, 1.0f, 1.0f));
	ThrusterFX->bAutoActivate = false;

	// Thruster Force Feedback
	static ConstructorHelpers::FObjectFinder<UForceFeedbackEffect> ThrustFeedback(TEXT("/Game/ForceFeedback/Ship_Thruster.Ship_Thruster"));
	ThrusterFeedback = ThrustFeedback.Object;

	// Distortion Visual Effect
	static ConstructorHelpers::FObjectFinder<UParticleSystem> DistortionParticleObject(TEXT("/Game/Particles/Emitter/DistortionWave.DistortionWave"));
	P_DistortionFX = DistortionParticleObject.Object;
	DistortionFX = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("DistortionFX"));
	DistortionFX->SetupAttachment(ShipMeshComponent, FName("DistortionEffectSocket"));
	DistortionFX->Template = P_DistortionFX;
	DistortionFX->SetWorldScale3D(FVector(0.3f, 0.3f, 0.3f));
	DistortionFX->bAutoActivate = true;

	// Player HUD Widget
	static ConstructorHelpers::FClassFinder<UUserWidget> HUDWidget(TEXT("/Game/Widgets/BP_PlayerHUD.BP_PlayerHUD_C"));
	W_PlayerHUD = HUDWidget.Class;
	
	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->bAbsoluteRotation = true; // Don't want arm to rotate when ship does
	CameraBoom->TargetArmLength = 1200.f;
	CameraBoom->RelativeRotation = FRotator(-80.0f, 0.0f, 0.0f);
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	CameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	CameraComponent->bUsePawnControlRotation = false;	// Camera does not rotate relative to arm

	// configure Magnet Radius
	MagnetRadius = CreateDefaultSubobject<USphereComponent>(TEXT("MagnetRadius"));
	MagnetRadius->SetCollisionProfileName(UCollisionProfile::PhysicsActor_ProfileName);
	MagnetRadius->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	MagnetRadius->SetSphereRadius(1000);  //  3000 seems to be a pretty good max range?  maybe 4000 would work too...
	MagnetRadius->bHiddenInGame = false;
}

void AEndlessReachHDPawn::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);

	// set up gameplay key bindings
	// AXIS
	PlayerInputComponent->BindAxis(MoveForwardBinding);
	PlayerInputComponent->BindAxis(MoveRightBinding);
	PlayerInputComponent->BindAxis(FireForwardBinding);
	PlayerInputComponent->BindAxis(FireRightBinding);
	// ACTIONS
	PlayerInputComponent->BindAction(ForwardGunsBinding, EInputEvent::IE_Pressed, this, &AEndlessReachHDPawn::FireForwardGuns);
	PlayerInputComponent->BindAction(ForwardGunsBinding, EInputEvent::IE_Released, this, &AEndlessReachHDPawn::StopForwardGuns);
	PlayerInputComponent->BindAction(ThrustersBinding, EInputEvent::IE_Pressed, this, &AEndlessReachHDPawn::FireThrusters);
	PlayerInputComponent->BindAction(ThrustersBinding, EInputEvent::IE_Released, this, &AEndlessReachHDPawn::StopThrusters);
}

// Called when the game starts or when spawned
void AEndlessReachHDPawn::BeginPlay()
{
	Super::BeginPlay();

	// delay configuration of some components so that the world can be brought online first
	FTimerHandle ConfigDelay;
	GetWorldTimerManager().SetTimer(ConfigDelay, this, &AEndlessReachHDPawn::ConfigureShip, 0.25f, false);
}

void AEndlessReachHDPawn::Tick(float DeltaSeconds)
{
	// Find movement direction
	const float ForwardValue = GetInputAxisValue(MoveForwardBinding);
	const float RightValue = GetInputAxisValue(MoveRightBinding);

	// Clamp max size so that (X=1, Y=1) doesn't cause faster movement in diagonal directions
	const FVector MoveDirection = FVector(ForwardValue, RightValue, 0.0f).GetClampedToMaxSize(1.0f);

	// Calculate  movement
	const FVector Movement = MoveDirection * MoveSpeed * DeltaSeconds;

	if (bCanMove)
	{
		
	}

	// If stick is being pressed, MOVE 
	if (Movement.SizeSquared() > 0.0f)
	{
		// verify that movement is possible 
		// We don't bother to make this check when the stick is not being pressed
		if (bCanMove)
		{
			ShipMeshComponent->SetLinearDamping(0.01f);  // RESET LINEAR DAMPING
			ShipMeshComponent->SetAngularDamping(0.01f);  // RESET ANGULAR DAMPING

			const FRotator NewRotation = Movement.Rotation();
			const FRotator CorrectedRotation = FRotator(NewRotation.Pitch, (NewRotation.Yaw - 90), NewRotation.Roll);   // correct rotation because of the ship's offset pivot point
			FHitResult Hit(1.0f);
			EngineIdleSound->VolumeMultiplier = 0.75f;  // increase engine noise
			RootComponent->MoveComponent(Movement, FMath::Lerp(GetActorRotation(), CorrectedRotation, 0.05f), true, &Hit);  // move ship with smooth rotation - (LINEAR) MOVEMENT METHOD

			// if the thrusters are not active and the ship is under max velocity, we'll allow light impulse to be applied with the analog stick
			if (GetVelocity().Size() < MaxVelocity)
			{
				ShipMeshComponent->AddImpulseAtLocation(MoveDirection * MaxVelocity, GetActorLocation());  // Apply impulse thrust  - (PHYSICS) MOVEMENT METHOD
			}

			if (Hit.IsValidBlockingHit())
			{
				const FVector Normal2D = Hit.Normal.GetSafeNormal2D();
				const FVector Deflection = FVector::VectorPlaneProject(Movement, Normal2D) * (1.0f - Hit.Time);
				RootComponent->MoveComponent(Deflection, NewRotation, true);
			}

			// increase fan speed while moving
			if (FanSpeed < 500)
			{
				FanSpeed++;  // increment fan speed
				UpdateFanSpeed();
			}

			// THRUSTER CONTROL
			if (bThustersActive)
			{
				// if you have fuel available when thrusters are activated
				if (FuelLevel > 0)
				{
					FuelLevel--;  // CONSUME FUEL

					 // Do not add thrust if ship has already reached maximum thrust velocity
					if (GetVelocity().Size() < MaxThrustVelocity)
					{
						ShipMeshComponent->AddImpulseAtLocation(MoveDirection * MaxThrustVelocity, GetActorLocation());  // Apply impulse thrust
					}
				}
				// if you're still using thrusters when you're out of fuel, we'll slow you down a lot as you overload the ship
				// TO DO:  maybe add an overheating visual effect, and maybe allow the ship to explode if you continue thrusting with no fuel
				if (FuelLevel <= (MaxFuel * 0.1f))
				{
					if (!bLowFuel)
					{
						LowFuelSafety();
					}
				}
				if (FuelLevel <= 0)
				{
					StopThrusters();  // if you completely run out of fuel, call full stop on thrusters
				}
			}
		}
		// if bCanMove becomes false
		else
		{
			StopThrusters();  // stop thrusters
			ShipMeshComponent->SetLinearDamping(2.5f);  // Increase linear damping to slow down translation
			ShipMeshComponent->SetAngularDamping(2.5f);  // Increase angular damping to slow down rotation
		}
	}
	// When analog stick is no longer being pressed, STOP
	if (Movement.SizeSquared() <= 0.0f)
	{
		EngineIdleSound->VolumeMultiplier = 0.4f;  // decrease engine noise

		// decrease fan speed while idling
		if (FanSpeed > 50)
		{
			FanSpeed--;  // decrement fan speed
			UpdateFanSpeed();
		}

		if (bLowFuel)
		{
			StopThrusters();  // if you stopped moving while low on fuel, then we call full stop on the thrusters (to disable audio and reset bLowFuel)
		}

		ShipMeshComponent->SetLinearDamping(0.5f);  // Increase linear damping to slow down translation
		ShipMeshComponent->SetAngularDamping(0.5f);  // Increase angular damping to slow down rotation
	}
	
	// Create fire direction vector
	const float FireForwardValue = GetInputAxisValue(FireForwardBinding);
	const float FireRightValue = GetInputAxisValue(FireRightBinding);
	const FVector FireDirection = FVector(FireForwardValue, FireRightValue, 0.f);
	//ShipMeshGuns->SetRelativeRotation(FRotationMatrix::MakeFromX(FireDirection).Rotator());  // rotate guns to face firing direction - removed because it looks weird (ship was not modeled to support a turret feature)

	// Try and fire a shot
	FireShot(FireDirection);

	// Update Player HUD with new information
	UpdatePlayerHUD();

	// DEBUG: WRITE VELOCITY TO SCREEN EACH FRAME
	//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Magenta, FString::Printf(TEXT("Velocity: %f"), GetVelocity().Size()));
	// DEBUG: WRITE FUEL LEVEL TO SCREEN EACH FRAME
	//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Blue, FString::Printf(TEXT("Fuel Level: %f"), FuelLevel));
}

// Update the HUD with new information each frame
void AEndlessReachHDPawn::UpdatePlayerHUD()
{
	if (PlayerHUD)
	{
		PlayerHUD->Player_CurrentHP = CurrentHP;  // set current hp
		PlayerHUD->Player_MaxHP = MaxHP;  // set max hp
		PlayerHUD->Player_CurrentFuel = FuelLevel;  // set fuel level
		PlayerHUD->Player_MaxFuel = MaxFuel;  // set max fuel level
		PlayerHUD->Player_OrbCount = UCommonLibrary::GetFloatAsTextWithPrecision(OrbCount, 0, false);  // set current orb count
	}	
}

void AEndlessReachHDPawn::FireShot(FVector FireDirection)
{
	// If we it's ok to fire again
	if (bCanFire == true)
	{
		// If we are pressing fire stick in a direction
		if (FireDirection.SizeSquared() > 0.0f)
		{
			const FRotator FireRotation = FireDirection.Rotation();
			// Spawn projectile at an offset from this pawn
			const FVector SpawnLocation = GetActorLocation() + FireRotation.RotateVector(GunOffset);

			UWorld* const World = GetWorld();
			if (World != NULL)
			{
				// FIRE PROJECTILE
				AEndlessReachHDProjectile* Pulse = World->SpawnActor<AEndlessReachHDProjectile>(SpawnLocation, FireRotation);  // spawn projectile

				// The following is velocity inheritance code for the projectile... it's almost working, but not quite, so commented out for now

				//float InheritanceMod = 1.0f;  // set inheritance level to 100%
				//FVector Inheritance = GetControlRotation().UnrotateVector(GetVelocity());  // unrotate the player's velocity vector
				//FVector NewVelocity = ((Inheritance * InheritanceMod) + ProjectileDefaultVelocity);  // add inherited velocity to the projectile's default velocity - THIS LINE IS INCORRECT
				//Pulse->GetProjectileMovement()->SetVelocityInLocalSpace(NewVelocity);  // update projectile velocity
				//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, FString::Printf(TEXT("Updated Projectile Velocity: %f"), Pulse->GetVelocity().Size()));
			}

			bCanFire = false;
			World->GetTimerManager().SetTimer(TimerHandle_ShotTimerExpired, this, &AEndlessReachHDPawn::ShotTimerExpired, FireRate);

			// try and play the sound if specified
			if (FireSound != nullptr)
			{
				UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());  // play sound
			}

			bCanFire = false;
		}
	}
}

// Configure the Ship's default settings - mostly finishing up attaching actors that require the world to have been brought online
void AEndlessReachHDPawn::ConfigureShip()
{
	// Configure Physics Constraint Components to maintain attachment of the ship's objects, like fans, magnet, etc
	// we need to use constraints because the ship's body is simulating physics
	FConstraintInstance ConstraintInstance_Guns;  // gun attachments
	FConstraintInstance ConstraintInstance_FanL;  // left fan
	FConstraintInstance ConstraintInstance_FanR;  // right fan
	FConstraintInstance ConstraintInstance_FanT;  // tail fan
	FConstraintInstance ConstraintInstance_Magnet;  // Magnet

	// We want the guns, fans, and magnet to be stationary and stay exactly where they're attached, which means we're using them more like simple sockets (which sort of defeats the purpose of these physics constraints...)
	// GUNS
	UCommonLibrary::SetLinearLimits(ConstraintInstance_Guns, true, 0, 0, 0, 0, false, 0, 0);
	UCommonLibrary::SetAngularLimits(ConstraintInstance_Guns, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	// LEFT FAN
	UCommonLibrary::SetLinearLimits(ConstraintInstance_FanL, true, 0, 0, 0, 0, false, 0, 0);
	UCommonLibrary::SetAngularLimits(ConstraintInstance_FanL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	// RIGHT FAN
	UCommonLibrary::SetLinearLimits(ConstraintInstance_FanR, true, 0, 0, 0, 0, false, 0, 0);
	UCommonLibrary::SetAngularLimits(ConstraintInstance_FanR, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	// TAIL FAN
	UCommonLibrary::SetLinearLimits(ConstraintInstance_FanT, true, 0, 0, 0, 0, false, 0, 0);
	UCommonLibrary::SetAngularLimits(ConstraintInstance_FanT, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	// MAGNET
	UCommonLibrary::SetLinearLimits(ConstraintInstance_Magnet, true, 0, 0, 0, 0, false, 0, 0);
	UCommonLibrary::SetAngularLimits(ConstraintInstance_Magnet, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

	// Gun Attachment Constraint
	ShipConstraintGuns = NewObject<UPhysicsConstraintComponent>(ShipMeshComponent);  // create gun attachment constraint
	ShipConstraintGuns->ConstraintInstance = ConstraintInstance_Guns;  // set constraint instance
	ShipConstraintGuns->AttachToComponent(ShipMeshComponent, FAttachmentTransformRules::SnapToTargetIncludingScale, NAME_None);  // attach constraint to ship - can add a socket if necessary
	ShipConstraintGuns->SetRelativeLocation(FVector(0, 0, 0));  // set default location of constraint
	ShipConstraintGuns->SetConstrainedComponents(ShipMeshComponent, NAME_None, ShipMeshGuns, NAME_None);  // constrain the guns to the ship body
	ShipMeshGuns->AttachToComponent(ShipConstraintGuns, FAttachmentTransformRules::SnapToTargetIncludingScale);  // Attach guns to constraint
	ShipMeshGuns->SetRelativeLocation(FVector(0, 0, 0));  // reset gun location

	// Left Fan Constraint
	ShipConstraintFanL = NewObject<UPhysicsConstraintComponent>(ShipMeshComponent);  // create left fan constraint
	ShipConstraintFanL->ConstraintInstance = ConstraintInstance_FanL;  // set constraint instance
	ShipConstraintFanL->AttachToComponent(ShipMeshComponent, FAttachmentTransformRules::SnapToTargetIncludingScale, NAME_None);  // attach constraint to ship - can add a socket if necessary
	ShipConstraintFanL->SetRelativeLocation(FVector(240, 30, 30));  // set default location of constraint
	ShipConstraintFanL->SetConstrainedComponents(ShipMeshComponent, NAME_None, ShipMeshFanL, NAME_None);  // constrain the left fan to the ship body
	ShipMeshFanL->AttachToComponent(ShipConstraintFanL, FAttachmentTransformRules::SnapToTargetIncludingScale);  // Attach left fan to constraint
	ShipMeshFanL->SetRelativeLocation(FVector(0, 0, 0));  // reset fan location

	// Right Fan Constraint
	ShipConstraintFanR = NewObject<UPhysicsConstraintComponent>(ShipMeshComponent);  // create right fan constraint
	ShipConstraintFanR->ConstraintInstance = ConstraintInstance_FanR;  // set constraint instance
	ShipConstraintFanR->AttachToComponent(ShipMeshComponent, FAttachmentTransformRules::SnapToTargetIncludingScale, NAME_None);  // attach constraint to ship - can add a socket if necessary
	ShipConstraintFanR->SetRelativeLocation(FVector(-240, 30, 30));  // set default location of constraint
	ShipConstraintFanR->SetConstrainedComponents(ShipMeshComponent, NAME_None, ShipMeshFanR, NAME_None);  // constrain the right fan to the ship body
	ShipMeshFanR->AttachToComponent(ShipConstraintFanR, FAttachmentTransformRules::SnapToTargetIncludingScale);  // Attach left fan to constraint
	ShipMeshFanR->SetRelativeLocation(FVector(0, 0, 0));  // reset fan location

	// Tail Fan Constraint
	ShipConstraintFanT = NewObject<UPhysicsConstraintComponent>(ShipMeshComponent);  // create tail fan constraint
	ShipConstraintFanT->ConstraintInstance = ConstraintInstance_FanT;  // set constraint instance
	ShipConstraintFanT->AttachToComponent(ShipMeshComponent, FAttachmentTransformRules::SnapToTargetIncludingScale, NAME_None);  // attach constraint to ship - can add a socket if necessary
	ShipConstraintFanT->SetRelativeLocation(FVector(0, -400, 130));  // set default location of constraint
	ShipConstraintFanT->SetConstrainedComponents(ShipMeshComponent, NAME_None, ShipMeshFanT, NAME_None);  // constrain the tail fan to the ship body
	ShipMeshFanT->AttachToComponent(ShipConstraintFanT, FAttachmentTransformRules::SnapToTargetIncludingScale);  // Attach left fan to constraint
	ShipMeshFanT->SetRelativeLocation(FVector(0, 0, 0));  // reset fan location

	// Magnet Constraint
	MagnetConstraint = NewObject<UPhysicsConstraintComponent>(ShipMeshComponent);  // create magnet constraint
	MagnetConstraint->ConstraintInstance = ConstraintInstance_Magnet;  // set constraint instance
	MagnetConstraint->AttachToComponent(ShipMeshComponent, FAttachmentTransformRules::SnapToTargetIncludingScale, NAME_None);  // attach constraint to ship - can add a socket if necessary
	MagnetConstraint->SetRelativeLocation(FVector(0, 0, 0));  // set default location of constraint
	MagnetConstraint->SetConstrainedComponents(ShipMeshComponent, NAME_None, MagnetRadius, NAME_None);  // constrain the magnet radios to the ship body
	MagnetRadius->AttachToComponent(MagnetConstraint, FAttachmentTransformRules::SnapToTargetIncludingScale);  // Attach magnet to constraint
	MagnetRadius->SetRelativeLocation(FVector(0, 0, 0));  // reset magnet location

	// Spawn and attach the PlayerHUD
	if (!PlayerHUD)
	{
		PlayerHUD = CreateWidget<UPlayerHUD>(GetWorld(), W_PlayerHUD);  // creates the hud widget
		PlayerHUD->AddToViewport();  // add player hud to viewport
	}
}

void AEndlessReachHDPawn::ShotTimerExpired()
{
	bCanFire = true;
}

void AEndlessReachHDPawn::UpdateFanSpeed()
{
	// apply rotation speed
	RotatingMovement_FanL->RotationRate = FRotator(0, FanSpeed, 0);
	RotatingMovement_FanR->RotationRate = FRotator(0, (FanSpeed * -1), 0);
	RotatingMovement_FanT->RotationRate = FRotator(0, 0, (FanSpeed * -1));
}

void AEndlessReachHDPawn::FireForwardGuns()
{
	bForwardGunsActive = true;
}

void AEndlessReachHDPawn::StopForwardGuns()
{
	bForwardGunsActive = false;
}

void AEndlessReachHDPawn::FireThrusters()
{
	bThustersActive = true;
	if (FuelLevel > 0)
	{
		EngineThrustSound->Play();
		ThrusterFX->Activate();
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);  // Get Player Controller
		PlayerController->ClientPlayForceFeedback(ThrusterFeedback, false, FName(TEXT("Thruster")));  // Play Thruster Force Feedback
	}	
}

void AEndlessReachHDPawn::StopThrusters()
{
	bThustersActive = false;
	bLowFuel = false;
	ThrusterFX->Deactivate();
	EngineThrustSound->FadeOut(0.25f, 0);
	LowFuelWarningSound->FadeOut(0.05f, 0);
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);  // Get Player Controller
	PlayerController->ClientStopForceFeedback(ThrusterFeedback, FName(TEXT("Thruster")));  // Stop Thruster Feedback
}

// This feature makes it harder to completely run out of fuel, and plays an audio warning when near empty
void AEndlessReachHDPawn::LowFuelSafety()
{
	if (FuelLevel > 0)
	{
		bLowFuel = true;
		LowFuelWarningSound->Play();  // play audio
		ShipMeshComponent->SetLinearDamping(2.0f);  // Increase linear damping to slow down translation
		ShipMeshComponent->SetAngularDamping(2.0f);  // Increase angular damping to slow down rotation	
	}	
}