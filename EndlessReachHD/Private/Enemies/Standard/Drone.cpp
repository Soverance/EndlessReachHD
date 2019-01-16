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
#include "Drone.h"

#define LOCTEXT_NAMESPACE "EndlessReachHD-TextLibrary"

// Sets default values
ADrone::ADrone()
{
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> EnemyMesh(TEXT("/Game/Enemies/Drone/SK_Drone.SK_Drone"));
	static ConstructorHelpers::FObjectFinder<UClass> AnimBP(TEXT("/Game/Enemies/Drone/AnimBP_Drone.AnimBP_Drone_C"));
	GetMesh()->SkeletalMesh = EnemyMesh.Object;
	GetMesh()->SetAnimInstanceClass(AnimBP.Object);
	GetMesh()->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
	GetMesh()->SetRelativeLocation(FVector(-50, 0, 0));
	GetMesh()->SetRelativeRotation(FRotator(0, -90, 0));
	//GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	/*static ConstructorHelpers::FObjectFinder<USoundCue> FireAudioObject(TEXT("SoundCue'/Game/Sounds/Stingers/Mono/StingerPickup_Health01_Cue.StingerPickup_Health01_Cue'"));
	S_FireAudio = FireAudioObject.Object;
	FireAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("FireAudio"));
	FireAudio->SetupAttachment(RootComponent);
	FireAudio->Sound = S_FireAudio;
	FireAudio->bAutoActivate = false;*/

	static ConstructorHelpers::FObjectFinder<USoundCue> DeathAudioObject(TEXT("/Game/Audio/Enemies/Robot/Robot_Gunfire_Cue.Robot_Gunfire_Cue"));
	S_DeathAudio = DeathAudioObject.Object;
	DeathAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("DeathAudio"));
	DeathAudio->SetupAttachment(RootComponent);
	DeathAudio->Sound = S_DeathAudio;
	DeathAudio->bAutoActivate = false;

	static ConstructorHelpers::FObjectFinder<UParticleSystem> ExplosionParticleObject(TEXT("/Game/Particles/Emitter/P_Explosion.P_Explosion"));
	P_ExplosionFX = ExplosionParticleObject.Object;
	ExplosionFX = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ExplosionFX"));
	ExplosionFX->SetupAttachment(RootComponent);
	ExplosionFX->Template = P_ExplosionFX;
	ExplosionFX->bAutoActivate = false;
	ExplosionFX->SetRelativeLocation(FVector(0, 0, 0));
	ExplosionFX->SetRelativeRotation(FRotator(0, 0, 0));
	ExplosionFX->SetRelativeScale3D(FVector(2, 2, 2));
	
	static ConstructorHelpers::FObjectFinder<UParticleSystem> DroneHitParticleObject(TEXT("/Game/Particles/Emitter/P_WeldSparks.P_WeldSparks"));
	P_HitFX = DroneHitParticleObject.Object;
	HitFX = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("HitFX"));
	HitFX->SetupAttachment(RootComponent);
	HitFX->Template = P_HitFX;
	HitFX->bAutoActivate = false;
	HitFX->SetRelativeLocation(FVector(0, 0, 0));
	HitFX->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));

	static ConstructorHelpers::FObjectFinder<UParticleSystem> DeathParticleObject(TEXT("/Game/Particles/Emitter/EnemyDeath.EnemyDeath"));
	P_DeathFX = DeathParticleObject.Object;
	DeathFX = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("DeathFX"));
	DeathFX->SetupAttachment(RootComponent);
	DeathFX->Template = P_DeathFX;
	DeathFX->bAutoActivate = false;
	DeathFX->SetRelativeLocation(FVector(0, 0, -90));
	DeathFX->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
	
	// DEFAULTS
	BattleType = EBattleTypes::BT_Standard;
	NameText = LOCTEXT("Drone_NameText", "Drone");
	AttackDelay = 0.75f;
	BaseEyeHeight = 16;
	GetCapsuleComponent()->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
	GetCharacterMovement()->MaxAcceleration = 30;
	GetCharacterMovement()->RotationRate = FRotator(0, 90, 0);

	// Enemy A.I. Config
	PawnSensing->HearingThreshold = 1250;
	PawnSensing->LOSHearingThreshold = 2000;
	PawnSensing->SightRadius = 1500;
	PawnSensing->SetPeripheralVisionAngle(120.0f);
	AcceptanceRadius = 5.0f;
	bRunAI = false;
	bSuicide = false;
}

// Called when the game starts or when spawned
void ADrone::BeginPlay()
{
	Super::BeginPlay();
	OnDeath.AddDynamic(this, &ADrone::DroneDeath);  // bind the death fuction to the OnDeath event
	OnAggro.AddDynamic(this, &ADrone::DroneAggro);  // bind the aggro function to the OnAggro event
	OnReachedTarget.AddDynamic(this, &ADrone::DroneAttack);  // bind the attack function to the OnReachedTarget event
	
}

// Called every frame
void ADrone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADrone::DroneAttack()
{
	//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, FString::Printf(TEXT("Drone attacking player!")));
	bIsAttacking = true;
	DroneSuicide();
}

void ADrone::DroneAggro()
{
	// do something else here, maybe.  A light, sort of like an eye that noticed you... or maybe a sound effect?
}

void ADrone::DroneDeathEffects()
{
	HitFX->Deactivate();
	ExplosionFX->Activate();
	DeathAudio->Play();
	GetMesh()->SetVisibility(false);
}

void ADrone::DroneDeath()
{
	DroneDeathEffects();

	if (Target)
	{
		Target->GenerateDrops(true, GetActorLocation());  // reward drops for the kill		
	}
	
	// wait a bit (delays the UI display for readability).
	FTimerDelegate DelegateDeath;
	DelegateDeath.BindUFunction(this, FName("FinalDeath"), true, false);
	FTimerHandle DeathTimer;
	GetWorldTimerManager().SetTimer(DeathTimer, DelegateDeath, 1.0f, false);
}

void ADrone::DroneSuicide()
{
	if (!bSuicide)
	{
		bSuicide = true;
		DroneDeathEffects();

		if (Target)
		{
			float damage = Target->CurrentHP * 0.25f;  // deals 25% damage if a drone suicides on the player
			Target->PlayerTakeDamage(damage);
		}

		// wait a bit (delays the UI display for readability).
		FTimerDelegate DelegateDeath;
		DelegateDeath.BindUFunction(this, FName("FinalDeath"), true, false);
		FTimerHandle DeathTimer;
		GetWorldTimerManager().SetTimer(DeathTimer, DelegateDeath, 1.0f, false);
	}	
}

#undef LOCTEXT_NAMESPACE
