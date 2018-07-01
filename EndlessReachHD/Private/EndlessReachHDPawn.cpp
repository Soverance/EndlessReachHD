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
#include "Environment/Asteroid.h"
#include "Pickups/PickupMaster.h"
#include "TimerManager.h"
#include "Projectiles/Cannonball.h"
#include "Projectiles/Bomb.h"
//#include "Widgets/HangarMenu.h"

// Create bindings for input - these are originally declared in DefaultInput.ini
// AXIS
const FName AEndlessReachHDPawn::MoveForwardBinding("MoveForward");
const FName AEndlessReachHDPawn::MoveRightBinding("MoveRight");
const FName AEndlessReachHDPawn::FireForwardBinding("FireForward");
const FName AEndlessReachHDPawn::FireRightBinding("FireRight");
// ACTIONS
const FName AEndlessReachHDPawn::LaserBinding("Laser");
const FName AEndlessReachHDPawn::ThrustersBinding("Thrusters");
const FName AEndlessReachHDPawn::ActionBinding("Action");
const FName AEndlessReachHDPawn::BackBinding("Back");
const FName AEndlessReachHDPawn::DebugBinding("Debug");
const FName AEndlessReachHDPawn::MenuBinding("Menu");
const FName AEndlessReachHDPawn::LeftBinding("Left");
const FName AEndlessReachHDPawn::RightBinding("Right");

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
	bMissilesEnabled = false;
	bMagnetEnabled = false;
	GunOffset = FVector(140.f, 0.f, 0.f);
	FireRate = 0.2f;
	bCanFire = true;
	bLaserUnlocked = false;
	bLaserEnabled = false;
	LaserChargeCount = 0;
	LaserChargeMax = 0;
	bIsDocked = false;
	bBombsUnlocked = false;
	bCanFireBomb = true;
	BombCount = 0;
	BombMax = 0;
	LookSensitivity = 5.0f;
	CamRotSpeed = 5.0f;
	ClampDegreeMin = -40.0f;
	ClampDegreeMax = 40.0f;
	RollX = 0;
	PitchY = 0;
	YawZ = 0;

	// Upgrade Level Initialization
	ShipTypeLevel = 0;
	HealthLevel = 0;
	ThrustersLevel = 0;
	CannonLevel = 0;
	LaserLevel = 0;
	MagnetLevel = 0;
	MissilesLevel = 0;
	ShieldLevel = 0;
	BombLevel = 0;
	
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
	
	// basic weapon pulse audio
	static ConstructorHelpers::FObjectFinder<USoundBase> FireAudio(TEXT("/Game/Audio/Guns/PlayerTurret_Pulse1_Cue.PlayerTurret_Pulse1_Cue"));
	FireSound = FireAudio.Object;

	// low fuel warning audio
	static ConstructorHelpers::FObjectFinder<USoundCue> LowFuelAudio(TEXT("/Game/Audio/Ship/PlayerShip_LowFuelWarning_Cue.PlayerShip_LowFuelWarning_Cue"));
	S_LowFuelWarning = LowFuelAudio.Object;
	LowFuelWarningSound = CreateDefaultSubobject<UAudioComponent>(TEXT("LowFuelWarningSound"));
	LowFuelWarningSound->SetupAttachment(RootComponent);
	LowFuelWarningSound->Sound = S_LowFuelWarning;
	LowFuelWarningSound->bAutoActivate = false;

	// engine idle noise audio
	static ConstructorHelpers::FObjectFinder<USoundCue> EngineIdleAudio(TEXT("/Game/Audio/Ship/PlayerShip_EngineIdle_Cue.PlayerShip_EngineIdle_Cue"));
	S_EngineIdle = EngineIdleAudio.Object;
	EngineIdleSound = CreateDefaultSubobject<UAudioComponent>(TEXT("EngineIdleSound"));
	EngineIdleSound->SetupAttachment(RootComponent);
	EngineIdleSound->Sound = S_EngineIdle;
	EngineIdleSound->bAutoActivate = true;
	EngineIdleSound->VolumeMultiplier = 0.4f;

	// engine thrust noise audio
	static ConstructorHelpers::FObjectFinder<USoundCue> EngineThrustAudio(TEXT("/Game/Audio/Ship/PlayerShip_EngineThrust_Cue.PlayerShip_EngineThrust_Cue"));
	S_EngineThrust = EngineThrustAudio.Object;
	EngineThrustSound = CreateDefaultSubobject<UAudioComponent>(TEXT("EngineThrustSound"));
	EngineThrustSound->SetupAttachment(RootComponent);
	EngineThrustSound->Sound = S_EngineThrust;
	EngineThrustSound->bAutoActivate = false;

	// beam cannon noise audio
	static ConstructorHelpers::FObjectFinder<USoundCue> LaserAudio(TEXT("/Game/Audio/Guns/ForwardGun_Laser_Cue.ForwardGun_Laser_Cue"));
	S_Laser = LaserAudio.Object;
	LaserSound = CreateDefaultSubobject<UAudioComponent>(TEXT("LaserSound"));
	LaserSound->SetupAttachment(RootComponent);
	LaserSound->Sound = S_Laser;
	LaserSound->bAutoActivate = false;

	// bomb shot audio
	static ConstructorHelpers::FObjectFinder<USoundBase> BombAudio(TEXT("/Game/Audio/Guns/Bomb_FireShot_Cue.Bomb_FireShot_Cue"));
	BombSound = BombAudio.Object;

	// Thruster Visual Effect
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ThrusterParticleObject(TEXT("/Game/Particles/Emitter/P_BlossomJet.P_BlossomJet"));
	P_ThrusterFX = ThrusterParticleObject.Object;
	ThrusterFX = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ThrusterFX"));
	ThrusterFX->SetupAttachment(ShipMeshComponent, FName("ThrusterEffectSocket"));
	ThrusterFX->Template = P_ThrusterFX;
	ThrusterFX->SetWorldScale3D(FVector(1.0f, 1.0f, 1.0f));
	ThrusterFX->bAutoActivate = false;

	// Thruster Force Feedback
	static ConstructorHelpers::FObjectFinder<UForceFeedbackEffect> ThrustFeedback(TEXT("/Game/ForceFeedback/FF_ShipThrusters.FF_ShipThrusters"));
	ThrusterFeedback = ThrustFeedback.Object;

	// Beam Cannon Force Feedback
	static ConstructorHelpers::FObjectFinder<UForceFeedbackEffect> CannonFeedback(TEXT("/Game/ForceFeedback/FF_Laser.FF_Laser"));
	LaserFeedback = CannonFeedback.Object;

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

	// Hangar Menu Widget
	static ConstructorHelpers::FClassFinder<UUserWidget> HangarMenuWidget(TEXT("/Game/Widgets/BP_HangarMenu.BP_HangarMenu_C"));
	W_HangarMenu = HangarMenuWidget.Class;
	
	// Create a camera boom...
	CameraBoom_TopDown = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom_TopDown"));
	CameraBoom_TopDown->SetupAttachment(RootComponent);
	CameraBoom_TopDown->bAbsoluteRotation = true; // Don't want arm to rotate when ship does
	CameraBoom_TopDown->TargetArmLength = 1200.f;
	CameraBoom_TopDown->RelativeRotation = FRotator(-80.0f, 0.0f, 0.0f);
	CameraBoom_TopDown->bDoCollisionTest = false; // don't want to pull this camera in when it collides with level

	// Create a camera...
	Camera_TopDown = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	Camera_TopDown->SetupAttachment(CameraBoom_TopDown, USpringArmComponent::SocketName);
	Camera_TopDown->bUsePawnControlRotation = false;	// Camera does not rotate relative to arm

	// Create a camera boom...
	CameraBoom_Rotational = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom_Rotational"));
	CameraBoom_Rotational->SetupAttachment(RootComponent);
	CameraBoom_Rotational->bAbsoluteRotation = true; // Don't want arm to rotate when ship does
	CameraBoom_Rotational->TargetArmLength = 500.f;
	CameraBoom_Rotational->RelativeRotation = FRotator(0.0f, 0.0f, 0.0f);
	CameraBoom_Rotational->bDoCollisionTest = false; // I actually did want this camera to collide with the environment, but it was causing issues inside the hangar... it's fine at the short arm length.  I didn't want to make zoom feature anyway...

	// Create a camera...
	Camera_Rotational = CreateDefaultSubobject<UCameraComponent>(TEXT("RotationalCamera"));
	Camera_Rotational->SetupAttachment(CameraBoom_Rotational, USpringArmComponent::SocketName);
	Camera_Rotational->bUsePawnControlRotation = false;	// Camera does not rotate relative to arm

	// configure Aggro Radius
	AggroRadius = CreateDefaultSubobject<USphereComponent>(TEXT("AggroRadius"));
	AggroRadius->SetCollisionProfileName(UCollisionProfile::PhysicsActor_ProfileName);
	AggroRadius->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	AggroRadius->SetSphereRadius(5000);  //  5000 max range for aggro by default... we'll try it out for now
	AggroRadius->bHiddenInGame = true;

	// configure Magnet Radius
	MagnetRadius = CreateDefaultSubobject<USphereComponent>(TEXT("MagnetRadius"));
	MagnetRadius->SetCollisionProfileName(UCollisionProfile::PhysicsActor_ProfileName);
	MagnetRadius->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	MagnetRadius->SetSphereRadius(1000);  //  3000 seems to be a pretty good max range?  maybe 4000 would work too...
	MagnetRadius->bHiddenInGame = true;

	// Beam Cannon Visual Effect
	static ConstructorHelpers::FObjectFinder<UParticleSystem> LaserParticleObject(TEXT("/Game/ShipScout_Upgrades/Particles/P_GreenBeam.P_GreenBeam"));
	P_LaserFX = LaserParticleObject.Object;
	LaserFX = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("LaserFX"));
	LaserFX->SetupAttachment(ShipMeshComponent, FName("LaserEffectSocket"));
	LaserFX->Template = P_LaserFX;
	LaserFX->SetRelativeRotation(FRotator(0, 90, 0));
	LaserFX->SetWorldScale3D(FVector(1.0f, 1.0f, 1.0f));
	LaserFX->bAutoActivate = false;

	// configure Beam Cannon Radius
	LaserRadius = CreateDefaultSubobject<UBoxComponent>(TEXT("LaserRadius"));
	LaserRadius->SetCollisionProfileName(UCollisionProfile::PhysicsActor_ProfileName);
	LaserRadius->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	LaserRadius->OnComponentBeginOverlap.AddDynamic(this, &AEndlessReachHDPawn::LaserBeginOverlap);  // set up a notification for when this component hits something
	LaserRadius->SetBoxExtent(FVector(250, 3000, 250));
	LaserRadius->bHiddenInGame = true;

	// configure Beam Cannon Cam shake
	static ConstructorHelpers::FObjectFinder<UClass> LaserCamShakeObject(TEXT("/Game/CamShakes/CS_Laser.CS_Laser_C"));
	LaserCamShake = LaserCamShakeObject.Object;

	// configure Thruster Cam shake
	static ConstructorHelpers::FObjectFinder<UClass> ThrusterCamShakeObject(TEXT("/Game/CamShakes/CS_Thrusters.CS_Thrusters_C"));
	ThrusterCamShake = ThrusterCamShakeObject.Object;

	// Create Combat Text Component
	CombatTextComponent = CreateDefaultSubobject<UCombatTextComponent>(TEXT("Combat Text Component"));
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
	PlayerInputComponent->BindAction(LaserBinding, EInputEvent::IE_Pressed, this, &AEndlessReachHDPawn::FireLaser);
	PlayerInputComponent->BindAction(LaserBinding, EInputEvent::IE_Released, this, &AEndlessReachHDPawn::LaserManualCutoff);
	PlayerInputComponent->BindAction(ThrustersBinding, EInputEvent::IE_Pressed, this, &AEndlessReachHDPawn::FireThrusters);
	PlayerInputComponent->BindAction(ThrustersBinding, EInputEvent::IE_Released, this, &AEndlessReachHDPawn::StopThrusters);
	PlayerInputComponent->BindAction(DebugBinding, EInputEvent::IE_Pressed, this, &AEndlessReachHDPawn::StartDebug);
	PlayerInputComponent->BindAction(DebugBinding, EInputEvent::IE_Released, this, &AEndlessReachHDPawn::StopDebug);
	PlayerInputComponent->BindAction(LeftBinding, EInputEvent::IE_Pressed, this, &AEndlessReachHDPawn::MenuLeft);
	//PlayerInputComponent->BindAction(LeftBinding, EInputEvent::IE_Released, this, &AEndlessReachHDPawn::MenuLeft);
	PlayerInputComponent->BindAction(RightBinding, EInputEvent::IE_Pressed, this, &AEndlessReachHDPawn::MenuRight);
	//PlayerInputComponent->BindAction(RightBinding, EInputEvent::IE_Released, this, &AEndlessReachHDPawn::MenuRight);
	PlayerInputComponent->BindAction(ActionBinding, EInputEvent::IE_Pressed, this, &AEndlessReachHDPawn::ActionInput);
	//PlayerInputComponent->BindAction(ActionBinding, EInputEvent::IE_Released, this, &AEndlessReachHDPawn::MenuAction);
	PlayerInputComponent->BindAction(BackBinding, EInputEvent::IE_Pressed, this, &AEndlessReachHDPawn::BackInput);
	//PlayerInputComponent->BindAction(BackBinding, EInputEvent::IE_Released, this, &AEndlessReachHDPawn::MenuBack);
}

// Called when the game starts or when spawned
void AEndlessReachHDPawn::BeginPlay()
{
	Super::BeginPlay();

	// delay configuration of some components so that the world can be brought online first
	FTimerHandle ConfigDelay;
	GetWorldTimerManager().SetTimer(ConfigDelay, this, &AEndlessReachHDPawn::ConfigureShip, 0.25f, false);
}

// Debug Test Function
void AEndlessReachHDPawn::StartDebug()
{
	Camera_TopDown->SetActive(false, false);  // disable top down cam
	Camera_Rotational->SetActive(true, false);  // enable rotational cam
	FViewTargetTransitionParams params;
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	PlayerController->SetViewTarget(this, params);  // set new camera
	bIsDocked = true;  // DOCKED
}

// Debug Test Function
void AEndlessReachHDPawn::StopDebug()
{
	Camera_TopDown->SetActive(true, false);  // enable top down cam
	Camera_Rotational->SetActive(false, false);  // disable rotational cam
	FViewTargetTransitionParams params;
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	PlayerController->SetViewTarget(this, params);  // set new camera
	bIsDocked = false;  // UNDOCKED
}

// Configure the Ship's default settings - mostly finishing up attaching actors that require the world to have been brought online
void AEndlessReachHDPawn::ConfigureShip()
{
	// Configure Physics Constraint Components to maintain attachment of the ship's objects, like fans, magnet, etc
	// we need to use constraints because the ship's body is simulating physics
	// We want the guns, fans, and magnet to be stationary and stay exactly where they're attached, which means we're using them more like simple sockets (which sort of defeats the purpose of these physics constraints...)
	FConstraintInstance ConstraintInstance_Static;  // a basic constraint instance with no intention to move - will function more as a socket than a physics constraint

	UCommonLibrary::SetLinearLimits(ConstraintInstance_Static, true, 0, 0, 0, 0, false, 0, 0);
	UCommonLibrary::SetAngularLimits(ConstraintInstance_Static, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

	// Left Fan Constraint
	ShipConstraintFanL = NewObject<UPhysicsConstraintComponent>(ShipMeshComponent);  // create left fan constraint
	ShipConstraintFanL->ConstraintInstance = ConstraintInstance_Static;  // set constraint instance
	ShipConstraintFanL->AttachToComponent(ShipMeshComponent, FAttachmentTransformRules::SnapToTargetIncludingScale, NAME_None);  // attach constraint to ship - can add a socket if necessary
	ShipConstraintFanL->SetRelativeLocation(FVector(240, 30, 30));  // set default location of constraint
	ShipConstraintFanL->SetConstrainedComponents(ShipMeshComponent, NAME_None, ShipMeshFanL, NAME_None);  // constrain the left fan to the ship body
	ShipMeshFanL->AttachToComponent(ShipConstraintFanL, FAttachmentTransformRules::SnapToTargetIncludingScale);  // Attach left fan to constraint
	ShipMeshFanL->SetRelativeLocation(FVector(0, 0, 0));  // reset fan location

	// Right Fan Constraint
	ShipConstraintFanR = NewObject<UPhysicsConstraintComponent>(ShipMeshComponent);  // create right fan constraint
	ShipConstraintFanR->ConstraintInstance = ConstraintInstance_Static;  // set constraint instance
	ShipConstraintFanR->AttachToComponent(ShipMeshComponent, FAttachmentTransformRules::SnapToTargetIncludingScale, NAME_None);  // attach constraint to ship - can add a socket if necessary
	ShipConstraintFanR->SetRelativeLocation(FVector(-240, 30, 30));  // set default location of constraint
	ShipConstraintFanR->SetConstrainedComponents(ShipMeshComponent, NAME_None, ShipMeshFanR, NAME_None);  // constrain the right fan to the ship body
	ShipMeshFanR->AttachToComponent(ShipConstraintFanR, FAttachmentTransformRules::SnapToTargetIncludingScale);  // Attach left fan to constraint
	ShipMeshFanR->SetRelativeLocation(FVector(0, 0, 0));  // reset fan location

	// Tail Fan Constraint
	ShipConstraintFanT = NewObject<UPhysicsConstraintComponent>(ShipMeshComponent);  // create tail fan constraint
	ShipConstraintFanT->ConstraintInstance = ConstraintInstance_Static;  // set constraint instance
	ShipConstraintFanT->AttachToComponent(ShipMeshComponent, FAttachmentTransformRules::SnapToTargetIncludingScale, NAME_None);  // attach constraint to ship - can add a socket if necessary
	ShipConstraintFanT->SetRelativeLocation(FVector(0, -400, 130));  // set default location of constraint
	ShipConstraintFanT->SetConstrainedComponents(ShipMeshComponent, NAME_None, ShipMeshFanT, NAME_None);  // constrain the tail fan to the ship body
	ShipMeshFanT->AttachToComponent(ShipConstraintFanT, FAttachmentTransformRules::SnapToTargetIncludingScale);  // Attach left fan to constraint
	ShipMeshFanT->SetRelativeLocation(FVector(0, 0, 0));  // reset fan location

	// Gun Attachment Constraint
	ShipConstraintGuns = NewObject<UPhysicsConstraintComponent>(ShipMeshComponent);  // create gun attachment constraint
	ShipConstraintGuns->ConstraintInstance = ConstraintInstance_Static;  // set constraint instance
	ShipConstraintGuns->AttachToComponent(ShipMeshComponent, FAttachmentTransformRules::SnapToTargetIncludingScale, NAME_None);  // attach constraint to ship - can add a socket if necessary
	ShipConstraintGuns->SetRelativeLocation(FVector(0, 0, 0));  // set default location of constraint
	ShipConstraintGuns->SetConstrainedComponents(ShipMeshComponent, NAME_None, ShipMeshGuns, NAME_None);  // constrain the guns to the ship body
	ShipMeshGuns->AttachToComponent(ShipConstraintGuns, FAttachmentTransformRules::SnapToTargetIncludingScale);  // Attach guns to constraint
	ShipMeshGuns->SetRelativeLocation(FVector(0, 0, 0));  // reset gun location

	// Beam Cannon Attachment Constraint
	LaserConstraint = NewObject<UPhysicsConstraintComponent>(ShipMeshComponent);  // create beam cannon constraint
	LaserConstraint->ConstraintInstance = ConstraintInstance_Static;  // set constraint instance
	LaserConstraint->AttachToComponent(ShipMeshComponent, FAttachmentTransformRules::SnapToTargetIncludingScale, NAME_None);  // attach constraint to ship - can add a socket if necessary
	LaserConstraint->SetRelativeLocation(FVector(0, 0, 0));  // set default location of constraint
	LaserConstraint->SetConstrainedComponents(ShipMeshComponent, NAME_None, LaserRadius, NAME_None);  // constrain beam cannon to the ship body
	LaserRadius->AttachToComponent(LaserConstraint, FAttachmentTransformRules::SnapToTargetIncludingScale);  // Attach beam cannon to constraint
	LaserRadius->SetRelativeLocation(FVector(0, 2500, 0));  // reset beam cannon location

	// Aggro Constraint
	AggroConstraint = NewObject<UPhysicsConstraintComponent>(ShipMeshComponent);  // create Aggro constraint
	AggroConstraint->ConstraintInstance = ConstraintInstance_Static;  // set constraint instance
	AggroConstraint->AttachToComponent(ShipMeshComponent, FAttachmentTransformRules::SnapToTargetIncludingScale, NAME_None);  // attach constraint to ship - can add a socket if necessary
	AggroConstraint->SetRelativeLocation(FVector(0, 0, 0));  // set default location of constraint
	AggroConstraint->SetConstrainedComponents(ShipMeshComponent, NAME_None, AggroRadius, NAME_None);  // constrain the Aggro radius to the ship body
	AggroRadius->AttachToComponent(AggroConstraint, FAttachmentTransformRules::SnapToTargetIncludingScale);  // Attach radius to constraint
	AggroRadius->SetRelativeLocation(FVector(0, 0, 0));  // reset radius location

	// Magnet Constraint
	MagnetConstraint = NewObject<UPhysicsConstraintComponent>(ShipMeshComponent);  // create magnet constraint
	MagnetConstraint->ConstraintInstance = ConstraintInstance_Static;  // set constraint instance
	MagnetConstraint->AttachToComponent(ShipMeshComponent, FAttachmentTransformRules::SnapToTargetIncludingScale, NAME_None);  // attach constraint to ship - can add a socket if necessary
	MagnetConstraint->SetRelativeLocation(FVector(0, 0, 0));  // set default location of constraint
	MagnetConstraint->SetConstrainedComponents(ShipMeshComponent, NAME_None, MagnetRadius, NAME_None);  // constrain the magnet radios to the ship body
	MagnetRadius->AttachToComponent(MagnetConstraint, FAttachmentTransformRules::SnapToTargetIncludingScale);  // Attach magnet to constraint
	MagnetRadius->SetRelativeLocation(FVector(0, 0, 0));  // reset magnet location

	InitializeAllWidgets();  // init widgets
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

	// If you're undocked, you must be flying, so try firing a shot
	if (!bIsDocked)
	{
		// Try and fire a shot
		FireShot(FireDirection);		
		UpdatePlayerHUD();  // Update Player HUD with new information
	}
	// if you are docked, we'll let you rotate the camera to get a good look at your ship
	if (bIsDocked)
	{
		CameraControl_RotateVertical(GetInputAxisValue(FireForwardBinding));  // update boom vertical rotation
		CameraControl_RotateHorizontal(GetInputAxisValue(FireRightBinding));  // update boom horizontal rotation
		//UpdateHangarMenu();  // Update Hangar Menu with new information
	}

	// DEBUG: WRITE VELOCITY TO SCREEN EACH FRAME
	//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Magenta, FString::Printf(TEXT("Velocity: %f"), GetVelocity().Size()));
	// DEBUG: WRITE FUEL LEVEL TO SCREEN EACH FRAME
	//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Blue, FString::Printf(TEXT("Fuel Level: %f"), FuelLevel));
}

// Initialize all widgets
void AEndlessReachHDPawn::InitializeAllWidgets()
{
	// Spawn and attach the PlayerHUD
	if (!PlayerHUD)
	{
		PlayerHUD = CreateWidget<UPlayerHUD>(GetWorld(), W_PlayerHUD);  // creates the player hud widget

		if (!bIsDocked)
		{
			PlayerHUD->AddToViewport();  // add player hud to viewport
		}		
	}

	if (!HangarMenu)
	{
		HangarMenu = CreateWidget<UHangarMenu>(GetWorld(), W_HangarMenu);  // creates the hangar menu widget
	}	

	if (HangarMenu)
	{
		if (bIsDocked)
		{
			HangarMenu->AddToViewport();  // add hangar menu to viewport
			UpdateHangarMenu();  // refresh the hangar menu with default information
		}
	}
}

// Update the HUD with new information each frame
void AEndlessReachHDPawn::UpdatePlayerHUD()
{
	if (PlayerHUD)
	{
		if (PlayerHUD->IsInViewport())
		{
			PlayerHUD->Player_CurrentHP = CurrentHP;  // set current hp
			PlayerHUD->Player_MaxHP = MaxHP;  // set max hp
			PlayerHUD->Player_CurrentFuel = FuelLevel;  // set fuel level
			PlayerHUD->Player_MaxFuel = MaxFuel;  // set max fuel level
			PlayerHUD->Player_OrbCount = UCommonLibrary::GetFloatAsTextWithPrecision(OrbCount, 0, false);  // set current orb count
		}
	}
}

// Update the Hangar Menu with new information
void AEndlessReachHDPawn::UpdateHangarMenu()
{
	if (HangarMenu)
	{
		if (HangarMenu->IsInViewport())
		{
			HangarMenu->Player_OrbCount = UCommonLibrary::GetFloatAsTextWithPrecision(OrbCount, 0, false);  // set current orb count
		}
	}
}

void AEndlessReachHDPawn::FireShot(FVector FireDirection)
{
	// If we it's ok to fire again
	if (bCanFire)
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
				ACannonball* Pulse = World->SpawnActor<ACannonball>(SpawnLocation, FireRotation);  // spawn projectile

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

			//bCanFire = false;
		}
	}
}

// Rotational Camera Control (only accessible when docked)
void AEndlessReachHDPawn::CameraControl_RotateHorizontal(float horizontal)
{
	bool bLookHorizontalInvert = true;  // for now, we're just defaulting to inverted controls.  in the future, this setting may be changed within the options menu 

	switch (bLookHorizontalInvert)
	{
	case true:
		LookSensitivity = LookSensitivity * 1;  // do nothing for inverted controls
		break;
	case false:
		LookSensitivity = LookSensitivity * -1;  // multiply by -1 for standard controls
		break;
	}

	// I thought I could store this current rotation update block earlier in the tick function, so that it could be used for both the horizontal and vertical rotation functions...
	// however, when I did that, the camera movement would only update one direction at a time (instead of both horizontal+vertical simultaneously)
	// therefore, this update block exists in both camera control functions
	RollX = CameraBoom_Rotational->GetComponentRotation().Roll;  // store current boom roll
	PitchY = CameraBoom_Rotational->GetComponentRotation().Pitch;  // store current boom pitch
	YawZ = CameraBoom_Rotational->GetComponentRotation().Yaw;  // store current boom yaw

	// if vertical == 0.0f then do nothing...

	if (horizontal > 0.1f)
	{
		CameraBoom_Rotational->SetWorldRotation(FRotator(PitchY, FMath::FInterpTo(YawZ, YawZ + LookSensitivity, GetWorld()->GetDeltaSeconds(), CamRotSpeed), RollX));
	}

	if (horizontal < -0.1f)
	{
		CameraBoom_Rotational->SetWorldRotation(FRotator(PitchY, FMath::FInterpTo(YawZ, YawZ - LookSensitivity, GetWorld()->GetDeltaSeconds(), CamRotSpeed), RollX));
	}
}

// Rotational Camera Control (only accessible when docked)
void AEndlessReachHDPawn::CameraControl_RotateVertical(float vertical)
{
	bool bLookVerticalInvert = true;  // for now, we're just defaulting to inverted controls.  in the future, this setting may be changed within the options menu 
	
	switch (bLookVerticalInvert)
	{
		case true:
			LookSensitivity = LookSensitivity * 1;  // do nothing for inverted controls
			break;
		case false:
			LookSensitivity = LookSensitivity * -1;  // multiply by -1 for standard controls
			break;
	}
	
	// I thought I could store this current rotation update block earlier in the tick function, so that it could be used for both the horizontal and vertical rotation functions...
	// however, when I did that, the camera movement would only update one direction at a time (instead of both horizontal+vertical simultaneously)
	// therefore, this update block exists in both camera control functions
	RollX = CameraBoom_Rotational->GetComponentRotation().Roll;  // store current boom roll
	PitchY = CameraBoom_Rotational->GetComponentRotation().Pitch;  // store current boom pitch
	YawZ = CameraBoom_Rotational->GetComponentRotation().Yaw;  // store current boom yaw

	// if vertical == 0.0f then do nothing...

	if (vertical > 0.1f)
	{
		float ClampedPitchLerp = FMath::Clamp(FMath::FInterpTo(PitchY, PitchY + LookSensitivity, GetWorld()->GetDeltaSeconds(), CamRotSpeed), ClampDegreeMin, ClampDegreeMax);
		CameraBoom_Rotational->SetWorldRotation(FRotator(ClampedPitchLerp, YawZ, RollX));
	}

	if (vertical < -0.1f)
	{
		float ClampedPitchLerp = FMath::Clamp(FMath::FInterpTo(PitchY, PitchY - LookSensitivity, GetWorld()->GetDeltaSeconds(), CamRotSpeed), ClampDegreeMin, ClampDegreeMax);
		CameraBoom_Rotational->SetWorldRotation(FRotator(ClampedPitchLerp, YawZ, RollX));
	}
}

void AEndlessReachHDPawn::ShotTimerExpired()
{
	bCanFire = true;
	bCanFireBomb = true;
}

void AEndlessReachHDPawn::UpdateFanSpeed()
{
	// apply rotation speed
	RotatingMovement_FanL->RotationRate = FRotator(0, FanSpeed, 0);
	RotatingMovement_FanR->RotationRate = FRotator(0, (FanSpeed * -1), 0);
	RotatingMovement_FanT->RotationRate = FRotator(0, 0, (FanSpeed * -1));
}

void AEndlessReachHDPawn::FireLaser()
{
	if (!bIsDocked)
	{
		if (bLaserUnlocked)
		{
			if (bLaserEnabled)  // if the laser is already enabled when this function is called, it means the player was still holding the button and had charges remaining, so we essentially loop the firing mechanism
			{
				APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);  // Get Player Controller
				PlayerController->ClientPlayForceFeedback(LaserFeedback, false, false, FName(TEXT("Laser")));  // Play Laser Force Feedback
				PlayerController->ClientPlayCameraShake(LaserCamShake);  // play laser cam shake

				UseLaserCharge();  // use a laser charge

				// laser firing duration
				FTimerHandle LaserDelay;
				GetWorldTimerManager().SetTimer(LaserDelay, this, &AEndlessReachHDPawn::StopLaser, 3.0f, false);
			}

			// if the laser has yet to be enabled...
			if (!bLaserEnabled)
			{
				if (LaserChargeCount > 0 && LaserChargeCount <= LaserChargeMax)  // if laser charges is greater than zero but less or equal to than max...
				{
					bLaserEnabled = true;  // enable laser

					if (!LaserSound->IsPlaying())  // if the laser sound is not already playing...
					{
						LaserSound->Play();  // play laser sfx
					}

					LaserFX->Activate();  // play laser vfx
					APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);  // Get Player Controller
					PlayerController->ClientPlayForceFeedback(LaserFeedback, false, false, FName(TEXT("Laser")));  // Play Laser Force Feedback
					PlayerController->ClientPlayCameraShake(LaserCamShake);  // play laser cam shake

					UseLaserCharge();  // use a laser charge

					// laser firing duration
					FTimerHandle LaserDelay;
					GetWorldTimerManager().SetTimer(LaserDelay, this, &AEndlessReachHDPawn::StopLaser, 3.0f, false);
				}
			}
		}		
	}	
}

void AEndlessReachHDPawn::StopLaser()
{	
	if (bLaserEnabled)
	{
		if (LaserChargeCount > 0)
		{
			FireLaser();  // fire laser again! (player is still holding the button and still has charges remaining)
		}
		else
		{
			LaserManualCutoff();  // force laser shutdown if there isn't at least one charge remaining
		}		
	}
	if (!bLaserEnabled)
	{
		bLaserEnabled = false;
		LaserFX->Deactivate();
		LaserSound->FadeOut(0.5f, 0);
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);  // Get Player Controller
		PlayerController->ClientStopForceFeedback(LaserFeedback, FName(TEXT("Laser")));  // Stop Beam Cannon force feedback
	}
}

void AEndlessReachHDPawn::UseLaserCharge()
{
	switch (LaserChargeCount)
	{
		case 0:  // if none
			break;
		case 1:  // if one
			PlayerHUD->DischargeLaser_Stage1();  // discharge laser stage 1 hud anim
			break;
		case 2:  // if two
			PlayerHUD->DischargeLaser_Stage2();  // discharge laser stage 2 hud anim
			break;
		case 3:  // if three
			PlayerHUD->DischargeLaser_Stage3();  // discharge laser stage 3 hud anim
			break;
		case 4:  // if four
			PlayerHUD->DischargeLaser_Stage4();  // discharge laser stage 4 hud anim
			break;
		case 5:  // if five
			PlayerHUD->DischargeLaser_Stage5();  // discharge laser stage 5 hud anim
			break;
	}

	if (LaserChargeCount > 0)  // if one or more laser charges...
	{
		LaserChargeCount--;  // decrease laser charge count
	}
}

void AEndlessReachHDPawn::LaserManualCutoff()
{
	bLaserEnabled = false;
	StopLaser();  // stop laser
}

void AEndlessReachHDPawn::LaserBeginOverlap(UPrimitiveComponent * HitComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (bLaserEnabled)  // verify beam cannon is enabled before proceeding
	{
		if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL))
		{
			if (OtherComp->IsSimulatingPhysics())
			{
				OtherComp->AddImpulseAtLocation(GetVelocity() * 20.0f, GetActorLocation());  // apply small physics impulse to any physics object you hit
			}

			AAsteroid* Asteroid = Cast<AAsteroid>(OtherActor);  // if object is an asteroid...
			if (Asteroid)
			{
				Asteroid->OnDestroyAsteroid.Broadcast();  // Broadcast Asteroid Destruction
			}
		}
	}	
}

void AEndlessReachHDPawn::FireThrusters()
{
	if (!bIsDocked)
	{
		bThustersActive = true;
		if (FuelLevel > 0)
		{
			EnableThrusterFX();
		}
	}	
}

void AEndlessReachHDPawn::StopThrusters()
{
	if (!bIsDocked)
	{
		bThustersActive = false;
		bLowFuel = false;
		DisableThrusterFX();		
	}	
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

// thruster effects were separated from the main Fire/StopThrusters() functions so that the effects could be activated during cutscenes (as opposed to just manually by the player)
void AEndlessReachHDPawn::EnableThrusterFX()
{
	EngineThrustSound->Play();
	ThrusterFX->Activate();
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);  // Get Player Controller
	PlayerController->ClientPlayForceFeedback(ThrusterFeedback, false, false, FName(TEXT("Thruster")));  // Play Thruster Force Feedback
	PlayerController->ClientPlayCameraShake(ThrusterCamShake);  // play cam shake
}

void AEndlessReachHDPawn::DisableThrusterFX()
{
	ThrusterFX->Deactivate();
	EngineThrustSound->FadeOut(0.25f, 0);
	LowFuelWarningSound->FadeOut(0.05f, 0);
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);  // Get Player Controller
	PlayerController->ClientStopForceFeedback(ThrusterFeedback, FName(TEXT("Thruster")));  // Stop Thruster Feedback
	//PlayerController->ClientStopCameraShake(ThrusterCamShake);  // we don't need to manually stop the cam shakes, because that causes them to look unnatural
}

void AEndlessReachHDPawn::EngageDockingClamps()
{
	Camera_TopDown->SetActive(false, false);  // disable top down cam
	Camera_Rotational->SetActive(true, false);  // enable rotational cam
	FViewTargetTransitionParams params;
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	PlayerController->SetViewTarget(this, params);  // set new camera
	bIsDocked = true;  // DOCKED
	bCanMove = false;  // no movement while docked
	bCanFire = false;  // no weapons while docked

	if (PlayerHUD)  // error checking
	{
		PlayerHUD->RemoveFromViewport();  // remove the player hud from the viewport
	}
	if (HangarMenu)
	{
		HangarMenu->AddToViewport();  // add the hangar menu to the viewport
		UpdateHangarMenu();  // refresh the hangar menu with updated information
		HangarMenu->ReturnToUpgradeMenu();  // return to the upgrade menu (this is really only necessary for subsequent docking entries after a level load)
	}
	else
	{
		InitializeAllWidgets();  // reinitialize widgets, since the hangar menu apparantly failed to load
	}
}

void AEndlessReachHDPawn::ReleaseDockingClamps()
{
	Camera_TopDown->SetActive(true, false);  // enable top down cam
	Camera_Rotational->SetActive(false, false);  // disable rotational cam
	FViewTargetTransitionParams params;
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	PlayerController->SetViewTarget(this, params);  // set new camera
	bIsDocked = false;  // UNDOCKED
	bCanMove = true;  // restore movement
	bCanFire = true;  // arm weapons

	if (HangarMenu)  // error checking
	{
		HangarMenu->RemoveFromViewport();  // remove hangar menu from the viewport
	}
	if (PlayerHUD)  // error checking
	{
		PlayerHUD->AddToViewport();  // add the player hud to the viewport
	}	
}

void AEndlessReachHDPawn::FireBomb()
{
	if (bBombsUnlocked)
	{
		if (bCanFireBomb)
		{
			if (BombCount > 0 && BombCount <= BombMax)
			{
				// Find movement direction
				const float ForwardValue = GetInputAxisValue(MoveForwardBinding);
				const float RightValue = GetInputAxisValue(MoveRightBinding);

				// Clamp max size so that (X=1, Y=1) doesn't cause faster movement in diagonal directions
				const FVector MoveDirection = FVector(ForwardValue, RightValue, 0.0f).GetClampedToMaxSize(1.0f);
				// Spawn projectile at an offset from this pawn
				const FRotator FireRotation = MoveDirection.Rotation();
				const FVector SpawnLocation = GetActorLocation() + FireRotation.RotateVector(GunOffset);

				UWorld* const World = GetWorld();
				if (World != NULL)
				{
					// FIRE PROJECTILE
					ABomb* Bomb = World->SpawnActor<ABomb>(SpawnLocation, FireRotation);  // spawn projectile

					// The following is velocity inheritance code for the projectile... it's almost working, but not quite, so commented out for now

					//float InheritanceMod = 1.0f;  // set inheritance level to 100%
					//FVector Inheritance = GetControlRotation().UnrotateVector(GetVelocity());  // unrotate the player's velocity vector
					//FVector NewVelocity = ((Inheritance * InheritanceMod) + ProjectileDefaultVelocity);  // add inherited velocity to the projectile's default velocity - THIS LINE IS INCORRECT
					//Pulse->GetProjectileMovement()->SetVelocityInLocalSpace(NewVelocity);  // update projectile velocity
					//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, FString::Printf(TEXT("Updated Projectile Velocity: %f"), Pulse->GetVelocity().Size()));
				}

				// Reduce bomb count by one for each bomb fired
				if (BombCount > 0)
				{
					BombCount--;
				}
				else
				{
					BombCount = 0;  // bomb count cannot be less than zero
				}

				bCanFireBomb = false;
				World->GetTimerManager().SetTimer(TimerHandle_ShotTimerExpired, this, &AEndlessReachHDPawn::ShotTimerExpired, FireRate);

				// try and play the sound if specified
				if (BombSound != nullptr)
				{
					UGameplayStatics::PlaySoundAtLocation(this, BombSound, GetActorLocation());  // play sound
				}

				bCanFireBomb = false;
			}
			
		}
	}
}

// Menu Left Control
void AEndlessReachHDPawn::MenuLeft()
{
	if (bIsDocked)
	{
		if(HangarMenu)
		{
			if (HangarMenu->IsInViewport())
			{
				HangarMenu->MoveLeft();
			}
		}
	}
}

// Menu Right Control
void AEndlessReachHDPawn::MenuRight()
{
	if (bIsDocked)
	{
		if (HangarMenu)
		{
			if (HangarMenu->IsInViewport())
			{
				HangarMenu->MoveRight();
			}
		}
	}
}

// Get Upgrade Cost
int32 AEndlessReachHDPawn::GetUpgradeCost(int32 UpgradeIndex, int32 PowerLevel)
{
	// cost of the upgrade for the specified level
	int32 UpgradeCost = 0;

	switch (UpgradeIndex)
	{
		// SHIP TYPE UPGRADE
		case 0:
			switch (PowerLevel)
			{
			case 0:
				UpgradeCost = 1750;
				break;
			case 1:
				UpgradeCost = 2500;
				break;
			case 2:
				UpgradeCost = 3250;
				break;
			case 3:
				UpgradeCost = 4500;
				break;
			case 4:
				UpgradeCost = 6000;
				break;
			case 5:
				UpgradeCost = 99999;
				break;
			}
			break;
		// HEALTH UPGRADE
		case 1:
			switch (PowerLevel)
			{
			case 0:
				UpgradeCost = 250;
				break;
			case 1:
				UpgradeCost = 500;
				break;
			case 2:
				UpgradeCost = 750;
				break;
			case 3:
				UpgradeCost = 1000;
				break;
			case 4:
				UpgradeCost = 1500;
				break;
			case 5:
				UpgradeCost = 99999;
				break;
			}
			break;
		// THURSTERS UPGRADE
		case 2:
			switch (PowerLevel)
			{
			case 0:
				UpgradeCost = 500;
				break;
			case 1:
				UpgradeCost = 750;
				break;
			case 2:
				UpgradeCost = 1000;
				break;
			case 3:
				UpgradeCost = 1250;
				break;
			case 4:
				UpgradeCost = 1500;
				break;
			case 5:
				UpgradeCost = 99999;
				break;
			}
			break;
		// MAIN CANNON UPGRADE
		case 3:
			switch (PowerLevel)
			{
			case 0:
				UpgradeCost = 500;
				break;
			case 1:
				UpgradeCost = 750;
				break;
			case 2:
				UpgradeCost = 1000;
				break;
			case 3:
				UpgradeCost = 1250;
				break;
			case 4:
				UpgradeCost = 1500;
				break;
			case 5:
				UpgradeCost = 99999;
				break;
			}
			break;
		// LASER UPGRADE
		case 4:
			switch (PowerLevel)
			{
			case 0:
				UpgradeCost = 1000;
				break;
			case 1:
				UpgradeCost = 1500;
				break;
			case 2:
				UpgradeCost = 2000;
				break;
			case 3:
				UpgradeCost = 2500;
				break;
			case 4:
				UpgradeCost = 3000;
				break;
			case 5:
				UpgradeCost = 99999;
				break;
			}
			break;
		// MAGNET UPGRADE
		case 5:
			switch (PowerLevel)
			{
			case 0:
				UpgradeCost = 1000;
				break;
			case 1:
				UpgradeCost = 1500;
				break;
			case 2:
				UpgradeCost = 2000;
				break;
			case 3:
				UpgradeCost = 2500;
				break;
			case 4:
				UpgradeCost = 3000;
				break;
			case 5:
				UpgradeCost = 99999;
				break;
			}
			break;
		// MISSILES UPGRADE
		case 6:
			switch (PowerLevel)
			{
			case 0:
				UpgradeCost = 1500;
				break;
			case 1:
				UpgradeCost = 2250;
				break;
			case 2:
				UpgradeCost = 3000;
				break;
			case 3:
				UpgradeCost = 3750;
				break;
			case 4:
				UpgradeCost = 4500;
				break;
			case 5:
				UpgradeCost = 99999;
				break;
			}
			break;
		// ENERGY SHIELD UPGRADE
		case 7:
			switch (PowerLevel)
			{
			case 0:
				UpgradeCost = 1500;
				break;
			case 1:
				UpgradeCost = 2250;
				break;
			case 2:
				UpgradeCost = 3000;
				break;
			case 3:
				UpgradeCost = 3750;
				break;
			case 4:
				UpgradeCost = 4500;
				break;
			case 5:
				UpgradeCost = 99999;
				break;
			}
			break;
		// BOMB LEVEL
		case 8:
			switch (PowerLevel)
			{
			case 0:
				UpgradeCost = 2000;
				break;
			case 1:
				UpgradeCost = 3000;
				break;
			case 2:
				UpgradeCost = 4000;
				break;
			case 3:
				UpgradeCost = 5000;
				break;
			case 4:
				UpgradeCost = 6000;
				break;
			case 5:
				UpgradeCost = 99999;
				break;
			}
			break;
	}

	return UpgradeCost;
}

// Upgrade Health
void AEndlessReachHDPawn::UpgradeHealth(int32 UpgradeCost, int32 Level, int32 NextUpgradeCost)
{
	if (OrbCount > UpgradeCost)
	{
		OrbCount = OrbCount - UpgradeCost;  // subtract Cost from OrbCount
		HealthLevel = Level;  // set health upgrade level
		MaxHP = (MaxHP + (MaxHP * 0.5f));  // Add 50% of current max HP for each upgrade
		HangarMenu->SetUpgradeLevel(HealthLevel, NextUpgradeCost);  // set the new upgrade information within the hangar menu
		UpdateHangarMenu();  // update the hangar menu display
	}
	else
	{
		HangarMenu->NotifyError();
	}
}

// Upgrade Thrusters
void AEndlessReachHDPawn::UpgradeThrusters(int32 UpgradeCost, int32 Level, int32 NextUpgradeCost)
{
	if (OrbCount > UpgradeCost)
	{
		OrbCount = OrbCount - UpgradeCost;  // subtract Cost from OrbCount
		ThrustersLevel = Level;  // set thrusters upgrade level
		MaxFuel = (MaxFuel + (MaxFuel * 0.5f));  // Add 50% of current max fuel for each upgrade
		HangarMenu->SetUpgradeLevel(ThrustersLevel, NextUpgradeCost);  // set the new upgrade information within the hangar menu
		UpdateHangarMenu();  // update the hangar menu display
	}
	else
	{
		HangarMenu->NotifyError();
	}
}

// Upgrade Main Cannon
void AEndlessReachHDPawn::UpgradeCannon(int32 UpgradeCost, int32 Level, int32 NextUpgradeCost, float NewFireRate)
{
	if (OrbCount > UpgradeCost)
	{
		OrbCount = OrbCount - UpgradeCost;  // subtract Cost from OrbCount
		CannonLevel = Level;  // set Cannon upgrade level
		FireRate = NewFireRate;  // set new fire rate
		HangarMenu->SetUpgradeLevel(CannonLevel, NextUpgradeCost);  // set the new upgrade information within the hangar menu
		UpdateHangarMenu();  // update the hangar menu display
	}
	else
	{
		HangarMenu->NotifyError();
	}
}

// Upgrade Laser
void AEndlessReachHDPawn::UpgradeLaser(int32 UpgradeCost, int32 Level, int32 NextUpgradeCost)
{
	if (OrbCount > UpgradeCost)
	{
		// if laser charge max is less than five, increase max by one for each upgrade
		if (LaserChargeMax < 5)
		{
			LaserChargeMax++;
		}
		else
		{
			LaserChargeMax = 5; // force the laser charge max to five if it exceeds for some reason
		}

		OrbCount = OrbCount - UpgradeCost;  // subtract Cost from OrbCount
		LaserLevel = Level;  // set laser upgrade level		
		HangarMenu->SetUpgradeLevel(LaserLevel, NextUpgradeCost);  // set the new upgrade information within the hangar menu
		UpdateHangarMenu();  // update the hangar menu display
	}
	else
	{
		HangarMenu->NotifyError();
	}
}

// Upgrade Magnet
void AEndlessReachHDPawn::UpgradeMagnet(int32 UpgradeCost, int32 Level, int32 NextUpgradeCost, int32 NewMagnetRadius)
{
	if (OrbCount > UpgradeCost)
	{
		OrbCount = OrbCount - UpgradeCost;  // subtract Cost from OrbCount
		MagnetLevel = Level;  // set magnet upgrade level
		MagnetRadius->SetSphereRadius(NewMagnetRadius);
		HangarMenu->SetUpgradeLevel(MagnetLevel, NextUpgradeCost);  // set the new upgrade information within the hangar menu
		UpdateHangarMenu();  // update the hangar menu display
	}
	else
	{
		HangarMenu->NotifyError();
	}
}

// Upgrade Bomb
void AEndlessReachHDPawn::UpgradeBomb(int32 UpgradeCost, int32 Level, int32 NextUpgradeCost)
{
	if (OrbCount > UpgradeCost)
	{
		// if bomb max is less than five, increase max by one for each upgrade
		if (BombMax < 5)
		{
			BombMax++;
		}
		else
		{
			BombMax = 5; // force the bomb max to five if it exceeds for some reason
		}

		OrbCount = OrbCount - UpgradeCost;  // subtract Cost from OrbCount
		BombLevel = Level;  // set bomb upgrade level
		HangarMenu->SetUpgradeLevel(BombLevel, NextUpgradeCost);  // set the new upgrade information within the hangar menu
		UpdateHangarMenu();  // update the hangar menu display
	}
	else
	{
		HangarMenu->NotifyError();
	}
}

// Action Button Control
void AEndlessReachHDPawn::ActionInput()
{
	if (bIsDocked)
	{
		if (HangarMenu)
		{
			if (HangarMenu->IsInViewport())
			{
				if (!HangarMenu->bIsPromptingExit)
				{
					switch (HangarMenu->MenuIndex)
					{
						// SHIP TYPE UPGRADE - INDEX 0
						case 0:
							switch (ShipTypeLevel)
							{
							case 0:
								//UpgradeCost = GetUpgradeCost(0, 0);
								break;
							case 1:
								//UpgradeCost = GetUpgradeCost(0, 1);
								break;
							case 2:
								//UpgradeCost = GetUpgradeCost(0, 2);
								break;
							case 3:
								//UpgradeCost = GetUpgradeCost(0, 3);
								break;
							case 4:
								//UpgradeCost = GetUpgradeCost(0, 4);
								break;
							case 5:
								//UpgradeCost = GetUpgradeCost(0, 5);
								break;
							}
							break;
						// HEALTH UPGRADE - INDEX 1
						case 1:
							switch (HealthLevel)
							{
							case 0:
								UpgradeHealth(GetUpgradeCost(1, 0), 1, GetUpgradeCost(1, 1));
								break;
							case 1:
								UpgradeHealth(GetUpgradeCost(1, 1), 2, GetUpgradeCost(1, 2));
								break;
							case 2:
								UpgradeHealth(GetUpgradeCost(1, 2), 3, GetUpgradeCost(1, 3));
								break;
							case 3:
								UpgradeHealth(GetUpgradeCost(1, 3), 4, GetUpgradeCost(1, 4));
								break;
							case 4:
								UpgradeHealth(GetUpgradeCost(1, 4), 5, GetUpgradeCost(1, 5));
								break;
							case 5:
								break;
							}
							break;
						// THURSTERS UPGRADE - INDEX 2
						case 2:
							switch (ThrustersLevel)
							{
							case 0:
								UpgradeThrusters(GetUpgradeCost(2, 0), 1, GetUpgradeCost(2, 1));
								break;
							case 1:
								UpgradeThrusters(GetUpgradeCost(2, 1), 2, GetUpgradeCost(2, 2));
								break;
							case 2:
								UpgradeThrusters(GetUpgradeCost(2, 2), 3, GetUpgradeCost(2, 3));
								break;
							case 3:
								UpgradeThrusters(GetUpgradeCost(2, 3), 4, GetUpgradeCost(2, 4));
								break;
							case 4:
								UpgradeThrusters(GetUpgradeCost(2, 4), 5, GetUpgradeCost(2, 5));
								break;
							case 5:
								break;
							}
							break;
						// MAIN CANNON UPGRADE - INDEX 3
						case 3:
							switch (CannonLevel)
							{
							case 0:
								UpgradeCannon(GetUpgradeCost(3, 0), 1, GetUpgradeCost(3, 1), 0.175f);
								break;
							case 1:
								UpgradeCannon(GetUpgradeCost(3, 1), 2, GetUpgradeCost(3, 2), 0.15f);
								break;
							case 2:
								UpgradeCannon(GetUpgradeCost(3, 2), 3, GetUpgradeCost(3, 3), 0.125f);
								break;
							case 3:
								UpgradeCannon(GetUpgradeCost(3, 3), 4, GetUpgradeCost(3, 4), 0.1f);
								break;
							case 4:
								UpgradeCannon(GetUpgradeCost(3, 4), 5, GetUpgradeCost(3, 5), 0.075f);
								break;
							case 5:
								break;
							}
							break;
						// LASER UPGRADE - INDEX 4
						case 4:
							switch (LaserLevel)
							{
							case 0:
								UpgradeLaser(GetUpgradeCost(4, 0), 1, GetUpgradeCost(4, 1));
								bLaserUnlocked = true;
								break;
							case 1:
								UpgradeLaser(GetUpgradeCost(4, 1), 2, GetUpgradeCost(4, 2));
								break;
							case 2:
								UpgradeLaser(GetUpgradeCost(4, 2), 3, GetUpgradeCost(4, 3));
								break;
							case 3:
								UpgradeLaser(GetUpgradeCost(4, 3), 4, GetUpgradeCost(4, 4));
								break;
							case 4:
								UpgradeLaser(GetUpgradeCost(4, 4), 5, GetUpgradeCost(4, 5));
								break;
							case 5:
								break;
							}
							break;
						// MAGNET UPGRADE - INDEX 5
						case 5:
							switch (MagnetLevel)
							{
							case 0:
								UpgradeMagnet(GetUpgradeCost(5, 0), 1, GetUpgradeCost(5, 1), 1000);
								bMagnetEnabled = true;							
								break;
							case 1:
								UpgradeMagnet(GetUpgradeCost(5, 1), 2, GetUpgradeCost(5, 2), 1500);
								break;
							case 2:
								UpgradeMagnet(GetUpgradeCost(5, 2), 3, GetUpgradeCost(5, 3), 2000);
								break;
							case 3:
								UpgradeMagnet(GetUpgradeCost(5, 3), 4, GetUpgradeCost(5, 4), 2500);
								break;
							case 4:
								UpgradeMagnet(GetUpgradeCost(5, 4), 5, GetUpgradeCost(5, 5), 3000);
								break;
							case 5:
								break;
							}
							break;
						// MISSILES UPGRADE - INDEX 6
						case 6:
							switch (MissilesLevel)
							{
							case 0:
								break;
							case 1:
								break;
							case 2:
								break;
							case 3:
								break;
							case 4:
								break;
							case 5:
								break;
							}
							break;
						// ENERGY SHIELD UPGRADE - INDEX 7
						case 7:
							switch (ShieldLevel)
							{
							case 0:
								break;
							case 1:
								break;
							case 2:
								break;
							case 3:
								break;
							case 4:
								break;
							case 5:
								break;
							}
							break;
						// BOMB LEVEL - INDEX 8
						case 8:
							switch (BombLevel)
							{
							case 0:
								UpgradeBomb(GetUpgradeCost(8, 0), 1, GetUpgradeCost(8, 1));
								bBombsUnlocked = true;
								break;
							case 1:
								UpgradeBomb(GetUpgradeCost(8, 1), 2, GetUpgradeCost(8, 2));
								break;
							case 2:
								UpgradeBomb(GetUpgradeCost(8, 2), 3, GetUpgradeCost(8, 3));
								break;
							case 3:
								UpgradeBomb(GetUpgradeCost(8, 3), 4, GetUpgradeCost(8, 4));
								break;
							case 4:
								UpgradeBomb(GetUpgradeCost(8, 4), 5, GetUpgradeCost(8, 5));
								break;
							case 5:
								break;
							}
							break;
					}
				}
				else
				{
					switch (HangarMenu->ExitPromptIndex)
					{
					case 0:
						HangarMenu->InitLaunchSequence();
						break;
					case 1:
						HangarMenu->ReturnToUpgradeMenu();
						break;
					}
				}
			}
		}
	}
}

// Back Button Control
void AEndlessReachHDPawn::BackInput()
{
	if (bIsDocked)
	{
		if (HangarMenu)
		{
			if (HangarMenu->IsInViewport())
			{
				if (!HangarMenu->bIsPromptingExit)
				{
					HangarMenu->PromptExit();
				}				
			}
		}
	}
	if (!bIsDocked)
	{
		if (bCanFireBomb)
		{
			if (BombCount > 0)
			{
				FireBomb();
			}
		}
	}
}

// Show combat damage text function
void AEndlessReachHDPawn::ShowCombatDamageText(bool bIsCritical, float Damage)
{
	if (bIsCritical)
	{
		// CRITICAL DAMAGE
		CombatTextComponent->ShowCombatText(ECombatTextTypes::TT_CritDmg, UCommonLibrary::GetFloatAsTextWithPrecision(Damage, 0, true));  // show combat text
	}
	else
	{
		// STANDARD DAMAGE
		CombatTextComponent->ShowCombatText(ECombatTextTypes::TT_Damage, UCommonLibrary::GetFloatAsTextWithPrecision(Damage, 0, true));  // show combat text
	}
}

// Add Status Effect Icon to HUD
void AEndlessReachHDPawn::AddStatusEffectIcon(FName ID, UTexture2D* Icon, bool bShouldBlink)
{
	if (PlayerHUD)
	{
		//PlayerHUD->
	}
}
