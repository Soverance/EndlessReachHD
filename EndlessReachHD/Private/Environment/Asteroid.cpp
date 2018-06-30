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
#include "Pickups/Orb.h"
#include "Pickups/FuelCell.h"
#include "Pickups/Laser.h"
#include "Pickups/BombCore.h"
#include "Environment/Asteroid.h"

// Sets default values
AAsteroid::AAsteroid()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Creates a scene component and sets it as the root
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	// Asteroid Materials	
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> GreyInst(TEXT("/Game/Environment/Materials/Rock/M_Asteroid_Inst.M_Asteroid_Inst"));
	RockColor = GreyInst.Object;
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> RedInst(TEXT("/Game/Environment/Materials/Rock/M_AsteroidHit_Inst.M_AsteroidHit_Inst"));
	RedColor = RedInst.Object;

	// Asteroid
	Asteroid = CreateDefaultSubobject<UDestructibleComponent>(TEXT("AsteroidBody"));
	Asteroid->SetupAttachment(RootComponent);
	Asteroid->SetCollisionProfileName(UCollisionProfile::PhysicsActor_ProfileName);
	Asteroid->SetSimulatePhysics(true);
	Asteroid->BodyInstance.bLockZTranslation = true;
	//static ConstructorHelpers::FObjectFinder<UDestructibleMesh> AsteroidMeshObject(TEXT("/Game/Environment/Meshes/Asteroids/SM_Asteroid_DM.SM_Asteroid_DM"));
	//DM_Asteroid = AsteroidMeshObject.Object;
	////Asteroid->SetMaterial(0, RockColor);  // outside color
	////Asteroid->SetMaterial(1, RedColor);  // inside color

	// create Explosion Visual Effect
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ExplosionParticleObject(TEXT("/Game/Particles/Emitter/P_Explosion.P_Explosion"));
	P_ExplosionFX = ExplosionParticleObject.Object;
	ExplosionFX = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ExplosionFX"));
	ExplosionFX->SetupAttachment(RootComponent);
	ExplosionFX->Template = P_ExplosionFX;
	ExplosionFX->SetWorldScale3D(FVector(2.0f, 2.0f, 2.0f));
	ExplosionFX->bAutoActivate = false;

	// create explosion audio effect
	static ConstructorHelpers::FObjectFinder<USoundCue> AsteroidExplosionAudio(TEXT("/Game/Audio/Environment/Asteroid_Explosion_Cue.Asteroid_Explosion_Cue"));
	S_AsteroidExplosion = AsteroidExplosionAudio.Object;
	AsteroidExplosionSound = CreateDefaultSubobject<UAudioComponent>(TEXT("AsteroidExplosionSound"));
	AsteroidExplosionSound->SetupAttachment(RootComponent);
	AsteroidExplosionSound->Sound = S_AsteroidExplosion;
	AsteroidExplosionSound->bAutoActivate = false;

	// Create Combat Text Component
	CombatTextComponent = CreateDefaultSubobject<UCombatTextComponent>(TEXT("Combat Text Component"));

	// Asteroid Settings
	Health = 300;
	bWasDestroyed = false;
}

// Called when the game starts or when spawned
void AAsteroid::BeginPlay()
{
	Super::BeginPlay();
	OnHitAsteroid.AddDynamic(this, &AAsteroid::HitAsteroid);  // bind asteroid hit function
	OnDestroyAsteroid.AddDynamic(this, &AAsteroid::DestroyAsteroid);  // bind asteroid hit function

	// add the destructible rock after a short delay... thanks 4.18 for moving apex destructibles into a plugin so they load super slow now
	FTimerHandle DestructibleAddTimer;
	GetWorldTimerManager().SetTimer(DestructibleAddTimer, this, &AAsteroid::AddDestructible, 3.0f, false);
}

// Called every frame
void AAsteroid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called after a short delay in order to add the destructible mesh to the component
void AAsteroid::AddDestructible()
{
	//DM_Asteroid = LoadObject<UDestructibleMesh>(GetOuter(), TEXT("/Game/Environment/Meshes/Asteroids/SM_Asteroid_DM.SM_Asteroid_DM"), NULL, LOAD_None, NULL);
	DM_Asteroid = Cast<UDestructibleMesh>(StaticLoadObject(UDestructibleMesh::StaticClass(), NULL, TEXT("DestructibleMesh'/Game/Environment/Meshes/SM_Asteroid_DM.SM_Asteroid_DM'")));
	Asteroid->SetDestructibleMesh(DM_Asteroid);
	//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Blue, FString::Printf(TEXT("Asteroid Destructible Mesh added: %f"), 0));
}

// Called when the asteroid is hit with player bullets
void AAsteroid::HitAsteroid()
{
	Asteroid->SetMaterial(0, RedColor);  // outside color
	Asteroid->SetMaterial(1, RedColor);  // inside color

	if (Health > 0)  // if if the asteroid has more than zero health
	{
		ShowCombatDamageText(false, 100);  // display asteroid damage taken
		Health = Health - 100;  // remove health

		if (Health <= 0)  // if the asteroid is dead
		{
			if (!bWasDestroyed)
			{
				DestroyAsteroid();  // if so, destroy this asteroid
			}			
		}
	}
	else
	{
		DestroyAsteroid();  // additional error catch, probably unnecessary
	}
}

// Called when the asteroid is destroyed
void AAsteroid::DestroyAsteroid()
{
	if (!bWasDestroyed)
	{
		bWasDestroyed = true;
		Asteroid->SetMaterial(0, RockColor);  // outside color
		Asteroid->SetMaterial(1, RockColor);  // inside color		
		ExplosionFX->Activate();  // explosion fx
		AsteroidExplosionSound->Play();  // explosion sound fx
		Asteroid->ApplyRadiusDamage(1000, this->GetActorLocation(), 1000, 1000, true);  // break apart destructible mesh
		FActorSpawnParameters Params;
		Params.OverrideLevel = GetLevel();  // make pickups spawn within the streaming level so they can be properly unloaded

		int32 DroppedOrbCount = FMath::RandRange(0, 15);  // drop a random amount of orbs
		//int32 DroppedOrbCount = 100;  // drop a static amount of orbs
		const FTransform Settings = FTransform(GetActorRotation(), GetActorLocation(), FVector(1,1,1));  // cache transform settings

		for (int32 i = 0; i < DroppedOrbCount; i++)
		{
			AOrb* Orb = GetWorld()->SpawnActor<AOrb>(AOrb::StaticClass(), Settings, Params);
		}

		// FUEL CELL
		int32 FuelDropChance = FMath::RandRange(0, 9);  // get a random number to determine whether or not this rock will drop a fuel cell
		const FTransform FuelSettings = FTransform(GetActorRotation(), GetActorLocation(), FVector(0.25f, 0.25f, 0.25f));  // cache transform settings
		
		// drop fuel @ 50%
		if (FuelDropChance > 4)
		{
			AFuelCell* Fuel = GetWorld()->SpawnActor<AFuelCell>(AFuelCell::StaticClass(), FuelSettings, Params);  // spawn fuel cell
		}

		// LASER CHARGE
		int32 LaserDropChance = FMath::RandRange(0, 9);  // get a random number to determine whether or not this rock will drop a laser charge
		const FTransform LaserSettings = FTransform(GetActorRotation(), GetActorLocation(), FVector(0.25f, 0.25f, 0.25f));  // cache transform settings

		// drop fuel @ 20%
		if (LaserDropChance > 7)
		{
			ALaser* Laser = GetWorld()->SpawnActor<ALaser>(ALaser::StaticClass(), LaserSettings, Params);  // spawn laser charge
		}

		// BOMB CORE
		int32 BombCoreDropChance = FMath::RandRange(0, 9);  // get a random number to determine whether or not this rock will drop a bomb core
		const FTransform BombCoreSettings = FTransform(GetActorRotation(), GetActorLocation(), FVector(0.25f, 0.25f, 0.25f));  // cache transform settings

		// drop fuel @ 20%
		if (BombCoreDropChance > 7)
		{
			ABombCore* BombCore = GetWorld()->SpawnActor<ABombCore>(ABombCore::StaticClass(), BombCoreSettings, Params);  // spawn fuel
		}
	}	
}

// Show combat damage text function
void AAsteroid::ShowCombatDamageText(bool IsCritical, float Damage)
{
	if (IsCritical)
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
