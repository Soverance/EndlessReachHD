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
#include "GameFramework/ProjectileMovementComponent.h"
#include "Environment/Asteroid.h"

AEndlessReachHDProjectile::AEndlessReachHDProjectile() 
{
	// Static reference to the mesh to use for the projectile
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ProjectileMeshAsset(TEXT("/Game/ShipScout_Upgrades/Ammo/PulseBall.PulseBall"));

	// Create mesh component for the projectile sphere
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	ProjectileMesh->SetStaticMesh(ProjectileMeshAsset.Object);
	ProjectileMesh->SetupAttachment(RootComponent);
	ProjectileMesh->BodyInstance.SetCollisionProfileName("Projectile");
	ProjectileMesh->OnComponentHit.AddDynamic(this, &AEndlessReachHDProjectile::OnHit);		// set up a notification for when this component hits something
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
void AEndlessReachHDProjectile::BeginPlay()
{
	Super::BeginPlay();
}

void AEndlessReachHDProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL) && OtherComp->IsSimulatingPhysics())
	{
		AEndlessReachHDPawn* Player = Cast<AEndlessReachHDPawn>(OtherActor);  // Check if hit actor is the player
		
		// Proceed with damage functions if you did not hit the player
		if (!Player)
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::White, TEXT("PROJECTILE HIT OBJECT: " + OtherActor->GetName()));
			OtherComp->AddImpulseAtLocation(GetVelocity() * 20.0f, GetActorLocation());  // apply small physics impulse to any object you hit

			AAsteroid* Asteroid = Cast<AAsteroid>(OtherActor);  // if object is an asteroid...
			if (Asteroid)
			{
				Asteroid->OnHitAsteroid.Broadcast();  // Broadcast Asteroid Hit
			}
		}		
	}

	Destroy();
}