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
#include "Cannonball.h"
#include "GameFramework/ProjectileMovementComponent.h"

ACannonball::ACannonball()
{
	// Static reference to the mesh to use for the projectile
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ProjectileMeshAsset(TEXT("/Game/ShipScout_Upgrades/Ammo/Cannonball.Cannonball"));

	// Create mesh component for the projectile sphere
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	ProjectileMesh->SetStaticMesh(ProjectileMeshAsset.Object);
	ProjectileMesh->SetupAttachment(RootComponent);
	ProjectileMesh->BodyInstance.SetCollisionProfileName("Projectile");
	ProjectileMesh->OnComponentHit.AddDynamic(this, &ACannonball::OnHit);		// set up a notification for when this component hits something
	RootComponent = ProjectileMesh;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = ProjectileMesh;
	ProjectileMovement->InitialSpeed = 3000.0f;
	ProjectileMovement->MaxSpeed = 250000.0f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.0f; // No gravity
		
	InitialLifeSpan = 3.0f;  // Die after 3 seconds by default
}

// Called when the game starts or when spawned
void ACannonball::BeginPlay()
{
	Super::BeginPlay();
}

void ACannonball::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
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
				Asteroid->OnHitAsteroid.Broadcast();  // Broadcast Asteroid Hit
			}

			AEnemyMaster* Enemy = Cast<AEnemyMaster>(OtherActor);  // if the object is an enemy...
			if (Enemy)
			{
				if (Player)
				{
					Enemy->EnemyTakeDamage(Player->PlayerDealDamage(Player->ATK));  // call take damage function
				}				
			}
		}		
	}

	Destroy();
}