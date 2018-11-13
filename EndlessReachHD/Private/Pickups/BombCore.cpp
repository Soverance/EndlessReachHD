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

#include "EndlessReachHD.h"
#include "BombCore.h"

#define LOCTEXT_NAMESPACE "PickupText"

// Sets default values
ABombCore::ABombCore()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	bPickedUp = false;

	// BombCore Materials	
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> BombCoreInst(TEXT("/Game/Pickups/Materials/Inst_TronRed.Inst_TronRed"));
	BombCoreColor = BombCoreInst.Object;

	// BombCoreCore Body
	static ConstructorHelpers::FObjectFinder<UStaticMesh> BombCoreMesh(TEXT("/Game/Pickups/diamond.diamond"));
	BombCoreMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BombCoreBody"));
	RootComponent = BombCoreMeshComponent;
	BombCoreMeshComponent->SetCollisionProfileName(UCollisionProfile::PhysicsActor_ProfileName);
	BombCoreMeshComponent->OnComponentHit.AddDynamic(this, &ABombCore::OnHit);  // set up a notification for when this component hits something
	BombCoreMeshComponent->OnComponentBeginOverlap.AddDynamic(this, &ABombCore::OnOverlap);
	BombCoreMeshComponent->SetStaticMesh(BombCoreMesh.Object);
	BombCoreMeshComponent->SetMaterial(0, BombCoreColor);
	BombCoreMeshComponent->SetRelativeRotation(FRotator(0, 0, 0));
	BombCoreMeshComponent->SetWorldScale3D(FVector(1.0f, 1.0f, 1.0f));
	BombCoreMeshComponent->SetSimulatePhysics(true);
	BombCoreMeshComponent->BodyInstance.bLockZTranslation = true;
	BombCoreMeshComponent->BodyInstance.bLockXRotation = true;
	BombCoreMeshComponent->BodyInstance.bLockYRotation = true;
	BombCoreMeshComponent->SetLinearDamping(15.0f);  // Increase linear damping to slow down translation
	BombCoreMeshComponent->SetAngularDamping(15.0f);  // Increase angular damping to slow down rotation	

	static ConstructorHelpers::FObjectFinder<USoundCue> BombCorePickupAudio(TEXT("/Game/Audio/Pickups/Pickup_Bomb_Cue.Pickup_Bomb_Cue"));
	S_BombCorePickup = BombCorePickupAudio.Object;
	BombCorePickupSound = CreateDefaultSubobject<UAudioComponent>(TEXT("BombCorePickupSound"));
	BombCorePickupSound->SetupAttachment(RootComponent);
	BombCorePickupSound->Sound = S_BombCorePickup;
	BombCorePickupSound->bAutoActivate = false;

	// BombCore Pickup Visual Effect
	static ConstructorHelpers::FObjectFinder<UParticleSystem> BombCorePickupParticleObject(TEXT("/Game/Particles/Emitter/P_BombCoreExplosion.P_BombCoreExplosion"));
	P_BombCorePickupFX = BombCorePickupParticleObject.Object;
	BombCorePickupFX = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("BombCorePickupFX"));
	BombCorePickupFX->SetupAttachment(RootComponent);
	BombCorePickupFX->Template = P_BombCorePickupFX;
	BombCorePickupFX->SetWorldScale3D(FVector(2.0f, 2.0f, 2.0f));
	BombCorePickupFX->bAutoActivate = false;
}

// Called when the game starts or when spawned
void ABombCore::BeginPlay()
{
	Super::BeginPlay();

	OnPlayerCollision.AddDynamic(this, &ABombCore::CollideWithPlayer);  // bind player collision function

	FVector Velocity = FMath::VRand();  // get random vector
	BombCoreMeshComponent->AddImpulseAtLocation(Velocity, GetActorLocation());  // spread the pickup around randomly  - (PHYSICS) MOVEMENT METHOD

	// reconfigure physics so the orb overlaps instead of colliding
	FTimerHandle PhysicsResetDelay;
	GetWorldTimerManager().SetTimer(PhysicsResetDelay, this, &ABombCore::ReconfigurePhysics, 0.25f, false);
}

// Called every frame
void ABombCore::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Reconfigure Physics
void ABombCore::ReconfigurePhysics()
{
	BombCoreMeshComponent->SetSimulatePhysics(false);
	BombCoreMeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
}

// Collide With Player
void ABombCore::CollideWithPlayer()
{
	if (Player)  // if player is valid (it should be, because they just collided with the BombCore pickup)
	{
		if (Player->bBombsUnlocked)
		{
			if (Player->BombCount < Player->BombMax)  // if the player's BombCore count is less than max
			{
				switch (Player->BombCount)
				{
				case 0:  // if none
						 //Player->PlayerHUD->ChargeLaser_Stage1();  // charge laser stage 1 hud anim
					break;
				case 1:  // if one
						 //Player->PlayerHUD->ChargeLaser_Stage2();  // charge laser stage 2 hud anim
					break;
				case 2:  // if two
						 //Player->PlayerHUD->ChargeLaser_Stage3();  // charge laser stage 3 hud anim
					break;
				case 3:  // if three
						 //Player->PlayerHUD->ChargeLaser_Stage4();  // charge laser stage 4 hud anim
					break;
				case 4:  // if four
						 //Player->PlayerHUD->ChargeLaser_Stage5();  // charge laser stage 5 hud anim
					break;
				}

				Player->BombCount++;  // increment charge count
				Player->CombatTextComponent->ShowCombatText(ECombatTextTypes::TT_Drop, FText::FromString("+ Bomb"));
				BombCorePickupFX->Activate();  // activate visual fx
				BombCorePickupSound->Play();  // play BombCore pickup sound
				
			}
			else
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

		BombCoreMeshComponent->SetVisibility(false);  // hide BombCore from player view		
		FTimerHandle DestroyDelay;
		GetWorldTimerManager().SetTimer(DestroyDelay, this, &ABombCore::StartDestruction, 2.0f, false); // delay destruction so that audio can finish playing
	}
}

#undef LOCTEXT_NAMESPACE
