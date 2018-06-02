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
#include "Hangar.h"


// Sets default values
AHangar::AHangar()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bIsDockingInProgress = false;
	bIsLaunchInProgress = false;

	// Creates a scene component and sets it as the root
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	// configure Node Entry Radius
	HangarEntryRadius = CreateDefaultSubobject<UBoxComponent>(TEXT("HangarEntryRadius"));
	HangarEntryRadius->SetupAttachment(RootComponent);
	HangarEntryRadius->SetCollisionProfileName(UCollisionProfile::PhysicsActor_ProfileName);
	HangarEntryRadius->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	HangarEntryRadius->OnComponentBeginOverlap.AddDynamic(this, &AHangar::OnOverlap);
	HangarEntryRadius->SetBoxExtent(FVector(800, 250, 500));
	HangarEntryRadius->SetRelativeLocation(FVector(0, 1800, 0));
	HangarEntryRadius->SetSimulatePhysics(true);
	HangarEntryRadius->BodyInstance.bLockZTranslation = true;
}

// Called when the game starts or when spawned
void AHangar::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AHangar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AHangar::OnOverlap(UPrimitiveComponent * HitComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL))
	{
		AEndlessReachHDPawn* HitPlayer = Cast<AEndlessReachHDPawn>(OtherActor);  // Check if hit actor is the player

		// Proceed with functions if you hit the player
		if (HitPlayer)
		{
			
		}
	}
}

// Called when the game starts or when spawned
void AHangar::Launch()
{
	InitiateLaunchSequence.Broadcast();
}
