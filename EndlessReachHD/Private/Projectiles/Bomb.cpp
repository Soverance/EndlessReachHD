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
#include "EndlessReachHDPawn.h"
#include "Enemies/EnemyMaster.h"
#include "Environment/Asteroid.h"
#include "Bomb.h"
#include "GameFramework/ProjectileMovementComponent.h"

ABomb::ABomb()
{
	// Static reference to the mesh to use for the projectile
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ProjectileMeshAsset(TEXT("/Game/ShipScout_Upgrades/Ammo/Cannonball.Cannonball"));

	// Bomb Materials	
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> BombInst(TEXT("/Game/ShipScout_Upgrades/Materials/Ammo_Materials/RedMaterial.RedMaterial"));
	BombColor = BombInst.Object;

	// Create mesh component for the projectile sphere
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	ProjectileMesh->SetStaticMesh(ProjectileMeshAsset.Object);
	ProjectileMesh->SetupAttachment(RootComponent);
	ProjectileMesh->BodyInstance.SetCollisionProfileName("Projectile");
	ProjectileMesh->OnComponentHit.AddDynamic(this, &ABomb::OnHit);		// set up a notification for when this component hits something
	RootComponent = ProjectileMesh;
	ProjectileMesh->SetMaterial(0, BombColor);

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = ProjectileMesh;
	ProjectileMovement->InitialSpeed = 1500.0f;
	ProjectileMovement->MaxSpeed = 250000.0f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.0f; // No gravity

	// Bomb Shockwave Visual Effect
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ShockwaveParticleObject(TEXT("/Game/ShipScout_Upgrades/Particles/P_BombShockwave.P_BombShockwave"));
	P_ShockwaveFX = ShockwaveParticleObject.Object;
	ShockwaveFX = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ShockwaveFX"));
	ShockwaveFX->SetupAttachment(ProjectileMesh, FName("ShockwaveSocket"));
	ShockwaveFX->Template = P_ShockwaveFX;
	ShockwaveFX->SetRelativeRotation(FRotator(0, 0, 0));
	ShockwaveFX->SetWorldScale3D(FVector(1.0f, 1.0f, 1.0f));
	ShockwaveFX->bAutoActivate = false;

	// configure Shockwave Cam shake
	static ConstructorHelpers::FObjectFinder<UClass> ShockwaveCamShakeObject(TEXT("/Game/CamShakes/CS_Shockwave.CS_Shockwave_C"));
	ShockwaveCamShake = ShockwaveCamShakeObject.Object;

	// configure Shockwave Force Feedback
	static ConstructorHelpers::FObjectFinder<UForceFeedbackEffect> ShockwaveFeedbackObject(TEXT("/Game/ForceFeedback/FF_Shockwave.FF_Shockwave"));
	ShockwaveFeedback = ShockwaveFeedbackObject.Object;

	// Shockwave noise audio
	static ConstructorHelpers::FObjectFinder<USoundCue> ShockwaveAudio(TEXT("/Game/Audio/Guns/Bomb_Shockwave_Cue.Bomb_Shockwave_Cue"));
	S_Shockwave = ShockwaveAudio.Object;
	ShockwaveSound = CreateDefaultSubobject<UAudioComponent>(TEXT("ShockwaveSound"));
	ShockwaveSound->SetupAttachment(RootComponent);
	ShockwaveSound->Sound = S_Shockwave;
	ShockwaveSound->bAutoActivate = false;
		
	InitialLifeSpan = 5.0f;  // Die after 5 seconds by default

	// configure ShockwaveRadius
	ShockwaveRadius = CreateDefaultSubobject<USphereComponent>(TEXT("ShockwaveRadius"));
	ShockwaveRadius->SetCollisionProfileName(UCollisionProfile::PhysicsActor_ProfileName);
	ShockwaveRadius->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	ShockwaveRadius->SetSphereRadius(1000); 
	ShockwaveRadius->bHiddenInGame = true;	
}

// Called when the game starts or when spawned
void ABomb::BeginPlay()
{
	Super::BeginPlay();

	ShockwaveRadius->AttachToComponent(ProjectileMesh, FAttachmentTransformRules::SnapToTargetIncludingScale);  // Attach shockwave radius to projectile
	ShockwaveRadius->SetRelativeLocation(FVector(0, 0, 0));  // reset shockwave radius location
}

void ABomb::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL))
	{
		AEndlessReachHDPawn* HitPlayer = Cast<AEndlessReachHDPawn>(OtherActor);  // Check if hit actor is the player
		
		// Proceed with damage functions if you did not hit the player
		if (!HitPlayer)
		{
			if (OtherComp->IsSimulatingPhysics())
			{
				OtherComp->AddImpulseAtLocation(GetVelocity() * 20.0f, GetActorLocation());  // apply small physics impulse to any object you hit
			}			

			AAsteroid* Asteroid = Cast<AAsteroid>(OtherActor);  // if object is an asteroid...
			if (Asteroid)
			{
				FireShockwave(OtherActor);  // shockwave effects
			}

			AEnemyMaster* Enemy = Cast<AEnemyMaster>(OtherActor);  // if object is an enemy...
			if (Enemy)
			{
				FireShockwave(OtherActor);  // shockwave effects
			}
		}		
	}

	ProjectileMesh->SetVisibility(false);  // hide the cannonball mesh

	// delay self destruction so that shockwave effects can finish playing
	FTimerHandle DestroyDelay;
	GetWorldTimerManager().SetTimer(DestroyDelay, this, &ABomb::DelayedSelfDestruction, 5.0f, false);
}

void ABomb::FireShockwave(AActor* OtherActor)
{
	ShockwaveFX->Activate();
	ShockwaveSound->Play();
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);  // Get Player Controller
	PlayerController->ClientPlayForceFeedback(ShockwaveFeedback, false, false, FName(TEXT("Shockwave")));  // Play Shockwave Force Feedback
	PlayerController->ClientPlayCameraShake(ShockwaveCamShake);  // play shockwave cam shake
	// delay enemy destruction to make it a bit more satisfying
	FTimerHandle DestroyDelay;
	GetWorldTimerManager().SetTimer(DestroyDelay, this, &ABomb::DelayedDestruction, 1.0f, false);
}

void ABomb::DelayedDestruction()
{
	TArray<AActor*> OverlappingAsteroids;
	ShockwaveRadius->GetOverlappingActors(OverlappingAsteroids, AAsteroid::StaticClass()); // check for and store overlapping asteroids	

	if (OverlappingAsteroids.Num() > 0)
	{
		for (AActor* Actor : OverlappingAsteroids) // for each actor found overlapping
		{
			AAsteroid* Asteroid = Cast<AAsteroid>(Actor);  // cast to asteroid
			
			if (Asteroid)
			{
				Asteroid->OnDestroyAsteroid.Broadcast();  // Destroy Asteroid
			}			
		}
	}

	TArray<AActor*> OverlappingEnemies;
	ShockwaveRadius->GetOverlappingActors(OverlappingEnemies, AEnemyMaster::StaticClass()); // check for and store overlapping enemies

	if (OverlappingEnemies.Num() > 0)
	{
		for (AActor* Actor : OverlappingEnemies) // for each actor found overlapping
		{
			AEnemyMaster* Enemy = Cast<AEnemyMaster>(Actor);  // cast to enemy master

			if (Enemy)
			{
				if (Enemy->BattleType == EBattleTypes::BT_Standard)
				{
					Enemy->Death();
				}
				if (Enemy->BattleType == EBattleTypes::BT_Boss)
				{
					if (Player)
					{
						Enemy->EnemyTakeDamage(Player->PlayerDealDamage((Player->ATK + 500)));  // bomb just deals damage to bosses...
					}					
				}
			}
		}
	}
}

void ABomb::DelayedSelfDestruction()
{
	Destroy();
}
