// © 2014 - 2018 Soverance Studios
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
#include "Laser.h"

// Sets default values
ALaser::ALaser()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	bPickedUp = false;

	// Laser Materials	
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> LaserInst(TEXT("/Game/Pickups/Materials/Inst_TronGreen.Inst_TronGreen"));
	LaserColor = LaserInst.Object;

	// Laser Body
	static ConstructorHelpers::FObjectFinder<UStaticMesh> LaserMesh(TEXT("/Game/Pickups/diamond.diamond"));
	LaserMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LaserBody"));
	RootComponent = LaserMeshComponent;
	LaserMeshComponent->SetCollisionProfileName(UCollisionProfile::PhysicsActor_ProfileName);
	LaserMeshComponent->OnComponentHit.AddDynamic(this, &ALaser::OnHit);  // set up a notification for when this component hits something
	LaserMeshComponent->OnComponentBeginOverlap.AddDynamic(this, &ALaser::OnOverlap);
	LaserMeshComponent->SetStaticMesh(LaserMesh.Object);
	LaserMeshComponent->SetMaterial(0, LaserColor);
	LaserMeshComponent->SetRelativeRotation(FRotator(0, 0, 0));
	LaserMeshComponent->SetWorldScale3D(FVector(1.0f, 1.0f, 1.0f));
	LaserMeshComponent->SetSimulatePhysics(true);
	LaserMeshComponent->BodyInstance.bLockZTranslation = true;
	LaserMeshComponent->BodyInstance.bLockXRotation = true;
	LaserMeshComponent->BodyInstance.bLockYRotation = true;
	LaserMeshComponent->SetLinearDamping(15.0f);  // Increase linear damping to slow down translation
	LaserMeshComponent->SetAngularDamping(15.0f);  // Increase angular damping to slow down rotation	

	static ConstructorHelpers::FObjectFinder<USoundCue> LaserPickupAudio(TEXT("/Game/Audio/Pickups/Pickup_Laser_Cue.Pickup_Laser_Cue"));
	S_LaserPickup = LaserPickupAudio.Object;
	LaserPickupSound = CreateDefaultSubobject<UAudioComponent>(TEXT("LaserPickupSound"));
	LaserPickupSound->SetupAttachment(RootComponent);
	LaserPickupSound->Sound = S_LaserPickup;
	LaserPickupSound->bAutoActivate = false;

	// Laser Pickup Visual Effect
	static ConstructorHelpers::FObjectFinder<UParticleSystem> LaserPickupParticleObject(TEXT("/Game/Particles/Emitter/P_LaserExplosion.P_LaserExplosion"));
	P_LaserPickupFX = LaserPickupParticleObject.Object;
	LaserPickupFX = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("LaserPickupFX"));
	LaserPickupFX->SetupAttachment(RootComponent);
	LaserPickupFX->Template = P_LaserPickupFX;
	LaserPickupFX->SetWorldScale3D(FVector(2.0f, 2.0f, 2.0f));
	LaserPickupFX->bAutoActivate = false;
}

// Called when the game starts or when spawned
void ALaser::BeginPlay()
{
	Super::BeginPlay();

	OnPlayerCollision.AddDynamic(this, &ALaser::CollideWithPlayer);  // bind player collision function

	FVector Velocity = FMath::VRand();  // get random vector
	LaserMeshComponent->AddImpulseAtLocation(Velocity, GetActorLocation());  // spread the pickup around randomly  - (PHYSICS) MOVEMENT METHOD

	// reconfigure physics so the orb overlaps instead of colliding
	FTimerHandle PhysicsResetDelay;
	GetWorldTimerManager().SetTimer(PhysicsResetDelay, this, &ALaser::ReconfigurePhysics, 0.25f, false);
}

// Called every frame
void ALaser::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Reconfigure Physics
void ALaser::ReconfigurePhysics()
{
	LaserMeshComponent->SetSimulatePhysics(false);
	LaserMeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
}

// Collide With Player
void ALaser::CollideWithPlayer()
{
	if (Player)  // if player is valid (it should be, because they just collided with the laser pickup)
	{
		if (Player->bLaserUnlocked)
		{
			if (Player->LaserChargeCount < Player->LaserChargeMax)  // if the player's laser charge count is less than the maximum
			{
				switch (Player->LaserChargeCount)
				{
				case 0:  // if none
					Player->PlayerHUD->ChargeLaser_Stage1();  // charge laser stage 1 hud anim
					break;
				case 1:  // if one
					Player->PlayerHUD->ChargeLaser_Stage2();  // charge laser stage 2 hud anim
					break;
				case 2:  // if two
					Player->PlayerHUD->ChargeLaser_Stage3();  // charge laser stage 3 hud anim
					break;
				case 3:  // if three
					Player->PlayerHUD->ChargeLaser_Stage4();  // charge laser stage 4 hud anim
					break;
				case 4:  // if four
					Player->PlayerHUD->ChargeLaser_Stage5();  // charge laser stage 5 hud anim
					break;
				}

				Player->LaserChargeCount++;  // increment charge count
				Player->CombatTextComponent->ShowCombatText(ECombatTextTypes::TT_Drop, FText::FromString("+ Laser"));
				LaserPickupFX->Activate();  // activate visual fx
				LaserPickupSound->Play();  // play laser pickup sound
				//LaserMeshComponent->SetVisibility(false);  // hide laser from player view				
			}
			else  // break into this section if the player already has max laser charges
			{
				Player->CombatTextComponent->ShowCombatText(ECombatTextTypes::TT_Text, FText::FromString("MAX"));
				PickupErrorSound->Play();  // play pickup error sound
			}
		}
		else
		{
			Player->CombatTextComponent->ShowCombatText(ECombatTextTypes::TT_Text, FText::FromString("LOCKED"));
			PickupErrorSound->Play();  // play pickup error sound
		}

		LaserMeshComponent->SetVisibility(false);  // hide laser from player view after collision		
		FTimerHandle DestroyDelay;
		GetWorldTimerManager().SetTimer(DestroyDelay, this, &ALaser::StartDestruction, 2.0f, false); // delay destruction so that audio can finish playing
	}
}