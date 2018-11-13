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

#include "EndlessReachHD.h"
#include "FuelCell.h"

// Sets default values
AFuelCell::AFuelCell()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	bPickedUp = false;

	// Orb Materials	
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> FuelInst(TEXT("/Game/Pickups/Materials/Inst_TronBlue.Inst_TronBlue"));
	FuelColor = FuelInst.Object;

	// Orb Body
	static ConstructorHelpers::FObjectFinder<UStaticMesh> FuelMesh(TEXT("/Game/Pickups/diamond.diamond"));
	FuelMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FuelBody"));
	RootComponent = FuelMeshComponent;
	FuelMeshComponent->SetCollisionProfileName(UCollisionProfile::PhysicsActor_ProfileName);
	FuelMeshComponent->OnComponentHit.AddDynamic(this, &AFuelCell::OnHit);  // set up a notification for when this component hits something
	FuelMeshComponent->OnComponentBeginOverlap.AddDynamic(this, &AFuelCell::OnOverlap);
	FuelMeshComponent->SetStaticMesh(FuelMesh.Object);
	FuelMeshComponent->SetMaterial(0, FuelColor);
	FuelMeshComponent->SetRelativeRotation(FRotator(0, 0, 0));
	FuelMeshComponent->SetWorldScale3D(FVector(1.0f, 1.0f, 1.0f));
	FuelMeshComponent->SetSimulatePhysics(true);
	FuelMeshComponent->BodyInstance.bLockZTranslation = true;
	FuelMeshComponent->BodyInstance.bLockXRotation = true;
	FuelMeshComponent->BodyInstance.bLockYRotation = true;
	FuelMeshComponent->SetLinearDamping(15.0f);  // Increase linear damping to slow down translation
	FuelMeshComponent->SetAngularDamping(15.0f);  // Increase angular damping to slow down rotation	

	static ConstructorHelpers::FObjectFinder<USoundCue> FuelPickupAudio(TEXT("/Game/Audio/Pickups/Pickup_Fuel_Cue.Pickup_Fuel_Cue"));
	S_FuelPickup = FuelPickupAudio.Object;
	FuelPickupSound = CreateDefaultSubobject<UAudioComponent>(TEXT("FuelPickupSound"));
	FuelPickupSound->SetupAttachment(RootComponent);
	FuelPickupSound->Sound = S_FuelPickup;
	FuelPickupSound->bAutoActivate = false;

	// FuelPickup Visual Effect
	static ConstructorHelpers::FObjectFinder<UParticleSystem> FuelPickupParticleObject(TEXT("/Game/Particles/Emitter/P_IceExplosion.P_IceExplosion"));
	P_FuelPickupFX = FuelPickupParticleObject.Object;
	FuelPickupFX = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("FuelPickupFX"));
	FuelPickupFX->SetupAttachment(RootComponent);
	FuelPickupFX->Template = P_FuelPickupFX;
	FuelPickupFX->SetWorldScale3D(FVector(2.0f, 2.0f, 2.0f));
	FuelPickupFX->bAutoActivate = false;
}

// Called when the game starts or when spawned
void AFuelCell::BeginPlay()
{
	Super::BeginPlay();

	OnPlayerCollision.AddDynamic(this, &AFuelCell::CollideWithPlayer);  // bind player collision function

	FVector Velocity = FMath::VRand();  // get random vector
	FuelMeshComponent->AddImpulseAtLocation(Velocity, GetActorLocation());  // spread the pickup around randomly  - (PHYSICS) MOVEMENT METHOD

	// reconfigure physics so the orb overlaps instead of colliding
	FTimerHandle PhysicsResetDelay;
	GetWorldTimerManager().SetTimer(PhysicsResetDelay, this, &AFuelCell::ReconfigurePhysics, 0.25f, false);
}

// Called every frame
void AFuelCell::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Reconfigure Physics
void AFuelCell::ReconfigurePhysics()
{
	FuelMeshComponent->SetSimulatePhysics(false);
	FuelMeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
}

// Collide With Player
void AFuelCell::CollideWithPlayer()
{
	if (Player)
	{
		Player->FuelLevel = Player->FuelLevel + 500;  // Add fuel

		// if current fuel level exceeds maximum...
		if (Player->FuelLevel > Player->MaxFuel)
		{
			Player->FuelLevel = Player->MaxFuel;  // reset the fuel level to maximum
		}
		
		Player->CombatTextComponent->ShowCombatText(ECombatTextTypes::TT_Drop, FText::FromString("+ Fuel"));
		FuelPickupFX->Activate();  // activate visual fx
		FuelPickupSound->Play();  // play orb pickup sound
		FuelMeshComponent->SetVisibility(false);  // hide orb from player view

		// delay destruction so that audio can finish playing
		FTimerHandle DestroyDelay;
		GetWorldTimerManager().SetTimer(DestroyDelay, this, &AFuelCell::StartDestruction, 2.0f, false);
	}
}