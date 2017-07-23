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
	MoveSpeed = 50.0f;
	MaxVelocity = 500.0f;
	MaxThrustVelocity = 1000.0f;
	FanSpeed = 50.0f;
	FuelLevel = 1000.0f;
	bThustersActive = false;
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
	ShipMeshGuns->SetupAttachment(ShipMeshComponent);
	ShipMeshGuns->SetRelativeLocation(FVector(0, 0, 0));
	ShipMeshGuns->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ShipMeshGuns->SetStaticMesh(ShipGuns.Object);
	// Left Fan
	static ConstructorHelpers::FObjectFinder<UStaticMesh> LeftFan(TEXT("/Game/ShipScout_Upgrades/Meshes/SM_ShipScout_Set1_Fan.SM_ShipScout_Set1_Fan"));
	ShipMeshFanL = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftFan"));
	ShipMeshFanL->SetupAttachment(ShipMeshComponent);
	ShipMeshFanL->SetRelativeLocation(FVector(240, 30, 30));
	ShipMeshFanL->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ShipMeshFanL->SetStaticMesh(LeftFan.Object);
	// Left Fan Rotation
	RotatingMovement_FanL = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RotatingMovement_FanL"));
	RotatingMovement_FanL->SetUpdatedComponent(ShipMeshFanL);  // set the updated component
	RotatingMovement_FanL->RotationRate = FRotator(0,FanSpeed,0);
	// Right Fan
	static ConstructorHelpers::FObjectFinder<UStaticMesh> RightFan(TEXT("/Game/ShipScout_Upgrades/Meshes/SM_ShipScout_Set1_Fan.SM_ShipScout_Set1_Fan"));
	ShipMeshFanR = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightFan"));
	ShipMeshFanR->SetupAttachment(ShipMeshComponent);
	ShipMeshFanR->SetRelativeLocation(FVector(-240, 30, 30));
	ShipMeshFanR->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ShipMeshFanR->SetStaticMesh(RightFan.Object);
	// Right Fan Rotation
	RotatingMovement_FanR = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RotatingMovement_FanR"));
	RotatingMovement_FanR->SetUpdatedComponent(ShipMeshFanR);  // set the updated component
	RotatingMovement_FanR->RotationRate = FRotator(0, (FanSpeed * -1), 0);
	// Tail Fan
	static ConstructorHelpers::FObjectFinder<UStaticMesh> TailFan(TEXT("/Game/ShipScout_Upgrades/Meshes/SM_ShipScout_Set1_Fan_Back.SM_ShipScout_Set1_Fan_Back"));
	ShipMeshFanT = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TailFan"));
	ShipMeshFanT->SetupAttachment(ShipMeshComponent);
	ShipMeshFanT->SetRelativeLocation(FVector(0, -400, 130));
	ShipMeshFanT->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ShipMeshFanT->SetStaticMesh(TailFan.Object);
	// Tail Fan Rotation
	RotatingMovement_FanT = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RotatingMovement_FanT"));
	RotatingMovement_FanT->SetUpdatedComponent(ShipMeshFanT);  // set the updated component
	RotatingMovement_FanT->RotationRate = FRotator(0, 0, (FanSpeed * -1));
	
	// Cache sound effect
	static ConstructorHelpers::FObjectFinder<USoundBase> FireAudio(TEXT("/Game/Audio/Guns/PlayerTurret_Pulse1_Cue.PlayerTurret_Pulse1_Cue"));
	FireSound = FireAudio.Object;

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

void AEndlessReachHDPawn::Tick(float DeltaSeconds)
{
	// Find movement direction
	const float ForwardValue = GetInputAxisValue(MoveForwardBinding);
	const float RightValue = GetInputAxisValue(MoveRightBinding);

	// Clamp max size so that (X=1, Y=1) doesn't cause faster movement in diagonal directions
	const FVector MoveDirection = FVector(ForwardValue, RightValue, 0.0f).GetClampedToMaxSize(1.0f);

	// Calculate  movement
	const FVector Movement = MoveDirection * MoveSpeed * DeltaSeconds;

	// If non-zero size, move this actor
	if (Movement.SizeSquared() > 0.0f)
	{
		ShipMeshComponent->SetLinearDamping(0.01f);  // RESET LINEAR DAMPING
		ShipMeshComponent->SetAngularDamping(0.01f);  // RESET ANGULAR DAMPING

		const FRotator NewRotation = Movement.Rotation();
		const FRotator CorrectedRotation = FRotator(NewRotation.Pitch, (NewRotation.Yaw - 90), NewRotation.Roll);   // correct rotation because of the ship's offset pivot point
		FHitResult Hit(1.0f);

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
			// TO DO:  make the controller vibrate, add a visual effect
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
			// TO DO:  make the controller vibrate, add an overheating visual effect, and maybe allow the ship to explode if you continue thrusting with no fuel
			if (FuelLevel <= 0)
			{
				ShipMeshComponent->SetLinearDamping(2.0f);  // Increase linear damping to slow down translation
				ShipMeshComponent->SetAngularDamping(2.0f);  // Increase angular damping to slow down rotation
			}
		}
	}
	if (Movement.SizeSquared() <= 0.0f)
	{
		// decrease fan speed while idling
		if (FanSpeed > 50)
		{
			FanSpeed--;  // decrement fan speed
			UpdateFanSpeed();
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

	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Magenta, FString::Printf(TEXT("Velocity: %f"), GetVelocity().Size()));
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
				// This is velocity inheritance code for the projectile... it seems almost there, but isn't quite perfect
				//float InheritanceMod = 1.0f;  // set inheritance level to 100%
				//FVector Inheritance = GetControlRotation().UnrotateVector(GetVelocity());  // unrotate the player's velocity vector
				//FVector NewVelocity = ((Inheritance * InheritanceMod) * FVector(Pulse->GetProjectileMovement()->InitialSpeed, 0, 0));  // add inherited velocity to the projectile's default velocity
				//Pulse->GetProjectileMovement()->SetVelocityInLocalSpace(NewVelocity);  // update projectile velocity
				//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Magenta, FString::Printf(TEXT("New Velocity: %f"), NewVelocity.Size()));
				//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, FString::Printf(TEXT("Updated Velocity from PAWN: %f"), Pulse->GetVelocity().Size()));
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
}

void AEndlessReachHDPawn::StopThrusters()
{
	bThustersActive = false;
}